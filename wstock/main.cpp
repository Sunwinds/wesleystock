#include "main.h"
#include <wx/datetime.h>
#include <wx/filename.h>
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wstockconfig.h"
#include "wstockcustomdialog.h"
#include "MyStockDialog.h"
#include "StockHistoryDialog.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(BuyInfoList);

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

int idMenuQuit = wxNewId();
int idMenuAbout = wxNewId();
int idMenuAddMyStock = wxNewId();
int idMenuConfig = wxNewId();
int idMenuUpdateMyStockFromGoogle = wxNewId();

#define REALTIME_DELTA_TIMER_ID  200
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
    EVT_MENU(idMenuAddMyStock, MyFrame::OnAddMyStock)
    EVT_MENU(idMenuConfig, MyFrame::OnConfigure)
    EVT_MENU(idMenuUpdateMyStockFromGoogle, MyFrame::OnUpdateFromGoogle)
    EVT_STOCK_DATA_GET_DONE(-1, MyFrame::OnStockDataGetDone)
    EVT_TIMER(REALTIME_DELTA_TIMER_ID, MyFrame::OnRealtimeDeltaTimer)
    EVT_GRID_CELL_LEFT_DCLICK(MyFrame::OnGridCellDbClick)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title),
    RealTimeDeltaTimer(this,REALTIME_DELTA_TIMER_ID)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* ToolMenu = new wxMenu(_T(""));
    ToolMenu->Append(idMenuAddMyStock, _("&Add MyStock\tCtrl-a"), _("Add One Stock Buy Record!"));
    ToolMenu->Append(idMenuUpdateMyStockFromGoogle, _("&Update MyStock\tCtrl-u"),
                        _("Update MyStock Data from google!"));
    ToolMenu->AppendSeparator();
    ToolMenu->Append(idMenuConfig, _("&Configure\tCtrl-Alt-c"), _("Global Configure"));
    mbar->Append(ToolMenu, _("&Tool"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS
    CurFetchObj = NULL;
    mainGrid = new wxGrid(this,-1);
    mainGrid->CreateGrid(1,5);
    mainGrid->SetDefaultCellAlignment(wxALIGN_CENTRE,wxALIGN_CENTRE);
    mainGrid->EnableEditing(false);
    gss = new GSpreadSheets();
#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    int Widths[]={-1,100};
    SetStatusWidths(2,Widths);
    SetStatusText(_("Hello Code::Blocks user !"),0);
    SetStatusText(_("WSTOCK"),1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::DoInitData(){
    StocksDataFetch*stock = GetCurFetchObj();
    stocks.SetParent(this);
    stocks.Init(stock->GetHistoryDataGroupNum());
    if (stocks.GetStockNum()==0){
        wxLogMessage(_("There is no stock Id in %s,you may need add some into it!"),
            WStockConfig::GetKeyPath().c_str());
    }
    mystocks.LoadDataFromFile();
    mystocks.UpdateStockList(stocks.GetList());
    gss->PutToGoogle(&mystocks.GetDatas());
}

void MyFrame::UpdateMainGrid(int stockidx){

    CurStockStartPos = stockidx;
    mainGrid->BeginBatch();
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock){
        int ColNum = stock->GetProptiesNum()+3; // stockName, earnings, earnings yield
        if (mainGrid->GetNumberCols()>ColNum){
            mainGrid->DeleteCols(mainGrid->GetNumberCols() - ColNum);
        }
        else if (mainGrid->GetNumberCols() < ColNum){
            mainGrid->AppendCols(ColNum - mainGrid->GetNumberCols());
        }
        mainGrid->SetColLabelValue(0,_("Stock Name"));
        for (int i=0;i<stock->GetProptiesNum();i++){
            mainGrid->SetColLabelValue(i+1,stock->GetPropertyName(i));
        }
        mainGrid->SetColLabelValue(stock->GetProptiesNum()+1,_("Earnings Yield"));
        mainGrid->SetColLabelValue(stock->GetProptiesNum()+2,_("Earnings"));
    }

    int TotalLeft = mystocks.GetList()->GetCount() - stockidx;
    if (TotalLeft > 10){ /*TODO:应该计算出目前可以容纳的股票个数*/
        TotalLeft = 10;
    }

    if (mainGrid->GetNumberRows() < TotalLeft){
        mainGrid->AppendRows(TotalLeft - mainGrid->GetNumberRows());
    }
    for (int i=0;i<TotalLeft;i++){
        wxString name = (*mystocks.GetList())[i]->GetName();
        if (name.Length()<=0){
            name = (*mystocks.GetList())[i]->GetId();
        }
        mainGrid->SetCellValue(i,0,name);
    }
    if (mainGrid->GetNumberRows() > TotalLeft){
        mainGrid->DeleteRows(TotalLeft,mainGrid->GetNumberRows() - TotalLeft);
    }
    mainGrid->AutoSizeColumns();
    mainGrid->EndBatch();
    if (mystocks.GetList()->size()>0){//if we have some custom value,start update it.
        stock->RetriveRealTimeData(mystocks.GetList(), (void*)0);
        //if some of the stock history data not ready, retrive it
        StockList::Node* node = mystocks.GetList()->GetFirst();
        while (node)
        {
            Stock* p = node->GetData();
            if (!p->IsHistoryDataReady()){
                stock->RetriveHistoryDayData(p,(void*)0);
                break;
            }
            node = node->GetNext();
        }
    }
}

MyFrame::~MyFrame()
{
}

void MyFrame::OnStockDataGetDone(wxStockDataGetDoneEvent&event){
    if (event.rtype == REALTIME_RETRIVE){
        int idx = (int)event.UserData;
        if (idx<mainGrid->GetNumberRows()){
            mainGrid->BeginBatch();
            for (int j=0;j<mainGrid->GetNumberRows();j++){
                double CurValue=0;
                (*mystocks.GetList())[idx+j]->GetPropertyValue(_("PRICE")).ToDouble(&CurValue);
                for (int i=1;i<mainGrid->GetNumberCols();i++){
                    MyStockStru* pmystock = mystocks.GetMyStockStruByStock((*mystocks.GetList())[idx+j]);
                    if ((mainGrid->GetColLabelValue(i)) == _("Earnings Yield")){
                        if (pmystock){
                            mainGrid->SetCellValue(idx+j,i, wxString::Format(wxT("%.3f%%"),
                                    pmystock->GetEarningYield(CurValue)*100));
                            if (pmystock->GetEarningYield(CurValue)>0){
                                mainGrid->SetCellTextColour(idx+j,i,*wxRED);
                            }
                            else{
                                mainGrid->SetCellTextColour(idx+j,i,*wxGREEN);
                            }
                        }
                    }
                    else if ((mainGrid->GetColLabelValue(i)) == _("Earnings")){
                        if (pmystock){
                            mainGrid->SetCellValue(idx+j,i, wxString::Format(wxT("%.2f"),
                                    pmystock->GetEarnings(CurValue)));
                            if (pmystock->GetEarnings(CurValue)>0){
                                mainGrid->SetCellTextColour(idx+j,i,*wxRED);
                            }
                            else{
                                mainGrid->SetCellTextColour(idx+j,i,*wxGREEN);
                            }
                        }
                    }
                    else{
                        wxString CellValue = (*mystocks.GetList())[idx+j]->GetPropertyValue(
                                mainGrid->GetColLabelValue(i));
                        mainGrid->SetCellValue(idx+j,i,CellValue);
                        if (mainGrid->GetColLabelValue(i) == _("DELTA")){
                            if (CellValue.StartsWith(wxT("-"))){
                                mainGrid->SetCellTextColour(idx+j,i,*wxGREEN);
                            }
                            else{
                                mainGrid->SetCellTextColour(idx+j,i,*wxRED);
                            }
                        }
                    }
                }
            }
            mainGrid->AutoSizeColumns();
            mainGrid->EndBatch();

            //股票数据已经刷新了一轮了，为了减轻服务器的压力，
            //休息一下(30秒)再刷新第二轮吧
            RealTimeDeltaTimer.Start(30000,true);
        }
    }
    else{
        int myflag=(int)event.UserData;
        Stock* s = (Stock*)event.HistoryStock;
        s->SaveHistoryDataToFile();
        if (myflag == 1){ //UserCall
            StockHistoryDialog dialog(NULL, -1, wxT("Stock History"));
            dialog.SetStock(s);
            dialog.ShowModal();
        }
        StocksDataFetch*stock = GetCurFetchObj();
        StockList::Node* node = mystocks.GetList()->GetFirst();
        while (node)
        {
            Stock* p = node->GetData();
            if (!p->IsHistoryDataReady()){
                stock->RetriveHistoryDayData(p,(void*)0);
                break;
            }
            node = node->GetNext();
        }
    }
    //if the check fail, just discard this event.
}


void MyFrame::OnConfigure(wxCommandEvent& event)
{
    wstockcustomdialog dialog(this,-1,wxT("Global Configure"));
    if (dialog.ShowModal() == wxID_OK){
        //change the configure and save to file.
        dialog.StoreSettings();
    };
}

void MyFrame::OnUpdateFromGoogle(wxCommandEvent& event)
{
    ;
}

void MyFrame::OnAddMyStock(wxCommandEvent& event)
{
    MyStockDialog dialog(this,-1,wxT("Add One Stock"));
    if (dialog.ShowModal() == wxID_OK){
        BuyInfo*pinfo =new BuyInfo;
        pinfo->BuyAmount = dialog.GetData().ACount;
        pinfo->BuyPrice = dialog.GetData().Price;
        pinfo->Op = dialog.GetData().Op;
        pinfo->data = wxDateTime::Now();
        if (mystocks.GetDatas().find(dialog.GetData().StockId) != mystocks.GetDatas().end()){
            mystocks.GetDatas()[dialog.GetData().StockId]->buyinfos.push_back(pinfo);
        }
        else{
            MyStockStru *p = new MyStockStru;
            p->stock = stocks.GetStockById(dialog.GetData().StockId);
            if (p->stock){
                p->buyinfos.push_back(pinfo);
                mystocks.GetDatas()[dialog.GetData().StockId] = p;
                mystocks.GetList()->push_back(p->stock);
            }
            else{
                wxLogError(_("The Stock ID[%s] is unacceptable!"),dialog.GetData().StockId.c_str());
                delete (p);
                delete (pinfo);
                return;
            }
        }
        mystocks.TestRemove(mystocks.GetDatas()[dialog.GetData().StockId]->stock);
        UpdateMainGrid(0);
        mystocks.SaveDataToFile();
        gss->PutToGoogle(&mystocks.GetDatas());
    }
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

StocksDataFetch*MyFrame::GetCurFetchObj(){
    if (CurFetchObj) return CurFetchObj;
    wxString DataProviderClass(wxT("SinaStock"));
    StocksDataFetch* stock = wxDynamicCast(wxCreateDynamicObject(DataProviderClass), StocksDataFetch);
    if (stock) stock->SetParent(this);
    return stock;
}

void MyFrame::OnRealtimeDeltaTimer(wxTimerEvent& event){
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock) stock->RetriveRealTimeData(mystocks.GetList(), (void*)(0));
}

void MyFrame::OnGridCellDbClick(wxGridEvent& event){
    Stock* s = (*mystocks.GetList())[CurStockStartPos+event.GetRow()];
    if (!s->IsHistoryDataReady()){
        if (!s->LoadHistoryDataFromFile()){
            GetCurFetchObj()->RetriveHistoryDayData(s,(void*)1);
        }
        else{
            wxLogStatus(_("Load History Data From File!"));
        }
    }
    else{
        StockHistoryDialog dialog(NULL, -1, wxT("Stock History"));
        dialog.SetStock(s);
        dialog.ShowModal();
    }
}


bool MyStocks::SaveDataToFile(){
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxT("mystocks.dat"));
    fn.MakeAbsolute();
    if (!fn.DirExists(fn.GetPath())){
        if (!fn.Mkdir(fn.GetPath())){
            wxLogError(_("Try to create directory %s fail!"),fn.GetPath().c_str());
            return false;
        }
    }
    wxFileOutputStream output(fn.GetFullPath());
    if (!output.Ok()){
        wxLogError(_("Try to open file output stream %s fail!"),fn.GetFullPath().c_str());
        return false;
    }

    wxDataOutputStream store(output);
    MyStockDataHash::iterator i = datas.begin();
    while (i != datas.end())    {
        wxString stockid = i->first;
        MyStockStru* pmystock = i->second;

        BuyInfoList::Node* node = pmystock->buyinfos.GetFirst();
        while (node)
        {
            BuyInfo* pbuyinfo = node->GetData();
            store <<stockid << (wxInt32)pbuyinfo->data.GetTicks()
                            << pbuyinfo->BuyAmount
                            << pbuyinfo->BuyPrice
                            << pbuyinfo->Op;
            node = node->GetNext();
        }
        i++;
    }
    return true;
}

bool MyStocks::LoadDataFromFile(){
    wxDateTime now = wxDateTime::Now();
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxT("mystocks.dat"));
    if (fn.FileExists()){
        wxFileInputStream input(fn.GetFullPath());
        wxDataInputStream store( input );
        while (!input.Eof()){
            wxString stockid;
            store >> stockid;
            if (input.Eof()){
                break;
            }
            if (datas.find(stockid) == datas.end()){
                datas[stockid] = new MyStockStru;
                datas[stockid]->stock = NULL;
            }
            BuyInfo* pbuyinfo = new BuyInfo;
            wxInt32 ticks;
            store   >>ticks >> pbuyinfo->BuyAmount >> pbuyinfo->BuyPrice >> pbuyinfo->Op;
            pbuyinfo->data = wxDateTime((time_t)ticks);
            datas[stockid]->buyinfos.push_back(pbuyinfo);
        }
    }

    return true;
}

void MyStocks::UpdateStockList(StockList* source){
        StockList::Node* node = source->GetFirst();
        while (node)
        {
            Stock* pstock = node->GetData();
            if (datas.find(pstock->GetId()) != datas.end()){
                datas[pstock->GetId()]->stock = pstock;
                stocks.push_back(pstock);
            }
            node = node->GetNext();
        }
}

double MyStockStru::GetTotalPay(){
    double ret=0;
    BuyInfoList::Node* node = buyinfos.GetFirst();
    while (node)
    {
        BuyInfo* p = node->GetData();
        if (p->Op==0){
            ret += (p->BuyPrice * p->BuyAmount) * 1.003;
        }
        else{
            ret -= (p->BuyPrice * p->BuyAmount) * 0.997;
        }
        node = node->GetNext();
    }
    return ret;
}

double MyStockStru::GetCurValue(double CurPrice){
    return GetCurrentAmount()*CurPrice * 0.997;
}

double MyStockStru::GetEarningYield(double CurPrice){
    if (GetTotalPay()<=1) return 0;
    return (GetCurValue(CurPrice) - GetTotalPay()) / GetTotalPay();
}

double MyStockStru::GetEarnings(double CurPrice){
    return GetCurValue(CurPrice) - GetTotalPay();
}

int MyStockStru::GetCurrentAmount(){
    int ret=0;
    BuyInfoList::Node* node = buyinfos.GetFirst();
    while (node)
    {
        BuyInfo* p = node->GetData();
        if (p->Op==0){
            ret += p->BuyAmount;
        }
        else{
            ret -= p->BuyAmount;
        }
        node = node->GetNext();
    }
    return ret;
}

MyStockStru* MyStocks::GetMyStockStruByStock(Stock*s){
    if (datas.find(s->GetId()) != datas.end()){
        return datas[s->GetId()];
    }
    return NULL;
}

void MyStocks::TestRemove(Stock* s){
    MyStockStru* p =GetMyStockStruByStock(s);
    if (p){
        if (p->GetCurrentAmount()==0){
            datas.erase(s->GetId());
            delete(p);
        }
    }
}

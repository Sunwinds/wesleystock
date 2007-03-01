#include "main.h"
#include <wx/datetime.h>
#include <wx/filename.h>
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wstockconfig.h"
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


#define REALTIME_DELTA_TIMER_ID  200
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
    EVT_MENU(idMenuAddMyStock, MyFrame::OnAddMyStock)
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
    stocks.SetParent(this);
    stocks.Init();
    if (stocks.GetStockNum()==0){
        wxLogMessage(_("There is no stock Id in %s,you may need add some into it!"),
            WStockConfig::GetKeyPath().c_str());
    }
    mystocks.LoadDataFromFile();
    mystocks.UpdateStockList(stocks.GetList());
}

void MyFrame::UpdateMainGrid(int stockidx){

    CurStockStartPos = stockidx;
    mainGrid->BeginBatch();
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock){
        if (mainGrid->GetNumberCols()>(stock->GetProptiesNum()+1)){
            mainGrid->DeleteCols(mainGrid->GetNumberCols() - stock->GetProptiesNum()-1);
        }
        else if (mainGrid->GetNumberCols() < (stock->GetProptiesNum()+1)){
            mainGrid->AppendCols(stock->GetProptiesNum() +1 - mainGrid->GetNumberCols());
        }
        mainGrid->SetColLabelValue(0,_("Stock Name"));
        for (int i=0;i<stock->GetProptiesNum();i++){
            mainGrid->SetColLabelValue(i+1,stock->GetPropertyName(i));
        }
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
    mainGrid->AutoSize();
    mainGrid->EndBatch();
    //stock->RetriveRealTimeData(stocks->GetList(), (void*)0);
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
                for (int i=1;i<mainGrid->GetNumberCols();i++){
                    mainGrid->SetCellValue(idx+j,i,stocks.GetStock(idx+j)->GetPropertyValue(
                            mainGrid->GetColLabelValue(i)));
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
        Stock* s = (Stock*)event.UserData;
        s->SaveHistoryDataToFile();
        StockHistoryDialog dialog(NULL, -1, wxT("Stock History"));
        dialog.SetStock(s);
        dialog.ShowModal();
    }
    //if the check fail, just discard this event.
}

void MyFrame::OnAddMyStock(wxCommandEvent& event)
{
    MyStockDialog dialog(this,-1,wxT("Add One Stock"));
    if (dialog.ShowModal() == wxID_OK){
        BuyInfo*pinfo =new BuyInfo;
        pinfo->BuyAmount = dialog.GetData().ACount;
        pinfo->BuyPrice = dialog.GetData().Price;
        pinfo->data = wxDateTime::Now();
        if (mystocks.GetDatas().find(dialog.GetData().StockId) != mystocks.GetDatas().end()){
            mystocks.GetDatas()[dialog.GetData().StockId]->buyinfos.push_back(pinfo);
        }
        else{
            MyStockStru *p = new MyStockStru;
            p->stock = stocks.GetStockById(dialog.GetData().StockId);
            p->buyinfos.push_back(pinfo);
            mystocks.GetDatas()[dialog.GetData().StockId] = p;
        }
        UpdateMainGrid(0);
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
    wxString DataProviderClass(wxT("YahooStock"));
    StocksDataFetch* stock = wxDynamicCast(wxCreateDynamicObject(DataProviderClass), StocksDataFetch);
    if (stock) stock->SetParent(this);
    return stock;
}

void MyFrame::OnRealtimeDeltaTimer(wxTimerEvent& event){
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock) stock->RetriveRealTimeData(stocks.GetList(), (void*)(0));
}

void MyFrame::OnGridCellDbClick(wxGridEvent& event){
    Stock* s = stocks.GetStock(CurStockStartPos+event.GetRow());
    if (!s->IsHistoryDataReady()){
        if (!s->LoadHistoryDataFromFile()){
            GetCurFetchObj()->RetriveHistoryDayData(s);
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
                            << pbuyinfo->BuyPrice;
            node = node->GetNext();
        }
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
            store   >>ticks >> pbuyinfo->BuyAmount >> pbuyinfo->BuyPrice;
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

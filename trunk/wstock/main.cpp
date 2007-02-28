#include "main.h"
#include <wx/datetime.h>
#include "StockHistoryDialog.h"

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
#define REALTIME_DELTA_TIMER_ID  200
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
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

void MyFrame::SetStockSource(Stocks* s){
    stocks =s;
    UpdateMainGrid(0);
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

    int TotalLeft = stocks->GetStockNum() - stockidx;
    if (TotalLeft > 10){ /*TODO:应该计算出目前可以容纳的股票个数*/
        TotalLeft = 10;
    }

    if (mainGrid->GetNumberRows() < TotalLeft){
        mainGrid->AppendRows(TotalLeft - mainGrid->GetNumberRows());
    }
    for (int i=0;i<TotalLeft;i++){
        wxString name = stocks->GetStockName(stockidx + i);
        if (name.Length()<=0){
            name = stocks->GetStockId(stockidx + i);
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
                    mainGrid->SetCellValue(idx+j,i,stocks->GetStock(idx+j)->GetPropertyValue(
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
    if (stock) stock->RetriveRealTimeData(stocks->GetList(), (void*)(0));
}

void MyFrame::OnGridCellDbClick(wxGridEvent& event){
    Stock* s = stocks->GetStock(CurStockStartPos+event.GetRow());
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
    return true;
}

bool MyStocks::LoadDataFromFile(){
    return true;
}


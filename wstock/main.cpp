#include "main.h"
#include <wx/datetime.h>
#include <wx/filename.h>
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "mystockdetail.h"
#include "wstockconfig.h"
#include "wstockcustomdialog.h"
#include "MyStockDialog.h"
#include "StockHistoryDialog.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(BuyInfoList);
WX_DEFINE_LIST(MainGridDef_StruList);

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
int idMenuPutMyStockToGoogle = wxNewId();
int REALTIME_DELTA_TIMER_ID=wxNewId();

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
    EVT_MENU(idMenuAddMyStock, MyFrame::OnAddMyStock)
    EVT_MENU(idMenuConfig, MyFrame::OnConfigure)
    EVT_MENU(idMenuUpdateMyStockFromGoogle, MyFrame::OnUpdateFromGoogle)
    EVT_MENU(idMenuPutMyStockToGoogle, MyFrame::OnPutToGoogle)
    EVT_STOCK_DATA_GET_DONE(-1, MyFrame::OnStockDataGetDone)
    EVT_TIMER(REALTIME_DELTA_TIMER_ID, MyFrame::OnRealtimeDeltaTimer)
    EVT_GRID_CELL_LEFT_DCLICK(MyFrame::OnGridCellDbClick)
    EVT_GSPREADSHEETS_GET_DONE(-1, MyFrame::OnUpdateFromGoogleDone)
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
    ToolMenu->AppendSeparator();
    ToolMenu->Append(idMenuUpdateMyStockFromGoogle, _("&Update MyStock\tCtrl-u"),
                        _("Update MyStock Data from google!"));
    ToolMenu->Append(idMenuPutMyStockToGoogle, _("&Put MyStock\tCtrl-u"),
                        _("Put MyStock Data to google!"));
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
    mainGrid->SetRowLabelSize(50);
    gss = new GSpreadSheets(this);
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

	ColDefs.push_back(new MainGridDef_Stru(_("Stock Name"),KT_FIXED,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("PRICE"),KT_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("DELTA"),KT_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("PRICE 10D AVG"),KT_HISTORY_CALC,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("EXCHANGE"),KT_REALTIME,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("Total Amount"),KT_MYSTOCK_FIXED,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("Earnings Yield"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("Earnings"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER));

    StocksDataFetch*stock = GetCurFetchObj();
    stocks.SetParent(this);
    stocks.Init(stock->GetHistoryDataGroupNum());
    if (stocks.GetStockNum()==0){
		//Now, this is a normal case.no need worry.
        //wxLogMessage(_("There is no stock Id in %s,you may need add some into it!"),
        //    WStockConfig::GetKeyPath().c_str());
    }
    mystocks.LoadDataFromFile();
    mystocks.UpdateStockList(stocks.GetList());
}

void MyFrame::UpdateMainGrid(int stockidx){

    CurStockStartPos = stockidx;
    mainGrid->BeginBatch();
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock){
		//Maybe Data privoder has change so the mainGrid's col label may change as well.
		//so Let's do mainGrid's col label change here.
		int SkipCount=0;
		{
			for (int i=0;i<ColDefs.size();i++){
				if (ColDefs[i]->KeyType == KT_REALTIME){
					//May this dateprovider provide this Key?
					if (!stock->HasKey(ColDefs[i]->KeyName)){
						SkipCount++;
					}
				}
			}
		}
		
        int ColNum = ColDefs.size()-SkipCount;
        if (mainGrid->GetNumberCols()>ColNum){
            mainGrid->DeleteCols(mainGrid->GetNumberCols() - ColNum);
        }
        else if (mainGrid->GetNumberCols() < ColNum){
            mainGrid->AppendCols(ColNum - mainGrid->GetNumberCols());
        }
		int colidx=0;
		for (int i=0;i<ColDefs.size();i++){
			if ((ColDefs[i]->KeyType != KT_REALTIME)||(stock->HasKey(ColDefs[i]->KeyName))){
				mainGrid->SetColLabelValue(colidx,ColDefs[i]->KeyName);
				colidx++;
			}
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
        
		for (int ci=0;ci<ColDefs.size();ci++){
			if ((ColDefs[ci]->KeyType == KT_FIXED) ||
				(ColDefs[ci]->KeyType == KT_HISTORY_CALC) || 
				(ColDefs[ci]->KeyType == KT_MYSTOCK_FIXED)){
				//Init the date
				UpdateMainGridCell(i,ci);
			}
		}
		/*mainGrid->SetCellValue(i,0,name);
        wxString Id=(*mystocks.GetList())[i]->GetId();
        mainGrid->SetCellValue(i,stock->GetProptiesNum()+1,
               wxString::Format(wxT("%d"),
                    mystocks.GetDatas()[Id]->GetCurrentAmount()));*/
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

void MyFrame::OnUpdateFromGoogleDone(wxNotifyEvent&event){
    mystocks.UpdateStockList(stocks.GetList());
    UpdateMainGrid(0);
}

void MyFrame::UpdateMainGridCell(int r, int c){
	Stock*s = (*mystocks.GetList())[r];
	switch (ColDefs[c]->KeyType){
	case KT_FIXED:
		mainGrid->SetCellValue(r,c,s->GetFixedPropValue(ColDefs[c]->KeyName));
		break;
	case KT_REALTIME:
		mainGrid->SetCellValue(r,c,s->GetRealTimeValue(ColDefs[c]->KeyName));
		break;
	case KT_REALTIME_CALC:
		mainGrid->SetCellValue(r,c,s->GetRealTimeCalcValue(ColDefs[c]->KeyName));
		break;
	case KT_HISTORY_CALC:
		mainGrid->SetCellValue(r,c,s->GetHistoryCalcValue(ColDefs[c]->KeyName));
		break;
	case KT_MYSTOCK_FIXED:
	case KT_MYSTOCK_REALTIME:
		{
			wxString v=ColDefs[c]->KeyName;
			MyStockStru*p = mystocks.GetDatas()[s->GetId()];
			mainGrid->SetCellValue(r,c, p->GetPropValue(v));
		}
		break;
	}
	if (ColDefs[c]->ValueType == VT_COLOR_NUMBER){
		if (mainGrid->GetCellValue(r,c).StartsWith(wxT("-"))){
            mainGrid->SetCellTextColour(r,c,*wxGREEN);
		}
		else
		{
            mainGrid->SetCellTextColour(r,c,*wxRED);
		}
	}
}

void MyFrame::OnStockDataGetDone(wxStockDataGetDoneEvent&event){
    if (event.rtype == REALTIME_RETRIVE){
		//Realtime value may change,what we need to do is:
		//1: Calc all the Realtime_calc value
		//2: If some of the value is inside of mainGrid,update it.
        mainGrid->BeginBatch();
		for (int si=0;si<mystocks.GetList()->size();si++){
			Stock* s= (*mystocks.GetList())[si];
			s->UpdateRealTimeCalcProps();
			for (int gridci=0;gridci<ColDefs.size();gridci++){
				if ((ColDefs[gridci]->KeyType == KT_REALTIME_CALC)
					||(ColDefs[gridci]->KeyType == KT_REALTIME)
					||(ColDefs[gridci]->KeyType == KT_MYSTOCK_REALTIME)){
					UpdateMainGridCell(si,gridci);
				}
			}
		}
        mainGrid->AutoSizeColumns();
        mainGrid->EndBatch();

        //股票数据已经刷新了一轮了，为了减轻服务器的压力，
        //休息一下(30秒)再刷新第二轮吧
        RealTimeDeltaTimer.Start(30000,true);

        /*int idx = (int)event.UserData;
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
                    else if ((mainGrid->GetColLabelValue(i)) != _("Total Amount")){
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
        }*/
    }
    else{
		int myflag=(int)event.UserData;
        Stock* s = (Stock*)event.HistoryStock;
		
		//Let's first save the data to file,so nexttime we need the data, just load from file
        s->SaveHistoryDataToFile();

		//Some stock History data may change,what we need to do is:
		//1: Calc all the KT_HISTORY_CALC value
		//2: If some of the value is inside of mainGrid,update it.
        mainGrid->BeginBatch();
		int si = mystocks.GetList()->IndexOf(s);
		wxASSERT(si!=wxNOT_FOUND);
		s->UpdateHistoryCalcProps();
		for (int gridci=0;gridci<ColDefs.size();gridci++){
			if (ColDefs[gridci]->KeyType == KT_HISTORY_CALC){
				UpdateMainGridCell(si,gridci);
			}
		}
        mainGrid->AutoSizeColumns();
        mainGrid->EndBatch();
        

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

void MyFrame::OnPutToGoogle(wxCommandEvent& event)
{
	gss->PutToGoogle(&mystocks.GetDatas());
}

void MyFrame::OnUpdateFromGoogle(wxCommandEvent& event)
{
    mystocks.GetDatas().clear();
    mystocks.GetList()->clear();
    UpdateMainGrid(0);
    gss->GetFromGoogle(&mystocks.GetDatas());
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
        if (mystocks.TestRemove(mystocks.GetDatas()[dialog.GetData().StockId]->stock)){
			StocksDataFetch*stock = GetCurFetchObj();
			stock->validRequest = wxDateTime::Now();
		}
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
	StocksDataFetch*stock = GetCurFetchObj();
	Stock* s = (*mystocks.GetList())[CurStockStartPos+event.GetRow()];

	wxString Explain=s->ExplainMePropValue(ColDefs[event.GetCol()]->KeyName);
	if (!Explain.IsEmpty()){
		wxMessageBox(Explain,_("Explain me:"));
		return;
	}

	switch (ColDefs[event.GetCol()]->KeyType){
	case KT_MYSTOCK_FIXED:
	case KT_MYSTOCK_REALTIME:
		{
			mystockdetail dialog(this,-1,wxString::Format(wxT("%s-%s"),
							_("MyStock Detail"),
							s->GetName().c_str()));
			dialog.SetMyStock(mystocks.GetDatas()[s->GetId()]);
			dialog.ShowModal();
		}
		break;
	default:
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
		break;
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
			if (input.GetSize() <= input.TellI()) break;
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
        stocks.clear();
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

wxString MyStockStru::GetPropValue(const wxString& v){
	if (v == _("Total Amount")){
		return wxString::Format(wxT("%d"),GetCurrentAmount());
	}
	else if (v == _("Earnings Yield")){
		double curprice=0;
		stock->GetRealTimeValue(_("PRICE")).ToDouble(&curprice);
		return wxString::Format(wxT("%.2f%%"),GetEarningYield(curprice)*100);
	}
	else if (v == _("Earnings")){
		double curprice=0;
		stock->GetRealTimeValue(_("PRICE")).ToDouble(&curprice);
		return wxString::Format(wxT("%.2f"),GetEarnings(curprice));
	}
	return wxT("");
}

MyStockStru* MyStocks::GetMyStockStruByStock(Stock*s){
    if (datas.find(s->GetId()) != datas.end()){
        return datas[s->GetId()];
    }
    return NULL;
}

bool MyStocks::TestRemove(Stock* s){
    MyStockStru* p =GetMyStockStruByStock(s);
    if (p){
        if (p->GetCurrentAmount()==0){
			stocks.DeleteObject(p->stock);
            datas.erase(s->GetId());
            delete(p);
			return true;
        }
    }
	return false;
}

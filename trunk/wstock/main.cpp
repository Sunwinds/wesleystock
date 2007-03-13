#include "app.h"
#include "main.h"
#include <wx/datetime.h>
#include <wx/dir.h>
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
int idMenuTestNet=wxNewId();

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
    EVT_MENU(idMenuTestNet, MyFrame::OnTestNet)
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
#ifdef __WXDEBUG__
    helpMenu->Append(idMenuTestNet, _("&Test Net\tF2"), _("Test the net connection!"));
#endif
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS
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
    SetStatusText(_("Hello wstock user !"),0);
    SetStatusText(_("WSTOCK"),1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::DoInitData(){
	ClearDataFile();
	ColDefs.push_back(new MainGridDef_Stru(_("Stock Name"),KT_FIXED,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("PRICE"),KT_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("DELTA"),KT_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("DELTA RATE"),KT_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("PRICE 10D AVG"),KT_HISTORY_CALC,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("PRICE AVG"),KT_MYSTOCK_FIXED,VT_OTHER));
	//	ColDefs.push_back(new MainGridDef_Stru(_("EXCHANGE"),KT_REALTIME,VT_OTHER));
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
			for (size_t i=0;i<ColDefs.size();i++){
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
		for (size_t i=0;i<ColDefs.size();i++){
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

		for (size_t ci=0;ci<ColDefs.size();ci++){
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

	MyStockDataHash::iterator i = mystocks.GetDatas().begin();
	while (i != mystocks.GetDatas().end())    {
		wxString stockid = i->first;
		MyStockStru* pmystock = i->second;
		Stock*s=stocks.GetStockById(stockid);
		i++;
		if (!s){
			wxLogStatus(_("StockId %s is not a valid StockId"),stockid);
			mystocks.GetDatas().erase(stockid);
			delete(pmystock);
		}
	}

    mystocks.UpdateStockList(stocks.GetList());
	mystocks.SaveDataToFile();
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
		for (size_t si=0;si<mystocks.GetList()->size();si++){
			Stock* s= (*mystocks.GetList())[si];
			s->UpdateRealTimeCalcProps();
			for (size_t gridci=0;gridci<ColDefs.size();gridci++){
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
		for (size_t gridci=0;gridci<ColDefs.size();gridci++){
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
	int DataProvider = WStockConfig::GetDataProvider();
    wstockcustomdialog dialog(this,-1,wxT("Global Configure"));
    if (dialog.ShowModal() == wxID_OK){
        //change the configure and save to file.
        dialog.StoreSettings();
		if (DataProvider != WStockConfig::GetDataProvider()){
			ClearDataFile(false);
			wxLogMessage(_("Some of the change may only take place when you restart the app!"));
		}
    };
}

void MyFrame::ClearDataFile(bool KeepToday){
    wxDir dir(WStockConfig::GetHistoryDataDir());
    if ( !dir.IsOpened() )
    {
        // deal with the error here - wxDir would already log an error message
        // explaining the exact reason of the failure
        return;
    }
    wxString filename;
	wxDateTime now=wxDateTime::Now();
    bool cont = dir.GetFirst(&filename);
    while ( cont )
    {
		Stock*s;
		wxString Id=filename.BeforeFirst(wxT('.'));
		s = stocks.GetStockById(Id);
		if (s){
			wxDateTime date;
			wxString Format=wxString::Format(wxT("%s.%s.%%Y_%%m_%%d"),				
				s->GetId().c_str(),
				s->GetStockType().c_str());
			if (date.ParseFormat(filename,Format)){
				if ((!KeepToday)||(!date.IsSameDate(now))){
					wxRemoveFile(wxFileName(WStockConfig::GetHistoryDataDir(),filename).GetFullPath());
				}
			}
			else{
				wxLogMessage(filename);
			}
		}
        cont = dir.GetNext(&filename);
    }
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
	dialog.SetStocks(&stocks);
    if (dialog.ShowModal() == wxID_OK){
        BuyInfo*pinfo =new BuyInfo;
        pinfo->BuyAmount = dialog.GetData().ACount;
        pinfo->BuyPrice = dialog.GetData().Price;
        pinfo->Op = dialog.GetData().Op;
        pinfo->data = wxDateTime::Now();
		//wxLogMessage(dialog.GetData().StockId);
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

void MyFrame::OnTestNet(wxCommandEvent& event)
{
    WStockGetUrl* geturl=new WStockGetUrl(NULL,
            wxT("http://stock.business.sohu.com/p/fl.php"),(void*)-1);
    wxString PostData(wxT("code=002069,&flsopt=uptcode&opt=update"));
    PostData << WStockConfig::GetGmailUserName() << wxT("&Passwd=")
              << WStockConfig::GetGmailPasswd() << wxT("&service=wise&source=wstock");
    geturl->SetPostData(PostData);
    geturl->Create();
    geturl->Run();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(_("Author Blog:http://wxblender.blogspot.com"), _("Welcome to wstock"));
}

StocksDataFetch*MyFrame::GetCurFetchObj(){
	return wxGetApp().GetCurFetchObj();
}

void MyFrame::OnRealtimeDeltaTimer(wxTimerEvent& event){
    StocksDataFetch*stock = GetCurFetchObj();
    if (stock) stock->RetriveRealTimeData(mystocks.GetList(), (void*)(0));
}

void MyFrame::OnGridCellDbClick(wxGridEvent& event){
	//StocksDataFetch*stock = GetCurFetchObj();
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

#define X(x) (xmlChar*)((const char*)x)
bool MyStocks::SaveDataToFile(){
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxT("mystocks.dat"));
    fn.MakeAbsolute();
    if (!fn.DirExists(fn.GetPath())){
        if (!fn.Mkdir(fn.GetPath())){
            wxLogError(_("Try to create directory %s fail!"),fn.GetPath().c_str());
            return false;
        }
    }
	{
		xmlDocPtr doc;
		xmlNodePtr xmlnode;
		doc = xmlNewDoc(X("1.0"));
		doc->children = xmlNewDocNode(doc, NULL, X("MyStocks"), NULL);
		xmlDocSetRootElement(doc, doc->children);
		MyStockDataHash::iterator i = datas.begin();
		while (i != datas.end())    {
			wxString stockid = i->first;
			MyStockStru* pmystock = i->second;

			BuyInfoList::Node* node = pmystock->buyinfos.GetFirst();
			while (node)
			{
				BuyInfo* pbuyinfo = node->GetData();
				xmlnode = xmlNewChild(doc->children, NULL, X("MyStock"), NULL);
				xmlSetProp(xmlnode,X("StockId"),X(stockid.mb_str()));
				char s1[20]="",
					 s2[20]="",
					 s3[20]="",
					 s4[20]="";
				sprintf(s1,"%d",pbuyinfo->data.GetTicks());
				sprintf(s2,"%d",pbuyinfo->BuyAmount);
				sprintf(s3,"%.2f",pbuyinfo->BuyPrice);
				sprintf(s4,"%d",pbuyinfo->Op);

				xmlSetProp(xmlnode,X("Date"),X(s1));
				xmlSetProp(xmlnode,X("BuyAmount"),X(s2));
				xmlSetProp(xmlnode,X("BuyPrice"),X(s3));
				xmlSetProp(xmlnode,X("Op"),X(s4));
            node = node->GetNext();
			}
			i++;
		}

		xmlSaveFormatFileEnc((const char*)fn.GetFullPath().mb_str(),doc,"utf-8",1);
		xmlFreeDoc(doc);
	}
    return true;
}

bool MyStocks::LoadDataFromFile(){
    wxDateTime now = wxDateTime::Now();
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxT("mystocks.dat"));
    if (fn.FileExists()){
		xmlDocPtr doc = xmlParseFile((const char*)fn.GetFullPath().mb_str());
		if (doc == NULL ) {
			wxLogError(_("Document not parsed successfully. \n"));
			return false;
		}
		for (xmlNodePtr node=doc->children->children;node;node=node->next){
			if (xmlStrcmp(node->name,(const xmlChar*)"MyStock")==0){
				wxString StockId(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"StockId")),*wxConvCurrent);
				wxString Date(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"Date")),*wxConvCurrent);
				wxString BuyAmount(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"BuyAmount")),*wxConvCurrent);
				wxString BuyPrice(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"BuyPrice")),*wxConvCurrent);
				wxString Op(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"Op")),*wxConvCurrent);

				if (!StockId.IsEmpty()){
					if (datas.find(StockId) == datas.end()){
						datas[StockId] = new MyStockStru;
						datas[StockId]->stock = NULL;
					}
					BuyInfo* pbuyinfo = new BuyInfo;
					long ticks;
					Date.ToLong(&ticks);
					BuyAmount.ToLong(&pbuyinfo->BuyAmount);
					BuyPrice.ToDouble(&pbuyinfo->BuyPrice);
					Op.ToLong(&pbuyinfo->Op);
					pbuyinfo->data = wxDateTime((time_t)ticks);
					datas[StockId]->buyinfos.push_back(pbuyinfo);
				}
			}
		}
		xmlFreeDoc(doc);
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
	else if (v == _("PRICE AVG")){
		return wxString::Format(wxT("%.3f"),GetTotalPay()/GetCurrentAmount());
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

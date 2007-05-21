// -*- C++ -*- generated by wxGlade 0.4.1 on Tue Mar 13 15:25:55 2007

#include "wstockglobalinfo.h"
#include "wstockcustomdialog.h"
#include "wstockconfig.h"

int idMyTimerId=wxNewId();
BEGIN_EVENT_TABLE(wstockglobalinfo, wxDialog)
    EVT_STOCK_DATA_GET_DONE(-1, wstockglobalinfo::OnStockDataGetDone)
    EVT_TIMER(idMyTimerId, wstockglobalinfo::OnRealtimeDeltaTimer)
	EVT_MOVE(wstockglobalinfo::OnMoved)
END_EVENT_TABLE()

void wstockglobalinfo::OnMoved(wxMoveEvent& event ){
	WStockConfig::SetGlobalInfoX(GetPosition().x);
	WStockConfig::SetGlobalInfoY(GetPosition().y);
	WStockConfig::Flush();
}

void wstockglobalinfo::UpdateRealtimeCell(){
		    GlobalStatus.Empty();
    for (size_t i=0;i<ColDefs.size();i++){
        if (ColDefs[i]->KeyType == KT_MYSTOCK_REALTIME){
            UpdateInfoGridCell(i);
        }
    }
    for (size_t gridci=0;gridci<ColDefs.size();gridci++){
        UpdateInfoGridCellColor(gridci);
    }
    grid_infos->AutoSizeColumns();
    UpdateStatusBar();
}

void wstockglobalinfo::UpdateInfoGridCell(int r){
    if (ColDefs[r]->KeyName == (_("ShenZhen Value"))){
        grid_infos->SetCellValue(r,1,stocks[0]->GetRealTimeValue(_("PRICE")));
    }
    else if (ColDefs[r]->KeyName == (_("Global Time"))){
        grid_infos->SetCellValue(r,1,stocks[0]->GetRealTimeValue(_("TIME")));
    }
    else if (ColDefs[r]->KeyName == (_("ShangHai Value"))){
        grid_infos->SetCellValue(r,1,stocks[1]->GetRealTimeValue(_("PRICE")));
    }
    else if (ColDefs[r]->KeyName == (_("ShenZhen Delta"))){
        grid_infos->SetCellValue(r,1,stocks[0]->GetRealTimeValue(_("DELTA")));
    }
    else if (ColDefs[r]->KeyName == (_("ShangHai Delta"))){
        grid_infos->SetCellValue(r,1,stocks[1]->GetRealTimeValue(_("DELTA")));
    }
    else if (ColDefs[r]->KeyName == (_("ShenZhen Delta Rate"))){
        grid_infos->SetCellValue(r,1,stocks[0]->GetRealTimeValue(_("DELTA RATE")));
    }
    else if (ColDefs[r]->KeyName == (_("ShangHai Delta Rate"))){
        grid_infos->SetCellValue(r,1,stocks[1]->GetRealTimeValue(_("DELTA RATE")));
    }
    else if (ColDefs[r]->KeyType ==KT_MYSTOCK_REALTIME){
        grid_infos->SetCellValue(r,1, mystocks->GetMyStockTotalinfo(ColDefs[r]->KeyName));
    }
}

void wstockglobalinfo::UpdateStatusBar(void){
    GlobalStatus.Empty();
    for (size_t i=0;i<ColDefs.size();i++){
        if (ColDefs[i]->ViewInStatus){
            GlobalStatus << ColDefs[i]->KeyName << wxT(":") <<grid_infos->GetCellValue(i,1)<<wxT(" ");
        }
    }
    wxLogStatus(GlobalStatus);
}

void wstockglobalinfo::UpdateInfoGridCellColor(int r){
	if (ColDefs[r]->ValueType == VT_COLOR_NUMBER){
	    wxString CurV=grid_infos->GetCellValue(r,1);
	    if (ColDefs[r]->ColorCol>=0){
	        CurV=grid_infos->GetCellValue(ColDefs[r]->ColorCol,1);
	    }
		if (CurV.StartsWith(wxT("-"))){
			if (grid_infos->GetCellTextColour(r,1) != WStockConfig::GetBadColor()){
				grid_infos->SetCellTextColour(r,1,WStockConfig::GetBadColor());
			}
		}
		else
		{
			if (grid_infos->GetCellTextColour(r,1) != WStockConfig::GetGoodColor()){
				grid_infos->SetCellTextColour(r,1,WStockConfig::GetGoodColor());
			}
		}
	}
}

void wstockglobalinfo::OnStockDataGetDone(wxStockDataGetDoneEvent&event){
		wxLogDebug(wxT("%s %s %s %s %s %s"),
					stocks[0]->GetRealTimeValue(_("PRICE")).c_str(),
					stocks[0]->GetRealTimeValue(_("DELTA")).c_str(),
					stocks[0]->GetRealTimeValue(_("DELTA RATE")).c_str(),
					stocks[1]->GetRealTimeValue(_("PRICE")).c_str(),
					stocks[1]->GetRealTimeValue(_("DELTA")).c_str(),
					stocks[1]->GetRealTimeValue(_("DELTA RATE")).c_str()
					);
		if (event.IsSucc){
            grid_infos->BeginBatch();
            for (size_t gridci=0;gridci<ColDefs.size();gridci++){
                if ((ColDefs[gridci]->KeyType == KT_GLOBAL_REALTIME)){
                    UpdateInfoGridCell(gridci);
                }
            }
			{
				for (size_t gridci=0;gridci<ColDefs.size();gridci++){
					UpdateInfoGridCellColor(gridci);
				}
			}
            grid_infos->AutoSizeColumns();
            grid_infos->EndBatch();
            UpdateStatusBar();
		}
        RealTimeDeltaTimer.Start(5000,true);
}

void wstockglobalinfo::OnRealtimeDeltaTimer(wxTimerEvent& event){
    if (fetchObj) {
        fetchObj->RetriveRealTimeData(&stocks, (void*)0);
    }
}

wstockglobalinfo::wstockglobalinfo(wxWindow* parent, int id, const wxString& title, MyStocks*ms,const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME|wxSTAY_ON_TOP)
{
    // begin wxGlade: wstockglobalinfo::wstockglobalinfo
    grid_infos = new wxGrid(this, -1);

    set_properties();
    do_layout();
    // end wxGlade

	ColDefs.push_back(new MainGridDef_Stru(_("Global Time"),KT_GLOBAL_REALTIME,VT_OTHER));
	ColDefs.push_back(new MainGridDef_Stru(_("ShenZhen Value"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER,2));
	ColDefs.push_back(new MainGridDef_Stru(_("ShenZhen Delta"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER,-1,true));
	ColDefs.push_back(new MainGridDef_Stru(_("ShenZhen Delta Rate"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("ShangHai Value"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER,5));
	ColDefs.push_back(new MainGridDef_Stru(_("ShangHai Delta"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER,-1,true));
	ColDefs.push_back(new MainGridDef_Stru(_("ShangHai Delta Rate"),KT_GLOBAL_REALTIME,VT_COLOR_NUMBER));
	ColDefs.push_back(new MainGridDef_Stru(_("MyStock Total Earning"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER,-1,true));
	ColDefs.push_back(new MainGridDef_Stru(_("MyStock Total Earning Rate"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER,-1,true));
	ColDefs.push_back(new MainGridDef_Stru(_("MyStock Total Money"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER,-1,true));

	ColDefs.push_back(new MainGridDef_Stru(_("Today Earnings"),KT_MYSTOCK_REALTIME,VT_COLOR_NUMBER,-1,true));

    grid_infos->SetRowLabelSize(0);
    grid_infos->SetColLabelSize(0);
    grid_infos->SetDefaultCellAlignment(wxALIGN_RIGHT,wxALIGN_CENTRE);
    if ((int)ColDefs.size()>grid_infos->GetNumberRows()){
        grid_infos->AppendRows(ColDefs.size() - grid_infos->GetNumberRows());
    }
    else if ((int)ColDefs.size()<grid_infos->GetNumberRows()){
        grid_infos->DeleteRows(0,grid_infos->GetNumberRows() - ColDefs.size());
    }
    for (size_t i=0;i<ColDefs.size();i++){
        grid_infos->SetCellValue(i,0,ColDefs[i]->KeyName);
    }
    grid_infos->AutoSizeColumns();

    stocks.Append(new Stock(wxT("399001"),_("ShenZhen Value")));
    stocks.Append(new Stock(wxT("000001"),_("ShangHai Value")));

    mystocks = ms;
	RealTimeDeltaTimer.SetOwner(this,idMyTimerId);

	wstockcustomdialog dialog(NULL,-1,wxT(""));
    wxString DataProviderClass(dialog.GetDataProvider());
    fetchObj = wxDynamicCast(wxCreateDynamicObject(DataProviderClass), StocksDataFetch);
    if (fetchObj) {
        fetchObj->SetParent(this);
        fetchObj->RetriveRealTimeData(&stocks, (void*)0);
    }
}


void wstockglobalinfo::set_properties()
{
    // begin wxGlade: wstockglobalinfo::set_properties
    SetTitle(_("wstock global info"));
    SetSize(wxSize(169, 342));
    grid_infos->CreateGrid(10, 2);
    // end wxGlade
}


void wstockglobalinfo::do_layout()
{
    // begin wxGlade: wstockglobalinfo::do_layout
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxVERTICAL);
    sizer_13->Add(grid_infos, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_13);
    Layout();
    // end wxGlade
}


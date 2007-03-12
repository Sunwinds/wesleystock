#ifndef MAIN_H
#define MAIN_H

#include "wx/wx.h"
#include <wx/list.h>
#include <wx/hashmap.h>
#include "wx/grid.h"
#include "stocks.h"
#include "gspreadsheet.h"

enum{
	KT_FIXED,
	KT_REALTIME,
	KT_REALTIME_CALC,
	KT_HISTORY_CALC,
	KT_MYSTOCK_FIXED,
	KT_MYSTOCK_REALTIME,
};

enum{
	VT_COLOR_NUMBER,
	VT_NUMBER,
	VT_OTHER
};

class MainGridDef_Stru:public wxObject{
public:
	MainGridDef_Stru(const wxString &k, int kt,int vt){
		KeyName = k;
		KeyType = kt;
		ValueType = vt;
	};
	wxString KeyName;
	int KeyType; //0:Fixed 1: RealTime 2:RealTimeCalc 3:HistoryCalc
	int ValueType; //0: Number With Color 1:Number Without Color 2:Other
};
WX_DECLARE_LIST(MainGridDef_Stru,MainGridDef_StruList);

class MyStocks : public wxObject{
    public:
        MyStocks(){};
        StockList* GetList(){ return &stocks;};
        MyStockDataHash& GetDatas(){return datas;};
        MyStockStru* GetMyStockStruByStock(Stock*s);
        bool SaveDataToFile();
        bool LoadDataFromFile();
        bool TestRemove(Stock* s);//if we have no amount of one stock, remove it;
        void UpdateStockList(StockList* source);
    private:
        StockList stocks;
        MyStockDataHash datas;
};

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxFrame *frame, const wxString& title);
		~MyFrame();
		StocksDataFetch*GetCurFetchObj();
		void DoInitData();
		StockList*  GetMyStockList(){return mystocks.GetList();};
		void UpdateMainGrid(int stockidx);
	private:
        int CurStockStartPos;
        wxGrid *mainGrid;
        Stocks stocks;
        MyStocks mystocks;
        wxTimer RealTimeDeltaTimer;
        GSpreadSheets * gss;
		MainGridDef_StruList ColDefs;

		void UpdateMainGridCell(int r, int c);
        void OnUpdateFromGoogleDone(wxNotifyEvent&event);
        void OnUpdateFromGoogle(wxCommandEvent& event);
		void OnPutToGoogle(wxCommandEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnGridCellDbClick(wxGridEvent& event);
		void OnRealtimeDeltaTimer(wxTimerEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnConfigure(wxCommandEvent& event);
		void OnAddMyStock(wxCommandEvent& event);
		void OnStockDataGetDone(wxStockDataGetDoneEvent&event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

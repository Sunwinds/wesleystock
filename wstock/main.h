#ifndef MAIN_H
#define MAIN_H

#include "wx/wx.h"
#include <wx/list.h>
#include <wx/hashmap.h>
#include "wx/grid.h"
#include "stocks.h"
#include "gspreadsheet.h"

class MyStocks : public wxObject{
    public:
        MyStocks(){};
        StockList* GetList(){ return &stocks;};
        MyStockDataHash& GetDatas(){return datas;};
        MyStockStru* GetMyStockStruByStock(Stock*s);
        bool SaveDataToFile();
        bool LoadDataFromFile();
        void TestRemove(Stock* s);//if we have no amount of one stock, remove it;
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
        StocksDataFetch* CurFetchObj;
        GSpreadSheets * gss;

        void OnUpdateFromGoogle(wxCommandEvent& event);
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

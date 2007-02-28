#ifndef MAIN_H
#define MAIN_H

#include "wx/grid.h"
#include "stocks.h"

class MyStockStru{
    public:
        Stock * stock;
        double BuyPrice;
        int BuyAmount;
};
WX_DECLARE_VOIDPTR_HASH_MAP(MyStockStru, MyStockDataHash);

class MyStocks : public wxObject{
    public:
        MyStocks(){};
        StockList* GetList(){ return &stocks;};
        bool SaveDataToFile();
        bool LoadDataFromFile();
    private:
        StockList stocks;
        MyStockDataHash datas;
};

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxFrame *frame, const wxString& title);
		~MyFrame();
		void SetStockSource(Stocks* stocks);
		StocksDataFetch*GetCurFetchObj();
	private:
        int CurStockStartPos;
        wxGrid *mainGrid;
        Stocks *stocks;
        StockList mystocks;
        wxTimer RealTimeDeltaTimer;
        StocksDataFetch* CurFetchObj;

		void OnQuit(wxCommandEvent& event);
		void OnGridCellDbClick(wxGridEvent& event);
		void OnRealtimeDeltaTimer(wxTimerEvent& event);
		void OnAbout(wxCommandEvent& event);
		void UpdateMainGrid(int stockidx);
		void OnStockDataGetDone(wxStockDataGetDoneEvent&event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

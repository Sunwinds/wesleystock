#ifndef MAIN_H
#define MAIN_H

#include "wx/wx.h"
#include <wx/list.h>
#include <wx/hashmap.h>
#include "wx/grid.h"
#include "stocks.h"


class BuyInfo{
    public:
        double BuyPrice;
        int BuyAmount;
        wxDateTime data;
};

WX_DECLARE_LIST(BuyInfo, BuyInfoList);

class MyStockStru{
    public:
        Stock * stock;
        BuyInfoList buyinfos;
};
WX_DECLARE_STRING_HASH_MAP(MyStockStru*, MyStockDataHash);

class MyStocks : public wxObject{
    public:
        MyStocks(){};
        StockList* GetList(){ return &stocks;};
        MyStockDataHash& GetDatas(){return datas;};
        bool SaveDataToFile();
        bool LoadDataFromFile();
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

		void OnQuit(wxCommandEvent& event);
		void OnGridCellDbClick(wxGridEvent& event);
		void OnRealtimeDeltaTimer(wxTimerEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnAddMyStock(wxCommandEvent& event);
		void OnStockDataGetDone(wxStockDataGetDoneEvent&event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

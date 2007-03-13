#ifndef MAIN_H
#define MAIN_H

#include "wx/wx.h"
#include <wx/list.h>
#include <wx/hashmap.h>
#include "wx/grid.h"
#include "stocks.h"
#include "gspreadsheet.h"
#include "wstockglobalinfo.h"

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxFrame *frame, const wxString& title);
		~MyFrame();
		StocksDataFetch*GetCurFetchObj();
		void DoInitData();
		StockList*  GetMyStockList(){return mystocks.GetList();};
		void UpdateMainGrid(int stockidx);
		void ClearDataFile(bool KeepToday=true);
	private:
        int CurStockStartPos;
        wxGrid *mainGrid;
        Stocks stocks;
        MyStocks mystocks;
        wxTimer RealTimeDeltaTimer;
        GSpreadSheets * gss;
		MainGridDef_StruList ColDefs;
        wstockglobalinfo *globalInfo;

		void OnTestNet(wxCommandEvent& event);
		void UpdateMainGridCell(int r, int c);
		void UpdateMainGridCellColor(int r, int c);
        void OnUpdateFromGoogleDone(wxNotifyEvent&event);
        void OnUpdateFromGoogle(wxCommandEvent& event);
		void OnPutToGoogle(wxCommandEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnGridCellDbClick(wxGridEvent& event);
		void OnRealtimeDeltaTimer(wxTimerEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnConfigure(wxCommandEvent& event);
		void OnAddMyStock(wxCommandEvent& event);
		void OnGlobalInfo(wxCommandEvent& event);
		void OnStockDataGetDone(wxStockDataGetDoneEvent&event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

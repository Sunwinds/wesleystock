#ifndef MAIN_H
#define MAIN_H

#include "wx/grid.h"
#include "stocks.h"

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxFrame *frame, const wxString& title);
		~MyFrame();
		void SetStockSource(Stocks* stocks);
	private:
        int CurStockStartPos;
        wxGrid *mainGrid;
        Stocks *stocks;

		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void UpdateMainGrid(int stockidx);
		void OnStockDataGetDone(wxStockDataGetDoneEvent&event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

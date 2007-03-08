#ifndef GSPREADSHEET_H_INCLUDED
#define GSPREADSHEET_H_INCLUDED

#include "wx/wx.h"
#include "wstockgeturl.h"
#include "stocks.h"

//typedef void (wxEvtHandler::*wxNotifyEventFunction)(wxNotifyEvent&);
extern const wxEventType wxEVT_GSPREADSHEETS_GET_DONE;
#define EVT_GSPREADSHEETS_GET_DONE(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
    wxEVT_GSPREADSHEETS_GET_DONE, id, -1, (wxObjectEventFunction) (wxEventFunction) \
    (wxNotifyEventFunction) & fn, \
    (wxObject *) NULL ),

class GSpreadSheetsOpData{
    public:
        wxString StockId;
        int Op;/*1: Buy, 2:Sell*/
        int Amount;
        double Price;
};

WX_DECLARE_LIST(GSpreadSheetsOpData,GSpreadSheetsOpDataList);

class GSpreadSheets :public wxEvtHandler
{
    public:
        GSpreadSheets(wxEvtHandler *p);
        void Auth(void);
        void RetriveCellFeedHref();
        void RetriveLinkFeedHref();
        void PutToGoogle(MyStockDataHash* data);
        void GetFromGoogle(MyStockDataHash* data);
    private:
        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        void UpdateCellsToGoogle(void);
        wxString UName,PassWd,AuthKey,linkFeedHref,cellsFeedHref;
        wxArrayString cellPostBuffer;
        MyStockDataHash* Data;
        wxEvtHandler *Parent;
        DECLARE_EVENT_TABLE()
};

#endif // GSPREADSHEET_H_INCLUDED

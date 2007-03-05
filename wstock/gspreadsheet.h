#ifndef GSPREADSHEET_H_INCLUDED
#define GSPREADSHEET_H_INCLUDED

#include "wx/wx.h"
#include "wstockgeturl.h"

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
        GSpreadSheets(const wxString& username,const wxString& passwd);
        void PutOperate(GSpreadSheetsOpData*opdata);
    private:
        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        wxString UName,PassWd,AuthKey;
        GSpreadSheetsOpDataList OpList;
        DECLARE_EVENT_TABLE()
};

#endif // GSPREADSHEET_H_INCLUDED

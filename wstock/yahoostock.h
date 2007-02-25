#ifndef YAHOOSTOCK_INCLUDED
#define YAHOOSTOCK_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class YahooStock:public Stock
{
    DECLARE_DYNAMIC_CLASS(YahooStock);
    public:
        static wxString Props[];
        YahooStock(){};
        YahooStock(wxEvtHandler* P, const wxString& si, const wxString &name):Stock(P,si,name){
            };
        virtual void RetriveRealTimeData(void* UserData);
        virtual void RetriveHistoryDayData();

        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
    private:
        DECLARE_EVENT_TABLE()

};

#endif // YAHOOSTOCK_INCLUDED

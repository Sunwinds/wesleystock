#ifndef YAHOOSTOCK_INCLUDED
#define YAHOOSTOCK_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class YahooStock:public StocksDataFetch
{
    DECLARE_DYNAMIC_CLASS(YahooStock);
    public:
        static wxString Props[];
        YahooStock(){FetchSeed=0;};
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData);
        virtual void RetriveHistoryDayData();

        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
    private:
        int FetchSeed;
        DECLARE_EVENT_TABLE()

};

#endif // YAHOOSTOCK_INCLUDED

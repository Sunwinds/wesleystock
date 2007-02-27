#ifndef YAHOOSTOCK_INCLUDED
#define YAHOOSTOCK_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class YahooStock:public StocksDataFetch
{
    DECLARE_DYNAMIC_CLASS(YahooStock);
    public:
        static wxString Props[];
        YahooStock(){RealtimeFetchSeed=0; HistoryFetchSeed=0;};
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData);
        virtual void RetriveHistoryDayData(Stock* s);

        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        void FetchHistoryData(Stock* s,int datatype=0);
        void FetchRealTimeData(StockList* ss, void* UserData, int StartIdx=0);
    private:
        int RealtimeFetchSeed,HistoryFetchSeed;
        DECLARE_EVENT_TABLE()

};

#endif // YAHOOSTOCK_INCLUDED

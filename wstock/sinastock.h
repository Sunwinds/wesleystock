#ifndef SinaStock_H_INCLUDED
#define SinaStock_H_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class SinaStock:public StocksDataFetch
{
    DECLARE_DYNAMIC_CLASS(SinaStock);
    public:
        static wxString Props[];
        SinaStock(){RealtimeFetchSeed=0; HistoryFetchSeed=0;};
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData);
        virtual void RetriveHistoryDayData(Stock* s, void* UserData);

        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };
		virtual int GetHistoryDataGroupNum(){return 1;};

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        void FetchHistoryData(Stock* s,int datatype=0,void* UserData=NULL);
        void FetchRealTimeData(StockList* ss, void* UserData, int StartIdx=0);
    private:
        int RealtimeFetchSeed,HistoryFetchSeed;
        DECLARE_EVENT_TABLE()

};

#endif // SinaStock_H_INCLUDED

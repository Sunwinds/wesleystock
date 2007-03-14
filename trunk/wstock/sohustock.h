#ifndef SohuStock_H_INCLUDED
#define SohuStock_H_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class SohuStock:public StocksDataFetch
{
    DECLARE_DYNAMIC_CLASS(SohuStock);
    public:
        wxArrayString Props;
        SohuStock(){
                RealtimeFetchSeed=0;
                HistoryFetchSeed=0;
                Props.Add(_("PRICE"));
                Props.Add(_("DELTA"));
                Props.Add(_("EXCHANGE"));
                Props.Add(_("DELTA RATE"));
                Props.Add(_("DATE"));
                Props.Add(_("TIME"));
        };
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData);
        virtual void RetriveHistoryDayData(Stock* s, void* UserData);
		virtual bool HasKey(const wxString& k){
			return Props.Index(k) != wxNOT_FOUND;
		}
        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };
		virtual int GetHistoryDataGroupNum(){return 3;};
		virtual void ValidateStockId(wxWindow*Owner,const wxString& Id);

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        void FetchHistoryData(Stock* s,int datatype=0,void* UserData=NULL);
        void FetchRealTimeData(StockList* ss, void* UserData, int StartIdx=0);
    private:
        int RealtimeFetchSeed,HistoryFetchSeed;
        DECLARE_EVENT_TABLE()

};

#endif // SohuStock_H_INCLUDED

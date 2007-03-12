#ifndef YAHOOSTOCK_INCLUDED
#define YAHOOSTOCK_INCLUDED

#include "stocks.h"
#include "wstockgeturl.h"

class YahooStock:public StocksDataFetch
{
    DECLARE_DYNAMIC_CLASS(YahooStock);
    public:
        wxArrayString Props;
        YahooStock(){
			RealtimeFetchSeed=0; 
			HistoryFetchSeed=0;
			Props.Add(_("PRICE"));
			Props.Add(_("DATE"));
			Props.Add(_("TIME"));
			Props.Add(_("DELTA"));
			Props.Add(_("TODAY START"));
			Props.Add(_("HIGHEST"));
			Props.Add(_("LOWEREST"));
			Props.Add(_("EXCHANGE"));
		};
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData);
        virtual void RetriveHistoryDayData(Stock* s, void* UserData);
		virtual bool HasKey(const wxString& k){
			return Props.Index(k) != wxNOT_FOUND;
		}
		virtual void ValidateStockId(wxWindow*Owner,const wxString& Id){};
        virtual int GetProptiesNum();
        virtual wxString GetPropertyName(int idx){return Props[idx]; };
		virtual int GetHistoryDataGroupNum(){return 3;};

        void OnUrlGetDone(wxUrlGetDoneEvent& event);
        void FetchHistoryData(Stock* s,int datatype=0,void* UserData=NULL);
        void FetchRealTimeData(StockList* ss, void* UserData, int StartIdx=0);
    private:
        int RealtimeFetchSeed,HistoryFetchSeed;
        DECLARE_EVENT_TABLE()

};

#endif // YAHOOSTOCK_INCLUDED

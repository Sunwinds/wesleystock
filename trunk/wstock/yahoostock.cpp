#include "yahoostock.h"
#include <wx/tokenzr.h>

BEGIN_EVENT_TABLE(YahooStock, StocksDataFetch)
    EVT_URL_GET_DONE(-1,YahooStock::OnUrlGetDone)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(YahooStock, StocksDataFetch);

typedef struct{
    void * OrignUserData;
    StockRetriveType rType;
    int FetchSeed;
    int StartIdx;
} YahooStock_UserData;

wxString YahooStock::Props[]={
    _("PRICE"),
    _("DATE"),
    _("TIME"),
    _("DELTA"),
    _("TODAY START"),
    _("HIGHEST"),
    _("LOWEREST"),
    _("EXCHANGE"),
};

int YahooStock::GetProptiesNum(){
    return WXSIZEOF(YahooStock::Props);
};


void YahooStock::OnUrlGetDone(wxUrlGetDoneEvent& event){
    YahooStock_UserData* data = (YahooStock_UserData*)event.UserData;
    if (data->FetchSeed == FetchSeed){
        if (data->rType == REALTIME_RETRIVE){ //Real Time Data come!
            //wxLogMessage(wxT("Get Url return %p %d:%s"), event.UserData,event.RetCode,event.Result.c_str());
            wxStringTokenizer tkzlines(event.Result);
            int lineindex=0;
            while (tkzlines.HasMoreTokens()){
                wxString line = tkzlines.GetNextToken();
                wxStringTokenizer tkz(line, wxT(","));
                int idx=0;
                while (tkz.HasMoreTokens()){
                    wxString token = tkz.GetNextToken();
                    if (idx>0){
                        token.Trim().Replace(wxT("\""),wxT(""),true);
                        (*stocks)[data->StartIdx+lineindex]->SetPropertyValue(Props[idx-1], token);
                    }
                    idx++;
                }
                lineindex++;
            }
            //Tell The main App we have finish this stocks's real time data fetch
            wxStockDataGetDoneEvent event(wxEVT_STOCK_DATA_GET_DONE,REALTIME_RETRIVE,
                    data->OrignUserData);
            Parent->AddPendingEvent(event);
        }
    }
    delete(data);
}

void YahooStock::RetriveRealTimeData(StockList* ss, void* UserData){
    FetchSeed++;
    stocks = ss;
    YahooStock_UserData *data=new YahooStock_UserData();
    data->OrignUserData = UserData;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = FetchSeed;
    data->StartIdx = 0;
    wxString Url(wxT("http://finance.yahoo.com/d/quotes.csv?s="));
    for (size_t i=0;i<ss->size();i++){
        if (i==0){
            Url << (*ss)[i]->GetId() << wxT(".") <<(*ss)[i]->GetStockType();
        }
        else{
            Url << wxT("+") << (*ss)[i]->GetId() << wxT(".") <<(*ss)[i]->GetStockType();
        }
    }
    Url <<wxT("&f=sl1d1t1c1ohgv&e=.csv");
    wxLogStatus(_("Retriving totally %d stock info from finance.yahoo.com"),ss->size());
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    geturl->Create();
    geturl->Run();
}

void YahooStock::RetriveHistoryDayData(){
}

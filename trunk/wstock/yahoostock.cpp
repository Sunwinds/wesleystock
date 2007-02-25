#include "yahoostock.h"
#include <wx/tokenzr.h>

BEGIN_EVENT_TABLE(YahooStock, Stock)
    EVT_URL_GET_DONE(-1,YahooStock::OnUrlGetDone)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(YahooStock, Stock);

typedef struct{
    void * OrignUserData;
    StockRetriveType rType;
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

    if (data->rType == REALTIME_RETRIVE){ //Real Time Data come!
        //wxLogMessage(wxT("Get Url return %p %d:%s"), event.UserData,event.RetCode,event.Result.c_str());
        wxStringTokenizer tkz(event.Result, wxT(","));
        int idx=0;
        while (tkz.HasMoreTokens()){
            wxString token = tkz.GetNextToken();
            if (idx>0){
                token.Trim().Replace(wxT("\""),wxT(""),true);
                RealTimeProps[Props[idx-1]]  = token;
            }
            idx++;
        }
        //Tell The main App we have finish this stocks's real time data fetch
        wxStockDataGetDoneEvent event(wxEVT_STOCK_DATA_GET_DONE,REALTIME_RETRIVE,
                data->OrignUserData,this);
        Parent->AddPendingEvent(event);
    }

    delete(data);
}

void YahooStock::RetriveRealTimeData(void* UserData){
    YahooStock_UserData *data=new YahooStock_UserData();
    data->OrignUserData = UserData;
    data->rType = REALTIME_RETRIVE;
    wxString Url(wxT("http://finance.yahoo.com/d/quotes.csv?s="));
    Url << GetId() << wxT(".") <<GetStockType()<<wxT("&f=sl1d1t1c1ohgv&e=.csv");
    wxLogStatus(_("Retriving stock %s(%s.%s) From %s"),GetName().c_str(),
            GetId().c_str(),
            GetStockType().c_str(),
            Url.c_str());
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    geturl->Create();
    geturl->Run();
}

void YahooStock::RetriveHistoryDayData(){
}

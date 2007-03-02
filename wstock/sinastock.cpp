#include "sinastock.h"
#include "htmltableparser.h"
#include <wx/tokenzr.h>
#include <wx/html/htmlpars.h>

BEGIN_EVENT_TABLE(SinaStock, StocksDataFetch)
    EVT_URL_GET_DONE(-1,SinaStock::OnUrlGetDone)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SinaStock, StocksDataFetch);

typedef struct{
    void * OrignUserData;
    Stock * HistoryStock;
    StockRetriveType rType;
    int FetchSeed;
    int StartIdx;
    int RealtimeStockNum;
} SinaStock_UserData;

wxString SinaStock::Props[]={
    _("PRICE"),
    _("DELTA"),
    _("EXCHANGE"),
};

int SinaStock::GetProptiesNum(){
    return WXSIZEOF(SinaStock::Props);
};


void SinaStock::OnUrlGetDone(wxUrlGetDoneEvent& event){
    SinaStock_UserData* data = (SinaStock_UserData*)event.UserData;
    if (data->rType == REALTIME_RETRIVE){ //Real Time Data come!
        if (data->FetchSeed == RealtimeFetchSeed){
            HtmlTableParser *p=new HtmlTableParser();
            MyHtmlParser parser(p);
            parser.Parse(event.Result);
            p->DumpTable();
            return;
            int tblidx = p->FindTable(0,0,wxT("当前价"));
            if (tblidx>=0){
                wxLogMessage(p->GetCellValue(tblidx,0,0));
                //(*stocks)[data->StartIdx]->SetPropertyValue(Props[0], p->GetCellValue(tblidx,0,1));
                //(*stocks)[data->StartIdx]->SetPropertyValue(Props[1], p->GetCellValue(tblidx,1,1));
            }
            return;
            wxStringTokenizer tkzlines(event.Result);
            int lineindex=0;
            while (tkzlines.HasMoreTokens()){
                if (data->StartIdx+lineindex>=(int)stocks->size()) break;
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

            if (data->StartIdx + data->RealtimeStockNum >= (int)stocks->size()){
                //Tell The main App we have finish this stocks's real time data fetch
                wxStockDataGetDoneEvent event(wxEVT_STOCK_DATA_GET_DONE,REALTIME_RETRIVE,
                        data->OrignUserData);
                Parent->AddPendingEvent(event);
            }
            else{
                FetchRealTimeData(stocks,(Stock*)data->OrignUserData,data->StartIdx+data->RealtimeStockNum);
            }
        }
    }
    else if (data->rType == HISTORY_RETRIVE){
        if (data->FetchSeed == HistoryFetchSeed){
            Stock*s = (Stock*)data->HistoryStock;
            wxStringTokenizer tkzlines(event.Result,wxT("\r\n"));
            int lineindex=0;
            while (tkzlines.HasMoreTokens()){
                wxString line = tkzlines.GetNextToken();
                if (lineindex != 0) { //Skip the first line
                    StockHistoryDataPiece *p = new StockHistoryDataPiece;
                    switch (data->StartIdx){
                        case 0:
                            s->AppendDayData(p);
                            break;
                        case 1:
                            s->AppendWeekData(p);
                            break;
                        case 2:
                            s->AppendMonthData(p);
                            break;
                        default:
                            delete(p);
                            p=NULL;
                    }
                    if (p){
                        wxStringTokenizer tkz(line, wxT(","));
                        int idx=0;
                        while (tkz.HasMoreTokens()){
                                wxString token = tkz.GetNextToken();
                                token.Trim().Replace(wxT("\""),wxT(""),true);
                                switch (idx){
                                    case 0:
                                        p->data.ParseDate(token);
                                        printf("%s\n",(const char*)token.mb_str());
                                    break;
                                    case 1:
                                        token.ToDouble(&p->open);
                                    break;
                                    case 2:
                                        token.ToDouble(&p->High);
                                    break;
                                    case 3:
                                        token.ToDouble(&p->Low);
                                    break;
                                    case 4:
                                        token.ToDouble(&p->Close);
                                    break;
                                    case 5:
                                        {
                                            long v=0;
                                            token.ToLong(&v);
                                            p->volume = v;
                                        }
                                    break;
                                    case 6:
                                        token.ToDouble(&p->adjClose);
                                    break;
                            }
                            idx++;
                        }
                    }
                }
                lineindex++;
            }
            if (data->StartIdx == 2){
                wxStockDataGetDoneEvent event(wxEVT_STOCK_DATA_GET_DONE,HISTORY_RETRIVE,
                        data->OrignUserData);
                event.SetHistoryStock(data->HistoryStock);
                Parent->AddPendingEvent(event);
            }
            else{
                FetchHistoryData(data->HistoryStock,data->StartIdx+1,data->OrignUserData);
            }
        }
    }
    delete(data);
}

void SinaStock::FetchRealTimeData(StockList* ss, void* UserData, int StartIdx){
    static int MaxRealtimeNum = 15;
    stocks = ss;
    SinaStock_UserData *data=new SinaStock_UserData();
    data->OrignUserData = UserData;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = RealtimeFetchSeed;
    data->StartIdx = StartIdx;
    data->RealtimeStockNum = 0;
    wxString country=wxT("ss");
    if ((*ss)[0]->GetId().StartsWith(wxT("6"))) country=wxT("sh");
    wxString Url(wxT("http://stock.finance.sina.com.cn/cgi-bin/stock/quote/quote.cgi?symbol="));
    Url << (*ss)[0]->GetId() << wxT("&country=") << country;
    /*for (size_t i=0;i<ss->size();i++){
        if (i==0){
            Url << (*ss)[i]->GetId() << wxT(".") <<(*ss)[i]->GetStockType();
        }
        else{
            Url << wxT("+") << (*ss)[i]->GetId() << wxT(".") <<(*ss)[i]->GetStockType();
        }
        data->RealtimeStockNum++;
        if (data->RealtimeStockNum>MaxRealtimeNum) break;
    }
    Url <<wxT("&f=sl1d1t1c1ohgv&e=.csv");*/
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    geturl->Create();
    geturl->Run();
}

void SinaStock::RetriveRealTimeData(StockList* ss, void* UserData){
    RealtimeFetchSeed++;
    FetchRealTimeData(ss,UserData);
}

void SinaStock::FetchHistoryData(Stock* s,int datatype,void* UserData){
    wxString keys[]={
        wxT("d"),
        wxT("w"),
        wxT("m"),
    };
    wxDateTime now = wxDateTime::Now();
    wxDateTime AYearBefore = now - wxDateSpan(1);
    SinaStock_UserData *data=new SinaStock_UserData();
    data->OrignUserData = UserData;
    data->rType = HISTORY_RETRIVE;
    data->FetchSeed = HistoryFetchSeed;
    data->StartIdx = datatype;
    data->HistoryStock = s;
    wxString Url= wxString::Format(wxT("%s%s.%s&d=%d&e=%d&f=%d&g=%s&a=%d&b=%d&c=%d&ignore=.csv"),
                wxT("http://ichart.finance.yahoo.com/table.csv?s="),
                s->GetId().c_str(),
                s->GetStockType().c_str(),
                now.GetMonth(),
                now.GetDay(),
                now.GetYear(),
                keys[datatype].c_str(),
                AYearBefore.GetMonth(),
                AYearBefore.GetDay(),
                AYearBefore.GetYear());
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    geturl->Create();
    geturl->Run();
}

void SinaStock::RetriveHistoryDayData(Stock* s, void* UserData){
    HistoryFetchSeed++;
    FetchHistoryData(s,0,UserData);
}

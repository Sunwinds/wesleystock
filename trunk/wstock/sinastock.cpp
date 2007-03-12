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
	wxString ValidId;
} SinaStock_UserData;

int SinaStock::GetProptiesNum(){
    return SinaStock::Props.size();
};


void SinaStock::OnUrlGetDone(wxUrlGetDoneEvent& event){
	wxString key1(wxT("当前价("));
	wxString key2(wxT("交易金额"));
	wxString key3(wxT("总股本("));
	wxString RealTimeKey(key1.wc_str(wxConvUTF8), *wxConvCurrent);
	wxString HistoryKey(key2.wc_str(wxConvUTF8), *wxConvCurrent);
	wxString ValidKey(key3.wc_str(wxConvUTF8), *wxConvCurrent);

    SinaStock_UserData* data = (SinaStock_UserData*)event.UserData;
    if (data->rType == REALTIME_RETRIVE){ //Real Time Data come!

		if (event.requestTime <validRequest){
			//it is a old request,and has been over time. skip it.
			delete(data);
			return;
		}
        
		wxLogStatus(wxT(""));
        if (data->FetchSeed == RealtimeFetchSeed){
            HtmlTableParser *p=new HtmlTableParser();
            MyHtmlParser parser(p);
            parser.Parse(event.Result);
			//p->DumpTable();
			if (data->StartIdx<0) //validate stock ID
			{
				wxString StockName = p->GetTitle().BeforeFirst(wxT('('));
				//Tell the App what we got
				wxStockValidateDoneEvent event(wxEVT_STOCK_VALIDATE_DONE,data->ValidId,StockName);
				((wxWindow*)data->OrignUserData)->AddPendingEvent(event);

			}
			else{
				int idx=p->GetTDIndex(RealTimeKey);
				if (idx>=0){
					(*stocks)[data->StartIdx]->SetPropertyValue(Props[0], p->GetValue(idx+1));
					(*stocks)[data->StartIdx]->SetPropertyValue(Props[1], p->GetValue(idx+3));
					(*stocks)[data->StartIdx]->SetPropertyValue(Props[2], p->GetValue(idx+19));
				}
				else{
					wxLogMessage(wxT("Not Found!"));
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
    }
    else if (data->rType == HISTORY_RETRIVE){
        if (data->FetchSeed == HistoryFetchSeed){
            HtmlTableParser *p=new HtmlTableParser();
            Stock*s = (Stock*)data->HistoryStock;
            MyHtmlParser parser(p);
            parser.Parse(event.Result);
			//p->DumpTable();
            int idx=p->GetTDIndex(HistoryKey);
            if (idx>=0){
                wxLogStatus(wxT("Get History Data From the url Done!"));
                while (idx<p->GetTDCount()){
                    wxDateTime date;
                    if (date.ParseDate(p->GetValue(idx+1))){
                        StockHistoryDataPiece *pp = new StockHistoryDataPiece;
                        if (pp){
                            s->AppendHistoryData(0,pp);
                            pp->data = date;
                            p->GetValue(idx+2).ToDouble(&pp->open);
                            p->GetValue(idx+3).ToDouble(&pp->High);
                            p->GetValue(idx+4).ToDouble(&pp->Close);
                            p->GetValue(idx+5).ToDouble(&pp->Low);
                            pp->adjClose = pp->Close;

                            long v=0;
                            p->GetValue(idx+6).ToLong(&v);
                            pp->volume = v;

                        }
                        idx += 7;
                    }
                    else{
                        break;
                    }
                }
            }
            else{
                wxLogStatus(wxT("Not Found History Data From the url!"));
            }
            if (data->StartIdx == 3){ //if all 4 season data get done
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
    //static int MaxRealtimeNum = 15;
    stocks = ss;
    SinaStock_UserData *data=new SinaStock_UserData();
    data->OrignUserData = UserData;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = RealtimeFetchSeed;
    data->StartIdx = StartIdx;
    data->RealtimeStockNum = 1;
    wxString country=wxT("ss");
    if ((*ss)[StartIdx]->GetId().StartsWith(wxT("6"))) country=wxT("sh");
    wxString Url(wxT("http://stock.finance.sina.com.cn/cgi-bin/stock/quote/quote.cgi?symbol="));
    Url << (*ss)[StartIdx]->GetId() << wxT("&country=") << country;
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
    wxDateTime now = wxDateTime::Now();
    wxDateTime AYearBefore = now - wxDateSpan(1);
    SinaStock_UserData *data=new SinaStock_UserData();
    data->OrignUserData = UserData;
    data->rType = HISTORY_RETRIVE;
    data->FetchSeed = HistoryFetchSeed;
    data->StartIdx = datatype;
    data->HistoryStock = s;
    wxString country=wxT("ss");
    if (s->GetId().StartsWith(wxT("6"))) country=wxT("sh");
    wxString Url;
    if (datatype==0){
        Url << wxT("http://biz.finance.sina.com.cn/company/history.php?symbol=");
        Url << country <<s->GetId();
    }
    else{
        Url << wxT("http://biz.finance.sina.com.cn/company/history.php");
    }
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    if (datatype>0){
        wxDateTime date=wxDateTime::Now() - wxDateSpan(0,datatype * 3);
        wxString PostData;
        PostData << wxT("year=") << (int)date.GetYear() << wxT("&quarter=")
                  << (int)(((int)date.GetMonth()) / 3 + 1)
                  << wxT("&symbol=") <<country <<s->GetId();
        wxLogStatus(PostData);
        geturl->SetPostData(PostData);
    }
    geturl->Create();
    geturl->Run();
}

void SinaStock::ValidateStockId(wxWindow*Owner,const wxString& Id){
    SinaStock_UserData *data=new SinaStock_UserData();
    data->OrignUserData = Owner;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = RealtimeFetchSeed;
    data->StartIdx = -1;
    data->RealtimeStockNum = 1;
	data->ValidId = Id;
    wxString country=wxT("ss");
    if (Id.StartsWith(wxT("6"))) country=wxT("sh");
    wxString Url(wxT("http://stock.finance.sina.com.cn/cgi-bin/stock/quote/quote.cgi?symbol="));
    Url << Id << wxT("&country=") << country;
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    geturl->Create();
    geturl->Run();
}

void SinaStock::RetriveHistoryDayData(Stock* s, void* UserData){
    HistoryFetchSeed++;
    FetchHistoryData(s,0,UserData);
}

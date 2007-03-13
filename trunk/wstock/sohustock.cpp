#include "sohustock.h"
#include "htmltableparser.h"
#include <wx/tokenzr.h>
#include <wx/html/htmlpars.h>

BEGIN_EVENT_TABLE(SohuStock, StocksDataFetch)
    EVT_URL_GET_DONE(-1,SohuStock::OnUrlGetDone)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SohuStock, StocksDataFetch);

typedef struct{
    void * OrignUserData;
    Stock * HistoryStock;
    StockRetriveType rType;
    int FetchSeed;
    int StartIdx;
    int RealtimeStockNum;
	wxString ValidId;
} SohuStock_UserData;

int SohuStock::GetProptiesNum(){
    return SohuStock::Props.size();
};


void SohuStock::OnUrlGetDone(wxUrlGetDoneEvent& event){
	wxString key1(wxT("浮动盈亏"));
	wxString key2(wxT("交易金额"));
	wxString RealTimeKey(key1.wc_str(wxConvUTF8), *wxConvCurrent);
	wxString HistoryKey(key2.wc_str(wxConvUTF8), *wxConvCurrent);

    SohuStock_UserData* data = (SohuStock_UserData*)event.UserData;
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
				wxString StockName(wxT(""));
				int idx=p->GetTDIndex(RealTimeKey);
				if (idx>=0){
					StockName = p->GetValue(idx+2);
				}
				//Tell the App what we got
				wxStockValidateDoneEvent event(wxEVT_STOCK_VALIDATE_DONE,data->ValidId,StockName);
				((wxWindow*)data->OrignUserData)->AddPendingEvent(event);

			}
			else{
				int idx=p->GetTDIndex(RealTimeKey);
				wxASSERT(p->GetTDCount()>=(idx+8+(stocks->size()-1)*15));
				if (idx>=0){
					for (size_t i=0;i<stocks->size();i++){
						(*stocks)[i]->SetPropertyValue(Props[0], p->GetValue(idx+6+i*14));
						(*stocks)[i]->SetPropertyValue(Props[1], p->GetValue(idx+7+i*14));
						(*stocks)[i]->SetPropertyValue(Props[2], p->GetValue(idx+9+i*14));
						(*stocks)[i]->SetPropertyValue(Props[3], p->GetValue(idx+8+i*14));
					}
				}
				else{
					wxLogMessage(wxT("Not Found!"));
				}
				//Tell The main App we have finish this stocks's real time data fetch
				wxStockDataGetDoneEvent event(wxEVT_STOCK_DATA_GET_DONE,REALTIME_RETRIVE,
						data->OrignUserData);
				Parent->AddPendingEvent(event);
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
					wxString dateStr=p->GetValue(idx+1);
					dateStr = dateStr.Mid(0,4) + wxT("-") + dateStr.Mid(4,2) + wxT("-") + dateStr.Mid(6,2);
                    if (date.ParseDate(dateStr)){
                        StockHistoryDataPiece *pp = new StockHistoryDataPiece;
                        if (pp){
                            s->InsertHistoryData(data->StartIdx,pp);
                            pp->data = date;
                            p->GetValue(idx+2).ToDouble(&pp->open);
                            p->GetValue(idx+3).ToDouble(&pp->High);
                            p->GetValue(idx+4).ToDouble(&pp->Low);
                            p->GetValue(idx+5).ToDouble(&pp->Close);
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
            if (data->StartIdx == 2){ //if all day week and month data done!
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

void SohuStock::FetchRealTimeData(StockList* ss, void* UserData, int StartIdx){
    //static int MaxRealtimeNum = 15;
    stocks = ss;
    SohuStock_UserData *data=new SohuStock_UserData();
    data->OrignUserData = UserData;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = RealtimeFetchSeed;
    data->StartIdx = StartIdx;
    data->RealtimeStockNum = 1;
    wxString Url(wxT("http://stock.business.sohu.com/p/fl.php"));
    wxString PostData(wxT("code="));
    for (size_t i=0;i<ss->size();i++){
        if (i==0){
            PostData << (*ss)[i]->GetId();
        }
        else{
            PostData << wxT(",") << (*ss)[i]->GetId();
        }
    }
	PostData <<wxT(",&flsopt=uptcode&opt=update");
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
	geturl->SetPostData(PostData);
    geturl->Create();
    geturl->Run();
}

void SohuStock::RetriveRealTimeData(StockList* ss, void* UserData){
    RealtimeFetchSeed++;
    FetchRealTimeData(ss,UserData);
}

void SohuStock::FetchHistoryData(Stock* s,int datatype,void* UserData){
    wxDateTime now = wxDateTime::Now();
    wxDateTime AYearBefore = now - wxDateSpan(1);
    SohuStock_UserData *data=new SohuStock_UserData();
    data->OrignUserData = UserData;
    data->rType = HISTORY_RETRIVE;
    data->FetchSeed = HistoryFetchSeed;
    data->StartIdx = datatype;
    data->HistoryStock = s;
    wxString Url(wxT("http://stock.business.sohu.com/q/hp.php"));
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
    wxDateTime date=now - wxDateSpan(1);
    wxString PostData;
	wxString ss(wxT("d"));
	switch (datatype){
	case 1:
		ss = wxT("w");
		break;
	case 2:
		ss = wxT("m");
		break;
	}
    PostData << wxT("code=") << s->GetId()
			 << wxT("&start_year=") << (int)date.GetYear()
			 << wxT("&start_month=") << wxString::Format(wxT("%02d"),((int)date.GetMonth())+1)
			 << wxT("&start_day=") << (int)date.GetDay()
			 << wxT("&end_year=") << (int)now.GetYear()
			 << wxT("&end_month=") << wxString::Format(wxT("%02d"),((int)now.GetMonth())+1)
			 << wxT("&end_day=") << (int)now.GetDay()
			 << wxT("&ss=") << ss;
    wxLogStatus(PostData);
    geturl->SetPostData(PostData);
    geturl->Create();
    geturl->Run();
}

void SohuStock::ValidateStockId(wxWindow*Owner,const wxString& Id){
    SohuStock_UserData *data=new SohuStock_UserData();
    data->OrignUserData = Owner;
    data->rType = REALTIME_RETRIVE;
    data->FetchSeed = RealtimeFetchSeed;
    data->StartIdx = -1;
    data->RealtimeStockNum = 1;
	data->ValidId = Id;
    wxString Url(wxT("http://stock.business.sohu.com/p/fl.php"));
    wxString PostData(wxT("code="));
    PostData << Id;
	PostData <<wxT(",&flsopt=uptcode&opt=update");
    wxLogStatus(Url);
    WStockGetUrl* geturl=new WStockGetUrl(this,Url,data);
	geturl->SetPostData(PostData);
    geturl->Create();
    geturl->Run();
}

void SohuStock::RetriveHistoryDayData(Stock* s, void* UserData){
    HistoryFetchSeed++;
    FetchHistoryData(s,0,UserData);
}

#include "app.h"
#include "wstockconfig.h"

#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/textfile.h"

#include "yahoostock.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StockList);

const wxEventType wxEVT_STOCK_DATA_GET_DONE = wxNewEventType();
const wxEventType wxEVT_STOCK_VALIDATE_DONE = wxNewEventType();
IMPLEMENT_DYNAMIC_CLASS(wxStockDataGetDoneEvent, wxNotifyEvent);
IMPLEMENT_DYNAMIC_CLASS(wxStockValidateDoneEvent, wxNotifyEvent);

void Stocks::InitHistoryDatas(int num){
	for (size_t i=0;i<stocks.size();i++){
		stocks[i]->InitHistoryData(num);
	}
}

void Stock::UpdateHistoryCalcProps(){
	//Update the 10Day avg price
	if (HistoryDatas.size()>0){//it is a rule, the first array is the day hostory data in descending order
		double price=0;
		if (HistoryDatas[0]->size()>5){
			for (int i=0;i<5;i++){
				price += (*HistoryDatas[0])[i]->adjClose;
			}
		}
		HistoryCalcProps[_("PRICE 5D AVG")] = wxString::Format(wxT("%.2f"),price/5);

        long amount=0;
		if (HistoryDatas[0]->size()>5){
			for (int i=0;i<5;i++){
				amount += (*HistoryDatas[0])[i]->volume;
			}
		}
        HistoryCalcProps[_("EXCHANGE 5D AVG")] = wxString::Format(wxT("%ld"),amount/5);
	}
}

wxString Stock::ExplainMePropValue(const wxString& v){
	if (v ==  _("PRICE 5D AVG")){
		wxString desc;
		if (HistoryDatas.size()>0){//it is a rule, the first array is the day hostory data in descending order
			//double price=0;
			if (HistoryDatas[0]->size()>5){
				for (int i=0;i<5;i++){
					desc += (*HistoryDatas[0])[i]->data.Format(wxT("%Y-%m-%d:"));
					desc += wxString::Format(wxT("%.2f\n"),(*HistoryDatas[0])[i]->adjClose);
				}
			}
		}
		return desc;
	}
	return wxT("");
}

void Stock::UpdateRealTimeCalcProps(){
    long v=0;
    double dv=0;
    RealTimeProps[_("EXCHANGE")].ToLong(&v);
    RealTimeProps[_("EXCHANGE PRICE")].ToDouble(&dv);
    if (v==0){
        RealTimeCalcProps[_("EXCHANGE AVG PRICE")] = wxT("N/A");
    }
    else{
        RealTimeCalcProps[_("EXCHANGE AVG PRICE")] = wxString::Format(wxT("%.3f"),dv/v);
    }
}

void Stock::InitHistoryData(int num){
	for (int i=0;i<num;i++){
		HistoryDatas.push_back(new StockHistoryDataArray());
	}
}

#define X(x) (xmlChar*)((const char*)x)
void Stocks::SaveStockIndex(){
    xmlDocPtr doc;
    xmlNodePtr stock;
    doc = xmlNewDoc(X("1.0"));
    doc->children = xmlNewDocNode(doc, NULL, X("Stocks"), NULL);
    xmlDocSetRootElement(doc, doc->children);
    for (size_t i=0;i<stocks.size();i++)    {
		Stock*s =stocks[i];
		stock = xmlNewChild(doc->children, NULL, X("Stock"), NULL);
		wxString utfid(wxConvCurrent->cWX2WC(s->GetId().c_str()),wxConvUTF8);
		wxString utfname(wxConvCurrent->cWX2WC(s->GetName().c_str()),wxConvUTF8);
		xmlSetProp(stock,X("Id"),X(utfid.mb_str()));
		xmlSetProp(stock,X("Name"),X(utfname.mb_str()));
    }

    xmlSaveFormatFileEnc((const char*)WStockConfig::GetKeyPath().mb_str(),doc,"utf-8",1);
    xmlFreeDoc(doc);
}

bool Stocks::Init(int HistoryDataNum){
    wxString keyPath=WStockConfig::GetKeyPath();
    wxFileName keyf(keyPath);
    keyf.MakeAbsolute();
    if (keyf.FileExists()){
        //Load Stocks Name From file
        //wxTextFile file;
		xmlDocPtr doc = xmlParseFile((const char*)keyf.GetFullPath().mb_str());
		if (doc == NULL ) {
			wxLogError(_("Document not parsed successfully. \n"));
			return false;
		}
        for (xmlNodePtr node=doc->children->children;node;node=node->next){
            if (xmlStrcmp(node->name,(const xmlChar*)"Stock")==0){
				wxString Id(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"Id")),*wxConvCurrent);
				wxString Name(wxConvUTF8.cMB2WC(
					(char*)xmlGetProp(node, (const xmlChar*)"Name")),*wxConvCurrent);
				if (!Id.IsEmpty()){
                    Stock*s = new Stock(Id,Name);
					s->InitHistoryData(HistoryDataNum);
                    s->LoadHistoryDataFromFile();
                    stocks.Append(s);
				}
            }
        }
        xmlFreeDoc(doc);
    }
    else{
        wxLogError(_("File %s does not exists!"),keyf.GetFullPath().c_str());
    }

    return true;
}

Stock::~Stock(){
    unsigned int i;
	for (size_t j=0;j<HistoryDatas.size();j++){
		for (i = 0; i < HistoryDatas[j]->GetCount(); i++)
		{
			StockHistoryDataPiece*p = (*HistoryDatas[j])[i];
			delete(p);
		}
        HistoryDatas[j]->Clear();
	}
}

Stocks::~Stocks(){
    StockList::Node* node = stocks.GetFirst();
    while (node)
    {
        Stock* p = node->GetData();
        delete (p);
        node = node->GetNext();
    }
    stocks.Clear();
}

bool Stock::LoadHistoryDataFromFile(){
	wxDateTime now = wxDateTime::Now();
	for (size_t i=0;i<HistoryDatas.size();i++){
		wxFileName fn(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_%d.dat"),
				GetId().c_str(),
				GetStockType().c_str(),
				now.GetYear(),
				now.GetMonth()+1,
				now.GetDay(),
				i));
        fn.MakeAbsolute();
		if (fn.FileExists()){

			xmlDocPtr doc = xmlParseFile((const char*)fn.GetFullPath().mb_str());
			if (doc == NULL ) {
				wxLogError(_("Document not parsed successfully. \n"));
				return false;
			}
			for (xmlNodePtr node=doc->children->children;node;node=node->next){
				if (xmlStrcmp(node->name,(const xmlChar*)"HistoryData")==0){
					wxString Year(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Year")),*wxConvCurrent);
					wxString Month(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Month")),*wxConvCurrent);
					wxString Day(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Day")),*wxConvCurrent);
					wxString Open(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Open")),*wxConvCurrent);
					wxString High(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"High")),*wxConvCurrent);
					wxString Low(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Low")),*wxConvCurrent);
					wxString Close(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"Close")),*wxConvCurrent);
					wxString volume(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"volume")),*wxConvCurrent);
					wxString adjClose(wxConvUTF8.cMB2WC(
						(char*)xmlGetProp(node, (const xmlChar*)"adjClose")),*wxConvCurrent);

					if (!Year.IsEmpty()){
						StockHistoryDataPiece *p = new StockHistoryDataPiece;
						long y,m,d;
						Year.ToLong(&y);
						Month.ToLong(&m);
						Day.ToLong(&d);
						HistoryDatas[i]->push_back(p);
						p->data  = wxDateTime(d,(wxDateTime::Month)m,y);
						Open.ToDouble(&p->open);
						High.ToDouble(&p->High);
						Low.ToDouble(&p->Low);
						Close.ToDouble(&p->Close);
						volume.ToLong(&m);
						p->volume = m;
						adjClose.ToDouble(&p->adjClose);
					}
				}
			}
			xmlFreeDoc(doc);
		}
		else{
			return false;
		}
	}
	UpdateHistoryCalcProps();
	HistoryDataReady=true;
    return true;
}

bool Stock::SaveHistoryDataToFile(){
    wxDateTime now = wxDateTime::Now();
    HistoryDataReady = true;
	for (size_t idx=0;idx<HistoryDatas.size();idx++){
		wxFileName fn(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_%d.dat"),
				GetId().c_str(),
				GetStockType().c_str(),
				now.GetYear(),
				now.GetMonth()+1,
				now.GetDay(),
				idx));
		fn.MakeAbsolute();
		if (!fn.DirExists(fn.GetPath())){
			if (!fn.Mkdir(fn.GetPath())){
				wxLogError(_("Try to create directory %s fail!"),fn.GetPath().c_str());
				return false;
			}
		}

		xmlDocPtr doc;
		xmlNodePtr node;
		doc = xmlNewDoc(X("1.0"));
		doc->children = xmlNewDocNode(doc, NULL, X("HistoryDatas"), NULL);
		xmlDocSetRootElement(doc, doc->children);
		size_t i;
		for (i =0;i<HistoryDatas[idx]->GetCount();i++){
			node = xmlNewChild(doc->children, NULL, X("HistoryData"), NULL);
			char y[20]="",
				 m[20]="",
				 d[20]="",
				 open[20]="",
				 High[20]="",
				 Low[20]="",
				 Close[20]="",
				 volume[20]="",
				 adjClose[20]="";
			sprintf(y,"%d",(*HistoryDatas[idx])[i]->data.GetYear());
			sprintf(m,"%d",(*HistoryDatas[idx])[i]->data.GetMonth());
			sprintf(d,"%d",(*HistoryDatas[idx])[i]->data.GetDay());
			sprintf(open,"%.2f",(*HistoryDatas[idx])[i]->open);
			sprintf(High,"%.2f",(*HistoryDatas[idx])[i]->High);
			sprintf(Low,"%.2f",(*HistoryDatas[idx])[i]->Low);
			sprintf(Close,"%.2f",(*HistoryDatas[idx])[i]->Close);
			sprintf(volume,"%d",(*HistoryDatas[idx])[i]->volume);
			sprintf(adjClose,"%.2f",(*HistoryDatas[idx])[i]->adjClose);

			xmlSetProp(node,X("Year"),X(y));
			xmlSetProp(node,X("Month"),X(m));
			xmlSetProp(node,X("Day"),X(d));
			xmlSetProp(node,X("Open"),X(open));
			xmlSetProp(node,X("High"),X(High));
			xmlSetProp(node,X("Low"),X(Low));
			xmlSetProp(node,X("Close"),X(Close));
			xmlSetProp(node,X("volume"),X(volume));
			xmlSetProp(node,X("adjClose"),X(adjClose));
		}

		xmlSaveFormatFileEnc((const char*)fn.GetFullPath().mb_str(),doc,"utf-8",1);
		xmlFreeDoc(doc);

	}
    return true;
}

Stock *Stocks::GetStockById(const wxString& id){
        StockList::Node* node = stocks.GetFirst();
        while (node)
        {
            Stock* pstock = node->GetData();
            if (pstock->GetId() == id) return pstock;
            node = node->GetNext();
        }
        return NULL;
}

double MyStocks::GetTotalEarning(){
        double ret=0;
        MyStockDataHash::iterator i = datas.begin();
        while (i != datas.end()){
            double CurValue=0;
            MyStockStru* pmystock = i->second;
            pmystock->stock->GetRealTimeValue(_("PRICE")).ToDouble(&CurValue);
            ret += pmystock->GetEarnings(CurValue);
            i++;
        }
        return ret;
}

double MyStocks::GetTotalPay(){
        double ret=0;
        MyStockDataHash::iterator i = datas.begin();
        while (i != datas.end()){
            MyStockStru* pmystock = i->second;
            ret += (pmystock->GetTotalPay());
            i++;
        }
        return ret;
}

double MyStocks::GetCurrentTotal(){
        double ret=0;
        MyStockDataHash::iterator i = datas.begin();
        while (i != datas.end()){
            double CurValue=0;
            MyStockStru* pmystock = i->second;
            pmystock->stock->GetRealTimeValue(_("PRICE")).ToDouble(&CurValue);
            ret += pmystock->GetCurValue(CurValue);
            i++;
        }
        return ret;
}

wxString MyStocks::GetMyStockTotalinfo(const wxString& key){
    if (key == _("MyStock Total Earning")){
        return wxString::Format(wxT("%.2f"),GetTotalEarning());
    }
    else if (key == _("MyStock Total Earning Rate")){
        double totalpay =GetTotalPay();
        if (totalpay!=0){
            return wxString::Format(wxT("%.3f%%"),GetTotalEarning()/totalpay*100);
        }
        else{
            return wxT("N/A");
        }
    }
    else if (key == _("MyStock Total Money")){
        return wxString::Format(wxT("%.2f"),GetCurrentTotal());
    }
    return wxT("");
}

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
IMPLEMENT_DYNAMIC_CLASS(wxStockDataGetDoneEvent, wxNotifyEvent);

bool Stocks::Init(){
    wxString keyPath=WStockConfig::GetKeyPath();
    wxFileName keyf(keyPath);
    keyf.MakeAbsolute();
    if (wxFileName::FileExists(keyPath)){
        //Load Stocks Name From file
        wxTextFile file;
        if (file.Open(keyPath))
        {
            size_t i;
            for (i = 0; i < file.GetLineCount(); i++)
            {
                if (!file[i].StartsWith(wxT("#"))){
                    wxString si = wxString(wxConvUTF8.cMB2WC(file[i].c_str()),*wxConvCurrent).Strip(wxString::both); //Strip Both Side
                    wxString id = si.BeforeFirst(wxT(' '));
                    wxString name = si.AfterFirst(wxT(' '));
                    name = name.Strip(wxString::both);
                    if (id.Len()>0){
                        Stock*s = new Stock(id,name);
                        s->LoadHistoryDataFromFile();
                        stocks.Append(s);
                    }
                }
            }
        }
    }
    else{
        wxLogError(_("File %s does not exists!"),keyf.GetFullPath().c_str());
    }

    return true;
}

Stock::~Stock(){
    for (unsigned int i = 0; i < DayHistoryData.GetCount(); i++)
    {
        StockHistoryDataPiece*p = DayHistoryData[i];
        delete(p);
    }
    DayHistoryData.Clear();

    for (i = 0; i < MonthHistoryData.GetCount(); i++)
    {
        StockHistoryDataPiece*p = MonthHistoryData[i];
        delete(p);
    }
    MonthHistoryData.Clear();

    for (i = 0; i < WeekHistoryData.GetCount(); i++)
    {
        StockHistoryDataPiece*p = WeekHistoryData[i];
        delete(p);
    }
    WeekHistoryData.Clear();
}

Stocks::~Stocks(){
    StockList::Node* node = stocks.GetFirst();
    // 节点遍历
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
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_D.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    if (fn.FileExists()){
        wxFileInputStream input(fn.GetFullPath());
        wxDataInputStream store( input );
        while (!input.Eof()){
            StockHistoryDataPiece *p = new StockHistoryDataPiece;
            int y,m,d;
            store >> y >> m >> d;
            if (input.Eof()){
                delete (p);
                break;
            }
            DayHistoryData.push_back(p);
            p->data  = wxDateTime(d,(wxDateTime::Month)m,y);
            store   >> p->open
                    >> p->High
                    >> p->Low
                    >> p->Close
                    >> p->volume
                    >> p->adjClose;
        }
    }
    else{
        return false;
    }

    wxFileName fn2(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_W.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    if (fn2.FileExists()){
        wxFileInputStream input(fn2.GetFullPath());
        wxDataInputStream store( input );
        while (!input.Eof()){
            StockHistoryDataPiece *p = new StockHistoryDataPiece;
            int y,m,d;
            store >> y >> m >> d;
            if (input.Eof()){
                delete (p);
                break;
            }
            WeekHistoryData.push_back(p);
            p->data  = wxDateTime(d,(wxDateTime::Month)m,y);
            store   >> p->open
                    >> p->High
                    >> p->Low
                    >> p->Close
                    >> p->volume
                    >> p->adjClose;
        }
    }
    else{
        return false;
    }

    wxFileName fn3(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_M.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    if (fn3.FileExists()){
        wxFileInputStream input(fn3.GetFullPath());
        wxDataInputStream store( input );
        while (!input.Eof()){
            StockHistoryDataPiece *p = new StockHistoryDataPiece;
            int y,m,d;
            store >> y >> m >> d;
            if (input.Eof()){
                delete (p);
                break;
            }
            MonthHistoryData.push_back(p);
            p->data  = wxDateTime(d,(wxDateTime::Month)m,y);
            store   >> p->open
                    >> p->High
                    >> p->Low
                    >> p->Close
                    >> p->volume
                    >> p->adjClose;
        }
    }
    else{
        return false;
    }

    return true;
}

bool Stock::SaveHistoryDataToFile(){
    wxDateTime now = wxDateTime::Now();
    wxFileName fn(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_D.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    fn.MakeAbsolute();
    if (!fn.DirExists(fn.GetPath())){
        if (!fn.Mkdir(fn.GetPath())){
            wxLogError(_("Try to create directory %s fail!"),fn.GetPath().c_str());
            return false;
        }
    }

    wxFileOutputStream output(fn.GetFullPath());
    if (!output.Ok()){
        wxLogError(_("Try to open file output stream %s fail!"),fn.GetFullPath().c_str());
        return false;
    }
    wxDataOutputStream store(output);
    for (size_t i =0;i<DayHistoryData.GetCount();i++){
        int y=DayHistoryData[i]->data.GetYear();
        int m=DayHistoryData[i]->data.GetMonth();
        int d=DayHistoryData[i]->data.GetDay();
        store << y
              << m
              << d
              << DayHistoryData[i]->open
              << DayHistoryData[i]->High
              << DayHistoryData[i]->Low
              << DayHistoryData[i]->Close
              << DayHistoryData[i]->volume
              << DayHistoryData[i]->adjClose;
    }

    wxFileName fn2(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_W.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    fn2.MakeAbsolute();
    wxFileOutputStream output2(fn2.GetFullPath());
    if (!output2.Ok()){
        wxLogError(_("Try to open file output stream %s fail!"),fn2.GetFullPath().c_str());
        return false;
    }
    wxDataOutputStream store2(output2);
    for (i =0;i<WeekHistoryData.GetCount();i++){
        int y=WeekHistoryData[i]->data.GetYear();
        int m=WeekHistoryData[i]->data.GetMonth();
        int d=WeekHistoryData[i]->data.GetDay();
        store2 << y
              << m
              << d
              << WeekHistoryData[i]->open
              << WeekHistoryData[i]->High
              << WeekHistoryData[i]->Low
              << WeekHistoryData[i]->Close
              << WeekHistoryData[i]->volume
              << WeekHistoryData[i]->adjClose;
    }

    wxFileName fn3(WStockConfig::GetHistoryDataDir(),wxString::Format(wxT("%s.%s.%d_%d_%d_M.dat"),
            GetId().c_str(),
            GetStockType().c_str(),
            now.GetYear(),
            now.GetMonth(),
            now.GetDay()));
    fn3.MakeAbsolute();
    wxFileOutputStream output3(fn3.GetFullPath());
    if (!output3.Ok()){
        wxLogError(_("Try to open file output stream %s fail!"),fn3.GetFullPath().c_str());
        return false;
    }
    wxDataOutputStream store3(output3);
    for (i =0;i<MonthHistoryData.GetCount();i++){
        int y=MonthHistoryData[i]->data.GetYear();
        int m=MonthHistoryData[i]->data.GetMonth();
        int d=MonthHistoryData[i]->data.GetDay();
        store3 << y
              << m
              << d
              << MonthHistoryData[i]->open
              << MonthHistoryData[i]->High
              << MonthHistoryData[i]->Low
              << MonthHistoryData[i]->Close
              << MonthHistoryData[i]->volume
              << MonthHistoryData[i]->adjClose;
    }

    return true;
}

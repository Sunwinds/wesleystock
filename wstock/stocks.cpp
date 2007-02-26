#include "app.h"
#include "wstockconfig.h"
#include "wx/filename.h"
#include "wx/textfile.h"
#include "yahoostock.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StockList);

const wxEventType wxEVT_STOCK_DATA_GET_DONE = wxNewEventType();
IMPLEMENT_DYNAMIC_CLASS(wxStockDataGetDoneEvent, wxNotifyEvent);

bool Stocks::Init(){
    wxConfig config(APP_CFG, VENDOR_CFG);
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
                    wxString si = file[i].Strip(wxString::both); //Strip Both Side
                    wxString id = si.BeforeFirst(wxT(' '));
                    wxString name = si.AfterFirst(wxT(' '));
                    if (id.Len()>0){
                        stocks.Append(new Stock(id,name));
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

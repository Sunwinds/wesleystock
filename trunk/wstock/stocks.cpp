#include "app.h"
#include "wstockconfig.h"
#include "wx/filename.h"
#include "wx/textfile.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StockList);

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
                    if (si.Len()>0){
                        stocks.Append(new Stock(si));
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

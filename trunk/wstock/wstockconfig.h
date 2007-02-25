#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <wx/config.h>
#include "wstockconst.h"

extern wxConfig config;
class WStockConfig :public wxObject
{
    public:
        static wxString GetKeyPath(){
            wxString keyPath = wxT("stocks.txt");
            config.Read(STOCK_KEY_PATH, &keyPath);
            return keyPath;
        };

        static wxString SetKeyPath(const wxString& keyPath){
            config.Write(STOCK_KEY_PATH, keyPath);
            return keyPath;
        };
        static wxString GetProxy(){
            wxString keyPath = wxT("");
            config.Read(wxT("General/Proxy"), &keyPath);
            return keyPath;
        }
};

#endif // CONFIG_H_INCLUDED

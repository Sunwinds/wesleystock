#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "wstockconst.h"

class WStockConfig :public wxObject
{
    public:
        static wxString GetKeyPath(){
            wxConfig config(APP_CFG, VENDOR_CFG);
            wxString keyPath = wxT("stocks.txt");
            config.Read(STOCK_KEY_PATH, &keyPath);
            return keyPath;
        };

        static wxString SetKeyPath(const wxString& keyPath){
            wxConfig config(APP_CFG, VENDOR_CFG);
            config.Write(STOCK_KEY_PATH, keyPath);
            return keyPath;
        };

};

#endif // CONFIG_H_INCLUDED

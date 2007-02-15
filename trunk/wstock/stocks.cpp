#include "app.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StockList);

bool Stocks::Init(){
    wxConfig config(APP_CFG, VENDOR_CFG);
    wxString keyPath = wxT("stocks.xml");
    config.Read(STOCK_KEY_PATH, &keyPath);

    return true;
}

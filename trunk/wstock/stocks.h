#ifndef STOCKS_H_INCLUDED
#define STOCKS_H_INCLUDED

#include "wx/wx.h"

class Stock :public wxObject
{
    public:
        Stock(const wxString& si){
            StockId = si;
        };
    private:
        wxString StockName, // The Company name
                 StockId,     // for example: 600000
                 StockType; //  SS or SZ (沪市或者深市)

};
WX_DECLARE_LIST(Stock, StockList);

class Stocks:public wxObject
{
    public:
        Stocks(){};
        bool Init();
        int GetStockNum(){ return stocks.size();};
    private:
        StockList stocks;
};

#endif // STOCKS_H_INCLUDED

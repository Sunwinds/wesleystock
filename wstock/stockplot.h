#ifndef STOCKPLOT_H_INCLUDED
#define STOCKPLOT_H_INCLUDED

#include <wx/wx.h>
#include "stocks.h"

class wxPlotWindow:public wxScrolledWindow
{
    public:
        wxPlotWindow(wxWindow *parent, int id);
        virtual void OnDraw(wxDC& dc);
        void SetStock(Stock* s);
        void DrawPricePlot(wxDC&dc, StockHistoryDataArray& value,bool DrawXY=true,wxColour c=*wxBLACK);
    private:
        Stock* stock;
};

#endif // STOCKPLOT_H_INCLUDED

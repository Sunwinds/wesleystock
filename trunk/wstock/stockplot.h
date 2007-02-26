#ifndef STOCKPLOT_H_INCLUDED
#define STOCKPLOT_H_INCLUDED

#include <wx/wx.h>

class wxPlotWindow:public wxScrolledWindow
{
    public:
    wxPlotWindow(wxWindow *parent, int id);
    virtual void OnDraw(wxDC& dc);
};

#endif // STOCKPLOT_H_INCLUDED

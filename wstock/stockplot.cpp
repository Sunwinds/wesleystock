#include "stockplot.h"


wxPlotWindow::wxPlotWindow(wxWindow*parent, int id):wxScrolledWindow(parent, wxID_ANY, wxPoint(0,0),
                           wxSize(200,200),
                           wxSUNKEN_BORDER |
                           wxNO_FULL_REPAINT_ON_RESIZE |
                           wxVSCROLL | wxHSCROLL)
{
    SetBackgroundColour(wxColour(_T("WHITE")));
};

void wxPlotWindow::OnDraw(wxDC& dc)
{
    dc.SetFont(*wxSWISS_FONT);
    dc.SetPen(*wxGREEN_PEN);
    wxPoint points[]={
        wxPoint(10,100),
        wxPoint(50,180),
        wxPoint(90,100),
    };
    dc.DrawSpline(WXSIZEOF(points),points);
}

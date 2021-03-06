// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Feb 26 17:05:25 2007

#include <wx/wx.h>
#include <wx/image.h>

#include "stockplot.h"

#ifndef STOCKHISTORYDIALOG_H
#define STOCKHISTORYDIALOG_H

// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade


class StockHistoryDialog: public wxDialog {
public:
    // begin wxGlade: StockHistoryDialog::ids
    // end wxGlade

    StockHistoryDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(100,100), long style=wxDEFAULT_DIALOG_STYLE);
    void SetStock(Stock* s);
private:
    // begin wxGlade: StockHistoryDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: StockHistoryDialog::attributes
    wxPlotWindow* m_chart;
    wxTextCtrl* text_ctrl_1;
    wxStaticLine* static_line_1;
    wxButton* button_1;
    // end wxGlade
    Stock* stock;
}; // wxGlade: end class


#endif // STOCKHISTORYDIALOG_H

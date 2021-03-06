// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Feb 26 17:05:25 2007

#include "StockHistoryDialog.h"


StockHistoryDialog::StockHistoryDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: StockHistoryDialog::StockHistoryDialog
    m_chart = new wxPlotWindow(this, -1);
    text_ctrl_1 = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    static_line_1 = new wxStaticLine(this, -1);
    button_1 = new wxButton(this, wxID_CANCEL, _("OK"));

    set_properties();
    do_layout();
    // end wxGlade
}


void StockHistoryDialog::set_properties()
{
    // begin wxGlade: StockHistoryDialog::set_properties
    m_chart->SetMinSize(wxSize(450,250));
    text_ctrl_1->SetBackgroundColour(wxColour(192, 192, 192));
    text_ctrl_1->SetForegroundColour(wxColour(0, 0, 0));
    // end wxGlade

}


void StockHistoryDialog::do_layout()
{
    // begin wxGlade: StockHistoryDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
    sizer_3->Add(m_chart, 1, wxEXPAND, 0);
    sizer_3->Add(text_ctrl_1, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_1->Add(sizer_3, 1, wxEXPAND, 0);
    sizer_1->Add(static_line_1, 0, wxEXPAND, 0);
    sizer_2->Add(20, 20, 1, wxADJUST_MINSIZE, 0);
    sizer_2->Add(button_1, 0, wxALL|wxADJUST_MINSIZE, 5);
    sizer_1->Add(sizer_2, 0, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();
    // end wxGlade
}

void StockHistoryDialog::SetStock(Stock* s){
    stock = s;
    m_chart->SetStock(stock);
}


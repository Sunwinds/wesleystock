// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Feb 26 17:05:25 2007

#include "StockHistoryDialog.h"


StockHistoryDialog::StockHistoryDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: StockHistoryDialog::StockHistoryDialog
    m_chart = new wxPlotWindow(this, -1);
    label_1 = new wxStaticText(this, -1, wxT("label_1"));
    label_2 = new wxStaticText(this, -1, wxT("label_2"));
    static_line_1 = new wxStaticLine(this, -1);
    button_1 = new wxButton(this, wxID_CANCEL, wxT("OK"));

    set_properties();
    do_layout();
    // end wxGlade
}


void StockHistoryDialog::set_properties()
{
    // begin wxGlade: StockHistoryDialog::set_properties
    m_chart->SetMinSize(wxSize(400,200));
    // end wxGlade

}


void StockHistoryDialog::do_layout()
{
    // begin wxGlade: StockHistoryDialog::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    sizer_3->Add(m_chart, 1, wxEXPAND, 0);
    sizer_4->Add(label_1, 0, wxADJUST_MINSIZE, 0);
    sizer_4->Add(label_2, 0, wxADJUST_MINSIZE, 0);
    sizer_3->Add(sizer_4, 1, wxEXPAND, 0);
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


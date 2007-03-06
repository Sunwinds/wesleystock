// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Mar  1 16:31:25 2007

#include "MyStockDialog.h"
#include <wx/valgen.h>


MyStockDialog::MyStockDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: MyStockDialog::MyStockDialog
    label_1 = new wxStaticText(this, -1, wxT("Stock Id:"));
    t_stockid = new wxTextCtrl(this, -1, wxT(""));
    label_2 = new wxStaticText(this, -1, wxT("Acount:"));
    t_acount = new wxTextCtrl(this, -1, wxT(""));
    label_3 = new wxStaticText(this, -1, wxT("Price:"));
    t_price = new wxTextCtrl(this, -1, wxT(""));
    label_9 = new wxStaticText(this, -1, wxT("Operate:"));
    const wxString c_op_choices[] = {
        wxT("Buy"),
        wxT("Sell")
    };
    c_op = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 2, c_op_choices, wxCB_DROPDOWN|wxCB_READONLY);
    static_line_2 = new wxStaticLine(this, -1);
    button_2 = new wxButton(this, wxID_CANCEL, wxT(""));
    button_3 = new wxButton(this, wxID_OK, wxT(""));

    set_properties();
    do_layout();
    // end wxGlade

    t_stockid->SetValidator(wxGenericValidator(&data.StockId));
    t_acount->SetValidator(wxGenericValidator(&data.ACount));
    t_price->SetValidator(wxDoubleValidator(&data.Price));
    c_op->SetValidator(wxGenericValidator(&data.Op));

}


void MyStockDialog::set_properties()
{
    // begin wxGlade: MyStockDialog::set_properties
    SetTitle(wxT("MyStock"));
    c_op->SetSelection(-1);
    button_3->SetDefault();
    // end wxGlade
}


void MyStockDialog::do_layout()
{
    // begin wxGlade: MyStockDialog::do_layout
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(4, 2, 0, 0);
    grid_sizer_1->Add(label_1, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5);
    grid_sizer_1->Add(t_stockid, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_1->Add(label_2, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5);
    grid_sizer_1->Add(t_acount, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_1->Add(label_3, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5);
    grid_sizer_1->Add(t_price, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_1->Add(label_9, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 5);
    grid_sizer_1->Add(c_op, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_1->AddGrowableCol(1);
    sizer_4->Add(grid_sizer_1, 1, wxALL|wxEXPAND, 5);
    sizer_4->Add(static_line_2, 0, wxEXPAND, 0);
    sizer_5->Add(20, 20, 1, wxADJUST_MINSIZE, 0);
    sizer_5->Add(button_2, 0, wxALL|wxADJUST_MINSIZE, 5);
    sizer_5->Add(button_3, 0, wxALL|wxADJUST_MINSIZE, 5);
    sizer_4->Add(sizer_5, 0, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_4);
    sizer_4->Fit(this);
    sizer_4->SetSizeHints(this);
    Layout();
    // end wxGlade
}


// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Mar  1 16:31:25 2007

#include <wx/wx.h>
#include <wx/image.h>

#ifndef MYSTOCKDIALOG_H
#define MYSTOCKDIALOG_H

// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade

class wxDoubleValidator: public wxValidator
{
public:
  wxDoubleValidator(double* val){v=val;};
  ~wxDoubleValidator(){};
  virtual wxObject *Clone() const { return new wxDoubleValidator(v); }
  wxDoubleValidator(const wxDoubleValidator& val)
    : wxValidator()
  {
     v= val.v;
  };
  virtual bool Validate(wxWindow * WXUNUSED(parent)) {
      return true;
  }
  virtual bool TransferToWindow(){
      if ( !m_validatorWindow )
        return false;

    if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
    {
        wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
        if (v)
        {
            pControl->SetValue(wxString::Format(wxT("%.2f"),*v));
            return true;
        }
    }
    return false;
};
  virtual bool TransferFromWindow(){
    if ( !m_validatorWindow )
        return false;

    if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
    {
        wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;
        if (v)
        {
            pControl->GetValue().ToDouble(v);
            return true;
        }
    }
    return false;
  };
protected:
    double *v;
};

class MyStockDialogData
{
 public:
    MyStockDialogData(){
        StockId=wxT("600171");
        ACount=3100;
        Price=3.87;
    };
    MyStockDialogData(wxString id, int a, double p){
        StockId = id;
        ACount = a;
        Price = p;
    };
    MyStockDialogData(const MyStockDialogData& v){
        StockId = v.StockId;
        ACount = v.ACount;
        Price = v.Price;
    };
    MyStockDialogData & operator = (const MyStockDialogData& v){
        StockId = v.StockId;
        ACount = v.ACount;
        Price = v.Price;
        return *this;
    };
    wxString StockId;
    int ACount;
    double Price;
};


class MyStockDialog: public wxDialog {
public:
    // begin wxGlade: MyStockDialog::ids
    // end wxGlade

    MyStockDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
    void SetData(const MyStockDialogData& d){
        data=d;
        TransferDataToWindow();
    };
    const MyStockDialogData& GetData(){return data;};

private:
    // begin wxGlade: MyStockDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade
    MyStockDialogData data;

protected:
    // begin wxGlade: MyStockDialog::attributes
    wxStaticText* label_1;
    wxTextCtrl* t_stockid;
    wxStaticText* label_2;
    wxTextCtrl* t_acount;
    wxStaticText* label_3;
    wxTextCtrl* t_price;
    wxStaticLine* static_line_2;
    wxButton* button_2;
    wxButton* button_3;
    // end wxGlade
}; // wxGlade: end class


#endif // MYSTOCKDIALOG_H

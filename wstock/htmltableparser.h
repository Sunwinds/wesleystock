#ifndef HTMLTABLEPARSER_H_INCLUDED
#define HTMLTABLEPARSER_H_INCLUDED

#include "wx/wx.h"
#include <wx/html/htmlpars.h>

WX_DECLARE_LIST(wxArrayString, TableDataType);
WX_DECLARE_LIST(TableDataType, TableListType);

class HtmlTableParser:public wxHtmlTagHandler
{
    public:
        HtmlTableParser(){tables.DeleteContents(true);};
        ~HtmlTableParser(){tables.Clear();};
        bool HandleTag(const wxHtmlTag& tag);
        wxString GetCellValue(int tblidx, int r, int c);
        wxSize GetTableSize(int tblidx);
        int GetTableNumber(){return tables.size();};
        int FindTable(int r, int c, wxString subv);
        virtual wxString GetSupportedTags(){
            return wxT("TABLE,TR,TD");
        };
        wxString GetPureText(wxString html);
        void HandleText(const wxChar* txt);
        void SetRecordText(bool b){RecText=b;};
        void DumpTable();
    private:
        TableListType tables;
        bool RecText;
        wxString Text;
};

class MyHtmlParser:public wxHtmlParser
{
    public:
        MyHtmlParser(HtmlTableParser* p){
            Parser=p;
            AddTagHandler(p);
        };
        virtual wxObject* GetProduct(){return NULL;};
        virtual void AddText(const wxChar* txt){
            Parser->HandleText(txt);
        };
    private:
        HtmlTableParser* Parser;
};


#endif // HTMLTABLEPARSER_H_INCLUDED

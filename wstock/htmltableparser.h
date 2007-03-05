#ifndef HTMLTABLEPARSER_H_INCLUDED
#define HTMLTABLEPARSER_H_INCLUDED

#include "wx/wx.h"
#include <wx/html/htmlpars.h>

class HtmlTableParser:public wxHtmlTagHandler
{
    public:
        HtmlTableParser(){};
        ~HtmlTableParser(){};
        bool HandleTag(const wxHtmlTag& tag);
        virtual wxString GetSupportedTags(){
            return wxT("TD");
        };
        wxString GetPureText(wxString html);
        void HandleText(const wxChar* txt);
        wxString GetValue(int idx){return tdtexts[idx];};
        int GetTDIndex(const wxString& value);
        int GetTDCount(){ return tdtexts.size();};
        void DumpTable();
    private:
        wxArrayString tdtexts;
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

#ifndef HTMLTABLEPARSER_H_INCLUDED
#define HTMLTABLEPARSER_H_INCLUDED

#include "wx/wx.h"
#include <wx/html/htmlpars.h>

class HtmlTableParser:public wxHtmlTagHandler
{
    public:
        HtmlTableParser(){WantBigTd=false;};
        ~HtmlTableParser(){};
        bool HandleTag(const wxHtmlTag& tag);
		void SetWantBigTd(bool v){WantBigTd=v;};
        virtual wxString GetSupportedTags(){
            return wxT("TD,TITLE");
        };
        wxString GetPureText(wxString html);
        void HandleText(const wxChar* txt);
        wxString GetValue(int idx){ if (idx<(int)tdtexts.size()) return tdtexts[idx]; return wxT("");};
        int GetTDIndex(const wxString& value);
        int GetTDCount(){ return tdtexts.size();};

        void DumpTable();
		wxString GetTitle(){return Title;};
    private:
        wxArrayString tdtexts;
		wxString Title;
		bool WantBigTd;
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

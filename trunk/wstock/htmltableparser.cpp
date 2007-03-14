#include "htmltableparser.h"

#include <wx/listimpl.cpp>

int HtmlTableParser::GetTDIndex(const wxString& value){
    for (size_t i=0;i<tdtexts.size();i++){
        if (tdtexts[i].StartsWith(value)){
            return i;
        }
    }
    return -1;
}

wxString HtmlTableParser::GetPureText(wxString html){
    int start=html.Find(wxT("<"));
    int end=html.Find(wxT(">"));
    while ((start>=0) && (end > start)){
        html.Remove(start,end-start+1);
        start=html.Find(wxT("<"));
        end=html.Find(wxT(">"));
    }
	html.Replace(wxT("&nbsp;"),wxT(" "));
	html.Trim();
	html.Trim(false);
    return html;
}

bool HtmlTableParser::HandleTag(const wxHtmlTag& tag){
    if (tag.GetName() == wxT("TD")){
		wxString Inner;
		if (WantBigTd){
			Inner=GetPureText(m_Parser->GetSource()->Mid(tag.GetBeginPos(),tag.GetEndPos1()-tag.GetBeginPos()));
		}
		else{
			Inner=m_Parser->GetSource()->Mid(tag.GetBeginPos(),tag.GetEndPos1()-tag.GetBeginPos());
		}
        if (Inner.size()<200){
            tdtexts.Add(GetPureText(Inner));
        }
    }
	else if (tag.GetName() == wxT("TITLE")){
        wxString Inner=m_Parser->GetSource()->Mid(tag.GetBeginPos(),tag.GetEndPos1()-tag.GetBeginPos());
        if (Inner.size()<200){
            Title = GetPureText(Inner);
        }
	}
    ParseInner(tag);
    return true;
}

void HtmlTableParser::HandleText(const wxChar* txt){

}

void HtmlTableParser::DumpTable(){
    for (size_t i=0;i<tdtexts.size();i++){
        wxLogMessage(tdtexts[i]);
    }
}

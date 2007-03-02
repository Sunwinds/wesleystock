#include "htmltableparser.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TableDataType);
WX_DEFINE_LIST(TableListType);

wxString HtmlTableParser::GetCellValue(int tblidx, int r, int c){
    return GetPureText((*(*tables[tblidx])[r])[c]);
}

wxString HtmlTableParser::GetPureText(wxString html){
    HtmlTableParser *p=new HtmlTableParser();
    p->SetRecordText(true);
    MyHtmlParser parser(p);
    parser.Parse(html);
    return Text;
}

int HtmlTableParser::FindTable(int r, int c, wxString subv){
    for (int i=0;i<GetTableNumber();i++){
        if ((GetTableSize(i).x >r) && (GetTableSize(i).y>c)){
            if (GetCellValue(i,r,c).Find(subv) >= 0){
                return i;
            }
        }
    }
    return -1;
}

wxSize HtmlTableParser::GetTableSize(int tblidx){
    return wxSize(tables[tblidx]->size(),(*tables[tblidx])[0]->size());
}

bool HtmlTableParser::HandleTag(const wxHtmlTag& tag){
    if (tag.GetName() == wxT("TABLE")){
        TableDataType *p=new TableDataType();
        p->DeleteContents(true);
        tables.push_back(p);
    }
    else if (tag.GetName() == wxT("TR")){
        tables[tables.size()-1]->push_back(new wxArrayString());
    }
    else if (tag.GetName() == wxT("TD")){
        wxString Inner=m_Parser->GetSource()->Mid(tag.GetBeginPos(),tag.GetEndPos1()-tag.GetBeginPos());
        if (Inner.size()>20) Inner.Empty();
        (*tables[tables.size()-1])[tables[tables.size()-1]->size()-1]->Add(Inner);
    }
    ParseInner(tag);
    return true;
}

void HtmlTableParser::HandleText(const wxChar* txt){
    if (RecText){
        Text += txt;
    }
}
void HtmlTableParser::DumpTable(){
    for (int i=0;i<GetTableNumber();i++){
        printf("TAble %d has %d r %d c\n",i,GetTableSize(i).x,GetTableSize(i).y);
        for (int x=0; x<GetTableSize(i).x;x++){
            for (int y=0; y<GetTableSize(i).y;y++){
                //printf("Table %d, R:%d C:%d: %s\n",i,x,y,(const char*)GetCellValue(i,x,y).mb_str());
            }
        }
    }
}

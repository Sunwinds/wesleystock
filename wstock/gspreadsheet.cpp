#include "gspreadsheet.h"
#include "wstockconfig.h"
#include <wx/tokenzr.h>


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(GSpreadSheetsOpDataList);

BEGIN_EVENT_TABLE(GSpreadSheets, wxEvtHandler)
    EVT_URL_GET_DONE(-1,GSpreadSheets::OnUrlGetDone)
END_EVENT_TABLE()

void GSpreadSheets::OnUrlGetDone(wxUrlGetDoneEvent& event){
    int rtype = (int)event.UserData;
    if (rtype == -1){
        wxStringTokenizer tkzlines(event.Result,wxT("\r\n"));
        while (tkzlines.HasMoreTokens()){
                wxString line = tkzlines.GetNextToken();
                wxStringTokenizer tkz(line,wxT("="));
                if (tkz.HasMoreTokens()){
                    wxString cmd=tkz.GetNextToken();
                    //wxLogMessage(cmd);
                    if (cmd.Lower() == wxT("auth")){
                        if (tkz.HasMoreTokens()){
                            AuthKey = tkz.GetNextToken();
                        }
                        break;
                    }
                }
        }
        if ((!AuthKey.IsEmpty()) &&(linkFeedHref.IsEmpty())){
            RetriveLinkFeedHref();
        }
    }
    else if (rtype == -2){ //linkFeedHref
        if (event.doc != NULL){
            for (xmlNodePtr node=event.doc->children->children;node;node=node->next){
                if (xmlStrcmp(node->name,(const xmlChar*)"entry")==0){
                    xmlChar *title=NULL;
                    xmlChar *link=NULL;
                    for (xmlNodePtr entrysubnode=node->children;entrysubnode;entrysubnode=entrysubnode->next){
                        if (xmlStrcmp(entrysubnode->name,(const xmlChar*)"title")==0){
                            //strcpy((char*)title,(char*)entrysubnode->content);
                            title = entrysubnode->children->content;
                        }
                        else if ((xmlStrcmp(entrysubnode->name,(const xmlChar*)"link")==0)
                                    && (!link)){
                            //strcpy((char*)link, (char*)xmlGetProp(entrysubnode, (const xmlChar*)"href"));
                            link = xmlGetProp(entrysubnode, (const xmlChar*)"href");
                        }
                        if (title && link){
                            break;
                        }
                    }
                    if (title && link){
                        if (xmlStrcmp(title, (const xmlChar*)(const char*) WStockConfig::GetGoogleMystockTitle().mb_str())==0){
                            linkFeedHref = wxString(wxConvUTF8.cMB2WC((const char*)link),*wxConvCurrent);
                            break;
                        }
                    }
                }
            }
            xmlFreeDoc(event.doc);
        }
        if (!linkFeedHref.IsEmpty()){
            RetriveCellFeedHref();
        };
    }
    else if (rtype == -3){ //cellFeedHref
        if (event.doc != NULL){
            for (xmlNodePtr node=event.doc->children->children;node;node=node->next){
                if (xmlStrcmp(node->name,(const xmlChar*)"entry")==0){
                    xmlChar *link0=NULL;
                    xmlChar *link=NULL;
                    for (xmlNodePtr entrysubnode=node->children;entrysubnode;entrysubnode=entrysubnode->next){
                        if (xmlStrcmp(entrysubnode->name,(const xmlChar*)"link")==0){
                            if (!link0){
                                link0 = xmlGetProp(entrysubnode, (const xmlChar*)"href");
                            }
                            else{
                                link = xmlGetProp(entrysubnode, (const xmlChar*)"href");
                                break;
                            }
                        }
                    }
                    if (link){
                        cellsFeedHref = wxString(wxConvUTF8.cMB2WC((const char*)link),*wxConvCurrent);
                        break;
                    }
                }
            }
            xmlFreeDoc(event.doc);
        }
        UpdateCellsToGoogle();//try to update the buffer to google.
    }
    else if (rtype == 0){ //update cell
        //wxLogMessage(event.Result);
        cellPostBuffer.RemoveAt(0);
        UpdateCellsToGoogle();
    }
    else{
        wxLogMessage(event.Result);
    }
}

void GSpreadSheets::Auth(void){
    if (!WStockConfig::GetGmailUserName().IsEmpty()){
        WStockGetUrl* geturl=new WStockGetUrl(this,
                wxT("https://www.google.com/accounts/ClientLogin"),(void*)-1);
        wxString PostData(wxT("accountType=GOOGLE&Email="));
        PostData << WStockConfig::GetGmailUserName() << wxT("&Passwd=")
                  << WStockConfig::GetGmailPasswd() << wxT("&service=wise&source=wstock");
        geturl->SetPostData(PostData);
        geturl->Create();
        geturl->Run();
    }
}

GSpreadSheets::GSpreadSheets(){
    Auth();
}

void GSpreadSheets::UpdateCellsToGoogle(void){
    if (cellPostBuffer.size()<=0) {
        wxLogStatus(_("No buffer request,exit!"));
        return ;
    }
    if (cellsFeedHref.IsEmpty()){
        wxLogStatus(_("cellsFeedHref is empty,abort update cells!"));
    }
    else{
        wxLogStatus(_("Try to Update mystock data to Google SpreadSheets (%d cell(s) left)..."),cellPostBuffer.size());
        WStockGetUrl* geturl=new WStockGetUrl(this,cellsFeedHref,(void*)0);
        wxString GoogleAuthHead(wxT("Authorization: GoogleLogin auth=\""));
        GoogleAuthHead << AuthKey<< wxT("\"");
        geturl->AppendCustomHead(GoogleAuthHead);
        geturl->AppendCustomHead(wxT("Content-type: application/atom+xml"));
        wxString postdata(wxT("<?xml version='1.0' ?>"));
        postdata << cellPostBuffer[0];
        geturl->SetPostData(postdata);
        geturl->Create();
        geturl->Run();
    }
}

void GSpreadSheets::PutToGoogle(MyStockDataHash* data){
    MyStockDataHash::iterator i = data->begin();
    int RecordIdx=1;
    wxString entry(wxT("<entry xmlns='http://www.w3.org/2005/Atom' xmlns:gs='http://schemas.google.com/spreadsheets/2006'>"));
    while (i != data->end())    {
        wxString stockid = i->first;
        MyStockStru* pmystock = i->second;
        BuyInfoList::Node* node = pmystock->buyinfos.GetFirst();
        while (node)
        {
            BuyInfo* pbuyinfo = node->GetData();
            {
                wxString postdata(entry);
                postdata <<  wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='1' inputValue='")
                          <<stockid <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='2' inputValue='")
                          <<(wxInt32)pbuyinfo->data.GetTicks() <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='3' inputValue='")
                      <<pbuyinfo->BuyAmount <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='4' inputValue='")
                      <<pbuyinfo->BuyPrice <<wxT("'/> </entry>");;
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='5' inputValue='")
                      <<pbuyinfo->Op <<wxT("'/> </entry>");;
                cellPostBuffer.Add(postdata);
            }
            node = node->GetNext();
            RecordIdx++;
        }
        i++;
    }

    //Add One Line Of Empty for seprate
            {
                wxString postdata(entry);
                postdata <<  wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='1' inputValue='")
                          <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='2' inputValue='")
                         <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='3' inputValue='")
                         <<wxT("'/> </entry>");
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='4' inputValue='")
                         <<wxT("'/> </entry>");;
                cellPostBuffer.Add(postdata);
            }
            {
                wxString postdata(entry);
                postdata<< wxT("<gs:cell row='") << RecordIdx+1 << wxT("' col='5' inputValue='")
                         <<wxT("'/> </entry>");;
                cellPostBuffer.Add(postdata);
            }
    UpdateCellsToGoogle();
}

void GSpreadSheets::RetriveLinkFeedHref(){
    if (AuthKey.IsEmpty()){
        Auth();
    }
    else{
        if (!WStockConfig::GetGoogleMystockTitle().IsEmpty()){
            WStockGetUrl* geturl=new WStockGetUrl(this,
                    wxT("http://spreadsheets.google.com/feeds/spreadsheets/private/full"),(void*)-2);
            wxString GoogleAuthHead(wxT("Authorization: GoogleLogin auth=\""));
            GoogleAuthHead << AuthKey<< wxT("\"");
            geturl->AppendCustomHead(GoogleAuthHead);
            geturl->AppendCustomHead(wxT("Content-type: application/atom+xml"));
            geturl->SetWantXml(true);
            geturl->Create();
            geturl->Run();
        }
    }
}

void GSpreadSheets::RetriveCellFeedHref(){
    if (linkFeedHref.IsEmpty()){
        RetriveLinkFeedHref();
    }
    else{
        WStockGetUrl* geturl=new WStockGetUrl(this,linkFeedHref,(void*)-3);
        wxString GoogleAuthHead(wxT("Authorization: GoogleLogin auth=\""));
        GoogleAuthHead << AuthKey<< wxT("\"");
        geturl->AppendCustomHead(GoogleAuthHead);
        geturl->AppendCustomHead(wxT("Content-type: application/atom+xml"));
        geturl->SetWantXml(true);
        geturl->Create();
        geturl->Run();
    }
}

void GSpreadSheets::GetFromGoogle(MyStockDataHash* data){
    if (AuthKey.IsEmpty()){
        Auth();
    }
    else if (cellsFeedHref.IsEmpty()){
        RetriveCellFeedHref();
    }
}

#include "gspreadsheet.h"
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

        if (!AuthKey.IsEmpty()){
            //wxLogMessage(AuthKey);
            wxLogStatus(wxT("Try to Update Google SpreadSheets..."));
            WStockGetUrl* geturl=new WStockGetUrl(this,
                    wxT("http://spreadsheets.google.com/feeds/cells/109692280057.3404530250775683464/od6/private/full"),(void*)0);
            geturl->SetPostData(wxT("<?xml version=\"1.0\"?> <entry> <gs:cell row=\"5\" col=\"5\" inputValue=\"SomeTestValue\"/> </entry>"));
            //geturl->SetCustomCmd(wxT("PUT"));
            wxString GoogleAuthHead(wxT("Authorization: GoogleLogin auth=\""));
            GoogleAuthHead << AuthKey<< wxT("\"");
            geturl->AppendCustomHead(GoogleAuthHead);
            geturl->AppendCustomHead(wxT("Content-type: application/atom+xml"));
            geturl->Create();
            geturl->Run();
        }
        else{
            wxLogMessage(wxT("Login to Google Fail!"));
        }
    }
    else{
        wxLogMessage(event.Result);
    }
}

GSpreadSheets::GSpreadSheets(const wxString& username,const wxString& passwd){
    WStockGetUrl* geturl=new WStockGetUrl(this,
            wxT("https://www.google.com/accounts/ClientLogin"),(void*)-1);
    geturl->SetPostData(wxT("accountType=GOOGLE&Email=cnwesleywang@gmail.com&Passwd=&service=wise&source=wstock"));
    geturl->Create();
    geturl->Run();
}

void GSpreadSheets::PutOperate(GSpreadSheetsOpData*opdata){
    ;
}

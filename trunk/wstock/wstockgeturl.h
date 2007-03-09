#ifndef WSTOCKGETURL_H_INCLUDED
#define WSTOCKGETURL_H_INCLUDED

/*
    这个类以多线程的方式从某个url中获取数据，在数据获取完毕以后向它的父控件中发送消息。
*/

#include "wx/wx.h"
#include "wx/datetime.h"
#include "wx/thread.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

class wxUrlGetDoneEvent : public wxNotifyEvent
{
    public:
    wxUrlGetDoneEvent(wxEventType commandType = wxEVT_NULL, int id = 0, void*data=NULL): wxNotifyEvent(commandType, id){
            UserData = data;
			requestTime = wxDateTime::Now();
        };
    wxUrlGetDoneEvent(const wxUrlGetDoneEvent& event): wxNotifyEvent(event){
        Result = event.Result;
        RetCode = event.RetCode;
        UserData = event.UserData;
        doc = event.doc;
		requestTime = event.requestTime;
        };
    virtual wxEvent *Clone() const {
        return new wxUrlGetDoneEvent(*this);
    }
    wxString Result;
    int RetCode;
    void *UserData;
	wxDateTime requestTime;
    xmlDocPtr doc;//valid when want xml;
    DECLARE_DYNAMIC_CLASS(wxUrlGetDoneEvent);
};
typedef void (wxEvtHandler::*wxUrlGetDoneEventFunction)(wxUrlGetDoneEvent&);

extern const wxEventType wxEVT_URL_GET_DONE;
#define EVT_URL_GET_DONE(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
    wxEVT_URL_GET_DONE, id, -1, (wxObjectEventFunction) (wxEventFunction) \
    (wxUrlGetDoneEventFunction) & fn, \
    (wxObject *) NULL ),

class WStockGetUrl:public wxThread
{
    public:
        WStockGetUrl(wxEvtHandler*parent, const wxString& url, void* data){
            Parent = parent;
            Url = url;
            WantXml=false;
            UserData = data;
            PostData = wxT("");
			requestTime = wxDateTime::Now();
        };
        void SetPostData(const wxString&data){
            PostData = data;
        };
        virtual void *Entry();
        void SetCustomCmd(const wxString& cmd){
            CusCmd = cmd;
        };
        void AppendCustomHead(const wxString& h){
            CustomHeads.Add(h);
        };
        void SetWantXml(bool v){WantXml=v;};
    private:
        wxString Url;
        wxString Proxy;
        wxString CusCmd;
        wxArrayString CustomHeads;
        wxString PostData;
        bool WantXml; //We expect xmlDocPtr in the retrive done event.//user need to release this doc
        void * UserData;
        wxEvtHandler *Parent;
		wxDateTime requestTime;

};

#endif // WSTOCKGETURL_H_INCLUDED

#ifndef WSTOCKGETURL_H_INCLUDED
#define WSTOCKGETURL_H_INCLUDED

/*
    这个类以多线程的方式从某个url中获取数据，在数据获取完毕以后向它的父控件中发送消息。
*/

#include "wx/wx.h"
#include "wx/thread.h"

class wxUrlGetDoneEvent : public wxNotifyEvent
{
    public:
    wxUrlGetDoneEvent(wxEventType commandType = wxEVT_NULL, int id = 0, void*data=NULL): wxNotifyEvent(commandType, id){
            UserData = data;
        };
    wxUrlGetDoneEvent(const wxUrlGetDoneEvent& event): wxNotifyEvent(event){
        Result = event.Result;
        RetCode = event.RetCode;
        UserData = event.UserData;
        };
    virtual wxEvent *Clone() const {
        return new wxUrlGetDoneEvent(*this);
    }
    wxString Result;
    int RetCode;
    void *UserData;
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
            UserData = data;
        };
        virtual void *Entry();
    private:
        wxString Url;
        wxString Proxy;
        void * UserData;
        wxEvtHandler *Parent;

};

#endif // WSTOCKGETURL_H_INCLUDED

#ifndef WSTOCKGETURL_H_INCLUDED
#define WSTOCKGETURL_H_INCLUDED

/*
    这个类以多线程的方式从某个url中获取数据，在数据获取完毕以后向它的父控件中发送消息。
*/

#include "wx/wx.h"
#include "wx/thread.h"

class WStockGetUrl:public wxThread
{
    public:
        WStockGetUrl(wxWindow*parent, const wxString& url, const wxString& proxy){
            Parent = parent;
            Url = url;
            Proxy = proxy;
        };
        virtual void *Entry();
    private:
        wxString Url;
        wxString Proxy;
        wxWindow *Parent;

};

#endif // WSTOCKGETURL_H_INCLUDED

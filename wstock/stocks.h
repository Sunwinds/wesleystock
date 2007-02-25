#ifndef STOCKS_H_INCLUDED
#define STOCKS_H_INCLUDED

#include "wx/wx.h"
#include <wx/arrstr.h>
#include <wx/event.h>

WX_DECLARE_STRING_HASH_MAP(wxString, StrStrHash);

/*

Stock类不能被直接哪来使用，而必须使用这个类的派生类，一个派生类决定一种数据获取的方式，比如
YahooStock这个类继承自Stock类，并且通过Yahoo股票接口实现股票数据的获取。

数据获取可能使用同步或者异步的方式，但是异步的方式可能更为实用一些。Stock类继承自wxEvtHandler以便
提供这种可能性。对于stocks类的一个实现，可以直接在相应的获取函数中直接获取数据，也可以在函数中启动
一个自定义的线程，在线程中获取数据，为安全起见，如果采用后一种方式，则需要在线程结束时以事件的方式
通知Stocks本身，然后在事件处理函数中更新数据以便保证线程安全。

无论那种方法，调用数据获取函数的类都应该期待异步数据获取方式，在调用以后，等待Stocks类发送数据获取
完成事件。这个事件将发送给其Parent处理

*/

class Stock :public wxEvtHandler
{
    public:
        Stock(){};
        Stock(wxEvtHandler* P, const wxString& si, const wxString &name){
            Parent = P;
            StockId = si;
            StockName = name;
        };
        wxString GetId(){ return StockId; };
        wxString GetName(){ return StockName; };
        wxString GetStockType(){
            if (StockId[0] == wxT('6')) return wxT("SS");
            if (StockId[0] == wxT('0')) return wxT("SZ");
            return wxT("XX");
        };

        virtual void RetriveRealTimeData(void* UserData)=0; //子类必须实现这个纯虚方法
        virtual void RetriveHistoryDayData()=0;  //子类必须实现这个纯虚方法
        virtual int GetProptiesNum()=0;
        virtual wxString GetPropertyName(int idx)=0;
        wxString GetPropertyValue(wxString name){
            return RealTimeProps[name];
        };

    private:
        wxString StockName, // The Company name
                 StockId,     // for example: 600000
                 StockType; //  SS or SZ (沪市或者深市)
    protected:
        wxEvtHandler* Parent;
        StrStrHash RealTimeProps;

};

typedef enum {
    REALTIME_RETRIVE,
    HISTORY_RETRIVE
} StockRetriveType;

class wxStockDataGetDoneEvent : public wxNotifyEvent
{
    public:
    wxStockDataGetDoneEvent(wxEventType commandType = wxEVT_NULL,
       StockRetriveType t=REALTIME_RETRIVE, void*data=NULL, Stock*s=NULL):
        wxNotifyEvent(commandType, -1){
            rtype = t;
            UserData = data;
            stock = s;
        };
    wxStockDataGetDoneEvent(const wxStockDataGetDoneEvent& event): wxNotifyEvent(event){
        UserData = event.UserData;
        rtype = event.rtype;
        stock = event.stock;
        };
    virtual wxEvent *Clone() const {
        return new wxStockDataGetDoneEvent(*this);
    }
    Stock* stock;
    StockRetriveType rtype;
    void *UserData;
    DECLARE_DYNAMIC_CLASS(wxStockDataGetDoneEvent);
};
typedef void (wxEvtHandler::*wxStockDataGetDoneEventFunction)(wxStockDataGetDoneEvent&);

extern const wxEventType wxEVT_STOCK_DATA_GET_DONE;
#define EVT_STOCK_DATA_GET_DONE(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
    wxEVT_STOCK_DATA_GET_DONE, id, -1, (wxObjectEventFunction) (wxEventFunction) \
    (wxStockDataGetDoneEventFunction) & fn, \
    (wxObject *) NULL ),



WX_DECLARE_LIST(Stock, StockList);

class Stocks:public wxObject
{
    public:
        Stocks(wxEvtHandler* P){Parent=P;};
        bool Init();
        int GetStockNum(){ return stocks.size();};
        wxString GetStockId(int idx){
            return stocks[idx]->GetId();
        }
        wxString GetStockName(int idx){
            return stocks[idx]->GetName();
        }
        Stock *GetStock(int idx){
            return stocks[idx];
        }
    private:
        wxEvtHandler* Parent;
        StockList stocks;
};

#endif // STOCKS_H_INCLUDED

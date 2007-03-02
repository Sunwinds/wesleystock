#ifndef STOCKS_H_INCLUDED
#define STOCKS_H_INCLUDED

#include "wx/wx.h"
#include "wx/datetime.h"
#include <wx/arrstr.h>
#include <wx/event.h>

WX_DECLARE_STRING_HASH_MAP(wxString, StrStrHash);

typedef struct{
    wxDateTime data;
    double     open;
    double     High;
    double     Low;
    double     Close;
    int        volume;
    double     adjClose;
}StockHistoryDataPiece;
WX_DEFINE_ARRAY (StockHistoryDataPiece *, StockHistoryDataArray);
WX_DEFINE_ARRAY (StockHistoryDataArray *, StockHistoryDataArrayArray);

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
class wxPlotWindow;
class Stock :public wxObject
{
    public:
        Stock(){HistoryDataReady=false;};
        Stock(const wxString& si, const wxString &name){
            StockId = si;
            StockName = name;
			HistoryDataReady=false;
        };
        ~Stock();
        wxString GetId(){ return StockId; };
		void InitHistoryDatas();
        wxString GetName(){ return StockName; };
        wxString GetStockType(){
            if (StockId[0] == wxT('6')) return wxT("SS");
            if (StockId[0] == wxT('0')) return wxT("SZ");
            return wxT("XX");
        };
        wxString GetPropertyValue(const wxString& name){
            return RealTimeProps[name];
        };
        bool IsHistoryDataReady(){
            return HistoryDataReady;
        }
        void AppendHistoryData(int idx, StockHistoryDataPiece*p){ HistoryDatas[idx]->push_back(p);};
        //void AppendWeekData(StockHistoryDataPiece*p){ WeekHistoryData.push_back(p);};
        //void AppendMonthData(StockHistoryDataPiece*p){ MonthHistoryData.push_back(p);};
        void SetPropertyValue(const wxString& name,const wxString& value){
            RealTimeProps[name] = value;
        };

        bool LoadHistoryDataFromFile();
        bool SaveHistoryDataToFile();

    private:
        wxString StockName, // The Company name
                 StockId,     // for example: 600000
                 StockType; //  SS or SZ (沪市或者深市)
    protected:
        friend class wxPlotWindow;
		bool HistoryDataReady;
        StrStrHash RealTimeProps;
		StockHistoryDataArrayArray HistoryDatas;
        //StockHistoryDataArray DayHistoryData;
        //StockHistoryDataArray WeekHistoryData;
        //StockHistoryDataArray MonthHistoryData;
};

typedef enum {
    REALTIME_RETRIVE,
    HISTORY_RETRIVE
} StockRetriveType;

class wxStockDataGetDoneEvent : public wxNotifyEvent
{
    public:
    wxStockDataGetDoneEvent(wxEventType commandType = wxEVT_NULL,
       StockRetriveType t=REALTIME_RETRIVE, void*data=NULL):
        wxNotifyEvent(commandType, -1){
            rtype = t;
            UserData = data;
            HistoryStock = NULL;
        };
    wxStockDataGetDoneEvent(const wxStockDataGetDoneEvent& event): wxNotifyEvent(event){
        UserData = event.UserData;
        rtype = event.rtype;
        HistoryStock  = event.HistoryStock;
        };
    virtual wxEvent *Clone() const {
        return new wxStockDataGetDoneEvent(*this);
    };
    void SetHistoryStock(Stock*s){HistoryStock=s;};
    StockRetriveType rtype;
    void *UserData;
    Stock* HistoryStock;
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
        Stocks(){};
        ~Stocks();
        void SetParent(wxEvtHandler* P){Parent=P;};
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
        Stock *GetStockById(const wxString& id);
        StockList* GetList(){ return &stocks;};
    private:
        wxEvtHandler* Parent;
        StockList stocks;
};

class StocksDataFetch:public wxEvtHandler
{
    public:
        StocksDataFetch(){};
        void SetParent(wxEvtHandler* P){Parent=P;};
        virtual void RetriveRealTimeData(StockList* stocks, void* UserData)=0;
        virtual void RetriveHistoryDayData(Stock* s, void* UserData)=0;
        virtual int GetProptiesNum()=0;
		virtual int GetHistoryDataGroupNum()=0;
        virtual wxString GetPropertyName(int idx)=0;
    protected:
        StockList* stocks;
        wxEvtHandler* Parent;
};

#endif // STOCKS_H_INCLUDED

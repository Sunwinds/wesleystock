#ifndef WSTOCKDATAPROVIDER_H_INCLUDED
#define WSTOCKDATAPROVIDER_H_INCLUDED

class WStockDataProvider:public wxObject
{
    virtual void RetriveRealTimeInfo()=0;
};

#endif // WSTOCKDATAPROVIDER_H_INCLUDED

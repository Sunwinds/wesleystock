#ifndef WSTOCKDATAPROVIDER_H_INCLUDED
#define WSTOCKDATAPROVIDER_H_INCLUDED

/*
假如你想要实现一种新的数据获取方法，需要实现一个这个类的派生类并且实现其所有的虚函数。
对于从网络获取数据来说，你可能希望使用异步的数据获取机制. 因此我们统一使用的接口为:

数据分为实时数据和历史数据两个部分。实时数据为一个字典数据结构，其中的属性名称为约定的名称，
当然，每一个获取数据的机制都可以对其进行扩展。 任何一个数据获取接口必须可以获取到属性为必选的那些值。

历史数据为一个字典的字典结构。第一个字典以日期为键，第二个字典则和实时数据的性质类似。

如需要获取某一个股票的实时数据，需要调用某个数据获取类的实时数据获取接口。并且将那个股票的实例作为参数
传递，

*/
class WStockDataProvider:public wxObject
{
    virtual void RetriveRealTimeData()=0;
    virtual void RetriveHistoryData()=0;
};

#endif // WSTOCKDATAPROVIDER_H_INCLUDED

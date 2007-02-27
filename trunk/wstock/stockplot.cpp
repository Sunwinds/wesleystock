#include "stockplot.h"


wxPlotWindow::wxPlotWindow(wxWindow*parent, int id):wxScrolledWindow(parent, wxID_ANY, wxPoint(0,0),
                           wxSize(200,200),
                           wxSUNKEN_BORDER |
                           wxNO_FULL_REPAINT_ON_RESIZE |
                           wxVSCROLL | wxHSCROLL)
{
    SetBackgroundColour(wxColour(_T("WHITE")));
    stock = NULL;
};

WX_DECLARE_LIST(wxPoint, wxPointList);
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxPointList);

void wxPlotWindow::OnDraw(wxDC& dc)
{
    dc.SetFont(*wxSWISS_FONT);
    //dc.SetPen(*wxGREEN_PEN);
    if (!stock){
        dc.DrawText(_("Please refind a stock data"),50,100);
    }
    else if (!stock->IsHistoryDataReady()){
        dc.DrawText(_("stock history data not ready"),50,100);
    }
    else{
        DrawPricePlot(dc, stock->DayHistoryData);
        DrawPricePlot(dc, stock->WeekHistoryData,false,*wxBLUE);
        DrawPricePlot(dc, stock->MonthHistoryData,false,*wxRED);
    }
}

void wxPlotWindow::SetStock(Stock* s){
    stock = s;
    Refresh();
}

void wxPlotWindow::DrawPricePlot(wxDC&dc,StockHistoryDataArray& value,bool DrawXY,wxColour c){
        int XStart=50;
        int YStart=10;
        int TotalX=370;
        int TotalY=200;
        int YFree=10;

        dc.SetPen(c);

        //让我们来画K线图
        //首先画一个X轴和一个Y轴
        if (DrawXY){
            dc.DrawLine(XStart,YStart + TotalY + YFree,XStart+TotalX,YStart + TotalY + YFree);
            dc.DrawLine(XStart,YStart,XStart,YStart + TotalY + YFree);
        }

        //我们需要确定Y轴的单位,为此，我们需要确定过去一年里股价的最高值和最低值。
        //而X轴，我们则总以时间为单位,每个像素代表一天
        double Highest=0;
        double Lowest=10000; //我们来假设没有哪个股票的股价超过1万
        for (size_t i=0;i<value.size();i++){
            if (value[i]->High > Highest) Highest = value[i]->High;
            if (value[i]->Low < Lowest) Lowest = value[i]->Low;
        }

        //现在我们已经知道了一年中股价的最高和最低值，让我们假定最低值出现在Y轴
        //像素为10的位置，而最高值出现在Y轴像素为210的位置。
        //那么如果给出一个股价x1,它对应的高度应该等于 10+(x1-Lowest) * 200 / (Highest - Lowest)
        //如果我们以X轴像素0代表1年以前的第一天，那么对于某一天x2,它在X轴的位置为
        //它距离1年以前第一天的天数 + 10。
        //由此我们可以得出来（x2,x1)点的序列，并以此划出曲线。
        wxPointList Points;
        wxDateTime now = wxDateTime::Now();
        wxDateTime AYearBefore = now - wxDateSpan(1);
        //Remove the Hour Minute Second part.
        AYearBefore = wxDateTime(AYearBefore.GetDay(),AYearBefore.GetMonth(),AYearBefore.GetYear());

        if (DrawXY){
            //现在我们来给X轴标刻度 我们知道XStart的点是年前的那一天。我们将每两个月显示一个刻度
            wxDateTime aday =AYearBefore + wxDateSpan(0,2);
            while (aday + wxDateSpan(0,1) < (wxDateTime::Now())){
                wxCoord tw, th;
                dc.GetTextExtent(aday.Format(wxT("%m-%d")), &tw, &th);
                dc.DrawText(aday.Format(wxT("%m-%d")),
                        XStart + (aday - AYearBefore).GetDays() - tw / 2,
                        YStart + TotalY + YFree + 5);
                aday += wxDateSpan(0,2);
            }
            //每个月显示一个小的竖格
            aday =AYearBefore;
            while (aday + wxDateSpan(0,1) < (wxDateTime::Now())){
                dc.DrawLine(XStart + (aday - AYearBefore).GetDays(),
                        YStart + TotalY + YFree - 5,
                        XStart + (aday - AYearBefore).GetDays(),
                        YStart + TotalY + YFree);
                aday += wxDateSpan(0,1);
            }
            //Y轴的刻度为价格，假设YStart为最高价格 YStart+TotalY为最低价格。
            //将TotayY分成5份，每一份代表的价格为 Lowest + (Highest -  Lowest) / 5
            for (int idx=0;idx<=5;idx++){
                dc.DrawText(wxString::Format(wxT("%.2f"),Lowest + (Highest -  Lowest) * idx / 5),
                            10, YStart + TotalY - idx * (TotalY / 5) - 5);
                dc.DrawLine(XStart,YStart + TotalY - idx * (TotalY / 5),
                             XStart + 5,YStart + TotalY - idx * (TotalY / 5));
            }
        }

        for (size_t i=0;i<value.size();i++){
            wxTimeSpan delta = value[i]->data - AYearBefore;
            int x2 = XStart + delta.GetDays();
            int y2 = (int)( TotalY+YStart - (double)(value[i]->adjClose - Lowest)
                                * TotalY /(Highest - Lowest));
            Points.push_back(new wxPoint(x2,y2));
        }
        dc.DrawSpline((wxList*)&Points);
        Points.DeleteContents(true);
        Points.Clear();
}

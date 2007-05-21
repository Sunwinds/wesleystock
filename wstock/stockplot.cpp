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

void wxPlotWindow::OnDraw(wxDC& dc)
{
	wxColour cs[]={
		*wxBLACK,
		*wxBLUE,
		*wxRED
	};
    dc.SetFont(*wxSWISS_FONT);
    //dc.SetPen(*wxGREEN_PEN);
    if (!stock){
        dc.DrawText(_("Please refind a stock data"),50,100);
    }
    else if (!stock->IsHistoryDataReady()){
        dc.DrawText(_("stock history data not ready"),50,100);
    }
    else{
		for (size_t i=0;i<stock->HistoryDatas.size();i++){
			DrawPricePlot(dc, *stock->HistoryDatas[i],i==0,cs[i % WXSIZEOF(cs)]);
		}
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

        if (DrawXY){
            dc.DrawLine(XStart,YStart + TotalY + YFree,XStart+TotalX,YStart + TotalY + YFree);
            dc.DrawLine(XStart,YStart,XStart,YStart + TotalY + YFree);
        }

		double Highest=0;
        double Lowest=10000;
        for (size_t j=0;j<value.size();j++){
            if (value[j]->High > Highest) Highest = value[j]->High;
            if (value[j]->Low < Lowest) Lowest = value[j]->Low;
        }

        wxPointList Points;
        wxDateTime now = wxDateTime::Now();
        wxDateTime AYearBefore = now - wxDateSpan(1);
        //Remove the Hour Minute Second part.
        AYearBefore = wxDateTime(AYearBefore.GetDay(),AYearBefore.GetMonth(),AYearBefore.GetYear());

        if (DrawXY){
            wxDateTime aday =AYearBefore + wxDateSpan(0,2);
            while (aday + wxDateSpan(0,1) < (wxDateTime::Now())){
                wxCoord tw, th;
                dc.GetTextExtent(aday.Format(wxT("%m-%d")), &tw, &th);
                dc.DrawText(aday.Format(wxT("%m-%d")),
                        XStart + (aday - AYearBefore).GetDays() - tw / 2,
                        YStart + TotalY + YFree + 5);
                aday += wxDateSpan(0,2);
            }
            aday =AYearBefore;
            while (aday + wxDateSpan(0,1) < (wxDateTime::Now())){
                dc.DrawLine(XStart + (aday - AYearBefore).GetDays(),
                        YStart + TotalY + YFree - 5,
                        XStart + (aday - AYearBefore).GetDays(),
                        YStart + TotalY + YFree);
                aday += wxDateSpan(0,1);
            }

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

#ifndef APP_H
#define APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/config.h"
#include "stocks.h"

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		StocksDataFetch*GetCurFetchObj();
    private :
		StocksDataFetch* CurFetchObj;
        Stocks* stocks;
        wxLocale m_locale;
		wxWindow* frame;
};

DECLARE_APP(MyApp);

#endif // APP_H

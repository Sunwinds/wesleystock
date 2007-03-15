#include "app.h"
#include "wstockconfig.h"
#include "wstockcustomdialog.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

wxConfig config(APP_CFG, VENDOR_CFG);

#ifdef __WXDEBUG__
FILE *fp=fopen("log.dat","w");
#endif
bool MyApp::OnInit()
{
#ifdef __WXDEBUG__
	wxLogStderr *globallog = new wxLogStderr(fp);
	wxLog::SetActiveTarget(globallog);
#endif

    SetAppName(APP_CFG);
    SetVendorName(VENDOR_CFG);
    CurFetchObj = NULL;

    m_locale.Init(/*wxLANGUAGE_CHINESE_SIMPLIFIED*/);
    wxLocale::AddCatalogLookupPathPrefix(wxT("."));
    wxLocale::AddCatalogLookupPathPrefix(wxT(".."));
    m_locale.AddCatalog (wxT("wstock"));

	frame = new MyFrame(0L, _("wstock stock manager software"));
    ((MyFrame*)frame)->DoInitData();
    ((MyFrame*)frame)->UpdateMainGrid(0);

	frame->Show();
	((MyFrame*)frame)->Maximize();
	return true;
}

StocksDataFetch*MyApp::GetCurFetchObj(){
    if (CurFetchObj) return CurFetchObj;
	wstockcustomdialog dialog(frame,-1,wxT(""));
    wxString DataProviderClass(dialog.GetDataProvider());
    StocksDataFetch* stock = wxDynamicCast(wxCreateDynamicObject(DataProviderClass), StocksDataFetch);
	CurFetchObj = stock;
    if (stock) stock->SetParent(frame);
    return stock;
}

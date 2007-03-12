#include "app.h"
#include "wstockconfig.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

wxConfig config(APP_CFG, VENDOR_CFG);

bool MyApp::OnInit()
{
    SetAppName(APP_CFG);
    SetVendorName(VENDOR_CFG);
    CurFetchObj = NULL;

    m_locale.Init(/*wxLANGUAGE_CHINESE_SIMPLIFIED*/);
    wxLocale::AddCatalogLookupPathPrefix(wxT("."));
    wxLocale::AddCatalogLookupPathPrefix(wxT(".."));
    m_locale.AddCatalog (wxT("wstock"));

	MyFrame* frame = new MyFrame(0L, _("wxWidgets Application Template"));
    frame->DoInitData();
    frame->UpdateMainGrid(0);

	frame->Show();
	frame->Maximize();
	return true;
}

StocksDataFetch*MyApp::GetCurFetchObj(){
    if (CurFetchObj) return CurFetchObj;
    wxString DataProviderClass(wxT("SinaStock"));
    StocksDataFetch* stock = wxDynamicCast(wxCreateDynamicObject(DataProviderClass), StocksDataFetch);
    if (stock) stock->SetParent(this);
    return stock;
}

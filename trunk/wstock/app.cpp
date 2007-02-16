#include "app.h"
#include "wstockconfig.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    SetAppName(APP_CFG);
    SetVendorName(VENDOR_CFG);

	MyFrame* frame = new MyFrame(0L, _("wxWidgets Application Template"));

    stocks = new Stocks();
    stocks->Init();
    if (stocks->GetStockNum()==0){
        wxLogMessage(_("There is no stock Id in %s,you may need add some into it!"),
            WStockConfig::GetKeyPath().c_str());
    }

	frame->Show();
	return true;
}

#include "app.h"
#include "wstockconfig.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

wxConfig config(APP_CFG, VENDOR_CFG);

bool MyApp::OnInit()
{
    SetAppName(APP_CFG);
    SetVendorName(VENDOR_CFG);

	MyFrame* frame = new MyFrame(0L, _("wxWidgets Application Template"));

    stocks = new Stocks(frame);
    stocks->Init();
    if (stocks->GetStockNum()==0){
        wxLogMessage(_("There is no stock Id in %s,you may need add some into it!"),
            WStockConfig::GetKeyPath().c_str());
    }
    frame->SetStockSource(stocks);

	frame->Show();
	frame->Maximize();
	return true;
}

#include "app.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	MyFrame* frame = new MyFrame(0L, _("wxWidgets Application Template"));

    stocks = new Stocks();
    stocks->Init();

	frame->Show();
	return true;
}

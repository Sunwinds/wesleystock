#ifndef MAIN_H
#define MAIN_H

#include "wx/grid.h"
#include "app.h"

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxFrame *frame, const wxString& title);
		~MyFrame();
	private:

        wxGrid *mainGrid;

		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		DECLARE_EVENT_TABLE();
};

#endif // MAIN_H

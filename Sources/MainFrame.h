#pragma once

#include "TaskBarIcon.h"
#include "wx/wxprec.h"

class MyDialog : public wxFrame
{
public:
	MyDialog(const wxString& title);
	virtual ~MyDialog();

protected:
	void OnAbout(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);
	void OnIpsus(wxCommandEvent& event);
	void OnMenuAbout(wxCommandEvent& event);
	void OnMenuQuit(wxCommandEvent& event);

	MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
	MyTaskBarIcon   *m_dockIcon;
#endif

	wxDECLARE_EVENT_TABLE();
};
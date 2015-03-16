class MyTaskBarIcon : public wxTaskBarIcon
{
public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
	MyTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
		: wxTaskBarIcon(iconType)
#else
	MyTaskBarIcon()
#endif
	{}

	void OnLeftButtonDClick(wxTaskBarIconEvent&);
	void OnMenuRestore(wxCommandEvent&);
	void OnMenuExit(wxCommandEvent&);
	void OnMenuStartService(wxCommandEvent&);
	void OnMenuStopService(wxCommandEvent&);
	virtual wxMenu *CreatePopupMenu();

	wxDECLARE_EVENT_TABLE();
};


// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};


class MyFrame : public wxFrame, public wxThreadHelper//*****************************************************
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

	//******************************************************************************
	void DoStartALongTask();//creates the background thread that reads the data file
	void OnClose(wxCloseEvent& evt);
	//******************************************************************************


protected:
    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
	void OnMenuQuit(wxCommandEvent& event);

	
	//*********************************************************************
	virtual wxThread::ExitCode Entry(); //function that executes the code for the background thread
	//*********************************************************************
	
	wxTextCtrl *myTextBox;

    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon   *m_dockIcon;
#endif

    wxDECLARE_EVENT_TABLE();
};

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/taskbar.h"
#include "main.h"
#include "services-functions.h"
#include "services-globals.h"
#include "wx\thread.h"
#include "guslib\util\config\configuration.h"
#include "guslib\util\filehelper.h"
#include "guslib\system\uacelevation.h"
#include "icon-selector.h"


// --------------------------------------------------------------------------------
// global variable *gs_dialog - used to point to the main window off the aplication
// --------------------------------------------------------------------------------
static MyFrame *gs_dialog = NULL;

//---------------------------------------------------
// global variable that holds the resources file path
//---------------------------------------------------
static std::string resources_file_path = "..\\res\\";//if you want to change this take a look at section 2 below

IMPLEMENT_APP(MyApp) //cals the main function of the application

//-------------------------------------------------
//The main function of the application
//
//Has 5 Sections:
//1. Admin rights elevation
//2. Checks the resource file path and changes it 
//	 if needed.
//3. Reads the .ini file for the service name
//4. Cheks if system tray is supported
//5. Creates  the main window of the aplication
//-------------------------------------------------
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

		

	//-------------------------------
	//Section 1.
	//Administrator rigths elevation
	//-------------------------------
	BOOL fIsRunAsAdmin;
	try
	{
		fIsRunAsAdmin = guslib::UAC::isRunningAsAdmin();
	}
	catch (DWORD dwError)
	{
		//report error
		[](LPCWSTR pszFunction, DWORD dwError)
		{
			wchar_t szMessage[200];
			if (SUCCEEDED(StringCchPrintf(szMessage, ARRAYSIZE(szMessage),
				L"%s failed w/err 0x%08lx", pszFunction, dwError)))
			{
				MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
			}
		}(L"IsRunAsAdmin", dwError);
	}

	// Elevate the process if it is not run as administrator.
	if (!fIsRunAsAdmin && !guslib::UAC::isElevated())
	{
		return !guslib::UAC::relaunchForManualElevation(true);
	}
	
	//-------------------------------------------------------
	//Section 2.
	//Changes the the resource_file_path if the program
	//is launched within Visual Studio and not the .exe file.
	//If the changed path is not valid two, it exits.
	//-------------------------------------------------------
	if (!_access(resources_file_path.c_str(), 0) == 0)
	{
		//change path for starting the program with Visual Studio
		resources_file_path = "..\\" + resources_file_path;
	}

	if (!_access(resources_file_path.c_str(), 0) == 0)
	{
		wxMessageBox("An invalid path was used for accessing the application resources. The program will now terminate.");
		return false;
	}

	//-----------------------------------------------
	//Section 3.
	//Get the service name from the settings.ini file
	//-----------------------------------------------

	guslib::config::Configuration config_;
	std::string cfgFileName(resources_file_path + "settings.ini");
	
	if (guslib::filehelper::IsFileAccessible(cfgFileName))
	{
		config_.load(cfgFileName);

		std::string traceFile(config_["service"]["name"].getAsStringOrDefaultVal(""));
	
		for (int i = 0; i < traceFile.size(); i++){
			szSvcName[i] = traceFile[i];			
		}
	}
  
	//-----------------------------
	//Section 4.
	//Check for system tray support
	//-----------------------------
	if ( !wxTaskBarIcon::IsAvailable() )			      	
    {													
        wxMessageBox									
        (
            "There appears to be no system tray support in your current environment. This app may not behave as expected.",
            "Warning",
            wxOK | wxICON_EXCLAMATION
        );
    }

	//------------------------------------------
	//Section 5.
	//Create the main window of the application
	//------------------------------------------
	gs_dialog = new MyFrame(wxT("Service handler"));

    gs_dialog->Show(false);//hide the main wondow

	gs_dialog->DoStartALongTask();//starts a background thread that reads the log file
    return true;
}


// -----------------------
// MyFrame implementation
// -----------------------	

enum{

	Menu_Quit = wxID_ANY
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_BUTTON(wxID_OK, MyFrame::OnOK)
	EVT_BUTTON(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(Menu_Quit, MyFrame::OnMenuQuit)
	EVT_CLOSE(MyFrame::OnCloseWindow)
	EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()



MyFrame::MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title)
{
#if wxUSE_MENUS
	// create a menu bar
	wxMenu *fileMenu = new wxMenu;

	fileMenu->Append(Menu_Quit, "E&xit\tAlt-X", "Quit this program");

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "&File");


	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);
#endif // wxUSE_MENUS

	
#if wxUSE_TEXTCTRL
	this->myTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(), wxSize(700, 500), wxTE_MULTILINE | wxTE_PROCESS_TAB);
#endif

    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    sizerTop->Add(new wxStaticText
                      (
                        this,
                        wxID_ANY,
                        wxT("Loggs:")
                      ), flags);

	wxSizer * const sizerTextCtr = new wxBoxSizer(wxHORIZONTAL);

	
	this->myTextBox->SetEditable(false);

	sizerTextCtr->Add(myTextBox, flags);

    sizerTop->AddStretchSpacer()->SetMinSize(700, 0);

    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Hide")), flags);
    sizerBtns->Add(new wxButton(this, wxID_EXIT, wxT("E&xit")), flags);
	
	sizerTop->Add(sizerTextCtr, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop); 
    Centre(); 

	

    m_taskBarIcon = new MyTaskBarIcon();
	
	IconSelector iconSelector;
	iconSelector.initializeIconsFromFile(resources_file_path + "settings.ini");

	int status = 3;
	if (CheckServiceStatus(status)){
		
		if (status == 1){
			
			if (!m_taskBarIcon->SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO)))
			{
				wxLogError(wxT("Could not set icon."));
			}
			this->SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO));

		} else if(status == 0){
		
			if (!m_taskBarIcon->SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO)))
			{
				wxLogError(wxT("Could not set icon."));
			}
			this->SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO));
		}
		else {
		
			if (!m_taskBarIcon->SetIcon(wxIcon(resources_file_path + iconSelector.somthingWentWrong(), wxBITMAP_TYPE_ICO)))
			{
				wxLogError(wxT("Could not set icon."));
			}
			this->SetIcon(wxIcon(resources_file_path + iconSelector.somthingWentWrong(), wxBITMAP_TYPE_ICO));
		}
	}
    
	
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    m_dockIcon = new MyTaskBarIcon(wxTBI_DOCK);
    if ( !m_dockIcon->SetIcon(wxICON(sample)) )
    {
        wxLogError(wxT("Could not set icon."));
    }
#endif
}

MyFrame::~MyFrame()
{
    delete m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
}


void MyFrame::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void MyFrame::OnMenuQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}


void MyFrame::DoStartALongTask()//called in main app
{
	// we want to start a long task, but we don't want our GUI to block
	// while it's executed, so we use a thread to do it.
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}
	// go!
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
		return;
	}
}
wxThread::ExitCode MyFrame::Entry() //the code for the thread that you want to be executed
{
	// IMPORTANT:
	// this function gets executed in the secondary thread context!
	// VERY IMPORTANT: do not call any GUI function inside this
	//                 function; rather use wxQueueEvent():
	guslib::config::Configuration config_;
	std::string cfgFileName(resources_file_path + "settings.ini");
	std::string loggFile = resources_file_path;
	if (guslib::filehelper::IsFileAccessible(cfgFileName))
	{
		config_.load(cfgFileName);

		loggFile += config_["loggfile"]["name"].getAsStringOrDefaultVal(""); 
	}
	FILE* fl = fopen(loggFile.c_str(), "r");
	char line[255];
	// here we do our long task, periodically calling TestDestroy():
	do{
		while (!GetThread()->TestDestroy())
		{
			// since this Entry() is implemented in MyFrame context we don't
			// need any pointer to access the m_data, m_processedData, m_dataCS
			// variables... very nice!
		
			if (fl != NULL){
				int i = 0;
				while(fgets(line, 255, fl) != NULL){
				
					this->myTextBox->AppendText(line);
				}
				break;
			}
		}
		Sleep(1900);
	} while (true);
	// TestDestroy() returned true (which means the main thread asked us
	// to terminate as soon as possible) or we ended the long task...
	return (wxThread::ExitCode)0;
}
void MyFrame::OnClose(wxCloseEvent&)
{
	// important: before terminating, we _must_ wait for our joinable
	// thread to end, if it's running; in fact it uses variables of this
	// instance and posts events to *this event handler
	if (GetThread() &&      // DoStartALongTask() may have not been called
		GetThread()->IsRunning())
		GetThread()->Wait();
	Destroy();
}

// ----------------------------
// MyTaskBarIcon implementation
// ----------------------------

enum
{
	PU_RESTORE = 10001,
	PU_EXIT,
	START_SERVICE,
	STOP_SERVICE
};


wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
EVT_MENU(PU_EXIT, MyTaskBarIcon::OnMenuExit)
EVT_TASKBAR_LEFT_DCLICK(MyTaskBarIcon::OnLeftButtonDClick)
EVT_MENU(START_SERVICE, MyTaskBarIcon::OnMenuStartService)
EVT_MENU(STOP_SERVICE, MyTaskBarIcon::OnMenuStopService)
wxEND_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent&)
{
	gs_dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent&)
{
	gs_dialog->Close(true);
}

static bool check = true;


void MyTaskBarIcon::OnMenuStartService(wxCommandEvent&){

	IconSelector iconSelector;
	iconSelector.initializeIconsFromFile(resources_file_path + "settings.ini");
	bool succeded;

	if (!SetIcon(wxIcon(resources_file_path + iconSelector.intermidiate(), wxBITMAP_TYPE_ICO)))
		wxMessageBox(wxT("Could not set new icon."));
	

	DoStartSvc(succeded);
	
	if (succeded){
		
		if (!SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO)))
			wxMessageBox(wxT("Could not set new icon."));
		gs_dialog->SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO));
	}
	else {
	
		if (!SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO)))
			wxMessageBox(wxT("Could not set new icon."));
		gs_dialog->SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO));
	}
	
}

void MyTaskBarIcon::OnMenuStopService(wxCommandEvent&){

	IconSelector iconSelector;
	iconSelector.initializeIconsFromFile(resources_file_path + "settings.ini");

	bool succeded;

	if (!SetIcon(wxIcon(resources_file_path + iconSelector.intermidiate(), wxBITMAP_TYPE_ICO)))
		wxMessageBox(wxT("Could not set new icon."));
	

	DoStopSvc(succeded);

	if (succeded){
	
		if (!SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO)))
			wxMessageBox(wxT("Could not set new icon."));
		gs_dialog->SetIcon(wxIcon(resources_file_path + iconSelector.stopped(), wxBITMAP_TYPE_ICO));
	}
	else {
	
		if (!SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO)))
			wxMessageBox(wxT("Could not set new icon."));
		gs_dialog->SetIcon(wxIcon(resources_file_path + iconSelector.running(), wxBITMAP_TYPE_ICO));
	}
	
}

wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
	wxMenu *menu = new wxMenu;
	menu->Append(START_SERVICE, wxT("&Start"));
	menu->AppendSeparator();
	menu->Append(STOP_SERVICE, wxT("&Stop"));
	menu->AppendSeparator();
	menu->Append(PU_RESTORE, wxT("&Main window"));
	/* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__ 
	if (OSXIsStatusItem())
#endif
	{
		menu->AppendSeparator();
		menu->Append(PU_EXIT, wxT("E&xit"));
	}
	return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	gs_dialog->Show(true);
}
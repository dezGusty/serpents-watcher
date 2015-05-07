//
// Includes
//

//
// Own header
//
#include "sw_frame.h"

//
// 3rd party libraries.
//
#include "guslib/util/filehelper.h"
#include "wx/button.h"
#include "wx/log.h"
#include "wx/menu.h"
#include "wx/sizer.h"
#include "wx/stattext.h"

//
// This project's headers.
//
#include "services_functions.h"

// -----------------------
// SWFrame implementation
// -----------------------  

namespace serpents
{
  enum{

    Menu_Quit = wxID_ANY
  };


  wxBEGIN_EVENT_TABLE(SWFrame, wxFrame)
    EVT_BUTTON(wxID_OK, SWFrame::OnOK)
    EVT_BUTTON(wxID_EXIT, SWFrame::OnExit)
    EVT_MENU(Menu_Quit, SWFrame::OnMenuQuit)
    EVT_CLOSE(SWFrame::OnCloseWindow)
    EVT_CLOSE(SWFrame::OnClose)
    wxEND_EVENT_TABLE()



    SWFrame::SWFrame(guslib::config::Configuration app_config, IconSelector icon_selector, const wxString& title)
    : wxFrame(NULL, wxID_ANY, title),
    app_config_(app_config),
    icon_selector_(icon_selector)
  {
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu();

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



    m_taskBarIcon = new serpents::SWTaskBarIcon(this->app_config_, this->icon_selector_, this);

    try
    {
      std::string service_name(app_config_["service"]["name"].getAsStringOrDefaultVal(""));
      serpents::services::Status service_status = serpents::services::GetServiceStatus(service_name.c_str());
      if (service_status == serpents::services::Status::SvcStatusRunning)
      {
        if (!m_taskBarIcon->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO)))
        {
          wxLogError(wxT("Could not set icon."));
        }

        this->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO));
      }
      else if (service_status == serpents::services::Status::SvcStatusStopped)
      {
        if (!m_taskBarIcon->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO)))
        {
          wxLogError(wxT("Could not set icon."));
        }

        this->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO));
      }
      else
      {
        if (!m_taskBarIcon->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.error_icon_name(), wxBITMAP_TYPE_ICO)))
        {
          wxLogError(wxT("Could not set icon."));
        }

        this->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.error_icon_name(), wxBITMAP_TYPE_ICO));
      }
    }
    catch (std::exception)
    {
    }

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    m_dockIcon = new SWTaskBarIcon(wxTBI_DOCK);
    if (!m_dockIcon->SetIcon(wxICON(sample)))
    {
      wxLogError(wxT("Could not set icon."));
    }
#endif
  }

  SWFrame::~SWFrame()
  {
    delete m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
  }


  void SWFrame::OnOK(wxCommandEvent& WXUNUSED(event))
  {
    Show(false);
  }

  void SWFrame::OnExit(wxCommandEvent& WXUNUSED(event))
  {
    Close(true);
  }

  void SWFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
  {
    Destroy();
  }

  void SWFrame::OnMenuQuit(wxCommandEvent& WXUNUSED(event))
  {
    Close(true);
  }


  void SWFrame::DoStartALongTask()//called in main app
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
  wxThread::ExitCode SWFrame::Entry() //the code for the thread that you want to be executed
  {
    // IMPORTANT:
    // this function gets executed in the secondary thread context!
    // VERY IMPORTANT: do not call any GUI function inside this
    //                 function; rather use wxQueueEvent():
    guslib::config::Configuration config_;
    std::string cfgFileName(this->icon_selector_.resource_root_path() + "settings.ini");
    std::string loggFile = this->icon_selector_.resource_root_path();
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
        // since this Entry() is implemented in SWFrame context we don't
        // need any pointer to access the m_data, m_processedData, m_dataCS
        // variables... very nice!

        if (fl != NULL){
          int i = 0;
          while (fgets(line, 255, fl) != NULL){

            this->myTextBox->AppendText(line);
          }
          break;
        }
      }

      ::wxMilliSleep(1750);// Sleep(1900);
    } while (true);
    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)0;
  }
  void SWFrame::OnClose(wxCloseEvent&)
  {
    // important: before terminating, we _must_ wait for our joinable
    // thread to end, if it's running; in fact it uses variables of this
    // instance and posts events to *this event handler
    if (GetThread() &&      // DoStartALongTask() may have not been called
      GetThread()->IsRunning())
      GetThread()->Wait();
    Destroy();
  }
}

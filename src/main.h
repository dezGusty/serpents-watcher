#ifndef SERPENTS_MAIN_H
#define SERPENTS_MAIN_H

//
// Includes
//

//
// 3rd party libs
//
#include "guslib/util/config/configuration.h"
#include "wx/taskbar.h"

class MyTaskBarIcon 
  : public wxTaskBarIcon
{
private:
  guslib::config::Configuration app_config_;

public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
  MyTaskBarIcon(guslib::config::Configuration app_config, wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
    : wxTaskBarIcon(iconType)
#else
  MyTaskBarIcon(guslib::config::Configuration app_config)
#endif
  {
    this->app_config_ = app_config;
  }

  /**
  *Makes the main window visible when the system tray icon is double clicked
  *
  *@param double ckick event
  *@return void
  */
  void OnLeftButtonDClick(wxTaskBarIconEvent&);

  /**
  *Makes the main window visible when the 'Main window' item is clicked in the system tray icon's popup menu
  *
  *@param click event
  *@return void
  */
  void OnMenuRestore(wxCommandEvent&);

  /**
  *Closes the application when the 'Exit' item is clicked in the system tray icon's popup menu
  *
  *@param click event
  *@return void
  */
  void OnMenuExit(wxCommandEvent&);

  /**
  *Starts the service if posible and sets the appropriate icon for the service state
  *when the 'Start' item is clicked in the system tray icon's popup menu
  *
  *@param click event
  *@return void
  */
  void OnMenuStartService(wxCommandEvent&);

  /**
  *Stops the service if posible and sets the appropriate icon for the service state
  *when the 'Stop' item is clicked in the system tray icon's popup menu
  *
  *@param click event
  *@return void
  */
  void OnMenuStopService(wxCommandEvent&);

  /**
  *Creats the system tray icon's popup menu 
  *
  *@return pointer to the created menu
  */
  virtual wxMenu *CreatePopupMenu();

  /**
  *This macro links the event handlers with the event triggerers
  *by giving each event handler an id that will be used in the construction of each event triggerer.
  *Applied only for taskbaricon elements
  *
  */
  wxDECLARE_EVENT_TABLE();
};


//
// The main application class for Serpents Watcher. Based on wxWidgets.
//
// @authors Petru Barko, Augustin Preda.
//
class SWApp : public wxApp
{
private:
  // Opaque pointer class, containing internals.
  class Impl;
  Impl* impl_;

public:
  //
  //  Constructor.
  //
  SWApp();

  //
  //  Destructor.
  //
  virtual ~SWApp();

  //
  //  Ensure that the application is running with admin rights.
  //  If it is not running with admin rights, relaunch it with admin rights.
  //  @return True if the application is running as an administrator.
  //
  //  @authors Petru Barko, Augustin Preda.
  //
  virtual bool EnsureRunningAsAdmin();

  //
  //  Loads the configuration file.
  //  @remarks Can throw an exception if a failure is encountered.
  //
  //  @author Augustin Preda.
  //  
  virtual void LoadConfigFile();

  //
  // The entry point of the application (echivalent to int main())
  // @return True if the application terminates with success. False otherwise (in case of failure).
  //
  // @author Petru Barko.
  //
  virtual bool OnInit();
};


class MyFrame : public wxFrame, public wxThreadHelper
{
private:
  guslib::config::Configuration app_config_;

public:
  MyFrame(guslib::config::Configuration app_config, const wxString& title);
  virtual ~MyFrame();

  
  /**
  *Starts the background thread that reads the data file
  *
  *@return void
  */
  void DoStartALongTask();

  /**
  *Closes the application when the user click the top right corner x button
  *
  *@param close event
  *@return void
  */
  void OnClose(wxCloseEvent& evt);



protected:

  /**
  *Hides the main window when the 'Hide' button is clicked
  *
  *@param click event
  *@return void
  */
    void OnOK(wxCommandEvent& event);

  /**
  *Closes the application when the 'Exit' button is clicked
  *
  *@param click event
  *@return void
  */
    void OnExit(wxCommandEvent& event);

  /**
  *Closes the application when the 'X' button is clicked
  *
  *@param click event
  *@return void
  */
    void OnCloseWindow(wxCloseEvent& event);

  /**
  *Closes the application when the 'Exit' item is clicked in the main menu
  *
  *@param click event
  *@return void
  */
  void OnMenuQuit(wxCommandEvent& event);

  
  /**
  *Reads the data file in real time and outputs the result into the main window's text field
  *
  *@return void
  */
  virtual wxThread::ExitCode Entry(); 
  
  
  wxTextCtrl *myTextBox;

    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon   *m_dockIcon;
#endif

  /**
  *This macro links the event handlers with the event triggerers
  *by giving each event handler an id that will be used in the construction of each event triggerer.
  *Applied only for main window elements
  *
  */
    wxDECLARE_EVENT_TABLE();
};

#endif // SERPENTS_MAIN_H

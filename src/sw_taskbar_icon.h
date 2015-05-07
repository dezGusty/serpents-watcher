#ifndef SW_TASKBAR_ICON_H
#define SW_TASKBAR_ICON_H

//
// Includes
//

//
// 3rd party libs
//
#include "guslib/util/config/configuration.h"

#include "wx/frame.h"
#include "wx/taskbar.h"

//
// This project's headers
//
#include "icon_selector.h"

namespace serpents
{
  //
  // A class to be used for controlling the application's icon in the taskbar.
  // Uses wxWidgets.
  //
  class SWTaskBarIcon
    : public wxTaskBarIcon
  {
  private:
    guslib::config::Configuration app_config_;
    wxFrame* owner_frame_;
    IconSelector icon_selector_;

    class Impl;
    Impl* impl_;

  public:
    //
    // Constructor.
    //
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    SWTaskBarIcon(guslib::config::Configuration app_config, IconSelector icon_selector, wxFrame* owner_frame, wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);
#else
    SWTaskBarIcon(guslib::config::Configuration app_config, IconSelector icon_selector, wxFrame* owner_frame);
#endif

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

}

#endif
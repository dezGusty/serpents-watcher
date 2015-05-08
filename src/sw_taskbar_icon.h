#ifndef SW_TASKBAR_ICON_H
#define SW_TASKBAR_ICON_H

//   This file is part of the "Serpents Watcher Utility", licensed under
//   the terms of the MIT License (seen below).
//
//   The MIT License
//   Copyright (C) 2015, the Serpents-Watcher team:
//      Augustin Preda (thegusty999@gmail.com)
//      Petru Barko
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.

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

    //
    // Reacts to the double mouse click event on the tray icon.
    // Makes the main window visible when the system tray icon is double clicked
    //
    // @author Petru Barko
    //
    void OnLeftButtonDClick(wxTaskBarIconEvent&);

    //
    // Makes the main window visible when the 'Main window' item is clicked in the system tray icon's popup menu
    //
    // @author Petru Barko
    //
    void OnMenuRestore(wxCommandEvent&);

    //
    // Closes the application when the 'Exit' item is clicked in the system tray icon's popup menu
    //
    // @author Petru Barko
    //
    void OnMenuExit(wxCommandEvent&);

    //
    // Starts the service if posible and sets the appropriate icon for the service state
    // when the 'Start' item is clicked in the system tray icon's popup menu
    //
    // @author Petru Barko
    //
    void OnMenuStartService(wxCommandEvent&);

    //
    // Stops the service if posible and sets the appropriate icon for the service state
    // when the 'Stop' item is clicked in the system tray icon's popup menu
    //
    // @author Petru Barko
    //
    void OnMenuStopService(wxCommandEvent&);

    //
    // Creats the system tray icon's popup menu
    //
    // @return pointer to the created menu
    // @author Petru Barko
    //
    virtual wxMenu *CreatePopupMenu();

    //
    // This macro links the event handlers with the event triggerers
    // by giving each event handler an id that will be used in the construction of each event triggerer.
    // Applied only for taskbaricon elements
    //
    wxDECLARE_EVENT_TABLE();
  };
}

#endif

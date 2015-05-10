#ifndef SW_FRAME_H
#define SW_FRAME_H

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
// 3rd party libs.
//
#include "guslib/util/config/configuration.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/thread.h"

//
// This project's headers
//
#include "icon_selector.h"
#include "sw_taskbar_icon.h"

namespace serpents
{
  class SWFrame : public wxFrame, public wxThreadHelper
  {
  private:
    guslib::config::Configuration app_config_;
    IconSelector icon_selector_;

    wxTextCtrl *myTextBox;
    serpents::SWTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    SWTaskBarIcon   *m_dockIcon;
#endif

  public:
    //
    // Constructor.
    //
    SWFrame(guslib::config::Configuration app_config, IconSelector icon_selector, const wxString& title);

    //
    // Destructor.
    //
    virtual ~SWFrame();

    //
    // Starts the background thread that reads the data file
    //
    // @author Petru Barko
    //
    void DoStartALongTask();

    //
    // Closes the application when the user click the top right corner x button
    // 
    // @author Petru Barko
    //
    void OnClose(wxCloseEvent& evt);

  protected:
    //
    // Hides the main window when the 'Hide' button is clicked
    //
    // @author Petru Barko
    //
    void OnOK(wxCommandEvent& event);

    //
    // Closes the application when the 'Exit' button is clicked
    //
    // @author Petru Barko
    //
    void OnExit(wxCommandEvent& event);

    //
    // Closes the application when the 'X' button is clicked
    // 
    // @author Petru Barko
    //
    void OnCloseWindow(wxCloseEvent& event);

    //
    // Closes the application when the 'Exit' item is clicked in the main menu
    //
    // @author Petru Barko
    //
    void OnMenuQuit(wxCommandEvent& event);


    //
    // Reads the data file in real time and outputs the result into the main window's text field
    //
    // @author Petru Barko
    //
    virtual wxThread::ExitCode Entry();

    //
    // This macro links the event handlers with the event triggerers
    // by giving each event handler an id that will be used in the construction of each event triggerer.
    // Applied only for main window elements
    //
    wxDECLARE_EVENT_TABLE();
  };
}

#endif  // SW_FRAME_H

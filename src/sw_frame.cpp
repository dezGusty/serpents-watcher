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
// Own header
//
#include "sw_frame.h"

//
// C++ system headers
//
#include <string>

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
#include "sw_service_functions.h"

// -----------------------
// SWFrame implementation
// -----------------------

namespace serpents
{
  enum
  {
    Menu_Quit = wxID_ANY
  };

  //
  // Bind the UI elements to events.
  //
  wxBEGIN_EVENT_TABLE(SWFrame, wxFrame)
    EVT_BUTTON(wxID_OK, SWFrame::OnOK)
    EVT_BUTTON(wxID_EXIT, SWFrame::OnExit)
    EVT_MENU(Menu_Quit, SWFrame::OnMenuQuit)
    EVT_CLOSE(SWFrame::OnCloseWindow)
    EVT_CLOSE(SWFrame::OnClose)
    wxEND_EVENT_TABLE()

  //
  // Frame constructor.
  //
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

    sizerTop->Add(
      new wxStaticText(
        this,
        wxID_ANY,
        wxT("Logs:")), 
      flags);

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
        if (!m_taskBarIcon->SetIcon(
          wxIcon(
            this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), 
            wxBITMAP_TYPE_ICO)))
        {
          wxLogError(wxT("Could not set icon."));
        }

        this->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO));
      }
      else if (service_status == serpents::services::Status::SvcStatusStopped)
      {
        if (!m_taskBarIcon->SetIcon(
          wxIcon(
            this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), 
            wxBITMAP_TYPE_ICO)))
        {
          wxLogError(wxT("Could not set icon."));
        }

        this->SetIcon(
          wxIcon(
            this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), 
            wxBITMAP_TYPE_ICO));
      }
      else
      {
        if (!m_taskBarIcon->SetIcon(
          wxIcon(
            this->icon_selector_.resource_root_path() + this->icon_selector_.error_icon_name(), 
            wxBITMAP_TYPE_ICO)))
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

  //
  // Create a background thread, tasked with monitoring a log file.
  //
  // @authors Petru Barko, Augustin Preda.
  //
  void SWFrame::RunBackgroundThread()
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

  //
  // The background thread to run.
  // This handles the tracking of a file on the disk, opening it and reading its contents on a regular basis,
  // and displaying these contents in the application's frame
  //
  // @authors Petru Barko, Augustin Preda.
  //
  wxThread::ExitCode SWFrame::Entry() //the code for the thread that you want to be executed
  {
    // IMPORTANT:
    // this function gets executed in the secondary thread context!
    // VERY IMPORTANT: do not call any GUI function inside this
    //                 function; rather use wxQueueEvent():
    guslib::config::Configuration config_;
    std::string logFile = this->icon_selector_.resource_root_path();
    logFile.append(this->app_config_["logfile"]["name"].getAsStringOrDefaultVal(""));

    FILE* fl = fopen(logFile.c_str(), "r");
    char line[255];
    // here we do our long task, periodically calling TestDestroy():
    do
    {
      while (!GetThread()->TestDestroy())
      {
        // TODO(Augustin Preda, 2015.05.15): shouldn't wxQueueEvent be used instead of AppendText?
        // TODO(Augustin Preda, 2015.05.15): this should be moved to another function.
        // The full file is read each and every time. Some caching would be in order.

        // since this Entry() is implemented in SWFrame context we don't
        // need any pointer to access the m_data, m_processedData, m_dataCS
        // variables... very nice!
        if (fl != NULL)
        {
          int i = 0;
          while (fgets(line, 255, fl) != NULL)
          {
            this->myTextBox->AppendText(line);
          }

          break;
        }
      }

      ::wxMilliSleep(2000);
    } while (true);
    
    // TestDestroy() returned true (which means the main thread asked us
    // to terminate as soon as possible) or we ended the long task...
    return (wxThread::ExitCode)0;
  }

  //
  // React to the frame close event.
  //
  // @authors Petru Barko, Augustin Preda.
  //
  void SWFrame::OnClose(wxCloseEvent&)
  {
    // Before closing, the application MUST wait for the joinable thread to end.
    // Of course, this is valid only of the thread was created in the first place.
    if (GetThread() && GetThread()->IsRunning())
    {
      GetThread()->Wait();
    }

    Destroy();
  }
}

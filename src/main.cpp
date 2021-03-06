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
#include "main.h"

//
// C system headers
//
#include <strsafe.h>

//
// C++ system headers
//
#include <string>
#include <vector>

//
// Other libraries' headers
//
#include "guslib/common/simpleexception.h"
#include "guslib/system/uacelevation.h"
#include "guslib/trace/trace.h"
#include "guslib/util/config/configuration.h"
#include "guslib/util/filehelper.h"
#include "wx/msgdlg.h"

//
// This project's headers
//
#include "icon_selector.h"
#include "sw_frame.h"

namespace serpents
{
  IMPLEMENT_APP(SWApp)  // cals the main function of the application

  //
  // Opaque pointer containing the hidden implementation.
  // Contains the application's configuration object. Containing it here, removes the need to included it in the header.
  //
  // @author Augustin Preda
  //
  class SWApp::Impl
  {
  public:
    guslib::config::Configuration app_config_;
    std::string resources_file_path_;
    serpents::SWFrame* frame_ptr;

    SWApp::Impl()
      : resources_file_path_("..\\..\\..\\res\\"),
      frame_ptr(0)
    {
    }
  };

  SWApp::SWApp()
    : impl_(new SWApp::Impl())
  {
  }

  SWApp::~SWApp()
  {
    delete impl_;
  }

  //
  //  Ensure that the application is running with admin rights.
  //  If it is not running with admin rights, relaunch it with admin rights.
  //
  //  @authors Petru Barko, Augustin Preda.
  //
  bool SWApp::EnsureRunningAsAdmin()
  {
    //-------------------------------
    //  Section 1.
    //  Administrator rigths elevation
    //-------------------------------
    BOOL fIsRunAsAdmin;
    try
    {
      fIsRunAsAdmin = guslib::UAC::isRunningAsAdmin();
    }
    catch (DWORD dwError)
    {
      // Report error
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

    return true;
  }

  //
  //  Loads the configuration file.
  //  @remarks Can throw an exception if a failure is encountered.
  //
  //  @author Augustin Preda.
  //
  void SWApp::LoadConfigFile()
  {
    // Try to load the config file. Allow several locations to be used.
    // Store the name of the config file to use.
    std::string config_file_name("settings.ini");

    // The following locations will be used in the order in which they are given here.
    // As soon as the first one is found, it is loaded. All others are ignored afterwards.
    std::vector <std::string> config_locations;
    config_locations.push_back("./");
    config_locations.push_back("../");
    config_locations.push_back("../../");
    config_locations.push_back("../../../res/");
    config_locations.push_back("../../res/");
    config_locations.push_back("../res/");
    config_locations.push_back("./res/");

    for (auto it = config_locations.begin(); it != config_locations.end(); ++it)
    {
      std::string target_file(*it);
      target_file.append(config_file_name);
      if (guslib::filehelper::IsFileAccessible(target_file))
      {
        // Store the folder with the config.
        this->impl_->resources_file_path_ = *it;
        this->impl_->app_config_.load(target_file);
        std::string service_name(this->impl_->app_config_["service"]["name"].getAsStringOrDefaultVal(""));

        GTRACE(3, "Loading configuration from file: " << target_file);
        GTRACE(3, "Service name to control is: " << service_name)

        break;
      }
    }

    // Ok, what if no config file was found? Throw an exception.
    if (this->impl_->app_config_.getGroups().size() == 0)
    {
      std::string errorMessage("Could not locate settings.ini file. Tried:");
      for (std::vector <std::string>::iterator it = config_locations.begin();
        it != config_locations.end(); ++it)
      {
        errorMessage.append("\n");
        errorMessage.append(*it);
      }

      throw guslib::SimpleException(errorMessage.c_str());
    }
  }

  //-------------------------------------------------
  // The main function of the application
  //
  // Has 5 Sections:
  // 1. Admin rights elevation
  // 2. Checks the resource file path and changes it
  //   if needed.
  // 3. Reads the .ini file for the service name
  // 4. Cheks if system tray is supported
  // 5. Creates  the main window of the aplication
  //-------------------------------------------------
  bool SWApp::OnInit()
  {
    if (!wxApp::OnInit())
    {
      return false;
    }

    if (!this->EnsureRunningAsAdmin())
    {
      return false;
    }

    GSTARTTRACING("sw.log", 5);

    //-------------------------------------------------------
    // Section 2.
    // Changes the the resource_file_path if the program
    // is launched within Visual Studio and not the .exe file.
    // If the changed path is not valid two, it exits.
    //-------------------------------------------------------
    // Get the service name from the settings.ini file
    //-----------------------------------------------
    try
    {
      this->LoadConfigFile();
    }
    catch (const std::exception& ex)
    {
      wxMessageBox(ex.what());
      return false;
    }

    //-----------------------------
    // Section 4.
    // Check for system tray support
    //-----------------------------
    if (!wxTaskBarIcon::IsAvailable())
    {
      wxMessageBox(
        "There appears to be no system tray support in your current environment. This app may not behave as expected.",
        "Warning",
        wxOK | wxICON_EXCLAMATION);
    }

    //------------------------------------------
    // Section 5.
    // Create the main window of the application
    //------------------------------------------

    IconSelector icon_selector(this->impl_->resources_file_path_);
    icon_selector.InitializeIconsFromConfig(this->impl_->app_config_);

    this->impl_->frame_ptr = new serpents::SWFrame(this->impl_->app_config_, icon_selector, wxT("Service handler"));

    this->impl_->frame_ptr->Show(false);  //  hide the main wondow

    if (this->impl_->app_config_["logfile"]["enabled"].getAsBoolOrDefault(false))
    {
      this->impl_->frame_ptr->RunBackgroundThread();  //  starts a background thread that reads the log file
    }

    return true;
  }
}

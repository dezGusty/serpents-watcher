//
// Includes
//
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "main.h"

#include <strsafe.h>

#include "guslib\common\simpleexception.h"
#include "guslib\system\uacelevation.h"
#include "guslib/trace/trace.h"
#include "guslib\util\config\configuration.h"
#include "guslib\util\filehelper.h"
#include "wx\thread.h"

#include "services_functions.h"
#include "icon_selector.h"
#include "sw_frame.h"



// --------------------------------------------------------------------------------
// global variable *gs_dialog - used to point to the main window off the aplication
// --------------------------------------------------------------------------------
static serpents::SWFrame *gs_dialog = NULL;

//---------------------------------------------------
// global variable that holds the resources file path
//---------------------------------------------------
static std::string resources_file_path = "..\\..\\..\\res\\";//if you want to change this take a look at section 2 below

IMPLEMENT_APP(SWApp) //cals the main function of the application

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
  SWApp::Impl()
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
      resources_file_path = *it;
      this->impl_->app_config_.load(target_file);
      std::string service_name(this->impl_->app_config_["service"]["name"].getAsStringOrDefaultVal(""));

      break;
    }
  }

  // Ok, what if no config file was found? Throw an exception.
  if (this->impl_->app_config_.getGroups().size() == 0)
  {
    std::string errorMessage = ("Could not locate settings.ini file. Tried:");
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
//The main function of the application
//
//Has 5 Sections:
//1. Admin rights elevation
//2. Checks the resource file path and changes it 
//   if needed.
//3. Reads the .ini file for the service name
//4. Cheks if system tray is supported
//5. Creates  the main window of the aplication
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
  //Section 2.
  //Changes the the resource_file_path if the program
  //is launched within Visual Studio and not the .exe file.
  //If the changed path is not valid two, it exits.
  //-------------------------------------------------------
  //Get the service name from the settings.ini file
  //-----------------------------------------------
  try
  {
    this->LoadConfigFile();
  }
  catch (std::exception& ex)
  {
    wxMessageBox(ex.what());
    return false;
  }

  //-----------------------------
  //Section 4.
  //Check for system tray support
  //-----------------------------
  if ( !wxTaskBarIcon::IsAvailable() )              
  {                          
      wxMessageBox(
          "There appears to be no system tray support in your current environment. This app may not behave as expected.",
          "Warning",
          wxOK | wxICON_EXCLAMATION);
  }

  //------------------------------------------
  //Section 5.
  //Create the main window of the application
  //------------------------------------------

  IconSelector icon_selector(resources_file_path);
  icon_selector.InitializeIconsFromConfig(this->impl_->app_config_);

  gs_dialog = new serpents::SWFrame(this->impl_->app_config_, icon_selector, wxT("Service handler"));

  gs_dialog->Show(false);//hide the main wondow

  gs_dialog->DoStartALongTask();//starts a background thread that reads the log file
  return true;
}


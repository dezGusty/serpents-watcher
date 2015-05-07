//
// Includes
//

//
// Own header
//
#include "sw_taskbar_icon.h"

//
// 3rd party libs
//
#include "wx/menu.h"
#include "wx/msgdlg.h"

//
// This project's headers
//
#include "services_functions.h"

namespace serpents
{
  // ----------------------------
  // SWTaskBarIcon implementation
  // ----------------------------

  enum
  {
    PU_RESTORE = 10001,
    PU_EXIT,
    START_SERVICE,
    STOP_SERVICE
  };


  wxBEGIN_EVENT_TABLE(SWTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, SWTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT, SWTaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_DCLICK(SWTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU(START_SERVICE, SWTaskBarIcon::OnMenuStartService)
    EVT_MENU(STOP_SERVICE, SWTaskBarIcon::OnMenuStopService)
    wxEND_EVENT_TABLE()

  //
  // Constructor.
  //
#if defined(__WXOSX__) && wxOSX_USE_COCOA
  SWTaskBarIcon::SWTaskBarIcon(guslib::config::Configuration app_config, IconSelector icon_selector, wxFrame* owner_frame, wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
  : wxTaskBarIcon(iconType)
  , app_config_(app_config)
  , icon_selector_(icon_selector)
  , owner_frame_(owner_frame)
#else
  SWTaskBarIcon::SWTaskBarIcon(guslib::config::Configuration app_config, IconSelector icon_selector, wxFrame* owner_frame)
  : app_config_(app_config)
  , icon_selector_(icon_selector)
  , owner_frame_(owner_frame)
#endif
  {
  }

  void SWTaskBarIcon::OnMenuRestore(wxCommandEvent&)
  {
    this->owner_frame_->Show(true);
  }

  void SWTaskBarIcon::OnMenuExit(wxCommandEvent&)
  {
    this->owner_frame_->Close(true);
  }

  static bool check = true;


  void SWTaskBarIcon::OnMenuStartService(wxCommandEvent&)
  {

    bool succeded;

    if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.intermediate_icon_name(), wxBITMAP_TYPE_ICO)))
      wxMessageBox(wxT("Could not set new icon."));


    std::string service_name(app_config_["service"]["name"].getAsStringOrDefaultVal(""));
    succeded = serpents::services::StartServiceWithName(service_name.c_str());

    if (succeded){

      if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO)))
        wxMessageBox(wxT("Could not set new icon."));
      this->owner_frame_->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO));
    }
    else {

      if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO)))
        wxMessageBox(wxT("Could not set new icon."));
      this->owner_frame_->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO));
    }

  }

  void SWTaskBarIcon::OnMenuStopService(wxCommandEvent&)
  {
    bool succeded;

    if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.intermediate_icon_name(), wxBITMAP_TYPE_ICO)))
    {
      wxMessageBox(wxT("Could not set new icon."));
    }

    std::string service_name(app_config_["service"]["name"].getAsStringOrDefaultVal(""));
    succeded = serpents::services::StopServiceWithName(service_name.c_str());

    if (succeded)
    {
      if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO)))
      {
        wxMessageBox(wxT("Could not set new icon."));
      }

      this->owner_frame_->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.stopped_icon_name(), wxBITMAP_TYPE_ICO));
    }
    else 
    {
      if (!SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO)))
      {
        wxMessageBox(wxT("Could not set new icon."));
      }

      this->owner_frame_->SetIcon(wxIcon(this->icon_selector_.resource_root_path() + this->icon_selector_.running_icon_name(), wxBITMAP_TYPE_ICO));
    }

  }

  wxMenu *SWTaskBarIcon::CreatePopupMenu()
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

  void SWTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
  {
    this->owner_frame_->Show(true);
  }
}
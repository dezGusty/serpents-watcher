#ifndef SERPENTS_MAIN_H
#define SERPENTS_MAIN_H

//
// Includes
//

//
// 3rd party libs
//
#include "guslib/util/config/configuration.h"

//
// This project's headers
//
#include "icon_selector.h"
#include "sw_taskbar_icon.h"

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



#endif // SERPENTS_MAIN_H

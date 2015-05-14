#ifndef SERPENTS_MAIN_H
#define SERPENTS_MAIN_H

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
#include "wx/app.h"

//
// This project's headers
//
#include "icon_selector.h"
#include "sw_taskbar_icon.h"

namespace serpents
{
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
}

#endif  // SERPENTS_MAIN_H

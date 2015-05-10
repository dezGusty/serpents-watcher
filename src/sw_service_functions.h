#ifndef SW_SERVICE_FUNCTIONS_H
#define SW_SERVICE_FUNCTIONS_H

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

namespace serpents
{
  namespace services
  {
    typedef enum
    {
      SvcStatusStopped = 0,
      SvcStatusRunning = 1,
      SvcStatusError = 2
    } Status;

    //
    // Start a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered
    // with, not the display name of the service.
    // @return true if the service is running (either already running or 
    // running at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StartServiceWithName(const char* service_name);

    //
    // Stop a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered
    // with, not the display name of the service.
    // @return true if the service is stopped (either already stopped or
    // stopped at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StopServiceWithName(const char* service_name);

    //
    // Get the status of a service, identified by its name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered
    // with, not the display name of the service.
    // @return The found status.
    //
    // @authors Petru Barko, Augustin Preda.
    //
    Status GetServiceStatus(const char* service_name);
  }
}

#endif  // SW_SERVICE_FUNCTIONS_H

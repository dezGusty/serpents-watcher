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
// Own header.
//
#include "services_functions.h"

//
// C system headers
//

#include <windows.h>

//
// C++ system headers
//

#include <sstream>

//
// 3rd party libraries.
//
#include "guslib/common/simpleexception.h"
#include "guslib/trace/trace.h"
#include "guslib/util/stringutil.h"


#pragma comment(lib, "advapi32.lib")

namespace serpents
{
  namespace services
  {
    //
    // A namespace containing functionality that is not exposed.
    //
    namespace hidden
    {
      //
      // Stop the services that depend on a given service.
      //
      // @authors Petru Barko, Augustin Preda.
      //
      bool StopDependentServices(SC_HANDLE service_control_manager, SC_HANDLE service_handle)
      {
        DWORD i;
        DWORD dwBytesNeeded;
        DWORD dwCount;

        LPENUM_SERVICE_STATUS   lpDependencies = NULL;
        ENUM_SERVICE_STATUS     ess;
        SC_HANDLE               hDepService;
        SERVICE_STATUS_PROCESS  ssp;

        DWORD dwStartTime = GetTickCount();
        DWORD dwTimeout = 30000; // 30-second time-out

        // Pass a zero-length buffer to get the required buffer size.
        if (EnumDependentServices(service_handle, SERVICE_ACTIVE,
          lpDependencies, 0, &dwBytesNeeded, &dwCount))
        {
          // If the Enum call succeeds, then there are no dependent
          // services, so do nothing.
          return TRUE;
        }
        else
        {
          if (GetLastError() != ERROR_MORE_DATA)
            return FALSE; // Unexpected error

          // Allocate a buffer for the dependencies.
          lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

          if (!lpDependencies)
            return FALSE;

          __try {
            // Enumerate the dependencies.
            if (!EnumDependentServices(service_handle, SERVICE_ACTIVE,
              lpDependencies, dwBytesNeeded, &dwBytesNeeded,
              &dwCount))
              return FALSE;

            for (i = 0; i < dwCount; i++)
            {
              ess = *(lpDependencies + i);
              // Open the service.
              hDepService = OpenService(service_control_manager,
                ess.lpServiceName,
                SERVICE_STOP | SERVICE_QUERY_STATUS);

              if (!hDepService)
                return FALSE;

              __try {
                // Send a stop code.
                if (!ControlService(hDepService,
                  SERVICE_CONTROL_STOP,
                  (LPSERVICE_STATUS)&ssp))
                  return FALSE;

                // Wait for the service to stop.
                while (ssp.dwCurrentState != SERVICE_STOPPED)
                {
                  Sleep(ssp.dwWaitHint);
                  if (!QueryServiceStatusEx(
                    hDepService,
                    SC_STATUS_PROCESS_INFO,
                    (LPBYTE)&ssp,
                    sizeof(SERVICE_STATUS_PROCESS),
                    &dwBytesNeeded))
                    return FALSE;

                  if (ssp.dwCurrentState == SERVICE_STOPPED)
                    break;

                  if (GetTickCount() - dwStartTime > dwTimeout)
                    return FALSE;
                }
              }
              __finally
              {
                // Always release the service handle.
                CloseServiceHandle(hDepService);
              }
            }
          }
          __finally
          {
            // Always free the enumeration buffer.
            HeapFree(GetProcessHeap(), 0, lpDependencies);
          }
        }
        return TRUE;
      }

      //
      // Wait for a process to stop.
      // @author Petru Barko, Augustin Preda.
      //
      bool WaitForServiceToStop(
        SC_HANDLE service_control_manager,
        SC_HANDLE service_handle,
        SERVICE_STATUS_PROCESS ssStatus)
      {
        DWORD dwBytesNeeded = 0;
        DWORD dwWaitTime = 0;

        // Save the tick count and initial checkpoint.
        DWORD dwStartTickCount = GetTickCount();
        DWORD dwOldCheckPoint = ssStatus.dwCheckPoint;

        while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
        {
          // Do not wait longer than the wait hint. A good interval is
          // one-tenth of the wait hint but not less than 1 second
          // and not more than 10 seconds.

          dwWaitTime = ssStatus.dwWaitHint / 10;

          if (dwWaitTime < 1000)
          {
            dwWaitTime = 1000;
          }
          else if (dwWaitTime > 10000)
          {
            dwWaitTime = 10000;
          }

          Sleep(dwWaitTime);

          // Check the status until the service is no longer stop pending.

          if (!QueryServiceStatusEx(
            service_handle,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // size needed if buffer is too small
          {
            GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());
            return false;
          }

          if (ssStatus.dwCheckPoint > dwOldCheckPoint)
          {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
          }
          else
          {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
              // Timeout waiting for the service to stop.
              return false;
            }
          }
        }

        return true;
      }

      //
      // Launch a service and wait for it to start.
      //
      bool StartServiceAndWait(
        SC_HANDLE service_control_manager,
        SC_HANDLE service_handle)
      {
        DWORD dwBytesNeeded = 0;
        DWORD dwWaitTime = 0;
        DWORD dwStartTickCount = 0;
        DWORD dwOldCheckPoint = 0;
        SERVICE_STATUS_PROCESS ssStatus;

        if (!StartService(
          service_handle,   // handle to service
          0,                // number of arguments
          NULL))            // no arguments
        {
          GTRACE(3, "Failed call to StartService. Last error: " << GetLastError());
          return false;
        }
        else
        {
          GTRACE(3, "Service start pending...");
        }

        // Check the status until the service is no longer start pending.

        if (!QueryServiceStatusEx(
          service_handle,                     // handle to service
          SC_STATUS_PROCESS_INFO,         // info level
          (LPBYTE)&ssStatus,             // address of structure
          sizeof(SERVICE_STATUS_PROCESS), // size of structure
          &dwBytesNeeded))              // if buffer too small
        {
          GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());
          return false;
        }

        // Save the tick count and initial checkpoint.

        dwStartTickCount = GetTickCount();
        dwOldCheckPoint = ssStatus.dwCheckPoint;

        while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
        {
          // Do not wait longer than the wait hint. A good interval is
          // one-tenth the wait hint, but no less than 1 second and no
          // more than 10 seconds.

          dwWaitTime = ssStatus.dwWaitHint / 10;

          if (dwWaitTime < 1000)
          {
            dwWaitTime = 1000;
          }
          else if (dwWaitTime > 10000)
          {
            dwWaitTime = 10000;
          }

          Sleep(dwWaitTime);

          // Check the status again.

          if (!QueryServiceStatusEx(
            service_handle,             // handle to service
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // if buffer too small
          {
            GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());
            break;
          }

          if (ssStatus.dwCheckPoint > dwOldCheckPoint)
          {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
          }
          else
          {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
              // No progress made within the wait hint.
              break;
            }
          }
        }

        return true;
      }
    }

    //
    // Start a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @return true if the service is running (either already running or running at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StartServiceWithName(const char* service_name)
    {
      bool result = false;
      SC_HANDLE service_control_manager = 0;
      SC_HANDLE service_handle = 0;

      SERVICE_STATUS_PROCESS ssStatus;
      DWORD dwBytesNeeded;

      // Get a handle to the SCM database.
      service_control_manager = OpenSCManager(
        NULL,                       // local computer
        SERVICES_ACTIVE_DATABASE,   // servicesActive database
        SC_MANAGER_ALL_ACCESS);     // full access rights

      if (NULL == service_control_manager)
      {
        GTRACE(3, "Failed call to OpenSCManager. Last error: " << GetLastError());
        return false;
      }

      // Get a handle to the service.
      service_handle = OpenService(
        service_control_manager,                                    // SCM database
        guslib::stringutil::StringToWString(service_name).c_str(),  // name of service
        SERVICE_ALL_ACCESS);                                        // full access

      if (service_handle == NULL)
      {
        GTRACE(3, "Failed call to OpenService. Last error: " << GetLastError());
        CloseServiceHandle(service_control_manager);
        return false;
      }

      // Check the status in case the service is not stopped.
      if (!QueryServiceStatusEx(
        service_handle,                   // handle to service
        SC_STATUS_PROCESS_INFO,           // information level
        (LPBYTE)&ssStatus,                // address of structure
        sizeof(SERVICE_STATUS_PROCESS),   // size of structure
        &dwBytesNeeded))                  // size needed if buffer is too small
      {
        GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());

        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
        return false;
      }

      // Check if the service is already running. If it's running and the command is to start it,
      // there is nothing remaining to be done, now is there?
      if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
      {
        GTRACE(3, "Cannot start the service because it is already running.");

        // Cannot start the service because it is already running.
        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
        return true;
      }

      // Wait for the service to stop before attempting to start it.
      if (!hidden::WaitForServiceToStop(service_control_manager, service_handle, ssStatus))
      {
        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
        return false;
      }

      // Attempt to start the service.
      if (!hidden::StartServiceAndWait(service_control_manager, service_handle))
      {
        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
        return false;
      }

      // Check the status at the end.
      if (!QueryServiceStatusEx(
        service_handle,                   // handle to service
        SC_STATUS_PROCESS_INFO,           // information level
        (LPBYTE)&ssStatus,                // address of structure
        sizeof(SERVICE_STATUS_PROCESS),   // size of structure
        &dwBytesNeeded))                  // size needed if buffer is too small
      {
        GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());

        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
        return false;
      }

      // Determine whether the service is running.

      if (ssStatus.dwCurrentState == SERVICE_RUNNING)
      {
        GTRACE(3, "Service started successfully.");
        result = true;
      }
      else
      {
        GTRACE(3, "Service not started.");
        GTRACE(3, "  Current State: " << ssStatus.dwCurrentState);
        GTRACE(3, "  Exit Code: " << ssStatus.dwWin32ExitCode);
        GTRACE(3, "  Check Point: " << ssStatus.dwCheckPoint);
        GTRACE(3, "  Wait Hint: " << ssStatus.dwWaitHint);
      }

      CloseServiceHandle(service_handle);
      CloseServiceHandle(service_control_manager);

      return result;
    }

    //
    // Stop a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @return true if the service is stopped (either already stopped or stopped at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StopServiceWithName(const char* service_name)
    {
      GTRACE(4, "Stopping service with name " << service_name);

      bool result = false;
      SC_HANDLE service_control_manager = 0;
      SC_HANDLE service_handle = 0;

      SERVICE_STATUS_PROCESS ssp;
      DWORD dwStartTime = GetTickCount();
      DWORD dwBytesNeeded;
      DWORD dwTimeout = 30000; // 30-second time-out

      // Get a handle to the SCM database.

      service_control_manager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

      if (NULL == service_control_manager)
      {
        std::stringstream error_message;
        error_message << "Failed call to OpenSCManager. Last error: " << GetLastError();
        GTRACE(3, error_message.str());
        return false;
      }

      // Get a handle to the service.
      service_handle = OpenService(
        service_control_manager,                                    // SCM database
        guslib::stringutil::StringToWString(service_name).c_str(),  // name of service
        SERVICE_ALL_ACCESS);

      if (service_handle == NULL)
      {
        std::stringstream error_message;
        error_message << "Failed call to OpenService. Last error: " << GetLastError();
        GTRACE(3, error_message.str());

        CloseServiceHandle(service_control_manager);
        return false;
      }

      try
      {
        // Make sure the service is not already stopped.

        if (!QueryServiceStatusEx(
          service_handle,
          SC_STATUS_PROCESS_INFO,
          (LPBYTE)&ssp,
          sizeof(SERVICE_STATUS_PROCESS),
          &dwBytesNeeded))
        {
          std::stringstream error_message;
          error_message << "Failed call to QueryServiceStatusEx. Last error: " << GetLastError();
          throw guslib::SimpleException(error_message.str().c_str());
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
          std::stringstream error_message;
          error_message << "Service is already stopped";
          result = true;
          throw guslib::SimpleException(error_message.str().c_str());
        }

        // If a stop is pending, wait for it.
        if (!hidden::WaitForServiceToStop(service_control_manager, service_handle, ssp))
        {
          throw guslib::SimpleException("Failed to stop service.");
        }

        // If the service is running, dependencies must be stopped first.
        GTRACE(4, "Stopping dependent service ...");
        hidden::StopDependentServices(service_control_manager, service_handle);

        // Send a stop code to the service.

        if (!ControlService(
          service_handle,
          SERVICE_CONTROL_STOP,
          (LPSERVICE_STATUS)&ssp))
        {
          std::stringstream error_message;
          error_message << "Failed call to ControlService. Last error: " << GetLastError();
          throw guslib::SimpleException(error_message.str().c_str());
        }

        // Wait for the service to stop.

        while (ssp.dwCurrentState != SERVICE_STOPPED)
        {
          Sleep(ssp.dwWaitHint);
          if (!QueryServiceStatusEx(
            service_handle,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
          {
            std::stringstream error_message;
            error_message << "Failed call to QueryServiceStatusEx. Last error: " << GetLastError();
            throw guslib::SimpleException(error_message.str().c_str());
          }

          if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

          if (GetTickCount() - dwStartTime > dwTimeout)
          {
            std::stringstream error_message;
            error_message << "Wait timed out";
            throw guslib::SimpleException(error_message.str().c_str());
          }
        }

        GTRACE(5, "Service stopped successfully");
        result = true;
       }
       catch (const std::exception& ex)
       {
         // Do anything?
         GTRACE(3, ex.what());
       }

       CloseServiceHandle(service_handle);
       CloseServiceHandle(service_control_manager);

      return result;
    }

    Status GetServiceStatus(const char* service_name)
    {
      SC_HANDLE service_control_manager = 0;
      SC_HANDLE service_handle = 0;

      service_control_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
      if (service_control_manager == NULL)
      {
        GTRACE(3, "Failed call to OpenSCManager. Last error: " << GetLastError());
        return SvcStatusError;
      }

      service_handle = OpenService(
        service_control_manager,
        guslib::stringutil::StringToWString(service_name).c_str(),
        SERVICE_ALL_ACCESS);
      if (service_handle == NULL)
      {
        GTRACE(3, "Failed call to OpenService. Last error: " << GetLastError());
        CloseServiceHandle(service_control_manager);
        return SvcStatusError;
      }

      Status result = SvcStatusError;
      SERVICE_STATUS_PROCESS stat;
      DWORD needed = 0;
      BOOL ret = QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (BYTE*)&stat, sizeof stat, &needed);
      if (ret == 0)
      {
        GTRACE(3, "Failed call to QueryServiceStatusEx. Last error: " << GetLastError());
        CloseServiceHandle(service_handle);
        CloseServiceHandle(service_control_manager);
      }

      if (stat.dwCurrentState == SERVICE_RUNNING)
      {
        GTRACE(3, "GetServiceStatus identified service [" << service_name << "] to be running");
        result = SvcStatusRunning;
      }
      else
      {
        GTRACE(3, "GetServiceStatus identified service [" << service_name << "] to be NOT running");
        result = SvcStatusStopped;
      }

      CloseServiceHandle(service_handle);
      CloseServiceHandle(service_control_manager);

      return result;
    }
  }
}

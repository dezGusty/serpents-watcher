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

// none

//
// C++ system headers
//

// none

//
// 3rd party libraries.
//
#include "guslib/trace/trace.h"
#include "guslib/util/stringutil.h"

TCHAR szSvcName[80];

SC_HANDLE schSCManager;
SC_HANDLE schService;

VOID __stdcall DoStartSvc(bool& succeded)
{
  SERVICE_STATUS_PROCESS ssStatus;
  DWORD dwOldCheckPoint;
  DWORD dwStartTickCount;
  DWORD dwWaitTime;
  DWORD dwBytesNeeded;

  succeded = false;

  // Get a handle to the SCM database. 

  schSCManager = OpenSCManager(
    NULL,                    // local computer
    SERVICES_ACTIVE_DATABASE,                    // servicesActive database 
    SC_MANAGER_ALL_ACCESS);  // full access rights 

  if (NULL == schSCManager)
  {

    printf("OpenSCManager failed (%d)\n", GetLastError());
    system("PAUSE");
    return;
  }

  // Get a handle to the service.

  schService = OpenService(
    schSCManager,         // SCM database 
    szSvcName,            // name of service 
    SERVICE_ALL_ACCESS);  // full access 

  if (schService == NULL)
  {
    printf("OpenService failed (%d)\n", GetLastError());
    CloseServiceHandle(schSCManager);
    return;
  }

  // Check the status in case the service is not stopped. 

  if (!QueryServiceStatusEx(
    schService,                     // handle to service 
    SC_STATUS_PROCESS_INFO,         // information level
    (LPBYTE)&ssStatus,             // address of structure
    sizeof(SERVICE_STATUS_PROCESS), // size of structure
    &dwBytesNeeded))              // size needed if buffer is too small
  {
    printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return;
  }

  // Check if the service is already running. It would be possible 
  // to stop the service here, but for simplicity this example just returns. 

  if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
  {
    printf("Cannot start the service because it is already running\n");
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    succeded = true;
    return;
  }

  // Save the tick count and initial checkpoint.

  dwStartTickCount = GetTickCount();
  dwOldCheckPoint = ssStatus.dwCheckPoint;

  // Wait for the service to stop before attempting to start it.

  while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
  {
    // Do not wait longer than the wait hint. A good interval is 
    // one-tenth of the wait hint but not less than 1 second  
    // and not more than 10 seconds. 

    dwWaitTime = ssStatus.dwWaitHint / 10;

    if (dwWaitTime < 1000)
      dwWaitTime = 1000;
    else if (dwWaitTime > 10000)
      dwWaitTime = 10000;

    Sleep(dwWaitTime);

    // Check the status until the service is no longer stop pending. 

    if (!QueryServiceStatusEx(
      schService,                     // handle to service 
      SC_STATUS_PROCESS_INFO,         // information level
      (LPBYTE)&ssStatus,             // address of structure
      sizeof(SERVICE_STATUS_PROCESS), // size of structure
      &dwBytesNeeded))              // size needed if buffer is too small
    {
      printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);
      return;
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
        printf("Timeout waiting for service to stop\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
      }
    }
  }

  // Attempt to start the service.

  if (!StartService(
    schService,  // handle to service 
    0,           // number of arguments 
    NULL))      // no arguments 
  {
    printf("StartService failed (%d)\n", GetLastError());
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return;
  }
  else printf("Service start pending...\n");

  // Check the status until the service is no longer start pending. 

  if (!QueryServiceStatusEx(
    schService,                     // handle to service 
    SC_STATUS_PROCESS_INFO,         // info level
    (LPBYTE)&ssStatus,             // address of structure
    sizeof(SERVICE_STATUS_PROCESS), // size of structure
    &dwBytesNeeded))              // if buffer too small
  {
    printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return;
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
      dwWaitTime = 1000;
    else if (dwWaitTime > 10000)
      dwWaitTime = 10000;

    Sleep(dwWaitTime);

    // Check the status again. 

    if (!QueryServiceStatusEx(
      schService,             // handle to service 
      SC_STATUS_PROCESS_INFO, // info level
      (LPBYTE)&ssStatus,             // address of structure
      sizeof(SERVICE_STATUS_PROCESS), // size of structure
      &dwBytesNeeded))              // if buffer too small
    {
      printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
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

  // Determine whether the service is running.

  if (ssStatus.dwCurrentState == SERVICE_RUNNING)
  {
    printf("Service started successfully.\n");
    succeded = true;
  }
  else
  {
    printf("Service not started. \n");
    printf("  Current State: %d\n", ssStatus.dwCurrentState);
    printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
    printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
    printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
  }

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);
}

VOID __stdcall DoStopSvc(bool& succeded)
{
  SERVICE_STATUS_PROCESS ssp;
  DWORD dwStartTime = GetTickCount();
  DWORD dwBytesNeeded;
  DWORD dwTimeout = 30000; // 30-second time-out
  DWORD dwWaitTime;

  succeded = false;

  // Get a handle to the SCM database. 

  schSCManager = OpenSCManager(
    NULL,                    // local computer
    NULL,                    // ServicesActive database 
    SC_MANAGER_ALL_ACCESS);  // full access rights 

  if (NULL == schSCManager)
  {
    printf("OpenSCManager failed (%d)\n", GetLastError());
    GetLastError();
    return;
  }

  // Get a handle to the service.

  schService = OpenService(
    schSCManager,         // SCM database 
    szSvcName,            // name of service 
    SERVICE_ALL_ACCESS);

  if (schService == NULL)
  {
    printf("OpenService failed (%d)\n", GetLastError());
    CloseServiceHandle(schSCManager);
    return;
  }

  // Make sure the service is not already stopped.

  if (!QueryServiceStatusEx(
    schService,
    SC_STATUS_PROCESS_INFO,
    (LPBYTE)&ssp,
    sizeof(SERVICE_STATUS_PROCESS),
    &dwBytesNeeded))
  {
    printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
    goto stop_cleanup;
  }

  if (ssp.dwCurrentState == SERVICE_STOPPED)
  {
    printf("Service is already stopped.\n");
    succeded = true;
    goto stop_cleanup;
  }

  // If a stop is pending, wait for it.

  while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
  {
    printf("Service stop pending...\n");

    // Do not wait longer than the wait hint. A good interval is 
    // one-tenth of the wait hint but not less than 1 second  
    // and not more than 10 seconds. 

    dwWaitTime = ssp.dwWaitHint / 10;

    if (dwWaitTime < 1000)
      dwWaitTime = 1000;
    else if (dwWaitTime > 10000)
      dwWaitTime = 10000;

    Sleep(dwWaitTime);

    if (!QueryServiceStatusEx(
      schService,
      SC_STATUS_PROCESS_INFO,
      (LPBYTE)&ssp,
      sizeof(SERVICE_STATUS_PROCESS),
      &dwBytesNeeded))
    {
      printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
      goto stop_cleanup;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
    {
      printf("Service stopped successfully.\n");
      succeded = true;
      goto stop_cleanup;
    }

    if (GetTickCount() - dwStartTime > dwTimeout)
    {
      printf("Service stop timed out.\n");
      goto stop_cleanup;
    }
  }

  // If the service is running, dependencies must be stopped first.

  StopDependentServices();

  // Send a stop code to the service.

  if (!ControlService(
    schService,
    SERVICE_CONTROL_STOP,
    (LPSERVICE_STATUS)&ssp))
  {
    printf("ControlService failed (%d)\n", GetLastError());
    goto stop_cleanup;
  }

  // Wait for the service to stop.

  while (ssp.dwCurrentState != SERVICE_STOPPED)
  {
    Sleep(ssp.dwWaitHint);
    if (!QueryServiceStatusEx(
      schService,
      SC_STATUS_PROCESS_INFO,
      (LPBYTE)&ssp,
      sizeof(SERVICE_STATUS_PROCESS),
      &dwBytesNeeded))
    {
      printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
      goto stop_cleanup;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
      break;

    if (GetTickCount() - dwStartTime > dwTimeout)
    {
      printf("Wait timed out\n");
      goto stop_cleanup;
    }
  }
  printf("Service stopped successfully\n");
  succeded = true;
stop_cleanup:
  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);
}

BOOL __stdcall StopDependentServices()
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
  if (EnumDependentServices(schService, SERVICE_ACTIVE,
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
      if (!EnumDependentServices(schService, SERVICE_ACTIVE,
        lpDependencies, dwBytesNeeded, &dwBytesNeeded,
        &dwCount))
        return FALSE;

      for (i = 0; i < dwCount; i++)
      {
        ess = *(lpDependencies + i);
        // Open the service.
        hDepService = OpenService(schSCManager,
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



BOOL __stdcall CheckServiceStatus(int& status)
{
  SC_HANDLE sch = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (sch == NULL){
    printf("OpenSCManager failed\n");
    return FALSE;
  }

  SC_HANDLE svc = OpenService(sch, szSvcName, SERVICE_ALL_ACCESS);
  if (svc == NULL){
    printf("OpenService failed\n");
    return FALSE;
  }

  SERVICE_STATUS_PROCESS stat;
  DWORD needed = 0;
  BOOL ret = QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO,
    (BYTE*)&stat, sizeof stat, &needed);
  if (ret == 0){
    printf("QueryServiceStatusEx failed\n");
    return FALSE;
  }

  if (stat.dwCurrentState == SERVICE_RUNNING){
    printf("%s is running\n", szSvcName);
    status = 1;
  }
  else{
    printf("%s is NOT running\n", szSvcName);
    status = 0;
  }

  CloseServiceHandle(svc);
  CloseServiceHandle(sch);

  return TRUE;
}

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
            CloseServiceHandle(service_handle);
            CloseServiceHandle(service_control_manager);

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
              CloseServiceHandle(service_handle);
              CloseServiceHandle(service_control_manager);

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

          CloseServiceHandle(service_handle);
          CloseServiceHandle(service_control_manager);
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
          printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
          CloseServiceHandle(service_handle);
          CloseServiceHandle(service_control_manager);
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
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
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
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @param service_name The name of the service to use.
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
      //DWORD dwOldCheckPoint;
      //DWORD dwStartTickCount;
      //DWORD dwWaitTime;
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
        return false;
      }
      
      // Attempt to start the service.
      if (!hidden::StartServiceAndWait(service_control_manager, service_handle))
      {
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
        printf("Service started successfully.\n");
        result = true;
      }
      else
      {
        printf("Service not started. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
      }

      CloseServiceHandle(service_handle);
      CloseServiceHandle(service_control_manager);

      return result;
    }
  }
}

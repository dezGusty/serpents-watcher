#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <aclapi.h>
#include <stdio.h>

#pragma comment(lib, "advapi32.lib")

/**
*Tries to start the service
*
*@param output variable, it will be initialized with true if the service was started and with false if it faild
*@return void
*/
VOID __stdcall DoStartSvc(bool&);

/**
*Tries to stop the service
*
*@param output variable, it will be initialized with true if the service was started and with false if it faild
*@return void
*/
VOID __stdcall DoStopSvc(bool&);

/**
*Tries to stop dependent services
*
*@return true if it succeded
*/
BOOL __stdcall StopDependentServices(void);

/**
*Checks the service status
*
*@return true if the check was successfull, false if not
*/
BOOL __stdcall CheckServiceStatus(int &);

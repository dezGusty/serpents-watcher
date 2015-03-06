#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <aclapi.h>
#include <stdio.h>

#pragma comment(lib, "advapi32.lib")

////////////////////////////////////////////SERVICES HEADER FUNCTIONS

VOID __stdcall DisplayUsage(void);

VOID __stdcall DoStartSvc(bool&);
VOID __stdcall DoStopSvc(bool&);

BOOL __stdcall StopDependentServices(void);

BOOL __stdcall CheckServiceStatus(int &);

////////////////////////////////////////////END SERVICES HEADER FUNCTIONS
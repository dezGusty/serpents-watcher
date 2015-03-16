#pragma once

#include <Windows.h>
#include <strsafe.h>


BOOL IsRunAsAdmin();
void ReportError(LPCWSTR pszFunction, DWORD dwError = GetLastError());
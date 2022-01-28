#pragma once
#ifndef _WIN32
#error This is a hook header FOR WINDOWS
#endif
// Windows headers
#ifndef _WINDOWS_
#include<Windows.h>
#endif

BOOL ApiInlineHook(LPCWSTR lpModule, LPCSTR lpFuncName, LPVOID lpFunction);
BOOL ApiInlineUnHook(LPCWSTR lpModule, LPCSTR lpFuncName);


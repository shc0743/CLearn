#pragma once
#ifndef _WIN32
#error This is a hook header FOR WINDOWS
#endif
// Windows headers
#ifndef _WINDOWS_
#include<Windows.h>
#endif

BOOL ApiInlineHook(PCWSTR lpModule, PCSTR lpFuncName, PVOID lpFunction);
BOOL ApiInlineUnHook(PCWSTR lpModule, PCSTR lpFuncName);


#pragma once
#ifndef _WIN32
#error This is a inject header FOR WINDOWS
#endif
// Windows headers
#ifndef _WINDOWS_
#include<Windows.h>
#endif

/**
@brief Inject a DLL file into a process (Unicode)
@param dwProcessId The Process ID to inject.
@param szDllPath The path of the dll.
@param dwTimeout The timeout waiting load finished.
@return Success - the handle of the dll in the process.  Failed - NULL or -1.\
		NULL: Use GetLastError to learn more. -1: timeout
*/
#ifdef __cplusplus
extern "C"
#endif
HMODULE InjectDllToProcess(DWORD dwProcessId, LPCWSTR szDllPath, DWORD dwTimeout
#ifdef __cplusplus
	= INFINITE
#endif
);


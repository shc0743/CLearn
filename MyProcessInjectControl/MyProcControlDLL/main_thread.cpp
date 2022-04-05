#include <Windows.h>
#ifdef _WIN64
#include "../utils/hook.x64.h"
#else
#include "../utils/hook.x64.h"
#endif
#include "../../resource/tool.h"
#include <string>


static void setHooks();
static bool UserControlUI(LPCSTR op_desc);

DWORD WINAPI ProcCtrl_MainThread(PVOID hDllModule) {
	/* Init */
	setHooks();

	return 0;
}


/*  BEGIN HOOK PROC DEFINES  */
HANDLE WINAPI MyOpenProcess(
	DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
BOOL WINAPI MyOpenProcessToken(HANDLE ProcessHandle,
	DWORD DesiredAccess, PHANDLE TokenHandle);
/*   END HOOK PROC DEFINES   */

void setHooks() {
	ApiInlineHook(L"kernel32.dll", "OpenProcess", MyOpenProcess);
	ApiInlineHook(L"kernel32.dll", "OpenProcessToken", MyOpenProcessToken);
}

bool UserControlUI(LPCSTR op_desc) {
	//HWND hwnd=CreateWindowExW(0,L"")

	return false;
}


/* HOOK PROCs */

HANDLE WINAPI MyOpenProcess(
	DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId
) {
	ApiInlineUnHook(L"kernel32.dll", "OpenProcess");
	HANDLE hResult = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	DWORD err = GetLastError();
	ApiInlineHook(L"kernel32.dll", "OpenProcess", MyOpenProcess);
	SetLastError(err);
	if (!hResult) return hResult;
	Process.flush();
	if (Process.find(dwProcessId).name() == TEXT("MyProcControl64.exe")) {
		//MessageBoxW(NULL, Process.find(dwProcessId).name().c_str(), L"test", 0);
		CloseHandle(hResult);
		//ApiInlineUnHook(L"kernel32.dll", "OpenProcess");
		//hResult = OpenProcess(dwDesiredAccess & PROCESS_QUERY_INFORMATION,
		//	bInheritHandle, dwProcessId);
		//ApiInlineHook(L"kernel32.dll", "OpenProcess", MyOpenProcess);
		SetLastError(ERROR_ACCESS_DENIED);
		return NULL;
	}
	return hResult;

}

BOOL __stdcall MyOpenProcessToken(HANDLE ProcessHandle,
	DWORD DesiredAccess, PHANDLE TokenHandle) {
	if (ProcessHandle == NULL || ProcessHandle == INVALID_HANDLE_VALUE
		|| TokenHandle == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	Process.flush();
	WCHAR path[MAX_PATH + 2]{}; DWORD size = MAX_PATH;
	if (QueryFullProcessImageName(ProcessHandle, 0, path, &size)) {
		if (std::wstring(path).substr(std::wstring(path).
			find(L"\\") + 1) == L"MyProcControl64.exe") {
			SetLastError(ERROR_ACCESS_DENIED);
			return NULL;
		}
	}

	if (ApiInlineUnHook(L"kernel32.dll", "OpenProcessToken")) {
		BOOL hResult = OpenProcessToken(ProcessHandle, DesiredAccess, TokenHandle);
		DWORD err = GetLastError();
		ApiInlineHook(L"kernel32.dll", "OpenProcessToken", MyOpenProcessToken);
		SetLastError(err);
		if (!hResult) return hResult;
		return hResult;
	}
	SetLastError(ERROR_FILE_NOT_FOUND);
	return 0;
}

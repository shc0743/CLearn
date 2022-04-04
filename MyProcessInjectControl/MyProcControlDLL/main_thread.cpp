#include <Windows.h>
#ifdef _WIN64
#include "../utils/hook.x64.h"
#else
#include "../utils/hook.x64.h"
#endif
#include "../../resource/tool.h"


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
/*   END HOOK PROC DEFINES   */

void setHooks() {
	ApiInlineHook(L"kernel32.dll", "OpenProcess", MyOpenProcess);
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
	if (
		Process.find(dwProcessId).name() == TEXT("MyProcControl64.exe")
		|| Process.find(dwProcessId).name() == TEXT("winlogon.exe")
	) {
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

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
	DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {

	return NULL;
}

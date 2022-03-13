// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <AclAPI.h>
#include "../../resource/tool.h"

DWORD WINAPI ProcCtrl_MainThread(PVOID hDllModule);
bool MyProtectWinObject(HANDLE hObject);
DWORD __stdcall ServiceWorker_subpentry(PVOID);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if (GetProgramInfo().name == "userinit.exe") {
            WCHAR SystemPath[MAX_PATH + 32]{ 0 };
            GetSystemDirectoryW(SystemPath, MAX_PATH);
            if (SystemPath[wcslen(SystemPath) - 1] == L'\\')
                SystemPath[wcslen(SystemPath) - 1] = 0;
            wcscat_s(SystemPath, L"\\userinit.exe");
            if (0 == _wcsicmp(s2wc(GetProgramDir()), SystemPath)) {
                HANDLE hTrd = CreateThread(0, 0, ServiceWorker_subpentry,
                    (LPVOID)(INT_PTR)GetCurrentProcessId(), CREATE_SUSPENDED, 0);
                if (hTrd) {
                    ResumeThread(hTrd);
                    CloseHandle(hTrd);
                }
            }
		}

        HANDLE hThread = CreateThread(NULL, 0, ProcCtrl_MainThread,
            hModule, CREATE_SUSPENDED, NULL);
        if (hThread) {
            MyProtectWinObject(hThread);
            ResumeThread(hThread);
            CloseHandle(hThread);
        } else {
            /* FATAL ERROR : CANNOT CREATE CONTROLLER THREAD */

        }
    }
    return TRUE;
}

bool MyProtectWinObject(HANDLE hObject) {
    SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = NULL;
    AllocateAndInitializeSid(&sia, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid); // init Everyone
    BYTE acl_buffer[0x400]{ 0 };
	PACL pAcl = (PACL)&acl_buffer;
	InitializeAcl(pAcl, 1024, ACL_REVISION);
	AddAccessDeniedAce(pAcl, ACL_REVISION, 
		GENERIC_WRITE | THREAD_TERMINATE | DELETE, pSid); 
	AddAccessAllowedAce(pAcl, ACL_REVISION,
		GENERIC_READ | READ_CONTROL | SYNCHRONIZE, pSid);
	DWORD result = SetSecurityInfo(hObject, SE_KERNEL_OBJECT, 
        DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
        NULL, NULL, pAcl, NULL);
	FreeSid(pSid);
    return result;
}


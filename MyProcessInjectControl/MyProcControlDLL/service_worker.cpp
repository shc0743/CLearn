#include <Windows.h>
#include "../../resource/tool.h"
using namespace std;

bool MyProtectWinObject(HANDLE hObject);
static DWORD WINAPI ServiceWorker_subpui(PVOID);
static DWORD WINAPI ServiceWorker_pprotect(PVOID);

static PCWSTR szServiceName;


DWORD __stdcall ServiceWorker_subpentry(PVOID) {
	MyProtectWinObject(GetCurrentProcess());
	CmdLineW cl(GetCommandLineW());
	std::wstring szServiceName, szParentPid;
	cl.getopt(L"service", szServiceName);
	cl.getopt(L"pid", szParentPid);
	if (szServiceName.empty() || szParentPid.empty())
		ExitProcess(ERROR_INVALID_PARAMETER);
	::szServiceName = szServiceName.c_str();
	DWORD pid = atol(ws2s(szParentPid).c_str());

	{ BYTE tmp = 0;
	s7::AdjustPrivilege(0x14, 1, 0, &tmp); }

	HANDLE hThread1 = NULL, hThread2 = NULL;

	hThread1 = CreateThread(0, 0, ServiceWorker_subpui, (PVOID)(INT_PTR)pid, 0, 0);
	hThread2 = CreateThread(0, 0, ServiceWorker_pprotect, (PVOID)(INT_PTR)pid, 0, 0);

	HANDLE hWaitsList[] = {
		hThread1, hThread2
	};
	WaitForMultipleObjects((sizeof(hWaitsList) / sizeof(HANDLE)),
		hWaitsList, TRUE, INFINITE);

	ExitProcess(0);
	return 0;
}

static DWORD WINAPI ServiceWorker_subpui(PVOID _pid) {
	STARTUPINFO si{ 0 };
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION pi{ 0 };

	wstring app_name, s_cmd_line;
	app_name = Process.find((DWORD)(INT_PTR)_pid).name();
	if (app_name.empty()) {
		/*WCHAR szTitle[] = L"ERROR";
		WCHAR szText[1024]{ 0 };
		wcscpy_s(szText, LastErrorStrW().c_str());
		DWORD resp = 0;
		WTSSendMessageW(NULL, WTSGetActiveConsoleSessionId(),
			szTitle, DWORD(wcslen(szTitle) *2),
			szText, DWORD(wcslen(szText) *2),
			MB_ICONHAND, 5000, &resp, TRUE);*/
		return 1;
	}
	s_cmd_line = L"\"" + app_name + L"\" --ui --hidden --from-service "
		"--service-name=\"" + ::szServiceName + L"\"";
	WCHAR cmd_line[512]{ 0 };
	wcscpy_s(cmd_line, s_cmd_line.c_str());

	while (1) {
		if (!Process.StartAsActiveUserT(app_name.c_str(), cmd_line, 0, 0,
			0, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
			Sleep(5000); continue;
		}
		/*  Adjust token to let the process have debug privilege
		    so that sub process can open this process
		*/  {
			HANDLE hToken = NULL;
			LUID Luid{ 0 };
			TOKEN_PRIVILEGES tp{ 0 };

			if (OpenProcessToken(pi.hProcess, TOKEN_ADJUST_PRIVILEGES |
				TOKEN_QUERY, &hToken)) {
				if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid)) {
					CloseHandle(hToken);
				} else {
					tp.PrivilegeCount = 1;
					tp.Privileges[0].Luid = Luid;
					tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
					AdjustTokenPrivileges(hToken, false, &tp, sizeof(tp), NULL, NULL);
					CloseHandle(hToken);
				}
			}
		}

		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		if (Process.find(Process.GetParentProcessId(
			GetCurrentProcessId())).id() == 0) // parent process died
			return 0;                          // ... end process
	}

	return 0;
}

DWORD __stdcall ServiceWorker_pprotect(PVOID _id) {
	DWORD pid = (DWORD)(LONG_PTR)_id;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		DWORD err = GetLastError();
		//fstream fp("error.log", ios::app);
		//fp << "ERROR " << err << ": " << ErrorCodeToStringA(err) << endl;
		//fp << "PID: " << pid << endl;
		//fp.close();
		return err;
	}
	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
	ServiceManager.Start(ws2s(szServiceName));
	ExitProcess(ERROR_PROCESS_ABORTED);
	return 0;
}


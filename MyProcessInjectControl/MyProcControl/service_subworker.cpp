#include <Windows.h>
#include "../../resource/tool.h"
#include "resource.h"
#include "../utils/inject.h"
using namespace std;

//bool MyProtectWinObject(HANDLE hObject);
DWORD __stdcall ServiceWorker_subpentry(PVOID);
static DWORD WINAPI ServiceWorker_subpui(PVOID);
static DWORD WINAPI ServiceWorker_pprotect(PVOID);
static DWORD WINAPI ServiceWorker_pipe_control(PVOID);

static PCWSTR szServiceName;
static HMODULE hInst;
static HMODULE hDllExports;
static WCHAR DllToInject_Path[MAX_PATH + 2];


DWORD __stdcall ServiceWorker_subpentry(PVOID) {
	//MyProtectWinObject(GetCurrentProcess());
	hInst = GetModuleHandle(NULL);
	CmdLineW cl(GetCommandLineW());
	std::wstring szServiceName, szParentPid;
	cl.getopt(L"service", szServiceName);
	cl.getopt(L"pid", szParentPid);
	if (szServiceName.empty() || szParentPid.empty())
		ExitProcess(ERROR_INVALID_PARAMETER);
	::szServiceName = szServiceName.c_str();
	DWORD pid = atol(ws2s(szParentPid).c_str());

	s7::AdjustPrivilege(0x14, 1, 0);
	EnableAllPrivileges();

	GetTempPathW(MAX_PATH - 32, DllToInject_Path);
	wcscat_s(DllToInject_Path, wstring(L"/tmp" + szServiceName).c_str());
#ifdef _WIN64
	FreeResFile(IDR_BIN_DLL_CTL_x64, L"BIN", DllToInject_Path + L"64.tmp"s);
#endif
	FreeResFile(IDR_BIN_DLL_CTL_x86, L"BIN", DllToInject_Path + L"86.tmp"s);

	WCHAR szDllExportsPath[MAX_PATH + 2]{};
	GetTempPathW(MAX_PATH - 18, szDllExportsPath);
	GetTempFileNameW(wstring(szDllExportsPath).c_str(), L"", 0, szDllExportsPath);
	FreeResFile(IDR_BIN_DLL_EXPORTS_x64, L"BIN", szDllExportsPath);
	hDllExports = LoadLibraryW(szDllExportsPath);

	HANDLE hThread[3]{ 0 };

	hThread[0] = CreateThread(0, 0, ServiceWorker_subpui, (PVOID)(INT_PTR)pid, 0, 0);
	hThread[1] = CreateThread(0, 0, ServiceWorker_pprotect, (PVOID)(INT_PTR)pid, 0, 0);
	hThread[2] = CreateThread(0, 0, ServiceWorker_pipe_control, (PVOID)(INT_PTR)pid, 0, 0);

	HANDLE hWaitsList[] = {
		hThread[0], hThread[1], hThread[2]
	};
	DWORD cnt = DWORD(sizeof(hWaitsList) / sizeof(HANDLE));
	WaitForMultipleObjects(cnt, hWaitsList, TRUE, INFINITE);
	for (DWORD n = 0; n < cnt; ++n)
		if (hThread[n]) CloseHandle(hThread[n]);

	DeleteFileW(szDllExportsPath);
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
	app_name = s2ws(GetProgramDir());
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

static DWORD __stdcall AttachProcessController(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) return GetLastError();
	//WCHAR dll_path[1024]{ 0 };
	//GetModuleFileNameW(hInst, dll_path, 1023);
	wstring final_path = DllToInject_Path;
	bool isx64 = false;

	using __util_Is_exe_32_or_64_bit = unsigned char (*__stdcall)(PCWSTR);
	__util_Is_exe_32_or_64_bit util_Is_exe_32_or_64_bit = nullptr;
	if (hDllExports) util_Is_exe_32_or_64_bit = (__util_Is_exe_32_or_64_bit)
		GetProcAddress(hDllExports, "util_Is_exe_32_or_64_bit");
	if (util_Is_exe_32_or_64_bit) isx64 = (util_Is_exe_32_or_64_bit(
		Process.GetProcessFullPathById(pid).c_str()) == 64);

	if (isx64) final_path += L"64.tmp";
	else       final_path += L"86.tmp";
	HMODULE result = InjectDllToProcess_HANDLE(
		hProcess, final_path.c_str(), 5000);
	CloseHandle(hProcess);
	return		(result == (HMODULE)-1) ? ERROR_TIMEOUT :
				(result ? 0 : GetLastError());
}

DWORD __stdcall ServiceWorker_pipe_control(PVOID) {
	WCHAR pipe_name[256] = { 0 };  // Pipe name
	LoadStringW(hInst, IDS_STRING_SVC_PIPENAME, pipe_name, 128);
	wcscat_s(pipe_name, L"\\");
	wcscat_s(pipe_name, szServiceName);
	HANDLE hCtlNamedPipe = CreateNamedPipeW(pipe_name,
		PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE
		| PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
		PIPE_UNLIMITED_INSTANCES, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (!hCtlNamedPipe || hCtlNamedPipe == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		return err;
	}
	auto connector = [](HANDLE pipe) {
		CHAR buffer[4096] = { 0 };
		while (1) {
			ConnectNamedPipe(pipe, 0);
			while (ReadFile(pipe, buffer, 4096, 0, 0)) {
				//fstream fp("debug.log", ios::app);
				//fp << buffer << endl;
				//fp.close();
				if (string(buffer).find("Attach-Process-Control ") == 0) {
					string str = buffer;
					DWORD pid = atol(str.substr(str.find("/pid=") + 5).c_str());
					WCHAR title[] = L"TEST";
					wstring msg = to_wstring(pid);
					DWORD tmp = 0;
					//WTSSendMessage(0, WTSGetActiveConsoleSessionId(), title
					//, 4, (PWSTR)msg.c_str(), msg.length(), 0, 10, &tmp, 1);
					if (!pid) {
						WriteFile(pipe, "87", 2, 0, 0);
						break;
					}
					DWORD err = AttachProcessController(pid);
					if (err) {
						string errs = to_string(err);
						WriteFile(pipe, errs.c_str(), DWORD(errs.length()), 0, 0);
						break;
					}
					WriteFile(pipe, "0", 1, 0, 0);
					break;
				}
			}
			DisconnectNamedPipe(pipe);
		}
	};
	HANDLE hTh = (HANDLE)_beginthread(connector, 0, hCtlNamedPipe);
	if (!hTh) {
		CloseHandle(hCtlNamedPipe);
		return GetLastError();
	}
	WaitForSingleObject(hTh, INFINITE);
	([] (HANDLE obj) {
		__try {
			if (obj) CloseHandle(obj);
		} __except (EXCEPTION_EXECUTE_HANDLER) {};
	})(hCtlNamedPipe);

	return 0;
}


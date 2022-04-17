#include "utilman.process.hpp"
#include "utilman.ui.hpp"
#include "../res/utilman_1.h"
using namespace std;

// defines
static DWORD WINAPI SubProcess_root_thread(PVOID pCLObject);
static DWORD WINAPI SubProcess_wait_for_parent_process(PVOID);
static int __stdcall SubProcess_type__ui(CmdLineW& cl);
static int __stdcall SubProcess_type__dllhost(CmdLineW& cl);
// end defines


int __stdcall SubProcess_type_handler(std::wstring type, CmdLineW& cl) {
	if (type == L"") return (int)0x80000000;
	else if (type == L"ui") {
		return SubProcess_type__ui(cl);
	}
	else if (type == L"dllhost") {
		return SubProcess_type__dllhost(cl);
	}

	throw SubProcess_type_ValueError();
	return 0;
}

int __stdcall SubProcess_root(CmdLineW& cl) {
	MyInitWndClasses();

	WCHAR wclass[256]{};
	LoadStringW(ThisInst, IDS_STRING_UI_CLASS_CTLWND, wclass, 255);
	HWND hwnd = NULL;
	if ((hwnd = FindWindowW(wclass, NULL))) {
		return (int)SendMessage(hwnd, MYWM_CTLWND_OPENUI, 0, 0);
	}
	hwnd = CreateWindowExW(0, wclass, L"MyUtilManager Control Window",
		WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, 0, 0);
	if (!hwnd) return (int)GetLastError();

	{
		PVOID param[2]{ &cl,hwnd };
		HANDLE hThread = CreateThread(0, 0, SubProcess_root_thread, &param, 0, 0);
		if (!hThread) return (int)GetLastError();
		else CloseHandle(hThread);
	}

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
}


static int __stdcall SubProcess_type__ui(CmdLineW& cl) {
	CreateThread(0, 0, SubProcess_wait_for_parent_process, 0, 0, 0);

	MyInitWndClasses();

	int nCmdShow = SW_NORMAL;
	if (cl.getopt(L"hidden")) nCmdShow = SW_HIDE;
	else {
		STARTUPINFO si{};
		GetStartupInfo(&si);
		if (si.dwFlags & STARTF_USESHOWWINDOW)
			nCmdShow = si.wShowWindow;
	}

	WCHAR wclass[256]{ 0 };
	LoadStringW(ThisInst, IDS_STRING_UI_CLASS_UIWND, wclass, 255);
	HWND hwnd = NULL;
	if ((hwnd = FindWindowW(wclass, NULL))) {
		ShowWindow(hwnd, SW_NORMAL);
		SetForegroundWindow(hwnd);
		return ERROR_ALREADY_EXISTS;
	}
	hwnd = CreateWindowExW(0, wclass, L"My Utility Manager",
		WS_OVERLAPPEDWINDOW, 0, 0, 500, 300, 0, 0, 0, 0);
	if (!hwnd) return (int)GetLastError();
	CenterWindow(hwnd);
	ShowWindow(hwnd, nCmdShow);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
}

static int __stdcall SubProcess_type__dllhost(CmdLineW& cl) {
	CreateThread(0, 0, SubProcess_wait_for_parent_process, 0, 0, 0);

	MyInitWndClasses();

	WCHAR wclass[256]{};
	LoadStringW(ThisInst, IDS_STRING_UI_CLASS_DLLHOST, wclass, 255);
	HWND hwnd = NULL;
	if ((hwnd = FindWindowW(wclass, NULL))) {
		return (int)SendMessage(hwnd, MYWM_CTLWND_OPENUI, 0, 0);
	}
	hwnd = CreateWindowExW(0, wclass, L"MyUtilManager dllhost Window [root]",
		WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, 0, 0);
	if (!hwnd) return (int)GetLastError();

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
}


HANDLE WINAPI MyCreateProcessSuspendedW(PCWSTR app, std::wstring cmdl,
	bool inheritHandles = false);


static DWORD WINAPI SubProcess_root_thread(PVOID pCLObject){
	CmdLineW& cl = *((CmdLineW*)(*((PVOID*)pCLObject)));
	HWND hwnd = (HWND)(*(((PVOID*)pCLObject) + 1));

	HANDLE hJob = CreateJobObject(NULL, NULL);
	if (!hJob) return GetLastError();
	wstring app_name = s2ws(GetProgramDir());
	HANDLE hProcess_ui = 0, hProcess_dllhost = 0;
	time_t last_fail_time = 0;
#pragma warning(push)
#pragma warning(disable: 6001)
	while (TRUE) {
		if(!hProcess_dllhost)
			hProcess_dllhost = MyCreateProcessSuspendedW(app_name.c_str(),
			L"SubProcess --type=dllhost --spawnwnd=" +
			to_wstring((INT_PTR)hwnd));
		if(!hProcess_ui)
			hProcess_ui = MyCreateProcessSuspendedW(app_name.c_str(),
			L"SubProcess --type=ui");

		if (!(hProcess_ui && hProcess_dllhost)) {
			if (hProcess_dllhost) {
				s7::CallNtTerminateProcess(hProcess_dllhost, 1);
				CloseHandle(hProcess_dllhost);
			}
			if (hProcess_ui) {
				s7::CallNtTerminateProcess(hProcess_ui, 1);
				CloseHandle(hProcess_ui);
			}

			if (last_fail_time == time(0)) {
				Sleep(5000);
			}
			else last_fail_time = time(0);

			continue;
		}

		AssignProcessToJobObject(hJob, GetCurrentProcess());
		AssignProcessToJobObject(hJob, hProcess_dllhost);
		AssignProcessToJobObject(hJob, hProcess_ui);

		Process.resume(hProcess_dllhost);
		Process.resume(hProcess_ui);

		HANDLE processes[]{ hProcess_dllhost,hProcess_ui };
		WaitForMultipleObjects(2, processes, FALSE, INFINITE);

		DWORD exitCode = 0;
		if (!GetExitCodeProcess(hProcess_dllhost, &exitCode) ||
			exitCode != STILL_ACTIVE) { // died
			CloseHandle(hProcess_dllhost);
			hProcess_dllhost = NULL;
		}
		if (!GetExitCodeProcess(hProcess_ui, &exitCode) ||
			exitCode != STILL_ACTIVE) { // died
			CloseHandle(hProcess_ui);
			hProcess_ui = NULL;
		}

		last_fail_time = time(0);
	}
#pragma warning(pop)

	CloseHandle(hJob);

	return 0;
}

static DWORD __stdcall SubProcess_wait_for_parent_process(PVOID) {
	HANDLE parentp = OpenProcess(SYNCHRONIZE, FALSE,
		Process.GetParentProcessId(GetCurrentProcessId()));
	if (!parentp) return GetLastError();
	DWORD exitCode = 1;
	WaitForSingleObject(parentp, INFINITE);
	GetExitCodeProcess(parentp, &exitCode);
	CloseHandle(parentp);
	ExitProcess((UINT)exitCode);
	return 0;
}


HANDLE __stdcall MyCreateProcessSuspendedW(
	PCWSTR app, std::wstring cmdl, bool inheritHandles
) {
	PWSTR pNewProcessCmdLine = new WCHAR[cmdl.length() + 2];
	if (!pNewProcessCmdLine) return NULL;
	wcscpy_s(pNewProcessCmdLine, cmdl.length() + 1, cmdl.c_str());
	STARTUPINFOW si{};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi{};
	BOOL bSuccess = CreateProcessW(app, pNewProcessCmdLine,
		0, 0, inheritHandles, CREATE_SUSPENDED, 0, 0, &si, &pi);
	DWORD err = GetLastError();
	delete[] pNewProcessCmdLine;
	SetLastError(err);
	if (!bSuccess) return NULL;
	CloseHandle(pi.hThread);
	return pi.hProcess;
}



#include <iostream>
#include "../../resource/tool.h"
using namespace std;

HWND WinLockMainWindow;
string UnlockKey;
string promptText;
string current_running_uuid;

LRESULT CALLBACK HookIgnoreAll(int code, WPARAM wParam, LPARAM lParam) {
	return 1;
}

DWORD __stdcall WindowOnTopThrd(LPVOID p) {
	::SetParent((HWND)p, WinLockMainWindow);
	while (1) ::SetWindowPos((HWND)p, (HWND)-1, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return 0;
}

void __stdcall MouseLock() {
	RECT rc{ 0,0,1,1 };
	if (!ClipCursor(&rc)) {
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
	}
}

DWORD __stdcall verify_main_thrd(LPVOID) {
	{fstream vbsfile(".vbs", ios::out);
	if (!vbsfile) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Create File", L"ERROR", MB_ICONERROR);
		exit(1);
	}
	vbsfile << "a=InputBox(\"" << promptText << "\",\"WinLock\")\nset w=wscript.createobject(\"Wscript."
		"Shell\")\nw.run \"cmd /c echo \" & a & \" > _\",0" << endl;
	vbsfile.close(); }
	auto pi = Process.Start(L"wscript .vbs");
	if (!pi.hProcess) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Create Process", L"ERROR", MB_ICONERROR);
		exit(1);
	}
	HANDLE wott = 0;
	while (1) {
		HWND hwnd = ::FindWindowA("#32770", "WinLock");
		if (hwnd) {
			wott = ::CreateThread(0, 0, WindowOnTopThrd, hwnd, 0, 0);
			break;
		}
		::Sleep(500);
	}
	::WaitForSingleObject(pi.hProcess, MAXDWORD);
	Process.CloseProcessHandle(pi);
	remove(".vbs");
	Sleep(300);
#pragma warning(disable: 6258)
	if(wott) ::TerminateThread(wott, 0);
	fstream ulk("./_", ios::in);
	if (!ulk) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Open File", L"ERROR", MB_ICONERROR);
		exit(1);
	}
	size_t bufsize = 8192;
	char* buffer = (char*)GetBufMemory(bufsize);
	printf("%lld", (__int64)(&buffer));
	if (!buffer) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Malloc", L"ERROR", MB_ICONERROR);
		exit(1);
	}
	ulk.getline(buffer, bufsize);
	ulk.close();
	remove("_");
	if (_stricmp(buffer, UnlockKey.c_str()) == 0) {
		HKEY hkAutoRun = NULL;
		if (0 == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0, KEY_ALL_ACCESS, &hkAutoRun)) {
			RegDeleteValueW(hkAutoRun, s2wc("WinLock_"s + current_running_uuid));
			RegCloseKey(hkAutoRun);
		} else
		if (0 == RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0, KEY_ALL_ACCESS, &hkAutoRun)) {
			RegDeleteValueW(hkAutoRun, s2wc("WinLock_"s + current_running_uuid));
			RegCloseKey(hkAutoRun);
		}
		ClipCursor(NULL); exit(0);
	}
	free(buffer);
	HANDLE hdl = (HANDLE)::_beginthread((_beginthread_proc_type)verify_main_thrd, 0, 0);
	if (!hdl) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Create New Thread", L"ERROR", MB_ICONERROR);
		return 1;
	}
	::CloseHandle(hdl);
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2) return -1;
	UnlockKey = argv[1]; UnlockKey += " ";
	if (argc >= 3) promptText = argv[2]; else promptText = "请输入解锁码";
	WinLockMainWindow = ::CreateWindowExW(WS_EX_NOACTIVATE, L"#32770", L"_WinLock_", WS_DISABLED,
		0, 0, 320, 200, NULL, NULL, ::GetModuleHandle(0), NULL);
	if (!WinLockMainWindow) {
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Create Window", L"ERROR", MB_ICONERROR);
		return 1;
	}
	::EnableWindow(WinLockMainWindow, true);
	::ShowWindow(WinLockMainWindow, SW_SHOW);
	::UpdateWindow(WinLockMainWindow);
	::SetWindowPos(WinLockMainWindow, (HWND)-1, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	current_running_uuid = GenerateUUID();
	HKEY hkAutoRun = NULL;
	if (0 == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_ALL_ACCESS, &hkAutoRun)) {
		string cl = GetCommandLineA();
		RegSetValueExA(hkAutoRun, ("WinLock_"s + current_running_uuid).c_str(), 0, REG_SZ, 
			(BYTE*)(cl.c_str()), DWORD(cl.length()));
		RegCloseKey(hkAutoRun);
	} else
	if (0 == RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_ALL_ACCESS, &hkAutoRun)) {
		string cl = GetCommandLineA();
		RegSetValueExA(hkAutoRun, ("WinLock_"s + current_running_uuid).c_str(), 0, REG_SZ, 
			(BYTE*)(cl.c_str()), DWORD(cl.length()));
		RegCloseKey(hkAutoRun);
	}

	::FullScreenWindow(WinLockMainWindow);

	//mshook = SetWindowsHookExA(WH_MOUSE, HookIgnoreAll, ::GetModuleHandle(0), 0);
	//if (!mshook) {
	//	::DestroyWindow(WinLockMainWindow);
	//	MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
	//		1, EVENTLOG_ERROR_TYPE);
	//	::MessageBoxW(NULL, L"Cannot SetWindowsHookExA", L"ERROR", MB_ICONERROR);
	//	return 1;
	//}

	MouseLock();

	HANDLE hdl = (HANDLE)::CreateThread(0, 0, verify_main_thrd, 0, 0, 0);
	if (!hdl) {
		::DestroyWindow(WinLockMainWindow); ClipCursor(NULL);
		MyWriteApplicationEvent((to_string(GetLastError()) + ::LastErrorStrA()).c_str(),
			1, EVENTLOG_ERROR_TYPE);
		::MessageBoxW(NULL, L"Cannot Create Thread", L"ERROR", MB_ICONERROR);
		return 1;
	}

	MSG msg; BOOL GMRes = 0;
	while (GMRes = ::GetMessage(&msg, WinLockMainWindow, 0, 0) && GMRes != -1) {
		::DispatchMessage(&msg);
		::TranslateMessage(&msg);
	}

	while (WAIT_TIMEOUT == WaitForSingleObject(hdl, MAXDWORD));
	::CloseHandle(hdl);

	ClipCursor(NULL);
	return 0;
}

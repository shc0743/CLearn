#include <Windows.h>
#include <iostream>
#include <map>
#include "../../resource/tool.h"
#include "resource.h"
using namespace std;

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma warning(disable: 6001)

#define RANDOM_CHARS_LIST \
	L"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
HWND __stdcall CreateMainWindow();
//static WCHAR MainWindowClassName[256]{ 0 };
LRESULT CALLBACK WndProc_mainwnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int argc, char* argv[]) {

	if (argc >= 3 && _stricmp(argv[1], "/service") == 0) {

	}


	STARTUPINFO si{ sizeof(STARTUPINFO),0 };
	GetStartupInfo(&si);
	HWND hWnd_main = CreateMainWindow();
	if (!hWnd_main) {
		MessageBoxW(NULL, L"无法创建应用程序主窗口!!", NULL, MB_ICONHAND);
		return GetLastError();
	}
	ShowWindow(hWnd_main, (si.dwFlags & STARTF_USESHOWWINDOW) ?
		si.wShowWindow : SW_NORMAL);

	{vector<ProcessInfo> mythwares;
	Process.find(_T("StudentMain.exe"), mythwares);
	if (!mythwares.empty() && !
		((si.dwFlags & STARTF_USESHOWWINDOW) && si.wShowWindow == SW_HIDE)) {
		int result = MessageBoxTimeoutW(NULL, L"您要暂停或结束极域电子教室吗?\n\n"
			"按[是]暂停,\n按[否]结束,\n或按[取消]取消。\n\nTimeout: 5000", L"",
			MB_YESNOCANCEL | MB_TOPMOST | MB_DEFBUTTON2, 0, 5000);
		HMODULE kernel32 = LoadLibrary(_T("kernel32.dll"));
		if (!kernel32) {
			MessageBoxW(NULL, L"无法加载 kernel32.dll", NULL, MB_ICONHAND);
		} else {
			LPTHREAD_START_ROUTINE pExitProcess = (LPTHREAD_START_ROUTINE)
				GetProcAddress(kernel32, "ExitProcess");
			if (result == IDYES) for (auto& i : mythwares) Process.suspend(i);
			else if (result == IDNO) for (auto& i : mythwares) {
				HANDLE h = NULL;
				h = CreateRemoteThread(i, 0, 0, pExitProcess, 0, 0, 0);
				if (h) CloseHandle(h);
			}
		}
	}}

	MSG msg{ 0 };
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;

	return 0;
}

HWND __stdcall CreateMainWindow() {
	WCHAR lpszClassName[256]{ 0 };
	LoadStringW(NULL, IDS_STRING_UI_MWCLAP, lpszClassName, 255);
	if (!lpszClassName[0]) return NULL;
	wstring random1, random2;
	srand((UINT)time(0));
	random1 = GenerateRandomString(8, (wstring)RANDOM_CHARS_LIST);
	srand((UINT)time(0) - 2);
	random2 = GenerateRandomString(8, (wstring)RANDOM_CHARS_LIST);
	wcscat_s(lpszClassName, random1.c_str());
	WNDCLASSEXW wcex{ 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.hbrBackground = CreateSolidBrush(RGB(0xCC, 0xCC, 0xCC));
	if (!s7::MyRegisterClassW(lpszClassName, WndProc_mainwnd, wcex)) return NULL;
	HWND hWnd = CreateWindowExW(0, lpszClassName, random2.c_str(), WS_OVERLAPPED |
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		256, 256, NULL, NULL, NULL, NULL);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 500, 300, SWP_NOMOVE | SWP_NOACTIVATE);
	return hWnd;
}

typedef struct {
	HWND b1, b2, b3, bExit;
	HFONT fnt;
} Tmainwnd_ctls;
map<HWND, Tmainwnd_ctls> mainwnd_ctls;
LRESULT CALLBACK WndProc_mainwnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		Tmainwnd_ctls c{ 0 };

		c.b1 = CreateWindowExW(0, L"Button", L"结束极域",
			WS_CHILD | WS_VISIBLE | BS_CENTER,
			10, 10, 465, 40, hwnd, (HMENU)41, 0, 0);
		c.b2 = CreateWindowExW(0, L"Button", L"暂停极域",
			WS_CHILD | WS_VISIBLE | BS_CENTER,
			10, 60, 225, 40, hwnd, (HMENU)42, 0, 0);
		c.b3 = CreateWindowExW(0, L"Button", L"恢复极域",
			WS_CHILD | WS_VISIBLE | BS_CENTER,
			250, 60, 225, 40, hwnd, (HMENU)43, 0, 0);
		c.bExit = CreateWindowExW(0, L"Button", L"退出程序",
			WS_CHILD | WS_VISIBLE | BS_CENTER,
			350, 210, 125, 40, hwnd, (HMENU)44, 0, 0);

		c.fnt = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		if (c.fnt) {
			SendMessage(c.b1, WM_SETFONT, (WPARAM)c.fnt, 0);
			SendMessage(c.b2, WM_SETFONT, (WPARAM)c.fnt, 0);
			SendMessage(c.b3, WM_SETFONT, (WPARAM)c.fnt, 0);
			SendMessage(c.bExit, WM_SETFONT, (WPARAM)c.fnt, 0);
		}

		mainwnd_ctls.insert(make_pair(hwnd, c));

		SetTimer(hwnd, 123, 10000, NULL);
		SetTimer(hwnd, 124, 300, NULL);

		SetWindowLong(hwnd, GWL_EXSTYLE,
			GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, NULL, 0xCC, LWA_ALPHA);

		EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);

		break;
	}

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmCommand = HIWORD(wParam);
		switch (wmId) {
		case 41: {
			vector<ProcessInfo> mythwares;
			Process.flush();
			Process.find(_T("StudentMain.exe"), mythwares);
			if (mythwares.empty()) {
				MessageBoxW(NULL, L"找不到极域电子教室", NULL, MB_ICONHAND);
			}
			HMODULE kernel32 = LoadLibrary(_T("kernel32.dll"));
			if (!kernel32) {
				MessageBoxW(NULL, L"无法加载 kernel32.dll", NULL, MB_ICONHAND);
			} else {
				LPTHREAD_START_ROUTINE pExitProcess = (LPTHREAD_START_ROUTINE)
					GetProcAddress(kernel32, "ExitProcess");
				for (auto& i : mythwares) {
					HANDLE h = NULL;
					h = CreateRemoteThread(i, 0, 0, pExitProcess, 0, 0, 0);
					if (h) CloseHandle(h);
					else MessageBoxW(NULL, (L"无法结束极域电子教室 (PID: " +
						to_wstring(i.id()) + L") : " + LastErrorStrW()).c_str(),
						NULL, MB_ICONHAND);
				}
			}
			break;
		}

		case 44:
			DestroyWindow(hwnd);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;
	}

	case WM_TIMER:
		switch (wParam) {
		case 123: {
			SetWindowTextW(hwnd, GenerateRandomString(
				8, (wstring)RANDOM_CHARS_LIST).c_str());
			break;
		}
		case 124:
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

	case WM_CLOSE:
		//DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		KillTimer(hwnd, 123);
		KillTimer(hwnd, 124);
		try { mainwnd_ctls.erase(hwnd); } catch (...) {};
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


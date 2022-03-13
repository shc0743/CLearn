#include <iostream>
#include <Richedit.h>
#include "../../resource/tool.h"
#include "../utils/inject.h"
using namespace std;

int WdMain();

int main(int argc, char* argv[]) {
	if (argc < 2) return WdMain();
	if (_stricmp("/?", argv[1]) == 0) {
		printf("Inject a DLL to a process.\nUsage:\n\t%s %s\n",
			argv[0], "<DLL_path> <PID|([-n ]ProcessName)>");
		return 0;
	}
	if (_stricmp("gui", argv[1]) == 0) return WdMain();

	if (argc >= 3) {
		string dll = argv[1];
		DWORD pid = atol(argv[2]);
		if (pid == 0 || ((_stricmp(argv[2], "/n") == 0 ||
						  _stricmp(argv[2], "-n") == 0) && argc >= 4)) {
			string pname = (argc >= 4 ? argv[3] : argv[2]);
			vector<ProcessInfo> ps;
			Process.find(to__str(pname), ps);
			if (ps.empty()) return ERROR_NOT_FOUND;
			HMODULE m = NULL; size_t err_count = 0;
			for (auto& i : ps) {
				HMODULE m = InjectDllToProcess(i, s2wc(dll));
				if (m) {
					printf("[%lu] OK: HMODULE %p\n", i.id(), m);
				} else {
					++err_count;
					DWORD e = GetLastError();
					fprintf(stderr, "[%lu] ERROR: %lu: %s\n", i.id(),
						e, ErrorCodeToStringA(e).c_str());
				}
			}
			if (err_count == 0) {
				printf("Everything is ok");
				return 0;
			}
			else if (err_count == ps.size()) {
				fprintf(stderr, "Everything is wrong");
				return 1;
			}
			else {
				fprintf(stderr, "Something is wrong");
				return 2;
			}
			return 0;
		}
		HMODULE m = InjectDllToProcess(pid, s2wc(dll));
		if (m) {
			printf("OK: HMODULE %p\n", m);
			return 0;
		}
		else {
			DWORD e = GetLastError();
			fprintf(stderr, "ERROR: %lu: %s\n", e, ErrorCodeToStringA(e).c_str());
			return e;
		}

	}
	fprintf(stderr, "Missing arguments\n");
	
	return 0;
}

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK WndProc_FileDragButton(
//	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_HandButton
	(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define myclassname L"Window.class.MYINJECTDLL_WND.main"
int WdMain() {
	if (GetConsoleWindow()) {
		DWORD wpid = 0;
		GetWindowThreadProcessId(GetConsoleWindow(), &wpid);
		if (wpid == GetCurrentProcessId()) FreeConsole();
	}
	s7::MyRegisterClassW(myclassname, WndProc_Main);
#if 0
	{WNDCLASSEXW wcex{ sizeof(WNDCLASSEXW),0 };
	::GetClassInfoExW(NULL, L"Button", &wcex);
	wcex.lpszClassName = L"MyFileDragButton";
	wcex.lpfnWndProc = WndProc_FileDragButton;
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	AssertEx(s7::MyRegisterClassW(NULL, NULL, wcex)); }
#else
	{WNDCLASSEXW wcex{ sizeof(WNDCLASSEXW),0 };
	::GetClassInfoExW(NULL, L"Button", &wcex);
	wcex.lpszClassName = L"MyButton{cursor_hand}";
	//wcex.lpfnWndProc = WndProc_HandButton;
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	AssertEx(s7::MyRegisterClassW(NULL, NULL, wcex)); }
#endif
	LoadLibrary(_T("MSFTEDIT.DLL"));
	InitCommonControls();
	HWND wmain = CreateWindowExW(WS_EX_ACCEPTFILES, myclassname, L"Inject DLL",
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 500, 240, 0, 0, 0, 0);
	if (!wmain) return GetLastError();

	CenterWindow(wmain);
	ShowWindow(wmain, 1);

	MSG msg{ 0 };
	while (GetMessage(&msg, nullptr, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return int(msg.wParam);
}

typedef struct {
	HWND s1, e1, b1, s2, e2, s3, e3, bOK, bCANCEL;
	HFONT font;
	bool closable;
} mywndctrl;
std::map<HWND, mywndctrl> wctls_main;
//#define MYWM_DROPFILETEXT (WM_USER+0x12)
LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
	{
		mywndctrl c{ 0 };
		c.closable = true;

		c.s1 = CreateWindowExW(0, L"Static", L"DLL path: ", WS_CHILD | WS_VISIBLE,
			10, 15+30, 80, 20, hWnd, (HMENU)33, 0, 0);
		c.e1 = CreateWindowExW(0, MSFTEDIT_CLASS, L"", WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL | ES_MULTILINE,
			100, 10, 290, 30*3, hWnd, (HMENU)34, 0, 0);
		c.b1 = CreateWindowExW(/*WS_EX_ACCEPTFILES*/0,
			L"MyButton{cursor_hand}", L"Browse...",
			WS_CHILD | WS_VISIBLE | BS_CENTER, 400, 10, 75, 30, hWnd, (HMENU)35, 0, 0);

		c.s2 = CreateWindowExW(0, L"Static", L"or PID: ", WS_CHILD | WS_VISIBLE,
			10, 50+60+55, 80, 20, hWnd, (HMENU)36, 0, 0);
		c.e2 = CreateWindowExW(0, L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL | ES_NUMBER,
			100, 50+60+55, 200, 20, hWnd, (HMENU)37, 0, 0);

		c.s3 = CreateWindowExW(0, L"Static", L"Image Name: ", WS_CHILD | WS_VISIBLE,
			10, 50+60+5+5, 80, 20, hWnd, (HMENU)38, 0, 0);
		c.e3 = CreateWindowExW(0, MSFTEDIT_CLASS, L"", WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_VSCROLL | ES_MULTILINE,
			100, 50+60+5, 375, 30, hWnd, (HMENU)39, 0, 0);

		c.bCANCEL = CreateWindowExW(0, L"MyButton{cursor_hand}", L"Close", WS_CHILD |
			WS_VISIBLE | BS_CENTER, 400, 140+20, 75, 30, hWnd, (HMENU)IDCANCEL, 0, 0);
		c.bOK = CreateWindowExW(0, L"MyButton{cursor_hand}", L"Inject!", WS_CHILD |
			WS_VISIBLE | BS_CENTER, 310, 140+20, 80, 30, hWnd, (HMENU)IDOK, 0, 0);

		c.font = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		if (c.font) {
			SendMessage(c.s1, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.b1, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.s2, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.e2, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.s3, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.e3, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.bCANCEL, WM_SETFONT, (WPARAM)c.font, 0);
			SendMessage(c.bOK, WM_SETFONT, (WPARAM)c.font, 0);
		}

		wctls_main[hWnd] = c;
	}
		break;

	case WM_DROPFILES: {
		/*POINT pt{ 0 }; GetCursorPos(&pt);
		RECT rc{ 0 }; GetWindowRect(wctls_main[hWnd].b1, &rc);
		if (IsPointInRect(pt, rc)) DragAcceptFiles(hWnd, TRUE);
		else DragAcceptFiles(hWnd, FALSE);*/
		HDROP hDrop = (HDROP)wParam;
		//UINT c_drfile = DragQueryFileW(hDrop, (UINT)-1, NULL, 0);
		//for (int i = 0; i < c_drfile; ++i) {
			wchar_t filepath[MAX_PATH] = { 0 };
			if (DragQueryFile(hDrop, 0, filepath, MAX_PATH) > 0) {
				if (IsFileOrDirectory(filepath) == 1) {
					SendMessage(wctls_main[hWnd].e1,
						WM_SETTEXT, 0, (LPARAM)(INT_PTR)filepath);
				}
			}
		//}
		//MessageBoxW(0, L"", 0, 0);
		break;
	}

	case WM_COMMAND:
	{
		auto wmId = LOWORD(wParam);
		switch (wmId) {
		case IDOK: {
			HANDLE hThread = CreateThread(0, 0, [](PVOID wp)->DWORD {
				HWND w = (HWND)wp;
				auto reenable = [w] {
					wctls_main[w].closable = true;
					EnableMenuItem(GetSystemMenu(w, FALSE), SC_CLOSE, MF_ENABLED);
					EnableWindow(wctls_main[w].b1, TRUE);
					EnableWindow(wctls_main[w].bOK, TRUE);
					//EnableWindow(wctls_main[w].bCANCEL, TRUE);
					SendMessageW(wctls_main[w].bCANCEL, WM_SETTEXT,
						NULL, (LPARAM)L"Close");
					EnableWindow(wctls_main[w].e1, TRUE);
					EnableWindow(wctls_main[w].e2, TRUE);
					EnableWindow(wctls_main[w].e3, TRUE);
					DragAcceptFiles(w, TRUE);
				};
				STARTUPINFO si{ 0 };
				si.cb = sizeof si;
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
				WCHAR dll[MAX_PATH + 1]{ 0 };
				WCHAR pid[32]{ 0 }; WCHAR pn[MAX_PATH + 1]{ 0 };
				SendMessage(wctls_main[w].e1, WM_GETTEXT, MAX_PATH + 1, (LPARAM)dll);
				SendMessage(wctls_main[w].e2, WM_GETTEXT, 31, (LPARAM)pid);
				SendMessage(wctls_main[w].e3, WM_GETTEXT, MAX_PATH + 1, (LPARAM)pn);
				if (pid[0] == 0 && pn[0] == 0) {
					MessageBoxW(0, L"Invalid input!! You must input "
						"[Image Name] or [PID].", L"ERROR", MB_ICONERROR);
					reenable();
					return 0;
				}
				string cml = "\"" + GetProgramDir() + "\" \"" + ws2s(dll) + "\"";
				if (pid[0] != 0) cml += " " + ws2s(pid);
				else if (pn[0] != 0) cml += " -n \"" + ws2s(pn) + "\"";
				auto pi = Process.Start_Suspended(to__str(cml), si);
				if (!pi.hProcess) {
					MessageBoxW(0, (L"Cannot CreateProcess: " +
						to_wstring(GetLastError()) + L": " +
						LastErrorStrW()).c_str(), NULL, MB_ICONERROR);
					reenable();
					return 0;
				}
				DWORD code = 1;
				CloseHandle(pi.hThread);
				Process.resume(pi.hProcess);
				WaitForSingleObject(pi.hProcess, INFINITE);
				GetExitCodeProcess(pi.hProcess, &code);
				CloseHandle(pi.hProcess);
				if (code) {
					MessageBoxW(0, (L"ERROR: error code " +
						to_wstring(code)).c_str(), NULL, MB_ICONERROR);
					reenable();
					return 0;
				}
				reenable();
				return 0;
			}, hWnd, 0, 0);
			if (hThread) {
				CloseHandle(hThread);
				wctls_main[hWnd].closable = false;
				EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE,
					MF_DISABLED | MF_GRAYED);
				EnableWindow(wctls_main[hWnd].b1, FALSE);
				EnableWindow(wctls_main[hWnd].bOK, FALSE);
				//EnableWindow(wctls_main[hWnd].bCANCEL, FALSE);
				SendMessageW(wctls_main[hWnd].bCANCEL, WM_SETTEXT,
					NULL, (LPARAM)L"Cancel");
				EnableWindow(wctls_main[hWnd].e1, FALSE);
				EnableWindow(wctls_main[hWnd].e2, FALSE);
				EnableWindow(wctls_main[hWnd].e3, FALSE);
				DragAcceptFiles(hWnd, FALSE);
			} else {
				MessageBoxW(hWnd, L"Cannot create thread", 0, MB_ICONHAND);
			}
		} break; 
		case IDCANCEL:
			if (!wctls_main[hWnd].closable) {
				if (MessageBoxW(hWnd, L"Are you sure you want to cancel?",
					L"Cancel Inject", MB_ICONWARNING | MB_OKCANCEL) == IDOK) {
					DestroyWindow(hWnd);
				}
				break;
			}
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case 35: {
			WCHAR szFile[MAX_PATH + 1]{ 0 };
			OPENFILENAMEW ofn{ 0 };
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"Dynamic Link Library (*.dll)\0*.dll\0All Files\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = L"Choose a DLL";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (!GetOpenFileNameW(&ofn)) {
				return false;
			}
			SendMessage(wctls_main[hWnd].e1, WM_SETTEXT, 0, (LPARAM)(INT_PTR)szFile);
		}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}

	case WM_CLOSE:
		if (!(wctls_main[hWnd].closable)) {
			return 0;
		}
	case WM_ENDSESSION:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		if (wctls_main[hWnd].font) DeleteObject(wctls_main[hWnd].font);
		try { wctls_main.erase(hWnd); }
		catch (std::exception&) {};
		PostQuitMessage(0);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#if 0
LRESULT WndProc_FileDragButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//if (message == WM_DROPFILES) {
	//	HWND hParent = GetParent(hWnd);
	//	if (hParent) return SendMessage(hWnd, message, wParam, lParam);
	//	return 0;
	//}
	static WNDPROC defwp = NULL;
	if (defwp == NULL) {
		HWND tmp = CreateWindowExW(0, L"Button", L"", 0, 0, 0, 0, 0, 0, 0, 0, 0);
		defwp = (WNDPROC)GetClassLongPtrW(tmp, /*GCL_WNDPROC*/(-24));
		DestroyWindow(tmp);
	}
	if (defwp) return defwp(hWnd, message, wParam, lParam);
	return 0;
}
#endif

LRESULT CALLBACK WndProc_HandButton(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static WNDPROC defwp = NULL;
	if (defwp == NULL) {
		HWND tmp = CreateWindowExW(0, L"Button", L"", 0, 0, 0, 0, 0, 0, 0, 0, 0);
		defwp = (WNDPROC)GetClassLongPtrW(tmp, /*GCL_WNDPROC*/(-24));
		DestroyWindow(tmp);
	}
	if (defwp) return defwp(hWnd, message, wParam, lParam);
	return 0;
}

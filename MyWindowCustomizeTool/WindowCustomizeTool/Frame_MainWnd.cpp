#include "Frame_MainWnd.h"
#include "resource.h"
#include <Richedit.h>
using namespace std;

static WCHAR szWindowClass[64] = { 0 };
static WCHAR szTitle[256] = { 0 };
static WCHAR autorun_keyname[256];
static Frame_MainWnd* wMainWindow;
#define ThisInst (GetModuleHandle(NULL))

static LRESULT WndProc_WindowFindDlg
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Frame_MainWnd::Frame_MainWnd() {
	ZeroMemory(this, sizeof(Frame_MainWnd));
}

Frame_MainWnd::~Frame_MainWnd() {
	if (IsWindow(hWnd)) DestroyWindow(hWnd);
	if (pnid) free(pnid);
}

void Frame_MainWnd::LoadGlobalString(HINSTANCE src) {
	src ? 0 : src = GetModuleHandle(NULL);
	::LoadStringW(src, IDS_STRING_UI_WNDCLASS, szWindowClass, 64);
	::LoadStringW(src, IDS_STRING_APP_TITLE, szTitle, 256);
	::LoadStringW(src, IDS_STRING_APP_REG_KEYNAME, autorun_keyname, 256);
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM Frame_MainWnd::MyRegisterClass() {
#pragma warning(push)
#pragma warning(disable: 4302)
	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = ThisInst;
	wcex.hIcon          = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
//	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	constexpr auto _RGBV = 0xfa;
	wcex.hbrBackground  = CreateSolidBrush(RGB(_RGBV, _RGBV, _RGBV));
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU_MAINWND);
//	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance,
						  MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
//	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);

#pragma warning(pop)
}

LRESULT Frame_MainWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
	{

		SendMessage(hWnd, WM_USER + 20, 14, 0);
		SendMessage(hWnd, WM_USER + 20, 10, 0);

		if (!wMainWindow->nCmdShow) {
			break;
		}
		LONG wStyle = 0;
#if 0

		//wStyle = ::GetWindowLongW(hWnd, GWL_STYLE);
		//wStyle &= ~WS_CAPTION;
		//wStyle &= ~WS_SYSMENU;
		//wStyle &= ~WS_SIZEBOX;
		//wStyle &= ~WS_MINIMIZE;
		//wStyle &= ~WS_MINIMIZEBOX;
		//wStyle &= ~WS_MAXIMIZE;
		//wStyle &= ~WS_MAXIMIZEBOX;
		//::SetWindowLongW(hWnd, GWL_STYLE, wStyle);
#endif

		wStyle = ::GetWindowLongW(hWnd, GWL_EXSTYLE);
		wStyle |= WS_EX_LAYERED;
		::SetWindowLongW(hWnd, GWL_EXSTYLE, wStyle);


		HMENU sysMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenuW(sysMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(sysMenu, MF_STRING, WM_USER + 17, L"Alpha&+");
		AppendMenuW(sysMenu, MF_STRING, WM_USER + 18, L"Alpha&-");
		AppendMenuW(sysMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(sysMenu, MF_STRING, WM_DESTROY, L"&Exit");

		wMainWindow->CreateControls(hWnd, ThisInst);

		SetLayeredWindowAttributes(hWnd, 0, (BYTE)0xee, LWA_ALPHA);
		UpdateWindow(hWnd);
		
	}
		break;
	case WM_USER+17:
	{
		BYTE alpha = 0; DWORD dw = LWA_ALPHA;
		GetLayeredWindowAttributes(hWnd, 0, &alpha, &dw);
		if (lParam == 1) alpha += 10;
		else if (lParam == 2) alpha -= 10;
		else if (lParam == 3) alpha ++;
		else if (lParam == 4) alpha --;
		SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
	}
		break;
	case (WM_USER+13): // Icon
		if (lParam == WM_LBUTTONUP/* || lParam == WM_LBUTTONDBLCLK*/) {
#if 0
			WCHAR szSubWindowClass[37] = { 0 };
			LoadStringW(ThisInst, IDS_STRING_UI_SUBWNDCLASS, szSubWindowClass, 37);
			HWND hMainWnd = FindWindowW(szSubWindowClass, NULL);
			if (hMainWnd) {
				ShowWindow(hMainWnd, 9);
				SetForegroundWindow(hMainWnd);
			}
			else RunUIProcess();
#else
			if (wMainWindow->nCmdShow) {
				ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd);
			} else {
				ShellExecuteW(hWnd, L"open", s2wc(GetProgramDir()),
					L"--no-icon", NULL, SW_NORMAL);
			}
#endif
			break;
		}
		if (lParam == WM_MBUTTONUP) {
			ShellExecuteW(hWnd, L"open", s2wc(GetProgramDir()),
				wMainWindow->nCmdShow ? L"--new-window" : L"--no-icon", NULL, SW_NORMAL);
			break;
		}
		if (lParam == WM_RBUTTONUP) {
			POINT pt = { 0 }; int resp = 0;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);

			if (wMainWindow->nCmdShow == SW_HIDE) {
				constexpr size_t IDR_OPEN = 1;
				constexpr size_t IDR_RUNAS = 2;
				constexpr size_t IDR_EXIT = 3;
				constexpr size_t IDR_EXITALL = 4;
				static HMENU hIconMenu = NULL;
				if (hIconMenu == NULL) {
					hIconMenu = CreatePopupMenu();
					if (hIconMenu == 0) {
						MessageBoxA(NULL, ("Exception! at WndProc\nError " +
							std::to_string(GetLastError()) + ": " + LastErrorStrA() +
							"\nPress OK to terminate application and exit.").c_str(),
							"Fatal Error", MB_ICONERROR);
						ExitProcess(GetLastError());
					}
					AppendMenu(hIconMenu, MF_STRING, IDR_OPEN, _T("&Open"));
					AppendMenu(hIconMenu, MF_STRING, IDR_RUNAS, _T("&runas"));
					AppendMenu(hIconMenu, MF_STRING, IDR_EXIT, _T("&Exit"));
					AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
					AppendMenu(hIconMenu, MF_STRING, IDR_EXITALL, _T("Exit &All"));
				}
				resp = TrackPopupMenu(hIconMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
					pt.x, pt.y, NULL, hWnd, NULL);
				if (resp == IDR_OPEN) {
					SendMessage(hWnd, WM_USER + 13, wParam, WM_LBUTTONUP);
				} else
				if (resp == IDR_RUNAS) {
					ShellExecuteW(hWnd, L"runas", s2wc(GetProgramDir()),
						L"--no-icon", NULL, 1);
				} else
				if (resp == IDR_EXIT) {
					SendMessage(hWnd, WM_USER + 4, 0, 0);
				} else
				if (resp == IDR_EXITALL) {
					ShellExecuteW(hWnd, L"open", s2wc(GetProgramDir()),
						L"--user-exit", NULL, 1);
					SendMessage(hWnd, WM_USER + 4, 0, 0);
				}
				break;
			}

			constexpr size_t IDR_SHOW = 1;
			constexpr size_t IDR_EXIT = 2;
			constexpr size_t IDR_HIDEICON = 3;
			constexpr size_t IDR_EXITALL = 4;
			static HMENU hIconMenu = NULL;
			if (hIconMenu == NULL) {
				hIconMenu = CreatePopupMenu();
				if (hIconMenu == 0) {
					MessageBoxA(NULL, ("Exception! at WndProc\nError " +
						std::to_string(GetLastError()) + ": " + LastErrorStrA() +
						"\nPress OK to terminate application and exit.").c_str(),
						"Fatal Error", MB_ICONERROR);
					ExitProcess(GetLastError());
				}
				AppendMenu(hIconMenu, MF_STRING, IDR_SHOW, _T("&Show"));
				AppendMenu(hIconMenu, MF_STRING, IDR_HIDEICON, _T("Hide Icon"));
				AppendMenu(hIconMenu, MF_STRING, IDR_EXIT, _T("&Exit"));
				AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
				AppendMenu(hIconMenu, MF_STRING, IDR_EXITALL, _T("Exit &All"));
			}
			resp = TrackPopupMenu(hIconMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
				pt.x, pt.y, NULL, hWnd, NULL);
			if (resp == IDR_SHOW){
				SendMessage(hWnd, WM_USER + 13, wParam, WM_LBUTTONUP);
			}
			if (resp == IDR_EXIT){
				DestroyWindow(hWnd);
			}
			if (resp == IDR_HIDEICON){
				SendMessage(hWnd, WM_USER + 20, 10, 1);
			}
			if (resp == IDR_EXITALL) {
				ShellExecuteW(hWnd, L"open", s2wc(GetProgramDir()),
					L"--user-exit", NULL, 1);
				SendMessage(hWnd, WM_USER + 4, 0, 0);
			}
			break;
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == WM_DESTROY) return DestroyWindow(hWnd) && false;
		if (wParam == WM_USER + 17) return SendMessageW(hWnd, WM_USER + 17, 0, 1);
		if (wParam == WM_USER + 18) return SendMessageW(hWnd, WM_USER + 17, 0, 2);
		
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
	{
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		//if (wmId) MessageBoxW(NULL, to_wstring(wmId).c_str(), L"Test", 0);
		switch (wmId) {
		case ID_MENU_FILE_CLOSE:
			SendMessageW(hWnd, WM_USER + 4, 2, 0);
			break;
		case ID_MENU_FILE_EXIT:
			SendMessageW(hWnd, WM_USER + 4, 0, 0);
			break;
		case ID_MENU_OPTIONS_TASKBARICON:
		{
			HMENU hMenu = GetMenu(hWnd);
			hMenu = GetSubMenu(hMenu, 2);
			if (!hMenu) break;
			//判断是否有复选标记，如果有就取消复选标记，没有就添加复选标记
			if (CheckMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_CHECKED) & MF_CHECKED)
			{ 
				SendMessage(hWnd, WM_USER + 20, 10, 1);
				CheckMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_UNCHECKED);
			} else {
				SendMessage(hWnd, WM_USER + 20, 10, 0);
			}
		}
			break;
		case ID_MENU_HELP_ABOUT:
			DialogBox(ThisInst, MAKEINTRESOURCE(IDD_ABOUTBOX),
				hWnd, WndProc_WindowFindDlg);
			EnableWindow(hWnd, TRUE);
			SetForegroundWindow(hWnd);
			break;
		case ID_MENU_WINDOW_FIND:
		{
			HWND targetHwnd = (HWND)DialogBoxW(ThisInst, MAKEINTRESOURCEW
			(IDD_DIALOG_WINDOWFINDER1), hWnd, WndProc_WindowFindDlg);
			//if (wMainWindow->targetHwnd == INVALID_HANDLE_VALUE) {
			//	MessageBoxW(hWnd, LastErrorStrW(), L"ERROR", MB_ICONERROR);
			//}
			EnableWindow(hWnd, TRUE);
			SetForegroundWindow(hWnd);
			if (targetHwnd == NULL) break;
			wMainWindow->targetHwnd = targetHwnd;
			wMainWindow->UpdateHwndInfo();
		}
			break;
		case ID_MENU_OPTIONS_ALWAYSONTOP:
		{
			HMENU hMenu = GetMenu(hWnd);
			hMenu = GetSubMenu(hMenu, 2);
			if (!hMenu) break;
			if (CheckMenuItem(hMenu, ID_MENU_OPTIONS_ALWAYSONTOP, MF_CHECKED) & MF_CHECKED)
			{ 
				CheckMenuItem(hMenu, ID_MENU_OPTIONS_ALWAYSONTOP, MF_UNCHECKED);
				SendMessage(hWnd, WM_USER + 20, 11, 0);
			} else {
				SendMessage(hWnd, WM_USER + 20, 11, 1);
			}
		}
			break;
		case ID_MENU_OPTIONS_HIDEWHENMINIMID:
		{
			HMENU hMenu = GetMenu(hWnd);
			hMenu = GetSubMenu(hMenu, 2);
			if (!hMenu) break;
			if (CheckMenuItem(hMenu, ID_MENU_OPTIONS_HIDEWHENMINIMID,
				MF_CHECKED) & MF_CHECKED) {
				CheckMenuItem(hMenu, ID_MENU_OPTIONS_HIDEWHENMINIMID, MF_UNCHECKED);
				SendMessage(hWnd, WM_USER + 20, 12, 0);
				EnableMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_ENABLED);
			} else {
				SendMessage(hWnd, WM_USER + 20, 12, 1);
				EnableMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_GRAYED);
				CheckMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_CHECKED);
				SendMessage(hWnd, WM_USER + 20, 10, 0);
			}
		}
			break;
		case ID_MENU_ALPHA_ADD10:
			SendMessage(hWnd, WM_USER + 17, 0, 1);
			break;
		case ID_MENU_ALPHA_SUB10:
			SendMessage(hWnd, WM_USER + 17, 0, 2);
			break;
		case ID_MENU_ALPHA_ADD:
			SendMessage(hWnd, WM_USER + 17, 0, 3);
			break;
		case ID_MENU_ALPHA_SUB:
			SendMessage(hWnd, WM_USER + 17, 0, 4);
			break;
		case ID_MENU_WINDOWMANAGER_RELOAD:
			SendMessage(hWnd, WM_USER + 20, 13, 0);
			break;
		case 0x600+8:  // Apply title
			if (wmEvent == BN_CLICKED) {
				WCHAR tit[2048] = { 0 };
				GetWindowTextW(wMainWindow->wEditWTitle, tit, 2048);
				SetWindowTextW(wMainWindow->targetHwnd, tit);
			}
			break;
		case ID_MENU_PROCESS_TERMINATE:
		{
			DWORD pid = 0;
			GetWindowThreadProcessId(wMainWindow->targetHwnd, &pid);
			if (pid) if (!Process.kill(pid, PROCESS_TERMINATE))
				MessageBoxW(hWnd, (L"Cannot Terminate Process " + to_wstring(pid) +
					L" because " + LastErrorStrW() + L" (" + to_wstring(GetLastError())
					+ L")").c_str(), L"Error - TerminateProcess", MB_ICONHAND);
			wMainWindow->UpdateHwndInfo();
		}
			break;
		case ID_MENU_PROCESS_OPENLOCATION:
		{
			DWORD pid = 0;
			GetWindowThreadProcessId(wMainWindow->targetHwnd, &pid);
			if (!pid) break;
			ShellExecute(NULL, _T("open"), _T("explorer"),
				(L"/select,\"" + Process.GetProcessFullPathById(pid) + L"\"").c_str(),
				NULL, SW_NORMAL);
		}
			break;
		case ID_MENU_PROCESS_KILLANDDELETE:
		{
			DWORD pid = 0;
			GetWindowThreadProcessId(wMainWindow->targetHwnd, &pid);
			if (!pid) break;
			if (MessageBoxW(hWnd, (L"Are you sure you want to delete " + Process.
				GetProcessFullPathById(pid) + L"?\nThis operation CANNOT be undo!!")
				.c_str(), L"Delete File", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2)
				!= IDYES) break;
			HANDLE hObj = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			TSTRING full_path = Process.GetProcessFullPathById(pid);
			if (hObj && !TerminateProcess(hObj, PROCESS_TERMINATE)) {
				if (hObj) CloseHandle(hObj);
				MessageBoxW(hWnd, (L"Cannot Terminate Process " + to_wstring(pid) +
					L" because " + LastErrorStrW() + L" (" + to_wstring(GetLastError())
					+ L")").c_str(), L"Error - TerminateProcess", MB_ICONHAND); break;
			}
			else if (hObj) WaitForSingleObject(hObj, MAXDWORD);
			else {
				MessageBoxW(hWnd, (L"Cannot Open Process " + to_wstring(pid) +
					L" because " + LastErrorStrW() + L" (" + to_wstring(GetLastError())
					+ L")").c_str(), L"Error - OpenProcess", MB_ICONHAND); break;
			}
			CloseHandle(hObj);
			wMainWindow->UpdateHwndInfo();
#if 0
			SHFILEOPSTRUCTW sh; AutoZeroMemory(sh);
			sh.hwnd = hWnd;
			sh.wFunc = FO_DELETE;
			TSTRING str2 = full_path + _T('\0');
			sh.pFrom = str2.c_str();
			sh.fFlags = 0;
			SHFileOperationW(&sh);
#else
			if (!DeleteFile(full_path.c_str())) {
				MessageBoxW(hWnd, (L"Cannot Delete File " + Process.GetProcessFullPathById
				(pid) +	L" because " + LastErrorStrW() + L" (" + to_wstring(GetLastError())
					+ L")").c_str(), L"Error - DeleteFile", MB_ICONHAND); break;
			}
#endif
		}
			break;
		case ID_MENU_WINDOWMANAGER_CLOSE:
			SendMessage(wMainWindow->targetHwnd, WM_CLOSE, 0, 0);
			break;
		case ID_MENU_WINDOWMANAGER_DESTROY:
			DestroyWindow(wMainWindow->targetHwnd);
			break;
		case ID_MENU_WINDOWMANAGER_ENDTASK:
		{
			typedef BOOL(WINAPI* EndTask_t)(HWND hWnd, BOOL fShutDown, BOOL fForce);
			HMODULE user32 = GetModuleHandleA("user32.dll");
			if (user32) {
				EndTask_t EndTask = (EndTask_t)GetProcAddress(user32, "EndTask");
				if (EndTask) {
					EndTask(wMainWindow->targetHwnd, FALSE, FALSE);
				}
			}
		}
			break;
		case ID_MENU_OPTIONS_RUNATLOGON:
		{
			HMENU hMenu = GetMenu(hWnd);
			hMenu = GetSubMenu(hMenu, 2);
			if (!hMenu) break;
			if (CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON,
				MF_CHECKED) & MF_CHECKED) {
				if (wMainWindow->autorun_type & 0x10) { // System
					SendMessage(hWnd, WM_USER + 20, 15, 2);
				}
				if ((wMainWindow->autorun_type & 0x1) &&
					(!(wMainWindow->autorun_type & 0x10))) { // User
					SendMessage(hWnd, WM_USER + 20, 15, 1);
				}
			} else { // install
				SendMessage(hWnd, WM_USER + 20, 15, 0);
			}
		}
			break;
		case ID_MENU_WINDOWMANAGER_BTF:
			BringWindowToTop(wMainWindow->targetHwnd);
			break;
		case ID_MENU_WINDOWMANAGER_SWITCHTO:
			SwitchToThisWindow(wMainWindow->targetHwnd, FALSE);
			break;
		case ID_MENU_WINDOWMANAGER_MINIMIZE:
			ShowWindow(wMainWindow->targetHwnd, SW_MINIMIZE);
			break;
		case ID_MENU_WINDOWMANAGER_MAXIMIZE:
			ShowWindow(wMainWindow->targetHwnd, SW_MAXIMIZE);
			break;
		case ID_MENU_WINDOWMANAGER_HIDE:
			ShowWindow(wMainWindow->targetHwnd, SW_HIDE);
			break;
		case ID_MENU_WINDOWMANAGER_SHOW:
			ShowWindow(wMainWindow->targetHwnd, SW_NORMAL);
			break;
		case ID_MENU_ABOUT:
		{
			WCHAR appname[256] = { 0 };
			LoadStringW(ThisInst, IDS_STRING_APP_TITLE, appname, 255);
			ShellAboutW(hWnd, appname, L"License: MIT License\r\nProject Location: "
				"https://github.com/shc0743/CLearn/ \r\n\r\n ", NULL);
		}
			break;
		case ID_MENU_WINDOWMANAGER_RESIZE:
			if (IsWindow(wMainWindow->targetHwnd)) {
				RECT rc{ 0 };
				GetWindowRect(wMainWindow->targetHwnd, &rc);
				ClassRegister_resizer();
				WCHAR cname[256] = { 0 };
				LoadStringW(ThisInst, IDS_STRING_UI_RESIZERCLASS, cname, 255);
				wMainWindow->_tmp_resizer = CreateWindowExW(0, cname,
					L"Window Resizer",
					WS_SYSMENU | WS_SIZEBOX, rc.left, rc.top,
					rc.right - rc.left, rc.bottom - rc.top,
					NULL, 0, 0, 0);
				if (wMainWindow->_tmp_resizer) {
					EnableWindow(hWnd, FALSE);
					ShowWindow(wMainWindow->_tmp_resizer, SW_NORMAL);
				}
			}
			break;
		case ID_MENU_WINDOWMANAGER_SWP:
			DialogBox(ThisInst, MAKEINTRESOURCE(IDD_DIALOG_SWP_ARGS),
				hWnd, WndProc_swpdlg);
			EnableWindow(hWnd, TRUE);
			SetForegroundWindow(hWnd);
			break;
		case ID_MENU_PROCESS_IMAGEPATH: {
			HMENU mnp = CreateMenu();
			if (!mnp) break;
			HMENU mn = CreateMenu();
			if (!mn) { DestroyMenu(mnp); break; }
			AppendMenu(mnp, MF_POPUP, (UINT_PTR)mn, 0);
			DWORD pid = 0; GetWindowThreadProcessId(wMainWindow->targetHwnd, &pid);
			TSTRING pname = Process.GetProcessFullPathById(pid);
			AppendMenu(mn, MF_STRING, 2, pname.c_str());
			AppendMenu(mn, MF_SEPARATOR, 0, 0);
			AppendMenuW(mn, MF_STRING, 3, L"&Close");
			POINT pt{ 0 }; GetCursorPos(&pt);
			int result = TrackPopupMenu(mn, TPM_RIGHTBUTTON | TPM_RETURNCMD,
				pt.x, pt.y, 0, hWnd, 0);
			if (result == 2) ShellExecute(NULL, _T("open"), _T("explorer"),
				(L"/select,\"" + pname + L"\"").c_str(), NULL, SW_NORMAL);
			DestroyMenu(mn); DestroyMenu(mnp);
		}
			break;
		case ID_MENU_WINDOWMANAGER_HIGHLIGHT:
			if (IsWindow(wMainWindow->targetHwnd)) {
				RECT rc{ 0 };
				GetWindowRect(wMainWindow->targetHwnd, &rc);
				ClassRegister_resizer();
				WCHAR cname[256] = { 0 };
				LoadStringW(ThisInst, IDS_STRING_UI_BACKGCLASS, cname, 255);
				ClassRegister_selector_background();
				wMainWindow->hightlighting = true;
				HWND hWnd2 = CreateWindowExW(0, cname,
					L"Window Shower", WS_SYSMENU | WS_VISIBLE, rc.left, rc.top,
					rc.right - rc.left, rc.bottom - rc.top,
					NULL, 0, 0, 0);
				if (!hWnd2) {
					MessageBoxW(hWnd, LastErrorStrW().c_str(), L"ERROR", MB_ICONERROR);
				} else {
					SetTimer(hWnd2, 4, 2000, NULL);
				}
				wMainWindow->hightlighting = false;
			}
			break;
		case 0x600+12 + 0x24:
			if (IsWindow(wMainWindow->targetHwnd)) {
				RECT rc; AutoZeroMemory(rc);
				GetWindowRect(wMainWindow->wButtonWinOpt, &rc);  //client坐标系
		
				HMENU mn = GetMenu(hWnd);
				if (!mn) break;
				mn = GetSubMenu(mn, 4);
				if (!mn) break;
				mn = GetSubMenu(mn, 4);
				if (!mn) break;
				int ret = TrackPopupMenu(mn, TPM_RIGHTBUTTON | TPM_RETURNCMD,
					rc.left, rc.bottom, 0, hWnd, NULL);
				if (ret) SendMessage(hWnd, WM_COMMAND, ret, 0);
			}
			break;
		case 0x600+13 + 0x27:
			if (wmEvent == BN_CLICKED) {
				BOOL enable = (SendMessage(wMainWindow->wCheckEnableWin,
					BM_GETCHECK, 0, 0) == BST_CHECKED);
				EnableWindow(wMainWindow->targetHwnd, enable);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_USER+20: // Menu Command Handler
		switch (wParam) {
		case 10: // Hide or Show Icon
			if (wMainWindow->attributes.noIcon) {
				HMENU hMenu = GetMenu(hWnd);
				hMenu = GetSubMenu(hMenu, 2);
				if (hMenu) {
					EnableMenuItem(hMenu, ID_MENU_OPTIONS_HIDEWHENMINIMID, MF_GRAYED);
					EnableMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_GRAYED);
					CheckMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_UNCHECKED);
				}
				break;
			}
			if (lParam) {
				Shell_NotifyIcon(NIM_DELETE, wMainWindow->pnid);
				free(wMainWindow->pnid);
				wMainWindow->pnid = NULL;
				ShowWindow(hWnd, SW_RESTORE);
				{
					HMENU hMenu = GetMenu(hWnd);
					hMenu = GetSubMenu(hMenu, 2);
					if (hMenu) {
						CheckMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_UNCHECKED);
						CheckMenuItem(hMenu,
							ID_MENU_OPTIONS_HIDEWHENMINIMID, MF_UNCHECKED);
						SendMessage(hWnd, WM_USER + 20, 12, 0);
						EnableMenuItem(hMenu, ID_MENU_OPTIONS_TASKBARICON, MF_ENABLED);
					}
				}
			} else if (wMainWindow->pnid == NULL) {
				wMainWindow->pnid = (decltype(pnid))calloc(1, sizeof(*pnid));
				if (wMainWindow->pnid == NULL) break;
				wMainWindow->pnid->cbSize = sizeof(NOTIFYICONDATA);
				wMainWindow->pnid->hWnd = hWnd;
				wMainWindow->pnid->uID = 0;
				wMainWindow->pnid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
				wMainWindow->pnid->uCallbackMessage = WM_USER + 13;
				if (wMainWindow->nCmdShow == SW_HIDE)
					wMainWindow->pnid->hIcon = LoadIcon(ThisInst,
						MAKEINTRESOURCEW(IDI_ICON_WINDOWFINDER_0));
				else wMainWindow->pnid->hIcon = LoadIcon(ThisInst,
					MAKEINTRESOURCEW(IDI_ICON_WINDOWFINDER_1));

				//wcscpy_s(wMainWindow->pnid->szInfo, L"");
				//wcscpy_s(wMainWindow->pnid->szInfoTitle, L"");
				WCHAR wcs_szTip[256] = { 0 };
				LoadStringW(ThisInst, IDS_STRING_UI_TSKICONTEXT, wcs_szTip, 256);
				wcscpy_s(wMainWindow->pnid->szTip, wcs_szTip);

				Shell_NotifyIconW(NIM_ADD, wMainWindow->pnid);
			}
			break;
		case 11: // Always on top
			if (lParam) SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, 0x2 | 0x1);
			else SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			break;
		case 12: // Hide when min
			wMainWindow->hide_when_min = lParam;
			break;
		case 13: // Reload 
			wMainWindow->UpdateHwndInfo();
			break;
		case 14: // Query autorun
		{
			HKEY hkAutorun = NULL; DWORD rsz = REG_SZ;
			RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\Current"
				"Version\\Run", 0, KEY_READ, &hkAutorun);
			if (hkAutorun) {
				if (!RegQueryValueExW(hkAutorun, autorun_keyname, NULL,
					&rsz, NULL, NULL)) {
					wMainWindow->autorun_type |= 0x10;
				}
				RegCloseKey(hkAutorun);
			}
			RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\Current"
				"Version\\Run", 0, KEY_READ, &hkAutorun);
			if (hkAutorun) {
				if (!RegQueryValueExW(hkAutorun, autorun_keyname, NULL,
					&rsz, NULL, NULL)) {
					wMainWindow->autorun_type |= 0x1;
				}
				RegCloseKey(hkAutorun);
			}
			if (wMainWindow->autorun_type) {
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), 2),
					ID_MENU_OPTIONS_RUNATLOGON, MF_CHECKED);
			}
		}
			break;
		case 15: // autorun manage
		{
			HMENU hMenu = GetMenu(hWnd);
			hMenu = GetSubMenu(hMenu, 2);
			//if (lParam == 0) { // install
			//	if ((LRESULT)(INT_PTR)ShellExecuteW(hWnd, L"open", s2wc(GetProgramDir()),
			//		L"--enable-run-at-logon", NULL, SW_HIDE) < 32) {
			//		CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, MF_CHECKED);
			//	}
			//	else CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, MF_UNCHECKED);
			//}
			SHELLEXECUTEINFOW si;
			memset(&si, 0, sizeof(si));
			std::wstring pd = s2ws(GetProgramDir());
			si.lpFile = pd.c_str();
			si.cbSize = sizeof(si);
			si.nShow = SW_SHOWDEFAULT;
			si.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
			si.lpVerb = ((lParam == 0 || lParam == 1) ?
				L"open" : (lParam == 2 ? L"runas" : L""));
			if (si.lpVerb[0] == L'\0') return ERROR_INVALID_PARAMETER;
			si.lpParameters = 
				(lParam == 0 ? L"--enable-run-at-logon" : L"--disable-run-at-logon");
			if (ShellExecuteExW(&si)) {
				EnableWindow(hWnd, FALSE);
				WaitForSingleObject(si.hProcess, MAXDWORD);
				DWORD ec = 0;
				GetExitCodeProcess(si.hProcess, &ec);
				if (ec == 0 && hMenu) {
					if (lParam == 0) CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, 0x8);
					else CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, MF_UNCHECKED);
				} else if (hMenu) {
					if (lParam == 0) CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, 0x0);
					else CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, MF_CHECKED);
					MessageBoxW(hWnd, ErrorCodeToStringW(ec).c_str(),
						L"ERROR", MB_ICONHAND);
				}
				EnableWindow(hWnd, TRUE);
			} else {
				if (lParam == 0) CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, 0x0);
				else CheckMenuItem(hMenu, ID_MENU_OPTIONS_RUNATLOGON, MF_CHECKED);
			}
			//SendMessage(hWnd, WM_USER + 20, 14, 0);
		}
			break;
		default:;
		}
		break;
	case WM_CONTEXTMENU:
	{
		POINT pt; GetCursorPos(&pt);
		//ScreenToClient(hWnd, &pt);
		//printf("[DEBUG] %ld %ld\n", pt.x, pt.y);
		RECT rc; AutoZeroMemory(rc);
		GetWindowRect(wMainWindow->wStatic1, &rc);  //client坐标系
		RECT rc2{ 0 };
		GetWindowRect(wMainWindow->wButtonWinOpt, &rc2);  //client坐标系
		if (((pt.x > rc.left && pt.x < rc.right
			&& pt.y> rc.top && pt.y < rc.bottom) ||
			(pt.x > rc2.left && pt.x < rc2.right
			&& pt.y> rc2.top && pt.y < rc2.bottom)
			) && IsWindow(wMainWindow->targetHwnd)) {
			HMENU mn = GetMenu(hWnd);
			if (!mn) break;
			mn = GetSubMenu(mn, 4);
			if (!mn) break;
			mn = GetSubMenu(mn, 4);
			if (!mn) break;
			int ret = TrackPopupMenu(mn, TPM_RIGHTBUTTON | TPM_RETURNCMD,
				pt.x, pt.y, 0, hWnd, NULL);
			if (ret) SendMessage(hWnd, WM_COMMAND, ret, 0);
			break;
		}
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		{
			POINT pt; GetCursorPos(&pt);
			RECT rect; GetWindowRect(wMainWindow->wIconWSelector, &rect);
			if (pt.x > rect.left && pt.x < rect.right
				&& pt.y> rect.top && pt.y < rect.bottom) {
				wMainWindow->m_lBtnDowned = true;
				SetCapture(hWnd);	//鼠标捕获
				HCURSOR hc = LoadCursor(ThisInst,
					MAKEINTRESOURCE(IDC_CURSOR_WINDOWFINDER));
				//IDC_CURSOR1是靶形光标资源号
				::SetCursor(hc);
				HICON hicon2 = LoadIcon(ThisInst,
					MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
				//IDI_ICON2为无靶图标资源号
				auto oldimage = (HBITMAP)SendMessage(wMainWindow->wIconWSelector,
					STM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon2);
				if (oldimage != NULL) {
					DeleteObject(oldimage);
				}
				DeleteObject(hicon2);
				//wMainWindow->nTimerId_WindowSelector =
				//	SetTimer(NULL, 0x41, 100, TimerProc_WindowSelect);
				break;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		if (wMainWindow->m_lBtnDowned == true) {
			POINT pnt; RECT rc;
#if 0
			//HWND DeskHwnd = ::GetDesktopWindow(); //取得桌面句柄
			//HDC DeskDC = ::GetWindowDC(hWnd/*DeskHwnd*/); //取得桌面设备场景
			//int oldRop2 = SetROP2(DeskDC, R2_NOTXORPEN);
#endif
			::GetCursorPos(&pnt); //取得鼠标坐标
			HWND UnHwnd = ::WindowFromPoint(pnt); //取得鼠标指针处窗口句柄
			if (UnHwnd == wMainWindow->hWnd) { break; }
			wMainWindow->_tmp_tarhw = UnHwnd;
			::GetWindowRect(UnHwnd, &rc); //获得窗口矩形

			//if (!wMainWindow->_tmp_select_targ) {
			//	ClassRegister_selector_background();
			//	WCHAR szWindowClass[256] = { 0 };
			//	LoadStringW(ThisInst, IDS_STRING_UI_BACKGCLASS, szWindowClass, 255);
			//	wMainWindow->_tmp_select_targ = CreateWindowExW(
			//		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, szWindowClass,
			//		L"Window Selector", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, 0,
			//		ThisInst, 0);
			//	if (!wMainWindow->_tmp_select_targ) {
			//		MessageBoxW(hWnd, LastErrorStrW().c_str(), L"ERROR", MB_ICONHAND);
			//		break;
			//	}
			//}
			//ShowWindow(wMainWindow->_tmp_select_targ, SW_HIDE);

#if 0
			//if (rc.left < 0) rc.left = 0;
			//if (rc.top < 0) rc.top = 0;
			//HPEN newPen = ::CreatePen(0, 3, RGB(0, 0, 0)); //建立新画笔,载入DeskDC
			//HGDIOBJ oldPen = ::SelectObject(DeskDC, newPen);
			////在窗口周围显示闪烁矩形
			//::Rectangle(DeskDC, rc.left, rc.top, rc.right, rc.bottom);
			//////Sleep(400); //设置闪烁时间间隔
			////::Rectangle(DeskDC, rc.left, rc.top, rc.right, rc.bottom);
			//::SetROP2(DeskDC, oldRop2);
			////::SelectObject(DeskDC, oldPen);
			//::DeleteObject(newPen);
			//::ReleaseDC(DeskHwnd, DeskDC);
			//DeskDC = NULL;
#endif
			SetWindowTextW(wMainWindow->wTextTargetHwnd,
				to_wstring((INT_PTR)UnHwnd).c_str());
			WCHAR buf[2048] = { 0 };
			GetClassNameW(UnHwnd, buf, 2047);
			SetWindowTextW(wMainWindow->wTextTHwndClass, buf);
			GetWindowTextW(UnHwnd, buf, 2047);
			SetWindowTextW(wMainWindow->wEditWTitle, buf);

			//SetWindowPos(wMainWindow->_tmp_select_targ, HWND_TOPMOST,
			//	rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
			//ShowWindow(wMainWindow->_tmp_select_targ, SW_SHOW);
		}
		break;
	case WM_LBUTTONUP:
		if (wMainWindow->m_lBtnDowned) {
			wMainWindow->m_lBtnDowned = false;
			KillTimer(NULL, wMainWindow->nTimerId_WindowSelector);
			ReleaseCapture(); //释放鼠标捕获
			HICON hicon1 = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_1));
			//IDI_ICON1是有靶图标资源号
			auto oldimage = (HBITMAP)SendMessage(wMainWindow->wIconWSelector,
				STM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon1);
			if (oldimage != NULL) {
				DeleteObject(oldimage);
			}
			DeleteObject(hicon1);

			RedrawWindow(GetDesktopWindow(), NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
			UpdateWindow(GetDesktopWindow());
			if (wMainWindow->_tmp_select_targ)
				ShowWindow(wMainWindow->_tmp_select_targ, SW_HIDE);

			POINT pnt = { 0 }; ::GetCursorPos(&pnt); //取得鼠标坐标
			HWND UnHwnd = ::WindowFromPoint(pnt); //取得鼠标指针处窗口句柄
			if (UnHwnd != hWnd) wMainWindow->targetHwnd = UnHwnd;
			wMainWindow->UpdateHwndInfo();
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_SIZING:
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
			if (wMainWindow->hide_when_min) ShowWindow(hWnd, 0);
			break;
		}
		wMainWindow->ResizeControls(hWnd);
		break;
	case WM_CLOSE:
		if (wMainWindow->hide_when_min) return !ShowWindow(hWnd, 0);
		SendMessageW(hWnd, WM_USER + 4, 0, 0);
		//return DefWindowProcW(hWnd, message, wParam, lParam);
		break;
	case WM_USER+4: // die
		if (wParam == 2 && wMainWindow->pnid) return ShowWindow(hWnd, SW_HIDE);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		if (wMainWindow->hFont) DeleteObject(wMainWindow->hFont);
		if (wMainWindow->_tmp_select_targ) DestroyWindow(wMainWindow->_tmp_select_targ);
		if (wMainWindow->pnid != NULL) {
			Shell_NotifyIcon(NIM_DELETE, wMainWindow->pnid);
			wMainWindow->pnid = NULL;
		}
	case WM_ENDSESSION:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	};
	return 0;
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL Frame_MainWnd::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //hInst = hInstance; // 将实例句柄存储在全局变量中
	
	wMainWindow = this;
	this->nCmdShow = nCmdShow;
	hWnd = CreateWindowExW(0,
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		0, 0, 600, 400,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	CenterWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int Frame_MainWnd::MessageLoop() {
	MSG msg;

	HACCEL hAcc = LoadAccelerators(ThisInst, MAKEINTRESOURCE(IDR_ACCELERATOR_MAINWND));
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (TranslateAccelerator(hWnd, hAcc, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyAcceleratorTable(hAcc);

	wMainWindow = nullptr;

	return (int) msg.wParam;
}

void Frame_MainWnd::CreateControls(HWND par, HINSTANCE hInst) {
	HMENU control_id = (HMENU)0x600;
	auto ghmenu/*generate*/ = [&control_id]()->HMENU { return HMENU(++control_id); };
	wStatic1 = CreateWindowExA(0, "static", "Target HWND: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | SS_CENTERIMAGE, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wTextTargetHwnd = CreateWindowExA(0, "Edit", "0", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
		0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wIconWSelector = CreateWindowExA(0, "static", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		SS_ICON | SS_CENTERIMAGE, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wStatic2 = CreateWindowExA(0, "static", "| Class Name: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | SS_CENTERIMAGE, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wTextTHwndClass = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
		0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wStaticWTitle = CreateWindowExA(0, "static", "Window Title: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | SS_CENTERIMAGE, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wEditWTitle = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wButtonApplyTitle = CreateWindowExA(0, "Button", "Apply", WS_CHILD | WS_VISIBLE |
		/*WS_BORDER | */WS_TABSTOP | BS_FLAT, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wStatic3 = CreateWindowExA(0, "static", "Parent Window: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | SS_CENTERIMAGE, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wTextParentHwnd = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
		0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wButtonSwToParent = CreateWindowExA(0, "Button", "Switch To", WS_CHILD | WS_VISIBLE |
		/*WS_BORDER |*/ WS_TABSTOP | BS_FLAT,
		0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wButtonWinOpt = CreateWindowExA(0, "Button", "Window Options...", WS_CHILD | WS_VISIBLE
		/*| WS_BORDER */| WS_TABSTOP/* | BS_FLAT */ ,
		0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wCheckEnableWin = CreateWindowExA(0, "Button", "", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 1, 1, par, ghmenu(), hInst, 0);
	wStaticEbWin = CreateWindowExA(0, "static", "EnableWindow", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP, 0, 0, 1, 1, par, ghmenu(), hInst, 0);

	if (wIconWSelector) {
		HICON hIcon = (HICON)::LoadImage(ThisInst, MAKEINTRESOURCE
			(IDI_ICON_WINDOWFINDER_1), IMAGE_ICON, 32, 32, 0);
		//SendMessage(wIconWSelector, STM_SETICON, ICON_BIG, (LPARAM)temp_icon);

		auto oldimage = (HBITMAP)SendMessage(wIconWSelector,
			STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		if (oldimage != NULL) {
			DeleteObject(oldimage);
		}
		DeleteObject(hIcon);
	}

	hFont = CreateFontW(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		L"Consolas");
	if (hFont) {
		SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wStatic1, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wTextTargetHwnd, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wStatic2, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wTextTHwndClass, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wStaticWTitle, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wEditWTitle, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wButtonApplyTitle, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wStatic3, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wTextParentHwnd, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wButtonSwToParent, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wButtonWinOpt, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wCheckEnableWin, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wStaticEbWin, WM_SETFONT, (WPARAM)hFont, 0);
	}

	ResizeControls(par);
}

void Frame_MainWnd::ResizeControls(HWND wd) {
	RECT rc = { 0 }; GetClientRect(wd, &rc);
	LONG w = rc.right - rc.left, h = rc.bottom - rc.top;

	SetWindowPos(wStatic1,          0,    10, 10, 100, 24, 0);
	SetWindowPos(wTextTargetHwnd,   0,    120, 10, 110, 24, 0);
	SetWindowPos(wIconWSelector,    0,    240-4, 10-4, 32, 32, 0);
	SetWindowPos(wStatic2,          0,    275, 10, 100, 24, 0);
	SetWindowPos(wTextTHwndClass,   0,    380, 10, w - 390, 24, 0);

	SetWindowPos(wStaticWTitle,     0,    10, 44, 100, 24, 0);
	SetWindowPos(wEditWTitle,       0,    120, 44, w - 200, 24, 0);
	SetWindowPos(wButtonApplyTitle, 0,    w - 70, 44, 60, 24, 0);

	SetWindowPos(wStatic3,          0,    10, 78, 100, 24, 0);
	SetWindowPos(wTextParentHwnd,   0,    120, 78, w - 230, 24, 0);
	SetWindowPos(wButtonSwToParent, 0,    w - 100, 78, 90, 24, 0);

	SetWindowPos(wButtonWinOpt,     0,    10, 112, 130, 24, 0);
	SetWindowPos(wCheckEnableWin,   0,    150, 116, 15, 15, 0);
	SetWindowPos(wStaticEbWin,      0,    170, 112, 90, 24, 0);
}

void Frame_MainWnd::UpdateHwndInfo() {
	if (!IsWindow(hWnd)) {
		hWnd = NULL;
		SetWindowTextW(wTextTargetHwnd, L"0x0");
		SetWindowTextW(wEditWTitle, L"");
		SetWindowTextW(wTextTHwndClass, L"");
		EnableWindow(wEditWTitle, FALSE);
		EnableWindow(wButtonApplyTitle, FALSE);
		return;
	}
	EnableWindow(wEditWTitle, TRUE);
	EnableWindow(wButtonApplyTitle, TRUE);
	SetWindowTextA(wTextTargetHwnd, to_string((INT_PTR)targetHwnd).c_str());
	WCHAR TitleCache[2048] = { 0 };
	GetWindowTextW(targetHwnd, TitleCache, 2048);
	SetWindowTextW(wEditWTitle, TitleCache);
	GetClassNameW(targetHwnd, TitleCache, 2048);
	SetWindowTextW(wTextTHwndClass, TitleCache);
	SendMessage(targetHwnd, BM_SETCHECK, IsWindowEnabled(targetHwnd), 0);
	HWND hParent = GetParent(targetHwnd);
	wstring szParentStr = L"NULL";
	if (hParent) {
		szParentStr = L"(" + to_wstring(UINT_PTR(hParent)) + L") [";
		GetWindowTextW(targetHwnd, TitleCache, 2048);
		szParentStr += TitleCache;
		szParentStr += L"] Class:[";
		GetClassNameW(hParent, TitleCache, 2048);
		szParentStr += TitleCache;
		szParentStr += L"]";
	}
	SetWindowTextW(wTextParentHwnd, szParentStr.c_str());

	DWORD pid = 0; GetWindowThreadProcessId(targetHwnd, &pid);
	HMENU hMenu = GetMenu(hWnd);
	HMENU hSubMenu = GetSubMenu(hMenu, 4);
	if (hMenu && hSubMenu) {
		if (IsWindow(targetHwnd)) EnableMenuItem(hSubMenu, 4, MF_ENABLED | MF_BYPOSITION);
		else EnableMenuItem(hSubMenu, 4, MF_GRAYED | MF_BYPOSITION);

		do {
			HMENU hmOwnProcess = GetSubMenu(GetSubMenu(hSubMenu, 4), 16);
			if (!hmOwnProcess) break;
			MENUITEMINFOW mii; AutoZeroMemory(mii);
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE; //注意掩码
			mii.fType = MFT_STRING;
			wstring a, b;
			a = L"PID: " + to_wstring(pid);
			b = L"Image Name: " + Process.GetProcessNameById(pid);
			if (b == L"Image Name: ") b = L"Image Name: ["s + LastErrorStrW() + L"]";
			mii.dwTypeData = (LPWSTR)a.c_str();
			::SetMenuItemInfoW(hmOwnProcess, ID_MENU_PROCESS_PID, FALSE, &mii);
			mii.dwTypeData = (LPWSTR)b.c_str();
			::SetMenuItemInfoW(hmOwnProcess, ID_MENU_PROCESS_IMAGENAME, FALSE, &mii);
		} while (0);

		SetMenu(hWnd, hMenu);
	}
}

void Frame_MainWnd::setAttribute(UINT attr, INT_PTR value) {
	(*(((INT_PTR*)&attributes) + attr)) = value;
}

VOID Frame_MainWnd::TimerProc_WindowSelect(HWND hw, UINT, UINT_PTR id, DWORD) {
	//KillTimer(hw, id);
	//while (wMainWindow->m_lBtnDowned) {
	//	POINT pnt;
	//	RECT rc;
	//	HWND DeskHwnd = ::GetDesktopWindow(); //取得桌面句柄
	//	HDC DeskDC = ::GetWindowDC(DeskHwnd); //取得桌面设备场景
	//	int oldRop2 = SetROP2(DeskDC, R2_NOTXORPEN);
	//	::GetCursorPos(&pnt); //取得鼠标坐标
	//	HWND UnHwnd = ::WindowFromPoint(pnt); //取得鼠标指针处窗口句柄
	//	if (UnHwnd == wMainWindow->hWnd) return;
	//	wMainWindow->_tmp_tarhw = UnHwnd;
	//	::GetWindowRect(UnHwnd, &rc); //获得窗口矩形
	//	if (rc.left < 0) rc.left = 0;
	//	if (rc.top < 0) rc.top = 0;
	//	HPEN newPen = ::CreatePen(0, 3, 0); //建立新画笔,载入DeskDC
	//	HGDIOBJ oldPen = ::SelectObject(DeskDC, newPen);
	//	::Rectangle(DeskDC, rc.left, rc.top, rc.right, rc.bottom); //在窗口周围显示闪烁矩形
	//	//Sleep(400); //设置闪烁时间间隔
	//	::Rectangle(DeskDC, rc.left, rc.top, rc.right, rc.bottom);
	//	::SetROP2(DeskDC, oldRop2);
	//	::SelectObject(DeskDC, oldPen);
	//	::DeleteObject(newPen);
	//	::ReleaseDC(DeskHwnd, DeskDC);
	//	DeskDC = NULL;
	//	SetWindowTextW(wMainWindow->wTextTargetHwnd, to_wstring((INT_PTR)UnHwnd).c_str());
	//	WCHAR buf[2048] = { 0 };
	//	GetClassNameW(UnHwnd, buf, 2047);
	//	SetWindowTextW(wMainWindow->wTextTHwndClass, buf);
	//	GetWindowTextW(UnHwnd, buf, 2047);
	//	SetWindowTextW(wMainWindow->wEditWTitle, buf);

	//	Sleep(1000);
	//}
}

#if 1
ATOM Frame_MainWnd::ClassRegister_selector_background() {
	static bool reged = false;
	if (reged) return 1;
#pragma warning(push)
#pragma warning(disable: 4302)
	WCHAR szWindowClass[256] = { 0 };
	LoadStringW(ThisInst, IDS_STRING_UI_BACKGCLASS, szWindowClass, 255);

	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc_selector_background;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance  = ThisInst;
	wcex.hIcon      = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
	wcex.hCursor    = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        =
		LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));

	ATOM a = RegisterClassExW(&wcex);
	reged = a;
	return a;

#pragma warning(pop)
}

LRESULT Frame_MainWnd::WndProc_selector_background
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		::SetWindowLongW(hWnd, GWL_EXSTYLE,
			::GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetWindowLongW(hWnd, GWL_EXSTYLE,
			::GetWindowLongW(hWnd, GWL_EXSTYLE) & ~WS_EX_APPWINDOW);
		::SetWindowLongW(hWnd, GWL_EXSTYLE,
			::GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		::SetWindowLongW(hWnd, GWL_STYLE, ::GetWindowLongW(hWnd, GWL_STYLE) & ~WS_CAPTION);
		::SetWindowLongW(hWnd, GWL_STYLE, ::GetWindowLongW(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
		SetLayeredWindowAttributes(hWnd, 0, (BYTE)0x7f, LWA_ALPHA);
		UpdateWindow(hWnd);
		RECT rc; GetWindowRect(hWnd, &rc);
		SetWindowPos(hWnd, 0, 0, 0, 1, 1, 0);
		SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top, 0);
		if (wMainWindow->hightlighting) {
			wMainWindow->hightlighting = false;
			RECT rc; GetWindowRect(wMainWindow->targetHwnd, &rc);
			SetWindowPos(hWnd, 0, rc.left, rc.top,
				rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}

		break;
	case WM_TIMER:
		if (wParam == 4) {
			KillTimer(hWnd, wParam);
			DestroyWindow(hWnd);
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
#endif

LRESULT WndProc_WindowFindDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	//case WM_INITDIALOG:
	//{	HCURSOR hc = LoadCursor(ThisInst, MAKEINTRESOURCE(IDC_CURSOR_WINDOWFINDER));
	////IDC_CURSOR1是靶形光标资源号
	//::SetCursor(hc); }
	//	break;
	case WM_COMMAND:
		if (wParam == IDCANCEL) EndDialog(hDlg, 0);
		if (wParam == IDOK) {
			HWND hw = 0;
			WCHAR caption[2048] = { 0 }, classn[256] = { 0 }, handle[32] = { 0 };
			GetDlgItemTextW(hDlg, IDC_WINDOWFINDER_CAPTION, caption, 2048);
			GetDlgItemTextW(hDlg, IDC_WINDOWFINDER_CLASS, classn, 256);
			GetDlgItemTextW(hDlg, IDC_WINDOWFINDER_HANDLE, handle, 32);
			if (handle[0] != 0) hw = (HWND)atoll(ws2c(handle));
			if (!IsWindow(hw)) {
				if (caption[0] || classn[0])
					hw = FindWindowW
					(classn[0] ? classn : NULL, caption[0] ? caption : NULL);
				else hw = NULL;
			}
			EndDialog(hDlg, (INT_PTR)hw);
			break;
		}
		break;
	//case WM_MOUSEMOVE:
	//	break;
	//case /*32*/111111:
	//{
	//	//POINT pt; GetCursorPos(&pt);
	//	//WINDOWPLACEMENT wp;
	//	//RECT rect;
	//	//GetWindowPlacement(GetDlgItem(hDlg, IDC_WINDOWFINDER_FT), &wp);  //client坐标系  
	//	//rect = wp.rcNormalPosition;
	//	//if (pt.x > rect.left && pt.x < rect.right
	//	//	&& pt.y> rect.top && pt.y < rect.bottom) {
	//		wMainWindow->m_lBtnDowned = true;
	//		SetCapture(hDlg);	//鼠标捕获
	//		HCURSOR hc = LoadCursor(ThisInst, MAKEINTRESOURCE(IDC_CURSOR_WINDOWFINDER));
	//		//IDC_CURSOR1是靶形光标资源号
	//		::SetCursor(hc);
	//		HICON hicon2 = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
	//		//IDI_ICON2为无靶图标资源号
	//		SendMessage(GetDlgItem(hDlg, IDC_WINDOWFINDER_FT), 
	//			WM_SETICON, (WPARAM)hicon2, 0);
	//		break;
	//	//}
	//}
	//	return DefWindowProc(hDlg, message, wParam, lParam);
	//	break;
	//case WM_LBUTTONUP:
	//	if (wMainWindow->m_lBtnDowned) {
	//		wMainWindow->m_lBtnDowned = false;
	//		ReleaseCapture(); //释放鼠标捕获
	//		HICON hicon1 = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_1));
	//		//IDI_ICON1是有靶图标资源号
	//		SendMessage(GetDlgItem(hDlg, IDC_WINDOWFINDER_FT), 
	//			WM_SETICON, (WPARAM)hicon1, 0);
	//	}
	//	break;
	default:
		return DefWindowProc(hDlg, message, wParam, lParam);
	}
	return 0;
}

ATOM Frame_MainWnd::ClassRegister_resizer() {
	static bool reged = false;
	if (reged) return 1;
#pragma warning(push)
#pragma warning(disable: 4302)
	WCHAR szWindowClass[256] = { 0 };
	LoadStringW(ThisInst, IDS_STRING_UI_RESIZERCLASS, szWindowClass, 255);

	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc_resizer;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance  = ThisInst;
	wcex.hIcon      = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
	wcex.hCursor    = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        =
		LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));

	ATOM a = RegisterClassExW(&wcex);
	reged = a;
	return a;

#pragma warning(pop)
}

LRESULT Frame_MainWnd::WndProc_resizer
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
	{
		LONG wstyle = 0;
		::SetWindowLongW(hWnd, GWL_STYLE,
			::GetWindowLongW(hWnd, GWL_STYLE) & ~WS_CAPTION);
		wstyle = ::GetWindowLongW(hWnd, GWL_EXSTYLE);
		wstyle |= WS_EX_LAYERED;
		wstyle |= WS_EX_APPWINDOW;
		wstyle |= WS_EX_TOPMOST;
		SetWindowLongW(hWnd, GWL_EXSTYLE, wstyle);
		SetLayeredWindowAttributes(hWnd, 0, (BYTE)0x7f, LWA_ALPHA);
		UpdateWindow(hWnd);
		RECT rc; GetWindowRect(wMainWindow->targetHwnd, &rc);
		SetWindowPos(hWnd, 0, 0, 0, 1, 1, 0);
		SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top, 0);
		//TextOutA(GetDC(wMainWindow->hWnd), 125, 48,
		//	"Press Enter to accept or Escape to cancel.", 42);
	}
		break;
	case WM_NCHITTEST:
	{
		LRESULT r = DefWindowProc(hWnd, message, wParam, lParam);
		if (r == HTCLIENT) r = HTCAPTION;
		return r;
	}
		break;

	case WM_MOVING:
	case WM_MOVE:
	case WM_SIZING:
	case WM_SIZE:
	{
		RECT rc{ 0 }; GetWindowRect(hWnd, &rc);
		HWND hParent = GetParent(wMainWindow->targetHwnd);
		if (hParent && (GetWindowLong(wMainWindow->targetHwnd, GWL_STYLE) & WS_CHILD)) {
			POINT pnt{ 0 };

			pnt.x = rc.left; pnt.y = rc.top;
			ScreenToClient(hParent, &pnt);
			rc.left = pnt.x; rc.top = pnt.y;

			pnt.x = rc.right; pnt.y = rc.bottom;
			ScreenToClient(hParent, &pnt);
			rc.right = pnt.x; rc.bottom = pnt.y;
		}
		if (message == WM_MOVING || message == WM_MOVE)
			SetWindowPos(wMainWindow->targetHwnd, 0,
				rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		else if (message == WM_SIZING || message == WM_SIZE)
			SetWindowPos(wMainWindow->targetHwnd, 0, 0, 0,
				rc.right - rc.left, rc.bottom - rc.top,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		{
			
		}
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE || wParam == VK_RETURN) DestroyWindow(hWnd);

	case WM_DESTROY:
		if (wMainWindow->hWnd) {
			EnableWindow(wMainWindow->hWnd, TRUE);
			SetForegroundWindow(wMainWindow->hWnd);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT Frame_MainWnd::WndProc_swpdlg
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
	{
		RECT rc{ 0 };
		GetWindowRect(wMainWindow->targetHwnd, &rc);
#define tws(v) (to_wstring(v).c_str())
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_hWndInsertAfter, L"0");
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_X, tws(rc.left));
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_Y, tws(rc.top));
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_cx, tws(rc.right - rc.left));
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_cy, tws(rc.bottom - rc.top));
		SetDlgItemTextW(hWnd, IDC_EDIT_SWP_uFlags, L"0");
#undef  tws
	}
		return TRUE;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON_SWP_APPLY) {
			typedef WCHAR t1[64]; typedef int t2;
			t1 thWndInsertAfter{ 0 }, tX{ 0 }, tY{ 0 }, tcx{ 0 }, tcy{ 0 }, tuFlags{ 0 };
			HWND hWndInsertAfter = 0; UINT uFlags = 0;
			t2 X = 0, Y = 0, cx = 0, cy = 0;

			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_hWndInsertAfter, thWndInsertAfter, 63);
			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_X, tX, 63);
			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_Y, tY, 63);
			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_cx, tcx, 63);
			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_cy, tcy, 63);
			GetDlgItemTextW(hWnd, IDC_EDIT_SWP_uFlags, tuFlags, 63);

#pragma warning(push)
#pragma warning(disable: 4312)
#define applyset(var) (var=((decltype(var))atoi(ws2c(t ## var))))
			applyset(hWndInsertAfter); applyset(uFlags);
			applyset(X);applyset(Y);applyset(cx);applyset(cy);
#undef  applyset
#pragma warning(pop)

			SetWindowPos(wMainWindow->targetHwnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
			break;
		}
		if (LOWORD(wParam) == IDOK)
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_SWP_APPLY, 0);
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hWnd, LOWORD(wParam)); break;
		}
		if (LOWORD(wParam) == IDC_BUTTON_SWP_HELP) {
			ShellExecuteW(NULL, L"open", L"https://docs.microsoft.com/en-us/windows/"
				"win32/api/winuser/nf-winuser-setwindowpos", NULL, NULL, SW_NORMAL);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	case WM_DESTROY:
		EnableWindow(wMainWindow->hWnd, TRUE);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

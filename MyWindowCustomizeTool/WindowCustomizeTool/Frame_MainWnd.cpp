#include "Frame_MainWnd.h"
#include "resource.h"
#include <Richedit.h>
using namespace std;

static WCHAR szWindowClass[64] = { 0 };
static WCHAR szTitle[256] = { 0 };
static Frame_MainWnd* wMainWindow;
#define ThisInst (GetModuleHandle(NULL))

static LRESULT WndProc_WindowFindDlg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

		SendMessage(hWnd, WM_USER + 20, 10, 0);

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
		if (lParam == WM_LBUTTONUP || lParam == WM_LBUTTONDBLCLK) {
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
			ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd);
#endif
			break;
		}
		if (lParam == WM_RBUTTONUP) {
			POINT pt = { 0 }; int resp = 0;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);

			constexpr size_t IDR_SHOW = 1;
			constexpr size_t IDR_EXIT = 2;
			constexpr size_t IDR_HIDEICON = 3;
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
			}
			resp = TrackPopupMenu(hIconMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
				pt.x, pt.y, NULL, hWnd, NULL);
			if (resp == IDR_SHOW){
				PostMessage(hWnd, WM_USER + 13, wParam, WM_LBUTTONUP);
			}
			if (resp == IDR_EXIT){
				DestroyWindow(hWnd);
			}
			if (resp == IDR_HIDEICON){
				SendMessage(hWnd, WM_USER + 20, 10, 1);
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
			DialogBox(ThisInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, WndProc_WindowFindDlg);
			EnableWindow(hWnd, TRUE);
			SetForegroundWindow(hWnd);
			break;
		case ID_MENU_WINDOW_FIND:
			wMainWindow->targetHwnd = (HWND)DialogBoxW(ThisInst, MAKEINTRESOURCEW
				(IDD_DIALOG_WINDOWFINDER1), hWnd, WndProc_WindowFindDlg);
			//if (wMainWindow->targetHwnd == INVALID_HANDLE_VALUE) {
			//	MessageBoxW(hWnd, LastErrorStrW(), L"ERROR", MB_ICONERROR);
			//}
			wMainWindow->UpdateHwndInfo();
			EnableWindow(hWnd, TRUE);
			SetForegroundWindow(hWnd);
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
		case 1044:  // Apply title
		{
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
			ShellExecute(NULL, _T("open"), _T("explorer"), (L"/select,\"" +
				Process.GetProcessFullPathById(pid) + L"\"").c_str(), NULL, SW_NORMAL);
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
			if (!DeleteFile(Process.GetProcessFullPathById(pid).c_str())) {
				MessageBoxW(hWnd, (L"Cannot Delete File " + Process.GetProcessFullPathById
				(pid) +	L" because " + LastErrorStrW() + L" (" + to_wstring(GetLastError())
					+ L")").c_str(), L"Error - DeleteFile", MB_ICONHAND); break;
			}
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
						CheckMenuItem(hMenu, ID_MENU_OPTIONS_HIDEWHENMINIMID, MF_UNCHECKED);
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
				wMainWindow->pnid->hIcon = LoadIcon(ThisInst,
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
		default:;
		}
		break;
	case WM_CONTEXTMENU:
	{
		POINT pt; GetCursorPos(&pt);
		//ScreenToClient(hWnd, &pt);
		printf("[DEBUG] %ld %ld\n", pt.x, pt.y);
		RECT rc; AutoZeroMemory(rc);
		GetWindowRect(wMainWindow->wStatic1, &rc);  //client坐标系  
		if (pt.x > rc.left && pt.x < rc.right
			&& pt.y> rc.top && pt.y < rc.bottom && IsWindow(wMainWindow->targetHwnd)) {
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
#if 1
		SendMessageW(hWnd, WM_USER + 4, 0, 0);
		//return DefWindowProcW(hWnd, message, wParam, lParam);
#else
		if (wMainWindow->pnid == NULL) DestroyWindow(hWnd);
		{
			static int r = 0;
			if (r == 0) r = MessageBoxW(hWnd, L"Do you want to exit or hide window?\n\n"
				"Press [Y] to exit,\nPress [N] to hide,\nor Press [ESC] to cancel.\n\n"
				"* Your choice will be remembered during this process.",
				L"Exit program?", MB_ICONQUESTION | MB_YESNOCANCEL);
			if (r == IDNO)	ShowWindow(hWnd, 0);
			else if (r == IDYES) DestroyWindow(hWnd);
			else if (r == IDCANCEL) r = 0;
		}
#endif
		break;
	case WM_USER+4: // die
		if (wParam == 2 && wMainWindow->pnid) return ShowWindow(hWnd, SW_HIDE);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
	case WM_ENDSESSION:
		if (wMainWindow->hFont) DeleteObject(wMainWindow->hFont);
		if (wMainWindow->pnid != NULL) {
			Shell_NotifyIcon(NIM_DELETE, wMainWindow->pnid);
			wMainWindow->pnid = NULL;
		}
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

	wMainWindow = nullptr;

	return (int) msg.wParam;
}

void Frame_MainWnd::CreateControls(HWND par, HINSTANCE hInst) {
	wStatic1 = CreateWindowExA(0, "static", "Current HWND: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP, 0, 0, 1, 1, par, (HMENU)0x400 + 1, hInst, 0);
	wTextTargetHwnd = CreateWindowExA(0, "Edit", "0x0", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_READONLY, 0, 0, 1, 1, par, (HMENU)0x400 + 2, hInst, 0);
	wStatic2 = CreateWindowExA(0, "static", "| Class Name: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP, 0, 0, 1, 1, par, (HMENU)0x400 + 6, hInst, 0);
	wTextTHwndClass = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_READONLY, 0, 0, 1, 1, par, (HMENU)0x400 + 7, hInst, 0);
	wStaticWTitle = CreateWindowExA(0, "static", "Window Title: ", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP, 0, 0, 1, 1, par, (HMENU)0x400 + 3, hInst, 0);
	wEditWTitle = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP, 0, 0, 1, 1, par, (HMENU)0x400 + 4, hInst, 0);
	wButtonApplyTitle = CreateWindowExA(0, "Button", "Apply", WS_CHILD | WS_VISIBLE |
		WS_BORDER | WS_TABSTOP | BS_FLAT, 0, 0, 1, 1, par, (HMENU)0x400 + 5, hInst, 0);

	hFont = CreateFontW(-13, -6, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
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
	}

	ResizeControls(par);
}

void Frame_MainWnd::ResizeControls(HWND wd) {
	RECT rc = { 0 }; GetClientRect(wd, &rc);
	LONG w = rc.right - rc.left, h = rc.bottom - rc.top;

	SetWindowPos(wStatic1,          0,  10, 10, 80, 20, 0);
	SetWindowPos(wTextTargetHwnd,   0,  100, 10, 100, 20, 0);
	SetWindowPos(wStatic2,          0,  210, 10, 80, 20, 0);
	SetWindowPos(wTextTHwndClass,   0,  300, 10, w - 310, 20, 0);
	SetWindowPos(wStaticWTitle,     0,  10, 40, 80, 20, 0);
	SetWindowPos(wEditWTitle,       0,  100, 40, w - 170, 20, 0);
	SetWindowPos(wButtonApplyTitle, 0,  w - 60, 40, 50, 20, 0);
}

void Frame_MainWnd::UpdateHwndInfo() {
	if (!IsWindow(hWnd)) {
		hWnd = NULL;
		SetWindowTextW(wTextTargetHwnd, L"0x0");
		SetWindowTextW(wEditWTitle, L"");
		SetWindowTextW(wTextTHwndClass, L"");
		return;
	}
	SetWindowTextA(wTextTargetHwnd, to_string((INT_PTR)targetHwnd).c_str());
	WCHAR TitleCache[2048] = { 0 };
	GetWindowTextW(targetHwnd, TitleCache, 2048);
	SetWindowTextW(wEditWTitle, TitleCache);
	GetClassNameW(targetHwnd, TitleCache, 2048);
	SetWindowTextW(wTextTHwndClass, TitleCache);
	DWORD pid = 0; GetWindowThreadProcessId(targetHwnd, &pid);

	HMENU hMenu = GetMenu(hWnd);
	HMENU hSubMenu = GetSubMenu(hMenu, 4);
	if (hMenu && hSubMenu) {
		if (IsWindow(targetHwnd)) EnableMenuItem(hSubMenu, 4, MF_ENABLED | MF_BYPOSITION);
		else EnableMenuItem(hSubMenu, 4, MF_GRAYED | MF_BYPOSITION);

		do {
			HMENU hmOwnProcess = GetSubMenu(GetSubMenu(hSubMenu, 4), 10);
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

LRESULT WndProc_WindowFindDlg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND:
		if (wParam == IDCANCEL) EndDialog(hWnd, 0);
		if (wParam == IDOK) {
			HWND hw = 0;
			WCHAR caption[2048] = { 0 }, classn[256] = { 0 }, handle[32] = { 0 };
			GetDlgItemTextW(hWnd, IDC_WINDOWFINDER_CAPTION, caption, 2048);
			GetDlgItemTextW(hWnd, IDC_WINDOWFINDER_CLASS, classn, 256);
			GetDlgItemTextW(hWnd, IDC_WINDOWFINDER_HANDLE, handle, 32);
			if (handle[0] != 0) hw = (HWND)atoll(ws2c(handle));
			if (!IsWindow(hw)) {
				if (caption[0] || classn[0])
					hw = FindWindowW
					(classn[0] ? classn : NULL, caption[0] ? caption : NULL);
				else hw = NULL;
			}
			EndDialog(hWnd, (INT_PTR)hw);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	{
		//POINT pt; GetCursorPos(&pt);
		//WINDOWPLACEMENT wp;
		//RECT rect;
		//GetWindowPlacement(GetDlgItem(hWnd, IDC_WINDOWFINDER_FT), &wp);  //client坐标系  
		//rect = wp.rcNormalPosition;
		//if (pt.x > rect.left && pt.x < rect.right
		//	&& pt.y> rect.top && pt.y < rect.bottom) {
			wMainWindow->m_lBtnDowned = true;
			SetCapture(hWnd);	//鼠标捕获
			HCURSOR hc = LoadCursor(ThisInst, MAKEINTRESOURCE(IDC_CURSOR_WINDOWFINDER));
			//IDC_CURSOR1是靶形光标资源号
			::SetCursor(hc);
			HICON hicon2 = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_0));
			//IDI_ICON2为无靶图标资源号
			SendMessage(GetDlgItem(hWnd, IDC_WINDOWFINDER_FT), 
				WM_SETICON, (WPARAM)hicon2, 0);
			break;
		//}
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		if (wMainWindow->m_lBtnDowned) {
			wMainWindow->m_lBtnDowned = false;
			ReleaseCapture(); //释放鼠标捕获
			HICON hicon1 = LoadIcon(ThisInst, MAKEINTRESOURCE(IDI_ICON_WINDOWFINDER_1));
			//IDI_ICON1是有靶图标资源号
			SendMessage(GetDlgItem(hWnd, IDC_WINDOWFINDER_FT), 
				WM_SETICON, (WPARAM)hicon1, 0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

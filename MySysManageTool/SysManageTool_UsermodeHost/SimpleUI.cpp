#include "SimpleUI.h"
#include "resource.h"
#include <Richedit.h>
using namespace std;

static WCHAR szWindowClass[37] = { 0 };
static WCHAR szTitle[256] = { 0 };
static Frame_SimpleUI* wMainWindow;
LRESULT NCHITTESTER(HWND hWnd, LPARAM lParam);
void RunUIProcess();
#define ThisInst (GetModuleHandle(NULL))

Frame_SimpleUI::Frame_SimpleUI() {
	ZeroMemory(this, sizeof(Frame_SimpleUI));
	pnid = (decltype(pnid))calloc(1, sizeof(*pnid));
	hRichEditHandle = LoadLibraryW(L"Msftedit.dll");
}

Frame_SimpleUI::~Frame_SimpleUI() {
	if (IsWindow(hWnd)) DestroyWindow(hWnd);
	if (pnid) free(pnid);
	if (hRichEditHandle) FreeLibrary(hRichEditHandle);
}

void Frame_SimpleUI::LoadGlobalString(HINSTANCE src) {
	src ? 0 : src = GetModuleHandle(NULL);
	::LoadStringW(src, IDS_STRING_UI_SIMPLEWNDCLASS, szWindowClass, 37);
	::LoadStringW(src, IDS_STRING_UI_SIMPLETITLE, szTitle, 256);
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM Frame_SimpleUI::MyRegisterClass() {
	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = ThisInst;
//	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CMYSNAKEGUI));
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	constexpr auto _RGBV = 0xf0;
	wcex.hbrBackground  = CreateSolidBrush(RGB(_RGBV, _RGBV, _RGBV));
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU_MAINFRAME);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
//	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);

}

LRESULT Frame_SimpleUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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

		wMainWindow->pnid->cbSize = sizeof(NOTIFYICONDATA);
		wMainWindow->pnid->hWnd = hWnd;
		wMainWindow->pnid->uID = 0;
		wMainWindow->pnid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
		wMainWindow->pnid->uCallbackMessage = WM_USER + 13;
		wMainWindow->pnid->hIcon = LoadIcon(NULL, IDI_APPLICATION);

		//wcscpy_s(wMainWindow->pnid->szInfo, L"");
		//wcscpy_s(wMainWindow->pnid->szInfoTitle, L"");
		wcscpy_s(wMainWindow->pnid->szTip,
			L"System Manage Tool UI Usermode Process");

		Shell_NotifyIconW(NIM_ADD, wMainWindow->pnid);

		HMENU sysMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenuW(sysMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(sysMenu, MF_STRING, WM_USER + 17, L"Alpha&+");
		AppendMenuW(sysMenu, MF_STRING, WM_USER + 18, L"Alpha&-");
		AppendMenuW(sysMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(sysMenu, MF_STRING, WM_DESTROY, L"&Exit");

		wMainWindow->CreateControls(hWnd, ThisInst);

		SetLayeredWindowAttributes(hWnd, 0, (BYTE)0xee, LWA_ALPHA);
		UpdateWindow(hWnd);

		wMainWindow->hHookKb = SetWindowsHookExW
		(WH_KEYBOARD, KbHookProc, ThisInst, GetCurrentThreadId());
		
	}
		break;
	case WM_USER+17:
	case WM_USER+18:
	{
		BYTE alpha = 0; DWORD dw = LWA_ALPHA;
		GetLayeredWindowAttributes(hWnd, 0, &alpha, &dw);
		if (message == WM_USER + 17) alpha+=10;
		else alpha-=10;
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
				static CmdLineA cl(GetCommandLineA());
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
				Shell_NotifyIcon(NIM_DELETE, wMainWindow->pnid);
				free(wMainWindow->pnid);
				wMainWindow->pnid = NULL;
				ShowWindow(hWnd, SW_RESTORE);
			}
			break;
		}
		break;
	case WM_KEYDOWN:
		if (VK_RETURN == wParam) {
			if (::GetFocus() == wMainWindow->wEdit_cmd) {
				SendMessage(hWnd, WM_COMMAND, 0x400 + 3, 0);
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_CONTEXTMENU:
		if (GetFocus() == wMainWindow->wEdit_data) {
			POINT pt = { 0 };
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
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
				AppendMenu(hIconMenu, MF_STRING, 1, _T("&Clear"));
			}
			auto resp = TrackPopupMenu(hIconMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
				pt.x, pt.y, NULL, hWnd, NULL);
			if (resp == 1) SetWindowTextW(wMainWindow->wEdit_data, L"");
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam == WM_DESTROY) return DestroyWindow(hWnd) && false;
		if (wParam == WM_USER + 17) return SendMessageW(hWnd, WM_USER + 17, 0, 0);
		if (wParam == WM_USER + 18) return SendMessageW(hWnd, WM_USER + 18, 0, 0);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
	{
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		if (lParam == LPARAM(wMainWindow->wBtn_Submit)) {  //按钮的HWND
			// do something...
			wMainWindow->ExecuteCommand();
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_SIZING:
	case WM_SIZE:
		wMainWindow->ResizeControls(hWnd);
		break;
	case WM_CLOSE:
#if 0
		do {
			HMENU m = GetMenu(hWnd);
			if (!m) break;
			HMENU m2 = GetSubMenu(m, 1);
			if (!m2) break;
			HMENU m3 = GetSubMenu(m2, 2);
			if (!m3) break;
			/*int r = MessageBoxW(hWnd, L"Do you want to exit or hide window?\n\n"
				"Press [Y] to hide,\nPress [N] to exit,\nor Press [ESC] to cancel.",
				L"Exit program?", MB_ICONQUESTION | MB_YESNOCANCEL);
			if (r == IDYES)*/	ShowWindow(hWnd, 0);
			//else if (r == IDNO) DestroyWindow(hWnd);
		} while (0);
#if 0
		DestroyWindow(hWnd);
#endif
		if (wMainWindow->pnid == NULL) DestroyWindow(hWnd);
		else ShowWindow(hWnd, 0);
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
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
	case WM_ENDSESSION:
		if (wMainWindow->hFont) DeleteObject(wMainWindow->hFont);
		if (wMainWindow->hHookKb) UnhookWindowsHookEx(wMainWindow->hHookKb);
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

LRESULT Frame_SimpleUI::KbHookProc(int code, WPARAM wParam, LPARAM lParam) {
	if (wParam == VK_RETURN) return
		SendMessage(wMainWindow->hWnd, WM_COMMAND, 0, LPARAM(wMainWindow->wBtn_Submit));
	return CallNextHookEx(wMainWindow->hHookKb, code, wParam, lParam);
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
BOOL Frame_SimpleUI::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //hInst = hInstance; // 将实例句柄存储在全局变量中
	
	wMainWindow = this;
	hWnd = CreateWindowExW(0,
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		0, 0, 500, 300,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	CenterWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int Frame_SimpleUI::MessageLoop() {
	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	wMainWindow = nullptr;

	return (int) msg.wParam;
}

void Frame_SimpleUI::CreateControls(HWND par, HINSTANCE hInst) {
	wEdit_cmd = CreateWindowExA(0, "Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
		WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1, 1, par, (HMENU)0x400 + 1, hInst, 0);
	wEdit_data = CreateWindowExW(0, L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE |
		WS_BORDER | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL | ES_READONLY,
		0, 0, 1, 1, par, (HMENU)0x400 + 2, hInst, 0);
	wBtn_Submit = CreateWindowExA(0, "Button", "&Execute", WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | BS_FLAT, 0, 0, 1, 1, par, (HMENU)0x400 + 3, hInst, 0);

	hFont = CreateFontW(-12, -6, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		L"Consolas");
	if (hFont) {
		SendMessage(wEdit_cmd, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wEdit_data, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(wBtn_Submit, WM_SETFONT, (WPARAM)hFont, 0);
	}

	ResizeControls(par);
}

void Frame_SimpleUI::ResizeControls(HWND wd) {
	RECT rc = { 0 }; GetClientRect(wd, &rc);
	LONG w = rc.right - rc.left, h = rc.bottom - rc.top;
	SetWindowPos(wEdit_data, HWND_TOP, 0, 0, w, h - 30, 0);
	SetWindowPos(wEdit_cmd, HWND_TOP, 0, h - 30, w - 80, h - 30, 0);
	SetWindowPos(wBtn_Submit, HWND_TOP, w - 80, h - 30, 80, 30, 0);
}

void Frame_SimpleUI::ExecuteCommand() {
	WCHAR* cmd = (LPWSTR)calloc(32768, sizeof(WCHAR));
	if (0 == cmd) {
		MessageBoxW(hWnd, L"Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
		return;
	}
	GetWindowTextW(wEdit_cmd, cmd, 32768);
	SetWindowTextW(wEdit_cmd, L"");
	if (cmd[0] == 0) { free(cmd); return; }
	CmdLineW scmd(cmd);
	if (scmd.argc() < 1) { free(cmd); return; }
	wstring cmd_user = L"command> "s + cmd;
	free(cmd); cmd = nullptr;
	if (scmd[0] == L"Get-Process") {
		wstring buffer = cmd_user +	L"\r\nProcess Name       |  PID  | Image Path";
		vector<ProcessInfo> pss;
		Process.find(pss);
		for (auto& i : pss) {
			buffer += L"\r\n" + i.name() + L" | " + to_wstring(i.id()) + L" | ";
		}
		cmd = (LPWSTR)calloc(65536, sizeof(WCHAR));
		if (!cmd) {
			MessageBoxW(hWnd, L"2 Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
			return;
		}
		GetWindowTextW(wEdit_data, cmd, int(65536 - buffer.length()));
		wcscat_s(cmd, 65536, buffer.c_str());
		SetWindowTextW(wEdit_data, cmd);
		free(cmd); return;
	}
	if (scmd[0] == L"Kill-Process" && scmd.argc() >= 2) {
		wstring buffer = cmd_user +	L"\r\n";
		Process.kill(Process.find(atol(ws2s(scmd[1]).c_str())));
		buffer += to_wstring(GetLastError()) + L": " + LastErrorStrW();
		cmd = (LPWSTR)calloc(65536, sizeof(WCHAR));
		if (!cmd) {
			MessageBoxW(hWnd, L"2 Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
			return;
		}
		GetWindowTextW(wEdit_data, cmd, int(65536 - buffer.length()));
		wcscat_s(cmd, 65536, buffer.c_str());
		SetWindowTextW(wEdit_data, cmd);
		free(cmd); return;
	}
	if (scmd[0] == L"loaddrv") {
		wstring buffer = cmd_user +	L"\r\n";
		if (MessageBoxW(hWnd, L"Do you really want to load KMDF?", L"Warning",
			MB_ICONWARNING | MB_YESNO) == IDYES) {
			auto h = LoadLibraryA("SysManageTool"
#ifdef _WIN64
				"64"
#else
				"32"
#endif
			);
			if (!h) {
				MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
			} else {
				typedef bool(*KD)(void);
				KD init = (KD)GetProcAddress(h, "InitKernelDriver");
				if (!init)
					MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
				else if (!init()) {
					MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
					buffer += LastErrorStrW();
				} else buffer += L"OK";
			}
		}
		cmd = (LPWSTR)calloc(65536, sizeof(WCHAR));
		if (!cmd) {
			MessageBoxW(hWnd, L"2 Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
			return;
		}
		GetWindowTextW(wEdit_data, cmd, int(65536 - buffer.length()));
		wcscat_s(cmd, 65536, buffer.c_str());
		SetWindowTextW(wEdit_data, cmd);
		free(cmd); return;
	}
	if (scmd[0] == L"trustdrv") {
		wstring buffer = cmd_user +	L"\r\n";
		if (MessageBoxW(hWnd, L"Do you really want to trust KMDF cert?", L"Warning",
			MB_ICONWARNING | MB_YESNO) == IDYES) {
			auto h = LoadLibraryA("SysManageTool"
#ifdef _WIN64
				"64"
#else
				"32"
#endif
			);
			if (!h) {
				MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
			} else {
				typedef bool(*KD)(bool);
				KD init = (KD)GetProcAddress(h, "InstallDriverCert");
				if (!init)
					MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
				else if (!init(true)) {
					MessageBox(hWnd, LastErrorStr(), _T("ERROR"), MB_ICONERROR);
					buffer += LastErrorStrW();
				} else buffer += L"OK";
			}
		}
		cmd = (LPWSTR)calloc(65536, sizeof(WCHAR));
		if (!cmd) {
			MessageBoxW(hWnd, L"2 Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
			return;
		}
		GetWindowTextW(wEdit_data, cmd, int(65536 - buffer.length()));
		wcscat_s(cmd, 65536, buffer.c_str());
		SetWindowTextW(wEdit_data, cmd);
		free(cmd); return;
	}

	cmd_user += L"\r\n";
	cmd_user += GetProcessStdOutputW(L"cmd /c \"" + cmd_user.substr(9) + L"\"");
	cmd = (LPWSTR)calloc(65536, sizeof(WCHAR));
	if (!cmd) {
		MessageBoxW(hWnd, L"2 Cannot alloc memory!!", L"Runtime Error", MB_ICONERROR);
		return;
	}
	GetWindowTextW(wEdit_data, cmd, 65536);
	cmd_user = cmd + cmd_user;
	free(cmd);
	SetWindowTextW(wEdit_data, cmd_user.c_str());
}


#include "Frame_MainWnd.h"
#include "resource.h"
using namespace std;

WCHAR szWindowClass[37] = { 0 };
WCHAR szTitle[256] = { 0 };
static std::map<HWND, Frame_Icon*> HwndAndObjBridge;
static Frame_Icon* _tempvar_pFrame_MainWnd;

Frame_Icon::Frame_Icon() {
	hWnd = wBtn1 = wStatic1 = NULL;
	pnid = (decltype(pnid))calloc(1, sizeof(*pnid));
}

Frame_Icon::~Frame_Icon() {
	if (IsWindow(hWnd)) DestroyWindow(hWnd);
	if (pnid) free(pnid);
}

void Frame_Icon::LoadGlobalString(HINSTANCE src) {
	src ? 0 : src = GetModuleHandle(NULL);
	::LoadString(src, IDS_STRING_UI_WNDCLASS, szWindowClass, 37);
	::LoadString(src, IDS_STRING_UI_TITLE, szTitle, 256);
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM Frame_Icon::MyRegisterClass() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
//	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CMYSNAKEGUI));
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	constexpr auto _RGBV = 255;
	wcex.hbrBackground  = CreateSolidBrush(RGB(_RGBV, _RGBV, _RGBV));
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU_MAINFRAME);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
//	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);

}

LRESULT Frame_Icon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
	{
		LONG wStyle = 0;

		//wStyle = ::GetWindowLongW(hWnd, GWL_STYLE);
		//wStyle &= ~WS_CAPTION;
		//wStyle &= ~WS_SYSMENU;
		//wStyle &= ~WS_SIZEBOX;
		//wStyle &= ~WS_MINIMIZE;
		//wStyle &= ~WS_MINIMIZEBOX;
		//wStyle &= ~WS_MAXIMIZE;
		//wStyle &= ~WS_MAXIMIZEBOX;
		//::SetWindowLongW(hWnd, GWL_STYLE, wStyle);

		wStyle = ::GetWindowLongW(hWnd, GWL_EXSTYLE);
		wStyle |= WS_EX_LAYERED;
		::SetWindowLongW(hWnd, GWL_EXSTYLE, wStyle);

		SetLayeredWindowAttributes(hWnd, 0, (BYTE)240, LWA_ALPHA);

		_tempvar_pFrame_MainWnd->pnid->cbSize = sizeof(NOTIFYICONDATA);
		_tempvar_pFrame_MainWnd->pnid->hWnd = hWnd;
		_tempvar_pFrame_MainWnd->pnid->uID = 0;
		_tempvar_pFrame_MainWnd->pnid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
		_tempvar_pFrame_MainWnd->pnid->uCallbackMessage = WM_USER + 13;
		_tempvar_pFrame_MainWnd->pnid->hIcon = LoadIcon(NULL, IDI_APPLICATION);

		//wcscpy_s(_tempvar_pFrame_MainWnd->pnid->szInfo, L"");
		//wcscpy_s(_tempvar_pFrame_MainWnd->pnid->szInfoTitle, L"");
		wcscpy_s(_tempvar_pFrame_MainWnd->pnid->szTip,
			L"System Manage Tool UI Usermode Process");

		Shell_NotifyIconW(NIM_ADD, _tempvar_pFrame_MainWnd->pnid);

		HMENU sysMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenuW(sysMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(sysMenu, MF_STRING, WM_DESTROY, L"&Exit");

		_tempvar_pFrame_MainWnd->CreateControls(hWnd, GetModuleHandle(NULL));
		
	}
		break;
	case (WM_USER+13):
		if (lParam == WM_LBUTTONUP || lParam == WM_LBUTTONDBLCLK) {
			ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd); break;
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
				if(cl.getopt("from-service"))
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
				Shell_NotifyIcon(NIM_DELETE, HwndAndObjBridge[hWnd]->pnid);
				free(HwndAndObjBridge[hWnd]->pnid);
				HwndAndObjBridge[hWnd]->pnid = NULL;
			}
			break;
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == WM_DESTROY) return DestroyWindow(hWnd) && false;
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
		return DefWindowProc(hWnd, message, wParam, lParam);
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
#endif
		DestroyWindow(hWnd);
#endif
		if (HwndAndObjBridge[hWnd]->pnid == NULL) DestroyWindow(hWnd);
		else ShowWindow(hWnd, 0);
		break;
	case WM_USER+4: // die
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		do {
		CmdLineA cl(GetCommandLineA());
		if (cl.getopt("from-service")) {
			string sn; cl.getopt("svc-name", sn);
			if (sn.empty()) break;
			ShellExecuteA(NULL, "runas", "net.exe", ("stop \"" + sn + "\"").c_str(), 0, 0);
		}
		} while (0);
	case WM_ENDSESSION:
		if (HwndAndObjBridge[hWnd]->pnid != NULL) {
			Shell_NotifyIcon(NIM_DELETE, HwndAndObjBridge[hWnd]->pnid);
			HwndAndObjBridge[hWnd]->pnid = NULL;
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
BOOL Frame_Icon::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //hInst = hInstance; // 将实例句柄存储在全局变量中
	
	_tempvar_pFrame_MainWnd = this;
	hWnd = CreateWindowExW(0,
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		0, 0,
		500, 300,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	HwndAndObjBridge[hWnd] = this;

	CenterWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int Frame_Icon::MessageLoop() {
	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	try {
		HwndAndObjBridge.erase(this->hWnd);
	} catch (std::exception&) {};

	return (int) msg.wParam;
}

void Frame_Icon::CreateControls(HWND par, HINSTANCE hInst) {
	wStatic1 = CreateWindowA("Static", "Service doesn't running",
		WS_VISIBLE, 10, 10, 480, 20, par, HMENU(1033), hInst, 0);
}


// ModalDialogBackground.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ModalDialogBackground.h"
#include "../../resource/tool.h"
#include "resource1.h"
using namespace std;

#define MAX_LOADSTRING 256

// 全局变量:
//HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
//long wsizex, wsizey, wposx, wposy, v_alpha; bool wnoclose;
HHOOK g_hHookWinKey;
class custom_t {
public:
	custom_t() { ZeroMemory(this, sizeof(custom_t)); }
	WCHAR szTitle[MAX_LOADSTRING];
	long wsizex, wsizey, wposx, wposy, v_alpha; bool wnoclose;
	bool useHook;
};
map<HWND, custom_t*> CustomList;
custom_t* _tempvar_custom_t_;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int, custom_t*);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                UpdateWindowPositionInformation(custom_t* custom, HWND hParent = NULL);
LRESULT CALLBACK    hHookFunc_FuckKeyInWindow(int nCode, WPARAM wParam, LPARAM lParam);
void                ShowAppHelpDocument();

#if 0
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MODALDIALOGBACKGROUND, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	CmdLineW cl(GetCommandLineW());
	if (cl.argc() > 1 && cl[1] == L"--help") {
		ShowAppHelpDocument(); return 0;
	}
	if (cl.getopt(L"title") == 1) {
		wstring $; cl.getopt(L"title", $);
		wcscpy_s(szTitle, $.c_str());
	}
	if (cl.getopt(L"parenthwnd") == 1) {
		wstring $; cl.getopt(L"parenthwnd", $);
		HWND parenthwnd = force_castr(HWND, atoll(ws2c($)));
		UpdateWindowPositionInformation(parenthwnd);
	} else UpdateWindowPositionInformation();
	if (cl.getopt(L"noclose")) wnoclose = true;
	if (cl.getopt(L"alpha") == 1) {
		wstring $; cl.getopt(L"alpha", $);
		v_alpha = atoi(ws2c($));
	} else v_alpha = 127;

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) {
		return 1;
	}

	//HACCEL hAccelTable = 
	//	LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MODALDIALOGBACKGROUND));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		//if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//return (int) msg.wParam;
	return 0;
}
#endif

#if 1
extern HINSTANCE hInst;
void WINAPI main_RunDLL(_In_ HWND hWnd,
	_In_opt_ HINSTANCE hInst_,
	_In_ LPCSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	// TODO: 在此处放置代码。


	custom_t* custom = new custom_t;

	CmdLineW cl(s2wc(lpCmdLine));
	if (cl.getopt(L"help")) {
		ShowAppHelpDocument(); return;
	}
	if (cl.getopt(L"title") == 1) {
		wstring $; cl.getopt(L"title", $);
		wcscpy_s(szTitle, $.c_str());
	}
	if (cl.getopt(L"parenthwnd") == 1) {
		wstring $; cl.getopt(L"parenthwnd", $);
		HWND parenthwnd = force_castr(HWND, atoll(ws2c($)));
		UpdateWindowPositionInformation(custom, parenthwnd);
		if (!IsWindow(parenthwnd)) custom->useHook = true;
	}
	else {
		UpdateWindowPositionInformation(custom);
		custom->useHook = true;
	}
	if (cl.getopt(L"noclose")) custom->wnoclose = true;
	if (cl.getopt(L"alpha") == 1) {
		wstring $; cl.getopt(L"alpha", $);
		custom->v_alpha = atoi(ws2c($));
	}
	else custom->v_alpha = 127;

	// 执行应用程序初始化:
	_tempvar_custom_t_ = custom;
	HWND hWin = InitInstance(hInst, nCmdShow, custom);
	if (!hWin) {
		return;
	}
	CustomList[hWin] = custom;

	//HACCEL hAccelTable = 
	//	LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MODALDIALOGBACKGROUND));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		//if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//return (int) msg.wParam;
	return;
}
#endif


//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	//wcex.hIcon          = 
	//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MODALDIALOGBACKGROUND));
	wcex.hIcon          = NULL;
	//wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MODALDIALOGBACKGROUND);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MODALDIALOGBACKGROUND);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	//wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm        = NULL;
	wcex.hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));

	return RegisterClassExW(&wcex);
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
HWND InitInstance(HINSTANCE hInstance, int nCmdShow, custom_t* custom)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  0, 0, 10, 10, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   CustomList[hWnd] = custom;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		//::SetWindowPos(hWnd, (HWND)-1, 0, 0, wsizex, wsizey, 0);
		WindowShowOnTaskBar(hWnd, false);

		LONG wStyle = 0;

		wStyle = ::GetWindowLongW(hWnd, GWL_STYLE);
		wStyle &= ~WS_CAPTION;
		//wStyle &= ~WS_SYSMENU;
		wStyle &= ~WS_SIZEBOX;
		wStyle &= ~WS_MINIMIZE;
		wStyle &= ~WS_MINIMIZEBOX;
		wStyle &= ~WS_MAXIMIZE;
		wStyle &= ~WS_MAXIMIZEBOX;
		::SetWindowLongW(hWnd, GWL_STYLE, wStyle);

		wStyle = ::GetWindowLongW(hWnd, GWL_EXSTYLE);
		wStyle |= WS_EX_LAYERED;
		//wStyle |= WS_EX_NOACTIVATE;
		wStyle |= WS_EX_TOOLWINDOW;
		wStyle &= ~WS_EX_APPWINDOW;
		::SetWindowLongW(hWnd, GWL_EXSTYLE, wStyle);

		SetLayeredWindowAttributes(hWnd, 0, (BYTE)_tempvar_custom_t_->v_alpha, LWA_ALPHA);

		if (_tempvar_custom_t_->wnoclose) {
			HMENU m = GetSystemMenu(hWnd, FALSE); if (m)
			EnableMenuItem(m, SC_CLOSE, (MF_DISABLED + MF_GRAYED) | MF_ENABLED);
		}
		HMENU m = GetSystemMenu(hWnd, FALSE); if (m)
		EnableMenuItem(m, SC_MOVE, MF_DISABLED | MF_GRAYED);

		::SetWindowPos(hWnd, (HWND)-1, _tempvar_custom_t_->wposx, _tempvar_custom_t_->wposy,
			_tempvar_custom_t_->wsizex, _tempvar_custom_t_->wsizey, 0);
		::UpdateWindow(hWnd);
		::SetForegroundWindow(hWnd);
		RECT rc{ 0 };
		rc.left = _tempvar_custom_t_->wposx; rc.top = _tempvar_custom_t_->wposy;
		rc.right = _tempvar_custom_t_->wposx + _tempvar_custom_t_->wsizex;
		rc.bottom = _tempvar_custom_t_->wposy + _tempvar_custom_t_->wsizey;
		::ClipCursor(&rc);

		if (!g_hHookWinKey && _tempvar_custom_t_->useHook) { // didn't hook
			g_hHookWinKey = ::SetWindowsHookExW(WH_KEYBOARD_LL,
				hHookFunc_FuckKeyInWindow, hInst, 0); // hook it
			if (!g_hHookWinKey) {
				std::cerr << LastErrorStrA() << std::endl;
				std::cout << "[ERROR] " << LastErrorStrA() << std::endl;
				::MessageBox(NULL, LastErrorStr(), _T("Error"), MB_ICONERROR);
			}
		}

		break;
	}
	case WM_MOVE:
	case WM_MOVING:
	case WM_SIZE:
	case WM_SIZING:
		::SetWindowPos(hWnd, (HWND)-1, CustomList[hWnd]->wposx, CustomList[hWnd]->wposy,
			CustomList[hWnd]->wsizex, CustomList[hWnd]->wsizey, 0);
		break;
	case WM_SETFOCUS:
	{
		static bool _firstrun = true;
		if (_firstrun) {
			_firstrun = false;
			CustomList[hWnd] = _tempvar_custom_t_;
		}

		UpdateWindowPositionInformation(CustomList[hWnd]);
		::SetWindowPos(hWnd, (HWND)-1, CustomList[hWnd]->wposx, CustomList[hWnd]->wposy,
			CustomList[hWnd]->wsizex, CustomList[hWnd]->wsizey, 0);
		RECT rc{ 0 }; rc.left = CustomList[hWnd]->wposx; rc.top = CustomList[hWnd]->wposy;
		rc.right = CustomList[hWnd]->wposx + CustomList[hWnd]->wsizex;
		rc.bottom = CustomList[hWnd]->wposy + CustomList[hWnd]->wsizey;
		::ClipCursor(&rc);
		break;
	}
	case WM_KILLFOCUS:
		::ClipCursor(NULL);
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			case IDM_EXIT:
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_NCHITTEST:
	{
		//POINT pt{0,0};
		//pt.x = GET_X_LPARAM(lParam);
		//pt.y = GET_Y_LPARAM(lParam);
		//ScreenToClient(hWnd, &pt);
		//RECT wr; memset(&wr, 0, sizeof(RECT));
		//::GetWindowRect(hWnd, &wr);
		//LONG w_w, w_h;
		//w_w = wr.right - wr.left;
		//w_h = wr.bottom - wr.top;

		////top
		//if (pt.x <= 10 && pt.y <= 10) return HTTOPLEFT;
		//if (w_w - pt.x <= 10 && pt.y <= 10) return HTTOPRIGHT;
		//if (pt.y <= 10) return HTTOP;
		//
		////bottom
		//if (w_w - pt.x <= 20 && w_h - pt.y <= 20) return HTBOTTOMRIGHT;
		//if (pt.x <= 20 && w_h - pt.y <= 20) return HTBOTTOMLEFT;
		//if (w_h - pt.y <= 20) return HTBOTTOM;

		////other
		//if (pt.x <= 10) return HTLEFT;
		//if (w_w - pt.x <= 20) return HTRIGHT;

		////if (pt.y <= 50) return HTCAPTION;
		//return HTCAPTION;
		//return HTCLIENT;
	}
		return HTCLIENT;
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE && !CustomList[hWnd]->wnoclose) {
			if (CustomList[hWnd]->wsizex == GetSystemMetrics(SM_CXSCREEN) &&
				CustomList[hWnd]->wsizey == GetSystemMetrics(SM_CYSCREEN)) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			} else
			SetForegroundWindow(GetDesktopWindow());
		}
		break;
	case WM_CLOSE:
		if (CustomList[hWnd]->wnoclose) return false;
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		::ClipCursor(NULL);
		CustomList.erase(hWnd);
		if (CustomList.empty())
		if (!UnhookWindowsHookEx(g_hHookWinKey)) {
			::MessageBox(NULL, LastErrorStr(), _T("Error"), MB_ICONERROR);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void UpdateWindowPositionInformation(custom_t* custom, HWND hParent) {
	static HWND hWindow = NULL;
	if (hParent) hWindow = hParent;
	if (!IsWindow(hWindow)) hWindow = NULL;
	if (hWindow) {
		RECT rc; GetWindowRect(hWindow, &rc);
		custom->wposx = rc.left; custom->wposy = rc.top;
		custom->wsizex = rc.right - rc.left;
		custom->wsizey = rc.bottom - rc.top;
	} else {
		custom->wposx = custom->wposy = 0;
		custom->wsizex = GetSystemMetrics(SM_CXSCREEN);
		custom->wsizey = GetSystemMetrics(SM_CYSCREEN);
	}
}

// 钩子子程
LRESULT CALLBACK hHookFunc_FuckKeyInWindow(int nCode, WPARAM wParam, LPARAM lParam) {
	// 这个Structure包含了键盘的信息
   /*typedef struct {
	   DWORD vkCode;
	   DWORD scanCode;
	   DWORD flags;
	   DWORD time;
	   ULONG_PTR dwExtraInfo;
   } KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;*/
   // 我们只需要那个vkCode
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;

	// MSDN说了,nCode < 0的时候别处理
	if (nCode >= 0)
	{
		// Check to see if the CTRL key is pressed
		bool bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);
		// 按键消息
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (pVirKey->vkCode == VK_LWIN || // Win
				pVirKey->vkCode == VK_RWIN || // Win
				((pVirKey->vkCode == VK_TAB || pVirKey->vkCode == VK_ESCAPE) // Alt+Tab
					&& (pVirKey->flags & LLKHF_ALTDOWN) != 0) ||             // Alt+ESC
				(pVirKey->vkCode == VK_ESCAPE && bCtrlKeyDown) // Ctrl+ESC
			) /* 吃掉消息 */ return 1;
			break;
		default:
			(void)0;
			break;
		}
	}

	return CallNextHookEx(g_hHookWinKey, nCode, wParam, lParam);
}

void ShowAppHelpDocument() {
	const wchar_t* _ = L"Help Document\n=============\n"
		"Create a modal window background.\n" // 描述
		"\nUsage:\n\t%s [--help] [--title=] [--parenthwnd=] [--noclose] [--alpha=]"
		"\n\nParamters:"
		"\n--help          Show this help document."
		"\n--title         Set the title of the window."
		"\n--parenthwnd    Set the parent window of the new window."
		"\n--noclose       Block the close button of the window."
		"\n--alpha         Set the alpha value of the window."
	; HANDLE t = ::CreateFileW(L"__doc__.tmp", FILE_ALL_ACCESS, 0, 0, 2, 0, 0);
	if (!t) {
		std::cerr << "Cannot create tmp file!\n"; return;
	}
	::WriteFile(t, _, DWORD(wcslen(_) * 2), 0, 0);
	::WriteFile(t, L"\n", 2, 0, 0);
	::CloseHandle(t);
	Process.StartOnly(L"cmd /c title [__doc__] - Command-Line Help Document Viewer"
		" v1.01 & type __doc__.tmp & pause & del /f /s /q __doc__.tmp");
}







void __stdcall MessageLoop(void* p) {
	HWND hWnd = force_cast(HWND, p);
	if (!IsWindow(hWnd)) return;

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		//if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//return (int) msg.wParam;
	return;
}

HWND CreateBackgroundWindowEx(
	const wchar_t* title,
	const HWND parent,
	const bool noClose,
	const BYTE bAlpha,
	const bool hUseHook,
	const int nCmdShow
) {
	custom_t* custom = new custom_t;
	wcscpy_s(custom->szTitle, title);
	UpdateWindowPositionInformation(custom, parent);
	custom->wnoclose = noClose;
	custom->v_alpha = bAlpha;
	custom->useHook = hUseHook;
	
	_tempvar_custom_t_ = custom;
	HWND hWnd = InitInstance(hInst, nCmdShow, custom);
	if (!hWnd) {
		delete custom;
		return NULL;
	}
	CustomList[hWnd] = custom;

	if (nCmdShow) SetForegroundWindow(hWnd);

	//_beginthread(MessageLoop, 0, hWnd);

	return hWnd;
}
HWND CreateBackgroundWindow() {
	return CreateBackgroundWindowEx(L"", NULL, false, 127, true, 1);
}

void WaitForWindow(HWND hWnd) {
	while (1) {
		if (!IsWindow(hWnd)) return;
		Sleep(1000);
	}
}

void DestroyBackgroundWindow(HWND h) {
	delete CustomList[h];
	::DestroyWindow(h);
}


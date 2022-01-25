#include "modaldlg.h"

#define wClassName L"a71d5d7e-3ded-407f-b374-b4d1b7b2294a"

static bool clReged;
static HWND hScLk;
static BYTE alpha = 0x7f;


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static LONG wposx = 0, wposy = 0, wsizex = 0, wsizey = 0;
	switch (message)
	{
	case WM_CREATE:
	{
		wsizex = GetSystemMetrics(SM_CXSCREEN);
		wsizey = GetSystemMetrics(SM_CYSCREEN);

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
		wStyle |= WS_EX_TOOLWINDOW;
		wStyle &= ~WS_EX_APPWINDOW;
		::SetWindowLongW(hWnd, GWL_EXSTYLE, wStyle);

		SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
		
		{	HMENU m = GetSystemMenu(hWnd, FALSE); if (m)
			EnableMenuItem(m, SC_CLOSE, MF_DISABLED | MF_GRAYED); }

		{HMENU m = GetSystemMenu(hWnd, FALSE); if (m)
			EnableMenuItem(m, SC_MOVE, MF_DISABLED | MF_GRAYED); }

		::SetWindowPos(hWnd, (HWND)-1, wposx, wposy, wsizex, wsizey, 0);
		::UpdateWindow(hWnd);
		::SetForegroundWindow(hWnd);
		RECT rc{ 0 }; rc.left = wposx; rc.top = wposy;
		rc.right = wposx + wsizex; rc.bottom = wposy + wsizey;
		::ClipCursor(&rc);

		//SetTimer(hWnd, 2, 500, NULL);

		//ShowCursor(FALSE);

		break;
	}
	case WM_MOVE:
	case WM_MOVING:
	case WM_SIZE:
	case WM_SIZING:
		::SetWindowPos(hWnd, (HWND)-1, wposx, wposy, wsizex, wsizey, 0);
		break;
	case WM_SETFOCUS:
	{
		::SetWindowPos(hWnd, (HWND)-1, wposx, wposy, wsizex, wsizey, 0);
		RECT rc{ 0 }; rc.left = wposx; rc.top = wposy;
		rc.right = wposx + wsizex; rc.bottom = wposy + wsizey;
		::ClipCursor(&rc);
		break;
	}
	case WM_DESTROY:
		::ClipCursor(NULL);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = wClassName;
	wcex.hIconSm        = NULL;
	wcex.hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));

	return RegisterClassExW(&wcex);
}

static HANDLE g_Event_window_created;
static DWORD __stdcall thrd_MsgLoop(void*) {
	if (!clReged) clReged = MyRegisterClass(GetModuleHandle(0));
	::hScLk = ::CreateWindowExW(0, wClassName, L"", WS_OVERLAPPEDWINDOW,
		0, 0, 10, 10, 0, 0, GetModuleHandle(0), nullptr);
	if (g_Event_window_created != NULL) SetEvent(g_Event_window_created);
	if (!hScLk) return 1;

	UpdateWindow(hScLk);
	ShowWindow(hScLk, 9);

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		//if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

HWND LockScreen(BYTE alpha) {
	::alpha = alpha;
	g_Event_window_created = CreateEventW(0, FALSE, FALSE, wClassName "-Created");
	::CreateThread(0, 0, thrd_MsgLoop, 0, 0, 0);
	WaitForSingleObject(g_Event_window_created, MAXDWORD);
	CloseHandle(g_Event_window_created); g_Event_window_created = NULL;
	return hScLk;
}

void UnLockScreen() {
	if (hScLk) DestroyWindow(hScLk);
}

void LockScreen_msgloop(){
	thrd_MsgLoop(0);
}

#include "ScreenLock.h"

static decltype(RegisterClassW(0)) MyRegisterClassW(
	PCWSTR className, WNDPROC WndProc,
	WNDCLASSEXW content = WNDCLASSEXW{ 0 });

void RegClass_BackgroundLayeredAlphaWindowClass() {
	WNDCLASSEXW wcex{ 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.lpszClassName = L"BackgroundLayeredAlphaWindowClass";
	wcex.hCursor = LoadCursor(NULL, IDC_NO);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpfnWndProc = WndProc_BackgroundLayeredAlphaWindowClass;
	MyRegisterClassW(0, 0, wcex);
}

static HHOOK g_hHookKb = 0;
static SIZE_T g_hHookKbCnt = 0;
static LRESULT CALLBACK HookProc_BLAWC(int code, WPARAM wParam, LPARAM lParam) {
	// 这个Structure包含了键盘的信息
   // 我们只需要那个vkCode
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;

	// MSDN说了,nCode < 0的时候别处理
	if (code >= 0) {
		return TRUE; /* 吃掉消息 */
	}

	return CallNextHookEx(g_hHookKb, code, wParam, lParam);
}
LRESULT CALLBACK WndProc_BackgroundLayeredAlphaWindowClass(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		SetWindowLong(hWnd, GWL_STYLE,
			(GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION)
			& ~WS_SIZEBOX & ~WS_BORDER);
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)
			| WS_EX_LAYERED | WS_EX_TOOLWINDOW & ~WS_EX_APPWINDOW);
		SetLayeredWindowAttributes(hWnd, 0, 127, LWA_ALPHA);
		RECT rc{ 0 };
		HWND hParent = GetParent(hWnd);
		if (hParent) GetWindowRect(hParent, &rc);
		else {
			rc.right = GetSystemMetrics(SM_CXSCREEN);
			rc.bottom = GetSystemMetrics(SM_CYSCREEN);
		}
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			rc.right - rc.left, rc.bottom - rc.top, 0);

		EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);
		RemoveMenu(GetSystemMenu(hWnd, FALSE), 1, MF_BYPOSITION);

		if (!g_hHookKb) {
			g_hHookKb = SetWindowsHookEx(WH_KEYBOARD_LL,
				HookProc_BLAWC, GetModuleHandle(NULL), 0);
		}
		g_hHookKbCnt++;

		SetTimer(hWnd, 6, 60000, NULL);
	}
		break;

	case MYWM_SET_TRANSPARENT:
		if (wParam) {
			//SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)
			//	& ~WS_EX_LAYERED | WS_EX_TRANSPARENT);
			SetLayeredWindowAttributes(hWnd, 0, 1, LWA_ALPHA);
		} else {
			//SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)
			//	& ~WS_EX_TRANSPARENT | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWnd, 0, 127, LWA_ALPHA);
		}
		break;

	case WM_TIMER:
		switch (wParam) { /* nIDEvent */
		case 6:
			SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;

	case WM_MOVE:
	case WM_SIZE:
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT rc{ 0 };
		HWND hParent = GetParent(hWnd);
		if (hParent) GetWindowRect(hParent, &rc);
		else {
			rc.right = GetSystemMetrics(SM_CXSCREEN);
			rc.bottom = GetSystemMetrics(SM_CYSCREEN);
		}
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			rc.right - rc.left, rc.bottom - rc.top, 0);
		break;
	}

	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		break;

	case WM_CLOSE:
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 6);
		if (((--g_hHookKbCnt) == 0) && g_hHookKb) UnhookWindowsHookEx(g_hHookKb);
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;

	default: 
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

HWND CreateScreenLockWindow() {
	return CreateWindowExW(0, L"BackgroundLayeredAlphaWindowClass", L"",
		WS_OVERLAPPED | WS_SYSMENU, 0, 0, 1, 1, 0, 0, 0, 0);
}


#define _myrclassf(key, _default) \
	if(content. ## key) wcex. ## key = content. ## key;\
	else wcex. ## key = _default;
#define _myrclassc \
	_myrclassf(style, CS_HREDRAW | CS_VREDRAW);\
	_myrclassf(lpfnWndProc, WndProc);\
	_myrclassf(cbClsExtra, 0);\
	_myrclassf(cbWndExtra, 0);\
	_myrclassf(hInstance, NULL);\
	_myrclassf(hIcon, NULL);\
	_myrclassf(hIconSm, NULL);\
	_myrclassf(lpszMenuName, NULL);\
	_myrclassf(hCursor, LoadCursor(nullptr, IDC_ARROW));\
	_myrclassf(hbrBackground, (HBRUSH)(COLOR_WINDOW + 1));\
	_myrclassf(lpszClassName, className);\

#pragma warning(push)
#pragma warning(disable: 4302)
decltype(RegisterClassW(0)) 
MyRegisterClassW(PCWSTR className, WNDPROC WndProc, WNDCLASSEXW content) {
	WNDCLASSEXW wcex{ 0 };
	wcex.cbSize = sizeof(wcex);

	_myrclassc

	return RegisterClassExW(&wcex);
}
#pragma warning(pop)
#undef _myrclassc
#undef _myrclassf


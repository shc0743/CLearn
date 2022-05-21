#include "cppwin_include.h"
#include "hook.x64.h"


extern HINSTANCE hInst;
static HWND WINAPI HookProc_CreateWindowExW(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCWSTR lpClassName,
	_In_opt_ LPCWSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam);
static HWND WINAPI HookProc_CreateWindowExA(
	_In_ DWORD dwExStyle,
    _In_opt_ LPCSTR lpClassName,
    _In_opt_ LPCSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam);
static BOOL WINAPI HookProc_ShowWindow(_In_ HWND hWnd, _In_ int nCmdShow);

static HWND hwnd_parent;
static HWND hwnd_main;
static HHOOK hhk;
static DWORD pid;

DWORD WINAPI Thread_BrowserInjectionWorker(PVOID param) {
	::pid = GetCurrentProcessId();

	::hwnd_parent = (HWND)(INT_PTR)param;
    //MessageBoxW(0, std::to_wstring((INT_PTR)param).c_str(), NULL, 0);
	if (!IsWindow(::hwnd_parent)) return 1;

	ApiInlineHook(L"user32.dll", "CreateWindowExW", HookProc_CreateWindowExW);
	ApiInlineHook(L"user32.dll", "CreateWindowExA", HookProc_CreateWindowExA);
	ApiInlineHook(L"user32.dll", "ShowWindow", HookProc_ShowWindow);

	return 0;
}


static HWND __stdcall HookProc_CreateWindowExW(_In_ DWORD dwExStyle,
    _In_opt_ LPCWSTR lpClassName,
    _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam)
{
    ApiInlineUnHook(L"user32.dll", "CreateWindowExW");
    ApiInlineUnHook(L"user32.dll", "CreateWindowExA");

    ::hwnd_main = CreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle & ~WS_POPUP | WS_CHILD,
        X, Y, nWidth, nHeight, ::hwnd_parent, hMenu, hInstance, lpParam);
    SendMessage(hwnd_parent, 0xcd0, (WPARAM)hwnd_main, 0);

	return hwnd_main;
}

static HWND __stdcall HookProc_CreateWindowExA(_In_ DWORD dwExStyle,
    _In_opt_ LPCSTR lpClassName,
    _In_opt_ LPCSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam)
{
    ApiInlineUnHook(L"user32.dll", "CreateWindowExW");
    ApiInlineUnHook(L"user32.dll", "CreateWindowExA");

    ::hwnd_main = CreateWindowExA(dwExStyle, lpClassName, lpWindowName,
        dwStyle & ~WS_POPUP | WS_CHILD,
        X, Y, nWidth, nHeight, ::hwnd_parent, hMenu, hInstance, lpParam);
    SendMessage(hwnd_parent, 0xcd0, (WPARAM)hwnd_main, 0);

	return hwnd_main;
}

BOOL __stdcall HookProc_ShowWindow(_In_ HWND hWnd, _In_ int nCmdShow) {
    ApiInlineUnHook(L"user32.dll", "ShowWindow");

    BOOL bResult;
    if (hWnd == hwnd_main) {
        bResult = ShowWindow(hWnd, SW_MAXIMIZE);
    }
    else {
        bResult = ShowWindow(hWnd, nCmdShow);
	    ApiInlineHook(L"user32.dll", "ShowWindow", HookProc_ShowWindow);
    }

    return bResult;
}




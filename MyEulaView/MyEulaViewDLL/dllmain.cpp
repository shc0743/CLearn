// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include "resource.h"

HINSTANCE hInst;
LRESULT CALLBACK WndProc_EulaView_HTML(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace s7 {
    decltype(RegisterClassW(0)) MyRegisterClassW(PCWSTR className, WNDPROC WndProc,
        WNDCLASSEXW content = WNDCLASSEXW{ 0 });
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        ::hInst = hModule;
        WCHAR szWndClass1[256]{};
        if (LoadStringW(hInst, IDS_STRING_WNDCLASS_HTHANDLE, szWndClass1, 255)) {
            WNDCLASSEXW wcex{ 0 };
            wcex.hInstance = hInst;
            s7::MyRegisterClassW(szWndClass1, WndProc_EulaView_HTML, wcex);
        }
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH: {
        WCHAR szWndClass1[256]{};
        if (LoadStringW(hInst, IDS_STRING_WNDCLASS_HTHANDLE, szWndClass1, 255)) {
            UnregisterClassW(szWndClass1, hInst);
        }
        break;
    }

    default:
        break;
    }
    return TRUE;
}

int __stdcall DLL_0() {
    return 0;
}

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


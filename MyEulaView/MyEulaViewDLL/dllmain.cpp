// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <string>
#include "resource.h"
#include "../../resource/tool.h"

HINSTANCE hInst;
LRESULT CALLBACK WndProc_EulaView_HTML(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI Thread_BrowserInjectionWorker(PVOID);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        ::hInst = hModule;
        WCHAR szWndClass1[1024]{};
        if (LoadStringW(hInst, IDS_STRING_WNDCLASS_HTHANDLE, szWndClass1, 255)) {
            WNDCLASSEXW wcex{ 0 };
            wcex.hInstance = hInst;
            wcex.hbrBackground = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
            s7::MyRegisterClassW(szWndClass1, WndProc_EulaView_HTML, wcex);
        }
#if 0
        if (GetModuleFileNameW(hInst, szWndClass1, 1023)) {
            std::wstring str = szWndClass1;
            if (str.find(L"__INJECTDLL_TARGET,WND=") != str.npos) {
                str = str.substr(str.find_last_of(L"__INJECTDLL_TARGET,WND=") - 12);
                std::string str2;
                for (auto& i : ws2s(str)) {
                    if (isdigit(i)) str2 += i;
                    else break;
                }
                INT_PTR phwnd = (INT_PTR)atoll(str2.c_str());
                if (phwnd) {
                    HANDLE ht = CreateThread(0, 0,
                        Thread_BrowserInjectionWorker, (PVOID)phwnd, 0, 0);
                    if (ht) CloseHandle(ht);
                }
            }
        }
#endif
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



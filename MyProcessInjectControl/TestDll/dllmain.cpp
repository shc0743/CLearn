// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <string>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBoxW(NULL, L"DLL_PROCESS_ATTACH", (L"PID: " + std::to_wstring(GetCurrentProcessId()) + L"  DLL Injected!").c_str(), 0);
        break;
    case DLL_THREAD_ATTACH:
        MessageBoxW(NULL, L"DLL_THREAD_ATTACH", (L"PID: " + std::to_wstring(GetCurrentProcessId()) + L"  DLL Injected!").c_str(), 0);
        break;
    case DLL_THREAD_DETACH:
        MessageBoxW(NULL, L"DLL_THREAD_DETACH", (L"PID: " + std::to_wstring(GetCurrentProcessId()) + L"  DLL Injected!").c_str(), 0);
        break;
    case DLL_PROCESS_DETACH:
        MessageBoxW(NULL, L"DLL_PROCESS_DETACH", (L"PID: " + std::to_wstring(GetCurrentProcessId()) + L"  DLL Injected!").c_str(), 0);
        break;
        break;
    }
    return TRUE;
}


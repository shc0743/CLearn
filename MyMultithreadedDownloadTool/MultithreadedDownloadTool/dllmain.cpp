// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <WinSock2.h>
#include "pch.h"
#include <map>
#include "def.h"

HINSTANCE hInst = NULL;
AppConfig_t AppConfig;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // init
        ::hInst = hModule;
        AppConfig.DownloadCancelMaxWaitTime = 5000;
        AppConfig.DownloadMaxThreadCount    = 8;
    }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:;
    }
    return TRUE;
}

#pragma comment(lib,"hv_static.lib")


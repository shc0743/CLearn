// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource1.h"

HINSTANCE hInst = NULL;

// 全局变量:
//HINSTANCE hInst;                                // 当前实例
#define MAX_LOADSTRING 256
extern WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
extern WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
//extern long wsizex, wsizey, wposx, wposy, v_alpha; bool wnoclose;
//extern HHOOK g_hHookWinKey;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                UpdateWindowPositionInformation(HWND hParent = NULL);
LRESULT CALLBACK    hHookFunc_FuckKeyInWindow(int nCode, WPARAM wParam, LPARAM lParam);
void                ShowAppHelpDocument();


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::hInst = hModule;
	    // 初始化全局字符串
        LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInst, IDC_MODALDIALOGBACKGROUND, szWindowClass, MAX_LOADSTRING);
        // 注册窗口类
        MyRegisterClass(hInst);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


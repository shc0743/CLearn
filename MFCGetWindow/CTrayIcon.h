#pragma once
#include<string>
#include<iostream>
#include<sstream>
#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<shlwapi.h>
#include <stdlib.h>
#include <tchar.h>
#include <shellapi.h>
//#d/efine BOOL bool
using namespace std;
#define dllexport dllimport
//#//pragma comment(lib,"CTrayIcon.lib")
_declspec(dllexport) class CTrayIcon
{
public:
    CTrayIcon();
    ~CTrayIcon();

    BOOL CreateTray(HWND, HICON, UINT, LPCTSTR = _T(""));
    BOOL ChangeTray(LPCTSTR, UINT = 3000);
    BOOL DeleteTray();

private:
    ULONGLONG GetVersion(LPCTSTR);
    DWORD GetSize_NotifyIconData();

    NOTIFYICONDATA m_Notify;
};

#if 0
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Restore the tray-icon after the explorer's recreation
    if (message == WM_TASKBARCREATED) {
        if (!IsWindowVisible(hWnd))
            trayIcon.CreateTray(hWnd, LoadIcon(hInst, MAKEINTRESOURCE(IDI_WIN32TEST)), WM_ICON_NOTIFY, szTitle);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    switch (message)
    {
    case WM_SIZE:
        switch (wParam)
        {
        case SIZE_RESTORED: // Restore the window and delete the tray-icon
            ShowWindow(hWnd, SW_SHOW);
            trayIcon.DeleteTray();
            break;
        case SIZE_MINIMIZED: // Hide the window and create the tray-icon
            ShowWindow(hWnd, SW_HIDE);
            trayIcon.CreateTray(hWnd, LoadIcon(hInst, MAKEINTRESOURCE(IDI_WIN32TEST)), WM_ICON_NOTIFY, szTitle);
            break;
        default:
            break;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_COMMAND:
        // ...
        break;
    case WM_PAINT:
        // ...
        break;
    case WM_ICON_NOTIFY:
        switch (lParam)
        {
        case WM_LBUTTONDOWN:
            SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, lParam);
            SetForegroundWindow(hWnd);
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
#endif
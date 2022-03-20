#pragma once
#include <Windows.h>

void RegClass_BackgroundLayeredAlphaWindowClass();
LRESULT CALLBACK WndProc_BackgroundLayeredAlphaWindowClass(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND CreateScreenLockWindow();

#define MYWM_SET_TRANSPARENT (WM_USER+WS_EX_TRANSPARENT)


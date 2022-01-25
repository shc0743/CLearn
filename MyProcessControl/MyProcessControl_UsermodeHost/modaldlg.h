#pragma once
#include <iostream>
#include <Windows.h>
using namespace std;

#ifndef _T
#ifdef UNICODE
#define _T(x) L ## x
#else
#define _T(x) x
#endif
#endif

HWND LockScreen(BYTE alpha = 0x7f);
void UnLockScreen();
void LockScreen_msgloop();

#pragma once
#ifndef __cplusplus
#error Please include this file in C++
#endif
#include <Windows.h>


// types

typedef INT_PTR AppVersion_t;
typedef USHORT AppVersionBit_t;
typedef struct {
	AppVersionBit_t
		v0, v1, v2;
} AppVersionInfo_t;
//typedef struct {
//
//} MpCtlEntry_t;


// functions

//BOOL WINAPI MCtlEntry(MpCtlEntry_t*);
AppVersion_t __stdcall MGetVersion();
INT_PTR WINAPI MExecutionServiceSubProcess(PCWSTR);
DWORD CALLBACK MRundInstall(HWND hWnd, HINSTANCE hInst, PSTR lpCmdLine, int nCmdShow);


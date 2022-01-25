#pragma once
/*
* This head file is for ONLY "MFrame.h".
* DO NOT include this file in others places!!
*/

#ifndef _MFrame_defines_h
#define _MFrame_defines_h 1

#ifdef UNICODE
using TSTRING = std::wstring;
#else
using TSTRING = std::string;
#endif

#define _CreateWindow_args _In_opt_ LPCTSTR lpClassName,\
_In_opt_ LPCTSTR lpWindowName,\
_In_ DWORD dwStyle,\
_In_ int X,\
_In_ int Y,\
_In_ int nWidth,\
_In_ int nHeight,\
_In_opt_ HWND hWndParent,\
_In_opt_ HMENU hMenu,\
_In_opt_ HINSTANCE hInstance,\
_In_opt_ LPVOID lpParam
#define _CreateWindow_args_with_default_value _In_opt_ LPCTSTR lpWindowName = NULL,\
_In_ int nWidth = CW_USEDEFAULT,\
_In_ int nHeight = CW_USEDEFAULT,\
_In_ DWORD dwStyle = WS_OVERLAPPEDWINDOW,\
_In_opt_ LPCTSTR lpClassName = NULL,\
_In_ int X = CW_USEDEFAULT,\
_In_ int Y = CW_USEDEFAULT,\
_In_opt_ HWND hWndParent = NULL,\
_In_opt_ HMENU hMenu = 0,\
_In_opt_ HINSTANCE hInstance = NULL,\
_In_opt_ LPVOID lpParam = NULL
#define _CreateWindow_args_name  lpClassName,lpWindowName,dwStyle,\
X,Y,nWidth,nHeight,\
hWndParent,hMenu,hInstance,lpParam

#define _EventsTable_(name) virtual LRESULT name(WPARAM wParam,LPARAM lParam);
#define _EventsTableEx_(name) virtual LRESULT name(UINT message,WPARAM,LPARAM);
#if 0
#define _Frame_EventsTable \
_EventsTable_(OnCreate)\
_EventsTable_(OnCommand)\
_EventsTable_(OnClose)\
_EventsTable_(OnDestroy)\
_EventsTable_(OnKeyDown)\
_EventsTable_(OnKeyPress)\
_EventsTable_(OnKeyUp)\
_EventsTableEx_(OnMouseEvents)\

#else
#define _Frame_EventsTable 
#endif

#else

#undef _CreateWindow_args
#undef _CreateWindow_args_with_default_value
#undef _CreateWindow_args_name
#undef _EventsTable_
#undef _EventsTableEx_
#undef _Frame_EventsTable

#endif


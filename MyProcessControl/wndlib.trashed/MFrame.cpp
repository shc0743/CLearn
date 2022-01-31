#pragma once
#include "MFrame.h"

namespace s7ui {
using namespace std;

#undef _MFrame_defines_h
#include "MFrame_defines.h"


/*global*/ map<HWND, MFrameBase*> _mFramesToHwndsTable;
std::map<DWORD, _MFrame_CreateHook::thrd_wnd_t> _MFrame_CreateHook::thrd_list;


/* _MFrame_CreateHook */

void _MFrame_CreateHook::hook(MFrameBase* w) {
	thrd_wnd_t t;
	t.wnd = w;
	t.hook = SetWindowsHookExW(WH_CBT, HookProc, NULL, GetCurrentThreadId());
	this->thrd_list[GetCurrentThreadId()] = t;
}

LRESULT _MFrame_CreateHook::HookProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HCBT_CREATEWND) {
        HWND hWnd = (HWND)wParam;
		_mFramesToHwndsTable[hWnd] = thrd_list[GetCurrentThreadId()].wnd;
	}
	return CallNextHookEx(thrd_list[GetCurrentThreadId()].hook, code, wParam, lParam);
}

void _MFrame_CreateHook::unhook() {
	UnhookWindowsHookEx(this->thrd_list[GetCurrentThreadId()].hook);
	this->thrd_list.erase(GetCurrentThreadId());
}


/* MFrameBase */

MFrameBase::MFrameBase() {
	ZeroMemory(this, sizeof(*this));
}

MFrameBase::~MFrameBase() {
	if (m_hWnd) destroy();
}

bool MFrameBase::CreateEx(_In_ DWORD dwExStyle, _CreateWindow_args) {
	if (lpWindowName == NULL) lpWindowName = m_windowTitle;
	if (lpClassName == NULL) lpClassName = m_className;

	hook(this);
	HWND hWnd = CreateWindowEx(dwExStyle, _CreateWindow_args_name);
	unhook();

	if (!hWnd) return false;
	this->m_hWnd = hWnd;
	return true;
}

int MFrameBase::MessageLoop() {
	MSG msg; ZeroMemory(&msg, sizeof(msg));

	// Main message loop:
	while (GetMessage(&msg, this->m_hWnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

LRESULT MFrameBase::_cWndProc(HWND h, UINT u, WPARAM w, LPARAM l) {
#ifdef _DEBUG
	auto c = _mFramesToHwndsTable[h];
	assert(c);
#ifndef _WIN64
	if (!c) __asm int 3;
#endif
	auto& v = *c;
	auto b = v.WndProc(h, u, w, l);
	return b;
#else
	return _mFramesToHwndsTable[h]->WndProc(h, u, w, l);
#endif
}

LRESULT MFrameBase::WndProc(HWND _0, UINT _1, WPARAM _2, LPARAM _3) {
	return DefWindowProc(_0, _1, _2, _3);
}


};

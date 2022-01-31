#pragma once
#include "includer.h"

namespace s7ui {
// s7ui namespace
using namespace std;

#include "MFrame_defines.h"
#define MFrame_WndProcT virtual LRESULT CALLBACK WndProc\
(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

class _base {

};

class MFrameBase;
class _MFrame_CreateHook {
public:
	void hook(MFrameBase*);
	void unhook();

private:
	typedef struct _thrd_list_type {
		MFrameBase* wnd;
		HHOOK hook;
	} thrd_wnd_t;
	static std::map<DWORD/*Thread ID*/, thrd_wnd_t> thrd_list;
	static LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam);
};

class MFrameBase : public _base, private _MFrame_CreateHook {
public:
	MFrameBase();
	~MFrameBase();

	/*
	@param lpWindowName if is empty, use this->m_windowTitle
	@param lpClassName if is empty, use this->m_className
	*/
	inline bool Create(_CreateWindow_args_with_default_value)
		{ return CreateEx(0, _CreateWindow_args_name); };
	bool CreateEx(_In_ DWORD dwExStyle, _CreateWindow_args);

	virtual int MessageLoop();

public:
	void show(int nCmdShow = SW_NORMAL) { ShowWindow(m_hWnd, nCmdShow); };
	void close() { SendMessage(m_hWnd, WM_CLOSE, 0, 0); };
	void destroy() { DestroyWindow(m_hWnd); };

public:
	MFrame_WndProcT = 0;
public:
	_Frame_EventsTable;

protected:
	static LRESULT CALLBACK _cWndProc(HWND, UINT, WPARAM, LPARAM);

protected:
	TCHAR m_windowTitle[256];
	TCHAR m_className[256];
	HWND m_hWnd;

};

#include "MFrame_defines.h"

}


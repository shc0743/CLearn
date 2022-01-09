#pragma once
#include "../../resource/includer.h"
#include <shellapi.h>

class Frame_MainWnd {
public:
	Frame_MainWnd();
	~Frame_MainWnd();

	static void             LoadGlobalString(HINSTANCE src);
	static ATOM             MyRegisterClass();
	BOOL InitInstance(HINSTANCE, int); // Create main window
	int MessageLoop();

	HWND GetSafeHwnd() { return hWnd; };

	bool m_lBtnDowned;
protected:

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	void CreateControls(HWND par, HINSTANCE hInst);
	void ResizeControls(HWND w);
	void UpdateHwndInfo();

	HWND hWnd;
    NOTIFYICONDATAW* pnid;
	HFONT hFont;

	HWND
		wStatic1,
		wTextTargetHwnd,
		wStatic2,
		wTextTHwndClass,
		wStaticWTitle,
		wEditWTitle,
		wButtonApplyTitle;

	HWND targetHwnd;
	bool hide_when_min;

};


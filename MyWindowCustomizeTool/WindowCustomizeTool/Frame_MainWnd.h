#pragma once
#include "../../resource/includer.h"
#include <shellapi.h>
#include <ShObjIdl.h>

class Frame_MainWnd {
public:
	Frame_MainWnd();
	~Frame_MainWnd();

	static void             LoadGlobalString(HINSTANCE src);
	static ATOM             MyRegisterClass();
	BOOL InitInstance(HINSTANCE, int); // Create main window
	int MessageLoop();

	HWND GetSafeHwnd() { return hWnd; };

protected:

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	void CreateControls(HWND par, HINSTANCE hInst);
	void ResizeControls(HWND w);
	void UpdateHwndInfo();
	static VOID CALLBACK TimerProc_WindowSelect(HWND, UINT, UINT_PTR, DWORD);
	static ATOM ClassRegister_selector_background();
	static LRESULT CALLBACK WndProc_selector_background(HWND, UINT, WPARAM, LPARAM);
	static ATOM ClassRegister_resizer();
	static LRESULT CALLBACK WndProc_resizer(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK WndProc_swpdlg(HWND, UINT, WPARAM, LPARAM);

	HWND hWnd;
	int nCmdShow;
    NOTIFYICONDATAW* pnid;
	HFONT hFont;
	UINT_PTR nTimerId_WindowSelector;

	bool m_lBtnDowned;

	HWND
		wStatic1,
		wTextTargetHwnd,
		wIconWSelector,
		wStatic2,
		wTextTHwndClass,
		wStaticWTitle,
		wEditWTitle,
		wButtonApplyTitle,
		wStatic3,
		wTextParentHwnd,
		wButtonSwToParent,
		wButtonWinOpt,
		wCheckEnableWin;

	HWND targetHwnd, _tmp_tarhw, _tmp_select_targ, _tmp_resizer;
	bool hide_when_min, hightlighting;
	int autorun_type/*0-Disabled 1-User 0x10-System*/;
	bool minimized;

	struct attributes_t {
		INT_PTR noIcon;
	};
public:
	static constexpr UINT A_NOICON = 0;
	void setAttribute(UINT attr, INT_PTR value);
protected:
	attributes_t attributes;

};


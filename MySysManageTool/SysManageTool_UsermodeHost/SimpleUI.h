#pragma once
#include "includer.h"
#include <shellapi.h>

class Frame_SimpleUI {
public:
	Frame_SimpleUI();
	~Frame_SimpleUI();

	static void             LoadGlobalString(HINSTANCE src);
	static ATOM             MyRegisterClass();
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK KbHookProc(int code, WPARAM wParam, LPARAM lParam);
	BOOL InitInstance(HINSTANCE, int); // Create main window
	int MessageLoop();

	void CreateControls(HWND par, HINSTANCE hInst);
	void ResizeControls(HWND w);

	void ExecuteCommand();

	HWND GetSafeHwnd() { return hWnd; };

protected:
	HWND hWnd;
    NOTIFYICONDATAW* pnid;
	UINT TimerID_KillFocus;
	HMODULE hRichEditHandle;
	HFONT hFont;
	HHOOK hHookKb;

	HWND // Window Controls
		wEdit_data,
		wEdit_cmd,
		wBtn_Submit;
};


#pragma once
#include "includer.h"
#include <shellapi.h>

class Frame_Icon {
public:
	Frame_Icon();
	~Frame_Icon();

	static void             LoadGlobalString(HINSTANCE src);
	static ATOM             MyRegisterClass();
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	BOOL InitInstance(HINSTANCE, int); // Create main window
	int MessageLoop();

	void CreateControls(HWND par, HINSTANCE hInst);

protected:
	HWND hWnd;
    NOTIFYICONDATAW* pnid;

	HWND // Window Controls
		wStatic1,
		wBtn1;
};


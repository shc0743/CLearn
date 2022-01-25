#pragma once
#include "../wndlib/MFrame.h"
#include "resload.h"

class MyWnd_uiMain : public s7ui::MFrameBase {
public:
	MyWnd_uiMain() {
		AutoZeroMemory(*this);
		LoadString(translate_file, IDS_STRING_UI_WNDCLASS, m_className, 256);
		LoadString(translate_file, IDS_STRING_UI_TITLE, m_windowTitle, 256);
	}

	ATOM RegClass();

	MFrame_WndProcT;

protected:
	HWND controls;
};


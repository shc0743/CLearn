#include <iostream>
#include "ScreenLock.h"
#include "../../resource/tool.h"

#pragma comment(linker, "/subsystem:Windows /entry:mainCRTStartup")

int main(int argc, char* argv[]) {
#if 0
	if (argc < 2 || 0 != _stricmp(argv[1], "/NoAddRegRun")) {
		std::wstring wobj = s2ws("\"" + GetProgramDir() + "\" /NoAddRegRun");
		HKEY hk1 = NULL;
		auto r1 = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\MICROSOFT\\Windows\\"
			"CurrentVersion\\Run", 0, KEY_WRITE, &hk1);
		if (hk1) {
			r1 = RegSetValueExW(hk1, L"ScreenLock{n38WLDQy}", 0, REG_SZ,
				(PBYTE)wobj.c_str(), DWORD(wobj.length() * 2));
			RegCloseKey(hk1);
			if (r1) {
				MessageBoxW(NULL, ErrorCodeToStringW(r1).c_str(), NULL, 0);
			}
		}
		else {
			MessageBoxW(NULL, ErrorCodeToStringW(r1).c_str(), NULL, 0);
		}
	}
#endif
	RegClass_BackgroundLayeredAlphaWindowClass();
	HWND hWnd = CreateScreenLockWindow();
	if (!hWnd) return GetLastError();
	if (argc >= 2 && 0 == _stricmp(argv[1], "/TransParentWindow"))
		SendMessage(hWnd, MYWM_SET_TRANSPARENT, TRUE, 0);
	ShowWindow(hWnd, SW_NORMAL);
	SetForegroundWindow(hWnd);
	SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
	MSG msg{ 0 };
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
	return 0;
}


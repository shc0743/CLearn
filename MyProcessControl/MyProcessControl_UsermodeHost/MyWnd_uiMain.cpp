#include "MyWnd_uiMain.h"

LRESULT MyWnd_uiMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		MessageBoxW(NULL, L"344", L"w", 0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyWnd_uiMain::RegClass() {
#pragma warning(push)
#pragma warning(disable: 4302)
	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = _cWndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = translate_file;
//	wcex.hIcon          = LoadIcon(translate_file, MAKEINTRESOURCE());
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	constexpr auto _RGBV = 0xfa;
	wcex.hbrBackground  = CreateSolidBrush(RGB(_RGBV, _RGBV, _RGBV));
//	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU_MAINWND);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = m_className;
//	wcex.hIconSm        = LoadIcon(translate_file, MAKEINTRESOURCE());
	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);

#pragma warning(pop)
}

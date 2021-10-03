#ifndef __cplusplus
#error "tool_ui.cpp is a C++ source file"
#endif
#if (!(defined(_TOOL_SHC0743_MAIN)))
#include "tool_core.h"
#endif
#include "tool_ui.h"


// mt_ui2

HINSTANCE mt_ui2::hInstance = NULL;

bool mt_ui2::UiInit() {
	// 执行应用程序初始化:
	hInstance = ::GetModuleHandle(NULL);
	if (!hInstance) return false;
	MyRegisterClass(hInstance);
	return true;
}
//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM mt_ui2::MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = UiBase::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = window_class_name;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}



// UiBase
mt_ui2::UiBase::UiBase() {
	this->m_hWnd = nullptr;
	this->m_uuid = ::GenerateUUID();
	AutoZeroMemory(this->WindowFlag);
}

mt_ui2::UiBase::~UiBase() {
	if (m_hWnd) ::DestroyWindow(m_hWnd);
}

LRESULT mt_ui2::UiBase::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

WPARAM mt_ui2::UiBase::MessageLoop()
{
	MSG msg; AutoZeroMemory(msg);

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

bool mt_ui2::UiBase::create(HINSTANCE hInstance, int nCmdShow) {
	m_hWnd = CreateWindowEx(0, window_class_name, title.c_str(), WS_OVERLAPPEDWINDOW,
		WindowFlag.x, WindowFlag.y, WindowFlag.w, WindowFlag.h,
		nullptr, nullptr, hInstance, nullptr);

   if (!m_hWnd) {
	  return false;
   }

   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);

   return true;
}
// UiBase


// DlgBase
mt_ui2::DlgBase::DlgBase() : UiBase() {

}

mt_ui2::DlgBase::~DlgBase() {

}

LRESULT mt_ui2::DlgBase::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		default:
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
// DlgBase


// InputDlg
mt_ui2::InputDlg::InputDlg() : DlgBase() {
	m_TextControl = ::CreateWindowA("static", "", WS_CHILD | WS_VISIBLE, 10, 10,
		100, 13, m_hWnd, (HMENU)&this->m_uuid, hInstance, 0);
}

mt_ui2::InputDlg::~InputDlg(){
	::DestroyWindow(m_TextControl);
	::DestroyWindow(m_EditControl);
	::DestroyWindow(m_EditControl);
	::DestroyWindow(m_BtnOk);
	::DestroyWindow(m_BtnCancel);
}

bool mt_ui2::InputDlg::create(STRING title, STRING text, STRING btn1, STRING btn2, STRING DefaultText,
	bool closable, long w, long h) {
	WindowFlag.w = w; WindowFlag.h = h;
	this->title = title;
	::SetWindowText(m_TextControl, text.c_str());
	UiBase::create(hInstance, SW_HIDE);
	return false;
}

WPARAM mt_ui2::InputDlg::MessageLoop() {
	::ShowWindow(m_hWnd, SW_RESTORE);
	return UiBase::MessageLoop();
}

LPCTSTR mt_ui2::InputDlg::GetInputText() {
	MessageLoop();
	return nullptr;
}
//InputDlg


// WindowCustomizeTool.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowCustomizeTool.h"
#include "resource.h"
#include "../../resource/tool.h"
#include <VersionHelpers.h>
#include "Frame_MainWnd.h"
using namespace std;

#define ThisInst (GetModuleHandle(NULL))


// wWinMain function: The application will start from here.
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// TODO: Place code here.
	UNREFERENCED_PARAMETER(0);

	if (!IsWindowsVistaOrGreater()) {
		fprintf(stderr, "[FATAL] Your OS version is TOO LOW!\nIf you want to run this "
			"program, please update your OS.\nAt least Windows 7 is required.\n"
			"Exiting...\n");
		return ERROR_NOT_SUPPORTED; // Exit
	}

	CmdLineA cl(GetCommandLineA()); // Command-Line Arguments Parser

	if (cl.getopt("enable-run-at-logon") || cl.getopt("disable-run-at-logon")) {
		HKEY hk = NULL; DWORD rsz = REG_SZ; WCHAR keyname[256] = { 0 };
		if (!LoadStringW(ThisInst, IDS_STRING_APP_REG_KEYNAME, keyname, 256))
			return GetLastError();

		RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\Current"
			"Version\\Run", 0, KEY_READ | KEY_WRITE, &hk);
		bool user_mode_passed = 0;
		user_mode:
		if (hk == NULL) RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft"
			"\\Windows\\CurrentVersion\\Run", 0, KEY_READ | KEY_WRITE, &hk);
		if (hk == NULL) return GetLastError();

		LSTATUS err = 0;
		wstring autopath = L"\"" + s2ws(GetProgramDir()) + L"\" --hidden";
		if (cl.getopt("enable-run-at-logon")) err = RegSetValueExW(hk, keyname, NULL, rsz,
			(BYTE*)autopath.c_str(), DWORD(autopath.length() * 2 + 1));
		else {
			err = RegDeleteValueW(hk, keyname);
		}

		RegCloseKey(hk);
		hk = NULL;
		if ((!user_mode_passed) && IsRunAsAdmin() && cl.getopt("disable-run-at-logon")) {
			user_mode_passed = 1;
			goto user_mode;
		}
		return err;
	}

	if (cl.getopt("user-exit")) {
		WCHAR szWindowClass[64] = { 0 };
		LoadStringW(ThisInst, IDS_STRING_UI_WNDCLASS, szWindowClass, 64);
		HWND hMainWnd = NULL;
		typedef BOOL(WINAPI* EndTask_t)(HWND hWnd, BOOL fShutDown, BOOL fForce);
		HMODULE user32 = GetModuleHandleA("user32.dll");
		if (user32) {
			EndTask_t EndTask = (EndTask_t)GetProcAddress(user32, "EndTask");
			if (EndTask) {
				while (hMainWnd = FindWindowW(szWindowClass, NULL)) {
					SendMessage(hMainWnd, WM_USER + 4, 0, 0);
					EndTask(hMainWnd, FALSE, TRUE);
				}
			}
		}
		return 0;

	}

	//if (IsCurrentUserInBuiltinGroup(WinBuiltinUsersSid)) { // Current user is in "Users"
	{
		if (GetConsoleWindow()) { // CLI

		}

		// GUI init
		Frame_MainWnd::LoadGlobalString(NULL);
		Frame_MainWnd::MyRegisterClass();

		// Open GUI
		if (cl.getopt("icon") || cl.getopt("hidden")) { // Icon Only
			nCmdShow = 0;
		}

		bool noIcon = false;// No Icon
		if (cl.getopt("no-icon")) { // No Icon
			noIcon = true;
		}

		Frame_MainWnd w;
		if (noIcon) w.setAttribute(w.A_NOICON, TRUE);
		w.InitInstance(ThisInst, nCmdShow);
		w.MessageLoop();

		return 0;
	}
	//}

	return ERROR_INVALID_PARAMETER;
	return 0;
}

int main() {
	return wWinMain(ThisInst, 0, (LPWSTR)L"", SW_SHOW);
}




#if 0
#define MAX_LOADSTRING 256

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
#if 0
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
#endif

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_STRING_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWCUSTOMIZETOOL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance,
		MAKEINTRESOURCE(IDC_WINDOWCUSTOMIZETOOL));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



#if 0
//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWCUSTOMIZETOOL));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWCUSTOMIZETOOL);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#endif
#endif

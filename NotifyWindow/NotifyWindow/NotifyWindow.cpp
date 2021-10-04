// NotifyWindow.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "NotifyWindow.h"
#include <ShObjIdl.h>
#include "../../resource/tool.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

using MainWindow = struct {
	HFONT static_titlebar_font;
	HWND static_titlebar;
	HFONT static_content_font;
	HWND static_content;
	HFONT button_closebtn_font;
	HWND button_closebtn;
	bool mousemoving;
	long msdown_x;
	long msdown_y;
	size_t close_sleep_id;

	RECT screensize;

	wstring title;
	wstring content;
	wstring contfont;
	unsigned autoclose_time;
};
MainWindow mainwindow;
CmdLine cl;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	cl.Split(::GetCommandLine());
	//if (cl.argc() < 2) {
	//	auto pi = Process.Start(wstring(L"\"") + s2ws(GetProgramDir()) + L"\" \"message demo\"");
	//	while (WaitForSingleObject(pi.hProcess, 0xffffffff));
	//	DWORD exitCode = 0;
	//	GetExitCodeProcess(pi.hProcess, &exitCode);
	//	CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
	//	exit(exitCode);
	//}

	mainwindow.title = L"Notify";
	mainwindow.content = L"message demo";
	mainwindow.contfont = L"楷体";
	mainwindow.autoclose_time = 0;

	RECT rectDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0);
	mainwindow.screensize = rectDesktop;

	for (auto i : cl) {
		if (i.length() > 18 && i.substr(0, 18) == L"content_from_file=") {
			wstring str = i;
			str.erase(0, 18);
			fstream fp(ws2s(str), ios::in);
			if (!fp) {
				fprintf(stderr, "Cannot open file %s!!!\n", ws2s(str).c_str());
				exit(1);
			}
			char buffer[4095];
			memset(buffer, 0, 4095);
			mainwindow.content = L"";
			while (fp.getline(buffer, 4095)) {
				mainwindow.content += s2ws(buffer);
				mainwindow.content += L"\r\n";
			}
			fp.close();
			continue;
		}
		if (i.length() > 13 && i.substr(0, 13) == L"content_font=") {
			mainwindow.contfont = i;
			mainwindow.contfont.erase(0, 13);
			continue;
		}
		if (i.length() > 8 && i.substr(0, 8) == L"content=") {
			mainwindow.content = i;
			mainwindow.content.erase(0, 8);
			continue;
		}
		if (i.length() > 6 && i.substr(0, 6) == L"title=") {
			mainwindow.title = i;
			mainwindow.title.erase(0, 6);
			continue;
		}
		if (i.length() > 6 && i.substr(0, 6) == L"close=") {
			if (i == L"close=auto") { mainwindow.autoclose_time = 15000; continue; };
			mainwindow.autoclose_time = atoi(ws2s(wstring(i).erase(0, 6)).c_str());
			continue;
		}
		fprintf(stderr, "Invalid argument %s\n", ws2s(i).c_str());
	}

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_NOTIFYWINDOW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance,MAKEINTRESOURCE(IDC_NOTIFYWINDOW));

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
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTIFYWINDOW));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NOTIFYWINDOW);
	wcex.lpszMenuName   = NULL;
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

   HWND hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, 500, 300, nullptr, nullptr, hInstance, nullptr);

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
	case WM_CREATE:
	{
		//LONG Style = 0;
		//Style = GetWindowLong(hWnd, GWL_STYLE);	 // 得到窗口风格   
		//Style &= ~WS_CAPTION;			 //去掉标题栏
		//SetWindowLong(hWnd, GWL_STYLE, Style);	 //为窗口设置新的风格
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION);
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MINIMIZE);
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZE);
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_THICKFRAME);
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0, 
					 SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
		WindowShowOnTaskBar(hWnd, false);

		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		{
			RECT rectDesktop;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0); 
				// 获得屏幕工作区大小 left:0,top:0,right:width,bottom:height
			RECT rectDlg;
			GetWindowRect(hWnd, &rectDlg); // 获取当前窗口工作区大小
			int dlgWidth = rectDlg.right - rectDlg.left;
			int dlgHeight = rectDlg.bottom - rectDlg.top;
			//dlgWidth  -= 10;
			//dlgHeight -= 40;
			SetWindowPos(hWnd,NULL,
				rectDesktop.right  - dlgWidth  - 20,
				rectDesktop.bottom - dlgHeight - 20,
				dlgWidth, dlgHeight, SWP_NOZORDER | SWP_NOSIZE);
		}

		RECT rt;
		GetClientRect(hWnd, &rt);

		//创建逻辑字体
		mainwindow.static_titlebar_font = CreateFontW(
			-30/*高度*/, -15/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			L"微软雅黑"  //字体名
		);
		//创建静态文本框控件
		mainwindow.static_titlebar = CreateWindowExW(0,
			L"static",  //静态文本框的类名
			mainwindow.title.c_str(),  //控件的文本
			WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | WS_BORDER /*带边框*/,
			0 /*X坐标*/, 0/*Y坐标*/, rt.right-rt.left/*宽度*/, 42/*高度*/, hWnd,
			(HMENU)1001,  //为控件指定一个唯一标识符
			hInst,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			mainwindow.static_titlebar,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)mainwindow.static_titlebar_font,  //字体句柄
			NULL  //传空值即可
		);

		//创建逻辑字体
		mainwindow.static_content_font = CreateFontW(
			-20/*高度*/, -10/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			mainwindow.contfont.c_str()  //字体名
		);
		//创建静态文本框控件
		mainwindow.static_content = CreateWindowExW(0,
			L"edit",  //静态文本框的类名
			mainwindow.content.c_str(),  //控件的文本
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_BORDER | ES_WANTRETURN |
			WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0 /*X坐标*/, 50/*Y坐标*/, rt.right-rt.left/*宽度*/, 250/*高度*/, hWnd,
			(HMENU)1003,  //为控件指定一个唯一标识符
			hInst,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			mainwindow.static_content,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)mainwindow.static_content_font,  //字体句柄
			NULL  //传空值即可
		);

		//创建逻辑字体
		mainwindow.button_closebtn_font = CreateFontW(
			-24/*高度*/, -12/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			L"微软雅黑"  //字体名
		);
		//创建控件
		mainwindow.button_closebtn = CreateWindowExW(0,
			L"button",  //静态文本框的类名
			L"x",  //控件的文本
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			rt.right - 35 /*X坐标*/, 5/*Y坐标*/, 30/*宽度*/, 30/*高度*/, hWnd/*父窗口*/,
			(HMENU)1002,  //为控件指定一个唯一标识符
			hInst,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			mainwindow.button_closebtn,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)mainwindow.button_closebtn_font,  //字体句柄
			NULL  //传空值即可
		);

		if (mainwindow.autoclose_time)
		{
			mainwindow.close_sleep_id = SetTimer(hWnd, 4881, mainwindow.autoclose_time,
				[](HWND h, UINT u, UINT_PTR id, DWORD dw)->void {
					KillTimer(h, id);
					SendMessageW(h, WM_CLOSE, 0, 0);
				});
		}

	}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		mainwindow.mousemoving = true;
		POINT point;
		::GetCursorPos(&point);
		::ScreenToClient(hWnd, &point);
		mainwindow.msdown_x = point.x + 0;
		mainwindow.msdown_y = point.y + 0;
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		mainwindow.mousemoving = false;
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		if (mainwindow.mousemoving) {
			POINT point;
			::GetCursorPos(&point);
			if (int(point.x - mainwindow.msdown_x) < 0) break;
			if (int(point.y - mainwindow.msdown_y) < 0) break;
			::SetWindowPos(
				hWnd,
				(HWND)-1,
				int(point.x - mainwindow.msdown_x),
				int(point.y - mainwindow.msdown_y),
				0, 0,
				SWP_NOSIZE);
		}
		break;
	case WM_MOVING:
	case WM_MOVE:
		//RECT rt;
		//GetWindowRect(hWnd, &rt);
		//SetWindowPos(mainwindow.static_titlebar, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//SetWindowPos(mainwindow.static_content, NULL, 0, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//SetWindowPos(mainwindow.button_closebtn, NULL,
		//			 465, 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//GetClientRect(hWnd, &rt);
		//
		//UpdateWindow(hWnd);

		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			//case IDM_ABOUT:
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			//	break;
			//case IDM_EXIT:
			//	DestroyWindow(hWnd);
			//	break;
			case 1002:
				SendMessageW(hWnd, WM_CLOSE, 0, 0);
				break;
			case 1003:

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
			//ChangeWindowBackground(hWnd, 500, 300, 170, 170, 170);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		DeleteObject(mainwindow.static_titlebar_font);
		DeleteObject(mainwindow.button_closebtn_font);
		DeleteObject(mainwindow.static_content_font);
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

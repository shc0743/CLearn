// ImageAdPopup.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ImageAdPopup.h"
#include "resource.h"
#include "../../resource/tool.h"
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui_c.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND hBtnClose;                                 // 关闭按钮
HWND hImgShow;                                  // 图片显示器
HWND hOpenCVImgWindow;                          // opencv窗口
cv::Mat MainImage;                              // 图片
struct {
	enum { ERR_VALUE = 0, OPEN_FILE, EXEC_FILE, OPEN_URL } action;
	string value;
} onclick_action;                               // --action
struct {
	string title;
	string img_path;
	struct { 
		string report; 
		string set;
		string about;
	} bin_path;
	bool force_popup;
} window_configs;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                ExecOpenFunction(void);
void                ExecOpenFunction_cvmscb(int event, int x, int y, int flags, void* userdata);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	CmdLineA argv(::GetCommandLineA());
	for (size_t cnt = 0, cll = argv.argc(); cnt < cll; ++cnt) {
		string i = argv[cnt];
		if (i.length() < 1) continue;
		if (i.substr(0, 6) == "--help") {
			//Process.GetParentProcessId(GetCurrentProcessId());
			if (!::EnableDebugPrivilege()) {
				fprintf(stderr, "Access denied at EnableDebugPrivilege()\n");
				return ERROR_ACCESS_DENIED;
			}
			ProcessInfo pi = Process.find(Process.GetCurrentProcess().parent());
			if (!pi) {
				fprintf_s(stderr, "Cannot open process %d!!!",
					(int)Process.GetCurrentProcess().parent());
				//return 1;
			}
			Process.suspend(pi);
			char* hlpinfo_buffer = (char*)calloc(2048, 1);
			if (hlpinfo_buffer == 0) {
				fprintf_s(stderr, "Cannot calloc(2048,1)!\n");
				return 3;
			}
			sprintf_s(hlpinfo_buffer, 2048, "%s Help Document\n================\n"
				"Usage: %s %s\n\nArguments: \n%s\n", argv[0].c_str(), argv[0].c_str(),
				" [--help] [--title=] <--image=> <--action=(open_file|exec_file|open_url)> "
				"<--value=> [--reportbin=] [--settingsbin=] [--aboutbin=] [--width=] [--height"
				"=] [--force]"/*usage-argv*/,
				"--help Show this help information.\n"
				"--title=title Set window title to \"title\".\n"
				"--image=imagepath Set the image \"imagepath\" to be displayed.\n"
				"--action=(open_file|exec_file|open_url) Set the click event of the image.\n"
				"\topen_file Open a local file as explorer.exe.\n"
				"\texec_file Open a local file as ::CreateProcess.\n"
				"\topen_url  Open a webpage.\n"
				"--value=value Set the value of action.\n"
				"--reportbin Set the executable file to open when you click the \"report\" "
				"button. Attached parameter: \"--image=imagepath --open=value --report\"."
				"--settingsbin "
				"Set the executable file that opens when you click the \"set\" button. Attached"
				" parameter: \"--image=imagepath --open=value\". If you click the \"no more "
				"pop - up\" button, the parameter \"--set=nopopupagain=No more pop up frequency"
				"\" will be attached. For example, if \"3 days\" is no longer popped up, the pa"
				"rameter \"--set=nopopupagain=3\" will be attached. If \"Never pop up again"
				"\", the parameter \"--set=nopopupagain=2147483647\" will be attached.\n"
				"--aboutbin Set the executable file to open when you click the \"About\" "
				"button. Attached parameter: \"--image=imagepath --open=value --about\"."
				"\n--width Set the window's width.\n--height Set the window's height."
				"\n--force Ignore the process-count checker and pop a window right now."
			/*arguments*/);
			::fputs(hlpinfo_buffer, stdout);
			::MessageBoxA(NULL, hlpinfo_buffer, "%s Help Document", MB_ICONINFORMATION);
			Process.resume(pi);
			return 0;
		}
		if (i.substr(0, 14) == "--settingsbin=") {
			window_configs.bin_path.set = i.erase(0, 14); continue;
		}
		if (i.substr(0, 9) == "--action=") {
			i.erase(0, 9);
			if (i == "open_file") onclick_action.action = onclick_action.OPEN_FILE;
			if (i == "exec_file") onclick_action.action = onclick_action.EXEC_FILE;
			if (i == "open_url") onclick_action.action = onclick_action.OPEN_URL;
			continue;
		}
		if (i.substr(0, 8) == "--title=") {
			window_configs.title = i.erase(0, 8); continue;
		}
		if (i.substr(0, 8) == "--value=") {
			onclick_action.value = i.erase(0, 8); continue;
		}
		if (i.substr(0, 8) == "--image=") {
			window_configs.img_path = i.erase(0, 8); continue;
		}
		if (i.substr(0, 7) == "--force") {
			window_configs.force_popup = true; continue;
		}
	}

	//检查参数
	if (window_configs.img_path.empty() || onclick_action.value.empty() ||
		(onclick_action.action == onclick_action.ERR_VALUE)) {
		cerr << "Incomplete parameters!!\n"; return 1;
	}

	//检查自身数量，如果多于一个那么排队等待
	if ((!window_configs.force_popup) && Process.count(Process.GetCurrentProcess()) - 1) {
		vector<ProcessInfo> ps; ps.reserve(3);
		Process.find(s2ws(GetProgramInfo().name), ps);
		for(auto i : ps){
			if (i.id() == GetCurrentProcessId()) {
				continue;
			}
			while (WaitForSingleObject(i, 0xffffffff));
		}
	}

	//检查图片是否存在
	if (!file_exists(window_configs.img_path)) {
		cerr << window_configs.img_path << " not exists or cannot open it\n";
		return 2;
	}
	//加载图片
	MainImage = cv::imread(window_configs.img_path);
	if (MainImage.empty()) {
		cerr << "Bad image " << window_configs.img_path << ",cannot read or display!!\n";
		return 4;
	}

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_IMAGEADPOPUP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGEADPOPUP));

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
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEADPOPUP));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IMAGEADPOPUP);
	wcex.lpszMenuName   = 0;
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
	  CW_USEDEFAULT, CW_USEDEFAULT, 500, 300, nullptr, nullptr, hInstance, nullptr);

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
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_BORDER);
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
				SetWindowPos(hWnd, NULL,
					rectDesktop.right - dlgWidth - 20,
					rectDesktop.bottom - dlgHeight - 20,
					dlgWidth, dlgHeight, SWP_NOZORDER | SWP_NOSIZE);
			}

			if (!window_configs.title.empty())
				::SetWindowTextA(hWnd, window_configs.title.c_str());

			hBtnClose = ::CreateWindowExW(0, L"button", L"X", WS_CHILD | WS_VISIBLE, 
				455, 0, 30, 30, hWnd, (HMENU)1003, hInst, 0);
			//hImgShow = ::CreateWindowExW(0, L"static", L"", WS_CHILD | WS_VISIBLE, 
			//	0, 0, 500, 300, hWnd, (HMENU)1004, hInst, 0);
			cv::namedWindow("WinImageShow1", 0);
			hOpenCVImgWindow = (HWND)cvGetWindowHandle("WinImageShow1");
			::ShowWindow(::GetParent(hOpenCVImgWindow), SW_HIDE);
			::SetParent(hOpenCVImgWindow, hWnd);
			cv::imshow("WinImageShow1", MainImage);
			cv::resizeWindow("WinImageShow1", 500, 300);
			cv::setMouseCallback("WinImageShow1", ExecOpenFunction_cvmscb);
			cv::waitKey(1);
		}
		break;
	case WM_NCHITTEST:
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &pt);
		RECT wr; memset(&wr, 0, sizeof(RECT));
		::GetWindowRect(hWnd, &wr);
		LONG w_w, w_h;
		w_w = wr.right - wr.left;
		w_h = wr.bottom - wr.top;

		//top
		if (pt.x <= 10 && pt.y <= 10) return HTTOPLEFT;
		if (w_w - pt.x <= 10 && pt.y <= 10) return HTTOPRIGHT;
		if (pt.y <= 10) return HTTOP;
		
		//bottom
		if (w_w - pt.x <= 20 && w_h - pt.y <= 20) return HTBOTTOMRIGHT;
		if (pt.x <= 20 && w_h - pt.y <= 20) return HTBOTTOMLEFT;
		if (w_h - pt.y <= 20) return HTBOTTOM;

		//other
		if (pt.x <= 10) return HTLEFT;
		if (w_w - pt.x <= 20) return HTRIGHT;

		//if (pt.y <= 50) return HTCAPTION;
		return HTCAPTION;
		return HTCLIENT;
	}
		break;
	case WM_SIZING:
	case WM_SIZE:
	{
		RECT wr; memset(&wr, 0, sizeof(RECT));
		::GetWindowRect(hWnd, &wr);
		::SetWindowPos(hBtnClose, (HWND)-1, (wr.right - wr.left) - 30 - 5, 5, 30, 30, 0);
		try {
			cv::resizeWindow("WinImageShow1", wr.right - wr.left, wr.bottom - wr.top);
		}
		catch (cv::Exception& exc) {
			cerr << "Error!! OpenCVException: " << exc.what() << endl;
		}
		//::UpdateWindow(hBtnClose); ::UpdateWindow(hWnd);
	}
		break;
	case WM_RBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_CONTEXTMENU:
		static HMENU hMenu = ::LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_CONTEXTMENU));
		{
			::SetForegroundWindow(hWnd);
			POINT pt, mt;
			::GetCursorPos(&pt);
			memcpy(&mt, &pt, sizeof(POINT));
			::ScreenToClient(hWnd, &pt);
			RECT wr; memset(&wr, 0, sizeof(RECT));
			::GetWindowRect(hWnd, &wr);
			LONG w_w, w_h;
			w_w = wr.right - wr.left;
			w_h = wr.bottom - wr.top;
			//top
			if (pt.y <= 30) {
				HMENU hMenu = GetSystemMenu(hWnd, 0);
				::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
					mt.x, mt.y, 0, hWnd, NULL);

				break;
			}

			if (!hMenu) return 0;
			::TrackPopupMenu(::GetSubMenu(hMenu, 0), TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,
				mt.x, mt.y, 0, hWnd, NULL);
		}
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			case IDM_ABOUT:
			case ID_MENU_32792:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
			case 1003:
				//DestroyWindow(hWnd);
				SendMessageW(hWnd, WM_CLOSE, 0, 0);
				break;
			case ID_MENU_32773:
			{
				wchar_t ti[256] = { 0 };
				::GetWindowTextW(hWnd, ti, 255);
				errno_t e = ::CopyText(ti);
				if (e) {
					stringstream ss;
					ss << "错误: 在" << CopyText(e) << "处发生错误,内部代码 " << e <<
						" ,GetLastError() == " << GetLastError();
					::MessageBoxA(hWnd, ss.str().c_str(), "Error", MB_ICONERROR);
				}
				else {
					::MessageBoxA(hWnd, "已复制。", "Copy", MB_ICONINFORMATION);
				}
				break;
			}
			case ID_MENU_32772:
			{
				errno_t e = ::CopyText(onclick_action.value);
				if (e) {
					stringstream ss;
					ss << "错误: 在" << CopyText(e) << "处发生错误,内部代码 " << e <<
						" ,GetLastError() == " << GetLastError();
					::MessageBoxA(hWnd, ss.str().c_str(), "Error", MB_ICONERROR);
				}
				else {
 					::MessageBoxA(hWnd, "已复制。", "Copy", MB_ICONINFORMATION);
				}
				break;
			}
			case ID_MENU_32775: //关闭
				::SendMessageW(hWnd, WM_CLOSE, 0, 0);
				break;
			case ID_MENU_32782:
			case ID_MENU_32784:
			case ID_MENU_32785:
			case ID_MENU_32788:
			case ID_MENU_32794:
				if (window_configs.bin_path.set.empty()) {
					::MessageBoxW(hWnd, L"未设定设置程序,无法进行配置!", L"Error", MB_ICONERROR);
					break;
				}
				if (wmId == ID_MENU_32782) {
					wstring str = s2ws(window_configs.bin_path.set);
					//--image=imagepath --open=value
					str = L"\"" + str + L"\" \"--image=" + s2ws(window_configs.img_path) +
						L"\" \"--open=" + s2ws(onclick_action.value) + L"\"";
					Process.Start(str);
				}
				break;
			case ID_MENU_32771:
				ExecOpenFunction();
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_CLOSE: 
			::DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		DestroyMenu(hMenu);
		DestroyWindow(hBtnClose);
		//DestroyWindow(hImgShow);
		cv::destroyWindow("WinImageShow1");
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
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

void ExecOpenFunction(void) {
	switch (onclick_action.action) {
	case onclick_action.OPEN_FILE:
	{
		string str = "explorer.exe \"" + onclick_action.value + "\"";
		Process.StartOnly(s2ws(str));
		break;
	}
	case onclick_action.EXEC_FILE:
	{
		if (!Process.StartOnly(s2ws(onclick_action.value))) {
			::MessageBoxW(NULL, (L"错误: 无法CreateProcessW(),GetLastError()==" +
				to_wstring(GetLastError())).c_str(), L"Error", MB_ICONERROR); return;
		}
		break;
	}
	case onclick_action.OPEN_URL:
	{
		string str = onclick_action.value;
		if (str.substr(0, 7) != "http://" && str.substr(0, 8) != "https://" && 
			str.substr(0, 6) != "ftp://") str = "http://" + str;
		str = "explorer.exe \"" + str + "\"";
		if (!Process.StartOnly(s2ws(str))) {
			::MessageBoxW(NULL, (L"错误: 无法CreateProcessW(),GetLastError()==" +
				to_wstring(GetLastError())).c_str(), L"Error", MB_ICONERROR); return;
		}
		break;
	}
	default:
		::MessageBoxW(NULL, L"错误: 内部错误", L"Error", MB_ICONERROR); return;
	}
}

void ExecOpenFunction_cvmscb(int event, int x, int y, int flags, void* userdata) {
	switch (event) {
	case CV_EVENT_LBUTTONUP:
		ExecOpenFunction();
		break;
	default:;
	}
}

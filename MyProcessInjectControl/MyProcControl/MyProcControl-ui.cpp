#include <Windows.h>
#include "MyProcControl-ui.h"
#include<map>
#include<string>
#include "resource.h"
#include "../../resource/tool.h"
using namespace std;

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WndProc_SvcCtrlWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_USER + 44: {
		WCHAR wclass_icon[256]{ 0 };
		LoadStringW(0, IDS_UI_ICONWND_CLASS, wclass_icon, 255);
		DestroyWindow(FindWindowW(wclass_icon, NULL));
		//ExitProcess((UINT)wParam);
		PostQuitMessage((int)wParam);
	}
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

typedef struct {
	NOTIFYICONDATAW nif;
	WCHAR SvcName[64];
} type_WndProc_TrayIconWindow;
static std::map<HWND, type_WndProc_TrayIconWindow> TrayIconData;
LRESULT CALLBACK WndProc_TrayIconWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static UINT WM_TaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));
	if (msg == WM_CREATE || msg == WM_TaskbarCreated) {
		type_WndProc_TrayIconWindow tt{ 0 };
		wstring svcname;
		try {
			svcname = TrayIconData.at(hWnd).SvcName;
		}
		catch (...) {}
		if (!svcname.empty()) wcscpy_s(tt.SvcName, svcname.c_str());
		NOTIFYICONDATAW nd{ 0 };
		nd.cbSize = sizeof(nd);
		nd.hWnd = hWnd;
		nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
		nd.uCallbackMessage = WM_USER + 13;
		nd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcscpy_s(nd.szTip, L"MyProcControl User Interface");
		tt.nif = nd;
		TrayIconData[hWnd] = tt;
		Shell_NotifyIconW(NIM_ADD, &nd);
		if (!svcname.empty()) {
			SendMessage(hWnd, WM_USER + 15, (WPARAM)tt.SvcName, 0);
		}
	}

	switch (msg) {
	case WM_CREATE:
		// do something...
		//MessageBoxW(hWnd, to_wstring(WM_TaskbarCreated).c_str(), L"test", 0);
		break;

	case WM_USER+13:
		switch (lParam) {
		case WM_LBUTTONUP: {
			WCHAR filename[MAX_PATH]{ 0 };
			LoadStringW(NULL, IDS_UI_FILENAME_PREFIX, filename, MAX_PATH-8);
#ifdef _WIN64
			wcscat_s(filename, L"64.exe");
#else
			wcscat_s(filename, L"32.exe");
#endif
			wstring cmdl = L"--ui";
			if (TrayIconData[hWnd].SvcName[0]) cmdl += 
				L" --from-service --service-name=\""s +
				TrayIconData[hWnd].SvcName + L"\"";
			ShellExecuteW(NULL, L"open", filename,
				cmdl.c_str(), NULL, SW_NORMAL);
			break;
		}

		case WM_RBUTTONUP: {
			POINT pt = { 0 }; int resp = 0;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);

			constexpr size_t IDR_OPEN = 1;
			constexpr size_t IDR_PAUSE = 6;
			constexpr size_t IDR_RESUME = 7;
			constexpr size_t IDR_CFG = 2;
			constexpr size_t IDR_ABOUT = 3;
			constexpr size_t IDR_EXIT = 4;
			constexpr size_t IDR_EXITSVC = 5;
			HMENU hIconMenu = CreatePopupMenu();
			AssertEx_AutoHandle(hIconMenu);

			AppendMenu(hIconMenu, MF_STRING, IDR_OPEN, _T("&Open User Interface"));
			SetMenuDefaultItem(hIconMenu, IDR_OPEN, FALSE);
			if (TrayIconData[hWnd].SvcName[0]) {
				AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
				AppendMenu(hIconMenu, MF_STRING, IDR_PAUSE, _T("&Pause control"));
				AppendMenu(hIconMenu, MF_STRING, IDR_RESUME, _T("Resume control"));
			}
			AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
			if (TrayIconData[hWnd].SvcName[0])
				AppendMenu(hIconMenu, MF_STRING, IDR_CFG, _T("&Config"));
			AppendMenu(hIconMenu, MF_STRING, IDR_ABOUT, _T("&About"));
			AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
			AppendMenu(hIconMenu, MF_STRING, IDR_EXIT, _T("&Hide Taskbar Icon"));
			if (TrayIconData[hWnd].SvcName[0]) AppendMenu(hIconMenu, MF_STRING,
				IDR_EXITSVC, _T("&Exit and Stop Control"));
			resp = TrackPopupMenu(hIconMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
				pt.x, pt.y, NULL, hWnd, NULL);

			DestroyMenu(hIconMenu);

			if (resp == IDR_OPEN) {
				SendMessage(hWnd, msg, 0, WM_LBUTTONUP);
			}
			if (resp == IDR_EXIT) {
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			if (resp == IDR_EXITSVC) {
				HANDLE hPipe = NULL;
				WCHAR pipe_name[256]{ 0 };
				DWORD tmp = 0;
				LoadStringW(NULL, IDS_SVC_SVCTL_PIPE_NAME, pipe_name, 255);
				wcscat_s(pipe_name, L"\\");
				wcscat_s(pipe_name, TrayIconData[hWnd].SvcName);
				if (::WaitNamedPipeW(pipe_name, NMPWAIT_WAIT_FOREVER)) {
					hPipe = ::CreateFileW(pipe_name, GENERIC_WRITE, 0,
						0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
					if (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
						MessageBoxW(NULL, LastErrorStrW().c_str(), NULL, MB_ICONHAND);
						break;
					}
					::WriteFile(hPipe, "Service Stop user_confirm", 26, &tmp, NULL);
					::CloseHandle(hPipe);
				}
				break;
			}
			if (resp == IDR_PAUSE) ShellExecuteW(NULL, L"runas", L"sc.exe",
				(L"pause \""s + TrayIconData[hWnd].SvcName + L"\"").c_str(),
				NULL, SW_HIDE);
			if (resp == IDR_RESUME) ShellExecuteW(NULL, L"runas", L"sc.exe",
				(L"continue \""s + TrayIconData[hWnd].SvcName + L"\"").c_str(),
				NULL, SW_HIDE);

			break;
		}
			break;

		default:
			break;
		}
		break;

	case WM_USER + 15: {
		wcscpy_s(TrayIconData[hWnd].nif.szTip, (L"["s + ((PCWSTR)wParam) +
			L"] - MyProcControl User Interface").c_str());
		Shell_NotifyIconW(NIM_MODIFY, &(TrayIconData[hWnd].nif));
		if (TrayIconData[hWnd].SvcName != (PCWSTR)wParam)
			wcscpy_s(TrayIconData[hWnd].SvcName, ((PCWSTR)wParam));
	}
		break;

	case WM_CLOSE:
		if (TrayIconData[hWnd].nif.cbSize) {
			Shell_NotifyIconW(NIM_DELETE, &(TrayIconData[hWnd].nif));
			TrayIconData[hWnd].nif.cbSize = 0;
		}
		if (TrayIconData[hWnd].SvcName[0] == L'\0')
			DestroyWindow(hWnd);
		break;

	case WM_DESTROY: {
		if (TrayIconData[hWnd].nif.cbSize) {
			Shell_NotifyIconW(NIM_DELETE, &(TrayIconData[hWnd].nif));
			TrayIconData[hWnd].nif.cbSize = 0;
		}
		try { TrayIconData.erase(hWnd); }
		catch (std::exception&) {};
		PostQuitMessage(0);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK WndProc_SetupWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		CreateWindowExW(0, L"Static", L"Click [Install] to install\n"
			"[MyProcControlSvc] to your computer.",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, 10, 265, 40, hWnd, 0, 0, 0);
		HWND hbInstall = CreateWindowExW(0, L"Button", L"Install [Enter]",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			10, 60, 130, 40, hWnd, (HMENU)IDOK, 0, 0);
		CreateWindowExW(0, L"Button", L"Cancel [ESC]",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			145, 60, 130, 40, hWnd, (HMENU)IDCANCEL, 0, 0);
		//MessageBoxW(NULL, LastErrorStrW().c_str(), 0, 0);
		if (hbInstall) {
			HMODULE imageres = LoadLibrary(_T("imageres.dll"));
			if (imageres) {
				HICON icon = LoadIcon(imageres, MAKEINTRESOURCE(78));
				SendMessage(hbInstall, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
			}
		}
	}
		break;

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDOK:
			ShellExecuteW(hWnd, L"runas", s2ws(GetProgramDir()).c_str(),
				L"--install --svc-name=\"MyProcControlSvc\" --display-"
				"name=\"My Process Control Service\" --startup-type=auto"
				" --start", NULL, SW_HIDE);
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	}

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) return SendMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
		if (wParam == VK_RETURN) return SendMessage(hWnd, WM_COMMAND, IDOK, 0);
	case WM_KEYUP:
	{
		WNDCLASSEXW wcex{ 0 };
		wcex.cbSize = sizeof(wcex);
		GetClassInfoExW(NULL, L"#32770", &wcex);
		if (wcex.lpfnWndProc) return wcex.lpfnWndProc(hWnd, msg, wParam, lParam);
		else return DefWindowProc(hWnd, msg, wParam, lParam);
	}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		ExitProcess(0);
		PostQuitMessage(0);
		break;

	default: 
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void RegClass_BackgroundLayeredAlphaWindowClass() {
	WNDCLASSEXW wcex{ 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.lpszClassName = L"BackgroundLayeredAlphaWindowClass";
	wcex.hCursor = LoadCursor(NULL, IDC_NO);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpfnWndProc = WndProc_BackgroundLayeredAlphaWindowClass;
	s7::MyRegisterClassW(0, 0, wcex);
}

LRESULT CALLBACK WndProc_BackgroundLayeredAlphaWindowClass(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)
			| WS_EX_LAYERED | WS_EX_TOOLWINDOW & ~WS_EX_APPWINDOW);
		SetLayeredWindowAttributes(hWnd, 0, 127, LWA_ALPHA);
		RECT rc{ 0 };
		HWND hParent = GetParent(hWnd);
		if (hParent) GetWindowRect(hParent, &rc);
		else {
			rc.right = GetSystemMetrics(SM_CXSCREEN);
			rc.bottom = GetSystemMetrics(SM_CYSCREEN);
		}
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
			rc.right - rc.left, rc.bottom - rc.top, 0);

		SetWindowLong(hWnd, GWL_STYLE,
			(GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION)
			& ~WS_SIZEBOX & ~WS_BORDER);
	}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		break;

	default: 
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

static bool UserConsentHelper_handler(/*HANDLE in, HANDLE out*/) {
	cout << "233" << endl;
	return true;
}
DWORD WINAPI UserConsentHelperProc(PVOID) {
	//HANDLE
	//	sin = GetStdHandle(STD_INPUT_HANDLE),
	//	sout = GetStdHandle(STD_OUTPUT_HANDLE);
	//DWORD nRead = 0;
	constexpr UINT buf_size = 16384 * sizeof(CHAR);
	PSTR pBuf = (PSTR)calloc(buf_size, 1);
	if (!pBuf) return ERROR_OUTOFMEMORY;

	//while (ReadFile(sin, pBuf, buf_size, &nRead, NULL)) {
	while (fread(pBuf, 1, buf_size, stdin)) {
		if (!UserConsentHelper_handler(/*sin, sout*/)) break;
	}

	free(pBuf);
	return 0;
}
DWORD WINAPI UserConsentHelperDebug(PVOID) {
	AllocConsole();
	HANDLE in = NULL, out = NULL;
	if (!CreatePipe(&out, &in, NULL, 8192)) {
		cerr << "ERROR: Cannot CreatePipe\n";
		return GetLastError();
	}

	STARTUPINFO si{ 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = in;
	si.hStdOutput = si.hStdError = out;

	PROCESS_INFORMATION pi = Process.Start(to__str(
		"\"" + GetProgramDir() + "\" --UserConsentHelper"), si);
	if (!pi.hProcess) {
		CloseHandle(in); CloseHandle(out);
		cerr << "ERROR: Cannot CreateProcess!\n";
		return GetLastError();
	}
	CloseHandle(pi.hThread);

	DWORD nRead = 0;
	PCHAR buffer = (PSTR)calloc(8192, sizeof(CHAR));
	if (!buffer) {
		CloseHandle(in); CloseHandle(out);
		CloseHandle(pi.hProcess);
		cerr << "ERROR: Cannot alloc memory!\n";
		return ERROR_OUTOFMEMORY;
	}
	while (cin.getline(buffer, 8192)) {
		WriteFile(in, buffer, (DWORD)strlen(buffer), &nRead, NULL);
		if (!ReadFile(out, buffer, 8192, &nRead, NULL)) break;
		printf("%s", buffer);
	}
	Process.kill(pi.hProcess);
	CloseHandle(in); CloseHandle(out);
	CloseHandle(pi.hProcess);
	free(buffer);

	return 0;
}

// @deprecated This function is deprecated.
#if 0
LRESULT WndProc_Dlg_Main(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:

		break;

	case WM_SIZING:
	case WM_SIZE:
	{
		RECT rc{ 0 }; GetWindowRect(hWnd, &rc);
		RECT crc{ 0 }; GetClientRect(hWnd, &crc);
		SetWindowPos(GetDlgItem(hWnd, IDC_STATIC_MAIN_GROUP), 0, 1, 1,
			crc.right - crc.left - 2, crc.bottom - crc.top - 2, SWP_NOZORDER);
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
#endif


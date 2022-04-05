#include <Windows.h>
#include "MyProcControl-ui.h"
#include<map>
#include<string>
#include "resource.h"
#include "../../resource/tool.h"
#include "../utils/base64helper/base64_2.h"
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
LRESULT CALLBACK WndProc_TrayIconWindow
	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static UINT WM_TaskbarCreated = RegisterWindowMessage(TEXT("TaskbarCreated"));
#if 0
	if (msg == WM_CREATE) {
		type_WndProc_TrayIconWindow tt{ 0 };
		//wstring svcname;
		//try {
		//	svcname = TrayIconData.at(hWnd).SvcName;
		//}
		//catch (...) {}
		//if (!svcname.empty()) wcscpy_s(tt.SvcName, svcname.c_str());
		NOTIFYICONDATAW nd{ 0 };
		nd.cbSize = sizeof(nd);
		nd.hWnd = hWnd;
		nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
		nd.uCallbackMessage = WM_USER + 13;
		nd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_MAIN));
		wcscpy_s(nd.szTip, L"MyProcControl User Interface");
		tt.nif = nd;
		TrayIconData[hWnd] = tt;
		Shell_NotifyIconW(NIM_ADD, &nd);
		//if (!svcname.empty()) {
		//	SendMessage(hWnd, WM_USER + 15, (WPARAM)tt.SvcName, 0);
		//}
	}
#endif
	if (msg == WM_TaskbarCreated) Shell_NotifyIconW(NIM_ADD, &(TrayIconData[hWnd].nif));

	switch (msg) {
	case WM_CREATE:
		// do something...
		{
			type_WndProc_TrayIconWindow tt{ 0 };
			NOTIFYICONDATAW nd{ 0 };
			nd.cbSize = sizeof(nd);
			nd.hWnd = hWnd;
			nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
			nd.uCallbackMessage = WM_USER + 13;
			nd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MAIN));
			wcscpy_s(nd.szTip, L"MyProcControl User Interface");
			tt.nif = nd;
			TrayIconData[hWnd] = tt;
			Shell_NotifyIconW(NIM_ADD, &nd);
		}
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
			ShellExecuteW(NULL, (wParam == 2) ? L"runas" : L"open", filename,
				cmdl.c_str(), NULL, SW_NORMAL);
			break;
		}

		case WM_RBUTTONUP: {
			POINT pt = { 0 }; int resp = 0;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);

			constexpr size_t IDR_OPEN = 1;
			constexpr size_t IDR_CFG = 2;
			constexpr size_t IDR_ABOUT = 3;
			constexpr size_t IDR_EXIT = 4;
			constexpr size_t IDR_EXITSVC = 5;
			constexpr size_t IDR_PAUSE = 6;
			constexpr size_t IDR_RESUME = 7;
			constexpr size_t IDR_UNINST = 8;
			constexpr size_t IDR_RUNASUI = 9;
			constexpr size_t IDR_RPWL = 10;
			HMENU hIconMenu = CreatePopupMenu();
			AssertEx_AutoHandle(hIconMenu);

			AppendMenu(hIconMenu, MF_STRING, IDR_OPEN, _T("&Open User Interface"));
			AppendMenu(hIconMenu, MF_STRING, IDR_RUNASUI, _T("Run&As User Interface"));
			SetMenuDefaultItem(hIconMenu, IDR_OPEN, FALSE);
			if (TrayIconData[hWnd].SvcName[0]) {
				AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
				AppendMenu(hIconMenu, MF_STRING, IDR_RPWL, _T("Run Program with &Limits"));
				AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
				AppendMenu(hIconMenu, MF_STRING, IDR_PAUSE, _T("&Pause control"));
				AppendMenu(hIconMenu, MF_STRING, IDR_RESUME, _T("&Resume control"));
			}
			AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
			if (TrayIconData[hWnd].SvcName[0])
				AppendMenu(hIconMenu, MF_STRING, IDR_CFG, _T("&Config"));
			AppendMenu(hIconMenu, MF_STRING, IDR_ABOUT, _T("A&bout"));
			if (TrayIconData[hWnd].SvcName[0]) {
				AppendMenu(hIconMenu, MFT_SEPARATOR, 0, 0);
				AppendMenu(hIconMenu, MF_STRING, IDR_UNINST, _T("&Uninstall"));
			}
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
			else if (resp == IDR_RUNASUI) {
				SendMessage(hWnd, msg, 2, WM_LBUTTONUP);
			}
			else if (resp == IDR_RPWL) {
				wstring cmdl = L"--ui --run-program-with-limits";
				if (TrayIconData[hWnd].SvcName[0]) cmdl +=
					L" --from-service --service-name=\""s +
					TrayIconData[hWnd].SvcName + L"\"";
				ShellExecuteW(NULL, L"open", s2wc(GetProgramDir()), cmdl.c_str(), 0, 1);
			}
			else if (resp == IDR_ABOUT || resp == IDR_CFG) {
				WCHAR filename[MAX_PATH]{ 0 };
				LoadStringW(NULL, IDS_UI_FILENAME_PREFIX, filename, MAX_PATH - 8);
#ifdef _WIN64
				wcscat_s(filename, L"64.exe");
#else
				wcscat_s(filename, L"32.exe");
#endif
				wstring cmdl;
				if (resp == IDR_ABOUT) cmdl = L"/about";
				else if (resp == IDR_ABOUT) cmdl = L"/config";
				if (TrayIconData[hWnd].SvcName[0]) cmdl +=
					L" --from-service --service-name=\""s +
					TrayIconData[hWnd].SvcName + L"\"";
				ShellExecuteW(NULL, L"open", filename,
					cmdl.c_str(), NULL, SW_NORMAL);
			}
			else if (resp == IDR_UNINST) {
				wstring keyname = L"SOFTWARE\\Microsoft\\Windows\\"
					"CurrentVersion\\Uninstall\\";
				keyname += TrayIconData[hWnd].SvcName;
				HKEY hk = NULL;
				if (0 == RegOpenKeyExW(HKEY_LOCAL_MACHINE,
					keyname.c_str(), 0, KEY_READ, &hk)) {
					DWORD type = REG_SZ;
					DWORD cbData = 2048;
					PBYTE data = (PBYTE)calloc(cbData, sizeof(BYTE));
					if (data) {
						if (RegQueryValueExW(hk, L"UninstallString", NULL,
							&type, data, &cbData) == 0 && type == REG_SZ) {
							Process.StartOnly((PCWSTR)data);
							break;
						}
						free(data);
					}
					RegCloseKey(hk);
				}
				MessageBoxW(NULL, L"Cannot uninstall because could not read"
					" uninstall data.", L"Cannot uninstall", MB_ICONHAND);
			}
			else if (resp == IDR_EXIT) {
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else if (resp == IDR_EXITSVC) {
				HANDLE hPipe = NULL;
				WCHAR pipe_name[256]{ 0 };
				DWORD tmp = 0;
				LoadStringW(NULL, IDS_SVC_SVCTL_PIPE_NAME, pipe_name, 255);
				wcscat_s(pipe_name, L"\\");
				wcscat_s(pipe_name, TrayIconData[hWnd].SvcName);
				if (::WaitNamedPipeW(pipe_name, 5000)) {
					hPipe = ::CreateFileW(pipe_name, GENERIC_WRITE, 0,
						0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
					if (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
						MessageBoxW(NULL, LastErrorStrW().c_str(), NULL, MB_ICONHAND);
						break;
					}
					::WriteFile(hPipe, "Service Stop user_confirm", 26, &tmp, NULL);
					::CloseHandle(hPipe);
				} else {
					MessageBoxW(NULL, ErrorCodeToStringW(ERROR_TIMEOUT).c_str(),
						NULL, MB_ICONHAND);
				}
				break; 
			}
			else if (resp == IDR_PAUSE) ShellExecuteW(NULL, L"runas", L"sc.exe",
				(L"pause \""s + TrayIconData[hWnd].SvcName + L"\"").c_str(),
				NULL, SW_HIDE);
			else if (resp == IDR_RESUME) ShellExecuteW(NULL, L"runas", L"sc.exe",
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

//static std::map<HWND, HWND> Wnds_SetupWindow;
LRESULT CALLBACK WndProc_SetupWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		CreateWindowExW(0, L"Static", L"Click [Install] to install\n"
			"[MyProcControlSvc] to your computer.",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, 10, 265, 40, hWnd, 0, 0, 0);

		CreateWindowExW(0, L"Static", L"Service Name:",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, 60, 70, 40, hWnd, 0, 0, 0);
		//Wnds_SetupWindow[hWnd] = 
		CreateWindowExW(0, L"Edit", L"MyProcControlSvc",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_WANTRETURN,
			90, 60, 185, 40, hWnd, 0, 0, 0);
		//if (!Wnds_SetupWindow[hWnd]) {
		//	MessageBoxW(NULL, L"Cannot create window!", NULL, MB_ICONHAND);
		//	DestroyWindow(hWnd);
		//}

		HWND hbInstall = CreateWindowExW(0, L"Button", L"Install [Enter]",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 60+50, 130, 40, hWnd, (HMENU)IDOK, 0, 0);
		CreateWindowExW(0, L"Button", L"Cancel [ESC]",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_TABSTOP,
			145, 60+50, 130, 40, hWnd, (HMENU)IDCANCEL, 0, 0);
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
		case IDOK: {
			WCHAR svcname[64]{ 0 };
			GetWindowTextW(FindWindowEx(hWnd, NULL, L"Edit", NULL), svcname, 63);
			if (svcname[0] == L'\0') {
				MessageBoxW(hWnd, L"Please input service name!", NULL, MB_ICONHAND);
				break;
			}

			CHAR fname[MAX_PATH + 1]{ 0 };
			GetTempPathA(200, fname);
			GetTempFileNameA(string(fname).c_str(), "", 0, fname);
			if (!FreeResFile(
#ifdef _WIN64
				IDR_BIN_DLL_EXPORTS_x64
#else
				IDR_BIN_DLL_EXPORTS_x86
#endif
				, "BIN", fname)) {
				MessageBoxW(hWnd, (L"Cannot release resource file!!\n"
					L"Error " + to_wstring(GetLastError()) + L": " +
					LastErrorStrW()).c_str(), NULL, MB_ICONHAND);
				break;
			}
			WCHAR current_dir[MAX_PATH]{ 0 };
			GetCurrentDirectoryW(MAX_PATH - 1, current_dir);

			SHELLEXECUTEINFO execinfo{ 0 };
			execinfo.hwnd = hWnd;
			execinfo.lpFile = L"rundll32.exe";
			execinfo.cbSize = sizeof(execinfo);
			execinfo.lpVerb = IsRunAsAdmin() ? L"open" : L"runas";
			execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			execinfo.nShow = SW_SHOWDEFAULT;
			wstring szParam =
				(L"\""s + s2ws(fname).c_str() + L"\",MRundInstall "
				"--install --svc-name=\"" + svcname + L"\" --display-"
				"name=\"My Process Control Service\" --startup-type=auto"
				" --binPath=\"" + s2ws(GetProgramDir()) + L"\""
				" --start --spawnwnd=" + to_wstring((UINT_PTR)hWnd));
			execinfo.lpParameters = szParam.c_str();
			execinfo.lpDirectory = current_dir;
			//ShellExecuteW(hWnd, L"runas", L"rundll32", L"", current_dir, SW_HIDE);
			if (!ShellExecuteExW(&execinfo)) break;
			EnableWindow(hWnd, FALSE);
#pragma warning(push)
#pragma warning(disable: 26495)
			typedef struct {
				HWND hWnd;
				HANDLE hProcess;
				wstring file_name;
			} mystruct;
#pragma warning(pop)
			mystruct* data = new mystruct;
			data->hWnd = hWnd;
			data->hProcess = execinfo.hProcess;
			data->file_name = s2ws(fname);
			HANDLE h2 = CreateThread(0, 0, [](PVOID p)->DWORD {
				mystruct* my = (mystruct*)p;
				HANDLE hProcess = my->hProcess;
				WaitForSingleObject(hProcess, INFINITE);
				DWORD exitCode = 0;
				GetExitCodeProcess(hProcess, &exitCode);
				CloseHandle(hProcess);
				EnableWindow(my->hWnd, TRUE);
				DeleteFileW(my->file_name.c_str());
				delete my;
				return 0;
			}, data, 0, 0);
			if (h2) CloseHandle(h2);

			break;
		}
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
		if (wParam == VK_TAB) return (BOOL)(INT_PTR)
			SetFocus(FindWindowEx(hWnd, GetFocus(), NULL, NULL));
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
		//try { Wnds_SetupWindow.erase(hWnd); }
		//catch (...) {}
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
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)
			| WS_EX_LAYERED/* | WS_EX_NOACTIVATE*/ | WS_EX_TOOLWINDOW
			& ~WS_EX_APPWINDOW);
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

		SetWindowLongPtr(hWnd, GWL_STYLE,
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
	using __base64_decode = size_t(__stdcall*)
		(const uint8* code, size_t code_len, char* str);
	WCHAR dll_path[MAX_PATH + 2]{ 0 };
	GetTempPathW(MAX_PATH - 14, dll_path);
	GetTempFileNameW(wstring(dll_path).c_str(),
		L"", 0, dll_path/* + wcslen(dll_path)*/);
	if (dll_path[0] == L'\0') return 1;
	//MessageBoxW(NULL, dll_path, L"test", 0);
#ifdef _WIN64
	if (!FreeResFile(IDR_BIN_DLL_EXPORTS_x64, L"BIN", dll_path))
#else
	if (!FreeResFile(IDR_BIN_DLL_EXPORTS_x86, L"BIN", dll_path))
#endif
		return GetLastError();

	HMODULE dll_utils = LoadLibraryW(dll_path);
	if (!dll_utils) return GetLastError();
	__base64_decode base64_decode = (__base64_decode)
		GetProcAddress(dll_utils, "base64_decode");
	if (!base64_decode) return ERROR_FILE_CORRUPT;

	CmdLineA cl(GetCommandLineA());
	string datas_to_parse;
	if (cl.getopt("data", datas_to_parse) == 1) {
#if 0
		// Example data:
		RXZlbnQ9Q3JpdGljYWwKUHJvY2Vzc05h
		bWU9Y21kLmV4ZQpQcm9jZXNzSWQ9OApQ
		cm9jZXNzUGF0aD1DOlxjbWQuZXhlCmRl
		dGFpbHNfdGV4dD1WR2hsSUhCeWIyTmxj
		M01nYVhNZ2RISjVhVzVuSUhSdklHdHBi
		R3dnZVc5MWNpQlBVeTQ9CkFjY2VwdEV4
		aXRDb2RlPTIzMw==
#endif
		//Base64 base64;
		string datas;
		datas.resize(datas_to_parse.length());
		base64_decode((const uint8*)datas_to_parse.c_str(),
			(datas_to_parse.length()), ((PSTR)datas.c_str()));
		vector<string> rows;
		str_split(datas, "\n", rows);
		if (rows.size() < 2) return ERROR_INVALID_PARAMETER;
		string
			ent_imp,   // Event Importance
			app_path,
			proc_name, // Process name
			proc_id,   // Process id
			sz_pop,    // program operation
			sz_opts,   // options
			details,
			nAcceptExitCode;
		for (const auto& i : rows) {
			if (i.find("Event=") == 0) {
				ent_imp = i.substr(6);
				continue;
			} else
			if (i.find("ProcessName=") == 0) {
				proc_name = i.substr(12);
				continue;
			}
			if (i.find("ProcessId=") == 0) {
				proc_id = i.substr(10);
				continue;
			}
			if (i.find("ProcessPath=") == 0) {
				app_path = i.substr(12);
				continue;
			}
			if (i.find("ProcessOp=") == 0) {
				sz_pop = i.substr(10);
				continue;
			}
			if (i.find("OptionsToDo=") == 0) {
				sz_opts = i.substr(12);
				continue;
			}
			if (i.find("details_text=") == 0) {
				string str = i.substr(13);
				string buffer;
				buffer.resize(str.length());
				base64_decode((const uint8*)str.c_str(),
					str.length(), (PSTR)buffer.c_str());
				details = buffer;
				continue;
			}
			if (i.find("AcceptExitCode=") == 0) {
				nAcceptExitCode = i.substr(15);
				continue;
			}
		}

		if(1)MessageBoxW(NULL, to__str(
			"event=" + ent_imp + "\n"
			"app_path=" + app_path + "\n"
			"procname=" + proc_name + "\n"
			"procid=" + proc_id + "\n"
			"pop=" + sz_pop + "\n"
			"opts=" + sz_opts + "\n"
			"details=" + details
		).c_str(), L"Debug", 0);

		//s7::MyRegisterClassW()

		FreeLibrary(dll_utils);
		DeleteFileW(dll_path);

		return 0;
	}
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

LRESULT CALLBACK WndProc_DesktopHelper(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
	switch (msg) {
	case WM_CREATE:
		CreateWindow(_T("BUTTON"), _T("Click to return to default desktop"),
			WS_CHILD | WS_VISIBLE | BS_CENTER, 10, 10, 300, 30, hWnd,
			(HMENU)33, NULL, NULL);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == 33) {
			HDESK desk = OpenDesktop(TEXT("default"), DF_ALLOWOTHERACCOUNTHOOK,
				FALSE, GENERIC_READ | GENERIC_EXECUTE);
			if (!desk) MessageBoxW(hWnd, L"Something's wrong", NULL, MB_ICONWARNING);
			else {
				SetThreadDesktop(desk);
				SwitchDesktop(desk);
				CloseDesktop(desk);
				DestroyWindow(hWnd);
			}
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);

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

typedef struct {
	WCHAR service_name[256];
	HFONT font, font2;
	HWND hStatic, b1, b2, b3, h5, hTitle,
		hStatic_CurrentStateP, hStatic_CurrentState,
		hProgress0, bQuickUninst, bDeleteAll;
	HWND hPrepar;
	bool quick, unclosable, deleteapp;
} Wnds_UninstWindow_t;
static map<HWND, Wnds_UninstWindow_t> Wnds_UninstWindow;
#define UpdateChildWindowData(hwnd) (SetWindowPos(hwnd, 0, 0, 0, 0, 0,\
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED))

static DWORD WINAPI Uninst_Proc(PVOID _hwnd) {
	HWND hwnd = (HWND)_hwnd;
	Wnds_UninstWindow_t& dat = Wnds_UninstWindow[hwnd];
	//EnableWindow(hwnd, 0); // It already disabled in WndProc
	CenterWindow(dat.hPrepar);
	ShowWindow(dat.hPrepar, 1);
	
	ShowWindow(dat.hStatic, 0);
	ShowWindow(dat.h5, 0);
	ShowWindow(dat.bQuickUninst, 0);
	ShowWindow(dat.bDeleteAll, 0);
	ShowWindow(dat.hStatic_CurrentStateP, 1);
	ShowWindow(dat.hStatic_CurrentState, 1);
	ShowWindow(dat.hProgress0, 1);
	SetWindowText(dat.hTitle, _T("Uninstalling..."));
	UpdateChildWindowData(dat.hTitle);
	SetWindowLongPtr(dat.hProgress0, GWL_STYLE,
		GetWindowLongPtr(dat.hProgress0, GWL_STYLE) | PBS_MARQUEE);
	UpdateChildWindowData(dat.hProgress0);
	SendMessage(dat.hProgress0, PBM_SETMARQUEE, TRUE, 0);
	dat.quick = dat.quick ? dat.quick :
		SendMessage(dat.bQuickUninst, BM_GETSTATE, 0, 0) & BST_CHECKED;
	dat.deleteapp = dat.deleteapp ? dat.deleteapp :
		SendMessage(dat.bDeleteAll, BM_GETSTATE, 0, 0) & BST_CHECKED;
	
	srand((unsigned)time(0));
	if (!dat.quick) Sleep(rand() % 3000 + 2000);
	//else Sleep(rand() % 100);

	EnableWindow(hwnd, 1);
	EnableWindow(dat.b3, 1);
	SendMessage(dat.hPrepar, WM_CLOSE, 0, 0);
	SetWindowText(dat.hStatic_CurrentState, _T("Preparing"));

	EnableAllPrivileges();
	HKEY hkey = NULL;

	srand((unsigned)time(0));
	if (!dat.quick) Sleep(rand() % 4000 + 1000);

	SendMessage(dat.hProgress0, PBM_SETMARQUEE, FALSE, 0);
	SetWindowLongPtr(dat.hProgress0, GWL_STYLE,
		GetWindowLongPtr(dat.hProgress0, GWL_STYLE) & ~PBS_MARQUEE);
	UpdateChildWindowData(dat.hProgress0);
	SendMessage(dat.hProgress0, PBM_SETRANGE32, 0, 100);

	SetWindowText(dat.hStatic_CurrentState, _T("Verifying installation"));
	if (!dat.quick) Sleep(rand() % 2000 + 1000);
	SendMessage(dat.hProgress0, PBM_SETPOS, 0, 0);

	// Verify installation: service
	SetWindowText(dat.hStatic_CurrentState, _T("Verifying installation: service"));
	if (dat.service_name[0] == L'\0' || RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
		(L"SYSTEM\\CurrentControlSet\\Services\\"s + dat.service_name).c_str(),
		NULL, KEY_READ, &hkey)) {
		MessageBoxW(hwnd, L"The installation is damaged! Please try reinstalling "
			"the program.", NULL, MB_ICONHAND);
		ExitProcess(ERROR_FILE_CORRUPT);
	} else {
		SendMessage(dat.hProgress0, PBM_SETPOS, 10, 0);
		bool install_ok = false;
		DWORD type = REG_EXPAND_SZ;
		DWORD cbData = 2048;
		PBYTE data = (PBYTE)calloc(cbData, sizeof(BYTE));
		if (data) {
			if (RegQueryValueExW(hkey, L"ImagePath", NULL,
				&type, data, &cbData) == 0 &&
				(type == REG_SZ || type == REG_EXPAND_SZ)) {
				if (CmdLineW(wstring((PCWSTR)data))[0] == s2ws(GetProgramDir())) {
					SendMessage(dat.hProgress0, PBM_SETPOS, 20, 0);
					install_ok = true;
				}
			}
			free(data);
		}
		RegCloseKey(hkey);

		if (!install_ok) {
			MessageBoxW(hwnd, L"Cannot verify installation", NULL, MB_ICONHAND);
			ExitProcess(1);
		}
	}
	srand((unsigned)time(0));
	if (!dat.quick) Sleep(rand() % 2500 + 500);

	// Verify installation: 
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall
	SendMessage(dat.hProgress0, PBM_SETPOS, 40, 0);
	SetWindowText(dat.hStatic_CurrentState, _T("Verifying installation: Uninstall"));
	{
		SendMessage(dat.hProgress0, PBM_SETPOS, 60, 0);
		bool install_ok = false;
		wstring keyname = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
		keyname += dat.service_name;
		HKEY hk = NULL;
		if (0 == RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyname.c_str(), 0, KEY_READ, &hk)) {
			DWORD type = REG_SZ;
			DWORD cbData = 2048;
			PBYTE data = (PBYTE)calloc(cbData, sizeof(BYTE));
			if (data) {
				if (RegQueryValueExW(hk, L"UninstallString", NULL,
					&type, data, &cbData) == 0 && type == REG_SZ) {
					if (CmdLineW(wstring((PCWSTR)data))[0] == s2ws(GetProgramDir())) {
						SendMessage(dat.hProgress0, PBM_SETPOS, 80, 0);
						install_ok = true;
					}
				}
				free(data);
			}
			RegCloseKey(hk);
		}

		if (!install_ok) {
			MessageBoxW(hwnd, L"Cannot verify installation", NULL, MB_ICONHAND);
			ExitProcess(1);
		}
	};
	if (!dat.quick) Sleep(rand() % 1500 + 500);
	SetWindowText(dat.hStatic_CurrentState, _T("Finishing: Verifying installation"));
	SendMessage(dat.hProgress0, PBM_SETPOS, 100, 0);
	if (!dat.quick) Sleep(rand() % 600 + rand() % 1200);
	srand((unsigned)time(0));

	SetWindowText(dat.hStatic_CurrentState, _T("Execution"));
	SendMessage(dat.hProgress0, PBM_SETPOS, 0, 0);
	SendMessage(dat.hProgress0, PBM_SETRANGE32, 0, 1000);
	if (!dat.quick) Sleep(rand() % 3500 + 1500);
	srand((unsigned)time(0));
	SetWindowText(dat.hStatic_CurrentState, _T("Deleting"));
	SendMessage(dat.hProgress0, PBM_SETPOS, 5, 0);
	if (!dat.quick) Sleep(500);

	EnableWindow(dat.b3, 0);
	{
		DWORD dwcs = 0;
		SendMessage(dat.hProgress0, PBM_SETPOS, 100, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Connecting to Service Manager"));
		SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!scm) {
			MessageBoxW(hwnd, L"Cannot OpenSCManager", NULL, MB_ICONHAND);
			ExitProcess(1);
		}
		SendMessage(dat.hProgress0, PBM_SETPOS, 120, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Connecting to Service"));
		SC_HANDLE sc = OpenServiceW(scm, dat.service_name, SERVICE_ALL_ACCESS);
		if (!sc) {
			MessageBoxW(hwnd, L"Cannot OpenServiceW", NULL, MB_ICONHAND);
			CloseServiceHandle(scm);
			ExitProcess(1);
		}
		SendMessage(dat.hProgress0, PBM_SETPOS, 140, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Querying service status"));
		SERVICE_STATUS ss{ 0 };
		if (!QueryServiceStatus(sc, &ss)) {
			MessageBoxW(hwnd, L"Cannot QueryServiceStatus", NULL, MB_ICONHAND);
			CloseServiceHandle(sc);
			CloseServiceHandle(scm);
			ExitProcess(1);
		}
		SendMessage(dat.hProgress0, PBM_SETPOS, 160, 0);
		DWORD dwBytesNeeded = 0;
		SERVICE_STATUS_PROCESS ssp{ 0 };
		if (!QueryServiceStatusEx(sc, SC_STATUS_PROCESS_INFO,
			(PBYTE)&ssp, sizeof(ssp), &dwBytesNeeded)) {
			SERVICE_STATUS_PROCESS* pssp =
				(SERVICE_STATUS_PROCESS*)calloc(dwBytesNeeded, 1);
			if (!pssp) {
				MessageBoxW(hwnd, L"Cannot alloc memory!", NULL, MB_ICONHAND);
				CloseServiceHandle(sc);
				CloseServiceHandle(scm);
				ExitProcess(ERROR_OUTOFMEMORY);
			}
			if (!QueryServiceStatusEx(sc, SC_STATUS_PROCESS_INFO,
				(PBYTE)pssp, dwBytesNeeded, &dwBytesNeeded)) {
				MessageBoxW(hwnd, L"Cannot QueryServiceStatusEx", NULL, MB_ICONWARNING);
			}
			RtlCopyMemory(&ssp, pssp, sizeof(SERVICE_STATUS_PROCESS));
			free(pssp);
		}
		dwcs = ss.dwCurrentState;
		SendMessage(dat.hProgress0, PBM_SETPOS, 180, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Checking service status"));
		if (dwcs != SERVICE_STOPPED) {
			SendMessage(dat.hProgress0, PBM_SETPOS, 200, 0);
			SetWindowText(dat.hStatic_CurrentState, _T("Trying to pause the service..."));
			ServiceManager.Pause(ws2s(dat.service_name));
			SendMessage(dat.hProgress0, PBM_SETPOS, 250, 0);
			for (size_t i = 0; i < 20; ++i) {
				SERVICE_STATUS ss{ 0 };
				if (QueryServiceStatus(sc, &ss)) {
					if (ss.dwCurrentState == SERVICE_PAUSED) break;
				}
				SendMessage(dat.hProgress0, PBM_SETPOS, 250 + (i * 5), 0);
				Sleep(500);
			}
			SendMessage(dat.hProgress0, PBM_SETPOS, 350, 0);
			SetWindowText(dat.hStatic_CurrentState, _T("Trying to stop the service..."));
			ServiceManager.Stop(ws2s(dat.service_name));
			SendMessage(dat.hProgress0, PBM_SETPOS, 400, 0);
			if (ssp.dwProcessId) do {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ssp.dwProcessId);
				if (!hProcess) break;
				SetWindowText(dat.hStatic_CurrentState, _T("Waiting"));
				for (size_t i = 0; i < 20; ++i) {
					SendMessage(dat.hProgress0, PBM_SETPOS, 400 + (i * 10), 0);
					if (WAIT_TIMEOUT != WaitForSingleObject(hProcess, 500)) break;
				}

				if (WAIT_TIMEOUT == WaitForSingleObject(hProcess, 1)) {
#define _do_3_times(expr) {expr;expr;expr;}
					_do_3_times(Process.suspend(hProcess));
					DebugSetProcessKillOnExit(TRUE);

					HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
					if (hSnap && hSnap != INVALID_HANDLE_VALUE) {
						PROCESSENTRY32 pe{ 0 };
						pe.dwSize = sizeof(PROCESSENTRY32);
						HANDLE childproc = NULL;
						if (Process32First(hSnap, &pe))
						do {
							if (pe.th32ParentProcessID == ssp.dwProcessId) {
								//MessageBoxW(hwnd, (L"Trying to terminate process "
								//	+ to_wstring(pe.th32ProcessID)).c_str(),
								//	L"[.] uninstall", 0);
								childproc = OpenProcess(
									PROCESS_TERMINATE | PROCESS_SUSPEND_RESUME,
									FALSE, pe.th32ProcessID);
								if (childproc) {
									_do_3_times(Process.suspend(childproc));
									Process.kill(childproc, 0);
									CloseHandle(childproc);
								}
							}
						} while (Process32Next(hSnap, &pe));
						CloseHandle(hSnap);
					} // if (hSnap && hSnap != INVALID_HANDLE_VALUE)

					Process.kill(hProcess, ERROR_TIMEOUT);
					wstring cmd_line = L"\"" + s2ws(GetProgramDir()) +
						L"\" --EndUserInterfaceInstances --service-name=\"" +
						dat.service_name + L"\"";
					Process.StartOnly(cmd_line);
#undef _do_3_times
				} // if (WAIT_TIMEOUT == WaitForSingleObject(hProcess, 1))

				CloseHandle(hProcess);
				 // if (hProcess)
			} while (0); // if (ssp.dwProcessId)
		}
		if ((!DeleteService(sc)) &&
			GetLastError() != ERROR_SERVICE_MARKED_FOR_DELETE)
		{
			MessageBoxW(hwnd, (L"Cannot DeleteService (" + to_wstring(GetLastError())
				+ L"): " + LastErrorStrW()).c_str(), NULL, MB_ICONHAND);
			ExitProcess(GetLastError());
		}
		SendMessage(dat.hProgress0, PBM_SETPOS, 650, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Deleting service"));
		srand((unsigned)time(0));
		if (!dat.quick) Sleep(rand() % 1000 + 500);
		SendMessage(dat.hProgress0, PBM_SETPOS, 680, 0);
		SetWindowText(dat.hStatic_CurrentState, _T("Closing SCM Handle"));
		CloseServiceHandle(sc);
		CloseServiceHandle(scm);
	}

	SendMessage(dat.hProgress0, PBM_SETPOS, 700, 0);
	SetWindowText(dat.hStatic_CurrentState, _T("Deleting [Uninstall] in register"));
	{
		bool install_ok = false;
		wstring keyname = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
		keyname += dat.service_name;
		HKEY hk = NULL;
		if (RegDeleteTreeW(HKEY_LOCAL_MACHINE, keyname.c_str())) {
			MessageBoxTimeoutW(hwnd, L"Cannot RegDeleteTreeW",
				L"Warning - Uninstall", MB_ICONWARNING, 0, 3000);
		}
	};
	SendMessage(dat.hProgress0, PBM_SETPOS, 800, 0);
	srand((unsigned)time(0));
	if (!dat.quick) Sleep(rand() % 1500 + 500);

	SendMessage(dat.hProgress0, PBM_SETPOS, 900, 0);
	SetWindowText(dat.hStatic_CurrentState, _T("Cleaning"));
	// cleaning
	if (dat.deleteapp) do {
		CHAR tmp[MAX_PATH + 2]{ 0 };
		GetTempPathA(MAX_PATH - 18, tmp);
		strcat_s(tmp, ("\\tmp." + to_string(GetCurrentProcessId()) + ".bat").c_str());

		fstream fp(tmp, ios::out);
		if (!fp) break;
		PSTR buffer = (PSTR)calloc(4096, sizeof(CHAR));
		if (!buffer) {
			fp.close();
			break;
		}
		CHAR _filename[MAX_PATH + 1]{ 0 };
		string cd = GetProgramInfo().path; // this string terminator is  '\\\0'
		if (cd.empty()) {
			free(buffer);
			fp.close();
			break;
		}
		LoadStringA(NULL, IDS_UI_FILENAME_PREFIX, _filename, MAX_PATH - 8);
#ifdef _WIN64
		strcat_s(_filename, "64.exe");
#else
		strcat_s(_filename, "32.exe");
#endif
		string uifn = cd + _filename;
		string pself = GetProgramDir();
		sprintf_s(buffer, 4096,
			"@echo off\n"
			":a\n"
			"timeout 3\n"
			"del /f /s /q \"%s\"\n"
			"if exist \"%s\" goto a\n"
			":b\n"
			"timeout 3\n"
			"del /f /s /q \"%s\"\n"
			"if exist \"%s\" goto b\n"
			"del /f /s /q %s"
			, uifn.c_str(), uifn.c_str()
			, pself.c_str(), pself.c_str(), "%0"
		);
		fp << buffer;
		free(buffer);
		fp.close();

		Process.StartOnly_HiddenWindow(to__str("cmd.exe /c \""s + tmp + "\""));
	} while (0);
	if (dat.deleteapp) {
		wstring szAppName = s2ws(GetProgramInfo().name);
		LPCWSTR lpszAppName = szAppName.c_str();
		TSTRING szAppFullPath = to__str(GetProgramDir());
		DWORD dwCurrentPID = GetCurrentProcessId();
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap && hSnap != INVALID_HANDLE_VALUE) {
			PROCESSENTRY32 pe{ 0 };
			pe.dwSize = sizeof(PROCESSENTRY32);
			HANDLE childproc = NULL;
			if (Process32First(hSnap, &pe)) do {
				if (0 == _wcsicmp(lpszAppName, pe.szExeFile)) {
					if (pe.th32ProcessID != dwCurrentPID && Process.
						GetProcessFullPathById(pe.th32ProcessID) ==
						szAppFullPath) {
						HANDLE hProcess = OpenProcess(
							PROCESS_SUSPEND_RESUME | PROCESS_TERMINATE,
							FALSE, pe.th32ProcessID);
						if (hProcess) {
							Process.suspend(hProcess);
							Process.kill(hProcess, 1);
							CloseHandle(hProcess);
						}
					}
				}
			} while (Process32Next(hSnap, &pe));
			CloseHandle(hSnap);
		} // if (hSnap && hSnap != INVALID_HANDLE_VALUE)
	}

	if (!dat.quick) Sleep(rand() % 2500 + 500);
	SetWindowText(dat.hStatic_CurrentState, _T("Finished"));
	SendMessage(dat.hProgress0, PBM_SETPOS, 1000, 0);
	if (IsWindowVisible(hwnd)) MessageBoxW(hwnd, L"The product has been removed "
		"from your computer.", L"Uninstall", MB_ICONINFORMATION);
	Wnds_UninstWindow[hwnd].unclosable = false;
	SendMessage(hwnd, WM_CLOSE, 0, 0);


	//Sleep(INFINITE);
	return 0;
}
LRESULT CALLBACK WndProc_UninstWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		Wnds_UninstWindow_t wnds{ 0 };
		HWND h1, h2, h3, h4, h7;
		wnds.hTitle = CreateWindowExW(0, L"Static", L"Secure Warning",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			5, 5, 465, 20, hWnd, 0, 0, 0);

		h1 = wnds.b1 = CreateWindowExW(WS_EX_TOPMOST, L"Button", L"x",
			WS_CHILD | WS_VISIBLE | BS_CENTER | BS_FLAT,
			470, 5, 20, 20, hWnd, (HMENU)IDCANCEL, 0, 0);
		h2 = wnds.b2 = CreateWindowExW(0, L"Button", L"[&U]ninstall", WS_CHILD |
			WS_VISIBLE | BS_CENTER, 290, 250, 100, 30, hWnd, (HMENU)IDOK, 0, 0);
		h3 = wnds.b3 = CreateWindowExW(0, L"Button", L"[&C]ancel", WS_CHILD | WS_VISIBLE
			| BS_CENTER, 400, 250, 80, 30, hWnd, (HMENU)IDCANCEL, 0, 0);

		h4 = wnds.hStatic = CreateWindowExW(0, L"Static",
			L"Are you sure you want to uninstall?",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, 35, 470, 120, hWnd, 0, 0, 0);
		wnds.h5 = CreateWindowExW(0, L"Static",
			L"This operation will reduce your computer protection.",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, 165, 470, 20, hWnd, 0, 0, 0);

		h7 = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | SS_ICON,
			10, 250, 32, 32, hWnd, 0, 0, 0);

		wnds.bQuickUninst = CreateWindowExW(0, L"Button", L"[&Q]uick uninstall",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 195, 140, 25, hWnd, (HMENU)3, 0, 0);
		wnds.bDeleteAll = CreateWindowExW(0, L"Button",
			L"[&D]elete the application at the same time?",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			160, 195, 320, 25, hWnd, (HMENU)3, 0, 0);

		wnds.hStatic_CurrentStateP = CreateWindowExW(0, L"Static", L"Current state: ",
			WS_CHILD | SS_CENTER, 10, 180, 100, 20, hWnd, 0, 0, 0);
		wnds.hStatic_CurrentState = CreateWindowExW(0, L"Static", L"",
			WS_CHILD | SS_LEFT, 120, 180, 360, 20, hWnd, 0, 0, 0);
		wnds.hProgress0 = CreateWindowExW(0, PROGRESS_CLASSW, L"0%",
			WS_CHILD | WS_DISABLED, 10, 210, 470, 30, hWnd, 0, 0, 0);

		wnds.font = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		wnds.font2 = CreateFont(-30, -15, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		if (wnds.font) {
			SendMessage(h1, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(h2, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(h3, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.h5, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.hTitle, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.bQuickUninst, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.bDeleteAll, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.hStatic_CurrentStateP, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.hStatic_CurrentState, WM_SETFONT, (WPARAM)wnds.font, 0);
			SendMessage(wnds.hProgress0, WM_SETFONT, (WPARAM)wnds.font, 0);
		}
		if (wnds.font2) {
			SendMessage(h4, WM_SETFONT, (WPARAM)wnds.font2, 0);
		}

		HICON icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_UNINST));
		if (icon) SendMessage(h7, STM_SETICON, (WPARAM)icon, 0);

		HMENU menu = GetSystemMenu(hWnd, FALSE);
		if (menu) {
			/*
			0 还原
			1 移动
			2 大小
			3 最小化
			4 最大化
			5 -
			6 关闭
			*/
			DeleteMenu(menu, 5, MF_BYPOSITION);
			DeleteMenu(menu, 4, MF_BYPOSITION);
			DeleteMenu(menu, 3, MF_BYPOSITION);
			DeleteMenu(menu, 2, MF_BYPOSITION);
			DeleteMenu(menu, 0, MF_BYPOSITION);
		}

		Wnds_UninstWindow[hWnd] = wnds;
		break;
	}

	case WM_KEYDOWN:
		if ((wParam == 'C') &&
			IsWindowEnabled(Wnds_UninstWindow[hWnd].b3))
			return SendMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
		if (!(Wnds_UninstWindow[hWnd].unclosable)) {
			if (wParam == VK_ESCAPE || wParam == 'X')
				return SendMessage(hWnd, WM_CLOSE, 0, 0);
			//if (wParam == VK_RETURN) if (IDYES == MessageBoxW(hWnd,
			//	L"Uninstall now?", L"Uninstall", MB_ICONQUESTION | MB_YESNO))
			//	return SendMessage(hWnd, WM_COMMAND, IDOK, 0);
			if (wParam == 'U') return SendMessage(hWnd, WM_COMMAND, IDOK, 0);
			if (wParam == 'Q') {
				return SendMessage(Wnds_UninstWindow[hWnd].bQuickUninst,
					BM_SETCHECK, (BST_CHECKED & SendMessage
					(Wnds_UninstWindow[hWnd].bQuickUninst, BM_GETSTATE, 0, 0))
					? BST_UNCHECKED : BST_CHECKED, 0);
			}
			if (wParam == 'D') {
				return SendMessage(Wnds_UninstWindow[hWnd].bDeleteAll,
					BM_SETCHECK, (BST_CHECKED & SendMessage
					(Wnds_UninstWindow[hWnd].bDeleteAll, BM_GETSTATE, 0, 0))
					? BST_UNCHECKED : BST_CHECKED, 0);
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDOK:
			Wnds_UninstWindow[hWnd].hPrepar = CreateWindowExW(WS_EX_TOPMOST,
				L"Static", L"Preparing...", WS_POPUP | WS_DLGFRAME,
				0, 0, 100, 30, 0, 0, 0, 0);
			if (HMENU menu = GetSystemMenu(hWnd, FALSE)) {
				EnableMenuItem(menu, SC_CLOSE, MF_DISABLED | MF_GRAYED);
				//DeleteMenu(menu, 0, MF_BYPOSITION);
			}
			//EnableWindow(hWnd, FALSE);
			EnableWindow(Wnds_UninstWindow[hWnd].b1, FALSE);
			EnableWindow(Wnds_UninstWindow[hWnd].b2, FALSE);
			EnableWindow(Wnds_UninstWindow[hWnd].b3, FALSE);
			Wnds_UninstWindow[hWnd].unclosable = true;
			if (HANDLE hThread = CreateThread(0, 0, Uninst_Proc, hWnd, 0, 0)) {
				CloseHandle(hThread);
			}
			EnableWindow(hWnd, FALSE);
			break;

		case IDCANCEL:
			if (Wnds_UninstWindow[hWnd].unclosable) {
			if (MessageBoxW(hWnd, L"Cancel uninstalling?\nMany components "
				"may not work properly. You may have to reinstall the application.",
				L"Cancel uninstalling", MB_ICONWARNING | MB_YESNO) != IDYES) break;
			if (!IsWindowEnabled(Wnds_UninstWindow[hWnd].b3)) {
				MessageBoxTimeoutW(hWnd, L"You cannot cancel at this time.",
					NULL, MB_ICONHAND, 0, 3000);
				break;
			}
			}
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	}

	case WM_USER+3:
		if (lParam == 0) {
			wcscpy_s(Wnds_UninstWindow[hWnd].service_name, (PCWSTR)wParam);
			SetWindowTextW(Wnds_UninstWindow[hWnd].hStatic, (
				L"Are you sure you want to uninstall\n"s
				+ (PCWSTR)wParam + L"?"
				).c_str());
		}
		else if (lParam == 2) {
			Wnds_UninstWindow[hWnd].quick = (bool)wParam;
		}
		else if (lParam == 3) {
			Wnds_UninstWindow[hWnd].deleteapp = (bool)wParam;
		}
		break;

	case WM_NCHITTEST: {
		//POINTS pts = MAKEPOINTS(lParam); // position of cursor
		return HTCAPTION;
	}
		break;

	case WM_SETFOCUS:
		EnableWindow(Wnds_UninstWindow[hWnd].hTitle, TRUE);
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	case WM_KILLFOCUS:
		if (GetForegroundWindow() != hWnd)
			EnableWindow(Wnds_UninstWindow[hWnd].hTitle, FALSE);
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;

	case WM_CLOSE:
		if (Wnds_UninstWindow[hWnd].unclosable) break;
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		try {
			DeleteObject(Wnds_UninstWindow[hWnd].font);
			DeleteObject(Wnds_UninstWindow[hWnd].font2);
			Wnds_UninstWindow.erase(hWnd);
		}
		catch (...) {}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_RunProgramWithLimits(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
	switch (msg) {
	case WM_CREATE:
		SetWindowLongPtr(hWnd, GWL_STYLE, (GetWindowLongPtr(hWnd, GWL_STYLE)
			| WS_POPUP | WS_DLGFRAME ) & ~WS_OVERLAPPED & ~WS_SIZEBOX
			& ~WS_BORDER & ~WS_MAXIMIZEBOX);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE)
			| WS_EX_LAYERED);
		SetLayeredWindowAttributes(hWnd, NULL, BYTE((255)*(0.9)), LWA_ALPHA);
		DragAcceptFiles(hWnd, TRUE);
		{
			HWND btn1, btn2;
			btn1 = CreateWindowExW(0, L"Button", L"or Click here to choose a file",
				WS_CHILD | WS_VISIBLE | BS_CENTER,
				10, 40, 222, 40, hWnd, (HMENU)33, 0, 0);
			btn2 = CreateWindowExW(0, L"Button", L"Close",
				WS_CHILD | WS_VISIBLE | BS_CENTER,
				170, 100, 60, 30, hWnd, (HMENU)IDCANCEL, 0, 0);
			static HFONT fnt = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0,
				DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
				DEFAULT_QUALITY, FF_DONTCARE, _T("Consolas"));
			SendMessage(hWnd, WM_SETFONT, (WPARAM)fnt, 0);
			SendMessage(btn1, WM_SETFONT, (WPARAM)fnt, 0);
			SendMessage(btn2, WM_SETFONT, (WPARAM)fnt, 0);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		if (LOWORD(wParam) == 33) {
			WCHAR file[MAX_PATH + 2]{ 0 };
			OPENFILENAMEW ofn{ 0 };
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrTitle = L"Open a executable file to run it with limits";
			ofn.lpstrFile = file;
			ofn.nMaxFile = MAX_PATH + 1;
			ofn.lpstrFilter = L"Executable (*.exe;*.com;*.bat;*.cmd;*.scr)\0"
				"*.exe;*.com;*.bat;*.cmd;*.scr\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileNameW(&ofn)) {
				// TODO: todo
				MessageBoxW(hWnd, file, L"Info", MB_ICONINFORMATION);
			}
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;

	case WM_PAINT: {
		PAINTSTRUCT ps{ 0 };
		HDC hdc = BeginPaint(hWnd, &ps);
		TextOutW(hdc, 10, 10, L"Drag a file here to run it with limits.", 39);
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_NCHITTEST:
		return HTCAPTION;
		break;

	case WM_CLOSE:
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE)
			& ~WS_EX_LAYERED);
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


// @deprecated This function is deprecated.
#if 0
INT_PTR CALLBACK WndProc_Dlg_Main(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
	switch (msg) {
	case WM_INITDIALOG:

		break;

	//case WM_SIZING:
	//case WM_SIZE:
	//{
	//	RECT rc{ 0 }; GetWindowRect(hWnd, &rc);
	//	RECT crc{ 0 }; GetClientRect(hWnd, &crc);
	//	SetWindowPos(GetDlgItem(hWnd, IDC_STATIC_MAIN_GROUP), 0, 1, 1,
	//		crc.right - crc.left - 2, crc.bottom - crc.top - 2, SWP_NOZORDER);
	//	break;
	//}

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


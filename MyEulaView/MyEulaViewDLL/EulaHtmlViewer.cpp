#include "cppwin_include.h"
#include "EulaHtmlViewer.h"
#include "../libs/tinyutf8.h"
#include "../../resource/tool.h"
#include "strutil.h"
#include "UrlProtocol.h"
#include "inject.h"
#include "resource.h"

extern HINSTANCE hInst;

typedef struct {
	HANDLE hProcess;
	DWORD time;
	HWND window;
} Thread_WaitForBrowser_waitdata;

BOOL DLL_FindUserBrowser(PWSTR str, DWORD size);
LRESULT CALLBACK WndProc_EulaView_HTML(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI Thread_WaitForBrowser(void* data);

BOOL WINAPI EulaView_HTML(PMYEULAVIEWDATA data) {
	using namespace std;
	SetLastError(ERROR_SUCCESS);

	if (!data) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	data->bAnyOperationFailed = true;
	bool bUserAgreed = false;
	data->bResult = bUserAgreed;

	if (data->szFile[0] == 0) {
		data->dwErrorCode = ERROR_INVALID_PARAMETER;
		return FALSE;
	}

	WCHAR tmp_name[MAX_PATH + 16]{ 0 };
	if (!GetTempPathW(MAX_PATH - 8, tmp_name)) {
		data->dwErrorCode = GetLastError();
		return FALSE;
	}
	srand((unsigned)time(0));
	wstring szSessionId = s2ws(GenerateUUID());
	wcscat_s(tmp_name, (szSessionId + L"\\").c_str());
	if (!CreateDirectoryW(tmp_name, NULL)) {
		data->dwErrorCode = GetLastError();
		return FALSE;
	}

	wstring szPageLocation = tmp_name + L"/page.html"s;
	if (wstring(data->szFile).find(L"://") == wstring::npos)
	if (!CopyFileW(data->szFile, szPageLocation.c_str(), TRUE)) {
		DWORD err = GetLastError();
		RemoveDirectoryW(tmp_name);
		data->dwErrorCode = (err);
		return FALSE;
	}

	wstring szMainHtmlName = tmp_name + L"/main.html"s;
	fstream fileobject(szMainHtmlName, ios::out, _SH_DENYWR);
	if (!fileobject) {
		DWORD err = GetLastError();
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		data->dwErrorCode = (err);
		return FALSE;
	}

	WCHAR szWndClass[256]{};
	LoadStringW(hInst, IDS_STRING_WNDCLASS_HTHANDLE, szWndClass, 255);
	HWND hwnd = CreateWindowExW(0, szWndClass,
		data->szTitle[0] ? data->szTitle : L"EULA HTML Viewer Window",
		WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, hInst, 0);
	if (!hwnd) {
		DWORD err = GetLastError();
		fileobject.close();
		DeleteFileW(szMainHtmlName.c_str());
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		data->dwErrorCode = (err);
		return FALSE;
	}
	{
		int x = 0, y = 0, cx = 800, cy = 600;
		if (data->position.usePosition) {
			x = data->position.x;
			y = data->position.y;
			cx = data->position.width;
			cy = data->position.height;
		}
		SetWindowPos(hwnd, 0, x, y, cx, cy, SWP_NOACTIVATE);
		if (!data->position.usePosition) {
			CenterWindow(hwnd);
		}
	}

	wstring ProtocolName = L"u" + szSessionId;
	str_replace(ProtocolName, L"-", L"");
	ProtocolName = ProtocolName.substr(0, 32);
	wstring ProtocolCommand;
	{
		WCHAR c[MAX_PATH + 8]{};
		GetModuleFileNameW(hInst, c, MAX_PATH + 4);
		ProtocolCommand = L"rundll32.exe \""s + c +
			L"\",EulaView_HTML_ProtocolHandle \"%1\"";
	}
	if (!UrlProtocol_Create(ProtocolName.c_str(), ProtocolCommand.c_str())) {
		DWORD err = GetLastError();
		fileobject.close();
		if (IsWindow(hwnd)) DestroyWindow(hwnd);
		DeleteFileW(szMainHtmlName.c_str());
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		data->dwErrorCode = (err);
		return FALSE;
	}

	{
#define myWriteFileObject(var) fileobject.write((char*)var.c_str(), var.size())
		tiny_utf8::u8string main_html;

		main_html = u8"<!DOCTYPE HTML>\n"
			"<html>\n<head>\n"
			"    <meta charset=\"utf-8\" />\n"
			"    <meta name=\"viewport\" content=\""
			"width=device-width, initial-scale=1\" />\n"
			"    <title>";
		myWriteFileObject(main_html);

		wstring title = data->szTitle;
		string u8title = str_to_u8str(title);
		myWriteFileObject(u8title);

		main_html = u8"</title>\n"
			"    <script type=\"text/javascript\">";
		myWriteFileObject(main_html);

		wstring str2 = L"window.__protname = '"s + ProtocolName + L"';\n"
			"window.__eventid = " + to_wstring((INT_PTR)hwnd) + L";";
		if (data->dwTimesToAccept) {
			str2 += L"\nwindow.__dwTimesToAccept = " +
				to_wstring(data->dwTimesToAccept) + L";";
		}
		if (data->szLabelAccept[0]) {
			str2 += L"\nwindow.__szLabelAccept = `"s +
				data->szLabelAccept + L"`;";
		}
		if (data->szLabelDecline[0]) {
			str2 += L"\nwindow.__szLabelDecline = `"s +
				data->szLabelDecline + L"`;";
		}
		string u8str2 = str_to_u8str(str2);
		myWriteFileObject(u8str2);

		main_html = u8"</script>\n    "
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></link>\n"
			"    <script type=\"text/javascript\" src=\"script.js\"></script>\n"
			"</head>\n\n<body>\n"
			"    <iframe src=\"";
		myWriteFileObject(main_html);

		wstring copy_of_szPageLocation;
		if (wstring(data->szFile).find(L"://") == wstring::npos) {
			copy_of_szPageLocation = (L"file:""///" + szPageLocation);
			str_replace(copy_of_szPageLocation, L"\\", L"/");
		}
		else {
			copy_of_szPageLocation = data->szFile;
		}
		string u8copy_of_szPageLocation = str_to_u8str(copy_of_szPageLocation);
		myWriteFileObject(u8copy_of_szPageLocation);

		main_html = u8"\"><span>Your browser doesn't support <b>iframe</b>."
			" Please update or change another one.</span></iframe>\n"
			"</body>\n</html>\n";
		myWriteFileObject(main_html);

#undef myWriteFileObject
	}

	fileobject.close();

	FreeResFile(IDR_BIN_HTMLSCRIPT, "BIN", tmp_name + L"/script.js"s, hInst);
	FreeResFile(IDR_BIN_HTMLSTYLE, "BIN", tmp_name + L"/style.css"s, hInst);

	wstring user_data_dir = tmp_name + L"\\browser_data"s;
	wstring szAppName;
	wstring szParamter = L"BrowserProcess \"" + szMainHtmlName +
		L"\" --user-data-dir=\"" + user_data_dir + L"\"";

	HANDLE hBrowserProcess = NULL, hBrowserJob = NULL;
	DWORD dwBrowserProcessId = 0;
	{
		if (!(data->szBrowser[0]))
		if (!DLL_FindUserBrowser(data->szBrowser, MAX_PATH + 1)) {
			if (IsWindow(hwnd)) DestroyWindow(hwnd);
			UrlProtocol_Remove(ProtocolName.c_str());
			DeleteFileW(szMainHtmlName.c_str());
			DeleteFileW(szPageLocation.c_str());
			RemoveDirectoryW(tmp_name);
			data->dwErrorCode = (ERROR_NOT_FOUND);
			return FALSE;
		}
		szAppName = data->szBrowser;

		STARTUPINFOW si{};
		PROCESS_INFORMATION pi{};
		si.cb = sizeof(si);
		if (data->position.usePosition) {
			si.dwFlags |= STARTF_USEPOSITION;
			si.dwX = data->position.x;
			si.dwY = data->position.y;
			si.dwXSize = data->position.width;
			si.dwYSize = data->position.height;
		}
		si.dwFlags |= STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		PWSTR cmdl = new WCHAR[szParamter.length() + 1];
		wcscpy_s(cmdl, szParamter.length() + 1, szParamter.c_str());

		if (!CreateProcessW(szAppName.c_str(), cmdl, NULL, NULL,
			FALSE, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
			DWORD err = GetLastError();
			if (IsWindow(hwnd)) DestroyWindow(hwnd);
			UrlProtocol_Remove(ProtocolName.c_str());
			DeleteFileW(szMainHtmlName.c_str());
			DeleteFileW(szPageLocation.c_str());
			RemoveDirectoryW(tmp_name);
			data->dwErrorCode = (err);
			return FALSE;
		}

		hBrowserJob = CreateJobObject(0, 0);
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobeli{};
		jobeli.BasicLimitInformation.LimitFlags =
			JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		SetInformationJobObject(hBrowserJob,
			JobObjectBasicLimitInformation, &jobeli, sizeof(jobeli));
		AssignProcessToJobObject(hBrowserJob, pi.hProcess);

		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		//CloseHandle(pi.hProcess);
		hBrowserProcess = pi.hProcess;
		dwBrowserProcessId = pi.dwProcessId;
	}

	Thread_WaitForBrowser_waitdata* pWaiterData = NULL;
	HANDLE hWaiter = NULL;
	do {
		pWaiterData = new Thread_WaitForBrowser_waitdata;
		if (!pWaiterData) break;
		memset(pWaiterData, 0, sizeof(pWaiterData));
		pWaiterData->hProcess = hBrowserProcess;
		pWaiterData->time = (data->dwTimeout) ? data->dwTimeout : INFINITE;
		pWaiterData->window = hwnd;
		hWaiter = CreateThread(0, 0, Thread_WaitForBrowser, pWaiterData, 0, 0);
	} while (0);

	ShowWindow(hwnd, SW_NORMAL);
	SendMessage(hwnd, WM_USER + 0xd40, dwBrowserProcessId, 0);
	SetTimer(hwnd, 0xd42, 100, 0);

	{
		MSG msg{ 0 };
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		bUserAgreed = (bool)msg.wParam;
	}

//clean:
	if (hWaiter) {
#pragma warning(push)
#pragma warning(disable: 6258)
		TerminateThread(hWaiter, -1);
#pragma warning(pop)
		CloseHandle(hWaiter);
	}
	if (pWaiterData) { delete pWaiterData; }
	if (hBrowserJob) {
		[] (HANDLE hBrowserJob, DWORD timeout) { __try {
			TerminateJobObject(hBrowserJob, STATUS_CONTROL_C_EXIT);
			WaitForMultipleObjects(1, &hBrowserJob, TRUE,
				(timeout) ? ((timeout > 1000) ? 1000 : timeout) : INFINITE);
			CloseHandle(hBrowserJob);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}; }
		(hBrowserJob, data->dwTimeout);
	}
	if (hBrowserProcess) {
		[&hBrowserProcess] {__try { CloseHandle(hBrowserProcess); }
		__except (EXCEPTION_EXECUTE_HANDLER) {}}();
	}
	if (IsWindow(hwnd)) DestroyWindow(hwnd);
	UrlProtocol_Remove(ProtocolName.c_str());
	FileDeleteTreeW(tmp_name);

	data->bAnyOperationFailed = false;
	data->bResult = bUserAgreed;
	if (data->pCallback) data->pCallback(bUserAgreed, data);
	if (bUserAgreed) data->dwErrorCode = ERROR_SUCCESS;
	else data->dwErrorCode = ERROR_CANCELLED;
	return bUserAgreed;
}


void CALLBACK EulaView_HTML_ProtocolHandle(
	HWND _hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow
) {
	using namespace std;

	CmdLineA cl(lpCmdLine);
	wstring url;

	for (auto& i : cl) {
		if (i.find("://") != i.npos) {
			url = s2ws(i);
			break;
		}
	}

	if (url.empty()) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}

	wstring part1, part2, part3, part4;
	try {
		part1 = url.substr(0, url.find(L"://"));
		part2 = url.substr(url.find(L"://") + 3);
		part3 = part2.substr(0, part2.find(L"/"));
		part4 = part2.substr(part2.find(L"/") + 1);
	}
	catch (...) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return ;
	}

	HWND hwnd = (HWND)(INT_PTR)atoll(ws2c(part4));
	if (!hwnd) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}
	if (part3 == L"accept")
		SendMessage(hwnd, WM_USER + 0xacc, 1, 0);
	else if (part3 == L"decline")
		SendMessage(hwnd, WM_USER + 0xacc, 0, 0);

}


void CALLBACK EulaView_HTML_CommandLine(
	HWND _hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow
) {
	using namespace std;

	CmdLineA cl(lpCmdLine);

	if (cl.getopt("help")) {
		char str[] = "--file=<File> --title=<title> [--timeout] [--dwTimesTo"
			"Accept] [--browser] [--accept] [--decline] [--position=x,y,w,h]";
		if (GetConsoleWindow()) {
			cout << str << endl;
		}
		else {
			MessageBoxA(NULL, str, "Help Information", MB_ICONINFORMATION);
		}

		return;
	}

	string szFile;
	if (cl.getopt("file", szFile) != 1)
		ExitProcess(ERROR_INVALID_PARAMETER);

	MYEULAVIEWDATA dat{};
	dat.cb = sizeof(dat);
	
	wcscpy_s(dat.szFile, s2wc(szFile));

	string str;
	if (cl.getopt("title", str) == 1) {
		wcscpy_s(dat.szTitle, s2wc(str));
	}
	else ExitProcess(ERROR_INVALID_PARAMETER);
	if (cl.getopt("timeout", str) == 1) {
		dat.dwTimeout = atol(str.c_str());
	}
	if (cl.getopt("dwTimesToAccept", str) == 1) {
		dat.dwTimesToAccept = atol(str.c_str());
	}
	if (cl.getopt("browser", str) == 1) {
		wcscpy_s(dat.szBrowser, s2wc(str));
	}
	if (cl.getopt("accept", str) == 1) {
		wcscpy_s(dat.szLabelAccept, s2wc(str));
	}
	if (cl.getopt("decline", str) == 1) {
		wcscpy_s(dat.szLabelDecline, s2wc(str));
	}

	if (cl.getopt("position", str) == 1) {
		vector<string> strs;
		str_split(str, ",", strs);
		if (strs.size() >= 4) {
			dat.position.usePosition = true;
			dat.position.x = atol(strs[0].c_str());
			dat.position.y = atol(strs[1].c_str());
			dat.position.width = atol(strs[2].c_str());
			dat.position.height = atol(strs[3].c_str());
		}
	}

	auto result = EulaView_HTML(&dat);

	auto bool2str = [](bool b) {
		return b ? "true" : "false";
	};
	
	char buffer[1024]{};
	sprintf_s(buffer, "%s\n"
		"Code: %lu\n"
		"Result: %s\n"
		"Failed : %s\n",
		ErrorCodeToStringA(dat.dwErrorCode).c_str(),
		dat.dwErrorCode,
		bool2str(dat.bResult),
		bool2str(dat.bAnyOperationFailed));

	AttachConsole(ATTACH_PARENT_PROCESS);
	::puts(buffer); DWORD dw1 = 0;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),
		buffer, (DWORD)strlen(buffer), &dw1, 0);

	ExitProcess(result);
}



BOOL DLL_FindUserBrowser(PWSTR str, DWORD size) {
	// Find a valid browser
	HKEY hk = NULL; LSTATUS nErrCode = 0; BOOL success = FALSE;
	if (nErrCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\"
		"Windows\\CurrentVersion\\App Paths", 0, KEY_READ, &hk)) {
		SetLastError(nErrCode);
		return FALSE;
	}
	if (hk) {
		HKEY hks = NULL;
		LPCWSTR browsers[] = {
			L"chrome.exe",
			L"chromium.exe",
			L"msedge.exe",
			L"firefox.exe",
			L"iexplore.exe"
		};
		for (size_t nIndex = 0; nIndex <
			(sizeof(browsers) / sizeof(LPCWSTR)); nIndex++) 
		if (!RegOpenKeyExW(hk, browsers[nIndex], 0, KEY_READ, &hks)) {
			WCHAR buf[2048]{ 0 }; DWORD size = 2047, type = REG_SZ;
			if (!RegGetValueW(hks, L"", L"", RRF_RT_REG_SZ, NULL, buf, &size)) {
				wcscpy_s(str, size, buf);
				RegCloseKey(hks);
				success = TRUE;
				break;
			}
			RegCloseKey(hks);
		}

		RegCloseKey(hk);
	}
	return success;
}


typedef struct {
	DWORD dwTimeout;
	bool bUserAgreed;
	HWND childHwnd;
	DWORD dwProcessId;
} WndProc_EulaView_HTML_t;
std::map<HWND, WndProc_EulaView_HTML_t*> WndProc_EulaView_HTML_d;
LRESULT CALLBACK WndProc_EulaView_HTML(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		WndProc_EulaView_HTML_t* obj = new WndProc_EulaView_HTML_t;
		memset(obj, 0, sizeof(WndProc_EulaView_HTML_t));
		WndProc_EulaView_HTML_d.insert(std::make_pair(hwnd, obj));
		break;
	}

	case WM_CLOSE:
	case WM_USER + 0x44:
		DestroyWindow(hwnd);

	case WM_USER+0xacc:
		try {
			WndProc_EulaView_HTML_d.at(hwnd)->bUserAgreed = (bool)wParam;
			DestroyWindow(hwnd);
		}
		catch (...) {}
		break;

	case WM_USER+0xcd0:
		try {
			WndProc_EulaView_HTML_d.at(hwnd)->childHwnd = (HWND)wParam;
		}
		catch (...) {}
		break;

	case WM_USER+0xd40:
		try {
			WndProc_EulaView_HTML_d.at(hwnd)->dwProcessId = (DWORD)wParam;
		}
		catch (...) {}
		break;

	case WM_SETFOCUS:
		try {
			SetForegroundWindow(WndProc_EulaView_HTML_d.at(hwnd)->childHwnd);
		}
		catch (...) {}
		break;

	case WM_SIZE:
		try {
			HWND hw = WndProc_EulaView_HTML_d.at(hwnd)->childHwnd;
			RECT rc{};
			GetClientRect(hwnd, &rc);
			SetWindowPos(hw, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}
		catch (...) {}
		break;

	case WM_TIMER:
		switch (wParam) {
		case 0xd42: {
			try {
				std::vector<HWND> windows;
				Process.findWindows(Process_t::ProcessInfo(TEXT(""),
					WndProc_EulaView_HTML_d.at(hwnd)->dwProcessId), windows);
				if (!windows.empty())
				for (auto& wnd : windows) {
					if (!(GetWindowLongPtr(wnd, GWL_STYLE) & WS_BORDER) ||
						!(GetWindowLongPtr(wnd, GWL_STYLE) & WS_SYSMENU)) continue;
					if (!IsWindowEnabled(wnd)) continue;
					if (GetParent(wnd) != NULL) continue;
					if (SendMessage(wnd, WM_GETTEXTLENGTH, 0, 0) < 1) continue;

					SendMessage(hwnd, WM_USER + 0xcd0, (WPARAM)wnd, 0);
					SetParent(wnd, hwnd);
					ShowWindow(wnd, SW_NORMAL);
					SetWindowLongPtr(wnd, GWL_STYLE, GetWindowLongPtr(wnd, GWL_STYLE)
						& ~WS_POPUP | WS_CHILD);
					SetWindowPos(wnd, 0, 0, 0, 0, 0,
						SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
					KillTimer(hwnd, wParam);
					SendMessage(hwnd, WM_SIZE, 0, 0);
					break;
				}
			}
			catch (...) {}
			break;
		}

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

	case WM_DESTROY: {
		int nExitCode = 0;
		try {
			auto obj = WndProc_EulaView_HTML_d.at(hwnd);
			if (obj) {
				nExitCode = obj->bUserAgreed;
				delete obj;
			}
			WndProc_EulaView_HTML_d.erase(hwnd);
		}
		catch (...) {}
		PostQuitMessage(nExitCode);
		break;
	}
		
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

static DWORD __stdcall Thread_WaitForBrowser(void* data) {
	Thread_WaitForBrowser_waitdata* dat = (Thread_WaitForBrowser_waitdata*)data;
	//MessageBoxW(0, std::to_wstring((INT_PTR)dat->window).c_str(), 0, 0);
	if (dat->hProcess) {
		HANDLE hp = dat->hProcess;
		WaitForMultipleObjects(1, &hp, TRUE,
			dat->time ? dat->time : INFINITE);
		SendMessage(dat->window, WM_USER + 0x44, 0, 0);
	}
	return 0;
}



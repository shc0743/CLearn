#include "EulaHtmlViewer.h"
#include "../libs/tinyutf8.h"
#include "../../resource/tool.h"
#include "strutil.h"
#include "UrlProtocol.h"
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

	bool bUserAgreed = FALSE;

	WCHAR tmp_name[MAX_PATH + 16]{ 0 };
	if (!GetTempPathW(MAX_PATH - 8, tmp_name)) {
		return FALSE;
	}
	srand((unsigned)time(0));
	wstring szSessionId = s2ws(GenerateUUID());
	wcscat_s(tmp_name, (szSessionId + L"\\").c_str());
	if (!CreateDirectoryW(tmp_name, NULL)) {
		return FALSE;
	}

	wstring szPageLocation = tmp_name + L"/page.html"s;
	if (!CopyFileW(data->szFile, szPageLocation.c_str(), TRUE)) {
		DWORD err = GetLastError();
		RemoveDirectoryW(tmp_name);
		SetLastError(err);
		return FALSE;
	}

	wstring szMainHtmlName = tmp_name + L"/main.html"s;
	fstream fileobject(szMainHtmlName, ios::out, _SH_DENYWR);
	if (!fileobject) {
		DWORD err = GetLastError();
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		SetLastError(err);
		return FALSE;
	}

	WCHAR szWndClass[256]{};
	LoadStringW(hInst, IDS_STRING_WNDCLASS_HTHANDLE, szWndClass, 255);
	HWND hwnd = CreateWindowExW(0, szWndClass, L"URL Handler Window",
		0, 0, 0, 1, 1, 0, 0, hInst, 0);
	if (!hwnd) {
		DWORD err = GetLastError();
		fileobject.close();
		DeleteFileW(szMainHtmlName.c_str());
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		SetLastError(err);
		return FALSE;
	}

	wstring ProtocolName = L"u" + szSessionId;
	str_replace(ProtocolName, L"-", L"");
	ProtocolName = ProtocolName.substr(0, 32);
	wstring ProtocolCommand;
	{
		WCHAR c[MAX_PATH + 8]{};
		GetModuleFileNameW(hInst, c, MAX_PATH + 4);
		ProtocolCommand = L"rundll32.exe \""s + c +
			L"\",EulaView_HTML_ProtocolHandle %1";
	}
	if (!UrlProtocol_Create(ProtocolName.c_str(), ProtocolCommand.c_str())) {
		DWORD err = GetLastError();
		fileobject.close();
		DeleteFileW(szMainHtmlName.c_str());
		DeleteFileW(szPageLocation.c_str());
		RemoveDirectoryW(tmp_name);
		SetLastError(err);
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
		string u8str2 = str_to_u8str(str2);
		myWriteFileObject(u8str2);

		main_html = u8"</script>\n    "
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></link>\n"
			"<script type=\"text/javascript\" src=\"script.js\"></script>\n"
			"</head>\n\n<body>\n"
			"    <iframe src=\"";
		myWriteFileObject(main_html);

		wstring copy_of_szPageLocation(L"file:""///" + szPageLocation);
		str_replace(copy_of_szPageLocation, L"\\", L"/");
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
	{
		WCHAR browser[MAX_PATH + 4]{};
		if (!DLL_FindUserBrowser(browser, MAX_PATH + 1)) {
			UrlProtocol_Remove(ProtocolName.c_str());
			DeleteFileW(szMainHtmlName.c_str());
			DeleteFileW(szPageLocation.c_str());
			RemoveDirectoryW(tmp_name);
			SetLastError(ERROR_NOT_FOUND);
			return FALSE;
		}
		szAppName = browser;

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

		PWSTR cmdl = new WCHAR[szParamter.length() + 1];
		wcscpy_s(cmdl, szParamter.length() + 1, szParamter.c_str());

		if (!CreateProcessW(szAppName.c_str(), cmdl, NULL, NULL,
			FALSE, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
			DWORD err = GetLastError();
			UrlProtocol_Remove(ProtocolName.c_str());
			DeleteFileW(szMainHtmlName.c_str());
			DeleteFileW(szPageLocation.c_str());
			RemoveDirectoryW(tmp_name);
			SetLastError(err);
			return FALSE;
		}
		CloseHandle(pi.hThread);
		hBrowserJob = CreateJobObject(0, 0);
		hBrowserProcess = pi.hProcess;
		AssignProcessToJobObject(hBrowserJob, hBrowserProcess);
	}

	do {
		Thread_WaitForBrowser_waitdata* dat = NULL;
		dat = (Thread_WaitForBrowser_waitdata*)calloc(1, sizeof(dat));
		if (!dat) break;
		dat->hProcess = hBrowserProcess;
		dat->time = (data->dwTimeout) ? data->dwTimeout : INFINITE;
		dat->window = hwnd;
		if (HANDLE ht = CreateThread(0, 0, Thread_WaitForBrowser, dat, 0, 0)) {
			CloseHandle(ht);
		}
	} while (0);
	Process.resume(hBrowserProcess);

	{
		MSG msg{ 0 };
		while (GetMessage(&msg, 0, 0, 0)) {
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
		bUserAgreed = (bool)msg.wParam;
	}

//clean:
	if (hBrowserJob) {
		[] (HANDLE hBrowserJob) { __try {
			TerminateJobObject(hBrowserJob, STATUS_CONTROL_C_EXIT);
			CloseHandle(hBrowserJob);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}; }(hBrowserJob);
	}
	if (hBrowserProcess) {
		[] (HANDLE hBrowserProcess) { __try {
			TerminateProcess(hBrowserProcess, 0);
			CloseHandle(hBrowserProcess);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}; }(hBrowserProcess);
	}
	UrlProtocol_Remove(ProtocolName.c_str());
	FileDeleteTreeW(tmp_name);
	FileDeleteTreeW(tmp_name);

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
		SendMessage(hwnd, WM_USER + 0xacc, 0, 0);
	else if (part3 == L"decline")
		SendMessage(hwnd, WM_USER + 0xdec, 0, 0);

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
			WndProc_EulaView_HTML_d.at(hwnd)->bUserAgreed = true;
			DestroyWindow(hwnd);
		}
		catch (...) {}
		break;
	case WM_USER+0xdec:
		try {
			WndProc_EulaView_HTML_d.at(hwnd)->bUserAgreed = false;
			DestroyWindow(hwnd);
		}
		catch (...) {}
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

DWORD __stdcall Thread_WaitForBrowser(void* data) {
	Thread_WaitForBrowser_waitdata* dat = (Thread_WaitForBrowser_waitdata*)data;
	//MessageBoxW(0, std::to_wstring((INT_PTR)dat->window).c_str(), 0, 0);
	if (dat->hProcess) WaitForSingleObject(dat->hProcess, dat->time);
	SendMessage(dat->window, WM_USER + 0x44, 0, 0);
	free(dat);
	return 0;
}


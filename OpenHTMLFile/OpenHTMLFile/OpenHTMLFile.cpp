#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include "../../resource/tool.h"
using namespace std;

void SIGINT_HANDLER(int signal);
static HANDLE hBrowserProcess;
static DWORD main_thread_id;
using std::cout; using std::wcout;
#ifndef _WIN64
#define nop __asm nop
#else
#define nop 
#endif

int main() {
	nop;
	// Close console window
	if (GetConsoleWindow()) {
		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(GetConsoleWindow(), &dwProcessId);
		if (dwProcessId == GetCurrentProcessId()) FreeConsole();
	}
	nop; nop;
	HWND wConsole = GetConsoleWindow();

	::main_thread_id = GetCurrentThreadId();
	signal(SIGINT, SIGINT_HANDLER);

	// Parse command-line
	CmdLineW cl(GetCommandLineW());
	nop; nop; nop; nop;
	if (cl.getopt(L"help")) {
		if (!wConsole) if (AllocConsole()) {
			(void)freopen("CONOUT$", "wt", stdout);
		}
		cout << "OpenHTMLFile.exe Help Document\n==============================\n"
			"Usage:\n\t%0 [options] Files_or_url_to_open\n"
			"\nOptions:"
			"\n--help                 Show this help message."
			"\n--browser=<Browser>    Use <Browser> as browser."
			"\n--find-all-browsers    Only find all browsers but not open files."
			"\n--no-wait              Do not wait for browser process."
			"\n--gui                  Use GUI"
			"\n--con                  Use console window (UNRECOMMENDED: Bugful)"
			"\n                       If you want to use, launch this application"
			"\n                       in another program"
			"\n--choose-browser       Use GetOpenFileNameW to choose a browser in GUI"
			"\n--add-assoc            Add assoc to this program (.htm,.html)."
			"\n--del-assoc            Remove assoc"
			<< endl;
		if (!wConsole) {
			puts("\nPress any key to continue...");
			(void)_getch();
		}
		return 0;
	}
	if (cl.getopt(L"add-assoc") || cl.getopt(L"del-assoc")) {
		if (cl.getopt(L"add-assoc")) {
			HKEY hk = NULL; LRESULT nCode = 0;
			if (nCode = RegOpenKeyExW(HKEY_CLASSES_ROOT, L".htm\\OpenWithProgIds",
				0, KEY_READ | KEY_WRITE, &hk)) return int(nCode);
			RegSetKeyValueW(hk, NULL, L"OpenHTMLFile.Generic", REG_SZ, L"", 4);
			RegCloseKey(hk); hk = NULL;
			if (nCode = RegOpenKeyExW(HKEY_CLASSES_ROOT, L".html\\OpenWithProgIds",
				0, KEY_READ | KEY_WRITE, &hk)) return int(nCode);
			RegSetKeyValueW(hk, NULL, L"OpenHTMLFile.Generic", REG_SZ, L"", 4);
			RegCloseKey(hk); hk = NULL;
			if (nCode = RegCreateKeyExW(HKEY_CLASSES_ROOT, L"OpenHTMLFile.Generic"
				"\\shell\\Open\\command", 0, 0, 0, KEY_ALL_ACCESS, NULL,
				&hk, NULL)) return int(nCode);
			wstring szCmdLine = L"\"" + s2ws(GetProgramDir()) + L"\" \"%1\" %*";
			RegSetKeyValueW(HKEY_CLASSES_ROOT, L"OpenHTMLFile.Generic", NULL, REG_SZ,
				L"Open HTML file or URL", 46);
			RegSetKeyValueW(hk, NULL, NULL, REG_SZ,
				szCmdLine.c_str(), DWORD(szCmdLine.length() + 1) * 2);
			RegCloseKey(hk);
			return 0;
		} else {
			LRESULT lCode1 = 0, lCode2 = 0, lCode3 = 0;
			HKEY hk = NULL;
			if (!(lCode1 = RegOpenKeyExW(HKEY_CLASSES_ROOT, L".htm\\OpenWithProgIds",
				0, KEY_READ | KEY_WRITE, &hk))) {
				RegDeleteValueW(hk, L"OpenHTMLFile.Generic");
				RegCloseKey(hk); hk = NULL;
			}
			if (!(lCode2 = RegOpenKeyExW(HKEY_CLASSES_ROOT, L".html\\OpenWithProgIds",
				0, KEY_READ | KEY_WRITE, &hk))) {
				RegDeleteValueW(hk, L"OpenHTMLFile.Generic");
				RegCloseKey(hk); hk = NULL;
			}
			lCode3 = RegDeleteTreeW(HKEY_CLASSES_ROOT, L"OpenHTMLFile.Generic");
			return lCode1 || lCode2 || lCode3 || 0;
		}
	}
	for (size_t i = 0; i < cl.size(); ++i) {
		if (cl[i] == L"--gui") {
			wConsole = NULL;
			cl.__get_data__().erase(cl.begin() + i);
		} else
		if (cl[i] == L"--con") {
			if (!wConsole) if (AllocConsole()) [&wConsole] {
				wConsole = GetConsoleWindow();
				__try {
					(void)freopen("CONIN$", "r", stdin);
					(void)freopen("CONOUT$", "wt+", stdout);
					(void)freopen("CONOUT$", "wt+", stderr);
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {};
			}();
			cl.__get_data__().erase(cl.begin() + i);
		}
	}
	wstring files, browser;
	if (wConsole == NULL && cl.argc() < 2) {
		WCHAR szFile[MAX_PATH + 1]{ 0 };
		OPENFILENAMEW fn; memset(&fn, 0, sizeof fn);
		fn.lStructSize = sizeof(fn);
		fn.hwndOwner = NULL;
		fn.lpstrFile = szFile;
		fn.lpstrFile[0] = L'\0';
		fn.nMaxFile = MAX_PATH;
		fn.lpstrFilter = L"HTML Files (*.htm;*.html)\0*.htm;*.html\0All Files\0*.*\0";
		fn.nFilterIndex = 1;
		fn.lpstrFileTitle = NULL;
		fn.nMaxFileTitle = 0;
		fn.lpstrInitialDir = NULL;
		fn.lpstrTitle = L"Choose files to open";
		fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (!GetOpenFileNameW(&fn)) {
			cerr << "[ERROR] " << ErrorCodeToStringA(ERROR_CANCELLED) << endl;
			cerr << "[FATAL] Missing HTML file path!\n[FATAL] Exiting...\n";
			return ERROR_CANCELLED;
		}
		files = L" "s + szFile;
	} else if (cl.argc() < 2) {
		cerr << "[FATAL] Missing HTML file path!\n[FATAL] Exiting...\n";
		return ERROR_INVALID_PARAMETER;
	}
	cl.getopt(L"browser", browser);
	bool browser_exists = file_exists(browser);
	if ((!browser.empty()) && !browser_exists) {
		PROCESS_INFORMATION pi;
		STARTUPINFOW si;
		AutoZeroMemory(si); AutoZeroMemory(pi);
		si.cb = sizeof si;
		si.dwFlags = 0;
		WCHAR cmdl[] = L"Browser";
		BOOL bResult = CreateProcessW(browser.c_str(), cmdl,
			NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
		if (!bResult) {
			DWORD nCode = GetLastError();
			cerr << "[ERROR] Error testing browser: " << nCode
				<< ": " << ErrorCodeToStringA(nCode) << endl;
		}
		else {
			TerminateProcess(pi.hProcess, 0);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			browser_exists = true;
		}
	}
	if (browser.empty() || (!browser_exists)) {
		// Find a valid browser
		HKEY hk = NULL; LSTATUS nErrCode = 0;
		if (nErrCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\"
			"Windows\\CurrentVersion\\App Paths", 0, KEY_READ, &hk)) {
			cerr << "[ERROR] Cannot open register key \"App Paths\"!\n";
			cerr << "[DEBUG] " << nErrCode << ": " << ErrorCodeToStringA(nErrCode) << endl;
		}
		if (hk) {
			HKEY hks = NULL; bool findall = false;
			if (cl.getopt(L"find-all-browsers")) findall = true;
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
					WCHAR buf[256]{ 0 }; DWORD size = 256, type = REG_SZ;
					if (!RegGetValueW(hks, L"", L"", RRF_RT_REG_SZ, NULL, buf, &size)) {
						browser = buf; RegCloseKey(hks); if (!findall) break;
						wcout << L"[DEBUG] Browser found at " << buf << endl;
					}
					RegCloseKey(hks);
				}

			RegCloseKey(hk);
		}

		if (cl.getopt(L"find-all-browsers")) return 0;

		auto chsbrs = [&browser] () -> bool {
			WCHAR szFile[MAX_PATH + 1]{ 0 };
			OPENFILENAMEW fn; memset(&fn, 0, sizeof fn);
			fn.lStructSize = sizeof(fn);
			fn.hwndOwner = NULL;
			fn.lpstrFile = szFile;
			fn.lpstrFile[0] = L'\0';
			fn.nMaxFile = MAX_PATH;
			fn.lpstrFilter = L"Executable (*.exe)\0*.exe\0";
			fn.nFilterIndex = 1;
			fn.lpstrFileTitle = NULL;
			fn.nMaxFileTitle = 0;
			fn.lpstrInitialDir = NULL;
			fn.lpstrTitle = L"Choose a browser";
			fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (!GetOpenFileNameW(&fn)) {
				return false;
			}
			browser = szFile;
			return true;
		};
		if (browser.empty() || (!file_exists(browser)))
			if (!wConsole) {
				if (!chsbrs()) {
					cerr << "[ERROR] " << ErrorCodeToStringA(ERROR_CANCELLED) << endl;
					cerr << "[FATAL] Cannot find a valid browser!\n[FATAL] Exiting...\n";
					return ERROR_FILE_NOT_FOUND;
				}
			}
			else {
				// 
			}
		else if (cl.getopt(L"choose-browser")) {
			chsbrs();
		}
		else {
			wcout << L"[INFO]  Browser found: " << browser << endl;
		}
	}
	browser_exists = file_exists(browser);
	if (browser.empty() || (!browser_exists)) {
		cerr << "[FATAL] Cannot find a valid browser!\n[FATAL] Exiting...\n";
		return ERROR_FILE_NOT_FOUND;
	}

	for (size_t i = 1; i < cl.size(); ++i) {
		if (file_exists(cl[i]) || cl[i].find(L"://") != wstring::npos) {
			files = files + L" \"" + cl[i] + L"\"";
		} else if (!(cl[i].size() > 0 && cl[i][0] == L'-')) {
			wclog << L"[WARN]  Invalid command-line paramter "
				"at " << i << L": " << cl[i] << endl;
		}
	}

	wstring user_data_dir;
	if (cl.getopt(L"user-data-dir", user_data_dir) != 1) {
		wstring id;
		for (size_t pos = files.size() - 1; pos != (size_t)-1; --pos) id += files[pos];
		id = id.substr(0, 32);

		/* Cannot use these chars as directory name */
		str_replace(id, L"/", L"");	str_replace(id, L"\\", L"");
		str_replace(id, L"*", L"");	str_replace(id, L"\"", L"");
		str_replace(id, L":", L"");	str_replace(id, L"?", L"");
		str_replace(id, L"<", L"");	str_replace(id, L">", L"");
		str_replace(id, L"|", L"");
		/* Unbeautiful */
		str_replace(id, L" ", L"_"); str_replace(id, L".", L"_");
		str_replace(id, L"-", L"_"); str_replace(id, L"=", L"_");
		str_replace(id, L"&", L"_"); str_replace(id, L"%", L"_");
		str_replace(id, L"$", L"_"); str_replace(id, L"^", L"_");
		str_replace(id, L"#", L"_"); str_replace(id, L"!", L"_");

		srand((unsigned)time(0) - rand()); wstring _rd;
		wchar_t cchhex[] = L"0123456789abcdef";
		for (size_t i = 0; i < 8; ++i) _rd += cchhex[rand() % 16];
		WCHAR TEMP[MAX_PATH]{ 0 }; GetEnvironmentVariableW(L"TEMP", TEMP, MAX_PATH);
		user_data_dir = L""s + TEMP + L"\\Data.OpenHTMLFile\\Data\\" + 
			to_wstring(time(0))	+ L"." + id + L".{" + (_rd)+L"}.dat";
	}
#if 0
	/* Create user data directory */ {
		size_t off = 1;
		while ((off = user_data_dir.find(L"\\", off) + 1) != wstring::npos) {
			if (off < 1) break;
			CreateDirectoryW(user_data_dir.substr(0, off-1).c_str(), NULL);
		}
	}
#endif
	wstring cmd_line = L"BrowserProcessPath" + files + 
		L" --user-data-dir=\"" + user_data_dir;
	PROCESS_INFORMATION pi; {
		STARTUPINFOW si;
		AutoZeroMemory(si); AutoZeroMemory(pi);
		si.cb = sizeof si;
		si.dwFlags = 0;
		WCHAR pcmd_line[2048]{ 0 };
		wcscpy_s(pcmd_line, cmd_line.c_str());
		BOOL bResult = CreateProcessW(browser.c_str(), pcmd_line,
			NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
		if (!bResult) {
			DWORD nCode = GetLastError();
			cerr << "[FATAL] Error creating process: " << nCode
				<< ": " << ErrorCodeToStringA(nCode) << endl;
			cerr << "[FATAL] Exiting...\n";
			return int(nCode);
		}
	}
	wcout << L"[INFO]  Process created\n[INFO]  Command line: " << cmd_line << endl;
	cout << "[DEBUG] hProcess: " << pi.hProcess << "\thThread: " <<
		pi.hThread << "\n[INFO]  dwProcessId: " << pi.dwProcessId <<
		"\tdwThreadId: " << pi.dwThreadId << endl;
	CloseHandle(pi.hThread);
	if (!cl.getopt(L"no-wait")) {
		DWORD dwExitCode = 0;
		::hBrowserProcess = pi.hProcess;
		Process.resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
		cout << "[INFO]  Browser exited with code " << dwExitCode << endl;
		/* Clean */ {
			wcout << L"[INFO]  Deleting temp data directory: " << user_data_dir << endl;
			SHFILEOPSTRUCTW so; AutoZeroMemory(so);
			wstring pFrom; so.wFunc = FO_DELETE; int result = 0;
			so.lpszProgressTitle = L"Deleting temp directory...";
			if (!cl.getopt(L"debug-SH-ui")) so.fFlags = FOF_NO_UI;
			
			pFrom = user_data_dir + L"\\\0";
			so.pFrom = pFrom.c_str();
			result = SHFileOperationW(&so);
			wcout << L"[DEBUG] SHFileOperationW pFrom:" << so.pFrom <<
				" result: " << result << endl;

			pFrom = user_data_dir + L"\\..\\..\0";
			so.pFrom = pFrom.c_str();
			result = SHFileOperationW(&so);
			wcout << L"[DEBUG] SHFileOperationW pFrom:" << so.pFrom <<
				" result: " << result << endl;
		}
		cout << "[INFO]  Exiting..." << endl;
		return dwExitCode;
	}
	Process.resume(pi.hProcess);
	CloseHandle(pi.hProcess);
	cout << "[INFO]  Exiting..." << endl;

	return 0;
}

void SIGINT_HANDLER(int signal) {
	if (signal != SIGINT) return;
	{HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, main_thread_id);
	if (hThread) {
		SuspendThread(hThread); CloseHandle(hThread);
	}}
	if (hBrowserProcess) __try {
		TerminateProcess(hBrowserProcess, PROCESS_TERMINATE);
		CloseHandle(hBrowserProcess);
	} __except (EXCEPTION_EXECUTE_HANDLER) {};
	fprintf(stderr, "[FATAL] User cancelled\n");
	ExitProcess(ERROR_CANCELLED);
}

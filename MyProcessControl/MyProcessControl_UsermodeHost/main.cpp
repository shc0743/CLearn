#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<iostream>
#include "../../resource/tool.h"
#include "res/0/resource.h"
#include "svcmain.h"
#include "modaldlg.h"
using namespace std;


HMODULE translate_file = NULL;


// wWinMain function: The application will start from here.
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow);
// main function: The entry of console application.
int main(int argc, char* argv[]);
// Service options
static int main_ServiceOptions(CmdLineA& cl);
// show help
static void showHelp();
// 
static int ServiceExitConfirm(CmdLineA& cl);
// execute [--debug-service]
static int ExecDebugService(DWORD pid/*, DWORD tid*/, LPCSTR restart_service_name);


/* ==================================== */


int main(int argc, char* argv[]) {
	return wWinMain(GetModuleHandle(NULL), NULL, GetCommandLineW(), SW_NORMAL);
	return 0;
}

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow
){
	::translate_file = hInstance;

	CmdLineA cl(GetCommandLineA());

	if (cl.argc() < 2) {
		if (IsRunAsAdmin() || IsCurrentUserInBuiltinGroup(WinBuiltinAdministratorsSid))
		if (1) { // 去掉滚动条
			SHELLEXECUTEINFO execinfo;
			memset(&execinfo, 0, sizeof(execinfo));
			std::wstring pd         = s2ws(GetProgramDir());
			execinfo.lpFile			= pd.c_str();
			execinfo.cbSize			= sizeof(execinfo);
			execinfo.lpVerb			= 0?L"runas":L"open";
			execinfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
			execinfo.nShow			= SW_SHOWDEFAULT;
			execinfo.lpParameters	= L"--ui";
 
			ShellExecuteExW(&execinfo);
 
			WaitForSingleObject(execinfo.hProcess, INFINITE);
			DWORD exitCode = 0;
			GetExitCodeProcess(execinfo.hProcess, &exitCode);
			CloseHandle(execinfo.hProcess);
			return (exitCode);
			return INT_PTR(ShellExecuteA(NULL, "runas", GetProgramDir().c_str(),
				"--ui", NULL, 1)) < 32 ? 0 : 1;
		}
		else if (IsCurrentUserInBuiltinGroup(WinBuiltinUsersSid)) {
			//AllocConsole();
			string str = "[FATAL] 5: "s + ErrorCodeToStringA(5) + "\n";
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(),
				DWORD(str.length() + 1), 0, 0);
			return ERROR_ACCESS_DENIED;
		}
		else {
			//AllocConsole();
			string str = "[FATAL] 87: "s + ErrorCodeToStringA(87) + "\n";
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(),
				DWORD(str.length() + 1), 0, 0);
			return ERROR_INVALID_PARAMETER;
		}
	}

	if (cl.getopt("vsdebug")) {
		if (Process.StartOnly(L"VsJITDebugger -p " + to_wstring(GetCurrentProcessId())))
			Process.suspend(GetCurrentProcess());
	}

	if (IsDebuggerPresent() || cl.getopt("debug-console")) {
		AllocConsole();
		SetConsoleTitleW(L"MyProcessControl_UsermodeHost Debug Console");
		(void)freopen("CONIN$", "rb", stdin);
		(void)freopen("CONOUT$", "wb+", stdout);
		(void)freopen("CONOUT$", "wb+", stderr);
	}

	if (cl.getopt("debug-console-attach")) {
		AttachConsole(Process.GetParentProcessId(GetCurrentProcessId()));
		(void)freopen("CONIN$", "rb", stdin);
		(void)freopen("CONOUT$", "wb+", stdout);
		(void)freopen("CONOUT$", "wb+", stderr);
	}

	if (cl.getopt("run-service") == 1) {
		string svc_name, cfg_name;
		cl.getopt("run-service", svc_name);
		cl.getopt("config", cfg_name);
		if (svc_name.empty() || cfg_name.empty())
			return ERROR_INVALID_PARAMETER;
		ServiceWorker_c svcwk;
		strcpy_s(svcwk.ServiceName, svc_name.c_str());
		svcwk.cfg_path = cfg_name;
		global_SvcObj = &svcwk;
		MyStartAsServiceW(s2ws(svc_name), svcwk.ServiceLaunch_main);
		return 0;
	}


	if (cl.getopt("ui")) {
		//MyWnd_uiMain w;
		//w.RegClass();
		//w.Create();
		//w.show();
		//return w.MessageLoop();
		return ERROR_NOT_SUPPORTED;
	}

	if (cl.getopt("service-exit-confirm")) {
		return ServiceExitConfirm(cl);
	}

	if (cl.getopt("svc-options")) return main_ServiceOptions(cl);

	if (cl.getopt("generate-config")) {
		if (cl.argc() < 3) return ERROR_INVALID_PARAMETER;
		if (!FreeResFile(IDR_BIN_CFG_DEFAULT, "BIN", cl[2], translate_file))
			return GetLastError(); else return 0;
	}
	if (cl.getopt("generate-example-config")) {
		if (cl.argc() < 3) return ERROR_INVALID_PARAMETER;
		if (!FreeResFile(IDR_BIN_CFG_EXAMPLE, "BIN", cl[2], translate_file))
			return GetLastError(); else return 0;
	}

	if (cl.getopt("debug-service") && cl.getopt("p") == 1/* && cl.getopt("tid") == 1*/) {
		string _spid/*, _stid*/; cl.getopt("p", _spid);/* cl.getopt("tid", _stid);*/
		DWORD pid = (DWORD)atol(_spid.c_str());
		//DWORD tid = (DWORD)atol(_stid.c_str());
		if (pid == 0/* || tid == 0*/) return 87;
		string _srestsvc; cl.getopt("restart-service-on-terminated", _srestsvc);
		if (cl.getopt("launch")) {
			//srand(time(NULL));
			CHAR chAppUuid[64] = { 0 };
			LoadStringA(ThisInst, IDS_STRING_APP_UUID, chAppUuid, 63);
			string platform =
#ifdef _WIN64
				"x64";
#else
				"x86";
#endif
			TSTRING szExePath = to__str(getenv("SystemRoot") + "\\Temp\\"s + 
				chAppUuid + "." + platform + ".exe");
			DeleteFile(szExePath.c_str());
			CopyFile(s2wc(GetProgramDir()), szExePath.c_str(), TRUE);
			TSTRING cl_co;
			CmdLine cl(GetCommandLine());
			for (auto& i : cl) {
				wprintf(L"[DEBUG] i: %s\n", i.c_str());
				if (CmdLine(i).getopt(_T("launch"))) continue;
				cl_co += _T("\""s) + i + _T("\" ");
			}
			wprintf(L"[DEBUG] Command Line: %s\n", cl_co.c_str());
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));
			{
				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				LPTSTR cl = (LPTSTR)calloc(cl_co.length() + 1, sizeof(TCHAR));
				if (!cl) return 0;
#ifdef UNICODE
				wcscpy_s(cl, cl_co.length() + 1, cl_co.c_str());
#else
				strcpy_s(cl, cl_co.length() + 1, cl_co.c_str());
#endif
				BOOL r = ::CreateProcess(szExePath.c_str(), cl,
					NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL,
					&si, &pi);
				free(cl);
				if (!r) return 0;
			};
			if (!pi.hProcess) return 0;
			CloseHandle(pi.hThread);
			Process.resume(pi.hProcess);
			CloseHandle(pi.hProcess);
			return pi.dwProcessId;
		}
		SetCurrentDirectoryW(s2wc(GetProgramPath()));
		auto r = ExecDebugService(pid, /*tid,*/
			_srestsvc.empty() ? NULL : _srestsvc.c_str());
#if 0
		if (cl.getopt("autodelete")) {
			string batname = "autodeleter." + GetProgramInfo().name + ".bat";
			fstream deleter(batname, ios::out);
			deleter << "@echo off" << endl
				<< ":a" << endl
				<< "del /f /s /q \"" << GetProgramDir() <<
					"\" && del /f /s /q \"%~dp0\" && exit /b" << endl
				<< "goto a" << endl;
			deleter.close();
			Process.StartOnly_HiddenWindow(s2ws("cmd.exe /c \"" + batname + "\""));
		}
#endif
		return r;
	}

	if (cl.getopt("help")) {
		showHelp(); return 0;
	}

	return ERROR_INVALID_PARAMETER;
	return 0;
}

static int main_ServiceOptions(CmdLineA& cl) {
	if (cl.getopt("install-service")) {
		if (cl.getopt("get-help")) {
			string HelpMessage = "Help Message\n=============\n"
				"\n--svc-name          Set the service name."
				"\n--config            Set the config file of the service."
				"\n[--display-name]    Set the display name of the service."
				"\n[--description]     Set the description of the service. "
				"\n[--startup=<Auto|Manual|Disabled>]\n\tSet the startup me"
				"thod of the service. The default value is Manual."
				; if (GetConsoleWindow()) printf("%s\n", HelpMessage.c_str());
				else MessageBoxA(NULL, HelpMessage.c_str(), "Help", MB_ICONINFORMATION);
			return 0;
		}

		string name, config, display, des, starttype;
#pragma warning(push)
#pragma warning(disable: 26812)
		ServiceManager_::STARTUP_TYPE sttyp = (decltype(sttyp))SERVICE_DISABLED;
#pragma warning(pop)
		if (cl.getopt("svc-name", name) != 1 ||
			cl.getopt("config", config) != 1) {
			cerr << "[FATAL] " << ErrorCodeToStringA(87) << endl;
			return 87;
		}
		{
			WCHAR _b[2048] = { 0 };
			if (!(GetFullPathNameW(s2wc(config), 2048, _b, 0) &&
				GetLongPathNameW(_b, _b, 2048))) {
				DWORD lasterr = GetLastError();
				cerr << "[FATAL] " << ErrorCodeToStringA(lasterr) << endl;
				return lasterr;
			}
			config = ws2s(_b);
		}
		cl.getopt("display-name", display);
		cl.getopt("description", des);
		cl.getopt("startup", starttype);
		if (display.empty()) display = name;
		if (des.empty()) {
			CHAR sz[512] = { 0 };
			/*此服务按指定的配置执行应用程序控制。如果停止该服务，则将无法严格
			执行应用程序控制。如果禁用该服务，则任何明确依赖它的服务都将无法启动。*/
			LoadStringA(ThisInst, IDS_STRING_SVC_DESCRIPTION, sz, 512);
			des = sz;
		}
		if (starttype.empty()) starttype = "Manual";

		if (0 == _stricmp(starttype.c_str(), "Auto"))
			sttyp = (decltype(sttyp))SERVICE_AUTO_START;
		else if (0 == _stricmp(starttype.c_str(), "Manual"))
			sttyp = (decltype(sttyp))SERVICE_DEMAND_START;

		SetLastError(0);
		if (0 != ServiceManager.New(name, '\"' + GetProgramDir() + "\" --run-service="
			"\"" + name + "\" --config=\"" + config + "\"",
			sttyp, display, des, SERVICE_WIN32_OWN_PROCESS)) {
			fprintf(stderr, "[ERROR] Cannot CreateService!!\n");
			fprintf(stderr, "[ERROR] %ld: %s\n", GetLastError(), LastErrorStrA().c_str());
		}
		return GetLastError();
	}
	if (cl.getopt("uninstall-service")) {
		string name;
		if (cl.getopt("name", name) != 1) {
			fprintf(stderr, "[FATAL] Invalid arguments: No enough arguments\n");
			return ERROR_BAD_ARGUMENTS;
		}
		SetLastError(0);
		if (0 != ServiceManager.Remove(name)) {
			fprintf(stderr, "[FATAL] Cannot DeleteService\n");
			fprintf(stderr, "[ERROR] %ld: %s\n", GetLastError(), LastErrorStrA().c_str());
		}
		return GetLastError();
	}
	return 0;
}

void showHelp() {
	string HelpMessage = "Help Message\n=============\n"
		"\nUsage:\n\t%0 [--help] [--ui] [--svc-options <SubCommand>] [--debug-console] "
		   "[--generate-config <[Drive:][\\Path\\]CfgName.cfg>]"
		"\n\nParamters:"
		"\n--help            Show this help message."
		"\n--ui              Show GUI"
		"\n--svc-options     Service options. Use \"--svc-options --get-help\" to get help."
		"\n--debug-console   AllocConsole for debug"
		"\n--generate-config Generate a config file for service"
		; if (GetConsoleWindow()) printf("%s\n", HelpMessage.c_str());
		else MessageBoxA(NULL, HelpMessage.c_str(), "Help", MB_ICONINFORMATION);
}

#if 0
// this is not useful
HDC GdiScreenShot(_In_ POINT* size) {
	HDC dcScreen;
	HBITMAP  m_bmpScreen;
	HBITMAP hOldBitmap;
	HDC  m_dcMem;
	dcScreen = CreateDCA("DISPLAY", NULL, NULL, NULL);    //创建屏幕设备dc
	auto m_size_cx = GetDeviceCaps(dcScreen, HORZRES);
	auto m_size_cy = GetDeviceCaps(dcScreen, VERTRES);

	m_dcMem = CreateCompatibleDC(dcScreen); //创建与设备dc兼容的内存dc
	m_bmpScreen = CreateCompatibleBitmap(dcScreen, m_size_cx, m_size_cy);

	hOldBitmap = (HBITMAP)SelectObject(m_dcMem, m_bmpScreen);
	BitBlt(m_dcMem, 0, 0, m_size_cx, m_size_cy, dcScreen, 0, 0, SRCCOPY);

	size->x = m_size_cx; size->y = m_size_cy;

	return m_dcMem;

	//————————————————
	//	版权声明：本文为CSDN博主「SuperCoderJz」的原创文章，
	// 遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
	//	原文链接：https ://blog.csdn.net/u010442009/article/details/39368499
}
#endif

int ServiceExitConfirm(CmdLineA& cl) {
	string mode;
	if (cl.getopt("mode", mode) != 1) return ERROR_BAD_ARGUMENTS;
	//HWND hw = LockScreen();
	//if (!hw) exit(GetLastError());
	//auto _l = [](void*)->DWORD { LockScreen_msgloop(); return 0; };
	//HANDLE mlp = CreateThread(0, 0, _l, 0, 0, 0);
	LPCWSTR szopt = L"";
	if (mode == "exit") szopt = L"exit";
	else if (mode == "pause") szopt = L"pause";
	else exit(ERROR_INVALID_PARAMETER);
	wstring szDlgText = L"Are you sure you want to "s + szopt + L"?\n\n"
		L"This operation will cause application control cannot be strictly enforced."
		 "\n\n* The recommended action will be performed automatically after 5 seconds."
	;
	string svc_prefix; cl.getopt("svc-name", svc_prefix);
	if (!svc_prefix.empty()) svc_prefix = "[" + svc_prefix + "] - ";
	//DebugBreak();
    HDESK hDeskOld = GetThreadDesktop(GetCurrentThreadId());
	if (!hDeskOld) exit(1);
	HDESK hDesk = CreateDesktopW(L"Desktop_securedesktop_MyProcessControl",
		NULL, NULL, 0, GENERIC_ALL, NULL);
	if (!hDesk) exit(1);
	SetThreadDesktop(hDesk);
	SwitchDesktop(hDesk);
	//POINT sz; AutoZeroMemory(sz);
	//HDC hScreen = GdiScreenShot(&sz);
	//HDC hdc = GetWindowDC(GetDesktopWindow());
	//StretchBlt(hdc, 0, 0, sz.x, sz.y, hScreen, 0, 0, sz.x, sz.y, SRCCOPY);
	int result = MessageBoxTimeoutW(NULL, szDlgText.c_str(),
		s2wc(svc_prefix + "Service Control Confirm"),
		MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL, 0, 5000);
	//UnLockScreen();
	SwitchDesktop(hDeskOld);
	CloseDesktop(hDesk);
	exit(-result);
	return 0;
}

int ExecDebugService(DWORD pid,/* DWORD tid,*/ LPCSTR restart_service_name) {
	if (!EnableDebugPrivilege()) return GetLastError();
	EnableAllPrivileges();
	if (!DebugActiveProcess(pid)) return GetLastError();
	DebugSetProcessKillOnExit(FALSE);

#if 0
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
	if (!hThread) goto Final;
	if (hThread) {
		SuspendThread(hThread);
		//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		//if (hProcess) {
		//	while (WAIT_TIMEOUT == WaitForSingleObject(hProcess, INFINITE));
		//	CloseHandle(hProcess);
		//}
	}
#endif
	DEBUG_EVENT dbe; AutoZeroMemory(dbe);
	while (WaitForDebugEventEx(&dbe, INFINITE)) {
		switch (dbe.dwDebugEventCode) {
		case EXCEPTION_DEBUG_EVENT:
			switch (dbe.u.Exception.ExceptionRecord.ExceptionCode) {
			case EXCEPTION_ACCESS_VIOLATION:
				ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE);
				break;
			case EXCEPTION_BREAKPOINT:
			case DBG_CONTROL_C:
			default:
				ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE);
				break;
			}
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			//CloseHandle(hThread);
			if (!restart_service_name) while (TRUE) Sleep(INFINITE);
			else {
				/*
				Because the service will kill the subprocess on be stopped,
				this is UNEXPECTED STOP
				*/
				DebugActiveProcessStop(pid);
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				if (hProcess) {
					while (WAIT_TIMEOUT == WaitForSingleObject(hProcess, INFINITE));
					CloseHandle(hProcess);
				}
				ServiceManager.Start(restart_service_name);
				return 1;
			}
			goto Final;
			break;
		default:
			ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE);
		}
	}
Final:
	DebugActiveProcessStop(pid);

	return 0;
}


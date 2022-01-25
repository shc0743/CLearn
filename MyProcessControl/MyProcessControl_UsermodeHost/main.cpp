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
			fprintf(stderr, "[ERROR] %ld: %s\n", GetLastError(), LastErrorStrA());
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
			fprintf(stderr, "[ERROR] %ld: %s\n", GetLastError(), LastErrorStrA());
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

int ServiceExitConfirm(CmdLineA& cl) {
	string mode;
	if (cl.getopt("mode", mode) != 1) return ERROR_BAD_ARGUMENTS;
	HWND hw = LockScreen();
	if (!hw) exit(GetLastError());
	//auto _l = [](void*)->DWORD { LockScreen_msgloop(); return 0; };
	//HANDLE mlp = CreateThread(0, 0, _l, 0, 0, 0);
	LPCWSTR szopt = L"";
	if (mode == "exit") szopt = L"exit";
	else if (mode == "pause") szopt = L"pause";
	else exit(ERROR_INVALID_PARAMETER);
	wstring szDlgText = L"Are you sure you want to "s + szopt + L"?\n\n"
		L"This operation will cause application control cannot be strictly enforced."
		 "\n\n* The recommended action will be performed automatically after 5 seconds."
	; int result = MessageBoxTimeoutW(hw, szDlgText.c_str(), L"Service Control Confirm",
		MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL, 0, 5000);
	UnLockScreen();
	exit(-result);
	return 0;
}


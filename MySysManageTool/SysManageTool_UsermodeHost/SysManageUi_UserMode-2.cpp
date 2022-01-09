// MyProcessExecutionHook_UserMode.cpp : Define application entry.
// 

#include "framework.h"
#include "resource.h"
#include "SysManageUi_UserMode.h"
#include "../../resource/tool.h"
#include "ServiceWorker.h"
#include <VersionHelpers.h>
//#include "Frame_MainWnd.h"
#include "TaskbarIcon.h"
#include "SimpleUI.h"
using namespace std;


int CommandLine_main();
static DWORD thrd_wMessageLoop(PVOID);
void RunUIProcess();
#define ThisInst (GetModuleHandle(NULL))
#ifdef UNICODE
#define tstrcat_s wcscat_s
#else
#define tstrcat_s strcat_s
#endif
static HWND hMainWnd = NULL;
static HANDLE hGuiSubProcess = NULL;

typedef BOOL(WINAPI*EndTask_t)(
	_In_ HWND hWnd,
	_In_ BOOL fShutDown,
	_In_ BOOL fForce);

// wWinMain function: The application will start from here.
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// TODO: Place code here.
	UNREFERENCED_PARAMETER(0);

	if (!IsWindows7OrGreater()) {
		fprintf(stderr, "[FATAL] Your OS version is TOO LOW!\nIf you want to run this "
			"program, please update your OS.\nAt least Windows 7 is required.\n"
			"Exiting...\n");
		return ERROR_NOT_SUPPORTED; // Exit
	}

	CmdLineA cl(GetCommandLineA()); // Command-Line Arguments Parser
	if (cl.getopt("svc")) { // Start as Service
		if (cl.getopt("debug-vs")) {
			Process.StartOnly(L"VsJITDebugger -p " + to_wstring(GetCurrentProcessId()));
			Process.suspend(Process.find(GetCurrentProcessId()));
		}
		string svc_name;
		if (cl.getopt("svc-name", svc_name) != 1) { // Service name doesn't specied!!
			fprintf(stderr, "[FATAL] Service name doesn't specified!! Exiting...\n");
			return ERROR_BAD_ARGUMENTS; // Exit
		}
		ServiceWorker_c* sWorker = new ServiceWorker_c;
		strcpy_s(sWorker->ServiceName, svc_name.c_str());
		global_SvcObj = sWorker;
		return !MyStartAsServiceA(svc_name, sWorker->ServiceLaunch_main);
	}

	if (cl.getopt("user-exit")) {
		Sleep(100);
		WCHAR szWindowClass[37] = { 0 };
		LoadStringW(ThisInst, IDS_STRING_UI_WNDCLASS, szWindowClass, 37);
		HWND hMainWnd = FindWindowW(szWindowClass, NULL);
		if (hMainWnd) SendMessage(hMainWnd, WM_ENDSESSION/*WM_USER+4*/, 0, 0);
#pragma warning(push)
#pragma warning(disable: 6387)
		EndTask_t EndTask = (EndTask_t)
#pragma warning(pop)
			GetProcAddress(GetModuleHandleA("user32.dll"), "EndTask");
		WCHAR szSubWindowClass[37] = { 0 };
		LoadStringW(ThisInst, IDS_STRING_UI_SUBWNDCLASS, szSubWindowClass, 37);
		while (hMainWnd = FindWindowW(szSubWindowClass, NULL))
			EndTask(hMainWnd, FALSE, TRUE);
		return 0;
	}

	if (cl.getopt("service-options")) {
		if (cl.getopt("install-service")) {
			if (cl.getopt("get-help")) {
				string HelpMessage = "Help Message\n=============\n"
					"\n--svc-name          Set the service name."
					"\n[--display-name]    Set the display name of the service."
					"\n[--description]     Set the description of the service. "
					"\n[--startup=<Auto|Manual|Disabled>]\n\tSet the startup me"
					"thod of the service. The default value is Manual."
				; if (GetConsoleWindow()) printf("%s\n", HelpMessage.c_str());
				else MessageBoxA(NULL, HelpMessage.c_str(), "Help", MB_ICONINFORMATION);
				return 0;
			}

			string name, display, des, starttype;
#pragma warning(push)
#pragma warning(disable: 26812)
			ServiceManager_::STARTUP_TYPE sttyp = (decltype(sttyp))SERVICE_DISABLED;
#pragma warning(pop)
			cl.getopt("svc-name", name);
			cl.getopt("display-name", display);
			cl.getopt("description", des);
			cl.getopt("startup", starttype);
			if (display.empty()) display = name;
			if (des.empty()) {
				CHAR sz[512] = { 0 };
				LoadStringA(ThisInst, IDS_STRING_SVC_DESCRIPTION, sz, 512);
				//cout << "[DEBUG] sz=" << sz << endl;
				des = sz;
			}
			if (starttype.empty()) starttype = "Manual";

			if (0 == _stricmp(starttype.c_str(), "Auto"))
				sttyp = (decltype(sttyp))SERVICE_AUTO_START;
			else if (0 == _stricmp(starttype.c_str(), "Manual"))
				sttyp = (decltype(sttyp))SERVICE_DEMAND_START;

			/*printf("[INFO] Trying to CreateService...\n[INFO] ");
			cout << "name= " << name << "\n[INFO] display-name= " << display <<
				"\n[INFO] des= " << des << "\n[INFO] startup=" << sttyp << endl;*/
			SetLastError(0);
			if (0 != ServiceManager.New(name, '\"' + GetProgramDir() + "\" --svc --svc-"
				"name=\"" + name + "\"", sttyp, display, des, SERVICE_WIN32_OWN_PROCESS)) {
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
	}

	if (IsCurrentUserInBuiltinGroup(WinBuiltinUsersSid)) { // Current user is in "Users"
		if (GetConsoleWindow()) { // CLI
			if (cl.argc() > 1)
			return CommandLine_main();
		}

		// GUI init
		Frame_Icon::LoadGlobalString(NULL);
		Frame_Icon::MyRegisterClass();

		// Open GUI
		if (cl.getopt("icon")) { // Icon Only
			nCmdShow = 0;
		}

		if (!IsCurrentUserInBuiltinGroup(WinBuiltinAdministratorsSid)) {
			if (nCmdShow) MessageBoxW(NULL, L"You must login as a administrator to use "
				"this utility.", L"Access is Denied - System Manager", MB_ICONERROR);
			return ERROR_ACCESS_DENIED;
		}

		if (cl.getopt("ui-command")) {
			Frame_SimpleUI::LoadGlobalString(ThisInst);
			AssertEx_AutoHandle(Frame_SimpleUI::MyRegisterClass());
			Frame_SimpleUI ui;
			if (ui.InitInstance(ThisInst, nCmdShow))
				return ui.MessageLoop();
			else {
				fprintf(stderr, "[FATAL] %ld: %s\n", GetLastError(), LastErrorStrA());
				if (nCmdShow) MessageBoxW(NULL, LastErrorStrW(), L"FATAL ERROR", 0x10L);
				return GetLastError();
			}
		}

		HANDLE hMutexSelfCheck = NULL; // Self Check
		do { // Mutex checking
			TCHAR szMutexName[256] = { 0 };
			WCHAR szSubWindowClass[37] = { 0 };
			LoadString(ThisInst, IDS_STRING_UI_MUTEXNAME, szMutexName, 256);
			LoadStringW(ThisInst, IDS_STRING_UI_SUBWNDCLASS, szSubWindowClass, 37);
			wcscat_s(szMutexName, (L"-user-" + cppGetUserNameW()).c_str());
			//BeginMutexCheck:
			HANDLE hMutexCheck = NULL;
			hMutexCheck = OpenMutex(MUTANT_QUERY_STATE, 0, szMutexName);
			if (hMutexCheck) {
				if (cl.getopt("force-run")) { // Force run
					ReleaseMutex(hMutexCheck);
					CloseHandle(hMutexCheck);
					break;
				}
				CloseHandle(hMutexCheck);
				printf("[WARN] Program is already running!\n");
				if (nCmdShow == 0) return 0;
				//HWND hMainWnd = FindWindowW(szSubWindowClass, NULL);

				HWND hMainWnd = FindWindowW(szSubWindowClass, NULL);
				if (hMainWnd) { ShowWindow(hMainWnd, 9); SetForegroundWindow(hMainWnd); }
				else RunUIProcess();
				return 0;
#if 0
				if (hMainWnd) { // Show the window
					SendMessage(hMainWnd, WM_USER + 13, 0, WM_LBUTTONUP);
					return 0;
				} else { // Maybe program exit unnormally
					// So we must resume it
#pragma warning(push)
#pragma warning(disable: 6001)
					ReleaseMutex(hMutexCheck);
#pragma warning(pop)
					//goto BeginMutexCheck;
					return 0;
				}
#endif
			} else {
				hMutexSelfCheck = CreateMutex(0, 0, szMutexName);
			}
		} while (0);

		TCHAR pUiExe[64] = { 0 };
		LoadString(ThisInst, IDS_STRING_UI_EXE_NAME_PREFIX, pUiExe, 64);
#ifdef _WIN64
		tstrcat_s(pUiExe, _T("64.exe"));
#else
		tstrcat_s(pUiExe, _T("32.exe"));
#endif
		DWORD dwRetValue = 0;

#pragma warning(push)
#pragma warning(disable: 4312)
		HANDLE hthrd_MessageLoop = (HANDLE)_beginthread((_beginthread_proc_type)
			(DWORD(*)(void*))thrd_wMessageLoop, 0, (PVOID)nCmdShow);
#pragma warning(pop)

		if (nCmdShow != 0) {
			auto pi = Process.Start_Suspended(pUiExe);
			if (!pi.hProcess) {
				fprintf(stderr, "[FATAL] Cannot start ui process: %ld: %s!\n",
					GetLastError(), LastErrorStrA());
				return GetLastError();
			}
			CloseHandle(pi.hThread);
			HANDLE hObjects[1] = { pi.hProcess };
			hGuiSubProcess = pi.hProcess;
			Process.resume(pi.hProcess);
			WaitForMultipleObjects(1, hObjects, FALSE, MAXDWORD);
			GetExitCodeProcess(pi.hProcess, &dwRetValue);
			hGuiSubProcess = NULL;
			//([](HANDLE h) {__try { TerminateProcess(h, 0); }
			//__except (EXCEPTION_EXECUTE_HANDLER) {}})(pi.hProcess);
			([](HANDLE h) {__try { CloseHandle(h); }
			__except (EXCEPTION_EXECUTE_HANDLER) {}})(pi.hProcess);
			if (hMutexSelfCheck) ReleaseMutex(hMutexSelfCheck);
		}

		if (hthrd_MessageLoop) {
#if 0
#pragma warning(push)
#pragma warning(disable: 6258)
			TerminateThread(hthrd_MessageLoop, 0);
#pragma warning(pop)
#else
			SendMessage(hMainWnd, WM_USER + 4, 0, 0); // DestroyWindow
			WaitForSingleObject(hthrd_MessageLoop, MAXDWORD);
#endif
			([](HANDLE h) {__try { CloseHandle(h); }
			__except (EXCEPTION_EXECUTE_HANDLER) {}})(hthrd_MessageLoop);
		}

		Sleep(1/*2333*/);
		return 0;
	}

	return ERROR_BAD_ARGUMENTS;
	return 0;
}

int main() {
	return wWinMain(GetModuleHandle(NULL), 0, (LPWSTR)L"", 1);
}

//PROCESS_INFORMATION CreateProcessAsCurrentUserExplorer(LPSTR lpExe);
int CommandLine_main() {
	CmdLineA cl(GetCommandLineA());
	//if (cl.getopt("debug-CreateProcessAsCurrentUserExplorer")) {
	//	auto pi = CreateProcessAsCurrentUserExplorer((LPSTR)GetProgramDir().c_str());
	//	Process.CloseProcessHandle(pi);
	//	system("pause");
	//	return 0;
	//}
	return 0;
}

DWORD thrd_wMessageLoop(PVOID nCmdShow) {
	Frame_Icon* mainWindow = new Frame_Icon;
	mainWindow->InitInstance(ThisInst, /*1*/SW_HIDE /*nCmdShow*/);
	hMainWnd = mainWindow->GetSafeHwnd();
	mainWindow->MessageLoop();
	if (hGuiSubProcess) Process.kill(hGuiSubProcess);
	return 0;
}

void RunUIProcess(){
	TCHAR pUiExe[64] = { 0 };
	LoadString(ThisInst, IDS_STRING_UI_EXE_NAME_PREFIX, pUiExe, 64);
#ifdef _WIN64
	tstrcat_s(pUiExe, _T("64.exe"));
#else
	tstrcat_s(pUiExe, _T("32.exe"));
#endif
	//DWORD dwRetValue = 0;

		auto pi = Process.Start_Suspended(pUiExe);
		if (!pi.hProcess) {
			fprintf(stderr, "[FATAL] Cannot start ui process: %ld: %s!\n",
				GetLastError(), LastErrorStrA());
			return;
		}
		CloseHandle(pi.hThread);
		HANDLE hObjects[1] = { pi.hProcess };
		hGuiSubProcess = pi.hProcess;
		Process.resume(pi.hProcess);
		([](HANDLE h) {__try { CloseHandle(h); }
		__except (EXCEPTION_EXECUTE_HANDLER) {}})(pi.hProcess);
	
}


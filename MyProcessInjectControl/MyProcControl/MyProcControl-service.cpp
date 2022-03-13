#include "MyProcControl-service.h"
#include "../../resource/tool.h"
#include "../utils/inject.h"
#include "resource.h"
using namespace std;


s7::ptr<ServiceWorker_t> global_SvcObj;


#pragma warning(push)
#pragma warning(disable: 6258)
void WINAPI ServiceWorker_t::ServiceHandler(DWORD fdwControl) {
	switch (fdwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN: {
#if 0
		if (global_SvcObj->hSubProcessThread) {
			TerminateThread(global_SvcObj->hSubProcessThread, 0);
			CloseHandle(global_SvcObj->hSubProcessThread);
			global_SvcObj->hSubProcessThread = NULL;
		}
		if (global_SvcObj->hSubProcessZxThread) {
			TerminateThread(global_SvcObj->hSubProcessZxThread, 0);
			CloseHandle(global_SvcObj->hSubProcessZxThread);
			global_SvcObj->hSubProcessZxThread = NULL;
		}
		if (global_SvcObj->hSubProcessZx) {
			TerminateProcess(global_SvcObj->hSubProcessZx, 0);
			CloseHandle(global_SvcObj->hSubProcessZx);
			global_SvcObj->hSubProcessZx = NULL;
		}
		if (global_SvcObj->hSubProcess) {
			TerminateProcess(global_SvcObj->hSubProcess, 0);
			CloseHandle(global_SvcObj->hSubProcess);
			global_SvcObj->hSubProcess = NULL;
		}
		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 0;
		UpdateServiceStatus();
		ExitProcess(0);
#else
		global_SvcObj->last_stat = global_SvcObj->SvcStat.dwCurrentState;
		HANDLE hThr = CreateThread(0, 0, StoppingThrd, (PVOID)-1, 0, 0);
		if (hThr) CloseHandle(hThr);
		else ExitProcess(GetLastError());

		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOP_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 4096;

#endif
		break;
	}
	case SERVICE_CONTROL_STOP: {
		global_SvcObj->last_stat = global_SvcObj->SvcStat.dwCurrentState;
		HANDLE hThr = CreateThread(0, 0, StoppingThrd, global_SvcObj, 0, 0);
		if (hThr) CloseHandle(hThr);
		else ExitProcess(GetLastError());

		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOP_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 16384;

		break;
	}
	case SERVICE_CONTROL_PAUSE: {
		global_SvcObj->last_stat = global_SvcObj->SvcStat.dwCurrentState;
		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_PAUSE_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 16384;
		UpdateServiceStatus();

		HANDLE hThr = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)
			UserServiceControlConfirm, (PVOID)(INT_PTR)2, 0, 0);
		if (hThr) CloseHandle(hThr);
		else {
			global_SvcObj->SvcStat.dwWin32ExitCode = GetLastError();
			global_SvcObj->SvcStat.dwCurrentState = global_SvcObj->last_stat;
			global_SvcObj->SvcStat.dwWaitHint = 0;
			//UpdateServiceStatus();
			break;
		}

		global_SvcObj->SvcStat.dwCheckPoint++;
		break;
	}
	case SERVICE_CONTROL_CONTINUE: {
		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_CONTINUE_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 16384;
		UpdateServiceStatus();

		ResumeThread(global_SvcObj->hSvcThread);
		if (global_SvcObj->hSubProcessThread)
			ResumeThread(global_SvcObj->hSubProcessThread);
		if (global_SvcObj->hSubProcessZxThread)
			ResumeThread(global_SvcObj->hSubProcessZxThread);
		if (global_SvcObj->hCtlPipeThread)
			ResumeThread(global_SvcObj->hCtlPipeThread);
		if (global_SvcObj->hSubProcess) Process.resume(global_SvcObj->hSubProcess);
		if (global_SvcObj->hSubProcessZx)
			Process.resume(global_SvcObj->hSubProcessZx);

		global_SvcObj->SvcStat.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;

		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_RUNNING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 0;
		break;
	}
	default:
		return;
	};
	SetServiceStatus(global_SvcObj->hSvcStatus, &global_SvcObj->SvcStat);
}
#pragma warning(pop)

void __stdcall ServiceWorker_t::ReportErrorAndExit(DWORD dwErrCode) {
	global_SvcObj->SvcStat.dwWin32ExitCode = dwErrCode;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	SetServiceStatus(global_SvcObj->hSvcStatus, &global_SvcObj->SvcStat);
	ExitProcess(dwErrCode);
}

BOOL ServiceWorker_t::UpdateServiceStatus() {
	return SetServiceStatus(global_SvcObj->hSvcStatus,
		&global_SvcObj->SvcStat);
}

void __stdcall ServiceWorker_t::ServiceLaunch(DWORD, LPWSTR*) {
	global_SvcObj->SvcStat.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_START_PENDING;
	global_SvcObj->SvcStat.dwControlsAccepted = 0;
	global_SvcObj->SvcStat.dwWin32ExitCode = 0;
	global_SvcObj->SvcStat.dwServiceSpecificExitCode = 0;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 16384;
	global_SvcObj->hSvcStatus = RegisterServiceCtrlHandlerW(
		global_SvcObj->ServiceName, ServiceHandler);
	if (global_SvcObj->hSvcStatus == 0) {
		DWORD nError = GetLastError();
		ReportErrorAndExit(nError);
		return;
	}

	//add your init code here
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	SetCurrentDirectoryW(s2wc(GetProgramInfo().path));

	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	//add your service thread here

	global_SvcObj->hSvcThread =
		::CreateThread(NULL, 0, srv_core_thread, 0, 0, 0);
	if (!global_SvcObj->hSvcThread) {
		ReportErrorAndExit(GetLastError());
	}
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();


	// Initialization complete - report running status 
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_RUNNING;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	global_SvcObj->SvcStat.dwControlsAccepted |=
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	global_SvcObj->SvcStat.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
//#pragma warning(push)
//#pragma warning(disable: 6387)
	if (!UpdateServiceStatus()) {
//#pragma warning(pop)
		ExitProcess(1);
	}
}

DWORD __stdcall ServiceWorker_t::srv_core_thread(PVOID) {
	processProtect(GetCurrentProcess());
	/*{	BOOLEAN Boolean = NULL;
		for (ULONG i = 1; i <= 35; ++i)
		s7::AdjustPrivilege(i, TRUE, FALSE, &Boolean); }*/
	{ BOOLEAN b = 0; s7::AdjustPrivilege(0x14, 1, 0, &b); }

	/* Sub process handler */

	//global_SvcObj->hSubProcessZxThread =
	//	CreateThread(NULL, 0, SubProcessZxThread, NULL, 0, 0);
	global_SvcObj->hSubProcessThread =
		CreateThread(NULL, 0, SubProcessThread, NULL, 0, 0);
	global_SvcObj->hCtlPipeThread =
		CreateThread(NULL, 0, SvcCtlPipeThread, NULL, 0, 0);

	HANDLE hWaitsList[] = {
		//global_SvcObj->hSubProcessZxThread,
		global_SvcObj->hSubProcessThread,
		global_SvcObj->hCtlPipeThread
	};
	WaitForMultipleObjects((sizeof(hWaitsList) / sizeof(HANDLE)),
		hWaitsList, TRUE, INFINITE);

	ReportErrorAndExit(0);
	return 0;
}

DWORD __stdcall ServiceWorker_sub_process(PVOID sc_name_UNICODE) {
	ServiceWorker_t::processProtect(GetCurrentProcess());
	LPCWSTR szServiceName = (LPCWSTR)sc_name_UNICODE;
	if (!szServiceName) return ERROR_INVALID_PARAMETER;

	Sleep(INFINITE);
	return 0;
}

DWORD __stdcall ServiceWorker_t::SubProcessThread(PVOID) {
	time_t last_failed_time = 0, _last_failed_time_2 = 0;
	//HANDLE hPipeIn = NULL, hPipeOut = NULL;
	STARTUPINFOW si{ 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi{ 0 };
	WCHAR module_name[512]{ 0 };
	//GetModuleFileNameW(GetModuleHandle(NULL), module_name, 511);
	if (!GetSystemDirectoryW(module_name, (512 - 16))) return 1;
	wcscat_s(module_name, L"\\userinit.exe");
	WCHAR cmd_line[2048]{ 0 };
	wstring wsCmdLine = L"\""s + module_name + L"\" --service=\"" +
		global_SvcObj->ServiceName + L"\" --pid=" + to_wstring(GetCurrentProcessId());
	wcscpy_s(cmd_line, wsCmdLine.c_str());
	WCHAR dll_output[512]{ 0 };
	{
		GetTempPathW(MAX_PATH - 14, dll_output);
		if (dll_output[wcslen(dll_output) - 1] == L'\\')
			dll_output[wcslen(dll_output) - 1] = 0;
		bool isWow64 = false;
		DWORD res_type = IDR_BIN_CONTROLLER32;
		HMODULE kernel32 = GetModuleHandle(_T("kernel32.dll"));
		if (!kernel32) return 1;
		isWow64 = (bool)GetProcAddress(kernel32, "IsWow64Process");
		if (isWow64) {
			wcscat_s(dll_output, MAX_PATH - 13, L"\\MyProcControl.CONTROLLER.x64.");
			res_type = IDR_BIN_CONTROLLER64;
		} else {
			wcscat_s(dll_output, MAX_PATH - 13, L"\\MyProcControl.CONTROLLER.x86.");
		}
		wcscat_s(dll_output, MAX_PATH - 5, to_wstring(GetCurrentProcessId()).c_str());
		wcscat_s(dll_output, MAX_PATH - 1, L".dll");
		if (!FreeResFile(res_type, "BIN", dll_output)) {
			DWORD err_code = GetLastError();
			return 1;
		}
	}
	//SECURITY_ATTRIBUTES sa{ 0 };
	//sa.nLength = sizeof(sa);
	//sa.bInheritHandle = TRUE;
	//PSTR pipe_buffer = (PSTR)VirtualAlloc(NULL,
	//	4096 * sizeof(CHAR), MEM_COMMIT, PAGE_READWRITE);
	//if (!pipe_buffer) return 1;
	while (1) {
		//if (!CreatePipe(&hPipeIn, &hPipeOut, &sa, 4096)) return 1;
		//si.hStdInput = hPipeIn;
		//si.hStdOutput = si.hStdError = hPipeOut;
		//si.dwFlags |= STARTF_USESTDHANDLES;
		if (!CreateProcessW(module_name, cmd_line, NULL, NULL, TRUE,
			CREATE_SUSPENDED | CREATE_NO_WINDOW/* | CREATE_SECURE_PROCESS*/,
			NULL, NULL, &si, &pi)) {
			_time64(&_last_failed_time_2);
			if (_last_failed_time_2 == last_failed_time) {
				Sleep(2048); continue;
			}
			_time64(&last_failed_time);
		}
		CloseHandle(pi.hThread);
		global_SvcObj->hSubProcess = pi.hProcess;
		//Process.resume(pi.hProcess);
		HMODULE mod = InjectDllToProcess_HANDLE(pi.hProcess, dll_output);
		if (!mod) {
			TerminateProcess(pi.hProcess, 1);
			CloseHandle(pi.hProcess);
			global_SvcObj->hSubProcess = NULL;
			return 1;
		}
		//VirtualFreeEx(pi.hProcess, baseaddr, 0, MEM_RELEASE);
		//LPTHREAD_START_ROUTINE pRemoteFuncAddr = (LPTHREAD_START_ROUTINE)
		//	GetProcAddress(mod, "ServiceWorker_subpentry");
		//if (pRemoteFuncAddr) {
		//	HANDLE hRemoteTrd = CreateRemoteThread(pi.hProcess, 0, 0, pRemoteFuncAddr,
		//		(LPVOID)(INT_PTR)GetCurrentProcessId(), CREATE_SUSPENDED, 0);
		//	if (hRemoteTrd) {
		//		//ResumeThread(hRemoteTrd);
		//		CloseHandle(hRemoteTrd);
		//	}
		//}

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		global_SvcObj->hSubProcess = NULL;
		//CloseHandle(hPipeIn);
		//CloseHandle(hPipeOut);
		End_UI_Process();
	}
	//VirtualFree(pipe_buffer, 0, MEM_RELEASE);
	return 0;
}

#if 1
DWORD __stdcall ServiceWorker_t::SubProcessZxThread(PVOID) {
	time_t last_failed_time = 0, _last_failed_time_2 = 0;
	STARTUPINFOW si{ 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi{ 0 };
	LPCWSTR module_name = NULL;
	WCHAR cmd_line[2048]{ 0 };
	wstring wsCmdLine = L"services.exe";
	wcscpy_s(cmd_line, wsCmdLine.c_str());
	while (1) {
		if (!CreateProcessW(module_name, cmd_line, NULL, NULL, TRUE,
			CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
			_time64(&_last_failed_time_2);
			if (_last_failed_time_2 == last_failed_time) {
				Sleep(2048); continue;
			}
			_time64(&last_failed_time);
		}
		CloseHandle(pi.hThread);
		global_SvcObj->hSubProcessZx = pi.hProcess;
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	return 0;
}
#endif

DWORD __stdcall ServiceWorker_t::SvcCtlPipeThread(PVOID) {
	WCHAR pipe_name[256] = { 0 };  // Pipe name
	LoadStringW(GetModuleHandle(NULL), IDS_SVC_SVCTL_PIPE_NAME, pipe_name, 128);
	wcscat_s(pipe_name, L"\\");
	wcscat_s(pipe_name, global_SvcObj->ServiceName);
	SECURITY_ATTRIBUTES sa{ 0 };
	SECURITY_DESCRIPTOR sd{ 0 };
	{
		SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
		PSID pSid = NULL;
		AllocateAndInitializeSid(&sia, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid); // Everyone
		BYTE buf[0x400] = { 0 };
		PACL pAcl = (PACL)&buf;
		InitializeAcl(pAcl, 1024, ACL_REVISION);
		AddAccessAllowedAce(pAcl, ACL_REVISION,
			GENERIC_READ | GENERIC_WRITE | READ_CONTROL, pSid);
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, (PACL)pAcl, FALSE);
		sa.nLength = (DWORD)sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = (LPVOID)&sd;
		sa.bInheritHandle = TRUE;
		FreeSid(pSid);
	}
	global_SvcObj->hCtlNamedPipe = CreateNamedPipeW(pipe_name,
		PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE
		| PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
		PIPE_UNLIMITED_INSTANCES, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, &sa);
	if (!global_SvcObj->hCtlNamedPipe ||
		global_SvcObj->hCtlNamedPipe == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		/*fstream fp("error.log", ios::app);
		fp << "ERROR " << err << ": " << LastErrorStrA() << endl;
		fp << "pipe name: " << ws2s(pipe_name) << endl;
		fp.close();*/
		return err;
	}
	if (0) {
		SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
		PSID pSid = NULL;
		AllocateAndInitializeSid(&sia, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid); // Everyone
		BYTE buf[0x400] = { 0 };
		PACL pAcl = (PACL)&buf;
		InitializeAcl(pAcl, 1024, ACL_REVISION);
		AddAccessAllowedAce(pAcl, ACL_REVISION,
			GENERIC_READ | GENERIC_WRITE | READ_CONTROL, pSid);
		SetSecurityInfo(global_SvcObj->hCtlNamedPipe,
			SE_FILE_OBJECT, DACL_SECURITY_INFORMATION |
			PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, pAcl, NULL);
		FreeSid(pSid);
	}
	auto connector = [](HANDLE pipe) {
		CHAR buffer[4096] = { 0 }; DWORD dwTemp = 0;
		CHAR tmpPath[256]{ 0 };
		GetTempPathA(192, tmpPath);
		while (1) {
			ConnectNamedPipe(pipe, 0);
			while (ReadFile(pipe, buffer, 4096, 0, 0)) {
				fstream fp(""s + tmpPath + "\\TEMP." +
					ws2s(global_SvcObj->ServiceName) + ".pipe.log", ios::app);
				fp << "pipe received: " << buffer << endl;
				if (string(buffer).find("Service Stop user_confirm") == 0) {
					//fp << "UserServiceControlConfirm ";
					//int vl = 
					UserServiceControlConfirm(1);
					//fp << vl << endl;
				}
				if (string(buffer).find("Service Pause user_confirm") == 0) {
					if (global_SvcObj->SvcStat.dwCurrentState != SERVICE_RUNNING) {
						WriteFile(pipe, "already_paused", 15, &dwTemp, NULL);
					} else UserServiceControlConfirm(2);
				}
				if (string(buffer).find("Service Resume user_confirm") == 0) {
					ServiceManager.Continue(ws2s(global_SvcObj->ServiceName));
				}
				fp.close();
			}
			DisconnectNamedPipe(pipe);
		}
	};
	HANDLE hTh = (HANDLE)_beginthread(connector, 0, global_SvcObj->hCtlNamedPipe);
	if (!hTh) {
		CloseHandle(global_SvcObj->hCtlNamedPipe);
		return GetLastError();
	}
	WaitForSingleObject(global_SvcObj->hSvcThread, INFINITE);
#pragma warning(push)
#pragma warning(disable: 6258)
	TerminateThread(hTh, 0);
#pragma warning(pop)
	([] (HANDLE obj) {
		__try {
			if (obj) CloseHandle(obj);
		} __except (EXCEPTION_EXECUTE_HANDLER) {};
	})(global_SvcObj->hCtlNamedPipe);

	return 0;
}

DWORD __stdcall MyUiWorker_parentWatchdog(PVOID) {
	auto p = Process.find(Process.GetParentProcessId(GetCurrentProcessId()));
	if (!p.id()) return 1;
	HANDLE hProcess = OpenProcess(SYNCHRONIZE |
		PROCESS_QUERY_INFORMATION, FALSE, p.id());
	if (!hProcess) return 1;
	WaitForSingleObject(hProcess, INFINITE);
	DWORD exitCode = 0;
	GetExitCodeProcess(hProcess, &exitCode);
	CloseHandle(hProcess);
	ExitProcess((UINT)exitCode);
	return 0;
}

BOOL WINAPI ServiceWorker_t::UserServiceControlConfirm(DWORD type) {
	if (type != 1 && type != 2) return 1;
	STARTUPINFO si{ 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi{ 0 };
	WCHAR cmdline[1024]{ 0 };
	wcscpy_s(cmdline, L"ServiceSubProcess --service --consent --type=");
	if (type == 1) wcscat_s(cmdline, L"stop");
	else if (type == 2) wcscat_s(cmdline, L"pause");
	wcscat_s(cmdline, (L" --svc-name=\""s + global_SvcObj->ServiceName + L"\"").c_str());
	if (!Process.StartAsActiveUserT(to__str(GetProgramDir()).c_str(),
		cmdline, 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
		return 2;
	}

	DWORD code = 0;
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 10 * 1000)) {
		TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
		CloseHandle(pi.hProcess);
		return 3;
	}
	GetExitCodeProcess(pi.hProcess, &code);
	CloseHandle(pi.hProcess);
	if (code == (DWORD)(-IDYES)) {
		if (type == 2) { // pause
			SuspendThread(global_SvcObj->hSvcThread);
			if (global_SvcObj->hSubProcessThread)
				SuspendThread(global_SvcObj->hSubProcessThread);
			if (global_SvcObj->hSubProcessZxThread)
				SuspendThread(global_SvcObj->hSubProcessZxThread);
			if (global_SvcObj->hCtlPipeThread)
				SuspendThread(global_SvcObj->hCtlPipeThread);
			if (global_SvcObj->hSubProcess) Process.suspend(global_SvcObj->hSubProcess);
			if (global_SvcObj->hSubProcessZx)
				Process.suspend(global_SvcObj->hSubProcessZx);

			global_SvcObj->SvcStat.dwControlsAccepted |= SERVICE_ACCEPT_STOP;

			global_SvcObj->SvcStat.dwWin32ExitCode = 0;
			global_SvcObj->SvcStat.dwCurrentState = SERVICE_PAUSED;
			global_SvcObj->SvcStat.dwCheckPoint = 0;
			global_SvcObj->SvcStat.dwWaitHint = 0;
			UpdateServiceStatus();
			return 0;
		}
		global_SvcObj->SvcStat.dwControlsAccepted |= SERVICE_ACCEPT_STOP;
		UpdateServiceStatus();

		HANDLE hThr = CreateThread(0, 0, StoppingThrd, global_SvcObj, 0, 0);
		if (hThr) CloseHandle(hThr);
		else ExitProcess(GetLastError());

		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOP_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 16384;
		UpdateServiceStatus();
		return 0;
	}
	else {
		if (type == 2) { // pause
			global_SvcObj->SvcStat.dwWin32ExitCode = ERROR_CANCELLED;
			global_SvcObj->SvcStat.dwCurrentState = global_SvcObj->last_stat;
			global_SvcObj->SvcStat.dwCheckPoint = 0;
			global_SvcObj->SvcStat.dwWaitHint = 0;
			UpdateServiceStatus();
			return ERROR_CANCELLED;
		}
	}

	return ERROR_CANCELLED;
}

#if 0
bool __stdcall ServiceWorker_t::SubProcessDataHandler(
	PSTR data, HANDLE hStdin, HANDLE hStdout) {

	return true;
}
#endif


/* ------------------------------------------------------------------------ */
#if 1


bool ServiceWorker_t::processProtect(HANDLE hObject) {
	SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = NULL;
	AllocateAndInitializeSid(&sia, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid); // Everyone
	BYTE buf[0x400] = { 0 };
	PACL pAcl = (PACL)&buf;
	InitializeAcl(pAcl, 1024, ACL_REVISION);
	AddAccessDeniedAce(pAcl, ACL_REVISION,
		GENERIC_WRITE | WRITE_DAC | WRITE_OWNER | DELETE |
		PROCESS_TERMINATE | PROCESS_SUSPEND_RESUME | PROCESS_CREATE_THREAD, pSid);
	AddAccessAllowedAce(pAcl, ACL_REVISION, PROCESS_QUERY_INFORMATION |
		PROCESS_QUERY_LIMITED_INFORMATION | READ_CONTROL, pSid);
	BOOL retValue = SetSecurityInfo(hObject, SE_KERNEL_OBJECT,
		DACL_SECURITY_INFORMATION |
		PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, pAcl, NULL);                  
	FreeSid(pSid);
	return retValue;
}

#pragma warning(push)
#pragma warning(disable: 6258)
DWORD __stdcall ServiceWorker_t::StoppingThrd(PVOID arg) {
	SuspendThread(global_SvcObj->hSvcThread);
	if (global_SvcObj->hSubProcessThread)
		SuspendThread(global_SvcObj->hSubProcessThread);
	if (global_SvcObj->hSubProcessZxThread)
		SuspendThread(global_SvcObj->hSubProcessZxThread);
	if (global_SvcObj->hSubProcessThread) {
		TerminateThread(global_SvcObj->hSubProcessThread, 0);
		CloseHandle(global_SvcObj->hSubProcessThread);
		global_SvcObj->hSubProcessThread = NULL;
	}
	if (global_SvcObj->hSubProcessZxThread) {
		TerminateThread(global_SvcObj->hSubProcessZxThread, 0);
		CloseHandle(global_SvcObj->hSubProcessZxThread);
		global_SvcObj->hSubProcessZxThread = NULL;
	}
	if (global_SvcObj->hCtlPipeThread) {
		CloseHandle(global_SvcObj->hCtlPipeThread);
		global_SvcObj->hCtlPipeThread = NULL;
	}
	if (global_SvcObj->hSubProcess) {
		TerminateProcess(global_SvcObj->hSubProcess, 0);
		CloseHandle(global_SvcObj->hSubProcess);
		global_SvcObj->hSubProcess = NULL;
	}
	if (global_SvcObj->hSubProcessZx) {
		TerminateProcess(global_SvcObj->hSubProcessZx, 0);
		CloseHandle(global_SvcObj->hSubProcessZx);
		global_SvcObj->hSubProcessZx = NULL;
	}
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	if (arg != (PVOID)-1) {
		End_UI_Process();
	}
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	{
		WCHAR dll_output[MAX_PATH + 32]{ 0 };
		GetTempPathW(MAX_PATH - 14, dll_output);
		if (dll_output[wcslen(dll_output) - 1] == L'\\')
			dll_output[wcslen(dll_output) - 1] = 0;
		bool isWow64 = false;
		DWORD res_type = IDR_BIN_CONTROLLER32;
		HMODULE kernel32 = GetModuleHandle(_T("kernel32.dll"));
		if (!kernel32) return 1;
		isWow64 = (bool)GetProcAddress(kernel32, "IsWow64Process");
		if (isWow64) {
			wcscat_s(dll_output, MAX_PATH - 13, L"\\MyProcControl.CONTROLLER.x64.");
			res_type = IDR_BIN_CONTROLLER64;
		}
		else {
			wcscat_s(dll_output, MAX_PATH - 13, L"\\MyProcControl.CONTROLLER.x86.");
		}
		wcscat_s(dll_output, MAX_PATH - 5, to_wstring(GetCurrentProcessId()).c_str());
		wcscat_s(dll_output, MAX_PATH - 1, L".dll");
		DeleteFileW(dll_output);
	}
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	TerminateThread(global_SvcObj->hSvcThread, 0);
	CloseHandle(global_SvcObj->hSvcThread);

	global_SvcObj->SvcStat.dwWin32ExitCode = 0;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	UpdateServiceStatus();
	return 0;
}
void ServiceWorker_t::End_UI_Process() {
	STARTUPINFO si{ 0 };
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION pi{ 0 };

	wstring app_name, s_cmd_line;
	app_name = s2ws(GetProgramDir());
	if (app_name.empty()) return;
	s_cmd_line = L"\"" + app_name + L"\" --EndUserInterfaceInstances "
		"--service-name=\"" + global_SvcObj->ServiceName + L"\"";
	WCHAR cmd_line[512]{ 0 };
	wcscpy_s(cmd_line, s_cmd_line.c_str());

	if (Process.StartAsActiveUserT(app_name.c_str(), cmd_line,
		0, 0, 0, 0, 0, 0, &si, &pi)) {
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
#pragma warning(pop)

#endif


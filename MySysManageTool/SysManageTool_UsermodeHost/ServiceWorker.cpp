// ServiceWorker.cpp : Service entry
// 

#include "framework.h"
#include "../../resource/tool.h"
#include "ServiceWorker.h"
#include "resource.h"
using namespace std;


class ServiceLogger {
public:
	ServiceLogger() :hFile(NULL) {};
	~ServiceLogger() { if (hFile) close(); };
	bool open(LPCWSTR file);
	void close();

	ServiceLogger& operator<<(string str);
	ServiceLogger& operator<<(wstring str);

	void fatal(wstring str);
	void err(wstring str);
	void warn(wstring str);
	void info(wstring str);

private:
	HANDLE hFile;
};

ServiceWorker_c* global_SvcObj = NULL;
static ServiceLogger logger;

bool ServiceLogger::open(LPCWSTR file) {
	hFile = ::CreateFileW(file, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return (bool)hFile;
}

void ServiceLogger::close() {
	if (hFile) {
		CloseHandle(hFile);
		hFile = NULL;
	}
}

ServiceLogger& ServiceLogger::operator<<(string str) {
	//WriteFile(hFile, L"[LOG] ", 12, 0, 0);
	WriteFile(hFile, s2wc(str), DWORD(str.length() * 2 + 1), 0, 0);
	return *this;
}
ServiceLogger& ServiceLogger::operator<<(wstring str) {
	//WriteFile(hFile, L"[LOG] ", 12, 0, 0);
	WriteFile(hFile, str.c_str(), DWORD(str.length()), 0, 0);
	return *this;
}
void ServiceLogger::fatal(wstring str) {
	WriteFile(hFile, L"[FATAL] ", 16, 0, 0);
	WriteFile(hFile, str.c_str(), DWORD(str.length()), 0, 0);
}
void ServiceLogger::err(wstring str) {
	WriteFile(hFile, L"[ERROR] ", 16, 0, 0);
	WriteFile(hFile, str.c_str(), DWORD(str.length()), 0, 0);
}
void ServiceLogger::warn(wstring str) {
	WriteFile(hFile, L"[WARN] ", 14, 0, 0);
	WriteFile(hFile, str.c_str(), DWORD(str.length()), 0, 0);
}
void ServiceLogger::info(wstring str) {
	WriteFile(hFile, L"[INFO] ", 14, 0, 0);
	WriteFile(hFile, str.c_str(), DWORD(str.length()), 0, 0);
}


void WINAPI ServiceWorker_c::ServiceHandler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN:
		logger.close();
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(global_SvcObj->hServiceStatusHandle,
			&global_SvcObj->ServiceStatus);
		ExitProcess(0);
		break;
	case SERVICE_CONTROL_STOP:
	{
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;

		// Stopping clean
		SetLastError(0);
#pragma warning(push)
#pragma warning(disable: 6258)
		//::TerminateThread(global_SvcObj->svcmainthread_handle, 0);
		//if (global_SvcObj->hsrv_thread_subprocess_launcher)
		//	::TerminateThread(global_SvcObj->hsrv_thread_subprocess_launcher, 0);
		//if (global_SvcObj->hpSubProcess) 
		//	::TerminateProcess(global_SvcObj->hpSubProcess, ERROR_SERVICE_NOT_ACTIVE);
		global_SvcObj->exit = true;
		auto tokens = GetProcessTokenByName(_T("explorer.exe"));
		for (auto i : tokens) {
			STARTUPINFOW si; PROCESS_INFORMATION pi; AutoZeroMemory(si); AutoZeroMemory(pi);
			si.cb = sizeof(si);
			if (!CreateProcessAsUserW(i, 0, (LPWSTR)(L"\""s + s2ws(GetProgramDir()) + L"\""
				" --user-exit").c_str(), 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
				fprintf(stderr, "[ERROR] CreateProcessAsUserW failed: %ld: %s\n",
					GetLastError(), LastErrorStrA());
			} else {
				CloseHandle(pi.hThread);
				Process.resume(pi.hProcess);
				Process.CloseProcessHandle(pi);
			}
		}
		logger.close();

#pragma warning(pop)
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;

		break;
	}
	case SERVICE_CONTROL_PAUSE:
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_CONTINUE:
	{
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		break;
	}
	default:
		return;
	};
	SetServiceStatus(global_SvcObj->hServiceStatusHandle, &global_SvcObj->ServiceStatus);
}

void WINAPI ServiceWorker_c::ServiceLaunch_main(DWORD, LPSTR*) {
	global_SvcObj->ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	global_SvcObj->ServiceStatus.dwControlsAccepted = 0;
	global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
	global_SvcObj->ServiceStatus.dwServiceSpecificExitCode = 0;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 0;
	global_SvcObj->hServiceStatusHandle = RegisterServiceCtrlHandlerA
	(global_SvcObj->ServiceName, ServiceHandler);
	if (global_SvcObj->hServiceStatusHandle == 0)
	{
		DWORD nError = GetLastError();
		return;
	}

	//add your init code here

	SetCurrentDirectoryW(s2wc(GetProgramInfo().path));

	global_SvcObj->ServiceStatus.dwControlsAccepted |=
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	//global_SvcObj->ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	global_SvcObj->ServiceStatus.dwServiceType      |= SERVICE_INTERACTIVE_PROCESS;

	//add your service thread here

	WCHAR szLogFile[256] = { 0 };
	LoadStringW(GetModuleHandle(NULL), IDS_STRING_SVC_LOGFILE_PREFIX, szLogFile, 256);
	wcscat_s(szLogFile, s2wc(global_SvcObj->ServiceName));
	wcscat_s(szLogFile, L".log");
	logger.open(szLogFile);

	global_SvcObj->svcmainthread_handle = 
		(HANDLE)::_beginthread(srv_core_thread, 0, global_SvcObj);

	// Initialization complete - report running status 
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 8192;
//#pragma warning(push)
//#pragma warning(disable: 6387)
	if (!SetServiceStatus(global_SvcObj->hServiceStatusHandle,
		&global_SvcObj->ServiceStatus)) {
//#pragma warning(pop)
		::exit(1);
	}

}

BOOL GetTokenByName(HANDLE &hToken, LPCWSTR lpName) {
	if(!lpName){
		return FALSE;
	}
	HANDLE          hProcessSnap = NULL; 
	BOOL            bRet         = FALSE; 
	PROCESSENTRY32W pe32         = {0}; 
 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) return FALSE; 
 
	pe32.dwSize = sizeof(PROCESSENTRY32); 
 
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if(!_wcsicmp(pe32.szExeFile, lpName)) {
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				return (bRet);
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else bRet = FALSE;
 
	CloseHandle (hProcessSnap); 
	return (bRet);
}

void WINAPI ServiceWorker_c::srv_thread_subprocess_launcher(LPVOID o) {
	STARTUPINFOW si; PROCESS_INFORMATION pi; AutoZeroMemory(si); AutoZeroMemory(pi);
	si.cb = sizeof(si); vector<HANDLE> tokens, processes; size_t nWaited = 0;
ExplorerWaiter:
	for (auto i : processes) {
		while (WaitForSingleObject(i, MAXDWORD) == WAIT_TIMEOUT);
		CloseHandle(i); Sleep(10);
	}
	if (++nWaited > 10) {
		nWaited = 0; Sleep(5000);
	}
	if (Process.count(_T("explorer.exe")) < 1) {
		while (!Process.count(_T("explorer.exe"))) Sleep(5000);
	}
	if (Process.count(to__str(GetProgramInfo().name)) - 1 ==
		Process.count(_T("explorer.exe"))) {
		vector<ProcessInfo> pses; Process.find(to__str(GetProgramInfo().name), pses);
		DWORD SelfId = GetCurrentProcessId();
		HANDLE* h_list = (HANDLE*)calloc(pses.size(), sizeof(HANDLE));
		DWORD last_pos = 0;
		for (auto& i : pses) {
			if (i.id() == SelfId) continue;
			if (!IsUserInBuiltinGroup(i, WinBuiltinAdministratorsSid))
				continue; // User is not admin
			h_list[last_pos++] = i;
		}
		WaitForMultipleObjects(last_pos, h_list, FALSE, MAXDWORD);
	}
	if (((ServiceWorker_c*)o)->exit) return;
	tokens.clear(); processes.clear();
	tokens = GetProcessTokenByName(L"explorer.exe");
	for (auto i : tokens) {
		if (!CreateProcessAsUserW(i, 0, (LPWSTR)(L"\""s + s2ws(GetProgramDir())
			+ L"\" --icon --from-service --svc-name=\"" + s2ws(((ServiceWorker_c*)o)
				->ServiceName) + L"\"").c_str(), 0, 0, 0, CREATE_SUSPENDED, 0, 0,
			&si, &pi)) {
			logger.err(L"Cannot CreateProcessAsUserW\n");
			Sleep(5000); goto ExplorerWaiter; // retry
		}
		CloseHandle(i);
		CloseHandle(pi.hThread);
		//((ServiceWorker_c*)o)->hpSubProcess = pi.hProcess;
		Process.resume(pi.hProcess);
		//while (WaitForSingleObject(pi.hProcess, MAXDWORD) == WAIT_TIMEOUT);
		//((ServiceWorker_c*)o)->hpSubProcess = NULL;
		//Process.CloseProcessHandle(pi);
		processes.push_back(pi.hProcess);
	}
	goto ExplorerWaiter;
}

void ServiceWorker_c::PipeDataHandler(HANDLE pipe, string command) {
	static string LastStatus;
	if (command == "GetProcessInfo" || (LastStatus == "GetProcessInfo" &&
		command.length() > 5 && command.substr(0, 5) == "next ")) {
		static vector<ProcessInfo>* ProcessList = nullptr;
		if (ProcessList == nullptr) {
			ProcessList = new vector<ProcessInfo>;
			Process.find(*ProcessList);    // 获取系统所有进程到ProcessList
			//for (auto& i : *ProcessList) {
			//	logger << "[DEBUG] Process " << to_string(i.id()) <<
			//		": " << ws2s(i.name()) << "\n";   // 正常输出
			//}
		}
		logger << "[DEBUG] Pipe command: " << command << "\n";
#if 1
		//for (auto& i : *ProcessList) {
		//	logger << "[DEBUG] Process " << to_string(i.id()) <<
		//		": " << ws2s(i.name()) << "\n";
		//}
		string sInfo;
		size_t nPos = atoll(command.substr(6).c_str());
		try {
			sInfo = ws2s(ProcessList->at(nPos).name()) + '|'
				+ to_string(ProcessList->at(nPos).id());
			logger << "[DEBUG] sInfo=" << sInfo << "\n";
			WriteFile(pipe, sInfo.c_str(), DWORD(sInfo.length() + 1), 0, 0);
		}
		catch (std::out_of_range&) {
			delete ProcessList;
			ProcessList = nullptr;
			WriteFile(pipe, "end|", 5, 0, 0);
		}
#else
		for (auto& i : *ProcessList) {
			string sInfo = ws2s(i.name()) + '|' + to_string(i.id()) + "\n";
			logger << "[DEBUG] sInfo=" << sInfo << "\n";
			WriteFile(pipe, sInfo.c_str(), DWORD(sInfo.length() + 1), 0, 0);
		}
			WriteFile(pipe, "end|", 5, 0, 0);
#endif
		return;
	}
}

DWORD __stdcall ServiceWorker_c::thPipeServer(PVOID obj) {
	WCHAR pipe_name[128] = { 0 };  // Pipe name
	LoadStringW(GetModuleHandle(NULL), IDS_STRING_SVC_PIPE_NAME, pipe_name, 128);
	//SECURITY_ATTRIBUTES sa; AutoZeroMemory(sa);
	//sa.nLength = sizeof sa;
	//sa.bInheritHandle = false;
	//sa.lpSecurityDescriptor = CreateSidForEveryone();
	HANDLE hPipe = NULL;
	hPipe = CreateNamedPipeW(pipe_name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | 
		PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
		1, 1024, 1024, NMPWAIT_USE_DEFAULT_WAIT, NULL/*&sa*/);
	if (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
		logger.err(L"Cannot CreateNamedPipeW: " + to_wstring(GetLastError())
			+ L": " + LastErrorStrW()); return GetLastError();
	}
	//LocalFree(sa.lpSecurityDescriptor);
	auto connector = [](HANDLE pipe) {
		CHAR buffer[1024] = { 0 };
		while (1) {
			global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
			global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_SvcObj->ServiceStatus.dwCheckPoint = 0;
			global_SvcObj->ServiceStatus.dwWaitHint = 0; 
			global_SvcObj->ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP;
			SetServiceStatus(global_SvcObj->hServiceStatusHandle,
				&global_SvcObj->ServiceStatus);

			ConnectNamedPipe(pipe, 0);

			global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
			global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_SvcObj->ServiceStatus.dwCheckPoint = 0;
			global_SvcObj->ServiceStatus.dwWaitHint = 0; 
			global_SvcObj->ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
			SetServiceStatus(global_SvcObj->hServiceStatusHandle,
				&global_SvcObj->ServiceStatus);

			while (ReadFile(pipe, buffer, 1024, 0, 0)) {
				PipeDataHandler(pipe, buffer);
			}

			DisconnectNamedPipe(pipe);
		}
	};
	HANDLE hTh = (HANDLE)_beginthread(connector, 0, hPipe);
	if (!hTh) {
		logger.err(L"Cannot Create Thread: " + to_wstring(GetLastError())
			+ L": " + LastErrorStrW());
		CloseHandle(hPipe); return GetLastError();
	}
	while (!((ServiceWorker_c*)obj)->exit)   // 等待退出
		WaitForSingleObject(((ServiceWorker_c*)obj)->svcmainthread_handle, MAXDWORD);
#pragma warning(push)
#pragma warning(disable: 6258)
	TerminateThread(hTh, 0);
#pragma warning(pop)
	([hPipe] {
		__try {
			if (hPipe) CloseHandle(hPipe);
		} __except (EXCEPTION_EXECUTE_HANDLER) {};
	})();

	return 0;
}

void WINAPI ServiceWorker_c::srv_core_thread(LPVOID obj) {
	EnableDebugPrivilege();
	((ServiceWorker_c*)obj)->hsrv_thread_subprocess_launcher = (HANDLE)
		_beginthread(srv_thread_subprocess_launcher, 0, obj);
	((ServiceWorker_c*)obj)->hPipeServer = CreateThread(0, 0, thPipeServer, obj, 0, 0);
	while (1) Sleep(MAXDWORD);
	return;
}


// ServiceWorker.cpp : Service entry
// 

#include "../../resource/tool.h"
#include "svcmain.h"
#include "resload.h"
#include <WtsApi32.h>
#pragma comment(lib, "wtsapi32.lib")
using namespace std;


ServiceWorker_c* global_SvcObj = NULL;
#if 0
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
#endif


static DWORD SvcShowMessage(
	LPWSTR lpszMessage, LPWSTR lpszTitle, DWORD style, DWORD timeout
) {
	DWORD dwSession = WTSGetActiveConsoleSessionId();
	DWORD dwResponse = 0;
	WTSSendMessageW(WTS_CURRENT_SERVER_HANDLE, dwSession,
		lpszTitle,
		static_cast<DWORD>((wcslen(lpszTitle) + 1) * sizeof(wchar_t)),
		lpszMessage,
		static_cast<DWORD>((wcslen(lpszMessage) + 1) * sizeof(wchar_t)),
		style, timeout, &dwResponse, FALSE);
	return dwResponse;
}

static bool PauseOrStopConfirm(const char* type) {
	do {
		STARTUPINFO si; PROCESS_INFORMATION pi;
		AutoZeroMemory(si); AutoZeroMemory(pi);
		si.cb = sizeof si;
		if (!Process.StartAsActiveUserT(s2wc(GetProgramDir()), (LPTSTR)s2wc
		("\"" + GetProgramDir() + "\" --service-exit-confirm --mode="+type),
			0, 0, 0, 0 | 0, 0, 0, &si, &pi)) break;
		DWORD code = 0;
		if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 15000)) break;
		GetExitCodeProcess(pi.hProcess, &code);
		Process.CloseProcessHandle(pi);
		if (code != -IDYES) break;
		goto ok;
	} while (0);
	return false;
ok:
	return true;
}

void WINAPI ServiceWorker_c::ServiceHandler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN: {
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(global_SvcObj->hServiceStatusHandle,
			&global_SvcObj->ServiceStatus);
		ExitProcess(0);
		break;
	}
	case SERVICE_CONTROL_STOP: {
			//auto& ss = global_SvcObj->ServiceStatus;
			//ss.dwWin32ExitCode = ERROR_ACCESS_DENIED;
			//ss.dwCheckPoint = 0;
			//ss.dwWaitHint = 0;
			//break;
		auto last_stat = global_SvcObj->ServiceStatus.dwCurrentState;
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 16384;
		SetServiceStatus(global_SvcObj->hServiceStatusHandle,
			&global_SvcObj->ServiceStatus);

		if (global_SvcObj->pause_needs_confirm)
		if (!PauseOrStopConfirm("exit")) {
			global_SvcObj->ServiceStatus.dwCheckPoint++;
			//SERVICE_STATUS ss; AutoZeroMemory(ss);
			//memcpy(&ss, &global_SvcObj->ServiceStatus, sizeof(SERVICE_STATUS));
			auto& ss = global_SvcObj->ServiceStatus;
			ss.dwWin32ExitCode = ERROR_ACCESS_DENIED;
			ss.dwCurrentState = last_stat;
			ss.dwCheckPoint = 0;
			ss.dwWaitHint = 0;
			SetServiceStatus(global_SvcObj->hServiceStatusHandle, &ss);
#if defined(_DEBUG) && 0
			SvcShowMessage((LPWSTR)to_wstring(ss.dwWin32ExitCode).c_str(),
				(LPWSTR)L"Debug", MB_ICONINFORMATION, 3000);
#endif
			//memcpy(&global_SvcObj->ServiceStatus, &ss, sizeof(SERVICE_STATUS));
			return;
		}
#if 0
		if (global_SvcObj->pause_needs_confirm) do {
			if (PauseOrStopConfirm("exit")) break;
			global_SvcObj->ServiceStatus.dwWin32ExitCode = ERROR_ACCESS_DENIED;
			global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_SvcObj->ServiceStatus.dwCheckPoint = 0;
			global_SvcObj->ServiceStatus.dwWaitHint = 0;
			//auto _oldAcceptControl = global_SvcObj->ServiceStatus.dwControlsAccepted;
			//global_SvcObj->ServiceStatus.dwControlsAccepted = 0;
			SetServiceStatus(global_SvcObj->hServiceStatusHandle,
				&global_SvcObj->ServiceStatus);
			//global_SvcObj->ServiceStatus.dwControlsAccepted = _oldAcceptControl;
			//SetServiceStatus(global_SvcObj->hServiceStatusHandle,
			//	&global_SvcObj->ServiceStatus);
			return;
		} while (0);
#endif
		//ok:
		global_SvcObj->ServiceStatus.dwCheckPoint++;
		// Stopping clean
		SetLastError(0);
#pragma warning(push)
#pragma warning(disable: 6258)
		::TerminateThread(global_SvcObj->svcmainthread_handle, 0);
		global_SvcObj->exit = true;
		global_SvcObj->ServiceStatus.dwCheckPoint++;
		if (global_SvcObj->cfgfilelk) 
			[]() {	__try { CloseHandle(global_SvcObj->cfgfilelk); }
					__except (EXCEPTION_EXECUTE_HANDLER) {} }();
		global_SvcObj->ServiceStatus.dwCheckPoint++;

#pragma warning(pop)
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;

		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		break;
	}
	case SERVICE_CONTROL_PAUSE: {
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 16384;
		SetServiceStatus(global_SvcObj->hServiceStatusHandle,
			&global_SvcObj->ServiceStatus);
		if (global_SvcObj->pause_needs_confirm) do {
			if (PauseOrStopConfirm("pause")) break;
			global_SvcObj->ServiceStatus.dwWin32ExitCode = ERROR_ACCESS_DENIED;
			global_SvcObj->ServiceStatus.dwWin32ExitCode = 5;
			global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_SvcObj->ServiceStatus.dwCheckPoint = 0;
			global_SvcObj->ServiceStatus.dwWaitHint = 0;
			SetServiceStatus(global_SvcObj->hServiceStatusHandle,
				&global_SvcObj->ServiceStatus);
			return;
		} while (0);
		SuspendThread(global_SvcObj->svcmainthread_handle);
		global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
		global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		global_SvcObj->ServiceStatus.dwCheckPoint = 0;
		global_SvcObj->ServiceStatus.dwWaitHint = 0;
		//if (global_SvcObj->stoppable && global_SvcObj->pause_needs_confirm)
		//	global_SvcObj->ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP;
		break;
	}
	case SERVICE_CONTROL_CONTINUE: {
		//if (global_SvcObj->pause_needs_confirm)
		//	global_SvcObj->ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
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

void WINAPI ServiceWorker_c::ServiceLaunch_main(DWORD, LPWSTR*) {
	global_SvcObj->ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	global_SvcObj->ServiceStatus.dwControlsAccepted = 0;
	global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
	global_SvcObj->ServiceStatus.dwServiceSpecificExitCode = 0;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 16384;
	global_SvcObj->hServiceStatusHandle = RegisterServiceCtrlHandlerW
	(s2wc(global_SvcObj->ServiceName), ServiceHandler);
	if (global_SvcObj->hServiceStatusHandle == 0)
	{
		DWORD nError = GetLastError();
		return;
	}

	//add your init code here
	global_SvcObj->ServiceStatus.dwCheckPoint++;

	SetCurrentDirectoryW(s2wc(GetProgramInfo().path));

	global_SvcObj->ServiceStatus.dwCheckPoint++;

	//add your service thread here

	global_SvcObj->svcmainthread_handle = 
		(HANDLE)::_beginthread(srv_core_thread, 0, global_SvcObj);
	global_SvcObj->ServiceStatus.dwCheckPoint++;

	// Initialization complete - report running status 
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 0;
	global_SvcObj->ServiceStatus.dwControlsAccepted |=
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
//#pragma warning(push)
//#pragma warning(disable: 6387)
	if (!SetServiceStatus(global_SvcObj->hServiceStatusHandle,
		&global_SvcObj->ServiceStatus)) {
//#pragma warning(pop)
		::exit(1);
	}

}

void __stdcall ServiceWorker_c::ReportErrorAndExit(DWORD dwErrCode) {
	global_SvcObj->ServiceStatus.dwWin32ExitCode = dwErrCode;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 0;
	SetServiceStatus(global_SvcObj->hServiceStatusHandle, &global_SvcObj->ServiceStatus);
	ExitProcess(dwErrCode);
}

#if 0
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
#endif

#if 0
void ServiceWorker_c::PipeDataHandler(HANDLE pipe, string command) {

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
	while (!((ServiceWorker_c*)obj)->exit)   // µÈ´ýÍË³ö
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
#endif

void WINAPI ServiceWorker_c::srv_core_thread(LPVOID obj) {
	EnableDebugPrivilege();

	global_SvcObj->parseConfig();

	while (1) Sleep(MAXDWORD);

	global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 0;
	SetServiceStatus(global_SvcObj->hServiceStatusHandle, &global_SvcObj->ServiceStatus);

	return;
}

void __stdcall ServiceWorker_c::parseConfig() {
	using namespace tinyxml2;
	auto assp = [](void* val) { if (!val) ReportErrorAndExit(ERROR_INVALID_DATA); };
	auto assi = [](INT_PTR val) { if (!val) ReportErrorAndExit(ERROR_INVALID_DATA); };
	auto tistrequ = [](LPCSTR str1, LPCSTR str2)->bool {
		if (str1 == NULL || str2 == NULL) return false;
		return 0 == _stricmp(str1, str2);
	};

	//DWORD i = 30*1000;  Sleep(i);
#ifdef _DEBUG
	//while (1) { if (IsDebuggerPresent()) { Sleep(15000); break; } else Sleep(1000); }
#endif

	if (!file_exists(cfg_path)) ReportErrorAndExit(ERROR_FILE_NOT_FOUND);
	tinyxml2::XMLDocument xld;
	assi(!xld.LoadFile(cfg_path.c_str()));

	global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 32767;
	SetServiceStatus(global_SvcObj->hServiceStatusHandle, &global_SvcObj->ServiceStatus);

	XMLElement* root = xld.RootElement(); assp(root);
	XMLElement* app_config = root->FirstChildElement("app_config"); assp(app_config);

	XMLElement* metas = app_config->FirstChildElement("meta"); 
	if (metas) do {
		string name, value;
		{
			const XMLAttribute* _name_, *_value_;
			_name_ = metas->FindAttribute("name");
			_value_ = metas->FindAttribute("value");
			if (_name_) name = _name_->Value();
			if (_value_) value = _value_->Value();
		}

		if (name == "no-parse-this-config"s && value == "true"s) {
			ReportErrorAndExit(ERROR_ACCESS_DENIED);
		}

	} while (metas = metas->NextSiblingElement());
	global_SvcObj->ServiceStatus.dwCheckPoint++;

	XMLElement* svccfg0 = app_config->FirstChildElement("service"); assp(svccfg0);
	{
		XMLElement* svccfg = svccfg0->FirstChildElement("config"); assp(svccfg);
		XMLElement* svcsec = svccfg0->FirstChildElement("security"); assp(svcsec);

		XMLElement* cfgitem = svccfg->FirstChildElement("cfg");
		global_SvcObj->stoppable = true;
		if (cfgitem) do {
			if (tistrequ(cfgitem->Attribute("startable"), "false"))
				ReportErrorAndExit(1052);
			if (tistrequ(cfgitem->Attribute("stoppable"), "false")) {
				global_SvcObj->ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
				global_SvcObj->stoppable = false;
			}
			if (tistrequ(cfgitem->Attribute("pausable"), "false")) global_SvcObj->
				ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_PAUSE_CONTINUE;
			// TODO: logfile
		} while (cfgitem = cfgitem->NextSiblingElement());
		global_SvcObj->ServiceStatus.dwCheckPoint++;

		XMLElement* secitem = svcsec->FirstChildElement("cfg");
		if (secitem) do {
			if (tistrequ(secitem->Attribute("enabled"), "false")) continue;
			if (tistrequ(secitem->Attribute("name"), "pause_needs_confirm") &&
				tistrequ(secitem->Attribute("value"), "true")) {
				global_SvcObj->pause_needs_confirm = true;
				//global_SvcObj->ServiceStatus.dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
			}
			if (tistrequ(secitem->Attribute("name"), "lock_config_file") &&
				tistrequ(secitem->Attribute("value"), "true")) {
				cfgfilelk = CreateFileW(s2wc(cfg_path), FILE_ALL_ACCESS, 0, 0,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			}
			if (tistrequ(secitem->Attribute("name"), "self_protection") &&
				tistrequ(secitem->Attribute("value"), "true")) {

			}

		} while (secitem = secitem->NextSiblingElement());
		global_SvcObj->ServiceStatus.dwCheckPoint++;
	}
	global_SvcObj->ServiceStatus.dwCheckPoint++;

	XMLElement* ruleroot = app_config->FirstChildElement("rules"); assp(ruleroot);
	XMLElement* rulechild = 0;
	do {
		if (tistrequ(ruleroot->Attribute("name"), "root")) break;
	} while (ruleroot = ruleroot->NextSiblingElement());
	assp(ruleroot); rulechild = ruleroot->FirstChildElement();
	if (rulechild) do {
		_findrules(rulechild);
	} while (rulechild = rulechild->NextSiblingElement());
	global_SvcObj->ServiceStatus.dwCheckPoint++;


	global_SvcObj->ServiceStatus.dwWin32ExitCode = 0;
	global_SvcObj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	global_SvcObj->ServiceStatus.dwCheckPoint = 0;
	global_SvcObj->ServiceStatus.dwWaitHint = 0;
	SetServiceStatus(global_SvcObj->hServiceStatusHandle, &global_SvcObj->ServiceStatus);
}

void ServiceWorker_c::_findrules(tinyxml2::XMLElement* el) {
	if (!el) return ;
	using namespace tinyxml2;
	auto assp = [](void* val) { if (!val) ReportErrorAndExit(ERROR_INVALID_DATA); };
	auto assi = [](INT_PTR val) { if (!val) ReportErrorAndExit(ERROR_INVALID_DATA); };
	auto tistrequ = [](LPCSTR str1, LPCSTR str2)->bool {
		if (str1 == NULL || str2 == NULL) return false;
		return 0 == _stricmp(str1, str2);
	};
	if (tistrequ(el->Name(), "rules")) {
		el = el->FirstChildElement();
		do { _findrules(el); }
		while (el = el->NextSiblingElement());
		return;
	}
	if (!tistrequ(el->Name(), "rule")) return /*87*/;

}


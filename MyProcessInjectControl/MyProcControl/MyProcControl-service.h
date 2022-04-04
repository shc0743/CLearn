#pragma once
#include <Windows.h>
#include <AclAPI.h>
#include <string>
using std::string;

class ServiceWorker_t {
public:
	ServiceWorker_t() { ZeroMemory(this, sizeof(*this)); };
	~ServiceWorker_t() = default;
	WCHAR ServiceName[256];

	static void WINAPI ServiceLaunch(DWORD, LPWSTR*);
	static bool processProtect(HANDLE hObject);

protected:
	static void WINAPI ReportErrorAndExit(DWORD dwErrCode);
	static void WINAPI ServiceHandler(DWORD fdwControl);
	static BOOL UpdateServiceStatus();
	static DWORD __stdcall srv_core_thread(PVOID);
	static DWORD WINAPI SubProcessThread(PVOID);
	static DWORD WINAPI SubProcessZxThread(PVOID);
	//static bool WINAPI SubProcessDataHandler(PSTR, HANDLE, HANDLE);
	static DWORD WINAPI SvcCtlPipeThread(PVOID);
	static DWORD WINAPI StoppingThrd(PVOID);
	static void End_UI_Process();
	/**
	@param type 1 stop ;  2 pause
	*/
	static BOOL WINAPI UserServiceControlConfirm(DWORD type);
	SERVICE_STATUS SvcStat;
	SERVICE_STATUS_HANDLE hSvcStatus;
	HANDLE hSvcThread;
	HANDLE
		hSubProcessThread,
		hSubProcessZxThread,
		hCtlPipeThread;
	DWORD last_stat;
	HANDLE
		hSubProcess,
		hSubProcessZx,
		hCtlNamedPipe;
	//typedef WCHAR FilePath_t[MAX_PATH + 1];
	std::wstring
		dll_exp_path_64,
		dll_exp_path_86;
};

extern ServiceWorker_t* global_SvcObj;

DWORD WINAPI ServiceWorker_sub_process(PVOID sc_name_UNICODE);
DWORD WINAPI MyUiWorker_parentWatchdog(PVOID);
DWORD WINAPI ServiceWorker_subpentry(PVOID);


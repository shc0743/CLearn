#pragma once
#include "includer.h"
using namespace std;

class ServiceWorker_c {
public:
	ServiceWorker_c() { ZeroMemory(this, sizeof(*this)); };
	~ServiceWorker_c() = default;
	SERVICE_STATUS ServiceStatus;
	SERVICE_STATUS_HANDLE hServiceStatusHandle;
	HANDLE svcmainthread_handle;
	CHAR   ServiceName[256];
	HANDLE hsrv_thread_subprocess_launcher;
	HANDLE hPipeServer;
	HANDLE hpSubProcess;
	bool exit;

	static void WINAPI ServiceLaunch_main(DWORD, LPSTR*);
	static void WINAPI ServiceHandler(DWORD fdwControl);
	static void WINAPI srv_core_thread(LPVOID);
	static void WINAPI srv_thread_subprocess_launcher(LPVOID);
	static DWORD WINAPI thPipeServer(PVOID);

protected:
	//static LPVOID CreateSidForEveryone();
	static void PipeDataHandler(HANDLE pipe, string command);
};

extern ServiceWorker_c* global_SvcObj;


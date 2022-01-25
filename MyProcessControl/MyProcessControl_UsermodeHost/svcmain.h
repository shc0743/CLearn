#pragma once
#include "../../resource/tool.h"
#include "../tinyxml2/tinyxml2.h"
using namespace std;

// ÃÌº”MessageBoxTimeout÷ß≥÷
extern "C" {
	int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption,
		IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
	int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption,
		IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
};

typedef struct _mpc_rule_t {

} mpc_rule_t;

class ServiceWorker_c {
public:
	ServiceWorker_c() { ZeroMemory(this, sizeof(*this)); };
	~ServiceWorker_c() = default;
	SERVICE_STATUS ServiceStatus;
	SERVICE_STATUS_HANDLE hServiceStatusHandle;
	HANDLE svcmainthread_handle;
	CHAR   ServiceName[256];
	string cfg_path;
	HANDLE cfgfilelk;
	vector<mpc_rule_t> rules;
	//HANDLE hPipeServer;
	//HANDLE hpSubProcess;
	bool pause_needs_confirm, stoppable;
	bool exit;

	static void WINAPI ServiceLaunch_main(DWORD, LPWSTR*);
	static void WINAPI ReportErrorAndExit(DWORD dwErrCode);
	static void WINAPI ServiceHandler(DWORD fdwControl);
	static void WINAPI srv_core_thread(LPVOID);
	void WINAPI parseConfig();
	//static DWORD WINAPI thPipeServer(PVOID);

protected:
	//static void PipeDataHandler(HANDLE pipe, string command);
	void _findrules(tinyxml2::XMLElement* el);
};

extern ServiceWorker_c* global_SvcObj;


#include <iostream>
#include "../../resource/tool.h"
#include "resource.h"

SERVICE_STATUS global_ServiceStatus;
SERVICE_STATUS_HANDLE global_hServiceStatusHandle;
string global_service_name;
string global_conf_file_path;
HANDLE global_svcmainthread_handle;
HANDLE global_handlechk_handle;
HANDLE hConfigFile = NULL;
#pragma warning(push)
#pragma warning(disable: 26495)
struct {
	bool disabled, startable, stoppable, pausable, LockConfigAtRunning, interactive;
	enum { Undefined = 0, Service = 1, Process = 2 } startupType;
	string serviceName, startupPassword;
	DWORD dwCheckDelayTime;
} global_config;
#pragma warning(pop)
void WINAPI service_main(DWORD argc, LPSTR* argv);
void WINAPI ServiceHandler(DWORD fdwControl);
DWORD WINAPI srv_core_thread(LPVOID);
void ParseConfig();


int main(int argc, char* argv[]) {
	CmdLineA cl(::GetCommandLineA());
	string sbuffer_stdstring;

	if (cl.getopt("help", sbuffer_stdstring)) {
		if (sbuffer_stdstring == "svc") {
			printf("Service Commands\n================\n%s",
				"install <ServiceName> [DisplayName] Install manual service as <ServiceName>.\n"
				"uninstall <ServiceName>      Uninstall service <ServiceName>.\n"
				"start <ServiceName>          Start service <ServiceName>.\n"
				"stop <ServiceName>           Stop service <ServiceName>.\n"
				"restart <ServiceName>        Restart service <ServiceName>.\n"
				"pause <ServiceName>          Pause service <ServiceName>.\n"
				"continue <ServiceName>       Continue service <ServiceName>.\n"
			); return 0;
		}
		if (sbuffer_stdstring == "startup") {
			printf("Startup Help\n================\nParamters:\n%s",
				"--startup=service     Start as a local service.\n"
				"--disabled            Disable service to startup\n"
				"--startable[=false]   Block service to startup\n"
				"--stoppable[=false]   Block service to stop\n"
				"--pausable[=false]    Block service to pause\n"
				"--interactive[=false] Block service to interactive\n"
			); return 0;
		}
		printf("%s Help Document\n================\nUsage:\n\t%s %s\n\nParamters:\n%s", argv[0], argv[0],
			"[--help] [--startup] [--svc <ChildCommands>]",
			"--help                   Show this help text.\n"
			"--startup                Set process startup type. Use --help=startup to learn more.\n"
			"--svc <ChildCommands>    Service commands  Use --help=svc to learn more\n"
		); return 0;
	}

	if (cl.getopt("startup", sbuffer_stdstring) == 1 && sbuffer_stdstring == "service") {
		puts("[LOG] Starting service...");
		if (1 != cl.getopt("svc-name", global_service_name)) {
			fprintf(stderr, "[ERR] Service name not specified! Exiting...\n");
			//MyWriteApplicationEvent("Service name not specified! Exiting...", 1, EVENTLOG_ERROR_TYPE);
			exit(1);
		}
		global_config.disabled = false;
		global_config.startable = true;
		global_config.stoppable = true;
		global_config.pausable = true;
		global_config.interactive = true;
		if (cl.getopt("disabled")) global_config.disabled = true;
		if (cl.getopt("startable", sbuffer_stdstring) == 1 && sbuffer_stdstring == "false") 
			global_config.startable = false;
		if (cl.getopt("stoppable", sbuffer_stdstring) == 1 && sbuffer_stdstring == "false") 
			global_config.stoppable = false;
		if (cl.getopt("pausable", sbuffer_stdstring) == 1 && sbuffer_stdstring == "false") 
			global_config.pausable = false;
		if (cl.getopt("interactive", sbuffer_stdstring) == 1 && sbuffer_stdstring == "false") 
			global_config.interactive = false;

		return !MyStartAsServiceA(global_service_name, service_main);
	}

	if (cl.getopt("svc")) {
		if (cl.argc() >= 3) {
			string svcname = cl[3];

			if ((cl[2] == "install" || cl[2] == "install-command") && cl.argc() >= 4) {
				string dname = svcname; if (cl.argc() > 4) dname = cl[4];
				unsigned err = ServiceManager.New(svcname, '\"' + ::GetProgramDir() + "\" --startup=se"
					"rvice --svc-name=\"" + svcname + "\"", ServiceManager_::Manual, dname, "", 
					SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS);
				if (err) {
					fprintf(stderr, "[ERR] Cannot Create Service: %ld: %s\n", GetLastError(), 
						LastErrorStrA()); return ::GetLastError();
				}
				return 0;
			}

			if (cl[2] == "uninstall") {
				if (ServiceManager.Remove(svcname)) { fprintf(stderr,
					"[ERR] Cannot Delete Service: %ld: %s\n", GetLastError(), LastErrorStrA()); return 1;
				} else return 0;
			}

			if (cl[2] == "start") {
				if (ServiceManager.Start(svcname)) { fprintf(stderr, "[ERR] Cannot Start "
					"Service: %ld: %s\n", GetLastError(), LastErrorStrA());
				}
				return 0;
			}

			if (cl[2] == "stop") {
				if (ServiceManager.Stop(svcname)) { fprintf(stderr, "[ERR] Cannot Stop"
					" Service: %ld: %s\n", GetLastError(), LastErrorStrA());
				}
				return 0;
			}

			if (cl[2] == "restart") {
				if (ServiceManager.ReStart(svcname)) { fprintf(stderr, 
					"[ERR] Cannot ReStart Service: %ld: %s\n", GetLastError(), LastErrorStrA());
				}
				return 0;
			}

			if (cl[2] == "pause") {
				if (ServiceManager.Stop(svcname)) { fprintf(stderr, "[ERR] Cannot Pause Service:"
					" %ld: %s\n", GetLastError(), LastErrorStrA());
				}
				return 0;
			}

			if (cl[2] == "continue") {
				if (ServiceManager.Stop(svcname)) { fprintf(stderr, "[ERR] Cannot Continue "
					"Service: %ld: %s\n", GetLastError(), LastErrorStrA());
				}
				return 0;
			}

		}
		fprintf(stderr, "[ERR] Invalid arguments!\nUse %s --help=svc to see usage.\n", argv[0]);
		return 1;
	}

	fprintf(stderr, "[ERR] Invalid arguments!\nUse %s --help to see usage.\n", argv[0]);
	return 1;
	return 0;
}


void WINAPI ServiceHandler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		global_ServiceStatus.dwWin32ExitCode = 0;
		global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;

		// Stopping clean
		SetLastError(0);
		::TerminateThread(global_svcmainthread_handle, 0);
		::TerminateThread(global_handlechk_handle, 0);
		::CloseHandle(global_svcmainthread_handle);::CloseHandle(global_handlechk_handle);
		::CloseHandle(hConfigFile);
		global_ServiceStatus.dwWin32ExitCode = ::GetLastError();

		break;
	}
	case SERVICE_CONTROL_PAUSE:
		::SuspendThread(global_svcmainthread_handle);
		::SuspendThread(global_handlechk_handle);
		global_ServiceStatus.dwWin32ExitCode = 0;
		global_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_CONTINUE:
	{
		bool ok = true;
		ok = ok && ::ResumeThread(global_svcmainthread_handle);
		ok = ok && ::ResumeThread(global_handlechk_handle);
		if (ok) {
			global_ServiceStatus.dwWin32ExitCode = 0;
			global_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_ServiceStatus.dwCheckPoint = 0;
			global_ServiceStatus.dwWaitHint = 0;
		} else {
			global_ServiceStatus.dwWin32ExitCode = ::GetLastError();
			global_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			global_ServiceStatus.dwCheckPoint = 0;
			global_ServiceStatus.dwWaitHint = 0;
		}
		break;
	}
	default:
		return;
	};
	SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
}

void WINAPI service_main(DWORD argc, LPSTR* argv)
{
	global_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	global_ServiceStatus.dwControlsAccepted = 0;
	global_ServiceStatus.dwWin32ExitCode = 0;
	global_ServiceStatus.dwServiceSpecificExitCode = 0;
	global_ServiceStatus.dwCheckPoint = 0;
	global_ServiceStatus.dwWaitHint = 0;
	global_hServiceStatusHandle = 
		RegisterServiceCtrlHandlerA(global_service_name.c_str(), ServiceHandler);
	if (global_hServiceStatusHandle == 0)
	{
		DWORD nError = GetLastError();
	}

	//add your init code here

	hConfigFile = NULL;
	global_conf_file_path = GetProgramPath() + "\\fv_conf_" + global_service_name + ".ini";
	if (MyGetFileSizeA(global_conf_file_path) == 0) {
		if (!FreeResFile(IDR_BIN_CONF_TEMPLATE, "BIN", global_conf_file_path)) {
			fprintf(stderr, "[ERR] Cannot Free Resource File\n");
			MyWriteApplicationEvent("[ERR] Cannot Free Resource File\n", 1, EVENTLOG_ERROR_TYPE);
		}
	}
	hConfigFile = CreateFileSimpleW(s2ws(global_conf_file_path).c_str(), 
		GENERIC_READ, OPEN_ALWAYS, FILE_SHARE_READ);
	DWORD lasterr_buffer = ::GetLastError();
	if (!hConfigFile || !ReadFile(hConfigFile, 0, 0, 0, 0)) {
		fprintf(stderr, "[ERR] Cannot open config file! Exiting...\n");
		MyWriteApplicationEvent("[ERR] Cannot open config file!\n", 1, EVENTLOG_ERROR_TYPE);
		global_ServiceStatus.dwWin32ExitCode = lasterr_buffer;
		global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
		exit(GetLastError());
	}
	if (global_config.disabled || !global_config.startable) {
		if (global_config.disabled) fprintf(stderr, "[ERR] Disabled");
		else if(!global_config.startable)fprintf(stderr,"[ERR] Cannot startup because it's disabled");
		global_ServiceStatus.dwWin32ExitCode = 5;
		global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
		exit(5);
	}
	if (global_config.stoppable)
		global_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	if (global_config.pausable)
		global_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	if (global_config.interactive)
		global_ServiceStatus.dwServiceType |= SERVICE_INTERACTIVE_PROCESS;

	//add your service thread here
	global_svcmainthread_handle = CreateThread(NULL, NULL, srv_core_thread, NULL, NULL, NULL);
	if (global_svcmainthread_handle == NULL)
	{
		fprintf(stderr, "[ERR] Create srv_core_thread failed");
		//MyWriteApplicationEvent("Create srv_core_thread failed", 1, EVENTLOG_ERROR_TYPE);
	}

	// Initialization complete - report running status 
	global_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	global_ServiceStatus.dwCheckPoint = 0;
	global_ServiceStatus.dwWaitHint = 8192;
#pragma warning(push)
#pragma warning(disable: 6387)
	if (!SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus)) {
#pragma warning(pop)
		//MyWriteApplicationEvent("Cannot SetServiceStatus! Exiting...", 1, EVENTLOG_ERROR_TYPE);
		exit(1);
	}
}

void ParseConfig() {
	AutoZeroMemory(global_config);
	// Load default config
	global_config.disabled = true;
	global_config.startable = false;
	global_config.stoppable = true;
	global_config.pausable = true;
	global_config.interactive = false;
	global_config.LockConfigAtRunning = true;
	global_config.startupType = global_config.Service;
	global_config.dwCheckDelayTime = 600000; // 10 minutes
	// Load from file
	size_t buffer_max_size = 131072;
	char* buffer = (char*)GetBufMemory(buffer_max_size);
	FILE* file; fopen_s(&file, global_conf_file_path.c_str(), "r");
	if (file != 0) {
		string now_tag;
		while (fgets(buffer, (int)buffer_max_size, file) && !feof(file)) {
			if (buffer[0] == 0) continue; // null line                                        //comment
			if (buffer[0] == '#' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/')) continue;

			if (!strcmp(buffer, "[global]")) { // this_line == "[global]"
				now_tag = "global"; continue;
			}
			if (!strcmp(buffer, "[rulesglobal]")) { // this_line == "[rulesglobal]"
				now_tag = "rulesglobal"; continue;
			}
			if (!strcmp(buffer, "[rule]")) { // this_line == "[rule]"
				now_tag = "rule"; continue;
			}
			if (buffer[0]=='[') { // others
				now_tag = ""; continue; // reset tag
			}

			size_t strle = strlen(buffer);
			if (now_tag == "global") { // Global config
				
			}
		}
		fclose(file);
	}
	free(buffer);
	// return
}

DWORD __stdcall HandleCheckerThread(LPVOID) {
	char buffer[1] = { 0 };
	while (1) {
		if (!hConfigFile || !ReadFile(hConfigFile, 0, 0, 0, 0)
			//|| !fread_s(buffer, 0, 0, 0, hLogFile)
			) {
			fprintf(stderr, "[ERR] Invalid handle!\n");
			MyWriteApplicationEvent("[ERR] Invalid handle!\n", 1, EVENTLOG_ERROR_TYPE);
			global_ServiceStatus.dwWin32ExitCode = 6;
			global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			global_ServiceStatus.dwCheckPoint = 0;
			global_ServiceStatus.dwWaitHint = 0;
			SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
			exit(6);
		}
		Sleep(rand() % 32768); srand((UINT)_time64(0));
	}
	return 0;
}

DWORD __stdcall srv_core_thread(LPVOID) {
	::SetCurrentDirectoryA(GetProgramPath().c_str());
	global_handlechk_handle = (HANDLE)_beginthread((_beginthread_proc_type)HandleCheckerThread, 0, 0);
	if (!global_handlechk_handle) {
		fprintf(stderr, "[ERR] Create HandleCheckerThread failed");
		MyWriteApplicationEvent("[ERR] Create HandleCheckerThread failed", 1, EVENTLOG_ERROR_TYPE);
		global_ServiceStatus.dwWin32ExitCode = ::GetLastError();
		global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
		exit(1);
	}
	ParseConfig();
	while (true) {
		Sleep(5000);
	}
	return 0;
}


#include "../../resource/tool.h"
using namespace std;
using ::fprintf;

SERVICE_STATUS global_ServiceStatus;
SERVICE_STATUS_HANDLE global_hServiceStatusHandle;
HANDLE global_svcmainthread_handle;
HANDLE global_subprocess;
void WINAPI ServiceLaunch_main(DWORD dwNumServicesArgs, LPSTR*);
#pragma warning(push)
#pragma warning(disable: 26495)
struct {
	bool disabled, startable, stoppable, pausable, LockConfigAtRunning, interactive;
	enum { Undefined = 0, Service = 1, Process = 2 } startupType;
	string serviceName, bin_path, args;
	DWORD dwCheckDelayTime;
} global_config;
#pragma warning(pop)
void WINAPI ServiceHandler(DWORD fdwControl);
void WINAPI srv_core_thread(LPVOID);

int main(int argc, char* argv[]) {
    CmdLineA cl(::GetCommandLineA());
    if (cl.getopt("_service")) {
        string sbuffer_stdstr;

        cl.getopt("name", global_config.serviceName);
        cl.getopt("bin", global_config.bin_path);
        cl.getopt("args", global_config.args);
		global_config.disabled = false;
		global_config.startable = true;
		global_config.stoppable = true;
		global_config.pausable = true;
		global_config.interactive = true;
		if (cl.getopt("disabled")) global_config.disabled = true;
		if (cl.getopt("startable", sbuffer_stdstr) == 1 && sbuffer_stdstr == "false") 
			global_config.startable = false;
		if (cl.getopt("stoppable", sbuffer_stdstr) == 1 && sbuffer_stdstr == "false") 
			global_config.stoppable = false;
		if (cl.getopt("pausable", sbuffer_stdstr) == 1 && sbuffer_stdstr == "false") 
			global_config.pausable = false;
		if (cl.getopt("interactive", sbuffer_stdstr) == 1 && sbuffer_stdstr == "false") 
			global_config.interactive = false;

		return !MyStartAsServiceA(global_config.serviceName, ServiceLaunch_main);
    }

	// Service Controller
	if (cl.getopt("svc")) {
		if (cl.argc() >= 3) {
			string svcname = cl[3];

			if (cl[2] == "new" && cl.argc() >= 4) {
				string svcbin, svcarg, dname;
				/*
				Command Line:
				<exe> --svc new Name Bin.exe [Argument[ DisplayName]]
				0     1     2   3    4        5         6
				*/
				svcbin = cl[4];
				if (cl.argc() >= 5) svcarg = cl[5];
				dname = svcname; if (cl.argc() >= 6) dname = cl[6];
				string svc_cl = '\"' + ::GetProgramDir() + "\" --_service --name=\"" +
					svcname + "\" --bin=\"" + svcbin + "\"";
				if (!svcarg.empty()) svc_cl += " --args=\"" + svcarg + "\"";
				unsigned err = ServiceManager.New(svcname, svc_cl, 
					ServiceManager_::Manual, dname, "", 
					SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS);
				if (err) {
					fprintf(stderr, "[ERR] Cannot Create Service: %ld: %s\n",
						GetLastError(), LastErrorStrA()); return ::GetLastError();
				}
				return 0;
			}

			if (cl[2] == "delete") {
				if (ServiceManager.Remove(svcname)) {
					fprintf(stderr, "[ERR] Cannot Delete Service: %ld: %s\n",
						GetLastError(), LastErrorStrA()); return 1;
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
				if (ServiceManager.ReStart(svcname)) {
					fprintf(stderr, "[ERR] Cannot ReStart Service: %ld: %s\n",
						GetLastError(), LastErrorStrA());
				}
				return 0;
			}

			if (cl[2] == "pause") {
				if (ServiceManager.Stop(svcname)) { fprintf(stderr, "[ERR] Cannot Pause"
					" Service: %ld: %s\n", GetLastError(), LastErrorStrA());
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
		fprintf(stderr, "[ERR] Invalid arguments!\n"
			"Use %s --help=svc to see usage.\n", argv[0]);
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
		if (::TerminateThread(global_svcmainthread_handle, 0))
			::CloseHandle(global_svcmainthread_handle);
		if (::TerminateProcess(global_subprocess, 0))
			::CloseHandle(global_subprocess);
		global_ServiceStatus.dwWin32ExitCode = ::GetLastError();

		break;
	}
	case SERVICE_CONTROL_PAUSE:
		::SuspendThread(global_svcmainthread_handle);
		Process.suspend(global_subprocess);
		global_ServiceStatus.dwWin32ExitCode = 0;
		global_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_CONTINUE:
	{
		::ResumeThread(global_svcmainthread_handle);
		Process.resume(global_subprocess);
			global_ServiceStatus.dwWin32ExitCode = 0;
			global_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			global_ServiceStatus.dwCheckPoint = 0;
			global_ServiceStatus.dwWaitHint = 0;
		break;
	}
	default:
		return;
	};
	SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus);
}

void WINAPI ServiceLaunch_main(DWORD   dwNumServicesArgs, LPSTR*) {
	global_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	global_ServiceStatus.dwControlsAccepted = 0;
	global_ServiceStatus.dwWin32ExitCode = 0;
	global_ServiceStatus.dwServiceSpecificExitCode = 0;
	global_ServiceStatus.dwCheckPoint = 0;
	global_ServiceStatus.dwWaitHint = 0;
	global_hServiceStatusHandle = 
		RegisterServiceCtrlHandlerA(global_config.serviceName.c_str(), ServiceHandler);
	if (global_hServiceStatusHandle == 0)
	{
		DWORD nError = GetLastError();
	}

	//add your init code here

	if (global_config.stoppable)
		global_ServiceStatus.dwControlsAccepted |=
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	if (global_config.pausable)
		global_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	if (global_config.interactive)
		global_ServiceStatus.dwServiceType |= SERVICE_INTERACTIVE_PROCESS;

	//add your service thread here
	STARTUPINFOW si{ 0 };
	PROCESS_INFORMATION pi{ 0 };
	si.cb = sizeof si;
	if (!CreateProcessW(s2wc(global_config.bin_path), (LPWSTR)s2wc
		(global_config.bin_path + ' ' + global_config.args), 0, 0, 0, 4, 0, 0, &si, &pi)) {
		fprintf(stderr, "[ERR] Create process failed");
		global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		global_ServiceStatus.dwCheckPoint = 0;
		global_ServiceStatus.dwWaitHint = 0;
		global_ServiceStatus.dwWin32ExitCode = ::GetLastError();
#pragma warning(push)
#pragma warning(disable: 6387)
		if (!SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus)) {
#pragma warning(pop)
			exit(1);
		}
		return;
	}
	global_subprocess = pi.hProcess;
	::CloseHandle(pi.hThread);
	Process.resume(pi.hProcess);

	global_svcmainthread_handle = (HANDLE)::_beginthread(srv_core_thread, 0, 0);

	// Initialization complete - report running status 
	global_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	global_ServiceStatus.dwCheckPoint = 0;
	global_ServiceStatus.dwWaitHint = 8192;
#pragma warning(push)
#pragma warning(disable: 6387)
	if (!SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus)) {
#pragma warning(pop)
		exit(1);
	}

}

void WINAPI srv_core_thread(LPVOID) {
	while (WAIT_TIMEOUT == ::WaitForSingleObject(global_subprocess, MAXDWORD));
	global_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	global_ServiceStatus.dwCheckPoint = 0;
	global_ServiceStatus.dwWaitHint = 0;
	global_ServiceStatus.dwWin32ExitCode = 0;
#pragma warning(push)
#pragma warning(disable: 6387)
	if (!SetServiceStatus(global_hServiceStatusHandle, &global_ServiceStatus)) {
#pragma warning(pop)
		exit(1);
	}
	return;
}

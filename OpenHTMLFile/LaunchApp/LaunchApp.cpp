#include <iostream>
#include "../../resource/tool.h"

int main() {
	CmdLineW cl(GetCommandLineW());
	if (cl.size()) cl.__get_data__().erase(cl.begin());
	std::wstring cmd_line;
	for (auto& i : cl) {
		cmd_line += L"\"" + i + L"\" ";
	}
	PROCESS_INFORMATION pi; {
		STARTUPINFOW si;
		AutoZeroMemory(si); AutoZeroMemory(pi);
		si.cb = sizeof si;
		si.dwFlags = 0;
		WCHAR pcmd_line[2048]{ 0 };
		wcscpy_s(pcmd_line, cmd_line.c_str());
		BOOL bResult = CreateProcessW(NULL, pcmd_line,
			NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
		if (!bResult) {
			DWORD nCode = GetLastError();
			return int(nCode);
		}
	}
	CloseHandle(pi.hThread);
	DWORD dwExitCode = 0;
	Process.resume(pi.hProcess);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	CloseHandle(pi.hProcess);
	return dwExitCode;
}


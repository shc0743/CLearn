#include <iostream>
#include "../../resource/tool.h"
using namespace std;

DWORD WINAPI thrd_waitchild(void* h);
DWORD WINAPI thrd_waitchild(void* h) {
	while (WAIT_TIMEOUT == ::WaitForSingleObject(h, MAXDWORD));
	string strbuf = GetCommandLineA(); str_replace(strbuf, "\"", "\"\"");
	auto pi = Process.Start_HiddenWindow(s2ws("\"" + GetProgramDir() + "\" " +
		to_string(GetCurrentProcessId()) + " \"" + strbuf + "\" -child"));
	::CloseHandle(pi.hThread);
	::CreateThread(NULL, 0, thrd_waitchild, pi.hProcess, 0, NULL);
	return 0;
}

int main(int argc, char* argv[]) {
	if (0 > argc || argc < 3) {
		cout << "Usage: " << argv[0] << " <ProcessID> <DiedStartPath>" << endl; return 0;
	}
	ProcessInfo prot = Process.find(atol(argv[1]));
	if (!prot) return 1;
	if (argc >= 4 && 0 == strcmp(argv[3], "-child")) {
		ProcessInfo prot = Process.find(Process.GetParentProcessId(GetCurrentProcessId()));
		if (!prot) return 1;
		while (WAIT_TIMEOUT == ::WaitForSingleObject(prot, MAXDWORD));
		Process.StartOnly_HiddenWindow(s2ws(argv[2]));
		return 0;
	} else {
		thrd_waitchild(NULL);
	}
	while (WAIT_TIMEOUT == ::WaitForSingleObject(prot, MAXDWORD));
	auto pi = Process.Start(s2ws(argv[2]));
	Process.CloseProcessHandle(pi);
	Process.StartOnly_HiddenWindow(s2ws("\"" + GetProgramDir() + "\" " + 
		to_string(pi.dwProcessId) + " \"" + argv[2] + "\""));
	return 0;
}

#include <iostream>
#include "../../resource/includer.h"
#include "../../resource/tool.h"


decltype(GetLastError()) FillProcessMemoryById(DWORD pid, BYTE hex/*, bool = false*/);
void SigHandler_SIGINT(int);
int main(int argc, const char** argv);

int main(int argc, const char** argv) {

	if (argc >= 2 && (0 == _stricmp("--help", argv[1]) ||
		0 == _stricmp("/?", argv[1]))) {  // show help
		printf("%s Help Document\n========\nUsage:\n\t%s %s\n\nParamters:\n%s",
			argv[0], argv[0],
			"[--help] [((-p <PID>)|(-n <ProcessName>))[ <Memval>]] [-k (PID))]",
			"--help           Print this help document.\n"
			"-p <PID>         Fill process <PID>'s memory with <Memval>||0xcc.\n"
			"-n <ProcessName> Fill process <ProcessName>'s memory with <Memval>||0xcc.\n"
			"-k <PID>         Call TerminateProcess to kill <PID>.\n"
		); return 0;
	}

	if (argc >= 3) {
		if (0 == _stricmp("-k", argv[1])) {  // kill
			HANDLE hPrc = OpenProcess(PROCESS_TERMINATE, FALSE, atol(argv[2]));
			if (!hPrc) {
				fprintf(stderr, "%s\n", LastErrorStrA());
				return GetLastError();
			}
			if (!TerminateProcess(hPrc, PROCESS_TERMINATE)) {
				fprintf(stderr, "%s\n", LastErrorStrA());
				return GetLastError();
			}
			printf("%s\n", ErrorCodeToStringA(ERROR_SUCCESS));
			return 0;
		}
		if (0 == _stricmp("-p", argv[1])) {  // pid
			signal(SIGINT, SigHandler_SIGINT);
			BYTE hex = 0xcc;
			if (argc >= 4) hex = (BYTE)atoi(argv[3]);
			auto _r = FillProcessMemoryById(atol(argv[2]), hex);
			if (_r == ERROR_SUCCESS) {
				printf("%s\n", ErrorCodeToStringA(ERROR_SUCCESS));
				return 0;
			}
			fprintf(stderr, "%s\n", ErrorCodeToStringA(_r));
			return _r;
		}
		if (0 == _stricmp("-n", argv[1])) {  // name
			DWORD pid = 0; bool success = true;
			std::vector<ProcessInfo> pses; Process.find(s2ws(argv[2]), pses);
			for (auto& i : pses) {
				const char* argvs[3] = { argv[0],"-p",std::to_string(i.id()).c_str()};
				if (main(3, argvs)) success = false;
			}
			return !success;
		}
	}

	if (argc >= 2 && 0 == _stricmp("-i", argv[1])) {  // interface
		printf("Which process do you want to fuck? (PID) ");
		DWORD pid = 0; std::cin >> pid;
		const char* argvs[3] = { argv[0],"-p",std::to_string(pid).c_str() };
		return main(3, argvs);
	}
	
	fprintf(stderr, "%s\n", ErrorCodeToStringA(ERROR_INVALID_PARAMETER));
	return ERROR_INVALID_PARAMETER;
	return 0;
}

#include<Psapi.h>
#pragma comment(lib, "psapi.lib")
auto test_process_died(DWORD pid) -> bool {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess) return true;
	CloseHandle(hProcess);
	return false;
};
decltype(GetLastError()) FillProcessMemoryById(DWORD pid, BYTE hex/*, bool KillOnExit*/) {
	constexpr auto BYTEsize = sizeof(BYTE);

#if 1
	HANDLE hProcess = NULL;
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		return GetLastError();
	}

	size_t MEMORYUSE = 0;
	{
		PROCESS_MEMORY_COUNTERS pmc; AutoZeroMemory(pmc);
		K32GetProcessMemoryInfo(hProcess, &pmc, sizeof pmc);
		MEMORYUSE = pmc.WorkingSetSize / BYTEsize;
	}

	void* baseAddr = GetModuleBaseAddrByPID(pid);	
	if (baseAddr) {
		constexpr unsigned int _hb_size = 1024 * 64;
		BYTE* hex_buffer = (BYTE*)calloc(_hb_size, BYTEsize);
		if (!hex_buffer) {
			CloseHandle(hProcess);
			return ERROR_OUTOFMEMORY;
		}
		for (size_t i = 0; i < _hb_size / BYTEsize; ++i)
			memcpy(hex_buffer + i, &hex, BYTEsize); // set buffer memory
#if 0
		{std::fstream fp("tmp.log", std::ios::out | std::ios::binary);
		fp.write((char*)hex_buffer, _hb_size);
		fp.close(); }
#endif
		for (size_t i = MEMORYUSE; i > 0; i -= _hb_size) {      // Write Process Memory
			//printf("Writing memory at %p ...\r", ((BYTE*)baseAddr) + i);
			//printf("[LOG] i=%lld\r", i);
			WriteProcessMemory(hProcess, ((BYTE*)baseAddr) + i,
				hex_buffer, _hb_size, NULL);
			if (WaitForSingleObject(hProcess, 0) != WAIT_TIMEOUT) break;
		}
		free(hex_buffer);
	}

	CloseHandle(hProcess);

#else
	if (!EnableDebugPrivilege()) return GetLastError();
	if (!DebugActiveProcess(pid)) return GetLastError();

	DebugSetProcessKillOnExit(KillOnExit);

	DebugActiveProcessStop(pid);
#endif

	return ERROR_SUCCESS;
}

void SigHandler_SIGINT(int)
{
	fprintf(stderr, "SIGINT");
	ExitProcess(ERROR_CANCELLED);
}


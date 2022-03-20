#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <conio.h>
#include <iostream>
#include <string>

#if 0
extern "C" NTSYSAPI NTSTATUS NtUnmapViewOfSection(
    HANDLE ProcessHandle,
    PVOID  BaseAddress
);
#else
NTSTATUS CallNtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress);
#endif
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next
std::string  ErrorCodeToStringA(DWORD ErrorCode);
std::wstring s2ws(const std::string);
std::string ws2s(const std::wstring);
#define s2wc(x) (s2ws(x).c_str())
#define ws2c(x) (ws2s(x).c_str())
#ifdef UNICODE
inline std::wstring to__str(const std::string& str) { return s2ws(str); }
inline std::wstring to__str(const std::wstring& str) { return str; }
#else
inline std::string to__str(const std::string& str) { return str; }
inline std::string to__str(const std::wstring& str) { return ws2s(str); }
#endif
#ifndef pause
#define pause {puts("Press any key to continue...");(void)_getch();}
#endif
BOOL AdjustPrivilege(ULONG Privilege, BOOLEAN Enable,
	BOOLEAN CurrentThread, PBOOLEAN Enabled = NULL);

PVOID __stdcall GetProcessModuleBaseAddress(
	DWORD dwProcessId, PCTSTR module_name, PMODULEENTRY32 result = nullptr
);

bool ExecuteNUVOS(DWORD pid, PCSTR module_name,
	bool useNtUnmapViewOfSection = false);

int main(int argc, char* argv[]) {
	using namespace std;
    if (argc < 3) {
        std::cout << "Usage:\n\t"
            "UnloadProcessModule <[/N ]ProcessName or PID>"
            " <Module name> [/useNtUnmapViewOfSection]" << std::endl;
        return ERROR_INVALID_PARAMETER;
    }
	if (!AdjustPrivilege(0x14, TRUE, FALSE)) {
		cout << "Warning: Cannot RtlAdjustPrivilege  (Privilege: ";
#ifdef UNICODE
		wcout << SE_DEBUG_NAME << L")" << endl;
#else
		cout << SE_DEBUG_NAME << ")" << endl;
#endif
	}
	//pause;
	bool argv1_is__n = (_stricmp(argv[1], "/n") == 0);
	bool usenu = (argc < 4) ? false :
		(argc >= 5) ? (_stricmp(argv[4], "/useNtUnmapViewOfSection") == 0) :
		(_stricmp(argv[3], "/useNtUnmapViewOfSection") == 0);
    DWORD pid = atol(argv[1]);
	PCSTR modname = (argv1_is__n) ? argv[3] : argv[2];
    if (pid == 0 || argv1_is__n) {
		PCSTR procname = argv1_is__n ? argv[2] : argv[1];
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot) {
			return NULL;
		}
		PROCESSENTRY32 pe = { sizeof(pe) };
		Process32First(hSnapshot, &pe);
		bool bSuccess = true;
		do {
			if (0 == _stricmp(pe.szExeFile, procname)) {
				bSuccess = bSuccess && ExecuteNUVOS(pe.th32ProcessID, modname, usenu);
			}
		} while (Process32Next(hSnapshot, &pe));
		CloseHandle(hSnapshot);
		return /*(bSuccess == true) ? 0 : 1*/!bSuccess;
    }
	return !ExecuteNUVOS(pid, modname, usenu);
    return 0;
}

bool ExecuteNUVOS(DWORD pid, PCSTR module_name, bool useNtUnmapViewOfSection) {
	using namespace std;
	bool bSuccess = true;
	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hp) {
		MODULEENTRY32 md32{ 0 };
		PVOID base_addr = GetProcessModuleBaseAddress
		(pid, to__str(module_name).c_str(), &md32);
		if (base_addr) {
			if (useNtUnmapViewOfSection) {
				NTSTATUS stat = CallNtUnmapViewOfSection(hp, base_addr);
				if (NT_SUCCESS(stat)) {
					cout << "NtUnmapViewOfSection (pid: " <<
						pid << ") successsfully" << endl;
				}
				else {
					bSuccess = false;
					DWORD err_code = GetLastError();
					cerr << "ERROR: NtUnmapViewOfSection " << pid
						<< " failed : (NTSTATUS)" << stat << endl;
				}
			}
			HMODULE kernel32_dll = LoadLibrary(_T("kernel32.dll"));
			if (kernel32_dll) {
				LPTHREAD_START_ROUTINE pFreeLibrary = (LPTHREAD_START_ROUTINE)
					GetProcAddress(kernel32_dll, "FreeLibrary");
				if (pFreeLibrary) {
					HANDLE hThread = CreateRemoteThread(
						hp, NULL, 0, pFreeLibrary, md32.hModule, 0, 0);
					if (hThread) {
						CloseHandle(hThread);
						cout << "FreeLibrary executed (pid: " << pid << ")" << endl;
					}
				}
				FreeLibrary(kernel32_dll);
			}
		}
		else {
			bSuccess = false;
			DWORD err_code = GetLastError();
			cerr << "ERROR: GetProcessModuleBaseAddress " << pid
				<< " failed (" << err_code << "): "
				<< ErrorCodeToStringA(err_code) << endl;
		}
	}
	else {
		bSuccess = false;
		DWORD err_code = GetLastError();
		cerr << "ERROR: OpenProcess " << pid <<
			" failed (" << err_code << "): " <<
			ErrorCodeToStringA(err_code) << endl;
	}
	return bSuccess;
}

PVOID __stdcall GetProcessModuleBaseAddress(
	DWORD dwProcessId, PCTSTR module_name, PMODULEENTRY32 result
) {
	HANDLE hSap/*nap*/ = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (!hSap) {
		DWORD err_code = GetLastError();
		hSap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32, dwProcessId);
		if (!hSap) {
			SetLastError(err_code);
			return NULL;
		}
	}
	MODULEENTRY32 me32{ 0 };
	me32.dwSize = sizeof(me32);
	if (Module32First(hSap, &me32))
	do {
		if (_tcsicmp(me32.szModule, module_name) == 0) {
			CloseHandle(hSap);
			if (result) memcpy(result, &me32, sizeof(me32));
			return me32.modBaseAddr;
		}
	} while (Module32Next(hSap, &me32));
	CloseHandle(hSap);
	return NULL;
}

std::string  ErrorCodeToStringA(DWORD ErrorCode) {
	using namespace std;
	typedef LPSTR str_t;
	str_t LocalAddress = NULL;
	if (NULL == ([&] {
		__try {
			if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
				return (str_t)NULL;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
		return LocalAddress;
	})()) return "";
	LocalAddress[strlen((str_t)LocalAddress) - 1] = 0;
	string szText = LocalAddress;
	LocalFree((HLOCAL)LocalAddress);
	return szText;
}

std::string ws2s(const std::wstring wstr) {
	using namespace std;
	string result;
	size_t len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), NULL, 0, NULL, NULL);
	if (len <= 0)return result;
	char* buffer = new char[len + 1];
	if (buffer == NULL)return result;
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), 
		(int)(wstr.size()), buffer, (int)len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
std::wstring s2ws(const std::string str) {
	using namespace std;
	wstring result;
	size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), 
		(int)(str.size()), NULL, 0);
	if (len < 0)return result;
	wchar_t* buffer = new wchar_t[len + 1];
	if (buffer == NULL)return result;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)(str.size()), 
		buffer, (int)len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

BOOL AdjustPrivilege(
	ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled
) {
	typedef NTSTATUS(WINAPI*pRtlAdjustPrivilege)
		(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	pRtlAdjustPrivilege RtlAdjustPrivilege = NULL;
	HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll) return FALSE;
	RtlAdjustPrivilege = (pRtlAdjustPrivilege)
		GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return FALSE;
	BOOLEAN _tmp0 = 0;
	NTSTATUS result = RtlAdjustPrivilege(
		Privilege, Enable, CurrentThread, Enabled ? Enabled : &_tmp0);
	BOOL success = NT_SUCCESS(result);
	return success;
	/* SeDebugPrivilege 0x14 */
}

NTSTATUS CallNtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress) {
	HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll) return (NTSTATUS)-1;
	typedef NTSTATUS(WINAPI*tNtUnmapViewOfSection)(HANDLE, PVOID);
	tNtUnmapViewOfSection NtUnmapViewOfSection =
		(tNtUnmapViewOfSection)GetProcAddress(ntdll, "NtUnmapViewOfSection");
	if (!NtUnmapViewOfSection) return (NTSTATUS)-1;
	return NtUnmapViewOfSection(ProcessHandle, BaseAddress);
}



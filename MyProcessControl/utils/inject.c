#include "inject.h"

HMODULE InjectDllToProcess(DWORD dwProcessId, LPCWSTR szDllPath, DWORD dwTimeout) {
	// Status code
	//DWORD dwStatCode = ERROR_SUCCESS;

	// Get the addresses
	// Note: the address of [ntdll,user32,kernel32] is same for all processes
	HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
	if (!kernel32) return NULL;
	LPTHREAD_START_ROUTINE pLoadLibraryW = (LPTHREAD_START_ROUTINE)
		GetProcAddress(kernel32, "LoadLibraryW");
	if (!pLoadLibraryW) return NULL;

	HANDLE hProcess = NULL; // the handle of the target process
	// Open the process
	hProcess = OpenProcess(GENERIC_READ | GENERIC_WRITE | /*Read and write*/
							 PROCESS_CREATE_THREAD         , FALSE, dwProcessId);
	if (!hProcess) return NULL;

	// Alloc memory for the process to write dll path
	SIZE_T pPathSize = wcslen(szDllPath) * 2;
	PVOID pAddrDllPath = VirtualAllocEx
		(hProcess, NULL, pPathSize, MEM_COMMIT, PAGE_READWRITE);
	if (!pAddrDllPath) {
		CloseHandle(hProcess);
		return NULL;
	}

	// Write the dll path to the memory
	if (!WriteProcessMemory(hProcess, pAddrDllPath, szDllPath, pPathSize, NULL)) {
		VirtualFreeEx(hProcess, pAddrDllPath, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return NULL;
	}

	/*
							***EXECUTE INJECT***    
	*/
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0,
		pLoadLibraryW, pAddrDllPath, CREATE_SUSPENDED, NULL);
	if (!hRemoteThread) {
		VirtualFreeEx(hProcess, pAddrDllPath, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return NULL;
	}
	HMODULE hModuleRemoteDll = ((HMODULE)(LONG_PTR)-1);
	ResumeThread(hRemoteThread);
	if (WAIT_TIMEOUT != WaitForSingleObject(hRemoteThread, dwTimeout)) {
		DWORD dwExitCode = 0;
		GetExitCodeThread(hRemoteThread, &dwExitCode);
		hModuleRemoteDll = *((HMODULE*)(&dwExitCode)); // Ignore the warning
	}
	CloseHandle(hRemoteThread);

	// do some cleaning
	if (pAddrDllPath) VirtualFreeEx(hProcess, pAddrDllPath, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	// and return
	return hModuleRemoteDll;
}


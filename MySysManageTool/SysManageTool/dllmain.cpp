﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"
#include "DriverLoader.h"
using namespace std;

HINSTANCE hInst;
static HANDLE hKdDrv = NULL;
static LPCWSTR szDrvName = L"MySysManageDrv_mbQ6o6vh";
static LPCWSTR szDrvPath = L".\\MySysManageDrv_mbQ6o6vh.sys";
#ifdef _WIN64
#define IDR_KDRIVER_CURRENT IDR_BIN_KDRIVER_64
#else
#define IDR_KDRIVER_CURRENT IDR_BIN_KDRIVER_32
#endif
bool InitKernelDriver();
bool UnInitKernelDriver();
void ReleaseDriver(LPCWSTR szSaveTo);

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		::hInst = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (hKdDrv) UnInitKernelDriver();
		if (file_exists(szDrvPath)) DeleteFileW(szDrvPath);
		break;
	}
	return TRUE;
}


bool InitKernelDriver() {
	if (!file_exists(szDrvPath)) ReleaseDriver(0);
	//ServiceManager.Remove(ws2s(szDrvName));
	WCHAR szDrvFullPath[MAX_PATH + 1] = { 0 };
	GetFullPathNameW(szDrvPath, MAX_PATH, szDrvFullPath, NULL);
	//MessageBoxW(NULL, szDrvFullPath, szDrvPath, 0);
	if (!MLoadKernelDriver(szDrvName, szDrvFullPath, nullptr)) {
		//MessageBoxW(NULL, LastErrorStrW(), L"", 0);
		ServiceManager.Remove(ws2s(szDrvName));
		return false;
	}
	hKdDrv = MOpenDriver(szDrvName);
	return hKdDrv;
}
bool UnInitKernelDriver() {
	return MUnLoadKernelDriver(szDrvName) &&
		MUnLoadDriverServiceWithMessage(szDrvName) &&
		CloseHandle(hKdDrv);
}

void ReleaseDriver(LPCWSTR szSaveTo) {
	szSaveTo ? 0 : szSaveTo = szDrvPath;
	FreeResFile(IDR_KDRIVER_CURRENT, "BIN", ws2c(szSaveTo), hInst);
	SetFileAttributesW(szSaveTo, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

bool InstallDriverCert(bool install) {
	#define szSaveTo "MySysManageDrv_mbQ6o6vh.cer"
	if (!FreeResFile(IDR_BIN_KDRIVER_CERT, "BIN", szSaveTo, hInst)) return false;
	SetFileAttributesA(szSaveTo, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	wstring v; if (install) v = L"-addstore"; else v = L"-delstore";
	auto pi = Process.Start_HiddenWindow(L"certutil " + v + L" root " szSaveTo);
	while (WaitForSingleObject(pi.hProcess, MAXDWORD) == WAIT_TIMEOUT);
	DWORD dwExitCode = 0;
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	
	Process.CloseProcessHandle(pi);
	DeleteFileA(szSaveTo);
	if (dwExitCode == 0) return true;
	return false;
	#undef szSaveTo
}


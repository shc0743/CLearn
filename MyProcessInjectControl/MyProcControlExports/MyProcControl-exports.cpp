#include "MyProcControl-exports.h"
#include <string>
#include "../../resource/tool.h"
#include "resource.h"
using namespace std;

extern HMODULE hInst;


AppVersion_t __stdcall util_GenerateVersionId(
	AppVersionBit_t v0,
	AppVersionBit_t v1,
	AppVersionBit_t v2
) {
	return ((AppVersion_t)v0 << 16) + ((AppVersion_t)v1 << 4) + (v2);
}
AppVersionInfo_t __stdcall util_VersionIdToInfo(AppVersion_t id) {
	AppVersionInfo_t info{ 0 };
	MessageBoxW(0, to_wstring((INT_PTR)&info).c_str(), L"test", 0);
	info.v0 = (id & 0xFF0000) >> 16;
	info.v1 = (id & 0x00FF00) >> 4;
	info.v2 = (id & 0x0000FF) >> 0;
	return info;
}


DWORD __stdcall __export__(PVOID) {
	return 0;
}

AppVersion_t __stdcall MGetVersion() {
	return util_GenerateVersionId(1, 3, 1);
}

DWORD CALLBACK MRundInstall(
	HWND hWnd, HINSTANCE hInst, PSTR lpCmdLine, int nCmdShow
) {
	CmdLineW cl(GetCommandLineW());
	wstring bin_path, svc_name, dsp_name, des_text, cfg_path,
		startup_type, Sspawn_wnd;
	cl.getopt(L"svc-name", svc_name);
	cl.getopt(L"display-name", dsp_name);
	cl.getopt(L"description", des_text);
	cl.getopt(L"config", cfg_path);
	cl.getopt(L"startup-type", startup_type);
	cl.getopt(L"binPath", bin_path);
	cl.getopt(L"spawnwnd", Sspawn_wnd);
	if (svc_name.empty()) return ERROR_INVALID_PARAMETER;
	if (dsp_name.empty()) dsp_name = svc_name;
	if (des_text.empty()) {
		WCHAR desctext[2048]{ 0 };
		LoadStringW(NULL, IDS_STRING_SVC_DESCRIPTION, desctext, 2047);
		des_text = desctext;
	}
	dsp_name += L" (" + svc_name + L")";
	if (bin_path.empty()) bin_path = s2ws(GetProgramDir());
	HWND spawn_wnd = NULL;
	if (!Sspawn_wnd.empty()) {
#ifdef _WIN64
		LONG_PTR n = atoll(ws2c(Sspawn_wnd));
#else
		LONG_PTR n = atol(ws2c(Sspawn_wnd));
#endif
		if (IsWindow((HWND)n)) spawn_wnd = (HWND)n;
	}

	wstring cmdl = L"\"" + bin_path + L"\" --service"
		" --run-svc-name=\"" + svc_name + L"\"";
	if (!cfg_path.empty()) cmdl += L" --config=\""s + cfg_path + L"\"";
#pragma warning(push)
#pragma warning(disable: 26812)
	UINT err = ServiceManager.New(svc_name, cmdl,
		(startup_type == L"auto" ? ServiceManager_::Auto :
			(startup_type == L"disabled" ?
				ServiceManager_::Disabled : ServiceManager_::Manual)),
		dsp_name, des_text, SERVICE_WIN32_OWN_PROCESS);
#pragma warning(pop)
	if (err) {
		DWORD ec = GetLastError();
		fprintf(stderr, "ERROR: Error code %u\n", err);
		if (ec == ERROR_SERVICE_EXISTS && nCmdShow != -1) {
			if (IDRETRY == MessageBoxW(spawn_wnd, (L"Cannot install ("
				+ to_wstring(ec) + L"): " + ErrorCodeToString(ec)).c_str(),
				NULL, MB_ICONHAND | MB_RETRYCANCEL)) {
				ServiceManager.Remove(ws2s(svc_name));
				return MRundInstall(hWnd, hInst, lpCmdLine, -1);
			}
			return ec;
		}
		MessageBoxW(spawn_wnd, (L"Cannot install (" + to_wstring(ec)
			+ L"): " + ErrorCodeToString(ec)).c_str(), NULL, MB_ICONHAND);
		return ec;
	}
	if (cl.getopt(L"start")) {
		ServiceManager.Start(ws2s(svc_name));
	}
	MessageBoxW(spawn_wnd, ErrorCodeToStringW(0).c_str(), L"OK", MB_ICONINFORMATION);
	return ERROR_SUCCESS;
}

/*
return 0    ERROR
return 32   32bit
return 64   64bit
*/
unsigned char __stdcall util_Is_exe_32_or_64_bit(PCWSTR filename) {
	HANDLE hFile = CreateFileW(filename, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	//printf("%s : %lu\n", ws2c(filename), GetLastError());
	if (!hFile || hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}
	LARGE_INTEGER high_size{ 0 };
	DWORD file_size = GetFileSizeEx(hFile, &high_size);
	
	DWORD mmf_size = 512 * 1024;
	DWORD size_high = 0;
	// 创建文件映射，如果要创建内存页面文件的映射，
	// 第一个参数设置为INVALID_HANDLE_VALUE
	HANDLE hFm = CreateFileMapping(hFile, NULL,
		PAGE_READONLY | SEC_IMAGE, size_high, mmf_size, NULL);

	if (hFm == NULL || hFm == INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return 0;
	}

	size_t view_size = size_t(1024) * 256;
	DWORD view_access = /*FILE_MAP_ALL_ACCESS*/FILE_MAP_READ;

	// 获得映射视图
	PIMAGE_DOS_HEADER base_address = (PIMAGE_DOS_HEADER)
		MapViewOfFile(hFm, view_access, 0, 0, view_size);
	if (base_address != NULL) __try {
		unsigned char result = 0;
		PIMAGE_DOS_HEADER pDos = base_address;
		PIMAGE_NT_HEADERS pNt = 
			(PIMAGE_NT_HEADERS)(pDos->e_lfanew + (char*)pDos);

		if (pNt->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 ||
			pNt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
			result = 64;
		else
			result = 32;
		// 卸载映射
		UnmapViewOfFile(base_address);
		// 关闭内存映射文件
		CloseHandle(hFm);
		// 关闭文件
		CloseHandle(hFile);
		return result;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		SetLastError(GetExceptionCode());
		UnmapViewOfFile(base_address);
		CloseHandle(hFm);
		CloseHandle(hFile);
		return 0;
	}
	else {
		CloseHandle(hFile);
		return 0;
	}
}



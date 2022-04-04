#include "hook.x64.h"
#include <map>

// https://www.cnblogs.com/LyShark/p/13653394.html

typedef struct {
	BYTE code[12];
} OldCodes_t;
static std::map<LPCWSTR, std::map<LPCSTR, OldCodes_t>> OldCodes;

//static BYTE OldCode[12] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static BYTE HookCode[12] = { 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xE0 };

BOOL ApiInlineHook(LPCWSTR lpModule, LPCSTR lpFuncName, LPVOID lpFunction) {
	HMODULE hMod = GetModuleHandle(lpModule);
	if (!hMod) return FALSE;
	DWORD_PTR FuncAddress = (UINT64)GetProcAddress(hMod, lpFuncName);
	if (!FuncAddress) return FALSE;
	DWORD OldProtect = 0; BOOL bRet = TRUE;

	if (VirtualProtect((LPVOID)FuncAddress, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		//memcpy(OldCode, (LPVOID)FuncAddress, 12);                   // 拷贝原始机器码指令

		BYTE oldcd[12]{ 0 };
		memcpy(oldcd, (LPVOID)FuncAddress, 12);                   // 拷贝原始机器码指令
		std::pair<LPCWSTR, std::map<LPCSTR, OldCodes_t>> pair1;
		std::pair<LPCSTR, OldCodes_t> pair2;
		pair2.first = lpFuncName;
		memcpy(pair2.second.code, oldcd, 12);
		pair1.first = lpModule;
		try {
			pair1.second = OldCodes.at(lpModule);
		}
		catch (...) {}
		pair1.second.insert(pair2);
		OldCodes.insert(pair1);

		*(PINT64)(HookCode + 2) = (UINT64)lpFunction;               // 填充90为指定跳转地址
	} else bRet = FALSE;
	memcpy((LPVOID)FuncAddress, &HookCode, sizeof(HookCode));       // 拷贝Hook机器指令
	VirtualProtect((LPVOID)FuncAddress, 12, OldProtect, &OldProtect);

	return bRet;
}

BOOL ApiInlineUnHook(LPCWSTR lpModule, LPCSTR lpFuncName) {
	HMODULE hMod = GetModuleHandle(lpModule);
	if (!hMod) return FALSE;
	UINT64 FuncAddress = (UINT64)GetProcAddress(hMod, lpFuncName);
	if (!FuncAddress) return FALSE;
	DWORD OldProtect = 0; BOOL bRet = TRUE;
	if (VirtualProtect((LPVOID)FuncAddress, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		try {
			memcpy((LPVOID)FuncAddress, OldCodes.at(lpModule).at(lpFuncName).code, 12);
			auto& sub1 = OldCodes.at(lpModule);
			sub1.erase(lpFuncName);
			if (sub1.empty()) OldCodes.erase(lpModule);
		}
		catch (...) {
			bRet = FALSE;
		}
	} else bRet = FALSE;
	VirtualProtect((LPVOID)FuncAddress, 12, OldProtect, &OldProtect);
	return bRet;
}

#if 0
int WINAPI MyMessageBoxW(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	// 首先恢复Hook代码
	UnHook(L"user32.dll", "MessageBoxW");
	int ret = MessageBoxW(0, L"hello lyshark", lpCaption, uType);

	// 调用结束后,再次挂钩
	Hook(L"user32.dll", "MessageBoxW", (PROC)MyMessageBoxW);
	return ret;
}
#endif


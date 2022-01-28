#include "hook.h"

// https://www.cnblogs.com/LyShark/p/13653394.html

BYTE OldCode[12] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
BYTE HookCode[12] = { 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xE0 };

BOOL ApiInlineHook(LPCWSTR lpModule, LPCSTR lpFuncName, LPVOID lpFunction) {
	HMODULE hMod = GetModuleHandle(lpModule);
	if (!hMod) return FALSE;
	DWORD_PTR FuncAddress = (UINT64)GetProcAddress(hMod, lpFuncName);
	if (!FuncAddress) return FALSE;
	DWORD OldProtect = 0; BOOL bRet = TRUE;

	if (VirtualProtect((LPVOID)FuncAddress, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		memcpy(OldCode, (LPVOID)FuncAddress, 12);                   // 拷贝原始机器码指令
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
		memcpy((LPVOID)FuncAddress, OldCode, sizeof(OldCode));
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


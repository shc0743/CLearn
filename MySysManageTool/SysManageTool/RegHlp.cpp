/*
Copied and modified from JiYuTrainer
https://github.com/imengyu/JiYuTrainer/blob/master/JiYuTrainer/RegHlp.cpp

MIT License

Copyright (c) 2019 Mengyu (√ŒÏ£) www.imengyu.top

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "RegHlp.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//…æ≥˝◊¢≤·±Ìº¸“‘º∞◊”º¸
BOOL MRegDeleteKey(HKEY hRootKey, LPWSTR path) {

	DWORD lastErr = SHDeleteKey(hRootKey, path);
	if (lastErr == ERROR_SUCCESS || lastErr == ERROR_FILE_NOT_FOUND)
		return TRUE;
	else
	{
		SetLastError(lastErr);
		return 0;
	}
}
BOOL MRegForceDeleteServiceRegkey(LPWSTR lpszDriverName)
{
	BOOL rs = FALSE;
	wchar_t regPath[MAX_PATH];
	wsprintfW(regPath, L"SYSTEM\\CurrentControlSet\\services\\%s", lpszDriverName);
	rs = MRegDeleteKey(HKEY_LOCAL_MACHINE, regPath);

	wchar_t regName[MAX_PATH] = { 0 };
	wcscpy_s(regName, lpszDriverName);
#pragma warning(push)
#pragma warning(disable: 6054)
	_wcsupr_s(regName);
#pragma warning(pop)
	wsprintfW(regPath, L"SYSTEM\\CurrentControlSet\\Enum\\Root\\LEGACY_%s", regName);
	rs = MRegDeleteKey(HKEY_LOCAL_MACHINE, regPath);

	if (!rs) {
		rs = TRUE;
	}

	return rs;
}
#if 0
BOOL MRegReadKeyString64And32(HKEY hRootKey, LPCWSTR path32, LPCWSTR path64, LPCWSTR keyName, LPWSTR buffer, SIZE_T count) {
	BOOL rs = FALSE;
	HKEY hKey;
	LRESULT lastError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SysHlp::Is64BitOS() ? path64 : path32, 0, KEY_WOW64_64KEY | KEY_READ, &hKey);
	if (lastError == ERROR_SUCCESS) {
		DWORD dwType = REG_SZ;
		WCHAR * Data = new WCHAR[count];
		DWORD cbData = count;
		lastError = RegQueryValueEx(hKey, keyName, 0, &dwType, (LPBYTE)Data, &cbData);		
		if (lastError == ERROR_SUCCESS) {
			wcscpy_s(buffer, count, Data);
			rs = TRUE;
		}
		delete Data;
		RegCloseKey(hKey);
	}
	SetLastError(lastError);
	return rs;
}
BOOL MRegReadKeyString(HKEY hRootKey, LPCWSTR path, LPCWSTR keyName, LPWSTR buffer, SIZE_T count) {
	BOOL rs = FALSE;
	HKEY hKey;
	LRESULT lastError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &hKey);
	if (lastError == ERROR_SUCCESS) {
		DWORD dwType = REG_SZ;
		WCHAR * Data = new WCHAR[count];
		DWORD cbData = count;
		lastError = RegQueryValueEx(hKey, keyName, 0, &dwType, (LPBYTE)Data, &cbData);
		if (lastError == ERROR_SUCCESS) {
			wcscpy_s(buffer, count, Data);
			rs = TRUE;
		}
		delete Data;
		RegCloseKey(hKey);
	}
	SetLastError(lastError);
	return rs;
}
/*
BOOL MRegCheckUninstallItemExists(LPCWSTR keyName) {
	WCHAR Data[MAX_PATH];
	if (MRegReadKeyString64And32(HKEY_LOCAL_MACHINE,
		FormatString(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s", keyName).c_str(),
		FormatString(L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s", keyName).c_str(),
		L"DisplayIcon", Data, MAX_PATH)) {
		return TRUE;
	}
	else currentLogger->LogWarn(L"MRegReadKeyString64And32 failed : %s (%d) ", PRINT_LAST_ERROR_STR);

	return FALSE;
}
*/
#endif

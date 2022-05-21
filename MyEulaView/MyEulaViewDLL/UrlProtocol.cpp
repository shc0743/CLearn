#include "UrlProtocol.h"


static bool IsRunAsAdmin();

bool __stdcall UrlProtocol_Create(PCWSTR protocol_name, PCWSTR command) {
	using namespace std;
	LRESULT result = 0;
	DWORD tmp = 0;
	HKEY hk = HKEY_CLASSES_ROOT;
	if (!IsRunAsAdmin()) {
		result = RegOpenKeyExW(HKEY_CURRENT_USER,
			L"SOFTWARE\\Classes", 0, KEY_READ | KEY_WRITE, &hk);
		if (result) {
			SetLastError((DWORD)result);
			return false;
		}
	}
	
	HKEY hk2 = NULL;
	HKEY hk3 = NULL;

	result = RegCreateKeyExW(hk, protocol_name, 0, NULL, REG_OPTION_VOLATILE,
		KEY_READ | KEY_WRITE, NULL, &hk2, &tmp);
	if (result) {
		goto quit;
	}

	result = RegCreateKeyExW(hk, (protocol_name +
		L"\\shell\\open\\command"s).c_str(), 0, NULL,
		REG_OPTION_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hk3, &tmp);
	if (result) {
		RegDeleteTreeW(hk, protocol_name);
		goto quit;
	}

	result = RegSetValueExW(hk3, NULL, 0, REG_SZ,
		(PBYTE)command, DWORD(DWORD(wcslen(command) + 1) * 2));
	if (result) {
		RegDeleteTreeW(hk, protocol_name);
		goto quit;
	}

	RegSetValueExW(hk2, L"URL Protocol", 0, REG_SZ, (PBYTE)L"", 2);
	
quit:
	if (hk3) RegCloseKey(hk3);
	if (hk2) RegCloseKey(hk2);
	if (hk != HKEY_CLASSES_ROOT) RegCloseKey(hk);
	SetLastError((DWORD)result);
	return !result;
}

bool __stdcall UrlProtocol_Remove(PCWSTR protocol_name) {
	HKEY hk = HKEY_CLASSES_ROOT;
	if (!IsRunAsAdmin()) {
		auto result = RegOpenKeyExW(HKEY_CURRENT_USER,
			L"SOFTWARE\\Classes", 0, KEY_READ | KEY_WRITE, &hk);
		if (result) {
			SetLastError((DWORD)result);
			return false;
		}
	}
	auto result = RegDeleteTreeW(hk, protocol_name);
	if (hk != HKEY_CLASSES_ROOT) RegCloseKey(hk);
	SetLastError((DWORD)result);
	return !result;
}

static bool IsRunAsAdmin() {
	BOOL bElevated = false;
	HANDLE hToken = NULL;
	// Get current process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return false;
	TOKEN_ELEVATION tokenEle = {0};
	DWORD dwRetLen = 0;
	// Retrieve token elevation information
	if (GetTokenInformation(hToken, TokenElevation,
		&tokenEle, sizeof(tokenEle), &dwRetLen))
	{
		if (dwRetLen == sizeof(tokenEle))
		{
			bElevated = tokenEle.TokenIsElevated;
		}
	}
	CloseHandle(hToken);
	return bElevated;
}


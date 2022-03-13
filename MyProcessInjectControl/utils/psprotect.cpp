#include "psprotect.h"
// author: "不爱说话的老师"
// https://space.bilibili.com/470551444
// https://www.bilibili.com/video/BV1TM4y1P7qM
// https://t.bilibili.com/617132898598375225?tab=2
//

BOOL _ProtectProcessOrThreadHandle(HANDLE hObject, BOOL bForce) {
	SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSid = { 0 };
	AllocateAndInitializeSid(&sia, 1,0,0,0,0,0,0,0,0, &pSid); // init Everyone
	BYTE buf[0x200] = { 0 };
	PACL pAcl = (PACL)&buf;
	InitializeAcl(pAcl, 1024, ACL_REVISION);
	AddAccessDeniedAce(pAcl, ACL_REVISION, 
		GENERIC_WRITE | DELETE | WRITE_DAC | WRITE_OWNER |
		SYNCHRONIZE | PROCESS_TERMINATE, pSid); 
	AddAccessAllowedAce(pAcl, ACL_REVISION,
		GENERIC_READ | GENERIC_EXECUTE | READ_CONTROL, pSid);
	if (bForce) {
		SetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION |
			PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL); // reset first
	}
	BOOL retValue = SetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION |
		PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, pAcl, NULL);
	FreeSid(pSid);
	return retValue;
}

BOOL ProtectProcessAndThread(HANDLE hProcess) {
	do {
		THREADENTRY32 stThread;
		ZeroMemory(&stThread, sizeof stThread);
		stThread.dwSize = sizeof(THREADENTRY32);
		HANDLE hThrdShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (!hThrdShot) break;
		Thread32First(hThrdShot, &stThread);
		do {
			if (stThread.th32OwnerProcessID != GetProcessId(hProcess)) continue;
			HANDLE hThread = OpenThread(WRITE_DAC, FALSE, stThread.th32ThreadID);
			if (hThread == NULL) continue;
			if (!_ProtectProcessOrThreadHandle(hThread, FALSE)) {
				_ProtectProcessOrThreadHandle(hThread, TRUE);
			}
			CloseHandle(hThread);
		} while (Thread32Next(hThrdShot, &stThread));
		CloseHandle(hThrdShot);
	} while (0);
	if (!_ProtectProcessOrThreadHandle(hProcess, FALSE)) {
		_ProtectProcessOrThreadHandle(hProcess, TRUE);
	}
	return TRUE;
}


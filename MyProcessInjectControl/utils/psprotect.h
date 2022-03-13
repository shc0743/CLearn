#pragma once
#include <windows.h>
#include <Tlhelp32.h>
#include <Aclapi.h>

// author: "不爱说话的老师"
// https://space.bilibili.com/470551444
// https://www.bilibili.com/video/BV1TM4y1P7qM
// https://t.bilibili.com/617132898598375225?tab=2
//

BOOL _ProtectProcessOrThreadHandle(HANDLE hObject, BOOL bForce);
BOOL ProtectProcessAndThread(HANDLE hProcess);


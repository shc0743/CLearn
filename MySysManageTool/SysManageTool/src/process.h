#pragma once
#include "../includer.h"
HANDLE ZwOpenProcess(_In_ DWORD dwDesiredAccess,
    _In_ BOOL bInheritHandle,
    _In_ DWORD dwProcessId);


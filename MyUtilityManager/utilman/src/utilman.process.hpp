#pragma once
#include "framework.h"
#include "../../../resource/tool.h"

DECLARE_HANDLE(SubProcess_type_ValueError);
int __stdcall SubProcess_type_handler(std::wstring type, CmdLineW& cl);

int __stdcall SubProcess_root(CmdLineW& cl);


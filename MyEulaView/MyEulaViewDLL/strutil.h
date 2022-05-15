#pragma once
#include "cppwin_include.h"


std::string str_to_u8str(const wchar_t* buffer, DWORD len);
std::string str_to_u8str(const std::wstring& str);
std::string str_to_u8str(const std::string& str);


#pragma once
#include "cppwin_include.h"


bool WINAPI UrlProtocol_Create(PCWSTR protocol_name, PCWSTR command);
bool WINAPI UrlProtocol_Remove(PCWSTR protocol_name);


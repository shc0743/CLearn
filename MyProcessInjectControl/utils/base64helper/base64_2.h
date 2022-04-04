#pragma once
// https://blog.csdn.net/qq_46106285/article/details/120248403
#include <string.h>
#include <stdlib.h>
#include <wchar.h> 
#include <assert.h>
#include <windows.h>
#include <iostream>
typedef unsigned char     uint8;
typedef unsigned long    uint32;
size_t __stdcall base64_encode(char* input, uint8* encode);
size_t __stdcall base64_decode(const uint8* code, size_t code_len, char* str);
//char* G2U(const char* gb2312);

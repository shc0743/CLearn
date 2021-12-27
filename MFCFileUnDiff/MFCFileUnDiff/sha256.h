#pragma once
#include <iostream> 
#include <cstdio>
#include <cstdlib>

using namespace std;

char* StrSHA256(const char* str, long long length, char* sha256, size_t sha256size);
char* FileSHA256(const char* file, char* sha256, size_t sha256size);


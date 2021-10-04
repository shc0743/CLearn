// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<algorithm>
#ifndef _WINDOWS_
#include<Windows.h>
#include<TlHelp32.h>
#endif
#include<tchar.h>
#include<typeinfo>
#include<assert.h>
#include<thread>
#include<chrono>
using namespace std;

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
wstring s2ws(const string);
string ws2s(const wstring);
#define s2wc(x) (s2ws(x).c_str())
#define ws2c(x) (ws2s(x).c_str())
string GetProgramDir();

extern string srcfile;
extern string destfile;
extern bool hlink;
extern bool dlink;

#endif //PCH_H

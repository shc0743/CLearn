#pragma once
/*
Copied and modified from JiYuTrainer
https://github.com/imengyu/JiYuTrainer/blob/master/JiYuTrainer/DriverLoader.h

MIT License

Copyright (c) 2019 Mengyu (梦欤) www.imengyu.top

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "includer.h"

//加载驱动
//    lpszDriverName：驱动的服务名
//    driverPath：驱动的完整路径
//    lpszDisplayName：驱动的显示名称
bool MLoadKernelDriver(const wchar_t* lpszDriverName,
	const wchar_t* driverPath, const wchar_t* lpszDisplayName = nullptr);
//卸载驱动
//    szSvrName：服务名
bool MUnLoadKernelDriver(const wchar_t* szSvrName);
bool MUnLoadDriverServiceWithMessage(const wchar_t * szSvrName);
//打开驱动
HANDLE MOpenDriver(LPCWSTR szDrvName);


#include<iostream>
#include<string>
#include<sstream>
#include<stdio.h>
#include<cstdio>
#include<conio.h>
#include<stdlib.h>

//Windows程序
#include<Windows.h>
//

#include "fn.h"

#define cls system("cls")

bool movePosition(int y/*X坐标*/, int x/*Y坐标*/) //移动控制台光标(仅Windows)
{
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;    // position 是一个 COORD 结构
	position.X = x; //设置列在x附近
	position.Y = y; //设置行在y附近
	//将光标放在那里，然后开始打印
	SetConsoleCursorPosition(screen, position);
	return false;
}

void startupExe(const std::wstring& paras)
{
    
}

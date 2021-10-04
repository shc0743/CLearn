#include <iostream>
#include<stdio.h>
#include<conio.h>
#include<Windows.h>
using namespace std;

static bool SetConsoleColor(WORD wAttr) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)			return FALSE;
	return SetConsoleTextAttribute(hConsole, wAttr);
}
/**
	第一个对应于背景，第二个对应于前景。
	0 = 黑色       8 = 灰色
	1 = 蓝色       9 = 淡蓝色
	2 = 绿色       A = 淡绿色
	3 = 浅绿色     B = 淡浅绿色
	4 = 红色       C = 淡红色
	5 = 紫色       D = 淡紫色
	6 = 黄色       E = 淡黄色
	7 = 白色       F = 亮白色
*/
#define SetConsColor(x) _SetConsColor(x)
#define _SetConsColor(ColorVal) SetConsoleColor(0x ## ColorVal)

#define _WAIT_TME 300

int main(){
	cout << "质数验证器\n========" << endl << "输入一个正实数: ";
	unsigned long long num = 0;
	SetConsColor(9);
	cin >> num;
	SetConsColor(7);
	if (!cin.good()) {
		SetConsColor(4);
		cout << "无效输入" << endl;
		SetConsColor(7);
		return 1;
	}
	cout << "是否显示验证过程?[y/N] [_";
	SetConsColor(9);
	char showdets = _getch();
	if (showdets == 'n' || showdets == 'N') {
		printf("N");
		showdets = 0;
	}
	else {
		printf("y");
		showdets = 1;
	}
	SetConsColor(7);
	cout << "_]" << endl;
	SetConsColor(2);
	cout << "您输入了: " << num << " ,即将开始验证";
	Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	printf(".");Sleep(_WAIT_TME);
	SetConsColor(7); cout << endl;

	bool isPrime = true; unsigned long long lostcnt = 0;
	for (unsigned long long cnt = 2; cnt < num;++cnt) {
		lostcnt = num % cnt;
		if(showdets) cout << num << " 除以 " << cnt << " 余数 " << lostcnt;
		if (lostcnt == 0) {
			if(!showdets) cout << num << " 除以 " << cnt << " 余数 " << lostcnt;
			cout << ",所以 " << num << " 不是质数." << endl;
			isPrime = false; break;
		}
		else if(showdets) cout << endl;
	}

	SetConsColor(5);
	if (isPrime) cout << num <<   " 是质数" << endl;
	else         cout << num << " 不是质数" << endl;
	SetConsColor(7);

	cout << endl << "Press any key to continue..." << endl;

	int tmp = _getch();

	return 0;
}
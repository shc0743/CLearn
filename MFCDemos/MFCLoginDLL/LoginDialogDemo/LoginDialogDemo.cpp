#include <iostream>
#include <Windows.h>
#include "../MFCLoginDLL/defines.h"
#include <conio.h>
int main(){
	HMODULE d = LoadLibraryA("mfclogindll");
	if (!d) return 1;
	userinfo u = GetLoginUserInfo(d, true);
	cout << u.name << ' ' << u.pswd << endl;
	(void)_getch();
	return 0;
}

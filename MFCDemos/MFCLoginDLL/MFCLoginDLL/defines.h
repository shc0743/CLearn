#pragma once
#include <string>
#include <Windows.h>
using namespace std;

using userinfo = struct {
	string name;
	string pswd;
};

inline userinfo GetLoginUserInfo(HINSTANCE dll, bool RegisterEnabled) {
	typedef userinfo(*gl)(bool);
	gl f = (gl)GetProcAddress(dll, "GetLoginUserInfo__");
	if (!f) return { "","" };
	return f(RegisterEnabled);
}

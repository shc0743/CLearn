#pragma once
#include<string>
using namespace std;

class MFCWindowSet {
public:
	bool ontop = false;
	bool alwaysrunas = false;
	bool passwd = false;
	string pwd = "";
	bool autorun = false;
	bool clickxtohide = false;
	bool noaskclose = false;
	string StartTitle = "Dialog";
};
wstring s2ws(string);
string ws2s(wstring);

extern MFCWindowSet promainset;
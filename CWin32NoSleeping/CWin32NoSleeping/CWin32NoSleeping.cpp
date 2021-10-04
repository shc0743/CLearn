#include <iostream>
#include<Windows.h>
#include "../../resource/tool.h"
using namespace std;

void __cdecl my_sigint_handle(int);
HANDLE mainThrd;

int main(int argc, char* argv[]) {
	CmdLineA cl(::GetCommandLineA());
	if (cl.getopt("help")) {
		printf( "%s Help Document\nUsage: %s %s\nArguments: \n%s\nMore: %s\nSee also: %s"
			"\nReturn Value: %s", argv[0], argv[0], "[--help] [<(-a|-d|-s|-u|-q)>]"/*Usage*/,
			"\n--help  Show this help document.\n-a      Away mode.\n-d      Forces the "
			"display to be on by resetting the display idle timer.\n-s      Forces the s"
			"ystem to be in the working state by resetting the system idle timer.\n-q   "
			"   Clear all states.\n-u      This value is not supported. If -u is combine"
			"d with other esFlags values, the call will fail and none of the specified s"
			"tates will be set.", "MSDN", "https://docs.microsoft.com/en-us/windows/win3"
			"2/api/winbase/nf-winbase-setthreadexecutionstate", "If it succeeds, the app"
			"lication is never return unless using TerminateProcess or using arg \"--hel"
			"p\" or \"-q\".In the latter case, 0 is returned.\nIf it fails, the return v"
			"alue is 0xcccccccc(-858993460).If it returned -114514, the program is likel"
			"y to have been tampered with. Reinstalling the program may solve this probl"
			"em.\n\nEgg: try to use \"-l\" parameter to start the program!(Entertainment"
			" only)\n\nAuthor: shc0743(https://github.com/shc0743/) Using WinAPI SetThre"
			"adExecutionState"
		);//args
		return 0;
	}
	//signal(SIGINT, my_sigint_handle);
	mainThrd = ::GetCurrentThread();
	DWORD es_bit = ES_CONTINUOUS;
	if (cl.getopt(codetostr(a))) es_bit |= ES_AWAYMODE_REQUIRED;
	if (cl.getopt(codetostr(d))) es_bit |= ES_DISPLAY_REQUIRED;
	if (cl.getopt(codetostr(s))) es_bit |= ES_SYSTEM_REQUIRED;
	if (cl.getopt(codetostr(u))) es_bit |= ES_USER_PRESENT;
	if (!::SetThreadExecutionState(es_bit)) {
		cerr << "Error!!Cannot SetThreadExecutionState(" << es_bit << ") ,GetLastError() == " <<
			::GetLastError() << endl; return 0xcccccccc;
	}
	else {
		cout << "SetThreadExecutionState was succeed.You can close this program to cancel.\n";
	}
	if (es_bit == ES_CONTINUOUS) return 0;
	if (cl.getopt("l")) {
		int col = 0; DWORD sleet = 0; string str;
		if (cl.getopt("l", str) == 1) sleet = atol(str.c_str());
		bool colorenable = !cl.getopt("l-no-change-color");
		while (1) {
			if (colorenable) {
				SetConsoleColor(col);
				if (++col > 0xff) col = 0;
			}
			printf("我是机房里最靓的仔\n");
			if(sleet) Sleep(sleet);
		}
	} else while (1) { Sleep(MAXDWORD); }//普通睡眠
	cerr << "ERROR!!!!Here should never be executed!! If the output of this line of text, "
		"then the program is likely to have been tampered with. Reinstalling the program"
		" may solve this problem.\n";
	return -114514;
}

DWORD my_sigint_hdl_thrd(void* args) {
	//::DuplicateHandle(
	//::SuspendThread(mainThrd);
	SetConsColor(07);
	cout << "+-----------------------------------+\n"
		    "|                                   |\n"
			"|   Do you really want to cancel?   |\n"
			"|                                   |\n"
			"|            [Yes]   [No]           |\n"
			"|                                   |\n"
			"+-----------------------------------+"
		; HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	cout.flush(); COORD cd = { 13,4 };
	SetConsoleCursorPosition(hStdOut, cd);
	while (1) {
		char ch = 0;
		while ((ch = _getch()) && (ch != '\n' && ch != ' ' && ch != '\t')) printf("\a");
	}
	::ResumeThread(mainThrd);
}

void __cdecl my_sigint_handle(int) {
	HANDLE h = ::CreateThread(0, 0, my_sigint_hdl_thrd, 0, CREATE_SUSPENDED, 0);
	if (!h) exit(114514);
	::ResumeThread(h);
	::WaitForSingleObject(h, MAXDWORD);
	//my_sigint_hdl_thrd(0);
}

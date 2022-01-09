#include <iostream>
#include "../MultithreadedDownloadTool/def.h"
#include "../../resource/tool.h"
#pragma comment(lib, "MultithreadedDownloadTool64.lib")
using namespace std;

// D:\software\VisualStudio\msvs\2022c\_\Common7\Tools\VsDevCmd.bat

#define LOADLIBATRUN 1

void SigIntHandler(int);

int main(int argc, char* argv[]) {

	// register SIGINT handler
	signal(SIGINT, SigIntHandler);

#if LOADLIBATRUN
	HMODULE _dM = ::LoadLibrary(_T("MultithreadedDownloadTool64"));
	if (!_dM) {
		cerr << "[FATAL] Cannot Load Library: " << LastErrorStrA() << endl;
		system("pause");
		return 1;
	}
#endif

_Begin:
	printf("\rDownload: ");
	string url; char buffer_char = 0;
	while (buffer_char = _getch()) {
		if (buffer_char == '\r' || buffer_char == '\n') break;
		if (buffer_char == 3/*Ctrl + C*/) {
			SigIntHandler(SIGINT);
			continue;
		}
		if (buffer_char == '\b') {
			if (!url.empty()) {
				printf("\b \b");
				url.erase(url.length() - 1);
			}
			continue;
		}
		if (buffer_char == 27/*ESC*/) {
			printf("\r");
			cout << string(10, ' ');
			for (size_t i = url.length(); i; --i) _putch(' ');
			url.erase(0);
			goto _Begin;
		}
		url += buffer_char;
		_putch(buffer_char);
		//printf("%d", (int)buffer_char);
	}
	cout << "\nYou will download: " << url << endl;

	string save_to("./" + url.substr(url.find_last_of('/') + 1));
_Get_saveto_filename:
	printf("\rSave to file: ");
	cout << save_to << endl;
	cout << "Do you want to change it? (y/N) ";
	while ((buffer_char = _getch()) &&
		(buffer_char != 'y' && buffer_char != 'N') &&
		(buffer_char != 'Y' && buffer_char != 'n') &&
		buffer_char != 3) _putch('\a');
	if (buffer_char == 3) { SigIntHandler(SIGINT); goto _Get_saveto_filename; }
	printf("%c\n", buffer_char);
	if (buffer_char == 'y' || buffer_char == 'Y') {
		echo "Input new file save path: ";
		char SaveFilePath[MAX_PATH + 1];
		cin.clear();
		cin.getline(SaveFilePath, MAX_PATH);
		save_to = SaveFilePath;
		goto _Get_saveto_filename;
	}

	puts("\nEverything is OK\nPress any key to download...");
	system("pause>nul");
	// /*Debug*/system("echo [Debug] Downloading...&pause");

	printf("[Debug] Creating download task... ");
	/* [Debug] - Load Library at Run*/
#if LOADLIBATRUN
	using CreateDownloadTask_t = DownloadTaskID(*)(const char*);
	CreateDownloadTask_t CreateDownloadTask = NULL;
	CreateDownloadTask = (CreateDownloadTask_t)GetProcAddress(_dM, "CreateDownloadTask");
	if (!CreateDownloadTask) return 1;
#endif
	auto tsk = CreateDownloadTask(url.c_str());
	if (!tsk) {
		cerr << "Err\n[FATAL] Cannot create download task: " << LastErrorStrA() << endl;
		system("pause");
		return GetLastError();
	}
	cout << tsk << endl;

#if LOADLIBATRUN
	using BeginDownloadTask_t = bool(*)(DownloadTaskID);
	BeginDownloadTask_t BeginDownloadTask = NULL;
	BeginDownloadTask = (BeginDownloadTask_t)GetProcAddress(_dM, "BeginDownloadTask");
	if (!BeginDownloadTask) return 1;
#endif
	printf("[Debug] Beginning download task... ");
	if (!BeginDownloadTask(tsk)) {
		cerr << "Err\n[FATAL] Cannot begin download task: " << LastErrorStrA() << endl;
		system("pause");
		return GetLastError();
	} else puts("OK");

	system("pause");

#if LOADLIBATRUN
	::FreeLibrary(_dM);
#endif

	return 0;
}

void SigIntHandler(int) {
	if (MessageBoxA(GetConsoleWindow(), "Do you want to abort?", "SIGINT",
		MB_ICONQUESTION | MB_OKCANCEL) == IDOK) ExitProcess(STATUS_CONTROL_C_EXIT);
	signal(SIGINT, SigIntHandler);
}

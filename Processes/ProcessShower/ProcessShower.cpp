#include <iostream>
#include "../../resource/tool.h"

void pid_exec(DWORD pid);
void start_exec();
int __cdecl exec__suspend(string dest);
int __cdecl exec__resume(string dest);

int main(int argc, char* argv[], char* env[]) {
	vector<Process_t::ProcessInfo> pri;
	if (argc == 2 && (argv[1][0] == 'i' || argv[1] == string("-i") ||
		argv[1] == string("--interface"))) {
		while (cls() || true) {
			Process.flush();
			DWORD pid;
			string str;
			cout << "Input process id or name\n"
				"type \"0\" to exit\n"
				"type \"\\f\" to flush process list\n"
				"type \"\\s\" to start new process\n"
				"Input on this: ";
			cin >> str;
			if (str == "0") break;
			else if (str == "\\f") {
				Process.flush(); continue;
			}
			else if (str == "\\s") {
				start_exec();
				continue;
			}
			else if (str.find(".exe") != string::npos) {
				vector<Process_t::ProcessInfo> pi;
				Process.find(s2ws(str), pi);
				if (!pi.size()) {
					cerr << "No Process!" << endl; sleep(1); continue;
				}
				if (pi.size() == 1) {
					pid = pi[0];
					pid_exec(pid);
					cls();
					continue;
				}
				cout << "Process Manager\n========\nProcess Name: " << str << endl;
				cout << "There are " << pi.size() << " processes named " << str <<
					" in your system.Please select one: " << endl;
				for (auto i : pi) {
					SetConsColor(01);
					cout << i.id();
					SetConsColor(07);
					cout << ",";
				}
				printf("\b  ");
				cout << "\nProcess ID: ";
				cin >> pid;
				if (!cin) {
					cerr << "Error DWORD!" << endl; cin.clear(); Sleep(1500); cls(); continue;
				}
				pid_exec(pid);
			}
			else if (isdigit(str[0])) {
				stringstream ss;
				ss << str; ss >> pid;
				pid_exec(pid); continue;
			}
			cls();
		}
		return 0;
	}
	if (argc > 1 && argv[1] == (string)"start") {
		if (argc == 3) {
			PROCESS_INFORMATION pi;
			pi = Process.Start(to__str(argv[2]));
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return pi.dwProcessId;
		}
		start_exec();
		return 0;
	}
	if (argc == 3 && argv[1] == (string)"kill") {
		string dest = argv[2];
		transform(dest.begin(), dest.end(), dest.begin(), tolower);
		if (isdigit(dest[0])) {
			stringstream ss; ss << dest;
			DWORD pid; ss >> pid;
			Process.kill(pid, 0);
			return 0;
		}
		else {
			vector<Process_t::ProcessInfo> pi;
			Process.find(to__str(dest), pi);
			if (!pi.size()) {
				cerr << "[E_FATAL]No process named " << dest << ".\n"; return 1;
			}
			for (auto i : pi) {
				cout << "Killing process id: " << (DWORD)i << "...\n";
				Process.kill(i, 0);
			}
			SetConsColor(01);
			cout << "Killed " << pi.size() << " process named " << dest << "." << endl;
			SetConsColor(07);
			return 0;
		}
	}
	if (argc == 3 && argv[1] == (string)"crash") {
		if (isdigit(argv[2][0])) {
			HANDLE hProcess = Process.find(atol(argv[2]));
			if (!hProcess) {
				colorset::r();
				cerr << "[ERROR] Cannot OpenProcess!!!\n"; colorset::rst(); return 1;
			}
			if (!Process.crash(hProcess, 1_b)) {
				colorset::r();
				cerr << "[ERROR] Cannot crash process!!!\n";colorset::rst(); return 2;
			}
			::CloseHandle(hProcess);
			return 0;
		}
		else {
			vector<ProcessInfo> pslist;
			Process.find(s2ws(argv[2]), pslist);
			bool ok = true;
			for (auto i : pslist) {
				ok = ok && (Process.crash(i, true) ? true : ([i] { colorset::r();
				fprintf(stderr, "[ERROR] Cannot crash process %d", (int)i);
				colorset::rst(); return false; })());
			}
			Process.CloseProcessHandles(pslist);
			if (!ok) return 1;
			return 0;
		}
	}
	if (argc == 3 && argv[1] == (string)"close") {
		if (isdigit(argv[2][0])) {
			ProcessInfo hProcess = Process.find(atol(argv[2]));
			if (!hProcess) {
				colorset::r();
				cerr << "[ERROR] Cannot Open process " << argv[2] << "!!!\n";
				colorset::rst(); return 1;
			}
			colorset::b();
			cout << "[Info]  Closing " << hProcess.id() << "...\n"; cout.flush();
			colorset::rst();
			if (!Process.CloseAllWindows(hProcess, 1_b, 10000)) {
				if (GetLastError() == 233) { colorset::r();
					cerr << "[ERROR] No visible windows found for this process\n";
					colorset::rst(); return 3;
				} colorset::r();
				fprintf(stderr, "[ERROR] Cannot close process %d's window or it was not ex"
					"ited.\n[Info]  Try {RunAsAdmin} or run \"%s kill %s\" to terminate it.\n"
					, int(hProcess.id()), argv[0], argv[2]); colorset::rst(); return 2;
			}
			else {
				colorset::g();
				printf("[OK]    %d was closed\n", int(hProcess.id()));
				colorset::rst();
			}
			::CloseHandle(hProcess);
			return 0;
		}
		else {
			vector<ProcessInfo> pslist;
			Process.find(s2ws(argv[2]), pslist);
			bool ok = true;
			cout << "There are " << pslist.size() << " " << argv[2] <<
				" in your computer.Trying to close them......" << endl;
			for (auto i : pslist) {
				colorset::b();
				cout << "[Info]  Closing process " << i.id() << "...\n"; cout.flush();
				colorset::rst();
				if (!Process.CloseAllWindows(i, 1_b, 10000)) {
					ok = false;
					if (GetLastError() == 233) {
						colorset::r();
						cerr << "[ERROR] No visible windows found for process " <<
							i.id() << "\n"; colorset::rst(); continue;
					}
					colorset::r(); fprintf(stderr, "[ERROR] Cannot close process %s's window or "
						"it was not exited.\n", argv[2]); colorset::b(); printf("[Info]  Try {RunA"
						"sAdmin} or run \"%s kill %s\" to terminate it.\n", argv[0], argv[2]); 
					colorset::rst();
				}
				else {
					colorset::g();
					printf("[OK]    %d was closed\n", int(i.id()));
					colorset::rst();
				}
			}
			if (!ok) return 1;
			return 0;
		}
	}
	if (argc == 3 && argv[1] == (string)"suspend") return exec__suspend(argv[2]);
	if (argc == 3 && argv[1] == (string)"resume") return exec__resume(argv[2]);
	if (argc == 1) {
		Process.find(pri);
		for (auto i : pri) {
			cout << ws2s(i.name()) << "|" << (DWORD)i << "|" << i.parent() << '\n';
		}
		cout.flush();
		return 0;
	}
	else {
		cerr << "Error arguments: " << GetCommandLineA() << endl;
		return 1;
	}
}

void pid_exec(DWORD pid) {
	Process_t::ProcessInfo ifo = Process.find(pid);
	if ((bool)ifo == 0) {
		cerr << "No Process!" << endl; sleep(1); return;
	}
	cls();
	cout << "Process Manager\n========\nProcess Name: ";
	SetConsColor(01);
	cout << ws2s(ifo.name());
	SetConsColor(07);
	cout << "\nProcess Id: ";
	SetConsColor(02);
	cout << ifo.id();
	SetConsColor(07);
	cout << "\nParent Process Id: " << ifo.parent() << endl;
	cout <<
		"0. Next\n"
		"1. Kill Process\n"
		"2. Suspend Process\n"
		"3. Resume Process\n"
		"4. Set to important process\n"
		"5. Crash Process\n"
		"6. Close All Windows\n"
		"Please select: ";
	char c;
	while (!isdigit(c = _getch())) { printf("\a"); }
	cout << c << endl;
	if (c == '1') {
		//unsigned ecc = 0;
		//cout << "Please input an exit code (unsigned): ";
		//cin >> ecc; 
		//Process.end(ifo, ecc);
		Process.kill(ifo, 0);
		cout << "OK." << endl;
		sleep(0.5);
		Process.flush();
	}
	else if (c == '2') {
		Process.suspend(ifo);
		cout << "OK." << endl;
		sleep(0.5);
		Process.flush();
	}
	else if (c == '3') {
		Process.resume(ifo);
		cout << "OK." << endl;
		sleep(0.5);
		Process.flush();
	}
	else if (c == '4') {
		cout << "0 - No Important\n1 - Important\n";
		char c = 0;
		while ((c = _getch()) && !isdigit(c)) printf("\a");
		ULONG key = 0; long res = 0;
		if (c == '0')
			res = Process.NtSetInformationProcess(static_cast<HANDLE>(ifo),
				(PROCESS_INFORMATION_CLASS)29, &key, sizeof(ULONG));
		else {
			key = 1;
			res = Process.NtSetInformationProcess(static_cast<HANDLE>(ifo),
				(PROCESS_INFORMATION_CLASS)29, &key, sizeof(ULONG));
		}
		if (res >= 0)cout << "OK: " << res << endl;
		else cerr << "Error: " << res << endl;
		sleep(1);
		Process.flush();
	}
	else if (c == '5') {
		Process.crash(ifo);
		cout << "OK." << endl;
		sleep(0.5);
		Process.flush();
	}
	else if (c == '6') {
		Process.CloseAllWindows(ifo);
		cout << "OK." << endl;
		sleep(0.5);
		Process.flush();
	}
	return;
}

void start_exec() {
	cls(); fflush(stdin);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	cout <<
		"|------------------------------------\n"
		"| Process Command Line : ____________\n"
		"|------------------------------------\n"
		"| <Start>                   <Cancel> \n"
		"|------------------------------------\n"
		<< endl;
	COORD crd;
	crd.X = 26;
	crd.Y = 2;
	crd.X--;
	crd.Y--;
	SetConsoleCursorPosition(hOut, crd);
	string stdstart; char ch[2047];
	cin.getline(ch, 1);
	cin.getline(ch, 2047);
	stdstart = ch;
	crd.X = 4;
	crd.Y = 4;
	crd.X--;
	crd.Y--;
	char c;
	while (1) {
		SetConsoleCursorPosition(hOut, crd);
		while ((c = _getch()) && !(c == '\t' || c == 13)) printf("\a");
		if (c == '\t') {
			if (crd.X == 3) crd.X = 29;
			else crd.X = 3;
		}
		else {
			if (crd.X == 29) return;
			PROCESS_INFORMATION pi;
			pi = Process.Start(to__str(stdstart));
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			break;
		}
	}
}

int __cdecl exec__suspend(string dest) {
	transform(dest.begin(), dest.end(), dest.begin(), tolower);
	if (dest.find(".exe") != dest.npos) {
		vector<Process_t::ProcessInfo> pi;
		Process.find(to__str(dest), pi);
		if (!pi.size()) {
			cerr << "[E_FATAL]No process named " << dest << ".\n"; return 1;
		}
		for (auto i : pi) {
			cout << "Suspending process id: " << (DWORD)i << "...\n";
			Process.suspend(i);
		}
		SetConsColor(01);
		cout << "Suspended " << pi.size() << " process named " <<
			dest << "." << endl;
		SetConsColor(07);
		return 0;
	}
	else if (isdigit(dest[0])) {
		stringstream ss; ss << dest;
		DWORD pid; ss >> pid;
		Process_t::ProcessInfo pi = Process.find(pid);
		Process.suspend(pi);
		return 0;
	}
	else {
		cerr << "Invalid parameter." << endl;
		return 1;
	}
}

int __cdecl exec__resume(string dest) {
	transform(dest.begin(), dest.end(), dest.begin(), tolower);
	if (dest.find(".exe") != dest.npos) {
		vector<Process_t::ProcessInfo> pi;
		Process.find(to__str(dest), pi);
		if (!pi.size()) {
			cerr << "[E_FATAL]No process named " << dest << ".\n"; return 1;
		}
		for (auto i : pi) {
			cout << "Resuming process id: " << (DWORD)i << "...\n";
			Process.resume(i);
		}
		SetConsColor(01);
		cout << "Resumed " << pi.size() << " process named " << dest << "." << endl;
		SetConsColor(07);
		return 0;
	}
	else if (isdigit(dest[0])) {
		stringstream ss; ss << dest;
		DWORD pid; ss >> pid;
		Process_t::ProcessInfo pi = Process.find(pid);
		Process.resume(pi);
		return 0;
	}
	else {
		cerr << "Invalid parameter." << endl;
		return 1;
	}
}

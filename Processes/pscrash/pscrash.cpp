#include <iostream>
#include "../../resource/tool.h"
using namespace std;

int main__pid(int argc, char* argv[]);

int main(int argc,char* argv[]){
	//return main__pid(argc, argv);
	if (argc < 2) {
		cout << argv[0] << " PID\n";
		return 1;
	}
	if (!isdigit(argv[1][0])) {
		vector<ProcessInfo> pslist;
		Process.find(s2ws(argv[1]), pslist);
		bool ok = true;
		for (auto i : pslist) {
			ok = ok && Process.crash(i,true);
		}
		if (!ok) return 1;
		return 0;
	}
	else {
		auto p = Process.find(atol(argv[1]));
		if (!p.operator HANDLE()) return 1;
		return Process.crash(p);
	}
}

int main__pid(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s PID\n", argv[0]);
		return 1;
	}
	HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, 0, atol(argv[1]));
	if (!p) {
		fprintf(stderr, "Cannot open process % s", argv[1]);
		return 1;
	}
	HANDLE hTh = CreateRemoteThread(p, 0, 0, 0, 0, 0, 0);
	if (!hTh) { 
		fprintf(stderr, "CreateRemoteThread(%p,0,0,0,0,0,0) failed,GetLastError() == %d", 
			p, GetLastError());
		return 1; 
	}
	CloseHandle(hTh);
	return 0;
}

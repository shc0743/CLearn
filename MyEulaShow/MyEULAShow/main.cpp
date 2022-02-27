#include <cstdlib>
#include <Windows.h>
#include "../../resource/tool.h"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <Richedit.h>
//#include <codecvt>
using namespace std;
#include "../tinyxml2/tinyxml2.h"
#include "resource.h"

//#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int main();
int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int);
int main_rungen(const wstring& Path7z);
int main_runEULA(const wstring& Path7z, const wstring& extraDir);
void ReportRuntimeError(DWORD dwStrId, LPCWSTR szReserveText, DWORD nBufferSize);

#if 1
int main() {
	/* Initial common controls */ {
		INITCOMMONCONTROLSEX icce{ 0,0 };
		icce.dwSize = sizeof(icce);
		icce.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES | 0x88FF;
		InitCommonControlsEx(&icce);
	}

	// Check compression data first
	wstring Path7z = L".7z_extra.tmp." + to_wstring(GetCurrentProcessId()) + L".exe";
	if (!FreeResFile(IDR_BIN_7zEXE, "BIN", Path7z)) {
		ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
		return ERROR_FILE_CORRUPT;
	}
	SetFileAttributesW(Path7z.c_str(), FILE_ATTRIBUTE_HIDDEN);
#if 0
	//HMODULE dll7z = LoadLibraryW(Path7z.c_str());
	//if (!dll7z) {
	//	DeleteFileW(Path7z.c_str());
	//	MessageBoxW(0, to_wstring(GetLastError()).c_str(), L"ERR", 0);
	//	return corrupt();
	//}
#endif

	DWORD dwFileType = 2;
	/* Check 7z.EXE */ {
		auto pi = Process.Start_HiddenWindow(to__str(L"\"" + Path7z +
			L"\" l \"" + s2ws(GetProgramDir()) + L"\" "));
		if (!pi.hProcess) {
			DeleteFileW(Path7z.c_str());
			ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
			return ERROR_FILE_CORRUPT;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwFileType);
		Process.CloseProcessHandle(pi);
		/* Code==0 Archived
		else Undecompressable => No archive */
	}

	if (dwFileType == 0) {
		wstring extraDir = L".{extra_" + to_wstring(GetCurrentProcessId()) + L"}.tmp";
		int result = main_runEULA(Path7z, extraDir);
		DeleteFileW(Path7z.c_str());
		Process.StartOnly_HiddenWindow(
			to__str(L"cmd.exe /d /c rd /s /q \"" + extraDir + L"\""));
		return result;
	} else {
		int res = main_rungen(Path7z);
		DeleteFileW(Path7z.c_str());
		return res;
	}

	return 0;
}
#else
int main(int argc, char* argv[]) {
	
	return 0;
}
#endif

void ReportRuntimeError(DWORD dwStrId, LPCWSTR szReserveText, DWORD nBufferSize) {
	LPWSTR err_text = (WCHAR*)calloc(nBufferSize, sizeof(WCHAR));
		/*VirtualAllocEx(NULL, NULL,
		nBufferSize * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);*/
	if (!err_text) return;
	if (dwStrId == NULL || !LoadStringW(NULL, dwStrId, err_text, nBufferSize - 1)) {
		wcscpy_s(err_text, nBufferSize, szReserveText);
	}
	if (GetConsoleWindow()) {
		//err_text[strlen(err_text)] = '\n';
		fwprintf(stderr, L"%s\n", err_text);
	}
	else {
		MessageBoxW(NULL, err_text, L"ERROR", MB_ICONHAND);
	}
	free(err_text);//VirtualFreeEx(NULL, err_text, 0, MEM_RELEASE);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	return main();
}


/* --------------------------------------------------------------- */


LRESULT CALLBACK WndProc_GenerateEula(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_ExtFiles(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_ShowEula(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static ATOM MyRegisterClass(LPCWSTR className, WNDPROC WndProc);
std::string to_utf8(const std::wstring& str);
std::string to_utf8(const wchar_t* buffer, DWORD len);
//std::string Utf8ToAnsi(const WCHAR* buffer, DWORD len);
inline bool PointInRect(POINT pt, RECT rc) {
	return (pt.x > rc.left && pt.x < rc.right) &&
		(pt.y > rc.top && pt.y < rc.bottom);
}
static HANDLE hUnzipSubProcess;
static BOOL bUnzipAborted;
static HFONT wndfont;

int main_rungen(const wstring& Path7z) {
	// Normal
	CmdLineA cl(GetCommandLineA());

	//if (cl.argc() < 2) DialogBox(0, MAKEINTRESOURCE(IDD_DIALOG_GENERATE_EULA),
	//	0, WndProc_GenerateEula);

	if (cl.argc() < 2 || (cl[1] == "--help" || cl[1] == "/?")) {
		wstring helpText = L"EULA displayer Help\n=================\n"
			"Usage:\n    EULA.exe [--help] [--generate <Arguments>]"
								" [--generate-manifest <args>]\n\n"
			"Paramters:"
			"\n--help                 Show this help document."
			"\n--generate             Generate a EULA displayer."
			"\n                       To learn more, use --generate --help."
			"\n--generate-manifest    Generate a EULA displayer."
			"\n                       To learn more, use --generate-manifest --help."
			"\n--test-extwnd          [TEST] test"
		; if (GetConsoleWindow()) {
			fwprintf(stdout, L"%s\n", helpText.c_str());
		} else {
			MessageBoxW(NULL, helpText.c_str(),
				L"EULA displayer Help Document", MB_ICONINFORMATION);
		}
		return 0;
	}

	if (cl.argc() >= 2 && (cl[1] == "--generate")) {
		if (cl.argc() == 2 || cl.getopt("help")) {
			wstring helpText = L"Usage:\n    EULA.exe --generate [--in=InFile.exe]"
				" OutFile.exe FileList"
				"\n--help                 Show this help document."
				"\n--in                   Input file"
				"\nOutFile.exe            Output file"
				"\nFileList               Files to include in OutFile.exe"
			; if (GetConsoleWindow()) {
				fwprintf(stdout, L"%s\n", helpText.c_str());
			} else {
				MessageBoxW(NULL, helpText.c_str(),
					L"EULA displayer Help Document", MB_ICONINFORMATION);
			}
			return 0;
		}
		string in = GetProgramDir(), out, files;
		cl.getopt("in", in);
		if (!file_exists(in)) {
			return ERROR_INVALID_PARAMETER;
		}
		size_t lsta = 3;
		for (size_t i = 1; i < cl.argc(); ++i) {
			if (cl[i].substr(0, 5) == "--in=") {
				lsta = i + 2; continue;
			}
			if (lsta - 1 == i) out = cl[i];
			if (i < lsta) continue;
			if (!file_exists(cl[i])) {
				cerr << "[ERROR] File not exists: " << cl[i] << endl;
				continue;
			}
			files = files + " \"" + cl[i] + "\"";
		}
		if (out.empty()) {
			return ERROR_INVALID_PARAMETER;
		}
		wstring mdfile = s2ws(out) + L".tmp." +
			to_wstring(GetCurrentProcessId()) + L".zip";

		TSTRING cml = to__str(L"\"" + Path7z + L"\" a \"" + mdfile + L"\"" + s2ws(files));
		//MessageBox(0, cml.c_str(), 0, 0);
		auto pi = Process.Start_HiddenWindow(cml);
		if (!pi.hProcess) {
			return GetLastError();
		}
		CloseHandle(pi.hThread);
		DWORD code = 1;
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hProcess);
		if (code) {
			cerr << "[FATAL] Error zipping\n"; return code;
		}
		pi = Process.Start_HiddenWindow(to__str("cmd.exe /c copy /b \"" + in + "\"+\""
			+ ws2s(mdfile) + "\" \"" + out + "\" /y"));
		if (!pi.hProcess) {
			cerr << "[FATAL] Error before copy\n"; return GetLastError();
		}
		CloseHandle(pi.hThread);
		code = 1;
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hProcess);
		DeleteFileW(mdfile.c_str());
		if (code) {
			cerr << "[FATAL] Error copying\n"; return code;
		}
		cout << "OK" << endl;

		return 0;
	}

	if (cl.argc() >= 2 && (cl[1] == "--generate-manifest")) {
		DeleteFileW(Path7z.c_str());
		if (cl.argc() == 2 || cl.getopt("help")) {
			wstring helpText = L"Usage:\n    EULA.exe --generate-manifest "
				"--eulaf=EULA.txt --exe=MAIN.exe --out=.\\out\\manifest.xml"
				"\n--help                 Show this help document."
				"\n--eulaf=EULA.txt       EULA to show"
				"\n--exe=MAIN.exe         EXE to execute after accept EULA"
				"\n--out=<OutputFile>     Output file"
			; if (GetConsoleWindow()) {
				fwprintf(stdout, L"%s\n", helpText.c_str());
			} else {
				MessageBoxW(NULL, helpText.c_str(),
					L"EULA displayer Help Document", MB_ICONINFORMATION);
			}
			return 0;
		}
		string eulaf, exe, out;
		cl.getopt("eulaf", eulaf);
		cl.getopt("exe", exe);
		cl.getopt("out", out);
		if (eulaf.empty() || exe.empty() || out.empty()) {
			fwprintf(stderr, L"[FATAL] Invalid command line!\n"
				"[INFO]  Use --help to learn more.\n");
			return ERROR_INVALID_PARAMETER;
		}
		//FILE* fp = NULL; fopen_s(&fp, out.c_str(), "wb+");
		//if (!fp) {
		//	DWORD code = GetLastError();
		//	wstring text = L"Cannot create output file (" + to_wstring(code)
		//		+ L"): " + ErrorCodeToStringW(code);
		//	ReportRuntimeError(0, text.c_str(), 1024);
		//	return code;
		//}
		if (eulaf.find("\\") != string::npos)
			eulaf = eulaf.substr(eulaf.find_last_of("\\") + 1);
		if (eulaf.find("/") != string::npos)
			eulaf = eulaf.substr(eulaf.find_last_of("/") + 1);
		if (exe.find("\\") != string::npos)
			exe = exe.substr(exe.find_last_of("\\") + 1);
		if (exe.find("/") != string::npos)
			exe = exe.substr(exe.find_last_of("/") + 1);

		using namespace tinyxml2;
		auto asp = [](const void* p) {if (p == NULL) {
			ReportRuntimeError(NULL, L"Cannot create XML", 1024);
			ExitProcess(EXCEPTION_ACCESS_VIOLATION);
		}};

		tinyxml2::XMLDocument doc;
		XMLDeclaration* dec = doc.NewDeclaration();
		asp(dec); doc.InsertFirstChild(dec);
		XMLElement* root = doc.NewElement("data");
		asp(root); doc.InsertEndChild(root);

		XMLElement* chEulaf = doc.NewElement("eula");
		asp(chEulaf); chEulaf->SetAttribute("type", "file");
		chEulaf->SetText(eulaf.c_str());
		root->InsertEndChild(chEulaf);

		XMLElement* chRun = doc.NewElement("run");
		asp(chRun); chRun->SetAttribute("op", "open");
		chRun->SetText(exe.c_str());
		root->InsertEndChild(chRun);

		auto err = doc.SaveFile(out.c_str());
		//fclose(fp);
		if (err) {
			DWORD code = GetLastError();
			wstring text = L"Cannot save output file (" + to_wstring(code)
				+ L"): " + ErrorCodeToStringW(code);
			ReportRuntimeError(0, text.c_str(), 1024);
			return code;
		}
		HANDLE fp2 = CreateFileW(s2ws(out).c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!fp2 || fp2 == INVALID_HANDLE_VALUE) {
			cerr << "[ERROR] Cannot save file as UTF-8 (2)\n";
		}
		else {
			wstring wfp3name = s2ws(out + "." + to_string(GetCurrentProcessId()) + ".tmp");
			if (0) do {
				fstream fp3_tmp(ws2s(wfp3name), ios::out | ios::binary);
				if (!fp3_tmp) break;

				unsigned char smarker[3]{ 0 };
				smarker[0] = 0xEF;
				smarker[1] = 0xBB;
				smarker[2] = 0xBF;

				fp3_tmp << smarker;
			} while (0);
			HANDLE fp3 = CreateFileW(wfp3name.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			//wfstream fp3(wfp3name, ios::/*out*/app|ios::binary);
			//FILE* fp3 = NULL; fopen_s(&fp3, ws2c(wfp3name), "wb,ccs=UTF-8");
			if (!fp3 || fp3 == INVALID_HANDLE_VALUE) {
				CloseHandle(fp2);
				cerr << "[ERROR] Cannot save file as UTF-8 (3)\n";
			} else {
				//std::locale utf8_locale(std::locale::classic(),
				//  new std::codecvt_utf8<wchar_t>);
				//fp3.imbue(utf8_locale);
				char* buffer_ANSI = (char*)calloc(32768, sizeof(char));
				wchar_t* buffer_UTF8 = (wchar_t*)calloc(32768, sizeof(wchar_t));
				if (!(buffer_ANSI && buffer_UTF8)) {
					CloseHandle(fp3); /*fp3.close();*/ /*fclose(fp3);*/
					CloseHandle(fp2);
					DeleteFileW(wfp3name.c_str());
					if (buffer_ANSI) free(buffer_ANSI);
					if (buffer_UTF8) free(buffer_UTF8);
					cerr << "[ERROR] Cannot save file as UTF-8 (4)\n";
				} else {
					wstring buffer_UNICODE;
					string buffer_temp;
					//const wchar_t* bf = NULL;
					DWORD dwBytes = 0;
					while (ReadFile(fp2, buffer_ANSI, 32767, &dwBytes, NULL) && dwBytes) {
						buffer_UNICODE = s2ws(buffer_ANSI);
						/*MultiByteToWideChar(CP_ACP, 0, buffer_ANSI, dwBytes,
							buffer_UTF8, dwBytes * 2);*/
						buffer_temp = to_utf8(
							buffer_UNICODE.c_str(), DWORD(buffer_UNICODE.length()));
						
						/*WriteFile(fp3, buffer_UNICODE.c_str(),
							DWORD(buffer_UNICODE.length() * 2), &dwBytes, NULL);*/
						//bf = buffer_UNICODE.c_str();
						//fp3.write(buffer_UTF8, buffer_UNICODE.length());
						//fp3 << buffer_UNICODE;
						//fwrite(bf, buffer_UNICODE.length(), sizeof(wchar_t), fp3);
						//fp3.write(buffer_UTF8, dwBytes);
						WriteFile(fp3, buffer_temp.c_str(),
							DWORD(buffer_temp.size()), &dwBytes, NULL);
					}
					free(buffer_ANSI); buffer_ANSI = NULL;
					free(buffer_UTF8); buffer_UTF8 = NULL;
					CloseHandle(fp3); /*fp3.close();*/ /*fclose(fp3);*/
					CloseHandle(fp2);
					DeleteFileW(s2wc(out));
					MoveFileW(wfp3name.c_str(), s2ws(out).c_str());
				}
			}
		}
		return 0;
	}

	if (cl.argc() >= 2 && (cl[1] == "--test-extwnd")) {
		HWND wSplash = NULL;
		WCHAR classn[256]{ 0 };
		LoadString(NULL, IDS_UI_EXTING_CLASS, classn, 255);
		if (classn[0] == 0) {
			ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
			return ERROR_FILE_CORRUPT;
		}
		MyRegisterClass(classn, WndProc_ExtFiles);

		wSplash = CreateWindowExW(0, classn, L"Extracting Files", WS_CAPTION |
			WS_SYSMENU, 0, 0, 240, 120, 0, 0, 0, 0);
		if (wSplash) {
			CenterWindow(wSplash);
			ShowWindow(wSplash, 1);
		}

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			 DispatchMessage(&msg);
			TranslateMessage(&msg);
		}

		return 0;
	}

	return 0;
}

int main_runEULA(const wstring& Path7z, const wstring& extraDir) {
	if (!CreateDirectoryW(extraDir.c_str(), NULL)) {
		DWORD err = GetLastError();
		if (!GetConsoleWindow()) {
			MessageBoxW(NULL, ErrorCodeToStringW(err).c_str(), L"ERROR", MB_ICONHAND);
		}
		return err;
	}
	SetFileAttributesW(extraDir.c_str(), FILE_ATTRIBUTE_HIDDEN);

	struct _wsplash { HWND* phWnd; HANDLE hEvent; };
	HWND wSplash = NULL; struct _wsplash _wspl; AutoZeroMemory(_wspl);
	/*PVOID ptrs_splash[2]{0};
	ptrs_splash[0] = &wSplash;
	ptrs_splash[1] = CreateEventW(NULL, TRUE, FALSE,
	L"Local_Event_EULA_SplashWnd_load");*/
	_wspl.phWnd = &wSplash;
	_wspl.hEvent = CreateEventW(NULL, TRUE, FALSE, L"Local_Event_EULA_SplashWnd_load");
	HANDLE hThread_splash = CreateThread(NULL, 0,
		[](PVOID ptrss)->DWORD {
		struct _wsplash ptrs = *((_wsplash*)ptrss);
		HWND* wSplash = ptrs.phWnd;
		WCHAR classn[256]{ 0 };
		LoadString(NULL, IDS_UI_EXTING_CLASS, classn, 255);
		if (classn[0] == 0) {
			ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
			return ERROR_FILE_CORRUPT;
		}
		MyRegisterClass(classn, WndProc_ExtFiles);

		*wSplash = CreateWindowExW(0, classn, L"Extracting Files", WS_CAPTION |
			WS_SYSMENU, 0, 0, 240, 120, 0, 0, 0, 0);
		if (wSplash) {
			CenterWindow(*wSplash);
			ShowWindow(*wSplash, 1);
		}
		if (ptrs.hEvent) SetEvent(HANDLE(ptrs.hEvent));

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			 DispatchMessage(&msg);
			TranslateMessage(&msg);
		}

		return 0;
	}, &_wspl, CREATE_SUSPENDED, 0);
	if (hThread_splash) {
		ResumeThread(hThread_splash);
		CloseHandle(hThread_splash);
		hThread_splash = NULL;
		if (_wspl.hEvent) {
			WaitForSingleObject(_wspl.hEvent, INFINITE);
			ResetEvent(_wspl.hEvent);
			CloseHandle(_wspl.hEvent);
		}
	}

	SetCurrentDirectoryW(extraDir.c_str());
	/* UNZIP */ {
		DWORD exitCode = 0;
		STARTUPINFO si; AutoZeroMemory(si);
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		auto pi = Process.Start_Suspended(to__str(L"\"" + Path7z +
			L"\" x \"" + s2ws(GetProgramDir()) + L"\" -y"), si);
		if (!pi.hProcess) {
			ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
			SetCurrentDirectoryW(L"..");
			return ERROR_FILE_CORRUPT;
		}
		::hUnzipSubProcess = pi.hProcess;
		Process.resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitCode);
		Process.CloseProcessHandle(pi);
		::hUnzipSubProcess = NULL;
	}
	//Sleep(10000);

	if (::bUnzipAborted) {
		SetCurrentDirectoryW(L"..");
		return ERROR_CANCELLED;
	}
	if (wSplash) SendMessage(wSplash, WM_USER + 4, 0, 0);

	if (!file_exists("manifest.xml")) {
		ReportRuntimeError(IDS_APP_ERR_NO_MANIFEST, L"Required file is missing.", 1024);
		SetCurrentDirectoryW(L"..");
		return ERROR_FILE_NOT_FOUND;
	}

	CmdLineA cl(GetCommandLineA());
	string eula_file, main_run, main_op; {
		auto e = [] {
			ReportRuntimeError(IDS_APP_ERR_BAD_MANIFEST, L"manifest.xml error.", 1024);
			SetCurrentDirectoryW(L"..");
			return ERROR_FILE_INVALID; 
		};
#if 0
		do {
			//FILE* wfs = NULL; fopen_s(&wfs, "manifest.xml", "rb,ccs=UTF-8");
			//if (!wfs) break;
			HANDLE fp2 = CreateFileW(L"manifest.xml", GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!fp2 || fp2 == INVALID_HANDLE_VALUE) {
				return e();
			}
			HANDLE fp3 = CreateFileW(L"manifest.ANSI.xml", GENERIC_READ | GENERIC_WRITE, 0,
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!fp3 || fp3 == INVALID_HANDLE_VALUE) {
				CloseHandle(fp2);
				return e();
			}
			WCHAR* buffer_UTF8 = (WCHAR*)calloc(32768, sizeof(char));
			if (!(buffer_UTF8)) {
				CloseHandle(fp3);
				CloseHandle(fp2);
				//if (buffer_ANSI) free(buffer_ANSI);
				//if (buffer_UTF8) free(buffer_UTF8);
				return e();
			}
			wstring buffer_UNICODE;
			string buffer_temp;
			//const wchar_t* bf = NULL;
			DWORD dwBytes = 0;
			while (ReadFile(fp2, buffer_UTF8, 32767, &dwBytes, NULL) && dwBytes) {
				//buffer_UNICODE = buffer_UTF8;
				//buffer_temp = Utf8ToAnsi(ws2c(buffer_UTF8), dwBytes);
				buffer_temp = Utf8ToAnsi(
					buffer_UNICODE.c_str(), DWORD(buffer_UNICODE.length()));
				MessageBoxA(NULL, buffer_temp.c_str(), "debug", 0);
				WriteFile(fp3, buffer_temp.c_str(),
					DWORD(buffer_temp.size()), &dwBytes, NULL);
			}
			free(buffer_UTF8); buffer_UTF8 = NULL;
			//free(buffer_UTF8); buffer_UTF8 = NULL;
			CloseHandle(fp3); /*fp3.close();*/ /*fclose(fp3);*/
			CloseHandle(fp2);
			/* fstream fso("manifest.ANSI.xml", ios::out);
			WCHAR wchtemp[2048]{ 0 };
			 CHAR cchtemp[2048]{ 0 };
			while (fread(wchtemp,sizeof(WCHAR), 2048,wfs)) {
				//Utf8ToAnsi(ws2c(wchtemp), cchtemp, 2048);
				Utf8ToAnsi(wchtemp, DWORD(wcslen(wchtemp) - 1));
				fso.write(cchtemp, strlen(cchtemp) - 1);
			}
			fclose(wfs); fso.close();*/
		} while (0);
		//FILE* wfs = NULL; fopen_s(&wfs, "manifest.xml", "rb,ccs=UTF-8");
		//if (!wfs) return e();
#endif
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile("manifest.xml"/*wfs*/)) return e();
		//fclose(wfs);
		tinyxml2::XMLElement* root = doc.RootElement();
		if (!root) return e();
		tinyxml2::XMLElement* eula = root->FirstChildElement("eula");
		if (!eula) return e();
		if (eula->Attribute("type") &&
			0 == _stricmp(eula->Attribute("type"), "file")) {
			eula_file = eula->GetText();
		}
		tinyxml2::XMLElement* run = root->FirstChildElement("run");
		if (!run) return e();
		main_op = run->Attribute("op") || "open";
		main_run = run->GetText() || "";
	}
	if (eula_file.empty() || main_run.empty() || main_op.empty()) {
		ReportRuntimeError(IDS_APP_ERR_BAD_MANIFEST, L"manifest.xml error.", 1024);
		SetCurrentDirectoryW(L"..");
		return ERROR_FILE_INVALID; 
	}
	//MessageBoxA(NULL, eula_file.c_str(), "eula_file", 0);
	//MessageBoxA(NULL, main_run.c_str(), "main_run", 0);
	//MessageBoxA(NULL, main_op.c_str(), "main_op", 0);

	WCHAR wnd_class_name[64]{ 0 };
	LoadString(NULL, IDS_UI_EULA_CLASS, wnd_class_name, 63);
	if (wnd_class_name[0] == 0) {
		ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
		SetCurrentDirectoryW(L"..");
		return ERROR_FILE_CORRUPT;
	}
	MyRegisterClass(wnd_class_name, WndProc_ShowEula);

	DWORD wnd_code = 0;
	auto wnd_message_loop = [](PVOID) -> DWORD {
		LoadLibrary(TEXT("Msftedit.dll"));
		WCHAR wnd_class_name[64]{ 0 };
		LoadString(NULL, IDS_UI_EULA_CLASS, wnd_class_name, 63);
		if (wnd_class_name[0] == 0) {
			ReportRuntimeError(IDS_APP_CORRUPT, L"This file is corrupt.", 256);
			SetCurrentDirectoryW(L"..");
			return ERROR_FILE_CORRUPT;
		}

		wndfont = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		HWND hWnd = CreateWindowExW(0, wnd_class_name, L"End User Licence Agreement",
			WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
			0, 0, 500, 400, NULL, 0, 0, 0);
		if (!hWnd) return ERROR_CREATE_FAILED;
		CenterWindow(hWnd);
		ShowWindow(hWnd, SW_NORMAL);

		MSG msg{ 0 };
		while (GetMessage(&msg, nullptr, 0, 0)) {
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
		DeleteObject(wndfont);
		return DWORD(msg.wParam);
	};
	HANDLE hThread_wnd = CreateThread(NULL, 0, wnd_message_loop, NULL, CREATE_SUSPENDED, 0);
	if (!hThread_wnd) {
		ReportRuntimeError(NULL, L"Cannot create window!\n"
			"Please try to reopen this program.", 64);
		return GetLastError();
	}
	ResumeThread(hThread_wnd);
	WaitForSingleObject(hThread_wnd, INFINITE);
	GetExitCodeThread(hThread_wnd, &wnd_code);
	int retValue = 0;
	if (wnd_code == IDOK) {
		// Execute it
		wstring lpVerb = s2ws(main_op);
		wstring lpFile = s2ws(main_run);
		SHELLEXECUTEINFOW sh{ 0 };
		sh.cbSize = sizeof(sh);
		sh.hwnd = NULL;
		sh.lpVerb = lpVerb.c_str();
		sh.lpFile = lpFile.c_str();
		sh.lpParameters = NULL;
		sh.lpDirectory = NULL;
		sh.nShow = SW_NORMAL;
		sh.fMask = SEE_MASK_NOCLOSEPROCESS;
		if (ShellExecuteExW(&sh)) {
			WaitForSingleObject(sh.hProcess, INFINITE);
			GetExitCodeProcess(sh.hProcess, &wnd_code);
			retValue = (int)wnd_code;
		}
		else retValue = (int)GetLastError();
	} else {
		retValue = 1;
	}


	// Exit and clean
	SetCurrentDirectoryW(L"..");
	return retValue;
}

ATOM MyRegisterClass(LPCWSTR className, WNDPROC WndProc) {
#pragma warning(push)
#pragma warning(disable: 4302)
	WNDCLASSEXW wcex; AutoZeroMemory(wcex);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);

#pragma warning(pop)
}

LRESULT WndProc_GenerateEula(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		//CenterWindow(hDlg);
		return TRUE;
		break;

	case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			switch (wmId) {
			case IDOK:
				// do something...
			case IDCANCEL:
				EndDialog(hDlg, 0);
				break;
			default:
				return DefWindowProc(hDlg, msg, wParam, lParam);
			}
		}
		break;

	default:
		return DefWindowProc(hDlg, msg, wParam, lParam);
	}
	return 0;
}

LRESULT WndProc_ExtFiles(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		CreateWindowExW(0, L"Static", L"Please wait while the\nprogram is"
			" extracting files...", WS_VISIBLE | WS_CHILD | SS_CENTER,
			10, 6, 205, 36, hWnd, (HMENU)(11), 0, 0);
		CreateWindowExW(0, L"Button", L"&Cancel", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			125, 48, 90, 25, hWnd, (HMENU)(IDCANCEL), 0, 0);
		HWND wProg = CreateWindowExW(0, PROGRESS_CLASSW, L"", WS_VISIBLE | WS_CHILD
			| PBS_MARQUEE, 10, 48, 105, 25, hWnd, (HMENU)(12), 0, 0);
		if (wProg) SendMessage(wProg, PBM_SETMARQUEE, TRUE, 0);
	}
		break;

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDCANCEL:
			::bUnzipAborted = TRUE;
			if (::hUnzipSubProcess) TerminateProcess(::hUnzipSubProcess, 1);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			/*TextOutW(hdc, 40, 5, L"Please wait while the", 21);
			TextOutW(hdc, 20, 25, L"program is extracting files...", 30);*/
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:
		if (MessageBoxW(hWnd, L"Are you sure you want to abort?",
			L"Abort - Extracting Files", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			SendMessage(hWnd, WM_COMMAND, IDCANCEL, BN_CLICKED);
		break;

	case WM_USER+4:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

typedef struct {
	HWND wS1, wS2, wEula, wOk, wCancel;
} Wnds_ShowEula_t;
static std::map<HWND, Wnds_ShowEula_t> Wnds_ShowEula;
LRESULT WndProc_ShowEula(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		Wnds_ShowEula_t w{ 0 };
		w.wS1 = CreateWindowExW(0, L"Static", L"End User Licence Agreement",
			WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_BORDER,
			0, 0, 1, 1, hWnd, (HMENU)(11), 0, 0);
		w.wS2 = CreateWindowExW(0, L"Static", L"Do you accept the "
			"End User Licence Agreement?",
			WS_VISIBLE | WS_CHILD | SS_CENTER,
			0, 0, 1, 1, hWnd, (HMENU)(12), 0, 0);
		w.wEula = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
			ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP
			| ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL,
			0, 0, 1, 1, hWnd, (HMENU)(13), NULL, NULL);
		w.wCancel = CreateWindowExW(0, L"Button", L"I don't accept",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			0, 0, 1, 1, hWnd, (HMENU)(IDCANCEL), 0, 0);
		w.wOk = CreateWindowExW(0, L"Button", L"I Accept",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			0, 0, 1, 1, hWnd, (HMENU)(IDOK), 0, 0);

		if (wndfont) {
			if (w.wS1) SendMessage(w.wS1, WM_SETFONT, (WPARAM)wndfont, 0);
			if (w.wS2) SendMessage(w.wS2, WM_SETFONT, (WPARAM)wndfont, 0);
			if (w.wEula) SendMessage(w.wEula, WM_SETFONT, (WPARAM)wndfont, 0);
			if (w.wCancel) SendMessage(w.wCancel, WM_SETFONT, (WPARAM)wndfont, 0);
			if (w.wOk) SendMessage(w.wOk, WM_SETFONT, (WPARAM)wndfont, 0);
		}

		Wnds_ShowEula[hWnd] = w;

		LONG wlong = 0;
		wlong = GetWindowLong(hWnd, GWL_EXSTYLE);
		wlong |= WS_EX_LAYERED;
		SetWindowLong(hWnd, GWL_EXSTYLE, wlong);
		SetLayeredWindowAttributes(hWnd, NULL, BYTE((255) * (0.9)), LWA_ALPHA);
	}
		break;

	case WM_SIZING:
	case WM_SIZE:
	{
		RECT rc{ 0 }; GetWindowRect(hWnd, &rc);
		LONG ww = rc.right - rc.left, wh = rc.bottom - rc.top;
		auto& w = Wnds_ShowEula[hWnd];
		SetWindowPos(w.wS1, 0,      10, 10,             ww - 35, 20, SWP_NOZORDER);
		SetWindowPos(w.wS2, 0,      10, 40,             ww - 35, 20, SWP_NOZORDER);
		SetWindowPos(w.wEula, 0,    10, 70,             ww - 35, wh - 155, SWP_NOZORDER);
		SetWindowPos(w.wOk, 0,      ww - 236, wh - 75,  80, 25, SWP_NOZORDER);
		SetWindowPos(w.wCancel, 0,  ww - 146, wh - 75,  120, 25, SWP_NOZORDER);
	}
		break;

	/*case WM_NCHITTEST:
	{
		auto v = DefWindowProc(hWnd, msg, wParam, lParam);
		if (v == HTCLIENT) {
			RECT rc{ 0 };
			GetWindowRect(Wnds_ShowEula[hWnd].wS1, &rc);
			//POINTS pts = MAKEPOINTS(lParam); // position of cursor
			POINT pt{ 0 }; GetCursorPos(&pt);
			if (PointInRect(pt, rc)) v = HTCAPTION;
		}
		return v;
	}
		break;*/

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDOK:
			PostQuitMessage(IDOK);
			break;
		case IDCANCEL:
			PostQuitMessage(IDCANCEL);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:
	case WM_USER+4:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

/*HWND CreateRichEdit(HWND hwndOwner,        // Dialog box handle.
	int x, int y,          // Location.
	int width, int height, // Dimensions.
	HINSTANCE hinst)       // Application or DLL instance.
{
	LoadLibrary(TEXT("Msftedit.dll"));

	HWND hwndEdit = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Type here"),
		ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
		x, y, width, height,
		hwndOwner, NULL, hinst, NULL);

	return hwndEdit;
}*/

/* https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/ */
std::string to_utf8(const wchar_t* buffer, DWORD len) {
	int nChars = ::WideCharToMultiByte(CP_UTF8,0,buffer,len,NULL,0,NULL,NULL);
	if (nChars == 0) return "";

	string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8,0,buffer,len,
		const_cast<char*>(newbuffer.c_str()),
		nChars,	NULL,NULL);

	return newbuffer;
}
std::string to_utf8(const std::wstring& str) {
	return to_utf8(str.c_str(), DWORD(str.size()));
}

//std::string Utf8ToAnsi(const WCHAR* buffer, DWORD len) {
//	//int nChars = ::MultiByteToWideChar(
//	//	CP_UTF8,
//	//	0,
//	//	buffer,
//	//	len,
//	//	NULL,
//	//	0);
//	//if (nChars == 0) return L"";
//	//
//	//wstring newbuffer;
//	//newbuffer.resize(nChars);
//	//::MultiByteToWideChar(
//	//	CP_UTF8,
//	//	0,
//	//	buffer,
//	//	len,
//	//	const_cast<WCHAR*>(newbuffer.c_str()),
//	//	nChars);
//	//
//	//return newbuffer;
//	int nChars = ::WideCharToMultiByte(CP_ACP,0,buffer,len,NULL,0,NULL,NULL);
//	if (nChars == 0) return "";
//
//	string newbuffer;
//	newbuffer.resize(nChars);
//	::WideCharToMultiByte(
//		CP_ACP,0,buffer,len,
//		const_cast<char*>(newbuffer.c_str()),
//		nChars,	NULL,NULL);
//
//	return newbuffer;
//}


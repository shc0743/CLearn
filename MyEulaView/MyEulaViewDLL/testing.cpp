#include "cppwin_include.h"
#include "../libs/tinyutf8.h"
#include "resource.h"
#include "../../resource/tool.h"
#include "strutil.h"
#include "EulaHtmlViewer.h"



extern HINSTANCE hInst;

BOOL WINAPI EulaView_HTML(PMYEULAVIEWDATA data);

DWORD __stdcall DLL_test(ULONGLONG test_id) {
	switch (test_id) {
	case 1:
		std::cout << "Hello world!" << std::endl;
		break;

	case 2:
		
		break;

	case 0xc08: {
#pragma warning(push)
#pragma warning(disable: 4566)
		tiny_utf8::u8string a = u8"test_中文··。、{}\u0080\u206000";
		tiny_utf8::utf8_string b = "test_中文··。、{}\u0080\u206000";
		tiny_utf8::string c = "test_中文··。、{}\u0080\u206000";
#pragma warning(pop)
		std::fstream fp;
		auto openway = std::ios::out | std::ios::binary;
		fp.open("test_0xc08_1.tmp.txt", openway);
		if (fp) {
			fp.write((PSTR)a.c_str(), a.size()); // utf8
			fp.close();
		}
		fp.open("test_0xc08_2.tmp.txt", openway);
		if (fp) {
			fp.write((PSTR)b.c_str(), b.size()); // ANSI
			fp.close();
		}
		fp.open("test_0xc08_3.tmp.txt", openway);
		if (fp) {
			fp.write((PSTR)c.c_str(), c.size()); // ANSI
			fp.close();
		}
		break;
	}

	case 0xc09: {
#pragma warning(push)
#pragma warning(disable: 4566)
		std::string a = "test_中文··。、{}\u0080\u206000";
#pragma warning(pop)
		std::fstream fp("test_0xc09_1.tmp.txt", std::ios::out | std::ios::binary);
		if (fp) {
			fp.write((PSTR)str_to_u8str(a).c_str(), a.size());
			fp.close();
		}
		break;
	}

	case 0xfe2: {
		MYEULAVIEWDATA data{};
		data.cb = sizeof(data);
		data.dwTimesToAccept = 3;
		wcscpy_s(data.szTitle, L"DEMO English 中文 ✔😊™™☸♂※ test");
		data.position.usePosition = true;
		data.position.x = data.position.y = 100;
		data.position.width = 1024;
		data.position.height = 768;

		OPENFILENAMEW fn{};
		fn.lStructSize = sizeof(fn);
		fn.hwndOwner = NULL;
		fn.lpstrFile = data.szFile;
		fn.lpstrFile[0] = L'\0';
		fn.nMaxFile = MAX_PATH;
		fn.lpstrFilter = L"HTML Files (*.htm;*.html)\0*.htm;*.html\0All Files\0*.*\0";
		fn.nFilterIndex = 1;
		fn.lpstrFileTitle = NULL;
		fn.nMaxFileTitle = 0;
		fn.lpstrInitialDir = NULL;
		fn.lpstrTitle = L"Choose files to test";
		fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (!GetOpenFileNameW(&fn)) {
			return ERROR_CANCELLED;
		}

		return EulaView_HTML(&data);
		break;
	}

	default:
		return ERROR_INVALID_PARAMETER;
	}
	return 0;
}
void CALLBACK DLL_test_RunDLL(HWND _hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow) {
	AllocConsole();
	HANDLE
		in = GetStdHandle(STD_INPUT_HANDLE),
		out = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD n = 0;
	CHAR buffer[256] = "Input test code: ";
	std::string str;
	WriteConsoleA(out, buffer, (DWORD)strlen(buffer), &n, NULL);
	ZeroMemory(buffer, 256);
	CmdLineA cl(lpCmdLine);
	if (cl.getopt("t", str) == 1) {
		strcpy_s(buffer, str.c_str());
		WriteConsoleA(out, str.c_str(), (DWORD)str.length() + 1, &n, NULL);
		WriteConsoleA(out, "\n", 1, &n, NULL);
	} else {
		ReadConsoleA(in, buffer, 255, &n, NULL);
	}
	ULONGLONG code = atoll(buffer);
	std::string result = "Return: " + std::to_string(DLL_test(code)) + "\n";
	strcpy_s(buffer, result.c_str());
	WriteConsoleA(out, buffer, (DWORD)strlen(buffer), &n, NULL);
	strcpy_s(buffer, "Press any key to continue...\n");
	WriteConsoleA(out, buffer, (DWORD)strlen(buffer), &n, NULL);
	(void)_getch();
}


bool __stdcall DLL_ReleaseLicenses(PCWSTR path) {
	return FreeResFile(IDR_BIN_LICENSEs, "BIN", path, hInst);
}


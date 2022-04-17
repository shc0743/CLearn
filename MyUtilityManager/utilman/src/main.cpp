#include "framework.h"
#include "../../../resource/tool.h"

#include "utilman.process.hpp"

using namespace std;


// Enable visual elements
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Unhandled Exception Filter
LONG WINAPI MyTopLevelExceptionFliter(
	_In_ PEXCEPTION_POINTERS ExceptionInfo
);

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
	SetUnhandledExceptionFilter(MyTopLevelExceptionFliter);

	STARTUPINFO startupinfo{};
	GetStartupInfo(&startupinfo);
	CmdLineW cl(GetCommandLineW());

	if (cl.getopt(L"type")) {
		wstring type;
		cl.getopt(L"type", type);
		try {
			return SubProcess_type_handler(type, cl);
		}
		catch (SubProcess_type_ValueError) {
			// Invalid paramter
			return ERROR_INVALID_PARAMETER;
		}
	}


	return SubProcess_root(cl);

#if 0
	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
#else
	return ERROR_INVALID_PARAMETER;
#endif
}

LONG WINAPI MyTopLevelExceptionFliter(_In_ PEXCEPTION_POINTERS ExceptionInfo) {
	MessageBoxW(NULL, L"Unhandled Exception.\nClick [OK] to terminate process.",
		NULL, MB_ICONERROR);
	return EXCEPTION_CONTINUE_SEARCH;
}


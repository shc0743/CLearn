#include <iostream>
#include <Windows.h>
#include "../../resource/tool.h"
#include "MyProcControl-service.h"
#include "MyProcControl-ui.h"
#include "resource.h"
using namespace std;

int main(int argc, char* argv[]) {
	CmdLineW cl(GetCommandLineW());

	if (argc < 2) {
		return !Process.StartOnly(to__str(
			"\"" + GetProgramDir() + "\" --ui --install --default"));
	}

	if (cl.getopt(L"service")) {
		wstring svc_name;
#if 0
		if (cl.getopt(L"sub-process")) {
			if (cl.getopt(L"parent-service", svc_name) != 1)
				return ERROR_INVALID_PARAMETER;
			FreeConsole();
			HANDLE hThread_main = CreateThread(NULL, 0, ServiceWorker_sub_process,
				(PVOID)svc_name.c_str(), CREATE_SUSPENDED, 0);
			if (!hThread_main) return GetLastError();
			DWORD dwExitCode = 0;
			ResumeThread(hThread_main);
			WaitForSingleObject(hThread_main, INFINITE);
			GetExitCodeThread(hThread_main, &dwExitCode);
			CloseHandle(hThread_main);
			ExitProcess((UINT)dwExitCode);
			return (int)dwExitCode;
		}
#endif
		if (cl.getopt(L"consent")) {
			FreeConsole();
			wstring type, svcname;
			cl.getopt(L"type", type);
			cl.getopt(L"svc-name", svcname);
			if (type.empty() || (type != L"stop" && type != L"pause"))
				return ERROR_INVALID_PARAMETER;
			wstring title, text;
			if (!svcname.empty()) title = L"["s + svcname + L"] - ";
			title += L"Service Control Confirm";
			text = L"Are you sure you want to ";
			text += type + L" the service";
			if (!svcname.empty()) text += L" [" + svcname + L"]";
			text += L"?\n\nThis operation will reduce your computer protection.";
			if (!cl.getopt(L"notimeout")) text +=
				L"\n\n\n* The default operation will be choose after 5 seconds.";
			RegClass_BackgroundLayeredAlphaWindowClass();
			//MessageBoxW(NULL, LastErrorStrW().c_str(), 0, 0);
			HWND hwnd_backg = CreateWindowExW(WS_EX_LAYERED,
				L"BackgroundLayeredAlphaWindowClass", L"",
				WS_OVERLAPPED | WS_SYSMENU,
				0, 0, 1, 1, 0, 0, 0, 0);
			//MessageBoxW(NULL, LastErrorStrW().c_str(), 0, 0);
			ShowWindow(hwnd_backg, SW_NORMAL);
			SetForegroundWindow(hwnd_backg);
			int val = MessageBoxTimeoutW(hwnd_backg, text.c_str(), title.c_str(),
				MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING | MB_SETFOREGROUND | MB_SYSTEMMODAL
				, 0, cl.getopt(L"notimeout") ? 0 : 5000);
			DestroyWindow(hwnd_backg);
			return -val;
			return ERROR_INVALID_PARAMETER;
		}
		if (cl.getopt(L"run-svc-name", svc_name) != 1)
			return ERROR_INVALID_PARAMETER;
		global_SvcObj = new ServiceWorker_t;
		wcscpy_s(global_SvcObj->ServiceName, svc_name.c_str());
		int ret = !MyStartAsServiceW(svc_name, global_SvcObj->ServiceLaunch);
		delete global_SvcObj;
		return ret;
	}

	if (cl.getopt(L"install")) {

		if (cl.getopt(L"ui")) {
			FreeConsole();
			if (cl.getopt(L"default")) {
				WCHAR wclass_icon[256]{ 0 };
				LoadStringW(0, IDS_UI_ICONWND_CLASS, wclass_icon, 255);
				if (wclass_icon[0] == 0) return ERROR_FILE_CORRUPT;
				HWND hw = FindWindowW(wclass_icon, NULL);
				if (hw) {
					SendMessage(hw, WM_USER + 13, 0, WM_LBUTTONUP);
					return 0;
				}
			}

			WCHAR wclass[256]{ 0 };
			LoadStringW(0, IDS_UI_INSTALL_WND_CLASS, wclass, 256);
			WNDCLASSEXW wcex{ 0 }, wcex2{ 0 };
			wcex.cbSize = sizeof(wcex);
			GetClassInfoExW(0, L"#32770", &wcex);
			wcex2.hbrBackground = wcex.hbrBackground;
			s7::MyRegisterClassW(wclass, WndProc_SetupWindow, wcex2);
			HWND hwnd = CreateWindowExW(0, wclass, L"MyProcControl Setup",
				WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				0, 0, 300, 150, 0, 0, 0, 0);
			if (!hwnd) return 1;
			CenterWindow(hwnd);
			ShowWindow(hwnd, SW_NORMAL);

			MSG msg{ 0 };
			while (GetMessage(&msg, 0, 0, 0)) {
				DispatchMessage(&msg);
				TranslateMessage(&msg);
			}
			return (int)msg.wParam;
		} // if (cl.getopt(L"ui"))

		wstring svc_name, dsp_name, des_text, cfg_path, startup_type;
		cl.getopt(L"svc-name", svc_name);
		cl.getopt(L"display-name", dsp_name);
		cl.getopt(L"description", des_text);
		cl.getopt(L"config", cfg_path);
		cl.getopt(L"startup-type", startup_type);
		if (svc_name.empty()) return ERROR_INVALID_PARAMETER;
		if (dsp_name.empty()) dsp_name = svc_name;
		if (des_text.empty()) {
			WCHAR desctext[2048]{ 0 };
			LoadStringW(NULL, IDS_SVC_DESCRIPTION, desctext, 2047);
			des_text = desctext;
		}
		wstring cmdl = L"\"" + s2ws(GetProgramDir()) + L"\" --service"
			" --run-svc-name=\"" + svc_name + L"\"";
		if (!cfg_path.empty()) cmdl += L" --config=\""s + cfg_path + L"\"";
#pragma warning(push)
#pragma warning(disable: 26812)
		if (cl.getopt(L"start")) {
			ServiceManager.Start(ws2s(svc_name));
		}
		UINT err = ServiceManager.New(svc_name, cmdl,
			(startup_type == L"auto" ? ServiceManager_::Auto :
				(startup_type == L"disabled" ?
					ServiceManager_::Disabled : ServiceManager_::Manual)),
			dsp_name, des_text, SERVICE_WIN32_OWN_PROCESS);
#pragma warning(pop)
		if (err) {
			fprintf(stderr, "ERROR: Error code %u\n", err);
			return 1;
		}
		puts("OK");
		if (cl.getopt(L"start")) {
			ServiceManager.Start(ws2s(svc_name));
		}
		return 0;
	}

	if (cl.getopt(L"ui")) {
		FreeConsole();
		STARTUPINFOW si{ 0 };
		si.cb = sizeof(si);
		GetStartupInfoW(&si);

		wstring svc_name;
		cl.getopt(L"service-name", svc_name);
		if (cl.getopt(L"from-service")) {
			HANDLE h = CreateThread(0, 0, MyUiWorker_parentWatchdog, 0, 0, 0);
			if (h) CloseHandle(h);
			WCHAR wclass[256]{ 0 };
			LoadStringW(0, IDS_UI_SVCCTLWINDOW_CLASS, wclass, 256);
			s7::MyRegisterClassW(wclass, WndProc_SvcCtrlWindow);
			CreateWindowExW(0, wclass, svc_name.empty() ?
				L"Service Control Sign Receive Window" : svc_name.c_str(),
				0, 0, 0, 1, 1, 0, 0, 0, 0);
		}

		if (((si.dwFlags & STARTF_USESHOWWINDOW) &&
			(si.wShowWindow == SW_HIDE)) || cl.getopt(L"hidden")) {
			WCHAR wclass_icon[256]{ 0 };
			LoadStringW(0, IDS_UI_ICONWND_CLASS, wclass_icon, 255);
			s7::MyRegisterClassW(wclass_icon, WndProc_TrayIconWindow);
			HWND hIconWindow = CreateWindowExW(0, wclass_icon, svc_name.empty() ?
				L"Icon Window" : svc_name.c_str(),
				0, 0, 0, 1, 1, 0, 0, 0, 0);
			if (!svc_name.empty()) SendMessage(hIconWindow,
				WM_USER + 15, (WPARAM)svc_name.c_str(), 0);

			MSG msg{ 0 };
			while (GetMessage(&msg, 0, 0, 0)) {
				DispatchMessage(&msg);
				TranslateMessage(&msg);
			}
			return (int)msg.wParam;
		}

		// @deprecated This UI is deprecated. Please use MFCMyProcCtlUI
		//// TODO: Create application window
		/*HWND hwnd_main = CreateDialog(NULL, MAKEINTRESOURCE(
			IDD_DIALOG_UI_MAIN), NULL, WndProc_Dlg_Main);
		if (!hwnd_main) return GetLastError();
		CenterWindow(hwnd_main);
		ShowWindow(hwnd_main, SW_NORMAL);*/

		MSG msg{ 0 };
		while (GetMessage(&msg, 0, 0, 0)) {
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
		return (int)msg.wParam;
	}

	if (cl.getopt(L"EndUserInterfaceInstances")) {
		wstring svc_name;
		cl.getopt(L"service-name", svc_name);
		HWND hWnd_uiproc = NULL;
		WCHAR wclass[256]{ 0 };
		LoadString(NULL, IDS_UI_SVCCTLWINDOW_CLASS, wclass, 255);
		while ((hWnd_uiproc = FindWindowW(wclass,
			svc_name.empty() ? NULL : svc_name.c_str())) != NULL) {
			SendMessage(hWnd_uiproc, WM_USER + 44, 0, 0);
		}
		LoadString(NULL, IDS_UI_MFCUI_CLASS, wclass, 255);
		while ((hWnd_uiproc = FindWindowW(wclass, NULL)) != NULL) {
			SendMessage(hWnd_uiproc, WM_CLOSE, 0, 0);
		}
		return 0;
	}

	return ERROR_INVALID_PARAMETER;
	return 0;
}

#if 0
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if (GetProgramInfo().name == "timeout.exe") {
			LPTHREAD_START_ROUTINE pRemoteFuncAddr = (LPTHREAD_START_ROUTINE)
				GetProcAddress(hModule, "ServiceWorker_subpentry");
			if (pRemoteFuncAddr) {
				HANDLE hRemoteTrd = CreateThread(0, 0, pRemoteFuncAddr,
					(LPVOID)(INT_PTR)GetCurrentProcessId(), CREATE_SUSPENDED, 0);
				if (hRemoteTrd) {
					ResumeThread(hRemoteTrd);
					CloseHandle(hRemoteTrd);
				}
			}
		}
    }
    return TRUE;
}
#endif


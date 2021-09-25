/*     
十年生死两茫茫，写程序，到天亮，
千行代码，Bug何处藏。
纵使上线又怎样，朝令改，夕断肠。
领导每天新想法，天天改，日日忙。
相顾无言，惟有泪千行。
每晚灯火阑珊处，程序员，加班狂。
*/
#pragma once
#ifndef __cplusplus
#error "tool_ui.h is a C++ header file"
#endif
#if (!(defined(_TOOL_SHC0743_MAIN)))
#include "tool_core.h"
#endif

#if (!(defined(_TOOL_SHC0743_UI)))
#define _TOOL_SHC0743_UI

namespace mt_ui2 {
	using namespace std;

	constexpr LPCWSTR window_class_name = L"TsUiLib_ui2";
	HINSTANCE hInstance = NULL;

	ATOM MyRegisterClass(HINSTANCE);
	bool UiInit();

	class UiBase {
	protected:
		HWND m_hWnd;
		struct {
			long x, y;
			long w, h;
		} WindowFlag;

	protected:
		UiBase();
		~UiBase();

		friend ATOM MyRegisterClass(HINSTANCE);

		bool InitInstance(HINSTANCE, int);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		WPARAM MessageLoop();
	};

	class DlgBase : public UiBase {
	protected:
		DlgBase();
		~DlgBase();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	};

	class InputDlg : public DlgBase {
	public:
		DlgBase();
		~DlgBase();
	};
};

#endif // defined(_TOOL_SHC0743_UI)

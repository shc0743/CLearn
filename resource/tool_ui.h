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

	constexpr LPCTSTR window_class_name = _T("TsUiLib.ui2.base");
	extern HINSTANCE hInstance;

	ATOM MyRegisterClass(HINSTANCE);
	bool UiInit();

	class UiBase {
	protected:
		HWND m_hWnd;
		string m_uuid;
		STRING title;
		struct {
			long x, y;
			long w, h;
		} WindowFlag;

	protected:
		UiBase();
		~UiBase();

		friend ATOM MyRegisterClass(HINSTANCE);

		bool create(HINSTANCE, int);
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
	protected:
		HWND m_TextControl;
		HWND m_EditControl;
		HWND m_BtnOk;
		HWND m_BtnCancel;

		WPARAM MessageLoop();
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		InputDlg();
		~InputDlg();

		static constexpr long WINDOW_WIDTH_AUTO = CW_USEDEFAULT;
		static constexpr long WINDOW_HEIGHT_AUTO = CW_USEDEFAULT;

		bool create(STRING title, STRING text, STRING btn1, STRING btn2, STRING DefaultText = _T(""),
			bool closable = true, long w = WINDOW_WIDTH_AUTO, long h = WINDOW_HEIGHT_AUTO);
		LPCTSTR GetInputText();
	};
};

#endif // defined(_TOOL_SHC0743_UI)

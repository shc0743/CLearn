/*     
ʮ��������ãã��д���򣬵�������
ǧ�д��룬Bug�δ��ء�
��ʹ����������������ģ�Ϧ�ϳ���
�쵼ÿ�����뷨������ģ�����æ��
������ԣ�Ω����ǧ�С�
ÿ��ƻ���ɺ��������Ա���Ӱ��
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

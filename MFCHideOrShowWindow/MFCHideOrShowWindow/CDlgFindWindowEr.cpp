// CDlgFindWindowEr.cpp: 实现文件
//

#include "pch.h"
#include "MFCHideOrShowWindow.h"
#include "CDlgFindWindowEr.h"
#include "afxdialogex.h"
#include "CDlgFindWindow.h"


// CDlgFindWindowEr 对话框

IMPLEMENT_DYNAMIC(CDlgFindWindowEr, CDialogEx)

CDlgFindWindowEr::CDlgFindWindowEr(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FINDWINDOWER, pParent)
{
	timerid = 0;
}

CDlgFindWindowEr::~CDlgFindWindowEr()
{
}

void CDlgFindWindowEr::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFindWindowEr, CDialogEx)
END_MESSAGE_MAP()


// CDlgFindWindowEr 消息处理程序

void __stdcall CDlgFindWindowErTimerProc(HWND h, UINT, UINT_PTR, DWORD) {
	POINT pt; ::GetCursorPos(&pt);
	::SetWindowPos(h, 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


BOOL CDlgFindWindowEr::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN) {
		if (!timerid) timerid = ::SetTimer(m_hWnd, 0, 100, CDlgFindWindowErTimerProc);
	}
	if (pMsg->message == WM_LBUTTONUP || pMsg->message == WM_RBUTTONUP) {
		::KillTimer(m_hWnd, timerid);
		timerid = 0;
		if (parent) {
			POINT pt; ::GetCursorPos(&pt);
			HWND hw = NULL;
			hw = ::WindowFromPoint(pt);
			if (!hw) return false;
			*(HWND*)parent = hw;
			TCHAR title[1024] = { 0 };
			::GetWindowText(hw, title, 1024);
			((CDlgFindWindow*)parent)->SetDlgItemTextW(IDC_EDIT_FINDWINDOW_TITLE, title);
			::GetClassNameW(hw, title, 1024);
			((CDlgFindWindow*)parent)->SetDlgItemTextW(IDC_EDIT_FINDWINDOW_CLASS, title);
			((CDlgFindWindow*)parent)->GetDlgItem(IDOK)->EnableWindow(1);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

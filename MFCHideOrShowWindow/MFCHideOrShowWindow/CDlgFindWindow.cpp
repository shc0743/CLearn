// CDlgFindWindow.cpp: 实现文件
//

#include "pch.h"
#include "MFCHideOrShowWindow.h"
#include "CDlgFindWindow.h"
#include "afxdialogex.h"
#include "../../resource/tool.h"


// CDlgFindWindow 对话框

IMPLEMENT_DYNAMIC(CDlgFindWindow, CDialogEx)

CDlgFindWindow::CDlgFindWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FINDWINDOW, pParent)
{
	hw = 0;
	finder = 0;
}

CDlgFindWindow::~CDlgFindWindow()
{
}

void CDlgFindWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFindWindow, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_FINDWINDOW_FIND, &CDlgFindWindow::OnBnClickedButtonFindwindowFind)
	ON_BN_CLICKED(IDOK, &CDlgFindWindow::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FW_WFP, &CDlgFindWindow::OnBnClickedButtonFwWfp)
END_MESSAGE_MAP()


// CDlgFindWindow 消息处理程序


void CDlgFindWindow::OnBnClickedButtonFindwindowFind() {
	CString c, t;
	GetDlgItemText(IDC_EDIT_FINDWINDOW_CLASS, c);
	GetDlgItemText(IDC_EDIT_FINDWINDOW_TITLE, t);
	HWND h = ::FindWindow(c.GetLength() ? c.GetBuffer() : NULL, 
						  t.GetLength() ? t.GetBuffer() : NULL);
	if (!h) return (void)(MessageBox(L"Cannot " codetostr(FindWindow), L"Error", MB_ICONERROR) &&
		GetDlgItem(IDOK)->EnableWindow(0));
	this->hw = h;
	GetDlgItem(IDOK)->EnableWindow(1);
}


#pragma warning(disable: 4311 4302)
void CDlgFindWindow::OnBnClickedOk(){
	EndDialog((int)(hw));
}


BOOL CDlgFindWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//finder = new CDlgFindWindowEr(this);
	//finder->parent = this;
	//finder->Create(IDD_DIALOG_FINDWINDOWER);
	//::SetWindowPos(finder->GetSafeHwnd(), 0, 90, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//finder->ShowWindow(SW_NORMAL);

	return TRUE;  
}



void CDlgFindWindow::OnBnClickedButtonFwWfp(){
	Sleep(2000);
	POINT pt; GetCursorPos(&pt);
	HWND h = ::WindowFromPoint(pt);
	if (!h) return (void)(MessageBox(L"Cannot WindowFromPoint", L"Error", MB_ICONERROR) &&
		GetDlgItem(IDOK)->EnableWindow(0));
	this->hw = h;
	TCHAR title[1024] = { 0 };
	::GetWindowText(hw, title, 1024);
	SetDlgItemTextW(IDC_EDIT_FINDWINDOW_TITLE, title);
	::GetClassNameW(hw, title, 1024);
	SetDlgItemTextW(IDC_EDIT_FINDWINDOW_CLASS, title);
	GetDlgItem(IDOK)->EnableWindow(1);
}

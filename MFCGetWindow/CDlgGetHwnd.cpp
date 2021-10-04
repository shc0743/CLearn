// CDlgGetHwnd.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgGetHwnd.h"
#include "afxdialogex.h"
#include "resource.h"


// CDlgGetHwnd 对话框

IMPLEMENT_DYNAMIC(CDlgGetHwnd, CDialogEx)

CDlgGetHwnd::CDlgGetHwnd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GETHWND, pParent)
{

}

CDlgGetHwnd::~CDlgGetHwnd()
{
}

void CDlgGetHwnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_GETSTAT, m_prog_getstat);
}


BEGIN_MESSAGE_MAP(CDlgGetHwnd, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgGetHwnd::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GETHWND_TOP, &CDlgGetHwnd::OnBnClickedButtonGethwndTop)
	ON_BN_CLICKED(IDC_BUTTON_GETHWND_MOUSE, &CDlgGetHwnd::OnBnClickedButtonGethwndMouse)
	ON_BN_CLICKED(IDC_BUTTON_INTTOHWND, &CDlgGetHwnd::OnBnClickedButtonInttohwnd)
END_MESSAGE_MAP()


// CDlgGetHwnd 消息处理程序

#include "hwnds.h"
#include <string>
#include "MFCGetWindowDlg.h"
bool gethwnd_okandr=false;
void CDlgGetHwnd::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!gethwnd_okandr)
	EndDialog(1);
	else
	{
		CMFCGetWindowDlg c;
		c.InitHwndU();
		EndDialog(123);
	}
}


using namespace std;
void CDlgGetHwnd::OnBnClickedButtonGethwndTop()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 3");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 2");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 1");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 正在获取...");
	uhwnd=::GetForegroundWindow();
	if (uhwnd == NULL) SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取失败");
	else {
		SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取成功");
		m_prog_getstat.SetRange(0, 100);
		m_prog_getstat.SetPos(100);
		gethwnd_okandr = 1;
	}
}


void CDlgGetHwnd::OnBnClickedButtonGethwndMouse()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 3");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 2");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 1");
	Sleep(1000);
	SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 正在获取...");
	POINT p;
	LPPOINT pot;
	pot = &p;
	GetCursorPos(pot);
	uhwnd = ::WindowFromPoint(p);
	if (uhwnd == NULL) SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取失败");
	else {
		SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取成功");
		m_prog_getstat.SetRange(0, 100);
		m_prog_getstat.SetPos(100);
		gethwnd_okandr = true;
	}
}


HWND__ h;
void CDlgGetHwnd::OnBnClickedButtonInttohwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	int a;
	GetDlgItemInt(IDC_EDIT_INT_TO_HWND, &a);
	h.unused = a;
	uhwnd = &h;
	if (uhwnd == NULL) SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取失败");
	else {
		SetDlgItemText(IDC_SHOW_GETSTATUS,L"状态: 获取成功");
		m_prog_getstat.SetRange(0, 100);
		m_prog_getstat.SetPos(100);
		gethwnd_okandr = true;
	}
}

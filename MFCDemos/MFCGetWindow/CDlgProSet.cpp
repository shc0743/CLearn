// CDlgProSet.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgProSet.h"
#include "afxdialogex.h"
#include "hwnds.h"


// CDlgProSet 对话框

IMPLEMENT_DYNAMIC(CDlgProSet, CDialogEx)

CDlgProSet::CDlgProSet(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PROSET, pParent)
{

}

CDlgProSet::~CDlgProSet()
{
}

void CDlgProSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_SETSAVING, m_prog_seting);
}


BEGIN_MESSAGE_MAP(CDlgProSet, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgProSet::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVESETS, &CDlgProSet::OnBnClickedButtonSavesets)
	ON_BN_CLICKED(IDC_CHECK_PWD, &CDlgProSet::OnBnClickedCheckPwd)
END_MESSAGE_MAP()


// CDlgProSet 消息处理程序


void CDlgProSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(0);
}


using namespace std;
#include<fstream>
#include<sstream>
#include "topvar.h"
void CDlgProSet::OnBnClickedButtonSavesets()
{
	// TODO: 在此添加控件通知处理程序代码
	MFCWindowSet a;
	a.passwd = ((CButton*)GetDlgItem(IDC_CHECK_PWD))->GetCheck();
	CString pw, pw2;
	GetDlgItemText(IDC_EDIT_SET_PW, pw);
	GetDlgItemText(IDC_EDIT_SET_PW2, pw2);
	if ((pw != pw2)) {
		MessageBox(L"参数错误!", L"ERROR", MB_ICONERROR); return;
	}
	CString startitle, nowtitle;
	GetDlgItemText(IDC_EDIT_STARTITLE, startitle);
	a.StartTitle=ws2s(startitle.GetBuffer()); 
	GetDlgItemText(IDC_EDIT_NOWTITLE, nowtitle); AfxGetMainWnd()->SetWindowText(nowtitle);
	a.alwaysrunas = ((CButton*)GetDlgItem(IDC_CHECK_SET_RUNAS))->GetCheck();
	a.autorun = ((CButton*)GetDlgItem(IDC_CHECK_SET_AUTOSTART))->GetCheck();
	a.clickxtohide = ((CButton*)GetDlgItem(IDC_CHECK_SET_HIDETO))->GetCheck();
	a.noaskclose = ((CButton*)GetDlgItem(IDC_CHECK_SET_NOALERT))->GetCheck();
	a.ontop = ((CButton*)GetDlgItem(IDC_CHECK_SET_ZDWIN))->GetCheck();
	a.pwd = (string)ws2s(pw.GetBuffer());
	fstream fp("m.f.c.window.settings-profile.dat",ios::out|ios::binary);
	fp.write((char*)&a, sizeof(a));
	fp.close();
	MessageBox(L"成功保存了设置!", L"好消息", MB_ICONINFORMATION);
	m_prog_seting.SetRange(0, 100);
	m_prog_seting.SetPos(0);
	m_prog_seting.SetPos(100);
	if (((promainset.ontop!=a.ontop||promainset.pwd!=a.pwd||promainset.alwaysrunas!=a.alwaysrunas||promainset.passwd!=a.passwd))&&MessageBox(L"需要重新启动应用程序来应用这些更改\n要重新启动吗?", L"问题", MB_ICONQUESTION|MB_YESNO)==IDYES) {
		ostringstream oss;
		oss << "powershell start-process -filepath \"" << ::GetCommandLineA() << "\"";
		WinExec(oss.str().c_str(),0);
		exit(0);
	}
#if 0
	if (MessageBox(L"需要重新启动应用程序来应用这些更改\n要重新启动吗?", L"问题", MB_ICONQUESTION|MB_YESNO)==IDYES) {
		ostringstream oss;
		oss << "powershell start-process -filepath \"" << ::GetCommandLineA() << "\"";
		sy/stem(oss.str().c_str());
		exit(0);
	}
#else
	promainset = a;
#endif
}


void CDlgProSet::OnBnClickedCheckPwd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_CHECK_PWD))->GetCheck() == 0) {
		GetDlgItem(IDC_EDIT_SET_PW)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SET_PW2)->EnableWindow(0);
	}
	else {
		GetDlgItem(IDC_EDIT_SET_PW)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SET_PW2)->EnableWindow(TRUE);
	}
}

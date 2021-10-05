// CDlgLogin.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginDLL.h"
#include "CDlgLogin.h"
#include "afxdialogex.h"
#include "../../resource/tool.h"
#include "CDlgRegister.h"


// CDlgLogin 对话框

IMPLEMENT_DYNAMIC(CDlgLogin, CDialogEx)

CDlgLogin::CDlgLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::showRegisterButton(bool show){
	auto p = GetDlgItem(IDC_BUTTON_REGISTER);
	if (!p) return;
	p->EnableWindow(show);
	p->ShowWindow(show);
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialogEx)
	ON_COMMAND(IDD_DIALOG_LOGIN, &CDlgLogin::OnIddDialogLogin)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CDlgLogin::OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLogin 消息处理程序


void CDlgLogin::OnIddDialogLogin() {
	CString str;
	GetDlgItemText(IDC_EDIT_TEXT, str);
	if (str.IsEmpty()) {
		MessageBox(L"Please input your username!!!", L"Error", MB_ICONERROR); return;
	}
	username_ = ws2s(str.GetBuffer());
	GetDlgItem(IDC_STATIC_TIPS1)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_TIPS2)->ShowWindow(1);
	GetDlgItem(IDC_BUTTON_CONTINUE)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_CONTINUE)->ShowWindow(0);
	GetDlgItem(IDC_CHECK_REMEMBER)->EnableWindow(1);
	GetDlgItem(IDC_CHECK_REMEMBER)->ShowWindow(1);
	GetDlgItem(IDOK)->EnableWindow(1);
	GetDlgItem(IDOK)->ShowWindow(1);
	((CEdit*)GetDlgItem(IDC_EDIT_TEXT))->SetPasswordChar(_T('*'));
}


void CDlgLogin::OnBnClickedButtonRegister(){
	CDlgRegister dlg;
	dlg.DoModal();
}


void CDlgLogin::OnBnClickedOk(){
	CString p; GetDlgItemText(IDC_EDIT_TEXT, p);
	password_ = ws2s(p.GetBuffer());
	EndDialog(0);
}

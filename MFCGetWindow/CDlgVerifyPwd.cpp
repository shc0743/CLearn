// CDlgVerifyPwd.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgVerifyPwd.h"
#include "afxdialogex.h"
#include "topvar.h"


// CDlgVerifyPwd 对话框

IMPLEMENT_DYNAMIC(CDlgVerifyPwd, CDialogEx)

CDlgVerifyPwd::CDlgVerifyPwd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TYPEPASSWD, pParent)
{

}

CDlgVerifyPwd::~CDlgVerifyPwd()
{
}

void CDlgVerifyPwd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVerifyPwd, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgVerifyPwd::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgVerifyPwd::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_PWD, &CDlgVerifyPwd::OnBnClickedCheckPwd)
END_MESSAGE_MAP()


// CDlgVerifyPwd 消息处理程序


void CDlgVerifyPwd::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(1);
}


#include "resource.h"
void CDlgVerifyPwd::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString upwd;
	GetDlgItemText(IDC_EDIT_MAINPWD, upwd);
	if (ws2s(upwd.GetBuffer()) != promainset.pwd) {
		MessageBox(L"密码错误,请重试", L"错误", MB_ICONERROR);
	}
	else EndDialog(123);
}
BOOL CDlgVerifyPwd::OnInitDialog()
{
	/*string clpw(::GetCommandLineA());
	if (clpw.find("--passwd")!=clpw.npos) { 
		while (clpw[0] == '"') clpw.erase(0, 1);
		clpw.erase(0, GetProgramDirEXINTHEVERIFYPWD().length());
		while (clpw[0] == ' ') clpw.erase(0, 1);
		if (clpw == promainset.pwd) {
			EndDialog(123);
		} else MessageBox(L"预置密码不正确", L"错误", MB_ICONERROR);
	}*/
	return 1;
}


void CDlgVerifyPwd::OnBnClickedCheckPwd()
{
	// TODO: 在此添加控件通知处理程序代码
#define s AfxGetMainWnd()
	if (((CButton*)GetDlgItem(IDC_CHECK_PWD))->GetCheck() == 0) {
		s->GetDlgItem(IDC_EDIT_SET_PW)->EnableWindow(0);
		s->GetDlgItem(IDC_EDIT_SET_PW2)->EnableWindow(0);
	} else {
		s->GetDlgItem(IDC_EDIT_SET_PW)->EnableWindow(TRUE);
		s->GetDlgItem(IDC_EDIT_SET_PW2)->EnableWindow(TRUE);
	}
#undef s
}

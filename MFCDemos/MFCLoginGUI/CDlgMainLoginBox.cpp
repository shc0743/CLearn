// CDlgMainLoginBox.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginGUI.h"
#include "CDlgMainLoginBox.h"
#include "afxdialogex.h"
#include "user.h"
#include "resource.h"
#include "CDlgRegBox.h"
using namespace std;


// CDlgMainLoginBox 对话框

IMPLEMENT_DYNAMIC(CDlgMainLoginBox, CDialogEx)

CDlgMainLoginBox::CDlgMainLoginBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

CDlgMainLoginBox::~CDlgMainLoginBox()
{
}

void CDlgMainLoginBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMainLoginBox, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMainLoginBox::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDlgMainLoginBox::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_TOREG, &CDlgMainLoginBox::OnBnClickedButtonToreg)
END_MESSAGE_MAP()


// CDlgMainLoginBox 消息处理程序


void CDlgMainLoginBox::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(1);
}




void CDlgMainLoginBox::OnBnClickedButtonLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	fstream us;
	us.open("users.data.pwd", ios::in | ios::binary);
	if (!us) {
		us.close();
		if (MessageBox(L"users不存在,是否创建?", L"Login", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			fstream userdcing;
			userdcing.open("users.data.pwd", ios::app | ios::binary);
			userud rootuser;
			rootuser.setUserName("root");
			rootuser.changePasswd("root");
			userdcing.write((char*)&rootuser, sizeof(rootuser));
			userdcing.close();
		}
		else exit(-1);
	}
	userud auser; bool isok = false; CString usiun, pwiun;
	usiun = ""; pwiun = "";
	while (!us.eof() && us.read((char*)&auser, sizeof(auser))) {
		//MessageBox("233");
		GetDlgItem(IDC_EDIT_UID)->GetWindowText(usiun);
		if (auser.getUserName() == ws2s(usiun.GetBuffer())) {
			isok = true;
			break;
		}
	}
	us.close();
	if (!isok) {
		MessageBox(L"错误: 账号或密码错误",L"错误",MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(pwiun);
	if (!auser.verifyPasswd(ws2s(pwiun.GetBuffer()))) {
		MessageBox(L"错误: 账号或密码错误",L"错误",MB_ICONERROR);
		return;
	}
	fstream nowu;
	nowu.open("login.user.pwd", ios::out | ios::binary);
	LoginedUser logineduser(ws2s(usiun.GetBuffer()),((CButton *)GetDlgItem(IDC_CHECK_NOFORGOT))->GetCheck());
	nowu.write((char*)&logineduser, sizeof(logineduser));
	nowu.close();
	MessageBox(L"登录成功,点击确定重新启动...",L"好消息",MB_ICONINFORMATION);
	ostringstream oss;
	oss << "powershell Start-Process -FilePath " << ws2s(::GetCommandLine()) << "";
	system(oss.str().c_str());
	exit(0);
}


void CDlgMainLoginBox::OnBnClickedButtonToreg()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgRegBox rb;
	rb.DoModal();
}

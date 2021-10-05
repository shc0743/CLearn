// CDlgChangePwd.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginGUI.h"
#include "CDlgChangePwd.h"
#include "afxdialogex.h"
#include <string>


// CDlgChangePwd 对话框

IMPLEMENT_DYNAMIC(CDlgChangePwd, CDialogEx)

CDlgChangePwd::CDlgChangePwd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_U_CPWD, pParent)
{

}

CDlgChangePwd::~CDlgChangePwd()
{
}

void CDlgChangePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChangePwd, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CPWD, &CDlgChangePwd::OnBnClickedButtonCpwd)
END_MESSAGE_MAP()


// CDlgChangePwd 消息处理程序


#include<fstream>
#include "user.h"

void CDlgChangePwd::OnBnClickedButtonCpwd()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace std;
	CString uu, up, up2;
	GetDlgItemText(IDC_EDIT_OLDPWD, uu);
	GetDlgItemText(IDC_EDIT_NEWPWD, up);
	GetDlgItemText(IDC_EDIT_NEWPWD2, up2);
	string oldp = ws2s(uu.GetBuffer()), newp = ws2s(up.GetBuffer()), np2 = ws2s(up2.GetBuffer());
	string uup = newp;
	if (uup.length() < 6||np2.length() < 6) {
		MessageBox(L"密码长度必须大于等于6!", L"错误", MB_ICONERROR);
		return;
	}
	if (uup.length() > 60) {
		MessageBox(L"您的密码太长了!短一点也不会有人盗号的啦~", L"错误", MB_ICONERROR);
		return;
	}
	if (uup == "123456" || uup == "654321") {
		MessageBox(L"您的密码也太简单了吧!换个再试试吧", L"错误", MB_ICONERROR);
		return;
	}
	std::fstream uaf,naf,newuf;
	uaf.open("users.data.pwd", ios::in | ios::binary);
	naf.open("login.user.pwd", ios::in | ios::binary);
	newuf.open("login.user.pwd.changing", ios::out | ios::binary);
	if (!uaf) exit(-1);
	userud lou, ausert;
	naf.read((char*)&lou, sizeof(lou));
	int okc = 0;
	while (uaf.read((char*)&ausert, sizeof(ausert))) {
		if (ausert.getUserName() == lou.getUserName()) {
			if (!ausert.verifyPasswd(oldp)) {
				okc = 2;
			    MessageBox(L"抱歉,我们无法更改您的密码,因为您输入的您原来的密码是不正确的...", L"错误", MB_ICONERROR);
				break;
			}
			if (newp!=np2) {
				okc = 2;
			    MessageBox(L"抱歉,我们无法更改您的密码,因为您两次输入的密码不一样...", L"错误", MB_ICONERROR);
				break;
			}
			okc = 1;
			ausert.changePasswd(newp);
			newuf.write((char*)&ausert, sizeof(ausert));
			MessageBox(L"成功修改了您的密码...", L"好消息", MB_ICONINFORMATION);
			continue;
		}
		else newuf.write((char*)&ausert, sizeof(ausert));
	}
	naf.close(); uaf.close(); newuf.close();
	if (okc == 0) {
		MessageBox(L"未知错误");
		exit(-2);
	} else if(okc == 1) {
		system("del /f /s /q users.data.pwd");
		system("del /f /s /q login.user.pwd");
		system("move login.user.pwd.changing users.data.pwd");
		MessageBox(L"一切已经准备就绪,按确定重新启动...", L"好消息", MB_ICONINFORMATION);
		ostringstream oss;
		oss << "powershell Start-Process -FilePath " << ws2s(::GetCommandLine()) << "";
		system(oss.str().c_str());
		exit(0);
	} else if(okc == 2) {
		system("del /f /s /q login.user.pwd.changing");
	}
}

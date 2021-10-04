// CDlgRegBox.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginGUI.h"
#include "CDlgRegBox.h"
#include "afxdialogex.h"
#include <fstream>
#include "user.h"


// CDlgRegBox 对话框

IMPLEMENT_DYNAMIC(CDlgRegBox, CDialogEx)

CDlgRegBox::CDlgRegBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REG, pParent)
{

}

CDlgRegBox::~CDlgRegBox()
{
}

void CDlgRegBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRegBox, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_TOLOG, &CDlgRegBox::OnBnClickedButtonTolog)
	ON_BN_CLICKED(IDC_BUTTON_REGID, &CDlgRegBox::OnBnClickedButtonRegid)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgRegBox::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgRegBox 消息处理程序


void CDlgRegBox::OnBnClickedButtonTolog()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(1);
}


void CDlgRegBox::OnBnClickedButtonRegid()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace std;
	CString uu, up;
	GetDlgItemText(IDC_EDIT_UID_R, uu);
	GetDlgItemText(IDC_EDIT_PWD_R, up);
	string uuu=ws2s(uu.GetBuffer()), uup=ws2s(up.GetBuffer());
	fstream uf; fstream ruf;
	ruf.open("users.data.pwd", ios::in | ios::binary);
	if (!ruf) exit(-1);
	userud ausert;
	while (ruf.read((char*)&ausert,sizeof(ausert))) {
		if (ausert.getUserName() == uuu) {
		    MessageBox(L"用户名太火热了!换一个试试8~", L"错误", MB_ICONERROR);
		    return;
		}
	}
	uf.open("users.data.pwd", ios::app | ios::binary);
	if (uuu.length() < 3) {
		MessageBox(L"账号长度必须大于等于3!", L"错误", MB_ICONERROR);
		return;
	}
	if (uup.length() < 6) {
		MessageBox(L"密码长度必须大于等于6!", L"错误", MB_ICONERROR);
		return;
	}
	if (uuu.length() > 30) {
		MessageBox(L"您的账号太长了!短一点吧求你了", L"错误", MB_ICONERROR);
		return;
	}
	if (uup.length() >60) {
		MessageBox(L"您的密码太长了!短一点也不会有人盗号的啦~", L"错误", MB_ICONERROR);
		return;
	}
	if (uup=="123456"||uup=="654321") {
	    MessageBox(L"您的密码也太简单了吧!换个再试试吧", L"错误", MB_ICONERROR);
		return;
	}
	userud uusn(uuu,uup);
	uf.write((char*)&uusn,sizeof(uusn));
	uf.close();
	MessageBox(L"注册成功,点击确定去登录...", L"好消息", MB_ICONINFORMATION);
	EndDialog(0);
}


void CDlgRegBox::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(1);
}

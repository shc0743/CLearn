// CDlgMyInfo.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginGUI.h"
#include "CDlgMyInfo.h"
#include "afxdialogex.h"
#include "resource.h"


// CDlgMyInfo 对话框

IMPLEMENT_DYNAMIC(CDlgMyInfo, CDialogEx)

CDlgMyInfo::CDlgMyInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_U_MYINFO, pParent)
{
	
}

CDlgMyInfo::~CDlgMyInfo()
{
}

void CDlgMyInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMyInfo, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgMyInfo::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_INFO_INIT, &CDlgMyInfo::OnBnClickedButtonInfoInit)
	ON_BN_CLICKED(IDC_BUTTON_INFSAVE, &CDlgMyInfo::OnBnClickedButtonInfsave)
END_MESSAGE_MAP()


// CDlgMyInfo 消息处理程序


void CDlgMyInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(0);
}

#include "user.h"
#include<fstream>

bool hasexdata = false;
void CDlgMyInfo::OnBnClickedButtonInfoInit()
{
	// TODO: 在此添加控件通知处理程序代码
    LoginedUser MainUser;
    std::fstream lu;
    lu.open("login.user.pwd", std::ios::in | std::ios::binary);
	lu.read((char*)&MainUser, sizeof(MainUser));
	lu.close();
	this->SetDlgItemText(IDC_SHOW_UNAME, s2ws(MainUser.getUserName()).c_str());
	this->GetDlgItem(IDC_BUTTON_INFO_INIT)->EnableWindow(FALSE);
	userudEx theuser;
    lu.open("user.dataex.pwd", std::ios::in | std::ios::binary);
	if (!lu) {
		lu.close();
		if (MessageBox(L"dataex不存在,是否创建?", L"", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			std::fstream ffex("user.dataex.pwd", std::ios::out | std::ios::binary);
			theuser.setUserName("root");
			ffex.write((char*)&theuser, sizeof(theuser));
			ffex.close();
		} else return;
	}
    lu.open("user.dataex.pwd", std::ios::in | std::ios::binary);
	while (lu.peek()!=EOF) {
		lu.read((char*)&theuser, sizeof(theuser));
		//if (theuser.getUserName() == "") break;
		if (theuser.getUserName() == MainUser.getUserName()) {
			hasexdata = true;
			break;
		}
	}
	lu.close();
	if (hasexdata) {
		this->SetDlgItemText(IDC_EDIT_SEX,s2ws(theuser.getSex()).c_str());
		this->SetDlgItemText(IDC_DATE_BIRTH,s2ws(theuser.getBirthday()).c_str());
		this->SetDlgItemText(IDC_EDIT_QQ,s2ws(theuser.getQQ()).c_str());
		this->SetDlgItemText(IDC_EDIT_WX,s2ws(theuser.getWeiXin()).c_str());
		this->SetDlgItemText(IDC_EDIT_GH,s2ws(theuser.getGithub()).c_str());
		this->SetDlgItemText(IDC_EDIT_OCC,s2ws(theuser.getOccupation()).c_str());
	}
	this->GetDlgItem(IDC_BUTTON_INFSAVE)->EnableWindow(TRUE);
}


void CDlgMyInfo::OnBnClickedButtonInfsave()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace std;
	LoginedUser MainUser;
    fstream lu;
    lu.open("login.user.pwd", std::ios::in | std::ios::binary);
	lu.read((char*)&MainUser, sizeof(MainUser));
	lu.close();
    userudEx theuser;
	lu.open("user.dataex.pwd", std::ios::in | std::ios::binary);
	if (!lu) {
		lu.close();
		if (MessageBox(L"dataex不存在,是否创建?", L"", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			std::fstream ffex("user.dataex.pwd", std::ios::out | std::ios::binary);
			theuser.setUserName("root");
			ffex.write((char*)&theuser, sizeof(theuser));
			ffex.close();
		} else return;
	}
    lu.open("user.dataex.pwd", std::ios::in | std::ios::binary);
    fstream cing("user.dataex.pwd.changing", std::ios::out | std::ios::binary);
	int xhcs = 0;
	while (1) {
		++xhcs;
		lu.read((char*)&theuser, sizeof(theuser));
		if (theuser.getUserName() == "") break;
		if (theuser.getUserName() == MainUser.getUserName()) {
			CString str; userudEx uschi;
			uschi.setUserName(MainUser.getUserName());
			this->GetDlgItemText(IDC_EDIT_SEX,str);
			uschi.setSex(ws2s((wstring)str.GetBuffer()));
			this->GetDlgItemText(IDC_DATE_BIRTH,str);
			uschi.setBirthday(ws2s((wstring)str.GetBuffer()));
			this->GetDlgItemText(IDC_EDIT_OCC,str);
			uschi.setOccupation(ws2s((wstring)str.GetBuffer()));
			this->GetDlgItemText(IDC_EDIT_QQ,str);
			uschi.setQQ(ws2s((wstring)str.GetBuffer()));
			this->GetDlgItemText(IDC_EDIT_WX,str);
			uschi.setWeiXin(ws2s((wstring)str.GetBuffer()));
			this->GetDlgItemText(IDC_EDIT_GH,str);
			uschi.setGithub(ws2s((wstring)str.GetBuffer())); 
			cing.write((char*)&uschi, sizeof(uschi));
		}
		else cing.write((char*)&theuser, sizeof(theuser));
	}
	if (xhcs == 1) {
		CString str; userudEx uschi;
		uschi.setUserName(MainUser.getUserName());
		this->GetDlgItemText(IDC_EDIT_SEX, str);
		uschi.setSex(ws2s((wstring)str.GetBuffer()));
		this->GetDlgItemText(IDC_DATE_BIRTH, str);
		uschi.setBirthday(ws2s((wstring)str.GetBuffer()));
		this->GetDlgItemText(IDC_EDIT_OCC, str);
		uschi.setOccupation(ws2s((wstring)str.GetBuffer()));
		this->GetDlgItemText(IDC_EDIT_QQ, str);
		uschi.setQQ(ws2s((wstring)str.GetBuffer()));
		this->GetDlgItemText(IDC_EDIT_WX, str);
		uschi.setWeiXin(ws2s((wstring)str.GetBuffer()));
		this->GetDlgItemText(IDC_EDIT_GH, str);
		uschi.setGithub(ws2s((wstring)str.GetBuffer()));
		cing.write((char*)&uschi, sizeof(uschi));
	}
	lu.close();
	cing.close();
	system("del /f /s /q user.dataex.pwd");
	system("move user.dataex.pwd.changing user.dataex.pwd");
	MessageBox(L"成功更新了您的资料~", L"温馨提示", MB_ICONINFORMATION);
}

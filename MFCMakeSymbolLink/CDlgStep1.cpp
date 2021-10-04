// CDlgStep1.cpp: 实现文件
//

#include "pch.h"
#include "MFCMakeSymbolLink.h"
#include "CDlgStep1.h"
#include "afxdialogex.h"


// CDlgStep1 对话框

IMPLEMENT_DYNAMIC(CDlgStep1, CDialogEx)

CDlgStep1::CDlgStep1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STEP1, pParent)
{

}

CDlgStep1::~CDlgStep1()
{
}

void CDlgStep1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStep1, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgStep1::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CONF_RBMENU, &CDlgStep1::OnBnClickedButtonConfRbmenu)
END_MESSAGE_MAP()


// CDlgStep1 消息处理程序


BOOL CDlgStep1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE; 
}


void CDlgStep1::OnBnClickedCancel()
{
	exit(2);
}


void CDlgStep1::OnBnClickedButtonConfRbmenu(){
	stringstream ss;

	ss.str("");
	ss << "reg add HKCR\\*\\shell\\MakeSymbolLinkWithMMSL /ve /d \"为 %1 建立链接\"" << endl;
	system(ss.str().c_str());

	ss.str("");
	ss << "reg add HKCR\\*\\shell\\MakeSymbolLinkWithMMSL\\command /ve /d \"\"\"" <<
		GetProgramDir() << "\"\" \"\"%1\"\"\"" << endl;
	system(ss.str().c_str());

	ss.str("");
	ss << "reg add HKCR\\Directory\\shell\\MakeSymbolLinkWithMMSL /ve /d \"为 %1 建立目录链接\"" << endl;
	system(ss.str().c_str());

	ss.str("");
	ss << "reg add HKCR\\Directory\\shell\\MakeSymbolLinkWithMMSL\\command /ve /d \"\"\""<<
		GetProgramDir() <<"\"\" -d \"\"%1\"\"\"" << endl;
	system(ss.str().c_str());

	ss.str("");
	ss << "reg add HKCR\\Directory\\Background\\shell\\MakeSymbolLinkWithMMSL /ve /d \"新建链接...\"" << endl;
	system(ss.str().c_str());

	ss.str("");
	ss << "reg add HKCR\\Directory\\Background\\shell\\MakeSymbolLinkWithMMSL\\command /ve /d \"\"\""<<
		GetProgramDir() <<"\"\"\"" << endl;
	system(ss.str().c_str());

	exit(3);
}

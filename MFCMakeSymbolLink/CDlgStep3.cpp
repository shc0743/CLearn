// CDlgStep3.cpp: 实现文件
//

#include "pch.h"
#include "MFCMakeSymbolLink.h"
#include "CDlgStep3.h"
#include "afxdialogex.h"


// CDlgStep3 对话框

IMPLEMENT_DYNAMIC(CDlgStep3, CDialogEx)

CDlgStep3::CDlgStep3(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STEP3, pParent)
{

}

CDlgStep3::~CDlgStep3()
{
}

void CDlgStep3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStep3, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgStep3::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgStep3::OnBnClickedCancel)
//	ON_BN_CLICKED(IDC_BUTTON1, &CDlgStep3::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON_CREATE_HL, &CDlgStep3::OnBnClickedButtonCreateHl)
END_MESSAGE_MAP()


// CDlgStep3 消息处理程序


void CDlgStep3::OnBnClickedOk()
{
	EndDialog(0);
}


void CDlgStep3::OnBnClickedCancel()
{
	exit(2);
}


void CDlgStep3::OnBnClickedButtonCreateHl()
{
	hlink = true; EndDialog(0);
}

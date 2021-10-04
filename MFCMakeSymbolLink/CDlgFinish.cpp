// CDlgFinish.cpp: 实现文件
//

#include "pch.h"
#include "MFCMakeSymbolLink.h"
#include "CDlgFinish.h"
#include "afxdialogex.h"


// CDlgFinish 对话框

IMPLEMENT_DYNAMIC(CDlgFinish, CDialogEx)

CDlgFinish::CDlgFinish(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_BYE, pParent)
{

}

CDlgFinish::~CDlgFinish()
{
}

void CDlgFinish::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFinish, CDialogEx)
	ON_BN_CLICKED(IDC_COMMAND1, &CDlgFinish::OnBnClickedCommand1)
END_MESSAGE_MAP()


// CDlgFinish 消息处理程序


void CDlgFinish::OnBnClickedCommand1()
{
	exit(0);
}

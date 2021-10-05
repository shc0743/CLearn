// CDlgWelPage.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginGUI.h"
#include "CDlgWelPage.h"
#include "afxdialogex.h"


// CDlgWelPage 对话框

IMPLEMENT_DYNAMIC(CDlgWelPage, CDialogEx)

CDlgWelPage::CDlgWelPage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_U_WELCOME, pParent)
{

}

CDlgWelPage::~CDlgWelPage()
{
}

void CDlgWelPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWelPage, CDialogEx)
END_MESSAGE_MAP()


// CDlgWelPage 消息处理程序

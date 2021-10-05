// CDlgAboutDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCCalcYourChineseZodiac.h"
#include "CAboutDlg.h"
#include "afxdialogex.h"


// CDlgAboutDlg 对话框

IMPLEMENT_DYNAMIC(CDlgAboutDlg, CDialogEx)

CDlgAboutDlg::CDlgAboutDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

CDlgAboutDlg::~CDlgAboutDlg()
{
}

void CDlgAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDlgAboutDlg 消息处理程序

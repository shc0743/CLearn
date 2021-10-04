// CDlgOkExit.cpp: 实现文件
//

#include "pch.h"
#include "MFCActiveSoftware.h"
#include "CDlgOkExit.h"
#include "afxdialogex.h"


// CDlgOkExit 对话框

IMPLEMENT_DYNAMIC(CDlgOkExit, CDialogEx)

CDlgOkExit::CDlgOkExit(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ACTOK, pParent)
{

}

CDlgOkExit::~CDlgOkExit()
{
}

void CDlgOkExit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOkExit, CDialogEx)
END_MESSAGE_MAP()


// CDlgOkExit 消息处理程序

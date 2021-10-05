// CDlgRegister.cpp: 实现文件
//

#include "pch.h"
#include "MFCLoginDLL.h"
#include "CDlgRegister.h"
#include "afxdialogex.h"


// CDlgRegister 对话框

IMPLEMENT_DYNAMIC(CDlgRegister, CDialogEx)

CDlgRegister::CDlgRegister(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REGISTER, pParent)
{

}

CDlgRegister::~CDlgRegister()
{
}

void CDlgRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRegister, CDialogEx)
END_MESSAGE_MAP()


// CDlgRegister 消息处理程序

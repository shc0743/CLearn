// CDlgWait_1.cpp: 实现文件
//

#include "pch.h"
#include "MFCFileUnDiff.h"
#include "afxdialogex.h"
#include "CDlgWait_1.h"


// CDlgWait_1 对话框

IMPLEMENT_DYNAMIC(CDlgWait_1, CDialogEx)

CDlgWait_1::CDlgWait_1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PleaseWait_1, pParent)
{

}

CDlgWait_1::~CDlgWait_1()
{
}

void CDlgWait_1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWait_1, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgWait_1 消息处理程序


//BOOL CDlgWait_1::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CDialogEx::PreTranslateMessage(pMsg);
//}


void CDlgWait_1::OnClose() {}
void CDlgWait_1::OnCancel(){}


BOOL CDlgWait_1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	pSysMenu->EnableMenuItem(SC_CLOSE,MF_DISABLED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


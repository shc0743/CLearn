// CDlgAdvancedOptions.cpp: 实现文件
//

#include "pch.h"
#include "SysManageToolUi.h"
#include "afxdialogex.h"
#include "CDlgAdvancedOptions.h"


// CDlgAdvancedOptions 对话框

IMPLEMENT_DYNAMIC(CDlgAdvancedOptions, CDialogEx)

CDlgAdvancedOptions::CDlgAdvancedOptions(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ADVANCED, pParent)
{

}

CDlgAdvancedOptions::~CDlgAdvancedOptions()
{
}

void CDlgAdvancedOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAdvancedOptions, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOADDRIVER, &CDlgAdvancedOptions::OnBnClickedButtonLoaddriver)
END_MESSAGE_MAP()


// CDlgAdvancedOptions 消息处理程序


void CDlgAdvancedOptions::OnBnClickedButtonLoaddriver()
{
	if (MessageBox(L"Are you sure you want to load kernel driver?\nIt might cause BSoD!"
		, L"Kernel Driver Loader", MB_ICONWARNING | MB_YESNO) != IDYES) return;

}

// CDlgStep2.cpp: 实现文件
//

#include "pch.h"
#include "MFCMakeSymbolLink.h"
#include "CDlgStep2.h"
#include "afxdialogex.h"


// CDlgStep2 对话框

IMPLEMENT_DYNAMIC(CDlgStep2, CDialogEx)

CDlgStep2::CDlgStep2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STEP2, pParent)
{

}

CDlgStep2::~CDlgStep2()
{
}

void CDlgStep2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStep2, CDialogEx)
//	ON_EN_CHANGE(IDC_EDIT1, &CDlgStep2::OnEnChangeEdit1)
ON_EN_CHANGE(IDC_EDIT_SRC, &CDlgStep2::OnEnChangeEditSrc)
ON_BN_CLICKED(IDCANCEL, &CDlgStep2::OnBnClickedCancel)
ON_BN_CLICKED(IDC_BUTTON_BROWSE_SRC, &CDlgStep2::OnBnClickedButtonBrowseSrc)
ON_BN_CLICKED(IDOK, &CDlgStep2::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgStep2 消息处理程序


void CDlgStep2::OnEnChangeEditSrc()
{
	CString str;
	GetDlgItemText(IDC_EDIT_SRC, str);
	if (str.GetLength() < 1) {
		SetDlgItemText(IDC_STATIC_SRCSTAT, L"请输入文件位置");
		GetDlgItem(IDOK)->EnableWindow(0);return;
	}
	else if (!file_exists(ws2s(str.GetBuffer()))) {
		SetDlgItemText(IDC_STATIC_SRCSTAT, L"指定的文件不存在或无法打开文件");
		GetDlgItem(IDOK)->EnableWindow(0);return;
	}
	SetDlgItemText(IDC_STATIC_SRCSTAT, L"请单击【继续】");
	GetDlgItem(IDOK)->EnableWindow(1);
}


void CDlgStep2::OnBnClickedCancel()
{
	exit(2);
}


void CDlgStep2::OnBnClickedButtonBrowseSrc()
{
	CString gReadFilePathName;
	CFileDialog fileDlg(true, 0,0, OFN_OVERWRITEPROMPT, _T("All Files(*.*)|*.*||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框
	{
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名
		GetDlgItem(IDC_EDIT_SRC)->SetWindowText(gReadFilePathName);//将路径显示
	}
}


void CDlgStep2::OnBnClickedOk()
{
	CString str;GetDlgItemText(IDC_EDIT_SRC, str);
	srcfile = ws2s(str.GetBuffer());
	dlink = ((CButton*)GetDlgItem(IDC_CHECK_DIRLINK))->GetCheck();
	EndDialog(0);
}

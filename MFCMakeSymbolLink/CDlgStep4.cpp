// CDlgStep4.cpp: 实现文件
//

#include "pch.h"
#include "MFCMakeSymbolLink.h"
#include "CDlgStep4.h"
#include "afxdialogex.h"


// CDlgStep4 对话框

IMPLEMENT_DYNAMIC(CDlgStep4, CDialogEx)

CDlgStep4::CDlgStep4(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STEP4, pParent)
{

}

CDlgStep4::~CDlgStep4()
{
}

void CDlgStep4::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStep4, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgStep4::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgStep4::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_BUTTON2, &CDlgStep4::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUT, &CDlgStep4::OnBnClickedButtonBrowseOut)
END_MESSAGE_MAP()


// CDlgStep4 消息处理程序


void CDlgStep4::OnBnClickedCancel()
{
	exit(2);
}


void CDlgStep4::OnBnClickedOk()
{
	CString str; GetDlgItemText(IDC_EDIT_OUT, str);
	destfile = ws2s(str.GetBuffer());
	EndDialog(0);
}


void CDlgStep4::OnBnClickedButtonBrowseOut()
{
	CString gReadFilePathName;
	CFileDialog fileDlg(false, 0, 0, OFN_OVERWRITEPROMPT, _T("All Files(*.*)|*.*||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框
	{
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名
		GetDlgItem(IDC_EDIT_OUT)->SetWindowText(gReadFilePathName);//将路径显示
	}
}

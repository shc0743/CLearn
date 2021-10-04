// CDlgHFuncDllInto.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgHFuncDllInto.h"
#include "afxdialogex.h"
#include <string>
using namespace std;


// CDlgHFuncDllInto 对话框

IMPLEMENT_DYNAMIC(CDlgHFuncDllInto, CDialogEx)

CDlgHFuncDllInto::CDlgHFuncDllInto(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HFUNC_INTODLL, pParent)
{

}

CDlgHFuncDllInto::~CDlgHFuncDllInto()
{
}

void CDlgHFuncDllInto::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHFuncDllInto, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgHFuncDllInto::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DLL_HIDEMAINWND, &CDlgHFuncDllInto::OnClickedButtonDllHidemainwnd)
	ON_BN_CLICKED(IDC_BUTTON_DLL_SELECTDLL, &CDlgHFuncDllInto::OnBnClickedButtonDllSelectdll)
	ON_BN_CLICKED(IDC_BUTTON_DLL_CRT, &CDlgHFuncDllInto::OnBnClickedButtonDllCrt)
END_MESSAGE_MAP()


// CDlgHFuncDllInto 消息处理程序


void CDlgHFuncDllInto::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	EndDialog(1);
}


bool DLL__ISHIDEDMAINWND=false;
void CDlgHFuncDllInto::OnClickedButtonDllHidemainwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (DLL__ISHIDEDMAINWND) {
		DLL__ISHIDEDMAINWND = false;
		SetDlgItemText(IDC_BUTTON_DLL_HIDEMAINWND, L"隐藏主窗口");
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
	} else {
	    DLL__ISHIDEDMAINWND = true;
		SetDlgItemText(IDC_BUTTON_DLL_HIDEMAINWND, L"显示主窗口");
		AfxGetMainWnd()->ShowWindow(SW_HIDE);
	}
}


void CDlgHFuncDllInto::OnBnClickedButtonDllSelectdll()
{
	// TODO: 在此添加控件通知处理程序代码
	CString gReadFilePathName;
	CFileDialog fileDlg(true, _T("dll"), _T("*.dll"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Dynamic Link Library(*.dll)|*.dll|动态链接库(*.dll)|*.dll|所有文件(*.*)|*.*||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框
	{
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名
		GetDlgItem(IDC_EDIT_DLLNAME)->SetWindowText(gReadFilePathName);//将路径显示
		//CString filename = fileDlg.GetFileName();
	}
}


#include<Windows.h>
#if 0
安慰法闺女
；；
fghjm
1000
#endif
void CDlgHFuncDllInto::OnBnClickedButtonDllCrt()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(L"抱歉,暂不支持此功能!",L"Sorry",MB_ICONERROR);
}

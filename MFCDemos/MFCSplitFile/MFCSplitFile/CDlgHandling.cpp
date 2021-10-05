// CDlgHandling.cpp: 实现文件
//

#include "pch.h"
#include "MFCSplitFile.h"
#include "CDlgHandling.h"
#include "afxdialogex.h"


// CDlgHandling 对话框

IMPLEMENT_DYNAMIC(CDlgHandling, CDialogEx)

CDlgHandling::CDlgHandling(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WAIT_FOR_DOING, pParent)
	, hThread(nullptr)
	, param_location(nullptr)
	, type(ErrorType)
	, paused(false)
{

}

CDlgHandling::~CDlgHandling()
{
}

void CDlgHandling::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHandling, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgHandling::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgHandling::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgHandling 消息处理程序


unsigned CDlgHandling::HandleSplitFile(void*p){
	AssertEx_(p, __LINE__, __FILE__, true, false);
	AssertEx_(p, __LINE__, __FILE__, true, true);
	CDlgHandling_SplitParamStruct thes;
	memcpy(&thes, p, sizeof thes);
	delete (CDlgHandling_SplitParamStruct*)p;

	thes.lpStartAddress->SetDlgItemText(IDC_STATIC_PROGRESSING, s2wc("正在处理: "s + thes.file));
	thes.lpStartAddress->SetDlgItemText(IDC_STATIC_USEDTIME, s2wc("用时: 00:00:00"s));

	((CProgressCtrl*)thes.lpStartAddress->GetDlgItem(IDC_PROGRESS_TOTAL))->SetPos(100);
	thes.lpStartAddress->EndDialog(0);
	return 0;
}

void CDlgHandling::OnBnClickedCancel(){
	if (MessageBoxW(L"您确定要取消吗?", L"Confirm", MB_ICONQUESTION | MB_YESNO |
		MB_DEFBUTTON2) != IDYES) return; exit(1);
}


void CDlgHandling::OnBnClickedOk(){
	paused = !paused;
	if (paused) {
		::SuspendThread(hThread);
		SetDlgItemTextW(IDOK, L"继续(&R)");
	} else {
		::ResumeThread(hThread);
		SetDlgItemTextW(IDOK, L"暂停(&P)");
	}
}


BOOL CDlgHandling::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (type == SplitDlg) {
		hThread = (HANDLE)_beginthreadex
		(NULL, 0, HandleSplitFile, param_location, CREATE_SUSPENDED, NULL);
		AssertEx_(hThread, __LINE__, __FILE__, true, false);
		AssertEx_(hThread, __LINE__, __FILE__, true, true);
		if (hThread == 0) exit(IDABORT);
		::ResumeThread(hThread);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

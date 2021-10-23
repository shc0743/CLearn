// CDlgExitConf.cpp: 实现文件
//

#include "pch.h"
#include "MFCNginxPhpMgr.h"
#include "CDlgExitConf.h"
#include "afxdialogex.h"

#include"../../resource/tool.h"


// CDlgExitConf 对话框

IMPLEMENT_DYNAMIC(CDlgExitConf, CDialogEx)

CDlgExitConf::CDlgExitConf(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EXIT_CONF, pParent)
{

}

CDlgExitConf::~CDlgExitConf()
{
}

void CDlgExitConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgExitConf, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgExitConf::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgExitConf::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_EXIT_AUTOOK, &CDlgExitConf::OnBnClickedButtonExitAutook)
END_MESSAGE_MAP()


// CDlgExitConf 消息处理程序


void CDlgExitConf::OnBnClickedCancel(){
	EndDialog(IDCANCEL);
}


void CDlgExitConf::OnBnClickedOk(){
	HANDLE hdl = 0,thishdl = GetModuleHandle(0);
	auto inf = ::GetProgramInfo();
	int selfid = GetCurrentProcessId();
	vector<DWORD>* nids = GetProcessIdFromAll(s2ws(inf.name).c_str());
	vector<DWORD>& nidraws = *nids;
	for (auto nid : nidraws) {
		if (nid == selfid) continue;
		hdl = GetProcessHandle(nid);
		TerminateProcess(hdl, 0);
	}
	delete nids;
	EndDialog(123);
}


void CDlgExitConf::OnBnClickedButtonExitAutook(){
	fstream fp("exit-no-ask.bin", ios::out | ios::binary);
	int noask = 1;
	fp.write((char*)&noask, sizeof(int));
	fp.close();
	OnBnClickedOk();
}


BOOL CDlgExitConf::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (!file_exists("exit-no-ask.bin")) {
		fstream fp("exit-no-ask.bin", ios::out | ios::binary);
		int noask = 0;
		fp.write((char*)&noask, sizeof(int));
		fp.close();
	}
	fstream fp("exit-no-ask.bin", ios::in | ios::binary);
	int noask = 0;
	fp.read((char*)&noask, sizeof(int));
	fp.close();

	if (noask) {
		OnBnClickedOk();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


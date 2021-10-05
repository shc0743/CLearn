// CDlgSelectProcess.cpp: 实现文件
//

#include "pch.h"
#include "ProcessSeparateConf.h"
#include "CDlgSelectProcess.h"
#include "afxdialogex.h"
#include "../../resource/tool.h"
#include "ProcessSeparateConfDlg.h"


// CDlgSelectProcess 对话框

IMPLEMENT_DYNAMIC(CDlgSelectProcess, CDialogEx)

CDlgSelectProcess::CDlgSelectProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SELECT, pParent)
{

}

CDlgSelectProcess::~CDlgSelectProcess()
{
}

void CDlgSelectProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELECTPROC, m_list1);
}


BEGIN_MESSAGE_MAP(CDlgSelectProcess, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgSelectProcess::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgSelectProcess::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &CDlgSelectProcess::OnBnClickedButtonFind)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTPROC, &CDlgSelectProcess::OnLbnSelchangeListSelectproc)
//	ON_LBN_KILLFOCUS(IDC_LIST_SELECTPROC, &CDlgSelectProcess::OnLbnKillfocusListSelectproc)
ON_LBN_SELCANCEL(IDC_LIST_SELECTPROC, &CDlgSelectProcess::OnSelcancelListSelectproc)
END_MESSAGE_MAP()


// CDlgSelectProcess 消息处理程序


void CDlgSelectProcess::OnBnClickedCancel(){
	exit(1);
}


void CDlgSelectProcess::OnBnClickedOk() {
	CString str;
	m_list1.GetText(m_list1.GetCurSel(), str);
	wstring ws = str.GetBuffer();
	vector<wstring> wsl;
	str_split(ws, L"|", wsl);
	auto _lvalue_t_0001 = Process.find(atoi_t(wsl[1]));
	memcpy(&CProcessSeparateConfDlg::mainInfo, &_lvalue_t_0001, sizeof(ProcessInfo));
	if (!(CProcessSeparateConfDlg::mainInfo.Handle())) {
		MessageBox(L"进程不存在!", L"ERROR", MB_ICONERROR);
	} else
	EndDialog(0);
}


void CDlgSelectProcess::OnBnClickedButtonFind(){
	Process.flush();
	CString str; UINT num;
	GetDlgItemText(IDC_EDIT_PN, str);
	num = GetDlgItemInt(IDC_EDIT_PID, nullptr, 0);
	if (num) {
		int Count = m_list1.GetCount();
		for (int i = Count; i >= 0; i--) m_list1.DeleteString(i);
		GetDlgItem(IDOK)->EnableWindow(0);
		ProcessInfo pi = Process.find(num);
		if (!pi) {
			MessageBox(L"进程不存在!", L"ERROR", MB_ICONERROR); return;
		}
		m_list1.InsertString(0, (pi.name() + L"|" + to_wstring((DWORD)pi)).c_str());
	}
	else if (str.GetLength()) {
		wstring ws = str.GetBuffer();
		vector<ProcessInfo> pi;
		Process.find(ws, pi);
		if (!pi.size()) {
			MessageBox(L"进程不存在!", L"ERROR", MB_ICONERROR); return;
		}
		UINT cnt = 0;
		for (auto i : pi) {
			m_list1.InsertString(cnt++, (i.name() + L"|" + to_wstring((DWORD)i)).c_str());
		}
	}
	else {
		MessageBox(L"请输入进程名或pid!", L"ERROR", MB_ICONERROR);
	}
}


void CDlgSelectProcess::OnLbnSelchangeListSelectproc(){
	GetDlgItem(IDOK)->EnableWindow(1);
}
void CDlgSelectProcess::OnSelcancelListSelectproc(){
	GetDlgItem(IDOK)->EnableWindow(0);
}

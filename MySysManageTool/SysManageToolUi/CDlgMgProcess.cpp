// CDlgMgProcess.cpp: 实现文件
//

#include "pch.h"
#include "SysManageToolUi.h"
#include "afxdialogex.h"
#include "CDlgMgProcess.h"
using namespace std;


// CDlgMgProcess 对话框

IMPLEMENT_DYNAMIC(CDlgMgProcess, CDialogEx)

CDlgMgProcess::CDlgMgProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PROCESS_MANAGER, pParent)
{

}

CDlgMgProcess::~CDlgMgProcess()
{
}

void CDlgMgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_list);
}


BEGIN_MESSAGE_MAP(CDlgMgProcess, CDialogEx)
END_MESSAGE_MAP()


// CDlgMgProcess 消息处理程序

static DWORD WINAPI ReadProcessData(CDlgMgProcess* dlg) {
	CHAR buffer[1024] = { 0 }; string str, sF, sP, command;
	WriteFile(hPipeConnect, "GetProcessInfo", 15, 0, 0);
	size_t pPos = 1;
	while (1) {
		if (FALSE == ReadFile(hPipeConnect, buffer, 1024, 0, 0)) {
			MessageBox(NULL, LastErrorStrW(), L"ERR", MB_ICONERROR);
			break;
		}
		MessageBoxA(NULL, buffer, "data", 0);
		str = buffer; if (str == "end|") break;
		while (!str.empty()) {
			sF = str.substr(0, str.find("|"));
			sP = str.substr(str.find("|") + 1);

			int pos = dlg->m_list.GetItemCount();
			dlg->m_list.InsertItem(pos, s2wc(sF));
			dlg->m_list.SetItemText(pos, 1, s2wc(sP));

			str.erase(0, str.find("\n"));
		}
		command = "next " + to_string(pPos++);
		command += string(1024 - command.length(), '\0');
		if (!WriteFile(hPipeConnect, command.c_str(), 1024, 0, 0)) {
			MessageBox(NULL, LastErrorStrW(), L"ERR", MB_ICONERROR);
		}
		Sleep(100);
	};
	return 0;
}


BOOL CDlgMgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_list.InsertColumn(0, L"Process Name", 0, 300, 0);
	m_list.InsertColumn(1, L"PID", 0, 100, 0);

	HANDLE hThrd = (HANDLE)_beginthread((_beginthread_proc_type)ReadProcessData, 0, this);
	if (!hThrd) {
		MessageBoxW(LastErrorStrW(), L"ERROR", MB_ICONERROR);
		exit(GetLastError());
	}
	CloseHandle(hThrd);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CDlgConnectToServer.cpp: 实现文件
//

#include "pch.h"
#include "SysManageToolUi.h"
#include "afxdialogex.h"
#include "CDlgConnectToServer.h"


// CDlgConnectToServer 对话框

IMPLEMENT_DYNAMIC(CDlgConnectToServer, CDialogEx)

CDlgConnectToServer::CDlgConnectToServer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONNECTING, pParent)
{

}

CDlgConnectToServer::~CDlgConnectToServer()
{
}

void CDlgConnectToServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConnectToServer, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgConnectToServer::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_STARTSERVER,
		&CDlgConnectToServer::OnBnClickedButtonStartserver)
	ON_MESSAGE(WM_USER + 4, &CDlgConnectToServer::OnUserEndDialog)
END_MESSAGE_MAP()


// CDlgConnectToServer 消息处理程序


void CDlgConnectToServer::OnBnClickedCancel() {
	ExitProcess(1);
}


static DWORD WINAPI Connecter(CDlgConnectToServer* dlg) {
	if (!dlg || (size_t)dlg == 0x8) return 1;
	Sleep(100);
	WaitNamedPipeA("\\\\.\\pipe\\fe72a322-d7e9-40eb-bc36-307d6b7ce86e-sysmanage-pipe", 0);
	hPipeConnect = CreateFileA("\\\\.\\pipe\\fe72a322-d7e9-40eb-bc36-307d6b7ce86e-sysman"
		"age-pipe", GENERIC_READ | GENERIC_WRITE, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hPipeConnect || hPipeConnect == INVALID_HANDLE_VALUE) {
		dlg->SetDlgItemTextW(IDC_STATIC_CONNECT_TEXT, LastErrorStrW());
#pragma warning(push)
#pragma warning(disable: 4302)
		((CStatic*)dlg->GetDlgItem(IDC_STATIC_CONNECT_ICON))->SetIcon(
			LoadIconA(GetModuleHandleA("imageres.dll"), MAKEINTRESOURCEA(IDI_ERROR)));
#pragma warning(pop)
		dlg->GetDlgItem(IDC_BUTTON_STARTSERVER)->EnableWindow(1);
		return 2;
	}
	dlg->SendMessage(WM_USER + 4);
	return 0;
}


BOOL CDlgConnectToServer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HANDLE hThrd = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Connecter, this, 0, 0);
	if (!hThrd) {
		MessageBoxW(LastErrorStrW(), L"FATAL ERROR", MB_ICONERROR);
		exit(GetLastError());
	}
	CloseHandle(hThrd);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgConnectToServer::OnBnClickedButtonStartserver() {
	EndDialog(0);
}


afx_msg LRESULT CDlgConnectToServer::OnUserEndDialog(WPARAM wParam, LPARAM lParam) {
	EndDialog(0);
	return 0;
}

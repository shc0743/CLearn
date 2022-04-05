
// MFCMyProcCtlUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCMyProcCtlUI.h"
#include "MFCMyProcCtlUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <TlHelp32.h>
using namespace std;


CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCMyProcCtlUIDlg 对话框



CMFCMyProcCtlUIDlg::CMFCMyProcCtlUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCMYPROCCTLUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hThread_refresh = NULL;
	//pImageList = new CImageList;
	//pImageList->Create(32, 32, ILC_COLOR32, 0, 1);
}

CMFCMyProcCtlUIDlg::~CMFCMyProcCtlUIDlg() {
	//delete pImageList;
}

void CMFCMyProcCtlUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCS, m_list_procs);
}

BEGIN_MESSAGE_MAP(CMFCMyProcCtlUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_PROCS,
		&CMFCMyProcCtlUIDlg::OnBnClickedButtonRefreshProcs)
	ON_BN_CLICKED(IDC_BUTTON_ATTACH_CONTROLLER,
		&CMFCMyProcCtlUIDlg::OnBnClickedButtonAttachController)
	ON_BN_CLICKED(IDC_BUTTON_DETACH_CONTROLLER,
		&CMFCMyProcCtlUIDlg::OnBnClickedButtonDetachController)
END_MESSAGE_MAP()


// CMFCMyProcCtlUIDlg 消息处理程序

DWORD __stdcall CMFCMyProcCtlUIDlg::Thread_RefreshList(PVOID arg) {
	CMFCMyProcCtlUIDlg* pobj = (CMFCMyProcCtlUIDlg*)arg;
	if (!pobj) return ERROR_INVALID_PARAMETER;

	pobj->GetDlgItem(IDC_BUTTON_ATTACH_CONTROLLER)->EnableWindow(0);
	pobj->GetDlgItem(IDC_BUTTON_DETACH_CONTROLLER)->EnableWindow(0);
	pobj->GetDlgItem(IDC_BUTTON_REFRESH_PROCS)->EnableWindow(0);

	pobj->m_list_procs.DeleteAllItems();
	HANDLE hsnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hsnap) return false;
	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hsnap, &pe);
	int index = 0;
	do {
		//HANDLE hProcess = 
		//	OpenProcess(pe.th32ProcessID, FALSE, PROCESS_QUERY_INFORMATION);
		//BOOL ok = FALSE;
		//if (hProcess) {
		//	TCHAR buffer[2048] = { 0 }; DWORD size = 2047;
		//	QueryFullProcessImageName(hProcess, 0, buffer, &size);
		//	CloseHandle(hProcess);
		//	HMODULE hMod = LoadLibrary(buffer);
		//	if (hMod) {
		//		pobj->pImageList->Add(LoadIcon(hMod, IDI_APPLICATION));
		//		ok = true;
		//	}
		//}
		//if (!ok) {
		//	pobj->pImageList->Add(LoadIcon(NULL, IDI_APPLICATION));
		//}
		pobj->m_list_procs.InsertItem(index, to_wstring(pe.th32ProcessID).c_str());
		pobj->m_list_procs.SetItemText(index, 1, pe.szExeFile);
		index++;
	} while (::Process32Next(hsnap, &pe));
	::CloseHandle(hsnap);
	if (pobj->cl.getopt(L"service-name")) {
		pobj->GetDlgItem(IDC_BUTTON_ATTACH_CONTROLLER)->EnableWindow();
		pobj->GetDlgItem(IDC_BUTTON_DETACH_CONTROLLER)->EnableWindow();
	}
	pobj->GetDlgItem(IDC_BUTTON_REFRESH_PROCS)->EnableWindow();

	return 0;
}

BOOL CMFCMyProcCtlUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_list_procs.InsertColumn(0, L"PID", 0, 80, 0);
	m_list_procs.InsertColumn(1, L"Image Name", 0, 200, 0);

	m_list_procs.InsertItem(0, L"Loading");
	m_list_procs.SetItemText(0, 1, L"Loading datas...");

	//m_list_procs.SetImageList(pImageList, LVSIL_NORMAL);

	cl.parse(GetCommandLineW());
	if (cl.getopt(L"service-name", ServiceName)) {
		CString wt;
		GetWindowText(wt);
		SetWindowTextW((L"[" + ServiceName + L"] - " + wt.GetBuffer()).c_str());
	}

	try_create_reflush_thread:
	hThread_refresh = CreateThread(0, 0, Thread_RefreshList, this, 0, 0);
	if (hThread_refresh) {
		CloseHandle(hThread_refresh);
	} else {
		if (IDRETRY == MessageBox(L"CANNOT CREATE THREAD",
			0, MB_ICONHAND | MB_RETRYCANCEL | MB_DEFBUTTON2))
				goto try_create_reflush_thread;
		else EndDialog(-1);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCMyProcCtlUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCMyProcCtlUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCMyProcCtlUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCMyProcCtlUIDlg::OnBnClickedButtonRefreshProcs() {
	try_create_reflush_thread:
	hThread_refresh = CreateThread(0, 0, Thread_RefreshList, this, 0, 0);
	if (hThread_refresh) {
		CloseHandle(hThread_refresh);
	} else {
		if (IDRETRY == MessageBox(L"Cannot create thread",
			0, MB_ICONHAND | MB_RETRYCANCEL | MB_DEFBUTTON2))
				goto try_create_reflush_thread;
	}
}

bool CMFCMyProcCtlUIDlg::CheckAdminPriv() {
	if (!IsRunAsAdmin()) {
		if (IDRETRY == MessageBoxW(L"Administrator privileges are required to "
			"complete this operation.\nClick [Retry] to retry as an administrator.",
			L"Access is Denied", MB_ICONHAND | MB_RETRYCANCEL)) {
			if ((INT_PTR)ShellExecuteW(m_hWnd, L"runas", s2wc(GetProgramDir()),
				(L"/runas "s + GetCommandLineW()).c_str(), 0, 1) > 32)
				EndDialog(0);
		}
		return false;
	}
	return true;
}


void CMFCMyProcCtlUIDlg::OnBnClickedButtonAttachController() {
	if (!CheckAdminPriv()) return;
	POSITION pos = m_list_procs.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_procs.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			CString text = m_list_procs.GetItemText(nItem, 0);
			DWORD pid = atol(ws2c(text.GetBuffer()));
			
			HANDLE hPipe = NULL;
			WCHAR pipe_name[256]{ 0 };
			DWORD tmp = 0;
			LoadStringW(NULL, IDS_STRING_SVC_CTRLPIPE, pipe_name, 255);
			wcscat_s(pipe_name, L"\\");
			wcscat_s(pipe_name, ServiceName.c_str());
			if (::WaitNamedPipeW(pipe_name, 10000)) {
#pragma warning(push)
#pragma warning(disable: 6001)
				hPipe = ::CreateFileW(pipe_name, GENERIC_WRITE, 0,
					0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
					MessageBoxW(LastErrorStrW().c_str(), NULL, MB_ICONHAND);
					break;
				}
				DWORD dwMode = PIPE_READMODE_MESSAGE;
				(VOID)SetNamedPipeHandleState(
					hPipe,    // pipe handle 
					&dwMode,  // new pipe mode 
					NULL,     // don't set maximum bytes 
					NULL);    // don't set maximum time 
				string str = "Attach-Process-Control /pid=" + to_string(pid);
				::WriteFile(hPipe, str.c_str(), (DWORD)str.length(), &tmp, NULL);
				auto _sub1 = [&] {
					MessageBox((_T("Error: "s) +
						ErrorCodeToString(atol(ws2s(pipe_name).c_str())) +
						TEXT("\nRaw data:") + pipe_name)
						.c_str(), 0, MB_ICONHAND);
				};
				if (ReadFile(hPipe, pipe_name, 255, &tmp, 0)) {
					if (0 == wcscmp(L"0", pipe_name)) {
						MessageBox(ErrorCodeToString(0).c_str(), 0, MB_ICONINFORMATION);
					}
					else _sub1();
				}
				else _sub1();
				::CloseHandle(hPipe);
#pragma warning(pop)
			}
			else {
				MessageBoxW(ErrorCodeToStringW(ERROR_TIMEOUT).c_str(), 0, MB_ICONHAND);
			}
		}
	}
}


void CMFCMyProcCtlUIDlg::OnBnClickedButtonDetachController() {
	if (!CheckAdminPriv()) return;

}

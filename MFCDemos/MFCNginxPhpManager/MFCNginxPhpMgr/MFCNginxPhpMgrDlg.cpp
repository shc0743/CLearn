
// MFCNginxPhpMgrDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCNginxPhpMgr.h"
#include "MFCNginxPhpMgrDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"
#include "CDlgExitConf.h"
#include "CDlgSetup.h"


// CMFCNginxPhpMgrDlg 对话框



CMFCNginxPhpMgrDlg::CMFCNginxPhpMgrDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCNGINXPHPMGR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (!(this->isIconBox)) this->isIconBox = false;
	else                    this->isIconBox =  true;
}

void CMFCNginxPhpMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCNginxPhpMgrDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOWTASK, onShowTask)
	ON_COMMAND(ID_ICON_MAIN, &CMFCNginxPhpMgrDlg::OnIconMain)
	ON_COMMAND(ID_ICON_EXIT, &CMFCNginxPhpMgrDlg::OnIconExit)
	ON_BN_CLICKED(IDC_BUTTON_STOP_WINDOWS, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStopWindows)
	ON_BN_CLICKED(IDC_BUTTON_REST_NGINX2, &CMFCNginxPhpMgrDlg::OnBnClickedButtonRestNginx2)
	ON_BN_CLICKED(IDC_BUTTON_CONF_MYSQL, &CMFCNginxPhpMgrDlg::OnBnClickedButtonConfMysql)
	ON_BN_CLICKED(IDC_BUTTON_PROGRAM_SET, &CMFCNginxPhpMgrDlg::OnBnClickedButtonProgramSet)
	ON_BN_CLICKED(IDC_BUTTON_STOP_NGINX, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStopNginx)
	ON_BN_CLICKED(IDC_BUTTON_START_NGINX, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStartNginx)
	ON_BN_CLICKED(IDC_BUTTON_REST_NGINX, &CMFCNginxPhpMgrDlg::OnBnClickedButtonRestNginx)
	ON_BN_CLICKED(IDC_BUTTON_CONF_NGINX, &CMFCNginxPhpMgrDlg::OnBnClickedButtonConfNginx)
	ON_BN_CLICKED(IDC_BUTTON_START_MYSQL, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStartMysql)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MYSQL, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStopMysql)
	ON_BN_CLICKED(IDC_BUTTON_CONF_PHP, &CMFCNginxPhpMgrDlg::OnBnClickedButtonConfPhp)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PHP, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStopPhp)
	ON_BN_CLICKED(IDC_BUTTON_START_PHP, &CMFCNginxPhpMgrDlg::OnBnClickedButtonStartPhp)
	ON_BN_CLICKED(IDC_BUTTON_REST_PHP, &CMFCNginxPhpMgrDlg::OnBnClickedButtonRestPhp)
	ON_BN_CLICKED(IDC_BUTTON_REST_MYSQL, &CMFCNginxPhpMgrDlg::OnBnClickedButtonRestMysql)
END_MESSAGE_MAP()



void CMFCNginxPhpMgrDlg::ToTray() {
	{
		m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
		m_nid.hWnd = this->m_hWnd;
		m_nid.uID = IDR_MAINFRAME;
		m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		m_nid.uCallbackMessage = WM_SHOWTASK;
		m_nid.hIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));//LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	}
	StrCpy(m_nid.szTip, L"WNMP Manager");//当鼠标放在上面时，所显示的内容
	Shell_NotifyIcon(NIM_ADD, &m_nid);//在托盘区添加图标
}
unsigned long long ClickIconCnt = 0;
void __stdcall ClearClickIconCnt(HWND eghmn, UINT dfghdh, UINT_PTR id, DWORD gdfhn) {
	::KillTimer(eghmn,id);
	ClickIconCnt = 0;
}
LRESULT CMFCNginxPhpMgrDlg::onShowTask(WPARAM wParam, LPARAM lParam) //wParam接收的是图标的ID，lParam接收的是鼠标的行为
{
	if (wParam != IDR_MAINFRAME)
		return 1;
	switch (lParam)
	{
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭”
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置
		CMenu menu;
		menu.LoadMenuW(IDR_MENU_ICON);
		CMenu* lpmenu = menu.GetSubMenu(0);
		//确定弹出式菜单的位置
		SetForegroundWindow();
		lpmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, lpoint->x, lpoint->y, this);
		//资源回收
		HMENU hmenu = lpmenu->Detach();
		lpmenu->DestroyMenu();
		menu.DestroyMenu();
		delete lpoint;
	}
	break;
	case WM_LBUTTONUP://双击左键的处理
	case WM_LBUTTONDBLCLK://双击左键的处理
	{
		//this->ShowWindow(SW_SHOWNORMAL);//简单的显示主窗口
		::SetTimer(NULL, 426638, 2000, ClearClickIconCnt);
		if (ClickIconCnt++) break;
		{
			STRING sConLin_s = L"\"" + s2ws(::GetProgramDir()) + L"\" --gui";
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			if (!::CreateProcess(
				NULL, const_cast<wchar_t*>(sConLin_s.c_str()), // 命令行字符串  
				NULL, NULL, false, 0, NULL, NULL,
				&si, // 决定新进程的主窗体如何显示的STARTUPINFO结构体  
				&pi  // 接收新进程的识别信息的PROCESS_INFORMATION结构体  
			)) {
				::MessageBoxA(nullptr, ("Error: CreateProcess Error: StatusCode:" + to_string(::GetLastError()) +
					"\nAt Creating Process with Command-Line Switch \"--gui").c_str(),
					"Error: CreateProcess Error",
					MB_ICONERROR); exit(::GetLastError());
			}
		} //创建新的GUI进程
	}
	break;
	}
	return 0;
}


// CMFCNginxPhpMgrDlg 消息处理程序

BOOL CMFCNginxPhpMgrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	/*HWND hwnd_oth = ::FindWindow(NULL, L"Windows Nginx Mysql Php GUI Process");
	if (hwnd_oth != NULL) {
		::SetForegroundWindow(hwnd_oth);
	}*/

	if (isIconBox) { 
		ToTray(); 
	}

	if (GetProcessHandle(L"nginx.exe") != 0) {
		GetDlgItem(IDC_STAT_NGINX_OK)->ShowWindow(1);
		GetDlgItem(IDC_STAT_NGINX_NO)->ShowWindow(0);
		GetDlgItem(IDC_BUTTON_STOP_NGINX)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_START_NGINX)->EnableWindow(0);
	}
	if (GetProcessHandle(L"php-cgi.exe") != 0) {
		GetDlgItem(IDC_STAT_PHP_OK)->ShowWindow(1);
		GetDlgItem(IDC_STAT_PHP_NO)->ShowWindow(0);
		GetDlgItem(IDC_BUTTON_STOP_PHP)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_START_PHP)->EnableWindow(0);
	}
	if (ServiceManager.Query(L"MySQL")==ServiceManager.STATUS_START) {
		GetDlgItem(IDC_STAT_MYSQL_OK)->ShowWindow(1);
		GetDlgItem(IDC_STAT_MYSQL_NO)->ShowWindow(0);
		GetDlgItem(IDC_BUTTON_STOP_MYSQL)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_START_MYSQL)->EnableWindow(0);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCNginxPhpMgrDlg::OnPaint()
{
	if (isIconBox) {
		ShowWindow(0);
	}
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
HCURSOR CMFCNginxPhpMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCNginxPhpMgrDlg::OnIconMain(){
	{
		STRING sConLin_s = L"\"" + s2ws(::GetProgramDir()) + L"\" --gui";
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		if (!::CreateProcess(
			NULL, const_cast<wchar_t*>(sConLin_s.c_str()), // 命令行字符串  
			NULL, NULL, false, 0, NULL, NULL,
			&si, // 决定新进程的主窗体如何显示的STARTUPINFO结构体  
			&pi  // 接收新进程的识别信息的PROCESS_INFORMATION结构体  
		)) {
			::MessageBoxA(nullptr, ("Error: CreateProcess Error: StatusCode:" + to_string(::GetLastError()) +
				"\nAt Creating Process with Command-Line Switch \"--gui").c_str(),
				"Error: CreateProcess Error",
				MB_ICONERROR); exit(::GetLastError());
		}
	} //创建新的GUI进程
}


void CMFCNginxPhpMgrDlg::OnIconExit(){
	CDlgExitConf dlg;
	if (dlg.DoModal() == 123) {
		::Shell_NotifyIcon(NIM_DELETE, &m_nid);
		exit(0);
	}
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStopWindows(){
	if (MessageBox(L"确定关闭 Windows 吗?", L"WNMP", MB_ICONQUESTION | MB_YESNOCANCEL) == IDYES) {
		//ExitWindowsEx(EWX_POWEROFF, SHTDN_REASON_MAJOR_APPLICATION);
		system("shutdown -s -t 60");
	}
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonRestNginx2(){
	if (MessageBox(L"重新启动 服务 MySql?", L"WNMP", MB_ICONQUESTION | MB_YESNOCANCEL) == IDYES) {
		//ExitWindowsEx(EWX_POWEROFF, SHTDN_REASON_MAJOR_APPLICATION);
		ServiceManager.ReStart("mysql");
		ReStart();
	}
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonConfMysql(){
	system("start mysql -uroot -p");
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonProgramSet(){
	Process.StartOnly(s2ws("\""+GetProgramDir()+"\" --setup"));
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStopNginx(){
	Process.killall(L"nginx.exe");
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStartNginx(){
	Process.StartOnly(s2ws(progSet.nginx_path));
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonRestNginx(){
	Process.killall(L"nginx.exe");
	Process.StartOnly(s2ws(progSet.nginx_path));
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonConfNginx(){
	string ngf = progSet.nginx_path;
	ngf = ngf.substr(0,ngf.find_last_of("nginx.exe")-9);
	ngf += "\\conf\\nginx.conf";
	string str = "explorer \"" + ngf + "\"";
	Process.StartOnly(s2ws(str));
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStartMysql(){
	UINT i=ServiceManager.Start("mysql");
	if (i != 0) {
		MessageBox((L"错误: "s + LastErrorStrW()).c_str(), L"WNMP", MB_ICONERROR);
	}
	else {
	    //MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	}
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStopMysql(){
	UINT i=ServiceManager.Stop("mysql");
	if (i != 0) {
		MessageBox((L"错误: "s + LastErrorStrW()).c_str(), L"WNMP", MB_ICONERROR);
	}
	else {
	    //MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	}
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonConfPhp(){
	string ngf = progSet.php_path;
	ngf = ngf.substr(0,ngf.find_last_of("php-cgi.exe")-11);
	ngf += "\\php.ini";
	string str = "explorer \"" + ngf + "\"";
	system(str.c_str());
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStopPhp(){
	HANDLE hdl = NULL, thishdl = GetModuleHandle(0);
	{
		vector<DWORD>* nids = GetProcessIdFromAll(L"php-cgi.exe");
		vector<DWORD>& nidraws = *nids;
		for (auto nid : nidraws) {
			hdl = GetProcessHandle(nid);
			TerminateProcess(hdl, 0);
		}
		delete nids;
	}
	{
		vector<DWORD>* nids = GetProcessIdFromAll(s2ws(GetProgramInfo().name).c_str());
		DWORD theid = GetCurrentProcessId();
		vector<DWORD>& nidraws = *nids;
		for (auto nid : nidraws) {
			if (nid == theid) continue;
			hdl = GetProcessHandle(nid);
			TerminateProcess(hdl, 0);
		}
		delete nids;
	}
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	Process.StartOnly(s2ws(GetProgramDir()));
	exit(0);
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonStartPhp()
{
	string ngf = progSet.php_path;
	ngf = ngf.substr(0, ngf.find_last_of("php-cgi.exe") - 11);
	::SetCurrentDirectoryA(ngf.c_str());
	PROCESS_INFORMATION pi = Process.Start_HiddenWindow(s2ws("\"" + progSet.php_path + "\" -b 127.0.0.1:9000 -c \"" + ngf + "\\php.ini\""));
	if (pi.hProcess == NULL) {
		MessageBox((L"错误: 错误ID" + to_wstring(::GetLastError())).c_str(), L"WNMP", MB_ICONERROR); return;
	}
	Sleep(500);
	::SetCurrentDirectoryW(s2ws(::GetProgramInfo().path).c_str());
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	{
		HANDLE hdl = NULL;
		vector<DWORD>* nids = GetProcessIdFromAll(s2ws(GetProgramInfo().name).c_str());
		DWORD theid = GetCurrentProcessId();
		vector<DWORD>& nidraws = *nids;
		for (auto nid : nidraws) {
			if (nid == theid) continue;
			hdl = GetProcessHandle(nid);
			TerminateProcess(hdl, 0);
		}
		delete nids;
	}
	Process.StartOnly(s2ws(GetProgramDir()));
	exit(0);
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonRestPhp(){
	{
		//stop
		HANDLE hdl = NULL;
		vector<DWORD>* nids = GetProcessIdFromAll(L"php-cgi.exe");
		vector<DWORD>& nidraws = *nids;
		for (auto nid : nidraws) {
			hdl = GetProcessHandle(nid);
			TerminateProcess(hdl, 0);
		}
		delete nids;
	}
	string ngf = progSet.php_path;
	ngf = ngf.substr(0, ngf.find_last_of("php-cgi.exe") - 11);
	::SetCurrentDirectoryA(ngf.c_str());
	PROCESS_INFORMATION pi = Process.Start(s2ws("\"" + progSet.php_path + "\" -b 127.0.0.1:9000 -c \"" + ngf + "\\php.ini\""));
	if (pi.hProcess == NULL) {
		MessageBox((L"错误: 错误ID" + to_wstring(::GetLastError())).c_str(), L"WNMP", MB_ICONERROR); return;
	}
	Sleep(500);
	HWND h = ::FindWindow(NULL, s2wc(progSet.php_path));
	if (h != NULL) ::ShowWindow(h, 0);
	::SetCurrentDirectoryW(s2ws(::GetProgramInfo().path).c_str());
	//MessageBox(L"完成。", L"WNMP", MB_ICONINFORMATION);
	ReStart();
}


void CMFCNginxPhpMgrDlg::OnBnClickedButtonRestMysql(){
	UINT i = ServiceManager.Stop("mysql");
	i = ServiceManager.Start("mysql");
	ReStart();
}

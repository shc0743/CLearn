
// MFCNginxPhpMgr.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MFCNginxPhpMgr.h"
#include "MFCNginxPhpMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"
#include "CDlgCheckPhpCgi.h"
#include "CDlgSetup.h"


// CMFCNginxPhpMgrApp

BEGIN_MESSAGE_MAP(CMFCNginxPhpMgrApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFCNginxPhpMgrApp 构造
CmdLine cl;

#if 1-1
#error "ERROR"
#endif

using ::exit;

CMFCNginxPhpMgrApp::CMFCNginxPhpMgrApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	::SetCurrentDirectoryW(s2ws(::GetProgramInfo().path).c_str());
	if (!file_exists("wnmp_conf.bin")) {
		CDlgSetup dlg;
		if (dlg.DoModal() == IDCANCEL) exit(-2);
	}
	else {
		fstream fp;
		char nginx_ph[260];
		fp.open("wnmp_conf_nginx.bin", ios::in);
		fp.getline(nginx_ph, sizeof(nginx_ph));
		fp.close();
		progSet.nginx_path = nginx_ph;
		fp.open("wnmp_conf_php.bin", ios::in);
		fp.getline(nginx_ph, sizeof(nginx_ph));
		fp.close();
		progSet.php_path = nginx_ph;
		fp.open("wnmp_conf_php_t.bin", ios::in|ios::binary);
		int php_b = 0;
		fp.read((char*)&php_b, sizeof(int));
		fp.close();
		progSet.BlockPhpExit = php_b;
		if (file_exists("php_cgi_timer.bin")) {
			fstream fp("php_cgi_timer.bin", ios::in | ios::binary);
			unsigned a = 0;
			fp.read((char*)&a, sizeof(unsigned));
			fp.close();
			progSet.PhpCheck = a;
		} else progSet.PhpCheck = 0;
	}
	cl.parse(::GetCommandLine());

	bool True = true;
	AssertEx_(True,__LINE__,__FILE__,true);

	if (cl.argc() > 2) {
		::MessageBoxA(0, ("ParseError: Unexpected CommandLine Argument Count: " + to_string(cl.argc())).c_str(),
			"ParseError",
			MB_ICONERROR);
		exit(-3);
	}
	if (cl.argc() == 1) { //如果只有一个命令行参数
		//如果不是管理员
		if (!IsRunAsAdmin()) {
			if (MessageBox(0, L"Warning:\nThe program is not running as an administrator.\n\nClick \"Retry\" "
				"to run as admin,or click CANCEL to ignore the warning.", L"Warning",
				MB_ICONWARNING | MB_RETRYCANCEL) == IDRETRY) {
				ostringstream oss;
				oss << "powershell start-process -filepath \"" << GetProgramDir() << "\" -verb runas";
				exit(system(oss.str().c_str()));
			}
		}
		//如果已经存在其他进程...
		vector<DWORD>* pid_list = ::GetProcessIdFromAll(s2ws(::GetProgramInfo().name).c_str());
		if (pid_list->size() > 2) { //有其他进程
			Process.StartOnly(L"\"" + s2ws(::GetProgramDir()) + L"\" --gui"); //创建新的GUI进程
			exit(0); //退出当前进程
		}
		delete pid_list;
		//第一次启动... 
		LPTSTR cWinDir = new TCHAR[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, cWinDir);

		STRING sConLin_s = L"\"" + s2ws(::GetProgramDir()) + L"\" --php-fastcgi";

		//创建亿个新进程  
		if (!Process.StartOnly(sConLin_s)){
			::MessageBoxA(nullptr, ("Error: CreateProcess Error: StatusCode:" + to_string(::GetLastError()) +
				"\nAt Creating Process with Command-Line Switch \"--php-fastcgi").c_str(),
				"Error: CreateProcess Error",
				MB_ICONERROR);exit(::GetLastError());
		} 
		sConLin_s = L"\""+s2ws(::GetProgramDir())+L"\" --ico";
		if (!Process.StartOnly(sConLin_s)){
			::MessageBoxA(nullptr, ("Error: CreateProcess Error: StatusCode:" + to_string(::GetLastError()) +
				"\nAt Creating Process with Command-Line Switch \"--ico").c_str(),
				"Error: CreateProcess Error",
				MB_ICONERROR);exit(::GetLastError());
		}
		sConLin_s = L"\""+s2ws(::GetProgramDir())+L"\" --gui";
		if (!Process.StartOnly(sConLin_s)){
			::MessageBoxA(nullptr, ("Error: CreateProcess Error: StatusCode:" + to_string(::GetLastError()) +
				"\nAt Creating Process with Command-Line Switch \"--ico").c_str(),
				"Error: CreateProcess Error",
				MB_ICONERROR);exit(::GetLastError());
		}
		exit(0);
	}
	if (cl[1] == L"--help") {
		::MessageBoxA(nullptr, 
			"WNMP Help Message\n"
			"========\n"
			"No Command Line Argument for Normal Start.\n"
			"\n\"--ico\" - Show the Task Icon.\n"
			"\"--gui\" - Show Main GUI.\n"
			"\"--php-fastcgi\" - Prevent PHP fastcgi from exiting.\n"
			"\"--setup\" - Open Setup window.\n"
			"\"--reset\" - Reset all program settings.\n"
			,"WNMP Help Message", MB_ICONINFORMATION);
		exit(0);
	}
	if (cl[1] == L"--reset") {
		int ur=::MessageBoxA(nullptr, "Are you want you to continue to reset all program settings?","Question", MB_ICONQUESTION | MB_YESNO);
		if(ur==IDYES){
			{
				//Delete all other process
				HANDLE hdl = 0, thishdl = GetModuleHandle(0);
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
			}
			remove("exit-no-ask.bin");
			remove("php_cgi_timer.bin");
			remove("wnmp_conf.bin");
			remove("wnmp_conf_nginx.bin");
			remove("wnmp_conf_php.bin");
			remove("wnmp_conf_php_t.bin");
			::MessageBoxA(nullptr, "Finished.", "Info", MB_ICONINFORMATION);
		}
		else ::MessageBoxA(nullptr, "Cancelled.", "Info", MB_ICONINFORMATION);
		exit(0);
	}

	wstring arg1 = cl.at(1);
	if (arg1 != L"--php-fastcgi" && arg1 != L"--ico" && arg1 != L"--gui" && arg1 != L"--setup") {
		::MessageBox(nullptr, (L"ParseError: Unexpected Command-Line Token: \""+arg1+L"\"").c_str(),
				L"ParseError",
		MB_ICONERROR);
	}

}


// 唯一的 CMFCNginxPhpMgrApp 对象

CMFCNginxPhpMgrApp theApp;


// CMFCNginxPhpMgrApp 初始化

BOOL CMFCNginxPhpMgrApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("CDlgWNMPManager"));

	do{
		fstream fp;
		char nginx_ph[260];
		fp.open("wnmp_conf_nginx.bin", ios::in);
		fp.getline(nginx_ph, sizeof(nginx_ph));
		fp.close();
		progSet.nginx_path = nginx_ph;
		fp.open("wnmp_conf_php.bin", ios::in);
		fp.getline(nginx_ph, sizeof(nginx_ph));
		fp.close();
		progSet.php_path = nginx_ph;
		fp.open("wnmp_conf_php_t.bin", ios::in | ios::binary);
		int php_b = 0;
		fp.read((char*)&php_b, sizeof(int));
		fp.close();
		progSet.BlockPhpExit = php_b;
		if (file_exists("php_cgi_timer.bin")) {
			fstream fp("php_cgi_timer.bin", ios::in | ios::binary);
			unsigned a = 0;
			fp.read((char*)&a, sizeof(unsigned));
			fp.close();
			progSet.PhpCheck = a;
		}
		else progSet.PhpCheck = 0;
	} while (0);

	//判断命令行参数
	AssertEx_(cl.size() == 2, __LINE__, __FILE__, true);
	if (cl[1] == L"--ico") {
		CMFCNginxPhpMgrDlg dlg;
		m_pMainWnd = &dlg;
		dlg.isIconBox = true;
		dlg.DoModal();
	}
	if (cl[1] == L"--php-fastcgi") {
		CDlgCheckPhpCgi dlg;
		dlg.DoModal();
	}
	if (cl[1] == L"--setup") {
		CDlgSetup dlg;
		dlg.DoModal();
		exit(0);
	}

	CMFCNginxPhpMgrDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = 0;
	if(cl[1] == L"--gui") nResponse = dlg.DoModal();
	if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}



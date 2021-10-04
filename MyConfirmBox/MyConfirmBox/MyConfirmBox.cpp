
// MyConfirmBox.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MyConfirmBox.h"
#include "MyConfirmBoxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"


// CMyConfirmBoxApp

BEGIN_MESSAGE_MAP(CMyConfirmBoxApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyConfirmBoxApp 构造

CMyConfirmBoxApp::CMyConfirmBoxApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMyConfirmBoxApp 对象

CMyConfirmBoxApp theApp;


// CMyConfirmBoxApp 初始化

BOOL CMyConfirmBoxApp::InitInstance()
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
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CmdLine cl(::GetCommandLineW());
	if (cl.getopt(L"help") || cl.argc() < 2) {
		printf("%s Help Document\n========\nUsage: \n\t%s %s\nParamters:\n%s", ws2s(cl[0]).c_str(), ws2s(cl[0]).c_str(),
			"ConfirmText [--help] [--count=] [--btn1=] [--btn2=] [--title=] [--topalert=]",
			"\nConfirmText    nConfirmText"
			"\n--help         Show this help document."
			"\n--count        Set the count of confirm."
			"\n--btni         Set the text of the button[i]."
			"\n--title        Set window title."
			"\n--topalert     Set the top text of the confirm dialog."
		); exit(0);
	}
	size_t confirmtotal = 0;
	wstring btntext1, btntext2, title, top, tex;
	if (cl.getopt(L"count")) {
		wstring cnt; cl.getopt(L"count", cnt); confirmtotal = atoll(ws2s(cnt).c_str());
	}
	if (cl.getopt(L"btn1")) cl.getopt(L"btn1", btntext1);
	if (cl.getopt(L"btn2")) cl.getopt(L"btn2", btntext2);
	if (cl.getopt(L"title")) cl.getopt(L"title", title);
	if (cl.getopt(L"topalert")) cl.getopt(L"topalert", top);
	tex = cl[1];
	

	CMyConfirmBoxDlg dlg;
	m_pMainWnd = &dlg;
	//dlg.Create(IDD_MYCONFIRMBOX_DIALOG);
	dlg.setAlertText(tex.c_str());
	if (!btntext1.empty()) dlg.setTextBtn1(btntext1.c_str());
	if (!btntext2.empty()) dlg.setTextBtn2(btntext2.c_str());
	if (!title.empty()) dlg.setTitle(title.c_str());
	if (!top.empty()) dlg.setTopText(top.c_str());
	if (confirmtotal) dlg.setConfirmTotal(confirmtotal);
	//dlg.ShowWindow(SW_NORMAL);
	dlg.DoModal();

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



// MFCSplitFile.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MFCSplitFile.h"
#include "MFCSplitFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDlgMerge.h"
#include "CDlgHandling.h"


// CMFCSplitFileApp

BEGIN_MESSAGE_MAP(CMFCSplitFileApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFCSplitFileApp 构造

CMFCSplitFileApp::CMFCSplitFileApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMFCSplitFileApp 对象

CMFCSplitFileApp theApp;


// CMFCSplitFileApp 初始化

BOOL CMFCSplitFileApp::InitInstance()
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

	CmdLineA cl(::GetCommandLineA());
	while (cl.argc() > 1) {
		if (cl[1] == "--help" || cl[1] == "/?") {
			stringstream ht; ht << "Help Document\n=============\nUsage: \n\t";
			ht << cl[0] << " [(--help|/?)] [[-G] [(-s -f --of -X [-M])|(-m[ (--files|-M) -o)]]]\n"
				"\nParamters: "
				"\n--help|/? Show this help."
				"\n-G        Use GUI"
				"\n-s        Split File"
				"\n-m        Merge file"
				"\n-f        The path of input file"
				"\n--files   Paths of input files"
				"\n-o        Output file"
				"\n--of      Output folder"
				"\n-X        Max size (Byte)"
				"\n-M        Using meta file";
			fstream fp(".msf_help_document.doc", ios::out);
			fp << ht.str(); fp.close();
			system("type .msf_help_document.doc & pause");
			exit(0);
		}
		if (cl.getopt("G")) {
			if (cl.getopt("m")) {
				vector<string> files; string files_, meta, out;
				bool had_files = cl.getopt("files", files_), had_meta = cl.getopt("M", meta);
				bool had_out = cl.getopt("o", out);
				if (had_files && had_meta) {
					cerr << "\"--files\" and \"-M\" cannot be used at the same time" << endl;
					::MessageBoxW(NULL, L"\"--files\" and \"-M\" cannot be used at the same time",
						L"Argument Error - MFCSplitFile", MB_ICONERROR); exit(1);
				}
				if (!had_files && !had_meta && !had_out) {
					CDlgMerge dlg;
					m_pMainWnd = &dlg;
					dlg.DoModal(); exit(0);
				}
				if (!had_out) {
					cerr << "\"-o\" is a required argument" << endl;
					::MessageBoxW(NULL, L"\"-o\" is a required argument",
						L"Argument Error - MFCSplitFile", MB_ICONERROR); exit(1);
				}

			}
			if (cl.getopt("s")) {
				string file, fold, maxsize_s; size_t maxsize = 0; bool meta = cl.getopt("M");
				cl.getopt("f", file); cl.getopt("of", fold); cl.getopt("X", maxsize_s); 
				if (file.empty() && fold.empty() && maxsize_s.empty()) {
					break;
				}
				if (file.empty() || fold.empty() || maxsize_s.empty()) {
					cerr << "Too few arguments" << endl;
					::MessageBoxW(NULL, L"Too few arguments",
						L"Argument Error - MFCSplitFile", MB_ICONERROR); exit(1);
				}
				CDlgHandling dlg;
				m_pMainWnd = &dlg;
				// dlg.Create(IDD_DIALOG_WAIT_FOR_DOING);
				CreateObjectNew(CDlgHandling_SplitParamStruct, param);
				param->lpStartAddress = &dlg;
				param->file = file; param->outfolder = fold;
				param->maxsize = maxsize; param->meta = meta;
				dlg.param_location = param;
				dlg.type = dlg.SplitDlg;
				dlg.DoModal();
				exit(0);
			}
		}
		break;
	}

	CMFCSplitFileDlg dlg;
	m_pMainWnd = &dlg;
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


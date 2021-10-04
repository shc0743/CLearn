
// ProcessSeparateConfDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ProcessSeparateConf.h"
#include "ProcessSeparateConfDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessSeparateConfDlg 对话框

Process_t::ProcessInfo CProcessSeparateConfDlg::mainInfo;



CProcessSeparateConfDlg::CProcessSeparateConfDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSSEPARATECONF_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessSeparateConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProcessSeparateConfDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_KILL, &CProcessSeparateConfDlg::OnBnClickedButtonKill)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CProcessSeparateConfDlg::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_SUSP, &CProcessSeparateConfDlg::OnBnClickedButtonSusp)
	ON_BN_CLICKED(IDC_BUTTON_RESUME, &CProcessSeparateConfDlg::OnBnClickedButtonResume)
	ON_BN_CLICKED(IDCANCEL, &CProcessSeparateConfDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_KILLTREE, &CProcessSeparateConfDlg::OnBnClickedButtonKilltree)
END_MESSAGE_MAP()


// CProcessSeparateConfDlg 消息处理程序

BOOL CProcessSeparateConfDlg::OnInitDialog()
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

	wstringstream wss;
	wss << L"进程名: " << mainInfo.name() << L"\r\n";
	wss << L"pid: " << mainInfo.id() << L"\r\n";
	SetDlgItemText(IDC_SHOW_PROCESS, wss.str().c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProcessSeparateConfDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CProcessSeparateConfDlg::OnPaint()
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
HCURSOR CProcessSeparateConfDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessSeparateConfDlg::OnBnClickedButtonKill(){
	Process.kill(mainInfo);
	ReStart();
}
void CProcessSeparateConfDlg::OnBnClickedButtonRestart(){
	if (MessageBox(L"确定崩溃此进程?\n此操作可能导致无法预料的后果!!!", L"Alert", MB_ICONQUESTION |
		MB_YESNOCANCEL) != IDYES) return;
	if (Process.crash(mainInfo, 1_b)) {
		MessageBox(L"成功", 0, MB_ICONINFORMATION);
		ReStart();
	}
	else MessageBox(L"失败", 0, MB_ICONERROR);
}
void CProcessSeparateConfDlg::OnBnClickedButtonSusp(){Process.suspend(mainInfo);}
void CProcessSeparateConfDlg::OnBnClickedButtonResume(){Process.resume(mainInfo);}
void CProcessSeparateConfDlg::OnBnClickedCancel() { exit(0); }



void CProcessSeparateConfDlg::OnBnClickedButtonKilltree() {
	if (!Process.CloseAllWindows(mainInfo, 1_b, 4999)) {
		if (GetLastError() == 233) {
			MessageBox(L"进程没有窗口", L"Error", MB_ICONERROR); return;
		}
		MessageBox(L"无法关闭窗口或进程无响应", L"ERROR", MB_ICONERROR);
	}
	else ReStart();
}

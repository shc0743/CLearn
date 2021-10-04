
// MFCMousePlayDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCMousePlay.h"
#include "MFCMousePlayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
using namespace std;


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


// CMFCMousePlayDlg 对话框



CMFCMousePlayDlg::CMFCMousePlayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCMOUSEPLAY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMousePlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCMousePlayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCMousePlayDlg::OnBnClickedOk)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_32771, &CMFCMousePlayDlg::On32771)
	ON_COMMAND(ID_32772, &CMFCMousePlayDlg::On32772)
END_MESSAGE_MAP()


// CMFCMousePlayDlg 消息处理程序

bool isstarted = 0; int dsqid = 0; POINT WinDx,WinXy;

BOOL CMFCMousePlayDlg::OnInitDialog()
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
	SetDlgItemText(IDC_EDIT_CX, L"60");
	SetDlgItemText(IDC_EDIT_CY, L"60");
	SetDlgItemText(IDC_EDIT_X, L"20");
	SetDlgItemText(IDC_EDIT_Y, L"20");
	SetWindowText(L"MFC Mouse Play 主进程");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCMousePlayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE || nID == SC_CLOSE) {
		MessageBox(L"无法完成操作。\n\n拒绝访问。", L"无法关闭窗口", MB_ICONERROR); return;
	}
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

void CMFCMousePlayDlg::OnPaint()
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
		CRect   rc;
		GetClientRect(&rc);// 获取客户区
		CPaintDC dc(this);
		dc.FillSolidRect(&rc, RGB(255, 255, 255));   // 填充客户区颜色
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCMousePlayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	POINT pt;
	::GetCursorPos(&pt);
	int x = pt.x, y = pt.y;
	//::SetCursorPos(x, y);
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, (HWND)-1, x+WinXy.x, y+WinXy.y, WinDx.x, WinDx.y, 0);
}


void CMFCMousePlayDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isstarted) {
		KillTimer(dsqid);
	}
	else {
		dsqid = SetTimer(dsqid, 10, TimerProc);
	}
	isstarted = !isstarted;
	GetDlgItem(IDOK)->EnableWindow(0);
	GetDlgItem(IDCANCEL)->EnableWindow(0);
	GetDlgItem(IDOK)->ShowWindow(0);
	GetDlgItem(IDCANCEL)->ShowWindow(0);
	GetDlgItem(IDC_EDIT_CX)->ShowWindow(0);
	GetDlgItem(IDC_EDIT_CY)->ShowWindow(0);
	GetDlgItem(IDC_EDIT_X)->ShowWindow(0);
	GetDlgItem(IDC_EDIT_Y)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_W)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_H)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_PY)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_x)->ShowWindow(0);
	GetDlgItem(IDC_STATIC_y)->ShowWindow(0);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	pSysMenu->EnableMenuItem(SC_CLOSE, FALSE);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	WinDx.x = GetDlgItemInt(IDC_EDIT_CX);
	WinDx.y = GetDlgItemInt(IDC_EDIT_CY);
	WinXy.x = GetDlgItemInt(IDC_EDIT_X);
	WinXy.y = GetDlgItemInt(IDC_EDIT_Y);
	SetWindowText(L"正在运行 - MFC Mouse Play 主进程");
}


void CMFCMousePlayDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此添加控件通知处理程序代码
	CMenu cMenu; cMenu.LoadMenuW(IDR_MENU_RB);
	CMenu* nMenu = cMenu.GetSubMenu(0);
	POINT pos;
	GetCursorPos(&pos);
	SetForegroundWindow();
	nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
}


string GetProgramDir()
{
	char exeFullPath[MAX_PATH]; // Full path
	string strPath = "";

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	return (string)exeFullPath;    // Get full path of the file
}

void CMFCMousePlayDlg::On32771()
{
	WinExec(::GetProgramDir().c_str(),SW_SHOW);
	exit(IDCANCEL);
}


void CMFCMousePlayDlg::On32772()
{
	exit(IDCANCEL);
}

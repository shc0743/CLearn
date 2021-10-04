
// MFCSendMessageDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCSendMessage.h"
#include "MFCSendMessageDlg.h"
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


// CMFCSendMessageDlg 对话框



CMFCSendMessageDlg::CMFCSendMessageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSENDMESSAGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSendMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCSendMessageDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCSendMessageDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_EXECMAIN, &CMFCSendMessageDlg::OnBnClickedButtonExecmain)
END_MESSAGE_MAP()


// CMFCSendMessageDlg 消息处理程序

BOOL CMFCSendMessageDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCSendMessageDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCSendMessageDlg::OnPaint()
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
HCURSOR CMFCSendMessageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCSendMessageDlg::OnClose() {
	return;
}

void CMFCSendMessageDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}



NOTIFYICONDATA m_Notify;
ULONGLONG GetVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    ULONGLONG dwVersion = 0;
 
    hinstDll = LoadLibrary(lpszDllName);
 
    if (hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
 
        if (pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;
 
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
 
            hr = (*pDllGetVersion)(&dvi);
 
            if (SUCCEEDED(hr))
            {
                dwVersion = MAKEDLLVERULL(dvi.dwMajorVersion, dvi.dwMinorVersion,
                    dvi.dwBuildNumber, dvi.dwPlatformID);
            }
        }
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}
DWORD GetSize_NotifyIconData()
{
	TCHAR lpszDllName[128];

	GetWindowsDirectory(lpszDllName, _TRUNCATE);
	_tcsncat_s(lpszDllName, _T("\\System32\\Shell32.dll"), _TRUNCATE);

	ULONGLONG NIDdllVer = GetVersion(lpszDllName);

	// before windows 2000
	if (NIDdllVer < MAKEDLLVERULL(5, 0, 0, 0)) {
		return NOTIFYICONDATA_V1_SIZE;
	}
	// Windows 2000
	else if (NIDdllVer < MAKEDLLVERULL(6, 0, 0, 0)) {
		return NOTIFYICONDATA_V2_SIZE;
	}
	// Windows XP or 2003
	else if (NIDdllVer < MAKEDLLVERULL(6, 0, 6000, 0)) {
		return NOTIFYICONDATA_V3_SIZE;
	}
	// Windows Vista and later
	else {
		return sizeof(NOTIFYICONDATA);
	}
}
BOOL CreateTray(HWND hWnd, HICON hIcon, UINT uCallbackMessage, LPCTSTR szTitle)
{
	m_Notify.cbSize = GetSize_NotifyIconData();
	m_Notify.hIcon = hIcon;
	m_Notify.hWnd = hWnd;
	m_Notify.uCallbackMessage = uCallbackMessage;
	m_Notify.uVersion = NOTIFYICON_VERSION;
	m_Notify.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_Notify.uID = 1;

	_tcscpy_s(m_Notify.szTip, szTitle);

	return Shell_NotifyIcon(NIM_ADD, &m_Notify);
}
bool ChangeTray(LPCTSTR msg, UINT uTimeout)
{
	m_Notify.uFlags = NIF_INFO;
	m_Notify.dwInfoFlags = NIIF_NONE;
	// m_Notify.uTimeout = uTimeout;

	_tcscpy_s(m_Notify.szInfo, msg);

	return Shell_NotifyIcon(NIM_MODIFY, &m_Notify);
}

void CMFCSendMessageDlg::OnBnClickedButtonExecmain()
{
	// TODO: 在此添加控件通知处理程序代码
	CString msg,tit;
	GetDlgItemText(IDC_EDIT_MAINMSGCONT, msg);
	GetDlgItemText(IDC_EDIT_IFMSTITLE, tit);
	CreateTray(NULL, NULL, NULL, tit.GetBuffer());
	ChangeTray(msg.GetBuffer(),GetDlgItemInt(IDC_EDIT_MSGSLEEP));
}

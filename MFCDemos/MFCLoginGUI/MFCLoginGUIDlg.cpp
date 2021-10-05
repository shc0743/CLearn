
// MFCLoginGUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCLoginGUI.h"
#include "MFCLoginGUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <fstream>
#include "CDlgMainLoginBox.h"
#include "user.h"
#include "resource.h"


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


// CMFCLoginGUIDlg 对话框



CMFCLoginGUIDlg::CMFCLoginGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCLOGINGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCLoginGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabmain);
	DDX_Control(pDX, IDC_PROGRESS_MAIN, m_mprog);
}

BEGIN_MESSAGE_MAP(CMFCLoginGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CMFCLoginGUIDlg::OnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_EXITLOG, &CMFCLoginGUIDlg::OnBnClickedButtonExitlog)
	ON_BN_CLICKED(IDC_BUTTON_CHGEPW, &CMFCLoginGUIDlg::OnBnClickedButtonChgepw)
	ON_BN_CLICKED(IDC_BUTTON_MYINF, &CMFCLoginGUIDlg::OnBnClickedButtonMyinf)
END_MESSAGE_MAP()


// CMFCLoginGUIDlg 消息处理程序

LoginedUser MainUser;
bool isinited = false;
void CALLBACK TimerProcDlgMainInit(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
	if (isinited) return;
	isinited = true;
	std::wstring un = s2ws(MainUser.getUserName());
	AfxGetApp()->m_pMainWnd/*->GetDlgItem(IDC_STATIC_WELNAME)*/->SetDlgItemText(IDC_STATIC_WELNAME, un.c_str());
}


BOOL CMFCLoginGUIDlg::OnInitDialog()
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

	using namespace std;
	SetWindowText(L"用户中心");
	fstream userd;
	userd.open("users.data.pwd", ios::in | ios::binary);
	if (!userd) {
		userd.close();
		if (MessageBox(L"users不存在,是否创建?", L"Login", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			fstream userdcing;
			userdcing.open("users.data.pwd", ios::app | ios::binary);
			userud rootuser("root", "root");
			userdcing.write((char*)&rootuser, sizeof(rootuser));
			userdcing.close();
		}
		else exit(-1);
	}
	if (userd) userd.close();
	fstream luserd;
	luserd.open("login.user.pwd", ios::in | ios::binary);
	if (luserd) {
		luserd.read((char*)&MainUser, sizeof(MainUser));
		luserd.close();
		SetTimer(1487,500,TimerProcDlgMainInit);

		//TODO
		/*m_tabmain.InsertItem(0, "欢迎");  
		m_tabmain.InsertItem(1, "我的信息");  
		m_tabmain.InsertItem(2, "我的隐私"); 
		m_tabmain.InsertItem(3, "我的密码");
        m_welpage.Create (IDD_U_WELCOME,&m_tabmain);
        m_myinfomainbox.Create (IDD_U_MYINFO,&m_tabmain);
		CRect rect;
		m_tabmain.GetClientRect(&rect);
		rect.top += 20;
		rect.bottom -= 4;
		rect.left += 4;
		rect.right -= 4;
		m_welpage.MoveWindow(&rect);
		m_myinfomainbox.MoveWindow(&rect);
		m_welpage.ShowWindow(FALSE);
		m_myinfomainbox.ShowWindow(FALSE);
		m_welpage.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	    m_tabmain.SetCurSel(0);*/
		m_mprog.SetRange(0,100);
		m_mprog.SetPos(100);

		return true;
	}
	CDlgMainLoginBox lm;
	lm.DoModal();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCLoginGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) exit(0);
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

void CMFCLoginGUIDlg::OnPaint() 
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
HCURSOR CMFCLoginGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCLoginGUIDlg::OnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}


void CMFCLoginGUIDlg::OnBnClickedButtonExitlog()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(L"真的要退出登录吗?", L"提示", MB_YESNO | MB_ICONQUESTION) == IDNO) return;
	system("DEL /f /s /q login.user.pwd");
	std::ostringstream oss;
	oss << "powershell Start-Process -FilePath " << ::ws2s(GetCommandLine()) << "";
	system(oss.str().c_str());
	exit(0);
}

#include "CDlgChangePwd.h"
#include "CDlgMyInfo.h"
void CMFCLoginGUIDlg::OnBnClickedButtonChgepw()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgChangePwd cd;
	cd.DoModal();
}


void CMFCLoginGUIDlg::OnBnClickedButtonMyinf()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgMyInfo mi;
	this->ShowWindow(SW_HIDE);
	mi.SetDlgItemText(IDC_SHOW_UNAME,s2ws(MainUser.getUserName()).c_str());
	mi.DoModal();
	this->ShowWindow(SW_SHOW);
}

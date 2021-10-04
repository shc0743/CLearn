
// ProcessesGUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ProcessesGUI.h"
#include "ProcessesGUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"


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


// CProcessesGUIDlg 对话框



CProcessesGUIDlg::CProcessesGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDDProcess_tESGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessesGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, m_list_main);
}

BEGIN_MESSAGE_MAP(CProcessesGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_KILL_NOW, &CProcessesGUIDlg::OnBnClickedButtonKillNow)
	ON_BN_CLICKED(IDC_BUTTON_KILL_CURRENT, &CProcessesGUIDlg::OnBnClickedButtonKillCurrent)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CProcessesGUIDlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CProcessesGUIDlg 消息处理程序

BOOL CProcessesGUIDlg::OnInitDialog()
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
	m_list_main.InsertColumn(0, L"进程名称", 0, 200);
	m_list_main.InsertColumn(1, L"PID", 0, 90);
	m_list_main.InsertColumn(2, L"父进程", 0, 90);

	OnBnClickedButtonRefresh();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProcessesGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CProcessesGUIDlg::OnPaint()
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
HCURSOR CProcessesGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessesGUIDlg::OnBnClickedButtonKillNow(){
	Process.kill(Process.GetCurrentProcess());
}


void CProcessesGUIDlg::OnBnClickedButtonKillCurrent(){
	POSITION pos = m_list_main.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		MessageBox(L"未选择任何项目!", L"Error", MB_ICONERROR); return;
	}
	DWORD_PTR pid=m_list_main.GetItemData((int)pos);
	Process.kill(pid);
	MessageBox(L"完成。", NULL, MB_ICONINFORMATION);
	OnBnClickedButtonRefresh();
}


void CProcessesGUIDlg::OnBnClickedButtonRefresh() {
	m_list_main.DeleteAllItems();
	vector<Process_t::ProcessInfo> pis;
	Process.find(pis);
	ULONG64 cnt = 0;
	for (auto i : pis) {
		m_list_main.InsertItem(cnt, to__str(i).c_str());
		m_list_main.SetItemText(cnt, 1, to_wstring(i.id()).c_str());
		m_list_main.SetItemText(cnt, 2, to_wstring(i.parent()).c_str());
		++cnt;
	}
	m_list_main.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

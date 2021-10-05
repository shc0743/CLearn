
// MFCHideOrShowWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCHideOrShowWindow.h"
#include "MFCHideOrShowWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDlgFindWindow.h"


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


// CMFCHideOrShowWindowDlg 对话框



CMFCHideOrShowWindowDlg::CMFCHideOrShowWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCHIDEORSHOWWINDOW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCHideOrShowWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WINDOWS, m_list_wind);
}

BEGIN_MESSAGE_MAP(CMFCHideOrShowWindowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCHideOrShowWindowDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FINDWINDOW, &CMFCHideOrShowWindowDlg::OnBnClickedButtonFindwindow)
	ON_BN_CLICKED(IDOK, &CMFCHideOrShowWindowDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCHideOrShowWindowDlg 消息处理程序

BOOL CMFCHideOrShowWindowDlg::OnInitDialog()
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
	windowslist.reserve(16);
	m_list_wind.InsertColumn(0, L"Title", 0, 400, 0);
	m_list_wind.InsertColumn(1, L"HWND", 0, 100, 0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCHideOrShowWindowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		return;
	}

	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCHideOrShowWindowDlg::OnPaint()
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
HCURSOR CMFCHideOrShowWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCHideOrShowWindowDlg::OnBnClickedCancel(){
	if (this->windowslist.size()) {
		int res = 
			MessageBox(L"And hidden windows!\nIf you exit directly, these windows will can"
			"not be displayed.\nDo you want to show all hidden windows before exiting?",
			L"Exit Confirm", MB_ICONQUESTION | MB_YESNOCANCEL);
		if (res == IDCANCEL) return;
		if (res == IDYES) {
			for (auto i : windowslist) {
				::ShowWindow(i.hwnd, SW_SHOW);
			}
		}
	}
	EndDialog(0);
}


BOOL CMFCHideOrShowWindowDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_SIZE || pMsg->message == WM_SIZING) {

	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMFCHideOrShowWindowDlg::OnBnClickedButtonFindwindow() {
	CDlgFindWindow w;
	INT_PTR r = w.DoModal();
	if (r == IDOK || r == IDCANCEL || r == 0) return;
	HWND hwnd = reinterpret_cast<HWND>(r);
	TCHAR title[1024] = { 0 };
	WINDOW wd;
	wd.hwnd = hwnd;
	::GetWindowText(hwnd, title, 1024);
	wd.title = title;
	windowslist.push_back(move(wd));
	int item = m_list_wind.GetItemCount();
	m_list_wind.InsertItem(item, title);
	m_list_wind.SetItemText(item, 1, to_wstring(r).c_str());
	::ShowWindow(hwnd, SW_HIDE);
}


void CMFCHideOrShowWindowDlg::OnBnClickedOk(){
	auto& list = m_list_wind;
	POSITION pos = list.GetFirstSelectedItemPosition(); //pos选中的首行位置
	vector<int> allpos; allpos.reserve(16);
	if (pos == NULL) {
		MessageBox(L"Nothing were selected", L"Error", MB_ICONERROR); return;
	}
	else
	{
		while (pos) //如果选择多行
		{
			int nIdx = -1;
			nIdx = list.GetNextSelectedItem(pos);
			if (nIdx >= 0 && nIdx < list.GetItemCount()) {
				allpos.push_back(nIdx);
			}
		}
	}
	for (auto i = allpos.size() - 1; i + 1; --i) {
		auto& d = windowslist[allpos[i]];
		::ShowWindow(d.hwnd, SW_SHOW);
		auto it = windowslist.begin() + allpos[i];
		windowslist.erase(it);
	}
	list.DeleteAllItems();
	for (auto i : windowslist) {
#pragma warning(disable: 4311 4302)
		TCHAR title[1024] = { 0 };
		::GetWindowText(i.hwnd, title, 1024);
		int item = m_list_wind.GetItemCount();
		m_list_wind.InsertItem(item, title);
		m_list_wind.SetItemText(item, 1, to_wstring((int)i.hwnd).c_str());
	}
}

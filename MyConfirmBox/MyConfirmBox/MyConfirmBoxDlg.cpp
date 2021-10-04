
// MyConfirmBoxDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyConfirmBox.h"
#include "MyConfirmBoxDlg.h"
#include "afxdialogex.h"
#include "resource.h"

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


// CMyConfirmBoxDlg 对话框



CMyConfirmBoxDlg::CMyConfirmBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYCONFIRMBOX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	confirmedtotal = 5;
	confirmednow = 0;
	ZeroMemory(&rectDesktop, sizeof(rectDesktop));
	ZeroMemory(&m_initdata, sizeof(m_initdata));
}

void CMyConfirmBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_progress_total);
}

void CMyConfirmBoxDlg::setConfirmTotal(size_t val) {
	confirmedtotal = val;
}

void CMyConfirmBoxDlg::setAlertText(LPCTSTR val) {
	m_initdata.alert_text = val;
}

void CMyConfirmBoxDlg::setTitle(LPCTSTR val) {
	m_initdata.title = val;
}

void CMyConfirmBoxDlg::setTopText(LPCTSTR val) {
	m_initdata.toptext = val;
}

void CMyConfirmBoxDlg::setTextBtn1(LPCTSTR val) {
	m_initdata.textbtn1 = val;
}

void CMyConfirmBoxDlg::setTextBtn2(LPCTSTR val) {
	m_initdata.textbtn2 = val;
}

BEGIN_MESSAGE_MAP(CMyConfirmBoxDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_ACCEPTED, &CMyConfirmBoxDlg::OnBnClickedCheckAccepted)
	ON_BN_CLICKED(IDCANCEL, &CMyConfirmBoxDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMyConfirmBoxDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMyConfirmBoxDlg 消息处理程序

BOOL CMyConfirmBoxDlg::OnInitDialog()
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
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0);
	if(m_initdata.alert_text) SetDlgItemText(IDC_EDIT_SHOWDETAILS, m_initdata.alert_text);
	if(m_initdata.title) SetWindowText(m_initdata.title);
	if(m_initdata.toptext) SetDlgItemText(IDC_STATIC_TOPTEXT, m_initdata.toptext);
	if(m_initdata.textbtn1) SetDlgItemText(IDOK, m_initdata.textbtn1);
	if(m_initdata.textbtn2) SetDlgItemText(IDCANCEL, m_initdata.textbtn2);
	m_progress_total.SetRange(0, 1000);
	wstring str_totaltex = L"ConfirmDialog - Confirmed " + to_wstring(confirmednow) + L" / " + 
		to_wstring(confirmedtotal); SetDlgItemTextW(IDC_STATIC_TOTALANDNOW, str_totaltex.c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyConfirmBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMyConfirmBoxDlg::OnPaint()
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
HCURSOR CMyConfirmBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CMyConfirmBoxDlg::OnBnClickedCheckAccepted() {
	GetDlgItem(IDOK)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK_ACCEPTED))->GetCheck());
}


void CMyConfirmBoxDlg::OnBnClickedCancel() {
	exit(3);
}


void CMyConfirmBoxDlg::OnBnClickedOk() {
	++confirmednow; wstring str = L"ConfirmDialog - Confirmed " + to_wstring(confirmednow) +
		L" / " + to_wstring(confirmedtotal);
	SetDlgItemTextW(IDC_STATIC_TOTALANDNOW, str.c_str());
	GetDlgItem(IDCANCEL)->SetFocus();
	m_progress_total.UpdateData(0);
	int _dbg_tmp_unnamed_var__000001 = int(((double)confirmednow / confirmedtotal) * 1000);
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_TOTAL))->SetPos(_dbg_tmp_unnamed_var__000001);
	int rx = 0, ry = 0; srand((unsigned)(time(0) % rand()));
	RECT wrect; GetWindowRect(&wrect);
	do{
		rx = rand() % rectDesktop.right;
		ry = rand() % rectDesktop.bottom;
	} while (rx > rectDesktop.right - (wrect.right - wrect.left) || rx < 0
		 || ry > rectDesktop.bottom - (wrect.bottom - wrect.top) || ry < 0);
	::SetWindowPos(m_hWnd, 0, rx, ry, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	if (confirmednow >= confirmedtotal) exit(2);
}

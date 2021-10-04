
// MFCMakeRandomHexDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCMakeRandomHex.h"
#include "MFCMakeRandomHexDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include "../resource/tool.h"
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


// CMFCMakeRandomHexDlg 对话框



CMFCMakeRandomHexDlg::CMFCMakeRandomHexDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCMAKERANDOMHEX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMakeRandomHexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCMakeRandomHexDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCMakeRandomHexDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_MAKES, &CMFCMakeRandomHexDlg::OnBnClickedButtonMakes)
END_MESSAGE_MAP()


// CMFCMakeRandomHexDlg 消息处理程序

HWND mainhwnd;
CMenu* sysmenu;

BOOL CMFCMakeRandomHexDlg::OnInitDialog()
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
	mainhwnd = AfxGetMainWnd()->GetSafeHwnd();
	sysmenu = pSysMenu;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCMakeRandomHexDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCMakeRandomHexDlg::OnPaint()
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
HCURSOR CMFCMakeRandomHexDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCMakeRandomHexDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int ws = GetDlgItemInt(IDC_EDIT_WS);
	string res = ""; char ls[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()-=_+{}|[]\\:\";\',./<>?`";
	int rndres = 0; srand(time(0)+(rand()%64));
	int z = (((CComboBox*)GetDlgItem(IDC_COMBO_FROMTO))->GetCurSel())+1;
	for (int i = 0; i < ws; i++) {
		rndres = rand() % z;
		res += ls[rndres];
	}
	CString s(s2ws(res).c_str());
	SetDlgItemText(IDC_SHOW_RES, s);
}


#include<thread>

void MakesRdmNum() {
	HWND& h = mainhwnd;
	fstream fp; LPWSTR fn=L""; GetDlgItemTextW(h,IDC_EDIT_OUTFILE, fn,2047);
	fp.open(ws2s(fn), ios::app); fp << endl;
	int ws = GetDlgItemInt(h,IDC_EDIT_WS,0,0), cnt = GetDlgItemInt(h,IDC_EDIT_MCOUNT,0,0);
	string res = ""; char ls[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()-=_+{}|[]\\:\";\',./<>?`";
	for (int j = 0; j < cnt; j++) {
		res = ""; int rndres = 0; srand(time(0) + (rand() % 64));
		int z = (((CComboBox*)GetDlgItem(h,IDC_COMBO_FROMTO))->GetCurSel()) + 1;
		for (int i = 0; i < ws; i++) {
			rndres = rand() % z;
			res += ls[rndres];  
		}
		fp << res << endl;
	}
	AfxGetMainWnd()->SetDlgItemTextW(IDC_SHOW_MSSTAT, L"状态: 完成!");
	sysmenu->EnableMenuItem(SC_CLOSE, MF_ENABLED);
	AfxGetMainWnd()->GetDlgItem(IDCANCEL)->EnableWindow(0);
}


void CMFCMakeRandomHexDlg::OnBnClickedButtonMakes()
{
	// TODO: 在此添加控件通知处理程序代码
	sysmenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	GetDlgItem(IDCANCEL)->EnableWindow(0);
	SetDlgItemText(IDC_SHOW_MSSTAT, L"状态: 生成中...");
	fstream fp; CString fn; GetDlgItemText(IDC_EDIT_OUTFILE, fn);
	if (fn.GetLength() < 1) {
		MessageBox(L"请输入输出文件名!", L"错误", MB_ICONERROR); return;
	}
	fp.open(ws2s(fn.GetBuffer()), ios::app); fp << endl;
	unsigned int ws = GetDlgItemInt(IDC_EDIT_WS), cnt = GetDlgItemInt(IDC_EDIT_MCOUNT);
	string res = ""; char ls[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()-=_+{}|[]\\:\";\',./<>?`";
	for (int j = 0; j < cnt; j++) {
		res = ""; int rndres = 0; srand(time(0) + (rand() % 64));
		int z = (((CComboBox*)GetDlgItem(IDC_COMBO_FROMTO))->GetCurSel()) + 1;
		for (int i = 0; i < ws; i++) {
			rndres = rand() % z;
			res += ls[rndres];  
		}
		fp << res << endl;
	}
	SetDlgItemText(IDC_SHOW_MSSTAT, L"状态: 完成!");
	sysmenu->EnableMenuItem(SC_CLOSE, MF_ENABLED);
	GetDlgItem(IDCANCEL)->EnableWindow(1);
}

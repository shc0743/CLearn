
// MFCGetWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCGetWindow.h"
#include "MFCGetWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>


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
public:
	afx_msg void OnEnChangeEditZzSaycdJbj();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_ZZ_SAYCD_JBJ, &CAboutDlg::OnEnChangeEditZzSaycdJbj)
END_MESSAGE_MAP()


// CMFCGetWindowDlg 对话框



CMFCGetWindowDlg::CMFCGetWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCGETWINDOW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCGetWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCGetWindowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCGetWindowDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GETHWND, &CMFCGetWindowDlg::OnBnClickedButtonGethwnd)
	ON_BN_CLICKED(IDC_BUTTON_WCLOSE, &CMFCGetWindowDlg::OnBnClickedButtonWclose)
	ON_BN_CLICKED(IDC_BUTTON_MAXING, &CMFCGetWindowDlg::OnBnClickedButtonMaxing)
	ON_BN_CLICKED(IDC_BUTTON_MINING, &CMFCGetWindowDlg::OnBnClickedButtonMining)
	ON_BN_CLICKED(IDC_BUTTON_HIDEW, &CMFCGetWindowDlg::OnBnClickedButtonHidew)
	ON_BN_CLICKED(IDC_BUTTON_WSHOW, &CMFCGetWindowDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SAVETITLE, &CMFCGetWindowDlg::OnClickedButtonSavetitle)
	ON_BN_CLICKED(IDC_BUTTON_CLEARHWND, &CMFCGetWindowDlg::OnBnClickedButtonClearhwnd)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPOS, &CMFCGetWindowDlg::OnBnClickedButtonSavepos)
	ON_BN_CLICKED(IDC_CHECK_ZHIDIN, &CMFCGetWindowDlg::OnBnClickedCheckZhidin)
	ON_BN_CLICKED(IDC_BUTTON_PROGSET, &CMFCGetWindowDlg::OnBnClickedButtonProgset)
	ON_BN_CLICKED(IDC_BUTTON_ALLHWND, &CMFCGetWindowDlg::OnBnClickedButtonAllhwnd)
	ON_BN_CLICKED(IDC_BUTTON_MOREHWND, &CMFCGetWindowDlg::OnBnClickedButtonMorehwnd)
	ON_BN_CLICKED(IDC_BUTTON_ABOUTER, &CMFCGetWindowDlg::OnClickedButtonAbouter)
	ON_BN_CLICKED(IDC_BUTTON_INTODLL, &CMFCGetWindowDlg::OnBnClickedButtonIntodll)
	ON_BN_CLICKED(IDC_BUTTON_RELOADARG, &CMFCGetWindowDlg::OnBnClickedButton1Reloadarg)
	ON_BN_CLICKED(IDC_BUTTON_TOZFIRST, &CMFCGetWindowDlg::OnBnClickedButtonTozfirst)
	ON_BN_CLICKED(IDC_BUTTON_CTRL_ECLASS, &CMFCGetWindowDlg::OnBnClickedButtonCtrlEclass)
	ON_BN_CLICKED(IDC_BUTTON_TONORMAL, &CMFCGetWindowDlg::OnBnClickedButtonTonormal)
	ON_BN_CLICKED(IDC_BUTTON_ACTSOFT, &CMFCGetWindowDlg::OnBnClickedButtonActsoft)
	ON_BN_CLICKED(IDC_CHECK_ENABLEDWINDOW, &CMFCGetWindowDlg::OnBnClickedCheckEnabledwindow)
END_MESSAGE_MAP()


// CMFCGetWindowDlg 消息处理程序
#include<fstream>
#include<sstream>
#include "resource.h"
#include "hwnds.h"
#include "topvar.h"
#include "CDlgVerifyPwd.h"
#include "../resource/tool.h"

BOOL CMFCGetWindowDlg::OnInitDialog()
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
	fstream fp("m.f.c.window.settings-profile.dat", ios::in | ios::binary);
	if(fp) fp.read((char*)&promainset,sizeof(promainset));
	fp.close();

	/*if (!system("MFCActiveSoftware --checkActiveStatus")) {
		SetDlgItemText(IDC_BUTTON_ACTSOFT, L"软件已激活");
		GetDlgItem(IDC_BUTTON_ACTSOFT)->EnableWindow(0);
	}*/

	if (promainset.alwaysrunas && (!IsRunAsAdmin())) {
		ostringstream oss; oss << "Start-Process -FilePath " << ::GetCommandLineA() << " -Verb runAs";
		exit((reinterpret_cast<int>(ShellExecuteA(*new HWND(),"open","PowerShell", oss.str().c_str(),NULL,SW_HIDE)))>32);
	}

	if (promainset.passwd) {
		CDlgVerifyPwd p;
		if (p.DoModal() != 123) exit(1);
	}

	if (promainset.ontop) {
		RECT r1;
		::GetWindowRect(::AfxGetMainWnd()->m_hWnd, &r1);
		CString udtcx, udtcy, udtx, udty;
		udtcx = to_string(r1.right - r1.left).c_str();
		udtcy = to_string(r1.bottom - r1.top).c_str();
		udtx = to_string(r1.left).c_str();
		udty = to_string(r1.top).c_str();
		::SetWindowPos(::AfxGetMainWnd()->m_hWnd,HWND_TOPMOST,
			atoi(ws2s(udtx.GetBuffer()).c_str()),
			atoi(ws2s(udty.GetBuffer()).c_str()),
			atoi(ws2s(udtcx.GetBuffer()).c_str()),
			atoi(ws2s(udtcy.GetBuffer()).c_str()),
			NULL);
	}

	AfxGetMainWnd()->SetWindowText(s2ws(promainset.StartTitle).c_str());

	HICON ico=NULL;
	const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
	//trayi.CreateTray(AfxGetMainWnd()->m_hWnd, ico, WM_TASKBARCREATED,L"Dialog");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCGetWindowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		if(promainset.clickxtohide){
			ShowWindow(SW_HIDE);
			return;
		}
		if (promainset.noaskclose) exit(0);
		if (MessageBox(L"确定退出程序?", L"确定吗?", MB_ICONQUESTION | MB_YESNO) == IDYES)
			exit(0);
		else
			return;
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

void CMFCGetWindowDlg::OnPaint()
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
HCURSOR CMFCGetWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



using namespace std;
void CMFCGetWindowDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (promainset.noaskclose) exit(0);
	if (MessageBox(L"确定退出程序?", L"确定吗?", MB_ICONQUESTION | MB_YESNO) == IDYES)
		exit(0);
	else
		return;
}


#include "CDlgProSet.h"
#include "CDlgAllhwnd.h"
#include "CDlgGetHwnd.h"
#include "CAbouterBox.h"
#include "CDlgHFuncDllInto.h"
void CMFCGetWindowDlg::OnBnClickedButtonGethwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString t,t2=L"";
	GetDlgItemText(IDC_EDIT_WTITLE,t);
	if (t.GetLength()<1) {
	    MessageBox(L"窗口标题为空!此操作将导致意想不到的后果!", L"错误", MB_ICONERROR);
		SetDlgItemText(IDC_STATIC_HWNDSTAT, L"无法获取");
		return;
	}
	uhwnd = ::FindWindow(NULL,t.GetBuffer());
	if (uhwnd == NULL)
		SetDlgItemText(IDC_STATIC_HWNDSTAT, L"获取失败");
	else {
		InitHwndU();
	}
}

void CMFCGetWindowDlg::InitHwndU(){
	CString t2 = L"";
	int StrWnd = reinterpret_cast<int>(uhwnd);
	CString strwnd; strwnd = to_string(StrWnd).c_str();
	SetDlgItemText(IDC_STATIC_HWNDSTAT, strwnd);
	SetDlgItemText(IDC_EDIT_WTITLE,t2);
	char udt[2047];
	::GetWindowTextA(uhwnd, udt, 2047);
	CString udt2(udt);
	SetDlgItemText(IDC_EDIT_WTITLES, udt2);
	AfxGetMainWnd()->GetDlgItem(IDC_BUTTON_GETHWND)->EnableWindow(FALSE);
	AfxGetMainWnd()->GetDlgItem(IDC_BUTTON_CLEARHWND)->EnableWindow(TRUE);
	RECT r1;
	::GetWindowRect(uhwnd, &r1);
	CString udtcx,udtcy,udtx,udty;
	udtcx = to_string(r1.right - r1.left).c_str();
	udtcy = to_string(r1.bottom - r1.top).c_str();
	udtx = to_string(r1.left).c_str();
	udty = to_string(r1.top).c_str();
	SetDlgItemText(IDC_EDIT_POS_CX, udtcx);
	SetDlgItemText(IDC_EDIT_POS_CY, udtcy);
	SetDlgItemText(IDC_EDIT_POS_X, udtx);
	SetDlgItemText(IDC_EDIT_POS_Y, udty);
}


void CMFCGetWindowDlg::OnBnClickedButtonWclose()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(uhwnd,WM_CLOSE,0,0);
	uhwnd = NULL;
	SetDlgItemText(IDC_STATIC_HWNDSTAT, L"未获取");
	//MessageBox(L"操作成功", L"好消息", MB_ICONINFORMATION);
}


void CMFCGetWindowDlg::OnBnClickedButtonMaxing()
{
	// TODO: 在此添加控件通知处理程序代码
    ::ShowWindowAsync(uhwnd, SW_MAXIMIZE); 
}


void CMFCGetWindowDlg::OnBnClickedButtonMining()
{
	// TODO: 在此添加控件通知处理程序代码
    ::ShowWindowAsync(uhwnd, SW_MINIMIZE); 
}


void CMFCGetWindowDlg::OnBnClickedButtonHidew()
{
	// TODO: 在此添加控件通知处理程序代码
    ::ShowWindowAsync(uhwnd, SW_HIDE); 
}


void CMFCGetWindowDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
    ::ShowWindowAsync(uhwnd, SW_SHOW); 
}

void CMFCGetWindowDlg::OnBnClickedButtonClearhwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	uhwnd = NULL;
	SetDlgItemText(IDC_STATIC_HWNDSTAT, L"未获取");
	GetDlgItem(IDC_BUTTON_GETHWND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLEARHWND)->EnableWindow(FALSE);
}


void CMFCGetWindowDlg::OnClickedButtonSavetitle()
{
	// TODO: 在此添加控件通知处理程序代码
	CString a;
	GetDlgItemText(IDC_EDIT_WTITLES, a);
	::SetWindowText(uhwnd, a);
}




void CMFCGetWindowDlg::OnBnClickedButtonSavepos()
{
	// TODO: 在此添加控件通知处理程序代码
	int hzx=(int)GetDlgItemInt(IDC_EDIT_WZINDEX);
	HWND hz=HWND_BOTTOM;
	if (hzx == 0) hz = HWND_TOP;
	if (hzx == -1) hz = HWND_TOPMOST;
	if (hzx == 1) hz = HWND_BOTTOM;
	if (hzx == -2) hz = HWND_NOTOPMOST;
	::SetWindowPos(uhwnd, hz,
		(int)GetDlgItemInt(IDC_EDIT_POS_X), (int)GetDlgItemInt(IDC_EDIT_POS_Y),
		(int)GetDlgItemInt(IDC_EDIT_POS_CX), (int)GetDlgItemInt(IDC_EDIT_POS_CY),(UINT)0);
}


void CMFCGetWindowDlg::OnBnClickedCheckZhidin()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* the = (CButton*)GetDlgItem(IDC_CHECK_ZHIDIN);
	if (the->GetCheck()) {
		SetDlgItemText(IDC_EDIT_WZINDEX,L"-1");
	} else SetDlgItemText(IDC_EDIT_WZINDEX,L"-2");
}


void CMFCGetWindowDlg::OnBnClickedButtonProgset()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgProSet* s;
	s = new CDlgProSet(this);
	s->Create(IDD_DIALOG_PROSET, nullptr);
	s->SetDlgItemText(IDC_SHOW_PROFILE, L"m.f.c.window.settings-profile.dat");
	s->SetDlgItemText(IDC_EDIT_STARTITLE, s2ws(promainset.StartTitle).c_str());
	CString wtex; AfxGetMainWnd()->GetWindowText(wtex);
	s->SetDlgItemText(IDC_EDIT_NOWTITLE, wtex);
	if (!promainset.passwd) {
		s->GetDlgItem(IDC_CHECK_PWD)->EnableWindow(TRUE);
	}
	else {
	    s->GetDlgItem(IDC_CHECK_PWD)->EnableWindow(TRUE);
	    ((CButton*)(s->GetDlgItem(IDC_CHECK_PWD)))->SetCheck(1);
	    s->GetDlgItem(IDC_EDIT_SET_PW)->EnableWindow(TRUE);
	    s->GetDlgItem(IDC_EDIT_SET_PW2)->EnableWindow(TRUE);
	}
	((CButton*)(s->GetDlgItem(IDC_CHECK_SET_ZDWIN)))->SetCheck(promainset.ontop);
	((CButton*)(s->GetDlgItem(IDC_CHECK_SET_RUNAS)))->SetCheck(promainset.alwaysrunas);
	((CButton*)(s->GetDlgItem(IDC_CHECK_SET_AUTOSTART)))->SetCheck(promainset.autorun);
	((CButton*)(s->GetDlgItem(IDC_CHECK_SET_NOALERT)))->SetCheck(promainset.noaskclose);
	((CButton*)(s->GetDlgItem(IDC_CHECK_SET_HIDETO)))->SetCheck(promainset.clickxtohide);
	s->ShowWindow(SW_SHOW);
}


void CMFCGetWindowDlg::OnBnClickedButtonAllhwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgAllhwnd a;
	if (a.DoModal() == 123) InitHwndU();
}


void CMFCGetWindowDlg::OnBnClickedButtonMorehwnd()
{
	// TODO: 在此添加控件通知处理程序代码
#if 0
	CDlgGetHwnd* g;
	g = new CDlgGetHwnd(nullptr);
	g->Create(IDD_DIALOG_GETHWND, nullptr);
	g->ShowWindow(SW_SHOW);
#else
	CDlgGetHwnd g;
	if (g.DoModal() == 123) InitHwndU();
#endif
}


#include "resource.h"
void CMFCGetWindowDlg::OnClickedButtonAbouter()
{
	// TODO: 在此添加控件通知处理程序代码
	CAbouterBox* a;
	a = new CAbouterBox(nullptr);
	a->Create(IDD_ABOUTER, nullptr);
	a->SetDlgItemText(IDC_SHOW_ZZIFMS,L"https://github.com/shc0743/");
	a->ShowWindow(SW_SHOW);
}


void CMFCGetWindowDlg::OnBnClickedButtonIntodll()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgHFuncDllInto* d;
	d = new CDlgHFuncDllInto(nullptr);
	d->Create(IDD_HFUNC_INTODLL,nullptr);
	d->ShowWindow(SW_SHOW);
}


void CAboutDlg::OnEnChangeEditZzSaycdJbj(){}


void CMFCGetWindowDlg::OnBnClickedButton1Reloadarg()
{
	// TODO: 在此添加控件通知处理程序代码 
	InitHwndU();
}


void CMFCGetWindowDlg::OnBnClickedButtonTozfirst(){
	RECT r1;
	::GetWindowRect(uhwnd, &r1);
	CString udtcx, udtcy, udtx, udty;
	udtcx = to_string(r1.right - r1.left).c_str();
	udtcy = to_string(r1.bottom - r1.top).c_str();
	udtx = to_string(r1.left).c_str();
	udty = to_string(r1.top).c_str();
	::SetWindowPos(uhwnd, HWND_TOPMOST,
		atoi(ws2s(udtx.GetBuffer()).c_str()),
		atoi(ws2s(udty.GetBuffer()).c_str()),
		atoi(ws2s(udtcx.GetBuffer()).c_str()),
		atoi(ws2s(udtcy.GetBuffer()).c_str()),
		NULL);
}


#include "CDlgAppCtrlEClass.h"
void CMFCGetWindowDlg::OnBnClickedButtonCtrlEclass()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgAppCtrlEClass* d;
	d = new CDlgAppCtrlEClass(nullptr);
	d->Create(IDD_APP_CTRL_ECLASS, nullptr);
	d->ShowWindow(SW_SHOW);
}


void CMFCGetWindowDlg::OnBnClickedButtonTonormal()
{
	// TODO: 在此添加控件通知处理程序代码
	::ShowWindow(uhwnd, SW_NORMAL);
}


void CMFCGetWindowDlg::OnBnClickedButtonActsoft()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
	int r = (system(/*this->GetSafeHwnd(),"open",*/"title 请勿关闭此窗口,否则无法检测激活状态,需要手动重新点击激活按钮 & start /wait MFCActiveSoftware"/*,NULL,NULL,SW_SHOW*/));
	ShowWindow(SW_SHOW);
	if (!r) { 
		SetDlgItemText(IDC_BUTTON_ACTSOFT, L"软件已激活");
		GetDlgItem(IDC_BUTTON_ACTSOFT)->EnableWindow(0); return;
	} else MessageBox(L"激活失败!", L"错误", MB_ICONERROR);
}


void CMFCGetWindowDlg::OnBnClickedCheckEnabledwindow(){
	::EnableWindow(uhwnd, ((CButton*)GetDlgItem(IDC_CHECK_ENABLEDWINDOW))->GetCheck());
}

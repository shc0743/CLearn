// CDlgCalcMain.cpp: 实现文件
//

#include "pch.h"
#include "MFCCalcYourChineseZodiac.h"
#include "CDlgCalcMain.h"
#include "afxdialogex.h"


// CDlgCalcMain 对话框

IMPLEMENT_DYNAMIC(CDlgCalcMain, CDialogEx)

CDlgCalcMain::CDlgCalcMain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CALCMAIN, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgCalcMain::~CDlgCalcMain()
{
}

void CDlgCalcMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCalcMain, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgCalcMain::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_NOHAS, &CDlgCalcMain::OnBnClickedButtonNextNohas)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_HAS, &CDlgCalcMain::OnBnClickedButtonNextHas)
END_MESSAGE_MAP()


// CDlgCalcMain 消息处理程序
#include "CAboutDlg.h"
#include "resource.h"
#include "topvar.h"
BOOL CDlgCalcMain::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码3


	SetDlgItemText(IDC_SHOW_MYSX, L"鼠 , 牛 , 虎 , 羊 , 狗 , 猪");
	this->SetFocus();
	this->ShowWindow(1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgCalcMain::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE || nID == WM_CLOSE) {
		if (MessageBox(L"确定退出吗?", L"问题", MB_ICONQUESTION | MB_YESNO) == IDYES)
			exit(0);
		else return;
	}
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CDlgAboutDlg dlgAbout;
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

void CDlgCalcMain::OnPaint()
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
HCURSOR CDlgCalcMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDlgCalcMain::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MessageBox(L"确定退出?",L"退出",MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2)==IDYES)
	exit(0);
}



#include <string>
#include <sstream>
#include "CDlgCalcResult.h"
using namespace std;
void CDlgCalcMain::OnBnClickedButtonNextHas()
{
	// TODO: 在此添加控件通知处理程序代码
	string n;
	n = to_string(researchresult);
	if (researchcount == 1) {
		researchresult = 1;
	}
	n += "1";
	researchcount++; 
	if (researchcount != 1) researchresult = atoi(n.c_str());
	wstring ws; 
	ws = L"问题 " + to_wstring(researchcount) + L"/4";
	SetDlgItemText(IDC_SHOW_QCOUNT, ws.c_str());
	switch (researchcount) {
	case 2:
		SetDlgItemText(IDC_SHOW_MYSX, L"牛 , 虎 , 兔 , 龙 , 蛇 , 狗");
		break;
	case 3:
		SetDlgItemText(IDC_SHOW_MYSX, L"牛 , 龙 , 蛇 , 马 , 羊 , 猴");
		break;
	case 4:
		SetDlgItemText(IDC_SHOW_MYSX, L"龙 , 羊 , 猴 , 鸡 , 狗 , 猪");
		break;
	case 5:
	{
		EndDialog(0);
		CDlgCalcResult re;
		re.DoModal();
	}
	default:
		SetDlgItemText(IDC_SHOW_MYSX, L"程序内部错误");
	}
	switch (researchresult) {
	case 111:
	{
		GetDlgItem(IDC_BUTTON_NEXT_HAS)->EnableWindow(FALSE); 
		this->ShowWindow(0);
		CDlgCalcMain c;
		c.OnBnClickedButtonNextNohas();
		break;
	}
	default:
		break;
	}
}

void CDlgCalcMain::OnBnClickedButtonNextNohas()
{
	// TODO: 在此添加控件通知处理程序代码
	string n;
	n = to_string(researchresult);
	if (researchcount == 1) {
		researchresult = 0;
	}
	n += "0";
	researchcount++; 
	if (researchcount != 1) researchresult = atoi(n.c_str());
	wstring ws; 
	ws = L"问题 " + to_wstring(researchcount) + L"/4";
	SetDlgItemText(IDC_SHOW_QCOUNT, ws.c_str());
	switch (researchcount) {
	case 2:
		SetDlgItemText(IDC_SHOW_MYSX, L"牛 , 虎 , 兔 , 龙 , 蛇 , 狗");
		break;
	case 3:
		SetDlgItemText(IDC_SHOW_MYSX, L"牛 , 龙 , 蛇 , 马 , 羊 , 猴");
		break;
	case 4:
		SetDlgItemText(IDC_SHOW_MYSX, L"龙 , 羊 , 猴 , 鸡 , 狗 , 猪");
		break;
	case 5:
	{
		EndDialog(0);
		CDlgCalcResult re;
		re.DoModal();
	}
	default:
		SetDlgItemText(IDC_SHOW_MYSX, L"程序内部错误");
	}
}

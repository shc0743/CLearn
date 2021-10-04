
// MFCMakeSymbolLinkDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCMakeSymbolLink.h"
#include "MFCMakeSymbolLinkDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDlgFinish.h"


// CMFCMakeSymbolLinkDlg 对话框



CMFCMakeSymbolLinkDlg::CMFCMakeSymbolLinkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCMAKESYMBOLLINK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMakeSymbolLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCMakeSymbolLinkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_EN_CHANGE(IDC_EDIT1, &CMFCMakeSymbolLinkDlg::OnEnChangeEdit1)
ON_BN_CLICKED(IDOK, &CMFCMakeSymbolLinkDlg::OnBnClickedOk)
ON_BN_CLICKED(IDC_BUTTON_NEXT, &CMFCMakeSymbolLinkDlg::OnBnClickedButtonNext)
END_MESSAGE_MAP()


// CMFCMakeSymbolLinkDlg 消息处理程序

BOOL CMFCMakeSymbolLinkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetDlgItemText(IDC_SHOW_SRC, s2wc(srcfile));
	SetDlgItemText(IDC_SHOW_OUT, s2wc(destfile));
	if(hlink) SetDlgItemText(IDC_STATIC_MAKETYPE, L"链接类型: 硬链接");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCMakeSymbolLinkDlg::OnPaint()
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
HCURSOR CMFCMakeSymbolLinkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCMakeSymbolLinkDlg::OnBnClickedOk()
{
	GetDlgItem(IDOK)->EnableWindow(0);
	GetDlgItem(IDCANCEL)->EnableWindow(0);
	auto& list = *((CListBox*)GetDlgItem(IDC_LIST_MAKEDETAIL));
	list.AddString(L"Readying...");
	list.AddString(L"正在准备...");
	list.AddString(L"检测外部程序可用性: mklink");
	//if (system("cmd /c mklink /?")) {
	//	MessageBox(L"[错误] 找不到 mklink", L"Error", MB_ICONERROR);
	//	list.AddString(L"[错误] 找不到 mklink"); 
	//	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(1);return;
	//}
	string cl = "mklink ";
	if (dlink) {
		if (hlink)
			cl += "/J ";
		else cl += "/D ";
	}
	if (hlink) cl += "/H ";
	cl = cl + "\"" + destfile + "\" \"" + srcfile + "\" ";
	cl += " >stdout 2>stderr";
	{
		fstream fp("mklink.log", ios::app);
		fp << cl << endl; fp.close();
	}
	list.AddString(s2wc(string("运行外部程序: ") + cl));
	int result = system(cl.c_str());
	if (!result) {
		remove("stdout");remove("stderr");
		list.AddString(L"外部程序退出,状态0");
		list.AddString(L"完成");
		list.AddString(L"Done");
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(1);
		return;
	}
	list.AddString((L"[错误]外部程序未成功退出,错误码" + to_wstring(result)).c_str());
	fstream fp("stderr", ios::in); char buffer[2047]; string error;
	while (fp.getline(buffer, 2047)) { 
		list.AddString(s2wc(buffer)); error += buffer;
	}
	fp.close();
	MessageBox(s2wc(error), L"Error", MB_ICONERROR);
	remove("stdout");remove("stderr");
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(1);
}


void CMFCMakeSymbolLinkDlg::OnBnClickedButtonNext()
{
	ShowWindow(0);
	CDlgFinish f;
	f.DoModal();
}

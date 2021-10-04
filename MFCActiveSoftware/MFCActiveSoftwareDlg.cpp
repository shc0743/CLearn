
// MFCActiveSoftwareDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCActiveSoftware.h"
#include "MFCActiveSoftwareDlg.h"
#include "afxdialogex.h"
#include "activity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDlgMakeDlg.h"
#include "CDlgOkExit.h"
#include "md5.h"


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


// CMFCActiveSoftwareDlg 对话框



CMFCActiveSoftwareDlg::CMFCActiveSoftwareDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCACTIVESOFTWARE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCActiveSoftwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCActiveSoftwareDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCActiveSoftwareDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMFCActiveSoftwareDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TOBUY, &CMFCActiveSoftwareDlg::OnNMClickSyslinkTobuy)
END_MESSAGE_MAP()


// CMFCActiveSoftwareDlg 消息处理程序

CActivitySoftware act;
string prod;

BOOL CMFCActiveSoftwareDlg::OnInitDialog()
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

	fstream fp("act.key", ios::in|ios::binary);
 	if (!fp) {
		fp.close();
		CDlgMakeDlg mk;
		mk.DoModal();
		exit(2);
	}

	fstream okfp("act.ok.key", ios::in | ios::binary);
	string cmdl = ::GetCommandLineA();
	int cmdlpos = cmdl.find_last_of('\\', cmdl.length());
	if(cmdlpos!=cmdl.npos) cmdl = cmdl.substr(cmdlpos,cmdl.length());
	vector<string> cmdlarr;
	str_split(cmdl, " ", cmdlarr);
	if (cmdlarr.size() > 1 && cmdlarr[1] == "--checkActiveStatus") {
		if (okfp) {
			vector<string> lst;
			GetActKeyList(lst);
			char ukay[64];
			okfp.getline(ukay, 64);
			okfp.close();
			for (auto i : lst) {
				if ((string)ukay == i) {
					exit(0);
				}
			}
			if (MessageBox(L"错误!文件遭到非法篡改!\n恢复文件?", L"错误", MB_ICONERROR | MB_YESNO) == IDYES) {
				system("del /f /s /q act.ok.key");
			}
			else
				exit(3);
		}
		else { okfp.close(); exit(1); }
		exit(1);
	}
	if (okfp) {
		vector<string> lst;
		GetActKeyList(lst);
		char ukay[64];
		okfp.getline(ukay, 64);
		okfp.close();
		for (auto i : lst) {
			if ((string)ukay == i) {
				exit(0);
			}
		}
		if (MessageBox(L"错误!文件遭到非法篡改!\n恢复文件?", L"错误", MB_ICONERROR | MB_YESNO) == IDYES) {
			system("del /f /s /q act.ok.key");
		} else
		exit(3);
	} else okfp.close();

	CActivitySoftwareIFMS ifms;
	ActKeyFileToObj(ifms);
	prod = ifms.product;

	SetDlgItemTextA(AfxGetMainWnd()->GetSafeHwnd(), IDC_SHOW_WEL_SOFTNAME, ((string)"欢迎使用 " + prod + "!").c_str());
	SetDlgItemTextA(this->GetSafeHwnd(), IDC_SHOW_ACTIFMS, ((string)"" +
		"软件名称: " + ifms.product +"\r\n"
		"公司名称: " + ifms.company +"\r\n"
		"购买激活码: " + ifms.buyurl +"\r\n"
		"").c_str());
	SetDlgItemTextA(this->GetSafeHwnd(), IDC_SHOW_CRIFMS, ifms.copyright.c_str());

	SetWindowTextA(this->GetSafeHwnd(), ((string)"激活" + prod).c_str());
	fp.close();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCActiveSoftwareDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCActiveSoftwareDlg::OnPaint()
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
HCURSOR CMFCActiveSoftwareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCActiveSoftwareDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(1);
}


string ws2s(wstring wstr)
{
    string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
#include<vector>
#include<algorithm>
void CMFCActiveSoftwareDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString uk=L"";
	GetDlgItemText(IDC_EDIT_ACTCODE, uk);
	if(uk.GetLength()<1) {
		MessageBox(L"请输入激活码!", L"错误", MB_ICONERROR); return;
	}

	string ukay = MD5(ws2s(uk.GetBuffer())).toString();
	vector<string> kls;
	GetActKeyList(kls);
	for (auto k : kls) {
		if (ukay==k) {
			fstream fpok("act.ok.key", ios::out); fpok << ukay; fpok.close();
			CDlgOkExit ok;
			ok.DoModal();
			exit(0);
		}
	}
    MessageBox(L"激活失败!", L"错误", MB_ICONERROR);
}


void CMFCActiveSoftwareDlg::OnNMClickSyslinkTobuy(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CActivitySoftwareIFMS i;
	ActKeyFileToObj(i);
	system(i.buyurl.c_str());
}


BOOL CMFCActiveSoftwareDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}

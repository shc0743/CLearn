
// MFCWorkResultUploadDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCWorkResultUpload.h"
#include "MFCWorkResultUploadDlg.h"
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


// CMFCWorkResultUploadDlg 对话框



CMFCWorkResultUploadDlg::CMFCWorkResultUploadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCWORKRESULTUPLOAD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCWorkResultUploadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Prog);
	DDX_Control(pDX, IDC_BUTTON_FINI, m_finishbtn);
}

BEGIN_MESSAGE_MAP(CMFCWorkResultUploadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STARTSJ, &CMFCWorkResultUploadDlg::OnBnClickedButtonStartsj)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CMFCWorkResultUploadDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_FINI, &CMFCWorkResultUploadDlg::OnClickedButtonFini)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCWorkResultUploadDlg::OnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TOACT, &CMFCWorkResultUploadDlg::OnNMClickSyslinkToact)
END_MESSAGE_MAP()


// CMFCWorkResultUploadDlg 消息处理程序

bool YiJiHuo;

BOOL CMFCWorkResultUploadDlg::OnInitDialog()
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

	YiJiHuo = !system("mfcactivesoftware");
	if (YiJiHuo) SetDlgItemText(IDC_SYSLINK_TOACT, L"已激活");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCWorkResultUploadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		if (MessageBox(L"真的不上缴工资了吗???", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES&&
			MessageBox(L"你确定???", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES&&
			MessageBox(L"我求求你了嘛", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			MessageBox(L"你是个不诚实的人!", L"温馨提示", MB_OK);
			MessageBox(L"我要让你付出代价!", L"老婆", MB_OK);
			MessageBox(L"我要你在这里点足一千下......", L"老婆", MB_OK);
			MessageBox(L"	什么？你开始有点后悔了？", L"老婆", MB_OK);
			MessageBox(L"	何必呢？", L"老婆", MB_OK);
			MessageBox(L"	你当初干什么去了？", L"老婆", MB_OK);
			MessageBox(L"	不原谅你！", L"老婆", MB_OK);
			MessageBox(L"	好从现在开始再点995下......", L"老婆", MB_OK);
			MessageBox(L"	你的手开始累了吗^?", L"老婆", MB_OK);
			MessageBox(L"	什么？你已经没力气了？", L"老婆", MB_OK);
			MessageBox(L"	你一直在求我原谅你啊", L"老婆", MB_OK);
			MessageBox(L"	看来你是真的知道错了", L"老婆", MB_OK);
			MessageBox(L"	下次你还会这么做吗？", L"老婆", MB_OK);
			MessageBox(L"真的不会了？", L"老婆", MB_OK);
			MessageBox(L"那好，今天就放你一马！", L"老婆", MB_OK);
			return;
		}
		else return;
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

void CMFCWorkResultUploadDlg::OnPaint()
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
HCURSOR CMFCWorkResultUploadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <clocale>
#include <sstream>
std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char* _Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
std::wstring s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t* _Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

int lasid;

void CALLBACK TimerProcRunProg2(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
	
};
void CALLBACK TimerProcRunProg(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
	if (lasid >= time(0)) return;
    CProgressCtrl* m_Prog = (CProgressCtrl*)(AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_PROGRESS1));
	int mnow = m_Prog->GetPos();
	if (mnow > 17) {
		lasid = 2147483647;
		if (MessageBox(NULL,L"确定要上缴所有钱钱，不留零花钱吗？", L"你确定?", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) {
			MessageBox(NULL,L"你是个不诚实的人!", L"温馨提示",MB_OK);
			//exit(0);
			CMFCWorkResultUploadDlg a;
			a.OnBnClickedButtonCancel();
			delete a;
			std::ostringstream oss;
			oss << "powershell Start-Process -FilePath \"" << ws2s(GetCommandLine()) << "\"";
			system(oss.str().c_str());
			exit(0);
		}
		Sleep(3000);lasid = 2147483647;
	    AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SYDSQ)->SetWindowText(L"0");
	    (AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_BUTTON_FINI))->EnableWindow(TRUE);
	    (AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
		(AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_BUTTON_CANCEL))->EnableWindow(FALSE);
        m_Prog->SetPos(20);
		((AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_WDQB)))->ShowWindow(FALSE);
		((AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_WDKQB)))->ShowWindow(TRUE);
		((AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_BUTTON1)))->ShowWindow(TRUE);
		MessageBox(NULL,L"上缴完毕!你是世界上最诚实的人!", L"温馨提示",MB_OK);
		return;
	}
	else if (mnow >= 20) {
		
	}
    m_Prog->SetPos(mnow+1);
	std::string ws = (
		mnow==0?"2000":
		mnow==1?"1800":
		mnow==2?"1600":
		mnow==3?"1400":
		mnow==4?"1300":
		mnow==5?"1200":
		mnow==6?"1100":
		mnow==7?"1000":
		mnow==8?"900":
		mnow==9?"850":
		mnow==10?"800":
		mnow==11?"750":
		mnow==12?"700":
		mnow==13?"650":
		mnow==14?"600":
		mnow==15?"500":
		mnow==16?"400":
		mnow==17?"200":
		mnow==18?"100":
		mnow==19?"50":
		mnow==20?"0":"错误"
	);
	AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SYDSQ)->SetWindowText(s2ws(ws).c_str());
	lasid = time(0);
	if(mnow<20) SetTimer(NULL,time(0),1500,TimerProcRunProg);
	//delete m_Prog;
	return;
};
void CMFCWorkResultUploadDlg::tprp() {
	/*int mnow = m_Prog.GetPos();
	if (mnow >= 2000) {
	    m_finishbtn.EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_CANCEL))->EnableWindow(FALSE);
		MessageBox(L"上缴完毕!你是个诚实的人!", L"温馨提示");
		return;
	}
	else if (mnow > 1799) {
		if (MessageBox(L"确定要上缴所有钱钱，不留零花钱吗？", L"你确定?", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) {
			MessageBox(L"你是个不诚实的人!", L"温馨提示");
			exit(0);
		}
	}
    m_Prog.SetPos(mnow+1);
	std::wstring ws;
	ws = (2000-mnow);
	GetDlgItem(IDC_STATIC_SYDSQ)->SetWindowText(ws.c_str());
	SetTimer(55500701,30,TimerProcRunProg);*/
}
void CMFCWorkResultUploadDlg::OnBnClickedButtonStartsj()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Prog.SetRange(0,20);
	SetTimer(55500701,1500,TimerProcRunProg);
	((CButton*)GetDlgItem(IDC_BUTTON_STARTSJ))->EnableWindow(FALSE);
	return;
}


void CMFCWorkResultUploadDlg::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(L"真的不上缴工资了吗???", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES &&
		MessageBox(L"你确定???", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES &&
		MessageBox(L"我求求你了嘛", L"老婆", MB_ICONQUESTION | MB_YESNO) == IDYES) {
		MessageBox(L"你是个不诚实的人!", L"温馨提示", MB_OK);
		MessageBox(L"我要让你付出代价!", L"老婆", MB_OK);
		MessageBox(L"我要你在这里点足一千下......", L"老婆", MB_OK);
		MessageBox(L"	什么？你开始有点后悔了？", L"老婆", MB_OK);
		MessageBox(L"	何必呢？", L"老婆", MB_OK);
		MessageBox(L"	你当初干什么去了？", L"老婆", MB_OK);
		MessageBox(L"	不原谅你！", L"老婆", MB_OK);
		MessageBox(L"	好从现在开始再点995下......", L"老婆", MB_OK);
		MessageBox(L"	你的手开始累了吗^?", L"老婆", MB_OK);
		MessageBox(L"	什么？你已经没力气了？", L"老婆", MB_OK);
		MessageBox(L"	你一直在求我原谅你啊", L"老婆", MB_OK);
		MessageBox(L"	看来你是真的知道错了", L"老婆", MB_OK);
		MessageBox(L"	下次你还会这么做吗？", L"老婆", MB_OK);
		MessageBox(L"真的不会了？", L"老婆", MB_OK);
		MessageBox(L"那好，今天就放你一马！", L"老婆", MB_OK);
		return;
	}
	else return;
}


void CMFCWorkResultUploadDlg::OnClickedButtonFini()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(00);
}


void CMFCWorkResultUploadDlg::OnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	std::ostringstream oss;
	oss << "powershell Start-Process -FilePath \"" << ws2s(GetCommandLine()) << "\"";
	system(oss.str().c_str());
	exit(0);
}


void CMFCWorkResultUploadDlg::OnNMClickSyslinkToact(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	if (YiJiHuo) return;
	YiJiHuo = !system("mfcactivesoftware");
	if (YiJiHuo) SetDlgItemText(IDC_SYSLINK_TOACT, L"已激活");
}

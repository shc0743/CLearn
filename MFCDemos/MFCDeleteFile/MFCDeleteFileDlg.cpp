
// MFCDeleteFileDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCDeleteFile.h"
#include "MFCDeleteFileDlg.h"
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


// CMFCMainDlg 对话框



CMFCMainDlg::CMFCMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDELETEFILE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECTDEL, &CMFCMainDlg::OnBnClickedButtonSelectdel)
	ON_BN_CLICKED(IDC_CHECK_OKDEL, &CMFCMainDlg::OnBnClickedCheckOkdel)
	ON_BN_CLICKED(IDOK, &CMFCMainDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_WILLDEL, &CMFCMainDlg::OnEnChangeEditWilldel)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMFCMainDlg 消息处理程序

BOOL CMFCMainDlg::OnInitDialog()
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
	CmdLineA cl(::GetCommandLineA());
	if (cl.argc() < 2) {
		((CEdit*)GetDlgItem(IDC_EDIT_WILLDEL))->SetReadOnly(0);
		GetDlgItem(IDC_BUTTON_SELECTDEL)->EnableWindow(1);
	}
	else {
		int cnt=0;
		for (auto i : cl) {
			if (!cnt++) continue;
			CString c; GetDlgItemText(IDC_EDIT_WILLDEL, c);
			wstring w = c.GetBuffer(); w += s2ws(i); w += L" "; SetDlgItemText(IDC_EDIT_WILLDEL, w.c_str());
		}
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCMainDlg::OnPaint()
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
HCURSOR CMFCMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCMainDlg::OnBnClickedButtonSelectdel()
{
	CFileDialog dlg(true, 0, 0, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_LONGNAMES);
	if (dlg.DoModal() == IDOK) {
		SetDlgItemText(IDC_EDIT_WILLDEL, dlg.GetPathName());
	}
	ShowWindow(0);
	ShowWindow(5);
}


void CMFCMainDlg::OnBnClickedCheckOkdel()
{
	CString p; GetDlgItemText(IDC_EDIT_WILLDEL, p);
	if (((CButton*)GetDlgItem(IDC_CHECK_OKDEL))->GetCheck()&&p.GetLength())
		GetDlgItem(IDOK)->EnableWindow(1);
	else GetDlgItem(IDOK)->EnableWindow(0);
}


void CMFCMainDlg::OnEnChangeEditWilldel()
{
	CString p; GetDlgItemText(IDC_EDIT_WILLDEL, p);
	if (((CButton*)GetDlgItem(IDC_CHECK_OKDEL))->GetCheck()&&p.GetLength())
		GetDlgItem(IDOK)->EnableWindow(1);
	else GetDlgItem(IDOK)->EnableWindow(0);
}


void CMFCMainDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if ((((CButton*)GetDlgItem(IDC_RADIO_DEL_OPT))->GetCheck())) {
		CString fn; GetDlgItemText(IDC_EDIT_WILLDEL, fn);
		string willrm = ws2s(fn.GetBuffer()).c_str();
		CmdLineA dtr(willrm); bool IG = false;
		for (auto i : dtr) {
			while (1) {
				struct stat s;int res = 1;
				if (stat(i.c_str(), &s) == 0) {
					if (s.st_mode & S_IFDIR) {
						res = system(("rd /s /q \""+i+"\" 2>err.log").c_str());
					}
					else if (s.st_mode & S_IFREG) {
						res = system(("del /f /s /q \""+i+"\" 2>err.log").c_str());
					}
					else {
						MessageBox(s2ws((string)"错误: 无法判断 " + i + " 是文件还是文件夹。").c_str(), L"错误", MB_ICONERROR); break;
					}
				}
				else {
					MessageBox(s2ws((string)"错误: 文件或文件夹 " + i + " 不存在。").c_str(), L"错误", MB_ICONERROR); break;
				}

				if (res == 0) break;
				fstream fp("err.log", ios::in);
				string err;//strerror(errno);
				fp >> err; fp >> err; fp.close();
				
				//fstream fp("err.log", ios::out); fp << err; fp.close();
				int resu = (MessageBox(s2ws((string)"错误: 删除文件时发生致命错误! \n文件(夹): "+i+"\n错误描述:" + err + "\n错误日志: err.log").c_str(), L"错误", MB_ICONERROR | MB_ABORTRETRYIGNORE));
				if (resu == IDABORT) exit(-1);
				if (resu == IDRETRY) continue; 
				if (resu == IDIGNORE) IG = true;
				break;
			}
		}
		if (IG) return;
		MessageBox(((wstring)L"成功删除 " + to_wstring(dtr.argc()) + L" 个文件(夹)。").c_str(), L"提示", MB_ICONINFORMATION);
		exit(0);
	}
	else if ((((CButton*)GetDlgItem(IDC_RADIO_DEL_OPT_I))->GetCheck())) {

	}
	else {
		MessageBox(L"请选择删除方式!", L"错误", MB_ICONERROR); return;
	}
}


void CMFCMainDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	//取得被拖动文件的数目
	for (int i = 0; i < DropCount; i++)
	{
		int NameSize = DragQueryFileA(hDropInfo, i, NULL, 0);
		//取得第i个拖动文件名所占字节数
		HANDLE hHeap = GetProcessHeap();
		char* pName = (LPSTR)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, NameSize++);//根据字节数分配缓冲区
		if (pName == NULL)
		{
			MessageBox(L"给文件名分配暂存空间时出错!", L"错误", MB_ICONERROR);
			return;
		}
		DragQueryFileA(hDropInfo, i, pName, NameSize);
		//把文件名拷贝到缓冲区
		CString s; GetDlgItemText(IDC_EDIT_WILLDEL, s); wstring str = s.GetBuffer(); str += L" \"";
		str += s2ws(pName); str += L"\""; SetDlgItemText(IDC_EDIT_WILLDEL, str.c_str());
		//文件名加入listbox中显示
		HeapFree(hHeap, HEAP_ZERO_MEMORY, pName);
		//释放缓冲区
	}

	CDialogEx::OnDropFiles(hDropInfo);
}

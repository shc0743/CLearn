
// FileLockerUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileLockerUI.h"
#include "FileLockerUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CFileLockerUIDlg 对话框



CFileLockerUIDlg::CFileLockerUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILELOCKERUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileLockerUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOCKED, m_list1);
}

BEGIN_MESSAGE_MAP(CFileLockerUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CFileLockerUIDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CFileLockerUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_AALL, &CFileLockerUIDlg::OnBnClickedCheckAall)
	ON_BN_CLICKED(IDC_BUTTON_UNLOCK, &CFileLockerUIDlg::OnBnClickedButtonUnlock)
	ON_BN_CLICKED(IDC_BUTTON_VIEWLOG, &CFileLockerUIDlg::OnBnClickedButtonViewlog)
END_MESSAGE_MAP()


// CFileLockerUIDlg 消息处理程序

BOOL CFileLockerUIDlg::OnInitDialog()
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
	m_list1.InsertColumn(0, L"文件名", 0, 432);
	m_list1.InsertColumn(1, L"句柄", 0, 100);
	((CButton*)GetDlgItem(IDC_CHECK_AALL))->SetCheck(1);
	OnBnClickedCheckAall();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileLockerUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileLockerUIDlg::OnPaint()
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
HCURSOR CFileLockerUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileLockerUIDlg::OnBnClickedCancel(){
	int mRes = 0;
	if (files.empty()) goto exit;
	mRes = MessageBoxW(L"仍有文件正在锁定!继续退出吗?", L"Exit FileLocker",
		MB_ICONQUESTION | MB_YESNOCANCEL);
	if (mRes == IDYES) goto exit;
	return;
exit:
	exit(0);
}


void CFileLockerUIDlg::OnBnClickedOk()
{
	CString sfp;
	GetDlgItemTextW(IDC_EDIT_FILEPATH, sfp);
	if (sfp.GetLength() < 1) {
		CFileDialog   dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, NULL, NULL);
		DWORD MAXFILE = 16384; //2562 is the max
		TCHAR* pc = new TCHAR[MAXFILE * (MAX_PATH + 1) + 1];
		pc[0] = pc[1] = 0;
		dlg.m_ofn.nMaxFile = MAXFILE;
		dlg.m_ofn.lpstrFile = pc;
		vector<wstring> files;
		INT_PTR iReturn = dlg.DoModal();
		if (iReturn == IDOK)
		{
			int nCount = 0;
			POSITION pos = dlg.GetStartPosition();
			while (pos != NULL)
			{
				CString szPathName = dlg.GetNextPathName(pos);
				files.push_back(szPathName.GetBuffer());
				nCount++;
			}
			//CString str;
			//str.Format("Successfully opened %d files/n", nCount);
			//AfxMessageBox(str);
		}
		delete[]pc;
		wstring ftx;
		for (auto i : files) ftx = ftx + L"\"" + i + L"\" ";
		SetDlgItemTextW(IDC_EDIT_FILEPATH, ftx.c_str()); return;
	}
	CmdLine cl(sfp.GetBuffer());
	HANDLE hf = NULL; 
	bool ignore = ((CButton*)GetDlgItem(IDC_CHECK_IGNORE_WARN))->GetCheck();
	bool log = !((CButton*)GetDlgItem(IDC_CHECK_NO_LOG))->GetCheck();
	fstream fp; bool warned = false;
	if (log) fp.open("./FileLockerUI.log", ios::app);
	for (auto i : cl) {
		hf = CreateFileSimpleW(i.c_str(), GENERIC_READ, OPEN_EXISTING, 0);
		if (hf == NULL || hf == INVALID_HANDLE_VALUE) {
			warned = true;
			if (fp) fp << "[" << time(0) << " ERROR] Cannot CreateFileW: " << ws2s(i) <<
				" : " << ws2s(LastErrorStr()) << endl;
			if(!ignore)MessageBoxW((L"无法打开文件"+i+L": "+LastErrorStr()).c_str(),
				L"FileLockerUI", MB_ICONERROR);
		}
		else {
			try {
				files.insert(make_pair(i, hf));
				m_list1.InsertItem(0, i.c_str());
				m_list1.SetItemText(0, 1, to_wstring(reinterpret_cast<long long>(hf)).c_str());
			}
			catch (exception& e) {
				if (fp) fp << "[" << time(0) << " ERROR] std::exception: " << e.what() << endl;
			}
		}
	}
	if (fp) fp.close();
	SetDlgItemTextW(IDC_EDIT_FILEPATH, L"");
	if (warned) MessageBoxW(L"操作已完成,但出现警告。", L"FileLockerUI", MB_ICONWARNING);
}


void CFileLockerUIDlg::OnBnClickedCheckAall(){
	if (((CButton*)GetDlgItem(IDC_CHECK_AALL))->GetCheck()) {
		GetDlgItem(IDC_COMBO_OPENMETH)->EnableWindow(0);
	}
	else {
		GetDlgItem(IDC_COMBO_OPENMETH)->EnableWindow(1);
	}
}


void CFileLockerUIDlg::OnBnClickedButtonUnlock(){
	vector<wstring> unlocking; CString buf; HANDLE hFile = 0;
	bool ignore = ((CButton*)GetDlgItem(IDC_CHECK_IGNORE_WARN))->GetCheck();
	bool log = !((CButton*)GetDlgItem(IDC_CHECK_NO_LOG))->GetCheck();
	fstream fp; if (log) fp.open("./FileLockerUI.log", ios::app);
	for (int i = 0; i < m_list1.GetItemCount(); i++){
		if (m_list1.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED){
			buf = m_list1.GetItemText(i, 0);
			unlocking.push_back(buf.GetBuffer());
		}
	}
	bool cont = false;
	if (unlocking.size() < 1) { 
		MessageBoxW(L"没有选中任何内容!!", L"Error", MB_ICONERROR);
		return;
	}
	else if (unlocking.size() == 1) {
		cont = (MessageBoxW((L"确定解锁" + unlocking[0] +L"?\n解锁后其他任何程序"
			"将能够访问此文件!!").c_str(), L"解锁文件", MB_ICONQUESTION | MB_YESNO) == IDYES);
	}
	else {
		cont = MessageBoxW((L"确定解锁" +to_wstring(unlocking.size())+ L"个文件?\n解锁后其他任何"
			"程序将能够访问这些文件!!").c_str(), L"解锁文件", MB_ICONQUESTION | MB_YESNO)==IDYES;
	}
	if (!cont) return;
	for (auto i : unlocking) {
		try {
			hFile = files.at(i);
			if (!CloseHandle(hFile)) {
				fp << "[" << time(0) << " ERROR] Cannot CloseHandle " << ws2s(i)
					<< " : " << ws2s(LastErrorStr()) << endl;
				if (!ignore) {
					MessageBoxW((L"无法关闭文件" + i + L": " + LastErrorStr()).c_str(),
						L"ERROR", MB_ICONERROR);
				}
			}
			else {
				files.erase(i);
			}
		}
		catch (exception& e) {
			fp << "[" << time(0) << " ERROR] std::exception : " << e.what() << endl;
		}
	}
	m_list1.DeleteAllItems();
	for (auto i : files) {
		m_list1.InsertItem(0, i.first.c_str());
		m_list1.SetItemText(0, 1, to_wstring(reinterpret_cast<long long>(i.second)).c_str());
	}
	if (fp) fp.close();
}


void CFileLockerUIDlg::OnBnClickedButtonViewlog()
{
	if(file_exists("FileLockerUI.log"))
	Process.StartOnly(L"notepad ./FileLockerUI.log");
}

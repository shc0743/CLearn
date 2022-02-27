
// EulaPopGenerateDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "EulaPopGenerate.h"
#include "EulaPopGenerateDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"
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


// CEulaPopGenerateDlg 对话框



CEulaPopGenerateDlg::CEulaPopGenerateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EULAPOPGENERATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEulaPopGenerateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEulaPopGenerateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_MANIFEST,
		&CEulaPopGenerateDlg::OnBnClickedButtonBrowseManifest)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_BASEFILE,
		&CEulaPopGenerateDlg::OnBnClickedButtonBrowseBasefile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUTFILE,
		&CEulaPopGenerateDlg::OnBnClickedButtonBrowseOutfile)
	ON_BN_CLICKED(IDC_BUTTON_ADD_FILE, 
		&CEulaPopGenerateDlg::OnBnClickedButtonAddFile)
	ON_BN_CLICKED(IDOK, &CEulaPopGenerateDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_MANIFEST_PATH,
		&CEulaPopGenerateDlg::OnEnChangeEditManifestPath)
	ON_BN_CLICKED(IDC_BUTTON_REMOVELISTITEM,
		&CEulaPopGenerateDlg::OnBnClickedButtonRemovelistitem)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_GEN_MANIFEST, &CEulaPopGenerateDlg::OnBnClickedButtonGenManifest)
END_MESSAGE_MAP()


// CEulaPopGenerateDlg 消息处理程序

BOOL CEulaPopGenerateDlg::OnInitDialog()
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
	if (file_exists("EULA64.exe"))
		GetDlgItem(IDC_EDIT_BASE_FILE)->SetWindowTextW(L".\\EULA64.exe");
	else if (file_exists("EULA32.exe"))
		GetDlgItem(IDC_EDIT_BASE_FILE)->SetWindowTextW(L".\\EULA32.exe");

	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_FILES);
	AssertEx(list);
	list->InsertColumn(0, L"Index", 0, 50);
	list->InsertColumn(1, L"File Path", 0, /*ERROR*/422);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEulaPopGenerateDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEulaPopGenerateDlg::OnPaint()
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
HCURSOR CEulaPopGenerateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEulaPopGenerateDlg::OnBnClickedButtonBrowseManifest() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER,
		L"manifest.xml|manifest.xml||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_MANIFEST_PATH, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CEulaPopGenerateDlg::OnBnClickedButtonBrowseBasefile() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER,
		L"Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_BASE_FILE, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CEulaPopGenerateDlg::OnBnClickedButtonBrowseOutfile() {
	CFileDialog openFileDlg(FALSE, L"", L"", OFN_EXPLORER,
		L"Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_OUT_FILE, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CEulaPopGenerateDlg::OnBnClickedButtonAddFile() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER | OFN_ALLOWMULTISELECT,
		L"All Files (*.*)|*.*||", NULL);
	openFileDlg.m_ofn.nMaxFile = 2048;
	WCHAR* buffer = new WCHAR[MAX_PATH * 2048];
	openFileDlg.m_ofn.lpstrFile = buffer;
	openFileDlg.m_ofn.lpstrFile[0] = 0;
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		CString strPath; int cnt = 0;
		CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_FILES);
		POSITION m_Position = openFileDlg.GetStartPosition();
		while(m_Position != NULL) {
			strPath = openFileDlg.GetNextPathName(m_Position);
			cnt = list->GetItemCount();
			list->InsertItem(cnt, to_wstring(cnt+1).c_str());
			list->SetItemText(cnt, 1, strPath);
		}
	}
	ShowWindow(0); ShowWindow(1);
}


void CEulaPopGenerateDlg::OnBnClickedOk() {
	CString ma, ba, ou, it;
	GetDlgItemText(IDC_EDIT_MANIFEST_PATH, ma);
	GetDlgItemText(IDC_EDIT_BASE_FILE, ba);
	GetDlgItemText(IDC_EDIT_OUT_FILE, ou);
	CListCtrl* pli = ((CListCtrl*)GetDlgItem(IDC_LIST_FILES));
	if (!pli) { MessageBoxW(L"ERROR"); return; }
	CListCtrl& li = *pli;
	int lc = pli->GetItemCount();
	if (ma.IsEmpty() || ba.IsEmpty() || ou.IsEmpty() || (lc<1) ||
		(!file_exists(ma.GetBuffer())) || (!file_exists(ba.GetBuffer()))) {
		MessageBoxW(L"1 or more paramter(s) error", L"ERROR", MB_ICONERROR); return;
	}
	if (file_exists(ou.GetBuffer())) {
		if (IDYES != MessageBoxW(L"Output file already exists.\nOverwrite?",
			L"Generator", MB_ICONQUESTION | MB_YESNO)) return;
	}

	wstring cl = L"\""s + ba.GetBuffer() + L"\" --generate --in=\"" +
		ba.GetBuffer() + L"\" \"" + ou.GetBuffer() + L"\" \"" + ma.GetBuffer() + L"\"";
	for (int i = 0; i < lc; ++i) {
		it = pli->GetItemText(i, 1);
		if (it.IsEmpty()) {
			MessageBoxW((L"Invalid data found at " + to_wstring(i)).c_str(),
				L"ERROR", MB_ICONERROR); return;
		}
		cl += L" \""s + it.GetBuffer() + L"\"";
	}

	auto pi = Process.Start_Suspended(cl);
	if (!pi.hProcess) {
		MessageBoxW(L"Cannot create process!", L"ERROR", MB_ICONERROR); return;
	}
	CloseHandle(pi.hThread);
	//MessageBoxW(L"Press [OK] to start...", L"Generator", MB_ICONINFORMATION);
	ShowWindow(0);
	DWORD code = 1;
	Process.resume(pi.hProcess);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &code);
	CloseHandle(pi.hProcess);
	ShowWindow(9);
	if (code) {
		MessageBoxW((L"Error: error code " + to_wstring(code)).c_str(),
			L"ERROR", MB_ICONERROR); return;
	}
	else {
		if (IDOK == MessageBoxW(L"Success\nDo you want to test it?", L"Generator",
			MB_ICONINFORMATION | MB_OKCANCEL | MB_DEFBUTTON2)) {
			ShellExecute(NULL, _T("open"), ou.GetBuffer(), NULL, NULL, SW_NORMAL);
		}
	}
}


void CEulaPopGenerateDlg::OnEnChangeEditManifestPath() {
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CString str; GetDlgItemTextW(IDC_EDIT_MANIFEST_PATH, str);
	GetDlgItem(IDOK)->EnableWindow(IsFileOrDirectory(str.GetBuffer()) == 1);
}


void CEulaPopGenerateDlg::OnBnClickedButtonRemovelistitem() {
	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_FILES); AssertEx(list);
	POSITION pos = list->GetFirstSelectedItemPosition();
	while (pos) {
		int nIndex = list->GetNextSelectedItem(pos);
		list->DeleteItem(nIndex);
		pos = list->GetFirstSelectedItemPosition(); //这步很重要，不然删除不完全
	}
}


void CEulaPopGenerateDlg::OnDropFiles(HDROP hDropInfo) {

	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_FILES);
	int count_droppedfile = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	int cnt = 0;
	for (int i = 0; i < count_droppedfile; ++i) {
		wchar_t filepath[MAX_PATH] = { 0 };
		if (DragQueryFile(hDropInfo, i, filepath, MAX_PATH) > 0) {
			if (IsFileOrDirectory(filepath) != 1) continue;
			cnt = list->GetItemCount();
			list->InsertItem(cnt, to_wstring(cnt+1).c_str());
			list->SetItemText(cnt, 1, filepath);
		}
	}

	CDialogEx::OnDropFiles(hDropInfo);
}


void CEulaPopGenerateDlg::OnBnClickedButtonGenManifest() {
	ShellExecuteA(NULL, "open", GetProgramDir().c_str(),
		"--generate-manifest", NULL, SW_NORMAL);
}

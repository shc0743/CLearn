
// MFCSplitFileDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCSplitFile.h"
#include "MFCSplitFileDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDlgMerge.h"


// CMFCSplitFileDlg 对话框



CMFCSplitFileDlg::CMFCSplitFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSPLITFILE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSplitFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCSplitFileDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCSplitFileDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, &CMFCSplitFileDlg::OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FOLDER, &CMFCSplitFileDlg::OnBnClickedButtonSelectFolder)
	ON_EN_CHANGE(IDC_EDIT_FILE, &CMFCSplitFileDlg::OnEnChangeEditFile)
	ON_EN_CHANGE(IDC_EDIT_OUT, &CMFCSplitFileDlg::OnEnChangeEditOut)
	ON_EN_CHANGE(IDC_EDIT_MAX_SIZE, &CMFCSplitFileDlg::OnEnChangeEditMaxSize)
	ON_CBN_SELCHANGE(IDC_COMBO_MAXSIZE_DW, &CMFCSplitFileDlg::OnCbnSelchangeComboMaxsizeDw)
	ON_BN_CLICKED(IDOK, &CMFCSplitFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_MERGE, &CMFCSplitFileDlg::OnBnClickedButtonMerge)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMFCSplitFileDlg 消息处理程序

BOOL CMFCSplitFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	((CComboBox*)GetDlgItem(IDC_COMBO_MAXSIZE_DW))->SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCSplitFileDlg::OnPaint()
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
HCURSOR CMFCSplitFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCSplitFileDlg::OnBnClickedCancel() {
	exit(1);
}


void CMFCSplitFileDlg::OnBnClickedButtonSelectFile() {
	CString gReadFilePathName;  
	CFileDialog fileDlg(true);  
	if (fileDlg.DoModal() == IDOK)    //弹出对话框  
	{  
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名  
		GetDlgItem(IDC_EDIT_FILE)->SetWindowText(gReadFilePathName);//将路径显示  
	} 
	ShowWindow(0); ShowWindow(1);
}


void CMFCSplitFileDlg::OnBnClickedButtonSelectFolder(){
	CFileFind finder;
	CString path;

	// 获取特定文件夹的LPITEMIDLIST，可以将之理解为HANDLE  
	// 所谓的特定文件夹,你可以用CSIDL_XXX来检索之。  
	LPITEMIDLIST rootLoation;
	HRESULT hr = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &rootLoation);
	if (rootLoation == NULL) {
		// unkown error  
		return;
	}
	// 配置对话框  
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.pidlRoot = rootLoation; // 文件夹对话框之根目录，不指定的话则为我的电脑  
	bi.lpszTitle = _T("选择输出文件夹"); // 可以不指定  
	// bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;  

	// 打开对话框, 有点像DoModal  
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation != NULL) {
		TCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		//MessageBox( targetPath );  
		GetDlgItem(IDC_EDIT_OUT)->SetWindowText(targetPath);//将路径显示  
	}
}


void CMFCSplitFileDlg::OnEnChangeEditFile(){
	CheckAndUpdateStartButton();
}


void CMFCSplitFileDlg::CheckAndUpdateStartButton(){
	CString str; GetDlgItem(IDOK)->EnableWindow(0);
	GetDlgItem(IDC_STATIC_START_STATUS)->EnableWindow(0);
	if (0 == GetDlgItemTextW(IDC_EDIT_FILE, str)) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"未指定输入文件");
		return;
	}
	if (!file_exists(str.GetBuffer())) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"指定的输入文件不存在");
		return;
	}
	if (0 == GetDlgItemTextW(IDC_EDIT_OUT, str)) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"未指定输出文件夹");
		return;
	}
	if (IsFileOrDirectory(str.GetBuffer()) != -1) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"指定的输出文件夹不存在");
		return;
	}
	if (0 == GetDlgItemTextW(IDC_EDIT_MAX_SIZE, str)) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"未指定单个文件最大大小");
		return;
	}
	if (str.GetBuffer() == L"0"s) {
		SetDlgItemTextW(IDC_STATIC_START_STATUS, L"单个文件最大大小不能为0");
		return;
	}
	SetDlgItemTextW(IDC_STATIC_START_STATUS, L"点击[开始(S)]以开始操作。");
	GetDlgItem(IDC_STATIC_START_STATUS)->EnableWindow(1);
	GetDlgItem(IDOK)->EnableWindow(1);
}


void CMFCSplitFileDlg::OnEnChangeEditOut(){
	CheckAndUpdateStartButton();
}


void CMFCSplitFileDlg::OnEnChangeEditMaxSize(){
	CheckAndUpdateStartButton();
}


void CMFCSplitFileDlg::OnCbnSelchangeComboMaxsizeDw(){
	CheckAndUpdateStartButton();
	int index = ((CComboBox*)GetDlgItem(IDC_COMBO_MAXSIZE_DW))->GetCurSel();
	if (index < 0) return; CString str;
	((CComboBox*)GetDlgItem(IDC_COMBO_MAXSIZE_DW))->GetLBText(index, str);
	wstring s = str.GetBuffer(); GetDlgItemTextW(IDC_EDIT_MAX_SIZE, str);
	ULONGLONG maxsize = (ULONGLONG)_wtoll(str.GetBuffer());
	if (s == L"TB") maxsize *= (ULONGLONG)1024 * 1024 * 1024 * 1024;
	if (s == L"GB") maxsize *= (ULONGLONG)1024 * 1024 * 1024;
	if (s == L"MB") maxsize *= (ULONGLONG)1024 * 1024;
	if (s == L"KB") maxsize *= (ULONGLONG)1024;
	SetDlgItemTextW(IDC_EDIT_MAX_SIZE, to_wstring(maxsize).c_str());
	((CComboBox*)GetDlgItem(IDC_COMBO_MAXSIZE_DW))->SetCurSel(0);
}


void CMFCSplitFileDlg::OnBnClickedOk(){
	sstream ss; CString str; GetDlgItemTextW(IDC_EDIT_FILE, str);
	ss << "\"" << GetProgramDir() << "\" -G -s -f\"" << ws2s(str.GetBuffer()) << "\" --of=\"";
	GetDlgItemTextW(IDC_EDIT_OUT, str); ss << ws2s(str.GetBuffer()) << "\" -X";
	GetDlgItemTextW(IDC_EDIT_MAX_SIZE, str); size_t max_byte = (size_t)_wtoll(str.GetBuffer());
	ss << max_byte; if (((CButton*)GetDlgItem(IDC_CHECK_ADD_META))->GetCheck()) ss << " -M";

	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
#pragma warning(push)
#pragma warning(disable: 6335)
	BOOL r = ::CreateProcessA(
		NULL, const_cast<CHAR*>(ss.str().c_str()), // 命令行字符串  
		NULL, NULL, false, CREATE_SUSPENDED, NULL, NULL,
		&si, &pi);
#pragma warning(pop)
	if (!r) {
		MessageBoxW(L"进程启动失败!!", L"错误", MB_ICONERROR); return;
	}
	ShowWindow(0);
	Process.resume(pi.hProcess);
	::WaitForSingleObject(pi.hProcess, MAXDWORD);
	DWORD exitCode = 1;
	::GetExitCodeProcess(pi.hProcess, &exitCode);
	Process.CloseProcessHandle(pi);
	if (exitCode == 0) exit(0);
	ShowWindow(1);
}


void CMFCSplitFileDlg::OnBnClickedButtonMerge(){
	ShowWindow(0);
	CDlgMerge dlg;
	dlg.DoModal();
	ShowWindow(1);
}


void CMFCSplitFileDlg::OnDropFiles(HDROP hDropInfo)
{
    //获取文件路径
    TCHAR szPath[MAX_PATH] = { 0 };
    DragQueryFile(hDropInfo, 0, szPath, MAX_PATH);
	auto fod = IsFileOrDirectory(szPath);
	if (fod == 1) SetDlgItemTextW(IDC_EDIT_FILE, szPath);
	else if (fod == -1) SetDlgItemTextW(IDC_EDIT_OUT, szPath);


	//CDialogEx::OnDropFiles(hDropInfo);
}

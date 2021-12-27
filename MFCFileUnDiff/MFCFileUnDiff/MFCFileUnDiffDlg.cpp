
// MFCFileUnDiffDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCFileUnDiff.h"
#include "MFCFileUnDiffDlg.h"
#include "afxdialogex.h"
#include <vector>
using namespace std;
#include "../../resource/tool.h"
#include "sha256.h"
#include "CDlgWait_1.h"

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


// CMFCFileUnDiffDlg 对话框



CMFCFileUnDiffDlg::CMFCFileUnDiffDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCFILEUNDIFF_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCFileUnDiffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_METHOD, m_combo_method);
	DDX_Control(pDX, IDC_LIST_FILES, m_list_files);
}

BEGIN_MESSAGE_MAP(CMFCFileUnDiffDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &CMFCFileUnDiffDlg::OnBnClickedButton1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_TARGET, &CMFCFileUnDiffDlg::OnBnClickedButtonTarget)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILES, &CMFCFileUnDiffDlg::OnNMRClickListFiles)
	ON_COMMAND(ID_CONTEXTMENU_32771, &CMFCFileUnDiffDlg::OnContextmenu32771)
	ON_COMMAND(ID_CONTEXTMENU_32772, &CMFCFileUnDiffDlg::OnContextmenu32772)
	ON_COMMAND(ID_CONTEXTMENU_32773, &CMFCFileUnDiffDlg::OnContextmenu32773)
	ON_BN_CLICKED(IDOK, &CMFCFileUnDiffDlg::OnBnClickedOk)
	ON_COMMAND(ID_CONTEXTMENU_32774, &CMFCFileUnDiffDlg::OnContextmenu32774)
	ON_COMMAND(ID_CONTEXTMENU_32775, &CMFCFileUnDiffDlg::OnContextmenu32775)
	ON_COMMAND(ID_CONTEXTMENU_32776, &CMFCFileUnDiffDlg::OnContextmenu32776)
	ON_COMMAND(ID_CONTEXTMENU_32777, &CMFCFileUnDiffDlg::OnContextmenu32777)
	ON_COMMAND(ID_CONTEXTMENU_32778, &CMFCFileUnDiffDlg::OnContextmenu32778)
END_MESSAGE_MAP()


// CMFCFileUnDiffDlg 消息处理程序

BOOL CMFCFileUnDiffDlg::OnInitDialog()
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
	m_combo_method.SetCurSel(0);
	m_list_files.InsertColumn(0, L"标识", 0, 50);
	m_list_files.InsertColumn(1, L"文件位置", 0, 420);
	m_list_files.InsertColumn(2, L"SHA-256", 0, 200);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCFileUnDiffDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCFileUnDiffDlg::OnPaint()
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
HCURSOR CMFCFileUnDiffDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCFileUnDiffDlg::OnBnClickedButton1()
{
    CString FilePathName;
    CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
        NULL, 
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        (LPCTSTR)_TEXT("所有文件 (*.*)|*.*||"),
        NULL);
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName(); //文件名保存在了FilePathName里
		m_list_files.InsertItem(m_list_files.GetItemCount(), L"-");
		m_list_files.SetItemText(m_list_files.GetItemCount() - 1,
			1, FilePathName.GetBuffer());
		Sha256ListItem(m_list_files.GetItemCount() - 1);
	}
	ShowWindow(0); ShowWindow(1);
}


void CMFCFileUnDiffDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// https://www.cnblogs.com/nonebutnow/p/6009005.html
    TCHAR szPath[MAX_PATH] = {0};
    UINT nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    for(UINT i = 0; i < nCount; i++) 
    {
        DragQueryFile(hDropInfo, i, szPath, MAX_PATH);
		m_list_files.InsertItem(m_list_files.GetItemCount(), L"-");
		m_list_files.SetItemText(m_list_files.GetItemCount() - 1, 1, szPath);
		Sha256ListItem(m_list_files.GetItemCount() - 1);
    }

	CDialogEx::OnDropFiles(hDropInfo);
}


void CMFCFileUnDiffDlg::OnBnClickedButtonTarget() {
	CString szHash, szText;
	GetDlgItemTextW(IDC_EDIT_HASHVALUE, szHash);
	int cnt = m_list_files.GetItemCount();
	for (int i = 0; i < cnt; ++i) {
		szText = m_list_files.GetItemText(i, 2);
		if (szHash.MakeUpper() == szText) m_list_files.SetItemText(i, 0, L"√");
		else m_list_files.SetItemText(i, 0, L"-");
	}
}


void CMFCFileUnDiffDlg::OnNMRClickListFiles(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (m_list_files.GetSelectionMark() != -1)
    {
		CMenu menu; menu.LoadMenuW(IDR_MENU_LISTCONTEXTMENU);
        CMenu *nMenu = menu.GetSubMenu(0);
        POINT pos; GetCursorPos(&pos);
		nMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
    }

	*pResult = 0;
}


void CMFCFileUnDiffDlg::OnContextmenu32771() { // clear state
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			m_list_files.SetItemText(nItem, 0, L"-");
		}
	}
}



void CMFCFileUnDiffDlg::OnContextmenu32772() { // remove
	vector<int> items_to_remove;
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			items_to_remove.push_back(nItem);
		}
	}
	if (items_to_remove.size())
	for (long long i = items_to_remove.size() - 1; i >= 0; --i) {
		m_list_files.DeleteItem(items_to_remove[i]);
	}
}


void CMFCFileUnDiffDlg::OnContextmenu32773() { // delete
	if (MessageBoxW(L"将删除选中的文件。", 0, MB_ICONWARNING | MB_YESNO) != IDYES) return;
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			retry:
			if (!DeleteFileW(m_list_files.GetItemText(nItem, 1).GetBuffer())) {
				auto r = MessageBoxW((L"Exception while deleting "s + 
					m_list_files.GetItemText(nItem, 1).GetBuffer() + L":\n" +
					LastErrorStrW()).c_str(), L"DeleteFileW",
					MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON3);
				if (r == IDABORT) return;
				if (r == IDRETRY) goto retry;
			}
		}
	}
	OnContextmenu32772();
}


void CMFCFileUnDiffDlg::OnBnClickedOk() {
	if (m_list_files.GetItemCount() < 2) return;
	CString first = m_list_files.GetItemText(0, 2); // the first hash
	CString next; bool same = true;
	for (int i = 1; i < m_list_files.GetItemCount(); ++i) {
		next = m_list_files.GetItemText(i, 2); // the next hash
		if (first != next) {
			same = false;
			break;
		}
	}
	if (same) MessageBoxW(L"Same", NULL, MB_ICONINFORMATION);
	else MessageBoxW(L"Not same", NULL, MB_ICONERROR);
}


typedef struct {
	CMFCFileUnDiffDlg* dlg;
	CDlgWait_1* waitdlg;
	int nItem;
} _sha256_thread_arg;
void CMFCFileUnDiffDlg::Sha256ListItem(int nItem) {
	if (IsFileOrDirectory(m_list_files.GetItemText(nItem, 1).GetBuffer()) != 1) return;
	auto _f = [](void* arg_) {
		_sha256_thread_arg* arg = (_sha256_thread_arg*)arg_;
		CString file = arg->dlg->m_list_files.GetItemText(arg->nItem, 1);
		if (!file.GetLength()) return;
		char sha256value[64 + 1] = { 0 };
		FileSHA256(ws2c(file.GetBuffer()), sha256value, 64 + 1);
		arg->dlg->m_list_files.SetItemText(arg->nItem, 2, s2wc(sha256value));
		arg->dlg->EnableWindow(1);
		arg->waitdlg->EndDialog(0);
		//delete arg->waitdlg;
		delete arg;
	};
	CDlgWait_1* wdlg = new CDlgWait_1;
	_sha256_thread_arg* arg = new _sha256_thread_arg;
	arg->dlg = this;
	arg->waitdlg = wdlg;
	arg->nItem = nItem;
	HANDLE hThrd = (HANDLE)_beginthread(_f, 0, arg);
	if (!hThrd) {
		delete wdlg; delete arg;
		return /*false*/;
	}
	else CloseHandle(hThrd);
	wdlg->Create(IDD_DIALOG_PleaseWait_1);
	//wdlg->SetParent(this);
	wdlg->ShowWindow(1);
	EnableWindow(0);
}


void CMFCFileUnDiffDlg::OnContextmenu32774() { // open
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			ShellExecuteW(this->GetSafeHwnd(), L"open", m_list_files.
				GetItemText(nItem, 1).GetBuffer(), L"", NULL, 1);
		}
	}
}


void CMFCFileUnDiffDlg::OnContextmenu32778() { // open method
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			ShellExecuteW(this->GetSafeHwnd(), L"open", L"OpenWith", (L"\""s +
				m_list_files.GetItemText(nItem, 1).GetBuffer() + L"\"").c_str(), NULL, 1);
		}
	}
}


void CMFCFileUnDiffDlg::OnContextmenu32775() { // open location
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			ShellExecuteW(this->GetSafeHwnd(), L"open", L"explorer", (L"/select,\""s +
				m_list_files.GetItemText(nItem, 1).GetBuffer() + L"\"").c_str(), NULL, 1);
		}
	}
}


void CMFCFileUnDiffDlg::OnContextmenu32776() { // copy location
	wstring strlist;
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			strlist += m_list_files.GetItemText(nItem, 1).GetBuffer() + L"\r\n"s;
		}
	}
	strlist.erase(strlist.length() - 2);
	CopyText(strlist);
}


void CMFCFileUnDiffDlg::OnContextmenu32777() { // copy sha256
	wstring strlist;
	POSITION pos = m_list_files.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		while (pos) {
			int nItem = m_list_files.GetNextSelectedItem(pos);
			// nItem是所选中行的序号 
			strlist += m_list_files.GetItemText(nItem, 2).GetBuffer() + L"\r\n"s;
		}
	}
	strlist.erase(strlist.length() - 2);
	CopyText(strlist);
}


// MFCMyProcCtlUIDlg.h: 头文件
//

#pragma once
#include <string>


// CMFCMyProcCtlUIDlg 对话框
class CMFCMyProcCtlUIDlg : public CDialogEx
{
// 构造
public:
	CMFCMyProcCtlUIDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMFCMyProcCtlUIDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCMYPROCCTLUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	//CImageList* pImageList;
	std::wstring ServiceName;

	static DWORD __stdcall Thread_RefreshList(PVOID arg);
	HANDLE hThread_refresh;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_procs;
	afx_msg void OnBnClickedButtonRefreshProcs();
	bool CheckAdminPriv();
	afx_msg void OnBnClickedButtonAttachController();
	afx_msg void OnBnClickedButtonDetachController();
};

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


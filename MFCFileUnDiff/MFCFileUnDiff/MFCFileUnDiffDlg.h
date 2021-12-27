
// MFCFileUnDiffDlg.h: 头文件
//

#pragma once


// CMFCFileUnDiffDlg 对话框
class CMFCFileUnDiffDlg : public CDialogEx
{
// 构造
public:
	CMFCFileUnDiffDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCFILEUNDIFF_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	CComboBox m_combo_method;
	CListCtrl m_list_files;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonTarget();
	afx_msg void OnNMRClickListFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextmenu32771();
	afx_msg void OnContextmenu32772();
	afx_msg void OnContextmenu32773();
	afx_msg void OnBnClickedOk();
	void Sha256ListItem(int nItem);
	afx_msg void OnContextmenu32774();
	afx_msg void OnContextmenu32775();
	afx_msg void OnContextmenu32776();
	afx_msg void OnContextmenu32777();
	afx_msg void OnContextmenu32778();
};

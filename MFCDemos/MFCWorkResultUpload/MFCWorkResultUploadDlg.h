
// MFCWorkResultUploadDlg.h: 头文件
//

#pragma once


// CMFCWorkResultUploadDlg 对话框
class CMFCWorkResultUploadDlg : public CDialogEx
{
// 构造
public:
	CMFCWorkResultUploadDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCWORKRESULTUPLOAD_DIALOG };
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
	afx_msg
		void tprp();
	void OnBnClickedButtonStartsj();
	afx_msg void OnBnClickedButtonCancel();
	CProgressCtrl m_Prog;
	CButton m_finishbtn;
	afx_msg void OnClickedButtonFini();
	afx_msg void OnClickedButton1();
	afx_msg void OnNMClickSyslinkToact(NMHDR* pNMHDR, LRESULT* pResult);
};

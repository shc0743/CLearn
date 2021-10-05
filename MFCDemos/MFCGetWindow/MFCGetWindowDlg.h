
// MFCGetWindowDlg.h: 头文件
//

#pragma once


// CMFCGetWindowDlg 对话框
class CMFCGetWindowDlg : public CDialogEx
{
// 构造
public:
	CMFCGetWindowDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCGETWINDOW_DIALOG };
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
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGethwnd();
	void InitHwndU();
	afx_msg void OnBnClickedButtonWclose();
	afx_msg void OnBnClickedButtonMaxing();
	afx_msg void OnBnClickedButtonMining();
	afx_msg void OnBnClickedButtonHidew();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClickedButtonSavetitle();
	afx_msg void OnBnClickedButtonClearhwnd();
	afx_msg void OnBnClickedButtonSavepos();
	afx_msg void OnBnClickedCheckZhidin();
	afx_msg void OnBnClickedButtonProgset();
	afx_msg void OnBnClickedButtonAllhwnd();
	afx_msg void OnBnClickedButtonMorehwnd();
	afx_msg void OnClickedButtonAbouter();
	afx_msg void OnBnClickedButtonIntodll();
	afx_msg void OnBnClickedButton1Reloadarg();
	afx_msg void OnBnClickedButtonTozfirst();
	afx_msg void OnBnClickedButtonCtrlEclass();
	afx_msg void OnBnClickedButtonTonormal();
	afx_msg void OnBnClickedButtonActsoft();
	afx_msg void OnBnClickedCheckEnabledwindow();
};

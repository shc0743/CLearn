
// MFCCalcTwoNumAddDlg.h: 头文件
//

#pragma once


// CMFCCalcTwoNumAddDlg 对话框
class CMFCCalcTwoNumAddDlg : public CDialogEx
{
// 构造
public:
	CMFCCalcTwoNumAddDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCALCTWONUMADD_DIALOG };
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
	CComboBox m_cbysf;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCDll();
	afx_msg void OnEnChangeEditNum2();
	afx_msg void OnCbnSelchangeComboMto();
};

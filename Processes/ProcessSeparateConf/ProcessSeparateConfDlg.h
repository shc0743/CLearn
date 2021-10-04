
// ProcessSeparateConfDlg.h: 头文件
//

#pragma once


// CProcessSeparateConfDlg 对话框
class CProcessSeparateConfDlg : public CDialogEx
{
// 构造
public:
	CProcessSeparateConfDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDDProcess_tSEPARATECONF_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	static Process_t::ProcessInfo mainInfo;


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
	afx_msg void OnBnClickedButtonKill();
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButtonSusp();
	afx_msg void OnBnClickedButtonResume();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonKilltree();
};

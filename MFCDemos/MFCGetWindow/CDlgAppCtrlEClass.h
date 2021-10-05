#pragma once


// CDlgAppCtrlEClass 对话框

class CDlgAppCtrlEClass : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAppCtrlEClass)

public:
	CDlgAppCtrlEClass(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgAppCtrlEClass();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APP_CTRL_ECLASS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		void OnSysCommand(int pID);
	BOOL OnInitDialog();
	void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonCecWecm();
	afx_msg void OnBnClickedButtonCecHidemain();
	afx_msg void OnBnClickedCtrlclass();
	afx_msg void OnBnClickedButtonCecSelecteclass();
	afx_msg void OnBnClickedButtonCecUnzd();
	afx_msg void OnBnClickedButtonCecHidehwnd();
};

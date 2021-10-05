#pragma once


// CDlgAllhwnd 对话框

class CDlgAllhwnd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAllhwnd)

public:
	CDlgAllhwnd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgAllhwnd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALLHWND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGetallhwnd();
	CButton m_sethwndbyall;
	CButton m_gethwndbyall;
	afx_msg void OnBnClickedButtonSethwndfa();
};

#pragma once


// CDlgGetHwnd 对话框

class CDlgGetHwnd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGetHwnd)

public:
	CDlgGetHwnd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgGetHwnd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GETHWND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGethwndTop();
	// 进度条
	CProgressCtrl m_prog_getstat;
	afx_msg void OnBnClickedButtonGethwndMouse();
	afx_msg void OnBnClickedButtonInttohwnd();
};

#pragma once
#include "CDlgFindWindowEr.h"


// CDlgFindWindow 对话框

class CDlgFindWindow : public CDialogEx
{
protected:
	HWND hw;
	DECLARE_DYNAMIC(CDlgFindWindow)

public:
	CDlgFindWindow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFindWindow();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FINDWINDOW };
#endif

protected:
	CDlgFindWindowEr* finder;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFindwindowFind();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonFwWfp();
};

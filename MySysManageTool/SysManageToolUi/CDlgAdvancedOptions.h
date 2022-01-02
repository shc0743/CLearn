#pragma once
#include "afxdialogex.h"


// CDlgAdvancedOptions 对话框

class CDlgAdvancedOptions : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAdvancedOptions)

public:
	CDlgAdvancedOptions(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgAdvancedOptions();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADVANCED };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoaddriver();
};

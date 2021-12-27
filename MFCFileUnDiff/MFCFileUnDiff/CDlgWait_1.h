#pragma once
#include "afxdialogex.h"


// CDlgWait_1 对话框

class CDlgWait_1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWait_1)

public:
	CDlgWait_1(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgWait_1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PleaseWait_1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};

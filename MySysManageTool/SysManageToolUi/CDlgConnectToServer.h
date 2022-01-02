#pragma once
#include "afxdialogex.h"


// CDlgConnectToServer 对话框

class CDlgConnectToServer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConnectToServer)

public:
	CDlgConnectToServer(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgConnectToServer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONNECTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStartserver();
protected:
	afx_msg LRESULT OnUserEndDialog(WPARAM wParam, LPARAM lParam);
};

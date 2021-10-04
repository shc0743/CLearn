
// MFCLoginGUIDlg.h: 头文件
//

#pragma once

#include "CDlgMyInfo.h"
#include "CDlgWelPage.h"


// CMFCLoginGUIDlg 对话框
class CMFCLoginGUIDlg : public CDialogEx
{
// 构造
public:
	CMFCLoginGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCLOGINGUI_DIALOG };
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
	afx_msg void OnClickedButtonClose();
	afx_msg void OnBnClickedButtonExitlog();
	afx_msg void OnBnClickedButtonChgepw();
	afx_msg void OnBnClickedButtonMyinf();
	// MainTab控件
	CTabCtrl m_tabmain;
	//MyInfo
	class CDlgMyInfo m_myinfomainbox;
	class CDlgWelPage m_welpage;
	//
	CProgressCtrl m_mprog;
};


// FileLockerUIDlg.h: 头文件
//

#pragma once
#include "../../resource/tool.h"


// CFileLockerUIDlg 对话框
class CFileLockerUIDlg : public CDialogEx
{
// 构造
public:
	CFileLockerUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

	

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILELOCKERUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	map<wstring, HANDLE> files;

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
	CListCtrl m_list1;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckAall();
	afx_msg void OnBnClickedButtonUnlock();
	afx_msg void OnBnClickedButtonViewlog();
};

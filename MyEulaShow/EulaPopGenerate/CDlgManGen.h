#pragma once
#include "afxdialogex.h"


// CDlgManGen 对话框

class CDlgManGen : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManGen)

public:
	CDlgManGen(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgManGen();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANGEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowseSavemanifest();
	afx_msg void OnBnClickedButtonBrowseEulaPath();
	afx_msg void OnBnClickedButtonBrowseGenmainexe();
	afx_msg void OnNMClickSyslinkLearnmoreMani(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonBrowseGenor();
};

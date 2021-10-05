#pragma once


// CDlgProSet 对话框

class CDlgProSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProSet)

public:
	CDlgProSet(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgProSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSavesets();
	CProgressCtrl m_prog_seting;
	afx_msg void OnBnClickedCheckPwd();
};

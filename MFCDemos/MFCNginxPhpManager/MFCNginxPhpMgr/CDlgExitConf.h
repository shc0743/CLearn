#pragma once


// CDlgExitConf 对话框

class CDlgExitConf : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExitConf)

public:
	CDlgExitConf(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgExitConf();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EXIT_CONF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonExitAutook();
};

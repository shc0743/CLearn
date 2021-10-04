#pragma once


// CDlgSetup 对话框

class CDlgSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetup)

public:
	CDlgSetup(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSetup();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSetupNginxPath();
	afx_msg void OnBnClickedButtonSetupPhpPath();
	afx_msg void OnBnClickedCheckSetupBlockphpexit();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

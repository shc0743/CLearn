#pragma once


// CDlgStep1 对话框

class CDlgStep1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStep1)

public:
	CDlgStep1(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStep1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STEP1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonConfRbmenu();
};

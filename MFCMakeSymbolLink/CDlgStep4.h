#pragma once


// CDlgStep4 对话框

class CDlgStep4 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStep4)

public:
	CDlgStep4(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStep4();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STEP4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
//	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonBrowseOut();
};

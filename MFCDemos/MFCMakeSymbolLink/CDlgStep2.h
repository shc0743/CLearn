#pragma once


// CDlgStep2 对话框

class CDlgStep2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStep2)

public:
	CDlgStep2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStep2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STEP2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEditSrc();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonBrowseSrc();
	afx_msg void OnBnClickedOk();
};

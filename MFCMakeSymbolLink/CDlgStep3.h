#pragma once


// CDlgStep3 对话框

class CDlgStep3 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStep3)

public:
	CDlgStep3(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStep3();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STEP3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonCreateHl();
};

#pragma once


// CDlgMainLoginBox 对话框

class CDlgMainLoginBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMainLoginBox)

public:
	CDlgMainLoginBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMainLoginBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonToreg();
};

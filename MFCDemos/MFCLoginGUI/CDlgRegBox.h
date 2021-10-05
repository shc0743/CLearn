#pragma once


// CDlgRegBox 对话框

class CDlgRegBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRegBox)

public:
	CDlgRegBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgRegBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTolog();
	afx_msg void OnBnClickedButtonRegid();
	afx_msg void OnBnClickedButton1();
};

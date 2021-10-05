#pragma once


// CAbouterBox 对话框

class CAbouterBox : public CDialogEx
{
	DECLARE_DYNAMIC(CAbouterBox)

public:
	CAbouterBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAbouterBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonFwzzwy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEditZzSaycdJbj();
};

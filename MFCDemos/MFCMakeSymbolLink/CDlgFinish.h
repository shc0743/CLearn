#pragma once


// CDlgFinish 对话框

class CDlgFinish : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFinish)

public:
	CDlgFinish(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFinish();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_BYE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCommand1();
};

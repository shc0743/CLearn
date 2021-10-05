#pragma once


// CDlgCheckPhpCgi 对话框

class CDlgCheckPhpCgi : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCheckPhpCgi)

public:
	CDlgCheckPhpCgi(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgCheckPhpCgi();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHECK_PHP_CGI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};

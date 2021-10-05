#pragma once


// CDlgAboutDlg 对话框

class CDlgAboutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAboutDlg)

public:
	CDlgAboutDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

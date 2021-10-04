#pragma once


// CDlgWelPage 对话框

class CDlgWelPage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWelPage)

public:
	CDlgWelPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgWelPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_U_WELCOME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

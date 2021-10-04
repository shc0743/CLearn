#pragma once


// CDlgChangePwd 对话框

class CDlgChangePwd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChangePwd)

public:
	CDlgChangePwd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgChangePwd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_U_CPWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCpwd();
};

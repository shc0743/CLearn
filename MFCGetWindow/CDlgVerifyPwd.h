#pragma once


// CDlgVerifyPwd 对话框

class CDlgVerifyPwd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVerifyPwd)

public:
	CDlgVerifyPwd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgVerifyPwd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TYPEPASSWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckPwd();
};

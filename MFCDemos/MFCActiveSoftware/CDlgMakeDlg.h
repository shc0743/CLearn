#pragma once


// CDlgMakeDlg 对话框

class CDlgMakeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakeDlg)

public:
	CDlgMakeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMakeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAKE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;

	int OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

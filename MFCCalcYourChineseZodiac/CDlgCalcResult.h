#pragma once


// CDlgCalcResult 对话框

class CDlgCalcResult : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalcResult)

public:
	CDlgCalcResult(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgCalcResult();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALCRESULT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		BOOL OnInitDialog();
	void OnBnClickedOk();
	afx_msg void OnBnClickedButtonComeagain();
};

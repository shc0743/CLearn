#pragma once


// CDlgHFuncDllInto 对话框

class CDlgHFuncDllInto : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHFuncDllInto)

public:
	CDlgHFuncDllInto(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgHFuncDllInto();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HFUNC_INTODLL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClickedButtonDllHidemainwnd();
	afx_msg void OnBnClickedButtonDllSelectdll();
	afx_msg void OnBnClickedButtonDllCrt();
};

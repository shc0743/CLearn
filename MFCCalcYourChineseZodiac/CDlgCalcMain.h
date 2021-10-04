#pragma once


// CDlgCalcMain 对话框

class CDlgCalcMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalcMain)

public:
	CDlgCalcMain(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgCalcMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALCMAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	void OnSysCommand(UINT nID, LPARAM lParam);
	void OnPaint();
	HCURSOR OnQueryDragIcon();
	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonNextNohas();
	afx_msg void OnBnClickedButtonNextHas();
};

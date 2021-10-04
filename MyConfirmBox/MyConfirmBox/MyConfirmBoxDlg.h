
// MyConfirmBoxDlg.h: 头文件
//

#pragma once


// CMyConfirmBoxDlg 对话框
class CMyConfirmBoxDlg : public CDialogEx
{
// 构造
public:
	CMyConfirmBoxDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYCONFIRMBOX_DIALOG };
#endif

	void setConfirmTotal(size_t val);
	void setAlertText(LPCTSTR val);
	void setTitle(LPCTSTR val);
	void setTopText(LPCTSTR val);
	void setTextBtn1(LPCTSTR val);
	void setTextBtn2(LPCTSTR val);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	struct {
		LPCTSTR alert_text;
		LPCTSTR title;
		LPCTSTR toptext;
		LPCTSTR textbtn1, textbtn2;
	} m_initdata;

// 实现
protected:
	HICON m_hIcon;
	RECT rectDesktop;
	size_t confirmedtotal, confirmednow;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckAccepted();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
protected:
	CProgressCtrl m_progress_total;
};


// MFCNginxPhpMgrDlg.h: 头文件
//

#pragma once


#define WM_SHOWTASK WM_USER+1

// CMFCNginxPhpMgrDlg 对话框
class CMFCNginxPhpMgrDlg : public CDialogEx
{
// 构造
public:
	CMFCNginxPhpMgrDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCNGINXPHPMGR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	bool isIconBox;

protected:
	NOTIFYICONDATA m_nid;
	virtual void ToTray();
	afx_msg LRESULT onShowTask(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnIconMain();
	afx_msg void OnIconExit();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonStopWindows();
	afx_msg void OnBnClickedButtonRestNginx2();
	afx_msg void OnBnClickedButtonConfMysql();
	afx_msg void OnBnClickedButtonProgramSet();
	afx_msg void OnBnClickedButtonStopNginx();
	afx_msg void OnBnClickedButtonStartNginx();
	afx_msg void OnBnClickedButtonRestNginx();
	afx_msg void OnBnClickedButtonConfNginx();
	afx_msg void OnBnClickedButtonStartMysql();
	afx_msg void OnBnClickedButtonStopMysql();
//	afx_msg void OnStnClickedStatPhpOk();
	afx_msg void OnBnClickedButtonConfPhp();
	afx_msg void OnBnClickedButtonStopPhp();
	afx_msg void OnBnClickedButtonStartPhp();
	afx_msg void OnBnClickedButtonRestPhp();
	afx_msg void OnBnClickedButtonRestMysql();
};

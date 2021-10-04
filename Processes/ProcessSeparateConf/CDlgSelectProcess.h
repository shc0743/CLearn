#pragma once


// CDlgSelectProcess 对话框

class CDlgSelectProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSelectProcess)

public:
	CDlgSelectProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSelectProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_list1;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnLbnSelchangeListSelectproc();
//	afx_msg void OnLbnKillfocusListSelectproc();
	afx_msg void OnSelcancelListSelectproc();
};


// EulaPopGenerateDlg.h: 头文件
//

#pragma once


// CEulaPopGenerateDlg 对话框
class CEulaPopGenerateDlg : public CDialogEx
{
// 构造
public:
	CEulaPopGenerateDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EULAPOPGENERATE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowseManifest();
	afx_msg void OnBnClickedButtonBrowseBasefile();
	afx_msg void OnBnClickedButtonBrowseOutfile();
	afx_msg void OnBnClickedButtonAddFile();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditManifestPath();
	afx_msg void OnBnClickedButtonRemovelistitem();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonGenManifest();
};

#pragma once


// CDlgMerge 对话框

class CDlgMerge : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMerge)

public:
	CDlgMerge(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMerge();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MERGE_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

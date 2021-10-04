#pragma once

// CDlgFindWindowEr 对话框3

class CDlgFindWindowEr : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFindWindowEr)

public:
	CDlgFindWindowEr(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFindWindowEr();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FINDWINDOWER };
#endif

	void* parent;

	UINT_PTR timerid;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#pragma once
#include "afxdialogex.h"


// CDlgMgProcess 对话框

class CDlgMgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMgProcess)

public:
	CDlgMgProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMgProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list;

	friend static DWORD WINAPI ReadProcessData(CDlgMgProcess* dlg);

};

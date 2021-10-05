#pragma once


// CDlgRegister 对话框
#include<afxdialogex.h>

class CDlgRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRegister)

public:
	CDlgRegister(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgRegister();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

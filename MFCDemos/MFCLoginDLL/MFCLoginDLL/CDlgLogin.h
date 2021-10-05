#pragma once


// CDlgLogin 对话框
#include <string>
#include <afxdialogex.h>
#include "pch.h"
#include "defines.h"
using namespace std;

class CDlgLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgLogin();

	void showRegisterButton(bool show);

	string username() { return username_; };
	string password() { return password_; };

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	string username_;
	string password_;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIddDialogLogin();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnBnClickedOk();
};

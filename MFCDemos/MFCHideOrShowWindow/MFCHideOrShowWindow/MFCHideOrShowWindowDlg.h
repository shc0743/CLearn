
// MFCHideOrShowWindowDlg.h: 头文件
//

#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<algorithm>
#ifdef _WIN32
#ifndef _WINDOWS_
#include<Windows.h>
#endif
#include<TlHelp32.h>
#include<ShObjIdl.h>
#endif
#include<tchar.h>
#include<typeinfo>
#include<assert.h>
#include<thread>
#include<chrono>
#if _HAS_CXX17
#include<any>
#endif
using namespace std;


// CMFCHideOrShowWindowDlg 对话框
class CMFCHideOrShowWindowDlg : public CDialogEx
{
// 构造
public:
	CMFCHideOrShowWindowDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCHIDEORSHOWWINDOW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	using WINDOW = struct {
		HWND hwnd;
		wstring title;
	};


// 实现
protected:
	HICON m_hIcon;
	CListCtrl m_list_wind;

	vector<WINDOW> windowslist;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonFindwindow();
	afx_msg void OnBnClickedOk();
};

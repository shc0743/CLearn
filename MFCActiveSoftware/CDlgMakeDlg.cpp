// CDlgMakeDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCActiveSoftware.h"
#include "CDlgMakeDlg.h"
#include "afxdialogex.h"
#include "activity.h"
#include "md5.h"


// CDlgMakeDlg 对话框

IMPLEMENT_DYNAMIC(CDlgMakeDlg, CDialogEx)

CDlgMakeDlg::CDlgMakeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MAKE_DLG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgMakeDlg::~CDlgMakeDlg()
{
}

void CDlgMakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMakeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMakeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMakeDlg 消息处理程序

int CDlgMakeDlg::OnInitDialog() {

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    ::SetDlgItemTextW(this->GetSafeHwnd(), IDC_EDIT_CRIFMS, L"Copyright(c) 2021.All rights reserved.");
    ::SetDlgItemTextW(this->GetSafeHwnd(), IDC_EDIT_SOFTNAME, L"Product");
    ::SetDlgItemTextW(this->GetSafeHwnd(), IDC_EDIT_COMPNAME, L"Company");
    ::SetDlgItemTextW(this->GetSafeHwnd(), IDC_EDIT_CODE_HOWTO, L"http://www.example.com/");

    return 1;
}


string ws2s2(wstring wstr)
{
    string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
void CDlgMakeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
    CString keys, prod, cmp, howto, cr; string stdstr;
	GetDlgItemText(IDC_EDIT_KEYS, keys);
    GetDlgItemText(IDC_EDIT_SOFTNAME, prod);
    GetDlgItemText(IDC_EDIT_COMPNAME, cmp);
    GetDlgItemText(IDC_EDIT_CODE_HOWTO, howto);
    GetDlgItemText(IDC_EDIT_CRIFMS, cr);
    vector<string> keyarg;
    str_split(ws2s2(keys.GetBuffer()), "\r\n", keyarg);
    fstream fp("act.key", ios::out | ios::binary);
    fp << "MFC Active Software Key Information\n""Copyright (c) 2021.All rights reserved." << endl;
    fp << "ProductName="          << ws2s2(prod.GetBuffer()) << endl;
    fp << "CompanyName="          << ws2s2(cmp .GetBuffer()) << endl;
    fp << "CopyRightInformation=" << ws2s2(cr  .GetBuffer()) << endl;
    fp << "BuyUrl=" << ((ws2s2(howto.GetBuffer()).find("http")!=stdstr.npos)?((string)"start "+ws2s2(howto.GetBuffer())):ws2s2(howto.GetBuffer())) + (string)"\r\n";
    fp << "KEYLIST={" << endl;
    for (auto i : keyarg) {
        if (i.length() < 1) continue;
        fp << MD5(i).toString() << endl;
    }
    fp << "}";
    fp.close();
    MessageBoxA(this->GetSafeHwnd(), "激活文件生成成功!\r\n文件名: act.key\n使用方法:将本软件与key放在同一目录下,然后启动本软件", "好消息", MB_ICONINFORMATION);
}

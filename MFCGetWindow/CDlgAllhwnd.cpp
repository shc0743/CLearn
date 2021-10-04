// CDlgAllhwnd.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgAllhwnd.h"
#include "afxdialogex.h"
#include <sstream>
using namespace std;


// CDlgAllhwnd 对话框

IMPLEMENT_DYNAMIC(CDlgAllhwnd, CDialogEx)

CDlgAllhwnd::CDlgAllhwnd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALLHWND, pParent)
{

}

CDlgAllhwnd::~CDlgAllhwnd()
{
}

void CDlgAllhwnd::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SETHWNDFA, m_sethwndbyall);
    DDX_Control(pDX, IDC_BUTTON_GETALLHWND, m_gethwndbyall);
}


BEGIN_MESSAGE_MAP(CDlgAllhwnd, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgAllhwnd::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GETALLHWND, &CDlgAllhwnd::OnBnClickedButtonGetallhwnd)
    ON_BN_CLICKED(IDC_BUTTON_SETHWNDFA, &CDlgAllhwnd::OnBnClickedButtonSethwndfa)
END_MESSAGE_MAP()


// CDlgAllhwnd 消息处理程序


void CDlgAllhwnd::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(0);
}



#include <vector>
#include "topvar.h"
#include "hwnds.h"
HWND* allhwnds[2047] = {nullptr};
int allhwnds_int[2047];
wstring allhwnds_titles[2047];
int  allhwnds_count = 0;
int CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    TCHAR szTitle[MAX_PATH] = { 0 };
    ::GetWindowText(hwnd, szTitle, MAX_PATH);
    //MessageBox(0, szTitle, L"子窗口", MB_OK);
    allhwnds_titles[allhwnds_count] = szTitle;
    allhwnds[allhwnds_count]=new HWND((HWND)(hwnd));
    allhwnds_int[allhwnds_count] = reinterpret_cast<int>(hwnd);
    allhwnds_count++;
    return TRUE;
}
void CDlgAllhwnd::OnBnClickedButtonGetallhwnd()
{
	// TODO: 在此添加控件通知处理程序代码
    allhwnds_count = 0;
    ::EnumChildWindows(::GetDesktopWindow(), EnumChildProc, NULL);
    CString s,hwr2;char hwr[513]=""; ostringstream oss;
    for (int i = 0; i < allhwnds_count;i++) {
        GetDlgItemText(IDC_SHOW_ALLHWND,s);
        oss.str(""); 
        //TCHAR szTitle[MAX_PATH] = { 0 };
        //::GetWindowText(*allhwnds[i], szTitle, MAX_PATH);
        if (allhwnds_titles[i] == L"") continue;
        oss << reinterpret_cast<int>(allhwnds[i]) << ". 标题:" << ws2s(allhwnds_titles[i]) << "\r\n";
        s += oss.str().c_str();
        SetDlgItemText(IDC_SHOW_ALLHWND, s);
        s = "";
    }
    m_sethwndbyall.EnableWindow(0);
    m_gethwndbyall.EnableWindow(FALSE);
}


void CDlgAllhwnd::OnBnClickedButtonSethwndfa()
{
    // TODO: 在此添加控件通知处理程序代码
    int id;
    GetDlgItemInt(IDC_EDIT_AHWNDSID, &id);
    if (id == 0) {
        MessageBox(L"请输入正确的数字", L"错误", MB_ICONERROR);
        return;
    }
    uhwnd = reinterpret_cast<HWND__*>(allhwnds_int[id]);
    EndDialog(123);
}

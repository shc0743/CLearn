// CDlgAppCtrlEClass.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CDlgAppCtrlEClass.h"
#include "afxdialogex.h"
#include "resource.h"


// CDlgAppCtrlEClass 对话框

IMPLEMENT_DYNAMIC(CDlgAppCtrlEClass, CDialogEx)

CDlgAppCtrlEClass::CDlgAppCtrlEClass(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APP_CTRL_ECLASS, pParent)
{

}

CDlgAppCtrlEClass::~CDlgAppCtrlEClass()
{
}

void CDlgAppCtrlEClass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAppCtrlEClass, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgAppCtrlEClass::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CEC_WECM, &CDlgAppCtrlEClass::OnBnClickedButtonCecWecm)
	ON_BN_CLICKED(IDC_BUTTON_CEC_HIDEMAIN, &CDlgAppCtrlEClass::OnBnClickedButtonCecHidemain)
	ON_BN_CLICKED(IDCANCEL_CTRLCLASS, &CDlgAppCtrlEClass::OnBnClickedCtrlclass)
	ON_BN_CLICKED(IDC_BUTTON_CEC_SELECTECLASS, &CDlgAppCtrlEClass::OnBnClickedButtonCecSelecteclass)
	ON_BN_CLICKED(IDC_BUTTON_CEC_UNZD, &CDlgAppCtrlEClass::OnBnClickedButtonCecUnzd)
	ON_BN_CLICKED(IDC_BUTTON_CEC_HIDEHWND, &CDlgAppCtrlEClass::OnBnClickedButtonCecHidehwnd)
END_MESSAGE_MAP()

void CDlgAppCtrlEClass::OnSysCommand(int pID) {
	if (pID == SC_CLOSE) return;
}
#include <TlHelp32.h>
#include "topvar.h"
#pragma comment(lib,"Kernel32.lib")

#include<string>
CWnd* CtrlClassMainCwnd=nullptr;
HWND EClassHwnd=nullptr;
CString EClassRoomName; 
HWND GetHandleByName(LPCWSTR procName) {
	DWORD iProcID = 0;
	PROCESSENTRY32 pEntry = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		if (Process32First(hSnapshot, &pEntry))
		{
			while (Process32Next(hSnapshot, &pEntry))
			{
				if (lstrcmpi(procName, pEntry.szExeFile) == 0)
				{
					iProcID = pEntry.th32ProcessID;
					CloseHandle(hSnapshot);
					break;
				}
			}
		}
	}
	HWND hwnd = ::GetTopWindow(NULL);
	while (hwnd)
	{
		DWORD pid = 0;
		DWORD dwProcessId = GetWindowThreadProcessId(hwnd, &pid);
		if (dwProcessId != 0)
		{
			if (pid == iProcID)
			{
				return hwnd;
			}
		}
		hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT);
	}
	return NULL;
}
void CALLBACK TimerProcOnCtrlEClass(HWND hWnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime) {
	HWND hwnd = *AfxGetMainWnd();
	RECT r1;
	::GetWindowRect(hwnd, &r1);
	CString udtcx, udtcy, udtx, udty;
	udtcx = to_string(r1.right - r1.left).c_str();
	udtcy = to_string(r1.bottom - r1.top).c_str();
	udtx = to_string(r1.left).c_str();
	udty = to_string(r1.top).c_str();
	::SetWindowPos(hwnd, HWND_TOPMOST,
	atoi(ws2s(udtx.GetBuffer()).c_str()),
	atoi(ws2s(udty.GetBuffer()).c_str()),
	atoi(ws2s(udtcx.GetBuffer()).c_str()),
	atoi(ws2s(udtcy.GetBuffer()).c_str()),
	NULL);
	HWND hWnd2=CtrlClassMainCwnd->GetSafeHwnd();
	CString n(to_string(time(0)).c_str());
	n += " - Microsoft PowerPoint";
	SetWindowText(hWnd2, n);
	CString ecn=EClassRoomName;
	if (ecn.GetLength() > 0) {
		HWND ec = (HWND)GetHandleByName(ecn.GetBuffer());
		SetDlgItemText(hWnd2, IDC_SHOW_CEC_HANDLE, s2ws(to_string(reinterpret_cast<int>(ec))).c_str());
		EClassHwnd = ec;
		::GetWindowRect(ec, &r1);
		CString udtcx, udtcy, udtx, udty;
		udtcx = to_string(r1.right - r1.left).c_str();
		udtcy = to_string(r1.bottom - r1.top).c_str();
		udtx = to_string(r1.left).c_str();
		udty = to_string(r1.top).c_str();
		::SetWindowPos(ec, HWND_TOPMOST,
			atoi(ws2s(udtx.GetBuffer()).c_str()),
			atoi(ws2s(udty.GetBuffer()).c_str()),
			atoi(ws2s(udtcx.GetBuffer()).c_str()),
			atoi(ws2s(udtcy.GetBuffer()).c_str()),
			NULL);
	}
#if 0
	else MessageBox(NULL, ecn, s2ws(to_string(ecn.GetLength())).c_str(), MB_OK);
#endif
}

bool CtrlClassTimerRunning = false;
bool isTrueEndCtrlEClass = false;
bool isHidedCtrlEClass = false;
BOOL CDlgAppCtrlEClass::OnInitDialog() {
	((CButton*)(this->GetDlgItem(IDC_CHECK_CEC_ALWAYSONTOP)))->SetCheck(TRUE);
	((CButton*)(this->GetDlgItem(IDC_CHECK_CEC_AUTOUNTOP)))->SetCheck(TRUE);
	((CButton*)(this->GetDlgItem(IDC_CHECK_CEC_RDMTLE)))->SetCheck(TRUE);
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	if(!CtrlClassTimerRunning) SetTimer(578564,500,TimerProcOnCtrlEClass);
	CtrlClassTimerRunning = true;
	isTrueEndCtrlEClass = false;
	CtrlClassMainCwnd = this;
	HWND hWnd2=CtrlClassMainCwnd->GetSafeHwnd();
	CString n(to_string(time(0)).c_str());
	n += " - Microsoft PowerPoint";
	::SetWindowText(hWnd2,n);
	return TRUE;
}
// CDlgAppCtrlEClass 消息处理程序

void CDlgAppCtrlEClass::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isTrueEndCtrlEClass) { 
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
		EndDialog(-1); 
		return;
	}
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
}
void CDlgAppCtrlEClass::OnBnClickedCtrlclass()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isTrueEndCtrlEClass) {
	    AfxGetMainWnd()->ShowWindow(SW_SHOW);
	    //AfxGetMainWnd()->ShowWindow(SW_HIDE);
		return;
	}
}


void CDlgAppCtrlEClass::OnBnClickedButtonCecWecm()
{
	// TODO: 在此添加控件通知处理程序代码
	isTrueEndCtrlEClass = true;
	GetDlgItem(IDCANCEL_CTRLCLASS)->EnableWindow(TRUE);
}


bool IsTrueHideMainOnCtrlEClass = true;
void CDlgAppCtrlEClass::OnBnClickedButtonCecHidemain()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsTrueHideMainOnCtrlEClass) {
		IsTrueHideMainOnCtrlEClass = false;
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_BUTTON_CEC_HIDEMAIN, L"隐藏主窗口");
	}
	else {
		IsTrueHideMainOnCtrlEClass = true;
		AfxGetMainWnd()->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_BUTTON_CEC_HIDEMAIN, L"显示主窗口");
	}
}



void CDlgAppCtrlEClass::OnBnClickedButtonCecSelecteclass()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog openDlg(TRUE, L"Material File(*.material)|*.material", NULL,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	L"可恶的电子教室主程序(*.exe)|*.exe||", this);
	CString fnm;
	if (openDlg.DoModal() == IDOK)    //弹出对话框  
	{
		fnm = openDlg.GetPathName();//得到完整的文件名和目录名拓展名  
		GetDlgItem(IDC_SHOW_ECLASSPATH)->SetWindowText(fnm);//将路径显示  
		EClassRoomName = openDlg.GetFileName();
	}
}


void CDlgAppCtrlEClass::OnBnClickedButtonCecUnzd()
{
	// TODO: 在此添加控件通知处理程序代码
	HWND hwnd = EClassHwnd;
	RECT r1;
	::GetWindowRect(hwnd, &r1);
	CString udtcx, udtcy, udtx, udty;
	udtcx = to_string(r1.right - r1.left).c_str();
	udtcy = to_string(r1.bottom - r1.top).c_str();
	udtx = to_string(r1.left).c_str();
	udty = to_string(r1.top).c_str();
	::SetWindowPos(hwnd, ((HWND)-2),
		atoi(ws2s(udtx.GetBuffer()).c_str()),
		atoi(ws2s(udty.GetBuffer()).c_str()),
		atoi(ws2s(udtcx.GetBuffer()).c_str()),
		atoi(ws2s(udtcy.GetBuffer()).c_str()),
	NULL);
}


void CDlgAppCtrlEClass::OnBnClickedButtonCecHidehwnd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isHidedCtrlEClass) {
		isHidedCtrlEClass = false;
		::ShowWindow(EClassHwnd, SW_SHOW);
	}
	else {
		isHidedCtrlEClass = true;
		::ShowWindow(EClassHwnd, SW_HIDE);
	}
}

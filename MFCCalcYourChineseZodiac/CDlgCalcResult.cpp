// CDlgCalcResult.cpp: 实现文件
//

#include "pch.h"
#include "MFCCalcYourChineseZodiac.h"
#include "CDlgCalcResult.h"
#include "afxdialogex.h"
#include <sstream>
#include "topvar.h"
using namespace std;


// CDlgCalcResult 对话框

IMPLEMENT_DYNAMIC(CDlgCalcResult, CDialogEx)

CDlgCalcResult::CDlgCalcResult(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CALCRESULT, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgCalcResult::~CDlgCalcResult()
{
}

void CDlgCalcResult::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCalcResult, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgCalcResult::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_COMEAGAIN, &CDlgCalcResult::OnBnClickedButtonComeagain)
END_MESSAGE_MAP()


// CDlgCalcResult 消息处理程序
string GetProgramDir()  
{
	char exeFullPath[MAX_PATH]; // Full path
	string strPath = "";

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	strPath = (string)exeFullPath;    // Get full path of the file
	return strPath;
}

BOOL CDlgCalcResult::OnInitDialog() {
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	wstring res;
	switch (researchresult) {
	case 1000:
		res = L"鼠";
		break;
	case 1110:
		res = L"牛";
		break;
	case 1100:
		res = L"虎";
		break;
	case 100:
		res = L"兔";
		break;
	case 111:
		res = L"龙";
		break;
	case 110:
		res = L"蛇";
		break;
	case 10:
		res = L"马";
		break;
	case 1011:
		res = L"羊";
		break;
	case 11:
		res = L"猴";
		break;
	case 1:
		res = L"鸡";
		break;
	case 1101:
		res = L"狗";
		break;
	case 1001:
		res = L"猪";
		break;
	default:
		res = L"您选择了不可能的结果";
	}
	SetDlgItemText(IDC_SHOW_MAINRES, res.c_str());
	return TRUE;
}


void CDlgCalcResult::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}


void CDlgCalcResult::OnBnClickedButtonComeagain()
{
	// TODO: 在此添加控件通知处理程序代码
	ostringstream oss;
	oss << "powershell Start-Process -FilePath \"" << GetCommandLineA() << "\"";
	exit(WinExec(oss.str().c_str(),SW_HIDE));
}

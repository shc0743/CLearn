// CDlgMerge.cpp: 实现文件
//

#include "pch.h"
#include "MFCSplitFile.h"
#include "CDlgMerge.h"
#include "afxdialogex.h"


// CDlgMerge 对话框

IMPLEMENT_DYNAMIC(CDlgMerge, CDialogEx)

CDlgMerge::CDlgMerge(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MERGE_FILE, pParent)
{

}

CDlgMerge::~CDlgMerge()
{
}

void CDlgMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMerge, CDialogEx)
END_MESSAGE_MAP()


// CDlgMerge 消息处理程序

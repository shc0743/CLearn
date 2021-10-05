// CAbouterBox.cpp: 实现文件
//

#include "pch.h"
#include "MFCGetWindow.h"
#include "CAbouterBox.h"
#include "afxdialogex.h"
#include "topvar.h"


// CAbouterBox 对话框

IMPLEMENT_DYNAMIC(CAbouterBox, CDialogEx)

CAbouterBox::CAbouterBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTER, pParent)
{

}

CAbouterBox::~CAbouterBox()
{
}

void CAbouterBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAbouterBox, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CAbouterBox::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FWZZWY, &CAbouterBox::OnBnClickedButtonFwzzwy)
	ON_BN_CLICKED(IDC_BUTTON1, &CAbouterBox::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT_ZZ_SAYCD_JBJ, &CAbouterBox::OnEnChangeEditZzSaycdJbj)
END_MESSAGE_MAP()


// CAbouterBox 消息处理程序


void CAbouterBox::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(0);
}


void CAbouterBox::OnBnClickedButtonFwzzwy()
{
	// TODO: 在此添加控件通知处理程序代码
	system("start https://github.com/shc0743/");
}


void CAbouterBox::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	system("start https://github.com/shc7432/download/tree/master/apps/execution/");
}


#include<fstream>
#include <iostream>
void CAbouterBox::OnEnChangeEditZzSaycdJbj()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_EDIT_ZZ_SAYCD_JBJ, str);
	if (ws2s(str.GetBuffer()) == "衣海洋") {
		AfxGetMainWnd()->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_HIDE);
		fstream fp("yhy.htm",ios::out);
		fp << "<pre style=font-size:20px;>";
		fp << "惊爆价惊爆价\r\n"
			"这里有优惠 这里有实惠 这里有为你省钱的好机会\r\n"
			"你们手拉手往这边走 东走走 西走走 该出手时就出手\r\n"
			"买优惠 买实惠 买来买去买机会\r\n";
		fp << "往前走  往前站 不买可以看一看\r\n"
			"见得多  识得广 咱家胜过淘宝网\r\n"
			"新款货品刚刚到 美女个个都需要\r\n"
			"家家都在奔小康 人人都要新衣裳\r\n"
			"人靠衣服马靠鞍 超级靓丽必须穿\r\n"
			"太阳出来照地球 咱的衣服好温柔\r\n"
			"颜色好  款式新 穿在身上好舒心\r\n"
			"面料强  做工精 价格实惠又贴心\r\n"
			"服务好  价格低 让你看看都开心\r\n"
			"帅锅美女处对象 必须穿衣要漂亮\r\n"
			"一分钱一分货   咱的衣服真不错\r\n"
			"买我衣服最实惠 因为价格不会贵\r\n"
			"该出手时就出手 机会不是天天有\r\n"
			"该享受时就享受 不能人比黄花瘦\r\n"
			"大家赚钱不容易 我这就是大让利\r\n"
			"我的衣服好不好 大家一眼就明了\r\n"
			"我的衣服妙不妙 大家一看就知道\r\n"
			"不骗人名不骗党 衣服合格才出厂 纸片您的RMB\r\n"
			"不骗男女和老少 衣服质量都可靠\r\n"
			"看看包装和外形 既赶时髦又流行\r\n"
			"衣服穿得很有型 证明你很有水平\r\n"
			"穿出气质和有型 李白登舟也说行\r\n"
			"山外青山楼外楼 咱的衣服数一流\r\n"
			"西湖歌舞几时休 买不到手才心揪\r\n"
			"暖风熏得游人醉 谁也没有我实惠\r\n"
			"直把杭州作汴州 我这就是大丰收\r\n";
		fp << "</pre>";
		fp.close();
		ShellExecuteA(AfxGetMainWnd()->m_hWnd,"open","yhy.htm",NULL,"",SW_NORMAL);
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		this->ShowWindow(SW_SHOW);
	}
}

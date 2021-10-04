// CDlgSetup.cpp: 实现文件
//

#include "pch.h"
#include "MFCNginxPhpMgr.h"
#include "CDlgSetup.h"
#include "afxdialogex.h"
#include"../../resource/tool.h"


// CDlgSetup 对话框

IMPLEMENT_DYNAMIC(CDlgSetup, CDialogEx)

CDlgSetup::CDlgSetup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SETUP, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CDlgSetup::~CDlgSetup()
{
}

void CDlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetup, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetup::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_NGINX_PATH, &CDlgSetup::OnBnClickedButtonSetupNginxPath)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_PHP_PATH, &CDlgSetup::OnBnClickedButtonSetupPhpPath)
	ON_BN_CLICKED(IDC_CHECK_SETUP_BLOCKPHPEXIT, &CDlgSetup::OnBnClickedCheckSetupBlockphpexit)
	ON_BN_CLICKED(IDOK, &CDlgSetup::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetup 消息处理程序


BOOL CDlgSetup::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_EDIT_SETUP_BLOCKPHP_T, L"5000");
	if (file_exists("wnmp_conf.bin")) {
		unsigned bpt = 0;
		{
			fstream fp("php_cgi_timer.bin", ios::in | ios::binary);
			fp.read((char*)&bpt, sizeof(unsigned));
			fp.close();
		}
		SetDlgItemInt(IDC_EDIT_SETUP_BLOCKPHP_T, bpt, 0);
		SetDlgItemText(IDC_EDIT_SETUP_PHP, s2wc(progSet.php_path));
		SetDlgItemText(IDC_EDIT_SETUP_NGINX, s2wc(progSet.nginx_path));
		((CButton*)GetDlgItem(IDC_CHECK_SETUP_BLOCKPHPEXIT))->SetCheck(progSet.BlockPhpExit);
		GetDlgItem(IDC_EDIT_SETUP_BLOCKPHP_T)->EnableWindow(progSet.BlockPhpExit);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgSetup::OnBnClickedCancel(){
	EndDialog(IDCANCEL);
}


void CDlgSetup::OnBnClickedButtonSetupNginxPath(){
	CString gReadFilePathName;
	CFileDialog fileDlg(true, _T(""), _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("nginx主程序|nginx.exe||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框
	{
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名
		GetDlgItem(IDC_EDIT_SETUP_NGINX)->SetWindowText(gReadFilePathName);//将路径显示
	}
}


void CDlgSetup::OnBnClickedButtonSetupPhpPath(){
	CString gReadFilePathName;
	CFileDialog fileDlg(true, _T(""), _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("php-cgi|php-cgi.exe||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框
	{
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名
		GetDlgItem(IDC_EDIT_SETUP_PHP)->SetWindowText(gReadFilePathName);//将路径显示
	}
}


void CDlgSetup::OnBnClickedCheckSetupBlockphpexit(){
	if (((CButton*)GetDlgItem(IDC_CHECK_SETUP_BLOCKPHPEXIT))->GetCheck()) {
		GetDlgItem(IDC_EDIT_SETUP_BLOCKPHP_T)->EnableWindow(1);
	}
	else {
		GetDlgItem(IDC_EDIT_SETUP_BLOCKPHP_T)->EnableWindow(0);
	}
}


void CDlgSetup::OnBnClickedOk(){
	progSet.BlockPhpExit = ((CButton*)GetDlgItem(IDC_CHECK_SETUP_BLOCKPHPEXIT))->GetCheck();
	if (((CButton*)GetDlgItem(IDC_CHECK_SETUP_BLOCKPHPEXIT))->GetCheck()) {
		fstream fp("php_cgi_timer.bin", ios::out | ios::binary);
		unsigned a = GetDlgItemInt(IDC_EDIT_SETUP_BLOCKPHP_T, 0, 0);
		if (a < 2000) {
			(MessageBox(L"您输入的延时太小了!换一个吧", L"Setup", MB_ICONINFORMATION)); return;
		}
		fp.write((char*)&a, sizeof(unsigned));
		fp.close();
		a = 1;
		fp.open("wnmp_conf_php_t.bin", ios::out | ios::binary);
		fp.write((char*)&a, sizeof a);
		fp.close();
	}
	CString str; unsigned tmpu = 0; string tmps;
	GetDlgItemText(IDC_EDIT_SETUP_NGINX, str);
	if (str.GetLength() < 2) {
		return (void)MessageBox(L"请输入nginx路径!", L"Setup", MB_ICONERROR);
	}
	//progSet.nginx_path = ws2s(str.GetBuffer());
	tmps = ws2s(str.GetBuffer()); 
	progSet.nginx_path = tmps;
	GetDlgItemText(IDC_EDIT_SETUP_PHP, str);
	if (str.GetLength() < 2) {
		return (void)MessageBox(L"请输入php路径!", L"Setup", MB_ICONERROR);
	}
	//progSet.php_path = ws2s(str.GetBuffer());
	tmps = ws2s(str.GetBuffer()); tmpu = (unsigned)(tmps.length());
	progSet.php_path = tmps;
	fstream fp("wnmp_conf.bin", ios::out | ios::binary);fp.close();
	fp.open("wnmp_conf_nginx.bin", ios::out);
	fp << progSet.nginx_path << endl;
	fp.close();
	fp.open("wnmp_conf_php.bin", ios::out);
	fp << progSet.php_path << endl;
	fp.close();
	EndDialog(0);
}


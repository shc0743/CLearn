// CDlgCheckPhpCgi.cpp: 实现文件
//

#include "pch.h"
#include "MFCNginxPhpMgr.h"
#include "CDlgCheckPhpCgi.h"
#include "afxdialogex.h"
#include "../../resource/tool.h"


// CDlgCheckPhpCgi 对话框

IMPLEMENT_DYNAMIC(CDlgCheckPhpCgi, CDialogEx)

CDlgCheckPhpCgi::CDlgCheckPhpCgi(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CHECK_PHP_CGI, pParent)
{

}

CDlgCheckPhpCgi::~CDlgCheckPhpCgi()
{
}

void CDlgCheckPhpCgi::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCheckPhpCgi, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgCheckPhpCgi 消息处理程序


void __stdcall CheckPhpCgi(HWND h, UINT u, UINT_PTR id, DWORD d) {
	HANDLE hdl = ::GetProcessHandle(L"php-cgi.exe");
	if (hdl == NULL) {
		//php没了,重启她
		string ngf = progSet.php_path;
		ngf = ngf.substr(0, ngf.find_last_of("php-cgi.exe") - 11);
		::SetCurrentDirectoryA(ngf.c_str());
		PROCESS_INFORMATION pi = Process.Start_HiddenWindow(s2ws("\"" + progSet.php_path + "\" -b 127.0.0.1:9000 -c \"" + ngf + "\\php.ini\""));
		Sleep(500);
		::SetCurrentDirectoryW(s2ws(::GetProgramInfo().path).c_str());
		fstream log("php_die_log.log", ios::app);
		log << "[timestamp " << time(0) << "] php-cgi exited,restart it,";
		if (pi.hProcess) log << "success,handle " << pi.hProcess << "." << endl; else log << "failed." << endl;
		WaitForSingleObject(pi.hProcess, progSet.PhpCheck);
	}
}


BOOL CDlgCheckPhpCgi::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (GetProcessHandle(L"php-cgi.exe") == NULL) {
		//不存在php进程，直接退出
		exit(0);
	}
	if (!file_exists("php_cgi_timer.bin")) {
		fstream fp("php_cgi_timer.bin", ios::out | ios::binary);
		unsigned int timer = 5000;
		fp.write((char*)&timer, sizeof(int));
		fp.close();
	}
	fstream fp("php_cgi_timer.bin", ios::in | ios::binary);
	unsigned int timer = 0;
	fp.read((char*)&timer, sizeof(int));
	fp.close();

	::SetTimer(this->GetSafeHwnd(), 747244, timer, CheckPhpCgi);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgCheckPhpCgi::OnPaint()
{
	ShowWindow(0);

	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}

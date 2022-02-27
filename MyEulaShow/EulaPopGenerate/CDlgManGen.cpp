// CDlgManGen.cpp: 实现文件
//

#include "pch.h"
#include "EulaPopGenerate.h"
#include "afxdialogex.h"
#include "CDlgManGen.h"
#include "../../resource/tool.h"
using namespace std;


// CDlgManGen 对话框

IMPLEMENT_DYNAMIC(CDlgManGen, CDialogEx)

CDlgManGen::CDlgManGen(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MANGEN, pParent)
{

}

CDlgManGen::~CDlgManGen()
{
}

void CDlgManGen::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgManGen, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_SAVEMANIFEST, &CDlgManGen::OnBnClickedButtonBrowseSavemanifest)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_EULA_PATH, &CDlgManGen::OnBnClickedButtonBrowseEulaPath)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_GENMAINEXE, &CDlgManGen::OnBnClickedButtonBrowseGenmainexe)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_LEARNMORE_MANI, &CDlgManGen::OnNMClickSyslinkLearnmoreMani)
	ON_BN_CLICKED(IDOK, &CDlgManGen::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_GENOR, &CDlgManGen::OnBnClickedButtonBrowseGenor)
END_MESSAGE_MAP()


// CDlgManGen 消息处理程序


void CDlgManGen::OnBnClickedButtonBrowseSavemanifest() {
	CFileDialog openFileDlg(FALSE, L"", L"manifest.xml", OFN_EXPLORER,
		L"manifest.xml|manifest.xml|All Files (*.*)|*.*||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_SAVEMANIFEST, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CDlgManGen::OnBnClickedButtonBrowseEulaPath() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER,
		L"Text Files (*.txt;*.rtf)|*.txt;*.rtf|All Files (*.*)|*.*||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_EULA_PATH, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CDlgManGen::OnBnClickedButtonBrowseGenmainexe() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER,
		L"Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||", NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_GEN_MAINFILE, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}



void CDlgManGen::OnNMClickSyslinkLearnmoreMani(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: 在此添加控件通知处理程序代码
	MessageBoxW(L"TODO: Open help document here", L"TODO", MB_ICONINFORMATION);
	*pResult = 0;
}


void CDlgManGen::OnBnClickedButtonBrowseGenor() {
	CFileDialog openFileDlg(TRUE, L"", L"", OFN_EXPLORER,
		L"Executable (*.exe)|*.exe||", this);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK) {
		SetDlgItemTextW(IDC_EDIT_GEN_MFOR, openFileDlg.GetPathName().GetBuffer());
	}
	ShowWindow(0); ShowWindow(1);
}


void CDlgManGen::OnBnClickedOk() {
	CString mf, ea, ma, go;
	GetDlgItemText(IDC_EDIT_SAVEMANIFEST, mf);
	GetDlgItemText(IDC_EDIT_EULA_PATH, ea);
	GetDlgItemText(IDC_EDIT_GEN_MAINFILE, ma);
	GetDlgItemText(IDC_EDIT_GEN_MFOR, go);
	if (!file_exists(go.GetBuffer())) {
		MessageBoxW(L"Cannot find generator!", L"ERROR", MB_ICONERROR); return;
	}
	if (!file_exists(ea.GetBuffer())) {
		MessageBoxW(L"Cannot find EULA file!", L"ERROR", MB_ICONERROR); return;
	}
	if (!file_exists(ma.GetBuffer())) {
		MessageBoxW(L"Cannot find main file!", L"ERROR", MB_ICONERROR); return;
	}
	if (file_exists(mf.GetBuffer())) {
		if (IDYES != MessageBoxW(L"manifest.xml already exists.\nOverwrite?",
			L"Generator", MB_ICONQUESTION | MB_YESNO)) return;
	}
	wstring cl = L"\""s + go.GetBuffer() + L"\" --generate-manifest --eulaf=\"" +
		ea.GetBuffer() + L"\" --exe=\"" + ma.GetBuffer() + L"\" --out=\"" +
		mf.GetBuffer() + L"\"";

	auto pi = Process.Start_Suspended(cl);
	if (!pi.hProcess) {
		MessageBoxW(L"Cannot create process!", L"ERROR", MB_ICONERROR); return;
	}
	CloseHandle(pi.hThread);
	//MessageBoxW(L"Press [OK] to start...", L"Generator", MB_ICONINFORMATION);
	ShowWindow(0);
	DWORD code = 1;
	Process.resume(pi.hProcess);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &code);
	CloseHandle(pi.hProcess);
	ShowWindow(9);
	if (code) {
		MessageBoxW((L"Error: error code " + to_wstring(code)).c_str(),
			L"ERROR", MB_ICONERROR); return;
	} else {
		MessageBoxW(L"Success", L"Generator", MB_ICONINFORMATION);
	}
}

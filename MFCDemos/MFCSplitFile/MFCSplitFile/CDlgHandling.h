#pragma once


// CDlgHandling 对话框

class CDlgHandling;
struct CDlgHandling_SplitParamStruct {
	CDlgHandling* lpStartAddress;
	string file, outfolder;
	size_t maxsize;
	bool meta;
};

class CDlgHandling : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHandling)

public:
	CDlgHandling(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgHandling();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WAIT_FOR_DOING };
#endif

protected:
	bool paused;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	void* param_location;
	HANDLE hThread;
	enum type__t { ErrorType = 0, SplitDlg = 1, MergeDlg = 2 } type;
	static unsigned HandleSplitFile(void*);

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

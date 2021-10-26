#pragma once


// CAddServerDlg 对话框

class CAddServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddServerDlg)

public:
	CAddServerDlg(CWnd* pParent = NULL);   // 标准构造函数
	CAddServerDlg(char const *_ServerAddr, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddServerDlg();

// 对话框数据
	enum { IDD = IDD_ADDSERVER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_Edit;
};

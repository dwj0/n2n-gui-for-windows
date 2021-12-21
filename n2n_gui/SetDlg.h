#pragma once

struct NetAdapters_Struct
{
	int Index;
	char Name[64];
	char Description[64];
};

// CSetDlg 对话框

class CSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetDlg)

public:
	CSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	CSetDlg(bool _Hide, bool _Connect, char const *_Resendif, char const *_Param, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDlg();

// 对话框数据
	enum { IDD = IDD_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool bHide,bConnect;
	CString ReSendIf;
	CString m_OtherParam;
	int AdaptersCnt;
	NetAdapters_Struct *pAdapters;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

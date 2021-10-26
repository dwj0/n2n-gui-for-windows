#pragma once
#include "afxwin.h"


#define  ON_SHOWLOG_MSG		(WM_USER+1)

// CLogDlg 对话框

class CLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogDlg();

// 对话框数据
	enum { IDD = IDD_LOG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	LRESULT OnShowLogMsg(WPARAM w, LPARAM l);
public:
	CEdit m_Log;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnClrLog();
};

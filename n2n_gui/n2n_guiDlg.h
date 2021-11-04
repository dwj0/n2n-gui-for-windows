
// n2n_guiDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "SetDlg.h"
#include "colorstatic.h"

#define ON_SHOWLOG_MSG			(WM_USER+1)
#define ON_NOTIFY_ICON_MSG		(WM_USER+2)


struct SERVER_ROUTE_Struct
{
	bool	Enable;
	UCHAR	Net[4];
	UCHAR	Mask;
	UCHAR	Gate[4];
	char	Note[16];
};

struct SERVER_Struct 
{
	char	Server[128];
	char	NetName[64];
	char	NetPasswd[32];
	UCHAR	IpAddr[4];
	UCHAR	IpMask[4];
	int		RouteCnts;
	SERVER_ROUTE_Struct *pRouteList;
};


// Cn2n_guiDlg 对话框
class Cn2n_guiDlg : public CDialogEx
{
// 构造
public:
	Cn2n_guiDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_N2N_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl m_List;
	afx_msg void OnBnClickedBtnStartStop();
	void ShowSelServer(SERVER_Struct const * pServer);
	afx_msg void OnCbnSelchangeComboServerlist();
	afx_msg void OnBnClickedBtnDelServer();
	afx_msg void OnBnClickedBtnAddServer();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	CArray<SERVER_Struct,SERVER_Struct&>	ServerArray;
	void SetRoute(bool bEnable);
	virtual void OnCancel();

	bool StartN2nClient(char * Cmdline);
	afx_msg void OnBnClickedBtnSave();

	HANDLE hClientProcess, hClientRead, hServerProcess;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnClrlog();
	afx_msg void OnBnClickedBtnLog();
	CColorStatic m_ConnectStatus;

	HICON m_Icon_Connected, m_Icon_NoConnect, m_Icon_ConnectErr;
	afx_msg void OnBnClickedBtnHide();

	NOTIFYICONDATA	m_Nid;
	LRESULT OnNotifyIconMsg(WPARAM w, LPARAM l);
	void OnMenuClickedShow(void);
	bool StartN2nServer(int Port);
	int SystemBits;
	afx_msg void OnBnClickedBtnEditServer();
	void OnMenuClickedAddRoute(void);
	void OnMenuClickedDelRoute(void);
	void OnMenuClickedEditRoute(void);
	UINT ConnectTick;
	afx_msg void OnBnClickedBtnSet();
	bool bAutoHide;
	CString ReSendIf;
	CString m_OtherParam;
	CEdit m_Log;
	LRESULT OnShowLogMsg(WPARAM w, LPARAM l);
	afx_msg void OnBnClickedBtnClrLog();
};

// LogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "LogDlg.h"
#include "afxdialogex.h"


// CLogDlg 对话框

IMPLEMENT_DYNAMIC(CLogDlg, CDialogEx)

CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogDlg::IDD, pParent)
{

}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_Log);
}


BEGIN_MESSAGE_MAP(CLogDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLR_LOG, &CLogDlg::OnBnClickedBtnClrLog)
	ON_MESSAGE(ON_SHOWLOG_MSG,OnShowLogMsg)
END_MESSAGE_MAP()


// CLogDlg 消息处理程序

LRESULT CLogDlg::OnShowLogMsg(WPARAM w, LPARAM l)
{
	m_Log.SetSel(INT_MAX,-1);
	m_Log.ReplaceSel((char*)w);
	return LRESULT();
}


BOOL CLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Log.SetLimitText(500*1024);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CLogDlg::OnBnClickedBtnClrLog()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_EDIT_LOG,"");
}

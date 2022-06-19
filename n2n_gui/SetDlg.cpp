// SetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "SetDlg.h"
#include "afxdialogex.h"

NetAdapters_Struct *GetAdapters(int *Cnt);



// CSetDlg 对话框

IMPLEMENT_DYNAMIC(CSetDlg, CDialogEx)

CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
	, m_OtherParam(_T(""))
{

}

CSetDlg::CSetDlg(bool _Hide, bool _Connect, bool _Resend, char const *_Param, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
{
	bConnect=_Connect;
	bHide=_Hide;
	bReSend=_Resend;
	m_OtherParam=_Param;
	AdaptersCnt=0;
}

CSetDlg::~CSetDlg()
{
	if (pAdapters) delete pAdapters;
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetDlg 消息处理程序
BOOL CSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->SetCheck(bHide);
	((CButton*)GetDlgItem(IDC_CHECK_AUTOCONNECT))->SetCheck(bConnect);
	((CButton*)GetDlgItem(IDC_CHECK_RESEND))->SetCheck(bReSend);
	SetDlgItemText(IDC_EDIT_PARAM, m_OtherParam);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	bHide=((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->GetCheck()!=0;
	bConnect=((CButton*)GetDlgItem(IDC_CHECK_AUTOCONNECT))->GetCheck()!=0;
	bReSend=((CButton*)GetDlgItem(IDC_CHECK_RESEND))->GetCheck() != 0;
	GetDlgItemText(IDC_EDIT_PARAM,m_OtherParam);

	CDialogEx::OnOK();
}

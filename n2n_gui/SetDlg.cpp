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

CSetDlg::CSetDlg(bool _Hide, bool _Connect, char const *_Resendif, char const *_Param, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
{
	bConnect=_Connect;
	bHide=_Hide;
	ReSendIf=_Resendif;
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
char *FormatIfName(char *dst, int dstsize, char const *name, char const *Description)
{
	sprintf_s(dst,dstsize-1,"%s{%s}",name,Description);
	return dst;
}

void CSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	bHide=((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->GetCheck()!=0;
	bConnect=((CButton*)GetDlgItem(IDC_CHECK_AUTOCONNECT))->GetCheck()!=0;
	GetDlgItemText(IDC_EDIT_PARAM,m_OtherParam);

	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_IF);
	int cur=pBox->GetCurSel();
	if (cur==0)
		ReSendIf="";
	else
	{
		char str[128];
		pBox->GetLBText(cur,ReSendIf);
		for (int i=0; i<AdaptersCnt; i++)
		{
			char const *ptmp=FormatIfName(str,sizeof(str),pAdapters[i].Name,pAdapters[i].Description);
			if (strcmp(ReSendIf,ptmp)==0)
			{
				ReSendIf=pAdapters[i].Name;
				break;
			}
		}
	}
	
	CDialogEx::OnOK();
}

BOOL CSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->SetCheck(bHide);
	((CButton*)GetDlgItem(IDC_CHECK_AUTOCONNECT))->SetCheck(bConnect);
	SetDlgItemText(IDC_EDIT_PARAM, m_OtherParam);

	int n=0;
	pAdapters=GetAdapters(&AdaptersCnt);
	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_IF);
	pBox->AddString("不转发");
	for (int i=0; i<AdaptersCnt; i++)
	{
		if (strncmp(pAdapters[i].Description,"TAP-Windows Adapter V9",22)!=0)
		{
			char str[128];
			int m=pBox->AddString(FormatIfName(str,sizeof(str),pAdapters[i].Name,pAdapters[i].Description));
			if (ReSendIf==pAdapters[i].Name) n=m;
		}
	}
	pBox->SetCurSel(n);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

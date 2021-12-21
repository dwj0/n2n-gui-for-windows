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
		pBox->GetLBText(cur,ReSendIf);
		for (int i=0; i<AdaptersCnt; i++)
		{
			if (strncmp(ReSendIf,pAdapters[i].Name,strlen(pAdapters[i].Name))==0)
			{
				ReSendIf=pAdapters[i].Name;
				break;
			}
		}
	}

	/*
//	if (ReSendIf!="无转发")
	{
		CoInitialize(NULL);
		CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

		INetSharingManager * pNSM = NULL;
		HRESULT hr = ::CoCreateInstance(__uuidof(NetSharingManager),
			NULL,
			CLSCTX_ALL,
			__uuidof(INetSharingManager),
			(void**)&pNSM);

		if (!pNSM)
		{
//			wprintf(L"failed to create NetSharingManager object\r\n");
		}
		else
		{
			//shareNet(pNSM, "WLAN", "以太网 4");
			disShareNet(pNSM);
			shareNet(pNSM, ReSendIf, "本地连接 2");
			//StartHostednetwork();
		}
	}*/

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
			sprintf_s(str,sizeof(str),"%s{%s}",pAdapters[i].Name,pAdapters[i].Description);
			pBox->AddString(str);
			if (ReSendIf==pAdapters[i].Name) n=i;
		}
	}
	pBox->SetCurSel(n);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

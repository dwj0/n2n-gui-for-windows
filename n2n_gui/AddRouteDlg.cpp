// AddRouteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "AddRouteDlg.h"
#include "afxdialogex.h"


// CAddRouteDlg 对话框

IMPLEMENT_DYNAMIC(CAddRouteDlg, CDialogEx)

CAddRouteDlg::CAddRouteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddRouteDlg::IDD, pParent)
	, m_Note(_T(""))
	, bEditFlag(false)
{

}

CAddRouteDlg::CAddRouteDlg(char const *_NetAddr, UCHAR const *_Gate, char const *_Note, CWnd* pParent)
	: CDialogEx(CAddRouteDlg::IDD, pParent)
{
	m_Note=_Note;
	strncpy_s(NetAddr,sizeof(NetAddr),_NetAddr,sizeof(NetAddr));
	memcpy(GATE,_Gate,4);
	bEditFlag=true;
}

CAddRouteDlg::~CAddRouteDlg()
{
}

void CAddRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddRouteDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddRouteDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddRouteDlg 消息处理程序


BOOL CAddRouteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (bEditFlag)
	{
		SetDlgItemText(IDC_EDIT_NOTE,m_Note);
		SetDlgItemText(IDC_EDIT_NETADDR,NetAddr);
		CIPAddressCtrl *p=(CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GATE);
		p->SetAddress(GATE[0],GATE[1],GATE[2],GATE[3]);
		SetWindowText("编辑路由");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CAddRouteDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItemText(IDC_EDIT_NOTE,m_Note);
	if (m_Note.GetLength()>15)
	{
		MessageBox("备注长度超过规定值");
		return;
	}

	GetDlgItemText(IDC_EDIT_NETADDR,NetAddr,sizeof(NetAddr));
	if (!StrNetaddrToIp(NetAddr,IP,&Mask))
	{
		MessageBox("网段格式错误");
		return;
	}

	CIPAddressCtrl *p=(CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GATE);
	p->GetAddress(GATE[0],GATE[1],GATE[2],GATE[3]);

	CDialogEx::OnOK();
}

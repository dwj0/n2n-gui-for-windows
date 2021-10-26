// AddServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "AddServerDlg.h"
#include "afxdialogex.h"


// CAddServerDlg 对话框

IMPLEMENT_DYNAMIC(CAddServerDlg, CDialogEx)

CAddServerDlg::CAddServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddServerDlg::IDD, pParent)
	, m_Edit(_T(""))
{
}

CAddServerDlg::CAddServerDlg(char const *_ServerAddr, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddServerDlg::IDD, pParent)
{
	m_Edit=_ServerAddr;
}

CAddServerDlg::~CAddServerDlg()
{
}

void CAddServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CAddServerDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddServerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddServerDlg 消息处理程序


void CAddServerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	int n=m_Edit.ReverseFind(':');
	if (n>2)
	{
		CString numberstr=m_Edit.Right(m_Edit.GetLength()-n-1);
		if (atoi(numberstr)>0)
		{
			CDialogEx::OnOK();
			return;
		}
	}
	MessageBox("格式错误");
}


BOOL CAddServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_Edit != "")
	{
		SetWindowText("编辑服务器");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

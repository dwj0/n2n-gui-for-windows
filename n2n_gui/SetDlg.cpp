// SetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "SetDlg.h"
#include "afxdialogex.h"
#include  < winsvc.h >


// CSetDlg 对话框

IMPLEMENT_DYNAMIC(CSetDlg, CDialogEx)

CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
{

}

CSetDlg::CSetDlg(bool _Hide,CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
{
	bHide=_Hide;
}

CSetDlg::~CSetDlg()
{
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_RESEND, &CSetDlg::OnBnClickedCheckResend)
END_MESSAGE_MAP()


// CSetDlg 消息处理程序


void CSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	bHide=((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->GetCheck()==1;
	CDialogEx::OnOK();
}

void SetServiceStartType(SC_HANDLE hSC,SC_HANDLE hSvc)
{
	UCHAR Buf[500];
	LPQUERY_SERVICE_CONFIG lpsc=(LPQUERY_SERVICE_CONFIG)Buf; 
	DWORD dwBytesNeeded; 

	if( !QueryServiceConfig( hSvc, lpsc, sizeof(Buf), &dwBytesNeeded))
	{
	/*	DWORD dwError = GetLastError();
		if( ERROR_INSUFFICIENT_BUFFER == dwError )
			printf("内存分配不足", dwError);
		else
			printf("QueryServiceConfig failed (%d)", dwError);*/
	}

	else if (lpsc->dwStartType==SERVICE_DISABLED)
	{
		SC_LOCK sclLock = LockServiceDatabase(hSC);
		if (sclLock == NULL)
		{
		/*	if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED)
				printf("LockServiceDatabase error/n");*/
		}
		if (! ChangeServiceConfig(
			hSvc,					// handle of service
			SERVICE_NO_CHANGE,		// service type: no change
			SERVICE_DEMAND_START,	// 这里做了更改
			SERVICE_NO_CHANGE,		// error control: no change
			NULL,					// binary path: no change
			NULL,				    // load order group: no change
			NULL,				    // tag ID: no change
			NULL,					// dependencies: no change
			NULL,					// account name: no change
			NULL,					// password: no change
			NULL))					//displayname
		{
		//	printf("ChangeServiceConfig error!/n");
		}
		UnlockServiceDatabase(sclLock);
	}
}

void CSetDlg::OnBnClickedCheckResend()
{
	// TODO: 在此添加控件通知处理程序代码
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSC == NULL)
	{
		TRACE( "open SCManager error");
		return;
	}
	// 打开RemoteAccess服务。
	SC_HANDLE hSvc = ::OpenService( hSC, "RemoteAccess", SERVICE_ALL_ACCESS);
	if( hSvc == NULL)
	{
		TRACE( "Open www erron。");
		::CloseServiceHandle( hSC);
		return;
	}
	// 获得服务的状态
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		TRACE( "Get Service state error。");
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return;
	}

	int Enable=((CButton*)GetDlgItem(IDC_CHECK_RESEND))->GetCheck();
	//开启 RemoteAccess 服务
	if (Enable)
	{
		if( status.dwCurrentState == SERVICE_RUNNING)
		{
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return;
		}
		// 更改启动类型，防止被禁用
		SetServiceStartType(hSC,hSvc);
		// 启动服务
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{
			TRACE( "start service error。");
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			((CButton*)GetDlgItem(IDC_CHECK_RESEND))->SetCheck(FALSE);
			return;
		}
		// 等待服务启动
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				AfxMessageBox( "start success。");
				::CloseServiceHandle( hSvc);
				::CloseServiceHandle( hSC);
				return;
			}
			::Sleep( status.dwWaitHint>100 ? 100:status.dwWaitHint);
		}
	}
	//关闭 RemoteAccess 服务
	else		//if (Enable)
	{
		if( status.dwCurrentState == SERVICE_STOPPED)
		{
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return;
		}
		// 停止服务
		if( ::ControlService( hSvc, SERVICE_CONTROL_STOP, &status) == FALSE)
		{
			TRACE( "stop service error。");
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			((CButton*)GetDlgItem(IDC_CHECK_RESEND))->SetCheck(TRUE);
			return;
		}
		// 等待服务停止
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			if( status.dwCurrentState == SERVICE_STOPPED)
			{
				AfxMessageBox( "stop success。");
				::CloseServiceHandle( hSvc);
				::CloseServiceHandle( hSC);
				return;
			}
			::Sleep( status.dwWaitHint>1000 ? 1000:status.dwWaitHint);
		}
	}
}


BOOL CSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CHECK_AUTOHIDE))->SetCheck(bHide);
	// 打开服务管理对象
	SC_HANDLE hSC = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		TRACE( "open SCManager error");
		return TRUE;
	}
	// 打开RemoteAccess服务。
	SC_HANDLE hSvc = ::OpenService( hSC, "RemoteAccess", SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if( hSvc == NULL)
	{
		TRACE( "Open www erron。");
		::CloseServiceHandle( hSC);
		return TRUE;
	}
	// 获得服务的状态
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		TRACE( "Get Service state error。");
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return TRUE;
	}
	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	if( status.dwCurrentState == SERVICE_RUNNING)
		((CButton*)GetDlgItem(IDC_CHECK_RESEND))->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

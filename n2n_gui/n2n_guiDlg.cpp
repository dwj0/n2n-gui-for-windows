
// n2n_guiDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "n2n_guiDlg.h"
#include "afxdialogex.h"
#include "AddServerDlg.h"
#include "AddRouteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库


void SafeGetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL == lpSystemInfo)
		return;
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo nsInfo =
		(LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");
	if (NULL != nsInfo)
	{
		nsInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}

int GetSystemBits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return 64;
	}
	return 86;
}

bool GetProfileServersInfo(char const *ProFile, char const *AppName, SERVER_Struct *pServer)
{
	char str[1024];
	int len=GetPrivateProfileString(AppName,"Server","",str,sizeof(str),ProFile);
	if (len==0 || len>=sizeof(pServer->Server)) return false;
	strcpy_s(pServer->Server,sizeof(pServer->Server),str);

	len=GetPrivateProfileString(AppName,"NetName","",str,sizeof(str),ProFile);
	if (len==0 || len>=sizeof(pServer->NetName)) return false;
	strcpy_s(pServer->NetName,sizeof(pServer->NetName),str);

	len=GetPrivateProfileString(AppName,"NetPasswd","",str,sizeof(str),ProFile);
	if (len>=sizeof(pServer->NetPasswd)) return false;
	strcpy_s(pServer->NetPasswd,sizeof(pServer->NetPasswd),str);

	len=GetPrivateProfileString(AppName,"LocalIP","",str,sizeof(str),ProFile);
	if (len>=16 || !StripToIp(str,pServer->IpAddr)) return false;

	len=GetPrivateProfileString(AppName,"Mask","",str,sizeof(str),ProFile);
	if (len>=16 || !StripToIp(str,pServer->IpMask)) return false;

	//路由表
	//先读取到动态数组中
	CArray<SERVER_ROUTE_Struct,SERVER_ROUTE_Struct&>	Array;
	len=GetPrivateProfileString(AppName,"Route","",str,sizeof(str),ProFile);
	for (int n=0; n<len; )
	{
		char *pStart=str+n, *pEnd=strchr(pStart,';'),*pt;
		if (pEnd)
		{
			*pEnd++=0;
			n+=pEnd-pStart;
		}
		//解析格式: 1 192.168.1.0/24 10.0.0.1 公司
		SERVER_ROUTE_Struct route={0};
		int enable,mask,ip[4],gate[4];
		int m=sscanf_s(pStart,"%d %d.%d.%d.%d/%d %d.%d.%d.%d ",&enable,ip,ip+1,ip+2,ip+3,&mask,gate,gate+1,gate+2,gate+3);
		if (m!=10 || enable>1 || enable<0 || mask<16 || mask>32) break;
		route.Enable=enable!=0;
		route.Mask=(UCHAR)mask;
		for (int i=0; i<4; i++)
		{
			route.Net[i]=ip[i];
			route.Gate[i]=gate[i];
		}
		pt=strrchr(pStart,' ');
		if (pt)
			strncpy_s(route.Note,sizeof(route.Note),pt+1,sizeof(route.Note)-1);

		Array.Add(route);
		if (pEnd==NULL) break;
	}
	pServer->RouteCnts=Array.GetCount();
	pServer->pRouteList=NULL;
	if (pServer->RouteCnts)
	{
		pServer->pRouteList = new SERVER_ROUTE_Struct[pServer->RouteCnts];
		if (pServer->pRouteList)
		{
			for (int i=0; i<pServer->RouteCnts; i++)
				pServer->pRouteList[i]=Array[i];
		}
		else
			pServer->RouteCnts=0;
	}

	return true;
}

bool CheckTapAdapters()
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO[10];
	unsigned long stSize = sizeof(IP_ADAPTER_INFO)*10;
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	bool flag=false;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
	}
	if (ERROR_SUCCESS == nRel)
	{
		//可能有多网卡,因此通过循环去判断
		for (PIP_ADAPTER_INFO p=pIpAdapterInfo;p!=NULL;p=p->Next)
		{
			if (strcmp(p->Description,"TAP-Windows Adapter V9")==0)
			{
				flag=true;
				break;
			}
		}
	}
	delete pIpAdapterInfo;
	return flag;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cn2n_guiDlg 对话框




Cn2n_guiDlg::Cn2n_guiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cn2n_guiDlg::IDD, pParent)
	, SystemBits(0)
	, ConnectTick(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hClientProcess= hClientRead= hServerProcess=0;
	SystemBits=GetSystemBits();
}

void Cn2n_guiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_STATIC_CONNECT_STATUS, m_ConnectStatus);
}

BEGIN_MESSAGE_MAP(Cn2n_guiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Cn2n_guiDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_START_STOP, &Cn2n_guiDlg::OnBnClickedBtnStartStop)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERLIST, &Cn2n_guiDlg::OnCbnSelchangeComboServerlist)
	ON_BN_CLICKED(IDC_BTN_DEL_SERVER, &Cn2n_guiDlg::OnBnClickedBtnDelServer)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &Cn2n_guiDlg::OnNMRClickList1)
	ON_BN_CLICKED(IDC_BTN_ADD_SERVER, &Cn2n_guiDlg::OnBnClickedBtnAddServer)
	ON_BN_CLICKED(IDC_BTN_SAVE, &Cn2n_guiDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(ID_MENU_SHOW, &Cn2n_guiDlg::OnMenuClickedShow)
	ON_BN_CLICKED(ID_MENU_ADD_ROUTE,&Cn2n_guiDlg::OnMenuClickedAddRoute)
	ON_BN_CLICKED(ID_MENU_DEL_ROUTE,&Cn2n_guiDlg::OnMenuClickedDelRoute)
	ON_BN_CLICKED(ID_MENU_EDIT_ROUTE,&Cn2n_guiDlg::OnMenuClickedEditRoute)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LOG, &Cn2n_guiDlg::OnBnClickedBtnLog)
	ON_MESSAGE(ON_MOVE_LOGWINDOW_MSG,OnMoveLogWindowMsg)
	ON_MESSAGE(ON_NOTIFY_ICON_MSG,OnNotifyIconMsg)
	ON_BN_CLICKED(IDC_BTN_HIDE, &Cn2n_guiDlg::OnBnClickedBtnHide)
	ON_BN_CLICKED(IDC_BTN_EDIT_SERVER, &Cn2n_guiDlg::OnBnClickedBtnEditServer)
END_MESSAGE_MAP()

// Cn2n_guiDlg 消息处理程序

BOOL Cn2n_guiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//
	CFont Font;
	Font.CreateFont(16,12,0,0,FW_THIN,false,false,false,
		CHINESEBIG5_CHARSET,OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,
		FF_MODERN,"宋体");
	m_ConnectStatus.SetTextFont(Font);

	//列表框
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_List.InsertColumn(0,"网段",0,132);
	m_List.InsertColumn(1,"网关",0,96);
	m_List.InsertColumn(2,"备注",0,64);

	char str[2048],ProfilePath[MAX_PATH];
	sprintf_s(ProfilePath,sizeof(ProfilePath),"%sn2n.ini",ProPath);
	//读取服务器列表
	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST);
	int len=GetPrivateProfileSectionNames(str,sizeof(str),ProfilePath);
	for (int i=0; i<len; )
	{
		char *p=str+i;
		i+=strlen(p)+1;
		if (memcmp(p,"SERVER_No",9)==0 && p[9]>='0' && p[9]<='9')
		{
			SERVER_Struct Host;
			if (GetProfileServersInfo(ProfilePath,p,&Host)) 
			{
				pBox->AddString(Host.Server);
				ServerArray.Add(Host);
			}
		}
	}
	//读取选择的服务器
	if (pBox->GetCount()>0)
	{
		int Sel=GetPrivateProfileInt("Config","LastSel",0,ProfilePath);
		if (Sel>=pBox->GetCount()) Sel=0;
		pBox->SetCurSel(Sel);
		OnCbnSelchangeComboServerlist();
	}
	//服务端
	int Enable=GetPrivateProfileInt("SERVER","Enable",0,ProfilePath);
	int Port=GetPrivateProfileInt("SERVER","Port",1235,ProfilePath);
	SetDlgItemInt(IDC_EDIT_SERVER_PORT,Port);
	((CButton*)GetDlgItem(IDC_CHECK_SERVER))->SetCheck(Enable);

	m_LogDlg.Create(IDD_LOG_DLG,this);
	PostMessage(ON_MOVE_LOGWINDOW_MSG,0,0);		//移动位置，在这移动的话主窗口位置还未确定
	//检测网卡
	m_Icon_Connected=AfxGetApp()->LoadIcon(IDI_ICON1);
	m_Icon_NoConnect=AfxGetApp()->LoadIcon(IDI_ICON2);
	m_Icon_ConnectErr=AfxGetApp()->LoadIcon(IDI_ICON3);
	if (!CheckTapAdapters()) 
	{
		SetDlgItemText(IDC_BTN_START_STOP,"安装网卡");
		SetDlgItemText(IDC_STATIC_CONNECT_STATUS,"未安装");
		m_ConnectStatus.SetColor(RGB(255,0,0));
		((CStatic*)GetDlgItem(IDC_PIC_CONNECT))->SetIcon(m_Icon_ConnectErr);
	}
	else
	{
		m_ConnectStatus.SetColor(RGB(155,100,75));
		((CStatic*)GetDlgItem(IDC_PIC_CONNECT))->SetIcon(m_Icon_NoConnect);
	}

	//添加托盘
	m_Nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	m_Nid.hWnd=m_hWnd;	
	m_Nid.uID=IDR_MAINFRAME;	
	m_Nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;	
	m_Nid.uCallbackMessage=ON_NOTIFY_ICON_MSG;		//自定义的消息名称	
	m_Nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(m_Nid.szTip,sizeof(m_Nid.szTip),"n2n Gui 未连接");//信息提示条	
	Shell_NotifyIcon(NIM_ADD,&m_Nid);				//在托盘区添加图标
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cn2n_guiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cn2n_guiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cn2n_guiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cn2n_guiDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

}

void Cn2n_guiDlg::SetRoute(bool bEnable)
{
	char Cmd[64],ip[24],gate[20];
	
	for (int i=0; i<m_List.GetItemCount();i++)
	{
		if (!m_List.GetCheck(i)) continue;
		m_List.GetItemText(i,0,ip,sizeof(ip));
		m_List.GetItemText(i,1,gate,sizeof(gate));
		if (bEnable)
			sprintf_s(Cmd,sizeof(Cmd),"route add %s %s",ip,gate);
		else
			sprintf_s(Cmd,sizeof(Cmd),"route delete %s",ip);
		TRACE("%s\r\n",Cmd);
		WinExec(Cmd,SW_HIDE);
	}
	if (bEnable) m_LogDlg.SendMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------添加路由完成.----------------------\r\n",0);
}

DWORD CALLBACK	ReadLogThread(LPVOID lp)
{
	Cn2n_guiDlg *pDlg = (Cn2n_guiDlg*)lp;
	bool bConnected=false;
	
	while (1)
	{
		char str[4096];
		DWORD bytesRead;

		if (ReadFile(pDlg->hClientRead,str,4095,&bytesRead,NULL)==NULL) break;
		str[bytesRead]=0;
		pDlg->m_LogDlg.SendMessage(ON_SHOWLOG_MSG,(WPARAM)str,0);
		//查找：[OK] Edge Peer <<< ================ >>> Super Node
		if (!bConnected)
		{
			if (strstr(str,"[OK] Edge Peer <<< ================ >>> Super Node")!=NULL)
			{
				bConnected=true;
				pDlg->SetDlgItemText(IDC_STATIC_CONNECT_STATUS,"已连接");	
				pDlg->m_ConnectStatus.SetColor(RGB(0,200,0));
				((CStatic*)pDlg->GetDlgItem(IDC_PIC_CONNECT))->SetIcon(pDlg->m_Icon_Connected);
				strcpy_s(pDlg->m_Nid.szTip,sizeof(pDlg->m_Nid.szTip),"n2n Gui 已连接");	
				Shell_NotifyIcon(NIM_MODIFY,&pDlg->m_Nid);				//修改托盘区图标
			}
		}
	}
	TRACE("线程退出\r\n");

	return 0;
}

bool Cn2n_guiDlg::StartN2nServer(int Port)
{
	char Cmd[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;  
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	int len=sprintf_s(Cmd,MAX_PATH,"%sn2n_client\\x%d\\supernode_v2_n2n.exe -l %d",ProPath,SystemBits,Port);
	TRACE("%s\r\n",Cmd);

	if (!CreateProcess(NULL, Cmd,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{
		return FALSE;
	}
	hServerProcess = pi.hProcess;
	return true;
}

bool Cn2n_guiDlg::StartN2nClient(char * Cmdline)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead,&hWrite,&sa,0))
		return FALSE;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;		//把创建进程的标准错误输出重定向到管道输入
	si.hStdOutput = hWrite;		//把创建进程的标准输出重定向到管道输入
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN
	if (!CreateProcess(NULL, Cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return FALSE;
	}

	CloseHandle(hWrite);
	CreateThread(NULL,0,ReadLogThread,this,0,NULL);

	hClientProcess=pi.hProcess;
	hClientRead=hRead;

	return true;
}

void Cn2n_guiDlg::OnBnClickedBtnStartStop()
{
	// TODO: 在此添加控件通知处理程序代码
	char Name[sizeof(((SERVER_Struct*)0)->NetName)],Passwd[sizeof(((SERVER_Struct*)0)->NetPasswd)];
	char Server[sizeof(((SERVER_Struct*)0)->Server)];
	char ClinePath[MAX_PATH+100],str1[32],str2[20];
	UCHAR ip[4],mask[4];

	GetDlgItemText(IDC_BTN_START_STOP,Name,10);
	if (strcmp(Name,"安装网卡")==0)
	{
		if (!SHGetSpecialFolderPath(m_hWnd,str1,CSIDL_PROGRAM_FILES,false))
			strcpy_s(str1,sizeof(str1),"C:\\Program Files");
		sprintf_s(ClinePath,MAX_PATH,"%sn2n_client\\tap-windows-9.21.2.exe /S /D=\"%s\\TAP-Windows\"",ProPath,str1);
		WinExec(ClinePath,SW_SHOW);
		KillTimer(2);
		SetTimer(2,1000,NULL);
	}
	else if (strcmp(Name,"启动")==0)
	{
		//----------------------------启动客户端-----------------------------
		if (GetDlgItemText(IDC_EDIT_NETNAME,Name,sizeof(Name))>0 && 
			GetDlgItemText(IDC_COMBO_SERVERLIST,Server,sizeof(Server))>0 && strchr(Server,':')!=NULL)
		{
			GetDlgItemText(IDC_EDIT_PASSWD,Passwd,sizeof(Passwd));
			CIPAddressCtrl *pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP);
			pIP->GetAddress(ip[0],ip[1],ip[2],ip[3]);
			pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MASK);
			pIP->GetAddress(mask[0],mask[1],mask[2],mask[3]);
			//执行N2N
			int len=sprintf_s(ClinePath,MAX_PATH,"%sn2n_client\\x%d\\edge_v2_n2n.exe -a %s -s %s -c %s -l %s",
				ProPath,SystemBits,IpToStrip(ip,str1),IpToStrip(mask,str2),Name,Server);
			if (Passwd[0]!=0)
				sprintf_s(ClinePath+len,sizeof(ClinePath)-len," -k %s",Passwd);
			TRACE("%s\r\n",ClinePath);
			if (StartN2nClient(ClinePath))
			{
				ConnectTick=0;
				SetTimer(1,250,NULL);	//添加路由,我的电脑上测试需要延时大于3600ms，路由才能生效,定时时间到后添加路由
				SetDlgItemText(IDC_STATIC_CONNECT_STATUS,"正在连接");
				m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------N2N客户端启动...----------------------\r\n");
			}
			else
				m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------N2N客户端失败.----------------------\r\n");
		}
		//----------------------------启动服务端-----------------------------
		int bEnable = ((CButton*)GetDlgItem(IDC_CHECK_SERVER))->GetCheck();
		int Port = GetDlgItemInt(IDC_EDIT_SERVER_PORT);
		if (bEnable && Port>0 && Port<65535)
		{
			if (StartN2nServer(Port))
				m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------N2N服务端端启动...----------------------\r\n");
		}
		//禁用控件
		for (int id=0; id<=10; id++)
			GetDlgItem(IDC_COMBO_SERVERLIST+id)->EnableWindow(FALSE);
		SetDlgItemText(IDC_BTN_START_STOP,"停止");
	}
	else
	{
		//删除路由
		SetRoute(false);
		//关闭进程
		if (hServerProcess!=0) 
		{
			TerminateProcess(hServerProcess,0);
			m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------N2N服务端关闭----------------------\r\n");
		}
		if (hClientProcess!=0) 
		{
			TerminateProcess(hClientProcess,0);
			Sleep(100);			//等待线程退出
			if (hClientRead!=0) CloseHandle(hClientRead);
			Sleep(100);
			hServerProcess=0;
			hClientProcess=0;
			hClientRead=0;
			SetDlgItemText(IDC_STATIC_CONNECT_STATUS,"未连接");
			m_ConnectStatus.SetColor(RGB(155,100,75));
			((CStatic*)GetDlgItem(IDC_PIC_CONNECT))->SetIcon(m_Icon_NoConnect);
			m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"----------------------N2N客户端关闭----------------------\r\n\r\n");
			strcpy_s(m_Nid.szTip,sizeof(m_Nid.szTip),"n2n Gui 未连接");
			Shell_NotifyIcon(NIM_MODIFY,&m_Nid);				//修改托盘区图标
			//
			KillTimer(1);
		}
		//启用控件
		for (int id=0; id<=10; id++)
			GetDlgItem(IDC_COMBO_SERVERLIST+id)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BTN_START_STOP,"启动");
	}
}

void Cn2n_guiDlg::ShowSelServer(SERVER_Struct const * pServer)
{
	char str[32];
	m_List.DeleteAllItems();
	for (int i=0; i<pServer->RouteCnts; i++)
	{
		SERVER_ROUTE_Struct *route=pServer->pRouteList+i;
		sprintf_s(str,sizeof(str),"%d.%d.%d.%d/%d",route->Net[0],route->Net[1],route->Net[2],route->Net[3],route->Mask);
		m_List.InsertItem(i,str);
		if (route->Enable) m_List.SetCheck(i);
		m_List.SetItemText(i,1,IpToStrip(route->Gate,str));
		m_List.SetItemText(i,2,route->Note);
	}
	SetDlgItemText(IDC_EDIT_NETNAME,pServer->NetName);
	SetDlgItemText(IDC_EDIT_PASSWD,pServer->NetPasswd);
	CIPAddressCtrl *pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP);
	pIP->SetAddress(pServer->IpAddr[0],pServer->IpAddr[1],pServer->IpAddr[2],pServer->IpAddr[3]);
	pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MASK);
	pIP->SetAddress(pServer->IpMask[0],pServer->IpMask[1],pServer->IpMask[2],pServer->IpMask[3]);
}


void Cn2n_guiDlg::OnCbnSelchangeComboServerlist()
{
	// TODO: 在此添加控件通知处理程序代码
	int Sel=((CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST))->GetCurSel();
	if (Sel!=-1)
	{
		SERVER_Struct Host=ServerArray[Sel];
		ShowSelServer(&Host);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_NETNAME,"");
		SetDlgItemText(IDC_EDIT_PASSWD,"");
		CIPAddressCtrl *pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP);
		pIP->SetAddress(0,0,0,0);
		pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MASK);
		pIP->SetAddress(0,0,0,0);
		m_List.DeleteAllItems();
	}
}

void Cn2n_guiDlg::OnBnClickedBtnDelServer()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST);
	int sel=pBox->GetCurSel();
	if (sel!=-1)
	{
		pBox->DeleteString(sel);
		delete ServerArray[sel].pRouteList;
		ServerArray.RemoveAt(sel);
		if (sel>=pBox->GetCount() && sel>0) sel--;
		pBox->SetCurSel(sel);
		OnCbnSelchangeComboServerlist();
	}
}

void Cn2n_guiDlg::OnBnClickedBtnAddServer()
{
	// TODO: 在此添加控件通知处理程序代码
	CAddServerDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST);
		pBox->AddString(dlg.m_Edit);
		SERVER_Struct Host;
		memset(&Host,0,sizeof(Host));
		strcpy_s(Host.Server,sizeof(Host.Server),dlg.m_Edit);
		ServerArray.Add(Host);
		int n=pBox->GetCount();
		pBox->SetCurSel(n-1);
		OnCbnSelchangeComboServerlist();
	}
}

void Cn2n_guiDlg::OnBnClickedBtnEditServer()
{
	// TODO: 在此添加控件通知处理程序代码
	char ServerAddr[sizeof(((SERVER_Struct*)0)->Server)];
	GetDlgItemText(IDC_COMBO_SERVERLIST,ServerAddr,sizeof(ServerAddr));
	CAddServerDlg dlg(ServerAddr);
	if (dlg.DoModal()==IDOK)
	{
		CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST);
		int sel=pBox->GetCurSel();
		pBox->DeleteString(sel);
		pBox->InsertString(sel,dlg.m_Edit);
		SERVER_Struct &Host=ServerArray[sel];
		strcpy_s(Host.Server,dlg.m_Edit);
		pBox->SetCurSel(sel);
		OnCbnSelchangeComboServerlist();
	}
}

void Cn2n_guiDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int SelCnt=m_List.GetSelectedCount();

	CMenu Menu;
	Menu.LoadMenu(IDR_MENU1);
	CMenu *pSubMenu=Menu.GetSubMenu(SelCnt!=0 ? 0:1);
	CPoint point;
	GetCursorPos(&point);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,point.x,point.y,this);

	*pResult = 0;
}

void Cn2n_guiDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	char Name[6];
	GetDlgItemText(IDC_BTN_START_STOP,Name,5);
	if (strcmp(Name,"停止")==0)
		OnBnClickedBtnStartStop();
	::Shell_NotifyIcon(NIM_DELETE,&m_Nid);  

	for (int i=0; i<ServerArray.GetCount(); i++)
	{
		if (ServerArray[i].pRouteList) delete ServerArray[i].pRouteList;
	}
	CDialogEx::OnCancel();
}

void Cn2n_guiDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	char str[20],ProFileName[MAX_PATH];
	sprintf_s(ProFileName,sizeof(ProFileName),"%sn2n.ini",ProPath);
	//删除所有配置
	DeleteFile(ProFileName);
	//服务端
	int Enable=((CButton*)GetDlgItem(IDC_CHECK_SERVER))->GetCheck();
	int Port=GetDlgItemInt(IDC_EDIT_SERVER_PORT);
	if (Port==0)
	{
		MessageBox("请输入服务端端口号.");
		return;
	}
	WritePrivateProfileString("SERVER","Enable",Itoa(Enable,str),ProFileName);
	WritePrivateProfileString("SERVER","Port",Itoa(Port,str),ProFileName);
	//读取当前参数
	int n=((CComboBox*)GetDlgItem(IDC_COMBO_SERVERLIST))->GetCurSel();
	if (n==-1) return;
	WritePrivateProfileString("Config","LastSel",Itoa(n,str),ProFileName);

	char Name[sizeof(((SERVER_Struct*)0)->NetName)],Passwd[sizeof(((SERVER_Struct*)0)->NetPasswd)];
	SERVER_Struct &NowHost=ServerArray[n];
	UCHAR ip[4],mask[4];
	if (GetDlgItemText(IDC_EDIT_NETNAME,Name,sizeof(Name))==0)
	{
		MessageBox("请填写虚拟网络名称。");
		return;
	}
	GetDlgItemText(IDC_EDIT_PASSWD,Passwd,sizeof(Passwd));
	CIPAddressCtrl *pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP);
	pIP->GetAddress(ip[0],ip[1],ip[2],ip[3]);
	pIP = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_MASK);
	pIP->GetAddress(mask[0],mask[1],mask[2],mask[3]);
	strcpy_s(NowHost.NetName,sizeof(NowHost.NetName),Name);
	strcpy_s(NowHost.NetPasswd,sizeof(NowHost.NetPasswd),Passwd);
	memcpy(NowHost.IpAddr,ip,4);
	memcpy(NowHost.IpMask,mask,4);
	n=m_List.GetItemCount();
	if (n!=NowHost.RouteCnts)
	{
		if (NowHost.pRouteList) 
			delete NowHost.pRouteList;
		NowHost.RouteCnts=0;
		if (n>0)
		{
			NowHost.pRouteList=new SERVER_ROUTE_Struct[n];
			if (NowHost.pRouteList) NowHost.RouteCnts=n;
		}
	}
	for (n=0; n<NowHost.RouteCnts; n++)
	{
		SERVER_ROUTE_Struct *proute=NowHost.pRouteList+n;
		proute->Enable=m_List.GetCheck(n)==TRUE;
		m_List.GetItemText(n,0,str,20);
		StrNetaddrToIp(str,proute->Net,&proute->Mask);
		m_List.GetItemText(n,1,str,16);
		StripToIp(str,proute->Gate);
		m_List.GetItemText(n,2,proute->Note,sizeof(proute->Note));
	}
	//保存
	for (n=0; n<ServerArray.GetCount(); n++)
	{
		char str1[100];
		SERVER_Struct &Host=ServerArray[n];
		sprintf_s(str,sizeof(str),"SERVER_No%d",n+1);
		WritePrivateProfileString(str,"Server",Host.Server,ProFileName);
		WritePrivateProfileString(str,"NetName",Host.NetName,ProFileName);
		WritePrivateProfileString(str,"NetPasswd",Host.NetPasswd[0]==0 ? NULL:Host.NetPasswd,ProFileName);
		WritePrivateProfileString(str,"LocalIP",IpToStrip(Host.IpAddr,str1),ProFileName);
		WritePrivateProfileString(str,"Mask",IpToStrip(Host.IpMask,str1),ProFileName);
		CString strroute;
		for (int i=0; i<Host.RouteCnts; i++)
		{
			SERVER_ROUTE_Struct *proute=Host.pRouteList+i;
			sprintf_s(str1,sizeof(str1),";%d %d.%d.%d.%d/%d %d.%d.%d.%d %s",proute->Enable,
				proute->Net[0],proute->Net[1],proute->Net[2],proute->Net[3],proute->Mask,
				proute->Gate[0],proute->Gate[1],proute->Gate[2],proute->Gate[3],proute->Note);
			strroute+= i==0 ? str1+1 : str1;
		}
		char const *pr=NULL;
		if (!strroute.IsEmpty())
			pr=strroute;
		WritePrivateProfileString(str,"Route",pr,ProFileName);
	}
}


void Cn2n_guiDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==1)			//定时器1：添加路由和连接时闪烁，250ms一次
	{
		char str[10];
		ConnectTick++;

		GetDlgItemText(IDC_STATIC_CONNECT_STATUS,str,sizeof(str));
		if (strcmp(str,"已连接")!=0)
			((CStatic*)GetDlgItem(IDC_PIC_CONNECT))->SetIcon(ConnectTick%2==0 ? m_Icon_NoConnect:NULL);
		else if (ConnectTick>=20)	//添加路由要延时3600ms以上，不然跃点数会有问题
		{
			SetRoute(true);
			KillTimer(1);
		}
	}
	else if (nIDEvent==2)		//定时器2：网卡安装检测
	{
		if (CheckTapAdapters())
		{
			KillTimer(2);
			SetDlgItemText(IDC_BTN_START_STOP,"启动");
			SetDlgItemText(IDC_STATIC_CONNECT_STATUS,"未连接");
			m_ConnectStatus.SetColor(RGB(155,100,75));
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void Cn2n_guiDlg::OnBnClickedBtnLog()
{
	// TODO: 在此添加控件通知处理程序代码
	m_LogDlg.ShowWindow(SW_SHOW);
	m_LogDlg.PostMessage(ON_SHOWLOG_MSG,(WPARAM)"");
}


LRESULT Cn2n_guiDlg::OnMoveLogWindowMsg(WPARAM w, LPARAM l)
{
	CRect Rect;
	GetWindowRect(&Rect);
	int top=Rect.top,right=Rect.right;
	m_LogDlg.GetWindowRect(Rect);
	Rect.OffsetRect(right-Rect.left+10, top-Rect.top+5);
	m_LogDlg.MoveWindow(Rect);
	
	return LRESULT();
}


void Cn2n_guiDlg::OnBnClickedBtnHide()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}


LRESULT Cn2n_guiDlg::OnNotifyIconMsg(WPARAM w, LPARAM l)
{ 
	if(w!=IDR_MAINFRAME) 
		return  LRESULT(); 
	switch(l) 
	{ 
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭” 
	{ 
		CMenu Menu;
		Menu.LoadMenu(IDR_MENU1);
		CMenu *pSubMenu=Menu.GetSubMenu(2);
		CPoint point;
		GetCursorPos(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,point.x,point.y,this);
	} 
		break; 
	case WM_LBUTTONDBLCLK://双击左键的处理 
	{
		ShowWindow(SW_SHOW);//简单的显示主窗口完事儿 
	}
	}
	return LRESULT();
}


void Cn2n_guiDlg::OnMenuClickedShow(void)
{
	ShowWindow(SW_SHOW);//简单的显示主窗口完事儿 
}


void Cn2n_guiDlg::OnMenuClickedDelRoute(void)
{
	for (int n=m_List.GetItemCount()-1; n>=0; n--)
	{
		if (m_List.GetItemState(n,LVIS_SELECTED)&LVIS_SELECTED)
		{
			m_List.DeleteItem(n);
		}
	}
}


void Cn2n_guiDlg::OnMenuClickedEditRoute(void)
{
	if (m_List.GetSelectedCount()!=1) return;
	int n=m_List.GetSelectionMark();
	char netaddr[20];
	char gate[16];
	char note[sizeof(((SERVER_ROUTE_Struct*)0)->Note)];
	UCHAR Gate[4];
	m_List.GetItemText(n,0,netaddr,sizeof(netaddr));
	m_List.GetItemText(n,1,gate,sizeof(gate));
	m_List.GetItemText(n,2,note,sizeof(note));

	StripToIp(gate,Gate);
	CAddRouteDlg dlg(netaddr,Gate,note);
	if (dlg.DoModal()==IDOK)
	{
		m_List.SetItemText(n,0,dlg.NetAddr);
		m_List.SetItemText(n,2,dlg.m_Note);
		m_List.SetItemText(n,1,IpToStrip(dlg.GATE,gate));
	}
}

void Cn2n_guiDlg::OnMenuClickedAddRoute(void)
{
	CAddRouteDlg dlg;
	char gate[16];
	if (dlg.DoModal()==IDOK)
	{
		int n=m_List.GetItemCount();
		m_List.InsertItem(n,dlg.NetAddr);
		m_List.SetItemText(n,2,dlg.m_Note);
		m_List.SetItemText(n,1,IpToStrip(dlg.GATE,gate));
	}
}

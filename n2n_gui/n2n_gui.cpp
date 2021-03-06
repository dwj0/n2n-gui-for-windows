
// n2n_gui.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "n2n_gui.h"
#include "n2n_guiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REG_SN_KEY		"SOFTWARE\\dwj0\\n2ngui"

char ProPath[MAX_PATH];
BOOL isReg = FALSE;

static char *ReadSN(char *str)
{
	HKEY key;
	str[0] = 0;
	LSTATUS rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_SN_KEY,0, KEY_READ,&key);
	if (key==NULL) return "";

	DWORD len = 10,dwType=REG_SZ;
	rc = RegQueryValueEx(key, "sn", 0, &dwType,(LPBYTE)str,&len);

	RegCloseKey(key);
	return str;
}

static void WriteSN(char const *str)
{
	HKEY key;

	DWORD dwDisposition = REG_CREATED_NEW_KEY;	// 如果不存在不创建
	LONG lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REG_SN_KEY, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisposition);
	if (lRet != ERROR_SUCCESS)
		return ;
	
	RegSetValueEx(key, "sn", 0, REG_SZ, (BYTE*)str, strlen(str));

	RegCloseKey(key);
}

static BOOL CheckReg(char const *str)
{
	return strlen(str) == 8 && memcmp(str, "dwj0", 4) == 0;
}

BOOL RegSN(char const *sn)
{
	isReg = CheckReg(sn);
	if (isReg) WriteSN(sn);
	return isReg;
}

char *Itoa(int n, char *str)
{
	sprintf_s(str,12,"%d",n);
	return str;
}

char *MaskBitToStr(int Mask, char *str)
{
	unsigned int val = 0;
	if (Mask > 32) return "";

	for (int j = 0; j < Mask; j++)
		val |= (1 << (31 - j));
	sprintf_s(str, 16,"%d.%d.%d.%d", (UCHAR)(val >> 24), (UCHAR)((val >> 16) & 0xff), (UCHAR)((val >> 8) & 0xff), (UCHAR)(val & 0xff));
	return str;
}

char *IpToStrip(UCHAR const *ip, char *str)
{
	sprintf_s(str,16,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
	return str;
}

bool StripToIp(char const *str, UCHAR *ip)
{
	int tmp[4]={0,0,0,0};
	int n=sscanf_s(str,"%d.%d.%d.%d",tmp,tmp+1,tmp+2,tmp+3);
	for (int i=0; i<4; i++) ip[i]=(UCHAR)tmp[i];
	return n==4 && tmp[0]<=255 && tmp[1]<=255 && tmp[2]<=255 && tmp[3]<=255;
}

bool StrNetaddrToIp(char const *str, UCHAR *ip, UCHAR *mask)
{
	int tmp[5]={0,0,0,0,24};
	int n=sscanf_s(str,"%d.%d.%d.%d/%d",tmp,tmp+1,tmp+2,tmp+3,tmp+4);
	for (int i=0; i<4; i++) ip[i]=(UCHAR)tmp[i];
	*mask=(UCHAR)tmp[4];
	return n==5 && tmp[0]<=255 && tmp[1]<=255 && tmp[2]<=255 && tmp[3]<=255 && tmp[4]<=32;
}

// Cn2n_guiApp

BEGIN_MESSAGE_MAP(Cn2n_guiApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cn2n_guiApp 构造
Cn2n_guiApp::Cn2n_guiApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 Cn2n_guiApp 对象

Cn2n_guiApp theApp;


// Cn2n_guiApp 初始化

BOOL Cn2n_guiApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	//注册查询
	isReg = CheckReg(ReadSN(ProPath));
	//获取软件路径
	DWORD n=GetModuleFileName(NULL,ProPath,sizeof(ProPath));
	char *p=strrchr(ProPath,'\\');
	if (p) *p=0;

	Cn2n_guiDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


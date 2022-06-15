
// n2n_gui.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


extern char ProPath[];
char *Itoa(int n, char *str);
char *IpToStrip(UCHAR const *ip, char *str);
bool StripToIp(char const *str, UCHAR *ip);
bool StrNetaddrToIp(char const *str, UCHAR *ip, UCHAR *mask);
char *MaskBitToStr(int Mask, char *str);

BOOL RegSN(char const *sn);
extern BOOL isReg;

// Cn2n_guiApp:
// 有关此类的实现，请参阅 n2n_gui.cpp
//

class Cn2n_guiApp : public CWinApp
{
public:
	Cn2n_guiApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Cn2n_guiApp theApp;
// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers




HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetTextColor(m_FontColor);
	pDC->SetBkMode(TRANSPARENT);
	return m_Brush;
	// TODO: Return a non-NULL brush if the parent's handler should not be called
//	return NULL;
}

void CColorStatic::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_Font.m_hObject==NULL)
	{
		LOGFONT lf;
		CFont *pOldFont=GetFont();
		pOldFont->GetLogFont(&lf);
		m_Font.CreateFontIndirect(&lf);
		HDC hDC=::GetDC(m_hWnd);
		m_FontColor=GetTextColor(hDC);
		m_BkColor=GetSysColor(COLOR_3DFACE);
		m_Brush.CreateSolidBrush(m_BkColor);
		::ReleaseDC(m_hWnd,hDC);
	}

	CStatic::PreSubclassWindow();
}

void CColorStatic::SetColor(COLORREF TextColor, COLORREF BkColor)
{
	m_FontColor=TextColor;
	if (m_BkColor!=BkColor && BkColor!=0xffffffff)
	{
		if (m_Brush.m_hObject!=NULL)
			m_Brush.DeleteObject();
		m_Brush.CreateSolidBrush(m_BkColor=BkColor);
	}
	Invalidate();
}

void CColorStatic::SetTextFont(CFont &font)
{
	if (m_Font.m_hObject!=NULL)
	{
		BOOL f=m_Font.DeleteObject();
	}
	LOGFONT lf;
	font.GetLogFont(&lf);
	m_Font.CreateFontIndirect(&lf);
	if (m_hWnd!=NULL)
		SetFont(&m_Font);
}


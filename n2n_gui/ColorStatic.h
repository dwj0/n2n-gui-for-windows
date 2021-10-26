#if !defined(AFX_COLORSTATIC_H__957C4B43_24D8_4CD9_83CD_71EE5DFF5C91__INCLUDED_)
#define AFX_COLORSTATIC_H__957C4B43_24D8_4CD9_83CD_71EE5DFF5C91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
public:
	CFont	m_Font;
	CBrush	m_Brush;
	COLORREF m_FontColor;
	CColorStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF m_BkColor;
	void SetTextFont(CFont &font);
	void SetColor(COLORREF TextColor, COLORREF BkColor=0xffffffff);
	virtual ~CColorStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATIC_H__957C4B43_24D8_4CD9_83CD_71EE5DFF5C91__INCLUDED_)

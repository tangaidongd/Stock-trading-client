#if !defined(AFX_LISTBOXEX_H__EE6B4DC1_7948_4762_B96E_EF9F741A3562__INCLUDED_)
#define AFX_LISTBOXEX_H__EE6B4DC1_7948_4762_B96E_EF9F741A3562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// listboxex.h : header file
//
#include "hotkey.h"
#include "GdiPlusTS.h"
#include "dllexport.h"
/////////////////////////////////////////////////////////////////////////////

typedef struct T_ItemColor
{
	COLORREF	 m_clrText;
	COLORREF	 m_clrBack;
	E_HotKeyType m_eHKType;
	int32		 m_iFileType;	
}T_ItemColor;

class CONTROL_EXPORT CListBoxEx : public CListBox
{	
// Construction
public:
	CListBoxEx();
// Attributes
public:
	void	SetItemColor(int32 iIndex,COLORREF clrText,COLORREF clrBack,E_HotKeyType eHKType);
	void	SetItemColor(int32 iIndex,COLORREF clrText,COLORREF clrBack,int32 iFileType);

	Image*	m_pImageWsp;
	Image*	m_pImageCfm;
	Image*	m_pImageVmg;

	Image*	m_pImageMerch;
	Image*	m_pImageBlock;
	Image*	m_pImageIndex;
	Image*	m_pImageShortCut;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__EE6B4DC1_7948_4762_B96E_EF9F741A3562__INCLUDED_)

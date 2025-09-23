/****************************************************************************
 *																			*		 
 *								GuiToolKit  								*	
 *                           (MFC extension)								*			 
 * Created by Francisco Campos G. www.beyondata.com fcampos@beyondata.com	*
 *--------------------------------------------------------------------------*		   
 *																			*
 * This program is free software; so you are free to use it any of your		*
 * applications(Freeware, Shareware, Commercial), but leave this header		*
 * intact.																	*
 *																			*
 * These files are provided "as is" without warranty of any kind.			*
 *																			*
 *			       GuiToolKit is forever FREE CODE !!!!!					*
 *																			*
 *--------------------------------------------------------------------------*
 * Created by: Francisco Campos G.											*
 * Bug Fixes and improvements :(Add your name)								*
 * -Francisco Campos														*				
 *																			*	
 ****************************************************************************/

// CTab.h : implementation file
#if !defined(AFX_CBASETAB_INCLUDED_)
#define AFX_CBASETAB_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dllexport.h"
class CONTROL_EXPORT CGuiTab 
{
public:
	CGuiTab(CString mlpMsg, CString mlpShortMsg, CString mlpTipMsg, CString mlpValue)
	{
		lpMsg		= mlpMsg;
		lpShortMsg	= mlpShortMsg;
		lpTipMsg	= mlpTipMsg;
		lpValue		= mlpValue;
		uIcon		= -1;
		nMenu		= 0;
		rect.SetRectEmpty();

		m_clrTextCur = 0x0000ff;
		m_clrTextOther = 0x808080;
	}
	
	void AddMenu(UINT nMENU)
	{
		nMenu = nMENU;
	}

	void SetIcon(UINT uiIcon)
	{
		uIcon = uiIcon;
	}

public:
	UINT    nMenu;
	CString	lpMsg;
	CString lpShortMsg;
	CString lpTipMsg;
	CString lpValue;

	UINT	uIcon;
	CRect	rect;
	
	COLORREF 	m_clrLTab; //new field for theme 2003
	COLORREF	m_clrHtab;

	COLORREF	m_clrLTabOther;
	COLORREF	m_clrHTabOther;

	COLORREF	m_clrTextCur;
	COLORREF	m_clrTextOther;
};



#endif
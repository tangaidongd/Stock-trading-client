#pragma once

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.
#include "dllexport.h"

#define WM_USER_EDITWITHBUTTON_CLICKED	(WM_USER + 3000)

// CEditWithButton

class CONTROL_EXPORT CEditFlat : public CEdit
{
	DECLARE_DYNAMIC(CEditFlat)

protected:
	CBitmap m_bMpPhone;
	CSize   m_sizeEmpty;
	CSize   m_sizeFilledBitmap;
	CSize   m_sizePhone;
	CSize	m_sizeBeginBitmap;
	CRect	m_rcEditArea;
	BOOL	m_bButtonExistsAlways;
	UINT	m_iButtonClickedMessageId;

	//
	BOOL    m_bFocused;
	//
	CFont	m_TipFont;
	CFont   m_EditFont;
	//
	CString m_strTip;
	//
	COLORREF m_clrTip;
	COLORREF m_activeColor;
	COLORREF m_normalColor;
	//
	TCHAR    m_chPassword;
private:
	//
	int m_iLeftOffset;
	int m_iTopOffset ;
	int m_iRightOffset;
	int m_iBottomOffset;
	//
	CBrush	  m_brBkgnd;
	COLORREF  m_clrBkgnd;
public:
	
	BOOL SetBitmaps(UINT iMpPone);
	void SetButtonClickedMessageId(UINT iButtonClickedMessageId);
	void SetButtonExistsAlways(BOOL bButtonExistsAlways);

	CEditFlat();
	virtual ~CEditFlat();
	virtual void PreSubclassWindow( );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	//
	void SetTipTextColor(COLORREF clr);
	void SetActiveFrameColor(COLORREF clr);
	void SetNormalFrameColor(COLORREF clr);
	//
	void SetTipText(CString strTip);
	//
	//void SetSize(CSize sz);
	//
	void SetTipFont(LOGFONT *pFont);
	void SetEditFont(LOGFONT *pFont);
	//
	void SetPasswordCharEx(TCHAR ch);
	//
	void SetEditArea(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset, int cx, int cy);
	void SetBitmapBeginPos(CSize size);
private:
	void ChangeEditSize();
	void ResizeWindow();
protected:
	DECLARE_MESSAGE_MAP()

public:
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnPaint();
	//
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//
	afx_msg void OnSize(UINT nType, int cx, int cy);
};



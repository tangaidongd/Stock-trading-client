#pragma once

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.
#include "dllexport.h"
#include "GdiPlusTS.h"

#define WM_USER_EDIT_SEARCH_CLICKED	(WM_USER + 600)

// CEditWithButton


class CONTROL_EXPORT CSpecialCEdit: public CEdit
{
	DECLARE_DYNAMIC(CSpecialCEdit)

protected:
	UINT	m_iButtonClickedMessageId;
	Image	*m_pImgBK;
	CSize   m_sizeEmpty;
	CSize   m_sizePhone;
	CRect	m_rcEditArea;
	//
	BOOL    m_bFocused;
	//
	CFont	m_TipFont;
	CFont   m_EditFont;
	//
	CString m_strTip;
	//
	COLORREF m_clrTip;
	//
	TCHAR    m_chPassword;
private:
	//
	int m_iLeftOffset;
	int m_iTopOffset ;
	int m_iRightOffset;
	int m_iBottomOffset;
	//
	COLORREF  m_clrBkgnd;

	bool m_bKeyBoardFistRefresh;
	//
	int m_iMaxText;	// 最大输入的字数
public:
	
	BOOL SetImage(Image *pImgLogo);

	CSpecialCEdit();
	virtual ~CSpecialCEdit();
	virtual void PreSubclassWindow( );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	//
	//
	void SetTipText(CString strTip);
	//
	void	SetTipColor(COLORREF clrTip);
	//
	//void SetSize(CSize sz);
	//
	void SetTipFont(LOGFONT *pFont);
	void SetEditFont(LOGFONT *pFont);
	//
	void SetPasswordCharEx(TCHAR ch);
	//
	void SetBkColorref(COLORREF cl);
	//
	void SetEditArea(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset, int cx, int cy);

	void SetButtonClickedMessageId(UINT iButtonClickedMessageId);

	void OnlyOnceRefresh();


	void SetEditMaxLength(int iMax);
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
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



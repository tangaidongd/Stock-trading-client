// EditWithButton.cpp : implementation file
//

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.

#include "StdAfx.h"
#include "EditFlat.h"

// CEditWithButton

IMPLEMENT_DYNAMIC(CEditFlat, CEdit)

CEditFlat::CEditFlat()
{
	m_iButtonClickedMessageId = WM_USER_EDITWITHBUTTON_CLICKED;
	m_bButtonExistsAlways = FALSE;

	m_rcEditArea = CRect(10,5,200,30);

	m_bFocused = FALSE;
	//
	m_clrTip = RGB(125,125,125);
	m_activeColor = RGB(0, 0, 0);
	m_normalColor = RGB(0, 0, 0);
	//
	//m_sizeEmpty = CSize(299, 33);
	m_sizeEmpty = CSize(200, 30);
	//
	m_sizeBeginBitmap = CSize(1, 1);
	m_strTip = L"";
	//
	m_chPassword = _T('\0');
	//
	//
	m_sizePhone = CSize(0, 0);
	//
	m_iLeftOffset = 1;
	m_iTopOffset = 1;
	m_iRightOffset = 1;
	m_iBottomOffset = 1;
	//
	m_TipFont.CreateFont(15, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
		L"宋体");
	//
	m_EditFont.CreateFont(15, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_SEMIBOLD,//FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac 

	//
   //m_clrBkgnd = RGB(235, 235, 235);
   m_clrBkgnd = RGB(255, 255, 255);
   m_brBkgnd.CreateSolidBrush( m_clrBkgnd );
}

CEditFlat::~CEditFlat()
{
	m_brBkgnd.DeleteObject();
	m_EditFont.DeleteObject();
	m_TipFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CEditFlat, CEdit)

	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CEditFlat::PreSubclassWindow( )
{	
	// We must have a multiline edit
	// to be able to set the edit rect
	ASSERT( GetStyle() & ES_MULTILINE );
	//
	SetFont(&m_EditFont);

	if ( _T('\0') != m_chPassword )
	{
		SetPasswordChar(m_chPassword);
	}
	ResizeWindow();
}

BOOL CEditFlat::PreTranslateMessage( MSG* pMsg )
{
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CEditFlat::SetBitmaps(UINT iMpPone)
{
	BITMAP bmpInfo;
	//
	if(iMpPone == 0)
	{
		m_sizePhone = CSize(0, 0);
	}
	else
	{
		m_bMpPhone.DeleteObject();
		m_bMpPhone.LoadBitmap(iMpPone);	
		m_bMpPhone.GetBitmap(&bmpInfo);
		m_sizePhone = CSize(bmpInfo.bmWidth, bmpInfo.bmHeight);
	}

	ChangeEditSize();

	return TRUE;
}

void CEditFlat::ResizeWindow()
{
	if (!::IsWindow(m_hWnd)) return;

	SetRect(&m_rcEditArea);

	Invalidate();
}
//设置整个控件大小和编辑区大小 //cx ,cy就是整个宽和高
void CEditFlat::SetEditArea(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset, int iCX, int iCY)
{	
	//m_sizeEmpty.cx = iCX;
	//m_sizeEmpty.cy = iCY;
	m_sizeEmpty.cx = iCX;
	if ( 0 == iCY)
	{
		m_sizeEmpty.cy = m_sizePhone.cy+2;
	}
	
	
	m_iLeftOffset = iLeftOffset;
	m_iTopOffset = iTopOffset;
	m_iRightOffset = iRightOffset;
	m_iBottomOffset = iBottomOffset;
	ChangeEditSize();
}

void CEditFlat::SetBitmapBeginPos(CSize size)
{
	m_sizeBeginBitmap = size;
}

void CEditFlat::SetButtonClickedMessageId(UINT iButtonClickedMessageId)
{
	m_iButtonClickedMessageId = iButtonClickedMessageId;

}

void CEditFlat::SetButtonExistsAlways(BOOL bButtonExistsAlways)
{
	m_bButtonExistsAlways = bButtonExistsAlways;
}

BOOL CEditFlat::OnEraseBkgnd(CDC* pDC)
{
	Default();
	/*
	CRect rect;
	GetClientRect(rect);
    pDC->FillSolidRect(rect, RGB(235, 235, 235));
	*/
	//
	/*
	CDC dcMemory;
	CBitmap* pOldBitmap = NULL;
	int iTextLength = GetWindowTextLength();
	
	dcMemory.CreateCompatibleDC(pDC);
	pOldBitmap = dcMemory.SelectObject(&m_bMpPhone);
	//
	CBrush brBorder;
	CRect rectWnd(0, 0, m_sizeEmpty.cx, m_sizeEmpty.cy);
	
	if ( FALSE == m_bFocused )
		brBorder.CreateSolidBrush(m_normalColor);
	else
		brBorder.CreateSolidBrush(m_activeColor);
	
	pDC->FrameRect(&rectWnd,&brBorder);
	
	int iHeight = (m_sizeEmpty.cy - m_sizePhone.cy)/2;
	
	pDC->BitBlt(m_sizeEmpty.cx - m_sizePhone.cx - 10,iHeight, m_sizePhone.cx , m_sizePhone.cy , &dcMemory,0, 0,SRCCOPY);
    dcMemory.SelectObject(pOldBitmap);
	
	//
	CRect rect(10, 0, m_sizeEmpty.cx, m_sizeEmpty.cy);
	
	if ( FALSE == m_bFocused && GetWindowTextLength() == 0 )
	{
		pDC->SetBkMode(TRANSPARENT);
		COLORREF oldTextCrl = pDC->SetTextColor(m_clrTip);
		//ont *pOldFont = pDC->SelectObject(&m_TipFont);
		pDC->DrawText(L"输入密码", rect, DT_LEFT|DT_SINGLELINE | DT_VCENTER );
		pDC->SetTextColor(oldTextCrl);
		//C->SelectObject(pOldFont);
	}
	*/
	return TRUE;
}

void CEditFlat::OnPaint()
{
	Default();
	CDC *pDC = GetDC();
	
	//
	CRect rtWnd;
	GetClientRect(&rtWnd);
	
	CRect rtIcon(0, 0, 0, 0);
	if (NULL != m_bMpPhone.GetSafeHandle())
	{
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);	
		CBitmap* pOldBitmap = NULL;
		BITMAP bmp;
		GetObject(m_bMpPhone, sizeof(BITMAP), &bmp);
		rtIcon.right = rtIcon.left + bmp.bmWidth;	
		rtIcon.top = (rtWnd.Height() - bmp.bmHeight) /2;
		rtIcon.bottom = rtIcon.top + bmp.bmHeight;
		pOldBitmap = dcMemory.SelectObject(&m_bMpPhone);
		pDC->BitBlt(rtIcon.left, rtIcon.top , rtIcon.Width() , rtIcon.Height() , &dcMemory,0, 0,SRCCOPY);
		dcMemory.SelectObject(pOldBitmap);
		dcMemory.DeleteDC();
	}
	
	CBrush brBorder;	
	CRect rtEdit(rtIcon.right , 0, rtWnd.right, rtWnd.bottom);
	if ( FALSE == m_bFocused && GetWindowTextLength() == 0 )
	{
	//	pDC->SetBkMode(TRANSPARENT);
		COLORREF oldTextCrl = pDC->SetTextColor(m_clrTip);
		CFont *pOldFont = pDC->SelectObject(&m_TipFont);
		pDC->DrawText(m_strTip, rtEdit, DT_LEFT|DT_SINGLELINE | DT_VCENTER );
		pDC->SetTextColor(oldTextCrl);
		pDC->SelectObject(pOldFont);
	}

	int iBorderWidth = 2;
	if ( FALSE == m_bFocused )
	{	
		brBorder.CreateSolidBrush(m_normalColor);		
		pDC->FrameRect(&rtWnd,&brBorder);		
	}	
	else
	{
		CRect rt;
		GetClientRect(&rt);
		CRgn rgn;
		rgn.CreateRectRgn(iBorderWidth, iBorderWidth, rt.right-iBorderWidth, rt.bottom-iBorderWidth);
		pDC->SelectClipRgn(&rgn,RGN_DIFF);			
		pDC->FillSolidRect(&rt, m_activeColor);		
		pDC->SelectClipRgn(NULL, RGN_AND);
		rgn.DeleteObject();
	}


	brBorder.DeleteObject();	
	ReleaseDC(pDC);
}

void CEditFlat::OnSetFocus(CWnd* pOldWnd)
{
	m_bFocused = TRUE;
	ResizeWindow();
	CEdit::OnSetFocus(pOldWnd);
}

void CEditFlat::OnKillFocus(CWnd* pNewWnd)
{
	m_bFocused = FALSE;
	ResizeWindow();
	CEdit::OnKillFocus(pNewWnd);
}
//提示颜色
void CEditFlat::SetTipTextColor(COLORREF clr)
{
	m_clrTip = clr;
}
//激活边框颜色
void CEditFlat::SetActiveFrameColor(COLORREF clr)
{
	m_activeColor = clr;
}
//正常边栏颜色
void CEditFlat::SetNormalFrameColor(COLORREF clr)
{
	m_normalColor = clr;
}
//提示内容
void CEditFlat::SetTipText(CString strTip)
{
	m_strTip = strTip;
}
//设置整个控件大小
/*
void CEditFlat::SetSize(CSize sz)
{
	m_sizeEmpty = sz;
	//CRect(10, 10, m_sizeEmpty.cx - m_sizePhone.cx - 15 , m_sizeEmpty.cy);
	//SetEditArea(10, );//编辑区大小
	ResizeWindow();
}
*/
//提示文本字体
void CEditFlat::SetTipFont(LOGFONT *pFont)
{

	m_TipFont.DeleteObject();
	m_TipFont.CreateFontIndirect(pFont);

}

//编辑框里字体大小
void CEditFlat::SetEditFont(LOGFONT *pFont)
{
	
	m_EditFont.DeleteObject();
	m_EditFont.CreateFontIndirect(pFont);
}

//设置密码提示符
void CEditFlat::SetPasswordCharEx(TCHAR ch)
{
	m_chPassword = ch;
}

void CEditFlat::ChangeEditSize()
{
	m_rcEditArea = CRect(m_sizePhone.cx + m_iLeftOffset, m_iTopOffset, m_sizeEmpty.cx - m_iRightOffset, m_sizeEmpty.cy - m_iBottomOffset);
	ResizeWindow(); 
}

HBRUSH CEditFlat::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the
	//   parent's handler should not be called
	pDC->SetBkColor( m_clrBkgnd );   // text bkgnd
	return m_brBkgnd;            // ctl bkgnd
}

void CEditFlat::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	GetWindowRect(rc);

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 11, 11);
	SetWindowRgn((HRGN)rgn.Detach(), TRUE);
	rgn.DeleteObject();
};
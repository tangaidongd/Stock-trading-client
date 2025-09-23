// EditWithButton.cpp : implementation file
//

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.

#include "StdAfx.h"
#include "SpecialCEdit.h"
#include "afxmsg_.h"



// CEditWithButton

IMPLEMENT_DYNAMIC(CSpecialCEdit, CEdit)

CSpecialCEdit::CSpecialCEdit()
{
	m_iButtonClickedMessageId = WM_USER_EDIT_SEARCH_CLICKED;
	m_rcEditArea = CRect(10,5,240,33);


	m_bKeyBoardFistRefresh = false;

	m_bFocused = FALSE;
	//
	m_clrTip = RGB(125,125,125);
	//
	m_sizeEmpty = CSize(299, 33);
	//
	m_strTip = L"";
	//
	m_chPassword = _T('\0');
	m_iMaxText = 0;
	//
	//
	m_sizePhone = CSize(0, 0);
	//
	m_iLeftOffset = 0;
	m_iTopOffset = 0;
	m_iRightOffset = 0;
	m_iBottomOffset = 0;
	//
	m_TipFont.CreateFont(12, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
		L"宋体");
	//
	m_EditFont.CreateFont(12, // nHeight 
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
		_T("宋体")); // lpszFac 

	//
   //m_clrBkgnd = RGB(235, 235, 235);
   m_clrBkgnd = RGB(40, 43, 49);
   m_pImgBK = NULL;
}

CSpecialCEdit::~CSpecialCEdit()
{
	m_EditFont.DeleteObject();
	m_TipFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CSpecialCEdit, CEdit)

	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CSpecialCEdit::PreSubclassWindow( )
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

	if (0 < m_iMaxText)
	{
		SetLimitText(m_iMaxText);
	}
	ResizeWindow();
}


BOOL CSpecialCEdit::PreTranslateMessage( MSG* pMsg )
{
	return CEdit::PreTranslateMessage(pMsg);
}


BOOL CSpecialCEdit::SetImage(Image *pImgLogo)
{
	if (!pImgLogo)
	{
		return FALSE;
	}

	m_pImgBK = pImgLogo;
	m_sizePhone.cx = m_pImgBK->GetWidth();
	m_sizePhone.cy = m_pImgBK->GetHeight();
	ChangeEditSize();
	return TRUE;
}


void CSpecialCEdit::ResizeWindow()
{
	if (!::IsWindow(m_hWnd)) return;

	CRect rect;
	rect = m_rcEditArea;
	//SetWindowPos(&wndTop,0,0,m_sizeEmpty.cx,m_sizeEmpty.cy,SWP_NOMOVE|SWP_NOZORDER);
	SetRect(&rect);

	Invalidate();
}


void CSpecialCEdit::OnlyOnceRefresh()
{
	if (m_bKeyBoardFistRefresh)
	{
		return;
	}
	
	m_bKeyBoardFistRefresh = true;
	Invalidate();
}


//设置整个控件大小和编辑区大小 //cx ,cy就是整个宽和高
void CSpecialCEdit::SetEditArea(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset, int iCX, int iCY)
{	
	m_sizeEmpty.cx = iCX;
	m_sizeEmpty.cy = iCY;

	/*if ( 0 == iCY)
	{
		m_sizeEmpty.cy = m_sizePhone.cy+2;
	}*/
	
	m_iLeftOffset = iLeftOffset;
	m_iTopOffset = iTopOffset;
	m_iRightOffset = iRightOffset;
	m_iBottomOffset = iBottomOffset;
	ChangeEditSize();
}


BOOL CSpecialCEdit::OnEraseBkgnd(CDC* pDC)
{
	Default();
	//绘制背景
	pDC->SetBkMode(TRANSPARENT);
	Graphics graphics(pDC->GetSafeHdc());

	CRect rcClient;
	GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	CRect animRect;
	GetClientRect(&animRect);


	//DrawRoundRectange(graphics, Color(255,0,0,0), 0, 0, animRect.Width() - 1,animRect.Height() - 1);
	//FillRoundRectangle(graphics, Color(255, 255, 255, 255),0, 0, animRect.Width() - 1,animRect.Height() - 1);

	//CRgn rgn;
	//rgn.CreateRoundRectRgn(0,0,animRect.Width()+1,animRect.Height()+1, 20,animRect.Height()-1);


	//CBrush brush;
	//brush.CreateSolidBrush(m_clrBkgnd);
	//pDC->FillRgn(&rgn, &brush);
	////SetWindowRgn(rgn,TRUE); 
	///*SetWindowRgn((HRGN)rgn.Detach(), TRUE);*/
	//graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

	//// 画线先去掉
	//CBrush br11;
	//br11.CreateSolidBrush( RGB(6, 5, 11) );
	//pDC->FrameRgn( &rgn, &br11, 1, 1 );


	RectF fRect;
	if (NULL != m_pImgBK)
	{
		fRect.X = animRect.left;
		fRect.Y = animRect.top;
		fRect.Width = (REAL)m_pImgBK->GetWidth();
		fRect.Height = (REAL)m_pImgBK->GetHeight();

		graphics.DrawImage(m_pImgBK, fRect, 0, 0, m_pImgBK->GetWidth(), m_pImgBK->GetHeight(), UnitPixel);
	}

	return TRUE;
}

void CSpecialCEdit::OnPaint()
{
	CPaintDC dc(this);
	Default();
	CDC *pDC = GetDC();
	CRect rcRect( m_iLeftOffset, m_iTopOffset, m_sizeEmpty.cx - m_iRightOffset, m_sizeEmpty.cy - m_iBottomOffset);;
	if ( FALSE == m_bFocused && GetWindowTextLength() == 0 )
	{
		pDC->SetBkMode(TRANSPARENT);
		COLORREF oldTextCrl = pDC->SetTextColor(m_clrTip);
		CFont *pOldFont = pDC->SelectObject(&m_TipFont);
		pDC->DrawText(m_strTip, rcRect, DT_LEFT|DT_SINGLELINE | DT_VCENTER );
		pDC->SetTextColor(oldTextCrl);
		pDC->SelectObject(pOldFont);
	}
	ReleaseDC(pDC);

	

}

void CSpecialCEdit::OnSetFocus(CWnd* pOldWnd)
{
	//SendMessageA(GetParent()->GetSafeHwnd(), m_iButtonClickedMessageId, 0, 0);
	m_bFocused = TRUE;
	ResizeWindow();
	CEdit::OnSetFocus(pOldWnd);
}

void CSpecialCEdit::OnKillFocus(CWnd* pNewWnd)
{
	m_bFocused = FALSE;
	ResizeWindow();
	CEdit::OnKillFocus(pNewWnd);
}

void CSpecialCEdit::SetEditMaxLength(int iMax)
{
	m_iMaxText = iMax;
}


//提示内容
void CSpecialCEdit::SetTipText(CString strTip)
{
	m_strTip = strTip;
}
void	CSpecialCEdit::SetTipColor(COLORREF clrTip)
{
	m_clrTip = clrTip;
}

//提示文本字体
void CSpecialCEdit::SetTipFont(LOGFONT *pFont)
{
	m_TipFont.DeleteObject();
	m_TipFont.CreateFontIndirect(pFont);
}

//编辑框里字体大小
void CSpecialCEdit::SetEditFont(LOGFONT *pFont)
{
	m_EditFont.DeleteObject();
	m_EditFont.CreateFontIndirect(pFont);
}

//设置密码提示符
void CSpecialCEdit::SetPasswordCharEx(TCHAR ch)
{
	m_chPassword = ch;
}

//设置背景颜色
void CSpecialCEdit::SetBkColorref(COLORREF cl)
{
	m_clrBkgnd = cl;
}


void CSpecialCEdit::SetButtonClickedMessageId(UINT iButtonClickedMessageId)
{
	m_iButtonClickedMessageId = iButtonClickedMessageId;
}



void CSpecialCEdit::ChangeEditSize()
{

	m_rcEditArea = CRect( m_iLeftOffset, m_iTopOffset, m_sizeEmpty.cx - m_iRightOffset, m_sizeEmpty.cy - m_iBottomOffset);
	
	ResizeWindow(); 
}

HBRUSH CSpecialCEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(RGB(255,255,255));
	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_clrBkgnd);
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

void CSpecialCEdit::OnSize(UINT nType, int cx, int cy)
{


	CRect rc;
	GetWindowRect(rc);

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 11, 11);
	//SetWindowRgn((HRGN)rgn.Detach(), TRUE);
	rgn.DeleteObject();
};

BOOL CSpecialCEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	// 去掉边框
	cs.style &= ~WS_BORDER;

	return CEdit::PreCreateWindow(cs);
}


void CSpecialCEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CEdit::OnLButtonDown(nFlags, point);
	PostMessageA(GetParent()->GetSafeHwnd(), m_iButtonClickedMessageId, 0, 0);
}

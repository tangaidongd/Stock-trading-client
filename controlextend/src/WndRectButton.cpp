#include "StdAfx.h"
#include <afxpriv.h>
#include "WndRectButton.h"

#include "ColorStep.h"
//#include "ShareFun.h"
#include "GdiPlusTS.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif


//////////////////////////////////////////////////////////////////////////
// CWndRectButton

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

CWndRectButton::CWndRectButton( CRectButtonWndInterface &wndTB )
: m_wndTB(wndTB)
{
	Initial(NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
	m_wndTB.OnNewBtn(*this);
}

CWndRectButton::CWndRectButton( CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip /*= NULL*/ )
:m_wndTB(wndTB)
{
	Initial(pWndOwner, nID, pszText, NULL, NULL, NULL, NULL, pszTip);
	m_wndTB.OnNewBtn(*this);
}

CWndRectButton::CWndRectButton( CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip /*= NULL*/ )
:m_wndTB(wndTB)
{
	Initial(pWndOwner, nID, NULL, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
	m_wndTB.OnNewBtn(*this);
}

CWndRectButton::CWndRectButton( CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip /*= NULL*/ )
:m_wndTB(wndTB)
{
	Initial(pWndOwner, nID, pszText, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
	m_wndTB.OnNewBtn(*this);
}

CWndRectButton::CWndRectButton( CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, E_MarlettChar chMarllet, LPCTSTR pszTip /*= NULL*/ )
:m_wndTB(wndTB)
{
	TCHAR chs[2];
	chs[0] = (TCHAR)chMarllet;
	chs[1] = '\0';
	Initial(pWndOwner, nID, chs, NULL, NULL, NULL, NULL, pszTip);
	SetUseMarlettChar(true);
}

CWndRectButton::~CWndRectButton()
{
	if ( m_pRadioGroup )
	{
		m_pRadioGroup->RemoveRadioBtn(this);
	}

	if( m_pImageDisable && m_pImageDisable != m_pImageNomal && m_pImageDisable != m_pImageSelected && m_pImageDisable != m_pImageFocus)
	{
		delete m_pImageDisable;
		m_pImageDisable = NULL;
	}
	if( m_pImageNomal && m_pImageNomal == m_pImageSelected && m_pImageNomal == m_pImageFocus && m_pImageNomal == m_pImageDisable)
	{
		delete m_pImageNomal;
		m_pImageNomal = NULL;
		m_pImageSelected = NULL;
		m_pImageFocus = NULL;
		m_pImageDisable = NULL;
	}

	if( m_pImageNomal && m_pImageNomal == m_pImageSelected && m_pImageNomal == m_pImageFocus)
	{
		delete m_pImageNomal;
		m_pImageNomal = NULL;
		m_pImageSelected = NULL;
		m_pImageFocus = NULL;
	}

	/*if( m_pImageNomal ) 
		delete m_pImageNomal;
	m_pImageNomal = NULL;

	if( m_pImageSelected )
		delete m_pImageSelected;
	m_pImageSelected = NULL;
	
	if( m_pImageFocus )
		delete m_pImageFocus;
	m_pImageFocus = NULL;

	if( m_pImageDisable )
		delete m_pImageDisable;
	m_pImageDisable = NULL;*/
}
//lint --e{648} suppress " Overflow in computing constant for operation: 'unsigned sub.'"
bool32 CWndRectButton::DoClicked( CPoint ptClick )
{
	// 只有在该矩形范围内的pt才能传进来
	if ( !m_RectButton.PtInRect(ptClick) )
	{
		ASSERT( 0 );
		return false;
	}

	bool32 bNotify = false;
	if ( BTNB_CHECKBOX == m_eBtnBehavior )
	{
		bool32 bCheck = !m_bCheck;
		bNotify = NotifyOwner(NM_CLICK, NULL);		// check & radio应该是先check，然后在通知，应该完善很多通知条件
		if ( bNotify )
		{
			SetCheck(bCheck);
		}
	}
	else if ( BTNB_RADIO == m_eBtnBehavior )
	{
		ASSERT( !m_bCheck );		// 只能是uncheck - > check
		if ( !m_bCheck )
		{
			bNotify = NotifyOwner();
			if ( bNotify )
			{
				SetCheck(true);
			}
		}
	}
	else		// 普通按钮
	{
		ASSERT( IsEnable() );
		bNotify =  NotifyOwner();
	}

	return bNotify;
}


void CWndRectButton::Draw( CDC &dc )
{
	//
	switch (m_eDrawStyle)
	{
	case Draw_Style2:
		DrawStyle2(dc);
		break;
	default:
		DrawStyle1(dc);
	}
}


void CWndRectButton::DrawStyle1( CDC &dc )
{
	if ( m_RectButton.IsRectEmpty() )
	{
		return;
	}

	if ( m_RectButton.Width() <0 || m_RectButton.Width() > 2000
		|| m_RectButton.Height() <0 || m_RectButton.Height() > 200)
	{
		ASSERT( 0 );
		return;
	}
	int iSave = dc.SaveDC();

	CRect rcDraw(m_RectButton);
	ASSERT( rcDraw.Width() < 200 && rcDraw.Width() >= 0 );
	ASSERT( rcDraw.Height() < 200 && rcDraw.Height() >= 0 );
	dc.SelectObject(&m_font);

	bool32 bColorOK = true;
	COLORREF clrBK1;
	COLORREF clrBK2;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;

	COLORREF clrText;
	COLORREF clrFrame;
	bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
	ASSERT( bColorOK );

	if ( IsSeparator() )	// 仅仅是一个分割条
	{
		rcDraw.InflateRect(0, -rcDraw.Height()/8);
		rcDraw.NormalizeRect();
		CRect rcLeft(rcDraw), rcRight(rcDraw);
		rcLeft.right = rcLeft.left + rcDraw.Width()/2;
		rcRight.left = rcLeft.right;
		
		CColorStep step;
		step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
		clrBK1 = step.GetColor(2);
		step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
		clrBK2 = step.GetColor(1);
		dc.FillSolidRect(rcLeft, clrBK2);
		dc.FillSolidRect(rcRight, clrBK1);
		return;
	}

	bool32	bDrawFocus = true;
	bool32	bDrawSelected = false;

	DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;

	Image *pImage = m_pImageNomal;
	if ( Disable == m_eBtnState )
	{
		if ( m_pImageDisable )
		{
			pImage = m_pImageDisable;
		}
	}
	else if ( Selected == m_eBtnState )
	{
		bDrawSelected = true;
		if ( m_pImageSelected )
		{
			pImage = m_pImageSelected;
		}
	}
	else if ( Focus == m_eBtnState )
	{
		if ( m_pImageFocus )
		{
			pImage = m_pImageFocus;
		}
	}

	if ( Disable != m_eBtnState )
	{
		if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
		{
			// check状态下，借用Select状态图片。颜色等
			pImage = m_pImageSelected;

			bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
			ASSERT( bColorOK );

			bDrawFocus = false;
			bDrawSelected = true;
		}
	}

	dc.SetTextColor(clrText);

	if ( NULL == pImage )
	{
		CRgn rgn;
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		CBrush brush;
		brush.CreateSolidBrush(clrFrame);
		dc.FrameRgn(&rgn, &brush, 1, 1);
		
		rcDraw.InflateRect(-1, -1);
		rgn.DeleteObject();
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
	}

	if ( bDrawSelected )
	{
		clrBK1 = clrBK2;
	}

	if ( pImage )
	{
		Graphics	gTest(dc.GetSafeHdc());
		Rect    rectDraw;
		int nIw = pImage->GetWidth();
		int nIh = pImage->GetHeight();

		int x = (rcDraw.Width()-nIw)/2;
		int y = (rcDraw.Height()-nIh)/2;
		rectDraw.X = rcDraw.left+x;
		rectDraw.Y = rcDraw.top+y;
		rectDraw.Width = nIw;//rcDraw.Width();
		rectDraw.Height= nIh;//rcDraw.Height();
		

		gTest.DrawImage(pImage, rectDraw, 0, 0, rectDraw.Width, rectDraw.Height, UnitPixel);

 		 if ( Focus == m_eBtnState && !m_bCheck )
		 { 	
			 const COLORREF clrStart = RGB(125,125,125);
			 const COLORREF clrEnd   = RGB(255,255,255);
			 CRect rt3D = rcDraw;
			
			 rt3D.InflateRect(-1, -1);

			 dc.Draw3dRect(&rt3D, clrEnd, clrStart);
 		 }
		 else if( bDrawSelected )
		 {
			 const COLORREF clrStart = RGB(125,125,125);
			 const COLORREF clrEnd   = RGB(255,255,255);
			 CRect rt3D = rcDraw;
			 
			 rt3D.InflateRect(-1, -1);
 			 dc.Draw3dRect(&rt3D, clrStart, clrEnd);
		 }
	}
	else
	{
		int32 iThdPartHeight = rcDraw.Height()/3; 
		int32 iMid = MAX(iThdPartHeight, 1);
		CColorStep step(clrBK2, clrBK1, iMid);
		step.SetColorSmooth(true);
		for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
		{
			dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
		}
	}

	rcDraw.InflateRect(-1,-1);

	// 画focus标志 - 没有图片的情况下
	if ( !pImage && Focus == m_eBtnState && bDrawFocus )
	{
		COLORREF clrFocus = clrFrame;

		// 画个小框框 o
 		CBrush brhFocus;
 		brhFocus.CreateSolidBrush(clrFocus);
		//dc.FrameRgn(&rgn, &brhFocus, 1, 1);
	}

	if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior && NULL == pImage )
	{
		dc.SetBkMode(TRANSPARENT);
		// 为了美观和focus留出空白，把rcDraw左右缩小那么一点
		rcDraw.InflateRect(-2, 0);
		if ( m_bUseMarllet && NULL == pImage )
		{
			rcDraw.InflateRect(-2, -2);
			CFont MyFont;
			LOGFONT logfont;
			logfont.lfHeight = -rcDraw.Height();
			logfont.lfWidth = 0;
			logfont.lfEscapement = 0;
			logfont.lfOrientation = 0;
			logfont.lfWeight = FW_NORMAL;
			logfont.lfItalic = FALSE;
			logfont.lfUnderline = FALSE;
			logfont.lfStrikeOut = FALSE;
			logfont.lfCharSet = DEFAULT_CHARSET;
			logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			logfont.lfQuality = DEFAULT_QUALITY;
			logfont.lfPitchAndFamily = DEFAULT_PITCH;
			
			_tcscpy(logfont.lfFaceName,_T("Marlett"));
			
			MyFont.CreateFontIndirect (&logfont);
			
			CFont* pOldFont = dc.SelectObject (&MyFont); 
			int OldMode = dc.SetBkMode(TRANSPARENT);  
			
			dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			dc.SetBkMode(OldMode);
			dc.SelectObject(pOldFont);
		}
		else
		{
			dc.DrawText(m_StrText, rcDraw, dwTextDT);
		}
	}

	dc.RestoreDC(iSave);
}

void CWndRectButton::DrawStyle2( CDC &dc )
{
	if ( m_RectButton.IsRectEmpty() )
	{
		return;
	}

	if ( m_RectButton.Width() <0 || m_RectButton.Width() > 2000
		|| m_RectButton.Height() <0 || m_RectButton.Height() > 200)
	{
		ASSERT( 0 );
		return;
	}
	int iSave = dc.SaveDC();

	CRect rcDraw(m_RectButton);
	ASSERT( rcDraw.Width() < 200 && rcDraw.Width() >= 0 );
	ASSERT( rcDraw.Height() < 200 && rcDraw.Height() >= 0 );
	dc.SelectObject(&m_font);

	bool32 bColorOK = true;
	COLORREF clrBK1;
	COLORREF clrBK2;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;

	COLORREF clrText;
	COLORREF clrFrame;
	bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
	ASSERT( bColorOK );

	if ( IsSeparator() )	// 仅仅是一个分割条
	{
		rcDraw.InflateRect(0, -rcDraw.Height()/6);
		rcDraw.NormalizeRect();
		CRect rcLeft(rcDraw), rcRight(rcDraw);
		rcLeft.right = rcLeft.left + rcDraw.Width()/2;
		rcRight.left = rcLeft.right;

		CColorStep step;
		step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
		clrBK1 = step.GetColor(1);
		step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
		clrBK2 = step.GetColor(1);
		dc.FillSolidRect(rcLeft, clrBK2);
		dc.FillSolidRect(rcRight, clrBK1);
		return;
	}

	bool32	bDrawFocus = false;
	bool32	bDrawSelected = false;
	bool32  bGrayImage = false;
	bool32  bSelectedLikePush = false;

	DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;

	Image *pImage = m_pImageNomal;
	if ( Disable == m_eBtnState )
	{
		if ( m_pImageDisable )
		{
			pImage = m_pImageDisable;
		}
		else
		{
			bGrayImage = true;
		}
	}
	else if ( Selected == m_eBtnState )
	{
		bDrawSelected = true;
		if ( m_pImageSelected )
		{
			pImage = m_pImageSelected;
		}
		else
		{
			bSelectedLikePush = true;
		}
	}
	else if ( Focus == m_eBtnState )
	{
		if ( m_pImageFocus )
		{
			pImage = m_pImageFocus;
		}
		bDrawFocus = true;
	}

	if ( Disable != m_eBtnState )
	{
		if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
		{
			// check状态下，借用Select状态图片。颜色等
			if ( m_pImageSelected != NULL )
			{
				pImage = m_pImageSelected;
			}
			else
			{
				bSelectedLikePush = true;
			}

			bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
			bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
			ASSERT( bColorOK );

			bDrawFocus = false;
			bDrawSelected = true;
		}
	}

	dc.SetTextColor(clrText);

	// 该画法矩形边框，背景采用1/2中间绘制，正常情况下不画边框，focus下绘制凸起边框，select绘制凹陷边框
	bool32 bInflateRectAfterFillBk = false;
	if ( bDrawFocus )
	{
		dc.DrawEdge(rcDraw, EDGE_RAISED, BF_RECT);
		rcDraw.InflateRect(-1, -1);
	}
	else if ( bDrawSelected )
	{
		dc.DrawEdge(rcDraw, EDGE_SUNKEN, BF_RECT);
		rcDraw.InflateRect(-1, -1);
	}
	else
	{
		bInflateRectAfterFillBk = true;
	}
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rcDraw);
	dc.SelectClipRgn(&rgn, RGN_AND);

	int32 iHalfHeight = rcDraw.Height()/2;
	int32 iMid = MAX(iHalfHeight, 1);
	CColorStep step(clrBK2, clrBK1, iMid);
	step.SetColorSmooth(true);
	//dc.FillSolidRect(m_RectButton, clrBK); 
	for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
	{
		dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
	}
	if ( bInflateRectAfterFillBk )
	{
		rcDraw.InflateRect(-1, -1);
	}

	if ( pImage != NULL )
	{
		Graphics	g(dc.m_hDC);
		//g.DrawImage(pImage, rcDraw.left, rcDraw.right, rcDraw.Width(), rcDraw.Height());
		CSize sizeImage(0, 0);
		sizeImage.cx = pImage->GetWidth();
		sizeImage.cy = pImage->GetHeight();
		if ( rcDraw.Width() > sizeImage.cx )
		{
			rcDraw.left += (rcDraw.Width() - sizeImage.cx)/2;
			rcDraw.right = rcDraw.left + sizeImage.cx;
		}
		if ( rcDraw.Height() > sizeImage.cy )
		{
			rcDraw.top += (rcDraw.Height() - sizeImage.cy)/2;
			rcDraw.bottom = rcDraw.top + sizeImage.cy;
		}
		if ( bGrayImage )	// 图片灰度画
		{
			ImageAttributes  imageAttributes;
			UINT             width = pImage->GetWidth();
			UINT             height = pImage->GetHeight();
			Status			 staRet;
			
			ColorMatrix colorMatrix = {
				0.299f,	0.299f, 0.299f, 0.0f, 0.0f,
				0.587f,	0.587f, 0.587f,	0.0f, 0.0f,
				0.144f,	0.144f,	0.144f, 0.0f, 0.0f,
				0.0f,	0.0f,	0.0f,	1.0f, 0.0f,
				0.37f,	0.37f,	0.37f,	0.0f, 1.0f};

			COLORREF clrDisable = GetSysColor(COLOR_GRAYTEXT);
			ColorMap	clrMapText[100];
			for ( int iMapItem =0; iMapItem < 100 ; iMapItem++ )
			{
				clrMapText[iMapItem].newColor = Color(255, GetRValue(clrDisable), GetGValue(clrDisable), GetBValue(clrDisable));
				clrMapText[iMapItem].oldColor = Color(255, (BYTE)iMapItem, (BYTE)iMapItem, (BYTE)iMapItem);
			}

// 			staRet = imageAttributes.SetRemapTable(100, clrMapText, ColorAdjustTypeBitmap);
// 			ASSERT( staRet == Ok );
			staRet = imageAttributes.SetColorMatrix(	&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
			ASSERT( staRet == Ok );
				
			g.DrawImage(
					pImage, 
					Rect(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height()),  // destination rectangle 
					0, 0,        // upper-left corner of source rectangle 
					width,       // width of source rectangle
					height,      // height of source rectangle
					UnitPixel,
					&imageAttributes);
		}
		else
		{
			CRect rcImage(rcDraw);
			if ( bSelectedLikePush )
			{
				rcImage.OffsetRect(1, 1);
			}
			DrawImage(g, pImage, rcImage, 1, 0, true);
// 			UINT             width = pImage->GetWidth();
// 			UINT             height = pImage->GetHeight();
// 			g.DrawImage(
// 				pImage, 
// 				Rect(rcImage.left, rcImage.top, width, height),  // destination rectangle 
// 				0, 0,        // upper-left corner of source rectangle 
// 				width,       // width of source rectangle
// 				height,      // height of source rectangle
// 				UnitPixel
// 					);
		}
	}
	else
	{
		rcDraw.InflateRect(-1,-1);
		
		if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior )
		{
			dc.SetBkMode(TRANSPARENT);
			// 为了美观和focus留出空白，把rcDraw左右缩小那么一点
			rcDraw.InflateRect(-2, 0);
			if ( m_bUseMarllet )
			{
				rcDraw.InflateRect(-2, -2);
				CFont MyFont;
				LOGFONT logfont;
				logfont.lfHeight = -rcDraw.Height();
				logfont.lfWidth = 0;
				logfont.lfEscapement = 0;
				logfont.lfOrientation = 0;
				logfont.lfWeight = FW_NORMAL;
				logfont.lfItalic = FALSE;
				logfont.lfUnderline = FALSE;
				logfont.lfStrikeOut = FALSE;
				logfont.lfCharSet = DEFAULT_CHARSET;
				logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
				logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				logfont.lfQuality = DEFAULT_QUALITY;
				logfont.lfPitchAndFamily = DEFAULT_PITCH;
				
				_tcscpy(logfont.lfFaceName,_T("Marlett"));
				
				MyFont.CreateFontIndirect (&logfont);
				
				CFont* pOldFont = dc.SelectObject (&MyFont); 
				int OldMode = dc.SetBkMode(TRANSPARENT);  
				
				dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				dc.SetBkMode(OldMode);
				dc.SelectObject(pOldFont);
			}
			else
			{
				dc.DrawText(m_StrText, rcDraw, dwTextDT);
			}
		}
	}
	
	dc.RestoreDC(iSave);
}


void CWndRectButton::DrawNude()
{
	if ( !m_RectButton.IsRectEmpty() )
	{
		CClientDC dc(&m_wndTB.GetWnd());
		Draw(dc);
	}
}

bool32 CWndRectButton::NotifyOwner(UINT nNotifyCode/* = NM_CLICK*/, LPARAM lParam/* = NULL*/)
{
	if ( IsWindow(m_pWndOwner->GetSafeHwnd()) && IsEnable() )
	{
		RECTBTN_NOTIFY_ITEM item = {0};
		item.hdr.idFrom = m_wndTB.GetWnd().GetDlgCtrlID();
		item.hdr.hwndFrom = m_wndTB.GetWnd().m_hWnd;
		item.hdr.code = nNotifyCode;
		item.lParam = lParam;
		item.uBtnId = m_nID;
		item.pBtn = this;
		return SendMessage(m_pWndOwner->GetSafeHwnd(), WM_NOTIFY, item.hdr.idFrom, (LPARAM)&item);
	}
	// 非窗口，不应当有消息发送
	ASSERT( 0 );
	return false;
}


void CWndRectButton::SetBtnState( E_BtnDrawState eState, bool32 bForce/* = false */)
{
	ASSERT( m_eBtnBehavior != BTNB_SEPARATOR );
	E_BtnDrawState eOld = m_eBtnState;
	if ( Selected == m_eBtnState && Focus == eState && !bForce )
	{
		// focus状态不能改变选择状态
	}
	else
	{
		m_eBtnState = eState;
	}

	if ( eOld != m_eBtnState )
	{
// 		TRACE(_T("SetBtnState: %d->%d\r\n"), eOld, m_eBtnState);
// 		if ( eOld == Selected )
// 		{
// 			int i = 0;
// 		}
		DrawNude();
		CRect rtRedraw(m_RectButton);
		rtRedraw.InflateRect(2,2);
		m_wndTB.GetWnd().RedrawWindow(rtRedraw);
	}
}

void CWndRectButton::Initial( CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip )
{
	m_pWndOwner		= pWndOwner;
	m_nID			= nID;
	m_StrText		= pszText;
	m_StrTip		= pszTip;
	m_pImageNomal	= pImageNomal;
	m_pImageSelected = pImageSelected;
	m_pImageFocus	= pImageFocus;
	m_pImageDisable = pImageDisable;

	m_RectButton.SetRectEmpty();

	m_eBtnState     = Normal;

	m_eBtnBehavior	= BTNB_NORMAL;
	m_pRadioGroup	= NULL;
	m_bCheck		= false;

	LOGFONT lf = {0};
	lf.lfHeight = 12;
	_tcscpy(lf.lfFaceName, _T("新宋体"));
	m_font.CreateFontIndirect(&lf);

	m_bUseMarllet = false;

	m_iGroupId = -1;

	m_eDrawStyle = Draw_Style1;

	m_eClickStyle = ClickAfterLButtonUpDraw;

	m_bInLButtonUp = false;

	// 初始化各个颜色
	COLORREF clrStartN = RGB(250,250,250);
	COLORREF clrEndN   = RGB(190,190,190);
	COLORREF clrStartS = RGB(140,140,140);
	COLORREF clrEndS   = clrEndN;
	COLORREF clrFrameN = RGB(110,110,110);
	COLORREF clrFrameF = RGB(205,104,65);
	COLORREF clrTextN   = RGB(0,0,0);
	COLORREF clrTextS   = RGB(255,0,0);
	COLORREF clrTextD   = GetSysColor(COLOR_GRAYTEXT);
	SetColor(BTN_COLOR_Background, BTN_Background1, clrStartN, _T("背景边沿"), false);		// 设置所有
	SetColor(BTN_COLOR_Background, BTN_Background2, clrEndN,   _T("背景中间"), false);
	SetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrStartS); // 特例selected
	SetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrEndS);

	SetColor(BTN_COLOR_Frame, 0, clrFrameN, _T("边框"), false);
	SetColor(BTN_COLOR_Frame, Focus, 0, clrFrameF);

	SetColor(BTN_COLOR_Text, 0, clrTextN, _T("文字"), false);
	SetColor(BTN_COLOR_Text, Focus, 0, clrFrameF);
	SetColor(BTN_COLOR_Text, Selected, 0, clrTextS);
	SetColor(BTN_COLOR_Text, Disable, 0, clrTextD);
}

void CWndRectButton::SetBtnBehavior( E_BtnBehavior eNewBehavior, CWndRectRadioBtnGroup *pGroup /*= NULL*/ )
{
	E_BtnBehavior eOld = m_eBtnBehavior;
	if ( BTNB_RADIO == eOld )
	{
		if ( m_pRadioGroup )
		{
			m_pRadioGroup->RemoveRadioBtn(this);
		}
	}
	m_eBtnBehavior	=	eNewBehavior;
	if ( BTNB_RADIO == eNewBehavior )
	{
		ASSERT( pGroup && !m_pRadioGroup );		// 只能加入一次组
		m_pRadioGroup = pGroup;
		if ( m_pRadioGroup )
		{
			SetCheck(false);		// 新进入组，则强制将check为false
			m_pRadioGroup->AddRadioBtn(this);
		}
	}
}

void CWndRectButton::SetCheck( bool32 bCheck )
{
	//ASSERT( IsEnable() );
	if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
	{
		bool32	bOld = m_bCheck;
		m_bCheck = bCheck;
		if ( m_bCheck != bOld )
		{
			DrawNude();
			
			CRect rtRedraw(m_RectButton);
			rtRedraw.InflateRect(2,2);
			m_wndTB.GetWnd().RedrawWindow(rtRedraw);
		}
		if ( m_bCheck && BTNB_RADIO == m_eBtnBehavior )
		{
			if ( m_pRadioGroup )
			{
				m_pRadioGroup->IamCheckBtn(this);
			}
		}
	}
	else
	{
		ASSERT( 0 );
	}
}

void CWndRectButton::SetFont( CFont &font )
{
	LOGFONT lf = {0};
	font.GetLogFont(&lf);
	m_font.DeleteObject();
	m_font.CreateFontIndirect(&lf);
}

void CWndRectButton::SetUseMarlettChar( bool32 bUse /*= false*/ )
{
	m_bUseMarllet = bUse;
}

void CWndRectButton::OnMouseMove( UINT nFlag, CPoint point )
{
	// 普通move， lButton Press后move， mouseLeave后发送的-1-1move
	ASSERT( IsEnable() );
	if ( !IsEnable() )
	{
		return;
	}
	if ( m_RectButton.PtInRect(point) )		// 状态可以尝试状态栈处理
	{
		if ( Selected != m_eBtnState )		
		{
			SetBtnState(Focus);		// 普通move
		}
		// else // lButton press后move - 不改变状态
	}
	else
	{
		if ( Selected != m_eBtnState )
		{
			SetBtnState(Normal);		// 普通离开该区域
		}
		else if ( m_wndTB.GetCaptureBtn() != this )
		{
			ASSERT( m_wndTB.GetCaptureBtn() == NULL );
			if ( !m_bInLButtonUp )
			{
				SetBtnState(Normal);		// 虽然有按下标志，但是这个按钮并没有capture，还原
			}
		}
		// else // lButton后的离开 - 已经在capture了，不需要改变
	}
}


void CWndRectButton::OnLButtonDown( UINT nFlag, CPoint point )
{
	ASSERT( IsEnable() );
	if ( !IsEnable() )
	{
		return;
	}
	if ( m_RectButton.PtInRect(point) )
	{
		SetBtnState(Selected);
		m_wndTB.RequestMouseCapture(*this);				// capture
		ASSERT( m_wndTB.GetCaptureBtn() == this );
	}
	else
	{
		ASSERT( 0 );  // 不是在该按钮区域，不应当收到down消息
	}
}


void CWndRectButton::OnLButtonUp( UINT nFlag, CPoint point )
{
	ASSERT( IsEnable() );
	m_wndTB.ReleaseMouseCapture(*this);		// 申请释放鼠标,wndTB作出释放处理
	if ( !IsEnable() )
	{
		return;
	}
	m_bInLButtonUp = true;

	if ( m_RectButton.PtInRect(point) )
	{
		if ( BTNB_RADIO == m_eBtnBehavior && m_bCheck )	// radio check不做click
		{
			
		}
		else if ( Selected == m_eBtnState )	// 必须先按下
		{
			if ( m_eClickStyle == ClickBeforLButtonUpDraw || BTNB_CHECKBOX == m_eBtnBehavior )
			{		
				DoClicked(point);		// 点击事件产生
				SetBtnState(Normal);		// 还在按钮上
			}
			else
			{
				SetBtnState(Normal);		// 还在按钮上
				DoClicked(point);		// 点击事件产生
				
			}
		}
		else
		{
			SetBtnState(Normal);		// 还在按钮上
		}
	}
	else
	{
		SetBtnState(Normal);	// 离开了
	}
	m_bInLButtonUp = false;
}

bool32 CWndRectButton::IsEnable() const
{
	return Disable != m_eBtnState && BTNB_SEPARATOR != m_eBtnBehavior;		// 分割条相当于禁止了
}

void CWndRectButton::EnableBtn(bool32 bEnable/* = true*/)
{
	if ( m_wndTB.GetCaptureBtn() == this )
	{
		m_wndTB.ReleaseMouseCapture(*this);		// 申请释放鼠标,wndTB作出释放处理
	}

	if ( bEnable && !IsEnable() )
	{
		SetBtnState(Normal);
	}
	else if ( !bEnable && IsEnable() )
	{
		SetBtnState(Disable);
	}
	// else // 已经是要求的状态了，不用处理
}
//lint --e{648} suppress " Overflow in computing constant for operation: 'unsigned sub.'"
void CWndRectButton::OnRButtonUp( UINT nFlag, CPoint point )
{
	// 不设置任何状态 - 可能将来有对工作区改名呀什么操作的需要这个事件
	ASSERT( IsEnable() );

	// 改色操作
	NotifyOwner(NM_RCLICK, NULL);
}

bool32 CWndRectButton::GetColor( E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF &clr )
{
	int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
	if ( m_mapColorItems.count(iColorIndex) > 0 )
	{
		clr = m_mapColorItems[iColorIndex].m_clr;
		return true;
	}
	else
	{
		// 尝试有没有Normal状态的 / index 0 的
		
		iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);
		if ( m_mapColorItems.count(iColorIndex) > 0 )
		{
			clr = m_mapColorItems[iColorIndex].m_clr;
			return true;
		}
		else
		{
			iColorIndex = CombineBtnColorIndex(eType, Normal, 0);
			clr = m_mapColorItems[iColorIndex].m_clr;
			return true;
		}
		
		ASSERT( 0 ); // 所有颜色都至少初始化Normal
		return false;
	}
}

void CWndRectButton::SetColor( E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc/*=NULL*/, bool32 bNeedDraw/* = false*/ )
{
	int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
	if ( m_mapColorItems.count(iColorIndex) > 0 )
	{
		if ( m_mapColorItems[iColorIndex].m_clr != clr )
		{
			m_mapColorItems[iColorIndex].m_clr = clr;
			if ( bNeedDraw && m_eBtnState == eState)
			{
				DrawNude();
			}
		}
	}
	else
	{
		// 初始化
		ASSERT( NULL != lpszDesc && _tcslen(lpszDesc) > 0 );
		CString StrDesc = lpszDesc;
		if ( StrDesc.IsEmpty() )
		{
			StrDesc.Format(_T("颜色%x"), iColorIndex);
		}
		CWndRectButtonColorValue	btnClr(this, StrDesc, clr);
		m_mapColorItems[iColorIndex] = btnClr;
	}
}

void CWndRectButton::SetColor( E_BtnColorType eType, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc/*=NULL*/, bool32 bNeedDraw/*=false*/ )
{
	typedef map<int, CString> MapStr;
	MapStr	Strs;
	Strs[Normal]   = _T("(Normal)");
	Strs[Focus]    = _T("(Focus)");
	Strs[Selected] = _T("(Press/Selected)");
	Strs[Disable]  = _T("(Disable)");


	int iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);

	CString StrDesc = lpszDesc == NULL ? _T("") : lpszDesc;
	if ( StrDesc.IsEmpty() )
	{
		StrDesc.Format(_T("颜色%x"), iColorIndex);
	}
	for ( MapStr::iterator it=Strs.begin(); it!=Strs.end(); it++ )
	{
		SetColor(eType, (E_BtnDrawState)it->first, iIndex, clr, StrDesc+it->second, bNeedDraw);
	}
}

int CWndRectButton::CombineBtnColorIndex( E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex )
{
	return (int)((eType<<16) | ((eState << 8)&0xff00) | ((iIndex)&0xff));
}

void CWndRectButton::SetBtnColors( const WndRectButtonColorItemMap &mapColors, bool32 bDrawIfChange/* = true*/ )
{
	bool32 bDraw = false;
	for ( WndRectButtonColorItemMap::const_iterator it=m_mapColorItems.begin();
		it != m_mapColorItems.end();
		it++)
	{
		WndRectButtonColorItemMap::const_iterator itSet = mapColors.find(it->first);
		if ( itSet != mapColors.end() )		// 有就进行更改设置
		{
			m_mapColorItems[it->first] = itSet->second;
			bDraw = true;
		}
	}
	
	if ( bDraw && bDrawIfChange )
	{
		DrawNude();
	}
}

CSize CWndRectButton::GetBtnTextExtend( CDC *pDC )	// 不完善
{
	bool32	bNeedReleaseDC = false;
	if ( pDC == NULL )
	{
		pDC = m_wndTB.GetWnd().GetDC();
		if ( NULL == pDC )
		{
			return CSize(0,0);
		}
		bNeedReleaseDC = true;
	}
	CSize sizeText(0,0);
	CFont *pFontOld = pDC->SelectObject(&m_font);
	CString StrText = m_StrText;
	if ( StrText.IsEmpty() )
	{
		StrText = _T("高度测试");
	}
	sizeText = pDC->GetTextExtent(StrText);
	pDC->SelectObject(m_font);
	if ( bNeedReleaseDC )
	{
		m_wndTB.GetWnd().ReleaseDC(pDC);
	}

	sizeText.cy += 2*2;
	if ( m_StrText.IsEmpty() )
	{
		sizeText.cx = 2*2;
	}
	if ( BTNB_SEPARATOR == m_eBtnBehavior )
	{
		sizeText.cx = 2;
	}

	pDC->SelectObject(pFontOld);
	return sizeText;
}

void CWndRectButton::MoveBtn( const CRect &RectBtn, bool32 bDraw/* = true*/ )
{
	m_RectButton = RectBtn;
	m_RectButton.NormalizeRect();
	if ( m_RectButton.Width() > 2000 || m_RectButton.Height() > 2000 )
	{
		ASSERT( 0 );
		m_RectButton.SetRectEmpty();
	}

	if ( bDraw )
	{
		DrawNude();
	}
}

void CWndRectButton::GetBtnRect( OUT CRect &RectBtn )
{
	RectBtn = m_RectButton;
}

void CWndRectButton::GetBtnColors( OUT WndRectButtonColorItemMap &mapColor ) const
{
	mapColor = m_mapColorItems;
}

CSize	CWndRectButton::MeasureBtnRect(CDC *pDC, OUT bool32 &bFixed)
{
	bFixed  = false;
	bool32 bDeleteDC = false;
	if ( NULL == pDC )
	{
		pDC = m_wndTB.GetWnd().GetDC();
		if ( NULL == pDC )
		{
			ASSERT( 0 );
			return CSize(0,0);
		}
		bDeleteDC = true;
	}

	// 根据正常图片或者文字来决定
	CSize sizeRet(0,0);
	Image *pImageNormal = m_pImageNomal;
	if ( NULL != pImageNormal )
	{
		sizeRet.cx = pImageNormal->GetWidth() + 2;	// 边框
		sizeRet.cy = pImageNormal->GetHeight() + 2;
		if ( sizeRet.cy < 20 )
		{
			sizeRet.cy = 20;	// 最小高度20
		}
		bFixed = true;
	}
	else if ( IsSeparator() )
	{
		// 分格条，固定2
		CFont *pFontOld = pDC->SelectObject(&m_font);
		sizeRet = pDC->GetTextExtent(_T("测试H"));
		pDC->SelectObject(&m_font);
		sizeRet.cx = 2;
		sizeRet.cy += 6;
		pDC->SelectObject(pFontOld);
	}
	else
	{
		CFont *pFontOld = pDC->SelectObject(&m_font);
		sizeRet = pDC->GetTextExtent(m_StrText);
		pDC->SelectObject(&m_font);
		sizeRet.cx += 14;
		sizeRet.cy += 6;
		pDC->SelectObject(pFontOld);
	}
	if ( bDeleteDC )
	{
		m_wndTB.GetWnd().ReleaseDC(pDC);
	}
	return sizeRet;
}

void CWndRectButton::SetBtnImages( Image *pImageNomal, Image *pImageSelected /*= NULL*/, Image *pImageFocus /*= NULL*/, Image *pImageDisable /*= NULL*/ )
{
	m_pImageNomal = pImageNomal;
	m_pImageSelected = pImageSelected;
	m_pImageFocus = pImageFocus;
	m_pImageDisable = pImageDisable;
}

void CWndRectButton::GetBtnImages( Image **ppImageNomal, Image **ppImageSelected /*= NULL*/, Image **ppImageFocus /*= NULL*/, Image **ppImageDisable /*= NULL*/ ) const
{
	if ( NULL != ppImageNomal )
	{
		*ppImageNomal = m_pImageNomal;
	}
	if ( NULL != ppImageSelected )
	{
		*ppImageSelected = m_pImageSelected;
	}
	if ( NULL != ppImageFocus )
	{
		*ppImageFocus = m_pImageFocus;
	}
	if ( NULL != ppImageDisable )
	{
		*ppImageDisable = m_pImageDisable;
	}
}

bool32 CWndRectButton::GetCheck() const
{
	if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
	{
	}
	else
	{
		ASSERT( 0 );
	}
	return m_bCheck;
}

void CWndRectButton::SetBtnTextWithDraw( LPCTSTR lpszText )
{
	if ( m_StrText != lpszText )
	{
		m_StrText = lpszText;
		DrawNude();
	}
}


// void CWndRectButton::GetDrawTextInRect( CDC &dc, DWORD dwStyle, const CRect &rc, const CString &StrText,OUT CString StrCanDraw )
// {
// 	CRect rcDraw;
// 	int iHeight = dc.DrawText(StrText, &rcDraw, dwStyle|DT_CALCRECT);
// }

//////////////////////////////////////////////////////////////////////////
//


CWndRectMenuButton::CWndRectMenuButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip/* = NULL*/, bool32 bLeftIsBtn/* = false*/)
:CWndRectButton(wndTB, pWndOwner, nID, pszText, pszTip)
{
	m_nRightRectWidth = 22;
	m_ptLastClick = CPoint(-1,-1);
	m_bEnableLeftButton = bLeftIsBtn;
	m_eClick = MENU_BTN_LeftPartClick;

	SetColor(BTN_COLOR_Text, 1, RGB(127,127,127), _T("文字右侧"), false);
}
//lint --e{648} suppress " Overflow in computing constant for operation: 'unsigned sub.'"
bool32 CWndRectMenuButton::DoClicked( CPoint ptClick )
{
	if ( !m_RectButton.PtInRect(ptClick) )
	{
		ASSERT( 0 );
		return false;
	}

	m_ptLastClick = ptClick;
	m_eClick = MENU_BTN_LeftPartClick;

	CRect rcRight;
	GetRightRect(rcRight);
	ASSERT( rcRight.left > m_RectButton.left );
	if ( rcRight.PtInRect(ptClick) )
	{
		// 这个按钮有select状态 - 当右侧击中时，发送消息，弹出菜单
		m_eClick = MENU_BTN_RightPartClick;
		bool32 bRet =  NotifyOwner(NM_CLICK, MENU_BTN_RightPartClick);
		return bRet;
	}
	else
	{
		return CWndRectButton::DoClicked(ptClick);
	}
}

void CWndRectMenuButton::OnMouseMove(UINT nFlag, CPoint ptMove )
{
	ASSERT( IsEnable() );
	// 进入按钮 - 则判断是在左侧还是右侧
	// 左侧不是按钮 - 右侧只响应lButtonDown，Down就发出消息
	CRect rcRight;
	GetRightRect(rcRight);
	if ( rcRight.PtInRect(ptMove)	// 在右侧
		|| (m_RectButton.PtInRect(ptMove) && !m_bEnableLeftButton)  // 在整个框 但是 左侧是文字
		|| (rcRight.PtInRect(m_ptLastClick) &&  Selected==m_eBtnState)  // 处于鼠标压下右侧状态
		)
	{
		if ( Selected != m_eBtnState )
		{
			SetBtnState(Focus);
		}
	}
	else
	{
		CWndRectButton::OnMouseMove(nFlag, ptMove);
	}
	
}


void CWndRectMenuButton::OnLButtonDown( UINT nFlag, CPoint point )
{
	// 如果在右侧
	CRect rcRight;
	GetRightRect(rcRight);
	if ( rcRight.PtInRect(point) )
	{
		SetBtnState(Selected);  // 不必等到up
		DoClicked(point);
		SetBtnState(Normal);	
	}
	else if ( m_bEnableLeftButton )	// 左侧按钮
	{
		CWndRectButton::OnLButtonDown(nFlag, point);
	}
}

void CWndRectMenuButton::OnLButtonUp( UINT nFlag, CPoint point )
{
	// 如果在右侧
	CRect rcRight;
	GetRightRect(rcRight);
	m_ptLastClick = point;
	if ( rcRight.PtInRect(point) )
	{
		// 不处理Up	
	}
	else if ( m_bEnableLeftButton )	// 左侧按钮
	{
		CWndRectButton::OnLButtonUp(nFlag, point);
	}
}

void CWndRectMenuButton::Draw( CDC &dc )
{
	if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior || BTNB_SEPARATOR == m_eBtnBehavior )
	{
		ASSERT( 0 );
	}

	if ( m_RectButton.IsRectEmpty() )
	{
		return;
	}

	if ( m_RectButton.Height() <0 || m_RectButton.Height() > 200
		|| m_RectButton.Width() <0 || m_RectButton.Width() > 2000 )
	{
		ASSERT( 0 );
		return;
	}

	int iSave = dc.SaveDC();

	CRect rcDraw(m_RectButton);
	
	dc.SelectObject(&m_font);

	COLORREF clrBK1 = RGB(250,250,250);
	COLORREF clrBK2 = RGB(190,190,190);
	bool32 bColorOK = true;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;
	
	COLORREF clrTextLeft, clrTextRight;
	COLORREF clrFrame;
	bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrTextLeft) && bColorOK;
	bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 1, clrTextRight) && bColorOK;
	ASSERT( bColorOK );

	bool32	bDrawFocus = true;

	Image *pImage = m_pImageNomal;
	if ( Disable == m_eBtnState )
	{
		if ( m_pImageDisable )
		{
			pImage = m_pImageDisable;
		}
	}
	else if ( Selected == m_eBtnState )
	{
		if ( m_pImageSelected )
		{
			pImage = m_pImageSelected;
		}
	}
	else if ( Focus == m_eBtnState )
	{
		if ( m_pImageFocus )
		{
			pImage = m_pImageFocus;
		}
	}

	CRgn rgn;
	rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
	dc.SelectClipRgn(&rgn, RGN_AND);

	int32 iThrdPartHeight = rcDraw.Height()/3;
	int32 iMid = MAX(iThrdPartHeight, 1);
	CColorStep step(clrBK2, clrBK1, iMid);
	step.SetColorSmooth(true);
	for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
	{
		dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
	}

	CBrush brush;
	brush.CreateSolidBrush(clrFrame);
	dc.FrameRgn(&rgn, &brush, 1, 1);
	rcDraw.InflateRect(-1,-1);


	CRect rcRight(rcDraw);
	rcRight.left = rcRight.right - m_nRightRectWidth;
	ASSERT( rcRight.left >= rcDraw.left );
	CRect rcTriangle(rcRight);
	rcTriangle.InflateRect(-rcTriangle.Width()/3, 0);
	rcTriangle.InflateRect(0, -(rcTriangle.Height() - rcTriangle.Width()/2)/2);
	COLORREF clrTriangle = clrTextRight;
	
	CRect rcLeft(rcDraw);
	rcLeft.right  = rcRight.left;

	if ( pImage )
	{
		Graphics	g(dc.m_hDC);
		g.DrawImage(pImage, m_RectButton.left, m_RectButton.right, m_RectButton.Width(), m_RectButton.Height());
	}

	// 画focus标志 - 没有图片的情况下 - 只画右边小框
	if ( !pImage && Focus == m_eBtnState && bDrawFocus )
	{
		COLORREF clrFocus = clrFrame;
		// 画个小框框 oo
		CRect rcFocus(rcRight);
 		CRgn rgnFocus;
 		rgnFocus.CreateRoundRectRgn(rcFocus.left, rcFocus.top, rcFocus.right, rcFocus.bottom, 3, 3);
 		CBrush brhFocus;
 		brhFocus.CreateSolidBrush(clrFocus);
		dc.FrameRgn(&rgnFocus, &brhFocus, 1, 1);
		rcRight.InflateRect(-1,-1);

		// 三角形变大，颜色改变
		rcTriangle = rcRight;
		rcTriangle.InflateRect(-rcTriangle.Width()/5, 0);
		rcTriangle.InflateRect(0, -(rcTriangle.Height()-rcTriangle.Width()/2)/2);
	}

	// 画三角
	if ( rcTriangle.Width() > 0 && rcTriangle.Height() > 0 )
	{
		POINT ptTriangles[3];
		ptTriangles[0].x = rcTriangle.left;
		ptTriangles[0].y = rcTriangle.top;
		ptTriangles[1].x = rcTriangle.right;
		ptTriangles[1].y = rcTriangle.top;
		ptTriangles[2].x = rcTriangle.CenterPoint().x;
		ptTriangles[2].y = rcTriangle.bottom;
		CBrush brhTriangle, *pBrushOld;
		CPen   penTriangle, *pPenOld;
		brhTriangle.CreateSolidBrush(clrTriangle);
		penTriangle.CreatePen(PS_SOLID, 1, clrTriangle);
		pBrushOld = dc.SelectObject(&brhTriangle);
		pPenOld   = dc.SelectObject(&penTriangle);
		dc.Polygon(ptTriangles, 3);
		dc.SelectObject(pBrushOld);
		dc.SelectObject(pPenOld);
	}

	if ( !m_StrText.IsEmpty() )
	{
		dc.SetTextColor(clrTextLeft);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(m_StrText, rcLeft, DT_SINGLELINE | DT_CENTER | DT_VCENTER| DT_END_ELLIPSIS);
	}

	dc.RestoreDC(iSave);
}


void CWndRectMenuButton::GetRightRect( CRect &rc )
{
	rc = m_RectButton;
	if ( m_RectButton.Width() > (int)m_nRightRectWidth )
	{
		rc.left = rc.right - m_nRightRectWidth;
	}
	else
	{
	}
	ASSERT( rc.Width() >=0 && rc.Width() < 200 );
	ASSERT( rc.Height() >=0 && rc.Height() < 200 );
}

CSize CWndRectMenuButton::GetBtnTextExtend( CDC *pDC )
{
	CSize sizeBtn = CWndRectButton::GetBtnTextExtend(pDC);
	if ( sizeBtn.cx > 0 )
	{
		sizeBtn.cx += m_nRightRectWidth+1;
	}
	return sizeBtn;
}

//////////////////////////////////////////////////////////////////////////
//
// CWndRectRadioBtnGroup
void CWndRectRadioBtnGroup::RemoveAllBtns()
{
	m_mapRadioBtns.RemoveAll();
}

void CWndRectRadioBtnGroup::AddRadioBtn( CWndRectButton *pBtn )
{
	WORD v;
	if ( !m_mapRadioBtns.Lookup(pBtn, v) )
	{
		m_mapRadioBtns[pBtn] = 0;		// 
	}
}

void CWndRectRadioBtnGroup::RemoveRadioBtn( CWndRectButton *pBtn )
{
	WORD v;
	if ( m_mapRadioBtns.Lookup(pBtn, v) )
	{
		m_mapRadioBtns.RemoveKey(pBtn);		// 
	}
}

void CWndRectRadioBtnGroup::IamCheckBtn( CWndRectButton *pBtn )
{
	WORD v;
	if ( m_mapRadioBtns.Lookup(pBtn, v) )
	{
		POSITION pos = m_mapRadioBtns.GetStartPosition();
		while ( pos )
		{
			WORD v1;
			void *pKey = NULL;
			m_mapRadioBtns.GetNextAssoc(pos, pKey, v1);
			if ( pKey && pKey!=pBtn )
			{
				((CWndRectButton *)pKey)->SetCheck(false);
			}
		}
	}
	else
	{
		ASSERT( 0 );
	}
}

int32 CWndRectRadioBtnGroup::GetBtnsCount() const
{
	return m_mapRadioBtns.GetCount();
}

void CWndRectRadioBtnGroup::ResetCheckState()
{
	POSITION pos = m_mapRadioBtns.GetStartPosition();
	while ( pos != NULL )
	{
		void *pKey;
		WORD v;
		m_mapRadioBtns.GetNextAssoc(pos, pKey, v);
		if ( NULL != pKey )
		{
			((CWndRectButton *)pKey)->SetCheck(false);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 简单视图实现
IMPLEMENT_DYNAMIC(CRectButtonWndSimpleImpl, CWnd);

BEGIN_MESSAGE_MAP(CRectButtonWndSimpleImpl, CWnd)
//{{AFX_MSG_MAP(CRectButtonWndSimpleImpl)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_CAPTURECHANGED()
ON_WM_CANCELMODE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CRectButtonWndSimpleImpl::CRectButtonWndSimpleImpl()
{
	m_pBtnCurCapture = m_pBtnLastFocus = NULL;
}

CRectButtonWndSimpleImpl::~CRectButtonWndSimpleImpl()
{
	m_pBtnCurCapture = NULL;
	m_pBtnLastFocus = NULL;
}

bool CRectButtonWndSimpleImpl::RequestMouseCapture( CWndRectButton &btn )
{
	CWndRectButton *pBtn = &btn;
	ASSERT( pBtn && NULL == m_pBtnCurCapture );
	if ( NULL != pBtn )
	{
		m_pBtnCurCapture = pBtn;
		SetCapture();
		return true;
	}
	return false;
}

CWndRectButton * CRectButtonWndSimpleImpl::GetCaptureBtn()
{
	CWndRectButton *pBtn = m_pBtnCurCapture;
	return pBtn;
}

bool CRectButtonWndSimpleImpl::ReleaseMouseCapture( CWndRectButton &btn )
{
	CWndRectButton *pBtn = (&btn);
	CWndRectButton *pBtnCap = GetCaptureBtn();
	ASSERT( pBtn && (pBtn == pBtnCap||pBtnCap==NULL) );	// 由于每次的down不一定会对应一次up，可能会是几次up，所以有可能多释放几次
	if ( NULL != pBtn && (pBtn == pBtnCap||pBtnCap==NULL) )
	{
		m_pBtnCurCapture = NULL;
		if ( GetCapture()==this )
		{
			ReleaseCapture();
		}
		return true;
	}
	return false;
}

BOOL CRectButtonWndSimpleImpl::PreTranslateMessage( MSG* pMsg )
{
	//FilterToolTipMessage(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

int CRectButtonWndSimpleImpl::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	int iRet = CWnd::OnToolHitTest(point, pTI);
	if ( -1 == iRet && pTI )
	{
		// 从这些button里面找
		WndRectButtonVector aBtns;
		const_cast<CRectButtonWndSimpleImpl &>(*this).GetAllBtns(aBtns);
		for ( uint32 i=0; i < aBtns.size() ; i++ )
		{
			CRect rcBtn;
			aBtns[i]->GetBtnRect(rcBtn);
			if ( rcBtn.PtInRect(point) && !aBtns[i]->GetBtnTip().IsEmpty() )
			{
				pTI->hwnd = m_hWnd;
				pTI->lpszText = LPSTR_TEXTCALLBACK;
				pTI->uFlags = 0;
				pTI->rect = rcBtn;
				pTI->uId	= i;
				iRet = i;
				break;
			}
		}
	}
	return iRet;
}

int CRectButtonWndSimpleImpl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iCreate = CWnd::OnCreate(lpCreateStruct);
	EnableToolTips(TRUE);
	if ( -1 == iCreate )
	{
		return iCreate;
	}
	
	return iCreate;
}

void CRectButtonWndSimpleImpl::OnSize( UINT nType, int cx, int cy )
{
	CWnd::OnSize(nType, cx, cy);
	
	RecalcLayout(false);
}

void CRectButtonWndSimpleImpl::OnPaint()
{
	CPaintDC dc(this);
	
	OnDraw(dc);
}

void CRectButtonWndSimpleImpl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CWnd::OnLButtonDown(nFlags, point);
	
	// 先看有capture的没
	CWndRectButton *pCaptureBtn = GetCaptureBtn();
	if ( NULL != pCaptureBtn )
	{
		pCaptureBtn->OnLButtonDown(nFlags, point);
		return;
	}
	
	WndRectButtonVector	aBtns;
	GetAllBtns(aBtns);
	
	for (uint32 i=0; i < aBtns.size(); i++)
	{
		CRect  rcBtn;
		aBtns[i]->GetBtnRect(rcBtn);
		if ( rcBtn.PtInRect(point)
			&& aBtns[i]->IsEnable() )
		{
			aBtns[i]->OnLButtonDown(nFlags, point);
			return;
		}
	}
}

void CRectButtonWndSimpleImpl::OnLButtonUp( UINT nFlags, CPoint point )
{
	CWnd::OnLButtonUp(nFlags, point);
	
	// 先看有capture的没
	CWndRectButton *pCaptureBtn = GetCaptureBtn();
	if ( NULL != pCaptureBtn )
	{
		pCaptureBtn->OnLButtonUp(nFlags, point);
		// 此时应该释放了capture
		ASSERT( NULL == GetCaptureBtn() );
		return;
	}
	
	WndRectButtonVector	aBtns;
	GetAllBtns(aBtns);
	
	for (uint32 i=0; i < aBtns.size(); i++)
	{
		CRect  rcBtn;
		aBtns[i]->GetBtnRect(rcBtn);
		if ( rcBtn.PtInRect(point)
			&& aBtns[i]->IsEnable() )
		{
			aBtns[i]->OnLButtonUp(nFlags, point);
			return;
		}
	}
}

void CRectButtonWndSimpleImpl::OnRButtonUp( UINT nFlags, CPoint point )
{
	CWnd::OnRButtonUp(nFlags, point);
	
	WndRectButtonVector	aBtns;
	GetAllBtns(aBtns);
	
	for (uint32 i=0; i < aBtns.size(); i++)
	{
		CRect  rcBtn;
		aBtns[i]->GetBtnRect(rcBtn);
		if ( rcBtn.PtInRect(point)
			&& aBtns[i]->IsEnable() )
		{
			aBtns[i]->OnRButtonUp(nFlags, point);
			return;
		}
	}
}

void CRectButtonWndSimpleImpl::OnMouseMove( UINT nFlags, CPoint point )
{
	CWnd::OnMouseMove(nFlags, point);
	
	// 先看有capture的没
	CWndRectButton *pCaptureBtn = GetCaptureBtn();
	if ( NULL != pCaptureBtn )
	{
		pCaptureBtn->OnMouseMove(nFlags, point);
		return;
	}
	
	WndRectButtonVector	aBtns;
	GetAllBtns(aBtns);
	
	CWndRectButton	*pFocusBtn = NULL;
	
	for (uint32 i=0; i < aBtns.size(); i++)
	{
		CRect rcBtn;
		aBtns[i]->GetBtnRect(rcBtn);
		if ( rcBtn.PtInRect(point)
			&& aBtns[i]->IsEnable() )
		{
			aBtns[i]->OnMouseMove(nFlags, point);
			pFocusBtn = aBtns[i];
			break;
		}
	}
	
	DWORD dwTmeFlag = 0;
	CWndRectButton *pLastButton = GetLastFocusBtn();
	
	if ( pLastButton != pFocusBtn )
	{
		dwTmeFlag = TME_HOVER;		// 变更button，需要tip提示
		
		// 由于可能存在如 中途删除 m_pLastFoucsButton的情况，所以坐下验证
		
		if ( pLastButton && pLastButton->IsEnable() )
		{
			pLastButton->OnMouseMove(nFlags, CPoint(-1,-1));
		}
		SetLastFocusBtn(pFocusBtn);		// 改变原来的focus
	}
	
	if ( GetLastFocusBtn() != NULL )
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE | dwTmeFlag;
		tme.dwHoverTime = 300;
		tme.hwndTrack = m_hWnd;
		_TrackMouseEvent(&tme);
	}
}

LRESULT CRectButtonWndSimpleImpl::OnMouseLeave( WPARAM w, LPARAM l )
{
	CWndRectButton *pFocusBtn = GetLastFocusBtn();
	if ( pFocusBtn && pFocusBtn->IsEnable() )
	{
		pFocusBtn->OnMouseMove(0, CPoint(-1,-1));
		pFocusBtn = NULL;
	}
	SetLastFocusBtn(NULL);
	
	return Default();
}

LRESULT CRectButtonWndSimpleImpl::OnMouseHover( WPARAM w, LPARAM l )
{
	return Default(); // 关于tip的	
}

BOOL CRectButtonWndSimpleImpl::OnToolTipNotify( UINT id, NMHDR* pNMHDR, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(id);
	UNREFERENCED_PARAMETER(pResult);
	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	
	// 我们所发送的id必属于所分配的按钮当中的一个
	UINT_PTR nID = pNMHDR->idFrom;
	WndRectButtonVector aBtns;
	GetAllBtns(aBtns);
	if ( nID < aBtns.size() )
	{
		strTipText = aBtns[nID]->GetBtnTip();
	}
	
	if (pNMHDR->code == TTN_NEEDTEXTW)
	{
		int iTipHeight = strTipText.GetLength()+1;
		wcsncpy(pTTTW->szText, strTipText, 
			MIN(iTipHeight, sizeof(pTTTW->szText)/(sizeof pTTTW->szText[0])-1));
	}
	else
	{
		::WideCharToMultiByte(CP_ACP , 0, strTipText, -1,
			pTTTA->szText, sizeof(pTTTA->szText), NULL, NULL);
	}
	
	return TRUE;
}

void CRectButtonWndSimpleImpl::OnCancelMode()
{
	OnLButtonUp(0, CPoint(-1, -1));
	
	CWndRectButton *pBtn = GetCaptureBtn();
	if ( NULL != pBtn )
	{
		ASSERT( 0 );
		ReleaseMouseCapture(*pBtn);
	}
}

void CRectButtonWndSimpleImpl::OnCaptureChanged( CWnd* pWnd )
{
	OnLButtonUp(0, CPoint(-1, -1));
	CWndRectButton *pBtn = GetCaptureBtn();
	if ( NULL != pBtn )
	{
		ASSERT( 0 );
		ReleaseMouseCapture(*pBtn);
	}
}

LRESULT CRectButtonWndSimpleImpl::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	// 应该考虑标志过滤掉不用的时机
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
		pTarget = GetParentFrame();
	if (pTarget != NULL)
		OnUpdateCmdUI(pTarget, (BOOL)w);
	return 0;
}

void CRectButtonWndSimpleImpl::OnDraw( CDC &dc )
{
	WndRectButtonVector aBtns;
	GetAllBtns(aBtns);
	for (int32 i=aBtns.size()-1; i >= 0; i-- )
	{
		CRect rcBtn;
		aBtns[i]->GetBtnRect(rcBtn);
		if ( rcBtn.Width() > 0 )
		{
			aBtns[i]->Draw(dc);
		}
	}
}

void CRectButtonWndSimpleImpl::SetLastFocusBtn( CWndRectButton *pBtn )
{
	m_pBtnLastFocus = pBtn;
}

CWndRectButton  * CRectButtonWndSimpleImpl::GetLastFocusBtn()
{
	return m_pBtnLastFocus;
}

// StaticSD.cpp : implementation file
//

#include "stdafx.h"
#include "CStaticSD.h"
#include "IoViewManager.h"
#include "facescheme.h"
#include "MerchManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticSD
CStaticSD::CStaticSD()
{
	m_StrTitle = L"";	
	m_StrSubTitle = L"";
	m_bActive	  = false;
	m_IsDrawCorner = false;
	m_iXposActive = -1;
	m_iYposActive = -1;
	m_ColorActive = RGB(0, 0, 0);

	m_DrawMarkItem.eDrawMarkFlag = EDMF_None;
	m_RectMark.SetRectEmpty();
}

CStaticSD::~CStaticSD()
{

}

BEGIN_MESSAGE_MAP(CStaticSD, CStatic)
	//{{AFX_MSG_MAP(CStaticSD)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSD message handlers

BOOL CStaticSD::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

// 边角的长短长度和离left,top边界的高度
void CStaticSD::GetPolygon(CRect rect, CPoint ptsLeft[7] ,CPoint ptsRight[7], int iMaxLength, int iMinLength, int iCornerBorderH)
{
	// 左上边角
	ptsLeft[0].x = rect.left;
	ptsLeft[0].y = rect.top;
	ptsLeft[1].x = rect.left + iMaxLength;
	ptsLeft[1].y = rect.top;
	ptsLeft[2].x = rect.left + iMaxLength;
	ptsLeft[2].y = rect.top  + iCornerBorderH;
	ptsLeft[3].x = rect.left + iMinLength;
	ptsLeft[3].y = rect.top  + iCornerBorderH;
	ptsLeft[4].x = rect.left + iCornerBorderH;
	ptsLeft[4].y = rect.top  + iMinLength;
	ptsLeft[5].x = rect.left + iCornerBorderH;
	ptsLeft[5].y = rect.top  + iMaxLength;
	ptsLeft[6].x = rect.left;
	ptsLeft[6].y = rect.top  + iMaxLength;

	// 右上边角
	ptsRight[0].x = rect.right;
	ptsRight[0].y = rect.top;
	ptsRight[1].x = rect.right;
	ptsRight[1].y = rect.top + iMaxLength;
	ptsRight[2].x = rect.right - iCornerBorderH;
	ptsRight[2].y = rect.top + iMaxLength;
	ptsRight[3].x = rect.right - iCornerBorderH;
	ptsRight[3].y = rect.top + iMinLength ;
	ptsRight[4].x = rect.right - iMinLength;
	ptsRight[4].y = rect.top + iCornerBorderH;
	ptsRight[5].x = rect.right - iMaxLength;
	ptsRight[5].y = rect.top + iCornerBorderH;
	ptsRight[6].x = rect.right - iMaxLength;
	ptsRight[6].y = rect.top;
}

void CStaticSD::DrawPolygonBorder(CPaintDC *pPaintDC, CRect rect,COLORREF clrFill /* = RGB(234, 23, 23)*/, COLORREF clrBorder /*= RGB(234, 23, 23)*/)
{
	if (pPaintDC)
	{
		CPen penBlue(PS_SOLID, 1, clrBorder);
		CPen* pOldPen = pPaintDC->SelectObject(&penBlue);
		CBrush brushRed(clrFill);
		CBrush* pOldBrush = pPaintDC->SelectObject(&brushRed);

		CPoint ptsLeft[7];
		CPoint ptsRight[7];
		GetPolygon(rect, ptsLeft, ptsRight);

		// we specified.
		pPaintDC->Polygon(ptsLeft, 7);
		pPaintDC->Polygon(ptsRight, 7);

		// Put back the old objects.
		pPaintDC->SelectObject(pOldPen);
		pPaintDC->SelectObject(pOldBrush);
	}
}

// 暂时只有相关数值和买卖盘用到，针对这两种情况分别处理，待优化
void CStaticSD::OnPaint() 
{
	CIoViewBase * pIoViewParent = NULL; 
	CWnd * pParent = GetParent();
	pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);

	CPaintDC dc(this); 

	CRect rect;
	GetClientRect(rect);

	// 填充背景色
	if ( NULL != pIoViewParent )
	{

		COLORREF colo = pIoViewParent->GetIoViewColor(ESCTitleBkColor);
		dc.FillSolidRect(rect,pIoViewParent->GetIoViewColor(ESCBackground));
		dc.SetTextColor(pIoViewParent->GetIoViewColor(ESCVolume));	
	}
	else
	{		
		dc.FillSolidRect(rect,CFaceScheme::Instance()->GetSysColor(ESCBackground));
		dc.SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCVolume));	
	}

	// 画底部边线
	CPen PenTop,PenBottom, *pOldPen;
	//if ( NULL != pIoViewParent)
	//{
	//	PenRed.CreatePen(PS_SOLID,1,pIoViewParent->GetIoViewColor(ESCChartAxisLine)); 
	//}
	//else
	//{
	//	PenRed.CreatePen(PS_SOLID,1,CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine)); 
	//}

	PenTop.CreatePen(PS_SOLID,1,RGB(10, 9, 14)); 
	pOldPen=dc.SelectObject(&PenTop); 
	dc.MoveTo(rect.left,rect.top + 1); 
	dc.LineTo(rect.right,rect.top + 1);
	PenBottom.CreatePen(PS_SOLID,1,RGB(10, 9, 14));
	pOldPen=dc.SelectObject(&PenBottom); 
	dc.MoveTo(rect.left,rect.bottom - 1); 
	dc.LineTo(rect.right,rect.bottom - 1); 

	dc.SelectObject(pOldPen);
	PenTop.DeleteObject();
	PenBottom.DeleteObject();

	CFont* pOldFont = NULL;
	if ( NULL != pIoViewParent)
	{
		pOldFont = dc.SelectObject(pIoViewParent->GetIoViewFontObject( ESFBig ));
	}
	else
	{
		pOldFont = dc.SelectObject(CFaceScheme::Instance()->GetSysFontObject( ESFBig ));
	}

	// 画文字
	{
		// 副标题为空，此时只绘制主标题
		if(m_StrSubTitle.IsEmpty())
		{
			dc.SetTextColor(RGB(249, 249, 148));
			CRect rectTitle = rect;
		    rectTitle.left += 15;
			dc.DrawText(m_StrTitle,rectTitle,DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		}
		else   // 副标题不为空，此时绘制商品的名称、代码、最新价、涨跌幅等相关信息
		{
			// 先绘制主标题(商品名称和商品代码)

			CFont fontLeft;
			fontLeft.CreateFont(30,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,L"Arial");
			CFont* pOldLeft = dc.SelectObject(&fontLeft);
			CString MerchName, MerchCode;
			int pos = m_StrTitle.Find('(');
			MerchName = m_StrTitle.Left(pos);
			MerchCode = m_StrTitle.Mid(pos, m_StrTitle.GetLength() - pos);
			CSize sizeLeft = dc.GetTextExtent(MerchName);
			CFont fontRight;
			fontRight.CreateFont(20,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,L"Arial");
			CFont* pOld = dc.SelectObject(&fontRight);
			CSize sizeRight = dc.GetTextExtent(MerchCode);
			int iTotalLen = sizeLeft.cx + sizeRight.cx + 1;

			dc.SelectObject(fontLeft);
			CRect rectLeft(rect);
			rectLeft.left = rect.left + 10;
			rectLeft.right = rectLeft.left + sizeLeft.cx;
			dc.SetTextColor(RGB(246, 253, 150));
			dc.DrawText(MerchName,rectLeft,DT_RIGHT | DT_VCENTER | DT_SINGLELINE );

			dc.SelectObject(fontRight);
			CRect rectRight(rect);
			rectRight.left = rectLeft.right + 1;
			rectRight.right = rectRight.left + sizeRight.cx + 1;
			rectRight.top = rect.top + 5;
			dc.SetTextColor(RGB(220, 220, 220));
			dc.DrawText(MerchCode,rectRight,DT_LEFT | DT_VCENTER | DT_SINGLELINE );

			// 绘制副标题(商品的最新价、涨跌、涨跌幅)

			// 分割副标题，分解为最新价、涨跌、涨跌幅
			CString StrNewPrice, StrRiseFall, StrRiseFallPercent;
			CString StrTmp = m_StrSubTitle;
		    pos = m_StrSubTitle.Find('|');
			if(-1 != pos)
			{
				StrNewPrice = m_StrSubTitle.Left(pos);
				StrTmp = m_StrSubTitle.Right(m_StrSubTitle.GetLength() - pos - 1);
			}
			pos = StrTmp.Find('|');
			if(-1 != pos)
			{
				StrRiseFall = StrTmp.Left(pos);
				StrRiseFallPercent = StrTmp.Right( StrTmp.GetLength() - pos - 1);
			}

			CFont fontNewPrice;   //最新价的字体
			fontNewPrice.CreateFont(30,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,L"Arial");
			CFont* pOldNewPrice = dc.SelectObject(&fontNewPrice);
			CSize sizeNewPrice = dc.GetTextExtent(StrNewPrice);
			CFont fontRiseFall;   // 涨跌、涨跌幅的字体
			fontRiseFall.CreateFont(17,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,L"Arial");
			CFont* pOldRiseFall = dc.SelectObject(&fontRiseFall);
			CSize sizeRiseFall = dc.GetTextExtent(StrRiseFall);
			CSize sizeRiseFallPercent = dc.GetTextExtent(StrRiseFallPercent);
			int RiseFallWidth = max(sizeRiseFall.cx, sizeRiseFallPercent.cx);

            // 设置绘制涨跌，现价的颜色
			if(pIoViewParent)
			{
				if(-1 != StrRiseFall.Find('-') )    // 跌
				{
					dc.SetTextColor(pIoViewParent->GetIoViewColor(ESCFall));
				}
				else if(StrRiseFall == L"0.00")
				{
					dc.SetTextColor(pIoViewParent->GetIoViewColor(ESCKeep));
				}
				else                                 // 涨
				{
					dc.SetTextColor(pIoViewParent->GetIoViewColor(ESCRise));
				}
			}
			else
			{
				dc.SetTextColor(RGB(220, 220, 220));
			}
		
			// 绘制涨跌和涨跌幅
			dc.SelectObject(fontRiseFall);
			CRect rectRiseFall(rect), rectRiseFallPercent(rect);
			int iYPos = rect.top + (rect.Height() - sizeRiseFall.cy - sizeRiseFallPercent.cy - 2) / 2;
			rectRiseFall.right = rect.right - 20 ;
			rectRiseFall.left = rectRiseFall.right  - RiseFallWidth;
			rectRiseFall.top = iYPos;
			rectRiseFall.bottom = rectRiseFall.top + sizeRiseFall.cy;

			rectRiseFallPercent.right = rect.right - 20 ;
			rectRiseFallPercent.left = rectRiseFall.right  - RiseFallWidth;
			rectRiseFallPercent.top = iYPos + sizeRiseFall.cy + 2;
			rectRiseFallPercent.bottom = rectRiseFallPercent.top + sizeRiseFallPercent.cy;
			dc.DrawText(StrRiseFall,rectRiseFall,DT_LEFT | DT_VCENTER | DT_SINGLELINE );
			dc.DrawText(StrRiseFallPercent,rectRiseFallPercent,DT_LEFT | DT_VCENTER | DT_SINGLELINE );

			// 绘制最新价
			dc.SelectObject(fontNewPrice);
			CRect rectNewPrice(rect);
			rectNewPrice.right = rectRiseFallPercent.left - 20 ;
			rectNewPrice.left = rectNewPrice.right - sizeNewPrice.cx;
			dc.DrawText(StrNewPrice,rectNewPrice,DT_RIGHT | DT_VCENTER | DT_SINGLELINE );
		}
	
	}

	//dc.DrawText(m_StrTitle,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	CSize SizeText = dc.GetTextExtent(m_StrTitle);	
	dc.SelectObject(pOldFont);
	// 上标
	m_RectMark.SetRectEmpty();
	if ( EDMF_None != m_DrawMarkItem.eDrawMarkFlag )
	{
		CFont FontUpHalf;
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(lf));
		dc.GetCurrentFont()->GetLogFont(&lf);
		_tcsncpy(lf.lfFaceName, _T("Tahoma"), LF_FACESIZE-1);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -(abs(lf.lfHeight)*2/3);
		lf.lfHeight = min(-12, lf.lfHeight);
		//lf.lfWeight = FW_NORMAL;
		lf.lfWidth = 0;
		if ( FontUpHalf.CreateFontIndirect(&lf) )
		{
			CFont *pFontOld2 = dc.SelectObject(&FontUpHalf);
			CRect rcDrawText(rect);
			rcDrawText.top = rect.top + (rect.Height()-SizeText.cy)/2 - 3;
			rcDrawText.top = max(rect.top, rcDrawText.top);
			rcDrawText.bottom = rcDrawText.top + SizeText.cy;
			rcDrawText.bottom = min(rcDrawText.bottom, rect.bottom);
			rcDrawText.left = rcDrawText.right - (rcDrawText.Width()-SizeText.cx)/2 + 4;
			if ( rcDrawText.left < rect.right )
			{
				if ( m_DrawMarkItem.clrMark != CLR_DEFAULT )
				{
					dc.SetTextColor(m_DrawMarkItem.clrMark);
				}
				else
				{
					if ( NULL != pIoViewParent)
					{
						dc.SetTextColor(pIoViewParent->GetIoViewColor( ESCAmount ));
					}
					else
					{
						dc.SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCAmount));
					}
				}

				DWORD nFmt = DT_SINGLELINE |DT_LEFT;
				if ( m_DrawMarkItem.eDrawMarkFlag == EDMF_Bottom )
				{
					nFmt |= DT_BOTTOM;
				}
				else
				{
					nFmt |= DT_TOP;
				}
				dc.DrawText(m_DrawMarkItem.strMark, rcDrawText, nFmt);
				CSize sizeMark = dc.GetTextExtent(m_DrawMarkItem.strMark);
				m_RectMark = rcDrawText;
				m_RectMark.right = rcDrawText.left + sizeMark.cx;
			}
			dc.SelectObject(pFontOld2);
		}
	}

	// 激活标志
	if ( m_bActive )
	{
		CRect rectActive(m_iXposActive, m_iYposActive, m_iXposActive*2, m_iYposActive*2);
		//dc.FillSolidRect(&rectActive, m_ColorActive);
	}

	if (m_IsDrawCorner)
	{
		DrawPolygonBorder(&dc, rect);
	}
}

void CStaticSD::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

void CStaticSD::SetTitle( const CString &StrTitle, CString StrSubTitle )
{
	m_StrTitle = StrTitle;
	m_StrSubTitle = StrSubTitle;
	Invalidate(FALSE); 
	UpdateWindow();
	//	OnPaint();
}

void CStaticSD::SetDrawActiveParams(int32 iXpos, int32 iYpos, COLORREF clrActive)
{
	m_iXposActive = iXpos;
	m_iYposActive = iYpos;
	m_ColorActive = clrActive;
}

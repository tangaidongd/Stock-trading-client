// NewsShowView.cpp : implementation file
//

#include "stdafx.h"

#include "NewsShowView.h"
#include "pathfactory.h"
#include "GdiPlusTS.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewsShowView

IMPLEMENT_DYNCREATE(CNewsShowView, CScrollView)

CNewsShowView::CNewsShowView()
{
	m_scrollSize.cx = 0;
	m_scrollSize.cy = 0;
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_LOGO, L"PNG", m_pImgLogo))
	{
		//ASSERT(0);
		m_pImgLogo = NULL;
	}
//	m_pImgLogo = Image::FromFile(CPathFactory::GetImageLoginLogoPath());	
	m_iHeightImg = 0;
	if (NULL != m_pImgLogo)
	{
		m_iHeightImg = m_pImgLogo->GetHeight() + 20;
	}
//	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	m_strAppName = L"投顾.点金手";//pApp->m_pszAppName;
}

CNewsShowView::~CNewsShowView()
{
	DEL(m_pImgLogo);
}

BEGIN_MESSAGE_MAP(CNewsShowView, CScrollView)
	//{{AFX_MSG_MAP(CNewsShowView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    ON_WM_MOUSEWHEEL()
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsShowView drawing

void CNewsShowView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CNewsShowView::OnDraw(CDC* pDC)
{
	CRect rcClient;
	GetClientRect( &rcClient );

	int32 iSaveDc = pDC->SaveDC();
	
	CPoint ptScroll = GetScrollPosition();
	ptScroll.y -= m_iHeightImg;
	rcClient += ptScroll;
		//	
	//CFont* pFont = GetIoViewFontObject(ESFNormal);
	//CFont* pFontOld = dc->SelectObject(pFont);

	//
	//COLORREF clrText	= GetIoViewColor(ESCText);
	COLORREF clrBkGroud	= RGB(240, 255, 240);	// 精品对话框的底色
	//COLORREF clrLine = GetIoViewColor(ESCChartAxisLine);
	CRect rcTotal(rcClient);	// 背景色填充，应该要按整个窗口的长度来填充，包括要通过滚动条下拉才能显示的部分
	CSize sizeTotal = GetTotalSize();
	rcTotal.top = 0;
	rcTotal.bottom = sizeTotal.cy;
	//
	pDC->FillSolidRect(rcTotal, clrBkGroud);
	//
	CRect rcLogo = rcClient;
	rcLogo.top = 0;
	rcLogo.bottom = m_iHeightImg;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	
	bmp.CreateCompatibleBitmap(pDC, rcLogo.Width(), rcLogo.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(rcLogo, RGB(240, 255, 240));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	RectF fRect;
	if (NULL != m_pImgLogo)
	{
		fRect.X = (REAL)20;
		fRect.Y = (REAL)10;
		fRect.Width = (REAL)m_pImgLogo->GetWidth();
		fRect.Height = (REAL)m_pImgLogo->GetHeight();
		
		graphics.DrawImage(m_pImgLogo, fRect, 0, 0, m_pImgLogo->GetWidth(), m_pImgLogo->GetHeight(), UnitPixel);
	}

	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)(fRect.X + fRect.Width + 8);
		grect.Y = (REAL)1;
		grect.Width = (REAL)80;
		grect.Height = (REAL)(m_iHeightImg);
		
		//绘制文字
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentCenter);
		strFormat.SetLineAlignment(StringAlignmentCenter);
		
		typedef struct T_NcFont 
		{
		public:
			CString	m_StrName;
			float   m_Size;
			int32	m_iStyle;
			
		}T_NcFont;
		T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
		m_Font.m_Size	 = 14;
		m_Font.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);
		
		RectF rcBound;
		PointF point;
		graphics.MeasureString(m_strAppName, m_strAppName.GetLength(), &font, point, &strFormat, &rcBound);
		grect.Width = rcBound.Width;
		
		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255,180,0));
		graphics.DrawString(m_strAppName, m_strAppName.GetLength(), &font, grect, &strFormat, &brush);
	}

	pDC->BitBlt(0, 0, rcLogo.Width(), rcLogo.Height(), &memDC, 0, 0, SRCCOPY);
	pDC->SelectClipRgn(NULL);

	rcClient.top += m_iHeightImg;
	rcClient.bottom += m_iHeightImg;

	m_NewsText.Draw(pDC,rcClient,ptScroll,-1,-1);

	//CPen pen(PS_SOLID, 1, clrLine);
	//CPen* pPenOld = dc->SelectObject(&pen);
	
	//
	//dc.SelectObject(pFontOld);
	//dc.SelectObject(pPenOld);
	//pen.DeleteObject();
	
	pDC->RestoreDC(iSaveDc);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CNewsShowView diagnostics

#ifdef _DEBUG
void CNewsShowView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CNewsShowView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewsShowView message handlers

void CNewsShowView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CNewsShowView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CNewsShowView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CNewsShowView::SetNewsConext(const CString &strTitle,const CString &strTimer,const CString &strText)
{
	CRect rcClient;
	GetClientRect( &rcClient );
	rcClient.right -=20;

	CGraphTextFont fontTemp;
	fontTemp.SetFontColor(RGB(105,105,105));
	fontTemp.GetLogFont()->lfHeight = -17;
	fontTemp.GetLogFont()->lfWeight = 600;

	m_NewsText.InitStatus(rcClient);
	m_NewsText.SetNewsText( strTitle ,0,&fontTemp,CGraphTextLine::GRPAHTXT_MKMODE_CENTER);

	int nLeng = strTitle.GetLength()+1;
	CGraphTextFont fontN;
	m_NewsText.SetNewsText(_T("\n"),nLeng,&fontN);
	nLeng +=1;

	fontN.SetFontColor(RGB(105,105,105));
	fontN.GetLogFont()->lfHeight = -14;
	m_NewsText.SetNewsText(strTimer,nLeng,&fontN,CGraphTextLine::GRPAHTXT_MKMODE_CENTER);
	nLeng +=strTimer.GetLength()+1;

	m_NewsText.SetNewsText(_T("\n"),nLeng,&fontN);
	nLeng +=1;
	CGraphTextFont fontT;
	fontT.SetFontColor(RGB(105,105,105));
	m_NewsText.AddLineFlag(nLeng, &fontT);
	nLeng +=2;

	m_NewsText.SetNewsText(_T("\n"),nLeng,&fontN);
	nLeng +=1;
	// 精品正文 
	fontT.SetFontColor(RGB(0, 0, 0));
	fontT.GetLogFont()->lfHeight = -15;
	CString strTemp = _T("　　");
	m_NewsText.SetNewsText(strTemp + strText,nLeng,&fontT);
	nLeng += (strText.GetLength() + strTemp.GetLength());

	fontT.GetLogFont()->lfHeight = -12;
	m_NewsText.SetNewsText(_T("\n\n"),nLeng,&fontN);
	nLeng += 2;
	m_NewsText.SetNewsText(_T("\r\n"),nLeng,&fontN);
	nLeng += 1;
	fontT.SetFontColor(RGB(153,0,0));
	m_NewsText.AddLineFlag(nLeng, &fontT,2);
	nLeng += 1;

	fontT.SetFontColor(RGB(105,105,105));
	fontT.GetLogFont()->lfHeight = -14;
	m_NewsText.SetNewsText(_T("\n"),nLeng,&fontN);
	nLeng += 1;

	// 免责声明
	CString strDes = _T("【免责声明】");
	fontT.SetFontColor(RGB(105, 105, 105));
	fontT.GetLogFont()->lfHeight = -12;

	m_NewsText.SetNewsText(strDes,nLeng,&fontT);
	nLeng +=strDes.GetLength();

	fontT.SetFontColor(RGB(155, 155, 155));
	strDes = ((CGGTongApp*)AfxGetApp())->m_pConfigInfo->m_StrResponsbility;
	m_NewsText.SetNewsText(strDes,nLeng,&fontT);
	nLeng +=strDes.GetLength();

	m_NewsText.MakeUpText();

	CalcScrollSize(NULL);
}


void CNewsShowView::SetDisclaimer(const CString &strTitle,const CString &strDisc)
{
	CRect rcClient;
	GetClientRect( &rcClient );
	rcClient.right -=20;
	
	CGraphTextFont fontTemp;
	fontTemp.SetFontColor(RGB(255,0,0));//(105,105,105));
	fontTemp.GetLogFont()->lfHeight = -16;
	fontTemp.GetLogFont()->lfWeight = 600;
	
	m_NewsText.InitStatus(rcClient);
	m_NewsText.SetNewsText( strTitle ,0,&fontTemp,CGraphTextLine::GRPAHTXT_MKMODE_CENTER);
	
	int nLeng = strTitle.GetLength();
	CGraphTextFont fontN;
	m_NewsText.SetNewsText(_T("\n\n"),strTitle.GetLength(),&fontN,CGraphTextLine::GRPAHTXT_MKMODE_CENTER);
	nLeng +=1;
	
	CGraphTextFont fontT;
	//fontT.SetFontColor(RGB(105,105,105));
	//m_NewsText.AddLineFlag(nLeng, &fontT);
	//nLeng +=1;
	
	m_NewsText.SetNewsText(_T("\r\n"),nLeng,&fontN);
	nLeng +=2;
	
	fontT.SetFontColor(RGB(105,105,105));
	fontT.GetLogFont()->lfHeight = -14;
	m_NewsText.SetNewsText(strDisc,nLeng,&fontT);
	nLeng +=strDisc.GetLength();

	m_NewsText.MakeUpText();
	
	CalcScrollSize(NULL);
}


void CNewsShowView::CalcScrollSize(CDC *pDC)
{
	CPoint point = m_NewsText.GetTextMaxSize();
	point.y += m_iHeightImg;
	CPoint ptmov = CPoint(0,0);
	
	CRect rcClient;
	GetClientRect( &rcClient );
	
	CSize scrollSize(0,0);
	
	CRect rect(ptmov.x,ptmov.y,point.x +ptmov.x,point.y +ptmov.y);
	
	if( pDC == NULL)
	{
		CPaintDC		dc(this);
		OnPrepareDC(&dc);
		
		scrollSize.cx =(int) (point.x +ptmov.x) + 1;//)( size.cx * m_fScaleX )+1;
		scrollSize.cy =(int) (point.y +ptmov.y) + 1;//( size.cy * m_fScaleY )+1;
		
		if( m_scrollSize != scrollSize )
		{
			CSize status = dc.SetWindowExt( scrollSize);
			ASSERT(status != CSize(0, 0));
			
			SetScrollSizes(MM_TEXT,scrollSize);
			
			SetScrollRange( SB_HORZ ,0, scrollSize.cx,TRUE );
			SetScrollRange( SB_VERT ,0, scrollSize.cy,TRUE );
			
			ASSERT( scrollSize == GetTotalSize());
			
			m_scrollSize = scrollSize ;
		}
		
	}
	else
	{
		scrollSize.cx =(int) (point.x +ptmov.x) + 1;//)( size.cx * m_fScaleX )+1;
		scrollSize.cy =(int) (point.y +ptmov.y) + 1;//( size.cy * m_fScaleY )+1;
		
		if( m_scrollSize != scrollSize )
		{
			CSize status = pDC->SetWindowExt( scrollSize);
			ASSERT(status != CSize(0, 0));
			
			SetScrollSizes(MM_TEXT, scrollSize);
			
			SetScrollRange( SB_HORZ ,0, scrollSize.cx,TRUE );
			SetScrollRange( SB_VERT ,0, scrollSize.cy,TRUE );
			
			ASSERT( scrollSize == GetTotalSize());
			
			m_scrollSize = scrollSize ;
		}
	}
}

int CNewsShowView::OnMouseActivate(CWnd*pDeskopWnd,UINT nHitTest,UINT message)
{
	int nResult = CWnd::OnMouseActivate(pDeskopWnd,nHitTest,message);
	this->SetFocus();
	if(nResult == MA_NOACTIVATE|| nResult == MA_NOACTIVATEANDEAT)
		return nResult;
	return nResult;
	
}

#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
BOOL CNewsShowView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    /*int nPagesScrolled = zDelta / 120;
	
    if (nPagesScrolled > 0)
	{
        for (int i = 0; i < nPagesScrolled; i++)
		{
            PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
		}
	}
    else
	{
        for (int i = 0; i > nPagesScrolled; i--)
		{
            PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
		}
	}
 
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);*/
	this->DoMouseWheel(nFlags, zDelta, pt);
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
#endif // !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
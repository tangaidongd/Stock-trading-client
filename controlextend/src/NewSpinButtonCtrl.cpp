// NewSpinButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "NewSpinButtonCtrl.h"
#include "GdiPlusTS.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewSpinButtonCtrl

CNewSpinButtonCtrl::CNewSpinButtonCtrl()
{
	m_bTopDown = FALSE;
	m_bBottomDown = FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_SPIN_ADD, L"PNG", m_pImgTop))
	{
		//ASSERT(0);
		m_pImgTop = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_SPIN_SUB, L"PNG", m_pImgBottom))
	{
		//ASSERT(0);
		m_pImgBottom = NULL;
	}
}

CNewSpinButtonCtrl::~CNewSpinButtonCtrl()
{
	if (NULL != m_pImgBottom)
	{
		delete m_pImgBottom;
		m_pImgBottom = NULL;
	}
	if (NULL != m_pImgTop)
	{
		delete m_pImgTop;
		m_pImgTop = NULL;
	}	
}

BEGIN_MESSAGE_MAP(CNewSpinButtonCtrl, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CNewSpinButtonCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewSpinButtonCtrl message handlers

void CNewSpinButtonCtrl::OnPaint() 
{
	CPaintDC dc(this); 
	CalculateRect();
	Graphics graphics(dc.GetSafeHdc());

	RectF destTop;
	destTop.X = 0;
	destTop.Y = 0;
	destTop.Width  = (Gdiplus::REAL)m_rctTop.Width();
	destTop.Height = (Gdiplus::REAL)m_rctTop.Height();
	RectF destBottom;
	destBottom.X = 0;
	destBottom.Y = (Gdiplus::REAL)m_rctBottom.top;
	destBottom.Width  = (Gdiplus::REAL)m_rctBottom.Width();
	destBottom.Height = (Gdiplus::REAL)m_rctBottom.Height();
	int iIndexTop = 0, iIndexBottom = 0;

	if (m_bTopDown)
	{
		iIndexTop += (int)destTop.Height;
		iIndexBottom = 0;
	}
	else if (m_bBottomDown)
	{
		iIndexTop = 0;
		iIndexBottom =  (int)destBottom.Height;
	}
	else
	{
		iIndexTop = 0;
		iIndexBottom = 0;
	}

	if (!IsWindowEnabled())
	{
		iIndexTop = (int)(2*destTop.Height);
		iIndexBottom =  (int)(2*destBottom.Height);
	}

	graphics.DrawImage(m_pImgTop, destTop, 0.0f, (REAL)iIndexTop, (REAL)destTop.Width, (REAL)destTop.Height, UnitPixel);
	graphics.DrawImage(m_pImgBottom, destBottom, 0.0f, (REAL)iIndexBottom, (REAL)destBottom.Width, (REAL)destBottom.Height, UnitPixel);
}

void CNewSpinButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rctTop.PtInRect(point))
	{
		m_bTopDown = TRUE;
		m_bBottomDown = FALSE;
	}
	else if (m_rctBottom.PtInRect(point))
	{
		m_bTopDown = FALSE;
		m_bBottomDown = TRUE;
	}

	CSpinButtonCtrl::OnLButtonDown(nFlags, point);
}

void CNewSpinButtonCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bTopDown = FALSE;
	m_bBottomDown = FALSE;
	CSpinButtonCtrl::OnLButtonUp(nFlags, point);
}

void CNewSpinButtonCtrl::CalculateRect()
{
	CRect rctClient;
	GetClientRect(&rctClient);
	
	m_rctTop = rctClient;
	m_rctTop.bottom = rctClient.Height()/2;
	
	m_rctBottom = m_rctTop;
	m_rctBottom.top = m_rctTop.bottom;
	m_rctBottom.bottom = rctClient.bottom;
}
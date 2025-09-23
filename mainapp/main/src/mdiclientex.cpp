// mdiclientex.cpp : implementation file
//
#include "stdafx.h"

#include "mdiclientex.h"
#include "PathFactory.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIClientEx

CMDIClientEx::CMDIClientEx()
{
	CString StrFile = CPathFactory::GetImageMainFrameBkGroundPath();

	//
	m_pImageBkGround = Image::FromFile(StrFile);
	
	if ( NULL != m_pImageBkGround && m_pImageBkGround->GetLastStatus() != Ok )
	{
		DEL(m_pImageBkGround);
	}

// 	if ( NULL == m_pImageBkGround )
// 	{
// 		if ( !ImageFromIDResource(IDR_JPG_MAINFRAME_BKGROUND, L"JPG", m_pImageBkGround) )
// 		{
// 			m_pImageBkGround = NULL;
// 		}
// 	}
}

CMDIClientEx::~CMDIClientEx()
{
	DEL(m_pImageBkGround);
}

BEGIN_MESSAGE_MAP(CMDIClientEx, CWnd)
	//{{AFX_MSG_MAP(CMDIClientEx)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMDIClientEx message handlers

WNDPROC* CMDIClientEx::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper = NULL;
	return &pfnSuper;
}

void CMDIClientEx::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	if ( NULL != m_pImageBkGround )
	{
		Graphics GraphicImage(dc.GetSafeHdc());
		DrawImage(GraphicImage, m_pImageBkGround, rect, 1, 0, true);
	}	
}

BOOL CMDIClientEx::OnEraseBkgnd(CDC* pDC) 
{
	//return Default();
	return TRUE;
}

void CMDIClientEx::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Invalidate(TRUE);
}

void CMDIClientEx::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp )
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	CRect rc(lpncsp->rgrc[0]);
	
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

	if ( dwStyle & WS_EX_CLIENTEDGE )
	{
		// 消除ClientEdge
		int iEdgeWidth = 2 * GetSystemMetrics(SM_CXBORDER);
		int iEdgeHeight = 2 * GetSystemMetrics(SM_CYBORDER);
		lpncsp->rgrc[0].left -= iEdgeWidth;
		lpncsp->rgrc[0].right += iEdgeHeight;
		//lpncsp->rgrc[0].top = 0;
		//lpncsp->rgrc[0].bottom += rc.top;
	}
}

void CMDIClientEx::OnNcPaint()
{
	
	CWindowDC dc(this);

	CRect rcWin;
	GetWindowRect(rcWin);
	ScreenToClient(&rcWin);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.OffsetRect(-rcWin.left, -rcWin.top);  // 相对于整个窗口的0,0，偏移client坐标

	rcWin.OffsetRect(-rcWin.left, -rcWin.top);	 // 将Client的坐标 转化为 整个窗口的 0,0 坐标

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcWin);

	CRgn rgnClient;
	rgnClient.CreateRectRgnIndirect(&rcClient);
	rgn.CombineRgn(&rgn, &rgnClient, RGN_DIFF);		// 取得窗口

	dc.SelectClipRgn(&rgn);

	rcWin.left -= 2 * GetSystemMetrics(SM_CXBORDER);
	rcWin.right += 2 * GetSystemMetrics(SM_CXBORDER);
	dc.DrawEdge(rcWin, EDGE_SUNKEN, BF_RECT);
}

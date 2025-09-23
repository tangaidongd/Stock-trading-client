// MyDlgBar.cpp : 实现文件
//

#include "stdafx.h"
#include "MyDlgBar.h"

#define MIN_DYNAMIC_LENGTHX (60)
#define MIN_DYNAMIC_LENGTHY (40)

#define MIN_DYNAMIC_DOCK_LENGTHY	(6)
#define DOCK_RESIZE_LENGTH		(4)

// CMyDlgBar

IMPLEMENT_DYNAMIC(CMyDlgBar, CDialogBar)

CMyDlgBar::CMyDlgBar()
{
	m_sizeMRU = CSize(-1, -1);
	m_sizeResizeTmp = CSize(-1, -1);
	m_sizeDockMRU = CSize(-1, -1);

	m_hOldCursor = NULL;
}

CMyDlgBar::~CMyDlgBar()
{
	m_hOldCursor = NULL;
}


BEGIN_MESSAGE_MAP(CMyDlgBar, CDialogBar)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CMyDlgBar::OnLButtonDblClk( UINT nFlags, CPoint pt )
{
	// only toggle docking if clicked in "void" space
	if (m_pDockBar != NULL && OnToolHitTest(pt, NULL) == -1)
	{
		// start the drag
		ASSERT(m_pDockContext != NULL);
		if ( !IsFloating()
			&& (m_pDockContext->m_ptMRUFloatPos.x < 0
			|| m_pDockContext->m_ptMRUFloatPos.y < 0) )
		{
			CRect rcFrame(0,0,0,0);
			CFrameWnd *pFrmTop = GetTopLevelFrame();
			if ( NULL != pFrmTop )
			{
				pFrmTop->GetWindowRect(&rcFrame);

				CPoint ptCenter = rcFrame.CenterPoint();
				ptCenter.x -= m_sizeDefault.cx/2;
				ptCenter.y -= m_sizeDefault.cy/2;
				m_pDockContext->m_ptMRUFloatPos = ptCenter;
			}
		}
		m_pDockContext->ToggleDocking();
	}
	else
	{
		CWnd::OnLButtonDblClk(nFlags, pt);
	}
}

void CMyDlgBar::OnSize( UINT nType, int cx, int cy )
{
	CDialogBar::OnSize(nType, cx, cy);
}

CSize CMyDlgBar::CalcDynamicLayout( int nLength, DWORD nMode )
{
	BOOL bStretch	= nMode&LM_STRETCH;
	BOOL bHorz		= nMode&LM_HORZ;
	CSize sizeParent(0, 0);
	CFrameWnd *pFrameParent = GetParentFrame();
	if ( NULL != pFrameParent )
	{
		CRect rc;
		pFrameParent->GetClientRect(rc);
		sizeParent = rc.Size();
	}

	if ( bStretch )
	{
		return CalcFixedLayout(bStretch, bHorz);
	}

	BOOL bFloating = IsFloating();
	if ( bFloating )
	{
		// 可以任意
		if ( nMode&LM_MRUWIDTH )
		{
			if ( m_sizeMRU.cx == -1 )
			{
				m_sizeMRU.cx = m_sizeDefault.cx;
			}
			if ( m_sizeMRU.cy == -1 )
			{
				m_sizeMRU.cy = m_sizeDefault.cy;
			}
			return m_sizeMRU;
		}
		else if ( nMode&LM_COMMIT )
		{
			m_sizeResizeTmp.cx = m_sizeResizeTmp.cx==-1? m_sizeDefault.cx : m_sizeResizeTmp.cx;
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;

			if ( nMode&LM_LENGTHY && nLength > 0 )
			{
				m_sizeResizeTmp.cy = nLength;
			}
			else if ( nLength > 0 )
			{
				m_sizeResizeTmp.cx = nLength;
			}

			m_sizeMRU = m_sizeResizeTmp;
			return m_sizeMRU;
		}
		else if ( nMode&LM_HORZDOCK )
		{
			CFrameWnd *pFrmTop = GetTopLevelFrame();
			if ( NULL != pFrmTop )
			{
				CRect rc;
				pFrmTop->GetClientRect(rc);
				sizeParent = rc.Size();
			}
			return CSize(sizeParent.cx, m_sizeDockMRU.cy==-1? m_sizeDefault.cy: m_sizeDockMRU.cy);
		}
		else if ( nLength >= 0 )
		{
			if ( nMode&LM_LENGTHY )
			{
				nLength  = max(MIN_DYNAMIC_LENGTHY, nLength);	// 不能太小了，小了在sizeParent会出错
				m_sizeResizeTmp.cy = nLength;
			}
			else
			{
				nLength  = max(MIN_DYNAMIC_LENGTHX, nLength);	// 不能太小了，小了在sizeParent会出错
				m_sizeResizeTmp.cx = nLength;
			}
			m_sizeResizeTmp.cx = m_sizeResizeTmp.cx==-1? m_sizeDefault.cx : m_sizeResizeTmp.cx;
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;
			return m_sizeResizeTmp;
		}

		return m_sizeDefault;
	}
	else
	{
		// 宽度不可变，高度可变
		if ( nMode&LM_COMMIT )
		{
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;
			m_sizeDockMRU.cy   = m_sizeResizeTmp.cy;
			return CSize(sizeParent.cx, m_sizeResizeTmp.cy);
		}
		else if ( nLength >= 0 && (LM_LENGTHY&nMode) )
		{
			nLength = max(MIN_DYNAMIC_DOCK_LENGTHY, nLength);
			m_sizeResizeTmp.cy = nLength;
			return CSize(sizeParent.cx, nLength);
		}

		return CSize(sizeParent.cx, m_sizeDockMRU.cy==-1? m_sizeDefault.cy: m_sizeDockMRU.cy);
	}
}

CSize CMyDlgBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	CSize sizeParent(0, 0);
	CFrameWnd *pFrameParent = GetParentFrame();
	if ( NULL != pFrameParent )
	{
		CRect rc;
		pFrameParent->GetClientRect(rc);
		sizeParent = rc.Size();
		sizeParent.cy = m_sizeDefault.cy;
	}

	if (bStretch) // if not docked stretch to fit
		return CSize(bHorz ? 32767 : m_sizeDefault.cx,
		bHorz ? m_sizeDefault.cy : 32767);
	else
		return sizeParent;
}

BOOL CMyDlgBar::OnEraseBkgnd( CDC* pDC )
{
	if ( NULL != pDC )
	{
		CRect rc;
		GetClientRect(rc);
		pDC->FillSolidRect(rc, RGB(0,0,0));
		return TRUE;
	}
	return FALSE;
}

int CMyDlgBar::OnCreate( LPCREATESTRUCT lpcs )
{
	int iRet = CDialogBar::OnCreate(lpcs);
	if ( iRet >= 0 )
	{
		CMyDockContext *pDockContext = new CMyDockContext(this);
		pDockContext->EnableDrag(false);
		m_pDockContext = pDockContext;
	}
	return iRet;
}

void CMyDlgBar::OnLButtonDown( UINT nFlags, CPoint pt )
{
	CDialogBar::OnLButtonDown(nFlags, pt);
}

BOOL CMyDlgBar::PreTranslateMessage( MSG* pMsg )
{
	if ( pMsg->message == WM_MOUSEMOVE )
	{
		if ( PtInResizeSep(pMsg->pt) )
		{
			m_hOldCursor = SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}
	}
	else if ( pMsg->message == WM_LBUTTONDOWN )
	{
		CRect rc(0,0,0,0);
		GetWindowRect(rc);
		rc.bottom = rc.top+4;
		if ( rc.PtInRect(pMsg->pt) )
		{			
			CControlBar* pBar = this;

			ASSERT(pBar);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL && pBar->m_pDockBar);

			// CBRS_SIZE_DYNAMIC toolbars cannot have the CBRS_FLOAT_MULTI style
			ASSERT((pBar->m_pDockBar->m_dwStyle & CBRS_FLOAT_MULTI) == 0);
			m_pDockContext->StartResize(HTTOP, pMsg->pt);

			SetCursor(m_hOldCursor);

			return TRUE;
		}

	}

	return CDialogBar::PreTranslateMessage(pMsg);
}

BOOL CMyDlgBar::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CPoint pt(0,0);
	GetCursorPos(&pt);
	if ( PtInResizeSep(pt) )
	{
		return TRUE;
	}
	return CDialogBar::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CMyDlgBar::PtInResizeSep( CPoint ptInScreen )
{
	CRect rc(0,0,0,0);
	GetWindowRect(rc);
	bool bTest = false;
	if ( !IsFloating() )
	{
		// 处在dock下, 
		if ( m_dwStyle&CBRS_SIZE_DYNAMIC )
		{
			if ( m_dwStyle&CBRS_ALIGN_BOTTOM )
			{
				// 顶部可调
				rc.bottom = rc.top+DOCK_RESIZE_LENGTH;
				bTest = true;
			}
			else if ( m_dwStyle&CBRS_ALIGN_TOP )
			{
				// 底部可调
				rc.top = rc.bottom-DOCK_RESIZE_LENGTH;
				bTest = true;
			}
		}
	}

	if ( bTest )
	{
		return rc.PtInRect(ptInScreen);
	}
	return FALSE;
}

// CMyDlgBar 消息处理程序



//////////////////////////////////////////////////////////////////////////
//
#ifndef m_rectRequestedSize
#define m_rectRequestedSize     m_rectDragHorz
#define m_rectActualSize        m_rectDragVert
#define m_rectActualFrameSize   m_rectFrameDragHorz
#define m_rectFrameBorders      m_rectFrameDragVert
#define HORZF(dw) (dw & CBRS_ORIENT_HORZ)
#define VERTF(dw) (dw & CBRS_ORIENT_VERT)
#endif

void CMyDockContext::StartDrag( CPoint pt )
{
	if ( m_bEnableDrag )
	{
		CDockContext::StartDrag(pt);
	}
	else
	{
		// 啥都不做
		// 浮动窗口允许拖动
		ASSERT_VALID(m_pBar);
		ASSERT_VALID(m_pBar->m_pDockBar);
		if ( m_pBar->IsFloating() )
		{
			CDockContext::StartDrag(pt);
		}
	}
}

void CMyDockContext::StartResize( int nHitTest, CPoint pt )
{
	ASSERT_VALID(m_pBar);
	ASSERT(m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC);
	if ( m_pBar->IsFloating() )
	{
		CDockContext::StartResize(nHitTest, pt);
		return;
	}
	else if ( !(m_pBar->m_dwStyle&CBRS_SIZE_DYNAMIC) )
	{
		return;	// 不能变更大小
	}

	// 停靠变更大小
	m_bDragging = FALSE;

	InitLoop();

	// GetWindowRect returns screen coordinates(not mirrored)
	// So if the desktop is mirrored then turn off mirroring
	// for the desktop dc so that we draw correct focus rect 

	if (m_pDC->GetLayout() & LAYOUT_RTL)
		m_pDC->SetLayout(LAYOUT_LTR);

	// get true bar size (including borders)
	CRect rect;
	m_pBar->GetWindowRect(rect);
	m_ptLast = pt;
	m_nHitTest = nHitTest;

	CSize size = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);
	m_rectRequestedSize = CRect(rect.TopLeft(), size);
	m_rectActualSize = CRect(rect.TopLeft(), size);
	m_rectActualFrameSize = CRect(rect.TopLeft(), size);

	// calculate frame rectangle
	AdjustWindowRectEx(&m_rectActualFrameSize, m_pBar->GetStyle(), FALSE, m_pBar->GetExStyle());

	m_rectFrameBorders = CRect(CPoint(0,0),
		m_rectActualFrameSize.Size() - m_rectActualSize.Size());

	// initialize tracking state and enter tracking loop
	m_dwOverDockStyle = 0;
	StretchDock(pt);   // call it here to handle special keys
	TrackDock();
}

void CMyDockContext::StretchDock( CPoint pt )
{
	CPoint ptOffset = pt - m_ptLast;

	// offset all drag rects to new position
	int nLength = 0;
	DWORD dwMode = LM_HORZ;
	if (m_nHitTest == HTLEFT || m_nHitTest == HTRIGHT)
	{
		if (m_nHitTest == HTLEFT)
			m_rectRequestedSize.left += ptOffset.x;
		else
			m_rectRequestedSize.right += ptOffset.x;
		nLength = m_rectRequestedSize.Width();
	}
	else
	{
		dwMode |= LM_LENGTHY;
		if (m_nHitTest == HTTOP)
			m_rectRequestedSize.top += ptOffset.y;
		else
			m_rectRequestedSize.bottom += ptOffset.y;
		nLength = m_rectRequestedSize.Height();
	}
	nLength = (nLength >= 0) ? nLength : 0;

	CSize size = m_pBar->CalcDynamicLayout(nLength, dwMode);

	CRect rectDesk;
	const int iSM_XVIRTUALSCREEN = 76;//SM_XVIRTUALSCREEN;
	const int iSM_YVIRTUALSCREEN = 77;//SM_YVIRTUALSCREEN;
	const int iSM_CXVIRTUALSCREEN = 78;//iSM_CXVIRTUALSCREEN;
	const int iSM_CYVIRTUALSCREEN = 79;//iSM_CYVIRTUALSCREEN;
	rectDesk.left=GetSystemMetrics(iSM_XVIRTUALSCREEN);
	rectDesk.top=GetSystemMetrics(iSM_YVIRTUALSCREEN);
	rectDesk.right=rectDesk.left+GetSystemMetrics(iSM_CXVIRTUALSCREEN);
	rectDesk.bottom=rectDesk.top+GetSystemMetrics(iSM_CYVIRTUALSCREEN);
	CRect rectTemp = m_rectActualFrameSize;

	if (m_nHitTest == HTLEFT || m_nHitTest == HTTOP)
	{
		rectTemp.left = rectTemp.right -
			(size.cx + m_rectFrameBorders.Width());
		rectTemp.top = rectTemp.bottom -
			(size.cy + m_rectFrameBorders.Height());
		CRect rect;
		if (rect.IntersectRect(rectDesk, rectTemp))
		{
			m_rectActualSize.left = m_rectActualSize.right - size.cx;
			m_rectActualSize.top = m_rectActualSize.bottom - size.cy;
			m_rectActualFrameSize.left = rectTemp.left;
			m_rectActualFrameSize.top = rectTemp.top;
		}
	}
	else
	{
		rectTemp.right = rectTemp.left +
			(size.cx + m_rectFrameBorders.Width());
		rectTemp.bottom = rectTemp.top +
			(size.cy + m_rectFrameBorders.Height());
		CRect rect;
		if (rect.IntersectRect(rectDesk, rectTemp))
		{
			m_rectActualSize.right = m_rectActualSize.left + size.cx;
			m_rectActualSize.bottom = m_rectActualSize.top + size.cy;
			m_rectActualFrameSize.right = rectTemp.right;
			m_rectActualFrameSize.bottom = rectTemp.bottom;
		}
	}
	m_ptLast = pt;

	// update feedback
	DrawResizeDockLine();
}

BOOL CMyDockContext::TrackDock()
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// set capture to the window which received this message
	m_pBar->SetCapture();
	ASSERT(m_pBar == CWnd::GetCapture());

	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == m_pBar)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		switch (msg.message)
		{
		case WM_LBUTTONUP:
			ASSERT(!m_bDragging);
			EndResizeDock();
			return TRUE;
		case WM_MOUSEMOVE:
			ASSERT(!m_bDragging);
			StretchDock(msg.pt);
			break;
		case WM_KEYDOWN:
			ASSERT(!m_bDragging);
			if (msg.wParam == VK_ESCAPE)
			{
				CancelLoopDock();
				return FALSE;
			}
			DispatchMessage(&msg);
			break;
		case WM_RBUTTONDOWN:
			CancelLoopDock();
			return FALSE;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	CancelLoopDock();

	return FALSE;
}

void CMyDockContext::DrawResizeDockLine( BOOL bRemove/*=FALSE*/ )
{
	ASSERT(m_pDC != NULL);
	if(NULL == m_pDC)
	{
		return;
	}
	CPen pen;
	pen.CreatePen(PS_DOT, 1, RGB(255,255,255));
	int iOldRop2 = m_pDC->SetROP2(R2_XORPEN);
	CPen *pOldPen = m_pDC->SelectObject(&pen);
	m_pDC->SetBkColor(RGB(0,0,0));

	// default to thin frame

	// determine new rect and size
	CRect rect;

	if (HORZF(m_dwOverDockStyle))
		rect = m_rectDragHorz;
	else if (VERTF(m_dwOverDockStyle))
		rect = m_rectDragVert;
	else
	{
		// use thick frame instead
		if ((HORZF(m_dwStyle) && !m_bFlip) || (VERTF(m_dwStyle) && m_bFlip))
			rect = m_rectFrameDragHorz;
		else
			rect = m_rectFrameDragVert;
	}

	// 绘制最后的
	if ( m_bDitherLast )
	{
		m_pDC->MoveTo(m_rectLast.TopLeft());
		m_pDC->LineTo(m_rectLast.right, m_rectLast.top);
	}
	
	// 绘制新的
	if ( !bRemove )
	{
		m_pDC->MoveTo(rect.TopLeft());
		m_pDC->LineTo(rect.right, rect.top);
		m_rectLast = rect;
		m_sizeLast = rect.Size();
		m_bDitherLast = TRUE;
	}

	m_pDC->SetROP2(iOldRop2);
	m_pDC->SelectObject(pOldPen);
}

void CMyDockContext::CancelLoopDock()
{
	DrawResizeDockLine(TRUE);    // gets rid of focus rect
	ReleaseCapture();

	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->UnlockWindowUpdate();
	if (m_pDC != NULL)
	{
		pWnd->ReleaseDC(m_pDC);
		m_pDC = NULL;
	}
}

void CMyDockContext::EndResizeDock()
{
	CancelLoopDock();

	m_pBar->CalcDynamicLayout(m_rectActualSize.Width(), LM_HORZ | LM_COMMIT);
	//m_pDockSite->ReDockControlBar(m_pBar, (CDockBar *)NULL, m_rectActualSize);
	m_pDockSite->RecalcLayout();
}

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNAMIC(CMyControlBar, CControlBar)

BEGIN_MESSAGE_MAP(CMyControlBar, CControlBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	
END_MESSAGE_MAP()

CMyControlBar::CMyControlBar()
{
	m_rcLeft.SetRectEmpty();

	m_sizeMRU = CSize(-1, -1);
	m_sizeResizeTmp = CSize(-1, -1);
	m_sizeDockMRU = CSize(-1, -1);

	m_sizeLastRestore = CSize(-1, -1);

	m_hOldCursor = NULL;

	m_sizeDefault = CSize(700, 80);
	
	m_bSetFixedSize = false;
	m_iFixedHeight = 0;
}

CMyControlBar::~CMyControlBar()
{
	m_hOldCursor = NULL;
}

void CMyControlBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

CSize CMyControlBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	CSize sizeParent(0, 0);
	CFrameWnd *pFrameParent = GetParentFrame();
	if ( NULL != pFrameParent )
	{
		CRect rc;
		pFrameParent->GetClientRect(rc);
		sizeParent = rc.Size();
		sizeParent.cy = m_sizeDefault.cy;
	}
	
	if (bStretch) // if not docked stretch to fit
		return CSize(bHorz ? 32767 : m_sizeDefault.cx,
		bHorz ? m_sizeDefault.cy : 32767);
	else
		return sizeParent;
}

void CMyControlBar::OnSize( UINT nType, int cx, int cy )
{

	m_rcLeft.top = 0;
	m_rcLeft.bottom = max(0, cy);
	m_rcLeft.left = 0;
	m_rcLeft.right = 50;


	CControlBar::OnSize(nType, cx, cy);
}

int CMyControlBar::OnCreate( LPCREATESTRUCT lpcs )
{
	int iRet = CControlBar::OnCreate(lpcs);
	if ( iRet >= 0 )
	{
		CMyDockContext *pDockContext = new CMyDockContext(this);
		pDockContext->EnableDrag(false);
		m_pDockContext = pDockContext;
	}
	return iRet;
}

BOOL CMyControlBar::Create( LPCTSTR lpszWindowName, DWORD dwStyle, CWnd* pParentWnd, UINT nId )
{
	m_dwStyle = dwStyle&CBRS_ALL;
	
	return CControlBar::Create(NULL, lpszWindowName, dwStyle, CRect(0,0,0,0), pParentWnd, nId);
}

void CMyControlBar::OnPaint()
{
	CPaintDC dc(this);
	OnEraseBkgnd(&dc);
}

CSize CMyControlBar::CalcDynamicLayout( int nLength, DWORD nMode )
{
	BOOL bStretch	= nMode&LM_STRETCH;
	BOOL bHorz		= nMode&LM_HORZ;
	CSize sizeParent(0, 0);
	CFrameWnd *pFrameParent = GetParentFrame();
	if ( NULL != pFrameParent )
	{
		CRect rc;
		pFrameParent->GetClientRect(rc);
		sizeParent = rc.Size();
	}

	if ( bStretch )
	{
		return CalcFixedLayout(bStretch, bHorz);
	}

	BOOL bFloating = IsFloating();
	if ( bFloating )
	{
		// 可以任意
		if ( nMode&LM_MRUWIDTH )
		{
			if ( m_sizeMRU.cx == -1 )
			{
				m_sizeMRU.cx = m_sizeDefault.cx;
			}
			if ( m_sizeMRU.cy == -1 )
			{
				m_sizeMRU.cy = m_sizeDefault.cy;
			}
			return m_sizeMRU;
		}
		else if ( nMode&LM_COMMIT )
		{
			m_sizeResizeTmp.cx = m_sizeResizeTmp.cx==-1? m_sizeDefault.cx : m_sizeResizeTmp.cx;
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;

			if ( nMode&LM_LENGTHY && nLength > 0 )
			{
				m_sizeResizeTmp.cy = nLength;
			}
			else if ( nLength > 0 )
			{
				m_sizeResizeTmp.cx = nLength;
			}

			m_sizeMRU = m_sizeResizeTmp;
			return m_sizeMRU;
		}
		else if ( nMode&LM_HORZDOCK )
		{
			CFrameWnd *pFrmTop = GetTopLevelFrame();
			if ( NULL != pFrmTop )
			{
				CRect rc;
				pFrmTop->GetClientRect(rc);
				sizeParent = rc.Size();
			}
			return CSize(sizeParent.cx, m_sizeDockMRU.cy==-1? m_sizeDefault.cy: m_sizeDockMRU.cy);
		}
		else if ( nLength > 0 )
		{
			if ( nMode&LM_LENGTHY )
			{
				nLength  = max(MIN_DYNAMIC_LENGTHY, nLength);	// 不能太小了，小了在sizeParent会出错
				m_sizeResizeTmp.cy = nLength;
			}
			else
			{
				nLength  = max(MIN_DYNAMIC_LENGTHX, nLength);	// 不能太小了，小了在sizeParent会出错
				m_sizeResizeTmp.cx = nLength;
			}
			m_sizeResizeTmp.cx = m_sizeResizeTmp.cx==-1? m_sizeDefault.cx : m_sizeResizeTmp.cx;
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;
			return m_sizeResizeTmp;
		}

		return m_sizeDefault;
	}
	else
	{
		// 宽度不可变，高度可变
		// 宽度不可变，高度可变
		if ( nMode&LM_COMMIT )
		{
			m_sizeResizeTmp.cy = m_sizeResizeTmp.cy==-1? m_sizeDefault.cy : m_sizeResizeTmp.cy;
			m_sizeDockMRU.cy   = m_sizeResizeTmp.cy;
			return CSize(sizeParent.cx, m_sizeResizeTmp.cy);
		}
		else if ( nLength >= 0 && (LM_LENGTHY&nMode) )
		{
			nLength = max(MIN_DYNAMIC_DOCK_LENGTHY, nLength);
			m_sizeResizeTmp.cy = nLength;
			return CSize(sizeParent.cx, nLength);
		}
		// lcq add 20131119
		if (m_bSetFixedSize)	// 设置固定高度
		{
			return CSize(sizeParent.cx, m_iFixedHeight);
		}

		return CSize(sizeParent.cx, m_sizeDockMRU.cy==-1? m_sizeDefault.cy: m_sizeDockMRU.cy);
	}
}

BOOL CMyControlBar::PreTranslateMessage( MSG* pMsg )
{
	if ( pMsg->message == WM_MOUSEMOVE )
	{
		if ( !IsMaximized() && PtInResizeSep(pMsg->pt) )
		{
			m_hOldCursor = SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}
	}
	else if ( pMsg->message == WM_LBUTTONDOWN && !IsMaximized() )
	{
		CRect rc(0,0,0,0);
		GetWindowRect(rc);
		rc.bottom = rc.top+4;
		if ( rc.PtInRect(pMsg->pt) )
		{			
			CControlBar* pBar = this;

			ASSERT(pBar);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL && pBar->m_pDockBar);

			// CBRS_SIZE_DYNAMIC toolbars cannot have the CBRS_FLOAT_MULTI style
			ASSERT((pBar->m_pDockBar->m_dwStyle & CBRS_FLOAT_MULTI) == 0);
			m_pDockContext->StartResize(HTTOP, pMsg->pt);

			SetCursor(m_hOldCursor);

			return TRUE;
		}

	}

	return CControlBar::PreTranslateMessage(pMsg);
}

BOOL CMyControlBar::PtInResizeSep( CPoint ptInScreen )
{
	CRect rc(0,0,0,0);
	GetWindowRect(rc);
	bool bTest = false;
	if ( !IsFloating() )
	{
		// 处在dock下, 
		if ( m_dwStyle&CBRS_SIZE_DYNAMIC )
		{
			if ( m_dwStyle&CBRS_ALIGN_BOTTOM )
			{
				// 顶部可调
				rc.bottom = rc.top+DOCK_RESIZE_LENGTH;
				bTest = true;
			}
			else if ( m_dwStyle&CBRS_ALIGN_TOP )
			{
				// 底部可调
				rc.top = rc.bottom-DOCK_RESIZE_LENGTH;
				bTest = true;
			}
		}
	}

	if ( bTest )
	{
		return rc.PtInRect(ptInScreen);
	}
	return FALSE;
}

void CMyControlBar::OnLButtonDblClk( UINT nFlags, CPoint pt )
{
	// only toggle docking if clicked in "void" space
	if (m_pDockBar != NULL && OnToolHitTest(pt, NULL) == -1)
	{
		ToggleMyDock();
	}
	else
	{
		CWnd::OnLButtonDblClk(nFlags, pt);
	}
}

BOOL CMyControlBar::OnEraseBkgnd( CDC* pDC )
{
	if ( NULL != pDC )
	{
// 		CDC &dc = *pDC;
// 
// 		CRect rc;
// 		GetClientRect(rc);
// 		
// 		COLORREF clrBk = GetSysColor(COLOR_WINDOW);
// 		dc.FillSolidRect(rc, clrBk);

// 		dc.FillSolidRect(rc, RGB(0,0,0));
// 
// 		dc.FillSolidRect(m_rcLeft, RGB(255, 0, 0));
// 
// 		dc.Draw3dRect(rc, RGB(0,127,0), RGB(0,255,0));
// 
// 		CPoint pt1;
// 		pt1 = rc.TopLeft();
// 		pt1.y += 4;
// 
// 		dc.SaveDC();
// 
// 		dc.SetBkColor(RGB(0,0,0));
// 
// 		CPen penSolid, penDot, penDash, penDashDot, penDashDD;
// 		penSolid.CreatePen(PS_SOLID, 1, RGB(255,255,255));
// 		penDot.CreatePen(PS_DOT, 1, RGB(255,255,255));
// 		penDash.CreatePen(PS_DASH, 1, RGB(255,255,255));
// 		penDashDot.CreatePen(PS_DASHDOT, 1, RGB(255,255,255));
// 		penDashDD.CreatePen(PS_DASHDOTDOT, 1, RGB(255,255,255));
// 
// 		const int iStep = 5;
// 
// 
// 
// 		dc.SelectObject(&penSolid);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDot);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDash);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDashDot);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDashDD);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 
// 
// 		dc.SetROP2(R2_XORPEN);
// 		pt1.y += iStep*3;
// 
// 
// 		dc.SelectObject(&penSolid);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDot);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep; 
// 
// 		dc.SelectObject(&penDash);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDashDot);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 		dc.SelectObject(&penDashDD);
// 		dc.MoveTo(pt1.x, pt1.y);
// 		dc.LineTo(rc.right, pt1.y);
// 		pt1.y += iStep;
// 
// 
// 		dc.RestoreDC(-1);

//		return TRUE;
	}
	return FALSE;
}

BOOL CMyControlBar::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CPoint pt(0,0);
	GetCursorPos(&pt);
	if ( PtInResizeSep(pt) )
	{
		return TRUE;
	}
	return CControlBar::OnSetCursor(pWnd, nHitTest, message);
}

void CMyControlBar::SetSizeDefault( CSize sizeDef )
{
	if ( m_sizeDefault != sizeDef )
	{
		m_sizeDefault = sizeDef;
	}
}

CSize CMyControlBar::GetCurrentSize()
{
	if ( NULL != m_hWnd )
	{
		CRect rc(0,0,0,0);
		GetClientRect(rc);
		return rc.Size();
	}
	return m_sizeDefault;
}

void CMyControlBar::SetNewSize( CSize sizeNew )
{
	if ( sizeNew.cx < 0 || sizeNew.cy < 0 )
	{
		ASSERT( 0 );
		return;
	}

	if ( NULL != m_hWnd )
	{
		if ( IsFloating() )
		{
			// 浮动时
			m_sizeMRU = sizeNew;
		}
		else
		{
			// 停靠
			m_sizeDockMRU = sizeNew;
		}
		if ( IsWindowVisible() )
		{
			if ( NULL!=m_pDockSite )
			{
				CFrameWnd *pFrame = GetParentFrame();
				if ( NULL!=pFrame )
				{
					pFrame->RecalcLayout();
				}
				m_pDockSite->RecalcLayout();
			}
			else
			{
				CFrameWnd *pFrame = GetParentFrame();
				if ( NULL!=pFrame )
				{
					pFrame->RecalcLayout();
				}
				else
				{
					ASSERT( 0 );	// 没辙
				}
			}
		}
	}
	else
	{
		SetSizeDefault(sizeNew);
	}
}


BOOL CMyControlBar::Maximize()
{
	CMDIFrameWnd *pFrame = GetParentMDIFrame();
	if ( NULL == pFrame )
	{
		return FALSE;
	}

	if ( IsFloating() )
	{
		return FALSE;	// 浮动下，禁止最大化
	}

	CRect rcClient(0,0,0,0);
	::GetClientRect(pFrame->m_hWndMDIClient, &rcClient);
	if ( rcClient.Height() > 0 )
	{
		CSize sizeNow = GetCurrentSize();
		if ( m_sizeLastRestore.cy <= 0 )
		{
			m_sizeLastRestore = sizeNow;
		}
		//sizeNow.cy += rcClient.Height();
		//sizeNow.cy += 4;
		sizeNow.cy = 32767;
		if ( IsMinimized() )
		{
			RestoreSize();
		}
		SetNewSize(sizeNow);
	}
	return TRUE;
}


BOOL CMyControlBar::IsMaximized()
{
	CMDIFrameWnd *pFrame = GetParentMDIFrame();
	if ( NULL == pFrame )
	{
		return FALSE;
	}
	return m_sizeLastRestore.cy > 0;	// 留点空隙？？
}

BOOL CMyControlBar::RestoreSize()
{
	BOOL bMin = IsMinimized();
	BOOL bRet = FALSE;
	if ( bMin )
	{
		if ( NULL!=m_pDockSite )
		{
			m_pDockSite->ShowControlBar(this, TRUE, FALSE);
			bRet = TRUE;
		}
	}

	BOOL bMax = IsMaximized();
	if ( bMax && m_sizeLastRestore.cy > 0 )
	{
		CSize sizeNow = GetCurrentSize();
		sizeNow.cy = m_sizeLastRestore.cy;
		SetNewSize(sizeNow);
		m_sizeLastRestore = CSize(-1,-1);
		return TRUE;
	}
	return bRet;
}

BOOL CMyControlBar::Minimize()
{
	if ( NULL != m_pDockSite )
	{
		m_pDockSite->ShowControlBar(this, FALSE, FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CMyControlBar::IsMinimized()
{
	if ( NULL != m_pDockSite )
	{
		return !IsWindowVisible();
	}
	return FALSE;
}

void CMyControlBar::SetFixedHeight(bool bFiexd,const int iHeight)	// 设置固定大小(高度)
{
	m_iFixedHeight = iHeight;
	m_bSetFixedSize = bFiexd;
}

CMDIFrameWnd *CMyControlBar::GetParentMDIFrame()
{
	if (GetSafeHwnd() == NULL) // no Window attached
	{
		return NULL;
	}

	ASSERT(this);

	CWnd* pParentWnd = GetParent();  // start with one parent up
	while (pParentWnd != NULL)
	{
		if (pParentWnd->IsFrameWnd()
			&& pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
		{
			return (CMDIFrameWnd*)pParentWnd;
		}
		pParentWnd = pParentWnd->GetParent();
	}
	return NULL;
}

void CMyControlBar::ToggleMyDock()
{
	if ( IsMaximized() && !IsFloating() )
	{
		return;	// 最大化停靠下禁止浮动
	}
	// start the drag
	ASSERT(m_pDockContext != NULL);
	if ( !IsFloating()
		&& (m_pDockContext->m_ptMRUFloatPos.x < 0
		|| m_pDockContext->m_ptMRUFloatPos.y < 0) )
	{
		CRect rcFrame(0,0,0,0);
		CFrameWnd *pFrmTop = GetTopLevelFrame();
		if ( NULL != pFrmTop )
		{
			pFrmTop->GetWindowRect(&rcFrame);

			CPoint ptCenter = rcFrame.CenterPoint();
			ptCenter.x -= m_sizeDefault.cx/2;
			ptCenter.y -= m_sizeDefault.cy/2;
			m_pDockContext->m_ptMRUFloatPos = ptCenter;
		}
	}
	m_pDockContext->ToggleDocking();
}

//////////////////////////////////////////////////////////////////////////
//
CSize CMyDockBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	ASSERT(this);

	CSize sizeFixed = CControlBar::CalcFixedLayout(bStretch, bHorz);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
	{
		CRect rect = m_rectLayout;
		CalcInsideRect(rect, bHorz);
		sizeMax = rect.Size();
	}
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame = GetParentFrame();
		pFrame->GetClientRect(&rectFrame);
		sizeMax = rectFrame.Size();
	}

	// prepare for layout
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos((int)m_arrBars.GetSize());
	//CPoint pt(-afxData.cxBorder2, -afxData.cyBorder2);
	CPoint pt(0, 0);
	int nWidth = 0;

	BOOL bWrapped = FALSE;

	// layout all the control bars
	for (int nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);		
		void* pVoid = m_arrBars[nPos];

		if (pBar != NULL)
		{
			if (pBar->IsVisible())
			{
				// get ideal rect for bar
				DWORD dwMode = 0;
				if ((pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
					(pBar->m_dwStyle & CBRS_FLOATING))
					dwMode |= LM_HORZ | LM_MRUWIDTH;
				else if (pBar->m_dwStyle & CBRS_ORIENT_HORZ)
					dwMode |= LM_HORZ | LM_HORZDOCK;
				else
					dwMode |=  LM_VERTDOCK;

				CSize sizeBar = pBar->CalcDynamicLayout(-1, dwMode);

				// 不允许超过最大尺寸
				sizeBar.cx = min(sizeBar.cx, sizeMax.cx);
				sizeBar.cy = min(sizeBar.cy, sizeMax.cy);

				CRect rect(pt, sizeBar);

				// get current rect for bar
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);

				if (bHorz)
				{
					sizeMax.cx -= sizeBar.cx;		// 不允许超过最大尺寸

					// Offset Calculated Rect out to Actual
					if (rectBar.left > rect.left && !m_bFloating)
						rect.OffsetRect(rectBar.left - rect.left, 0);

					// If ControlBar goes off the right, then right justify
					if (rect.right > sizeMax.cx && !m_bFloating)
					{
						//int x = rect.Width() - afxData.cxBorder2;
						int x = rect.Width();
						x = max(sizeMax.cx - x, pt.x);
						rect.OffsetRect(x - rect.left, 0);
					}

					// If ControlBar has been wrapped, then left justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						//rect.OffsetRect(-(rect.left + afxData.cxBorder2), 0);
						rect.OffsetRect(-(rect.left), 0);
					}
					// If ControlBar is completely invisible, then wrap it
					//else if ((rect.left >= (sizeMax.cx - afxData.cxBorder2)) &&
					else if ((rect.left >= (sizeMax.cx)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (NULL != pBar)
							{
								if (!m_bLayoutQuery &&
									!(pBar->m_dwStyle & CBRS_FLOATING))
								{
									pBar->m_pDockContext->m_rectMRUDockPos = rect;
								}
								AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
							}
							
						}
						//pt.x = rect.left + sizeBar.cx - afxData.cxBorder2;
						pt.x = rect.left + sizeBar.cx;
						nWidth = max(nWidth, sizeBar.cy);
					}
				}
				else
				{
					sizeMax.cy -= sizeBar.cy; // 不允许超过最大尺寸

					// Offset Calculated Rect out to Actual
					if (rectBar.top > rect.top && !m_bFloating)
						rect.OffsetRect(0, rectBar.top - rect.top);

					// If ControlBar goes off the bottom, then bottom justify
					if (rect.bottom > sizeMax.cy && !m_bFloating)
					{
						//int y = rect.Height() - afxData.cyBorder2;
						int y = rect.Height();
						y = max(sizeMax.cy - y, pt.y);
						rect.OffsetRect(0, y - rect.top);
					}

					// If ControlBar has been wrapped, then top justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						//rect.OffsetRect(0, -(rect.top + afxData.cyBorder2));
						rect.OffsetRect(0, -(rect.top));
					}
					// If ControlBar is completely invisible, then wrap it
					//else if ((rect.top >= (sizeMax.cy - afxData.cyBorder2)) &&
					else if ((rect.top >= (sizeMax.cy)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (NULL != pBar)
							{
								if (!m_bLayoutQuery &&
									!(pBar->m_dwStyle & CBRS_FLOATING))
								{
									pBar->m_pDockContext->m_rectMRUDockPos = rect;
								}
								AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
							}
							
						}
						//pt.y = rect.top + sizeBar.cy - afxData.cyBorder2;
						pt.y = rect.top + sizeBar.cy;
						nWidth = max(nWidth, sizeBar.cx);
					}
				}
			}
			if (!bWrapped && NULL != pBar)
			{
				// handle any delay/show hide for the bar
				pBar->RecalcDelayShow(&layout);
			}

			// 每一个窗口就是一行
			pBar = NULL;
			pVoid = NULL;
		}
		if (pBar == NULL && pVoid == NULL && nWidth != 0)
		{
			// end of row because pBar == NULL
			if (bHorz)
			{
				//pt.y += nWidth - afxData.cyBorder2;
				pt.y += nWidth;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				//pt.x = -afxData.cxBorder2;
				pt.x = 0;
			}
			else
			{
				//pt.x += nWidth - afxData.cxBorder2;
				pt.x += nWidth;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				//pt.y = -afxData.cyBorder2;
				pt.y = 0;
			}
			nWidth = 0;
		}
	}
	if (!m_bLayoutQuery)
	{
		// move and resize all the windows at once!
		if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
		{
			TRACE(_T("Warning: DeferWindowPos failed - low system resources.\n"));
			//TRACE(traceAppMsg, 0, "Warning: DeferWindowPos failed - low system resources.\n");
		}
	}

	// adjust size for borders on the dock bar itself
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect, bHorz);

	if ((!bStretch || !bHorz) && sizeFixed.cx != 0)
		sizeFixed.cx += -rect.right + rect.left;
	if ((!bStretch || bHorz) && sizeFixed.cy != 0)
		sizeFixed.cy += -rect.bottom + rect.top;

	return sizeFixed;
}


//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNCREATE(CMyMiniDockFrameWnd, CMiniDockFrameWnd)

CMyMiniDockFrameWnd::CMyMiniDockFrameWnd()
: CMiniDockFrameWnd()
{
	ASSERT( sizeof(CMyDockBar) == sizeof(CDockBar) );
	//CMyDockBar myDock(TRUE);
	//int iSize = min(sizeof(CMyDockBar), sizeof(CDockBar));
	//memcpy(&m_wndDockBar, &myDock, iSize);
}

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNAMIC(CMyContainerBar, CMyControlBar)

BEGIN_MESSAGE_MAP(CMyContainerBar, CMyControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CMyContainerBar::CMyContainerBar()
{
	m_iCur = -1;
}

CMyContainerBar::~CMyContainerBar()
{

}

CWnd		* CMyContainerBar::GetCurWin()
{
	if ( m_iCur >=0 && m_iCur < m_aWndPtrs.GetSize() )
	{
		return m_aWndPtrs[m_iCur];
	}
	return NULL;
}

void CMyContainerBar::AddWin( CWnd *pWnd )
{
	ASSERT( NULL!=pWnd );
	if ( NULL == pWnd  )
	{
		return;
	}

	int iExist = -1;
	for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
	{
		if ( pWnd == m_aWndPtrs[i] )
		{
			ASSERT( pWnd->GetSafeHwnd() == m_aWndPtrs[i]->GetSafeHwnd() );
			iExist = i;
			break;
		}
	}

	pWnd->ShowWindow(SW_HIDE);
	if ( iExist == -1 )
	{
		m_aWndPtrs.Add(pWnd);
		iExist = m_aWndPtrs.GetSize()-1;
	}
	if ( NULL != GetSafeHwnd() )
	{
		pWnd->SetParent(this);
		UINT uUIState = SendMessage(0x0129/*WM_QUERYUISTATE*/);
		SendMessage(0x0127/*WM_CHANGEUISTATE*/, MAKELONG(1/*UIS_SET*/, uUIState));
		pWnd->SendMessage(0x0127/*WM_CHANGEUISTATE*/, MAKELONG(1/*UIS_SET*/, uUIState));
	}
	AdjustSubWinRect(pWnd);
}

void CMyContainerBar::RemoveWin( CWnd *pWnd )
{
	int iDel = -1;
	for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
	{
		if ( m_aWndPtrs[i] == pWnd )
		{
			iDel = i;
			break;
		}
	}
	if ( iDel == -1 )
	{
		return;
	}
	if ( m_iCur == iDel )
	{
		m_iCur = -1;
	}
	m_aWndPtrs.RemoveAt(iDel);
}

int CMyContainerBar::SetCurWin( CWnd *pWnd )
{
	if ( NULL == pWnd )
	{
		// 隐藏所有窗口
		for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
		{
			CWnd *pWin = m_aWndPtrs[i];
			if (  NULL != pWin )
			{
				pWin->ShowWindow(SW_HIDE);
			}
		}
		m_iCur = -1;
		return -1;
	}

	int iCur = -1;
	for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
	{
		if ( m_aWndPtrs[i] == pWnd )
		{
			iCur = i;
			break;
		}
	}
	if ( iCur == -1 )
	{
		return -1;
	}
	if ( iCur == m_iCur )
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->BringWindowToTop();
		return m_iCur;
	}
	CWnd *pCurWin = GetCurWin();
	if ( NULL != pCurWin )
	{
		pCurWin->ShowWindow(SW_HIDE);
	}

	pWnd->ShowWindow(SW_SHOW);
	pWnd->BringWindowToTop();
	
	m_iCur = iCur;
	return m_iCur;
}

void CMyContainerBar::AdjustSubWinRect( CWnd *pWndSub, LPRECT pRect )
{
	ASSERT( NULL != pWndSub );

	if(NULL != pWndSub)
	{
		CRect rc(0,0,0,0);
		if ( NULL!=pRect )
		{
			rc = *pRect;
		}
		else if ( NULL != GetSafeHwnd() )
		{
			GetClientRect(&rc);
			rc.top += DOCK_RESIZE_LENGTH;
			if ( rc.bottom < rc.top )
			{
				rc.bottom = rc.top;
			}
		}
		pWndSub->MoveWindow(rc);
	}
	
}

int CMyContainerBar::OnCreate( LPCREATESTRUCT lpcs )
{
	int iRet = CMyControlBar::OnCreate(lpcs);
	if ( iRet >= 0 )
	{
		for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
		{
			CWnd *pWin = m_aWndPtrs[i];
			if (  NULL != pWin )
			{
				pWin->SetParent(this);
			}
		}
	}
	return iRet;
}

void CMyContainerBar::OnSize( UINT nType, int cx, int cy )
{
	CMyControlBar::OnSize(nType, cx, cy);
	CRect rc(0, 0, cx, cy);
	if ( !(m_dwStyle&CBRS_FLOATING) )
	{
		// 多加一个边框
		rc.top += DOCK_RESIZE_LENGTH;
		if ( rc.bottom < rc.top )
		{
			rc.bottom = rc.top;
		}
	}
	for ( int i=0; i < m_aWndPtrs.GetSize() ; ++i )
	{
		CWnd *pWin = m_aWndPtrs[i];
		if (  NULL != pWin )
		{
			AdjustSubWinRect(pWin, &rc);
		}
	}
}

void CMyContainerBar::OnPaint()
{
	CPaintDC dc(this);
	OnEraseBkgnd(&dc);

	CRect rc(0,0,0,0);
	GetClientRect(rc);
	rc.bottom = rc.top + DOCK_RESIZE_LENGTH;

	dc.FillSolidRect(rc.left, rc.top, rc.Width(), 1, RGB(235,235,235));
	dc.FillSolidRect(rc.left, rc.top+1, rc.Width(), 1, RGB(255,255,255));
	dc.FillSolidRect(rc.left, rc.top+2, rc.Width(), 1, RGB(235,235,235));
	dc.FillSolidRect(rc.left, rc.top+3, rc.Width(), 1, RGB(150,150,150));
}

BOOL CMyContainerBar::OnEraseBkgnd( CDC* pDC )
{
	// NULL
	return GetCurWin()==NULL ? FALSE : TRUE;
}


#include "StdAfx.h"
#include "DlgStatic.h"

#include "MemPartDC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDlgStatic, CStatic)

BEGIN_MESSAGE_MAP(CDlgStatic, CStatic)
	//{{AFX_MSG_MAP(CDlgStatic)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDlgStatic::CDlgStatic()
:CStatic()
{
	m_bTransparentDrawText = true;

	m_crText = 0x17fb12;//RGB(255,255,255);//GetSysColor(COLOR_WINDOWTEXT);
	m_hBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);
	
	memset(&m_lf, 0, sizeof(m_lf));

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	m_dwOldSSStyle = 0;
}

CDlgStatic::~CDlgStatic()
{
	if ( m_font.GetSafeHandle()!=NULL )
	{
		m_font.DeleteObject();
	}
	::DeleteObject(m_hBrush);
}

void CDlgStatic::PostNcDestroy()
{
	delete this;
}


void CDlgStatic::PreSubclassWindow()
{
	UINT nBS = GetStyle();
    //  the button should not be owner draw
    ASSERT((nBS & SS_TYPEMASK) != SS_OWNERDRAW);
	m_dwOldSSStyle = nBS;

	// Switch to owner-draw
	CStatic::PreSubclassWindow();
    ModifyStyle(0, SS_OWNERDRAW, SWP_FRAMECHANGED);
	Invalidate();
}

void CDlgStatic::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	ASSERT( lpDrawItemStruct );
	if (NULL == lpDrawItemStruct)
	{
		return;
	}
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CMemSwapDC dcMem;
	dcMem.CreateSwapDC(&dc, lpDrawItemStruct->rcItem, lpDrawItemStruct->rcItem);
	
	CWnd *pWndParent = GetParent();
	ASSERT( pWndParent );
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	if ( pWndParent )
	{
		hBrush = (HBRUSH)::SendMessage(pWndParent->GetSafeHwnd(), WM_CTLCOLORSTATIC, (WPARAM)dcMem.m_hDC, (LPARAM)m_hWnd);
	}
	
	dcMem.SelectObject(GetFont());	

	
	DWORD dwTextAlign = DT_LEFT;
	if ( m_dwOldSSStyle & SS_RIGHT )
	{
		dwTextAlign = DT_RIGHT;
	}
	else if ( m_dwOldSSStyle & SS_CENTER )
	{
		dwTextAlign = DT_CENTER;
	}

	if ( m_dwOldSSStyle&SS_CENTERIMAGE )
	{
		dwTextAlign |= DT_VCENTER;
	}
	
	CString StrTitle;
	GetWindowText(StrTitle);
	CRect rc;
	GetClientRect(rc);
	
	dcMem.SetBkMode(TRANSPARENT);
	if ( !m_bTransparentDrawText )
	{
		FillRect(dcMem.m_hDC, rc, hBrush);
	}
	else
	{
		// ÈÃ¸¸´°¿Ú»­±³¾°
		DrawTransparentBk(&dcMem);
	}
	
	CFont *pFont, *pOldFont;
	pFont = &m_font;//GetFont();
	if ( pFont->GetSafeHandle()==NULL )
	{
		pFont = GetFont();
	}
	pOldFont = NULL;
	if ( NULL != pFont )
	{
		pOldFont = dcMem.SelectObject(pFont);
	}
 
	dcMem.DrawText(StrTitle, rc, dwTextAlign | DT_SINGLELINE);
	
	if( pOldFont )
	{
		dcMem.SelectObject(pOldFont);
	} 
	dcMem.Swap(&dc);

	dc.Detach();
}

void CDlgStatic::DrawTransparentBk( CDC *pDC )
{
	CWnd *pWndParent = GetParent();
	ASSERT( pWndParent != NULL );
	ASSERT( pDC != NULL );
	if (NULL == pWndParent || NULL == pDC)
	{
		return;
	}
	CRect rcClient;
	GetClientRect(rcClient);
	MapWindowPoints(pWndParent, &rcClient);
	
	CRect rcParent;
	pWndParent->GetClientRect(rcParent);
	
	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect(rcClient);
	
	CDC *pDCParent = pWndParent->GetDC();
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDCParent);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDCParent, rcParent.Width(), rcParent.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);
	
	dcMem.SelectClipRgn(&rgnClip);
	pWndParent->SendMessage(WM_ERASEBKGND, (WPARAM)dcMem.m_hDC);
	
	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, rcClient.left, rcClient.top, SRCCOPY);
	
	dcMem.SelectObject(pBmpOld);
	pWndParent->ReleaseDC(pDCParent);
	rgnClip.DeleteObject();
	bmp.DeleteObject();
	dcMem.DeleteDC();
}


void CDlgStatic::SetTextFont(int nHeight, LPCTSTR fontName, BOOL bRedraw )
{
	if ( m_font.GetSafeHandle()!=NULL )
	{
		m_font.DeleteObject();
	}
	
	nHeight*=-1;
	m_lf.lfHeight = nHeight;
	wcscpy_s(m_lf.lfFaceName, sizeof(m_lf.lfFaceName)/sizeof(m_lf.lfFaceName[0]),fontName);
	BOOL bRet = m_font.CreateFontIndirect(&m_lf);
	ASSERT(bRet);
	if( !bRet )
	{
		m_font.CreateFontIndirect(&m_lf);
	}
	if( bRedraw && GetSafeHwnd())
	{
		Invalidate();
	}
}

HBRUSH CDlgStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_crText);
	
	if ( NULL!=m_font.GetSafeHandle() )
	{
		pDC->SelectObject(&m_font);
	}
	
	if(m_bTransparentDrawText)
	{
		return   (HBRUSH)::GetStockObject(NULL_BRUSH);   
	}
	else
	{
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO: Return a non-NULL brush if the parent's handler should not be called 	return NULL;
}

void CDlgStatic::SetMyTextColor( COLORREF clr )
{
	m_crText = clr;
}

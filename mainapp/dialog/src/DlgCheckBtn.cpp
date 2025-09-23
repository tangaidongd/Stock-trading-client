#include "StdAfx.h"
#include "DlgCheckBtn.h"
#include "ColorStep.h"
#include "DIBSectionLite.h"

#include "GdiPlusTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDlgCheckBtn, BaseButton)

BEGIN_MESSAGE_MAP(CDlgCheckBtn, BaseButton)
//{{AFX_MSG_MAP(CDlgCheckBtn)
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
//ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
//ON_CONTROL_REFLECT_EX(BN_DBLCLK,  OnClicked)
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_MESSAGE(BM_SETCHECK, OnSetCheck)
ON_MESSAGE(BM_GETCHECK, OnGetCheck)
ON_WM_ERASEBKGND()
ON_WM_SETFOCUS()
ON_WM_KILLFOCUS()
ON_WM_CTLCOLOR_REFLECT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CDlgCheckBtn::CDlgCheckBtn()
:BaseButton()
{
	m_bMouseOver = false;
	m_bTransparentDrawText = false;
	m_iCheck = BST_UNCHECKED;
	m_bDrawedFocus = false;
	m_bLButtonDown = false;

	m_pImageUnCheck = NULL;
	m_pImageCheck = NULL;

	m_clrTextColor = GetSysColor(COLOR_BTNTEXT);

	m_font.DeleteObject();
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(m_lf),&m_lf);
	//m_font.CreateFontIndirect(&m_lf);
}

CDlgCheckBtn::~CDlgCheckBtn()
{
	if( m_pImageCheck )
		delete m_pImageCheck;
	m_pImageCheck = NULL;

	if( m_pImageUnCheck )
		delete m_pImageUnCheck;
	m_pImageUnCheck = NULL;

	m_font.DeleteObject();
}




void CDlgCheckBtn::PostNcDestroy()
{
	delete this;
}

void CDlgCheckBtn::OnMouseMove( UINT nFlags, CPoint point )
{
	if ( !(GetStyle() & BS_CHECKBOX) )
	{
		BaseButton::OnMouseMove(nFlags, point);
		return;
	}


	TRACKMOUSEEVENT tme = {0};
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags   = TME_LEAVE;
	_TrackMouseEvent(&tme);

	CRect rc;
	GetClientRect(rc);
	if ( rc.PtInRect(point) )
	{
		if ( !m_bMouseOver )
		{
			// 第一次进来
			m_bMouseOver = true;
			Invalidate();
		}
	}
	else
	{
		// 外面去了
		if ( m_bMouseOver )
		{
			m_bMouseOver = false;
			Invalidate();
		}
	}

	//BaseButton::OnMouseMove(nFlags, point);
}

LRESULT CDlgCheckBtn::OnMouseLeave( WPARAM w, LPARAM l )
{
	if ( !(GetStyle() & BS_CHECKBOX) )
	{
		return Default();
	}

	bool32 bOld  = m_bMouseOver;
	m_bMouseOver = false;
	if ( bOld )
	{
		Invalidate();
	}
	return Default();	
}

void CDlgCheckBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	DWORD dwStyle = GetStyle();
	if ( dwStyle & BS_CHECKBOX )
	{
		// 只绘制checkBox
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CString StrTitle;
		GetWindowText(StrTitle);
		CRect rc(lpDrawItemStruct->rcItem);

// 		CClientDC dc(this);
// 		GetClientRect(rc);
		
		HBRUSH hBrush = (HBRUSH)::SendMessage(GetParent()->GetSafeHwnd(), WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd);
		
		if ( m_bTransparentDrawText )
		{
			// 让父窗口画背景
			DrawTransparentBk(&dc);
		}
		else
		{
			// 画个背景小框
			HPEN hOldPen = (HPEN)SelectObject(dc.m_hDC, (HPEN)GetStockObject(NULL_PEN));
			FillRect(dc.m_hDC, rc, hBrush);
			SelectObject(dc.m_hDC, hOldPen);
		}

		//TRACE(_T("State: %d\r\n"), lpDrawItemStruct->itemState);
		
		CFont *pFont, *pOldFont;
		pFont = &m_font;//GetFont();
		if ( NULL==pFont->GetSafeHandle() )
		{
			pFont = GetFont();
		}
		pOldFont = NULL;
		if ( NULL != pFont )
		{
			pOldFont = dc.SelectObject(pFont);
		}

		COLORREF clrSign = RGB(33,161,33); // √的颜色

		// 只支持画左边小框
		int ndel = 13;
		CRect rcCheck(0,0,ndel,ndel);
		if( m_pImageCheck )
		{
			rcCheck = CRect(0,0,m_pImageCheck->GetWidth(),m_pImageCheck->GetHeight());
			ndel = m_pImageCheck->GetHeight();
		}
		if ( rc.Height() - rcCheck.Height() >= 0 )
		{
			rcCheck.top = rc.top + (rc.Height()-rcCheck.Height())/2;
			rcCheck.bottom = rcCheck.top + ndel;
		}
		if( m_pImageUnCheck )
			CDIBSectionLite::DrawImageGp(&dc,m_pImageUnCheck,rcCheck);

		COLORREF clrLeftRect = lpDrawItemStruct->itemState & ODS_DISABLED ? GetSysColor(COLOR_GRAYTEXT) : RGB(127,127,127);
		dc.Draw3dRect(rcCheck, clrLeftRect, clrLeftRect);

		// 内部小框框
		if ( lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			// 禁止状态下
			CRect rcInner(rcCheck);
			rcInner.InflateRect(-1, -1);
			FillRect(dc.m_hDC, rcInner, (HBRUSH)GetStockObject(WHITE_BRUSH));

			// 改变字体颜色和√颜色
			dc.SetTextColor(clrLeftRect);
			clrSign = clrLeftRect;
		}
		else
		{
			bool32 bMousePress = lpDrawItemStruct->itemState & ODS_SELECTED;
			CRect rcInner(rcCheck);
			rcInner.InflateRect(-1, -1);
			CColorRayStep step;
			if ( bMousePress )
			{
				step.InitColorRayRange(RGB(255,255,255), RGB(180,180,180), 16, rcInner.BottomRight());
			}
			else
			{
				step.InitColorRayRange(RGB(255,255,255), RGB(222,222,222), 16, rcInner.BottomRight());
			}
			for ( int i=rcInner.left; i < rcInner.right; i++ )
			{
				for ( int j=rcInner.top; j < rcInner.bottom; j++ )
				{
					dc.SetPixel(i, j, step.GetColor(i, j));
				}
			}

			
			if ( bMousePress )
			{
				// 鼠标压下 - 不需要画其它框了
			}
			else if ( m_bMouseOver )
			{
				// 黄色小框代表鼠标移动到上面来了
				// 难道用SetPixel	
				int32 iRadius = 16; 
				CColorRayStep clrStep;
				clrStep.InitColorRayRange(RGB(255,240,207), RGB(248,170,40), iRadius, rcInner.TopLeft());
				CRect rcNull(rcInner);
				rcNull.InflateRect(-2,-2);
				for ( int32 i=rcInner.left; i < rcInner.right; i++ )
				{
					for ( int32 j=rcInner.top; j < rcInner.bottom; j++ )
					{
						if ( !rcNull.PtInRect(CPoint(i, j)) )
						{
							dc.SetPixel(i, j, clrStep.GetColor(i,j));
						}
					}
				}
			}
		}

		// √，画的实在不像
		if ( GetCheck() == BST_CHECKED )
		{
			if( m_pImageCheck )
				CDIBSectionLite::DrawImageGp(&dc,m_pImageCheck,rcCheck);
			else
			{
				//1/2 -> 1/3 -> 0
				CRect rcSign(rcCheck);
				rcSign.InflateRect(-3,-3);
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, clrSign);
				CPen *pOldPen = dc.SelectObject(&pen);
				int32 iDivide = 3;
				dc.MoveTo(rcSign.left,			rcSign.bottom - iDivide);
				dc.LineTo(rcSign.left +iDivide, rcSign.bottom);
				dc.MoveTo(rcSign.left +iDivide-1, rcSign.bottom-1);
				dc.LineTo(rcSign.right,			rcSign.top+1);
				
				dc.MoveTo(rcSign.left,			rcSign.bottom - iDivide -1);
				dc.LineTo(rcSign.left +iDivide, rcSign.bottom-1);
				dc.MoveTo(rcSign.left +iDivide-1, rcSign.bottom-2);
				dc.LineTo(rcSign.right,			rcSign.top);
				
				dc.MoveTo(rcSign.left,			rcSign.bottom - iDivide -2);
				dc.LineTo(rcSign.left +iDivide, rcSign.bottom-2);
				dc.MoveTo(rcSign.left +iDivide-1, rcSign.bottom-3);
				dc.LineTo(rcSign.right,			rcSign.top-1);
				
				// 			dc.MoveTo(rcSign.right,			rcSign.top );
				// 			dc.LineTo(rcSign.left +iDivide-0, rcSign.bottom-2);
				// 			dc.MoveTo(rcSign.right,			rcSign.top +1);
				// 			dc.LineTo(rcSign.left +iDivide-0, rcSign.bottom-1);
				// 			dc.MoveTo(rcSign.right,			rcSign.top +2);
				// 			dc.LineTo(rcSign.left +iDivide-1, rcSign.bottom);
				//dc.LineTo(rcSign.right, rcSign.top);
				dc.SelectObject(pOldPen);
			}
		}

		

		rc.left = rcCheck.right + 3;
		
		dc.SetBkMode(TRANSPARENT);
		COLORREF clrText = m_clrTextColor;
		if ( lpDrawItemStruct->itemState & ODS_DISABLED )
		{
			clrText = GetSysColor(COLOR_GRAYTEXT);
		}
		dc.SetTextColor(clrText);
		int iTextH = dc.DrawText(StrTitle, rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		// 还要画Focus不
		if ( iTextH > 0)
		{
			CRect rcText(rc);
			CSize sizeText = dc.GetOutputTextExtent(StrTitle);
			rcText.bottom = rc.top + iTextH;
			rcText.top = rcText.bottom - sizeText.cy;
			rcText.right = rcText.left + sizeText.cx;
			rcText.InflateRect(1,1);
			
			if ( lpDrawItemStruct->itemState & ODS_FOCUS /*&& !m_bDrawedFocus*/ )
			{
				dc.DrawFocusRect(rcText);
				m_bDrawedFocus = true;
			}
// 			else if ( !(lpDrawItemStruct->itemState & ODS_FOCUS) && m_bDrawedFocus )
// 			{
// 				//dc.DrawFocusRect(rcText);
// 				//m_bDrawedFocus = false;
// 			}
		}

		if ( NULL != pOldFont )
		{
			dc.SelectObject(pFont);
		}
		dc.Detach();
	}
	else
	{
		BaseButton::DrawItem(lpDrawItemStruct);
	}
}

void CDlgCheckBtn::PreSubclassWindow()
{
	UINT nBS = GetButtonStyle();
	
    //  the button should not be owner draw
    ASSERT((nBS & SS_TYPEMASK) != BS_OWNERDRAW);
	
    //  This class supports only check boxes
    ASSERT(nBS & BS_CHECKBOX);
	
	SetCheck(GetCheck());

    // Switch to owner-draw
    ModifyStyle(SS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);

	
    BaseButton::PreSubclassWindow();
}

LRESULT CDlgCheckBtn::OnSetCheck( WPARAM w, LPARAM l )
{
	SetCheck((int)w);
	return 0;
}

BOOL CDlgCheckBtn::OnClicked()
{
	if ( BST_CHECKED == m_iCheck )
	{
		SetCheck(BST_UNCHECKED);
	}
	else if ( BST_UNCHECKED == m_iCheck )
	{
		SetCheck(BST_CHECKED);
	}
	else
	{
		//BST_INDETERMINATE 是不应该出现的
		ASSERT( 0 );
	}

	//return FALSE; // 让父窗口能接收Notify
	// 发送父窗口BN_CLICK消息
	::SendMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);

	return FALSE;
}

int CDlgCheckBtn::GetCheck() const
{
	return m_iCheck;
}

void CDlgCheckBtn::SetCheck( int nCheck )
{
	int iOld = m_iCheck;
	m_iCheck = nCheck;
	if ( nCheck != iOld )
	{
		//TRACE(_T("SetCheck: %d\r\n"), nCheck);
		//Invalidate(TRUE);
		DrawMyButton();
	}
}

LRESULT CDlgCheckBtn::OnGetCheck( WPARAM w, LPARAM l )
{
	return m_iCheck;
}


void CDlgCheckBtn::DrawMyButton()
{
	CClientDC dc(this);
	CRect rc;
	GetClientRect(rc);
	DRAWITEMSTRUCT dit = {0};
	dit.CtlID = GetDlgCtrlID();
	dit.CtlType = ODT_BUTTON;
	dit.hDC = dc.m_hDC;
	dit.hwndItem = m_hWnd;
	dit.itemAction = ODA_DRAWENTIRE;
	dit.itemID = 0;
	dit.rcItem = rc;
	dit.itemState = ODS_FOCUS | (BST_CHECKED == m_iCheck ? ODS_SELECTED : 0);
	DrawItem(&dit);
}

void CDlgCheckBtn::OnLButtonDown( UINT nFlags, CPoint point )
{
	SetFocus();
	m_bLButtonDown = true;
	SetCapture();

	CClientDC dc(this);
	CRect rc;
	GetClientRect(rc);
	DRAWITEMSTRUCT dit = {0};
	dit.CtlID = GetDlgCtrlID();
	dit.CtlType = ODT_BUTTON;
	dit.hDC = dc.m_hDC;
	dit.hwndItem = m_hWnd;
	dit.itemAction = ODA_FOCUS | ODA_SELECT;
	dit.itemID = 0;
	dit.rcItem = rc;
	dit.itemState = ODS_FOCUS | ODS_SELECTED;
	
	SendMessage(WM_DRAWITEM, GetDlgCtrlID(), (LPARAM)&dit);
}

void CDlgCheckBtn::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bLButtonDown = false;
	ReleaseCapture();

	CRect rc;
	GetClientRect(rc);
	if ( rc.PtInRect(point) )
	{
		OnClicked();

		CClientDC dc(this);
		DRAWITEMSTRUCT dit = {0};
		dit.CtlID = GetDlgCtrlID();
		dit.CtlType = ODT_BUTTON;
		dit.hDC = dc.m_hDC;
		dit.hwndItem = m_hWnd;
		dit.itemAction = ODA_FOCUS | ODA_SELECT;
		dit.itemID = 0;
		dit.rcItem = rc;
		dit.itemState = ODS_FOCUS;
		
		SendMessage(WM_DRAWITEM, GetDlgCtrlID(), (LPARAM)&dit);
	}
}

BOOL CDlgCheckBtn::OnEraseBkgnd( CDC* pDC )
{
	m_bDrawedFocus = false;
	CRect rc;
	GetClientRect(rc);

	if ( !m_bTransparentDrawText )
	{
		return BaseButton::OnEraseBkgnd(pDC);
	}
	else
	{
		// 让父窗口画背景
		DrawTransparentBk(pDC);
	}
	return TRUE;
}

void CDlgCheckBtn::OnSetFocus( CWnd* pOldWnd )
{
	BaseButton::OnSetFocus(pOldWnd);
	Invalidate(TRUE);
}

void CDlgCheckBtn::OnKillFocus( CWnd* pNewWnd )
{
	BaseButton::OnKillFocus(pNewWnd);
	Invalidate(TRUE);
}

void CDlgCheckBtn::DrawTransparentBk( CDC *pDC )
{
	CWnd *pWndParent = GetParent();
	ASSERT( pWndParent != NULL );
	ASSERT( pDC != NULL );
	
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
	dcMem.DeleteDC();
	bmp.DeleteObject();
}

void CDlgCheckBtn::SetImageInfo(const UINT &unIdCheck,const UINT &unIdUnCheck)
{
	if( m_pImageCheck )
		delete m_pImageCheck;
	m_pImageCheck = NULL;
	ImageFromIDResource (AfxGetResourceHandle(),unIdCheck, L"PNG", m_pImageCheck);
	ASSERT(m_pImageCheck);

	if( m_pImageUnCheck )
		delete m_pImageUnCheck;
	m_pImageUnCheck = NULL;
	ImageFromIDResource (AfxGetResourceHandle(),unIdUnCheck, L"PNG", m_pImageUnCheck);
	ASSERT(m_pImageUnCheck);
}

void CDlgCheckBtn::SetTextFont(int nHeight, LPCTSTR fontName, BOOL bRedraw )
{
	m_font.DeleteObject();

	nHeight*=-1;
	m_lf.lfHeight = nHeight;
	wcscpy(m_lf.lfFaceName,fontName);
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

HBRUSH CDlgCheckBtn::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_clrTextColor);
	
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

void CDlgCheckBtn::SetMyTextColor( COLORREF clr )
{
	m_clrTextColor = clr;
}

//////////////////////////////////////////////////////////////////////////
//
void CCheckBtnOnlyOneCheckGroup::AddCheckBtn( CButton *pBtn )
{
	int iSize = (int)m_aBtn.size();
	for ( int i=0; i<iSize; ++i )
	{
		if ( m_aBtn[i]==pBtn )
		{
			return;
		}
	}

	m_aBtn.push_back(pBtn);
}

void CCheckBtnOnlyOneCheckGroup::RemoveCheckBtn( CButton *pBtn )
{
	for ( int i=0; i<(int)m_aBtn.size(); ++i )
	{
		if ( m_aBtn[i]==pBtn )
		{
			m_aBtn.erase(m_aBtn.begin()+i);
			return;
		}
	}
}

void CCheckBtnOnlyOneCheckGroup::OnSomeOneCheck( CButton *pBtn )
{
	int iSize = (int)m_aBtn.size();
	for ( int i=0; i<iSize; ++i )
	{
		CButton *pOther = m_aBtn[i];
		if ( pOther!=pBtn && pOther )
		{
			// 除了本身，其它的全部clear
			if ( pOther->GetCheck()==BST_CHECKED )
			{
				pOther->SetCheck(BST_UNCHECKED);
			}
		}
	}
}

void CCheckBtnOnlyOneCheckGroup::ClearAllCheckStatus()
{
	int iSize = (int)m_aBtn.size();
	for ( int i=0; i<iSize; ++i )
	{
		CButton *pOther = m_aBtn[i];
		if ( pOther )
		{
			// 全部clear
			if ( pOther->GetCheck()==BST_CHECKED )
			{
				pOther->SetCheck(BST_UNCHECKED);
			}
		}
	}
}

// ComboBoxCheck.cpp : 实现文件
//

#include "stdafx.h"
#include "ComboBoxCheck.h"
#include "FontFactory.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CComboBoxCheck, CComboBox)
CComboBoxCheck::CComboBoxCheck()
{
	m_bOver = FALSE;
	m_pBrsh = CBrush::FromHandle( ( HBRUSH )GetStockObject( NULL_BRUSH ) );
	m_bgBrush = new CBrush;
	m_bgBrush->CreateSolidBrush( RGB( 255,255,255 ) );
	m_bgPen.CreatePen( PS_SOLID,1,RGB( 230,230,230 ) );
	m_font.CreateFont(-14,0,0,0,0,0,0,0,0,0,0,0,0,gFontFactory.GetExistFontName(L"微软雅黑"));//...
	m_bDown = FALSE;
	m_selItem = 666666666;
	m_preSelItem = 666666666;
	m_bFous		= FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bMpPhone.LoadBitmap(IDB_BITMAP_ARROW);
}

CComboBoxCheck::~CComboBoxCheck()
{
	if ( m_bgPen.m_hObject != NULL )
	{
		m_bgPen.DeleteObject();
	}
	if ( m_pBrsh->m_hObject != NULL )
	{
		m_pBrsh->DeleteObject();
	}
	if ( m_bgBrush->m_hObject != NULL )
	{
		m_bgBrush->DeleteObject();
	}
	if ( m_bgBrush != NULL )
	{
		delete m_bgBrush;
	}
	if ( m_font.m_hObject != NULL )
	{
		m_font.DeleteObject();
	}

	for ( IterItem iter = m_vecItemList.begin();iter != m_vecItemList.end();iter ++ )
	{
		if ( *iter != NULL )
		{
			delete *iter;
		}
	}
}


BEGIN_MESSAGE_MAP(CComboBoxCheck, CComboBox)
//{{AFX_MSG_MAP(CComboBoxCheck)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE( WM_MOUSELEAVE,OnMouseLeave )
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
//	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnCbnSelchange)
	//
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CComboBoxCheck 消息处理程序

BOOL CComboBoxCheck::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CComboBoxCheck::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( lpDrawItemStruct->CtlType != ODT_COMBOBOX )
	{
		return;
	}
	UINT itemID = lpDrawItemStruct->itemID;
	CRect rcClient = lpDrawItemStruct->rcItem;
	UINT  iState = lpDrawItemStruct->itemState;
	
	CBitmap MemBit;

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	DrawContent( iState,rcClient,pDC,itemID );
}

void CComboBoxCheck::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bOver == FALSE )
	{
		m_bOver = TRUE;

		//更新当前区域
		UpdateRect();

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof( TRACKMOUSEEVENT );
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 0;
		tme.hwndTrack = m_hWnd;
		
		_TrackMouseEvent( &tme );
	}
	CComboBox::OnMouseMove(nFlags, point);
}
LRESULT CComboBoxCheck::OnMouseLeave( WPARAM wParam,LPARAM lParam )
{
	if ( m_bOver )
	{
		m_bOver = FALSE;
	}
	UpdateRect();

	return 1;
}

void CComboBoxCheck::UpdateRect(void)
{
	CRect rcClient;
	GetWindowRect( &rcClient );
	rcClient.DeflateRect( -2,-2 );
	GetParent()->ScreenToClient( &rcClient );
	GetParent()->InvalidateRect( &rcClient,FALSE );

	return;
}

void CComboBoxCheck::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 20;
	lpMeasureItemStruct->itemWidth   = 200;
}

void CComboBoxCheck::InitControl(void)
{
}
void CComboBoxCheck::OnNcPaint( CDC* pDC )
{
	//绘制客户区
	CDC dMemDC;
	dMemDC.CreateCompatibleDC(pDC);
	dMemDC.SetMapMode(pDC->GetMapMode());

	//画动作
	CBitmap mNewBmp;
	CRect rc;
	GetClientRect(&rc);
	
	mNewBmp.CreateCompatibleBitmap(pDC, rc.right - rc.left, rc.bottom - rc.top);
	CBitmap* pOldBmp = dMemDC.SelectObject(&mNewBmp);
	
	CPen* pOldPen = dMemDC.SelectObject( &m_bgPen );
	CBrush* pOldBrsh = dMemDC.SelectObject( m_bgBrush );
	dMemDC.Rectangle( &rc );
	dMemDC.SelectObject( pOldPen );
	dMemDC.SelectObject( &pOldBrsh );

	int32 iSel = GetCurSel();
	if ( 0 > iSel )
	{
		return;
	}
	PItemList pItem = m_vecItemList[iSel];

	CPen	m_penLeft;
	CPen	m_penRight;
	CRect rcLeft,rcMid,rcRight;
	rcLeft = rcMid = rcRight = rc;
	dMemDC.SetBkMode( TRANSPARENT );

	//画第一段图标
	/*
	rcLeft.right = rcLeft.left + 16;
	rcLeft.left += 1;
	rcLeft.top += 4;
	::DrawIconEx( dMemDC.m_hDC,rcLeft.left,rcLeft.top,pItem->hIcon,16,16,NULL,NULL,DI_NORMAL );
 
	//画第二段文本
	rcMid.left = rcLeft.right + 10;
	rcMid.right = rc.right - 110;
	CFont* pOldFont = dMemDC.SelectObject( &m_font );
	dMemDC.SetTextColor( pItem->clrLeft );
	dMemDC.DrawText( pItem->strUrl,&rcMid,DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	*/
	//画第三段文本
	//rcLeft.right = rcLeft.left + 16;
	rcLeft.left += 5;
	//rcLeft.top += 4;

	CFont* pOldFont = dMemDC.SelectObject( &m_font );
	//rcRight.left = rcMid.right;
	dMemDC.SetTextColor( pItem->clrLeft);
	//dMemDC.DrawText( pItem->strTitle,&rcRight,DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	dMemDC.DrawText( pItem->strTitle,&rcLeft,DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	dMemDC.SelectObject( pOldFont );
	
	CRect rcEnd(rc);
	rcEnd.left = rc.right - 20;
	

	if( m_bDown )
	{
		//dMemDC.DrawFrameControl( &rcEnd,DFC_SCROLL,DFCS_SCROLLDOWN|DFCS_FLAT|DFCS_MONO|DFCS_PUSHED );
		//::DrawIconEx(dMemDC.m_hDC,rcEnd.left,rcEnd.top,hIcon,16,16,NULL,NULL,DI_NORMAL );
		//dMemDC.BitBlt(rcEnd.left,rcEnd.top, 30 , 27 , &dMemDC,0, 0,SRCCOPY);
	}
	else
	{
		//dMemDC.DrawFrameControl( &rcEnd,DFC_SCROLL,DFCS_SCROLLDOWN|DFCS_FLAT|DFCS_MONO );
		//::DrawIconEx(dMemDC.m_hDC,rcEnd.left,rcEnd.top,hIcon,16,16,NULL,NULL,DI_NORMAL );
		//dMemDC.BitBlt(rcEnd.left,rcEnd.top, 30 , 27 , &dMemDC,0, 0,SRCCOPY);
	}

	//dMemDC.SelectObject(pOldBitmap);
	
	pDC->BitBlt(rc.left, rc.top , rc.right - rc.left, rc.bottom - rc.top, &dMemDC,rc.left ,rc.top, SRCCOPY);
	dMemDC.SelectObject(&m_bMpPhone);
	if (m_bOver)
	{
		pDC->BitBlt(rcEnd.left,(rcEnd.Height()-12)/2, 16 , 12 , &dMemDC,0, 13,SRCCOPY);
	}
	else
	{
		pDC->BitBlt(rcEnd.left,(rcEnd.Height()-12)/2, 16 , 12 , &dMemDC,0, 0,SRCCOPY);
	}

	//恢复
	dMemDC.SelectObject(pOldBmp);
	pOldBmp->DeleteObject();
	dMemDC.DeleteDC();
	mNewBmp.DeleteObject();
}
void CComboBoxCheck::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	OnNcPaint( &dc );
}

void CComboBoxCheck::DrawContent( UINT iState,CRect rcClient, CDC* pDC,int itemID)
{
	PItemList pItem = m_vecItemList[itemID];
	
	CPen	m_penLeft;
	CPen	m_penRight;
	CRect rcLeft,rcMid,rcRight;
	rcLeft = rcMid = rcRight = rcClient;
	pDC->SetBkMode( TRANSPARENT );
	
	if (iState & ODS_SELECTED)
	{
		UpdateRect(); //及时更新选择区域
	}

	if ( (itemID == m_selItem) && !(iState&ODS_COMBOBOXEDIT) )
	{
		CPen bgPen;
		bgPen.CreatePen( PS_SOLID,1,RGB( 141,178,227 ) );

		CPen* pOldPen = pDC->SelectObject( &bgPen );
		CBrush* pOldBrush = pDC->SelectObject( m_pBrsh );
		pDC->RoundRect( &rcClient,CPoint( 5,5 ) );
		pDC->SelectObject( pOldBrush );
		pDC->SelectObject( pOldPen );
		bgPen.DeleteObject();
	}
	//画第一段图标
	/*
	rcLeft.right = rcLeft.left + 16;
	rcLeft.top += 4;
	::DrawIconEx( pDC->m_hDC,rcLeft.left,rcLeft.top,pItem->hIcon,16,16,NULL,NULL,DI_NORMAL );
	
	//画第二段文本
	rcMid.left = rcLeft.right + 10;
	rcMid.right = rcClient.right - 100;
	pDC->SetTextColor( pItem->clrLeft );
	CFont* pOldFont = pDC->SelectObject( &m_font );
	pDC->DrawText( pItem->strUrl,&rcMid,DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	*/
	//画第三段文本
	//rcMid.left = rcLeft.right + 10;
	//rcMid.right = rcClient.right - 100;
	rcLeft.left += 2;
	CFont* pOldFont = pDC->SelectObject( &m_font );
	//rcRight.left = rcMid.right + 8;
	pDC->SetTextColor( pItem->clrLeft );
	pDC->DrawText( pItem->strTitle,&rcLeft,DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	pDC->SelectObject( pOldFont );
	
	/*
	CRect rcEnd( rcClient );
	rcEnd.left = rcEnd.right - 18;
	rcEnd.top += 3;
	if ( itemID == m_selItem )
	{
		if ( m_bFous )
		{
			DrawIconEx( pDC->m_hDC,rcEnd.left,rcEnd.top,AfxGetApp()->LoadIcon( IDI_ICON6 ),16,16,NULL,NULL,DI_NORMAL );
		}
		else
		{
			DrawIconEx( pDC->m_hDC,rcEnd.left,rcEnd.top,AfxGetApp()->LoadIcon( IDI_ICON7 ),16,16,NULL,NULL,DI_NORMAL );
		}
		
	}
	*/
}

void CComboBoxCheck::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDown = TRUE;
	UpdateRect();
	CComboBox::OnLButtonDown(nFlags, point);
}

void CComboBoxCheck::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDown = FALSE;
	UpdateRect();
	CComboBox::OnLButtonUp(nFlags, point);
}

void CComboBoxCheck::OnCbnSelchange()
{
	UpdateRect();
}

LRESULT CComboBoxCheck::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( WM_CTLCOLORLISTBOX == message)  
	{  
		HWND hListBox = (HWND)lParam;  
		CListBox* pListBox = (CListBox*)FromHandle(hListBox);  
	//	pListBox->ModifyStyle(WS_BORDER ,0,0);
		if (NULL == pListBox)
		{
			return 0;
		}
		ASSERT(pListBox);  
		int nCount = pListBox->GetCount();  

		if(CB_ERR != nCount)  
		{             
			CPoint pt;  
			GetCursorPos(&pt);  
			pListBox->ScreenToClient(&pt);  

			CRect rc;  
			for(int i=0; i<nCount; i++)  
			{  
				pListBox->GetItemRect(i, &rc);
				if(rc.PtInRect(pt))  
				{
					m_preSelItem = m_selItem;
					m_selItem = i;
					if ( m_selItem != m_preSelItem )
					{
						CRect preRc;
						CRect rcFous( rc );
						rcFous.left = rcFous.right - 18;

						pListBox->GetItemRect( m_preSelItem,&preRc );
						pListBox->InvalidateRect( &preRc );
						
						if ( rcFous.PtInRect( pt ) )
						{
							m_bFous = TRUE;
							pListBox->InvalidateRect( &rc );
						}
						else
						{
							m_bFous = FALSE;
							pListBox->InvalidateRect( &rc );
						}
					}
					break;  
				}
			} 
		}  
	}  
	else if (WM_KILLFOCUS == message)
	{
		m_selItem = 666666666;
		return 0;
	}
	else if (WM_KEYDOWN == message)
	{
		return 0;
	}

	return CComboBox::WindowProc(message, wParam, lParam);
}

void CComboBoxCheck::AddItemEx(CString strContent)
{
	ItemList* pItem = new ItemList;
	pItem->clrLeft	= RGB(0x4d, 0x4d, 0x4d);
	pItem->clrRight	= RGB( 0,255,125 );

	//pItem->hIcon		= AfxGetApp()->LoadIcon( IDI_ICON1 );
	pItem->strTitle	= strContent;
	pItem->strUrl   = _T("");
	pItem->iItem = AddString( strContent);
	m_vecItemList.push_back( pItem );
}

void CComboBoxCheck::OnSetFocus(CWnd* pOldWnd)
{
	Invalidate();
	CComboBox::OnSetFocus(pOldWnd);
}
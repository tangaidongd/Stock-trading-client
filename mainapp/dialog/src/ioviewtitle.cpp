// ioviewtitle.cpp : implementation file
//
#include "stdafx.h"
#include "ioviewtitle.h"
#include "facescheme.h"

#include "ChartRegion.h"									// ...fangz1028 把这里面的ID 号都拿到resource.h 中.并且规范命名. 这里不应该包含 chartregion 的头文件

#include "IoViewChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
const UINT	KTimerClearFlag			= 1;					// 鼠标离开的事件不好捕捉,用定时器定时清理一下
const UINT	KTimerPeriodClaerFlag	= 1000*1;				// 1 秒钟
//
const COLORREF KColorTextNormal = RGB(150, 150, 150);
const COLORREF KColorTextForcus = RGB(255, 255, 255);
const COLORREF KColorTextSelf   = RGB(251, 206, 3);//CFaceScheme::Instance()->GetActiveColor();

const COLORREF KColorPublicNormalBk	= RGB(0, 0, 0);
const COLORREF KColorPublicFocusBk	= RGB(0, 0, 0)/*RGB(89, 75, 66)*/;

const COLORREF KColorFocusBk  = RGB(0, 0, 0);
const COLORREF KColorNormalBk = RGB(0, 0, 0);
const COLORREF KColorSelfBk   = RGB(89, 75, 66);

const int32	   KiSpanBettweenBtnGroups = 0;		// 不同组按钮之间的间隔

// 左右箭头:
const T_FuctionButton g_KstLeftButton  = T_FuctionButton( L"<-",	L"前一视图",	IOVIEW_TITILE_LEFT_BUTTON_ID,  IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE);
const T_FuctionButton g_KstRightButton = T_FuctionButton( L"->",	L"后一视图",	IOVIEW_TITILE_RIGHT_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE);

/////////////////////////////////////////////////////////////////////////////
// CIoViewTitle
CIoViewTitle::CIoViewTitle()
{
	if( !m_TipWnd.Create(this))		 
	{
		//ASSERT(0);
	}
	
	m_ColorBk = RGB(0, 0, 0);
	m_TipWnd.m_bWindowsPosition = true;
	
	Reset();
}

CIoViewTitle::~CIoViewTitle()
{ 
	DEL(m_pLeftButton);
	DEL(m_pRightButton);
	
	int32 i = 0;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{
		DEL(m_apFuctionBtns[i]);
	}
	
	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{
		DEL(m_apPublicBtns[i]);
	}
	
	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{
		DEL(m_apPrivateBtns[i]);
	}
	
	m_apFuctionBtns.RemoveAll();
	m_apPublicBtns.RemoveAll();
	m_apPrivateBtns.RemoveAll();
}

BEGIN_MESSAGE_MAP(CIoViewTitle, CStatic)
	//{{AFX_MSG_MAP(CIoViewTitle)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIoViewTitle message handlers

void CIoViewTitle::OnPaint()
{
	if ( NULL == m_pDCParent )
	{	
		CPaintDC dc(this);
		CRect rect;		
		GetClientRect(&rect);
		
		dc.FillSolidRect(&rect,RGB(0,0,0));
		
		int32 i = 0;
		for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
		{
			m_apPublicBtns.GetAt(i)->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
		}
		
		for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
		{
			m_apPrivateBtns.GetAt(i)->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
		}
		
		for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
		{
			m_apFuctionBtns.GetAt(i)->DrawButton(&dc, m_ColorBk, m_bParentMaxed);			
		}
		
		if (m_bShowLeftRight)
		{
			m_pLeftButton->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
			m_pRightButton->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
		}
	}
	else
	{
		ForcePaint();
	}
}

// BOOL CIoViewTitle::PreTranslateMessage(MSG* pMsg)
// {
// 	CPoint pt;				
// 	GetCursorPos(&pt);
// 	ScreenToClient(&pt);
// 	
// 	if ( WM_LBUTTONDOWN == pMsg->message )
// 	{
// 		OnLButtonDown(0, pt);
// 		return true;
// 	}
// 	else if ( WM_MOUSEMOVE == pMsg->message )
// 	{
// 		OnMouseMove(0, pt);
// 		return true;
// 	}
// 	
// 	return CStatic::PreTranslateMessage(pMsg);
// }

void CIoViewTitle::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KTimerClearFlag)
	{	
		if ( !m_bParentDrag )
		{
			// 增加判断鼠标是否还在title区域内，如果不在，则让tip隐藏，在，不理会 0001752
			CPoint pointCur;
			if ( GetCursorPos(&pointCur) )
			{
				CRect rcWindow;
				GetClientRect(&rcWindow);
				ClientToScreen(&rcWindow);
				
				CPoint pointCurInWindow = pointCur;
				pointCurInWindow.Offset( -rcWindow.left, -rcWindow.top );
				//CIoViewTitleButton* pButtonBefore = GetButtonByFocus();
				CIoViewTitleButton* pButtonNow    = GetButtonByPoint(pointCurInWindow);
				
				if ( rcWindow.PtInRect(pointCur) && NULL != pButtonNow )
				{
					// 如果在区域内，且有聚焦则return
					return;
				}
			}
			
			// 以前的代码 clear之后由于invalidate了，导致又产生一次mousemove，然后又启动
			// 现在在前面加一个如果不在区域才隐藏，否则全部交给mousemove和mouseleave处理，这个只是以防万一
			// 处理chartRegion中，mouse leave不适合处理的情况
			if ( m_TipWnd.IsWindowVisible() )
			{
				//TRACE(_T("title timer clear\r\n"));
				ClearButtonFocusFlag();
			}
			
			KillTimer(nIDEvent);
		} 		
	}		
}

void CIoViewTitle::SetBkGroundColor(COLORREF clrBk)
{
	m_ColorBk = clrBk;
}

void CIoViewTitle::SetDC(CMemDCEx* pDC)
{
	// 删除的时候传空值
	
	if ( NULL != pDC )
	{
		ASSERT_VALID(pDC);
	}

	m_pDCParent	= pDC;
}

void CIoViewTitle::SetDCTransformParam(int32 ix,int32 iy)
{
	m_ixDC = ix;
	m_iyDC = iy;
}

void Trans(CRect &rect,int32 ix,int32 iy)
{
	rect.top	+= iy;
	rect.bottom += iy;
	rect.left   += ix;
	rect.right  += ix;
}

void CIoViewTitle::Transform(int32 ix,int32 iy)
{
	// 坐标转化:
	int32 i;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++ )
	{
		CRect rect = m_apFuctionBtns[i]->GetShowRect();
		Trans(rect,ix,iy);		
		m_apFuctionBtns[i]->SetShowRect(rect);
	}

	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{
		CRect rect = m_apPrivateBtns[i]->GetShowRect();
		Trans(rect,ix,iy);		
		m_apPrivateBtns[i]->SetShowRect(rect);		
	}

	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{
		CRect rect = m_apPublicBtns[i]->GetShowRect();
		Trans(rect,ix,iy);		
		m_apPublicBtns[i]->SetShowRect(rect);		
	}
	
	if ( NULL != m_pLeftButton)
	{
		CRect rect = m_pLeftButton->GetShowRect();
		Trans(rect,ix,iy);		
		m_pLeftButton->SetShowRect(rect);
	}

	if ( NULL != m_pRightButton)
	{
		CRect rect = m_pRightButton->GetShowRect();
		Trans(rect,ix,iy);		
		m_pRightButton->SetShowRect(rect);
	}
}

void CIoViewTitle::ForcePaint()
{	
	if ( NULL == m_pDCParent )
	{
		return;
	}
	
	// 传入的DC 区域和本控件所在区域的原点不一定重合,所以要坐标转换:	
	Transform(m_ixDC,m_iyDC);
	//

	int32 i = 0;
	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{
		m_apPublicBtns.GetAt(i)->DrawButton(m_pDCParent, m_ColorBk, m_bParentMaxed);
	}
	
	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{
		m_apPrivateBtns.GetAt(i)->DrawButton(m_pDCParent, m_ColorBk, m_bParentMaxed);
	}
	
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{
		m_apFuctionBtns.GetAt(i)->DrawButton(m_pDCParent, m_ColorBk, m_bParentMaxed);			
	}
	
	if (m_bShowLeftRight)
	{
		if ( NULL != m_pLeftButton)
		{
			m_pLeftButton->DrawButton(m_pDCParent, m_ColorBk, m_bParentMaxed);
		}
		
		if ( NULL != m_pRightButton)
		{
			m_pRightButton->DrawButton(m_pDCParent, m_ColorBk, m_bParentMaxed);
		}	
	}	

	// 还原:
	Transform(-m_ixDC,-m_iyDC);
	//
	
	if ( NULL != m_pDCParent )
	{
		// 每次画的时候传进来的值,所以画完重置为空. 
 		m_pDCParent = NULL;
	}
}

void CIoViewTitle::Reset()
{
	m_RectSet		  = CRect(0,0,0,0);
	m_RectFuctionBtns = CRect(0,0,0,0);
	m_RectPrivateBtns = CRect(0,0,0,0);
	m_RectPublicBtns  = CRect(0,0,0,0);
	
	m_bParentMaxed	  = false;
	m_bTracking		  = false;
	m_bShowLeftRight  = false;	
	m_bParentDrag	  = false;

	m_uIDOwner		  = (UINT)-1;
	m_pLeftButton	  = NULL;
	m_pRightButton	  = NULL;
	m_pDCParent		  = NULL;
	m_ixDC			  = 0;
	m_iyDC			  = 0;

	m_apPrivateBtns.RemoveAll();
	m_apPublicBtns.RemoveAll();
	m_apFuctionBtns.RemoveAll();

	m_bShowFuctionBtnSameTime = true;	
	ClearButtonFocusFlag();

	if ( GetSafeHwnd())
	{
		SetTimer(KTimerClearFlag, KTimerPeriodClaerFlag, NULL);
	}	
}
 
CRect CIoViewTitle::GetRect()
{
	return m_RectSet;
}

int32 CIoViewTitle::GetButtonNums()
{
	int32 iNums = 0; 
	
	iNums += m_apPublicBtns.GetSize();	
	iNums += m_apPrivateBtns.GetSize();
	iNums += m_apFuctionBtns.GetSize();

	return iNums;
}

void CIoViewTitle::SetOwnerID(const UINT& uID)
{
	m_uIDOwner = uID;
}

void CIoViewTitle::SetShowRect(const CRect& rectSet)
{
	// 清空显示区域

	CRect rectZero    = CRect(0,0,0,0);
	m_RectFuctionBtns = rectZero;
	m_RectPrivateBtns = rectZero;
	m_RectPublicBtns  = rectZero;

	int32 i;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{
		if ( NULL != m_apFuctionBtns.GetAt(i))
		{
			m_apFuctionBtns.GetAt(i)->SetShowRect(rectZero);
		}
	}

	for ( i = 0 ; i < m_apPrivateBtns.GetSize() ; i++)
	{
		if ( NULL != m_apPrivateBtns.GetAt(i))
		{
			m_apPrivateBtns.GetAt(i)->SetShowRect(rectZero);
		}
	}

	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{
		if ( NULL != m_apPublicBtns.GetAt(i))
		{
			m_apPublicBtns.GetAt(i)->SetShowRect(rectZero);
		}
	}

	if ( NULL != m_pLeftButton )
	{
		m_pLeftButton->SetShowRect(rectZero);
	}

	if ( NULL != m_pRightButton)
	{
		m_pRightButton->SetShowRect(rectZero);
	}
	//////////////////////////////////////////////////////////////////////////

	// 从父窗口那里得到显示区域
	m_RectSet = rectZero;
	m_RectSet.right  = rectSet.Width();
	m_RectSet.bottom = rectSet.Height(); 
	//m_RectSet = rectSet;
	
	// 分配自己的显示区域
	int32 iFuctionBtnNums = m_apFuctionBtns.GetSize();
	int32 iPublicBtnNums  = m_apPublicBtns.GetSize();
	int32 iPrivateBtnNums = m_apPrivateBtns.GetSize();
	
	
	// 高度不用考虑,考虑宽度; 总共给的宽度,功能按钮需要多少,私有的能显示多少,公有的能显示多少
	int32 iWidthAll = m_RectSet.Width();
	int32 iWidthFuctionNeed = IOVIEW_TITLE_HEIGHT*iFuctionBtnNums;
	
	if ( iWidthAll < iWidthFuctionNeed )
	{
		if ( m_bShowFuctionBtnSameTime )
		{
			// 如果连功能按钮都显示不下,就都不显示了:
			m_bShowLeftRight  = false;
			m_RectSet		  = CRect(0,0,0,0);
			m_RectPrivateBtns = CRect(0,0,0,0);
			m_RectPublicBtns  = CRect(0,0,0,0);
			m_RectFuctionBtns = CRect(0,0,0,0);
			
			this->ShowWindow(SW_HIDE);
			return;
		}			
	}
	else
	{
		m_bShowLeftRight = false;

		// 给每个功能按钮分配空间,再看能显示几个私有的几个公有的:
		
		CRect rect = m_RectSet;
		
		for ( int32 i = m_apFuctionBtns.GetSize()-1; i >= 0 ; i--)
		{
			CIoViewTitleButton* pFuctionButton = m_apFuctionBtns.GetAt(i);
			
			if ( NULL == pFuctionButton)
			{
				continue;
			}
			
			CRect rectFuction = rect;			

			rectFuction.left = rectFuction.right - IOVIEW_TITLE_HEIGHT;
			pFuctionButton->SetShowRect(rectFuction);
			
			rect.right -= IOVIEW_TITLE_HEIGHT;
		}

		// 保存功能按钮区域
		m_RectFuctionBtns	   = m_RectSet;
		m_RectFuctionBtns.left = rect.right;

		rect.right -= KiSpanBettweenBtnGroups;

		// 计算还能显示多少按钮:
	
		int32 iNumsAvailable = rect.Width() / IOVIEW_TITLE_HEIGHT;			

		if ( 0 == iNumsAvailable )
		{
			// 都不用显示了
			return;
		}		

		if ( iNumsAvailable < iPublicBtnNums )
		{
			// 显示不下所有共有按钮,都不显示了
			return;
		}
		else
		{
			// 全部显示的下

			for ( int32 j = m_apPublicBtns.GetSize()-1 ; j >=0 ; j--)
			{
				CRect rectPublic = rect;
				rectPublic.left  = rectPublic.right - IOVIEW_TITLE_HEIGHT;
				m_apPublicBtns.GetAt(j)->SetShowRect(rectPublic);
				rect.right -= IOVIEW_TITLE_HEIGHT;
			}		
		}

		// 
		m_RectPublicBtns.right = m_RectFuctionBtns.left;
		m_RectPublicBtns.left  = rect.right;
		
		iNumsAvailable	  -= iPublicBtnNums;
		m_RectPublicBtns  = m_RectSet;
		
		rect.right -= KiSpanBettweenBtnGroups;
		//

		if ( iNumsAvailable > 0 )
		{
			// 显示完所有的功能按钮和公有按钮以后,看还剩下多少能显示公共按钮
			if ( iNumsAvailable >= iPrivateBtnNums)
			{			
				// 显示全部
				for ( i = m_apPrivateBtns.GetSize()-1; i >= 0 ; i--)
				{
					CRect rectPrivate = rect;
					rectPrivate.left = rectPrivate.right - IOVIEW_TITLE_HEIGHT;
					
					m_apPrivateBtns.GetAt(i)->SetShowRect(rectPrivate);					
					rect.right -= IOVIEW_TITLE_HEIGHT;
				}
				
				m_RectPrivateBtns  = m_RectSet;
				m_RectPrivateBtns.right = m_RectPublicBtns.left;
				m_RectPrivateBtns.left  = rect.right;
			}
		}	
	}
}

void CIoViewTitle::AddLeftRightButton(bool32 bLeft,CIoViewTitleButton* pButton)
{
	if ( bLeft )
	{
		m_pLeftButton = pButton;
	}
	else
	{
		m_pRightButton = pButton;
	}
}

void CIoViewTitle::AddFuctionButton(CIoViewTitleButton* pButton)
{
	if ( NULL == pButton)
	{
		return;
	}
	m_apFuctionBtns.Add(pButton);
}

void CIoViewTitle::AddButton(bool32 bPublic,CIoViewTitleButton* pButton)
{	
	if ( m_uIDOwner == pButton->GetButtonID() )
	{		
		pButton->SetSelfFlag(true);
	}

	if (bPublic)
	{
		m_apPublicBtns.Add(pButton);
	}
	else
	{
		m_apPrivateBtns.Add(pButton);
	}
}

void CIoViewTitle::DelButton(bool32 bPublic,UINT iGuid)
{
	if ( bPublic)
	{
		for ( int32 i = 0 ; i < m_apPublicBtns.GetSize(); i++)
		{
			if ( iGuid == m_apPublicBtns.GetAt(i)->GetButtonID())
			{
				m_apPublicBtns.RemoveAt(i);
				break;
			}
		}
	}
	else
	{
		for ( int32 i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
		{
			if ( iGuid == m_apPrivateBtns.GetAt(i)->GetButtonID())
			{
				m_apPrivateBtns.RemoveAt(i);
				break;
			}
		}
	}	
}

CIoViewTitleButton* CIoViewTitle::GetButtonByPoint(CPoint point)
{
	int32 i;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{		
		if ( m_apFuctionBtns.GetAt(i)->GetShowRect().PtInRect(point))
		{
			return m_apFuctionBtns.GetAt(i);			
		}
	}
	//
	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{		
		if ( m_apPrivateBtns.GetAt(i)->GetShowRect().PtInRect(point))
		{
			return m_apPrivateBtns.GetAt(i);			
		}
	}
	//
	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{		
		if ( m_apPublicBtns.GetAt(i)->GetShowRect().PtInRect(point))
		{
			return m_apPublicBtns.GetAt(i);			
		}
	}
	//
	if ( NULL != m_pLeftButton)
	{	
		if ( m_pLeftButton->GetShowRect().PtInRect(point))
		{
			return m_pLeftButton;			
		}
	}
	//
	if ( NULL != m_pRightButton)
	{	
		if ( m_pRightButton->GetShowRect().PtInRect(point))
		{
			return m_pRightButton;			
		}
	}
	
	return NULL;
}

CIoViewTitleButton* CIoViewTitle::GetPrivateButtonByID(UINT uID)
{
	for ( int32 i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{
		CIoViewTitleButton* pPrivateButton = m_apPrivateBtns.GetAt(i);

		if ( uID == pPrivateButton->GetButtonID())
		{
			return pPrivateButton;
		}		 
	}

	return NULL;
}

CIoViewTitleButton* CIoViewTitle::GetFuctionButtonByID(UINT uID)
{
	for ( int32 i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{
		CIoViewTitleButton* pFuctionButton = m_apFuctionBtns.GetAt(i);
		
		if ( uID == pFuctionButton->GetButtonID())
		{
			return pFuctionButton;
		}		 
	}
	
	return NULL;
}

CIoViewTitleButton* CIoViewTitle::GetButtonByFocus()
{
	int32 i;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{		
		if ( NULL != m_apFuctionBtns.GetAt(i))
		{
			if ( m_apFuctionBtns.GetAt(i)->BeFocusButton())
			{
				return m_apFuctionBtns.GetAt(i);			
			}			
		}
	}
	//
	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{		
		if ( NULL != m_apPrivateBtns.GetAt(i))
		{
			if ( m_apPrivateBtns.GetAt(i)->BeFocusButton())
			{
				return m_apPrivateBtns.GetAt(i);			
			}
		}		
	}
	//
	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{		
		if ( NULL != m_apPublicBtns.GetAt(i))
		{
			if ( m_apPublicBtns.GetAt(i)->BeFocusButton())
			{
				return m_apPublicBtns.GetAt(i);			
			}
		}
	}
	//
	if ( NULL != m_pLeftButton)
	{	
		if ( m_pLeftButton->BeFocusButton())
		{
			return m_pLeftButton;			
		}
	}
	//
	if ( NULL != m_pRightButton)
	{	
		if ( m_pRightButton->BeFocusButton())
		{
			return m_pRightButton;			
		}
	}
	
	return NULL;
}

void CIoViewTitle::OnLButtonDown(UINT nFlags, CPoint point)
{
	CIoViewTitleButton* pButton = GetButtonByPoint(point);

 	if ( NULL != pButton)
 	{
		pButton->OnLButtonDown();
	}
	
	CStatic::OnLButtonDown(nFlags,point);
}

void CIoViewTitle::OnRButtonDown(UINT nFlags, CPoint point)
{
	CIoViewTitleButton* pButton = GetButtonByPoint(point);
		
	if ( NULL != pButton)
	{
		pButton->OnRButtonDown();
	}

	CStatic::OnRButtonDown(nFlags,point);
}

void CIoViewTitle::ClearButtonFocusFlag()
{
	int32 i;
	for ( i = 0 ; i < m_apFuctionBtns.GetSize(); i++)
	{		
		if ( NULL != m_apFuctionBtns.GetAt(i))
		{
			m_apFuctionBtns.GetAt(i)->SetFocusButton(false);		
		}
	}
	//
	for ( i = 0 ; i < m_apPrivateBtns.GetSize(); i++)
	{
		if ( NULL != m_apPrivateBtns.GetAt(i))
		{
			m_apPrivateBtns.GetAt(i)->SetFocusButton(false);
		}		
	}
	//
	for ( i = 0 ; i < m_apPublicBtns.GetSize(); i++)
	{		
		if ( NULL != m_apPublicBtns.GetAt(i))
		{
			m_apPublicBtns.GetAt(i)->SetFocusButton(false);
		}		
	}
	//
	if ( NULL != m_pLeftButton)
	{	
		m_pLeftButton->SetFocusButton(false);
	}
	//
	if ( NULL != m_pRightButton)
	{	
		m_pRightButton->SetFocusButton(false);
	}
	
	if ( GetSafeHwnd())
	{
		// 不必要发送消息了，button会处理 0001752
	//	PostMessage(WM_PAINT,0,0);
	}

	//
	if ( m_TipWnd.IsWindowVisible() )
	{
		m_TipWnd.Hide();
	}	
}

void CIoViewTitle::SetFuctionBtnFlag(bool32 bShowSameTime)
{
	m_bShowFuctionBtnSameTime = bShowSameTime;
}

void CIoViewTitle::SetMaxFlag(bool32 bMax)
{
	m_bParentMaxed = bMax;
	Invalidate();

	CIoViewTitleButton* pButton = GetFuctionButtonByID((UINT)IOVIEW_TITLE_F7_BUTTON_ID);
	if ( NULL != pButton )
	{
		if (bMax)
		{
			pButton->SetTips(L"还原");
		}
		else
		{
			pButton->SetTips(L"最大");
		}	
	}
}

void CIoViewTitle::SetParentDragFlag(bool32 bParentDrag)
{
	m_bParentDrag = bParentDrag;
}

void CIoViewTitle::OnMouseMove(UINT nFlags, CPoint point)
{	
	CClientDC dc(this);
	
	// 使用mouse event, leave 0001752
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd; 
	tme.dwFlags = TME_LEAVE ;
	tme.dwHoverTime = 1;
	
	m_bTracking = _TrackMouseEvent(&tme);		
 	
	CIoViewTitleButton* pButtonBefore = GetButtonByFocus();
	CIoViewTitleButton* pButtonNow    = GetButtonByPoint(point);

	if ( pButtonBefore == pButtonNow )
	{
		return;
	}

	Transform(m_ixDC,m_iyDC);
 	if ( NULL != pButtonNow )
 	{
 		pButtonNow->SetFocusButton(true);
		
		CPoint pt;
		GetCursorPos(&pt);
		
		m_TipWnd.Show(pt, pButtonNow->m_StrTips);
		
		KillTimer(KTimerClearFlag);
		SetTimer(KTimerClearFlag, KTimerPeriodClaerFlag, NULL);

		if ( NULL == m_pDCParent)
		{
			pButtonNow->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
		}
		else
		{
			ForcePaint();
		}		
 	}	
	
	if ( NULL != pButtonBefore)
	{
		pButtonBefore->SetFocusButton(false);

		if ( NULL == m_pDCParent)
		{
			pButtonBefore->DrawButton(&dc, m_ColorBk, m_bParentMaxed);
		}
		else
		{
			ForcePaint();
		}
	}
	
	Transform(-m_ixDC,-m_iyDC);

	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CIoViewTitle::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{		
	//TRACE(L"鼠标离开\n");

	//m_bTracking = false;
	
	// 由于chartRegion使用的不是将title move到固定位置显示的方式，所以mouse leave基本上在mouse move之后迅速就来了，交给time处理吧
	// 0001752
	CWnd *pWnd	=	GetParent();
	if ( NULL != pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
	{
		return Default();
	}
	
	ClearButtonFocusFlag();
	// RedrawWindow();
	
	return Default();
}

//////////////////////////////////////////////////////////////////////////
CIoViewTitleButton::CIoViewTitleButton()
{
	m_pImageNormal = NULL;
	m_pImageFocus = NULL;
	m_pImageSelf = NULL;

	m_uiBmpNormal = 0;
	m_uiBmpFocus = 0;
	m_uiBmpSelf = 0;

	m_uID = 0;
	m_hOwner = NULL;
	m_pOwner = NULL;

	m_bSelf	  = false;
	m_bFocus  = false;
	m_bLeft	  = true;
	m_bRight  = false;
	m_bPublic = true;
}

CIoViewTitleButton::CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const bool32 bLeft)
{
	m_RectButton = CRect(0,0,0,0);
	m_hOwner	 = hOwner;
	m_pOwner	 = pParent;
	m_bPublic	 = true;
	m_bSelf		 = false;
	m_bFocus	 = false;

	m_pImageNormal = NULL;
	m_pImageFocus = NULL;
	m_pImageSelf = NULL;

	m_uiBmpNormal = 0;
	m_uiBmpFocus = 0;
	m_uiBmpSelf = 0;

	if ( bLeft )
	{
		m_bLeft	  = true;
		m_bRight  = false;

		m_uID	  = g_KstLeftButton.m_uID;
		m_StrText = g_KstLeftButton.m_StrText;
		m_StrTips = g_KstLeftButton.m_StrTips;
		/*
		m_uiBmpNormal = g_KstLeftButton.m_uiBmpNormal;
		m_uiBmpFocus  = g_KstLeftButton.m_uiBmpFocus;
		m_uiBmpSelf	  = g_KstLeftButton.m_uiBmpSelf;
		*/
		
		if ( IMAGEID_NOUSEIMAGE == g_KstLeftButton.m_uiBmpNormal || !ImageFromIDResource(AfxGetResourceHandle(),g_KstLeftButton.m_uiBmpNormal, L"PNG", m_pImageNormal))
		{
			m_pImageNormal = NULL;
		}
		
		if ( IMAGEID_NOUSEIMAGE == g_KstLeftButton.m_uiBmpFocus || !ImageFromIDResource(AfxGetResourceHandle(),g_KstLeftButton.m_uiBmpFocus, L"PNG", m_pImageFocus))
		{
			m_pImageFocus = NULL;
		}
		
		if ( IMAGEID_NOUSEIMAGE == g_KstLeftButton.m_uiBmpSelf || !ImageFromIDResource(AfxGetResourceHandle(),g_KstLeftButton.m_uiBmpNormal, L"PNG", m_pImageSelf))
		{
			m_pImageSelf = NULL;
		}
	}
	else
	{
		m_bLeft	  = false;
		m_bRight  = true;

		m_uID	  = g_KstLeftButton.m_uID;
		m_StrText = g_KstLeftButton.m_StrText;
		m_StrTips = g_KstLeftButton.m_StrTips;
		/*
		m_uiBmpNormal = g_KstRightButton.m_uiBmpNormal;
		m_uiBmpFocus  = g_KstRightButton.m_uiBmpFocus;
		m_uiBmpSelf	  = g_KstRightButton.m_uiBmpSelf;
		*/

		if ( IMAGEID_NOUSEIMAGE == g_KstRightButton.m_uiBmpNormal || !ImageFromIDResource(AfxGetResourceHandle(),g_KstRightButton.m_uiBmpNormal, L"PNG", m_pImageNormal))
		{
			m_pImageNormal = NULL;
		}
		
		if ( IMAGEID_NOUSEIMAGE == g_KstRightButton.m_uiBmpFocus || !ImageFromIDResource(AfxGetResourceHandle(),g_KstRightButton.m_uiBmpFocus, L"PNG", m_pImageFocus))
		{
			m_pImageFocus = NULL;
		}
	
		if ( IMAGEID_NOUSEIMAGE == g_KstRightButton.m_uiBmpSelf || !ImageFromIDResource(AfxGetResourceHandle(),g_KstRightButton.m_uiBmpNormal, L"PNG", m_pImageSelf))
		{
			m_pImageSelf = NULL;
		}
	}
}

CIoViewTitleButton::CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const UINT uID, const bool32 bPublic, const CString& StrText, const CString& StrTips, UINT uiBmpNormal, UINT uiBmpFocus, UINT uiBmpSelf)
{
	m_bSelf		 = false;
	m_bLeft		 = false;
	m_bRight	 = false;
	m_bFocus	 = false;
	m_RectButton = CRect(0,0,0,0);
	
	m_hOwner	 = hOwner;
	m_pOwner	 = pParent;
	m_uID		 = uID;
	m_bPublic	 = bPublic;
	m_StrText	 = StrText;
	m_StrTips	 = StrTips;

	m_uiBmpNormal= uiBmpNormal;
	m_uiBmpFocus = uiBmpFocus;
	m_uiBmpSelf	 = uiBmpSelf;

	if ( IMAGEID_NOUSEIMAGE == uiBmpNormal || !ImageFromIDResource(AfxGetResourceHandle(),uiBmpNormal, L"PNG", m_pImageNormal))
	{
		m_pImageNormal = NULL;
	}
	
	if ( IMAGEID_NOUSEIMAGE == uiBmpFocus || !ImageFromIDResource(AfxGetResourceHandle(),uiBmpFocus, L"PNG", m_pImageFocus))
	{
		m_pImageFocus = NULL;
	}
	
	if ( IMAGEID_NOUSEIMAGE == uiBmpSelf || !ImageFromIDResource(AfxGetResourceHandle(),uiBmpSelf	, L"PNG", m_pImageSelf))
	{
		m_pImageSelf = NULL;
	}
}

CIoViewTitleButton::~CIoViewTitleButton()
{
	DEL(m_pImageNormal);
	DEL(m_pImageFocus);
	DEL(m_pImageSelf);
}

void CIoViewTitleButton::FillWithBitmap(CDC* pDC, UINT uiBitmapID, const CRect& rt)
{
	if ( NULL == pDC)
	{
		return;
	}
	
	int32 iOldMode = pDC->SetBkMode(TRANSPARENT);

	CDC bmpdc;
	bmpdc.CreateCompatibleDC(NULL);
	
	CBitmap bmp;	
	bmp.LoadBitmap(uiBitmapID);
	
	BITMAP BitMap;
	bmp.GetObject(sizeof(BitMap),&BitMap);
	
	CBitmap * pOldBmp = bmpdc.SelectObject(&bmp);
	pDC->StretchBlt(rt.left, rt.top, rt.Width(), rt.Height(), &bmpdc, 0, 0, BitMap.bmWidth, BitMap.bmHeight, SRCCOPY);

	pDC->SetBkMode(iOldMode);
	bmpdc.SelectObject(pOldBmp);
	//
	bmpdc.DeleteDC();
	bmp.DeleteObject();
}

void CIoViewTitleButton::DrawButton(CDC* pDC, COLORREF clrBk, bool32 bMax)
{	
	if ( 0 == m_RectButton.Height() || 0 == m_RectButton.Width())
	{
		return;
	}

	if ( NULL == pDC || NULL == pDC->m_hAttribDC || NULL == pDC->m_hDC)
	{
		//ASSERT(0);
		return;
	}

	ASSERT_VALID(pDC);

	Graphics GraphicImage(pDC->GetSafeHdc());

	// 画角用到
	int32 iScale  = 4;
	int32 iCorner = IOVIEW_TITLE_HEIGHT/iScale;

	CRect rectIN = m_RectButton;
	rectIN.top		+= 2;
	rectIN.bottom	-= 2;
	rectIN.left		+= 2;
	rectIN.right	-= 1;

	// rectIN.DeflateRect(1,1);
	
	if ( m_bFocus)
	{
		pDC->SetTextColor( KColorTextForcus);
		
		if ( m_bPublic )
		{
			if ( m_bSelf)
			{
				if ( NULL != m_pImageSelf)
				{
					// 有图就就贴图
					DrawImage(GraphicImage, m_pImageSelf, rectIN, 1, 0, true);	
				}
				else
				{
					// 没图就画背景色
					// pDC->FillSolidRect(rectIN, KColorPublicFocusBk);
					pDC->FillSolidRect(rectIN, KColorSelfBk);
				}
			}
			else
			{
				if ( NULL != m_pImageFocus)
				{
					// 有图就就贴图
					DrawImage(GraphicImage, m_pImageFocus, rectIN, 1, 0, false);	
				}
				else
				{
					// 没图就画背景色
					// pDC->FillSolidRect(rectIN, KColorPublicFocusBk);
					pDC->FillSolidRect(rectIN, KColorPublicFocusBk);
				}
			}	
		}
		else
		{
			if ( NULL != m_pImageFocus)
			{
				// 有图就就贴图
				DrawImage(GraphicImage, m_pImageFocus, rectIN, 1, 0, false);	
			}
			else
			{
				// 没图就画背景色
				// pDC->FillSolidRect(rectIN, KColorFocusBk);
				pDC->FillSolidRect(rectIN, KColorPublicFocusBk);
			}			
		}		
	}
	else
	{
		pDC->SetTextColor( KColorTextNormal);

		if ( m_bPublic )
		{
			if ( m_bSelf)
			{
				if ( NULL != m_pImageSelf)
				{
					// 有图就就贴图
					DrawImage(GraphicImage, m_pImageSelf, rectIN, 1, 0, true);	
				}
				else
				{
					// 没图就画背景色
					// pDC->FillSolidRect(rectIN, KColorPublicNormalBk);
					
					int32 iCornerSmall = rectIN.Height()/iScale;

					CRect rectTop   = rectIN;
					rectTop.bottom -= iCornerSmall;

					CRect rectBottom = rectIN;
					rectBottom.top   = rectTop.bottom;
					rectBottom.left += iCornerSmall;
					

					pDC->FillSolidRect(rectTop, KColorSelfBk);
					pDC->FillSolidRect(rectBottom, KColorSelfBk);

					CPoint ptBegin(rectTop.left, rectTop.bottom);
					CPoint ptEnd(rectBottom.left, rectBottom.bottom);

					COLORREF clrActive = KColorSelfBk;
					CPen* pPen = new CPen(PS_SOLID, 1, clrActive);
					CPen* pOldPen = pDC->SelectObject(pPen);
					
					for ( ; ptBegin.x <= rectBottom.left; ptBegin.x++,ptEnd.y--)
					{
						pDC->MoveTo(ptBegin);
						pDC->LineTo(ptEnd);
					}
				
					pDC->SelectObject(pOldPen);
					pPen->DeleteObject();				
				}
			}
			else
			{
				if ( NULL != m_pImageFocus)
				{
					// 有图就就贴图
					DrawImage(GraphicImage, m_pImageNormal, rectIN, 1, 0, false);	
				}
				else
				{
					// 没图就画背景色
					// pDC->FillSolidRect(rectIN, KColorPublicNormalBk);
					pDC->FillSolidRect(rectIN, clrBk);
				}
			}		
		}
		else
		{
			if ( NULL != m_pImageFocus)
			{
				// 有图就就贴图
				DrawImage(GraphicImage, m_pImageNormal, rectIN, 1, 0, false);	
			}
			else
			{
				// 没图就画背景色
				// pDC->FillSolidRect(rectIN, KColorNormalBk);
				pDC->FillSolidRect(rectIN, clrBk);
			}			
		}
	}

	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
	CFont* pOldFont = pDC->SelectObject(pFont);

	CRect RectText = m_RectButton;
	RectText.top  += 1;

	if ( m_bSelf )
	{
		pDC->SetTextColor(KColorTextSelf);
	}

	pDC->SetBkMode(TRANSPARENT);

 	if ( (UINT)IOVIEW_TITLE_F7_BUTTON_ID == m_uID )
 	{
 		// 补丁:
		COLORREF clr;
		if ( m_bFocus )
		{
			clr = KColorTextForcus;
		}
		else
		{
			clr = KColorTextNormal;
		}

 		CRect rectNotMaxed = m_RectButton;
 		rectNotMaxed.DeflateRect(4, 4);
 		
		CBrush BrushF7;
 		BrushF7.CreateSolidBrush(clr);
 		CBrush* pOldBrush = pDC->SelectObject(&BrushF7);
		CPen  penF7(PS_SOLID, 1, clr);
		CPen* pOldPen = pDC->SelectObject(&penF7);

		if ( bMax )
		{
			CRect rect1  = rectNotMaxed;
			rect1.top   += 2;
			rect1.right -= 2;				
			pDC->FrameRect(&rect1, &BrushF7);
			
			CPoint pt1,pt2;
			pt1.x = rect1.left;
			pt1.y = rect1.top + 1;
			
			pt2.x = rect1.right;
			pt2.y = rect1.top + 1;
			pDC->MoveTo(pt1);
			pDC->LineTo(pt2);

			//
			CRect rect2  = rectNotMaxed;
			rect2.right += 1;
			rect2.left  += 2;
			rect2.bottom-= 2;
			pDC->FrameRect(rect2, &BrushF7);

			pt1.x = rect2.left;
			pt1.y = rect2.top + 1;
			
			pt2.x = rect2.right;
			pt2.y = rect2.top + 1;
			//pDC->MoveTo(pt1);
			//pDC->LineTo(pt2);
			//
			CRect rect3 = rect1;
			
			rect3.top	+= 2;
			rect3.left	+= 1;
			rect3.right	-= 1;
			rect3.bottom-= 1;
			
			pDC->FillSolidRect(&rect3, clrBk);				
		}
		else
		{
			pDC->FrameRect(&rectNotMaxed, &BrushF7);
			CPoint pt1,pt2;
			
			pt1.x = rectNotMaxed.left;
			pt1.y = rectNotMaxed.top + 1;

			pt2.x = rectNotMaxed.right;
			pt2.y = rectNotMaxed.top + 1;

			pDC->MoveTo(pt1);
			pDC->LineTo(pt2);

			pt1.y += 1;
			pt2.y += 1;

			pDC->MoveTo(pt1);
			pDC->LineTo(pt2);

			pDC->SelectObject(pOldBrush);			
		}

		BrushF7.DeleteObject();	
		penF7.DeleteObject();
		
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
 	}
	else if ( (UINT)IOVIEW_TITLE_DEL_BUTTON_ID == m_uID || (UINT)REGIONTITLEDELBUTTONID == m_uID )
	{
		COLORREF clr;
		if ( m_bFocus )
		{
			clr = KColorTextForcus;
		}
		else
		{
			clr = KColorTextNormal;
		}
		
		CRect rectDraw = m_RectButton;
 		rectDraw.DeflateRect(5, 5);
		
		if ( rectDraw.Width() != rectDraw.Height() )
		{
			int32 iDif = rectDraw.Width() - rectDraw.Height();
			rectDraw.top -= iDif;
		}

		CPen  penDel(PS_SOLID, 1, clr);
		CPen* pOldPen = pDC->SelectObject(&penDel);
		
		pDC->MoveTo(rectDraw.left, rectDraw.top);
		pDC->LineTo(rectDraw.right, rectDraw.bottom);		
		pDC->SetPixel(CPoint(rectDraw.right, rectDraw.bottom), clr);
		

		pDC->MoveTo(rectDraw.right, rectDraw.top);
		pDC->LineTo(rectDraw.left,	rectDraw.bottom);
		pDC->SetPixel(CPoint(rectDraw.left, rectDraw.bottom), clr);
		
		pDC->SelectObject(pOldPen);
		penDel.DeleteObject();
	}
	else
 	{
		// 写标题文字:
		pDC->DrawText(m_StrText, &RectText, DT_CENTER|DT_VCENTER);
	}

	pDC->SelectObject(pOldFont);

	/* 两个角
	pDC->MoveTo(m_RectButton.left,m_RectButton.top );
	pDC->LineTo(m_RectButton.right,m_RectButton.top);
	pDC->LineTo(m_RectButton.right,m_RectButton.bottom-1 - iCorner);
	pDC->LineTo(m_RectButton.right-iCorner,m_RectButton.bottom-1);
	pDC->LineTo(m_RectButton.left+iCorner,m_RectButton.bottom-1);
	pDC->LineTo(m_RectButton.left,m_RectButton.bottom -1 - iCorner);
	pDC->LineTo(m_RectButton.left,m_RectButton.top );
	*/
	
	// 

	CPen * pPen    = NULL;
	CPen * pOldPen = NULL;
	
	// 一个角
	if ( m_bSelf )
	{
 		pPen = new CPen(PS_SOLID, 1, RGB(89, 75, 66));
 		pOldPen = pDC->SelectObject(pPen);
	}
	else
	{
		pPen = new CPen(PS_SOLID, 1, RGB(89, 75, 66));
		pOldPen = pDC->SelectObject(pPen);
	}	

	pDC->MoveTo(m_RectButton.left,m_RectButton.top );
	pDC->LineTo(m_RectButton.right,m_RectButton.top);
	pDC->LineTo(m_RectButton.right,m_RectButton.bottom-1);	
	pDC->LineTo(m_RectButton.left+iCorner,m_RectButton.bottom-1);
	pDC->LineTo(m_RectButton.left,m_RectButton.bottom -1 - iCorner);
	pDC->LineTo(m_RectButton.left,m_RectButton.top );

	pDC->SelectObject(pOldPen);
	pPen->DeleteObject();
	DEL(pPen);
}

CRect CIoViewTitleButton::GetShowRect()
{	
	return m_RectButton;
}

void CIoViewTitleButton::SetShowRect(const CRect& RectButton)
{
	m_RectButton = RectButton;
}

void CIoViewTitleButton::OnLButtonDown()
{
	::PostMessage(m_hOwner, UM_IoViewTitle_Button_LButtonDown, (WPARAM)m_uID, 0);
}

void CIoViewTitleButton::OnRButtonDown()
{
	::PostMessage(m_hOwner, UM_IoViewTitle_Button_RButtonDown, (WPARAM)m_uID, 0);
}

UINT CIoViewTitleButton::GetButtonID()
{
	return m_uID;
}

void CIoViewTitleButton::SetSelfFlag(bool32 bSelf)
{
	m_bSelf = bSelf;
}

bool32 CIoViewTitleButton::GetSelfFlag()
{
	return m_bSelf;
}

bool32 CIoViewTitleButton::BeFocusButton()
{
	return m_bFocus;
}

void CIoViewTitleButton::SetFocusButton(bool32 bFocus /* = false */)
{
	bool32 bFocusPre = m_bFocus;
	m_bFocus = bFocus;
	if ( m_bFocus != bFocusPre )
	{
		// 改变状态，需要重绘 - 如果够细化，可以直接调用重绘 0001752
		// 由于m_RectButton是相对于title 的client位置，所以应该先尝试调用title
		// IoViewChart直接让它全绘制吧
// 		if ( NULL != m_pOwner && IsWindow(m_pOwner->m_Title.GetSafeHwnd()) && !m_pOwner->IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
// 		{
// 			m_pOwner->m_Title.InvalidateRect(&m_RectButton, FALSE);
// 		}
// 		else
// 		{
// 			InvalidateRect(m_hOwner, NULL, TRUE);
// 		}
	}
}

void CIoViewTitleButton::SetText(const CString& StrText)
{
	m_StrText = StrText;
}

void CIoViewTitleButton::SetTips(const CString& StrTip)
{
	m_StrTips = StrTip;
}

CIoViewBase* CIoViewTitleButton::TestGetParentIoView()
{
	return m_pOwner;
}
// Region.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "ShareFun.h"
#include "Region.h"
#include "ShareFun.h"
#include "XLTimerTrace.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define VIEW_TOOLBAR_HEIGHT 55

//////////////////////////////////////////////////////////////////////////
// 
bool32 CAxisSmooth::GetSpecialPixelValue(int32 iPixel, float &fValue)
{
	if (m_iPixelWidth <= 0 || iPixel < 0 || iPixel > m_iPixelWidth)
		return false;
	
	return ((iPixel - 0) * (m_fMax - m_fMin) / m_iPixelWidth + m_fMin) == 0.0f ? false : true;
}
//lint -e{438}
bool32 CAxisCustom::GetSpecialPixelInfo(int32 iPixel, int32 &iIndex, int32 &iPixelStart, int32 iPixelEnd, CString &StrSliderText1, CString &StrSliderText2)
{
	if (m_iPixelWidth <= 0 || iPixel < 0 || iPixel > m_iPixelWidth)
		return false;

	int32 iAxisNodeCount = m_aAxisNodes.GetSize();
	if (0 == iAxisNodeCount)
		return false;

	{
		float fPixelPerNode = (float)((m_iPixelWidth + 1)*1.0f / (iAxisNodeCount + 1));
	
		float fPixelStart = fPixelPerNode / 2;	
		float fPixelEnd = (float)m_iPixelWidth - fPixelPerNode / 2;

		float fPixel = (float)iPixel;
		if (fPixel < fPixelStart || fPixel > fPixelEnd)
			return false;

		fPixel -= fPixelPerNode / 2;
		iIndex = (int32)(fPixel / fPixelPerNode + 0.5);
		if (iIndex < 0 || iIndex >= iAxisNodeCount)
		{
			////ASSERT(0);
			return false;
		}

		fPixelStart = fPixelPerNode / 2 + fPixelPerNode * iIndex;
		fPixelEnd = fPixelPerNode / 2 + fPixelPerNode * (iIndex + 1);
		iPixelStart = (int32)(fPixelStart + 0.5);
		iPixelEnd = (int32)(fPixelEnd + 0.5) - 1;

		StrSliderText1 = m_aAxisNodes[iIndex].m_StrSliderText1;
		StrSliderText2 = m_aAxisNodes[iIndex].m_StrSliderText2;
		return true;
	}		

	return false;
}












/////////////////////////////////////////////////////////////////////////
int32 CRegionViewParam::g_iID = 0;
CRegionViewParam::CRegionViewParam ( IN CWnd* pWnd)
{	
	m_pWnd			= pWnd;
	m_pDC			= NULL;
	m_pOverlayDC	= NULL;
	m_pMainDC		= NULL;
	m_pPickDC		= NULL;
	m_bDragEdgeIn	= true;
	m_RectView		= CRect(-1,-1,1,1);
	m_RectLast		= CRect(-1,-1,1,1);
	m_PointMouse	= CPoint(-1,-1);
	m_PointLastMouse= CPoint(-2,-2);
	m_iLayerMax		= 0;
	m_iLevelMax		= 0;
	m_iFlag			= 0;
	m_iCurveID		= 0;
	CRegionViewParam::g_iID++;

	m_bShowTopBtn = false;
	m_bShowIndexBtn = false;
	m_bRightButPress = false;

	m_iViewToolBarHeight = VIEW_TOOLBAR_HEIGHT;
}
void CRegionViewParam::SetParentWnd ( CWnd* pWnd )
{
	m_pWnd = pWnd;
}

void CRegionViewParam::SetViewToolBarHeight(int32 iViewToolBarHeight)
{
	m_iViewToolBarHeight = iViewToolBarHeight;
}
void CRegionViewParam::UpdateGDIObject()
{	
	
	if (NULL == m_pWnd)
	{
		return;
	}

	if (!::IsWindow ( m_pWnd->GetSafeHwnd() ) )
	{
		return;
	}

	ReleaseMemDC();
	
	m_pWnd->GetClientRect ( &m_RectView );
	if (m_bShowTopBtn)
	{
		m_RectView.top += m_iViewToolBarHeight;
		if (!m_bShowIndexBtn)
		{
			m_RectView.bottom += m_iViewToolBarHeight;
		}
		else
		{

			m_RectView.bottom += m_iViewToolBarHeight - 25;
		}
	}
	else if (m_bShowIndexBtn)
	{
		m_RectView.bottom -= 25;//VIEW_TOOLBAR_HEIGHT;
	}
//	GetViewRegionRect(&m_RectView);
	
	// t..fangz0615
	// CIoViewChart* pView = (CIoViewChart*)GetView();
	// m_RectView = pView->m_rectClient;

	// 
	m_pDC = new CClientDC(m_pWnd);
	m_pOverlayDC = new CMemDCEx ( m_pDC, m_RectView );
	m_pMainDC = new CMemDCEx ( m_pOverlayDC, m_RectView );
	m_pPickDC = new CMemDCEx(m_pDC,m_RectView,true);
	
}

CRegionViewParam::CRegionViewParam()
{
	////ASSERT(0);
	m_bDragEdgeIn	= false;
	m_iLevelMax		= 0;
	m_iLayerMax		= 0;
	m_iFlag			= 0;
	m_pDC			= NULL;
	m_pPickDC		= NULL;
	m_pMainDC		= NULL;
	m_pOverlayDC	= NULL;
	m_pWnd			= NULL;
	m_iCurveID		= 0;
	m_bShowTopBtn	= false;
	m_bShowIndexBtn	= true;
}
//lint --e{1579}
CRegionViewParam::~CRegionViewParam()
{
	//modify by  weng.cx
	ReleaseMemDC();
	ASSERT(NULL==m_pDC);
	ASSERT(NULL==m_pOverlayDC);
	ASSERT(NULL==m_pMainDC);
	ASSERT(NULL==m_pPickDC);
}
void CRegionViewParam::EnableClipDiff ( bool32 bEnable,CRect& Rect )
{
	if (NULL != m_pOverlayDC && NULL != m_pMainDC)
	{
		if ( bEnable )
		{
			m_pOverlayDC->m_RectClipDiff = Rect;
			m_pMainDC->m_RectClipDiff = Rect;
		}
		else
		{
			m_pOverlayDC->m_RectClipDiff = CRect(0,0,0,0);
			m_pMainDC->m_RectClipDiff = CRect(0,0,0,0);
		}
	}
	

}
CMemDCEx* CRegionViewParam::GetMainDC()
{
	return m_pMainDC;
}
CMemDCEx* CRegionViewParam::GetOverlayDC()
{ 
	return m_pOverlayDC;
}
CMemDCEx* CRegionViewParam::GetPickDC()
{ 
	return m_pPickDC;
}
CWnd* CRegionViewParam::GetView()
{ 
	return m_pWnd;
}

void CRegionViewParam::ReleaseMemDC()
{
	if (NULL == m_pWnd)
	{
		return;
	}
	if (!::IsWindow(m_pWnd->m_hWnd ))
	{
		return;
	}
	//������IoViewChart�Ĵ�������֮ǰ
	::KillTimer(m_pWnd->GetSafeHwnd(),KRegionMouseOverAWhileTimerId);
	
	/*
	{
	while ( m_Nodes.GetSize() )
	{
	CNodeSequence* pNodes = m_Nodes.GetAt(0);
	m_Nodes.RemoveAt(0);
	delete pNodes;
	}
	}
	*/
	if ( NULL != m_pPickDC )
	{
		DEL(m_pPickDC);
	}
	if ( NULL != m_pMainDC )
	{
		DEL(m_pMainDC);		
	}
	if ( NULL != m_pOverlayDC )
	{
		DEL(m_pOverlayDC);
	}
	if ( NULL != m_pDC )
	{
		DEL(m_pDC);		
	}
}

bool32 CRegionViewParam::GetViewRegionRect( OUT LPRECT pRect )
{
	CWnd *pWnd = GetView();
	if ( NULL != pWnd && NULL != pRect )
	{
		pWnd->GetClientRect(pRect);
		if (m_bShowTopBtn)
		{
			pRect->top += m_iViewToolBarHeight;
		}
		if (m_bShowIndexBtn)
		{
			pRect->bottom -= 25;//VIEW_TOOLBAR_HEIGHT;
		}
		return true;
	}
	return false;
}

void CRegionViewParam::BShowViewToolBar(bool32 bShowIndexToolbar, bool32 bShowTopToolbar)
{
	m_bShowTopBtn = bShowTopToolbar;
	m_bShowIndexBtn = bShowIndexToolbar;
}

void CRegionViewParam::GetShowViewToolBar(OUT bool32& bShowIndexToolbar, OUT bool32& bShowTopToolbar)
{
	bShowTopToolbar = m_bShowTopBtn;
	bShowIndexToolbar = m_bShowIndexBtn;
}

//////////////////////////////////////////////////////////////////////
//
CRegion::CRegion( IN CRegionViewParam* pViewParam, CString StrName ,uint32 iFlag)
{
	
	m_pViewParam	= pViewParam;	
	m_StrName		= StrName;
	m_eDirection	= EDLeft;
	m_eMouseOpType	= EMOTDefault;
	m_iFlag			= 0;
	AddFlag ( m_iFlag, KRegionMaskSegment&iFlag);
	m_pRegionParent	= NULL;
	m_pRegionChild1	= NULL;
	m_pRegionChild2	= NULL;
	m_pRegionRoot	= this;
	m_fSizePercent	= 1.0;
	m_iLayer		= 0;
	m_iLevel		= 0;
	m_clrBackground = RGB(0,0,0);
	m_clrBorder		= RGB(255,0,0);
	m_bFullOverlay	= false;

	m_RectView.SetRectEmpty();
}
//lint --e{1540}
CRegion::~CRegion()
{
}

void CRegion::FreeAll(CRegion* pRegion)
{
	pRegion = pRegion->m_pRegionRoot;
	RegionArray Regions;
	pRegion->GetRegions(pRegion,Regions);
	int32 i,iSize = Regions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		pRegion = Regions.GetAt(i);
		pRegion->OnDestroy();
		DEL(pRegion);
	}
	Regions.RemoveAll();
}

//pRegion:��this�з�,��pRegion��������.
//eDir:�����λ��(��/��:V�з�;��/��:H�з�)
//SizeWish:�����Ĵ�С
//SizeWish.cx��V�з�ʱpRegion��ռ�Ŀ��
//SizeWish.cy��H�з�ʱpRegion��ռ�ĸ߶�

bool32 CRegion::SplitRegion ( IN CRegion* pRegion, E_Direction eDir, CSize SizeWish  )
{
	if ( NULL == pRegion)
	{
		return false;
	}
	
	// ASSERT ( NULL!= pRegion );
	
	ASSERT(SizeWish.cx >= 0 );
	ASSERT(SizeWish.cy >= 0 );
	
	//�����Region���ܴ�����Region(��ʱ����Լ��,�Ժ��ȥ�����Լ��?)
	ASSERT(pRegion->m_pRegionChild1==NULL);
	ASSERT(pRegion->m_pRegionChild2==NULL);
	
	//���ܲ���һ���̶��ߴ�<ָ��ֵ��
	if ( CheckFlag(pRegion->m_iFlag,CRegion::KFixHeight) && SizeWish.cy < GetRegionMinWH() )
	{
		////ASSERT(0);
	}
	if ( CheckFlag(pRegion->m_iFlag,CRegion::KFixWidth) && SizeWish.cx < GetRegionMinWH() )
	{
		////ASSERT(0);
	}
	
	//���ܰ�һ���̶��߶ȵ�Region���뵽�̶��߶ȵ�Region����/�·�
	if ( CheckFlag(m_iFlag,CRegion::KFixHeight) &&
		CheckFlag(pRegion->m_iFlag,CRegion::KFixHeight) &&
		(eDir==EDTop||eDir==EDBottom))
	{
		////ASSERT(0);
	}
	
	//���ܰ�һ���̶���ȵ�Region���뵽�̶���ȵ�Region����/�ҷ�
	if ( CheckFlag(m_iFlag,CRegion::KFixWidth) &&
		CheckFlag(pRegion->m_iFlag,CRegion::KFixWidth) &&
		(eDir==EDLeft||eDir==EDRight))
	{
		////ASSERT(0);
	}
	
	//�̶���ȵĲ��������϶�(��ʱ����Լ��,�Ժ��ȥ�����Լ��?)
	if ( CheckFlag(pRegion->m_iFlag,CRegion::KFixWidth))
	{
		if ( CheckFlag(pRegion->m_iFlag,CRegion::KRightDragAble ))
		{
			////ASSERT(0);
		}
	}
	//�̶��߶ȵĲ��������϶�(��ʱ����Լ��,�Ժ��ȥ�����Լ��?)
	if ( CheckFlag(pRegion->m_iFlag,CRegion::KFixHeight))
	{
		if ( CheckFlag(pRegion->m_iFlag,CRegion::KBottomDragAble ) )
		{
			////ASSERT(0);
		}
	}
	
	if ( NULL == m_pRegionParent )
	{
		//�����ǰRegion���зֵ�ʱ��,���ֵ�ǰRegionû�и��ڵ�,���з�ǰҪ������OnCreate
		this->OnCreate();
	}
	
	//����һ���յ�Region
	CString StrFrameRegionName = _T("[") + this->m_StrName + _T("-") + pRegion->m_StrName + _T("]");
	CRegion* pFrameRegion = new CRegion ( m_pViewParam, StrFrameRegionName );

	AddFlag ( pFrameRegion->m_iFlag, CRegion::KFrame );//KFrame���͵�Region,����DragAble
	if ( CheckFlag(m_iFlag,CRegion::KFixHeight )) 	AddFlag(pFrameRegion->m_iFlag,CRegion::KFixHeight);
	if ( CheckFlag(m_iFlag,CRegion::KFixWidth ))	AddFlag(pFrameRegion->m_iFlag,CRegion::KFixWidth);

	pFrameRegion->m_SizeWish = SizeWish;
	pFrameRegion->m_pRegionParent = m_pRegionParent;
	pFrameRegion->m_pRegionChild1 = this;
	pFrameRegion->m_pRegionChild2 = pRegion;
	pFrameRegion->m_eDirection = m_eDirection;
	pFrameRegion->m_RectView = m_RectView;
	pFrameRegion->m_iLayer = m_iLayer;
	pFrameRegion->OnCreate();
	
	m_iLayer ++;
	pRegion->m_iLayer = m_iLayer;
	
	CRegion* pRoot = NULL;
	if ( NULL != m_pRegionParent )
	{
		if ( m_pRegionParent->m_pRegionChild1 == this )
		{
			m_pRegionParent->m_pRegionChild1 = pFrameRegion;
		}
		else
		{
			m_pRegionParent->m_pRegionChild2 = pFrameRegion;
		}
		pRoot = m_pRegionRoot;
		pFrameRegion->m_pRegionRoot = pRoot;
	}
	else
	{
		//root��pFrameRegion
		pRoot = pFrameRegion;
	}
	
	CSize SizeThis   = CSize ( m_RectView.Width(), m_RectView.Height() );
	CSize SizeParent = CSize ( pFrameRegion->m_RectView.Width(), pFrameRegion->m_RectView.Height());
	float fValue1,fValue2;
	switch( eDir) {
	case EDLeft:
	case EDRight:
		pRegion->m_eDirection = eDir;
		m_eDirection = (EDLeft==eDir)? EDRight:EDLeft;
		//����ռָ��������
		if ( SizeWish.cx <= GetRegionMinWH() )
		{
			pFrameRegion->m_fSizePercent = 1.0;//Child1ռ100%
		}
		else
		{
			//�����Region��SizeΪ��С��.
			if ( SizeParent.cx <= GetRegionMinWH() )
			{
				pFrameRegion->m_fSizePercent = 1.0;//Child1ռ100%
				//SizeThis.cx = 0;
			}
			else
			{
				//����з�һ��		
				if ( SizeWish.cx > SizeParent.cx/2 ) SizeWish.cx = SizeParent.cx/2;
				SizeThis.cx = SizeParent.cx - SizeWish.cx;	
				fValue1 = (float)SizeThis.cx;
				fValue2 = (float)SizeParent.cx;
				pFrameRegion->m_fSizePercent = fValue1 / fValue2;
			}
		}
		break;
		
	case EDTop:
	case EDBottom:
		pRegion->m_eDirection = eDir;
		m_eDirection = (EDTop==eDir)? EDBottom:EDTop;
		//����ռָ��������
		if ( SizeWish.cy <= GetRegionMinWH() )
		{
			pFrameRegion->m_fSizePercent = 1.0;//Child1ռ100%
		}
		else
		{
			//�����Region��SizeΪ��С��.
			if ( SizeParent.cy <= GetRegionMinWH() )
			{
				pFrameRegion->m_fSizePercent = 1.0;//Child1ռ100%
				//SizeThis.cy = 0;
			}
			else
			{
				//����з�һ��		
				if ( SizeWish.cy > SizeParent.cy/2 ) SizeWish.cy = SizeParent.cy/2;
				SizeThis.cy = SizeParent.cy - SizeWish.cy;
				fValue1 = (float)SizeThis.cy;
				fValue2 = (float)SizeParent.cy;
				pFrameRegion->m_fSizePercent = fValue1 / fValue2;
			}
		}
		break;
	default:
		////ASSERT(0);
		break;
	}
	
	m_pRegionParent = pFrameRegion;
	pRegion->m_pRegionParent = pFrameRegion;
	pRegion->m_SizeWish = SizeWish;
	
	//��������Region��RootΪpRoot
	if ( NULL != pRoot )
	{
		RegionArray Regions;
		GetRegions ( pRoot,Regions );
		int32 i,iSize = Regions.GetSize();
		for ( i = 0; i < iSize ; i ++ )
		{
			Regions.GetAt(i)->m_pRegionRoot = pRoot;
		}
		Regions.RemoveAll();
	}
	NestedSize( pFrameRegion );
	pRegion->OnCreate();
	
	return true;
}
//�ݹ����,��pParent(һ��Ϊ���ڵ�)��ʼ,�����ӽڵ�Ĵ�С.
void CRegion::NestedSize ( IN CRegion* pParent)
{
	if ( NULL == pParent ) return;
	if ( NULL == pParent->m_pRegionChild1 ) return;
	
	///////////////////////////////////////////
	//�������ֿ���:(1)����Child��Ϊ��(2)����Child����Ϊ��
	CRegion* pChild1 = pParent->m_pRegionChild1;
	CRegion* pChild2 = pParent->m_pRegionChild2;	
	if(NULL == pChild2 || NULL == pChild1 )
	{
		return;
	}
	RemoveFlag ( pChild1->m_iFlag, CRegion::KTooSmall );
	RemoveFlag ( pChild2->m_iFlag, CRegion::KTooSmall );
	
	if (L"[Trend-split]" == pParent->m_StrName)
	{
		;
	}

	int32 iWidth,iHeight,iValue;
	float fValue;
	
	switch( pChild1->m_eDirection) {
	case EDLeft:
	case EDRight:
		//���Child1��FixWidth
		if ( CheckFlag( pChild1->m_iFlag, CRegion::KFixWidth ) )
		{
			iWidth = pChild1->m_SizeWish.cx;
			//����ռ䲻��
			if ( iWidth >= pParent->m_RectView.Width() )
			{
				iWidth = pParent->m_RectView.Width() - GetRegionMinWH();
			}
			if ( EDLeft == pChild1->m_eDirection )
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.left + iWidth,
					pParent->m_RectView.bottom );
				pChild2->m_RectView = CRect ( pChild1->m_RectView.right,
					pParent->m_RectView.top,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
			else
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right - iWidth,
					pParent->m_RectView.bottom );
				
				pChild1->m_RectView = CRect ( pChild2->m_RectView.right,
					pParent->m_RectView.top,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
		}
		//���Child2��FixWidth
		else if ( CheckFlag( pChild2->m_iFlag, CRegion::KFixWidth ))
		{
			iWidth = pChild2->m_SizeWish.cx;
			if ( iWidth >= pParent->m_RectView.Width() )
			{
				iWidth = pParent->m_RectView.Width() - GetRegionMinWH();
			}
			if ( EDLeft == pChild2->m_eDirection )
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.left + iWidth,
					pParent->m_RectView.bottom );
				pChild1->m_RectView = CRect ( pChild2->m_RectView.right,
					pParent->m_RectView.top,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
			else
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right - iWidth,
					pParent->m_RectView.bottom );
				
				pChild2->m_RectView = CRect ( pChild1->m_RectView.right,
					pParent->m_RectView.top,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
		}
		//���������FixWidth
		else
		{
			fValue = (float) ( pParent->m_RectView.Width());
			fValue = pParent->m_fSizePercent * fValue;
			iValue = roundf_(fValue);
			if ( iValue <= GetRegionMinWH() )
			{
				iValue = GetRegionMinWH();
			}
			
			if ( EDLeft == pChild1->m_eDirection )
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left, 
					pParent->m_RectView.top, 
					pParent->m_RectView.left + iValue, 
					pParent->m_RectView.bottom );	
				pChild2->m_RectView = CRect ( pChild1->m_RectView.right, 
					pParent->m_RectView.top, 
					pParent->m_RectView.right, 
					pParent->m_RectView.bottom );
			}
			else
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left, 
					pParent->m_RectView.top, 
					pParent->m_RectView.right-iValue, 
					pParent->m_RectView.bottom );
				pChild1->m_RectView = CRect ( pChild2->m_RectView.right, 
					pParent->m_RectView.top, 
					pParent->m_RectView.right, 
					pParent->m_RectView.bottom );
			}
		}
		break;
	case EDTop:
	case EDBottom:
		//���Child1��FixHeight
		if ( CheckFlag( pChild1->m_iFlag, CRegion::KFixHeight ) )
		{
			iHeight = pChild1->m_SizeWish.cy;
			//����ռ䲻��
			if ( iHeight >= pParent->m_RectView.Height() )
			{
				iHeight = pParent->m_RectView.Height() - GetRegionMinWH();
			}
			if ( EDTop == pChild1->m_eDirection )
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right,
					pParent->m_RectView.top + iHeight);
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pChild1->m_RectView.bottom,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
			else
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right,
					pParent->m_RectView.bottom - iHeight );
				
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pChild2->m_RectView.bottom,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
		}
		//���Child2��FixHeight
		else if ( CheckFlag( pChild2->m_iFlag, CRegion::KFixHeight ))
		{
			iHeight = pChild2->m_SizeWish.cy;
			
			if ( iHeight >= pParent->m_RectView.Height() )
			{
				iHeight = pParent->m_RectView.Height() - GetRegionMinWH();
			}
			if ( EDTop == pChild2->m_eDirection )
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right,
					pParent->m_RectView.top + iHeight );
				
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pChild2->m_RectView.bottom,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
			else
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left,
					pParent->m_RectView.top, 
					pParent->m_RectView.right,
					pParent->m_RectView.bottom - iHeight);
				
				pChild2->m_RectView = CRect ( pParent->m_RectView.left,
					pChild1->m_RectView.bottom,
					pParent->m_RectView.right,
					pParent->m_RectView.bottom );
			}
		}
		//���������FixHeight
		else
		{
			fValue = (float) ( pParent->m_RectView.Height() );
			fValue = pParent->m_fSizePercent * fValue;
			iValue = roundf_(fValue);
			if ( iValue < GetRegionMinWH() ) iValue = GetRegionMinWH();
			if ( EDTop == pChild1->m_eDirection )
			{
				pChild1->m_RectView = CRect ( pParent->m_RectView.left, 
					pParent->m_RectView.top,
					pParent->m_RectView.right, 
					pParent->m_RectView.top+iValue );
				
				pChild2->m_RectView = CRect ( pParent->m_RectView.left, 
					pChild1->m_RectView.bottom, 
					pParent->m_RectView.right, 
					pParent->m_RectView.bottom );
				
			}
			else
			{
				pChild2->m_RectView = CRect ( pParent->m_RectView.left, 
					pParent->m_RectView.top,
					pParent->m_RectView.right, 
					pParent->m_RectView.bottom-iValue );
				
				pChild1->m_RectView = CRect ( pParent->m_RectView.left, 
					pChild2->m_RectView.bottom, 
					pParent->m_RectView.right, 
					pParent->m_RectView.bottom );
				
			}
		}
		break;
	default:
		////ASSERT(0);
		break;
	}
	if ( pChild1->m_RectView.Width() <= GetRegionMinWH() || 
		pChild1->m_RectView.Height() <= GetRegionMinWH())
	{
		AddFlag ( pChild1->m_iFlag, CRegion::KTooSmall );
	}
	if ( pChild2->m_RectView.Width() <= GetRegionMinWH() || 
		pChild2->m_RectView.Height() <= GetRegionMinWH())
	{
		AddFlag ( pChild2->m_iFlag, CRegion::KTooSmall );
	}

	// TRACE(L"Root:%d Parent: Name: %s Rect: %d %d %d %d\r\n", pParent == m_pRegionRoot, pParent->m_StrName, pParent->m_RectView.top, pParent->m_RectView.bottom, pParent->m_RectView.left, pParent->m_RectView.right);
	// TRACE(L" Child1: Name: %s Rect: %d %d %d %d\r\n", pChild1->m_StrName, pChild1->m_RectView.top, pChild1->m_RectView.bottom, pChild1->m_RectView.left, pChild1->m_RectView.right);
	// TRACE(L" Child2: Name: %s Rect: %d %d %d %d\r\n", pChild2->m_StrName, pChild2->m_RectView.top, pChild2->m_RectView.bottom, pChild2->m_RectView.left, pChild2->m_RectView.right);
	
	NestedSize ( pChild1 );
	NestedSize ( pChild2 );
}

bool32 CRegion::Delete()
{
	ASSERT(!CheckFlag(m_iFlag,CRegion::KFrame));
	ASSERT(NULL==m_pRegionChild1&&NULL==m_pRegionChild2);//���һ��
	//�����Parent,��this�ǵ�����һ��.
	if ( NULL == m_pRegionParent )
	{
		OnDestroy();
		return true;
	}
	CRegion* pBrother = NULL;
	//�ҵ��ֵ�
	if ( this == m_pRegionParent->m_pRegionChild2 )
	{
		pBrother = m_pRegionParent->m_pRegionChild1;
	}
	else
	{
		pBrother = m_pRegionParent->m_pRegionChild2;
	}
	//�Ƿ���Grandpa
	CRegion* pGrandpa = m_pRegionParent->m_pRegionParent;
	if ( NULL != pGrandpa )
	{
		if ( m_pRegionParent == pGrandpa->m_pRegionChild1 )
		{
			pGrandpa->m_pRegionChild1 = pBrother;
		}
		else
		{
			pGrandpa->m_pRegionChild2 = pBrother;
		}
	}
	pBrother->m_pRegionParent = pGrandpa;//maybe null
	pBrother->m_RectView = m_pRegionParent->m_RectView;
	pBrother->m_iLayer--;
	pBrother->m_eDirection = m_pRegionParent->m_eDirection;
	this->m_pRegionParent->OnDestroy();
	DEL(this->m_pRegionParent);
	OnDestroy();
	return true;
}

void CRegion::ReSizeAll ( )
{
	if ( NULL == m_pRegionRoot ) return;
	if ( NULL == m_pViewParam ) return;
	CRect Rect;
	//m_pViewParam->GetView()->GetClientRect( &Rect );
	m_pViewParam->GetViewRegionRect(&Rect);
	
	// t..fangz0615
	// CIoViewChart* pView = (CIoViewChart*)m_pViewParam->GetView();
	// Rect = pView->m_rectClient;

	if ( Rect.Width() < 1 || Rect.Height() < 1 ) return;
	
	m_pViewParam->UpdateGDIObject();
	m_pRegionRoot->m_RectView = Rect;
	m_pRegionRoot->NestedSize ( m_pRegionRoot );

	bool32 bTest = false;
	if (bTest)
	{
		if ( Rect.Height() > 0 )
		{
			RegionArray aRegions;
			GetRegions(m_pRegionRoot, aRegions);
			int32 iLay = m_pViewParam->m_iLayerMax;
			TRACE(_T("\r\n"));
			for ( int32 i =0; i < iLay ; i++ )
			{
				TRACE(_T("Lay :%d\r\n"), i);
				for ( int j=0; j < aRegions.GetSize() ; j++ )
				{
					if ( i == aRegions[j]->m_iLayer )
					{
						TRACE(_T("    %s[%d,%d,%d,%d]\r\n"), (LPCTSTR)aRegions[j]->m_StrName,aRegions[j]->m_RectView.top,aRegions[j]->m_RectView.left,aRegions[j]->m_RectView.bottom,aRegions[j]->m_RectView.right);
					}
				}
			}
 		}
	} 	
}

void CRegion::NestSizeAll ( )
{
	NestedSizeReverse();
	NestedSize ( m_pRegionRoot );
}
void CRegion::SetBottomDragAble ( bool32 bAble )
{
	if ( bAble )
	{
		AddFlag(m_iFlag,CRegion::KBottomDragAble);
	}
	else
	{
		RemoveFlag(m_iFlag,CRegion::KBottomDragAble);
	}
}
bool32 CRegion::RegionTooSmall( )
{
	return CheckFlag(m_iFlag,CRegion::KTooSmall);
}
void CRegion::OnCreate  ( )
{
}
void CRegion::OnDestroy ( )
{
}
void CRegion::OnActive(bool32 bActive )
{
}
///////////////////////////////////////////////////////////////////////////////
void CRegion::OnDraw( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC )
{	
	if ( CheckFlag(m_iFlag,CRegion::KFrame))return;
	if ( NULL == pDC || NULL == pPickDC ) return;

	if ( !CheckFlag(GetDrawFlag(),CRegion::KDrawCurves) &&
		!CheckFlag(GetDrawFlag(),CRegion::KDrawNotify))
	{
		return;
	}

	//pDC
	CPen pen,*pOldPen;
	int32 oldMode = pDC->GetBkMode();
	//��䱳��ɫ
	pDC->SetBkMode(OPAQUE);
	
	//����Pen
	pen.CreatePen(PS_SOLID,1,m_clrBorder);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	pDC->_Clear(m_RectView,m_clrBackground);

	//���Ʊ߿�,ɾ��Pen
	pDC->SetBkMode(TRANSPARENT);
	if ( CheckFlag(m_iFlag,0x0008))//UserChart��־���ƶ����˴�.
	{

	}
	else
	{
		// IoView��������������ߣ�X����಻����
		
		// TopLeft ˳ʱ����� -> TR -> BR -> BL ->TL
		int32 left, top, right, bottom;
		left = m_RectView.left;
		top  = m_RectView.top;
		right = m_RectView.right;
		bottom = m_RectView.bottom;
		pDC->MoveTo(m_RectView.TopLeft());
		if ( m_RectView.top < 3 )
		{
			pDC->MoveTo(right, top);
		}
		else
		{
			pDC->LineTo(right, top);
		}
		
		pDC->LineTo(right, bottom);
		
		pDC->LineTo(left, bottom);
		
		if ( /*left < 3 ||*/ _T("x��") == m_StrName )
		{
			// ����
			;
		}
		else
		{
			pDC->LineTo(left, top);
		}
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	pDC->SetBkMode(oldMode);

	//pPickDC
	oldMode = pPickDC->GetBkMode();
	//��䱳��ɫ
	pPickDC->SetBkMode(OPAQUE);	
	pPickDC->_Clear(m_RectView,RGB(0,0,0));//����ΪRGB(0,0,0)
	pPickDC->SetBkMode(oldMode);

}
void CRegion::OnPreDraw ( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC )
{

}
void CRegion::OnAfterDraw ( IN CMemDCEx* pOverlayDC )
{

}
void CRegion::BeginOverlay( bool32 bFullClient )
{
	m_bFullOverlay = bFullClient;
	if ( NULL != m_pViewParam &&  m_pViewParam->GetMainDC())
	{
		if (!m_bFullOverlay )
		{
			m_pViewParam->GetMainDC()->Swap(m_RectView);
		}
		else
		{
			m_pViewParam->GetMainDC()->Swap();
		}
	}
	
}
void CRegion::EndOverlay()
{
	// xl 0303 ����ʱ����������ӦParentFrame��hide��ʾ
	if (NULL == m_pViewParam)
	{
		return;
	}

	bool32 bHide = false;
	try
	{
		//CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, m_pViewParam->GetView()->GetParentFrame());
		if (NULL != m_pViewParam->GetView())
		{
			CFrameWnd *pFrame = m_pViewParam->GetView()->GetParentFrame();
			if ( NULL != pFrame
				&& pFrame->SendMessage(UM_ISKINDOFCMPIChildFrame) && pFrame->SendMessage(UM_IsHideMode))
			{
				bHide = true;
			}
		}		
	}
	catch (...)
	{
	}
	if ( bHide )
	{
		return;
	}
	
	if(NULL != m_pViewParam->GetOverlayDC())
	{
		if ( !m_bFullOverlay )
		{
			m_pViewParam->GetOverlayDC()->Swap(m_RectView);
		}
		else
		{
			m_pViewParam->GetOverlayDC()->Swap();
		}
	}
	
}

#if 0

void CRegion::ReDrawAll()
{
	if ( !::IsWindow(m_pViewParam->m_pWnd->m_hWnd)) 
	{
		return;
	}

	//
	WINDOWPLACEMENT wp;
	m_pViewParam->m_pWnd->GetWindowPlacement(&wp);	
	if ( SW_HIDE == wp.showCmd || SW_MINIMIZE == wp.showCmd || SW_SHOWMINIMIZED == wp.showCmd )
	{
		return;
	}
	
	//
  	if ( NULL == m_pRegionRoot ) 
	{
		return;
	}

	//
  	if ( this != m_pRegionRoot )
  	{
  		m_pRegionRoot->ReDrawAll();
  		return;
  	}
	
	//
	CPaintDC dc(m_pViewParam->m_pWnd);
	
	CMemDCEx* pDC = m_pViewParam->GetMainDC();

	RegionArray Regions;
	GetRegions (m_pRegionRoot, Regions);
	
	int32 i = 0;
	int32 j = 0;
	int32 iSize = Regions.GetSize();

	//
	if ( CheckFlag(GetDrawFlag(), CRegion::KDrawTransform) )
	{
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				
				if (NULL == pRegion)
				{
					continue;
				}

				//
				if ( j == pRegion->m_iLevel )
				{
					// ...fangz 0416 ����ÿ��Region ����ҪPreDraw ��
					pRegion->OnPreDraw(pDC, m_pViewParam->GetPickDC());
				}
			}
		}
	}

	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				CRgn rgn;
				rgn.CreateRectRgnIndirect(pRegion->m_RectView);//���RectΪ(0,0,0,0)Ҳ����
				pDC->SelectClipRgn(&rgn);
				m_pViewParam->GetPickDC()->SelectClipRgn(&rgn);
				pRegion->OnDraw( pDC,m_pViewParam->GetPickDC());
				pDC->SelectClipRgn(NULL);
				m_pViewParam->GetPickDC()->SelectClipRgn(NULL);
				rgn.DeleteObject();
			}
		}
	}

// 	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
// 	{
// 		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
// 		{
// 			for ( i = 0; i < iSize; i ++ )
// 			{
// 				CRegion* pRegion = Regions.GetAt(i);
// 				if ( j == pRegion->m_iLevel )
// 				{
// 					CRgn rgn;
// 					rgn.CreateRectRgnIndirect(pRegion->m_RectView);//���RectΪ(0,0,0,0)Ҳ����
// 					pDC->SelectClipRgn(&rgn);
// 					m_pViewParam->GetPickDC()->SelectClipRgn(&rgn);
// 					pRegion->DrawNotify(pDC,m_pViewParam->GetPickDC(),ERDNAfterDrawAllRegion);
// 					pDC->SelectClipRgn(NULL);
// 					m_pViewParam->GetPickDC()->SelectClipRgn(NULL);
// 					rgn.DeleteObject();
// 				}
// 			}
// 		}
// 	}

	BeginOverlay();

	CMemDCEx* pOverLay = m_pViewParam->GetOverlayDC();
	
//	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
//	{
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{
					CRgn rgn;
					rgn.CreateRectRgnIndirect(pRegion->m_RectView);//���RectΪ(0,0,0,0)Ҳ����
					pOverLay->SelectClipRgn(&rgn);
					pRegion->OnAfterDraw(pOverLay);
					pOverLay->SelectClipRgn(NULL);
					rgn.DeleteObject();
				}
			}
		}
//	}

	EndOverlay();
	// m_pViewParam->GetPickDC()->Swap();//������.��PickDC�ϻ�����ʲô.
	Regions.RemoveAll();
	ClearDrawFlag();

}

#else
// ��ͼ�Ż�

void CRegion::ReDrawAll()
{
	if(NULL == m_pViewParam)
	{
		return;
	}

	if ( !::IsWindow(m_pViewParam->m_pWnd->m_hWnd)) 
	{
		return;
	}

	//
	WINDOWPLACEMENT wp;
	m_pViewParam->m_pWnd->GetWindowPlacement(&wp);	
	if ( SW_HIDE == wp.showCmd || SW_MINIMIZE == wp.showCmd || SW_SHOWMINIMIZED == wp.showCmd )
	{
		return;
	}
	
	//
  	if ( NULL == m_pRegionRoot ) 
	{
		return;
	}

	//
  	if ( this != m_pRegionRoot )
  	{
  		m_pRegionRoot->ReDrawAll();
  		return;
  	}
	
	//
	CPaintDC dc(m_pViewParam->m_pWnd);
	
	CMemDCEx* pDC = m_pViewParam->GetMainDC();
	if (NULL == pDC)
	{
		return;
	}

	RegionArray Regions;
	GetRegions (m_pRegionRoot, Regions);
	
	int32 i = 0;
	int32 j = 0;
	int32 iSize = Regions.GetSize();


	//
	if ( CheckFlag(GetDrawFlag(), CRegion::KDrawTransform) )
	{
		

		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				
				if (NULL == pRegion)
				{
					continue;
				}

				//
				if ( j == pRegion->m_iLevel )
				{
					// ...fangz 0416 ����ÿ��Region ����ҪPreDraw ��
					pRegion->OnPreDraw(pDC, m_pViewParam->GetPickDC());

				}
			}
		}

	}

	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				if(NULL != m_pViewParam->GetPickDC())
				{
					CRgn rgn;
					CRect rcRegion = pRegion->m_RectView;
					rcRegion.InflateRect(0, 0, 1, 1);
					rgn.CreateRectRgnIndirect(rcRegion);//���RectΪ(0,0,0,0)Ҳ����
					pDC->SelectClipRgn(&rgn);
					m_pViewParam->GetPickDC()->SelectClipRgn(&rgn);
					pRegion->OnDraw( pDC,m_pViewParam->GetPickDC());
					pDC->SelectClipRgn(NULL);
					m_pViewParam->GetPickDC()->SelectClipRgn(NULL);
					rgn.DeleteObject();
				}
				

			}
		}
	}

	BeginOverlay();

	CMemDCEx* pOverLay = m_pViewParam->GetOverlayDC();
	
//	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
//	{
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{

					CRgn rgn;
					rgn.CreateRectRgnIndirect(pRegion->m_RectView);//���RectΪ(0,0,0,0)Ҳ����
					//pOverLay->SelectClipRgn(&rgn);		// ���ƻ����п���ҪԽ��
					pRegion->OnAfterDraw(pOverLay);
					//pOverLay->SelectClipRgn(NULL);
					rgn.DeleteObject();

				}
			}
		}
//	}
	EndOverlay();
	// m_pViewParam->GetPickDC()->Swap();//������.��PickDC�ϻ�����ʲô.
	Regions.RemoveAll();
	ClearDrawFlag();
	
}

#endif

void CRegion::DrawNotify(CMemDCEx* pDC, IN CMemDCEx* pPickDC, E_RegionDrawNotifyType eType)
{
	CRegionDrawElement *pRegionDrawElements = (CRegionDrawElement *)m_aRegionDrawElements.GetData();
	for (int32 i = 0; i < m_aRegionDrawElements.GetSize(); i++)
	{
		if (NULL == pRegionDrawElements[i].m_pRegionDrawNotify)
			continue;

		if ( CheckFlag(pRegionDrawElements[i].m_uiDrawFlag, eType))
		{
			pRegionDrawElements[i].m_pRegionDrawNotify->OnRegionDrawNotify(pDC, pPickDC, this, eType);
		}
	}
}
bool32 CRegion::AddDrawNotify(CRegionDrawNotify* pDrawNotify, E_RegionDrawNotifyType eType)
{
	CRegionDrawElement *pRegionDrawElements = (CRegionDrawElement *)m_aRegionDrawElements.GetData();
	int32 i;
	for ( i = 0; i < m_aRegionDrawElements.GetSize(); i++)
	{
		if (pRegionDrawElements[i].m_pRegionDrawNotify == pDrawNotify)
		{
			break;
		}
	}

	if (i < m_aRegionDrawElements.GetSize())
	{
		AddFlag(pRegionDrawElements[i].m_uiDrawFlag, eType);
	}
	else
	{
		CRegionDrawElement RegionDrawElement;
		RegionDrawElement.m_pRegionDrawNotify = pDrawNotify;
		RegionDrawElement.m_uiDrawFlag = (int)eType;
		m_aRegionDrawElements.Add(RegionDrawElement);
	}
	return true;
}

bool32 CRegion::RemoveDrawNotify ( CRegionDrawNotify* pDrawNotify,E_RegionDrawNotifyType eType)
{
	CRegionDrawElement *pRegionDrawElements = (CRegionDrawElement *)m_aRegionDrawElements.GetData();
	int32 i;
	for ( i = 0; i < m_aRegionDrawElements.GetSize(); i++)
	{
		if (pRegionDrawElements[i].m_pRegionDrawNotify == pDrawNotify)
		{
			break;
		}
	}

	if (i < m_aRegionDrawElements.GetSize())
	{
		if (CheckFlag(pRegionDrawElements[i].m_uiDrawFlag, eType))
		{
			m_aRegionDrawElements.RemoveAt(i);
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
void CRegion::SetLevel( int32 iLevel )
{
	ASSERT( iLevel >=0 );
	ASSERT( iLevel < 256 );
	if (iLevel >= 0 && iLevel < 256)
	{
		m_iLevel = iLevel;
		if (NULL != m_pViewParam)
		{
			if ( m_iLevel > m_pViewParam->m_iLevelMax )
			{
				m_pViewParam->m_iLevelMax = m_iLevel;
			}
		}
	}
	
	
}

void CRegion::SetDrawFlag ( uint32 iFlag )
{
	if ( NULL == m_pRegionRoot ) return;
	if ( this != m_pRegionRoot )
	{
		m_pRegionRoot->SetDrawFlag(iFlag);
		return;
	}
	ClearDrawFlag();
	AddDrawFlag(iFlag);
}
//lint --e{570} suppress "Loss of sign (arg. no. 2) (int to unsigned long) "
////lint AddFlag�β�Ϊ�޷��ţ�CRegion::KDrawNoSwapö��ֵ�����з��ţ����ڵ������⣬�����޸İ취 modifyer weng.cx
void CRegion::AddDrawFlag ( uint32 iFlag )
{
	if ( NULL == m_pRegionRoot ) return;
	if ( this != m_pRegionRoot )
	{
		m_pRegionRoot->AddDrawFlag(iFlag);
		return;
	}
	if ( CheckFlag(iFlag,CRegion::KDrawTransform))
	{
		AddFlag(m_iFlag,CRegion::KDrawTransform);
	}
	if ( CheckFlag(iFlag,CRegion::KDrawCurves))
	{
		AddFlag(m_iFlag,CRegion::KDrawCurves);
	}
	if ( CheckFlag(iFlag,CRegion::KDrawNotify))
	{
		AddFlag(m_iFlag,CRegion::KDrawNotify);
	}
	if ( CheckFlag(iFlag,CRegion::KDrawNoSwap))
	{
		AddFlag(m_iFlag,CRegion::KDrawNoSwap);
	}
}
//lint --e{570} suppress "Loss of sign (arg. no. 2) (int to unsigned long) "
////lint AddFlag�β�Ϊ�޷��ţ�CRegion::KDrawNoSwapö��ֵ�����з��ţ����ڵ������⣬�����޸İ취 modifyer weng.cx
uint32 CRegion::GetDrawFlag ( )
{
	if ( NULL == m_pRegionRoot ) return 0;
	if ( this != m_pRegionRoot )
	{
		return m_pRegionRoot->GetDrawFlag();
	}
	uint32 iFlag = 0;
	if ( CheckFlag(m_iFlag,CRegion::KDrawTransform))
	{
		AddFlag(iFlag,CRegion::KDrawTransform);
	}
	if ( CheckFlag(m_iFlag,CRegion::KDrawCurves))
	{
		AddFlag(iFlag,CRegion::KDrawCurves);
	}
	if ( CheckFlag(m_iFlag,CRegion::KDrawNotify))
	{
		AddFlag(iFlag,CRegion::KDrawNotify);
	}
	if ( CheckFlag(m_iFlag,CRegion::KDrawNoSwap))
	{
		AddFlag(iFlag,CRegion::KDrawNoSwap);
	}
	return iFlag;
}
//lint --e{570} suppress "Loss of sign (arg. no. 2) (int to unsigned long) "
////lint RemoveFlag�β�Ϊ�޷��ţ�CRegion::KDrawNoSwapö��ֵ�����з��ţ����ڵ������⣬�����޸İ취 modifyer weng.cx
void CRegion::ClearDrawFlag()
{
	if ( NULL == m_pRegionRoot ) return;
	if ( this != m_pRegionRoot )
	{
		m_pRegionRoot->ClearDrawFlag();
		return;
	}
	RemoveFlag(m_iFlag,CRegion::KDrawTransform);
	RemoveFlag(m_iFlag,CRegion::KDrawCurves);
	RemoveFlag(m_iFlag,CRegion::KDrawNotify);
	RemoveFlag(m_iFlag,CRegion::KDrawNoSwap);
}
void CRegion::MessageAll(int32 mid,void* pData )
{
	if ( NULL == m_pRegionRoot ) return;
	if ( this != m_pRegionRoot )
	{
		m_pRegionRoot->MessageAll(mid,pData);
		return;
	}

	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, j, iSize = Regions.GetSize();
	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				pRegion->OnMessage(mid,pData);
			}
		}
	}
	Regions.RemoveAll();
}
void CRegion::OnMessage( int32 mid, void* pData )
{
	//
}
///////////////////////////////////////////////////////////////////////////////
bool32 CRegion::Timer ( int32 iTimerID )
{
	if (NULL == m_pViewParam)
	{
		return false;
	}
	if ( NULL == m_pRegionRoot ) 
	{
		return false;
	}
	if ( this != m_pRegionRoot ) return m_pRegionRoot->Timer(iTimerID);
	//if ( m_pViewParam->GetView()->GetFocus() == NULL ) return false;
	if ( KRegionMouseOverAWhileTimerId != iTimerID ) return false;
	//�����Root�͹���
	if ( CheckFlag(m_iFlag,CRegion::KMouseOverAWhile)) return true;
	
	
	CPoint pt;
	::GetCursorPos(&pt);
	if (NULL != m_pViewParam->GetView())
	{
		ScreenToClient(m_pViewParam->GetView()->GetSafeHwnd(),&pt);
		CRect Rect(pt.x-1,pt.y-1,pt.x+1,pt.y+1);
		if ( !Rect.PtInRect(m_pViewParam->m_PointMouse)) return true;

		//����ÿһ��Region��OnMouseOverAWhile
		RegionArray Regions;
		GetRegions ( m_pRegionRoot,Regions );
		int32 i, j, iSize = Regions.GetSize();
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if(NULL != pRegion)
				{
					if ( !CheckFlag(pRegion->m_iFlag,CRegion::KFrame ) && j == pRegion->m_iLevel )
					{
						AddFlag(pRegion->m_iFlag,CRegion::KMouseOverAWhile);
						//				m_pViewParam->_Log(_T("OnMouseOverAWhile\n"));
						pt = m_pViewParam->m_PointMouse;
						m_pViewParam->m_RectLast = CRect(pt.x-1,pt.y-1,pt.x+1,pt.y+1);
						pRegion->OnMouseOverAWhile(m_pViewParam->m_PointMouse.x,m_pViewParam->m_PointMouse.y);
					}
				}			
			}
		}
		Regions.RemoveAll();
		return true;
	}
	
	
	return false;
}
bool32 CRegion::TestKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	if ( NULL == m_pRegionRoot ) return false;
	if ( this != m_pRegionRoot ) return m_pRegionRoot->TestKeyDown(nChar,nRepCnt,nFlags);
	
	bool32 bRet = false;
	
	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, j, iSize = Regions.GetSize();
	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				if ( pRegion->OnKeyDown(nChar,nRepCnt,nFlags) )
				{
					bRet = true;
				}
			}
		}
	}
	Regions.RemoveAll();
	return bRet;
}
bool32 CRegion::TestChar(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	if ( NULL == m_pRegionRoot ) return false;
	if ( this != m_pRegionRoot ) return m_pRegionRoot->TestChar(nChar,nRepCnt,nFlags);
	
	bool32 bRet = false;
	
	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, j, iSize = Regions.GetSize();
	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				if ( pRegion->TestChar(nChar,nRepCnt,nFlags) )
				{
					bRet = true;
				}
			}
		}
	}
	Regions.RemoveAll();
	return bRet;
}
bool32 CRegion::TestKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	if ( NULL == m_pRegionRoot ) return false;
	if ( this != m_pRegionRoot ) return m_pRegionRoot->TestKeyUp(nChar,nRepCnt,nFlags);
	
	bool32 bRet = false;
	
	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, j, iSize = Regions.GetSize();
	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				if ( pRegion->TestKeyUp(nChar,nRepCnt,nFlags) )
				{
					bRet = true;
				}
			}
		}
	}
	Regions.RemoveAll();
	return bRet;
}

void CRegion::OnMousePress ( int32 x, int32 y, int32 iPass, bool bRightButton)
{
	// 	CPoint pt(x,y);
	// 	if ( !CheckFlag(m_iFlag,CRegion::KFrame) && m_RectView.PtInRect(pt))
	// 	{
	// 		CString StrText;
	// 		StrText.Format(_T("%s %d %d"),m_StrName, x, y );
	// 		AfxGetMainWnd()->SetWindowText ( StrText );
	// 	}
}
void CRegion::OnMousePressUp(int32 x, int32 y)
{

}
void CRegion::OnMouseRClick ( int32 x, int32 y )
{
}
void CRegion::OnMouseDBClick ( int32 x, int32 y )
{
}
void CRegion::OnMouseMove ( int32 x, int32 y )
{
}
void CRegion::OnMouseOverAWhile ( int32 x, int32 y )
{
}
void CRegion::OnMouseOverLeave ( int32 x, int32 y )
{
	
}
void CRegion::OnMouseDragMove ( int32 x, int32 y, bool bRightButton)
{
}
void CRegion::OnMouseDragDrop ( int32 x, int32 y, bool bRightButton )
{
}
void CRegion::OnMouseClickN ( int32 n, int32 x, int32 y )
{
}
void CRegion::OnMouseMoveN ( int32 n, int32 x, int32 y )
{
}
void CRegion::OnMouseEnd ( int32 n, int32 x, int32 y )
{
}
///////////////////////////////////////////////////////////////////////////////
/*
EMOTDefault = 0,//ȱʡ��������
EMOTPan,//����ƽ��
EMOTRuler,//���
EMOTUserDrawing //�Ի���,���ж���������,Ԥ����չ.
*/
int32 CRegion::RButtonDblClk( int32 iFlags, int32 x, int32 y )
{
	if (NULL == m_pRegionRoot)
	{
		return 0;
	}
	if ( this != m_pRegionRoot ) return m_pRegionRoot->RButtonDblClk( iFlags, x, y );
	//����;
	return 0;
}
int32 CRegion::RButtonUp( int32 iFlags, int32 x, int32 y )
{
	if ( this != m_pRegionRoot ) return m_pRegionRoot->RButtonUp( iFlags, x, y );

	//PressEdge��up
	if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePressEdge ))
	{
		//������
		RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMousePressEdge );
		//�ͷ�
		::ReleaseCapture();
		//�Ƿ���Drag����
		if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseDragEdge ))
		{
			//������
			RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMouseDragEdge );
			if ( m_pViewParam->m_bDragEdgeIn )
			{
				//����ʵ�ʵ�Resize
				int32 iNewX,iNewY;
				TestDragEdge ( x,y,iNewX,iNewY,true);
				if ( m_pViewParam->m_bDragEdgeIn )
				{
					SetDragForceSize( );
					NestedSizeReverse( );
					NestedSize(m_pRegionRoot);
				}
			}
			SetDrawFlag(CRegion::KDrawFull);
			//ReDrawAll();
			m_pViewParam->GetView()->Invalidate();
			//ɾ����һ��ʰȡ������
			m_pViewParam->m_PickEdges.RemoveAll();
			m_pViewParam->m_ForceRegions.RemoveAll();
		}
	}
	//Press��up
    if (m_pViewParam->m_bRightButPress /*CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePressRight )*/)
	{
		//������
		//RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMousePressRight );
		m_pViewParam->m_bRightButPress = false;
		//�ͷ�
		::ReleaseCapture();
		//�Ƿ���Drag����
		if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseDrag ))
		{
			//������
			RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMouseDrag );
			//�������ദ��
			RegionArray Regions;
			GetRegions ( m_pRegionRoot,Regions );
			int32 i, j, iSize = Regions.GetSize();
			for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CRegion* pRegion = Regions.GetAt(i);
					if ( j == pRegion->m_iLevel )
					{
						pRegion->OnMouseDragDrop( x, y, true );
					}
				}
			}			
			Regions.RemoveAll();
			ReDrawAll();//�Ƿ���Ҫȫ���ػ���?����pRegion->OnMouseDragDrop�����е���?
		}
		else
		{
			RegionArray Regions;
			GetRegions ( m_pRegionRoot,Regions );
			int32 i, j, iSize = Regions.GetSize();
			for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CRegion* pRegion = Regions.GetAt(i);
					if ( j == pRegion->m_iLevel )
					{
						pRegion->OnMouseRClick ( x, y );
					}
				}
			}

			Regions.RemoveAll();
		}
	}

	//�������:������ͨ��Click
	{
		//�������ദ��
		RegionArray Regions;
		GetRegions ( m_pRegionRoot,Regions );
		int32 i, j, iSize = Regions.GetSize();
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{
					pRegion->OnMousePressUp( x, y );
				}
			}
		}			
		Regions.RemoveAll();
	}
	return 1;

	//����;
	return 0;
}
int32 CRegion::MouseWheel ( int32 iFlags,int32 x,int32 y )
{
	if ( this != m_pRegionRoot ) return m_pRegionRoot->MouseWheel( iFlags, x, y );
	//����;
	return 0;
}

int32 CRegion::RButtonDown( int32 iFlags, int32 x, int32 y )
{
	if ( this != m_pRegionRoot ) return m_pRegionRoot->RButtonDown( iFlags, x, y );
	if ( EMOTDefault == m_eMouseOpType )
	{
		m_pViewParam->m_PointMouse = CPoint(x, y);
		// ɾ��ʰȡ�ı߿�����
		m_pViewParam->m_PickEdges.RemoveAll();
		NestedPickEdge(x,y,true);
		if ( m_pViewParam->m_PickEdges.GetSize() > 0 )   // ��ʱ�Ҽ�����˱߿򣬲�����
		{

		}
		else
		{
			//AddFlag( m_pViewParam->m_iFlag, CRegion::KMousePressRight );
			m_pViewParam->m_bRightButPress = true;
			m_pViewParam->GetView()->SetCapture();

			RegionArray Regions;
			GetRegions ( m_pRegionRoot,Regions );
			int32 i, j, iSize = Regions.GetSize();

			for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CRegion* pRegion = Regions.GetAt(i);
					if ( j == pRegion->m_iLevel )
					{
						pRegion->OnMousePress ( x, y, 0, true);
					}
				}
			}

			// fangz1110 # ����ʱ��,����λ
			for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CRegion* pRegion = Regions.GetAt(i);
					if ( j == pRegion->m_iLevel )
					{
						pRegion->OnMousePress ( x, y, 1, true);
					}
				}
			}

			Regions.RemoveAll();
		}

		//if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePress ) ||
		//	CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseDrag ))
		//{
		//}
		//else
		//{
		//	RegionArray Regions;
		//	GetRegions ( m_pRegionRoot,Regions );
		//	int32 i, j, iSize = Regions.GetSize();
		//	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		//	{
		//		for ( i = 0; i < iSize; i ++ )
		//		{
		//			CRegion* pRegion = Regions.GetAt(i);
		//			if ( j == pRegion->m_iLevel )
		//			{
		//				pRegion->OnMouseRClick ( x, y );
		//			}
		//		}
		//	}
		//	Regions.RemoveAll();
		//}
	}
	else if ( EMOTPan == m_eMouseOpType )
	{
	}
	else if ( EMOTRuler == m_eMouseOpType )
	{
	}
	else if ( EMOTUserDrawing == m_eMouseOpType )
	{
	}
	else
	{
		////ASSERT(0);
	}
	return 1;
}
int32 CRegion::LButtonDblClk( int32 iFlags, int32 x, int32 y )
{
	if(NULL == m_pViewParam)
	{
		return 1;
	}
	if ( this != m_pRegionRoot ) return m_pRegionRoot->LButtonDblClk( iFlags, x, y );
	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, j, iSize = Regions.GetSize();
	for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( j == pRegion->m_iLevel )
			{
				pRegion->OnMouseDBClick ( x, y );
			}
		}
	}
	Regions.RemoveAll();
	return 1;
}

int32 CRegion::MouseMove( int32 iFlags, int32 x, int32 y )
{
	if(NULL == m_pViewParam)
	{
		return 1;
	}
	if(NULL == m_pRegionRoot)
	{
		return 1;
	}

	if ( this != m_pRegionRoot ) return m_pRegionRoot->MouseMove( iFlags, x, y );

	CPoint pt(x,y);
	//m_pViewParam->m_PointMouse = pt;
	if ( pt == m_pViewParam->m_PointLastMouse ) return 0;
	

	// Ϊ����m_pRegionMain�п����ױߵ�����Ҳ��������cross�ƶ�����ʱֻ���ƻ�ģ������
	bool32 bIsActiveCross = false;
	CWnd *pWndOwer = m_pViewParam->GetView();
	if (NULL == pWndOwer)
	{
		return 1;
	}
	if ( !IsWindow(pWndOwer->GetSafeHwnd()) )
	{
		ASSERT( 0 );
		return 1;
	}
	
	if ( !pWndOwer->SendMessage(UM_ISKINDOFCIoViewChart))//IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
	{
		// Ŀǰ����region��ֻ����IoViewChart
		ASSERT( 0 );
	}
	else
	{
		//bIsActiveCross = ((CIoViewChart *)pWndOwer)->m_pRegionMain->IsActiveCross();
		bIsActiveCross = pWndOwer->SendMessage(UM_IsActiveCross);
	}
	
	NestedPickEdge ( x,y, false );
	if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePressEdge ))
	{
		AddFlag(m_pViewParam->m_iFlag, CRegion::KMouseDragEdge );
		//�϶�Edge,������Ƥ��
		BeginOverlay();
		int32 i, iNewX,iNewY, iSize = m_pViewParam->m_PickEdges.GetSize();
		if (  iSize > 0 )
		{
			TestDragEdge(x,y,iNewX,iNewY,false );
			CMemDCEx* pDC = m_pViewParam->GetOverlayDC();
			if (NULL != pDC)
			{
				int32 oldMode = pDC->GetBkMode();
				pDC->SetBkMode(TRANSPARENT);
				CPen pen,*pOldPen;
				pen.CreatePen(PS_DOT,1,m_clrBorder);
				pOldPen = (CPen*)pDC->SelectObject(&pen);
				for ( i = 0; i < iSize; i ++ )
				{
					T_PickEdge sPickEdge = m_pViewParam->m_PickEdges.GetAt(i);
					CRect Rect = sPickEdge.m_pRegion->m_RectView;
					switch(sPickEdge.m_eDir)
					{
					case EDLeft:
					case EDRight:
						pDC->_DrawLine ( iNewX, Rect.top, iNewX, Rect.bottom);
						break;
					case EDTop:
					case EDBottom:
						pDC->_DrawLine ( Rect.left, iNewY, Rect.right, iNewY);
						break;
					default:
						////ASSERT(0);
						break;
					}
				}
				pDC->SetBkMode(oldMode);
				pDC->SelectObject(pOldPen);
				pen.DeleteObject();
			}
		
		}
		EndOverlay();
	}
	else if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseOverEdge ) && !bIsActiveCross )
	{
		// ��activecross����£��п�����ͼ�ĵױ߽�����mouseOverEdge�߽磬���¼����ƶ�cross��ͬ��
		RemoveFlag(m_pViewParam->m_iFlag,CRegion::KMouseOverEdge);
	}
	else
	{
		if ( EMOTDefault == m_eMouseOpType )
		{
			BeginOverlay();
			int iXDelt = (pt.x - m_pViewParam->m_PointMouse.x) > 0 ? (pt.x - m_pViewParam->m_PointMouse.x):(m_pViewParam->m_PointMouse.x - pt.x);
			int iYDelt = (pt.y - m_pViewParam->m_PointMouse.y) > 0 ? (pt.y - m_pViewParam->m_PointMouse.y):(m_pViewParam->m_PointMouse.y - pt.y);
			///bool bRightPress = CheckFlag(m_pViewParam->m_iFlag, CRegion::KMousePressRight);
			bool bRightDrag = m_pViewParam->m_bRightButPress && ((iXDelt > 5) || (iYDelt > 5));
			if ( CheckFlag ( m_pViewParam->m_iFlag, CRegion::KMousePress) ||  bRightDrag)
			{
				AddFlag ( m_pViewParam->m_iFlag, CRegion::KMouseDrag );
				
				RegionArray Regions;
				GetRegions ( m_pRegionRoot,Regions );
				int32 i, j, iSize = Regions.GetSize();
				for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
				{
					for ( i = 0; i < iSize; i ++ )
					{
						CRegion* pRegion = Regions.GetAt(i);
						if ( j == pRegion->m_iLevel )
						{
							if(bRightDrag)
							{
								pRegion->OnMouseDragMove ( x, y,  true );
							}
							else
							{
								pRegion->OnMouseDragMove ( x, y, false );
							}
							
						}
					}
				}
				Regions.RemoveAll();
			}
			else
			{
				if (NULL != m_pViewParam->GetView())
				{
					::KillTimer(m_pViewParam->GetView()->GetSafeHwnd(),KRegionMouseOverAWhileTimerId);
				}
			
				RegionArray Regions;
				GetRegions ( m_pRegionRoot,Regions );
				int32 i, j, iSize = Regions.GetSize();
				for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
				{
					for ( i = 0; i < iSize; i ++ )
					{
						CRegion* pRegion = Regions.GetAt(i);
						if (NULL != pRegion)
						{
							if ( j == pRegion->m_iLevel )
							{
								if ( CheckFlag(pRegion->m_iFlag,CRegion::KMouseOverAWhile ))
								{
									if ( !m_pViewParam->m_RectLast.PtInRect(m_pViewParam->m_PointMouse ) )
									{
										//									m_pViewParam->_Log(_T("OnMouseOverLeave\n"));
										RemoveFlag(pRegion->m_iFlag,CRegion::KMouseOverAWhile);
										pRegion->OnMouseOverLeave ( x, y );
									}
								}
								pRegion->OnMouseMove ( x, y );
							}
						}						
					}
				}
				Regions.RemoveAll();
				::SetTimer(m_pViewParam->GetView()->GetSafeHwnd(),KRegionMouseOverAWhileTimerId,600,NULL);
			}
			EndOverlay();
			Sleep(1);
		}
		else if ( EMOTPan == m_eMouseOpType )
		{
		}
		else if ( EMOTRuler == m_eMouseOpType )
		{
		}
		else if ( EMOTUserDrawing == m_eMouseOpType )
		{
		}
		else
		{
			////ASSERT(0);
		}
	}
	m_pViewParam->m_PointLastMouse = pt;
	return 1;
}
void CRegion::AddToPickEdges ( E_Direction eDir )
{

	ASSERT( NULL != m_pRegionRoot );
	if (NULL == m_pRegionRoot)
	{
		return;
	}
	if (NULL == m_pViewParam)
	{
		return;
	}
	T_PickEdge sPickEdge;
	bool32 bAdd = false;
	if ( m_pViewParam->m_PickEdges.GetSize() < 1 )
	{
		bAdd = true;
	}
	else
	{
		//��������ˮƽ�϶��򵥶���ֱ�϶�
		sPickEdge = m_pViewParam->m_PickEdges.GetAt(0);
		if ( sPickEdge.m_eDir == EDRight &&
			eDir==EDRight )
		{
			bAdd = true;
		}
		if ( sPickEdge.m_eDir == EDBottom &&
			eDir==EDBottom )
		{
			bAdd = true;
		}
	}
	if ( bAdd )
	{
		sPickEdge.m_pRegion = this;
		sPickEdge.m_eDir = eDir;
		m_pViewParam->m_PickEdges.Add(sPickEdge);
	}
}
void CRegion::NestedPickEdge ( int32 x, int32 y, bool32 bClick)
{
	if (NULL == m_pViewParam)
	{
		return;
	}
	CRect Rect;
	CRect RectClient;
	//m_pViewParam->GetView()->GetClientRect(&RectClient);
	m_pViewParam->GetViewRegionRect(&RectClient);
	CPoint pt(x,y);
	if ( !(CheckFlag(m_iFlag,CRegion::KFrame)) )
	{
		if ( CheckFlag( m_iFlag, CRegion::KRightDragAble ))
		{
			Rect = CRect ( m_RectView.right-2,RectClient.top, m_RectView.right+2,RectClient.bottom);
			if ( Rect.PtInRect(pt))
			{
				SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				if ( bClick )
				{
					AddToPickEdges(EDRight);
				}
				else
				{
					AddFlag(m_pViewParam->m_iFlag,CRegion::KMouseOverEdge);
				}
			}
		}
		else if ( CheckFlag( m_iFlag, CRegion::KBottomDragAble ))
		{
			Rect = CRect ( RectClient.left,m_RectView.bottom-2,RectClient.right,m_RectView.bottom+2);
			if ( Rect.PtInRect(pt))
			{
				SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				if ( bClick )
				{
					AddToPickEdges(EDBottom);
				}
				else
				{
					AddFlag(m_pViewParam->m_iFlag,CRegion::KMouseOverEdge);
				}
			}
		}
	}
	if ( NULL != m_pRegionChild1 ) m_pRegionChild1->NestedPickEdge( x, y, bClick );
	if ( NULL != m_pRegionChild2 ) m_pRegionChild2->NestedPickEdge( x, y, bClick );
}

void CRegion::GetRegions ( IN CRegion* pParent,INOUT RegionArray& Regions)
{
	if ( NULL == pParent ) return;
	Regions.Add(pParent);
	if (NULL == m_pViewParam)
	{
		return;
	}
	if ( m_pViewParam->m_iLayerMax < pParent->m_iLayer ) m_pViewParam->m_iLayerMax = pParent->m_iLayer;
	GetRegions ( pParent->m_pRegionChild1,Regions);
	GetRegions ( pParent->m_pRegionChild2,Regions);
}
void CRegion::NestedSizeReverse ( )
{
	RegionArray Regions;
	GetRegions ( m_pRegionRoot,Regions );
	int32 i, iSize = Regions.GetSize();
	int32 iLayerMax = m_pViewParam->m_iLayerMax;
	while ( iLayerMax )
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CRegion* pRegion = Regions.GetAt(i);
			if ( iLayerMax == pRegion->m_iLayer )
			{
				CRegion* pParent = pRegion->m_pRegionParent;
				if (NULL == pParent)
				{
					return;
				}

				if (L"[Trend-split]" == pParent->m_StrName)
				{
					;
				}

				CRegion* pChild1 = pParent->m_pRegionChild1;
				CRegion* pChild2 = pParent->m_pRegionChild2;
				pParent->m_RectView = pChild1->m_RectView;
				
				if ( CheckFlag(pChild1->m_iFlag,CRegion::KFixHeight) )
				{
					if ( CheckFlag(pParent->m_iFlag,CRegion::KFrame) )
					{
						pParent->m_SizeWish.cy = pChild1->m_SizeWish.cy;
					}
				}

				if ( CheckFlag(pChild1->m_iFlag,CRegion::KFixWidth) )
				{
					if ( CheckFlag(pParent->m_iFlag,CRegion::KFrame) )
					{
						pParent->m_SizeWish.cx = pChild1->m_SizeWish.cx;
					}
				}

				switch( pChild1->m_eDirection) {
				case EDLeft:
					if ( pParent->m_RectView.right < pChild2->m_RectView.right )
					{
						pParent->m_RectView.right = pChild2->m_RectView.right;
					}
// 					if ( pParent->m_RectView.Width() <= GetRegionMinWH() )
// 					{
// 						pParent->m_fSizePercent = 100.0f;//
// 					}
// 					else
					{
						int32 iParentWidth = pParent->m_RectView.Width();
						int32 iChild1Width = pChild1->m_RectView.Width();
						if ( iParentWidth <= 0 && iChild1Width > 0 )
						{
							// ��region��ѹ����0�ˣ�������region�ı������䣬���Ŵ�ʱ�����԰�������ԭ
						}
						else
						{
							pParent->m_fSizePercent = ((float)pChild1->m_RectView.Width()) / (float)pParent->m_RectView.Width();
						}
						//pParent->m_fSizePercent = ((float)pChild1->m_RectView.Width()) / (float)pParent->m_RectView.Width();
					}
					break;
				case EDRight:
					if ( pParent->m_RectView.left > pChild2->m_RectView.left )
					{
						pParent->m_RectView.left = pChild2->m_RectView.left;
					}
// 					if ( pParent->m_RectView.Width() <= GetRegionMinWH() )
// 					{
// 						pParent->m_fSizePercent = 100.0f;//
// 					}
// 					else
					{
						int32 iParentWidth = pParent->m_RectView.Width();
						int32 iChild1Width = pChild1->m_RectView.Width();
						if ( iParentWidth <= 0 && iChild1Width > 0 )
						{
							// ��region��ѹ����0�ˣ�������region�ı������䣬���Ŵ�ʱ�����԰�������ԭ
						}
						else
						{
							pParent->m_fSizePercent = ((float)pChild1->m_RectView.Width()) / (float)pParent->m_RectView.Width();
						}
						//pParent->m_fSizePercent = ((float)pChild1->m_RectView.Width()) / (float)pParent->m_RectView.Width();
					}
					break;
				case EDTop:
					if ( pParent->m_RectView.bottom < pChild2->m_RectView.bottom )
					{
						pParent->m_RectView.bottom = pChild2->m_RectView.bottom;
					}
// 					if ( pParent->m_RectView.Width() <= GetRegionMinWH() )
// 					{
// 						pParent->m_fSizePercent = 100.0f;//
// 					}
// 					else
					{
						int32 iParentHeight = pParent->m_RectView.Height();
						int32 iChild1Height = pChild1->m_RectView.Height();
						if ( iParentHeight <= 0 && iChild1Height > 0 )
						{
							// ��region��ѹ����0�ˣ�������region�ı������䣬���Ŵ�ʱ�����԰�������ԭ
						}
						else
						{
							pParent->m_fSizePercent = ((float)pChild1->m_RectView.Height()) / (float)pParent->m_RectView.Height();
						}
						//pParent->m_fSizePercent = ((float)pChild1->m_RectView.Height()) / (float)pParent->m_RectView.Height();
					}
					break;
				case EDBottom:
					if ( pParent->m_RectView.top > pChild2->m_RectView.top )
					{
						pParent->m_RectView.top = pChild2->m_RectView.top;
					}
// 					if ( pParent->m_RectView.Width() <= GetRegionMinWH() )
// 					{
// 						pParent->m_fSizePercent = 100.0f;//
// 					}
// 					else
					{
						int32 iParentHeight = pParent->m_RectView.Height();
						int32 iChild1Height = pChild1->m_RectView.Height();
						if ( iParentHeight <= 0 && iChild1Height > 0 )
						{
							// ��region��ѹ����0�ˣ�������region�ı������䣬���Ŵ�ʱ�����԰�������ԭ
						}
						else
						{
							pParent->m_fSizePercent = ((float)pChild1->m_RectView.Height()) / (float)pParent->m_RectView.Height();
						}
						//pParent->m_fSizePercent = ((float)pChild1->m_RectView.Height()) / (float)pParent->m_RectView.Height();
					}
					break;
				default:
					////ASSERT(0);
					break;
				}
			}
		}
		iLayerMax --;
	}
}
void CRegion::SetDragForceSize( )
{
	if (NULL == m_pViewParam)
	{
		return;
	}

	int32 i, iSize = m_pViewParam->m_ForceRegions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		T_ForceRegion sForceRegion = m_pViewParam->m_ForceRegions.GetAt(i);		
		CRect Rect = sForceRegion.m_pRegion->m_RectView;
		
		switch ( sForceRegion.m_eDir ) {
		case EDLeft:
			Rect.left = sForceRegion.m_iNewValue;
			break;
		case EDRight:
			Rect.right = sForceRegion.m_iNewValue;
			break;
		case EDTop:
			Rect.top = sForceRegion.m_iNewValue;
			break;
		case EDBottom:
			Rect.bottom = sForceRegion.m_iNewValue;
			break;
		default:
			break;
		}
		sForceRegion.m_pRegion->m_RectView = Rect;
	}
}
void CRegion::FindNeighborRegion ( IN CRegion* pSrc,E_Direction eDir,IN CRegion* pParent, INOUT RegionArray& Regions )
{
	//����İ취ʵ��:
	if ( CheckFlag(pParent->m_iFlag,CRegion::KFrame))
	{
	}
	else
	{
		switch(eDir) {
		case EDLeft:
			if ( pParent->m_RectView.right == pSrc->m_RectView.left ) Regions.Add(pParent);
			break;
		case EDRight:
			if ( pParent->m_RectView.left == pSrc->m_RectView.right ) Regions.Add(pParent);
			break;
		case EDTop:
			if ( pParent->m_RectView.bottom == pSrc->m_RectView.top ) Regions.Add(pParent);
			break;
		case EDBottom:
			if ( pParent->m_RectView.top == pSrc->m_RectView.bottom ) Regions.Add(pParent);
			break;		
		default:
			break;
		}
	}
	if ( NULL != pParent->m_pRegionChild1 )
	{
		ASSERT(NULL != pParent->m_pRegionChild2 );
		FindNeighborRegion ( pSrc, eDir, pParent->m_pRegionChild1,Regions );
		FindNeighborRegion ( pSrc, eDir, pParent->m_pRegionChild2,Regions );
	}
}
void CRegion::TestDragEdge ( int32 x, int32 y, OUT int32& iNewX, OUT int32& iNewY, bool32 bSave )
{
	if (NULL == m_pViewParam)
	{
		return;
	}

	int32 i = 0,j = 0,iNewValue = 0,iSize = m_pViewParam->m_PickEdges.GetSize();
	m_pViewParam->m_bDragEdgeIn = true;
	iNewX = x;
	iNewY = y;
	RegionArray Regions;
	CRegion* pRegion = NULL;
	E_Direction eDirForce = EDLeft;
	
	for ( i = 0; i < iSize; i ++ )
	{
		T_PickEdge sPickEdge = m_pViewParam->m_PickEdges.GetAt(i);
		
		//�����н���m_eDir��(Edge)�ķ�KFrame���͵�Region
		FindNeighborRegion ( sPickEdge.m_pRegion,sPickEdge.m_eDir,m_pRegionRoot,Regions);
		
		CRect Rect = sPickEdge.m_pRegion->m_RectView;
		switch(sPickEdge.m_eDir) {
		case EDLeft://�϶���Edge
			////ASSERT(0);
			break;
		case EDRight://�϶���Edge
			eDirForce = EDLeft;
			iNewValue = x;
			if ( x < Rect.right )//����
			{
				if ( (x-Rect.left) <= GetRegionMinWH() )
				{
					iNewX = Rect.left + GetRegionMinWH();
					m_pViewParam->m_bDragEdgeIn = false;
					break;
				}
			}
			else if ( x > Rect.right )//����
			{
				for ( j = 0; j < Regions.GetSize(); j ++ )
				{
					pRegion = Regions.GetAt(j);
					//���ж�FixWidth,��ΪKXXXDragAble��FixXXXX�ڴ���Region��ʱ��ͽ����˹���������ж�.
					if ( (pRegion->m_RectView.right-x) <= GetRegionMinWH() )
					{
						iNewX = pRegion->m_RectView.right - GetRegionMinWH();
						m_pViewParam->m_bDragEdgeIn = false;
						break;
					}
				}				
			}
			else//���
			{
			}			
			break;
		case EDTop://�϶���Edge
			////ASSERT(0);
			break;
		case EDBottom://�϶���Edge
			eDirForce = EDTop;
			iNewValue = y;
			if ( y < Rect.bottom )//����
			{
				if ( (y-Rect.top) <= GetRegionMinWH() )
				{
					iNewY = Rect.top + GetRegionMinWH();
					m_pViewParam->m_bDragEdgeIn = false;
				}
			}
			else if ( y > Rect.bottom )//����
			{
				for ( j = 0; j < Regions.GetSize(); j ++ )
				{
					pRegion = Regions.GetAt(j);
					//���ж�FixHeight,��ΪKXXXDragAble��FixXXXX�ڴ���Region��ʱ��ͽ����˹���������ж�.
					if ( (pRegion->m_RectView.bottom-y) <= GetRegionMinWH() )
					{
						iNewY = pRegion->m_RectView.bottom - GetRegionMinWH();
						m_pViewParam->m_bDragEdgeIn = false;
						break;
					}
				}
			}
			else//���
			{
			}			
			break;
		default:
			////ASSERT(0);
			break;
		}
	}
	if ( bSave )
	{
		iSize = Regions.GetSize();
		for ( i = 0; i < iSize; i ++ )
		{
			T_ForceRegion sForceRegion;
			sForceRegion.m_eDir = eDirForce;
			sForceRegion.m_iNewValue = iNewValue;
			sForceRegion.m_pRegion = Regions.GetAt(i);
			m_pViewParam->m_ForceRegions.Add(sForceRegion);
		}
		iSize = m_pViewParam->m_PickEdges.GetSize();
		for ( i = 0; i < iSize; i ++ )
		{
			T_PickEdge sPickEdge = m_pViewParam->m_PickEdges.GetAt(i);
			T_ForceRegion sForceRegion;
			sForceRegion.m_eDir = sPickEdge.m_eDir;
			sForceRegion.m_pRegion = sPickEdge.m_pRegion;
			sForceRegion.m_iNewValue = iNewValue;
			m_pViewParam->m_ForceRegions.Add(sForceRegion);			
		}
	}
}
int32 CRegion::LButtonDown( int32 iFlags, int32 x, int32 y )
{
	if(NULL == m_pViewParam)
	{
		return 1;
	}
	if ( this != m_pRegionRoot ) return m_pRegionRoot->LButtonDown( iFlags, x, y );
	
	//ɾ����һ��ʰȡ������
	m_pViewParam->m_PickEdges.RemoveAll();
	m_pViewParam->m_ForceRegions.RemoveAll();
	//���ȼ���Ƿ�ʰȡ���˱߿�
	m_pViewParam->m_bDragEdgeIn = true;
	m_pViewParam->m_PointMouse = CPoint(x, y);
	NestedPickEdge(x,y,true);
	if ( m_pViewParam->m_PickEdges.GetSize() > 0 )
	{
		//		CString StrText = _T("Select ");
		//		for ( int32 i = 0; i < m_pViewParam->m_PickEdges.GetSize(); i ++ )
		//		{
		//			T_PickEdge sPickEdge = m_pViewParam->m_PickEdges.GetAt(i);
		//			StrText = StrText + sPickEdge.m_pRegion->m_StrName + DirToString(sPickEdge.m_eDir) + _T("|");
		//		}
		//		AfxGetMainWnd()->SetWindowText(StrText);
		AddFlag( m_pViewParam->m_iFlag, CRegion::KMousePressEdge );
		m_pViewParam->GetView()->SetCapture();
	}
	else
	{
		AddFlag( m_pViewParam->m_iFlag, CRegion::KMousePress );
		m_pViewParam->GetView()->SetCapture();
		
		RegionArray Regions;
		GetRegions ( m_pRegionRoot,Regions );
		int32 i, j, iSize = Regions.GetSize();
		
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{
					pRegion->OnMousePress ( x, y, 0 );
				}
			}
		}
		// fangz1110 # ����ʱ��,����λ
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{
					pRegion->OnMousePress ( x, y, 1 );
				}
			}
		}
		
		Regions.RemoveAll();
	}
	return 1;
}
int32 CRegion::LButtonUp( int32 iFlags, int32 x, int32 y )
{
	if ( this != m_pRegionRoot ) return m_pRegionRoot->LButtonUp( iFlags, x, y );
	if (NULL == m_pViewParam)
	{
		return 0;
	}
	//PressEdge��up
	if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePressEdge ))
	{
		//������
		RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMousePressEdge );
		//�ͷ�
		::ReleaseCapture();
		//�Ƿ���Drag����
		if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseDragEdge ))
		{
			//������
			RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMouseDragEdge );
			if ( m_pViewParam->m_bDragEdgeIn )
			{
				//����ʵ�ʵ�Resize
				int32 iNewX,iNewY;
				TestDragEdge ( x,y,iNewX,iNewY,true);
				if ( m_pViewParam->m_bDragEdgeIn )
				{
					SetDragForceSize( );
					NestedSizeReverse( );
					NestedSize(m_pRegionRoot);
				}
			}
			SetDrawFlag(CRegion::KDrawFull);
			//ReDrawAll();
			m_pViewParam->GetView()->Invalidate();
			//ɾ����һ��ʰȡ������
			m_pViewParam->m_PickEdges.RemoveAll();
			m_pViewParam->m_ForceRegions.RemoveAll();
		}
	}
	//Press��up
	else if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMousePress ))
	{
		//������
		RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMousePress );
		//�ͷ�
		::ReleaseCapture();
		//�Ƿ���Drag����
		if ( CheckFlag( m_pViewParam->m_iFlag, CRegion::KMouseDrag ))
		{
			//������
			RemoveFlag(m_pViewParam->m_iFlag, CRegion::KMouseDrag );
			//�������ദ��
			RegionArray Regions;
			GetRegions ( m_pRegionRoot,Regions );
			int32 i, j, iSize = Regions.GetSize();
			for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CRegion* pRegion = Regions.GetAt(i);
					if ( j == pRegion->m_iLevel )
					{
						pRegion->OnMouseDragDrop( x, y );
					}
				}
			}			
			Regions.RemoveAll();
			//ReDrawAll();//�Ƿ���Ҫȫ���ػ���?����pRegion->OnMouseDragDrop�����е���?
		}
	}

	//�������:������ͨ��Click
	{
		//�������ദ��
		RegionArray Regions;
		GetRegions ( m_pRegionRoot,Regions );
		int32 i, j, iSize = Regions.GetSize();
		for ( j = m_pViewParam->m_iLevelMax; j >= 0; j --)
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CRegion* pRegion = Regions.GetAt(i);
				if ( j == pRegion->m_iLevel )
				{
					pRegion->OnMousePressUp( x, y );
				}
			}
		}			
		Regions.RemoveAll();
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////////
bool32 CRegion::OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	return false;
}
bool32 CRegion::OnChar(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	return false;
}
bool32 CRegion::OnKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	return false;
}
///////////////////////////////////////////////////////////////////////////////
void CRegion::OnPick ( uint32 uiPickColor, int32 x, int32 y )
{
}
void CRegion::OnPick ( CString StrPickName, int32 x, int32 y )
{
}
int32 CRegion::GetRegionMinWH ()
{
	return KRegionMinWH;
}
///////////////////////////////////////////////////////////////////////////////
CString CRegion::ToString()
{
	CString StrText = _T("");
	
	CString StrDir = DirToString ( m_eDirection ) + _T(" Parent:");
	if ( NULL == m_pRegionParent )
	{
		StrDir += _T("Root");
	}
	else
	{
		StrDir += m_pRegionParent->m_StrName;
	}
	
	StrText.Format( _T("%s Flag:%d Layer:%d Level:%d Rect:[%d %d %d %d] Dir:%s\r\n"), 
		(LPCTSTR)m_StrName,
		m_iFlag,
		m_iLayer,
		m_iLevel,
		m_RectView.left,
		m_RectView.top,
		m_RectView.right,
		m_RectView.bottom,
		(LPCTSTR)StrDir);
	return StrText;	
}
CString CRegion::ToStringAll()
{
	CString StrText = _T("");
	if ( NULL == m_pRegionRoot ) return StrText;
	RegionArray Regions;
	GetRegions(m_pRegionRoot,Regions);
	int32 i, iSize = Regions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		StrText += Regions.GetAt(i)->ToString();
	}
	Regions.RemoveAll();
	return StrText;
}
CString CRegion::DirToString ( E_Direction eDir )
{
	CString StrText = _T("Unknown");
	switch( eDir) {
	case EDLeft:
		StrText = _T("Left");
		break;
	case EDRight:
		StrText = _T("Right");
		break;
	case EDTop:
		StrText = _T("Top");
		break;
	case EDBottom:
		StrText = _T("Bottom");
		break;		
	default:
		break;
	}
	return StrText;
}


///////////////////////////////////////////////////////////////////////////////
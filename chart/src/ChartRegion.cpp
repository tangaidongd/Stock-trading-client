// ChartRegionBase.cpp: implementation of the CChartRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChartRegion.h"
#include "Region.h"
#include "ShareFun.h"
#include "tinyxml.h"
#include <float.h>
//#include "viewdata.h"
#include "GdiPlusTS.h"
#include "XLTimerTrace.h"
#include "SaneIndex.h"
#include "DlgSetText.h"
#include "TabSplitWnd.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TIME 500

extern HWND g_hwndTrace;
class CIoViewChart;
//
const char* KIoViewChartSelfDrawRgn		= "region";
const char* KIoViewChartSelfDrawRgnID	= "id";
const char* KIoViewChartSelfDrawNum		= "selfdrawcount";
class CIoViewChart;

//////////////////////////////////////////////////////////////////////////
// ���ܰ�ť

static const T_FuctionButton s_astRegionButtons[] =
{
	T_FuctionButton(L"ǰ",		L"ǰһָ��",		REGIONTITLELEFTBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"ָ",		L"ѡ��ָ��",		REGIONTITLEFOMULARBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"��",		L"��һָ��",		REGIONTITLERIGHTBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	
	T_FuctionButton(L"+",		L"���Ӹ�ͼ",		REGIONTITLEADDBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"��",		L"ɾ����ǰ��ͼ",	REGIONTITLEDELBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
};

// ���ܰ�ť����
static const int KiRegionButtonNums = sizeof(s_astRegionButtons)/sizeof(T_FuctionButton);

#define  INVALID_ID -1

//////////////////////////////////////////////////////////////////////////
CChartRegionViewParam::CChartRegionViewParam(IN CWnd* pWnd )
:CRegionViewParam(pWnd)
{
	m_pDragRegion = NULL;
	m_pDragCurve  = NULL;
	m_pDragCurveSD= NULL;
	m_PointPress  = CPoint(-1,-1);
	m_PointRectZoomOutEnd = CPoint(-1,-1);
	m_bPressXAxis = false;
	m_bRefreshLast	= false;
}

CChartRegionViewParam::~CChartRegionViewParam()
{
	//modify by weng.cx
	while(m_Nodes.GetSize() != 0)
	{
		delete m_Nodes.GetAt(0);
	}
}

CNodeSequence* CChartRegionViewParam::CreateNodes ( )
{
	CNodeSequence* pNodes = new CNodeSequence();
	m_Nodes.Add(pNodes);
	return pNodes;
}

void CChartRegionViewParam::RemoveNodes ( CNodeSequence* pNodes )
{
	int32 i, iSize = m_Nodes.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( m_Nodes.GetAt(i) == pNodes )
			break;
	}
	if ( i < iSize )
	{
		m_Nodes.RemoveAt(i);
	}
}

int32 CChartRegionViewParam::GetNodesNum()
{
	return m_Nodes.GetSize();
}

CNodeSequence* CChartRegionViewParam::GetNodes( int32 iPos)
{
	int32 iSize = m_Nodes.GetSize();
	if ( iPos < 0 || iPos >= iSize )
	{
		return NULL;
	}
	return m_Nodes.GetAt(iPos);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
E_SelfDrawType CChartRegion::m_eSelfDrawType = ESDTInvalid;
int32 CChartRegion::m_iSelfDrawAppendOp = 0;
int32 CChartRegion::m_iPreNodePos = -1;

CChartRegion::CChartRegion( IN CRegionViewParam* pViewParam,CChartRegion*& pRegion,CString StrName, uint32 iFlag, CNodeSequence* pDefaultNodes )
:CRegion(pViewParam,StrName,iFlag)
{
	pRegion = this;
	//�����������һ��
	int32 iFlagInc = 0;
	if ( CheckFlag(m_iFlag,CChartRegion::KChart) )		iFlagInc ++;
	if ( CheckFlag(m_iFlag,CChartRegion::KUserChart) )	iFlagInc ++;
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis) )		iFlagInc ++;
	if ( CheckFlag(m_iFlag,CChartRegion::KYAxis) )		iFlagInc ++;
	ASSERT( iFlagInc == 1 );

	//��������,����Pan/HScale/CrossAble
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis) ||
		 CheckFlag(m_iFlag,CChartRegion::KYAxis))
	{
		if ( CheckFlag(m_iFlag,CChartRegion::KPanAble) || 
			 CheckFlag(m_iFlag,CChartRegion::KHScaleAble) ||
			 CheckFlag(m_iFlag,CChartRegion::KCrossAble ))
		{
			////ASSERT(0);
		}
	}
	//ֻ��x��ſ����еı�־
	if ( CheckFlag(m_iFlag,CChartRegion::KDragXAxisPanAble))
	{
		ASSERT(CheckFlag(m_iFlag,CChartRegion::KXAxis));
	}
	//����ʹ���ڲ��ñ�־
	ASSERT(!CheckFlag(m_iFlag,CChartRegion::KLockMouseMove));

	m_iNodeCross		= 0;
	m_bActiveCross		= false;
	m_StrTitle			= _T("");

	m_ptCross			= CPoint(0,0);

	m_clrSliderText		= 0xc0c0c0; //RGB(255,255,255);
	m_FontAxisText		= NULL;
	m_clrSliderBorder	= RGB(255,0,0);
	m_clrSliderFill		= RGB(0, 0, 128); //RGB(0,255,255);
	m_clrTitle			= 0xc0c0c0; //RGB(255,255,255);
	m_FontTitle			= NULL;
	m_clrAxisText		= RGB(255,0,0);
	m_FontSliderText	= NULL;
	m_clrAxisRule		= RGB(255,0,0);
	m_clrCross			= RGB(180,180,180);

	m_iLeftSkip			= 0;
	m_iRightSkip		= 0;
	m_iTopSkip			= 0;
	m_iBottomSkip		= 0;
	m_pDataSourceRegion	= NULL;

	m_iAxisTextMaxWidth	= 64;
	m_iAxisTextMaxHeight= 18;

	m_iJumpNodeID		= -1;
	m_iJumpNodePos		= -1;
	m_iPreNodeID		= -1;
	
	m_bSplitWndDrag		= false;
	m_iZoomOutWishNums	= 0;
	m_bAbleToRectZoomOut= true;
	m_iMouseMoveTrendIndexPos = -1;

    m_dwTickCnt = 0;

	m_pImgSet   = NULL;

	m_iIndexBtnHovering = INVALID_ID;
	InitIndexBtn();
	
	ClearSelfDrawValue();
	GetViewData()->OnRegionCurvesNumChanged(this,0);
	
	if (!m_TipWnd.Create(GetParentIoViewData().pWnd))
	{
		////ASSERT(0);
	}	
}

void CChartRegion::Destruct()
{
	// ...fangz0826 ����Bug, ɾ����ͼ��ʱ��, �����Region ������.����Region �ĳ�Ա��������ChartRegion ����,
	// û�е���CChartRegion ������,�ᵼ���ڴ�й©�ͻ�ͼʱ���Bug, ������ɾ����ͼ��ʱ����ʾ����һ������
	m_TipWnd.DestroyWindow();

	//lint --e{438}
	while ( m_Curves.GetSize() > 0 )
	{
		CChartCurve* pCurve = m_Curves.GetAt(0); 
		m_Curves.RemoveAt(0);
		DEL(pCurve);		
	}	
	m_YDependentRegions.RemoveAll();

	//modify by weng.cx
	while(m_SelfDrawingCurves.GetSize())
	{
		CSelfDrawCurve* p = m_SelfDrawingCurves.GetAt(0); 
		m_SelfDrawingCurves.RemoveAt(0);
		DEL(p);		
	}
}
//lint -e{1579}
CChartRegion::~CChartRegion()
{
	DEL(m_pImgSet);
	Destruct();
}

bool32 CChartRegion::AddYDependentRegion ( CChartRegion* pRegion)
{
	int32 i,iSize = m_YDependentRegions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( pRegion == m_YDependentRegions.GetAt(i))
		{
			break;
		}
	}
	
	if ( i < iSize )
	{
		return false;
	}
	
	m_YDependentRegions.Add(pRegion);
	AddFlag(pRegion->m_iFlag,CChartRegion::KYDependent);
	
	return true;
}

bool32 CChartRegion::RemoveYDependentRegion ( CChartRegion* pRegion)
{
	int32 i,iSize = m_YDependentRegions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( pRegion == m_YDependentRegions.GetAt(i))
		{
			break;
		}
	}
	if ( i < iSize )
	{
		return false;
	}
	m_YDependentRegions.RemoveAt(i);
	RemoveFlag(pRegion->m_iFlag,CChartRegion::KYDependent);
	return true;
}

CChartCurve* CChartRegion::CreateCurve ( uint32 iFlag )
{
	CChartCurve* pCurve = CurveNew(iFlag);

// #ifdef _DEBUG
// 	CString StrLog;
// 	StrLog.Format(_T("CreateCurve 0x%08x"),pCurve);
// 	_Log ( StrLog);
// #endif// _DEBUG

	m_Curves.Add(pCurve);
	GetViewData()->OnRegionCurvesNumChanged(this,m_Curves.GetSize());
	if ( CheckFlag(iFlag,CChartCurve::KDependentCurve))
	{
		pCurve->SetDependent();
	}
	return pCurve;
}

CChartCurve* CChartRegion::CurveNew ( int32 iFlag )
{
	 return new CChartCurve(*this,iFlag);
}

//lint --e{438}
bool32 CChartRegion::RemoveCurve ( CChartCurve* pCurve)
{
	int32 i, iSize = m_Curves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( m_Curves.GetAt(i) == pCurve )
		{
			break;
		}
	}
	if ( i < iSize )
	{
		m_Curves.RemoveAt(i);
// #ifdef _DEBUG
// 	CString StrLog;
// 	StrLog.Format(_T("DeleteCurve 0x%08x"),pCurve);
// 	_Log ( StrLog);
// #endif// _DEBUG

		DEL(pCurve);
		GetViewData()->OnRegionCurvesNumChanged(this,iSize-1);
		return true;
	}
	return false;
}

CChartCurve* CChartRegion::GetCurve ( int32 iPos )
{
	int32 iSize = m_Curves.GetSize();
	if ( iPos < 0 || iPos >= iSize )
	{
		return NULL;
	}
	return m_Curves.GetAt(iPos);
}

CChartCurve* CChartRegion::GetDependentCurve()
{
	int32 i, iSize = m_Curves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = m_Curves.GetAt(i);
		if (NULL != pCurve)
		{
			if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KDependentCurve) )
			{
				return pCurve;
			}
		}
		
	}
	
	return NULL;
}

void CChartRegion::OnDependentChanged ( CChartCurve* pCurve )
{
	int32 i, iSize = m_Curves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pTmpCurve = m_Curves.GetAt(i);
		RemoveFlag(pTmpCurve->m_iFlag,CChartCurve::KDependentCurve);
	}
	AddFlag(pCurve->m_iFlag,CChartCurve::KDependentCurve);
}

void CChartRegion::ClipY ( int32& y )
{
	CRect rect = GetRectCurves();
	if ( y >= rect.bottom )
	{
		y = rect.bottom -1;
	}
	if ( y < rect.top )
	{
		y = rect.top;
	}
}

CRect CChartRegion::GetRectCurves()
{
	m_RectCurves.left = m_RectView.left + m_iLeftSkip;
	m_RectCurves.top = m_RectView.top + m_iTopSkip;
	m_RectCurves.right = m_RectView.right - m_iRightSkip;
	m_RectCurves.bottom = m_RectView.bottom - m_iBottomSkip;
	return m_RectCurves;
}

bool32 CChartRegion::FlyCross ( int32 iCross, bool32 bLeft )
{
	CChartCurve* pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	CNodeSequence* pNodes = pCurveDependent->GetNodes();
	if ( NULL == pNodes )
	{
		return false;
	}
	int32 i, iSize = pNodes->GetSize();
	if ( bLeft )
	{
		for ( i = iCross; i >= 0; i -- )
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);
			if ( !CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
			{
				break;
			}
		}
	}
	else
	{
		for ( i = iCross; i < iSize ; i ++ )
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);
			if ( !CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
			{
				break;
			}
		}
	}
	if ( i >= 0 && i < iSize )
	{
		m_iNodeCross = i;
	}
	else
	{
	}
	return true;
}

void CChartRegion::OnMouseDBClick( int32 x, int32 y )
{
	//IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
	//if (NULL != pParent)
	//{
	//	if ( pParent->m_pRegionXBottom->m_RectView.PtInRect(CPoint(x, y)) ) //  ˫��x��Region�����κη�ӳ
	//	{
	//		return;
	//	}
	//}

	//if ( CheckFlag(m_iFlag,CChartRegion::KCrossAble ))
	//{
	//	if ( GetTopSkip() < 6 )
	//	{
	//		int32 i=0;
	//	}
	//	m_bActiveCross = !m_bActiveCross;		
	//	if ( m_bActiveCross )
	//	{
	//		m_ptCross = CPoint(x,y);
	//		OnMouseMove(x,y);
	//		// ˫������ʮ�ֹ��
	//		GetViewData()->GetOverlayDC()->Swap();
	//	}
	//	else
	//	{
	//		// ����ʾ.
	//		DrawIndexValueByMouseMove(0,0,true);
	//		
	//		CNodeData NodeData;
	//		GetViewData()->OnCrossData(this,0,NodeData,0.0f,0.0f,false);	
	//	}
	//}

	// �ҵ�������Ӧ��ʱ��������
	if ( m_RectView.PtInRect(CPoint(x, y)) )
	{
		CChartCurve* pCurveDependent = GetDependentCurve();
		if (NULL != pCurveDependent)
		{
			int32 rx = x;
			int32 iNode;
			ClientXToRegion(rx);
			pCurveDependent->RegionXToCurvePos(rx, iNode);	
			GetViewData()->OnChartDBClick(CPoint(x, y), iNode);
		}
	}
}

bool32 CChartRegion::OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags)
{
	// esc������Ҫ��ô����Ϣ
	if ( VK_ESCAPE == nChar )
	{	
		bool32 bNeedGGTongDeal = !m_bActiveCross;
		
		ActiveCross(false);
		CNodeData NodeData;
		GetViewData()->OnCrossData(this,0,NodeData,0.0f,0.0f,false);
		BeginOverlay(false);
		EndOverlay();
		
		DrawIndexValueByMouseMove(0,0,true);
		//ReDrawAll();
		GetParentIoViewData().pWnd->Invalidate();
		
		SetGGTongESCFlag();

		if (bNeedGGTongDeal)
		{			
			return false;
		}
		else
		{
			return true;
		}
	}

	CChartCurve* pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	CNodeSequence* pNodes = pCurveDependent->GetNodes();
	if ( NULL == pNodes )
	{
		return false;
	}
	

	
	//ɾ��ѡ�����?
	if ( VK_DELETE == nChar )
	{
		OnKeyDel();
		return true;
	}
	return false;//������,�򷵻�false
}

void CChartRegion::OnMessage( int32 mid, void* pData )
{
	CNodeData NodeData;
	switch(mid)
	{
	case KMsgInactiveCross:
		ActiveCross(false);
		GetViewData()->OnCrossData(this,0,NodeData,0.0f,0.0f,false);
		break;
	default:
		break;
	}
}

void CChartRegion::OnMouseRClick ( int32 x, int32 y )
{
	CPoint pt(x,y);
	if ( CheckFlag(m_iFlag,CRegion::KFrame)) return;
	if ( !m_RectView.PtInRect(pt)) return;
	CDrawingCurve* pDrawingCurve = NULL;
	CChartCurve* pCurve = NULL;
	CNodeSequence* pNodes = NULL;
	CNodeData NodeData;
	if ( PickNode ( x,y,pDrawingCurve,pNodes,NodeData) )
	{
		SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
		GetViewData()->GetView()->Invalidate();
	}

	bool32 bSDMenu = false;
	if ( NULL == pDrawingCurve )
	{
		pCurve = NULL;
	}
	else
	{
		if ( pDrawingCurve->m_bSelfDraw )
		{
			pCurve = NULL;
			bSDMenu = true;
			GetViewData()->OnRegionMenu2 ( this,pDrawingCurve,x,y );
		}
		else if ( CheckFlag(pDrawingCurve->m_iFlag, CDrawingCurve::KSelect) && CheckFlag(pDrawingCurve->m_iFlag, CChartCurve::KTypeIndex))
		{
			pCurve = NULL;
			bSDMenu = true;
			GetViewData()->OnRegionIndexMenu ( this,pDrawingCurve,x,y );
		}
		else
		{
			pCurve = (CChartCurve*)pDrawingCurve;
		}
	}
	if ( !bSDMenu )
	{
		GetViewData()->OnRegionMenu ( this,pCurve,pNodes,&NodeData,x,y );
	}
}

void CChartRegion::OnMousePress ( int32 x, int32 y, int32 iPass, bool bRightButton )
{	
	CPoint pt(x,y);	
	//
	if ( 0 == iPass )
	{
		GetViewData()->m_pDragCurveSD	= NULL;
		GetViewData()->m_pDragCurve		= NULL;
		GetViewData()->m_pDragRegion	= NULL;
		GetViewData()->m_bPressXAxis	= false;

		return;
	}

	int iButton = TIndexButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapIndexBtn[iButton].LButtonDown(FALSE);
		CRect rtBtn;
		DrawIndexBtn(NULL, m_iIndexBtnHovering, rtBtn);
	//	return;
	}
	
	bool32 bPicked = false;

	//
	GetViewData()->m_PointPress = pt;
	GetViewData()->m_PointRectZoomOutEnd = GetViewData()->m_PointPress;

	//
	if ( CheckFlag(m_iFlag,CRegion::KFrame))
	{
		return;
	}

	if(bRightButton)
	{
		if ( CheckFlag(m_iFlag,CChartRegion::KXAxis))
		{
			if ( m_RectView.PtInRect(CPoint(x,y)))
			{
				GetViewData()->m_bPressXAxis = true;
				//IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
				//if (NULL != pParent)
				//{
				//	::SetCursor(LoadCursor(  NULL, IDC_NO ));
				//	pParent->m_bForbidDrag = TRUE;
				//}

				IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
				if (NULL != pParent)
				{
					if (1 == pParent->m_iChartType || 4 == pParent->m_iChartType) // K�ߣ�����K��
					{
						m_iPreNodePos = x;
						::SetCursor(LoadCursor(  NULL, IDC_NO ));
						pParent->m_bForbidDrag = FALSE;
					}
				}
			}
		}
	}
	else
	{
		if ( CheckFlag(m_iFlag,CChartRegion::KXAxis))
		{
			if ( m_RectView.PtInRect(CPoint(x,y)))
			{
				GetViewData()->m_bPressXAxis = true;

				IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
				if (NULL != pParent)
				{
					if (1 == pParent->m_iChartType || 4 == pParent->m_iChartType) // K�ߣ�����K��
					{
						m_iPreNodePos = x;
						::SetCursor(LoadCursor(  NULL, IDC_SIZEWE ));
						pParent->m_bForbidDrag = FALSE;
					}
				}
			}
		}

		// ֪ͨ��region�����ѡȡ��
		if ( CheckFlag(m_iFlag,CChartRegion::KChart)
			|| CheckFlag(m_iFlag, CChartRegion::KYAxis) )
		{

			if ( m_RectView.PtInRect(CPoint(x,y)))
			{
				GetViewData()->OnPickChart(this, x, y, 0);
			}
		}

		if ( CheckFlag(m_iFlag,CChartRegion::KChart) &&
			CChartRegion::m_eSelfDrawType != ESDTInvalid &&
			CChartRegion::m_iSelfDrawAppendOp > 0 )
		{
			OnSelfDrawEnd(x,y);
			return;
		}

		if ( CheckFlag(m_iFlag,CChartRegion::KChart) &&
			CChartRegion::m_eSelfDrawType != ESDTInvalid &&
			0 == CChartRegion::m_iSelfDrawAppendOp )
		{
			OnSelfDrawPress(x,y);
			return;
		}

		//ʰȡ
		CDrawingCurve* pDrawingCurve = NULL;
		CChartCurve* pCurve = NULL;
		CSelfDrawCurve* pCurveSD = NULL;
		CNodeSequence* pNodes = NULL;
		CNodeData NodeData;
		if ( PickNode ( x,y,pDrawingCurve,pNodes,NodeData) )
		{
			if ( pDrawingCurve->m_bSelfDraw )
			{
				pCurveSD = (CSelfDrawCurve*)pDrawingCurve;
				pCurveSD->HitTest(this,x,y);
				GetViewData()->m_pDragCurveSD = pCurveSD;
				GetViewData()->m_pDragRegion = this;
				//((CWnd*) GetParentIoViewData())->SendMessage(UM_SetPickedSelfDrawCurve,(WPARAM)pDrawingCurve);->SetPickedSelfDrawCurve((CSelfDrawCurve*)pDrawingCurve);
				GetParentIoViewData().pChartRegionData->SetPickedSelfDrawCurve((CSelfDrawCurve*)pDrawingCurve);
				// ��������
				ClipSelfDrawCursor();
			}
			else
			{
				pCurve = (CChartCurve*)pDrawingCurve;
				GetViewData()->m_pDragCurve = pCurve;
				GetViewData()->m_pDragRegion = this;
				GetParentIoViewData().pWnd->SendMessage(UM_SetPickedSelfDrawCurve,0);//->SetPickedSelfDrawCurve(NULL);
			}

			bPicked = true;
		}

		SetDrawFlag(CRegion::KDrawFull);
		GetViewData()->GetView()->Invalidate();

		if ( CheckFlag(m_iFlag, KRectZoomOut) && !bPicked )
		{
			m_bAbleToRectZoomOut = true;
		}

		//�õ���ǰʱ��  
		DWORD   dwCurTick   =   GetTickCount();     

		if((dwCurTick-m_dwTickCnt)<= TIME) //dblclick    
		{     

		}
		else
		{
			IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
			if (NULL != pParent)
			{
				if ( pParent->m_pRegionXBottom->m_RectView.PtInRect(CPoint(x, y)) ) //  ˫��x��Region�����κη�ӳ
				{
					return;
				}
			}

			if ( CheckFlag(m_iFlag,CChartRegion::KCrossAble ))
			{
				if ( GetTopSkip() < 6 )
				{
					;
				}
				m_bActiveCross = !m_bActiveCross;		
				if ( m_bActiveCross )
				{
					m_ptCross = CPoint(x,y);
					OnMouseMove(x,y);
					//��������ʮ�ֹ��
					GetViewData()->GetOverlayDC()->Swap();
				}
				else
				{
					// ����ʾ.
					DrawIndexValueByMouseMove(0,0,true);      
					CNodeData TmpNodeData;
					GetViewData()->OnCrossData(this,0,TmpNodeData,0.0f,0.0f,false);	
				}
			}
		}
		m_dwTickCnt   =   dwCurTick;   
	}
}

void CChartRegion::OnMousePressUp(int32 x, int32 y)
{
	CPoint pt(x,y);	
	int iButton = TIndexButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapIndexBtn[iButton].LButtonUp(FALSE);
		CRect rtBtn;
		DrawIndexBtn(NULL, m_iIndexBtnHovering, rtBtn);
		GetViewData()->OnRegionIndexBtn(this, GetDependentCurve(), iButton);
		//return;
	}

	ClipCursor(NULL);

	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis))
	{
		IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
		if (NULL != pParent)
		{
			if ( pParent->m_pRegionXBottom->m_RectView.PtInRect(CPoint(x,y)))
			{
				if (1 == pParent->m_iChartType || 4 == pParent->m_iChartType) // K�ߣ�����K��
				{
					::SetCursor(LoadCursor(  NULL, IDC_SIZEWE ));
				}
			}
		}
	}

// 	if ( CheckFlag(m_iFlag, KRectZoomOut) )
// 	{
// 		int32 iBeginPos = 0;
// 		int32 iEndPos	= 0;
// 
// 		int32 iBeginCur,iEndCur;
// 		GetParentIoView()->GetNodeBeginEnd(iBeginCur, iEndCur);
// 
// 		// ������ֹ�ڵ�
// 		CChartCurve* pCurveDependent = GetDependentCurve();		
// 		if (NULL == pCurveDependent)
// 		{
// 			return;
// 		}
// 
// 		//
// 		int32 iBeginX = GetViewData()->m_PointPress.x;
// 		ClientXToRegion(iBeginX);
// 		pCurveDependent->RegionXToCurvePos(iBeginX, iBeginPos);	
// 
// 		//
// 		int32 iEndX = GetViewData()->m_PointRectZoomOutEnd.x;
// 		ClientXToRegion(iEndX);
// 		pCurveDependent->RegionXToCurvePos(iEndX, iEndPos);	
// 		
// 
// 		// ֪ͨ���� IoView ����Ŵ��¼�:
// 		GetParentIoView()->OnRectZoomOut((iBeginPos + iBeginCur), (iEndPos + iBeginCur));
// 	
// 		// ��ԭ��־�ͱ���:
// 		m_bAbleToRectZoomOut = false;
// 		GetViewData()->m_PointRectZoomOutEnd = GetViewData()->m_PointPress;
// 
// 		ReDrawAll();
// 	}	
}

void CChartRegion::OnMouseDragMove ( int32 x, int32 y, bool bRightButton )
{
	// ...fangz0327 bug

	if ( CheckFlag(m_iFlag,CRegion::KFrame) )
	{
		return;
	}

	if ( CheckFlag(m_iFlag,CChartRegion::KChart) &&  CChartRegion::m_eSelfDrawType != ESDTInvalid )
	{
		OnSelfDrawDrag(x,y);
		return;
	}

	if ( NULL != GetViewData()->m_pDragRegion )
	{
		if ( NULL != GetViewData()->m_pDragCurveSD )
		{
			GetViewData()->m_pDragCurveSD->Moving (this,GetViewData()->GetOverlayDC(),NULL,x,y);
		}
		if ( NULL != GetViewData()->m_pDragCurve &&
			 CheckFlag(GetViewData()->m_pDragCurve->m_iFlag,CChartCurve::KTypeIndex ))
		{
			if ( !m_RectView.PtInRect(CPoint(x,y))) return;
			if ( GetViewData()->m_pDragRegion == this )
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDMOVE));
			}
			else
			{
				if ( CheckFlag(m_iFlag,CChartRegion::KXAxis) ||
					 CheckFlag(m_iFlag,CChartRegion::KUserChart) ||
					 CheckFlag(m_iFlag,CChartRegion::KYAxis))
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDNONE));
				}
				else
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDMOVE));
				}
			}
		}
	}
	else
	{
		if (GetViewData()->m_bPressXAxis)
		{
			IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
			if (NULL == pParent)
			{
				return;
			}
			if (1 != pParent->m_iChartType && 4 != pParent->m_iChartType) // ����K�ߺ�����K��
			{
				return;
			}


			if(bRightButton)
			{
				::SetCursor(LoadCursor(  NULL, IDC_NO ));
				return;
			}

			if (pParent->m_bForbidDrag)
			{
				::SetCursor(LoadCursor(  NULL, IDC_NO ));
			}
			else
			{
				::SetCursor(LoadCursor(  NULL, IDC_SIZEWE ));
			}

			CRect rct = pParent->m_pRegionXBottom->m_RectView;
			GetViewData()->GetView()->ClientToScreen(&rct);
			ClipCursor(rct);

			int32 iPos = 0, iPrePos = 0;
			int32 rx = x;
			pParent->m_pRegionXBottom->ClientXToRegion(rx);
			CChartCurve* pCurve = pParent->m_pRegionXBottom->GetDependentCurve();
			if (NULL != pCurve)
			{
				pCurve->RegionXToCurvePos(rx, iPos);

				rx = m_iPreNodePos;
				pParent->m_pRegionXBottom->ClientXToRegion(rx);
				pCurve->RegionXToCurvePos(rx, iPrePos);
			}			

			pParent->m_pRegionMain->m_iNodeVPos = iPos;
			pParent->DragMoveKLine(iPos-iPrePos);
			m_iPreNodePos = x;
			return;
		}

		//
		if ( CheckFlag(m_iFlag, CChartRegion::KRectZoomOut) && !bRightButton)
		{
			// ���Ŵ����:			
			GetViewData()->m_PointRectZoomOutEnd = CPoint(x, y);

			DrawRectZoomOut(GetViewParam()->GetOverlayDC());	
		}

		if(/*GetViewData()->m_bRightButPress && CheckFlag(GetViewData()->m_iFlag, CRegion::KMouseDrag)*/bRightButton)
		{
			GetViewData()->m_PointRectZoomOutEnd = CPoint(x, y);
			DrawRightDragRect(GetViewParam()->GetOverlayDC());
		}
	}
}

void CChartRegion::OnMouseDragDrop ( int32 x, int32 y, bool bRightButton )
{
	m_iJumpNodeID  = -1;
	m_iJumpNodePos = -1;
	m_iPreNodeID   = -1;	
	if ( CheckFlag(m_iFlag,CRegion::KFrame) )
	{
		return;
	}
	if ( !m_RectView.PtInRect(CPoint(x,y)))
	{
		return;
	}

	if(!bRightButton)
	{
		if ( CheckFlag(m_iFlag,CChartRegion::KChart) &&  CChartRegion::m_eSelfDrawType != ESDTInvalid )
		{
			OnSelfDrawRelease(x,y);
			return;
		}

	if ( NULL != GetViewData()->m_pDragRegion )
	{
		if ( NULL != GetViewData()->m_pDragCurveSD && this == GetViewData()->m_pDragRegion )
		{
			GetViewData()->m_pDragCurveSD->Move(this,GetViewData()->GetOverlayDC(),GetViewData()->GetPickDC(),x,y);
		}
		if ( NULL != GetViewData()->m_pDragCurve && this != GetViewData()->m_pDragRegion)
		{
			GetViewData()->OnDropCurve(GetViewData()->m_pDragRegion,GetViewData()->m_pDragCurve,this);
		}

		return;
	}

	int32 iBeginPos = 0;
	int32 iEndPos	= 0;
	
	int32 iBeginCur,iEndCur;
	//((CWnd*) GetParentIoViewData())->SendMessage(UM_GetNodeBeginEnd,(WPARAM)&iBeginCur,(LPARAM)&iEndCur);//->GetNodeBeginEnd(iBeginCur, iEndCur);
	GetParentIoViewData().pChartRegionData->GetNodeBeginEnd(iBeginCur, iEndCur);
	// ������ֹ�ڵ�
	CChartCurve* pCurveDependent = GetDependentCurve();		
	if (NULL == pCurveDependent)
	{
		return;
	}
	
	//
	int32 iBeginX = GetViewData()->m_PointPress.x;
	ClientXToRegion(iBeginX);
	pCurveDependent->RegionXToCurvePos(iBeginX, iBeginPos);	
	
	//
	int32 iEndX = GetViewData()->m_PointRectZoomOutEnd.x;
	ClientXToRegion(iEndX);
	pCurveDependent->RegionXToCurvePos(iEndX, iEndPos);	
	
	if ( IsCtrlPressed() )
	{
		// ����ͳ��
		GetParentIoViewData().pChartRegionData->OnRectIntervalStatistics((iBeginPos + iBeginCur), (iEndPos + iBeginCur));
		GetViewData()->m_PointRectZoomOutEnd = GetViewData()->m_PointPress;
	
		//ReDrawAll();		
		GetParentIoViewData().pWnd->Invalidate();
		return;
	}
	
	if ( CheckFlag(m_iFlag, KRectZoomOut) )
	{
		// ֪ͨ���� IoView ����Ŵ��¼�:
		GetParentIoViewData().pChartRegionData->OnRectZoomOut((iBeginPos + iBeginCur), (iEndPos + iBeginCur));
		
		// ��ԭ��־�ͱ���:
		m_bAbleToRectZoomOut = false;
		GetViewData()->m_PointRectZoomOutEnd = GetViewData()->m_PointPress;
			//ReDrawAll();
			GetParentIoViewData().pWnd->Invalidate();
		}
	}
	else
	{
		int32 iBeginPos = 0;
		int32 iEndPos	= 0;

		int32 iBeginCur,iEndCur;
		//((CWnd*) GetParentIoViewData())->SendMessage(UM_GetNodeBeginEnd,(WPARAM)&iBeginCur,(LPARAM)&iEndCur);//->GetNodeBeginEnd(iBeginCur, iEndCur);
		GetParentIoViewData().pChartRegionData->GetNodeBeginEnd(iBeginCur, iEndCur);
		// ������ֹ�ڵ�
		CChartCurve* pCurveDependent = GetDependentCurve();		
		if (NULL == pCurveDependent)
		{
			return;
		}

		//
		int32 iBeginX = GetViewData()->m_PointPress.x;
		ClientXToRegion(iBeginX);
		pCurveDependent->RegionXToCurvePos(iBeginX, iBeginPos);	

		//
		int32 iEndX = GetViewData()->m_PointRectZoomOutEnd.x;
		ClientXToRegion(iEndX);
		pCurveDependent->RegionXToCurvePos(iEndX, iEndPos);	

		// ����ͳ��
		GetParentIoViewData().pChartRegionData->OnRectIntervalStatistics((iBeginPos + iBeginCur), (iEndPos + iBeginCur));
		GetViewData()->m_PointRectZoomOutEnd = GetViewData()->m_PointPress;

		//ReDrawAll();		
		GetParentIoViewData().pWnd->Invalidate();
		return;
	}
}

void CChartRegion::OnMouseMove ( int32 x, int32 y )
{
	bool32 bSelfDrawing = false;
	if ( CChartRegion::m_eSelfDrawType != ESDTInvalid )
	{
		// �����Ի��ߵ�ʱ��,�������Ϊ�Ի��ߵ�.
		bSelfDrawing = true;
	}

	CPoint pt(x,y);
	m_ptCross = CPoint(x,y);

	//
	CChartRegionViewParam* pViewParam = GetViewData();

	if ( NULL != pViewParam )
	{	
		CString StrTips = L"";
		CString StrTitle;
		
		if ( m_TipWnd.GetSafeHwnd() && GetParentIoViewData().pChartRegionData->m_pRegionMain == this )
		{			
			if ( pViewParam->GetTips(pt, StrTips, StrTitle) && StrTips.GetLength() > 0 )
			{
				m_TipWnd.Show(pt, StrTips, StrTitle);
			}
			else
			{
				m_TipWnd.Hide();
			}
		}
	}

	//
	CRect rect;
	//GetViewData()->GetView()->GetClientRect(&rect);
	GetViewData()->GetViewRegionRect(&rect);
	rect.left = m_RectView.left;
	rect.right = m_RectView.right;

	// ���������Ƶ�ʮ�ֹ��
	if ( CheckFlag(m_iFlag,CChartRegion::KCrossAble ) &&
		 !CheckFlag(m_iFlag,CChartRegion::KLockMouseMove ) &&
		 m_bActiveCross && 
		 rect.PtInRect(pt))
	{	
		DrawIndexValueByMouseMove(x,y,false);
		
		CChartCurve* pCurveDependent = GetDependentCurve();
		if (NULL == pCurveDependent)
		{
			// ���Ƽ򵥵���
			DrawCross(GetViewData()->GetOverlayDC(),x,y,EDCMouseMove);
			return;
		}

		// ���ԭ���ĵ�Ҳ�����λ�ã��򲻽���nodecross�ı䶯
		int32 iOldRX;
		int32 rx = x;
		ClientXToRegion(rx);
		if ( !pCurveDependent->CurvePosToRegionX(m_iNodeCross, iOldRX)
			|| rx != iOldRX )
		{
			pCurveDependent->RegionXToCurvePos(rx,m_iNodeCross);
			DrawCross(GetViewData()->GetOverlayDC(),x,y,EDCMouseMove);
		}
		else
		{
			int32 iOld = m_iNodeCross;
			DrawCross(GetViewData()->GetOverlayDC(),x,y,EDCMouseMove);
			ASSERT( iOld == m_iNodeCross );
		}
	}

	if ( !bSelfDrawing )
	{
		if( CheckFlag(m_iFlag,CChartRegion::KXAxis) &&
			CheckFlag(m_iFlag,CChartRegion::KDragXAxisPanAble))
		{
			if ( m_RectView.PtInRect(pt))
			{
				IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;

				if (NULL != pParent)
				{
					if (1 == pParent->m_iChartType || 4 == pParent->m_iChartType) // K��,����K��
					{
						//	::SetCursor(LoadCursor(  NULL, IDC_SIZEWE ));
						if (pParent->m_bForbidDrag)
						{
							pParent->SetNewCursor(LoadCursor(  NULL, IDC_NO ));
							pParent->m_bForbidDrag = FALSE;
						}
						else
						{
							pParent->SetNewCursor(LoadCursor(  NULL, IDC_SIZEWE ));
						}
					}
				}
			}
		}
		else
		{
			if ( m_RectView.PtInRect(pt))
			{
				::SetCursor(LoadCursor(  NULL, IDC_ARROW ));
			}
		}
	}

	if ( CheckFlag(m_iFlag,CChartRegion::KChart) &&
		 CChartRegion::m_eSelfDrawType != ESDTInvalid &&
		 CChartRegion::m_iSelfDrawAppendOp > 0 )
	{
		OnSelfDrawMove(x,y);
	}

	/*	ʰȡ
	CDrawingCurve* pDrawingCurve;
	CNodeSequence* pNodes;
	CNodeData NodeData;
	if ( PickNode ( x,y,pDrawingCurve,pNodes,NodeData,false) )
	{
		if ( pDrawingCurve->m_bSelfDraw )
		{
			CSelfDrawCurve* pSelfDrawCurve = (CSelfDrawCurve*)pDrawingCurve;
			int32 hit = pSelfDrawCurve->HitTest(this,x,y);
			switch ( hit )
			{
			case 0:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDCHG));
				break;
			case 1:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDCHG));
				break;
			case 2:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDCHG));
				break;
			case 3:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDMOVE));
				break;
			default:
				break;
			}
		}
	}
*/
	DrawAxisSlider ( GetViewData()->GetOverlayDC(), pt );

	int iButton = TIndexButtonHitTest(pt);
	if (iButton != m_iIndexBtnHovering)
	{
		CRect rtBtn;
		if (INVALID_ID != m_iIndexBtnHovering)
		{
			m_mapIndexBtn[m_iIndexBtnHovering].MouseLeave(FALSE);
			DrawIndexBtn(NULL, m_iIndexBtnHovering, rtBtn);
			m_iIndexBtnHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iIndexBtnHovering = iButton;
			m_mapIndexBtn[m_iIndexBtnHovering].MouseHover(FALSE);
			DrawIndexBtn(NULL, m_iIndexBtnHovering, rtBtn);
		}
	}
}

void CChartRegion::DrawIndexValueByMouseMove(int32 x,int32 y,bool32 bHideCross)
{
	bool32 bShowTrendIndex = false;
	bool32 bShowSaneIndex  = false;

	IChartRegionData * pIoViewParent = GetParentIoViewData().pChartRegionData;

	if ( 1 == pIoViewParent->m_iChartType && this == pIoViewParent->m_pRegionMain )
	{	
		if ( pIoViewParent->GetShowSaneIndexFlag() )
		{
			bShowSaneIndex = true;
		}
	}

	if ( bShowTrendIndex || bShowSaneIndex )
	{
		// ����ָ��:
		
		CMemDCEx* pMainDC = GetViewData()->GetMainDC();
		CMemDCEx* pPickDC = GetViewData()->GetPickDC();
		
		//CIoViewKLine* pKLine = (CIoViewKLine*)GetParentIoView();
		
		int32 ix = x;
		int32 iPos = 0;						
		ClientXToRegion(ix);
		CChartCurve* pDependCurve = GetDependentCurve();
		if ( NULL == pDependCurve )
		{
			return;
		}
		
		if ( !pDependCurve->RegionXToCurvePos(ix,iPos) )
		{
			return;
		}

		//
		int32 iBegin,iEnd;
		GetParentIoViewData().pChartRegionData->GetNodeBeginEnd(iBegin,iEnd);

		if ( !bHideCross)
		{
			m_iMouseMoveTrendIndexPos = iPos+iBegin;
			
			if ( m_iMouseMoveTrendIndexPos >= iEnd )
			{
				m_iMouseMoveTrendIndexPos = iEnd - 1;
			}
		}
		else
		{
			m_iMouseMoveTrendIndexPos = iEnd - 1;
		}

		DrawTitle(pMainDC,pPickDC,true);
	}
	else
	{
		CArray<T_IndexParam*,T_IndexParam*> IndexArray;
		
		for (int32 i = 0 ; i < m_Curves.GetSize(); i++)
		{		
			CChartCurve * pCurve = m_Curves.GetAt(i);		
			T_IndexParam* pParam = GetParentIoViewData().pChartRegionData->FindIndexParamByCurve(pCurve);
			
			if ( NULL == pParam )
			{
				CString StrCurvePostfix;
				CNodeSequence *pNodes = pCurve->GetNodes();
				if ( NULL != pNodes 
					&& GetViewData()->OnGetCurveTitlePostfixString(pCurve, CPoint(x, y), bHideCross, StrCurvePostfix) )
				{
					//ASSERT( pNodes->GetSize() <= 0 || StrCurvePostfix != _T(":0") );
					// ���ƻ�ȡ�ı���
					pNodes->SetNamePostfix(StrCurvePostfix);	
					CString StrTitle = pNodes->GetNamePrefix() + pNodes->GetName() + pNodes->GetNamePostfix(); 
					pCurve->SetTitle(StrTitle);

					CMemDCEx* pMainDC = GetViewData()->GetMainDC();
					CMemDCEx* pPickDC = GetViewData()->GetPickDC();				
					
					if ( NULL != pMainDC && NULL != pPickDC)
					{
						DrawTitle(pMainDC,pPickDC,true);
					}
				}
				continue;
			}

			if ( 0 == IndexArray.GetSize() )
			{
				IndexArray.Add(pParam);
			}
			else
			{
				bool32 bExist = false;
				for (int32 j = 0 ; j < IndexArray.GetSize() ; j++)
				{
					if ( pParam == IndexArray.GetAt(j))
					{
						bExist = true;
					}
				}
				
				if ( !bExist )
				{
					IndexArray.Add(pParam);
				}
			}
		}
		
		if ( 0 == IndexArray.GetSize() )
		{
			return;
		}
		else
		{
			// ÿ��ָ���ÿ����.���ݵ�ǰ���λ��,�õ�ָ���ֵ.
			for (int32 i = 0 ; i < IndexArray.GetSize() ; i++)
			{
				CArray<CChartCurve*,CChartCurve*> Curves;
				T_IndexParam * pParam = IndexArray.GetAt(i);
				
				GetParentIoViewData().pChartRegionData->FindCurvesByIndexParam(pParam, Curves);
				if ( Curves.GetSize() <= 0 )
				{
					continue;
				}
				else
				{
					for (int32 j = 0 ; j < Curves.GetSize() ; j++)
					{
						CChartCurve * pCurve = Curves.GetAt(j);
						
						if ( NULL == pCurve )
						{
							continue;
						}					
						
						CNodeSequence * pNodes = ((CChartCurve*)pCurve)->GetNodes();
						
						if ( NULL == pNodes)
						{
							continue;
						}
						
						CNodeData NodeData;
						
						if (!bHideCross)
						{		
							// ����ʮ�ֹ��λ�õõ�NodeData.						
							int32 ix = x;
							int32 iPos = -1;
							
							ClientXToRegion(ix);
							pCurve->RegionXToCurvePos(ix,iPos);												
							
							if ( iPos >= pNodes->GetSize() || -1 == iPos )
							{
								iPos = pNodes->GetSize() - 1;
							}

							pNodes->GetAt(iPos,NodeData);							
						}
						else
						{
							// �õ����µ�һ��ָ��ֵ											
							int32 iSize = pCurve->GetNodes()->GetSize();
							pNodes->GetAt(iSize-1,NodeData);
						}
						
						T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
						if(NULL == pData)
						{
							continue;
						}
						int32 iSaveDec = 2;
						CMerch* pMerch = pData->m_pMerchNode;
						
						if ( NULL != pMerch )
						{
							// t.. fangz1210 ����ʾ��λС��,������
							// iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
						}
						
						CString StrValue,StrPostfix;
						if (NodeData.m_fClose == FLT_MAX || NodeData.m_fClose == FLT_MIN || CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
						{
							StrValue = Float2String(0.0,iSaveDec,true);
						}
						else
						{
							StrValue = Float2String(NodeData.m_fClose,iSaveDec,true);
						}
						
						StrPostfix.Format(L":%s",(LPCTSTR)StrValue);
						pNodes->SetNamePostfix(StrPostfix);	
						CString StrTitle = pNodes->GetNamePrefix() + pNodes->GetName() + pNodes->GetNamePostfix(); 
						pCurve->SetTitle(StrTitle);
						
					}
					
					CMemDCEx* pMainDC = GetViewData()->GetMainDC();
					CMemDCEx* pPickDC = GetViewData()->GetPickDC();				
					
					if ( NULL != pMainDC && NULL != pPickDC)
					{
						DrawTitle(pMainDC,pPickDC,true);
					}
				}
			}
		}
	}		
}

bool32 CChartRegion::RegionToSD ( int32 x1,int32 y1, int32& x2, int32& y2 )
{
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return false;
	}

	// ���������չ�����Ҳ�����ʾ������ô�ͷ����˴�ת��

	int32 iPos;
	if ( !pCurveDependent->RegionXToCurvePos(x1,iPos) )
	{
		return false;
	}

	// zhangbo 20090706 #��ʱ�ܵ�
//	x2 = m_fWidthPerNode * ( iPos + 0.5 );
	x2 = x1;

	y2 = y1;
	return true;
}

bool32 CChartRegion::RegionToSD ( int32 x,int32 y, int32& id,float& fVal )
{
	id = 0;
	fVal = 0.;

	// 
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return false;
	}

	// ���������չ�����Ҳ�����ʾ������ô�ͷ����˴�ת��

	int32 iPos;
	if ( !pCurveDependent->RegionXToCurvePos(x,iPos) )
	{
		return false;
	}
	if ( !pCurveDependent->RegionYToPriceY(y,fVal) )
	{
		return false;
	}

	CNodeData NodeData;
	if (!pCurveDependent->GetNodes()->GetAt(iPos, NodeData))
		return false;

	id = NodeData.m_iID;
	return true;
}

bool32 CChartRegion::IsSDInRegion ( CSelfDrawNode* pSelfDrawNode )
{
	return true;//��ʱû���Ǻ����ʵ��.	
}

bool32 CChartRegion::MoveSDY ( int32 x1,int32 y1,int32 x2,int32 y2,float& fVal1,float& fVal2)
{
	// 
	ClientYToRegion(y1);
	ClientYToRegion(y2);

	CChartCurve *pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	// ���������չ�����Ҳ�����ʾ������ô�ͷ����˴�ת��

	float f1,f2;
	if (!pCurveDependent->RegionYToPriceY(y1,f1) || !pCurveDependent->RegionYToPriceY(y2,f2))
		return false;

	fVal1 += ( f2 - f1 );
	fVal2 += ( f2 - f1 );

	return true;
}

bool32 CChartRegion::MoveSDId ( int32 x1,int32 y1,int32 x2,int32 y2,int32& id1,int32& id2)
{
	CChartCurve* pCurve = GetDependentCurve();
	if ( NULL == pCurve )
	{
		return false;
	}
	int32 iPos1,iPos2;
	ClientXToRegion(x1);
	if ( !pCurve->RegionXToCurvePos(x1,iPos1) )
	{
		return false;
	}
	ClientXToRegion(x2);
	if ( !pCurve->RegionXToCurvePos(x2,iPos2) )
	{
		return false;
	}

	GetViewData()->OnSliderId(id1,iPos2-iPos1);
	GetViewData()->OnSliderId(id2,iPos2-iPos1);

	return true;

}

bool32 CChartRegion::SDToRegion ( int32 id, float fVal, int32& x,int32& y )
{
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return false;
	}

	// �����Ի��߿����������ŵ�ĳһ��λ�ڲ��ɼ�λ��
	// �����Ի�Ӧ��������չ����ϵ

	// x����չ
	if ( !ExtendSDIdToRegionX(id, x) )
	{
		return false;
	}

	// y��չ
	if ( !pCurveDependent->ExtendPriceYToRegionY(fVal,y))
	{
		// ��չ����ʧ�ܣ�ֱ��false
		return false;
	}

	return true;
}

bool32 CChartRegion::ExtendSDIdToRegionX( int32 id, int32 &x )
{
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return false;
	}
	
	// x����չ
	int32 iPos = 0;
	if ( pCurveDependent->GetNodes()->Lookup(id,iPos) )
	{
		return pCurveDependent->CurvePosToRegionX(iPos,x);
	}
	else if ( m_aXAxisNodes.GetSize()>0 )
	{
		// û���ҵ�ʵ�������
		const int32 iLastPos = m_aXAxisNodes.GetSize()-1;
		if ( id >= m_aXAxisNodes[0].m_iTimeId && id <= m_aXAxisNodes[iLastPos].m_iTimeId )
		{
			// ������ʵ�����������ڣ��Ǿ�ʹ��ģ������
			return pCurveDependent->GetNodes()->LookupEx(id, iPos) && pCurveDependent->CurvePosToRegionX(iPos,x);
		}
		else
		{
			// �����⣬����ʹ��ʵ��K����������
			// ����x��û��ͳһ�����������������ʹ����ͬ�����������
			const float fUnitWidth = m_aXAxisNodes[0].m_fPixelWidthPerUnit;
			
			IChartRegionData *pChart = GetParentIoViewData().pChartRegionData;
			ASSERT(NULL!=pChart);
			if (NULL == pChart)
			{
				return false;
			}
			if ( pChart->m_MerchParamArray.GetSize()>0 && pChart->m_MerchParamArray[0]!=NULL )
			{
				T_MerchNodeUserData *pMainData = pChart->m_MerchParamArray[0];
				
				CGmtTime TimeId(id);
				int32 iPosId = CMerchKLineNode::QuickFindKLineByTime(pMainData->m_aKLinesFull, TimeId);
				if ( iPosId>=0 && iPosId<pMainData->m_aKLinesFull.GetSize() )
				{
					int32 iIdSub = pMainData->m_iShowPosInFullList - iPosId;
					x = m_aXAxisNodes[0].m_iCenterPixel - (int32)(fUnitWidth*iIdSub);
					x -= GetRectCurves().left;
					return true;
				}
			}
		}
	}
	return false;
}

bool32 CChartRegion::ExtendRegionXToSDId( int32 x, int32 id )
{
	return false;
}

void CChartRegion::ClearSelfDrawValue()
{
	m_PtPress = CPoint(-1,-1 );
	m_PtRelease = CPoint(-1,-1 );
	m_PtEnd = CPoint(-1,-1 );
	m_iIdPress = 0;
	m_iIdRelease = 0;
	m_iIdEnd = 0;
	m_fValPress = 0.0;
	m_fValRelease = 0.0;
	m_fValEnd = 0.0;
}

void CChartRegion::OnSelfDrawPress ( int32 x,int32 y )
{
	CRect rect = GetRectCurves();

	// �������, �û�ʵ�ʵ����ʱ��, ���ؼ����жϺ����������㲻����ߵ�, �ſ������
	if ((y - rect.top) >= -2 && (y - rect.top <= 0))
	{
		y = rect.top;
	}

	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	
	IChartRegionData* pChart = GetParentIoViewData().pChartRegionData;
	if (NULL == pChart)
	{
		return;
	}

	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}
	m_PtPress = CPoint(-1,-1);
	ClientXToRegion(x);
	ClientYToRegion(y);
	//��һ��:���뵽KLine
	if ( !RegionToSD(x,y,m_iIdPress,m_fValPress) )
	{		
		if ( GetParentIoViewData().pIoViewBase->IsKindOfIoViewKLine() )
		{
			pChart->CalcLayoutRegions();
		}
		if ( !RegionToSD(x,y,m_iIdPress,m_fValPress) )
		{
			return;
		}		
	}
	int32 tx,ty;
	if ( !RegionToSD(x,y,tx,ty))
	{
		return;
	}
	RegionXToClient(tx);
	RegionYToClient(ty);
	m_PtPress = CPoint(tx,ty);

	// 45 �ȵ���Ҫ��ʱ�ͻ�����
	if (ESDTZ45UP == CChartRegion::m_eSelfDrawType || ESDTZ45DOWN == CChartRegion::m_eSelfDrawType || ESDTZVertical == CChartRegion::m_eSelfDrawType || ESDTZHorizontal == CChartRegion::m_eSelfDrawType)
	{
		//
		BeginOverlay(false);
		CMemDCEx* pDC = GetViewData()->GetOverlayDC();
		CPen pen,*pOldPen;
		pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		pOldPen = (CPen*)pDC->SelectObject(&pen);

		CPoint ptTmp(m_PtPress);
		CPoint pts[4];
		
		//
		if (ESDTZ45UP == CChartRegion::m_eSelfDrawType)
		{
			ptTmp.x++;
			ptTmp.y--;
		}
		else if (ESDTZ45DOWN == CChartRegion::m_eSelfDrawType)
		{
			ptTmp.x++;
			ptTmp.y++;
		}
		else if (ESDTZVertical == CChartRegion::m_eSelfDrawType)
		{
			ptTmp.y--;
		}
		else if (ESDTZHorizontal == CChartRegion::m_eSelfDrawType)
		{
			ptTmp.x--;
		}
		
		//
		CaclTwoPoint(rect,m_PtPress,ptTmp,pts);
		pDC->_DrawLine(pts[0],pts[1]);

		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		
		EndOverlay();

		//lint --e{429}
		{
			CSelfDrawNode* pSelfDrawNode = new CSelfDrawNode;
		
			//
			CTime Time = CTime::GetCurrentTime();
			pSelfDrawNode->t = (int32)Time.GetTime();
			pSelfDrawNode->m_eType = CChartRegion::m_eSelfDrawType;
			pSelfDrawNode->m_id1 = m_iIdPress;
			pSelfDrawNode->m_fVal1 = m_fValPress;
			pSelfDrawNode->m_id2 = m_iIdPress;
			pSelfDrawNode->m_fVal2 = m_fValPress;
			pSelfDrawNode->m_ide = 0;
			pSelfDrawNode->m_fVale = 0.0;
			
			IChartRegionData* pTmpChart = GetParentIoViewData().pChartRegionData;
			if (NULL != pTmpChart && pTmpChart->m_MerchParamArray.GetSize() > 0)
			{
				pSelfDrawNode->m_iInternal = (int32)pTmpChart->m_MerchParamArray[0]->m_eTimeIntervalFull;
				pSelfDrawNode->m_iUserMin  = pTmpChart->m_MerchParamArray[0]->m_iTimeUserMultipleMinutes;
				pSelfDrawNode->m_iUserDay  = pTmpChart->m_MerchParamArray[0]->m_iTimeUserMultipleDays;
				pSelfDrawNode->m_pMerch    = GetParentIoViewData().pIoViewBase->GetMerchXml();
			}

			
			CSelfDrawCurve* pSelfDrawCurve = new CSelfDrawCurve(this);
			pSelfDrawCurve->SetNode(pSelfDrawNode);
			pSelfDrawCurve->SetT(pSelfDrawNode->t);
			m_SelfDrawingCurves.Add(pSelfDrawCurve);
			
			ClearSelfDrawValue();
			SetSelfDrawType (ESDTInvalid, GetParentIoViewData().pChartRegionData);
			
			SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
			GetViewData()->GetView()->Invalidate();
		}
	}
	else
	{
		ClipSelfDrawCursor();
	}
}

void CChartRegion::OnSelfDrawDrag ( int32 x,int32 y )
{
	CRect rect = GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_PtPress.x < 0 )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}
	ClientXToRegion(x);
	ClientYToRegion(y);
	int32 id;
	float fVal;
	int32 tx,ty;
	if ( !RegionToSD(x,y,tx,ty))
	{
		return;
	}

	RegionXToClient(tx);
	RegionYToClient(ty);

	BeginOverlay(false);
	CMemDCEx* pDC = GetViewData()->GetOverlayDC();
	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1, KSelfDrawDefaultClr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1,pt2;
	pt1 = m_PtPress;
	pt2 = CPoint(tx,ty);
	CPoint pts[4];
	switch ( CChartRegion::m_eSelfDrawType )
	{
	case ESDTLine:
		pDC->_DrawLine(pt1,pt2);
		break;
	case ESDTLineEx:
		CaclTwoPoint(rect,pt1,pt2,pts);
		pDC->_DrawLine(pts[0],pts[1]);
		break;
	case ESDTJgtd:
		CaclTwoPoint(rect,pt1,pt2,pts);
		pDC->_DrawLine(pts[0],pts[1]);
		break;
	case ESDTPxzx:
		pDC->_DrawLine(pt1,pt2);
		break;
	case ESDTYhx:
		SDDrawYhx(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTHjfg:
		SDDrawHjfg(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBfb:
		SDDrawBfb(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBd:
		SDDrawBd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJx:
		SDDrawJx(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJxhgd:
		if ( RegionToSD(x,y,id,fVal) )
		{
			SDDrawJxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,id);
		}		
		break;
	case ESDTYcxxhgd:
		if ( RegionToSD(x,y,id,fVal) )
		{
			SDDrawYcxxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,id);
		}
		break;
	case ESDTXxhgd:
		if ( RegionToSD(x,y,id,fVal) )
		{
			SDDrawXxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,id);
		}
		break;
	case ESDTZq:
		SDDrawZq(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTFblq:
		// test
		//SDDrawFblq(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		SDDrawFblq(this,NULL,pDC,NULL,pt1.x,pt1.y,TRUE);//2013-5-15 cym modify �����д���
		break;
	case ESDTZs:
		SDDrawZs(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJejd:
		SDDrawJejd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTZbj:
		break;
	case ESDTDbj:
		break;
	case ESDTZ45UP:
		{
			CPoint ptTmp(pt2);
			ptTmp.x++;
			ptTmp.y--;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTZ45DOWN:
		{
			CPoint ptTmp(pt2);
			ptTmp.x++;
			ptTmp.y++;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTZVertical:
		{
			CPoint ptTmp(pt2);
			ptTmp.y++;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTText:
		break;
	default:
		break;
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	EndOverlay();
}

void CChartRegion::OnSelfDrawRelease ( int32 x,int32 y )
{
	//
	ClipCursor(NULL);

	CRect rect = GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_PtPress.x < 0 )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}
	if ( m_PtPress == CPoint(x,y))
	{
		return;
	}
	ClientXToRegion(x);
	ClientYToRegion(y);
	//��һ��:���뵽KLine
	if ( !RegionToSD(x,y,m_iIdRelease,m_fValRelease) )
	{
		return;
	}
	int32 tx,ty;
	if ( !RegionToSD(x,y,tx,ty))
	{
		return;
	}
	RegionXToClient(tx);
	RegionYToClient(ty);
	m_PtRelease = CPoint(tx,ty);

	BeginOverlay(false);
	CMemDCEx* pDC = GetViewData()->GetOverlayDC();	
	
	//
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,RGB(255,255,0));
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	bool32 bSave = false;

	CPoint pt1,pt2;
	pt1 = m_PtPress;
	pt2 = m_PtRelease;
	CPoint pts[4];
	switch ( CChartRegion::m_eSelfDrawType )
	{
	case ESDTLine:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		pDC->_DrawLine(m_PtPress,m_PtRelease);
		break;
	case ESDTLineEx:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		CaclTwoPoint(rect,pt1,pt2,pts);
		pDC->_DrawLine(pts[0],pts[1]);
		break;
	case ESDTJgtd:
		CChartRegion::m_iSelfDrawAppendOp = 1;
		CaclTwoPoint(rect,pt1,pt2,pts);
		pDC->_DrawLine(pts[0],pts[1]);
		break;
	case ESDTPxzx:
		CChartRegion::m_iSelfDrawAppendOp = 1;
		pDC->_DrawLine(m_PtPress,m_PtRelease);
		break;
	case ESDTYhx:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawYhx(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTHjfg:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawHjfg(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBfb:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawBfb(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBd:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawBd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJx:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawJx(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJxhgd:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawJxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,m_iIdRelease);
		break;
	case ESDTYcxxhgd:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawYcxxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,m_iIdRelease);
		break;
	case ESDTXxhgd:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawXxhgd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_iIdPress,m_iIdRelease);
		break;
	case ESDTZq:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawZq(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTFblq:
		// test
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		//SDDrawFblq(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		SDDrawFblq(this,NULL,pDC,NULL,pt1.x,pt1.y,TRUE);//2013-5-15 cym modify �����д�
		break;
	case ESDTZs:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawZs(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJejd:
		bSave = true;
		m_PtEnd = CPoint(-1,-1);
		m_iIdEnd = 0;
		m_fValEnd = 0;
		SDDrawJejd(this,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTZbj:
		break;
	case ESDTDbj:
		break;
	case ESDTZ45UP:
		{
			bSave = true;
			m_PtEnd = CPoint(-1,-1);
			m_iIdEnd = 0;
			m_fValEnd = 0;
			CPoint ptTmp(pt2);
			ptTmp.x++;
			ptTmp.y--;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTZ45DOWN:
		{
			bSave = true;
			m_PtEnd = CPoint(-1,-1);
			m_iIdEnd = 0;
			m_fValEnd = 0;
			CPoint ptTmp(pt2);
			ptTmp.x++;
			ptTmp.y++;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTZVertical:
		{
			bSave = true;
			m_PtEnd = CPoint(-1,-1);
			m_iIdEnd = 0;
			m_fValEnd = 0;
			CPoint ptTmp(pt2);
			ptTmp.y++;
			CaclTwoPoint(rect,pt2,ptTmp,pts);
			pDC->_DrawLine(pts[0],pts[1]);
		}
		break;
	case ESDTText:
		break;
	default:
		break;
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	EndOverlay();

	//lint --e{429}
	if ( bSave )
	{
		CSelfDrawNode* pSelfDrawNode = new CSelfDrawNode;

		CTime Time = CTime::GetCurrentTime();
		pSelfDrawNode->t = (int32)Time.GetTime();
		pSelfDrawNode->m_eType = CChartRegion::m_eSelfDrawType;
		pSelfDrawNode->m_id1 = m_iIdPress;
		pSelfDrawNode->m_fVal1 = m_fValPress;
		pSelfDrawNode->m_id2 = m_iIdRelease;
		pSelfDrawNode->m_fVal2 = m_fValRelease;
		pSelfDrawNode->m_ide = 0;
		pSelfDrawNode->m_fVale = 0.0;

		IChartRegionData* pChart = GetParentIoViewData().pChartRegionData;
		if (NULL != pChart && pChart->m_MerchParamArray.GetSize() > 0)
		{
			pSelfDrawNode->m_iInternal = (int32)pChart->m_MerchParamArray[0]->m_eTimeIntervalFull;
			pSelfDrawNode->m_iUserMin  = pChart->m_MerchParamArray[0]->m_iTimeUserMultipleMinutes;
			pSelfDrawNode->m_iUserDay  = pChart->m_MerchParamArray[0]->m_iTimeUserMultipleDays;
			pSelfDrawNode->m_pMerch    = GetParentIoViewData().pIoViewBase->GetMerchXml();
		}

		//
		CSelfDrawCurve* pSelfDrawCurve = new CSelfDrawCurve(this);
		pSelfDrawCurve->SetNode(pSelfDrawNode);
		m_SelfDrawingCurves.Add(pSelfDrawCurve);

		ClearSelfDrawValue();
		SetSelfDrawType (ESDTInvalid, GetParentIoViewData().pChartRegionData);

		SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
		GetViewData()->GetView()->Invalidate();

		GetParentIoViewData().pChartRegionData->SetPickedSelfDrawCurve(pSelfDrawCurve);
	}
}

void CChartRegion::OnSelfDrawMove ( int32 x,int32 y )
{
	CRect rect = GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}
	ClientXToRegion(x);
	ClientYToRegion(y);
	int32 tx,ty;
	if ( !RegionToSD(x,y,tx,ty))
	{
		return;
	}
	RegionXToClient(tx);
	RegionYToClient(ty);

	BeginOverlay(false);
	CMemDCEx* pDC = GetViewData()->GetOverlayDC();	
	if (NULL == pDC)
	{
		return;
	}
	//
	CPen pen,*pOldPen = NULL;
	pen.CreatePen(PS_SOLID,1,KSelfDrawDefaultClr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);



	CPoint pt1,pt2;
	pt1 = m_PtPress;
	pt2 = m_PtRelease;

	switch ( CChartRegion::m_eSelfDrawType )
	{
	case ESDTLine:
		break;
	case ESDTLineEx:
		break;
	case ESDTJgtd:
		SDDrawJgtd ( this, NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,tx,ty);
		break;
	case ESDTPxzx:
		SDDrawPxzx ( this, NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,tx,ty);
		break;
	case ESDTYhx:
		break;
	case ESDTHjfg:
		break;
	case ESDTBfb:
		break;
	case ESDTBd:
		break;
	case ESDTJx:
		break;
	case ESDTJxhgd:
		break;
	case ESDTYcxxhgd:
		break;
	case ESDTXxhgd:
		break;
	case ESDTZq:
		break;
	case ESDTFblq:
		break;
	case ESDTZs:
		break;
	case ESDTJejd:
		break;
	case ESDTZbj:
		break;
	case ESDTDbj:
		break;
	case ESDTText:
		break;
	case ESDTZ45UP:
		break;
	case ESDTZ45DOWN:
		break;
	case ESDTZVertical:
		break;
	default:
		break;
	}
}

void CChartRegion::OnSelfDrawEnd ( int32 x,int32 y )
{
	CRect rect = GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}
	ClientXToRegion(x);
	ClientYToRegion(y);
	//��һ��:���뵽KLine
	if ( !RegionToSD(x,y,m_iIdEnd,m_fValEnd) )
	{
		return;
	}
	int32 tx,ty;
	if ( !RegionToSD(x,y,tx,ty))
	{
		return;
	}
	RegionXToClient(tx);
	RegionYToClient(ty);
	m_PtEnd = CPoint(tx,ty);

	CPoint pt1,pt2;
	pt1 = m_PtPress;
	pt2 = m_PtRelease;

	BeginOverlay(false);
	CMemDCEx* pDC = GetViewData()->GetOverlayDC();	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
	
//	pen.CreatePen(PS_SOLID,1,CSelfDrawNode::m_clr);//2013
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgSetText dlg;
	CString StrText;
	bool32 bSave = false;
	switch ( CChartRegion::m_eSelfDrawType )
	{
	case ESDTLine:
		break;
	case ESDTLineEx:
		break;
	case ESDTJgtd:
		bSave = true;
		SDDrawJgtd ( this, NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,tx,ty);
		break;
	case ESDTPxzx:
		bSave = true;
		SDDrawPxzx ( this, NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,tx,ty);
		break;
	case ESDTYhx:
		break;
	case ESDTHjfg:
		break;
	case ESDTBfb:
		break;
	case ESDTBd:
		break;
	case ESDTJx:
		break;
	case ESDTJxhgd:
		break;
	case ESDTYcxxhgd:
		break;
	case ESDTXxhgd:
		break;
	case ESDTZq:
		break;
	case ESDTFblq:
		break;
	case ESDTZs:
		break;
	case ESDTJejd:
		break;
	case ESDTZbj:
		bSave = true;
		m_PtPress = CPoint(-1,-1);
		m_iIdPress = 0;
		m_fValPress = 0;
		m_PtRelease = CPoint(-1,-1);
		m_iIdRelease = 0;
		m_fValRelease = 0;
		SDDrawBj(this,NULL,pDC,NULL,m_PtEnd.x,m_PtEnd.y,true);
		break;
	case ESDTDbj:
		bSave = true;
		m_PtPress = CPoint(-1,-1);
		m_iIdPress = 0;
		m_fValPress = 0;
		m_PtRelease = CPoint(-1,-1);
		m_iIdRelease = 0;
		m_fValRelease = 0;
		SDDrawBj(this,NULL,pDC,NULL,m_PtEnd.x,m_PtEnd.y,false);
		break;
	case ESDTZ45UP:
		break;
	case ESDTZ45DOWN:
		break;
	case ESDTZVertical:
		break;
	case ESDTText:
		m_PtPress = CPoint(-1,-1);
		m_iIdPress = 0;
		m_fValPress = 0;
		m_PtRelease = CPoint(-1,-1);
		m_iIdRelease = 0;
		m_fValRelease = 0;
		if ( IDOK == dlg.DoModal())
		{
			bSave = true;
			StrText = dlg.m_StrText;
			SDDrawText ( this, NULL, pDC,NULL,m_PtEnd.x,m_PtEnd.y,StrText);
		}
		break;
	default:
		break;
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	EndOverlay();

	if ( bSave )
	{
		CSelfDrawNode* pSelfDrawNode = new CSelfDrawNode;
		CTime Time = CTime::GetCurrentTime();
		pSelfDrawNode->t =(int32) Time.GetTime();
		pSelfDrawNode->m_eType = CChartRegion::m_eSelfDrawType;
		pSelfDrawNode->m_id1 = m_iIdPress;
		pSelfDrawNode->m_fVal1 = m_fValPress;
		pSelfDrawNode->m_id2 = m_iIdRelease;
		pSelfDrawNode->m_fVal2 = m_fValRelease;
		pSelfDrawNode->m_ide = m_iIdEnd;
		pSelfDrawNode->m_fVale = m_fValEnd;
		pSelfDrawNode->m_StrText = StrText;

		IChartRegionData* pChart = GetParentIoViewData().pChartRegionData;
		if (NULL != pChart && pChart->m_MerchParamArray.GetSize() > 0)
		{
			pSelfDrawNode->m_iInternal = (int32)pChart->m_MerchParamArray[0]->m_eTimeIntervalFull;
			pSelfDrawNode->m_iUserMin  = pChart->m_MerchParamArray[0]->m_iTimeUserMultipleMinutes;
			pSelfDrawNode->m_iUserDay = pChart->m_MerchParamArray[0]->m_iTimeUserMultipleDays;
			pSelfDrawNode->m_pMerch    = GetParentIoViewData().pIoViewBase->GetMerchXml();
		}

		//lint --e{423,429}
		CSelfDrawCurve* pSelfDrawCurve = new CSelfDrawCurve(this);
		pSelfDrawCurve->SetNode(pSelfDrawNode);
		pSelfDrawCurve->SetT(pSelfDrawNode->t);
		m_SelfDrawingCurves.Add(pSelfDrawCurve);
	

		SetSelfDrawType (ESDTInvalid, GetParentIoViewData().pChartRegionData);
		ClearSelfDrawValue();

		SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
		GetViewData()->GetView()->Invalidate();	
	}
}

bool32 CChartRegion::SetSelfDrawType ( E_SelfDrawType eDrawType, IChartRegionData* pChart)
{
	CChartRegion::m_iSelfDrawAppendOp = 0;

	switch ( eDrawType )
	{
	case ESDTLine:
		break;
	case ESDTLineEx:
		break;
	case ESDTJgtd:
		break;
	case ESDTPxzx:
		break;
	case ESDTYhx:
		break;
	case ESDTHjfg:
		break;
	case ESDTBfb:
		break;
	case ESDTBd:
		break;
	case ESDTJx:
		break;
	case ESDTJxhgd:
		break;
	case ESDTYcxxhgd:
		break;
	case ESDTXxhgd:
		break;
	case ESDTZq:
		break;
	case ESDTFblq:
		break;
	case ESDTZs:
		break;
	case ESDTJejd:
		break;
	case ESDTZbj:
		CChartRegion::m_iSelfDrawAppendOp = 1;
		break;
	case ESDTDbj:
		CChartRegion::m_iSelfDrawAppendOp = 1;
		break;
	case ESDTZ45UP:
		break;
	case ESDTZ45DOWN:
		break;
	case ESDTZVertical:
		break;
	case ESDTZHorizontal:
		break;
	case ESDTText:
		CChartRegion::m_iSelfDrawAppendOp = 1;
		break;
	default://ESDTInvalid
		CChartRegion::m_iSelfDrawAppendOp = 0;
	//	UpdateSelfDrawBar();
		CTabSplitWnd::m_pMainFram->SendMessage(UM_UpdateSelfDrawBar);
		break;
	}

	CChartRegion::m_eSelfDrawType = eDrawType;

	if (eDrawType != ESDTColor)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_OWNDRAW));
	}
	
	return true;
}

void CChartRegion::DrawSelfDrawing ( CMemDCEx* pDC,CMemDCEx* pPickDC)
{
	int32 i, iSize = m_SelfDrawingCurves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CSelfDrawCurve* pCurve = m_SelfDrawingCurves.GetAt(i);
		if (NULL != pCurve && pCurve->NeedShow())
		{
			pCurve->Draw(this,pDC,pPickDC);
		}		
	}
}

void CChartRegion::OnPreDraw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC)
{
	if ( NULL == pDC || NULL == pPickDC )
	{		
		return;
	}

	CString StrLog;

	//���ɱ���
	int32 iSize = m_Curves.GetSize();

	for ( int32 i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = m_Curves.GetAt(i);
		if ( NULL == pCurve )
		{
			continue;
		}

		//
		if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KUseNodesNameAsTitle) )
		{
			CNodeSequence* pNodes = pCurve->GetNodes();
			
			if ( NULL != pNodes )
			{
				CString StrTitle = pNodes->GetNamePrefix() + pNodes->GetName() + pNodes->GetNamePostfix();				
				pCurve->SetTitle(StrTitle);
			}
		}
	}

	//���û���
	CRegion::OnPreDraw(pDC, pPickDC);

	// ����ת��

	if ( !CheckFlag(m_iFlag, CChartRegion::KYDependent) )
	{
		// ...fangz0731 ÿ���ػ�����Ҫ����������?

		
		GetViewData()->OnCalcXAxis(this, pDC, m_aXAxisNodes, m_aXAxisDivide);
		
			
		GetViewData()->OnCalcYAxis(this, pDC, m_aYAxisDivide);		
		
	}
	
	//
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis))
	{
		CFont* pOldFont = NULL;
		if ( NULL != m_FontAxisText )
		{
			pOldFont = (CFont*)pDC->SelectObject(m_FontAxisText);
		}
		else
		{
			// xl 0303 Ĭ��ѡ��С����
			pOldFont = (CFont *)pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
		}

		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
		}
	}

	//
	if ( CheckFlag(m_iFlag,CChartRegion::KYAxis))
	{
		CFont* pOldFont = NULL;
		if ( NULL != m_FontAxisText )
		{
			pOldFont = (CFont*)pDC->SelectObject(m_FontAxisText);
		}
		else
		{
			// xl 0303 Ĭ��ѡ��С����
			pOldFont = (CFont *)pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
		}
	
		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
		}
	}
} 

#if 0
void CChartRegion::OnDraw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC)
{
	int32 i, iSize;
	CString StrText;
	CRect rect;

	// ���û����OnDraw
	CRegion::OnDraw(pDC,pPickDC);

	if ( NULL== pDC || NULL == pPickDC ) 
	{
		return;
	}
	// 
	CRect RectClient = GetRectCurves();
	if ( RectClient.right-RectClient.left < 16 
		|| RectClient.bottom-RectClient.top < 8 )
	{
		if ( !CheckFlag(m_iFlag,CChartRegion::KUserChart))
		{
			return;
		}
	}

	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
	{
		DrawNotify(pDC,pPickDC,ERDNBeforeDrawCurve);
	}

	// ����ÿһ��Curve��Draw
	iSize = m_Curves.GetSize();
	int32 iLoop = 0;
	
	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawCurves) )
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CChartCurve* pCurve = m_Curves.GetAt(i);

			if ( !CheckFlag(pCurve->m_iFlag,CChartCurve::KInVisible) )
			{				
				pCurve->Draw(pDC,pPickDC,iLoop);
				iLoop ++;
			}
		}
	}

	DrawSelfDrawing(pDC,pPickDC);

 	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
 	{
 		DrawTitle ( pDC,pPickDC );
 	}

	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
	{
		DrawNotify(pDC,pPickDC,ERDNAfterDrawCurve);
	}

	//////////////////////////////////////////////////////////////////////////
	// ������,���.�����ڵ�:
	if (NULL != m_pTitle)
	{
		SetRegionTitlePosition();
		SetTitleDC();		
		m_pTitle->ForcePaint();		
	}
	//////////////////////////////////////////////////////////////////////////
}
#else
// ��ͼ�Ż� - ����ЩTrace��Ϣ

void CChartRegion::OnDraw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC)
{
	int32 i, iSize;
	CString StrText;
	CRect rect;
	
	// ���û����OnDraw
	CRegion::OnDraw(pDC,pPickDC);
	
	if ( NULL== pDC || NULL == pPickDC ) 
	{
		return;
	}
	// xl 0607 ��С��һ��ֵ����ǩ����Ҫ����
	//			��ʱ��YLeft���Բ��û������� - ��Ҫ��IoView�е���
	CRect RectClient = GetRectCurves();
	if ( RectClient.right-RectClient.left < 16 
		|| RectClient.bottom-RectClient.top < 8 )
	{
		if ( !CheckFlag(m_iFlag,CChartRegion::KUserChart))
		{
			if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
			{
				DrawTitle ( pDC,pPickDC );
			}
			return;
		}
	}
	
	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
	{
		
		DrawNotify(pDC,pPickDC,ERDNBeforeDrawCurve);
		
	}

	IIoViewBase* pIoViewParent = GetParentIoViewData().pIoViewBase;
	
	if ( NULL != pIoViewParent )
	{
		if ( CheckFlag(m_iFlag, CChartRegion::KActiveFlag ) )
		{
			CRect rectActive = m_RectView;
			rectActive.top  += pIoViewParent->m_iActiveYpos;
			rectActive.left += pIoViewParent->m_iActiveXpos;
			rectActive.right = rectActive.left + 2;
			rectActive.bottom= rectActive.top  + 2;
			
			if ( pIoViewParent->IsActive() )
			{
				pDC->FillSolidRect(rectActive, pIoViewParent->m_ColorActive);
			}
			else
			{
				pDC->FillSolidRect(rectActive, pIoViewParent->GetIoViewColor(ESCBackground));
			}			
		}
	}
	
	// ����ÿһ��Curve��Draw
	iSize = m_Curves.GetSize();
	int32 iLoop = 0;
	
	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawCurves) )
	{
		for ( i = 0; i < iSize; i ++ )
		{
			CChartCurve* pCurve = m_Curves.GetAt(i);
			
			if ( !CheckFlag(pCurve->m_iFlag,CChartCurve::KInVisible) )
			{	
				pCurve->Draw(pDC,pPickDC,iLoop);

				// ������ÿ��curve����Ҫ�����ɫ�����������ض��������еĶ�����
				if ( pCurve->GetOccupyIndexLineClr() )
				{
					iLoop ++;
				}
			}
		}
	}
	
	DrawSelfDrawing(pDC,pPickDC);
		
	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
	{
		DrawTitle ( pDC,pPickDC );
	}
	
	if ( CheckFlag(GetDrawFlag(),CRegion::KDrawNotify) )
	{
		DrawNotify(pDC,pPickDC,ERDNAfterDrawCurve);
	}
	
	//////////////////////////////////////////////////////////////////////////
}
#endif

void CChartRegion::OnAfterDraw ( IN CMemDCEx* pOverlayDC )
{
	// ...fangz0828 ʮ�ֹ����˸. ��������Ϊû�м�⵽ CRegion::KDrawNotify �����־
	// ��ʱ���������. �������

	//if ( 1 /*CheckFlag(GetDrawFlag(),CRegion::KDrawNotify)*/ )
	{
		if ( CheckFlag(m_iFlag,CChartRegion::KCrossAble ) && m_bActiveCross )
		{
			// 0001903 ���ڲ��ܲ�׽����뿪�Ķ�����ȫ�����������ʾ��
			if ( GetViewData()->GetView()->GetSafeHwnd() == GetFocus() )
			{
				DrawCross(pOverlayDC,m_ptCross.x,m_ptCross.y,EDCMouseMove);											
			}
		}
		
		// ��仰���ܷŵ� if ����. ��Ϊ X ��� Y �ᶼû�� KCrossAble ����.�����ػ���ʱ�򻬿鶼û�л�
		DrawAxisSlider (pOverlayDC, m_ptCross );	
		DrawRectZoomOut(pOverlayDC);
	}
}

void CChartRegion::OnActive  ( bool32 bActive )
{

}

bool32 CChartRegion::PickNode (int32 x,int32 y, OUT CDrawingCurve*& pCurve, OUT CNodeSequence*& pNodes, OUT CNodeData& NodeData,bool32 bClearLast)
{
	pCurve = NULL;
	pNodes = NULL;
	CChartCurve* pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	CPoint pt(x,y);
	if ( !m_RectView.PtInRect(pt))
	{
		return false;
	}
	if ( bClearLast )
	{
		SetPickFlag ( NULL, NodeData );
		SetPickFlag ( NULL );
	}
	int32 iPickColor = GetViewData()->GetPickDC()->GetPixel(x,y);
	CDrawingCurve* pDrawingCurve = (CDrawingCurve*)CDrawingCurve::GetCurve(iPickColor);
	if ( NULL == pDrawingCurve 
		|| CheckFlag(pDrawingCurve->m_iFlag, CChartCurve::KDonotPick) )
	{
		return false;
	}
	if ( !pDrawingCurve->m_bSelfDraw )
	{
		CChartCurve *pChartCurve = (CChartCurve *)pDrawingCurve;
		if ( &pChartCurve->GetChartRegion() != this )
		{
			return false;	// �������region��
		}
	}
	pCurve = pDrawingCurve;
	if ( pDrawingCurve->m_bSelfDraw )
	{
		SetPickFlag ( (CSelfDrawCurve*)pDrawingCurve );
		return true;
	}
	int32 x2=x;
	ClientXToRegion(x2);

	int32 iPos = 0;
 	if ( pCurve->RegionXToCurvePos(x2,iPos) )
	{
		pNodes = ((CChartCurve*)pCurve)->GetNodes();
		if ( NULL != pNodes )
		{
			if ( pNodes->GetAt(iPos,NodeData) )
			{
				SetPickFlag ( NULL,NodeData );
				SetPickFlag((CChartCurve*)pCurve,NodeData);
				return true;
			}
		}
	}
	return false;
}

void CChartRegion::SetPickFlag(CChartCurve* pCurve, CNodeData& NodeData)
{
	//ɾ��KSelect��־
	if (NULL == m_pRegionRoot)
	{
		return;
	}
	RegionArray Regions;
	GetRegions (m_pRegionRoot, Regions);
	int32 i, iSize = Regions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CRegion* pRegion = Regions.GetAt(i);
		CChartRegion* pChartRegion = dynamic_cast<CChartRegion*>(pRegion);
		if ( NULL != pChartRegion )
		{
			int32 j, iSize2 = pChartRegion->m_Curves.GetSize();
			for ( j = 0; j < iSize2; j ++ )
			{
				CChartCurve* pCurve2 = pChartRegion->m_Curves.GetAt(j);
				RemoveFlag(pCurve2->m_iFlag,CDrawingCurve::KSelect);
				pCurve2->SetSelectNodeID(-1);
			}
		}
	}
	Regions.RemoveAll();

	if ( pCurve == NULL )
	{
		return;
	}
	//����KSelect��־
	AddFlag(pCurve->m_iFlag,CDrawingCurve::KSelect);
	//��¼ʰȡ����NodeData��m_iID
	pCurve->SetSelectNodeID(NodeData.m_iID);
}

void CChartRegion::SetPickFlag ( CSelfDrawCurve* pCurve )
{
	//ɾ��KSelect��־
	RegionArray Regions;
	GetRegions (m_pRegionRoot, Regions);
	int32 i, iSize = Regions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CRegion* pRegion = Regions.GetAt(i);
		CChartRegion* pChartRegion = dynamic_cast<CChartRegion*>(pRegion);
		if ( NULL != pChartRegion )
		{
			int32 j, iSize2 = pChartRegion->m_SelfDrawingCurves.GetSize();
			for ( j = 0; j < iSize2; j ++ )
			{
				CSelfDrawCurve* pCurve2 = pChartRegion->m_SelfDrawingCurves.GetAt(j);
				RemoveFlag(pCurve2->m_iFlag,CDrawingCurve::KSelect);
			}
		}
	}
	Regions.RemoveAll();

	if ( pCurve == NULL )
	{
		return;
	}
	//����KSelect��־
	AddFlag(pCurve->m_iFlag,CDrawingCurve::KSelect);
}

void CChartRegion::OnMouseOverAWhile ( int32 x, int32 y )
{
	if ( m_bActiveCross )
	{
		return;
	}
	if ( !m_RectView.PtInRect(CPoint(x,y)))
	{
		return;
	}
	CDrawingCurve* pDrawingCurve;
	CChartCurve* pCurve;
	CNodeSequence* pNodes;
	CNodeData NodeData;
	if ( PickNode (x, y, pDrawingCurve, pNodes, NodeData, false) )
	{
		SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
		GetViewData()->GetView()->Invalidate();
		if ( pDrawingCurve->m_bSelfDraw )
		{
			
		}
		else
		{
			pCurve = (CChartCurve*)pDrawingCurve;
			GetViewData()->OnPickNode (this, pCurve, pNodes, &NodeData, x, y, 0);
		}
	}
}

void CChartRegion::OnMouseOverLeave ( int32 x, int32 y )
{
	CPoint pt(x,y);
	if ( !m_RectView.PtInRect(pt))
	{
		return;
	}
	GetViewData()->OnPickNode ( this,NULL,NULL,NULL,x,y,0);
}

void CChartRegion::OnKeyDel()
{
	CChartCurve* pCurve = NULL;
	int32 i, iSize = m_Curves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		pCurve = m_Curves.GetAt(i);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
			break;
		}
		pCurve = NULL;
	}
	if ( pCurve != NULL )
	{
		GetViewData()->OnDropCurve ( this, pCurve, NULL);
		return;
	}
	bool32 bReDraw = false;
	iSize = m_SelfDrawingCurves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CSelfDrawCurve* pSelfDrawCurve = m_SelfDrawingCurves.GetAt(i);
		if ( CheckFlag(pSelfDrawCurve->m_iFlag,CDrawingCurve::KSelect))
		{
			if (pSelfDrawCurve == GetParentIoViewData().pChartRegionData->GetPickedSelfDrawCurve())
			{
				GetParentIoViewData().pChartRegionData->SetPickedSelfDrawCurve(NULL);
			}

			
			m_SelfDrawingCurves.RemoveAt(i);
			//	modify by weng.cx
			//	doubt memory leak, 
			DEL(pSelfDrawCurve);
			//SUP
			bReDraw = true;
			break;
		}
	}
	if ( bReDraw )
	{
		SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
		GetViewData()->GetView()->Invalidate();
	}
}

void CChartRegion::OnDrawAxisSliderText ( IN CMemDCEx* pDC,CRect rect,const CPoint pt )
{
	int32 id,rx = pt.x;
	int32 ry = pt.y;
	float fValue;
	CString StrText;

	CChartRegion* pDataSourceRegion = GetDataSourceRegion();
	
	if ( NULL == pDataSourceRegion)
	{
		return;
	}

	CChartCurve* pCurveDependent = pDataSourceRegion->GetDependentCurve();
	
	if (NULL == pCurveDependent)
	{
		return;
	}

	//���������x��
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis ))
	{
		ClientXToRegion(rx);
		
		if (bNoData())
		{
			// û�����ݵ�ʱ��,X ��Ļ�����ʾ���ݲ���Ϊ��						
			
			IChartRegionData * pIoViewChart = GetViewData()->GetRegionParentIoView().pChartRegionData;
			
			if ( NULL == pIoViewChart)
			{				
				// zhangbo 20090703 #��ʱ�ܵ�
				// �쳣�����,�Լ���
				// 	for (int32 i = 0 ; i < m_AxisRects.GetSize();i++)
				// 	{
				// 		CRect RectX = m_AxisRects.GetAt(i);
				// 		if ( pt.x < RectX.right && pt.x >= RectX.left)
				// 		{
				// 			m_StrAxisTextsOfNoData = m_AxisTexts.GetAt(i);
				// 		}				
				// 	}			
			}
			else
			{
				// �������,��K�� Reaion ����ȡ��.
				m_StrAxisTextsOfNoData = pIoViewChart->m_pRegionMain->m_StrAxisTextsOfNoData;
			}
			
			StrText = m_StrAxisTextsOfNoData;						
		}
		else
		{
			if ( pCurveDependent->RegionXToCurvePos(rx, id))
			{
				if (id >= 0 && id < m_aXAxisNodes.GetSize())
				{
					CAxisNode *pAxisNodes = (CAxisNode *)m_aXAxisNodes.GetData();
					//StrText = pAxisNodes[id].m_StrSliderText1;
					GetParentIoViewData().pChartRegionData->GetChartXAxisSliderText1(StrText, pAxisNodes[id]);
				}
			}
		}

		pDC->SetTextColor(m_clrSliderText);
		CFont* pOldFont = NULL;
		if ( NULL != m_FontSliderText )
		{
			pOldFont = (CFont*)pDC->SelectObject(m_FontSliderText);
		}
		pDC->DrawText(StrText,rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
		}				
	}
	else if ( CheckFlag(m_iFlag,CChartRegion::KYAxis ))
	{
		// ...fangz0815 ��ʱ����,�Ժ������Ͻ�����:
		// ������ͼ��ֻ����һ��Y�����ÿ��price��λ��, ����һ��Y����ṩһ����ʾ����
		// tdx��ò�Ʒ�ʱͼ��������

		// ����ʾ��ֵ

// 		if ( GetParentIoView()->IsKindOf(RUNTIME_CLASS(CIoViewTrend)) && m_StrName == L"��y��")
// 		{
// 			// ��ʱ���Ƶ��� Y �Ử����ʾ�ٷֱ�:
// 			ClientYToRegion(ry);
// 			float fBase;
// 			if ( pCurveDependent->RegionYToAxisY(ry,fValue) && pCurveDependent->GetAxisBaseY(fBase) )
// 			{
// 				// ʵ�ʴ�ʱ��ȡ���ǰٷֱȵ�axisֵ
// 				fValue = (fValue-fBase)*100;
// 				if ( fValue > -0.01 && fValue < 0.01 )
// 				{
// 					fValue = 0.0f;
// 				}
// 				StrText.Format(_T("%0.2f%%"), fValue);
// 				pDC->SetTextColor(m_clrSliderText);
// 				CFont* pOldFont = NULL;
// 				if ( NULL != m_FontSliderText )
// 				{
// 					pOldFont = (CFont*)pDC->SelectObject(m_FontSliderText);
// 				}
// 				pDC->DrawText(StrText,rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
// 				if ( NULL != pOldFont )
// 				{
// 					pDC->SelectObject(pOldFont);
// 				}
// 			}
// 			 
// 		}
// 		else
		{
			ClientYToRegion(ry);
			if ( pCurveDependent->RegionYToPriceY(ry,fValue))
			{
				StrText = GetViewData()->OnFloat2String ( fValue, false, false );
				pDC->SetTextColor(m_clrSliderText);
				CFont* pOldFont = NULL;
				if ( NULL != m_FontSliderText )
				{
					pOldFont = (CFont*)pDC->SelectObject(m_FontSliderText);
				}
				pDC->DrawText(StrText,rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
				if ( NULL != pOldFont )
				{
					pDC->SelectObject(pOldFont);
				}
			}			
		}
	}
}

void CChartRegion::DrawRound ( CMemDCEx* pDC)
{
/*
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis))
	{
		return;
	}
	if ( CheckFlag(m_iFlag,CChartRegion::KYAxis))
	{
		return;
	}

	//����Pen
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clrBorder);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	//����Skip
	CRect rect = GetRectCurves();
	CChartCurve* pCurve = GetDependentCurve();
	if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeTrend))
	{

	}
	else
	{

	}
	pDC->_DrawLine(m_RectView.left,rect.top,m_RectView.right,rect.top);
	pDC->_DrawLine(m_RectView.left,rect.bottom,m_RectView.right,rect.bottom);

	//ɾ��Pen
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
*/
 
} 

void CChartRegion::DrawSaneIndexTitle(CMemDCEx* pDC,int32 iPos,bool32 bMouseMove /* = false */)
{	
	CString StrForPrompt  = L"";

	IChartRegionData* pKLine  = GetParentIoViewData().pChartRegionData;
	CChartCurve* pDependCurve = GetDependentCurve();
	if ( NULL == pDependCurve )
	{
		return;
	}

	CString StrMainTitle  = pDependCurve->GetTitle();
	//
	CMemDCEx * pMainDC = GetViewData()->GetMainDC();	
	CSize size = pMainDC->GetTextExtent(StrMainTitle);
	//
	CFont* pOldFont = NULL;
	if ( NULL != m_FontTitle )
	{
		pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
	}	

	COLORREF clr = m_clrTitle;	
	pDC->SetBkMode(TRANSPARENT);	
	//
// 	CImageList Imaglist;
// 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 	Imaglist.Create(MAKEINTRESOURCE(IDB_GRID_REPORT/*IDB_TREND_TITLE*/),7,1,RGB(255,255,255));
	
	// ...fangz0808 �����ҵ�������,�Ժ��ٸ�.��ʱ�ȴ���
	if ( pKLine->GetRisFallFlag())
	{
		clr =GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise);
	}
	else
	{
		clr =GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall);
	}

	if ( bMouseMove )
	{
		// ����MainDc ���������.
		
		CRect Rect1  = m_RectView;
		
		Rect1.top   += 2;
		Rect1.left	+= 2;
		Rect1.right  = m_RectView.right;	
		Rect1.bottom = Rect1.top + size.cy;		
		//Rect1.left	+= size.cx;
		
		IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
		if(NULL == pIoViewParent)
		{
			return;
		}		
		pMainDC->FillSolidRect(Rect1,pIoViewParent->GetIoViewColor(ESCBackground));	
	}
	
	CRect RectMain = m_RectView;
	RectMain.left += 2;
	RectMain.top  += 2;
	pDC->SetTextColor(clr);
	DrawTitleText(pDC,NULL,NULL,StrMainTitle,RectMain);

	clr = m_clrTitle;
	pDC->SetTextColor(clr);

	int32 iImageWidth = 13;

	if ( NULL != pKLine->m_pImageShake)
	{
		iImageWidth = pKLine->m_pImageShake->GetWidth();
	}

	CRect rect	 = m_RectView;
	rect.top	+= 2;
	rect.bottom  = rect.top + size.cy;
	rect.left	+= size.cx;

	// ��ǰ���ڶ�Ӧ�Ľڵ�

	T_SaneIndexNode Node;
		
	bool32 bOK1 = pKLine->GetSaneIndexNode(iPos, Node);

	if ( !bOK1 )
	{
		return;
	}
	
	// 1: ͼ�� 2: K �߸���(��Ļ����ʾ����) 3: ���״��� 4: ׼ȷ�� 5: ������
	Graphics GraphicImage(pDC->GetSafeHdc());
	
	E_SaneIntensity eSaneIntensity = BeStrong(Node);
	
	COLORREF clr0 = GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCKeep);
	
	if ( ESMIDT == Node.m_eSaneMarketInfo )
	{
		clr0 = GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise);
		pDC->SetTextColor(clr0);
	}
	else if ( ESMIKT == Node.m_eSaneMarketInfo )
	{
		clr0 = GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall);
		pDC->SetTextColor(clr0);
	}

	if ( ESMIDT == Node.m_eSaneMarketInfo )
	{
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,����
				StrForPrompt += L"ǿ,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"һ��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityUp, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,����
				StrForPrompt += L"ǿ,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"һ��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityUp, rect, 1, 0, false);
			}					
		}
	}
	else if ( ESMIKT == Node.m_eSaneMarketInfo )
	{
		// �½�����
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,�½�
				StrForPrompt += L"ǿ,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"һ��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityDown, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,�½�
				StrForPrompt += L"ǿ,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"һ��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityDown, rect, 1, 0, false);
			}
		}
	}
	else
	{
		// ������			
		StrForPrompt += L"������\r\n";
		DrawImage(GraphicImage, pKLine->m_pImageShake, rect, 1, 0, false);
	}

	/*
	if ( ESISDTOC == Node.m_eSaneIndexState || ESISDTCY == Node.m_eSaneIndexState)
	{
		clr0 = pKLine->GetIoViewColor(ESCRise);
		pDC->SetTextColor(clr0);
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState || ESISKTCY == Node.m_eSaneIndexState)
	{
		clr0 = pKLine->GetIoViewColor(ESCFall);
		pDC->SetTextColor(clr0);
	}

	if ( ESISDTOC == Node.m_eSaneIndexState || ESISDTCY == Node.m_eSaneIndexState )
	{		
		// ��������
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,����
				StrForPrompt += L"ǿ,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"һ��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityUp, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,����
				StrForPrompt += L"ǿ,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"һ��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityUp, rect, 1, 0, false);
			}					
		}
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState || ESISKTCY == Node.m_eSaneIndexState )
	{
		// �½�����
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,�½�
				StrForPrompt += L"ǿ,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"һ��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityDown, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,�½�
				StrForPrompt += L"ǿ,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"һ��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityDown, rect, 1, 0, false);
			}
		}
	}
	else
	{
		// ������			
		StrForPrompt += L"������\r\n";
		DrawImage(GraphicImage, pKLine->m_pImageShake, rect, 1, 0, false);
	}
	*/

	rect.left += iImageWidth + 3;
		
	// �ֲ�״̬/������ʾ :
	CMerch* pMerch = GetParentIoViewData().pIoViewBase->m_pMerchXml;
	bool32 bFuture = false;

	if ( NULL != pMerch )
	{
		bFuture = CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
	}
	
	CString StrTrendTitle = GetNodeStateString(bFuture, Node);
	StrTrendTitle += L" ";
	pDC->DrawText(StrTrendTitle,&rect,DT_LEFT|DT_TOP|DT_SINGLELINE);
	size = pDC->GetTextExtent(StrTrendTitle);
	rect.left += size.cx;
	pDC->SetTextColor(m_clrTitle);
	
	// ���״���,׼ȷ��,������:
	CString StrText = L" K ����:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ����:
	StrText.Format(L"%d",pKLine->m_iSaneRealKlineNums);
	
	
	pDC->SetTextColor(RGB(224, 128, 224));
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	pDC->SetTextColor(m_clrTitle);
	//
	
	// ����������
	StrText = L"������:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ���״���
	StrText.Format(L"%d", pKLine->m_iSaneTradeTimes);
	pDC->SetTextColor(RGB(224, 128, 224));
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ׼ȷ��
	StrText = L"׼ȷ��:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ׼ȷ������
	float fRate = (float)100 * pKLine->m_fSaneAccuracyRate;	
	StrText.Format(L"%.2f%%",fRate);
	
	if ( fRate >= (float)50 )
	{
		pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCRise));
	}
	else
	{
		pDC->SetTextColor(RGB(224, 128, 224));
	}
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	// ������
	StrText = L"������: ";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ������
	float fProfitRate = (float)100 * pKLine->m_fSaneProfitability;
	if( fProfitRate >= 0.0 )
	{
		pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCRise));
	}
	else
	{
		pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCFall));
	}
	
	StrText.Format(L"%.2f%% ",fProfitRate);
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ����
	
	StrText = L"����: ";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ����

	int32 iSaveDec = 2;

	if ( NULL != pMerch )
	{		
		iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}
	
	float fProfit = pKLine->m_fProfit;
	
	StrText = Float2String(fProfit, iSaveDec, false, false);
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
#ifdef DEBUG	
	//
	CString StrDebug;
//	CString StrText;

// 	StrDebug.Format(L" ����ֵ: %.2f, %s ", Node.m_fTrendValue, GetNodeMarketString(Node.m_eSaneMarketInfo));
// 	pDC->DrawText(StrDebug, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
// 	size = pDC->GetTextExtent(StrDebug);
// 	rect.left += size.cx;
// 
// 	//
//  	StrText.Format(L"%.2f [%d]", Node.m_fIntensity, BeStrong(Node));
//  	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
//  	rect.left += 100;
 	
	// 	�г�,�ڵ�����(������):
 	CString StrNodeCross;
 	StrNodeCross.Format(L" %d", iPos);
 	//  
 	pDC->DrawText(StrNodeCross, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
#endif

	pDC->SelectObject(pOldFont);
}

/*
void CChartRegion::DrawSaneIndexTitle(CMemDCEx* pDC,int32 iPos,bool32 bMouseMove )
{	
	CString StrForPrompt  = L"";

	CIoViewKLine* pKLine  = (CIoViewKLine*)GetParentIoView();
	CChartCurve* pDependCurve = GetDependentCurve();
	if ( NULL == pDependCurve )
	{
		return;
	}

	CString StrMainTitle  = pDependCurve->GetTitle();
	//
	CMemDCEx * pMainDC = GetViewData()->GetMainDC();	
	CSize size = pMainDC->GetTextExtent(StrMainTitle);
	//
	CFont* pOldFont = NULL;
	if ( NULL != m_FontTitle )
	{
		pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
	}	

	COLORREF clr = m_clrTitle;	
	pDC->SetBkMode(TRANSPARENT);	
	//
	CImageList Imaglist;
	Imaglist.Create(MAKEINTRESOURCE(IDB_TREND_TITLE),7,1,RGB(255,255,255));
	
	// ...fangz0808 �����ҵ�������,�Ժ��ٸ�.��ʱ�ȴ���
	if ( pKLine->GetRisFallFlag())
	{
		clr = GetParentIoView()->GetIoViewColor(ESCRise);
	}
	else
	{
		clr = GetParentIoView()->GetIoViewColor(ESCFall);
	}
	
	CRect RectMain = m_RectView;
	RectMain.left += 2;
	RectMain.top  += 2;
	pDC->SetTextColor(clr);
	DrawTitleText(pDC,NULL,NULL,StrMainTitle,RectMain);

	clr = m_clrTitle;
	pDC->SetTextColor(clr);

	if ( bMouseMove )
	{
		// ����MainDc ���������.
		
		CRect Rect1  = m_RectView;

		Rect1.top   += 2;
		Rect1.right  = m_RectView.right;	
		
		if ( NULL != m_pTitle )
		{
			Rect1.right -= (m_pTitle->GetButtonNums()*IOVIEW_TITLE_HEIGHT + 20);				
		}

		Rect1.bottom = Rect1.top + size.cy;		
		Rect1.left	+= size.cx;

		CIoViewBase * pIoViewParent = GetParentIoView();
		ASSERT( NULL != pIoViewParent );
		pMainDC->FillSolidRect(Rect1,pIoViewParent->GetIoViewColor(ESCBackground));	
	}

	int32 iImageWidth = 13;

	if ( NULL != pKLine->m_pImageShake)
	{
		iImageWidth = pKLine->m_pImageShake->GetWidth();
	}

	CRect rect	 = m_RectView;
	rect.top	+= 2;
	rect.bottom  = rect.top + size.cy;
	rect.left	+= size.cx;

	// ��ǰ���ڶ�Ӧ�Ľڵ�

	T_SaneIndexNode Node;
		
	bool32 bOK1 = pKLine->GetSaneIndexNode(iPos, Node);

	if ( !bOK1 )
	{
		return;
	}
	
	// 1: ͼ�� 2: K �߸���(��Ļ����ʾ����) 3: ���״��� 4: ׼ȷ�� 5: ������
	Graphics GraphicImage(pDC->GetSafeHdc());
	
	E_SaneIntensity eSaneIntensity = BeStrong(Node);
	
	COLORREF clr0 = pKLine->GetIoViewColor(ESCKeep);
	
	if ( ESISDTOC == Node.m_eSaneIndexState || ESISDTCY == Node.m_eSaneIndexState)
	{
		clr0 = pKLine->GetIoViewColor(ESCRise);
		pDC->SetTextColor(clr0);
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState || ESISKTCY == Node.m_eSaneIndexState)
	{
		clr0 = pKLine->GetIoViewColor(ESCFall);
		pDC->SetTextColor(clr0);
	}

	if ( ESISDTOC == Node.m_eSaneIndexState || ESISDTCY == Node.m_eSaneIndexState )
	{
	
		
		// ��������
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,����
				StrForPrompt += L"ǿ,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"һ��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,����
				StrForPrompt += L"��,�ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityUp, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,����
				StrForPrompt += L"ǿ,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"һ��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityUp, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,����
				StrForPrompt += L"��,���ȶ�,��������\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityUp, rect, 1, 0, false);
			}					
		}
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState || ESISKTCY == Node.m_eSaneIndexState )
	{
		// �½�����
		if ( Node.m_bStability )
		{
			// �ȶ�
			
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,�ȶ�,�½�
				StrForPrompt += L"ǿ,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"һ��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,�ȶ�,�½�
				StrForPrompt += L"��,�ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekStabilityDown, rect, 1, 0, false);
			}	
		}
		else
		{
			// ���ȶ�
			if ( ETISTRONG == eSaneIntensity )
			{
				// ǿ,���ȶ�,�½�
				StrForPrompt += L"ǿ,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageStrongUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETINORMAL == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"һ��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageNormalUnStabilityDown, rect, 1, 0, false);
			}
			else if ( ETIWEEK == eSaneIntensity)
			{
				// ��,���ȶ�,�½�
				StrForPrompt += L"��,���ȶ�,�½�����\r\n";
				DrawImage(GraphicImage, pKLine->m_pImageWeekUnStabilityDown, rect, 1, 0, false);
			}
		}
	}
	else
	{
		// ������			
		StrForPrompt += L"������\r\n";
		DrawImage(GraphicImage, pKLine->m_pImageShake, rect, 1, 0, false);
	}
	
	rect.left += iImageWidth + 3;
		
	// �ֲ�״̬/������ʾ :
	CMerch* pMerch = GetParentIoView()->m_pMerchXml;
	bool32 bFuture = false;

	if ( NULL != pMerch )
	{
		bFuture = CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
	}
	
	CString StrTrendTitle = GetNodeStateString(bFuture, Node);
	StrTrendTitle += L" ";
	pDC->DrawText(StrTrendTitle,&rect,DT_LEFT|DT_TOP|DT_SINGLELINE);
	size = pDC->GetTextExtent(StrTrendTitle);
	rect.left += size.cx;
	pDC->SetTextColor(m_clrTitle);
	
	// ���״���,׼ȷ��,������:
	CString StrText = L" K ����:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ����:
	StrText.Format(L"%d",pKLine->m_iSaneRealKlineNums);
	pDC->SetTextColor(RGB(224, 128, 224));
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	pDC->SetTextColor(m_clrTitle);
	//
	
	// ����������
	StrText = L"������:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ���״���
	StrText.Format(L"%d", pKLine->m_iSaneTradeTimes);
	pDC->SetTextColor(RGB(224, 128, 224));
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ׼ȷ��
	StrText = L"׼ȷ��:";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	
	// ׼ȷ������
	float fRate = (float)100 * pKLine->m_fSaneAccuracyRate;	
	StrText.Format(L"%.2f%%",fRate);
	
	if ( fRate >= (float)50 )
	{
		pDC->SetTextColor(pKLine->GetIoViewColor(ESCRise));
	}
	else
	{
		pDC->SetTextColor(RGB(224, 128, 224));
	}
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	rect.left += 3;
	// ������
	StrText = L"������: ";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ������
	float fProfitRate = (float)100 * pKLine->m_fSaneProfitability;
	if( fProfitRate >= 0.0 )
	{
		pDC->SetTextColor(pKLine->GetIoViewColor(ESCRise));
	}
	else
	{
		pDC->SetTextColor(pKLine->GetIoViewColor(ESCFall));
	}
	
	StrText.Format(L"%.2f%% ",fProfitRate);
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ����
	
	StrText = L"����: ";
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
	// ����
	int32 iSaveDec = 2;

	if ( NULL != pMerch )
	{		
		iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}
	
	float fProfit = pKLine->m_fProfit;
	
	StrText = Float2String(fProfit, iSaveDec, false, false);
	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(m_clrTitle);
	size = pDC->GetTextExtent(StrText);
	rect.left += size.cx;
	
#ifdef DEBUG
	// 	�г�,�ڵ�����(������):
// 	StrText.Format(L"%.2f [%d]", Node.m_fIntensity, BeStrong(Node));
// 	pDC->DrawText(StrText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
// 	rect.left += 100;
 	
 	CString StrNodeCross;
 	StrNodeCross.Format(L" %d", iPos);
 	//  
 	pDC->DrawText(StrNodeCross, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
#endif

	pDC->SelectObject(pOldFont);
}
*/

void CChartRegion::DrawTitle ( CMemDCEx* pDC,CMemDCEx* pPickDC,bool32 bMouseMove /*= false*/)
{
	if (NULL == pDC || NULL == pPickDC)
	{
		return;
	}
	// ����ƶ���ʱ��,��ָ��ֵ,Ҫ�Ȳ���maindc ��ԭ��������.�ٻ�.
	if ( IsActiveCross() && !bMouseMove )
	{
		// ��ʮ�ֹ��,���ݸ��µ�ʱ��:
		CPoint pt;
		GetCursorPos(&pt);		
		GetParentIoViewData().pWnd->ScreenToClient(&pt);
		DrawIndexValueByMouseMove(pt.x, pt.y, false);
	}

	if ( NULL != pDC )
	{
		CString StrTest(_T("�߶�"));
		CFont *pOldFont =  NULL == m_FontTitle ? NULL : (CFont *)pDC->SelectObject(m_FontTitle);
		CSize sizeTest = pDC->GetTextExtent(StrTest);
		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
		}
		if ( GetTopSkip() < sizeTest.cy )
		{
			return;	// ��chartԤ���ı���߶�̫�٣����ػ���
		}
	}

	IChartRegionData * pParent = GetParentIoViewData().pChartRegionData;
	if ( NULL == pParent)
	{
		return;
	}

	CChartCurve* pDependCurve = GetDependentCurve();
	if ( NULL == pDependCurve )
	{
		return;
	}

	bool32 bKLineMain = false;
	CIoViewTrend *pTrend = NULL;
	bool32 bArbKMain = false;

	if ( 1 == pParent->m_iChartType && this == pParent->m_pRegionMain )
	{
		bKLineMain = true;
	}
	else if ( 0 == pParent->m_iChartType && this == pParent->m_pRegionMain )
	{
		if(GetParentIoViewData().pIoViewBase->IsKindOfIoViewTrend())
			pTrend = (CIoViewTrend *)pParent;
	}
	else if ( 4 == pParent->m_iChartType && this == pParent->m_pRegionMain )
	{
		bArbKMain = true;
	}

	// K/��ʱ��ͼ��Ϊioview������ͼ����£�������Щ�ر�
	bool32 bParentIsIoView = GetParentIoViewData().pIoViewBase->ParentIsIoView();
	if ( bParentIsIoView 
		&& (bKLineMain || pTrend != NULL) )
	{
		// K��: ��Ʒ ���� �ּ� �ǵ��� �ǵ� ���� ������
		
		// ��ʱ: ��Ʒ �ּ� �ǵ��� �ǵ� ���� ������
		CMerch *pMerch = GetParentIoViewData().pIoViewBase->GetMerchXml();
		if ( NULL != pMerch )
		{
			CRect rect = m_RectView;
			rect.left += 2;
			rect.top += 2;

			// ����MainDc ������ͼ�ı���
			if ( pDC == GetViewData()->GetMainDC() )
			{
				CMemDCEx * pMainDC = GetViewData()->GetMainDC();						
				
				CString StrTitle = _T("�߶Ȳ���");
				CSize size = pMainDC->GetTextExtent(StrTitle);
				
				CRect Rect1 = rect;
				Rect1.right = m_RectView.right;						
				Rect1.bottom = Rect1.top + size.cy;
				
				IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
				if (NULL != pIoViewParent)
				{
					pMainDC->FillSolidRect(Rect1,pIoViewParent->GetIoViewColor(ESCBackground));
				}
				
			}
			
			CString StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
			if ( bKLineMain )
			{
				// ��������
				T_MerchNodeUserData *pData = pParent->m_MerchParamArray.GetSize() > 0 ? pParent->m_MerchParamArray[0] : NULL;
				if ( NULL != pData )
				{
					StrMerchName += _T(" (") 
						+ TimeInterval2String(pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, pData->m_iTimeUserMultipleDays)
						+ _T(")    ");
				}
			}
			pDC->SetTextColor(m_clrTitle);
			CFont* pOldFont = NULL;
			if ( NULL != m_FontTitle )
			{
				pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
			}
			
			int iOldBkMode = TRANSPARENT;
			COLORREF clrOldBk = 0;
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCBackground));
			if (GetParentIoViewData().pIoViewBase->IsActive() )
			{
				iOldBkMode  = pDC->SetBkMode(OPAQUE);	// �������ֱ���
				clrOldBk	= pDC->SetBkColor(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCGuideLine16));
			}
			bool32 bDraw = DrawTitleText(pDC,NULL,NULL,StrMerchName,rect);	// ��Ʒ����
			if ( GetParentIoViewData().pIoViewBase->IsActive() )
			{
				pDC->SetBkMode(iOldBkMode);
				pDC->SetBkColor(clrOldBk);
			}

			if ( NULL != pMerch->m_pRealtimePrice && bDraw && pMerch->m_pRealtimePrice->m_fPriceNew != 0.0f )
			{
				CString StrValue;
				const CRealtimePrice &RealTime =  *(pMerch->m_pRealtimePrice);
				float fPreClose = pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTFuturesCn ?
					RealTime.m_fPricePrevAvg : RealTime.m_fPricePrevClose;
				COLORREF clrPrice = GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCKeep);
				if ( RealTime.m_fPriceNew > fPreClose )
				{
					clrPrice =  GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise);
				}
				else if ( RealTime.m_fPriceNew < fPreClose )
				{
					clrPrice =  GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall);
				}

				StrValue = Float2String(RealTime.m_fPriceNew, pMerch->m_MerchInfo.m_iSaveDec);
				StrValue += _T("      ");
				float fRise = RealTime.m_fPriceNew-fPreClose;
				if ( fPreClose > 0.000001f || fPreClose < -0.000001f )
				{
					float fV = (fRise)/fPreClose*100;	
					CString StrV;
					StrV.Format(_T("%0.2f%%      "), (fV));
					StrValue += StrV;
				}
				if ( fRise > 0.0f )
				{
					CString StrV;
					StrV.Format(_T("��%0.*f      "), pMerch->m_MerchInfo.m_iSaveDec, fRise);
					StrValue += StrV;
				}
				else if ( fRise < 0.0f )
				{
					CString StrV;
					StrV.Format(_T("��%0.*f      "), pMerch->m_MerchInfo.m_iSaveDec, fRise);
					StrValue += StrV;
				}
				else
				{
					CString StrV;
					StrV.Format(_T("%0.*f      "), pMerch->m_MerchInfo.m_iSaveDec, fRise);
					StrValue += StrV;
				}

				pDC->SetTextColor(clrPrice);
				bDraw = DrawTitleText(pDC,NULL,NULL,StrValue,rect);	// �ּ� %% �ǵ�

				StrValue.Empty();
				// ������Ҫ��ȥ�������ͻ�����
				/*pDC->SetTextColor(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCText));				
				StrValue.Format(_T("���� %0.0f ������ %0.2f%%"), RealTime.m_fVolumeCur, RealTime.m_fTradeRate);
				if ( bDraw )
				{
					bDraw = DrawTitleText(pDC,NULL,NULL,StrValue,rect);
				}*/
			}
		}
		return;
		
	}

	if ( bKLineMain )
	{
		IChartRegionData* pKLine = GetParentIoViewData().pChartRegionData;
		
		if ( NULL == pKLine )
		{
			return;
		}
		
		if ( pKLine->GetShowSaneIndexFlag() )
		{
			if ( !bMouseMove && !IsActiveCross())
			{
				// �����µ�һ���ڵ�
				if ( pParent->m_MerchParamArray.GetSize() <= 0)				
				{
					return;
				}
				
				T_MerchNodeUserData* pData = pParent->m_MerchParamArray.GetAt(0);
				
				if ( NULL != pData)
				{
					int32 iSize = pData->m_aKLinesFull.GetSize();
					DrawSaneIndexTitle(pDC,iSize-1,bMouseMove);
				}			
			}	
			else
			{
				// �����ָ�Ľڵ�
				DrawSaneIndexTitle(pDC,m_iMouseMoveTrendIndexPos,bMouseMove);
			}
			
			return;				
		}
	}

	int32 oldMode = pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);
	if ( CheckFlag(m_iFlag,CChartRegion::KChart) )
	{
		CString strZhouQi = _T("");
		CMerch *pMerch = GetParentIoViewData().pIoViewBase->GetMerchXml();
		if ((NULL!=pMerch) && bKLineMain)
		{
			CString strCode = pMerch->m_MerchInfo.m_StrMerchCode + _T(" ");
			CString strName = pMerch->m_MerchInfo.m_StrMerchCnName;

			T_MerchNodeUserData *pData = pParent->m_MerchParamArray.GetSize() > 0 ? pParent->m_MerchParamArray[0] : NULL;
			if ( NULL != pData )
			{
				strZhouQi = _T(" [") + strName + _T(" ")
					+ TimeInterval2String(pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, pData->m_iTimeUserMultipleDays)
					+ _T("] ");
			}

			m_StrTitle =  strZhouQi + pData->m_pKLinesShow->GetWeightName();
		}

		CString StrTitle = m_StrTitle;
		CRect rect = m_RectView;
		rect.left += 2;
		rect.top += 2;

		// ��region����
		if ( StrTitle.GetLength() > 0 )
		{
			pDC->SetTextColor(m_clrTitle);
			CFont* pOldFont = NULL;
			if ( NULL != m_FontTitle )
			{
				pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
			}
			
			//////////////////////////////////////////////////////////////////////////
			if ( bMouseMove && pDC == GetViewData()->GetMainDC() )
			{
				// ����MainDc ���������.
				
				CMemDCEx * pMainDC = GetViewData()->GetMainDC();						
				
				CString StrTitle1 = pDependCurve->GetTitle();
				CSize size = pMainDC->GetTextExtent(StrTitle1);
				
				CRect Rect1 = rect;
				Rect1.right = m_RectView.right;						
				Rect1.bottom = Rect1.top + size.cy;

				IChartRegionData * pParent1 = GetParentIoViewData().pChartRegionData;
				// if ( pParent->m_pRegionMain == this )
				//{
				//	Rect1.left += size.cx;
				//}
				if (NULL != GetParentIoViewData().pChartRegionData)
				{
					if (!GetParentIoViewData().pChartRegionData->BeShowTopToolBar())
					{

						if ( pParent1->m_pRegionMain == this )
						{
							Rect1.left += size.cx;
						}
					}
				}
				
			
				IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
				if (NULL != pIoViewParent)
				{
					pMainDC->FillSolidRect(Rect1, pIoViewParent->GetIoViewColor(ESCBackground));
				}
				
				
			}
			
			pDC->SetTextColor(0xfeed68);
			DrawTitleText(pDC,NULL,NULL,StrTitle,rect);
			if ( NULL != pOldFont )
			{
				pDC->SelectObject(pOldFont);
			}		

			// ��ʾ������ť
			if ((1==pParent->m_iChartType) || (4==pParent->m_iChartType))
			{
				DrawIndexBtn(pDC, INDEX_HELP, rect);
			}
		}
		
		/////////////////////////////////////////////////////////////////////////		
		// ������߱��ⱳ��
		if ( pDC == GetViewData()->GetMainDC() && bMouseMove)
		{
			// ����MainDc ���������.
				
			CString StrcCurveTitle = pDependCurve->GetTitle();
			CMemDCEx * pMainDC = GetViewData()->GetMainDC();						
			
			CSize size = pMainDC->GetTextExtent(StrcCurveTitle);

			CRect Rect1 = rect;

			Rect1.right = m_RectView.right;			
			Rect1.bottom = Rect1.top + size.cy;

			if (!GetParentIoViewData().pChartRegionData->BeShowTopToolBar())
			{
				if ( pParent->m_pRegionMain == this )
				{
					Rect1.left += size.cx;
				}
			}
			//if ( pParent->m_pRegionMain == this )
			//{
			//	Rect1.left += size.cx;
			//}
						
			IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
			if (NULL != pIoViewParent)
			{
				pMainDC->FillSolidRect(Rect1, pIoViewParent->GetIoViewColor(ESCBackground));
			}
			
		}
		else
		{
			pDC = pDC;
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ʱ�������Ҫ�ǵ�ͣ���ƣ������������
		if ( NULL != pTrend && pParent->IsShowRiseFallMax() )
		{
			float fRise, fFall;
			CMerch *pTmpMerch = NULL;
			if ( pParent->CalcRiseFallMaxPrice(fRise, fFall) && (pTmpMerch=GetParentIoViewData().pIoViewBase->GetMerchXml()) != NULL )
			{
				// �ǵ�ͣ����
				CString StrFmt;
				StrFmt.Format(_T("%%0.%df"), pTmpMerch->m_MerchInfo.m_iSaveDec);
				CStringArray aStrs;
				aStrs.SetSize(4);
				aStrs[0] = _T("��ͣ:");
				aStrs[1].Format(StrFmt, fRise);
				aStrs[2] = _T("��ͣ:");
				aStrs[3].Format(StrFmt, fFall);
				
				StrTitle = aStrs[0] + aStrs[1] + aStrs[2] + aStrs[3];
				CArray<COLORREF, COLORREF> aClrs;
				aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCText));
				aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise));
				aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCText));
				aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall));
				ASSERT( aClrs.GetSize() == aStrs.GetSize() );
				
				if ( pDC == GetViewData()->GetMainDC() && bMouseMove)
				{
					// ����MainDc ���������.
					CMemDCEx * pMainDC = GetViewData()->GetMainDC();						
					
					CSize size = pMainDC->GetTextExtent(StrTitle);
					
					CRect Rect1 = rect;
					
					Rect1.right = m_RectView.right;			
					Rect1.bottom = Rect1.top + size.cy;
					
					//if ( pParent->m_pRegionMain == this )
					//{
					//	Rect1.left += size.cx;
					//}
					
					IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
					ASSERT( NULL != pIoViewParent );
					pMainDC->FillSolidRect(Rect1, pIoViewParent->GetIoViewColor(ESCBackground));
				}
				else
				{
				}

				// һ���ִ���Ϊ4���� ��ͣ ���� ��ͣ ����
				CFont *pOldFont = NULL;
				if ( NULL != m_FontTitle )
				{
					pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
				}
				for ( int32 i=0; i < aStrs.GetSize() ; i++ )
				{
					pDC->SetTextColor(aClrs[i]);
					DrawTitleText(pDC, pPickDC, pDependCurve, aStrs[i], rect);
				}
				if ( NULL != pOldFont )
				{
					pDC->SelectObject(pOldFont);
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////
		// ��ʱ�������Ҫ���ƾ��ۺ͵�ǰ�ۣ����������
		if ( NULL != pTrend && pParent->IsShowAvgPrice() )
		{
			float favg, fNewPrive;
			CMerch *pTmpMerch = NULL;
			if ((pTmpMerch=GetParentIoViewData().pIoViewBase->GetMerchXml()) != NULL )
			{
				if (pTmpMerch->m_pRealtimePrice)
				{
					CRealtimePrice *pRealtimePrice = pTmpMerch->m_pRealtimePrice;
					favg		= pRealtimePrice->m_fPriceAvg;
					fNewPrive	= pRealtimePrice->m_fPriceNew;

					// �ǵ�ͣ����
					CString StrFmt;
					StrFmt.Format(_T("%%0.%df"), pTmpMerch->m_MerchInfo.m_iSaveDec);
					CStringArray aStrs;
					aStrs.SetSize(6);
					aStrs[0] = _T("    ����:");
					aStrs[1].Format(StrFmt, favg);
					aStrs[2];
					aStrs[3] = _T("    ����:");
					aStrs[4].Format(StrFmt, fNewPrive);
					aStrs[5];

					
					CArray<COLORREF, COLORREF> aClrs;
					aClrs.Add(RGB(247,252,168));
					aClrs.Add(RGB(247,252,168));

					// ������ɫ
					if (pRealtimePrice->m_fPriceNew - pRealtimePrice->m_fPricePrevClose >= 1e-7)
					{
						aStrs[2] = _T("��");
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise));
					}
					else
					{
						aStrs[2] = _T("��");
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall));
					}

					aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCText));
					// ��ǰ����ɫ
					if (pRealtimePrice->m_fPriceNew - pRealtimePrice->m_fPricePrevClose >= 1e-7)
					{
						aStrs[5] = _T("��");
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise));
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise));
					}
					else
					{
						aStrs[5] = _T("��");
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall));
						aClrs.Add(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall));
					}


					StrTitle = aStrs[0] + aStrs[1] + aStrs[2] + aStrs[3] + aStrs[4] + aStrs[5];
					ASSERT( aClrs.GetSize() == aStrs.GetSize() );
					if ( pDC == GetViewData()->GetMainDC() && bMouseMove)
					{
						// ����MainDc ���������.
						CMemDCEx * pMainDC = GetViewData()->GetMainDC();						

						CSize size = pMainDC->GetTextExtent(StrTitle);

						CRect Rect1 = rect;

						Rect1.right = m_RectView.right;			
						Rect1.bottom = Rect1.top + size.cy;

						//if ( pParent->m_pRegionMain == this )
						//{
						//	Rect1.left += size.cx;
						//}

						IIoViewBase * pIoViewParent = GetParentIoViewData().pIoViewBase;
						ASSERT( NULL != pIoViewParent );
						pMainDC->FillSolidRect(Rect1, pIoViewParent->GetIoViewColor(ESCBackground));
					}
					else
					{
					}

					// һ���ִ���Ϊ4���� ��ͣ ���� ��ͣ ����
					CFont *pOldFont = NULL;
					if ( NULL != m_FontTitle )
					{
						pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
					}
					for ( int32 i=0; i < aStrs.GetSize() ; i++ )
					{
						pDC->SetTextColor(aClrs[i]);
						DrawTitleText(pDC, pPickDC, pDependCurve, aStrs[i], rect);
					}
					if ( NULL != pOldFont )
					{
						pDC->SelectObject(pOldFont);
					}
				}	
			}
		}


		//////////////////////////////////////////////////////////////////////////
		
		int32 i, iSize = m_Curves.GetSize();

		for ( i = 0; i < iSize; i ++ )
		{
			CChartCurve* pCurve = m_Curves.GetAt(i);

			if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KInVisible) )
			{
				continue;	// �����߲����Ʊ���
			}
			
			if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeIndex) && CheckFlag(pCurve->m_iFlag,CChartCurve::KIndex1))
			{
				if (!bKLineMain && !bArbKMain)
				{
					// ��ʾ������ť
					if ((2!=pParent->m_iChartType) && (6!=pParent->m_iChartType) && (7!=pParent->m_iChartType))
					{
						DrawIndexBtn(pDC, INDEX_HELP, rect);
					}

					if (1 == pParent->m_iChartType)
					{
						DrawIndexBtn(pDC, INDEX_UPDATE, rect);
					}
				}

				StrTitle = pCurve->GetGroupTitle();
				if ( StrTitle.GetLength() > 0 )
				{
					pDC->SetTextColor(m_clrTitle);
					CFont* pOldFont = NULL;
					if ( NULL != m_FontTitle )
					{
						pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
					}
					bool32 bContinue = DrawTitleText(pDC,pPickDC,pCurve,StrTitle,rect);
					if ( NULL != pOldFont )
					{
						pDC->SelectObject(pOldFont);
					}
					if ( !bContinue ) break;
				}
			}

			if ( CheckFlag(m_iFlag, KDonotShowCurveDataText) )
			{
				continue;	// ����Ҫ��ʾ���ݲ���
			}

			if (!pCurve->BeShowTitleText())
			{
				continue;
			}

		/*	if (GetParentIoViewData().pChartRegionData->BeShowTopToolBar())*/
			{
				if (bKLineMain)
				{
					if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KDependentCurve) )
					{
						continue;
					}
				}
				
// 				
// 				if(NULL != pTrend)
// 				{
// 					if ( L"����" == pCurve->GetTitle() )
// 					{
// 						continue;
// 					}
// 
// 					if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KDependentCurve) )
// 					{
// 						continue;
// 					}
// 				}
			}
		
			//
			StrTitle = pCurve->GetTitle();			

 			if ( StrTitle.GetLength() > 0 )
 			{
 				pDC->SetTextColor(pCurve->m_clrTitleText);

 				CFont* pOldFont = NULL;

 				if ( NULL != m_FontTitle )
 				{
 					pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
 				}
		
				bool32 bContinue = false;

				if ( pCurve == pDependCurve && bMouseMove && pParent->m_pRegionMain == this && !bArbKMain )
				{
					// ����ƶ�ʱ,��Ҫ���� Curve �ı���.����Ӱ.ֻҪ��λ�ÿճ���������
					// ����K������

					StrTitle += _T(" ");
					CSize sz = pDC->GetTextExtent(StrTitle);
					
					if (  rect.left + sz.cx > m_RectView.right )
					{
						bContinue = false;							
					}
					else
					{
						bContinue =  true;
						rect.left += sz.cx;
					}					
				}
				else
				{
					bContinue = DrawTitleText(pDC,pPickDC,pCurve,StrTitle,rect);
					
					pDC->SetTextColor(GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCText));
					
					if ( NULL != pOldFont )
					{
						pDC->SelectObject(pOldFont);
					}
				} 				  				
 			}
		}

		if ( bKLineMain )
		{
			IChartRegionData* pKLine = GetParentIoViewData().pChartRegionData;
			
			if ( NULL == pKLine )
			{
				return;
			}

			if ( pKLine->GetShowClrKLineFlag() )
			{
				CString StrName = pKLine->GetClrKLineName();
				if ( StrName.GetLength() > 0 )
				{
					// ��ͼ��
					CRect rectIcon = rect;
					rectIcon.right = rectIcon.left + 10;
					CRect rectPick = rect;
					

					CBrush br;
					if ( pKLine->BeSysClrKLine() )
					{
						br.CreateSolidBrush(RGB(255, 0, 255));
					}
					else
					{
						br.CreateSolidBrush(RGB(0, 255, 255));
					}
					
					CBrush* pOldBrush = pDC->SelectObject(&br);	

					pDC->BeginPath();
					CPen penIcon(PS_SOLID, 1, RGB(0,0,0));
					CPen* pOldPen = pDC->SelectObject(&penIcon);

					pDC->MoveTo(rectIcon.left + 1, rectIcon.top + 4);
					pDC->LineTo(rectIcon.left + 3, rectIcon.top + 4);
					pDC->LineTo(rectIcon.left + 3, rectIcon.top + 2);
					pDC->LineTo(rectIcon.left + 5, rectIcon.top + 2);
					pDC->LineTo(rectIcon.left + 5, rectIcon.top + 4);
					pDC->LineTo(rectIcon.left + 7, rectIcon.top + 4);
					pDC->LineTo(rectIcon.left + 7, rectIcon.top + 10);
					pDC->LineTo(rectIcon.left + 5, rectIcon.top + 10);					
					pDC->LineTo(rectIcon.left + 5, rectIcon.top + 12);
					pDC->LineTo(rectIcon.left + 3, rectIcon.top + 12);
					pDC->LineTo(rectIcon.left + 3, rectIcon.top + 10);
					pDC->LineTo(rectIcon.left + 1, rectIcon.top + 10);
					pDC->LineTo(rectIcon.left + 1, rectIcon.top + 4);
						
					pDC->EndPath();
					pDC->FillPath();
					
					pDC->SelectObject(pOldBrush);
					br.DeleteObject();


					// д��
					CRect rectText = rect;
					rectText.left  = rectIcon.right + 2;

					CFont* pFont =  GetParentIoViewData().pIoViewBase->GetIoViewFontObject(ESFSmall);
					CFont* pOldFont = pDC->SelectObject(pFont);

					CSize SizeClrKLneName = pDC->GetTextExtent(StrName);
					rectText.right = rectText.left + SizeClrKLneName.cx;

					pDC->DrawText(StrName, rectText, DT_SINGLELINE|DT_LEFT);
					
					rectPick.left  = rect.left;
					rectPick.right = rectText.right;
					pPickDC->_FillSolidRect(&rectPick, 0);

					pDC->SelectObject(pOldFont);
					pDC->SelectObject(pOldPen);
				}
			}
		}
		else if ( NULL != pTrend )
		{
			// ��ʱͼ������ͼ������region�ı���Ҫ����������
			if ( pParent->m_SubRegions.GetSize() >= 1 )
			{
				CFont *pOldFont = NULL;
				bool32 bContinue = true;
				if ( NULL != m_FontTitle )
				{
					pOldFont = (CFont*)pDC->SelectObject(m_FontTitle);
 				}
				// �ɽ�����ͼ 
				CChartCurve *pCurve = NULL;
// 				if ( pParent->m_SubRegions[0].m_pSubRegionMain->GetCurveNum() > 0
// 					&& NULL != (pCurve = pParent->m_SubRegions[0].m_pSubRegionMain->GetCurve(0)) )
// 				{
// 					
// 					if ( CTabSplitWnd::m_pMainFram->SendMessage(UM_IsShowVolBuySellColor)/*CIoViewChart::IsShowVolBuySellColor()*/ )
// 					{
// 						// ����̽�����
// 						CString StrVol;
// 						StrVol = _T(" ��");
// 						pDC->SetTextColor(pCurve->GetChartRegion().GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCRise));
// 						bContinue = DrawTitleText(pDC, pPickDC, pCurve, StrVol, rect, false);
// 						StrVol = _T("��");
// 						pDC->SetTextColor(pCurve->GetChartRegion().GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCFall));
// 						bContinue = DrawTitleText(pDC, pPickDC, pCurve, StrVol, rect, false);
// 						StrVol = _T("�� ");
// 						pDC->SetTextColor(pCurve->GetChartRegion().GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCKeep));
// 						bContinue = DrawTitleText(pDC, pPickDC, pCurve, StrVol, rect, false);
// 					}
// 					else
// 					{
// 						CString StrVol;
// 						StrVol = _T(" �ɽ��� ");
// 						//pDC->SetTextColor(pCurve->m_clrTitleText);
// 						pDC->SetTextColor(pCurve->GetChartRegion().GetParentIoViewData().pIoViewBase->GetIoViewColor(ESCVolume));
// 						bContinue = DrawTitleText(pDC, pPickDC, pCurve, StrVol, rect, false);
// 					}
// 				}

				// ָ�긱ͼ
				if ( bContinue
					&& pParent->m_SubRegions.GetSize() > 1
					&& pParent->m_SubRegions[1].m_pSubRegionMain->GetCurveNum() > 0
					&& NULL != (pCurve=pParent->m_SubRegions[1].m_pSubRegionMain->GetCurve(0)))
				{
					CString StrIndex = pParent->GetSubRegionTitle();
					//pDC->SetTextColor(pCurve->m_clrTitleText);
					IIoViewBase*pParent1 =  pCurve->GetChartRegion().GetParentIoViewData().pIoViewBase;
					pDC->SetTextColor(pParent1->GetIoViewColor(ESCGuideLine1));
					bContinue = DrawTitleText(pDC, pPickDC, pCurve, StrIndex, rect);
				}
				else if ( bContinue )
				{
					CString StrIndex = pParent->GetSubRegionTitle();
					if ( !StrIndex.IsEmpty() )
					{
						IIoViewBase *pParent1 = GetParentIoViewData().pIoViewBase;
						pDC->SetTextColor(pParent1->GetIoViewColor(ESCGuideLine1));
						bContinue = DrawTitleText(pDC, pPickDC, NULL, StrIndex, rect);
					}
				}

				if ( NULL != pOldFont )
				{
					pDC->SelectObject(pOldFont);
				}
			}
		}
	}

	/*if (1 == pParent->m_iChartType)
	{
		for (int32 i = 0; i < pParent->m_SubRegions.GetSize(); i++)
		{
			T_SubRegionParam &SubRegion = pParent->m_SubRegions[i];
			if (this == SubRegion.m_pSubRegionMain)
			{
				if (pParent->m_iPickSubRegionIndex == i)
				{
					// ѡ�и�ͼ��־
					CRect RectSel(m_RectView);
					CBrush brush;
					brush.CreateSolidBrush( RGB( 255, 183, 0 ) );
					CBrush * pOldBrush = pDC->SelectObject( &brush );
					CPen Pen(PS_SOLID, 1, RGB(50,50,50)), *pOldPen;
					pOldPen = pDC->SelectObject(&Pen);

					POINT points[4];
					points[0].x = RectSel.right - 15; 
					points[0].y = RectSel.top; 
					points[1].x = RectSel.right; 
					points[1].y = RectSel.top; 
					points[2].x = RectSel.right;
					points[2].y = RectSel.top + 15; 
					points[3].x = RectSel.right - 15; 
					points[3].y = RectSel.top; 
					pDC->Polygon(points, 4); 

					pDC->SelectObject(pOldPen);			
					pDC->SelectObject( pOldBrush );
				}

				break;
			}
		}
	}*/

	pDC->SetBkMode(oldMode);
}

bool32 CChartRegion::DrawTitleText ( CMemDCEx* pDC,CMemDCEx* pPickDC, CChartCurve* pCurve,CString StrTitle, CRect& rect, bool32 bAppendSpaceSep/*=true*/ )
{
	bool32 bMouseMoveAndMainRegion = false;
	CString StrInstead = L"";
	CChartCurve * pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	if (NULL != pCurveDependent && pCurveDependent == pCurve && pDC == GetViewData()->GetOverlayDC() && this == GetParentIoViewData().pChartRegionData->m_pRegionMain)
	{
		bMouseMoveAndMainRegion = true;		
	}
	if ( bAppendSpaceSep )
	{
		StrTitle += _T(" ");
	}
	CSize sz = pDC->GetTextExtent(StrTitle);
	if ( rect.left + sz.cx > m_RectView.right )
	{
		return false;
	}

	if (bMouseMoveAndMainRegion)
	{
		pDC->DrawText(StrInstead,rect,DT_SINGLELINE|DT_LEFT );
	}
	else
	{
		pDC->DrawText(StrTitle,rect,DT_SINGLELINE|DT_LEFT );
	}
	
	if ( NULL != pCurve && NULL != pPickDC )
	{
		CRect RectPick = rect;
		RectPick.right = RectPick.left + sz.cx;
		RectPick.bottom = RectPick.top + sz.cy;
		pPickDC->BeginDraw(pCurve,1);
		pPickDC->_FillSolidRect(&RectPick,0);
		pPickDC->EndDraw();
	}
	rect.left += sz.cx;

	return true;
}

void CChartRegion::DrawAxisSlider ( CMemDCEx* pDC,const CPoint pt )
{
	CRect rect;

	CChartCurve* pDependentCurve = GetDependentCurve();
	
	if ( NULL == pDependentCurve )
	{
		CChartRegion* pDataSourceRegion = GetDataSourceRegion();
		if ( NULL != pDataSourceRegion)
		{
			pDependentCurve = pDataSourceRegion->GetDependentCurve();
		}				
	}
	
	//���������x��,�����x�Ử��
	if ( CheckFlag(m_iFlag,CChartRegion::KXAxis ))
	{
		//GetViewData()->GetView()->GetClientRect(&rect);
		GetViewData()->GetViewRegionRect(&rect);
		rect.left = m_RectView.left;
		rect.right = m_RectView.right;
		
		if ( rect.PtInRect(pt) && (!m_RectView.PtInRect(pt)))
		{
			//
			bool32 bValidText = false;
			int32 id;
			int32 irx = pt.x;
			ClientXToRegion(irx);
			
			if ( NULL == pDependentCurve )
			{
				// û�����ݵ�ʱ��,ֻ�����յľ��γ���������
				rect.top   = m_RectView.top;
				rect.left  = pt.x;
				rect.right = rect.left + m_iAxisTextMaxWidth * 2 + 2;
				DrawAxisSliderRect(pDC,rect);
				return;
			}
			
			CSize size;
			
			if ( pDependentCurve->RegionXToCurvePos(irx,id) && id >= 0 && id < m_aXAxisNodes.GetSize())
			{
				bValidText = true;
				CAxisNode *pAxisNodes = (CAxisNode *)m_aXAxisNodes.GetData();
				//CString StrText = pAxisNodes[id].m_StrSliderText1;
				CString StrText;
				GetParentIoViewData().pChartRegionData->GetChartXAxisSliderText1(StrText, pAxisNodes[id]);
				size = pDC->GetTextExtent(StrText);	
				
				if ( StrText.GetLength() <= 0 || size.cx <= 0 )
				{
					return;
				}
			}
			
			if ( !bValidText )
			{
				return;	// û���ִ��򲻻���
			}
			
			//
			rect = m_RectView;
			rect.left = pt.x;
			
			if (!bValidText)
			{
				rect.right = rect.left + m_iAxisTextMaxWidth * 2 + 2;
			}
			else
			{
				rect.right = rect.left + size.cx;
			}
			
			
			if( rect.right > m_RectView.right )
			{
				rect.right = pt.x;
				rect.left = rect.right - size.cx;
			}
			
			DrawAxisSliderRect(pDC,rect);
			rect.left++;
			rect.right--;
			rect.top++;
			rect.bottom--;
			OnDrawAxisSliderText(pDC,rect,pt);
		}		
	}
	//���������y��,�����y�Ử��
	if ( CheckFlag(m_iFlag,CChartRegion::KYAxis ))
	{		
		//GetViewData()->GetView()->GetClientRect(&rect);
		GetViewData()->GetViewRegionRect(&rect);
		
		rect.top = m_RectView.top;
		rect.bottom = m_RectView.bottom;

		if ( m_RectView.Width() <= 0)
		{
			return;
		}

		rect.PtInRect(pt);
		m_RectView.PtInRect(pt);

		if ( rect.PtInRect(pt) && (!m_RectView.PtInRect(pt)) )
		{
			rect = m_RectView;
			rect.top = pt.y - m_iAxisTextMaxHeight;
			rect.bottom = rect.top + m_iAxisTextMaxHeight;
			// ����ʾ����������RectView��
			// �����˾�����ʾ��Ծ�����������ڷ�MouseMove����ʱ������ʾ���
			if ( rect.top < m_RectView.top )
			{
// 				rect.top = m_RectView.top;
// 				rect.bottom = rect.top + m_iAxisTextMaxHeight;
			}
			if ( rect.bottom > m_RectView.bottom )
			{
// 				rect.bottom = m_RectView.bottom;
// 				rect.top = rect.bottom - m_iAxisTextMaxHeight;
			}
			if ( rect.top >= 0 )
			//if ( rect.top >= m_RectView.top )
			{
				//rect.bottom = pt.y;		
				
				rect.top  -= 1;
				rect.left += 0;
				rect.right-= 0;

				// Y���Ӧ��û�����ݵ�ʱ����Ҫ�� xl 0617
				CChartRegion* pDataSourceRegion = GetDataSourceRegion();
				
				if ( NULL == pDataSourceRegion)
				{
					return;
				}
				
				CChartCurve* pCurveDependent = pDataSourceRegion->GetDependentCurve();
				
				if (NULL == pCurveDependent)
				{
					return;
				}
				
				int32 ry = pt.y;
				ClientYToRegion(ry);
				float fy;
				if ( pCurveDependent->RegionYToPriceY(ry, fy) )
				{
					// ��rect�����DC�пճ�������֤���ǿɻ��Ƶ�
					DrawAxisSliderRect(pDC,rect);
					rect.DeflateRect(1, 1);
					OnDrawAxisSliderText(pDC,rect,pt);
				}
			}
		}
	}
}

bool32 CChartRegion::bNoData()
{
	// fangz 1201# �ж��Ƿ������ݵĹ���.
	
/*	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent)
	{
		return true;
	}
	CNodeSequence * pNodes = (CNodeSequence *)pCurveDependent->GetNodes();

	if ( NULL == pNodes)
	{
		return true;
	}

	T_MerchNodeUserData * pNodeData = (T_MerchNodeUserData *)pNodes->GetUserData();
	
	if ( NULL == pNodeData)
	{
		return true;
	}

	if ( pNodeData->pKLineNodesFull->GetSize() < 2)
	{	
		return true;
	}
	*/
	return false;
}

void CChartRegion::DrawVLine(CMemDCEx* pDC, int32 x)
{
	CChartCurve* pCurveDependent = GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	CPoint pt1, pt2;
	COLORREF clr = m_clrCross;

	pt1.x = x;
	pt1.y = m_RectView.top + GetTopSkip();
	pt2.x = x;
	pt2.y = m_RectView.bottom;

	pDC->_DrawDotLine(pt1, pt2, 3, clr);
}

void CChartRegion::DrawCross(CMemDCEx* pDC, int32 x, int32 y, E_DrawCross eDrawCross)
{	
	if ( EDCUpDownKey ==  eDrawCross)
	{
		DrawHVLine(pDC,x,false);
		DrawHVLine(pDC,y,true);
		CChartCurve* pCurveDependent = GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		int32 rx = x;
		ClientXToRegion(rx);
		pCurveDependent->RegionXToCurvePos(rx, m_iNodeCross);
		return;
	}

	if (m_bSplitWndDrag)
	{
		return;
	}

	// �����ǰ����㳬����view wnd�����򣬲�����
	if ( EDCMouseMove == eDrawCross )
	{
		CPoint pt(x, y);
		CRect rcView;
		if ( GetViewParam()->GetViewRegionRect(&rcView) && !(rcView.InflateRect(-1, -1), rcView.PtInRect(pt)) )			
		{
			return;
		}
	}

	CChartCurve* pCurveDependent = GetDependentCurve();
	
	if ( NULL == pCurveDependent)
	{
		//����ֱ��
		DrawHVLine(pDC, x, false);

		// ����ˮƽ�ߣ�ֻ�ǲ�����Ӧֵ��
		if ( y <= m_RectCurves.bottom && y >= m_RectCurves.top  )
		{
			DrawHVLine(pDC,y);
		}

		return;
	}

	//����ֱ��
	DrawHVLine(pDC, x, false);

	if ( x < 0 )
	{		
		// �����Ƿ�
		pCurveDependent->CurvePosToRegionX(m_iNodeCross, x);
		RegionXToClient(x);
	}
	
	bool32 bRequestCurve = CheckFlag(pCurveDependent->m_iFlag,CChartCurve::KRequestCurve);

	int32 i, iSize, iPos;
	float fValue,fPricePrevClose;
	int32 iRegionX = x;
	
	//Client(OnMouseMove)��x-->Region��x
	ClientXToRegion(iRegionX);

	//Region��x-->region��iPos(m_ChartCurve��ÿһ��Curve��iPos��m_iID����һһ��Ӧ/��ͬ��)
	if ( pCurveDependent->RegionXToCurvePos(iRegionX, iPos) )
	{
		//
		iSize = m_Curves.GetSize();
		for ( i = 0; i < iSize; i ++ )
		{
			CChartCurve* pCurve = m_Curves.GetAt(i);
			if ( NULL != pCurve && pCurve == pCurveDependent)			
			{
				if ( EDCLeftRightKey == eDrawCross )
				{
					//��ˮƽ��
					if ( y <= m_RectCurves.bottom && y >= m_RectCurves.top  )
					{
						DrawHVLine(pDC,y);
					}
					
					//x��iPos-->Curve��y(NodeData��m_fClose)
					if ( pCurve->NodePosValue(iPos,fValue) )
					{
						//Curve��y-->Region��y
						if ( pCurveDependent->PriceYToRegionY(fValue,y) )					
						{
							//Region��y-->Client��y
							RegionYToClient(y);

							if ( bRequestCurve && y <= m_RectCurves.bottom)
							{
								//��ˮƽ��
								DrawHVLine(pDC,y);
							}

							CPoint ptscreen(x,y);
							GetViewParam()->GetView()->ClientToScreen(&ptscreen);
							AddFlag(m_iFlag,CChartRegion::KLockMouseMove);
							
							if ( bRequestCurve) 
							{
								::SetCursorPos(ptscreen.x,ptscreen.y);
							}

							RemoveFlag(m_iFlag,CChartRegion::KLockMouseMove);							
						}
					}					
				}
				else
				{
					if ( /*bRequestCurve && */ y <= m_RectCurves.bottom)
					{
						DrawHVLine(pDC,y);//��ˮƽ��
					}
				}

				ClientYToRegion(y);
				if ( !pCurve->RegionYToPriceY(y,fValue) )
				{
					fValue = 0.0f;
				}

				if ( iPos >= pCurveDependent->GetNodes()->GetSize() )
				{
					iPos = pCurveDependent->GetNodes()->GetSize() - 1;
				}

				if ( !pCurve->NodePosValue(iPos-1,fPricePrevClose) )
				{
					fPricePrevClose = 0.0f;
				} 

				////////////////////////////////////////////////////////////////////
				CNodeData NodeData;
				pCurveDependent->GetNodes()->GetAt(iPos, NodeData);

				// ������ƶ����ҵ�����Ϣ������ʾ��ʮ�ֹ�껹�������� 0001903 - ȫˢ��
				if ( GetViewData()->GetView()->GetSafeHwnd() == GetFocus() && !GetParentIoViewData().pChartRegionData->GetCrossLockInfo() )
				{					
					GetViewData()->OnCrossData(this, m_iNodeCross, NodeData, fPricePrevClose, fValue, true);
				}
			}
		}
	}
	else if ( EDCLeftRightKey != eDrawCross
		&& m_iNodeCross >= 0 && m_iNodeCross < pCurveDependent->GetNodes()->GetSize() )
	{
		// û��x����Ӧ�� ˮƽ����ô����,
		// ������ˮƽ�ߣ���֪ͨ��ͼ�������ݣ�
		// ������Ч����һ��nodeCrossֵ��
		iSize = m_Curves.GetSize();
		iPos = m_iNodeCross;
		for ( i = 0; i < iSize; i ++ )
		{
			CChartCurve* pCurve = m_Curves.GetAt(i);
			if ( NULL != pCurve && pCurve == pCurveDependent)			
			{
				if ( /*bRequestCurve && */ y <= m_RectCurves.bottom)
				{
					DrawHVLine(pDC,y);//��ˮƽ��
				}
				
				ClientYToRegion(y);
				if ( !pCurve->RegionYToPriceY(y,fValue) )
				{
					fValue = 0.0f;
				}
				
				if ( iPos >= pCurveDependent->GetNodes()->GetSize() )
				{
					iPos = pCurveDependent->GetNodes()->GetSize() - 1;
				}
				
				if ( !pCurve->NodePosValue(iPos-1,fPricePrevClose) )
				{
					fPricePrevClose = 0.0f;
				} 
				
				////////////////////////////////////////////////////////////////////
				CNodeData NodeData;
				pCurveDependent->GetNodes()->GetAt(iPos, NodeData);
				
				// ������ƶ����ҵ�����Ϣ������ʾ��ʮ�ֹ�껹�������� 0001903 - ȫˢ��
				if ( GetViewData()->GetView()->GetSafeHwnd() == GetFocus() )
				{					
					GetViewData()->OnCrossData(this,m_iNodeCross,NodeData,fPricePrevClose,fValue,true);
				}
			}
		}
	}
	else if ( y <= m_RectCurves.bottom && y >= m_RectCurves.top  )
	{
		// ����һ���򵥵�ˮƽ��
		DrawHVLine(pDC,y);
	}

	// �����û������:
  	if ( bNoData())
  	{
  		GetViewData()->OnCrossNoData(m_StrAxisTextsOfNoData);		
  		DrawHVLine(pDC,y);
  		DrawHVLine(pDC,x,false);//����ֱ��				
 	}
}

void CChartRegion::DrawCross( CMemDCEx* pDC,int32 iXNodePos, E_DrawCross eDrawCross )
{
	// ���ݸ�nodepos�ҵ���Ӧλ�ã��ڻ���
	int32 x,y;
	float fValue;
	// û����Ч���ݵĲ���ͨ�����������cross
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;	// ������curve
	}
	if ( !pCurveDependent->CurvePosToRegionX(iXNodePos, x) )
	{
		return;
	}
	RegionXToClient(x);
	if ( !pCurveDependent->NodePosValue(iXNodePos, fValue) )
	{
		return;
	}
	if ( !pCurveDependent->PriceYToRegionY(fValue,y) )
	{
		return;
	}
	RegionYToClient(y);

	if ( EDCUpDownKey ==  eDrawCross)
	{
		DrawHVLine(pDC,x,false);
		DrawHVLine(pDC,y,true);
		
		return;
	}

	if (m_bSplitWndDrag)
	{
		return;
	}

	// �����ǰ����㳬����view wnd�����򣬲�����
	if ( EDCMouseMove == eDrawCross )
	{
		CPoint pt(x, y);
		CRect rcView;
		if ( GetViewParam()->GetViewRegionRect(&rcView)
			&& !(rcView.InflateRect(-1, -1), rcView.PtInRect(pt)) )
		{
			return;
		}
	}
	
	//����ֱ��
	DrawHVLine(pDC,x,false);

	if ( NULL == pCurveDependent)
	{
		// ����ˮƽ�ߣ�ֻ�ǲ�����Ӧֵ��
		if ( y <= m_RectCurves.bottom && y >= m_RectCurves.top  )
		{
			DrawHVLine(pDC,y);
		}
		return;
	}
	else
	{
		DrawHVLine(pDC,y);
	}
	
	bool32 bRequestCurve = CheckFlag(pCurveDependent->m_iFlag,CChartCurve::KRequestCurve);

	int32 i, iSize;
	float fPricePrevClose;
	int32 iRegionX = x;
	
	//Client(OnMouseMove)��x-->Region��x
	ClientXToRegion(iRegionX);

	//Region��x-->region��iPos(m_ChartCurve��ÿһ��Curve��iPos��m_iID����һһ��Ӧ/��ͬ��)
	iSize = m_Curves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = m_Curves.GetAt(i);
		if ( NULL != pCurve && pCurve == pCurveDependent)			
		{
			if ( EDCLeftRightKey == eDrawCross )
			{	
				//x��iPos-->Curve��y(NodeData��m_fClose)
				//Curve��y-->Region��y
				CPoint ptscreen(x,y);
				GetViewParam()->GetView()->ClientToScreen(&ptscreen);
				AddFlag(m_iFlag,CChartRegion::KLockMouseMove);
				
				if ( bRequestCurve) 
				{
					::SetCursorPos(ptscreen.x,ptscreen.y);
				}
				
				RemoveFlag(m_iFlag,CChartRegion::KLockMouseMove);							
			}
			
			if ( !pCurve->NodePosValue(iXNodePos-1,fPricePrevClose) )
			{
				fPricePrevClose = 0.0f;
			} 
			
			////////////////////////////////////////////////////////////////////
			CNodeData NodeData;
			pCurveDependent->GetNodes()->GetAt(iXNodePos, NodeData);
			
			// ������ƶ����ҵ�����Ϣ������ʾ��ʮ�ֹ�껹�������� 0001903 - ȫˢ��
			if ( GetViewData()->GetView()->GetSafeHwnd() == GetFocus() && !GetParentIoViewData().pChartRegionData->GetCrossLockInfo() )
			{					
				GetViewData()->OnCrossData(this, iXNodePos, NodeData, fPricePrevClose, fValue, true);
			}
		}
	}

}

void CChartRegion::DrawRectZoomOut(CMemDCEx* pDC)
{
	if ( NULL == pDC || !CheckFlag(m_iFlag, CChartRegion::KRectZoomOut) )
	{
		return;
	}

	// ���Ŵ����:			
	
	CPoint ptPress	= GetViewData()->m_PointPress;
	CPoint ptNow	= GetViewData()->m_PointRectZoomOutEnd;
	
	CRect  RectDraw;
	
	if ( ptPress.x == ptNow.x || ptPress.y == ptNow.y )
	{
		return;
	}
	else if ( ptPress.x < ptNow.x )
	{
		RectDraw = CRect(ptPress, ptNow);
	}
	else 
	{
		RectDraw = CRect(ptNow, ptPress);
	}

	pDC->_DrawRect(RectDraw, RGB(110,110,110)/*GetParentIoView()->GetIoViewColor(ESCChartAxisLine)*/);			
}

void CChartRegion::DrawHVLine ( CMemDCEx* pDC,int32 iVal, bool32 bHorizontal)
{
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clrCross);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1,pt2;
	//ˮƽ
	if ( bHorizontal )
	{
		// Ҫ��Ҫ��skip���򲻻��ƣ�
		pt1.x = m_RectView.left;
		pt1.y = iVal;
		pt2.x = m_RectView.right;
		pt2.y = iVal;
		pDC->_DrawLine(pt1,pt2);			

	}
	//��ֱ
	else
	{
		pt1.x = iVal;
		pt1.y = m_RectView.top + GetTopSkip();
		// ��ʱͼ�ĳɽ�����ͼ�����⴦��: ֱ�Ӵ���TopSkip��������
		IChartRegionData *pParent = GetParentIoViewData().pChartRegionData;
		CIoViewChart *pTmpChart = NULL;
		if(GetParentIoViewData().pIoViewBase->IsKindOfIoViewChart())
			pTmpChart = (CIoViewChart*)pParent;

		if ( NULL != pTmpChart 
			&& pParent->m_iChartType == 0
			&& pParent->m_SubRegions.GetSize() > 0
			&& pParent->m_SubRegions[0].m_pSubRegionMain == this
			&& GetTopSkip() < 6)
		{
			pt1.y = m_RectView.top;
		}

		pt2.x = iVal;
		pt2.y = m_RectView.bottom;

		pDC->_DrawLine(pt1,pt2);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CChartRegion::DrawAxisSliderRect ( CMemDCEx* pDC,CRect rect )
{
	if ( pDC != GetViewData()->GetOverlayDC())
	{
		;
	}

	pDC->_FillSolidRect(&rect,m_clrSliderFill);
	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clrSliderBorder);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	int32 oldMode = pDC->GetBkMode();
	
	pDC->SetBkMode(TRANSPARENT);
	pDC->_DrawRect(rect);
	pDC->SetBkMode(OPAQUE);

	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}
//////////////////////////////////////////////////////////////////////////
IChartBrige CChartRegion::GetParentIoViewData()
{
	//  ����IoView ��ָ��	
	return  GetViewData()->GetRegionParentIoView();
}

void CChartRegion::SetGGTongESCFlag()
{		
	CWnd * pIoView = GetParentIoViewData().pWnd;
	if (NULL == pIoView)
	{
		return;
	}
	ASSERT(NULL != pIoView);
	CWnd * pWnd = pIoView->GetParent();
	while (pWnd)
	{
		if (pWnd->SendMessage(UM_ISKINDOFCGGTongView))
		{
			//pGGTongView->SetActiveCrossFlag(m_bActiveCross);
			pWnd->SendMessage(UM_SetActiveCrossFlag,(WPARAM)m_bActiveCross);
			break;
		}
		pWnd = pWnd->GetParent();
	}
}

CString CChartRegion::GetSelfDrawXmlString(int32 iIndex)
{
	CString StrXml = L"";

	//
	StrXml.Format(L"<%s %s = \"%d\" %s = \"%d\" >", (LPCTSTR)CString(KIoViewChartSelfDrawRgn), (LPCTSTR)CString(KIoViewChartSelfDrawRgnID), iIndex, (LPCTSTR)CString(KIoViewChartSelfDrawNum), m_SelfDrawingCurves.GetSize());
	for (int32 i = 0; i < m_SelfDrawingCurves.GetSize(); i++)
	{
		CSelfDrawCurve* pCurve = m_SelfDrawingCurves.GetAt(i);
		if (NULL == pCurve || NULL == pCurve->GetNode())
		{
			continue;
		}

		//
		((CSelfDrawNode*)pCurve->GetNode())->_ToXml();
		StrXml += pCurve->GetNode()->m_StrXml;
	}

	StrXml += L"</";
	StrXml += CString(KIoViewChartSelfDrawRgn);
	StrXml += L">";

	//
	return StrXml;
}

void CChartRegion::ClipSelfDrawCursor()
{
	IChartRegionData* pChart = GetParentIoViewData().pChartRegionData;
	if (NULL == pChart)
	{
		return;
	}
	
	CChartCurve* pCurveDependent = GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	if ( m_Curves.GetSize() < 1 )
	{
		return;
	}

	// �޶���귶Χ
	CRect rtCurves(m_RectCurves);
	rtCurves.right -= 2;
	GetParentIoViewData().pWnd->ClientToScreen(&rtCurves);	
	
	//
	CNodeSequence* pNodes = pCurveDependent->GetNodes();
	if (NULL != pNodes && pNodes->GetSize() > 0)
	{
		int32 iSize = pNodes->GetSize();
		CNodeData NodeLast;
		pNodes->GetAt(iSize-1, NodeLast);
		int32 iX,iY;
		
		SDToRegion(NodeLast.m_iID, 0, iX, iY);
		RegionXToClient(iX);
		CPoint pt(iX, iY);
		GetParentIoViewData().pWnd->ClientToScreen(&pt);
		
		//
		rtCurves.right = pt.x;
	}
	
 	ClipCursor(&rtCurves);
}

void CChartRegion::InitIndexBtn()
{
	CRect rcBtn(0, 0, 0, 0);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_KLINE_SET, L"PNG", m_pImgSet);

	AddIndexBtn(&rcBtn, m_pImgSet, 3, INDEX_UPDATE, _T(""));
	AddIndexBtn(&rcBtn, NULL, 3, INDEX_HELP, L"?");
}

void CChartRegion::AddIndexBtn(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, GetViewData()->GetView(), pImage, nCount, nID);

	if (INDEX_HELP == nID)
	{
		btnControl.SetTextBkgColor(RGB(58,58,58), RGB(176,0,0), RGB(176,0,0));
		btnControl.SetTextFrameColor(RGB(58,58,58), RGB(176,0,0), RGB(176,0,0));
	}

	m_mapIndexBtn[nID] = btnControl;
}

void CChartRegion::DrawIndexBtn(CMemDCEx* pDC, int32 ID, CRect &rect)
{
	if (NULL == pDC)
	{
		if ((INDEX_HELP<=ID) && (INDEX_UPDATE>=ID))
		{
			CMemDCEx* pMainDC = GetViewData()->GetMainDC();
			Gdiplus::Graphics graphics(pMainDC->GetSafeHdc());
			m_mapIndexBtn[ID].DrawButton(&graphics);
		}
	}
	else
	{
		if ((INDEX_HELP<=ID) && (INDEX_UPDATE>=ID))
		{
			Gdiplus::Graphics graphics(pDC->GetSafeHdc());
			CRect rtBtn = rect;
			if (INDEX_HELP == ID)
			{
				rtBtn.right = rtBtn.left + 20;
				rtBtn.bottom = rtBtn.top + 15;
			}
			else if (INDEX_UPDATE == ID)
			{
				rtBtn.left = rect.right - 18;
				rtBtn.right = rtBtn.left + 18;
				rtBtn.bottom = rtBtn.top + 18;
			}

			m_mapIndexBtn[ID].SetRect(rtBtn);
			m_mapIndexBtn[ID].DrawButton(&graphics);

			if (INDEX_HELP == ID)
			{
				rect.left += 22;
			}
		}
	}
}

int32 CChartRegion::TIndexButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapIndexBtn.begin(); iter!=m_mapIndexBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

void CChartRegion::DrawRightDragRect( CMemDCEx* pDC )
{
	if ( NULL == pDC /*|| !CheckFlag(GetViewData()->m_iFlag, CRegion::KMousePressRight)*/ )
	{
		return;
	}

	// ���Ŵ����:			

	CPoint ptPress	= GetViewData()->m_PointPress;
	CPoint ptNow	= GetViewData()->m_PointRectZoomOutEnd;

	CRect  RectDraw;

	if ( ptPress.x == ptNow.x || ptPress.y == ptNow.y )
	{
		return;
	}
	else if ( ptPress.x < ptNow.x )
	{
		RectDraw = CRect(ptPress, ptNow);
	}
	else 
	{
		RectDraw = CRect(ptNow, ptPress);
	}

	pDC->_DrawRect(RectDraw, RGB(110,110,110)/*GetParentIoView()->GetIoViewColor(ESCChartAxisLine)*/);		
}

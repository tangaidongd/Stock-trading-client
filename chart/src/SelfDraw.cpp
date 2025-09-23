#include "stdafx.h"
#include "float.h"
#include "tinyxml.h"
#include "ShareFun.h"
#include "PathFactory.h"
#include "Region.h"
#include "ChartRegion.h"
#include "afxtempl.h"
#include "SelfDraw.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "EngineCenterBase.h"
#include "ConstVal.h"
#include "TabSplitWnd.h"
#include "..\res\resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char* KStrSelfDrawGroup		= "region";
const char* KStrSelfDrawGuid		= "guid";

const char* KStrSelfDrawNode		= "curve";
const char* KStrSelfDrawTime		= "time";
const char* KStrSelfDrawType		= "type";
const char* KStrSelfDrawColor		= "color";
const char* KStrSelfDrawId1			= "id1";
const char* KStrSelfDrawId2			= "id2";
const char* KStrSelfDrawIde			= "ide";
const char* KStrSelfDrawVal1		= "val1";
const char* KStrSelfDrawVal2		= "val2";
const char* KStrSelfDrawVale		= "vale";
const char* KStrSelfDrawText		= "text";
const char* KStrSelfDrawMarket		= "market";
const char* KStrSelfDrawCode		= "code";

const int32 KSelfDrawPointSize		= 3;

const COLORREF KSelfDrawDefaultClr  = RGB(255,255,0);
CSelfDrawNode::CSelfDrawNode()
{
	t			= 0;
	m_eType		= ESDTInvalid;	
	m_id1		= -1;
	m_id2		= -1;
	m_fVal1		= FLT_MAX;
	m_fVal2		= FLT_MAX;
	m_clr		= KSelfDrawDefaultClr;
	m_iInternal = -1;
	m_iUserDay  = 0;
	m_iUserMin	= 0;
	m_pMerch    = NULL;
	m_ide		= 0;
	m_fVale		= 0.0f;
}
//lint --e{1540}
CSelfDrawNode::~CSelfDrawNode()
{

}

void CSelfDrawNode::_FromXml( TiXmlElement * pNode)
{
	const char* strValue = NULL;
	if ( 0 == strcmp(pNode->Value(),KStrSelfDrawNode))
	{
		strValue = pNode->Attribute(KStrSelfDrawTime);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			t = atoi(strValue);
		}
		strValue = pNode->Attribute(KStrSelfDrawType);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_eType = (E_SelfDrawType)atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawColor);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_clr = atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawId1);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_id1 = atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawVal1);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_fVal1 = (float) atof ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawId2);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_id2 = atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawVal2);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_fVal2 = (float) atof ( strValue );
		}

		strValue = pNode->Attribute(KStrSelfDrawIde);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_ide = atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrSelfDrawVale);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_fVale = (float)atof ( strValue );
		}

		strValue = pNode->Attribute(KStrSelfDrawText);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_StrText = _A2W(strValue);
		}

		strValue = pNode->Attribute(KIoViewChartKLineInterval);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_iInternal = atoi(strValue);
		}
		
		strValue = pNode->Attribute(KIoViewChartUserMinutes);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_iUserMin = atoi(strValue);
		}
		
		strValue = pNode->Attribute(KIoViewChartUserDays);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			m_iUserDay = atoi(strValue);
		}

		int32 iMarket = -1;
		CString StrCode = L"";

		//
		strValue = pNode->Attribute(KStrSelfDrawMarket);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			iMarket = atoi(strValue);
		}

		//
		strValue = pNode->Attribute(KStrSelfDrawCode);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			StrCode = _A2W(strValue);
		}
		CAbsCenterManager* pCenterManager = (CAbsCenterManager*)CTabSplitWnd::m_pMainFram->SendMessage(UM_GetCenterManager);
		if (NULL != pCenterManager)
		{
			pCenterManager->GetMerchManager().FindMerch(StrCode, iMarket, m_pMerch);
		}	
	}
}

void CSelfDrawNode::_ToXml()
{
	if (NULL == m_pMerch)
	{
		return;		
	}

	//
	m_StrXml.Format(L"<%s %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%f\" %s=\"%d\" %s=\"%f\" %s=\"%d\" %s=\"%f\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" />",
		CString(KStrSelfDrawNode).GetBuffer(),
		CString(KStrSelfDrawTime).GetBuffer(), t,
		CString(KStrSelfDrawType).GetBuffer(), m_eType,
		CString(KStrSelfDrawColor).GetBuffer(), m_clr,
		CString(KStrSelfDrawId1).GetBuffer(), m_id1,
		CString(KStrSelfDrawVal1).GetBuffer(), m_fVal1,
		CString(KStrSelfDrawId2).GetBuffer(), m_id2,
		CString(KStrSelfDrawVal2).GetBuffer(), m_fVal2,
		CString(KStrSelfDrawIde).GetBuffer(), m_ide,
		CString(KStrSelfDrawVale).GetBuffer(), m_fVale,
		CString(KStrSelfDrawText).GetBuffer(), m_StrText.GetBuffer(),
		CString(KIoViewChartKLineInterval).GetBuffer(),m_iInternal,
		CString(KIoViewChartUserMinutes).GetBuffer(),m_iUserMin,
		CString(KIoViewChartUserDays).GetBuffer(),m_iUserDay,
		CString(KStrSelfDrawMarket).GetBuffer(),m_pMerch->m_MerchInfo.m_iMarketId,
		CString(KStrSelfDrawCode).GetBuffer(),m_pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer()
		);
}

///////////////////////////////////////////////////////////////////////////
CSelfDrawCurve::CSelfDrawCurve(CChartRegion* p)
{
	m_t			= 0;
	m_hit		= 0;
	m_hitx		= 0;
	m_hity		= 0;
	m_bSelfDraw = true;
	m_pNode = NULL;	
	assert(p);
	m_pParentRegion = p;
}
CSelfDrawCurve::~CSelfDrawCurve()
{
	DEL(m_pNode);
}
void CSelfDrawCurve::SetT ( int32 t )
{
	m_t = t;
}
void CaclTwoPoint ( CRect rect,CPoint p1,CPoint p2,CPoint* pts)
{
	int32 x1 = p1.x;
	int32 y1 = p1.y;
	int32 x2 = p2.x;
	int32 y2 = p2.y;

	int32 xsub = x2-x1;
	int32 ysub = y2-y1;

	pts[0] = p1;
	pts[1] = p2;

	int32 iNum = 0;
	//垂直
	if ( 0 == xsub )
	{
		p1.y = rect.top;
		p2.y = rect.bottom;
		pts[iNum++] = p1;
		pts[iNum++] = p2;
	}
	//水平
	else if ( 0 == ysub )
	{
		p1.x = rect.left;
		p2.x = rect.right;
		pts[iNum++] = p1;
		pts[iNum++] = p2;
	}
	else
	{
		float k = (float)ysub;
		k /= xsub;
		float b = y1 - k * x1;

		//左边
		int32 left_x,left_y;
		left_x = rect.left+1;
		left_y =(int32)( k * left_x + b);
		if ( rect.PtInRect(CPoint(left_x,left_y)))
		{
			pts[iNum++] = CPoint(left_x,left_y);
		}

		//右边
		int32 right_x,right_y;
		right_x = rect.right-1;
		right_y = (int32)(k * right_x + b);
		if ( rect.PtInRect(CPoint(right_x,right_y)))
		{
			pts[iNum++] = CPoint(right_x,right_y);
		}

		//上边
		int32 top_x,top_y;
		top_y = rect.top+1;
		top_x = (int32)(( top_y - b ) / k);
		if ( rect.PtInRect(CPoint(top_x,top_y)))
		{
			pts[iNum++] = CPoint(top_x,top_y);
		}

		//下边
		int32 bottom_x,bottom_y;
		bottom_y = rect.bottom-1;
		bottom_x = (int32)(( bottom_y - b ) / k);
		if ( rect.PtInRect(CPoint(bottom_x,bottom_y)))
		{
			pts[iNum++] = CPoint(bottom_x,bottom_y);
		}
	}
}

void CSelfDrawCurve::Draw ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC)
{
	if ( NULL == m_pNode)
	{
		return;//可能在其他chart中,被用户删除了?
	}
	//
	m_pNode->Draw(pRegion,this,pDC,pPickDC);

}
int32 CSelfDrawCurve::HitTest ( CChartRegion* pRegion, int32 x,int32 y )
{
	if ( NULL == m_pNode )
	{
		return -1;
	}
	
	//
	m_pNode->HitTest(pRegion,x,y,m_hit);
	m_hitx = x;
	m_hity = y;
	return m_hit;
}

bool32 CSelfDrawCurve::NeedShow()
{
	if (NULL == m_pNode || NULL == m_pParentRegion)
	{
		////ASSERT(0);
		return false;
	}
	
	//
	IIoViewBase* pIoView = m_pParentRegion->GetParentIoViewData().pIoViewBase;
	if (NULL == pIoView)
	{
		////ASSERT(0);
		return false;
	}
	
	if (NULL == m_pNode->m_pMerch)
	{
		return false;
	}
	
	if (m_pNode->m_pMerch != pIoView->GetMerchXml())
	{
		return false;
	}
	
	//
	if(m_pParentRegion->GetParentIoViewData().pChartRegionData->m_MerchParamArray.GetSize() <= 0)
	{
		////ASSERT(0);
		return false;
	}
	
	//
	T_MerchNodeUserData* pData = m_pParentRegion->GetParentIoViewData().pChartRegionData->m_MerchParamArray.GetAt(0);
	if (m_pNode->m_iInternal != (int32)pData->m_eTimeIntervalFull)
	{
		return false;
	}
	
	if(ENTIMinuteUser == pData->m_eTimeIntervalFull && m_pNode->m_iUserMin != pData->m_iTimeUserMultipleMinutes)
	{
		return false;
	}
	
	if(ENTIDayUser == pData->m_eTimeIntervalFull && m_pNode->m_iUserDay != pData->m_iTimeUserMultipleDays)
	{
		return false;
	}
	
	return true;	
}
void CSelfDrawCurve::Moving ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y )
{
	if ( NULL == m_pNode )
	{
		return;
	}
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch ( m_hit )
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
	
	m_pNode->Moving(pRegion,this,pDC,pPickDC,x,y,m_hit,m_hitx,m_hity);
}
void CSelfDrawCurve::Move ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y )
{
	if ( NULL == m_pNode )
	{
		return;
	}

	m_pNode->Move(pRegion,this,pDC,pPickDC,x,y,m_hit,m_hitx,m_hity);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSelfDrawNode::HitTest ( CChartRegion* pRegion, int32 x,int32 y, int32& hit )
{
	if ( m_eType == ESDTZbj ||
		 m_eType == ESDTDbj ||
		 m_eType == ESDTFblq ||
		 m_eType == ESDTText )
	{
		hit = 2;
		return;
	}

	hit = 3;
	int32 x1,y1,x2,y2,xe,ye;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}
	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	if ( Rect1.PtInRect(CPoint(x,y)))
	{
		hit = 0;
		return;
	}

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);
	CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
	if ( Rect2.PtInRect(CPoint(x,y)))
	{
		hit = 1;
	}

	pRegion->SDToRegion(m_ide,m_fVale,xe,ye);
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);
	CRect Rect3(xe-KSelfDrawPointSize,ye-KSelfDrawPointSize,xe+KSelfDrawPointSize+1,ye+KSelfDrawPointSize+1);
	if ( Rect3.PtInRect(CPoint(x,y)))
	{
		hit = 2;
	}
}
void CSelfDrawNode::Moving ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y,int32 hit,int32 hitx,int32 hity )
{
	if(NULL == pRegion)
	{
		return;
	}

	bool32 bInRegion = false;
	int32 i, iSize = pRegion->m_SelfDrawingCurves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( pCurve == pRegion->m_SelfDrawingCurves.GetAt(i))
		{
			bInRegion = true;
			break;
		}
	}
	if ( !bInRegion )
	{
		return;
	}
	CRect rect = pRegion->GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}
	int32 _id1 = m_id1;
	int32 _id2 = m_id2;
	int32 _ide = m_ide;
	float _fVal1 = m_fVal1;
	float _fVal2 = m_fVal2;
	float _fVale = m_fVale;
	//body 平移
	if ( 3 == hit )
	{
		pRegion->MoveSDId(hitx,hity,x,y,_id1,_id2);
		pRegion->MoveSDY (hitx,hity,x,y,_fVal1,_fVal2);
	}
	// end
	if ( 2 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);
		if ( !pRegion->RegionToSD(x,y,_ide,_fVale) )
		{
			return;
		}
	}
	// head
	if ( 0 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);
		if ( !pRegion->RegionToSD(x,y,_id1,_fVal1) )
		{
			return;
		}
	}
	// tail
	if ( 1 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);
		if ( !pRegion->RegionToSD(x,y,_id2,_fVal2) )
		{
			return;
		}
	}

	//转换为屏幕坐标
	int32 x1,y1,x2,y2,xe,ye;
	pRegion->SDToRegion(_id1,_fVal1,x1,y1);
	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(_id2,_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	pRegion->SDToRegion(_ide,_fVale,xe,ye);
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	pRegion->BeginOverlay(false);

	CPen pen,*pOldPen;
	//pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1(x1,y1);
	CPoint pt2(x2,y2);
	CPoint pts[4];
	switch ( m_eType )
	{
	case ESDTLine:
		//pDC->_DrawDotLine(CPoint(x1,y1),CPoint(x2,y2),2,RGB(255,255,0));
		pDC->_DrawLine(CPoint(x1,y1),CPoint(x2,y2));
		break;
	case ESDTLineEx:
		CaclTwoPoint(rect,pt1,pt2,pts);
		pDC->_DrawLine(pts[0],pts[1]);
		break;
	case ESDTJgtd:
		SDDrawJgtd(pRegion,NULL,pDC,NULL,x1,y1,x2,y2,xe,ye);
		break;
	case ESDTPxzx:
		SDDrawPxzx(pRegion,NULL,pDC,NULL,x1,y1,x2,y2,xe,ye);
		break;
	case ESDTYhx:
		SDDrawYhx(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTHjfg:
		SDDrawHjfg(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBfb:
		SDDrawBfb(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTBd:
		SDDrawBd(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJx:
		SDDrawJx(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJxhgd:
		SDDrawJxhgd(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_id1,m_id2);
		break;
	case ESDTYcxxhgd:
		SDDrawYcxxhgd(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_id1,m_id2);
		break;
	case ESDTXxhgd:
		SDDrawXxhgd(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y,m_id1,m_id2);
		break;
	case ESDTZq:
		SDDrawZq(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTFblq:
		SDDrawFblq(pRegion,NULL,pDC,NULL,xe,ye,TRUE);
		break;
	case ESDTZs:
		SDDrawZs(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTJejd:
		SDDrawJejd(pRegion,NULL,pDC,NULL,pt1.x,pt1.y,pt2.x,pt2.y);
		break;
	case ESDTZbj:
		SDDrawBj(pRegion,NULL,pDC,NULL,xe,ye,true);
		break;
	case ESDTDbj:
		SDDrawBj(pRegion,NULL,pDC,NULL,xe,ye,false);
		break;
	case ESDTZ45UP:
		break;
	case ESDTZ45DOWN:
		break;
	case ESDTZHorizontal:
		break;
	case ESDTZVertical:
		break;
	case ESDTText:
		SDDrawText ( pRegion, NULL, pDC,NULL,xe,ye,m_StrText);
		break;
	default:
		break;
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	pRegion->EndOverlay();

}
void CSelfDrawNode::Move ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y,int32 hit,int32 hitx,int32 hity )
{
	bool32 bInRegion = false;
	int32 i, iSize = pRegion->m_SelfDrawingCurves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( pCurve == pRegion->m_SelfDrawingCurves.GetAt(i))
		{
			bInRegion = true;
			break;
		}
	}
	if ( !bInRegion )
	{
		return;
	}
	CRect rect = pRegion->GetRectCurves();
	if ( !PtInRect(&rect,CPoint(x,y)))
	{
		return;
	}

	//body 平移
	if ( 3 == hit )
	{
		pRegion->MoveSDId(hitx,hity,x,y,m_id1,m_id2);
		pRegion->MoveSDY (hitx,hity,x,y,m_fVal1,m_fVal2);
	}
	// end
	if ( 2 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);

		if ( ESDTFblq == m_eType )
		{
			if ( !pRegion->RegionToSD(x,y,m_id1,m_fVal1) )
			{
				return;
			}
		}
		else
		{
			if ( !pRegion->RegionToSD(x,y,m_ide,m_fVale) )
			{
				return;
			}
		}
	}

	// head
	if ( 0 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);
		if ( !pRegion->RegionToSD(x,y,m_id1,m_fVal1) )
		{
			return;
		}
	}
	// tail
	if ( 1 == hit )
	{
		pRegion->ClientXToRegion(x);
		pRegion->ClientYToRegion(y);
		if ( !pRegion->RegionToSD(x,y,m_id2,m_fVal2) )
		{
			return;
		}
	}

	pRegion->SetDrawFlag(CRegion::KDrawCurves|CRegion::KDrawNotify);
	if(NULL != pRegion->GetViewData() && NULL != pRegion->GetViewData()->GetView())
	{
		pRegion->GetViewData()->GetView()->Invalidate();
	}
	
}

void CSelfDrawNode::Draw ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	switch ( m_eType )
	{
	case ESDTLine:
		DrawLine ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTLineEx:
		DrawLineEx ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTJgtd:
		DrawJgtd ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTPxzx:
		DrawPxzx ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTYhx:
		DrawYhx ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTHjfg:
		DrawHjfg ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTBfb:
		DrawBfb ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTBd:
		DrawBd ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTJx:
		DrawJx ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTJxhgd:
		DrawJxhgd ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTYcxxhgd:
		DrawYcxxhgd ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTXxhgd:
		DrawXxhgd(pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTZq:
		DrawZq ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTFblq:
		DrawFblq ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTZs:
		DrawZs ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTJejd:
		DrawJejd ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTZbj:
		DrawZbj ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTDbj:
		DrawDbj ( pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTZ45UP:
		Draw45Degree(pRegion,pCurve,pDC,pPickDC,true);
		break;
	case ESDTZ45DOWN:
		Draw45Degree(pRegion,pCurve,pDC,pPickDC,false);
		break;
	case ESDTZVertical:
		DrawVertical(pRegion,pCurve,pDC,pPickDC); 
		break;
	case ESDTZHorizontal:
		DrawHorizontal(pRegion,pCurve,pDC,pPickDC);
		break;
	case ESDTText:
		DrawText ( pRegion,pCurve,pDC,pPickDC);
		break;
	default:
		break;
	}
}

void CSelfDrawNode::DrawLine( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	pDC->_DrawLine(CPoint(x1,y1),CPoint(x2,y2));
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
	if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
	{
		//pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
		//pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
		pDC->_FillSolidRect(&Rect1,m_clr);
		pDC->_FillSolidRect(&Rect2,m_clr);
	}	
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	pPickDC->BeginDraw(pCurve,2);
	pPickDC->_DrawLine(CPoint(x1,y1),CPoint(x2,y2));
	pPickDC->_FillSolidRect(&Rect1,0);
	pPickDC->_FillSolidRect(&Rect2,0);
	pPickDC->EndDraw();
}	
void CSelfDrawNode::DrawLineEx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1(x1,y1);
	CPoint pt2(x2,y2);
	CRect rect = pRegion->GetRectCurves();
	CPoint pts[4];
	CaclTwoPoint(rect,pt1,pt2,pts);
	pDC->_DrawLine(pts[0],pts[1]);

	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
	if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
	{
		//pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
		//pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
	pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
	pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	pPickDC->BeginDraw(pCurve,2);
	pPickDC->_DrawLine(pts[0],pts[1]);
	pPickDC->_FillSolidRect(&Rect1,0);
	pPickDC->_FillSolidRect(&Rect2,0);
	pPickDC->EndDraw();

}

void CSelfDrawNode::DrawJgtd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2,xe,ye;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	pRegion->SDToRegion(m_ide,m_fVale,xe,ye);
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawJgtd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,xe,ye,m_clr);

}

void CSelfDrawNode::DrawPxzx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2,xe,ye;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	pRegion->SDToRegion(m_ide,m_fVale,xe,ye);
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawPxzx(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,xe,ye,m_clr);

}

	
void CSelfDrawNode::DrawYhx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawYhx(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);
}

	
void CSelfDrawNode::DrawHjfg( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawHjfg(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawBfb( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawBfb(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawBd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawBd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawJx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawJx(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawJxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawJxhgd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_id1,m_id2,m_clr);

}

	
void CSelfDrawNode::DrawYcxxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawYcxxhgd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_id1,m_id2,m_clr);

}

	
void CSelfDrawNode::DrawXxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawXxhgd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_id1,m_id2,m_clr);


}

	
void CSelfDrawNode::DrawZq( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawZq(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawFblq( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 xe,ye;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,xe,ye) )
	{
		return;
	}
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawFblq(pRegion,pCurve,pDC,pPickDC,xe,ye,true,m_clr);
}

	
void CSelfDrawNode::DrawZs( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawZs(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawJejd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	if (NULL == pRegion)
	{
		return;
	}

	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1,x2,y2;
	if ( !pRegion->SDToRegion(m_id1,m_fVal1,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);

	pRegion->SDToRegion(m_id2,m_fVal2,x2,y2);
	pRegion->RegionXToClient(x2);
	pRegion->RegionYToClient(y2);

	SDDrawJejd(pRegion,pCurve,pDC,pPickDC,x1,y1,x2,y2,m_clr);

}

	
void CSelfDrawNode::DrawZbj( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 xe,ye;
	if ( !pRegion->SDToRegion(m_ide,m_fVale,xe,ye) )
	{
		return;
	}
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawBj(pRegion,pCurve,pDC,pPickDC,xe,ye,true,m_clr);
}

	
void CSelfDrawNode::DrawDbj( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 xe,ye;
	if ( !pRegion->SDToRegion(m_ide,m_fVale,xe,ye) )
	{
		return;
	}
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawBj(pRegion,pCurve,pDC,pPickDC,xe,ye,false,m_clr);
}

void CSelfDrawNode::Draw45Degree(CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, bool32 bUp)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1;
	if ( !pRegion->SDToRegion(m_id2,m_fVal2,x1,y1) )
	{
		return;
	}

	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);
	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	
	CPoint pt1(x1,y1);
	CPoint pt2(pt1);
	
	pt2.x++;
	if (bUp)
	{
		pt2.y--;
	}
	else
	{
		pt2.y++;
	}
	
	CRect rect = pRegion->GetRectCurves();
	CPoint pts[4];
	CaclTwoPoint(rect,pt1,pt2,pts);
	pDC->_DrawLine(pts[0],pts[1]);
	
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	
	if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
	{
		//pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
		pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	
	pPickDC->BeginDraw(pCurve,2);
	pPickDC->_DrawLine(pts[0],pts[1]);
	pPickDC->_FillSolidRect(&Rect1,0);
	pPickDC->EndDraw();
}
//2013-5-16 add by cym 水平线

void CSelfDrawNode::DrawHorizontal (CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC)
{
	
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1;
	if ( !pRegion->SDToRegion(m_id2,m_fVal2,x1,y1) )
	{
		return;
	}
	
	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);
	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	
	CPoint pt1(x1,y1);
	CPoint pt2(pt1);
	
	pt2.x--;
	CRect rect = pRegion->GetRectCurves();
	CPoint pts[4];
	CaclTwoPoint(rect,pt1,pt2,pts);
	pDC->_DrawLine(pts[0],pts[1]);
	
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	
	if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
	{
		pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	
	pPickDC->BeginDraw(pCurve,2);
	pPickDC->_DrawLine(pts[0],pts[1]);
	pPickDC->_FillSolidRect(&Rect1,0);
	pPickDC->EndDraw();
	
}

void CSelfDrawNode::DrawVertical(CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 x1,y1;
	if ( !pRegion->SDToRegion(m_id2,m_fVal2,x1,y1) )
	{
		return;
	}
	
	pRegion->RegionXToClient(x1);
	pRegion->RegionYToClient(y1);
	
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,m_clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	
	CPoint pt1(x1,y1);
	CPoint pt2(pt1);
	pt2.y--;
	
	//
	CRect rect = pRegion->GetRectCurves();
	CPoint pts[4];
	CaclTwoPoint(rect,pt1,pt2,pts);
	pDC->_DrawLine(pts[0],pts[1]);
	
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	
	if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
	{
		//pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
		pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	
	pPickDC->BeginDraw(pCurve,2);
	pPickDC->_DrawLine(pts[0],pts[1]);
	pPickDC->_FillSolidRect(&Rect1,0);
	pPickDC->EndDraw();
}
	
void CSelfDrawNode::DrawText( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	int32 xe,ye;
	if ( !pRegion->SDToRegion(m_ide,m_fVale,xe,ye) )
	{
		return;
	}
	pRegion->RegionXToClient(xe);
	pRegion->RegionYToClient(ye);

	SDDrawText ( pRegion, pCurve, pDC,pPickDC,xe,ye,m_StrText,m_clr);
}

	
void CSelfDrawNode::DrawDel( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC )
{
}
////////////////////////////////////////////////////////////////////////////    
void SDDrawYhx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}

	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	int32 w = x2-x1;
	int32 h = y2-y1;
	if ( 0 == w || 0 == h )
	{
		return;
	}

	int32 left,right,top,bottom;
	int32 startx,starty,endx,endy;

	if ( h > 0 )
	{
		startx = x1;
		starty = y1;

		if ( w > 0 )
		{
			endx = x2 + w;
			endy = y1;

			left = x1;
			right = endx;
			top = y1 - h;
			bottom = y2;
		}
		else
		{
			endx = x1;
			endy = y2 + h;

			left = x1 - w;
			right = x2;
			top = endy;
			bottom = y1;

		}
	}
	else
	{
		startx = x1;
		starty = y1;

		if ( w > 0 )
		{
			endx = x1;
			endy = y2 + h;

			left = x1 - w;
			right = x2;
			top = endy;
			bottom = y1;
		}
		else
		{
			endx = x2 + w;
			endy = y1;

			left = x1;
			right = endx;
			top = y1 - h;
			bottom = y2;

		}
	}

	CRect rect = CRect(left,top,right,bottom);
	pDC->Arc(&rect,CPoint(startx,starty),CPoint(endx,endy));

	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC)
	{
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
		}
		pPickDC->BeginDraw(pCurve,2);
		pPickDC->Arc(&rect,CPoint(startx,starty),CPoint(endx,endy));
		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);
		pPickDC->EndDraw();	
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

}
void SDDrawBfb( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	CRect rect = pRegion->GetRectCurves();
 
	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}

	{
		CPoint pt1(rect.left,y1);
		CPoint pt2(rect.right,y1);
		pDC->_DrawLine(pt1,pt2);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt1,pt2);
		}

		CPoint pt3(rect.left,y2);
		CPoint pt4(rect.right,y2);
		pDC->_DrawLine(pt3,pt4);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt3,pt4);
		}
	}
	{
		int32 iSub = (int32)((y2-y1)*0.25f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("25%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.33f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("33%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	}

	{
		int32 iSub = (int32)((y2-y1)*0.500f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("50%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	
}

void SDDrawHjfg( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	
	int32 iID;
	float fValue;
	CString StrText;

	pen.CreatePen(PS_SOLID,1,clr);

	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	CRect rect = pRegion->GetRectCurves();

	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
	CFont* pFontOld = pDC->SelectObject(pFont);

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}

	//
	{
		CPoint pt1(rect.left,y1);
		CPoint pt2(rect.right,y1);
		pDC->_DrawLine(pt1,pt2);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt1,pt2);
		}

		// 0
		int32 yTmp = y1;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
 		CRect RectText = CRect ( rect.left+1,y1-20,rect.right,y1-1);
		StrText.Format(L"00.0%%  [%.2f]", fValue);
		
 		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		// 100
		CPoint pt3(rect.left,y2);
		CPoint pt4(rect.right,y2);
		pDC->_DrawLine(pt3,pt4);
		
		yTmp = y2;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);	
		RectText = CRect ( rect.left+1,y2-20,rect.right,y2-1);
		StrText.Format(L"100.0%% [%.2f]", fValue);
		
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt3,pt4);
		}
	}

	// 23.6%
	{		
		int32 iSub = (int32)((y2-y1)*0.236f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);

		pDC->_DrawLine(pt1,pt2);

		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"23.6%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);

		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.382f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);

		pDC->_DrawLine(pt1,pt2);

		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"38.2%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	}

	{
		int32 iSub = (int32)((y2-y1)*0.500f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawLine(pt1,pt2);

		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"50.0%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.618f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawLine(pt1,pt2);

		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"61.8%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*1.618f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawLine(pt1,pt2);

		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"161.8%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*2.618f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawLine(pt1,pt2);
		
		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"261.8%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*4.236f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawLine(pt1,pt2);
		
		int32 yTmp = pt1.y;
		pRegion->ClientYToRegion(yTmp);
		pRegion->RegionToSD(0, yTmp, iID, fValue);
		StrText.Format(L"423.6%% [%.2f]", fValue);
		CRect RectText = CRect ( rect.left+1,pt1.y-20,rect.right,pt1.y-1);
		pDC->DrawText(StrText,RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	if ( NULL != pCurve && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
			COLORREF clrPick = clr;
			COLORREF clrPick2= 0xFFFFFF - clrPick;
			pDC->_FillSolidRect(&Rect1,clrPick);
			pDC->_FillSolidRect(&Rect2,clrPick2);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pFontOld);
	pen.DeleteObject();

	/*
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);

	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	CRect rect = pRegion->GetRectCurves();

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}

	{
		CPoint pt1(rect.left,y1);
		CPoint pt2(rect.right,y1);
		pDC->_DrawLine(pt1,pt2);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt1,pt2);
		}

		CPoint pt3(rect.left,y2);
		CPoint pt4(rect.right,y2);
		pDC->_DrawLine(pt3,pt4);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt3,pt4);
		}
	}
	{
		int32 iSub = (y2-y1)*0.236;
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("23.6%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (y2-y1)*0.382;
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("38.2%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	}

	{
		int32 iSub = (y2-y1)*0.500;
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("50%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (y2-y1)*0.618;
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);

		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("61.8%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (y2-y1)*1.618;
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);

		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("161.8%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	
	*/
}
void SDDrawBd( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	CRect rect = pRegion->GetRectCurves();

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}

	{
		CPoint pt1(rect.left,y1);
		CPoint pt2(rect.right,y1);
		pDC->_DrawLine(pt1,pt2);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt1,pt2);
		}

		CPoint pt3(rect.left,y2);
		CPoint pt4(rect.right,y2);
		pDC->_DrawLine(pt3,pt4);

		if ( NULL != pCurve && NULL != pPickDC )
		{
			pPickDC->_DrawLine(pt3,pt4);
		}
	}
	{
		int32 iSub = (int32)((y2-y1)*0.125f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("12.5%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.25f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("25%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	}
	{
		int32 iSub = (int32)((y2-y1)*0.375f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("37.5%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.500f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("50%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.625f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);

		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("62.5%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.75f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);

		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("75%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	{
		int32 iSub = (int32)((y2-y1)*0.875f);
		CPoint pt1(rect.left,y1+iSub);
		CPoint pt2(rect.right,y1+iSub);
		pDC->_DrawDotLine(pt1,pt2,3,clr);
		CRect RectText = CRect ( rect.left+1,y1+iSub-20,rect.left+80,y1+iSub-1);
		pDC->DrawText(_T("87.5%"),RectText,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	
}

void SDDrawZq( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
// 	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
// 	if ( NULL == pDependentCurve )
// 	{
// 		return;
// 	}
	if ( NULL == pRegion )
	{
		return;
	}

	float fWidth = pRegion->m_aXAxisNodes[0].m_fPixelWidthPerUnit;

	int32 iWidth = x2 - x1;
	if ( iWidth < 2.0 || fWidth <= 0.0 )
	{
		return;
	}
	int32 iCycle = (int32)(iWidth/fWidth + 0.5);	// 跳过的周期, 其实跟直接用iWidth没区别
	if ( iCycle < 1 )
	{
		return;
	}

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	CRect rect = pRegion->GetRectCurves();

// 	int32 id1,id2;
// 	float fVal1,fVal2;
// 	pRegion->RegionToSD(x1,y1,id1,fVal1);
// 	pRegion->RegionToSD(x2,y2,id2,fVal2);

	float fb = (float)x1;
	while ( true )
	{
		float ib = fb;
		CPoint pt1((int32)ib,rect.top);
		CPoint pt2((int32)ib,rect.bottom);
		if ( ib == x1 )
		{
			pDC->_DrawLine(pt1,pt2);
			if ( NULL != pCurve && NULL != pPickDC )
			{
				pPickDC->_DrawLine(pt1,pt2);
			}
		}
		else
		{
			pDC->_DrawDotLine(pt1,pt2,4,clr);
		}
		fb += fWidth*iCycle;

		if ( ib >= rect.right )
		{
			break;
		}

	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	
}


void SDDrawJx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF _clr)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}

	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CRect rect = CRect(x1>x2?x2:x1,y1>y2?y2:y1,x1>x2?x1:x2,y1>y2?y1:y2);
	pDC->_DrawRect(rect);
	CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
	CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
		}
		pPickDC->BeginDraw(pCurve,2);
		pPickDC->_DrawRect(rect);
		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);
		pPickDC->EndDraw();	
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

}
void SDDrawJgtd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 x3,int32 y3,COLORREF _clr)
{
	
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1(x1,y1);
	CPoint pt2(x2,y2);
	CRect rect = pRegion->GetRectCurves();
	CPoint pts[4];
	CaclTwoPoint(rect,pt1,pt2,pts);
	pt1 = pts[0];
	pt2 = pts[1];
	pDC->_DrawLine(pt1,pt2);

	int32 x4,y4;
	CalcParallel ( x1,y1,x2,y2,x3,y3,x4,y4);
	CPoint pt3(x3,y3);
	CPoint pt4(x4,y4);
	CaclTwoPoint(rect,pt3,pt4,pts);
	pt3 = pts[0];
	pt4 = pts[1];
	pDC->_DrawLine(pt3,pt4);

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		CRect Rect3(x3-KSelfDrawPointSize,y3-KSelfDrawPointSize,x3+KSelfDrawPointSize+1,y3+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect3,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2, pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect3, pCurve->GetNode()->m_clr);
		}
		pPickDC->BeginDraw(pCurve,2);
		pPickDC->_DrawLine(pt1,pt2);
		pPickDC->_DrawLine(pt3,pt4);
		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);
		pPickDC->_FillSolidRect(&Rect3,0);
		pPickDC->EndDraw();
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}
void SDDrawPxzx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 x3,int32 y3,COLORREF _clr)
{
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	CPoint pt1(x1,y1);
	CPoint pt2(x2,y2);
	pDC->_DrawLine(pt1,pt2);

	int32 x4,y4,x5,y5,x6,y6;
	CalcParallel2 ( x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6);
	CPoint pt3(x3,y3);
	CPoint pt4(x4,y4);
	pDC->_DrawLine(pt3,pt4);

	CPoint pt5(x5,y5);
	CPoint pt6(x6,y6);
	pDC->_DrawLine(pt5,pt6);

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		CRect Rect3(x3-KSelfDrawPointSize,y3-KSelfDrawPointSize,x3+KSelfDrawPointSize+1,y3+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect3,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect3,pCurve->GetNode()->m_clr);
		}
		pPickDC->BeginDraw(pCurve,2);
		pPickDC->_DrawLine(pt1,pt2);
		pPickDC->_DrawLine(pt3,pt4);
		pPickDC->_DrawLine(pt5,pt6);
		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);
		pPickDC->_FillSolidRect(&Rect3,0);
		pPickDC->EndDraw();
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}
static int32 siFblqValue = 0;
static int32 siFblqValue2 = 0;
void ResetFblq ()
{
	siFblqValue = 0;
	siFblqValue2 = 0;
}
int32 GetNextFblq ( )
{
	if ( 0 == siFblqValue )
	{
		siFblqValue = 1;
		return 0;
	}
	if ( 1 == siFblqValue )
	{
		siFblqValue = 2;
		return 1;
	}
	if ( 2 == siFblqValue )
	{
		siFblqValue = -1;
		siFblqValue2 = 1;
		return 2;
	}
	if ( -1 == siFblqValue )
	{
		siFblqValue = 2;
	}
	int32 last = siFblqValue;
	siFblqValue += siFblqValue2;
	siFblqValue2 = last;
	return siFblqValue;

}
void SDDrawFblq ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,bool32 bRise,COLORREF _clr )
{
	if (NULL == pRegion || NULL == pDC)
	{
		return;
	}

 	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
 	if ( NULL == pDependentCurve )
 	{
 		return;
 	}

	ResetFblq();

	if ( pRegion->m_aXAxisNodes.GetSize() <= 0 )
	{
		return;
	}

	// float fWidth = pRegion->m_aXAxisNodes[0].m_iEndPixel - pRegion->m_aXAxisNodes[0].m_iStartPixel;
	float fWidth = pRegion->m_aXAxisNodes[0].m_fPixelWidthPerUnit;
	ASSERT(fWidth > 0.0);
	if ( fWidth <= 0.0 )
	{
		return;
	}
	int32 iLastXPixel =	pRegion->m_aXAxisNodes[pRegion->m_aXAxisNodes.GetSize()-1].m_iEndPixel;

	if (NULL !=  pRegion->GetDependentCurve() &&NULL != pRegion->GetDependentCurve()->GetNodes() )
	{
		CNodeSequence *pNodes =	pRegion->GetDependentCurve()->GetNodes();
		int32 iPos = pNodes->GetSize() -1;
		int32 iXEnd	= 0;
		if ( iPos > 0 && pRegion->GetDependentCurve()->CurvePosToRegionX(iPos, iXEnd) )
		{
			pRegion->RegionXToClient(iXEnd);
			iLastXPixel	= iXEnd;
		}		
	}

	//
 	if ( NULL != pCurve && NULL != pPickDC )
 	{
 		pPickDC->BeginDraw(pCurve,2);
 	}
 	CPen pen,*pOldPen;
 	COLORREF clr;
 	if ( 0 == _clr )
 	{
 		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
 	}
 	else
 	{
 		clr = _clr;
 	}

 	pen.CreatePen(PS_SOLID,1,clr);
 	pOldPen = (CPen*)pDC->SelectObject(&pen);
 	COLORREF clrOld = pDC->SetTextColor(clr);
 	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
 	CRect rect = pRegion->GetRectCurves();
 
	//
 	int32 ib;
	int32 ibOld = -1;
 	while ( true )
 	{
 		int32 ifblq = GetNextFblq();
 		ib = x + roundf_(ifblq * fWidth);
 		
		if ( ib >= rect.right || ib >= iLastXPixel )
 		{
 			break;
		}
		
		if ( ibOld == ib )
		{
			continue;
		}
		
		ibOld = ib;
 
		//
 		CPoint pt1(ib,rect.top);
 		CPoint pt2(ib,rect.bottom);
 		if ( ib == x )
 		{
 			pDC->_DrawLine(pt1,pt2);
 			if ( NULL != pCurve && NULL != pPickDC )
 			{
 				pPickDC->_DrawLine(pt1,pt2);
 			}
 		}
 		else
 		{
 			pDC->_DrawDotLine(pt1,pt2,4,clr);
 		}
 		CRect RectText(pt1.x+1,rect.top+1,pt1.x+40,rect.top+20);
 		CString StrText;
 		StrText.Format(_T("%d"),ifblq);
 		pDC->DrawText(StrText,&RectText,DT_LEFT | DT_TOP | DT_SINGLELINE);
 	}
 
 	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
 	{
 		CRect Rect1(x-KSelfDrawPointSize,y-KSelfDrawPointSize,x+KSelfDrawPointSize+1,y+KSelfDrawPointSize+1);
 		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
 		{
 		//	pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
				pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
 		}
 //		pPickDC->BeginDraw(pCurve,2);
 
 		pPickDC->_FillSolidRect(&Rect1,0);
 		pPickDC->EndDraw();
 	}
 
 	pDC->SetTextColor(clrOld);
 	pDC->SetBkMode(oldMode);
 	pDC->SelectObject(pOldPen);
 	pen.DeleteObject();	

}
bool32 _CalcDir ( CRect rect,int32 x1,int32 y1,int32& x2,int32& y2 )
{
	int32 xsub = x2-x1;
	int32 ysub = y2-y1;

	//垂直
	if ( 0 == xsub || 0 == ysub )
	{
		return false;
	}
	//水平

	float k = (float)ysub;
	k /= xsub;
	float b = y2 - k * x2;

	if ( x2 > x1 )
	{
		x2 = 2000;
	}
	else
	{
		x2 = 0;
	}
	y2 = (int32)(k * x2 + b);

	return true;
}

void SDDrawZs ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1, int32 x2, int32 y2,COLORREF _clr)
{
// 	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
// 	if ( NULL == pDependentCurve )
// 	{
// 		return;
// 	}

	if ( x2 <= x1 )
	{
		return;
	}

	CRect rect = pRegion->GetRectCurves();
	int32 iHeight = ( y2 - y1 ) / 3;

	CPoint pt(x1,y1);
	CPoint pt1(x2,y2);
	CPoint pt2(x2,y2-iHeight);
	CPoint pt3(x2,y2-iHeight*2);

	if ( !_CalcDir ( rect, x1,y1,pt1.x,pt1.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,pt2.x,pt2.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,pt3.x,pt3.y))
	{
		return;
	}

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->_DrawLine(pt,pt1);
	}
	pDC->_DrawLine(pt,pt1);
	pDC->_DrawDotLine(pt,pt2,3,clr);
	pDC->_DrawDotLine(pt,pt3,3,clr);


	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	

}

void SDDrawJejd( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1, int32 x2, int32 y2,COLORREF _clr)
{
// 	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
// 	if ( NULL == pDependentCurve )
// 	{
// 		return;
// 	}
	if ( x2 <= x1 )
	{
		return;
	}
	
	CRect rect = pRegion->GetRectCurves();
	
	CPoint pt(x1,y1);
	CPoint ph1 ( x2,y2 );//
	CPoint ph2 ( x2,y1+(y2-y1)/2);//2:1
	CPoint ph3 ( x2,y1+(y2-y1)/3);//3:1
	CPoint ph4 ( x2,y1+(y2-y1)/4);//4:1
	CPoint ph5 ( x2,y1+(y2-y1)/8);//8:1
	CPoint ph6 ( 2000,y1);//

	CRect Rect0 ( x1-20,y1-20,x1-1,y1-1);
	CRect RectX ( x2,y1-20,x2+40,y1-1);
	CRect RectY ( x1-20,y2-20,x1-1,y2-1);

	CRect Recth1 ( ph1.x+1,ph1.y-20,ph1.x+40,ph1.y-1);
	CRect Recth2 ( ph2.x+1,ph2.y-20,ph2.x+40,ph2.y-1);
	CRect Recth3 ( ph3.x+1,ph3.y-20,ph3.x+40,ph3.y-1);
	CRect Recth4 ( ph4.x+1,ph4.y-20,ph4.x+40,ph4.y-1);
	CRect Recth5 ( ph5.x+1,ph5.y-20,ph5.x+40,ph5.y-1);

	CPoint pv2 ( x1+(x2-x1)/2, y2);//2:1
	CPoint pv3 ( x1+(x2-x1)/3, y2);//3:1
	CPoint pv4 ( x1+(x2-x1)/4, y2);//4:1
	CPoint pv5 ( x1+(x2-x1)/8, y2);//8:1
	CPoint pv6 ( x1,y2>y1?2000:0);//

	CRect Rectv2 ( pv2.x+1,pv2.y-20,pv2.x+40,pv2.y-1);
	CRect Rectv3 ( pv3.x+1,pv3.y-20,pv3.x+40,pv3.y-1);
	CRect Rectv4 ( pv4.x+1,pv4.y-20,pv4.x+40,pv4.y-1);
	CRect Rectv5 ( pv5.x+1,pv5.y-20,pv5.x+40,pv5.y-1);

	if ( !_CalcDir ( rect, x1,y1,ph1.x,ph1.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,ph2.x,ph2.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,ph3.x,ph3.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,ph4.x,ph4.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,ph5.x,ph5.y))
	{
		return;
	}
	
	if ( !_CalcDir ( rect, x1,y1,pv2.x,pv2.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,pv3.x,pv3.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,pv4.x,pv4.y))
	{
		return;
	}
	if ( !_CalcDir ( rect, x1,y1,pv5.x,pv5.y))
	{
		return;
	}

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);

	pDC->_DrawLine(pt,ph1);
	pDC->_DrawLine(pt,ph6);
	pDC->_DrawLine(pt,pv6);

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->_DrawLine(pt,ph1);
	}
	pDC->_DrawDotLine(pt,ph2,3,clr);
	pDC->_DrawDotLine(pt,ph3,3,clr);
	pDC->_DrawDotLine(pt,ph4,3,clr);
	pDC->_DrawDotLine(pt,ph5,3,clr);

	pDC->_DrawDotLine(pt,pv2,3,clr);
	pDC->_DrawDotLine(pt,pv3,3,clr);
	pDC->_DrawDotLine(pt,pv4,3,clr);
	pDC->_DrawDotLine(pt,pv5,3,clr);

	pDC->DrawText(_T("0"),&Rect0,DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("X"),&RectX,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("Y"),&RectY,DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	pDC->DrawText(_T("1:1"),&Recth1,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("2:1"),&Recth2,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("3:1"),&Recth3,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("4:1"),&Recth4,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("8:1"),&Recth5,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	pDC->DrawText(_T("1:2"),&Rectv2,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("1:3"),&Rectv3,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("1:4"),&Rectv4,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
	pDC->DrawText(_T("1:8"),&Rectv5,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	

}
void SetPointHuiGui(CChartRegion* pRegion,int32 foot, int32 footOld,int32& y1,int32& y2,int32& xmax,int32& ymax,int32& xmin,int32& ymin)
{
	CChartCurve* pCurveDependent = pRegion->GetDependentCurve();
	if ( NULL == pCurveDependent )
	{
		return;
	}
	CNodeSequence* pNodes = pCurveDependent->GetNodes();
	if(footOld >= foot)
		return ;
	if(foot >= pCurveDependent->GetNodes()->GetSize())
		return;

	float fMax = FLT_MIN;
	float fMin = FLT_MAX;
	//--		计算平均值
	float x=0;// X=(1/N) Σxi
	float y=0;// Y=(1/N) ΣYi
	for (int32 i = footOld; i <= foot; i++)
	{
		x += i;
		CNodeData NodeData;
		if (!pNodes->GetAt(i, NodeData))
			break;

		y += NodeData.m_fClose;
		if ( fMax < NodeData.m_fHigh )
		{
			fMax = NodeData.m_fHigh;
			xmax = i;
			//pDependentCurve->CurvePosToRegionX(i,xmax);
		}
		if ( fMin > NodeData.m_fLow )
		{
			fMin = NodeData.m_fLow;
			xmin = i;
			//pDependentCurve->CurvePosToRegionX(i,xmin);
		}
	}

	if((foot-footOld+1)==0)
		return;

	x=x/(foot-footOld+1);
	y=y/(foot-footOld+1);
	/*根据一元回归分析的最小二乘法求N根日K线的线性回归。
	Yi=K0+K1*Xi    I=1，…n,
	K0=Y-K1*X
	K1=(Σ(Xi-X)Yi/ Σ(Xi-X) (Xi-X)
	其中: X=(1/N) Σxi
	Y=(1/N) ΣYi
	N=1,   N
	*/
	float sumTop=0;
	float sumBottom=0;
	for(int32 i=footOld;i<=foot;i++)
	{
		CNodeData NodeData;
		pNodes->GetAt(i,NodeData);
		sumTop+=(i-x)*NodeData.m_fClose;
		sumBottom+=(i-x)*(i-x);
	}
	float k1=sumTop/sumBottom;
	float k0=y-k1*x;
	
	//---		计算2点
	float yNow=footOld*k1+k0;
	pCurveDependent->PriceYToRegionY(yNow,y1);
	pRegion->RegionYToClient(y1);

	yNow=foot*k1+k0;
	pCurveDependent->PriceYToRegionY(yNow,y2);
	pRegion->RegionYToClient(y2);

	pCurveDependent->PriceYToRegionY(fMax,ymax);
	pRegion->RegionYToClient(ymax);

	pCurveDependent->PriceYToRegionY(fMin,ymin);
	pRegion->RegionYToClient(ymin);
}

void SDDrawXxhgd( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF _clr)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	CNodeSequence* pNodes = pDependentCurve->GetNodes();
	if ( NULL == pNodes )
	{
		return;
	}

	int32 iStart,iEnd;
	if ( !pNodes->LookupEx(id1,iStart) )
	{
		return;
	}
	if ( !pNodes->LookupEx(id2,iEnd) )
	{
		return;
	}
	if ( iStart > iEnd )
	{
		int32 iTemp = iEnd;
		iEnd = iStart;
		iStart = iTemp;
	}
	if ( iStart >= (iEnd-1) )
	{
		return;
	}
	int32 ya = y1;
	int32 yb = y2;
	int32 maxPos,ymax,minPos,ymin;
	SetPointHuiGui ( pRegion, iEnd,iStart,ya,yb,maxPos,ymax,minPos,ymin);

	CRect rect = pRegion->GetRectCurves();

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);

	CPoint pt1(x1,ya);
	CPoint pt2(x2,yb);
	pDC->_DrawLine(pt1,pt2);

	if ( 0 == _clr || ( NULL != pCurve ) && CheckFlag(pCurve->m_iFlag,CChartCurve::KSelect))
	{
		CPoint pta(x1,rect.top);
		CPoint ptb(x1,rect.bottom);
		pDC->_DrawDotLine(pta,ptb,3,clr);

		CPoint ptc(x2,rect.top);
		CPoint ptd(x2,rect.bottom);
		pDC->_DrawDotLine(ptc,ptd,3,clr);

	}

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->_DrawLine(pt1,pt2);
	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	

}

bool32 CalcParallelHgd ( int32 x1,int32 y1,int32 x2,int32 y2, int32 x3,int32 y3,int32& ya,int32& yb )
{
	int32 xsub = x2-x1;
	int32 ysub = y2-y1;

	//垂直
	if ( 0 == xsub )
	{
		return false;
	}
	//水平
	else if ( 0 == ysub )
	{
		ya = y3;
		yb = y3;
		return true;
	}
	else
	{
		float k = (float)ysub;
		k /= xsub;
		float b = (float)(y3 - k * x3);
		ya = (int32)(k * x1 + b);
		yb = (int32)(k * x2 + b);
		return true;
	}
}

void SDDrawYcxxhgd( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF _clr)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	CNodeSequence* pNodes = pDependentCurve->GetNodes();
	if ( NULL == pNodes )
	{
		return;
	}

	int32 iStart,iEnd;
	if ( !pNodes->LookupEx(id1,iStart) )
	{
		return;
	}
	if ( !pNodes->LookupEx(id2,iEnd) )
	{
		return;
	}
	if ( iStart > iEnd )
	{
		int32 iTemp = iEnd;
		iEnd = iStart;
		iStart = iTemp;
	}
	if ( iStart >= (iEnd-1) )
	{
		return;
	}
	int32 ya = y1;
	int32 yb = y2;
	int32 maxPos,ymax,minPos,ymin;
	SetPointHuiGui ( pRegion, iEnd,iStart,ya,yb,maxPos,ymax,minPos,ymin);
	int32 xmax,xmin;
	pDependentCurve->CurvePosToRegionX(maxPos,xmax);
	pRegion->RegionXToClient(xmax);
	pDependentCurve->CurvePosToRegionX(minPos,xmin);
	pRegion->RegionXToClient(xmin);

	int32 yi,yii;
	CalcParallelHgd ( x1,ya,x2,yb,xmin,ymin,yi,yii);

	int32 yj,yjj;
	CalcParallelHgd ( x1,ya,x2,yb,xmax,ymax,yj,yjj);

	CRect rect = pRegion->GetRectCurves();

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);

	CPoint pt1(x1,ya);
	int32 xx = x2;
	int32 yy = yb;
	_CalcDir(rect,x1,ya,xx,yy);
	CPoint pt2(xx,yy);
	pDC->_DrawLine(pt1,pt2);

	CPoint pti(x1,yi);
	xx = x2;
	yy = yii;
	_CalcDir(rect,x1,yi,xx,yy);
	CPoint ptii(xx,yy);
	pDC->_DrawLine(pti,ptii);

	CPoint ptj(x1,yj);
	xx = x2;
	yy = yjj;
	_CalcDir(rect,x1,yj,xx,yy);
	CPoint ptjj(xx,yy);
	pDC->_DrawLine(ptj,ptjj);

	pDC->_FillSolidRect(CRect(xmax-2,ymax-2,xmax+2,ymax+2),RGB(0,255,255));
	pDC->_FillSolidRect(CRect(xmin-2,ymin-2,xmin+2,ymin+2),RGB(0,255,255));

	if ( 0 == _clr || ( NULL != pCurve ) && CheckFlag(pCurve->m_iFlag,CChartCurve::KSelect))
	{
		CPoint pta(x1,rect.top);
		CPoint ptb(x1,rect.bottom);
		pDC->_DrawDotLine(pta,ptb,3,clr);

		CPoint ptc(x2,rect.top);
		CPoint ptd(x2,rect.bottom);
		pDC->_DrawDotLine(ptc,ptd,3,clr);

	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_DrawLine(pt1,pt2);
		pPickDC->_DrawLine(pti,ptii);
		pPickDC->_DrawLine(ptj,ptjj);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	

}
void SDDrawJxhgd( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF _clr)
{
	CChartCurve* pDependentCurve = pRegion->GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		return;
	}
	CNodeSequence* pNodes = pDependentCurve->GetNodes();
	if ( NULL == pNodes )
	{
		return;
	}

	int32 iStart,iEnd;
	if ( !pNodes->LookupEx(id1,iStart) )
	{
		return;
	}
	if ( !pNodes->LookupEx(id2,iEnd) )
	{
		return;
	}
	if ( iStart > iEnd )
	{
		int32 iTemp = iEnd;
		iEnd = iStart;
		iStart = iTemp;
	}
	if ( iStart >= (iEnd-1) )
	{
		return;
	}
	int32 ya = y1;
	int32 yb = y2;
	int32 maxPos,ymax,minPos,ymin;
	SetPointHuiGui ( pRegion, iEnd,iStart,ya,yb,maxPos,ymax,minPos,ymin);
	int32 xmax,xmin;
	pDependentCurve->CurvePosToRegionX(maxPos,xmax);
	pRegion->RegionXToClient(xmax);
	pDependentCurve->CurvePosToRegionX(minPos,xmin);
	pRegion->RegionXToClient(xmin);

	int32 yi,yii;
	CalcParallelHgd ( x1,ya,x2,yb,xmin,ymin,yi,yii);

	int32 yj,yjj;
	CalcParallelHgd ( x1,ya,x2,yb,xmax,ymax,yj,yjj);

	CRect rect = pRegion->GetRectCurves();

	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,2);
	}
	CPen pen,*pOldPen;
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = KSelfDrawDefaultClr;//RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	pen.CreatePen(PS_SOLID,1,clr);
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);

	CPoint pt1(x1,ya);
	CPoint pt2(x2,yb);
	pDC->_DrawLine(pt1,pt2);

	CPoint pti(x1,yi);
	CPoint ptii(x2,yii);
	pDC->_DrawLine(pti,ptii);

	CPoint ptj(x1,yj);
	CPoint ptjj(x2,yjj);
	pDC->_DrawLine(ptj,ptjj);

	pDC->_FillSolidRect(CRect(xmax-2,ymax-2,xmax+2,ymax+2),RGB(0,255,255));
	pDC->_FillSolidRect(CRect(xmin-2,ymin-2,xmin+2,ymin+2),RGB(0,255,255));

	if ( 0 == _clr || ( NULL != pCurve ) && CheckFlag(pCurve->m_iFlag,CChartCurve::KSelect))
	{
		CPoint pta(x1,rect.top);
		CPoint ptb(x1,rect.bottom);
		pDC->_DrawDotLine(pta,ptb,3,clr);

		CPoint ptc(x2,rect.top);
		CPoint ptd(x2,rect.bottom);
		pDC->_DrawDotLine(ptc,ptd,3,clr);

	}

	if ( NULL != pCurve && NULL != pCurve->GetNode() && NULL != pPickDC )
	{
		CRect Rect1(x1-KSelfDrawPointSize,y1-KSelfDrawPointSize,x1+KSelfDrawPointSize+1,y1+KSelfDrawPointSize+1);
		CRect Rect2(x2-KSelfDrawPointSize,y2-KSelfDrawPointSize,x2+KSelfDrawPointSize+1,y2+KSelfDrawPointSize+1);
		if ( CheckFlag(pCurve->m_iFlag,CDrawingCurve::KSelect))
		{
// 			pDC->_FillSolidRect(&Rect1,RGB(255,255,255));
// 			pDC->_FillSolidRect(&Rect2,RGB(255,255,255));
			pDC->_FillSolidRect(&Rect1,pCurve->GetNode()->m_clr);
			pDC->_FillSolidRect(&Rect2,pCurve->GetNode()->m_clr);
		}
//		pPickDC->BeginDraw(pCurve,2);

		pPickDC->_DrawLine(pt1,pt2);
		pPickDC->_DrawLine(pti,ptii);
		pPickDC->_DrawLine(ptj,ptjj);

		pPickDC->_FillSolidRect(&Rect1,0);
		pPickDC->_FillSolidRect(&Rect2,0);

		pPickDC->EndDraw();
	}

	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();	

}



void SDDrawBj ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,bool32 bRise,COLORREF _clr )
{
	static CImageList* spSDDrawImageList = NULL;
	if ( NULL == spSDDrawImageList )
	{
		spSDDrawImageList = new CImageList;
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		spSDDrawImageList->Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
	}
	int32 iImageId = 3;
	if ( bRise )
	{
		iImageId = 1;
	}
	spSDDrawImageList->Draw(pDC,iImageId,CPoint(x-8,y-8),ILD_NORMAL);
	if ( NULL != pCurve && NULL != pPickDC )
	{
		CRect rect = CRect(x-8,y-8,x+9,y+9);
		pPickDC->BeginDraw(pCurve,1);
		pPickDC->_FillSolidRect(rect,0);
		pPickDC->EndDraw();
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KSelect))
		{
			CPen pen,*pOldPen;
			pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
			pOldPen = (CPen*)pDC->SelectObject(&pen);
			pDC->_DrawRect(rect);
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
}
void SDDrawText ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,CString StrText,COLORREF _clr )
{
	CSize sz = pDC->GetTextExtent(StrText);
	COLORREF clr;
	if ( 0 == _clr )
	{
		clr = RGB(255,255,255);
	}
	else
	{
		clr = _clr;
	}
	CRect rect = CRect (x,y,x+sz.cx+1,y+sz.cy+1);
	COLORREF clrOld = pDC->SetTextColor(clr);
	int32 oldMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(StrText,&rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	if ( NULL != pCurve && NULL != pPickDC )
	{
		pPickDC->BeginDraw(pCurve,1);
		pPickDC->_FillSolidRect(rect,0);
		pPickDC->EndDraw();
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KSelect))
		{
			CPen pen,*pOldPen;
			pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
			pOldPen = (CPen*)pDC->SelectObject(&pen);
			pDC->_DrawRect(rect);
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(oldMode);
}

void CalcParallel ( int32 x1,int32 y1,int32 x2,int32 y2, int32 x3,int32 y3,int32& x4,int32& y4 )
{
	int32 xsub = x2-x1;
	int32 ysub = y2-y1;

	//垂直
	if ( 0 == xsub )
	{
		x4 = x3;
		y4 = y3 + 10;
		return;
	}
	//水平
	else if ( 0 == ysub )
	{
		y4 = y3;
		x4 = x3 + 10;
		return;
	}
	else
	{

		float k = (float)ysub;
		k /= xsub;
		float b = y3 - k * x3;
		x4 = x3 * 2;
		y4 = (int32)(k * x4 + b);
	}
}

void _CalcParallel ( int32 x1,int32 y1,int32 x2,int32 y2, int32& x3,int32& y3,int32& x4,int32& y4 )
{
	int32 xsub = x2-x1;
	int32 ysub = y2-y1;

	//垂直
	if ( 0 == xsub )
	{
		y3 = y1;
		y4 = y2;
		x4 = x3;
		return;
	}
	//水平
	else if ( 0 == ysub )
	{
		y3 = y1;
		y4 = y2;
		x4 = x2;
		return;
	}
	else
	{

		float k = (float)ysub;
		k /= xsub;
		float b = y3 - k * x3;

		y3 = y1;
		y4 = y2;

		x3 = (int32)(( y3 - b ) / k);
		x4 = (int32)(( y4 - b ) / k);
	}
}

void CalcParallel2 ( int32 x1,int32 y1,int32 x2,int32 y2, int32& x3,int32& y3,int32& x4,int32& y4,int32& x5,int32& y5,int32& x6,int32& y6 )
{
	_CalcParallel(x1,y1,x2,y2,x3,y3,x4,y4);
	x5 = x1 - (x3-x1);
	y5 = y1 - (y3-y1);
	_CalcParallel(x1,y1,x2,y2,x5,y5,x6,y6);
}


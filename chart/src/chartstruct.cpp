#include "stdafx.h"
#include "chartstruct.h"
#include "ChartCurve.h"

void KLine2NodeData ( const CKLine& KLine, CNodeData& NodeData)
{
	NodeData.m_iID		= (int32)KLine.m_TimeCurrent.GetTime();

	NodeData.m_fOpen	= KLine.m_fPriceOpen;
	NodeData.m_fClose	= KLine.m_fPriceClose;
	NodeData.m_fLow		= KLine.m_fPriceLow;
	NodeData.m_fHigh	= KLine.m_fPriceHigh;
	NodeData.m_fAvg		= KLine.m_fPriceAvg;
	NodeData.m_fVolume	= KLine.m_fVolume;
	NodeData.m_fAmount	= KLine.m_fAmount;
	NodeData.m_fHold	= KLine.m_fHold;
}

CString TimeInterval2String ( E_NodeTimeInterval eTimeInterval ,UINT uiUserMinutes, UINT uiUserDays)
{
	CString StrText;
	switch ( eTimeInterval )
	{
	case ENTIMinute:
		StrText = _T("1分钟");
		break;
	case ENTIDay:
		StrText = _T("日线");
		break;
	case ENTIMinute5:
		StrText = _T("5分钟");
		break;
	case ENTIMinute15:
		StrText = _T("15分钟");
		break;
	case ENTIMinute30:
		StrText = _T("30分钟");
		break;
	case ENTIMinute60:
		StrText = _T("60分钟");
		break;
	case ENTIMinuteUser:
		StrText.Format(L"%d分钟",uiUserMinutes);			
		break;
	case ENTIDayUser:
		StrText.Format(L"%d日",uiUserDays);
		break;
	case ENTIWeek:
		StrText = _T("周线");
		break;
	case ENTIMonth:
		StrText = _T("月线");
		break;
	case ENTIQuarter:
		StrText = _T("季线");
		break;
	case ENTIYear:
		StrText = _T("年线");
		break;
	case ENTIMinute180:
		StrText = _T("180分钟");
		break;
	case ENTIMinute240:
		StrText = _T("240分钟");
		break;
	default:
		////ASSERT(0);
		break;
	}
	return StrText;
}
//////////////////////////////////////////////////////
// CTrendTradingDayInfo
CTrendTradingDayInfo::CTrendTradingDayInfo()
{	
	m_pMerch			= NULL;

	m_bInit				= false;
	m_bNeedRecalc		= false;
	m_fPricePrevClose	= 0.;
	m_fPricePrevAvg		= 0.;
	m_fHoldPrev			= 0.;
}

void CTrendTradingDayInfo::Set(IN CMerch *pMerch, const CMarketIOCTimeInfo &MarketIOCTime)
{
	m_pMerch			= pMerch;

	// 
	m_MarketIOCTime		= MarketIOCTime;

	m_bInit = true;

	m_fPricePrevClose	= 0.;
	m_fPricePrevAvg		= 0.;
	m_fHoldPrev			= 0.0f;
}

void CTrendTradingDayInfo::RecalcPrice(const CMerch &Merch)
{
	m_fPricePrevClose	= 0.;
	m_fPricePrevAvg		= 0.;
	m_bNeedRecalc		= false;

	//
	if (m_pMerch == &Merch)
	{
		// xl 1016 对于跨天交易，init时间为前一天时间，后面函数调用找寻k线时，找寻的为错误的k线，修改为统一使用收盘时间作为当天时间
		//CGmtTime Time(m_MarketIOCTime.m_TimeInit.m_Time);
		CGmtTime Time(m_MarketIOCTime.m_TimeClose.m_Time);
		if (!Merch.GetSpecialDayPrice(Time.GetTime(), m_fPricePrevClose, m_fPricePrevAvg))
		{
			m_fPricePrevClose	= 0.;
			m_fPricePrevAvg		= 0.;
		}

		if (NULL == m_pMerch->m_pRealtimePrice || 0. == m_pMerch->m_pRealtimePrice->m_fPricePrevAvg || 0. == m_pMerch->m_pRealtimePrice->m_fPricePrevClose || 0. == m_fPricePrevClose || 0. == m_fPricePrevAvg)
		{
			m_bNeedRecalc = true;
		}
	}
}

float CTrendTradingDayInfo::GetPrevReferPrice()
{
	if (NULL != m_pMerch)
	{
		//	if( CReportScheme::IsFuture(m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
		if ( ERTFuturesCn == m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType )
		{
			return m_fPricePrevAvg;
		}
		else
		{
			return m_fPricePrevClose;
		}
	}


	return m_fPricePrevClose;// 找不到商品信息
}

void CTrendTradingDayInfo::RecalcHold(const CMerch &Merch)
{
	m_fHoldPrev = 0.;

	if (m_pMerch == &Merch)
	{
		// xl 1016 对于跨天交易，init时间为前一天时间，后面函数调用找寻k线时，找寻的为错误的k线，修改为统一使用收盘时间作为当天时间
		//CGmtTime Time(m_MarketIOCTime.m_TimeInit.m_Time);
		CGmtTime Time(m_MarketIOCTime.m_TimeClose.m_Time);
		if (!Merch.GetSpecialDayHold(Time.GetTime(), m_fHoldPrev))
		{
			m_fHoldPrev = 0.;			
		}
	}
}

float CTrendTradingDayInfo::GetPrevHold()
{
	if (NULL != m_pMerch)
	{	
		return m_fHoldPrev;
	}

	
	return m_fHoldPrev;// 找不到商品信息
}
////////////////////////////////////////////////////////////////////////

T_MerchNodeUserData::T_MerchNodeUserData(/*CViewData* p*/)
{
	//m_pAbsCenterManager				= p;

	m_eTimeIntervalCompare	= ENTIDay;
	m_eTimeIntervalFull		= ENTIDay;	

	m_iShowPosInFullList	= 0;					// 当前屏幕显示的起始位置
	m_iShowCountInFullList	= 0;	

	m_pKLinesShow			= NULL;

	m_iTimeUserMultipleMinutes = 8;
	m_iTimeUserMultipleDays = 10;

	bMainMerch				= false;
	bUpdateOnce				= true;
	bNew					= false;
	m_pMerchNode			= NULL;
}

T_MerchNodeUserData::~T_MerchNodeUserData()
{
	int nsize = aIndexs.GetSize();
	//lint --e{438}
	for (int i = 0 ; i < nsize ; ++i)
	{
		T_IndexParam *pIndexParam = aIndexs.GetAt(i);
		if (pIndexParam)
		{
			delete pIndexParam;
			pIndexParam = NULL;
		}
	}
	aIndexs.RemoveAll();
}

bool32 T_MerchNodeUserData::UpdateShowData(int32 iShowPosInFullList, int32 iShowCountInFullList)
{
	/////////////////////////////////////////////////////////
	// 1. 先验证数据的完整性,  商品数据、指标数据中的Full数据一定是一样的的长度， 并且其中的显示区间都是一样的
	if (NULL == m_pKLinesShow)
	{		
		return false;
	}

	//  再验证参数设置范围是否合法
	if (iShowPosInFullList < 0 || iShowPosInFullList >= m_aKLinesFull.GetSize() ||
		iShowPosInFullList + iShowCountInFullList - 1 < 0 || iShowPosInFullList + iShowCountInFullList - 1 >= m_aKLinesFull.GetSize())
	{
	//	////ASSERT(0);
		return false;
	}

	//
	if (m_aKLinesFull.GetSize() <= 0)
		return true;

	if ( iShowCountInFullList == 0
		&& iShowPosInFullList >= 0
		&& iShowPosInFullList < m_aKLinesFull.GetSize() )
	{
		return true;
	}

	// do it
	int i = 0;
	for ( i = 0; i < aIndexs.GetSize(); i++)
	{
		T_IndexParam *pIndex = aIndexs[i];
		if (NULL == pIndex)
		{		
			return false;
		}

		for (int32 j = 0; j < pIndex->m_aIndexLines.GetSize(); j++)
		{
			CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[j];
			if (NULL == IndexNodeList.m_pNodesShow)
			{				
				return false;
			}

			if (IndexNodeList.m_aNodesFull.GetSize() != m_aKLinesFull.GetSize())
			{				
				return false;
			}
		}

		if (pIndex->m_iIndexShowPosInFullList != m_iShowPosInFullList || 
			pIndex->m_iIndexShowCountInFullList != m_iShowCountInFullList)
		{			
			return false;
		}
	}

	// 清空数据先
	m_pKLinesShow->m_aNodes.SetSize(0);
	for (i = 0; i < aIndexs.GetSize(); i++)
	{
		T_IndexParam* pIndex = aIndexs[i];		
		for (int32 j = 0; j < pIndex->m_aIndexLines.GetSize(); j++)
		{
			CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[j];
			((CNodeSequence *)IndexNodeList.m_pNodesShow)->m_aNodes.SetSize(0);
		}
	}

	//////////////////////////////////////////////////////////////
	// 2. 设置新的值
	m_iShowPosInFullList	= iShowPosInFullList;
	m_iShowCountInFullList	= iShowCountInFullList;
	for (i = 0; i < aIndexs.GetSize(); i++)
	{
		T_IndexParam* pIndex = aIndexs[i];		
		pIndex->m_iIndexShowPosInFullList	= iShowPosInFullList;
		pIndex->m_iIndexShowCountInFullList = iShowCountInFullList;	
	}

	//////////////////////////////////////////////////////////////////////////
	// 检查是否设置了数据有效标志
	bool32 bHasFlag = false;
	if ( m_aKLinesShowDataFlag.GetSize() == m_iShowCountInFullList )
	{
		// 长度 一样则视为设置了
		bHasFlag = true;
	}
	else if ( m_aKLinesShowDataFlag.GetSize() > 0 )
	{
		// 有设置，却不一样？		
		m_aKLinesShowDataFlag.RemoveAll();
	}

	///////////////////////////////////////////////////////////////
	// 4. 商品K线截断
	m_pKLinesShow->m_aNodes.SetSize(m_iShowCountInFullList);
	CNodeData *pKLinesShow	= (CNodeData *)m_pKLinesShow->m_aNodes.GetData();
	CKLine *pKLineNodesFull	= (CKLine *)m_aKLinesFull.GetData();
	for (int32 iChangeIndex = 0; iChangeIndex < m_iShowCountInFullList; iChangeIndex++)
	{
		KLine2NodeData(*(pKLineNodesFull + m_iShowPosInFullList + iChangeIndex), *(pKLinesShow + iChangeIndex));
		if ( bHasFlag )
		{
			// 设置flag
			pKLinesShow[iChangeIndex].m_iFlag = m_aKLinesShowDataFlag[iChangeIndex];
		}
	}

	///////////////////////////////////////////////////////////////
	// 5. 各个指标截断
	for (i = 0; i < aIndexs.GetSize(); i++)
	{
		T_IndexParam* pIndex = aIndexs[i];		
		for (int32 j = 0; j < pIndex->m_aIndexLines.GetSize(); j++)
		{
			CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[j];
			if ( IndexNodeList.m_aNodesFull.GetSize() < pIndex->m_iIndexShowPosInFullList+pIndex->m_iIndexShowCountInFullList )
			{
				// 数据不匹配, 怎么办
				ASSERT( 0 );
			}

			((CNodeSequence *)IndexNodeList.m_pNodesShow)->m_aNodes.SetSize(pIndex->m_iIndexShowCountInFullList);
			CNodeData *pNodesShow	= (CNodeData *)((CNodeSequence *)IndexNodeList.m_pNodesShow)->m_aNodes.GetData();
			CNodeData *pNodesFull	= (CNodeData *)IndexNodeList.m_aNodesFull.GetData();

			memcpyex(pNodesShow, pNodesFull + pIndex->m_iIndexShowPosInFullList, sizeof(CNodeData) * pIndex->m_iIndexShowCountInFullList);
			if ( bHasFlag )
			{
				// 设置各点的标志
				ASSERT( m_iShowCountInFullList == pIndex->m_iIndexShowCountInFullList );
				for ( int32 iFlagIndex =0; iFlagIndex < m_iShowCountInFullList ; iFlagIndex++ )
				{
					// 由于指标有可能出现某些点无效，所以判断下
					// 目前取并集
					DWORD dwIndex = pNodesShow[iFlagIndex].m_iFlag;
					pNodesShow[iFlagIndex].m_iFlag = (dwIndex | m_aKLinesShowDataFlag[iFlagIndex]);
				}
			}
		}
	}

	return true;
}


#include "StdAfx.h"
#include "MarketRadar.h"




#include "XmlShare.h"
#include "PathFactory.h"
#include "UserBlockManager.h"
#include "ReportScheme.h"
#include "ShareFun.h"
//#include "datamanager.h"


#ifdef _DEBUG 
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RADAR_MERCHCOUNT_PRE_REQ	(18) // 每次请求要发出去的商品数量
#define RADAR_TIMESALE_COUNT (20)		// 分笔数据个数

T_MarketRadarSubParameter::T_MarketRadarSubParameter()
{
	m_dwConditions = EMRC_BigBuy | EMRC_BigSell
		| EMRC_LowOpenRise | EMRC_HighOpenFall
		| EMRC_RiseOver | EMRC_FallBelow
		| EMRC_SingleTradeRiseFall | EMRC_HugeAmount
		| EMRC_BigVol
		;

	m_fWhatIsBigVol = 0.001f;		// 0.1%为大量

	m_fRiseOverValue = 0.05f;		// 涨幅破5%
	m_fFallBelowValue = 0.05f;		// 跌幅破5%
	m_fSingleTradeRiseFallValue = 0.012f;	// 异常单笔涨跌率 1.2%
	m_fHugeAmountValue = 1500000.0f;	// 巨额成交 150w

	m_iBigVolTimeRange = 5;			// 最近5分钟内大量
	m_iBigVolOverCount = 3;			// 成交> 3笔

	m_eVoiceType = EMRAVT_System;	// 系统声音
	m_bShowDlgListAlarm = true;		// 弹出列表
}

bool32 T_MarketRadarSubParameter::operator==( const T_MarketRadarSubParameter &Param ) const
{
	bool32 bEqual = m_dwConditions == Param.m_dwConditions
		&& m_fWhatIsBigVol == Param.m_fWhatIsBigVol
		&& m_fRiseOverValue == Param.m_fRiseOverValue
		&& Param.m_fFallBelowValue == m_fFallBelowValue
		&& Param.m_fSingleTradeRiseFallValue == m_fSingleTradeRiseFallValue
		&& Param.m_fHugeAmountValue == m_fHugeAmountValue
		&& Param.m_iBigVolTimeRange == m_iBigVolTimeRange
		&& Param.m_iBigVolOverCount == m_iBigVolOverCount
		&& Param.m_eVoiceType == m_eVoiceType
		&& (Param.m_bShowDlgListAlarm ? m_bShowDlgListAlarm : !m_bShowDlgListAlarm);
	if ( bEqual
		&& EMRAVT_User == m_eVoiceType )
	{
		bEqual = m_StrVoiceFilePath == m_StrVoiceFilePath;
	}
	return bEqual;
}

bool32 T_MarketRadarSubParameter::CompareCalcParam( const T_MarketRadarSubParameter &Param ) const
{
	// 仅比较与计算参数相关的
	bool32 bEqual = m_dwConditions == Param.m_dwConditions
		&& m_fWhatIsBigVol == Param.m_fWhatIsBigVol
		&& m_fRiseOverValue == Param.m_fRiseOverValue
		&& Param.m_fFallBelowValue == m_fFallBelowValue
		&& Param.m_fSingleTradeRiseFallValue == m_fSingleTradeRiseFallValue
		&& Param.m_fHugeAmountValue == m_fHugeAmountValue
		&& Param.m_iBigVolTimeRange == m_iBigVolTimeRange
		&& Param.m_iBigVolOverCount == m_iBigVolOverCount;
	return bEqual;
}

//////////////////////////////////////////////////////////////////////////
bool32 T_MarketRadarParameter::CompareCalcParam( const T_MarketRadarParameter &Param ) const
{
	bool32 bEqual = Param.m_ParamSub.CompareCalcParam( m_ParamSub )
		&& Param.m_eMerchRange == m_eMerchRange;
	if ( bEqual
		&& EMRMR_MerchArray == m_eMerchRange )
	{
		bEqual = m_aMerchs.GetSize() == Param.m_aMerchs.GetSize()
			&& 0 == memcmp(m_aMerchs.GetData(), Param.m_aMerchs.GetData(), sizeof(CMerch *)* m_aMerchs.GetSize());
	}
	return bEqual;
}

//////////////////////////////////////////////////////////////////////////

CMarketRadarCalc::T_CalcMerchData::T_CalcMerchData()
{
	m_pMerch = NULL;
	m_fRiseFallStopRate = 0.0f;
}

CMarketRadarCalc::T_CalcMerchData::T_CalcMerchData( const T_CalcMerchData &MerchData )
{
	m_pMerch = MerchData.m_pMerch;
	m_RealTimePrice = MerchData.m_RealTimePrice;
	m_aTicks.Copy(MerchData.m_aTicks);
	m_fRiseFallStopRate = MerchData.m_fRiseFallStopRate;
	m_TimeInit = MerchData.m_TimeInit;
}

const CMarketRadarCalc::T_CalcMerchData & CMarketRadarCalc::T_CalcMerchData::operator=( const T_CalcMerchData &MerchData )
{
	if ( this != &MerchData )
	{
		m_pMerch = MerchData.m_pMerch;
		m_RealTimePrice = MerchData.m_RealTimePrice;
		m_aTicks.Copy(MerchData.m_aTicks);
		m_fRiseFallStopRate = MerchData.m_fRiseFallStopRate;
		m_TimeInit = MerchData.m_TimeInit;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
VOID CALLBACK MyAPCProc( ULONG_PTR dwParam )
{

}

UINT __cdecl CMarketRadarCalc::ReqThreadFunc( LPVOID pParam )
{
	CMarketRadarCalc *pCalcObj = (CMarketRadarCalc *)pParam;
	if ( NULL == pCalcObj )
	{
		ASSERT( 0 );
		return 1;
	}
	
	T_MarketRadarParameter Param;
	pCalcObj->GetParameter(Param);	// 取参数快照，如果参数变更，只能中断计算，重新开始
	if ( Param.m_aMerchs.GetSize() <= 0
		|| Param.m_ParamSub.m_dwConditions == 0 )
	{
		return 0;	// 无需请求
	}

	HANDLE hWait = CreateWaitableTimer(NULL, FALSE, NULL);
	LARGE_INTEGER liTimer;
	liTimer.QuadPart = -5000*10000;		// 5秒钟后开始，防止反复的开/停操作导致的数据不停发送
	const LONG lPeriod = 3000;	// 若干ms请求一次
	const LONG lPeriod2 = 60000;//10*60000;	// 当不能请求数据时，休息若干ms
	ASSERT( lPeriod != lPeriod2 );
	if ( NULL == hWait
		|| !SetWaitableTimer(hWait, &liTimer, 0, NULL, NULL, FALSE) )
	{
		DEL_HANDLE(hWait);
		ASSERT( 0 );
		return 1;
	}
	WaitForSingleObjectEx(hWait, INFINITE, TRUE);	// 等待若干时间才开始
	
	LONG lPeriodCur = lPeriod2;
	while ( pCalcObj->GetRunFlag() != 0 )
	{
		bool32 bReq;
		if ( lPeriod2 == lPeriodCur )
		{
			bReq = pCalcObj->RequestStartViewData();
		}
		else
		{
			bReq = pCalcObj->RequestNextViewData();
		}

		if ( bReq )
		{
			liTimer.QuadPart = (LONGLONG)(-(int64)(lPeriod)*10000);
			SetWaitableTimer(hWait, &liTimer, 0, NULL, NULL, FALSE);
			lPeriodCur = lPeriod;
		}
		else
		{
			liTimer.QuadPart = (LONGLONG)(-(int64)(lPeriod2)*10000);
			SetWaitableTimer(hWait, &liTimer, 0, NULL, NULL, FALSE);
			lPeriodCur = lPeriod2;
		}
	    WaitForSingleObjectEx(hWait, INFINITE, TRUE);
	}

	DEL_HANDLE(hWait);

	TRACE(_T("雷达请求线程(%d)退出~~\n"), GetCurrentThreadId());
	return 0;
}


#define IsBigVol(fLiuTongGuBen, fVol, fRate)  (fLiuTongGuBen>0.0f ? (fVol/fLiuTongGuBen>=fRate) : false)
#define MyABS(fValue) (fValue>=0.0f ? fValue : -fValue)
UINT __cdecl CMarketRadarCalc::CalcThreadFunc( LPVOID pParam )
{
	CMarketRadarCalc *pCalcObj = (CMarketRadarCalc *)pParam;
	if ( NULL == pCalcObj )
	{
		ASSERT( 0 );
		return 1;
	}

	if ( NULL == pCalcObj->m_EventNewMerchData.m_hObject )
	{
		ASSERT( 0 );
		return 1;	// 没有初始化等待数据event
	}

	T_MarketRadarParameter Param;
	pCalcObj->GetParameter(Param);	// 取参数快照，如果参数变更，只能中断计算，重新开始
	const CGmtTime TimeCalcStartServer = pCalcObj->m_TimeCalcStart;	// 取时间快照
	const DWORD dwTimeStart = timeGetTime();
	//const CGmtTime TimeCalcStartLocal = CGmtTime::GetCurrentTime();
	ASSERT( TimeCalcStartServer.GetTime() > 0 );

	if ( Param.m_aMerchs.GetSize() <= 0
		|| Param.m_ParamSub.m_dwConditions == 0 )
	{
		return 0;	// 无需计算
	}

	while ( pCalcObj->GetRunFlag() != 0 )
	{
		// 开始等待计算, 无论结果是什么，都根据flag来判断
		DWORD dwWait = WaitForSingleObjectEx(pCalcObj->m_EventNewMerchData, INFINITE, TRUE);
		if ( dwWait != WAIT_OBJECT_0 )
		{
			ASSERT( pCalcObj->GetRunFlag() == 0 );
			break;	// 必然是要退出了
		}
		T_CalcMerchData MerchData;
		MarketRadarResultMap mapResults;
		while ( pCalcObj->GetNextNewCalcMerchData(MerchData, true) )
		{
			// 验证数据完整
			if (
				MerchData.m_RealTimePrice.m_fVolumeTotal == 0.0f
				|| MerchData.m_aTicks.GetSize() <= 0 )
			{
				continue;	// 无成交不计算
			}

			T_MarketRadarResult Result;
			Result.m_eCondition = EMRC_None;
			Result.m_iRiseFallFlag = 0;
			Result.m_pMerch = MerchData.m_pMerch;
			Result.m_TimeInit = MerchData.m_TimeInit;
			DWORD dwTime = timeGetTime();
			CGmtTimeSpan Span((time_t)(dwTime-dwTimeStart)/1000);
			Result.m_TimeCalc = TimeCalcStartServer + Span;	// 服务器时间，有误差时间

			// 找到最后一个有效分笔数据
			int32 iLastTick = MerchData.m_aTicks.GetUpperBound();
			for ( ; iLastTick >= 0 ; iLastTick-- )
			{
				if ( MerchData.m_aTicks[iLastTick].m_fVolume > 0.0f )
				{
					break;
				}
			}
			MerchData.m_aTicks.SetSize(iLastTick+1);	// 重设大小
			if ( iLastTick >= 0 )
			{
				Result.m_TickLast = MerchData.m_aTicks[iLastTick];
			}

			float fLiuTongGuBen = 0;
			if ( MerchData.m_RealTimePrice.m_fTradeRate > 0.0f )
			{
				// 目前换手率单位为%单位, 算出来的股本为手单位
				fLiuTongGuBen = MerchData.m_RealTimePrice.m_fVolumeTotal / MerchData.m_RealTimePrice.m_fTradeRate *100;
			}
			// 流通股本可能为0

			const DWORD dwConditions = Param.m_ParamSub.m_dwConditions;
			
			// 按照顺序判断
			// 连续大量 涨停 涨幅破 巨额 大量 单笔 低开 高开 大买挂单 大卖挂单 今日新高/低
			if ( EMRC_None == Result.m_eCondition
				&& (dwConditions & EMRC_BigVolInTime)
				&& iLastTick >= 0
				&& fLiuTongGuBen > 0.0f )
			{
				// 连续大量 xx时间内x大量
				int32 iBigVol = 0;
				int32 i = iLastTick;
				CGmtTimeSpan curSpan;
				const CGmtTimeSpan Span2(0, 0, Param.m_ParamSub.m_iBigVolTimeRange, 0);
				const CGmtTime Time2 = MerchData.m_aTicks[i].m_TimeCurrent.m_Time;
				for ( ; i >= 0 ; i-- )
				{
					const CTick &tick = MerchData.m_aTicks[i];
					curSpan = tick.m_TimeCurrent.m_Time - Time2;
					if ( curSpan > Span2  )
					{
						break;
					}
					if ( tick.m_fVolume < 0 )
					{
						continue;		// 无成交数据的分笔跳过
					}
					if ( IsBigVol(fLiuTongGuBen, tick.m_fVolume, Param.m_ParamSub.m_fWhatIsBigVol) )
					{
						iBigVol++;
						if ( iBigVol > Param.m_ParamSub.m_iBigVolOverCount )
						{
							break;
						}
					}
				}
				if ( iBigVol > Param.m_ParamSub.m_iBigVolOverCount )
				{
					// 无需标志
					Result.m_eCondition = EMRC_BigVolInTime;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 涨跌停
			if ( EMRC_None == Result.m_eCondition
				&& (dwConditions&EMRC_RiseFallMax)
				&& MerchData.m_fRiseFallStopRate > 0.0f
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				float fRiseRate = MerchData.m_RealTimePrice.m_fPriceNew/MerchData.m_RealTimePrice.m_fPricePrevClose;
				fRiseRate = fRiseRate - 1.0f;
				float fRiseFallStop = MerchData.m_fRiseFallStopRate - 0.0005;	// 给与一点点偏移
				float fAbs = fRiseRate >= 0.0f ? fRiseRate : - fRiseRate;
				if ( fAbs >= fRiseFallStop )
				{
					Result.m_eCondition = EMRC_RiseFallMax;
					if ( fRiseRate > 0.0f )
					{
						Result.m_iRiseFallFlag = 1;
					}
					else
					{
						Result.m_iRiseFallFlag = -1;
					}
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 涨幅破
			if ( EMRC_None == Result.m_eCondition
				&& ( dwConditions&EMRC_RiseOver )
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				float fRiseRate = MerchData.m_RealTimePrice.m_fPriceNew/MerchData.m_RealTimePrice.m_fPricePrevClose;
				fRiseRate = fRiseRate - 1.0f;
				if ( fRiseRate >= Param.m_ParamSub.m_fRiseOverValue )
				{
					Result.m_eCondition = EMRC_RiseOver;
					Result.m_iRiseFallFlag = 1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 跌幅破
			if ( EMRC_None == Result.m_eCondition
				&& ( dwConditions&EMRC_FallBelow )
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				float fRiseRate = MerchData.m_RealTimePrice.m_fPriceNew/MerchData.m_RealTimePrice.m_fPricePrevClose;
				fRiseRate = fRiseRate - 1.0f;
				if ( -fRiseRate >= Param.m_ParamSub.m_fRiseOverValue )
				{
					Result.m_eCondition = EMRC_FallBelow;
					Result.m_iRiseFallFlag = -1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 巨额
			if ( EMRC_None == Result.m_eCondition
				&& ( EMRC_HugeAmount&dwConditions )
				&& iLastTick >= 0 )
			{
				// 与最后一笔在时差内的都检查一下
				CGmtTime TimeLast = Result.m_TickLast.m_TimeCurrent.m_Time;
				CGmtTimeSpan tmpSpan(0, 0, 1, 0);
				for ( int32 i=iLastTick; i >= 0 ; i-- )
				{
					const CTick &tick = MerchData.m_aTicks[i];
					if ( TimeLast-tick.m_TimeCurrent.m_Time > tmpSpan )
					{
						break;
					}
					
					if ( tick.m_fAmount >= Param.m_ParamSub.m_fHugeAmountValue )
					{
						Result.m_eCondition = EMRC_HugeAmount;
						if ( tick.m_eTradeKind == CTick::ETKBuy )
						{
							Result.m_iRiseFallFlag = -1;
						}
						else if ( CTick::ETKSell == tick.m_eTradeKind )
						{
							Result.m_iRiseFallFlag = 1;
						}
						if ( i < iLastTick )
						{
							iLastTick = i;		// 修改最后一笔分笔数据
							Result.m_TickLast = tick;
						}
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// 大量
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigVol & dwConditions)
				&& iLastTick >= 0
				&& fLiuTongGuBen > 0.0f )
			{
				// 与最后一笔在时差内的都检查一下
				CGmtTime TimeLast = Result.m_TickLast.m_TimeCurrent.m_Time;
				CGmtTimeSpan cTimeSpan(0, 0, 1, 0);
				for ( int32 i=iLastTick; i >= 0 ; i-- )
				{
					const CTick &tick = MerchData.m_aTicks[i];
					if ( TimeLast-tick.m_TimeCurrent.m_Time > cTimeSpan )
					{
						break;
					}

					if ( IsBigVol(fLiuTongGuBen, tick.m_fVolume, Param.m_ParamSub.m_fWhatIsBigVol) )
					{
						if ( tick.m_eTradeKind == CTick::ETKBuy )
						{
							Result.m_iRiseFallFlag = -1;
						}
						else if ( CTick::ETKSell == tick.m_eTradeKind )
						{
							Result.m_iRiseFallFlag = 1;
						}
						Result.m_eCondition = EMRC_BigVol;
						if ( i < iLastTick )
						{
							iLastTick = i;		// 修改最后一笔分笔数据
							Result.m_TickLast = tick;
						}
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// 单笔异常
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_SingleTradeRiseFall & dwConditions)
				&& iLastTick > 0 )
			{
				int32 i=iLastTick-1;
				for ( ; i >= 0 ; i-- )
				{
					if ( MerchData.m_aTicks[i].m_fVolume > 0.0f
						&& MerchData.m_aTicks[i].m_fPrice > 0.0f )
					{
						break;
					}
				}

				if ( i >= 0 )
				{
					float fRiseRate = Result.m_TickLast.m_fPrice/MerchData.m_aTicks[i].m_fPrice - 1;
					if ( MyABS(fRiseRate) >= Param.m_ParamSub.m_fSingleTradeRiseFallValue )
					{
						Result.m_eCondition = EMRC_SingleTradeRiseFall;
						if ( fRiseRate > 0.0f )
						{
							Result.m_iRiseFallFlag = 1;
						}
						else
						{
							Result.m_iRiseFallFlag = -1;
						}
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// 低开长阳
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_LowOpenRise&dwConditions)
				&& iLastTick >= 0 
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				float fRiseRate = (MerchData.m_RealTimePrice.m_fPriceNew-MerchData.m_RealTimePrice.m_fPriceOpen)/MerchData.m_RealTimePrice.m_fPricePrevClose;
				if ( fRiseRate >= 0.3f )
				{
					Result.m_eCondition = EMRC_LowOpenRise;
					Result.m_iRiseFallFlag = 1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 高开长阴
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_HighOpenFall&dwConditions)
				&& iLastTick >= 0 
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				float fRiseRate = (MerchData.m_RealTimePrice.m_fPriceNew-MerchData.m_RealTimePrice.m_fPriceOpen)/MerchData.m_RealTimePrice.m_fPricePrevClose;
				if ( fRiseRate <= -0.3f )
				{
					Result.m_eCondition = EMRC_HighOpenFall;
					Result.m_iRiseFallFlag = -1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 大买挂单
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigBuy & dwConditions)
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				// 判断实时报价中的五档买卖盘
				for (int32 i = 0; i < PRICE_ARRAY_SIZE; i++)
				{
					float fVol = MerchData.m_RealTimePrice.m_astBuyPrices[i].m_fVolume;
					if ( IsBigVol(fLiuTongGuBen, fVol, Param.m_ParamSub.m_fWhatIsBigVol) )
					{
						Result.m_eCondition = EMRC_BigBuy;
						Result.m_iRiseFallFlag = 0;
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// 大卖挂单
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigSell & dwConditions)
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				// 判断实时报价中的五档买卖盘
				for (int32 i = 0; i < PRICE_ARRAY_SIZE; i++)
				{
					float fVol = MerchData.m_RealTimePrice.m_astSellPrices[i].m_fVolume;
					if ( IsBigVol(fLiuTongGuBen, fVol, Param.m_ParamSub.m_fWhatIsBigVol) )
					{
						Result.m_eCondition = EMRC_BigSell;
						Result.m_iRiseFallFlag = 0;
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// 今日新高
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_TodayNewHigh &dwConditions)
				&& iLastTick >= 2 		// 必须要大于一定笔数
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				if ( MerchData.m_RealTimePrice.m_fPriceNew >= MerchData.m_RealTimePrice.m_fPriceHigh
					&& MerchData.m_RealTimePrice.m_fPriceNew > MerchData.m_RealTimePrice.m_fPricePrevClose )
				{
					// 只有可能是等于 && 涨
					Result.m_eCondition = EMRC_TodayNewHigh;
					Result.m_iRiseFallFlag = 1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 今日新低
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_TodayNewLow &dwConditions)
				&& iLastTick >= 2 		// 必须要大于一定笔数 
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				if ( MerchData.m_RealTimePrice.m_fPriceNew <= MerchData.m_RealTimePrice.m_fPriceLow
					&& MerchData.m_RealTimePrice.m_fPriceNew < MerchData.m_RealTimePrice.m_fPricePrevClose )
				{
					// 只有可能是等于 && 跌
					Result.m_eCondition = EMRC_TodayNewLow;
					Result.m_iRiseFallFlag = -1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// 判断完毕
			if ( Result.m_eCondition != EMRC_None )
			{
				mapResults[ Result.m_pMerch ] = Result;
			}
			else
			{
				pCalcObj->RemoveResult(Result.m_pMerch, CMarketRadarListener::EMC_NotSatisfyCondition);	// 该商品没有满足条件了，
			}
		}

		if ( mapResults.GetCount() > 0 )
		{
			pCalcObj->AddResult(mapResults);
		}
	}

	TRACE(_T("雷达计算线程(%d)退出~~\n"), GetCurrentThreadId());
	return 0;
}

bool32 CMarketRadarCalc::RequestStartViewData()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

#ifdef _DEBUG
	FILETIME ft1, ft2, ftCreate, ftExit;
	BOOL bFt = GetThreadTimes(GetCurrentThread(), &ftCreate, &ftExit, &ft1,& ft2);
	ASSERT( bFt );
#endif

	OnTimerFilterNeedInitMerch(NULL);	// 清除不符合时间要求的商品参数与结果数据

	// 由于遍历一次大的市场范围需要n长时间，所以看哪些商品有实时报价的
	// 可能是前面访问过的商品，先处理这些商品
	MerchArray aMerchs;
	aMerchs.Copy(m_Param.m_aMerchs);	// 运行期间参数不可能变更
	int32 iUp = aMerchs.GetUpperBound();
	int32 iDown = 0;
	for ( ; iDown < iUp ; ++iDown,--iUp )
	{
		if ( aMerchs[iDown]->m_pRealtimePrice != NULL )	// 异步仅读取下指针
		{
			continue;
		}
		while ( iUp > iDown && aMerchs[iUp]->m_pRealtimePrice == NULL )
		{
			--iUp;
		}
		if ( iDown < iUp )
		{
			CMerch *pMerch = aMerchs[iDown];
			aMerchs[iDown] = aMerchs[iUp];
			aMerchs[iUp] = pMerch;
		}
	}

	typedef set<CMerch *> MerchSet;
	MerchSet merchExist;
	int32 i = 0;
	for ( i=0; i < aMerchs.GetSize() ; i++ )
	{
		MerchSet::_Pairib itb = merchExist.insert(aMerchs[i]);
		ASSERT( itb.second );
	}

	CSingleLock lock(&m_LockReq, TRUE);
//	m_aMerchsWaitReq.Copy(aMerchs);
	// 现有请求队列中有的商品保留，没有的删除，新的商品附加到尾部
	// 效率很低，不过线程优先级为belownormal~~，应该不会卡主线程
	m_aMerchsWaitReq.SetSize(m_aMerchsWaitReq.GetSize(), m_aMerchsWaitReq.GetSize()+aMerchs.GetSize());
	for ( i=m_aMerchsWaitReq.GetUpperBound(); i >=0 ; --i )
	{
		MerchSet::iterator it = merchExist.find(m_aMerchsWaitReq[i]);
		if ( it != merchExist.end() )
		{
			merchExist.erase(it);	// 存在就删除这个商品的存在记录 
		}
		else
		{
			m_aMerchsWaitReq.RemoveAt(i);	// 删除这个点
		}
	}
	for ( i=0 ; i < aMerchs.GetSize() ; i++ )
	{
		MerchSet::iterator it = merchExist.find(aMerchs[i]);
		if ( it != merchExist.end() )
		{
			merchExist.erase(it);	// 存在就删除
			m_aMerchsWaitReq.Add(aMerchs[i]);	// 添加进请求队列
		}
		else
		{
			// 已经存在在请求队列中了
		}
	}

	m_aMerchsWaitEcho.RemoveAll();
	lock.Unlock();

#ifdef _DEBUG
	FILETIME ft11, ft22;
	GetThreadTimes(GetCurrentThread(), &ftCreate, &ftExit, &ft11, &ft22);
	LARGE_INTEGER lgK, lgU, lgK2, lgU2;
	lgK.LowPart = ft1.dwLowDateTime;
	lgK.HighPart = ft1.dwHighDateTime;
	lgU.LowPart = ft2.dwLowDateTime;
	lgU.HighPart = ft2.dwHighDateTime;
	lgK2.LowPart = ft11.dwLowDateTime;
	lgK2.HighPart = ft11.dwHighDateTime;
	lgU2.LowPart = ft22.dwLowDateTime;
	lgU2.HighPart = ft22.dwHighDateTime;
	lgK.QuadPart = (lgK2.QuadPart-lgK.QuadPart)/10000;
	lgU.QuadPart = (lgU2.QuadPart-lgU.QuadPart)/10000;
	TRACE(_T("市场雷达准备请求数据：%d ms\r\n"), lgK.LowPart+lgU.LowPart);
#endif

	return RequestNextViewData();
}

bool32 CMarketRadarCalc::RequestNextViewData()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	// TODO: 请求过滤优化

	const int32 iReqDataCount = RADAR_MERCHCOUNT_PRE_REQ;
	const CGmtTimeSpan Span(0, 0, 1, 0) ;		// 若干时间内的发送过请求的不再发送请求
	CGmtTime TimeNow = GetNow();
	MerchArray	aMerchs;

	CSingleLock lock(&m_LockReq, TRUE);
	if ( m_aMerchsWaitReq.GetSize() <= 0 )
	{
		return false;	// 没有请求数据了
	}
	if ( m_aMerchsWaitEcho.GetSize() > 0 )
	{
		// 有数据在等待队列，不发送请求
		// 一般状态下，这些数据在这么长时间内应该都回来了，没回来可能是中间断开了服务啊什么的
		// 也有可能是根本请求的商品根本就没有合适的数据返回
		// 再次请求等待应答队列中数据
		m_aMerchsWaitEcho.RemoveAll();  // 暂时抛弃
		//return TryRequestWaitEchoData();
	}
	
	// 优先选择本地没有结果缓存的数据发送
	int32 iReqSize = m_aMerchsWaitReq.GetSize();
	const int32 iSize = min(iReqDataCount, iReqSize);

	aMerchs.SetSize(0, iSize);
	int32 iReqPos=0;
	for ( ; aMerchs.GetSize() < iSize && iReqPos < m_aMerchsWaitReq.GetSize() ; ++iReqPos )
	{
		CMerch *pMerch = m_aMerchsWaitReq[iReqPos];
		MerchTimeMap::iterator it = m_mapMerchReqTimes.find(pMerch);
		if ( it == m_mapMerchReqTimes.end()
			|| TimeNow-it->second >= Span)
		{
			// 没有请求过的商品 或者超过最小请求间隔的
			// 是否要做开收盘时间段过滤？上次请求与这次请求在同一个开收盘时间段内
			aMerchs.Add(pMerch);
			m_mapMerchReqTimes[pMerch] = TimeNow;	// 更新请求时间
		}
	}
	// 移除比较过的商品
	m_aMerchsWaitReq.RemoveAt(0, iReqPos);

	m_aMerchsWaitEcho.Copy(aMerchs);
	lock.Unlock();

	return RequestMerchData(aMerchs) == iSize;		// 数量不够证明可以休息比较长时间了
}

bool32 CMarketRadarCalc::TryRequestWaitEchoData()
{
	MerchArray aMerchs;
	CSingleLock lock(&m_LockReq, TRUE);
	if ( m_aMerchsWaitEcho.GetSize() <= 0 )
	{
		return false;
	}
	aMerchs.Copy(m_aMerchsWaitEcho);
	lock.Unlock();

	return RequestMerchData(aMerchs) > 0;
}

int32	CMarketRadarCalc::RequestMerchData(const MerchArray &aMerchs)
{
	if ( CGGTongApp::m_bOffLine )
	{
		return aMerchs.GetSize();	// 脱机情况下就不要发送数据了，监测个毛呀
	}

	if ( NULL == m_pAbsCenterManager
		|| aMerchs.GetSize() <= 0 )
	{
		return 0;
	}

	// 通过dataManager发送请求
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc
		|| NULL == pDoc->m_pAbsDataManager )
	{
		ASSERT( 0 );
		return 0;
	}

	// 发送实时报价请求
	// 这里的都是异步发送，商品数据中id与code都是初始化就不在变动的，如果变动这里要做处理
	const int32 iSize = aMerchs.GetSize();
	CMmiReqRealtimePrice reqReal;
	if ( iSize > 0 )
	{
		reqReal.m_iMarketId = aMerchs[0]->m_MerchInfo.m_iMarketId;
		reqReal.m_StrMerchCode = aMerchs[0]->m_MerchInfo.m_StrMerchCode;
	}

	int32 i;
	for ( i=1; i < iSize ; i++ )
	{
		CMerchKey key;
		key.m_iMarketId		= aMerchs[i]->m_MerchInfo.m_iMarketId;
		key.m_StrMerchCode	= aMerchs[i]->m_MerchInfo.m_StrMerchCode;
		reqReal.m_aMerchMore.Add(key);
	}
	//m_pAbsCenterManager->RequestViewData(&reqReal);
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	pDoc->m_pAbsDataManager->RequestData(&reqReal, aMmiReqNodes);
	
	// 发送商品的分笔数据请求，	 超出现在时间的简单保证最新~~
	CGmtTime TimeNow = GetNow() + CGmtTimeSpan(0, 1, 0, 0);
	for ( i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchs[i];
		
		CMmiReqMerchTimeSales info;
		info.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType	= ERTYFrontCount;
		info.m_TimeSpecify	= TimeNow;
		info.m_iFrontCount  = RADAR_TIMESALE_COUNT;		// 固定请求若干
		
		//m_pAbsCenterManager->RequestViewData(&info);
		pDoc->m_pAbsDataManager->RequestData(&info, aMmiReqNodes);
		if ( info.m_iFrontCount <= 0 )
		{
			TRACE(_T("!!!分笔请求错误: id=%d code=%s time=%d count=0\r\n"), info.m_iMarketId, info.m_StrMerchCode.GetBuffer(), TimeNow.GetTime());
		}
	}

	return iSize;
}

#define IsValidCalcMerchData(MerchData) ( MerchData.m_aTicks.GetSize() > 0 \
		&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f \
		&& MerchData.m_RealTimePrice.m_fVolumeTotal > 0.0f  )

bool32 CMarketRadarCalc::GetNextNewCalcMerchData( OUT T_CalcMerchData &MerchData, bool32 bPop/*=true*/ )
{
	// 使用旗语
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);

	bool32 bRet = false;
	POSITION pos = m_mapCalcMerchDataNew.GetStartPosition();
	while ( NULL != pos )
	{
		CMerch *pMerch = NULL;
		m_mapCalcMerchDataNew.GetNextAssoc(pos, pMerch, MerchData);
		if ( IsValidCalcMerchData(MerchData) )
		{
			m_mapCalcMerchDataNew.RemoveKey(MerchData.m_pMerch);
			const int iCount = m_mapCalcMerchDataNew.GetCount();
			if ( 0 == iCount )
			{
				m_EventNewMerchData.ResetEvent();	// 空了
			}
			bRet = true;
			break;
		}
		else
		{
			ASSERT( 0 );
			m_mapCalcMerchDataNew.RemoveKey(pMerch);
			pos = m_mapCalcMerchDataNew.GetStartPosition();
		}
	}
	if ( !bRet )
	{
		// 等要取的时候发现没有数据鸟~
		m_EventNewMerchData.ResetEvent();	// 已经没有有效数据了
	}
	lock.Unlock();

	

	return bRet;
}

void CMarketRadarCalc::ClearNewCalcMerchData()
{
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);
	m_mapCalcMerchDataNew.RemoveAll();
	m_EventNewMerchData.ResetEvent();
}


void CMarketRadarCalc::DoAddNewCalcMerchData( INOUT T_CalcMerchData &MerchData )
{
	CMerch *pMerch = MerchData.m_pMerch;
	ASSERT( NULL != pMerch );
	if ( NULL == pMerch )
	{
		return;
	}

	// 计算其涨跌停幅度
	if ( MerchData.m_fRiseFallStopRate == 0.0f )
	{
		E_ReportType eRT = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
		if ( ERTStockCn == eRT
			|| ERTExp == eRT )
		{
			if ( 'N' != pMerch->m_MerchInfo.m_StrMerchCnName[0] )// 新股 不用计算涨跌停, 理解为无涨跌停
			{
				//
				if ( -1 != pMerch->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
				{
					MerchData.m_fRiseFallStopRate = 0.05f;
				}
				else
				{
					MerchData.m_fRiseFallStopRate = 0.1f;	// 10%
				}
			}
		}
	}

	// 计算今天或前面的初始化时间
	CMarket *pMarket = &pMerch->m_Market;
	CMarketIOCTimeInfo IOC;
	CGmtTime TimeNow = GetNow();
	if ( pMarket->GetRecentTradingDay(GetNow(), IOC, pMerch->m_MerchInfo) )	// 希望这个函数没有同步问题！！
	{
		ASSERT( TimeNow >= IOC.m_TimeInit.m_Time );
		// 必然是今天或以前
		MerchData.m_TimeInit = IOC.m_TimeInit.m_Time;
	}

	m_mapCalcMerchDataAll[ pMerch ] = MerchData;
}


void CMarketRadarCalc::AddCalcMerchData( CMerch *pMerch, const CRealtimePrice &RealTimePrice )
{
	if ( NULL == pMerch )
	{
		return;
	}
	// 有新的实时报价数据
	bool32 bAddNew = false;
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);
	T_CalcMerchData MerchData;
	if ( m_mapCalcMerchDataAll.Lookup(pMerch, MerchData) )
	{
		if ( IsMerchDataExpire(MerchData) )
		{
			// 超时了的数据，必须要清空缓存
			m_mapCalcMerchDataAll.RemoveKey(pMerch);
			m_mapCalcMerchDataNew.RemoveKey(pMerch);
			if ( m_mapCalcMerchDataNew.IsEmpty() )
			{
				m_EventNewMerchData.ResetEvent();
			}
			// 重新加入
			T_CalcMerchData MerchData2;
			MerchData2.m_pMerch = pMerch;
			MerchData2.m_RealTimePrice = RealTimePrice;
			DoAddNewCalcMerchData(MerchData2);
		}
		else
		{
			// 更新数据
			MerchData.m_RealTimePrice = RealTimePrice;
			m_mapCalcMerchDataAll[ pMerch ] = MerchData;	
			if ( IsValidCalcMerchData(MerchData) )
			{
				m_mapCalcMerchDataNew[pMerch] = MerchData;
				bAddNew = true;
				// 清除等待队列中的数据
				CSingleLock lock2(&m_LockReq, TRUE);
				for ( int32 i=m_aMerchsWaitEcho.GetUpperBound(); i >= 0 ; i-- )
				{
					if ( m_aMerchsWaitEcho[i] == pMerch )
					{
						m_aMerchsWaitEcho.RemoveAt(i);
					}
				}
				lock2.Unlock();
				m_EventNewMerchData.SetEvent();
			}
		}
	}
	else
	{
		MerchData.m_pMerch = pMerch;
		MerchData.m_RealTimePrice = RealTimePrice;

		// 加入缓存，继续等待
		DoAddNewCalcMerchData(MerchData);
	}

	lock.Unlock();
}

void CMarketRadarCalc::AddCalcMerchData( CMerch *pMerch, const TickArray &aTicks )
{
	if ( NULL == pMerch )
	{
		return;
	}
	// 有新的分笔数据
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);
	T_CalcMerchData MerchData;
	if ( m_mapCalcMerchDataAll.Lookup(pMerch, MerchData) )
	{
		if ( IsMerchDataExpire(MerchData) )
		{
			// 超时了的数据，必须要清空缓存
			m_mapCalcMerchDataAll.RemoveKey(pMerch);
			m_mapCalcMerchDataNew.RemoveKey(pMerch);
			if ( m_mapCalcMerchDataNew.IsEmpty() )
			{
				m_EventNewMerchData.ResetEvent();
			}
			// 重新加入
			T_CalcMerchData MerchData2;
			MerchData2.m_pMerch = pMerch;
			MerchData2.m_aTicks.Copy(aTicks);
			DoAddNewCalcMerchData(MerchData2);
		}
		else
		{
			MerchData.m_aTicks.Copy(aTicks);
			m_mapCalcMerchDataAll[ pMerch ] = MerchData;
			if ( IsValidCalcMerchData(MerchData) )
			{
				m_mapCalcMerchDataNew[pMerch] = MerchData;
				// 清除等待队列中的数据
				CSingleLock lock2(&m_LockReq, TRUE);
				for ( int32 i=m_aMerchsWaitEcho.GetUpperBound(); i >= 0 ; i-- )
				{
					if ( m_aMerchsWaitEcho[i] == pMerch )
					{
						m_aMerchsWaitEcho.RemoveAt(i);
					}
				}
				lock2.Unlock();
				m_EventNewMerchData.SetEvent();
			}
		}
	}
	else
	{
		MerchData.m_aTicks.Copy(aTicks);
		MerchData.m_pMerch = pMerch;
		DoAddNewCalcMerchData(MerchData);
	}

	lock.Unlock();
}

bool32 CMarketRadarCalc::AddResult( const T_MarketRadarResult &Result )
{
	if ( Result.m_pMerch == NULL
		|| Result.m_eCondition == 0 )
	{
		ASSERT( 0 );
		return false;
	}

	MarketRadarResultMap mapResults;
	mapResults[ Result.m_pMerch ] = Result;
	AddResult(mapResults);

	return true;
}

int32 CMarketRadarCalc::AddResult( const MarketRadarResultMap &Results )
{
	if ( Results.GetCount() <= 0 )
	{
		return 0;
	}

	MarketRadarResultMap ResultUpdate, ResultEqual;

	CSingleLock lock(&m_LockResult, TRUE);
	POSITION pos = Results.GetStartPosition();
	while ( NULL != pos )
	{
		CMerch *pMerch;
		T_MarketRadarResult Result;
		Results.GetNextAssoc(pos, pMerch, Result);

		T_MarketRadarResult Result2;
		if ( m_mapResult.Lookup(pMerch, Result2)
			&& Result2.m_eCondition == Result.m_eCondition
			&& Result2.m_iRiseFallFlag == Result.m_iRiseFallFlag )
		{
			ResultEqual[pMerch] = Result;
		}
		else
		{
			ResultUpdate[pMerch] = Result;
		}
		m_mapResult[ pMerch ] = Result;
	}
	lock.Unlock();

	FireResultAdd(ResultUpdate, CMarketRadarListener::EAC_UpdateCondition);	
	FireResultAdd(ResultEqual,  CMarketRadarListener::EAC_EqualCondition);	

	return Results.GetCount();
}

void CMarketRadarCalc::ClearResults(CMarketRadarListener *pIgnoreListener/* = NULL*/)
{
	CSingleLock lock(&m_LockResult, TRUE);
	m_mapResult.RemoveAll();
	lock.Unlock();

	FireResultClear(pIgnoreListener);
}

void CMarketRadarCalc::RemoveResult( CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener/* = NULL*/ )
{
	CSingleLock lock(&m_LockResult, TRUE);
	BOOL bExist = m_mapResult.RemoveKey(pMerch);
	lock.Unlock();

	if ( bExist )
	{
		FireResultRemove(pMerch, eCause, pIgnoreListener);
	}
}

void CMarketRadarCalc::OnDataRespMerchTimeSales( int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales )
{
	if ( NULL == pMmiRespMerchTimeSales
		|| NULL == m_pAbsCenterManager )
	{
		return;
	}

	CMerch *pMerch = NULL;
	if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode, pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId, pMerch) )
	{
		return;
	}

	// 现在pMerch中必然已经处理好了分笔序列，直接使用pMerch中的最新n笔数据
	// 这里返回的可能不是与请求对应得到的
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CMarketIOCTimeInfo IOCTime;
	if ( !pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo) )
	{
		return;
	}

	// 最仅成交日期, 必是今天或者以前的天, 寻找最近交易日的分笔数据,
	// 依赖于这些数据是顺序且含最新的
	if ( NULL == pMerch->m_pMerchTimeSales )
	{
		return;
	}

	int32 iSize = pMerch->m_pMerchTimeSales->m_Ticks.GetSize();
	if ( iSize <= 0 )
	{
		return;
	}
	int32 iPosDayStart = CMerchTimeSales::QuickFindTickWithBigOrEqualReferTime(pMerch->m_pMerchTimeSales->m_Ticks, IOCTime.m_TimeInit);
	if (iPosDayStart < 0 || iPosDayStart >= pMerch->m_pMerchTimeSales->m_Ticks.GetSize())
		return;
	
	int32 iPosDayEnd = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(pMerch->m_pMerchTimeSales->m_Ticks, IOCTime.m_TimeEnd);
	if (iPosDayEnd < 0 || iPosDayEnd >= pMerch->m_pMerchTimeSales->m_Ticks.GetSize())
		return;
	
	if (iPosDayEnd < iPosDayStart)
	{
		return;
	}

	iSize = iPosDayEnd - iPosDayStart +1;
	TickArray aTicks;
	aTicks.SetSize(iSize);
	memcpyex(aTicks.GetData(), pMerch->m_pMerchTimeSales->m_Ticks.GetData()+iPosDayStart, iSize*sizeof(CTick));

	AddCalcMerchData(pMerch, aTicks);
}

void CMarketRadarCalc::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{
	CMerch *pMerch = NULL;
	if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch) )
	{
		return;
	}
	
	// 成交量数据要初始化为手，保持与viewdata中一致
	CRealtimePrice RealTime = RealtimePrice;
	RealTime.ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);
	
	AddCalcMerchData(pMerch, RealTime);
}

bool32 CMarketRadarCalc::IsAttendCommType( E_CommType eType )
{
	if ( ECTReqRealtimePrice == eType
		|| ECTReqMerchTimeSales == eType )
	{
		return true;
	}
	return false;
}

CMarketRadarCalc::CMarketRadarCalc()
:m_EventNewMerchData(FALSE, TRUE)
//, m_SemaphoreMerchData(0, INT_MAX)
{
	m_hThreadCalc = m_hThreadReq = NULL;

	m_lRunFlag = 0;
	m_pAbsCenterManager = NULL;

	m_dwTimeCalcStart = 0;
}

CMarketRadarCalc::~CMarketRadarCalc()
{
	StopCalc();
}

CMarketRadarCalc & CMarketRadarCalc::Instance()
{
	static CMarketRadarCalc calc;
	// 尝试获取viewdata
	if ( NULL == calc.m_pAbsCenterManager )
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		if ( NULL != pDoc )
		{
			calc.m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
		}
	}
	return calc;
}

const TCHAR KStrXmlRadarSection[] = _T("MarketRadar");		// 段名
const TCHAR KStrXmlRadarConditions[]				= _T("Conditions");		// 条件
const TCHAR KStrXmlRadarWhatIsBigVol[]				= _T("WhatIsBigVol");	// 什么是大量
const TCHAR KStrXmlRadarRiseOverValue[]				= _T("RiseOverValue");	// 涨幅破
const TCHAR KStrXmlRadarFallBelowValue[]			= _T("FallBelowValue");	// 跌幅破
const TCHAR KStrXmlRadarSingleTradeRiseFallValue[]  = _T("SingleTradeRiseFallValue");	// 单笔异常
const TCHAR KStrXmlRadarHugeAmountValue[]  = _T("HugeAmountValue");	// 巨额
const TCHAR KStrXmlRadarBigVolTimeRange[]  = _T("BigVolTimeRange");	// x分中连续成交
const TCHAR KStrXmlRadarBigVolOverCount[]  = _T("BigVolOverCount");	// x笔
const TCHAR KStrXmlRadarVoiceType[]  = _T("VoiceType");	// 报警声音类别
const TCHAR KStrXmlRadarVoiceFilePath[]  = _T("VoiceFilePath");	// 用户选择的文件
const TCHAR KStrXmlRadarShowDlgListAlarm[]  = _T("ShowDlgListAlarm");	// 显示列表框
const TCHAR KStrXmlRadarMerchRange[]  = _T("MerchRange");	// 选择的商品类型

const TCHAR KStrXmlRadarStart[]		=	_T("Start");	// 是否需要启动
#ifdef _UNICODE
#define TToF(TStrValue, fValue, fValueDefault) (_stscanf(TStrValue, _T("%f"), &fValue) == 1 ? fValue : fValueDefault)
										  
#else
#define TToF(TStrValue, fValue, fValueDefault) atof(TStrValue)
#endif

void CMarketRadarCalc::ReadInitParam()
{
	CString StrValue;
	
	T_MarketRadarParameter &Param = m_Param;
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarConditions, NULL, StrValue) )
	{
		Param.m_ParamSub.m_dwConditions = (DWORD)_ttol(StrValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarWhatIsBigVol, NULL, StrValue) )
	{
		Param.m_ParamSub.m_fWhatIsBigVol = TToF(StrValue, Param.m_ParamSub.m_fWhatIsBigVol, Param.m_ParamSub.m_fWhatIsBigVol);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarRiseOverValue, NULL, StrValue) )
	{
		Param.m_ParamSub.m_fRiseOverValue = TToF(StrValue, Param.m_ParamSub.m_fRiseOverValue, Param.m_ParamSub.m_fRiseOverValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarFallBelowValue, NULL, StrValue) )
	{
		Param.m_ParamSub.m_fFallBelowValue = TToF(StrValue, Param.m_ParamSub.m_fFallBelowValue, Param.m_ParamSub.m_fFallBelowValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarSingleTradeRiseFallValue, NULL, StrValue) )
	{
		Param.m_ParamSub.m_fSingleTradeRiseFallValue = TToF(StrValue, Param.m_ParamSub.m_fSingleTradeRiseFallValue, Param.m_ParamSub.m_fSingleTradeRiseFallValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarHugeAmountValue, NULL, StrValue) )
	{
		Param.m_ParamSub.m_fHugeAmountValue = TToF(StrValue, Param.m_ParamSub.m_fHugeAmountValue, Param.m_ParamSub.m_fHugeAmountValue);
	}
	
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarBigVolTimeRange, NULL, StrValue) )
	{
		Param.m_ParamSub.m_iBigVolTimeRange = _ttol(StrValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarBigVolOverCount, NULL, StrValue) )
	{
		Param.m_ParamSub.m_iBigVolOverCount = _ttol(StrValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarVoiceType, NULL, StrValue) )
	{
		Param.m_ParamSub.m_eVoiceType = (E_MarketRadarAlarmVoiceType)_ttol(StrValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarVoiceFilePath, NULL, StrValue) )
	{
		Param.m_ParamSub.m_StrVoiceFilePath = StrValue;
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarShowDlgListAlarm, NULL, StrValue) )
	{
		Param.m_ParamSub.m_bShowDlgListAlarm = 0 != _ttol(StrValue);
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarMerchRange, NULL, StrValue) )
	{
		Param.m_eMerchRange = (E_MarketRadarMerchRange)_ttol(StrValue);
	}

	switch ( Param.m_eMerchRange )
	{
	case EMRMR_HSA:
		{
			// 等到开始服务时取沪深A股列表
		}
		break;
	case EMRMR_ZX:
		{
			// 等到服务时取自选股信息，并关注变更？
		}
		break;
	case EMRMR_MerchArray:
		{
			// 读取选择的商品信息
			ReadMerchArray(Param.m_aMerchs);
		}
		break;
	}

	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXmlRadarSection, KStrXmlRadarStart, NULL, StrValue) )
	{
		if ( 0 != _ttol(StrValue) )
		{
			m_lRunFlag = 1;
		}
	}
}

void CMarketRadarCalc::SaveInitParam()
{
	CString StrValue;

	StrValue.Format(_T("%d"), m_Param.m_ParamSub.m_dwConditions);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarConditions, StrValue);

	StrValue.Format(_T("%f"), m_Param.m_ParamSub.m_fWhatIsBigVol);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarWhatIsBigVol, StrValue);

	StrValue.Format(_T("%f"), m_Param.m_ParamSub.m_fRiseOverValue);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarRiseOverValue, StrValue);

	StrValue.Format(_T("%f"), m_Param.m_ParamSub.m_fFallBelowValue);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarFallBelowValue, StrValue);

	StrValue.Format(_T("%f"), m_Param.m_ParamSub.m_fSingleTradeRiseFallValue);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarSingleTradeRiseFallValue, StrValue);

	StrValue.Format(_T("%f"), m_Param.m_ParamSub.m_fHugeAmountValue);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarHugeAmountValue, StrValue);

	StrValue.Format(_T("%d"), m_Param.m_ParamSub.m_iBigVolTimeRange);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarBigVolTimeRange, StrValue);

	StrValue.Format(_T("%d"), m_Param.m_ParamSub.m_iBigVolOverCount);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarBigVolOverCount, StrValue);

	StrValue.Format(_T("%d"), m_Param.m_ParamSub.m_eVoiceType);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarVoiceType, StrValue);

	StrValue = m_Param.m_ParamSub.m_StrVoiceFilePath;
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarVoiceFilePath, StrValue);

	StrValue.Format(_T("%d"), (DWORD)m_Param.m_ParamSub.m_bShowDlgListAlarm);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarShowDlgListAlarm, StrValue);

	StrValue.Format(_T("%d"), m_Param.m_eMerchRange);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarMerchRange, StrValue);

	SaveStartFlag();
	if ( m_Param.m_eMerchRange == EMRMR_MerchArray )
	{
		SaveMerchArray(m_Param.m_aMerchs);
	}
}

void CMarketRadarCalc::SaveStartFlag()
{
	CString StrValue;
	StrValue.Format(_T("%d"), m_lRunFlag);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXmlRadarSection, KStrXmlRadarStart, StrValue);
}

struct T_MarketRadarMerchKey
{
	int32	iMarketId;
	TCHAR	szMerchCode[16];
};

CString GetMarketRadarResultString(const T_MarketRadarResult &Result)
{
	static CStringArray saNames;
	if ( saNames.GetSize() <= 0 )
	{
		saNames.Add(_T("今日新高"));
		saNames.Add(_T("今日新低"));
		saNames.Add(_T("大买挂单"));
		saNames.Add(_T("大卖挂单"));
		saNames.Add(_T("低开长阳"));
		saNames.Add(_T("高开长阴"));
		saNames.Add(_T("价幅突破"));
		saNames.Add(_T("价幅跌破"));
		saNames.Add(_T("单笔异涨"));
		saNames.Add(_T("巨额成交"));
		saNames.Add(_T("大量成交"));
		saNames.Add(_T("涨停预警"));
		saNames.Add(_T("连续大量"));
	}
	CString Str;
	for ( int32 i=0; i < saNames.GetSize() ; i++ )
	{
		if ( ((Result.m_eCondition)>>(i))&1 )
		{
			Str = saNames[i];
			break;
		}
	}

	switch (Result.m_eCondition)
	{
	case EMRC_RiseFallMax:
		if ( Result.m_iRiseFallFlag < 0 )
		{
			Str = _T("跌停预警");
		}
		break;
	case EMRC_SingleTradeRiseFall:
		if ( Result.m_iRiseFallFlag < 0 )
		{
			Str = _T("单笔异跌");
		}
		break;
	}

	return Str;	// 名称修正 TODO
}

const TCHAR KStrRadarMerchFileName[] = _T("MerchRange.dat");	// 商品列表数据
void CMarketRadarCalc::SaveMerchArray( const MerchArray &aMerchs )
{
	CString StrFilePath = CPathFactory::GetSaveConfigFileName(KStrRadarMerchFileName);
	if ( aMerchs.GetSize() <= 0 )
	{
		::DeleteFile(StrFilePath);	// 删除该文件
	}
	else
	{
		CStdioFile file;
		if ( file.Open(StrFilePath, CFile::typeBinary |CFile::modeCreate |CFile::modeWrite) )
		{
			// 写入头部信息
			const DWORD dwSize = sizeof(T_MarketRadarMerchKey);
			file.Write(&dwSize, sizeof(dwSize));

			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				CMerch *pMerch = aMerchs[i];
				if ( NULL != pMerch )
				{
					T_MarketRadarMerchKey key;
					ZeroMemory(&key, sizeof(key));
					key.iMarketId = pMerch->m_MerchInfo.m_iMarketId;
					_tcsncpy(key.szMerchCode, pMerch->m_MerchInfo.m_StrMerchCode, sizeof(key.szMerchCode)/sizeof(key.szMerchCode[0])-1);

					file.Write(&key, dwSize);
				}
			}
			file.Close();
		}
	}
}

void CMarketRadarCalc::ReadMerchArray( OUT MerchArray &aMerchs )
{
	CString StrFilePath;
	aMerchs.RemoveAll();

	CAbsCenterManager *pAbsCenterManager = NULL;
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL != pDoc )
	{
		pAbsCenterManager = pDoc->m_pAbsCenterManager;
	}
	if ( NULL == pAbsCenterManager )
	{
		ASSERT( 0 );
		return;
	}

	if ( CPathFactory::GetExistConfigFileName(StrFilePath, KStrRadarMerchFileName) )
	{
		CStdioFile file;
		if ( file.Open(StrFilePath, CFile::typeBinary |CFile::modeRead) )
		{
			// 头部信息
			DWORD dwSize = 0;
			DWORD dwRead = 0;
			dwRead = file.Read(&dwSize, sizeof(dwSize));
			if ( dwRead != sizeof(dwSize)
				|| dwSize != sizeof(T_MarketRadarMerchKey) )
			{
				file.Close();
				return;
			}

			T_MarketRadarMerchKey key;
			while ( (dwRead=file.Read(&key, dwSize)) == dwSize )
			{
				CMerch *pMerch = NULL;
				if ( pAbsCenterManager->GetMerchManager().FindMerch(key.szMerchCode, key.iMarketId, pMerch) )
				{
					aMerchs.Add(pMerch);	// 不过滤重复
				}
			}
			file.Close();
		}
	}
}

void CMarketRadarCalc::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	if ( BlockModified == eNotifyType
		&& m_Param.m_eMerchRange == EMRMR_ZX )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		if ( NULL == pBlock
			|| !pBlock->IsValidBlock() )
		{
			return;
		}

		T_Block *pBlockUser = CUserBlockManager::Instance()->GetServerBlock();
		if ( NULL == pBlockUser
			|| (pBlockUser->m_StrName != pBlock->m_blockInfo.m_StrBlockName)
			)
		{
			return;
		}

		if ( m_Param.m_aMerchs.GetSize() != pBlock->m_blockInfo.m_aSubMerchs.GetSize()
			|| memcmp(m_Param.m_aMerchs.GetData(), pBlock->m_blockInfo.m_aSubMerchs.GetData(), sizeof(CMerch *) * m_Param.m_aMerchs.GetSize())!=0 )
		{
			// 商品变更
			typedef set<CMerch *> MerchSet;
			MerchSet merchKeys, merchDels;
			int32 i = 0;
			for ( i=0; i < pBlockUser->m_aMerchs.GetSize() ; i++ )
			{
				merchKeys.insert(pBlockUser->m_aMerchs[i]);
			}
			for ( i=0; i < m_Param.m_aMerchs.GetSize() ; i++ )
			{
				if ( merchKeys.erase(m_Param.m_aMerchs[i]) <= 0 )
				{
					merchDels.insert(m_Param.m_aMerchs[i]);	// 现在没有的就是要从请求队列中删除的
				}
			}
			m_Param.m_aMerchs.Copy(pBlockUser->m_aMerchs);	// 更新参数中的商品数组

			// 对于新增的商品添加到请求队列
			// 删除的商品从现有请求队列中删除，不处理该商品的计算结果
			if ( merchKeys.size() > 0 )
			{
				CSingleLock lock(&m_LockReq, TRUE);
				for ( i=m_aMerchsWaitReq.GetUpperBound(); i >= 0 ; --i )
				{
					if ( merchDels.count(m_aMerchsWaitReq[i]) )
					{
						m_aMerchsWaitReq.RemoveAt(i);
					}
				}
				int32 iAdded = 0;
				for ( MerchSet::iterator it=merchKeys.begin(); it != merchKeys.end() ; ++it )
				{
					if ( *it != NULL )
					{
						m_aMerchsWaitReq.Add( *it );
						iAdded++;
					}
				}
				lock.Unlock();
				if ( iAdded > 0 )
				{
					RequestNextViewData();		// 如果碰到频繁的添加会导致频繁的请求，优化TODO
				}
			}
		}
	}
}

bool32 CMarketRadarCalc::InitAndStartIf()
{
	ReadInitParam();
	if ( m_lRunFlag != 0 )
	{
		return StartCalc();
	}
	return true;
}

bool32 CMarketRadarCalc::SetParameter( const T_MarketRadarParameter &Param )
{
	// 任何参数的变更都会导致重新请求&计算的发生(如果当前在运行)
	if ( !m_Param.CompareCalcParam(Param) )
	{
		// 停止运行
		LONG lRun = m_lRunFlag;
		StopCalc();
		
		m_Param = Param;
		SaveInitParam();	// 保存参数, 仅在此时机保存
		
		if ( lRun != 0 )
		{
			StartCalc();
		}
	}
	else
	{
		// 警报信息不同
		m_Param.m_ParamSub.m_bShowDlgListAlarm = Param.m_ParamSub.m_bShowDlgListAlarm;
		m_Param.m_ParamSub.m_eVoiceType = Param.m_ParamSub.m_eVoiceType;
		m_Param.m_ParamSub.m_StrVoiceFilePath = Param.m_ParamSub.m_StrVoiceFilePath;

		SaveInitParam();	// 保存参数, 仅在此时机保存
	}
	return true;
}

bool32 CMarketRadarCalc::StartCalc()
{
	//return true;
	// 开始计算
	StopCalc();
	ASSERT( NULL == m_hThreadCalc );

	if ( NULL == m_pAbsCenterManager )
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		m_pAbsCenterManager = NULL != pDoc ? pDoc->m_pAbsCenterManager : NULL;
		if ( NULL == m_pAbsCenterManager )
		{
			return false;
		}
	}

	if ( EMRMR_HSA == m_Param.m_eMerchRange )
	{
		// 沪深A
		m_Param.m_aMerchs.RemoveAll();
		
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
		if ( NULL != pBlock )
		{
			m_Param.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		}
	}
	else if ( EMRMR_ZX == m_Param.m_eMerchRange )
	{
		// 自选股, 拷贝现在的商品，并且稍后关注变化
		m_Param.m_aMerchs.RemoveAll();

		T_Block *pBlockUser = CUserBlockManager::Instance()->GetServerBlock();
		if ( NULL != pBlockUser )
		{
			m_Param.m_aMerchs.Copy(pBlockUser->m_aMerchs);
		}
	}
	else
	{
		// 不用处理了, 用参数中的商品
	}

	// 商品数据，暂时不处理

	// 开始线程
	CWinThread *pThreadReq = AfxBeginThread(ReqThreadFunc
		, (LPVOID)(CMarketRadarCalc *)this
		, THREAD_PRIORITY_BELOW_NORMAL
		, 0
		, CREATE_SUSPENDED);
	
	CWinThread *pThreadCalc = AfxBeginThread(CalcThreadFunc
		, (LPVOID)(CMarketRadarCalc *)this
		, THREAD_PRIORITY_BELOW_NORMAL
		, 0
		, CREATE_SUSPENDED);
	if ( NULL == pThreadReq
		|| NULL == pThreadReq->m_hThread
		|| NULL == pThreadCalc
		|| NULL == pThreadCalc->m_hThread)
	{
		ASSERT( 0 );
		SetRunFlag(0);
		if ( NULL != pThreadCalc )
		{
			pThreadCalc->ResumeThread();
		}
		if ( NULL != pThreadReq )
		{
			pThreadReq->ResumeThread();
		}
		Sleep(10);	// 休息一小段，让他们退出？
		// 让他们自生自灭
		return false;
	}

	ASSERT( NULL == m_hThreadCalc );
	DuplicateHandle(GetCurrentProcess()
		, pThreadCalc->m_hThread
		, GetCurrentProcess()
		, &m_hThreadCalc
		, 0
		, FALSE
		, DUPLICATE_SAME_ACCESS);

	ASSERT( NULL == m_hThreadReq );
	DuplicateHandle(GetCurrentProcess()
		, pThreadReq->m_hThread
		, GetCurrentProcess()
		, &m_hThreadReq
		, 0
		, FALSE
		, DUPLICATE_SAME_ACCESS);

	SetRunFlag(1);
	m_pAbsCenterManager->AddViewDataListner(this);	// 数据接收

	if ( EMRMR_ZX == m_Param.m_eMerchRange )
	{
		CBlockConfig::Instance()->AddListener(this);
	}

	m_TimeCalcStart = m_pAbsCenterManager->GetServerTime();
	m_dwTimeCalcStart = timeGetTime();

	AddListener(&m_Alarm);

	pThreadReq->ResumeThread();
	pThreadCalc->ResumeThread();

	return true;
}

typedef VOID (CALLBACK *MYPAPCFUNC)( ULONG_PTR dwParam );
typedef DWORD (WINAPI  *MyQueueUserAPC)(
								          MYPAPCFUNC pfnAPC,
								          HANDLE hThread,
								          ULONG_PTR dwData
								  );



DWORD CallQueueUserAPC(MYPAPCFUNC func, HANDLE hThread, ULONG_PTR dwData)
{
	CLoadLibHelperT<MyQueueUserAPC> loader(_T("Kernel32.dll"), "QueueUserAPC");
	if ( loader.m_pProg != NULL )
	{
		return loader.m_pProg(func, hThread, dwData);
	}
	else
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		return 1;
	}
}

void CMarketRadarCalc::StopCalc()
{
	SetRunFlag(0);

	if ( NULL != m_hThreadReq )
	{
		DWORD dwRet = WaitForSingleObjectEx(m_hThreadReq, 100, FALSE);
		if ( WAIT_TIMEOUT == dwRet )
		{
			CallQueueUserAPC(MyAPCProc, m_hThreadReq, 0);
		}
		WaitForSingleObjectEx(m_hThreadReq, INFINITE, FALSE);
		DEL_HANDLE(m_hThreadReq);
		m_hThreadReq = NULL;
	}
	if ( NULL != m_hThreadCalc )
	{
		DWORD dwRet= WaitForSingleObjectEx(m_hThreadCalc, 100, FALSE);
		if ( WAIT_TIMEOUT == dwRet )
		{
			CallQueueUserAPC(MyAPCProc, m_hThreadCalc, 0);
		}
		WaitForSingleObjectEx(m_hThreadCalc, INFINITE, FALSE);
		DEL_HANDLE(m_hThreadCalc);
		m_hThreadCalc = NULL;
	}

	RemoveListener(&m_Alarm);

	if ( NULL != m_pAbsCenterManager
		&& NULL != AfxGetDocument() )
	{
		// doc有可能比它先析构~~
		m_pAbsCenterManager->DelViewDataListner(this);
	}

	if ( NULL != CBlockConfig::PureInstance() )
	{
		CBlockConfig::PureInstance()->RemoveListener(this);
	}
}

void CMarketRadarCalc::FireResultAdd( const MarketRadarResultMap &mapResults, CMarketRadarListener::E_AddCause eCause, CMarketRadarListener *pIgnoreListener /*= NULL*/ )
{
	CSingleLock lock(&m_LockResult, TRUE);
	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != pIgnoreListener )
		{
			try
			{
				m_aListeners[i]->OnAddMarketRadarResult(mapResults, eCause);
			}
			catch (...)
			{
				TRACE(_T("错误: 市场雷达通知异常！！！！"));
			}
		}
	}
}

void CMarketRadarCalc::FireResultRemove( CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener/* = NULL*/  )
{
	CSingleLock lock(&m_LockResult, TRUE);
	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != pIgnoreListener )
		{
			try
			{
				m_aListeners[i]->OnRemoveMarketRadarResult(pMerch, eCause);
			}
			catch (...)
			{
				TRACE(_T("错误: 市场雷达通知异常！！！！"));
			}
		}
	}
}


void CMarketRadarCalc::FireResultClear(CMarketRadarListener *pIgnoreListener/* = NULL*/ )
{
	CSingleLock lock(&m_LockResult, TRUE);
	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != pIgnoreListener )
		{
			try
			{
				m_aListeners[i]->OnClearMarketRadarResult();
			}
			catch (...)
			{
				TRACE(_T("错误: 市场雷达通知异常！！！！"));
			}
		}
	}
}


void CMarketRadarCalc::AddListener( CMarketRadarListener *pListener )
{
	if ( NULL == pListener )
	{
		ASSERT( 0 );
		return;
	}
	CSingleLock lock(&m_LockResult, TRUE);
	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] == pListener )
		{
			return;
		}
	}
	m_aListeners.Add(pListener);
}

void CMarketRadarCalc::RemoveListener( CMarketRadarListener *pListener )
{
	if ( NULL == pListener )
	{
		ASSERT( 0 );
		return;
	}
	CSingleLock lock(&m_LockResult, TRUE);
	for ( int32 i=m_aListeners.GetUpperBound(); i >= 0 ; i-- )
	{
		if ( m_aListeners[i] == pListener )
		{
			m_aListeners.RemoveAt(i);
		}
	}
}

void CMarketRadarCalc::OnTimerFilterNeedInitMerch(CMarketRadarListener *pIgnoreListener/* = NULL*/)
{
	if ( NULL == m_pAbsCenterManager )
	{
		ASSERT( 0 );
		return;
	}

	if ( GetRunFlag() == 0 )
	{
		return;	// 非计算阶段不用过滤任何数据
	}

	CGmtTime TimeNow = GetNow();
	int32 i=0;

	typedef map<CMarket *, CMarketIOCTimeInfo> MarketIOCMap;
	MarketIOCMap mapIOC;
	MerchArray aMerchDel;

	// 计算现在时间对应的初始化时间，并根据参数或结果中的初始化时间，清理数据
	CSingleLock lock(&m_LockResult, TRUE);

	POSITION pos = m_mapResult.GetStartPosition();
	T_MarketRadarResult Result;
	aMerchDel.SetSize(0, m_mapResult.GetCount());
	while ( pos != NULL )
	{
		CMerch *pMerch;
		m_mapResult.GetNextAssoc(pos, pMerch, Result);
		ASSERT( Result.m_TimeInit.GetTime() > 0 );

		CMarket *pMarket = &pMerch->m_Market;
		MarketIOCMap::iterator it = mapIOC.find(pMarket);
		if ( it == mapIOC.end() )
		{
			// 在结果中出现的，必然有相应的计算参数
			CMarketIOCTimeInfo IOC;
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// 希望这个函数没有同步问题！！
			{
				MarketIOCMap::_Pairib iS = mapIOC.insert(MarketIOCMap::value_type(pMarket, IOC));
				if ( iS.second )
				{
					it = iS.first;
				}
			}
		}

		if ( it == mapIOC.end() )
		{
			continue;	// 没有开收盘信息，无法判断, 不过滤它
		}

		if ( Result.m_TimeInit != it->second.m_TimeInit.m_Time )
		{
			// 不是同一个初始化时间的
			aMerchDel.Add(pMerch);
		}
	}

	for ( i=0; i < aMerchDel.GetSize() ; i++ )
	{
		// 抛弃结果集, 在这里将结果删除
		m_mapResult.RemoveKey(aMerchDel[i]);
	}

	lock.Unlock();

	for ( i=0; i < aMerchDel.GetSize() ; i++ )
	{
		//  通知删除了结果集
		FireResultRemove(aMerchDel[i], CMarketRadarListener::EMC_OverInitTime, pIgnoreListener);
	}	

	// 对于在最近交易日前的商品计算数据做删除处理，避免计算时使用不在时间段内的数据
	CSingleLock lockMerchData(&m_LockCalcMerchDataNew, TRUE);
	T_CalcMerchData MerchData;
	aMerchDel.RemoveAll();
	aMerchDel.SetSize(0, m_mapCalcMerchDataNew.GetCount());
	pos = m_mapCalcMerchDataNew.GetStartPosition();
	while ( NULL != pos )
	{
		CMerch *pMerch;
		m_mapCalcMerchDataNew.GetNextAssoc(pos, pMerch, MerchData);

		ASSERT( IsValidCalcMerchData(MerchData) );

		CMarket *pMarket = &pMerch->m_Market;
		MarketIOCMap::iterator it = mapIOC.find(pMarket);
		if ( it == mapIOC.end() )
		{
			CMarketIOCTimeInfo IOC;
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// 希望这个函数没有同步问题！！
			{
				MarketIOCMap::_Pairib iS = mapIOC.insert(MarketIOCMap::value_type(pMarket, IOC));
				if ( iS.second )
				{
					it = iS.first;
				}
			}
		}
		
		if ( it == mapIOC.end() )
		{
			continue;	// 没有开收盘信息，无法判断, 不过滤它
		}
		
		if ( MerchData.m_TimeInit != it->second.m_TimeInit.m_Time)
		{
			// 实时报价数据无效了 - 直接删除
			// 分笔数据无效了 - 直接删除
			aMerchDel.Add(pMerch);
		}
	}

	for ( i=0; i < aMerchDel.GetSize() ; i++)
	{
		m_mapCalcMerchDataNew.RemoveKey(aMerchDel[i]);
	}
	if ( m_mapCalcMerchDataNew.IsEmpty() )
	{
		m_EventNewMerchData.ResetEvent();
	}

	// 检查所有数据的缓存，清除超时数据
	// 数据可能有点多
	aMerchDel.SetSize(0, m_mapCalcMerchDataAll.GetCount());
	pos = m_mapCalcMerchDataAll.GetStartPosition();
	while ( NULL != pos )
	{
		CMerch *pMerch;
		m_mapCalcMerchDataAll.GetNextAssoc(pos, pMerch, MerchData);
		
		CMarket *pMarket = &pMerch->m_Market;
		MarketIOCMap::iterator it = mapIOC.find(pMarket);
		if ( it == mapIOC.end() )
		{
			CMarketIOCTimeInfo IOC;
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// 希望这个函数没有同步问题！！
			{
				MarketIOCMap::_Pairib iS = mapIOC.insert(MarketIOCMap::value_type(pMarket, IOC));
				if ( iS.second )
				{
					it = iS.first;
				}
			}
		}
		
		if ( it == mapIOC.end() )
		{
			continue;	// 没有开收盘信息，无法判断, 不过滤它
		}
		
		if ( MerchData.m_TimeInit != it->second.m_TimeInit.m_Time)
		{
			// 实时报价数据无效了 - 直接删除
			// 分笔数据无效了 - 直接删除
			aMerchDel.Add(pMerch);
		}
	}
	
	for ( i=0; i < aMerchDel.GetSize() ; i++)
	{
		m_mapCalcMerchDataAll.RemoveKey(aMerchDel[i]);
	}

	lockMerchData.Unlock();


}

void CMarketRadarCalc::GetResults( OUT MarketRadarResultMap &mapResults, CMarketRadarListener *pIgnoreListener /*= NULL*/ )
{
	mapResults.RemoveAll();

	// 过滤不合适的商品
	OnTimerFilterNeedInitMerch(pIgnoreListener);

	CSingleLock lock(&m_LockResult, TRUE);
	POSITION pos = m_mapResult.GetStartPosition();
	T_MarketRadarResult R;
	while ( pos != NULL )
	{
		CMerch *pMerch;
		m_mapResult.GetNextAssoc(pos, pMerch, R);
		mapResults[pMerch] = R;
	}
	lock.Unlock();
}

void CMarketRadarCalc::GetResults( OUT MarketRadarResultArray &aResults, CMarketRadarListener *pIgnoreListener /*= NULL*/ )
{
	aResults.RemoveAll();

	// 过滤不合适的商品
	OnTimerFilterNeedInitMerch(pIgnoreListener);

	CSingleLock lock(&m_LockResult, TRUE);
	ResultMapToArray(m_mapResult, aResults);
	lock.Unlock();
}

void CMarketRadarCalc::ResultMapToArray( const MarketRadarResultMap &mapResult, OUT MarketRadarResultArray &aResults )
{
	aResults.SetSize(0, mapResult.GetCount());

	CMerch *pMerch;
	T_MarketRadarResult Result;
	POSITION pos = mapResult.GetStartPosition();
	if ( NULL != pos )
	{
		mapResult.GetNextAssoc(pos, pMerch, Result);
		aResults.Add(Result);
	}
	while ( NULL != pos )
	{
		mapResult.GetNextAssoc(pos, pMerch, Result);
		
		T_MarketRadarResult *pResult = aResults.GetData();
		int32 iUp = aResults.GetUpperBound();
		int32 iDown = 0;
		int32 iHalf = (iUp+iDown)/2;
		// 找寻最后一个大于当前时间的点
		for ( ; iUp >= iDown ;  )
		{
			iHalf = (iUp+iDown)/2;
			if ( pResult[iHalf].m_TimeCalc > Result.m_TimeCalc )
			{
				// 当前点大于现在的时间，前移
				iUp = iHalf-1;
			}
			else
			{
				// 当前点小于等于时间，后移
				iDown = iHalf+1;
			}
		}

		// 当前判断点
		if ( aResults[iHalf].m_TimeCalc > Result.m_TimeCalc )
		{
			// 大于现在的，则直接插在前面
			aResults.InsertAt(iHalf, Result);
		}
		else
		{
			// 小于等于，则第二个点必然是要插入的位置,无论是否有效
			aResults.InsertAt(iHalf+1, Result);
		}
		
	}
}

CGmtTime CMarketRadarCalc::GetNow()
{
	if ( m_dwTimeCalcStart > 0 )
	{
		return m_TimeCalcStart + CGmtTimeSpan((time_t)((timeGetTime()-m_dwTimeCalcStart)/1000));
	}
	else if ( NULL != m_pAbsCenterManager )
	{
		return m_pAbsCenterManager->GetServerTime();
	}
	ASSERT( 0 );
	return CGmtTime();
}

bool32 CMarketRadarCalc::IsMerchDataExpire( const T_CalcMerchData &MerchData )
{
	if ( NULL == MerchData.m_pMerch )
	{
		return true;
	}

	CMarketIOCTimeInfo IOC;
	if ( MerchData.m_pMerch->m_Market.GetRecentTradingDay(GetNow(), IOC, MerchData.m_pMerch->m_MerchInfo) )
	{
		if ( MerchData.m_TimeInit == IOC.m_TimeInit.m_Time )
		{
			return false;	// 没有超时
		}
	}
	return true;
}

CMarketRadarListener::~CMarketRadarListener()
{
	CMarketRadarCalc::Instance().RemoveListener(this);
}

//////////////////////////////////////////////////////////////////////////
//
void CMarketRadarAlarm::OnAddMarketRadarResult( const MarketRadarResultMap &mapResult, E_AddCause eCause )
{
	if ( mapResult.GetCount() > 0 && eCause == EAC_UpdateCondition )
	{
		// 满足的条件变更时，警报
		CWnd *pWnd = AfxGetMainWnd();
		if ( NULL != pWnd )
		{
			::PostMessage(pWnd->GetSafeHwnd(), WM_COMMAND, ID_MARKETRADAR_ADDRESULT, 0);
		}
	}
}

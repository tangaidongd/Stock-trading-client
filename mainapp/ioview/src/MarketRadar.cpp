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

#define RADAR_MERCHCOUNT_PRE_REQ	(18) // ÿ������Ҫ����ȥ����Ʒ����
#define RADAR_TIMESALE_COUNT (20)		// �ֱ����ݸ���

T_MarketRadarSubParameter::T_MarketRadarSubParameter()
{
	m_dwConditions = EMRC_BigBuy | EMRC_BigSell
		| EMRC_LowOpenRise | EMRC_HighOpenFall
		| EMRC_RiseOver | EMRC_FallBelow
		| EMRC_SingleTradeRiseFall | EMRC_HugeAmount
		| EMRC_BigVol
		;

	m_fWhatIsBigVol = 0.001f;		// 0.1%Ϊ����

	m_fRiseOverValue = 0.05f;		// �Ƿ���5%
	m_fFallBelowValue = 0.05f;		// ������5%
	m_fSingleTradeRiseFallValue = 0.012f;	// �쳣�����ǵ��� 1.2%
	m_fHugeAmountValue = 1500000.0f;	// �޶�ɽ� 150w

	m_iBigVolTimeRange = 5;			// ���5�����ڴ���
	m_iBigVolOverCount = 3;			// �ɽ�> 3��

	m_eVoiceType = EMRAVT_System;	// ϵͳ����
	m_bShowDlgListAlarm = true;		// �����б�
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
	// ���Ƚ�����������ص�
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
	pCalcObj->GetParameter(Param);	// ȡ�������գ�������������ֻ���жϼ��㣬���¿�ʼ
	if ( Param.m_aMerchs.GetSize() <= 0
		|| Param.m_ParamSub.m_dwConditions == 0 )
	{
		return 0;	// ��������
	}

	HANDLE hWait = CreateWaitableTimer(NULL, FALSE, NULL);
	LARGE_INTEGER liTimer;
	liTimer.QuadPart = -5000*10000;		// 5���Ӻ�ʼ����ֹ�����Ŀ�/ͣ�������µ����ݲ�ͣ����
	const LONG lPeriod = 3000;	// ����ms����һ��
	const LONG lPeriod2 = 60000;//10*60000;	// ��������������ʱ����Ϣ����ms
	ASSERT( lPeriod != lPeriod2 );
	if ( NULL == hWait
		|| !SetWaitableTimer(hWait, &liTimer, 0, NULL, NULL, FALSE) )
	{
		DEL_HANDLE(hWait);
		ASSERT( 0 );
		return 1;
	}
	WaitForSingleObjectEx(hWait, INFINITE, TRUE);	// �ȴ�����ʱ��ſ�ʼ
	
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

	TRACE(_T("�״������߳�(%d)�˳�~~\n"), GetCurrentThreadId());
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
		return 1;	// û�г�ʼ���ȴ�����event
	}

	T_MarketRadarParameter Param;
	pCalcObj->GetParameter(Param);	// ȡ�������գ�������������ֻ���жϼ��㣬���¿�ʼ
	const CGmtTime TimeCalcStartServer = pCalcObj->m_TimeCalcStart;	// ȡʱ�����
	const DWORD dwTimeStart = timeGetTime();
	//const CGmtTime TimeCalcStartLocal = CGmtTime::GetCurrentTime();
	ASSERT( TimeCalcStartServer.GetTime() > 0 );

	if ( Param.m_aMerchs.GetSize() <= 0
		|| Param.m_ParamSub.m_dwConditions == 0 )
	{
		return 0;	// �������
	}

	while ( pCalcObj->GetRunFlag() != 0 )
	{
		// ��ʼ�ȴ�����, ���۽����ʲô��������flag���ж�
		DWORD dwWait = WaitForSingleObjectEx(pCalcObj->m_EventNewMerchData, INFINITE, TRUE);
		if ( dwWait != WAIT_OBJECT_0 )
		{
			ASSERT( pCalcObj->GetRunFlag() == 0 );
			break;	// ��Ȼ��Ҫ�˳���
		}
		T_CalcMerchData MerchData;
		MarketRadarResultMap mapResults;
		while ( pCalcObj->GetNextNewCalcMerchData(MerchData, true) )
		{
			// ��֤��������
			if (
				MerchData.m_RealTimePrice.m_fVolumeTotal == 0.0f
				|| MerchData.m_aTicks.GetSize() <= 0 )
			{
				continue;	// �޳ɽ�������
			}

			T_MarketRadarResult Result;
			Result.m_eCondition = EMRC_None;
			Result.m_iRiseFallFlag = 0;
			Result.m_pMerch = MerchData.m_pMerch;
			Result.m_TimeInit = MerchData.m_TimeInit;
			DWORD dwTime = timeGetTime();
			CGmtTimeSpan Span((time_t)(dwTime-dwTimeStart)/1000);
			Result.m_TimeCalc = TimeCalcStartServer + Span;	// ������ʱ�䣬�����ʱ��

			// �ҵ����һ����Ч�ֱ�����
			int32 iLastTick = MerchData.m_aTicks.GetUpperBound();
			for ( ; iLastTick >= 0 ; iLastTick-- )
			{
				if ( MerchData.m_aTicks[iLastTick].m_fVolume > 0.0f )
				{
					break;
				}
			}
			MerchData.m_aTicks.SetSize(iLastTick+1);	// �����С
			if ( iLastTick >= 0 )
			{
				Result.m_TickLast = MerchData.m_aTicks[iLastTick];
			}

			float fLiuTongGuBen = 0;
			if ( MerchData.m_RealTimePrice.m_fTradeRate > 0.0f )
			{
				// Ŀǰ�����ʵ�λΪ%��λ, ������Ĺɱ�Ϊ�ֵ�λ
				fLiuTongGuBen = MerchData.m_RealTimePrice.m_fVolumeTotal / MerchData.m_RealTimePrice.m_fTradeRate *100;
			}
			// ��ͨ�ɱ�����Ϊ0

			const DWORD dwConditions = Param.m_ParamSub.m_dwConditions;
			
			// ����˳���ж�
			// �������� ��ͣ �Ƿ��� �޶� ���� ���� �Ϳ� �߿� ����ҵ� �����ҵ� �����¸�/��
			if ( EMRC_None == Result.m_eCondition
				&& (dwConditions & EMRC_BigVolInTime)
				&& iLastTick >= 0
				&& fLiuTongGuBen > 0.0f )
			{
				// �������� xxʱ����x����
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
						continue;		// �޳ɽ����ݵķֱ�����
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
					// �����־
					Result.m_eCondition = EMRC_BigVolInTime;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// �ǵ�ͣ
			if ( EMRC_None == Result.m_eCondition
				&& (dwConditions&EMRC_RiseFallMax)
				&& MerchData.m_fRiseFallStopRate > 0.0f
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				float fRiseRate = MerchData.m_RealTimePrice.m_fPriceNew/MerchData.m_RealTimePrice.m_fPricePrevClose;
				fRiseRate = fRiseRate - 1.0f;
				float fRiseFallStop = MerchData.m_fRiseFallStopRate - 0.0005;	// ����һ���ƫ��
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

			// �Ƿ���
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

			// ������
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

			// �޶�
			if ( EMRC_None == Result.m_eCondition
				&& ( EMRC_HugeAmount&dwConditions )
				&& iLastTick >= 0 )
			{
				// �����һ����ʱ���ڵĶ����һ��
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
							iLastTick = i;		// �޸����һ�ʷֱ�����
							Result.m_TickLast = tick;
						}
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// ����
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigVol & dwConditions)
				&& iLastTick >= 0
				&& fLiuTongGuBen > 0.0f )
			{
				// �����һ����ʱ���ڵĶ����һ��
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
							iLastTick = i;		// �޸����һ�ʷֱ�����
							Result.m_TickLast = tick;
						}
						mapResults[ Result.m_pMerch ] = Result;
						break;
					}
				}
			}

			// �����쳣
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

			// �Ϳ�����
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

			// �߿�����
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

			// ����ҵ�
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigBuy & dwConditions)
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				// �ж�ʵʱ�����е��嵵������
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

			// �����ҵ�
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_BigSell & dwConditions)
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f )
			{
				// �ж�ʵʱ�����е��嵵������
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

			// �����¸�
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_TodayNewHigh &dwConditions)
				&& iLastTick >= 2 		// ����Ҫ����һ������
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				if ( MerchData.m_RealTimePrice.m_fPriceNew >= MerchData.m_RealTimePrice.m_fPriceHigh
					&& MerchData.m_RealTimePrice.m_fPriceNew > MerchData.m_RealTimePrice.m_fPricePrevClose )
				{
					// ֻ�п����ǵ��� && ��
					Result.m_eCondition = EMRC_TodayNewHigh;
					Result.m_iRiseFallFlag = 1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// �����µ�
			if ( EMRC_None == Result.m_eCondition
				&& (EMRC_TodayNewLow &dwConditions)
				&& iLastTick >= 2 		// ����Ҫ����һ������ 
				&& MerchData.m_RealTimePrice.m_fPricePrevClose > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f
				&& MerchData.m_RealTimePrice.m_fPriceNew > 0.0f )
			{
				if ( MerchData.m_RealTimePrice.m_fPriceNew <= MerchData.m_RealTimePrice.m_fPriceLow
					&& MerchData.m_RealTimePrice.m_fPriceNew < MerchData.m_RealTimePrice.m_fPricePrevClose )
				{
					// ֻ�п����ǵ��� && ��
					Result.m_eCondition = EMRC_TodayNewLow;
					Result.m_iRiseFallFlag = -1;
					mapResults[ Result.m_pMerch ] = Result;
					break;
				}
			}

			// �ж����
			if ( Result.m_eCondition != EMRC_None )
			{
				mapResults[ Result.m_pMerch ] = Result;
			}
			else
			{
				pCalcObj->RemoveResult(Result.m_pMerch, CMarketRadarListener::EMC_NotSatisfyCondition);	// ����Ʒû�����������ˣ�
			}
		}

		if ( mapResults.GetCount() > 0 )
		{
			pCalcObj->AddResult(mapResults);
		}
	}

	TRACE(_T("�״�����߳�(%d)�˳�~~\n"), GetCurrentThreadId());
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

	OnTimerFilterNeedInitMerch(NULL);	// ���������ʱ��Ҫ�����Ʒ������������

	// ���ڱ���һ�δ���г���Χ��Ҫn��ʱ�䣬���Կ���Щ��Ʒ��ʵʱ���۵�
	// ������ǰ����ʹ�����Ʒ���ȴ�����Щ��Ʒ
	MerchArray aMerchs;
	aMerchs.Copy(m_Param.m_aMerchs);	// �����ڼ���������ܱ��
	int32 iUp = aMerchs.GetUpperBound();
	int32 iDown = 0;
	for ( ; iDown < iUp ; ++iDown,--iUp )
	{
		if ( aMerchs[iDown]->m_pRealtimePrice != NULL )	// �첽����ȡ��ָ��
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
	// ��������������е���Ʒ������û�е�ɾ�����µ���Ʒ���ӵ�β��
	// Ч�ʺܵͣ������߳����ȼ�Ϊbelownormal~~��Ӧ�ò��Ῠ���߳�
	m_aMerchsWaitReq.SetSize(m_aMerchsWaitReq.GetSize(), m_aMerchsWaitReq.GetSize()+aMerchs.GetSize());
	for ( i=m_aMerchsWaitReq.GetUpperBound(); i >=0 ; --i )
	{
		MerchSet::iterator it = merchExist.find(m_aMerchsWaitReq[i]);
		if ( it != merchExist.end() )
		{
			merchExist.erase(it);	// ���ھ�ɾ�������Ʒ�Ĵ��ڼ�¼ 
		}
		else
		{
			m_aMerchsWaitReq.RemoveAt(i);	// ɾ�������
		}
	}
	for ( i=0 ; i < aMerchs.GetSize() ; i++ )
	{
		MerchSet::iterator it = merchExist.find(aMerchs[i]);
		if ( it != merchExist.end() )
		{
			merchExist.erase(it);	// ���ھ�ɾ��
			m_aMerchsWaitReq.Add(aMerchs[i]);	// ��ӽ��������
		}
		else
		{
			// �Ѿ������������������
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
	TRACE(_T("�г��״�׼���������ݣ�%d ms\r\n"), lgK.LowPart+lgU.LowPart);
#endif

	return RequestNextViewData();
}

bool32 CMarketRadarCalc::RequestNextViewData()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	// TODO: ��������Ż�

	const int32 iReqDataCount = RADAR_MERCHCOUNT_PRE_REQ;
	const CGmtTimeSpan Span(0, 0, 1, 0) ;		// ����ʱ���ڵķ��͹�����Ĳ��ٷ�������
	CGmtTime TimeNow = GetNow();
	MerchArray	aMerchs;

	CSingleLock lock(&m_LockReq, TRUE);
	if ( m_aMerchsWaitReq.GetSize() <= 0 )
	{
		return false;	// û������������
	}
	if ( m_aMerchsWaitEcho.GetSize() > 0 )
	{
		// �������ڵȴ����У�����������
		// һ��״̬�£���Щ��������ô��ʱ����Ӧ�ö������ˣ�û�����������м�Ͽ��˷���ʲô��
		// Ҳ�п����Ǹ����������Ʒ������û�к��ʵ����ݷ���
		// �ٴ�����ȴ�Ӧ�����������
		m_aMerchsWaitEcho.RemoveAll();  // ��ʱ����
		//return TryRequestWaitEchoData();
	}
	
	// ����ѡ�񱾵�û�н����������ݷ���
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
			// û�����������Ʒ ���߳�����С��������
			// �Ƿ�Ҫ��������ʱ��ι��ˣ��ϴ����������������ͬһ��������ʱ�����
			aMerchs.Add(pMerch);
			m_mapMerchReqTimes[pMerch] = TimeNow;	// ��������ʱ��
		}
	}
	// �Ƴ��ȽϹ�����Ʒ
	m_aMerchsWaitReq.RemoveAt(0, iReqPos);

	m_aMerchsWaitEcho.Copy(aMerchs);
	lock.Unlock();

	return RequestMerchData(aMerchs) == iSize;		// ��������֤��������Ϣ�Ƚϳ�ʱ����
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
		return aMerchs.GetSize();	// �ѻ�����¾Ͳ�Ҫ���������ˣ�����ëѽ
	}

	if ( NULL == m_pAbsCenterManager
		|| aMerchs.GetSize() <= 0 )
	{
		return 0;
	}

	// ͨ��dataManager��������
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc
		|| NULL == pDoc->m_pAbsDataManager )
	{
		ASSERT( 0 );
		return 0;
	}

	// ����ʵʱ��������
	// ����Ķ����첽���ͣ���Ʒ������id��code���ǳ�ʼ���Ͳ��ڱ䶯�ģ�����䶯����Ҫ������
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
	
	// ������Ʒ�ķֱ���������	 ��������ʱ��ļ򵥱�֤����~~
	CGmtTime TimeNow = GetNow() + CGmtTimeSpan(0, 1, 0, 0);
	for ( i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchs[i];
		
		CMmiReqMerchTimeSales info;
		info.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType	= ERTYFrontCount;
		info.m_TimeSpecify	= TimeNow;
		info.m_iFrontCount  = RADAR_TIMESALE_COUNT;		// �̶���������
		
		//m_pAbsCenterManager->RequestViewData(&info);
		pDoc->m_pAbsDataManager->RequestData(&info, aMmiReqNodes);
		if ( info.m_iFrontCount <= 0 )
		{
			TRACE(_T("!!!�ֱ��������: id=%d code=%s time=%d count=0\r\n"), info.m_iMarketId, info.m_StrMerchCode.GetBuffer(), TimeNow.GetTime());
		}
	}

	return iSize;
}

#define IsValidCalcMerchData(MerchData) ( MerchData.m_aTicks.GetSize() > 0 \
		&& MerchData.m_RealTimePrice.m_fPriceOpen > 0.0f \
		&& MerchData.m_RealTimePrice.m_fVolumeTotal > 0.0f  )

bool32 CMarketRadarCalc::GetNextNewCalcMerchData( OUT T_CalcMerchData &MerchData, bool32 bPop/*=true*/ )
{
	// ʹ������
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
				m_EventNewMerchData.ResetEvent();	// ����
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
		// ��Ҫȡ��ʱ����û��������~
		m_EventNewMerchData.ResetEvent();	// �Ѿ�û����Ч������
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

	// �������ǵ�ͣ����
	if ( MerchData.m_fRiseFallStopRate == 0.0f )
	{
		E_ReportType eRT = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
		if ( ERTStockCn == eRT
			|| ERTExp == eRT )
		{
			if ( 'N' != pMerch->m_MerchInfo.m_StrMerchCnName[0] )// �¹� ���ü����ǵ�ͣ, ���Ϊ���ǵ�ͣ
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

	// ��������ǰ��ĳ�ʼ��ʱ��
	CMarket *pMarket = &pMerch->m_Market;
	CMarketIOCTimeInfo IOC;
	CGmtTime TimeNow = GetNow();
	if ( pMarket->GetRecentTradingDay(GetNow(), IOC, pMerch->m_MerchInfo) )	// ϣ���������û��ͬ�����⣡��
	{
		ASSERT( TimeNow >= IOC.m_TimeInit.m_Time );
		// ��Ȼ�ǽ������ǰ
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
	// ���µ�ʵʱ��������
	bool32 bAddNew = false;
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);
	T_CalcMerchData MerchData;
	if ( m_mapCalcMerchDataAll.Lookup(pMerch, MerchData) )
	{
		if ( IsMerchDataExpire(MerchData) )
		{
			// ��ʱ�˵����ݣ�����Ҫ��ջ���
			m_mapCalcMerchDataAll.RemoveKey(pMerch);
			m_mapCalcMerchDataNew.RemoveKey(pMerch);
			if ( m_mapCalcMerchDataNew.IsEmpty() )
			{
				m_EventNewMerchData.ResetEvent();
			}
			// ���¼���
			T_CalcMerchData MerchData2;
			MerchData2.m_pMerch = pMerch;
			MerchData2.m_RealTimePrice = RealTimePrice;
			DoAddNewCalcMerchData(MerchData2);
		}
		else
		{
			// ��������
			MerchData.m_RealTimePrice = RealTimePrice;
			m_mapCalcMerchDataAll[ pMerch ] = MerchData;	
			if ( IsValidCalcMerchData(MerchData) )
			{
				m_mapCalcMerchDataNew[pMerch] = MerchData;
				bAddNew = true;
				// ����ȴ������е�����
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

		// ���뻺�棬�����ȴ�
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
	// ���µķֱ�����
	CSingleLock lock(&m_LockCalcMerchDataNew, TRUE);
	T_CalcMerchData MerchData;
	if ( m_mapCalcMerchDataAll.Lookup(pMerch, MerchData) )
	{
		if ( IsMerchDataExpire(MerchData) )
		{
			// ��ʱ�˵����ݣ�����Ҫ��ջ���
			m_mapCalcMerchDataAll.RemoveKey(pMerch);
			m_mapCalcMerchDataNew.RemoveKey(pMerch);
			if ( m_mapCalcMerchDataNew.IsEmpty() )
			{
				m_EventNewMerchData.ResetEvent();
			}
			// ���¼���
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
				// ����ȴ������е�����
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

	// ����pMerch�б�Ȼ�Ѿ�������˷ֱ����У�ֱ��ʹ��pMerch�е�����n������
	// ���ﷵ�صĿ��ܲ����������Ӧ�õ���
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CMarketIOCTimeInfo IOCTime;
	if ( !pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo) )
	{
		return;
	}

	// ����ɽ�����, ���ǽ��������ǰ����, Ѱ����������յķֱ�����,
	// ��������Щ������˳���Һ����µ�
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
	
	// �ɽ�������Ҫ��ʼ��Ϊ�֣�������viewdata��һ��
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
	// ���Ի�ȡviewdata
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

const TCHAR KStrXmlRadarSection[] = _T("MarketRadar");		// ����
const TCHAR KStrXmlRadarConditions[]				= _T("Conditions");		// ����
const TCHAR KStrXmlRadarWhatIsBigVol[]				= _T("WhatIsBigVol");	// ʲô�Ǵ���
const TCHAR KStrXmlRadarRiseOverValue[]				= _T("RiseOverValue");	// �Ƿ���
const TCHAR KStrXmlRadarFallBelowValue[]			= _T("FallBelowValue");	// ������
const TCHAR KStrXmlRadarSingleTradeRiseFallValue[]  = _T("SingleTradeRiseFallValue");	// �����쳣
const TCHAR KStrXmlRadarHugeAmountValue[]  = _T("HugeAmountValue");	// �޶�
const TCHAR KStrXmlRadarBigVolTimeRange[]  = _T("BigVolTimeRange");	// x���������ɽ�
const TCHAR KStrXmlRadarBigVolOverCount[]  = _T("BigVolOverCount");	// x��
const TCHAR KStrXmlRadarVoiceType[]  = _T("VoiceType");	// �����������
const TCHAR KStrXmlRadarVoiceFilePath[]  = _T("VoiceFilePath");	// �û�ѡ����ļ�
const TCHAR KStrXmlRadarShowDlgListAlarm[]  = _T("ShowDlgListAlarm");	// ��ʾ�б��
const TCHAR KStrXmlRadarMerchRange[]  = _T("MerchRange");	// ѡ�����Ʒ����

const TCHAR KStrXmlRadarStart[]		=	_T("Start");	// �Ƿ���Ҫ����
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
			// �ȵ���ʼ����ʱȡ����A���б�
		}
		break;
	case EMRMR_ZX:
		{
			// �ȵ�����ʱȡ��ѡ����Ϣ������ע�����
		}
		break;
	case EMRMR_MerchArray:
		{
			// ��ȡѡ�����Ʒ��Ϣ
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
		saNames.Add(_T("�����¸�"));
		saNames.Add(_T("�����µ�"));
		saNames.Add(_T("����ҵ�"));
		saNames.Add(_T("�����ҵ�"));
		saNames.Add(_T("�Ϳ�����"));
		saNames.Add(_T("�߿�����"));
		saNames.Add(_T("�۷�ͻ��"));
		saNames.Add(_T("�۷�����"));
		saNames.Add(_T("��������"));
		saNames.Add(_T("�޶�ɽ�"));
		saNames.Add(_T("�����ɽ�"));
		saNames.Add(_T("��ͣԤ��"));
		saNames.Add(_T("��������"));
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
			Str = _T("��ͣԤ��");
		}
		break;
	case EMRC_SingleTradeRiseFall:
		if ( Result.m_iRiseFallFlag < 0 )
		{
			Str = _T("�������");
		}
		break;
	}

	return Str;	// �������� TODO
}

const TCHAR KStrRadarMerchFileName[] = _T("MerchRange.dat");	// ��Ʒ�б�����
void CMarketRadarCalc::SaveMerchArray( const MerchArray &aMerchs )
{
	CString StrFilePath = CPathFactory::GetSaveConfigFileName(KStrRadarMerchFileName);
	if ( aMerchs.GetSize() <= 0 )
	{
		::DeleteFile(StrFilePath);	// ɾ�����ļ�
	}
	else
	{
		CStdioFile file;
		if ( file.Open(StrFilePath, CFile::typeBinary |CFile::modeCreate |CFile::modeWrite) )
		{
			// д��ͷ����Ϣ
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
			// ͷ����Ϣ
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
					aMerchs.Add(pMerch);	// �������ظ�
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
			// ��Ʒ���
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
					merchDels.insert(m_Param.m_aMerchs[i]);	// ����û�еľ���Ҫ�����������ɾ����
				}
			}
			m_Param.m_aMerchs.Copy(pBlockUser->m_aMerchs);	// ���²����е���Ʒ����

			// ������������Ʒ��ӵ��������
			// ɾ������Ʒ���������������ɾ�������������Ʒ�ļ�����
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
					RequestNextViewData();		// �������Ƶ������ӻᵼ��Ƶ���������Ż�TODO
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
	// �κβ����ı�����ᵼ����������&����ķ���(�����ǰ������)
	if ( !m_Param.CompareCalcParam(Param) )
	{
		// ֹͣ����
		LONG lRun = m_lRunFlag;
		StopCalc();
		
		m_Param = Param;
		SaveInitParam();	// �������, ���ڴ�ʱ������
		
		if ( lRun != 0 )
		{
			StartCalc();
		}
	}
	else
	{
		// ������Ϣ��ͬ
		m_Param.m_ParamSub.m_bShowDlgListAlarm = Param.m_ParamSub.m_bShowDlgListAlarm;
		m_Param.m_ParamSub.m_eVoiceType = Param.m_ParamSub.m_eVoiceType;
		m_Param.m_ParamSub.m_StrVoiceFilePath = Param.m_ParamSub.m_StrVoiceFilePath;

		SaveInitParam();	// �������, ���ڴ�ʱ������
	}
	return true;
}

bool32 CMarketRadarCalc::StartCalc()
{
	//return true;
	// ��ʼ����
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
		// ����A
		m_Param.m_aMerchs.RemoveAll();
		
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
		if ( NULL != pBlock )
		{
			m_Param.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		}
	}
	else if ( EMRMR_ZX == m_Param.m_eMerchRange )
	{
		// ��ѡ��, �������ڵ���Ʒ�������Ժ��ע�仯
		m_Param.m_aMerchs.RemoveAll();

		T_Block *pBlockUser = CUserBlockManager::Instance()->GetServerBlock();
		if ( NULL != pBlockUser )
		{
			m_Param.m_aMerchs.Copy(pBlockUser->m_aMerchs);
		}
	}
	else
	{
		// ���ô�����, �ò����е���Ʒ
	}

	// ��Ʒ���ݣ���ʱ������

	// ��ʼ�߳�
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
		Sleep(10);	// ��ϢһС�Σ��������˳���
		// ��������������
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
	m_pAbsCenterManager->AddViewDataListner(this);	// ���ݽ���

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
		// doc�п��ܱ���������~~
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
				TRACE(_T("����: �г��״�֪ͨ�쳣��������"));
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
				TRACE(_T("����: �г��״�֪ͨ�쳣��������"));
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
				TRACE(_T("����: �г��״�֪ͨ�쳣��������"));
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
		return;	// �Ǽ���׶β��ù����κ�����
	}

	CGmtTime TimeNow = GetNow();
	int32 i=0;

	typedef map<CMarket *, CMarketIOCTimeInfo> MarketIOCMap;
	MarketIOCMap mapIOC;
	MerchArray aMerchDel;

	// ��������ʱ���Ӧ�ĳ�ʼ��ʱ�䣬�����ݲ��������еĳ�ʼ��ʱ�䣬��������
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
			// �ڽ���г��ֵģ���Ȼ����Ӧ�ļ������
			CMarketIOCTimeInfo IOC;
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// ϣ���������û��ͬ�����⣡��
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
			continue;	// û�п�������Ϣ���޷��ж�, ��������
		}

		if ( Result.m_TimeInit != it->second.m_TimeInit.m_Time )
		{
			// ����ͬһ����ʼ��ʱ���
			aMerchDel.Add(pMerch);
		}
	}

	for ( i=0; i < aMerchDel.GetSize() ; i++ )
	{
		// ���������, �����ｫ���ɾ��
		m_mapResult.RemoveKey(aMerchDel[i]);
	}

	lock.Unlock();

	for ( i=0; i < aMerchDel.GetSize() ; i++ )
	{
		//  ֪ͨɾ���˽����
		FireResultRemove(aMerchDel[i], CMarketRadarListener::EMC_OverInitTime, pIgnoreListener);
	}	

	// ���������������ǰ����Ʒ����������ɾ�������������ʱʹ�ò���ʱ����ڵ�����
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
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// ϣ���������û��ͬ�����⣡��
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
			continue;	// û�п�������Ϣ���޷��ж�, ��������
		}
		
		if ( MerchData.m_TimeInit != it->second.m_TimeInit.m_Time)
		{
			// ʵʱ����������Ч�� - ֱ��ɾ��
			// �ֱ�������Ч�� - ֱ��ɾ��
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

	// ����������ݵĻ��棬�����ʱ����
	// ���ݿ����е��
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
			if ( pMarket->GetRecentTradingDay(TimeNow, IOC, pMerch->m_MerchInfo) )	// ϣ���������û��ͬ�����⣡��
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
			continue;	// û�п�������Ϣ���޷��ж�, ��������
		}
		
		if ( MerchData.m_TimeInit != it->second.m_TimeInit.m_Time)
		{
			// ʵʱ����������Ч�� - ֱ��ɾ��
			// �ֱ�������Ч�� - ֱ��ɾ��
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

	// ���˲����ʵ���Ʒ
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

	// ���˲����ʵ���Ʒ
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
		// ��Ѱ���һ�����ڵ�ǰʱ��ĵ�
		for ( ; iUp >= iDown ;  )
		{
			iHalf = (iUp+iDown)/2;
			if ( pResult[iHalf].m_TimeCalc > Result.m_TimeCalc )
			{
				// ��ǰ��������ڵ�ʱ�䣬ǰ��
				iUp = iHalf-1;
			}
			else
			{
				// ��ǰ��С�ڵ���ʱ�䣬����
				iDown = iHalf+1;
			}
		}

		// ��ǰ�жϵ�
		if ( aResults[iHalf].m_TimeCalc > Result.m_TimeCalc )
		{
			// �������ڵģ���ֱ�Ӳ���ǰ��
			aResults.InsertAt(iHalf, Result);
		}
		else
		{
			// С�ڵ��ڣ���ڶ������Ȼ��Ҫ�����λ��,�����Ƿ���Ч
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
			return false;	// û�г�ʱ
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
		// ������������ʱ������
		CWnd *pWnd = AfxGetMainWnd();
		if ( NULL != pWnd )
		{
			::PostMessage(pWnd->GetSafeHwnd(), WM_COMMAND, ID_MARKETRADAR_ADDRESULT, 0);
		}
	}
}

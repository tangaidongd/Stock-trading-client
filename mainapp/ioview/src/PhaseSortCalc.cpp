#include "StdAfx.h"

#include "PhaseSortCalc.h"

#include "sharestruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
T_PhaseSortCalcParam::T_PhaseSortCalcParam()
{
	m_hWndMsg = NULL;
	m_uMsgBase = 0;

	m_bAutoDelThread = true;
	m_hThread = NULL;

	m_bTerminate = true;

	m_bDoPreWeight = false;	// 离线数据暂无除权信息，默认不复权
	m_eResultFlag = EPSCRF_OK;

	m_pAbsDataManager = NULL;

	m_bNeedRequsetDataParam = true;
}

void T_PhaseSortCalcParam::CopyResult( const T_PhaseSortCalcParam &Phase )
{
	m_aCalcResults.Copy(Phase.m_aCalcResults);
	m_aMerchsToCalc.Copy(Phase.m_aMerchsToCalc);
	m_eResultFlag = Phase.m_eResultFlag;

	m_TimeStart = Phase.m_TimeStart;
	m_TimeEnd = Phase.m_TimeEnd;
	m_bDoPreWeight = Phase.m_bDoPreWeight;
}

UINT AFX_CDECL PhaseSortCalcThread(LPVOID);

bool32 StartPhaseSortCalc( T_PhaseSortCalcParam *pParam )
{
	if ( NULL == pParam )
	{
		ASSERT( 0 );
		return false;
	}

	pParam->m_eResultFlag = EPSCRF_Error;
	ASSERT( NULL != pParam->m_hWndMsg );
	ASSERT( WM_USER <= pParam->m_uMsgBase );
	ASSERT( NULL != pParam->m_pAbsDataManager );
	if ( NULL == pParam->m_pAbsDataManager )
	{
		return false;
	}

	ASSERT( pParam->m_TimeEnd >= pParam->m_TimeStart );
	if ( pParam->m_TimeStart < pParam->m_TimeStart )
	{
		return false;
	}

	pParam->m_aCalcResults.SetSize(pParam->m_aMerchsToCalc.GetSize());
	if ( pParam->m_aMerchsToCalc.GetSize() <= 0 )
	{
		pParam->m_eResultFlag = EPSCRF_OK;
		::PostMessage(pParam->m_hWndMsg, pParam->m_uMsgBase+EPSCM_Start, (WPARAM)pParam, NULL);
		::PostMessage(pParam->m_hWndMsg, pParam->m_uMsgBase+EPSCM_End, (WPARAM)pParam, EPSCRF_OK);
		return true;	// 不用计算
	}

	// 设置参数初始值
	ASSERT( NULL == pParam->m_hThread );	// 前面的必须被清除了
	pParam->m_hThread = NULL;

	pParam->m_bTerminate = false;

	CWinThread *pThread = AfxBeginThread(PhaseSortCalcThread, pParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if ( NULL == pThread )
	{
		return false;
	}

	BOOL b = ::DuplicateHandle(GetCurrentProcess(), pThread->m_hThread, GetCurrentProcess(), &pParam->m_hThread, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if ( !b )
	{
		return false;
	}
	
	pThread->ResumeThread();	// 开始计算
	Sleep(0);

	return true;
}

UINT AFX_CDECL PhaseSortCalcThread( LPVOID pVoidParam)
{
	T_PhaseSortCalcParam *pParam = (T_PhaseSortCalcParam *)pVoidParam;
	if ( NULL == pParam || pParam->m_pAbsDataManager == NULL )
	{
		ASSERT( 0 );
		return 1;
	}

	pParam->m_eResultFlag = EPSCRF_Calculating;

#ifndef _DEBUG
	try
#endif //_DEBUG
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CGGTongDoc *pDoc = pApp->m_pDocument;						
		CAbsDataManager *pAbsDataManager = pDoc->m_pAbsDataManager;
		if (!pAbsDataManager)
		{
			return 0;
		}

		CGmtTime TimeStart, TimeEnd;
		TimeStart = pParam->m_TimeStart;
		SaveDay(TimeStart);		// 开始的要取整
		TimeEnd = pParam->m_TimeEnd;
		//SaveDay(TimeEnd);	// 都是日线数据，取整
		bool32 bNeedPreWeight = pParam->m_bDoPreWeight;
		bool32 bNeedReq = pParam->m_bNeedRequsetDataParam;
		
		HWND hWnd = pParam->m_hWndMsg;
		const UINT uMsgBase = pParam->m_uMsgBase;

		CIoViewBase::MerchArray aMerchSrc;
		aMerchSrc.Copy(pParam->m_aMerchsToCalc);	// 计算的源商品列表

		PhaseSortCalcDataParamArray aDataParams;
		aDataParams.Copy(pParam->m_aDataParamToCalc);

		PhaseSortCalcResultArray aResults;	// 结果
		aResults.SetSize(aMerchSrc.GetSize());
		T_PhaseSortCalcReslt *pRes = aResults.GetData();
		const int32 iResSize = aResults.GetSize();
		ZeroMemory(pRes, iResSize*sizeof(T_PhaseSortCalcReslt));	// 结果集清0

		
		::PostMessage(hWnd, uMsgBase+EPSCM_Start, (WPARAM)pVoidParam, NULL);

		CGmtTimeSpan TimeSpan = TimeEnd - TimeStart;
		const int32 iMaxBaseKlineCount = TimeSpan.GetDays()+5;	// 基本数据所需要的若干K线

		int32 i = 0;
		CArray<CKLine, CKLine> aKlines;
		aKlines.SetSize(0, iMaxBaseKlineCount);
		for ( i=0; i < aMerchSrc.GetSize() && !pParam->m_bTerminate ; i++ )
		{
			CMerch *pMerch = aMerchSrc[i];
			T_PhaseSortCalcReslt &ResCur = pRes[i];

			if ( NULL == pMerch )
			{
				ASSERT( 0 );
				continue;
			}

			::PostMessage(hWnd, uMsgBase+EPSCM_Pos, (WPARAM)pVoidParam, (LPARAM)i);		// 位置

			T_PhaseSortCalcRequestData ReqData;
			ReqData.fCircAssert = ReqData.fMarketAmount = 0.0f;
			ReqData.pMerch = pMerch;
			if ( aDataParams.GetSize() > i )
			{
				ReqData.fMarketAmount = aDataParams[i].fMarketAmount;
				ReqData.fCircAssert = aDataParams[i].fCircAssert;
				ReqData.m_RealTimePrice = aDataParams[i].m_RealTimePrice;
			}

			int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
			const CString &StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;

			// 读取离线数据库的K线数据, iMaxBase + 5
			aKlines.SetSize(0, iMaxBaseKlineCount);
			pAbsDataManager->ReadOfflineKLines(iMarketId, StrMerchCode, EKTBDay, EOKTOfflineData, TimeEnd, iMaxBaseKlineCount, aKlines);

			// 如果实时报价的日期小于等于要求的日期，则修改或者添加最后一个日K线，其它情况不处理
			// 一般情况下，实时报价时间>=最后一个日K线数据的时间
			CGmtTime TimeRealTimePrice = ReqData.m_RealTimePrice.m_TimeCurrent.m_Time;
			CGmtTime TimeEndDay(TimeEnd);
			SaveDay(TimeRealTimePrice);
			SaveDay(TimeEndDay);
			if ( TimeRealTimePrice.GetTime() > 0 && TimeRealTimePrice <= TimeEndDay )
			{
				// 要求今日的
				bool32 bAppend = true;
				const int32 iKlineCount = aKlines.GetSize();
				if ( iKlineCount > 0 && aKlines[iKlineCount-1].m_TimeCurrent == TimeRealTimePrice )
				{
					CKLine &kline = aKlines[iKlineCount-1];
					// 开盘价和时间保持不变
					kline.m_fPriceClose		= ReqData.m_RealTimePrice.m_fPriceNew;		// 收盘价替换
					kline.m_fPriceAvg		= ReqData.m_RealTimePrice.m_fPriceAvg;		// 均价替换
					
					// 有今日的日k线，则实时报价是否会出现初始化0的情况？
					kline.m_fPriceHigh = max(kline.m_fPriceHigh, ReqData.m_RealTimePrice.m_fPriceHigh);	// 有日K线出现？
					kline.m_fPriceLow = max(kline.m_fPriceLow, ReqData.m_RealTimePrice.m_fPriceLow);
					
					//
					kline.m_fVolume			= ReqData.m_RealTimePrice.m_fVolumeTotal;			// 成交量
					kline.m_fAmount			= ReqData.m_RealTimePrice.m_fAmountTotal;			// 成交金额
					kline.m_fHold			= ReqData.m_RealTimePrice.m_fHoldTotal;			// 持仓替换
					
					// 
					//kline.m_usFallMerchCount	= ;	// 涨跌数不知道怎么替换
					//kline.m_usRiseMerchCount	= KLine.m_usRiseMerchCount;	
					bAppend = false;
				}
				
				if ( bAppend )
				{
					// 添加一个今日的日线？
					CKLine kline;
					kline.m_TimeCurrent = ReqData.m_RealTimePrice.m_TimeCurrent.m_Time;
					kline.m_fPriceOpen	= ReqData.m_RealTimePrice.m_fPriceOpen;
					kline.m_fPriceClose		= ReqData.m_RealTimePrice.m_fPriceNew;		// 收盘价替换
					kline.m_fPriceAvg		= ReqData.m_RealTimePrice.m_fPriceAvg;		// 均价替换
					
					// 有今日的日k线，则实时报价是否会出现初始化0的情况？
					kline.m_fPriceHigh  = ReqData.m_RealTimePrice.m_fPriceHigh;	// 有日K线出现？
					kline.m_fPriceLow	= ReqData.m_RealTimePrice.m_fPriceLow;
					
					//
					kline.m_fVolume			= ReqData.m_RealTimePrice.m_fVolumeTotal;			// 成交量
					kline.m_fAmount			= ReqData.m_RealTimePrice.m_fAmountTotal;			// 成交金额
					kline.m_fHold			= ReqData.m_RealTimePrice.m_fHoldTotal;			// 持仓替换
					
					// 
					//kline.m_usFallMerchCount	= ;	// 涨跌数不知道怎么替换
					//kline.m_usRiseMerchCount	= KLine.m_usRiseMerchCount;

					aKlines.Add(kline);
				}
			}

			// 读取离线数据
			if ( bNeedPreWeight && aKlines.GetSize() > 0 )	// 总要有数据吧
			{
				// 取文件路径:
				CString StrFilePath;
				if ( pAbsDataManager->GetPublishFilePath(EPFTWeight, iMarketId , StrMerchCode, StrFilePath) )
				{
					// 读数据:
					HANDLE hFile = ::CreateFile(StrFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if ( INVALID_HANDLE_VALUE != hFile )
					{
						DWORD iDataLen = (int32)GetFileSize(hFile, NULL);
						if ( iDataLen > 0  )
						{
							char *pBuf = new char[iDataLen];
							DWORD dwRead = 0;
							ReadFile(hFile, pBuf, iDataLen, &dwRead, NULL);
							if ( dwRead == iDataLen )
							{
								CArray<CWeightData,CWeightData&> aWeightData;
								if ( CMerch::ReadWeightDatasFromBuffer(pBuf, iDataLen, aWeightData) )
								{
									// 权重数据已经读取，开始做除权运算
									CArray<CKLine, CKLine> aKlineBk;
									if ( CMerchKLineNode::WeightKLine(aKlines, aWeightData, true, aKlineBk) )
									{
										aKlines.Copy(aKlineBk);	// 已经除权了
									}
								}
							}
							delete []pBuf;
						}
						DEL_HANDLE(hFile);
					}	
				}			
			}

			const int32 iPos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aKlines, TimeStart);
			// 终点就是数据尾部
			const int32 iKlineCount = aKlines.GetSize();
			if ( iPos <0 || iPos >= iKlineCount )
			{
				continue;
			}

			

			CKLine *pKline = aKlines.GetData();
			if ( iPos < aKlines.GetSize() )
			{
				ResCur.fHigh	= pKline[iPos].m_fPriceHigh;
				ResCur.fLow		= pKline[iPos].m_fPriceLow;
				ResCur.fAmount	= pKline[iPos].m_fAmount;
				ResCur.fVol		= pKline[iPos].m_fVolume;
			}
			for ( int32 iKlinePos=iPos+1; iKlinePos < aKlines.GetSize() ; iKlinePos++ )		// 中间的数据
			{
				const CKLine &kline = pKline[iKlinePos];
				ResCur.fVol += kline.m_fVolume;
				ResCur.fAmount += kline.m_fAmount;

				// 高，低
				ResCur.fHigh = max(ResCur.fHigh, kline.m_fPriceHigh);
				ResCur.fLow  = min(ResCur.fLow, kline.m_fPriceLow);
			}
			

			int32 iPrePos = iPos > 0 ? iPos-1 : iPos;
			ResCur.fPreClose = iPos==iPrePos ? pKline[iPos].m_fPriceOpen : pKline[iPrePos].m_fPriceClose;			// 前收
			ResCur.fClose = pKline[iKlineCount-1].m_fPriceClose;		// 今收

			// 5日量变
// 			期末成交量=SUM(VOL,5)=VOL+REF(VOL,1)+REF(VOL,2)+REF(VOL,3)+REF(VOL,4);
// 			所以期初成交量=REF(期末成交量,1*4);
// 			五日量变幅度:((期末成交量-期初成交量)/期初成交量)*100;	
// 			转化为公式应该是：
// 				期末成交量:SUM(VOL,5);
// 			期初成交量:REF(期末成交量,1*N);
// 			五日量变幅度:((期末成交量-期初成交量)/期初成交量)*100;
// 			比如000823这个股的5日量变幅度(2010-10-25 10-29一周)
// 				期末成交量:SUM(VOL,5)=3303848
// 				期初成交量:REF(期末成交量,1*4{N=4})=1009256
//			五日量变幅度:((期末成交量-期初成交量)/期初成交量)*100=227.35
			
			// 实际算: 最后5日的成交量 最初前5日成交量
			float fLast5V = 0.0f;
			const int32 iLast5VPos = max(iKlineCount-5, 0);	// 最后5日，含最后一日
			int32 i5VPos = 0;
			for ( i5VPos = iLast5VPos; i5VPos < iKlineCount ; i5VPos++ )
			{
				fLast5V += pKline[i5VPos].m_fVolume;
			}
			float fFirst5V = 0.0f;
			const int32 iFirst5VPos = max(iPos-4, 0);	// 开始日前5日，含开始日
			for ( i5VPos=iFirst5VPos; i5VPos < iFirst5VPos+5 && i5VPos < iKlineCount ; i5VPos++ )
			{
				fFirst5V += pKline[i5VPos].m_fVolume;
			}
			if ( fFirst5V > 1.0f )
			{
				ResCur.f5DayVolChangeRate = (fLast5V-fFirst5V)/fFirst5V;
			}

			// 市场比 换手率
			// 市场比需要对应的指数的成交量
			// 换手率需要商品的财务数据
			// 是否尝试自己读取？ 现在统一发消息
			// 计算换手率/市场比，向窗口询问数据
			if ( pParam->m_bTerminate )
			{
				break;
			}

			
			if ( bNeedReq
				&& !pParam->m_bTerminate
				&& (0.0f == ReqData.fCircAssert
				|| 0.0f == ReqData.fMarketAmount) )
			{
				// 尝试请求
				//SendMessage(hWnd, uMsgBase+EPSCM_RequestData, (WPARAM)pVoidParam, (LPARAM)&ReqData);
				DWORD dwResult = 0;
				if ( !SendMessageTimeout(hWnd, uMsgBase+EPSCM_RequestData, (WPARAM)pVoidParam, (LPARAM)&ReqData,
					SMTO_ABORTIFHUNG, 1500, &dwResult) )
				{
					TRACE(_T("阶段排行计算线程发送msg超时或者错误(超时:%d): %d\r\n"), ERROR_TIMEOUT, GetLastError());
					break;		// 主线程出现挂起或者超时严重
				}
			}
			if ( 0.0f != ReqData.fCircAssert )
			{
				ResCur.fTradeRate = ResCur.fVol / ReqData.fCircAssert*100; // 流通股本单位 股
			}

			if ( 0.0f != ReqData.fMarketAmount )
			{
				ResCur.fMarketRate = ResCur.fAmount / ReqData.fMarketAmount;
			}
		}

		// 赋值结果集
		pParam->m_aCalcResults.Copy(aResults);
		if ( i < aMerchSrc.GetSize() )
		{
			pParam->m_eResultFlag = EPSCRF_UserCancel;
		}
		else
		{
			pParam->m_eResultFlag = EPSCRF_OK;
		}
	}
#ifndef _DEBUG
	catch(...)
	{
		pParam->m_eResultFlag = EPSCRF_Error;
	}
#endif //_DEBUG

	if ( pParam->m_bAutoDelThread )
	{
		DEL_HANDLE(pParam->m_hThread);		// 删除这个句柄
		pParam->m_hThread = NULL;
	}
	::PostMessage(pParam->m_hWndMsg, pParam->m_uMsgBase+EPSCM_End, (WPARAM)pVoidParam, pParam->m_eResultFlag);
	return 0;
}

void CancelPhaseSortCalc( T_PhaseSortCalcParam *pParam )
{
	if ( NULL != pParam )
	{
		bool bOld = pParam->m_bAutoDelThread;
		pParam->m_bAutoDelThread = false;
		pParam->m_bTerminate = true;
		if ( NULL != pParam->m_hThread )
		{
			//WaitForSingleObject(pParam->m_hThread, INFINITE);
			DWORD dwWait = 0;
			while ( (dwWait=MsgWaitForMultipleObjects(1, &pParam->m_hThread, FALSE, INFINITE, QS_SENDMESSAGE))
				!= WAIT_OBJECT_0 )
			{
				AfxGetApp()->PumpMessage();	// 交给app处理
			}
			ClearPhaseSortCalcParamResource(pParam);
		}
		pParam->m_bAutoDelThread = bOld;
	}
}

void ClearPhaseSortCalcParamResource( T_PhaseSortCalcParam *pParam )
{
	if ( NULL != pParam && NULL != pParam->m_hThread )
	{
		DEL_HANDLE(pParam->m_hThread);
		pParam->m_hThread = NULL;
	}
}

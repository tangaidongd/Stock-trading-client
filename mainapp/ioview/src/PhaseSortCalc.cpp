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

	m_bDoPreWeight = false;	// �����������޳�Ȩ��Ϣ��Ĭ�ϲ���Ȩ
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
		return true;	// ���ü���
	}

	// ���ò�����ʼֵ
	ASSERT( NULL == pParam->m_hThread );	// ǰ��ı��뱻�����
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
	
	pThread->ResumeThread();	// ��ʼ����
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
		SaveDay(TimeStart);		// ��ʼ��Ҫȡ��
		TimeEnd = pParam->m_TimeEnd;
		//SaveDay(TimeEnd);	// �����������ݣ�ȡ��
		bool32 bNeedPreWeight = pParam->m_bDoPreWeight;
		bool32 bNeedReq = pParam->m_bNeedRequsetDataParam;
		
		HWND hWnd = pParam->m_hWndMsg;
		const UINT uMsgBase = pParam->m_uMsgBase;

		CIoViewBase::MerchArray aMerchSrc;
		aMerchSrc.Copy(pParam->m_aMerchsToCalc);	// �����Դ��Ʒ�б�

		PhaseSortCalcDataParamArray aDataParams;
		aDataParams.Copy(pParam->m_aDataParamToCalc);

		PhaseSortCalcResultArray aResults;	// ���
		aResults.SetSize(aMerchSrc.GetSize());
		T_PhaseSortCalcReslt *pRes = aResults.GetData();
		const int32 iResSize = aResults.GetSize();
		ZeroMemory(pRes, iResSize*sizeof(T_PhaseSortCalcReslt));	// �������0

		
		::PostMessage(hWnd, uMsgBase+EPSCM_Start, (WPARAM)pVoidParam, NULL);

		CGmtTimeSpan TimeSpan = TimeEnd - TimeStart;
		const int32 iMaxBaseKlineCount = TimeSpan.GetDays()+5;	// ������������Ҫ������K��

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

			::PostMessage(hWnd, uMsgBase+EPSCM_Pos, (WPARAM)pVoidParam, (LPARAM)i);		// λ��

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

			// ��ȡ�������ݿ��K������, iMaxBase + 5
			aKlines.SetSize(0, iMaxBaseKlineCount);
			pAbsDataManager->ReadOfflineKLines(iMarketId, StrMerchCode, EKTBDay, EOKTOfflineData, TimeEnd, iMaxBaseKlineCount, aKlines);

			// ���ʵʱ���۵�����С�ڵ���Ҫ������ڣ����޸Ļ���������һ����K�ߣ��������������
			// һ������£�ʵʱ����ʱ��>=���һ����K�����ݵ�ʱ��
			CGmtTime TimeRealTimePrice = ReqData.m_RealTimePrice.m_TimeCurrent.m_Time;
			CGmtTime TimeEndDay(TimeEnd);
			SaveDay(TimeRealTimePrice);
			SaveDay(TimeEndDay);
			if ( TimeRealTimePrice.GetTime() > 0 && TimeRealTimePrice <= TimeEndDay )
			{
				// Ҫ����յ�
				bool32 bAppend = true;
				const int32 iKlineCount = aKlines.GetSize();
				if ( iKlineCount > 0 && aKlines[iKlineCount-1].m_TimeCurrent == TimeRealTimePrice )
				{
					CKLine &kline = aKlines[iKlineCount-1];
					// ���̼ۺ�ʱ�䱣�ֲ���
					kline.m_fPriceClose		= ReqData.m_RealTimePrice.m_fPriceNew;		// ���̼��滻
					kline.m_fPriceAvg		= ReqData.m_RealTimePrice.m_fPriceAvg;		// �����滻
					
					// �н��յ���k�ߣ���ʵʱ�����Ƿ����ֳ�ʼ��0�������
					kline.m_fPriceHigh = max(kline.m_fPriceHigh, ReqData.m_RealTimePrice.m_fPriceHigh);	// ����K�߳��֣�
					kline.m_fPriceLow = max(kline.m_fPriceLow, ReqData.m_RealTimePrice.m_fPriceLow);
					
					//
					kline.m_fVolume			= ReqData.m_RealTimePrice.m_fVolumeTotal;			// �ɽ���
					kline.m_fAmount			= ReqData.m_RealTimePrice.m_fAmountTotal;			// �ɽ����
					kline.m_fHold			= ReqData.m_RealTimePrice.m_fHoldTotal;			// �ֲ��滻
					
					// 
					//kline.m_usFallMerchCount	= ;	// �ǵ�����֪����ô�滻
					//kline.m_usRiseMerchCount	= KLine.m_usRiseMerchCount;	
					bAppend = false;
				}
				
				if ( bAppend )
				{
					// ���һ�����յ����ߣ�
					CKLine kline;
					kline.m_TimeCurrent = ReqData.m_RealTimePrice.m_TimeCurrent.m_Time;
					kline.m_fPriceOpen	= ReqData.m_RealTimePrice.m_fPriceOpen;
					kline.m_fPriceClose		= ReqData.m_RealTimePrice.m_fPriceNew;		// ���̼��滻
					kline.m_fPriceAvg		= ReqData.m_RealTimePrice.m_fPriceAvg;		// �����滻
					
					// �н��յ���k�ߣ���ʵʱ�����Ƿ����ֳ�ʼ��0�������
					kline.m_fPriceHigh  = ReqData.m_RealTimePrice.m_fPriceHigh;	// ����K�߳��֣�
					kline.m_fPriceLow	= ReqData.m_RealTimePrice.m_fPriceLow;
					
					//
					kline.m_fVolume			= ReqData.m_RealTimePrice.m_fVolumeTotal;			// �ɽ���
					kline.m_fAmount			= ReqData.m_RealTimePrice.m_fAmountTotal;			// �ɽ����
					kline.m_fHold			= ReqData.m_RealTimePrice.m_fHoldTotal;			// �ֲ��滻
					
					// 
					//kline.m_usFallMerchCount	= ;	// �ǵ�����֪����ô�滻
					//kline.m_usRiseMerchCount	= KLine.m_usRiseMerchCount;

					aKlines.Add(kline);
				}
			}

			// ��ȡ��������
			if ( bNeedPreWeight && aKlines.GetSize() > 0 )	// ��Ҫ�����ݰ�
			{
				// ȡ�ļ�·��:
				CString StrFilePath;
				if ( pAbsDataManager->GetPublishFilePath(EPFTWeight, iMarketId , StrMerchCode, StrFilePath) )
				{
					// ������:
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
									// Ȩ�������Ѿ���ȡ����ʼ����Ȩ����
									CArray<CKLine, CKLine> aKlineBk;
									if ( CMerchKLineNode::WeightKLine(aKlines, aWeightData, true, aKlineBk) )
									{
										aKlines.Copy(aKlineBk);	// �Ѿ���Ȩ��
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
			// �յ��������β��
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
			for ( int32 iKlinePos=iPos+1; iKlinePos < aKlines.GetSize() ; iKlinePos++ )		// �м������
			{
				const CKLine &kline = pKline[iKlinePos];
				ResCur.fVol += kline.m_fVolume;
				ResCur.fAmount += kline.m_fAmount;

				// �ߣ���
				ResCur.fHigh = max(ResCur.fHigh, kline.m_fPriceHigh);
				ResCur.fLow  = min(ResCur.fLow, kline.m_fPriceLow);
			}
			

			int32 iPrePos = iPos > 0 ? iPos-1 : iPos;
			ResCur.fPreClose = iPos==iPrePos ? pKline[iPos].m_fPriceOpen : pKline[iPrePos].m_fPriceClose;			// ǰ��
			ResCur.fClose = pKline[iKlineCount-1].m_fPriceClose;		// ����

			// 5������
// 			��ĩ�ɽ���=SUM(VOL,5)=VOL+REF(VOL,1)+REF(VOL,2)+REF(VOL,3)+REF(VOL,4);
// 			�����ڳ��ɽ���=REF(��ĩ�ɽ���,1*4);
// 			�����������:((��ĩ�ɽ���-�ڳ��ɽ���)/�ڳ��ɽ���)*100;	
// 			ת��Ϊ��ʽӦ���ǣ�
// 				��ĩ�ɽ���:SUM(VOL,5);
// 			�ڳ��ɽ���:REF(��ĩ�ɽ���,1*N);
// 			�����������:((��ĩ�ɽ���-�ڳ��ɽ���)/�ڳ��ɽ���)*100;
// 			����000823����ɵ�5���������(2010-10-25 10-29һ��)
// 				��ĩ�ɽ���:SUM(VOL,5)=3303848
// 				�ڳ��ɽ���:REF(��ĩ�ɽ���,1*4{N=4})=1009256
//			�����������:((��ĩ�ɽ���-�ڳ��ɽ���)/�ڳ��ɽ���)*100=227.35
			
			// ʵ����: ���5�յĳɽ��� ���ǰ5�ճɽ���
			float fLast5V = 0.0f;
			const int32 iLast5VPos = max(iKlineCount-5, 0);	// ���5�գ������һ��
			int32 i5VPos = 0;
			for ( i5VPos = iLast5VPos; i5VPos < iKlineCount ; i5VPos++ )
			{
				fLast5V += pKline[i5VPos].m_fVolume;
			}
			float fFirst5V = 0.0f;
			const int32 iFirst5VPos = max(iPos-4, 0);	// ��ʼ��ǰ5�գ�����ʼ��
			for ( i5VPos=iFirst5VPos; i5VPos < iFirst5VPos+5 && i5VPos < iKlineCount ; i5VPos++ )
			{
				fFirst5V += pKline[i5VPos].m_fVolume;
			}
			if ( fFirst5V > 1.0f )
			{
				ResCur.f5DayVolChangeRate = (fLast5V-fFirst5V)/fFirst5V;
			}

			// �г��� ������
			// �г�����Ҫ��Ӧ��ָ���ĳɽ���
			// ��������Ҫ��Ʒ�Ĳ�������
			// �Ƿ����Լ���ȡ�� ����ͳһ����Ϣ
			// ���㻻����/�г��ȣ��򴰿�ѯ������
			if ( pParam->m_bTerminate )
			{
				break;
			}

			
			if ( bNeedReq
				&& !pParam->m_bTerminate
				&& (0.0f == ReqData.fCircAssert
				|| 0.0f == ReqData.fMarketAmount) )
			{
				// ��������
				//SendMessage(hWnd, uMsgBase+EPSCM_RequestData, (WPARAM)pVoidParam, (LPARAM)&ReqData);
				DWORD dwResult = 0;
				if ( !SendMessageTimeout(hWnd, uMsgBase+EPSCM_RequestData, (WPARAM)pVoidParam, (LPARAM)&ReqData,
					SMTO_ABORTIFHUNG, 1500, &dwResult) )
				{
					TRACE(_T("�׶����м����̷߳���msg��ʱ���ߴ���(��ʱ:%d): %d\r\n"), ERROR_TIMEOUT, GetLastError());
					break;		// ���̳߳��ֹ�����߳�ʱ����
				}
			}
			if ( 0.0f != ReqData.fCircAssert )
			{
				ResCur.fTradeRate = ResCur.fVol / ReqData.fCircAssert*100; // ��ͨ�ɱ���λ ��
			}

			if ( 0.0f != ReqData.fMarketAmount )
			{
				ResCur.fMarketRate = ResCur.fAmount / ReqData.fMarketAmount;
			}
		}

		// ��ֵ�����
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
		DEL_HANDLE(pParam->m_hThread);		// ɾ��������
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
				AfxGetApp()->PumpMessage();	// ����app����
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

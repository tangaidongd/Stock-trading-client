#include "StdAfx.h"

//#include "ShareFun.h"

#include "datamanager.h"

#include <afxconv.h>
#include <process.h>
#include "PlugInStruct.h"
#include "TraceLog.h"
#include "commmanager.h"
#include "AbsOfflineDataManager.h"
// fangz0301
static CString GetTimeString()
{
	CTime Time = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(L"%02d:%02d:%02d", Time.GetHour(), Time.GetMinute(), Time.GetSecond());
	
	return StrTime;
}

////////////////////////////////////////////////////////////////////////
// ��ʱ
const uint32 KReqTimeOutMSecond						= 1000 * 30 * 4;		// ����Ӧ��ʱʱ��
const uint32 KReqWaitingTimeOutMSecond				= 20;				// ����������ʱ��

////////////////////////////////////////////////////////////////////////
// ��ʱ����ʱ�䲻��Ӧ������
const int32 KTimerIdClearMmiReqList					= 1;
const int32 KTimerPeriodClearMmiReqList				= 1000 * 5;		// ms unit

// ��ʱ������ 3 ����ʱ��, ֪ͨ�ϲ㳤ʱ��δ�յ�����, ����
const uint32 KiTimeOutReqsForPrompt					= 3;

// �г��� 5 ��û�صİ�, �������ϲ�
const uint32 KiTimeForPrompt						= 1000 * 5;	

// ��ʱ�����ѻ���������
const int32 KTimerIdProcessOfflineReq				= 2;
const int32 KTimerPeriodProcessOfflineReq			= 30;			// ms unit

// ��ʱ���������������
const int32 KTimerIdProcessMmiReqWaitingList		= 3;
const int32 KTimerPeriodProcessMmiReqWaitingList	= 16;			// ms unit

// ��������ʱ�� ��������ĳ��������������������������
const uint32 KReqKLineGrowCount						= 20;			// ����K�ߵ�ȡ������
const uint32 KReqTimeSalesGrowCount					= 20;			// ����ֱʵ�ȡ������

////////////////////////////////////////////////////////////////////////////////
//

CMmiReqWaitingNode::CMmiReqWaitingNode()
:m_pMmiCommReq(NULL)
{
	m_uiTimeRequest = timeGetTime();
}

CMmiReqWaitingNode::~CMmiReqWaitingNode()
{
	DEL(m_pMmiCommReq);
}


////////////////////////////////////////////////////////////////////////////////
//

CMmiReqNode::CMmiReqNode()
:m_pMmiCommReq(NULL)
{
	m_uiTimeRequest = timeGetTime();
}

CMmiReqNode::~CMmiReqNode()
{
	DEL(m_pMmiCommReq);
}


////////////////////////////////////////////////////////////////////////////////
//

CMmiRespNode::CMmiRespNode()
:m_pMmiCommResp(NULL)
{
	m_uiTimeResponse	= timeGetTime();

	m_iMmiRespId		= -1;
	m_iCommunicationId	= -1;
}

CMmiRespNode::~CMmiRespNode()
{
	DEL(m_pMmiCommResp);
}

////////////////////////////////////////////////////////////////////////////////
//
CDataManager::CDataManager(CAbsCommManager *pCommManager, CAbsOfflineDataManager &OfflineDataManager)
:m_pCommManager(pCommManager),
m_OfflineDataManager(OfflineDataManager)
{
	m_bThreadExit			= true;
	m_iLastMmiReqId			= 10001;	// zhangbo 0513 #���ԣ� ���⽫��ֵ����Ϊһ���Ƚϴ��ֵ ������ͨѶ�����	
	m_bHasSyncServerTime	= false;
	m_eForceDataSource		= EDSCounts;
	m_hwndTrace				= NULL;

	m_hThreadOffLineKLine	= NULL;
	m_hThreadOffLineTick	= NULL;
	m_hThreadOffLineRealtimePrice = NULL;

	m_aOffLineKLines.RemoveAll();
	m_aOffLineTicks.RemoveAll();
	m_aOffLineRealtimePrice.RemoveAll();
	m_hEventThreadOffLineTick = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEventThreadOffLineKLine = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hThreadProcessOffLineRealtimePrice = CreateEvent(NULL,TRUE,FALSE,NULL);
}

CDataManager::~CDataManager()
{
	m_bThreadExit = true;
	WaitForSingleObject(m_hEventThreadOffLineTick,INFINITE);
	WaitForSingleObject(m_hEventThreadOffLineKLine,INFINITE);
	WaitForSingleObject(m_hThreadProcessOffLineRealtimePrice,INFINITE);


	m_pDataManagerNotifyPtrList.RemoveAll();

	StopTimer(KTimerIdClearMmiReqList);
	StopTimer(KTimerIdProcessOfflineReq);
	StopTimer(KTimerIdProcessMmiReqWaitingList);
	
	RemoveAllWaitingReq();
	RemoveAllReq();
	RemoveAllResp();
}

void CDataManager::GetLocalTime(CString &strTime)
{
	CTime currentTime = CTime::GetCurrentTime();
	strTime = currentTime.Format(_T("%H:%M:%S"));
}

bool32 CDataManager::Construct()
{
#ifdef TRACE_DLG
	// zhangbo 1022 #for trace
	CString StrHwnd;
	GetPrivateProfileStringGGTong(L"TRACE", L"HWND", L"0", StrHwnd,		L"./trace.ini");
	m_hwndTrace = (HWND)_wtoi(StrHwnd);
#endif

	// �����ѻ����ݴ����߳�
	if ( !BeginThreadProcessOffLineData() )
	{
		ASSERT(0);
		m_bThreadExit  = true;		
		return false;
	}

	//
	StartTimer(KTimerIdClearMmiReqList, KTimerPeriodClearMmiReqList);
	StartTimer(KTimerIdProcessOfflineReq, KTimerPeriodProcessOfflineReq);
	StartTimer(KTimerIdProcessMmiReqWaitingList, KTimerPeriodProcessMmiReqWaitingList);
	
	// 
	return true;
}

void CDataManager::AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify)
{
	if (NULL == pDataManagerNotify)
		return;
	
	bool32 bFind = FALSE;
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		if (m_pDataManagerNotifyPtrList[i] == pDataManagerNotify)
		{
			bFind = TRUE;
			break;
		}
	}
	
	if (!bFind)
	{
		m_pDataManagerNotifyPtrList.Add(pDataManagerNotify);
	}
}

void CDataManager::RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify)
{
	if (NULL == pDataManagerNotify)
		return;
	
	// �ҵ����е�, �޳����������ظ��������
	while (1)
	{
		int32 iFindPos = -1;
		for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
		{
			if (m_pDataManagerNotifyPtrList[i] == pDataManagerNotify)
			{
				iFindPos = i;
				break;
			}
		}
		
		if (iFindPos >= 0)
		{
			m_pDataManagerNotifyPtrList.RemoveAt(iFindPos, 1);
		}
		else
		{
			break;
		}
	}
}

bool32 CDataManager::BeginThreadProcessOffLineData()
{	
	m_bThreadExit = false;

	// K ��
	m_hThreadOffLineKLine = (void*)_beginthread(ThreadProcessOffLineKLine, 0, this);
	if ( NULL == m_hThreadOffLineKLine )
	{
		ASSERT(0);
		return false;
	}

	// Tick
	m_hThreadOffLineTick = (void*)_beginthread(ThreadProcessOffLineTick, 0, this);
	if ( NULL == m_hThreadOffLineTick )
	{
		ASSERT(0);
		return false;
	}

	// RealtimePrice
	m_hThreadOffLineRealtimePrice = (void*)_beginthread(ThreadProcessOffLineRealtimePrice, 0, this);
	if ( NULL == m_hThreadOffLineRealtimePrice )
	{
		ASSERT(0);
		return false;
	}

	//
	return true;
}

void CDataManager::ThreadProcessOffLineKLine(LPVOID lParam)
{
	CDataManager* pThis = (CDataManager*)lParam;

	while ( !pThis->m_bThreadExit )
	{
		pThis->ThreadProcessOffLineKLine();
		Sleep(5);
	}
	SetEvent(pThis->m_hEventThreadOffLineKLine);
}

void CDataManager::ThreadProcessOffLineKLine()
{	
	CArray<T_OffLineKLineParam, T_OffLineKLineParam &> aTmp;
	{
		RGUARD(LockSingle, m_LockOffLineKLine, LockOffLineKLine);
		aTmp.Copy(m_aOffLineKLines);
		m_aOffLineKLines.RemoveAll();
	}
	// 
	int32 iSizeKLine = aTmp.GetSize();
	
	//	
	T_OffLineKLineParam* pOffLineKLine = (T_OffLineKLineParam*)aTmp.GetData();
	
	if ( 0 == iSizeKLine || NULL == pOffLineKLine )
	{
		return;
	}
	
	// д�ѻ�����
	for ( int32 i = 0; i < iSizeKLine; i++ )
	{
		m_OfflineDataManager.WriteOfflineKLines(pOffLineKLine[i].m_iMarketId, pOffLineKLine[i].m_StrMerchCode, pOffLineKLine[i].m_eKLineTypeBase, EOKTVipData, pOffLineKLine[i].m_aKLines);
	}
	
	// ��ն���
	aTmp.RemoveAll();
}

void CDataManager::PushOffLineKLineList(T_OffLineKLineParam& stOffLineKLineParam)
{
	RGUARD(LockSingle, m_LockOffLineKLine, LockOffLineKLine);
	m_aOffLineKLines.Add(stOffLineKLineParam);
}

void CDataManager::ThreadProcessOffLineTick(LPVOID lParam)
{
	CDataManager* pThis = (CDataManager*)lParam;
	while ( !pThis->m_bThreadExit )
	{
		pThis->ThreadProcessOffLineTick();
		Sleep(5);
	}
	SetEvent(pThis->m_hEventThreadOffLineTick);
}

void CDataManager::ThreadProcessOffLineTick()
{
	CArray<T_OffLineTickParam, T_OffLineTickParam &> aTmp;
	{
		RGUARD(LockSingle, m_LockOffLineTick, LockOffLineTick);
		aTmp.Copy(m_aOffLineTicks);
		m_aOffLineTicks.RemoveAll();
	}
	
	//
	int32 iSizeTick = aTmp.GetSize();
	T_OffLineTickParam* pOffLineTick = (T_OffLineTickParam*)aTmp.GetData();
	
	if ( 0 == iSizeTick || NULL == pOffLineTick )
	{
		return;
	}
	
	// д�ѻ�����
	for ( int32 i = 0; i < iSizeTick; i++ )
	{
		m_OfflineDataManager.WriteOfflineTicks(pOffLineTick[i].m_iMarketId, pOffLineTick[i].m_StrMerchCode, pOffLineTick[i].m_aTicks);
	}
	
	// ���
	aTmp.RemoveAll();
}

void CDataManager::PushOffLineTickList(T_OffLineTickParam& stOffLineTickParam)
{
	RGUARD(LockSingle, m_LockOffLineTick, LockOffLineTick);
	m_aOffLineTicks.Add(stOffLineTickParam);
}

void CDataManager::ThreadProcessOffLineRealtimePrice(LPVOID lParam)
{
	CDataManager* pThis = (CDataManager*)lParam;
	while ( !pThis->m_bThreadExit )
	{
		pThis->ThreadProcessOffLineRealtimePrice();
		Sleep(5);
	}
	SetEvent(pThis->m_hThreadProcessOffLineRealtimePrice);
}

void CDataManager::ThreadProcessOffLineRealtimePrice()
{	
	CArray<T_OffLineRealtimePriceParam, T_OffLineRealtimePriceParam&>	aTmp;
	{
		RGUARD(LockSingle, m_LockOffLineRealtimePrice, LockOffLineRealtimePrice);
		aTmp.SetSize(0, aTmp.GetSize());
		for ( int32 i=0; i<aTmp.GetSize(); ++i )
		{
			aTmp.Add( m_aOffLineRealtimePrice.ElementAt(i) );
		}
		m_aOffLineRealtimePrice.RemoveAll();
	}
	
	//
	int32 iSize = aTmp.GetSize();
	
	//
	T_OffLineRealtimePriceParam* pOff = (T_OffLineRealtimePriceParam*)aTmp.GetData();
	if ( 0 >= iSize || NULL == pOff )
	{
		return;
	}
	
	// д����
	for ( int32 i = 0; i < iSize; ++i )
	{
		m_OfflineDataManager.WriteOfflineRealtimePrice(pOff[i].m_RealtimePrice);
	}
	
	//
	aTmp.RemoveAll();
}

void CDataManager::PushOffLineRealtimePriceList(T_OffLineRealtimePriceParam& stOffLineRealtimePriceParam)
{
	RGUARD(LockSingle, m_LockOffLineRealtimePrice, LockOffLineRealtimePrice);
	m_aOffLineRealtimePrice.Add(stOffLineRealtimePriceParam);
}

// �������ݣ� �����ݿ������ѻ������У� Ҳ���ܴ����ڷ�����
// ���ᴦ������ <0 ��ʾ����ʧ�� =0 ��������� >0 �Ѿ����뷢�Ͷ���
int32 CDataManager::RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource, int32 iCommunicationId)
{
	ASSERT(NULL != pMmiCommReq);
	if (NULL == pMmiCommReq)
	{
		return -1;
	}
	ASSERT(EDSOfflineData == eDataSource || EDSCommunication == eDataSource || EDSVipData == eDataSource || EDSAuto == eDataSource);

	//
	aMmiReqNodes.RemoveAll();

	//
	if ( EDSCounts != m_eForceDataSource )
	{		
		eDataSource = m_eForceDataSource;
	}

	//
	bool32 bRequestDataImmediately = false;

	if (EDSCommunication == eDataSource)	// ���ڱ������������������ݣ� ����Ҫ�����������У� ֱ�Ӷ���������
	{
		bRequestDataImmediately = true;
	}
	else 
	{
		if ( EDSOfflineData == eDataSource )
		{
			if ( ECTReqMerchKLine == pMmiCommReq->m_eCommType || ECTReqRealtimePrice == pMmiCommReq->m_eCommType )
			{
				bRequestDataImmediately = true;
			}	
			else
			{
				return -1;
			}
		}
		else 
		{
			if (ECTReqMerchKLine != pMmiCommReq->m_eCommType &&	ECTReqMerchTimeSales != pMmiCommReq->m_eCommType)// ����ʷ������Ҫ���뷢�Ͷ���				
			{
				bRequestDataImmediately = true;		
			}
		}
	}
		

	// 
	if (bRequestDataImmediately)
	{
		// ��������������
		RequestDataImmediately(pMmiCommReq, aMmiReqNodes, eDataSource, iCommunicationId);

		if (aMmiReqNodes.GetSize() > 0)
			return 1;		// �Ѿ�����������
		else
			return -1;		// ����ʧ��
	}
	else
	{
		PushReqIntoWaitingQueue(pMmiCommReq, eDataSource, iCommunicationId);
		return 0;	// ���������
	}

	return -1;
}

void CDataManager::RequestDataImmediately(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource, int32 iCommunicationId)
{
	ASSERT(NULL != pMmiCommReq);
	if (NULL == pMmiCommReq)
	{
		return;
	}
	ASSERT(EDSOfflineData == eDataSource || EDSCommunication == eDataSource || EDSVipData == eDataSource || EDSAuto == eDataSource);
	
	//
	aMmiReqNodes.RemoveAll();
	
	// 
	// RGUARD(LockSingle, m_LockReqList, LockReqList);
	{
		// 
		if (EDSCommunication == eDataSource)	// �����������������
		{
			CMmiReqNode *pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromCommunication(pMmiCommReq, pMmiReqNodeOut, iCommunicationId))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else if (EDSVipData == eDataSource)	// ����vipdata�����л�ȡ
		{
			CMmiReqNode *pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromVipData(pMmiCommReq, pMmiReqNodeOut))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else if ( EDSOfflineData == eDataSource ) // ���������ѻ�������ȡ
		{
			CMmiReqNode* pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromOfflineData(pMmiCommReq, pMmiReqNodeOut))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else// if (EDSAuto == eDataSource)		// �Զ���ʽ�� ���ѻ����ݣ� �����ѻ����ݣ� �ѻ������ģ� �ٴӷ�����ȡ�������ѻ������У� �ٴ��ѻ�������ȡ������
		{
			if (ECTReqMerchKLine == pMmiCommReq->m_eCommType)
			{
				CMmiReqMerchKLine *pMmiReqMerchKLine = (CMmiReqMerchKLine *)pMmiCommReq;
				
				CGmtTime TimeStartOfflineKLine, TimeEndOfflineKLine;
				int32 iOfflineKLineCount = 0;
				
				bool32 bGetOfflineKLineSnapshotOk = m_OfflineDataManager.GetOfflineKLinesSnapshot(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, EOKTVipData, TimeStartOfflineKLine, TimeEndOfflineKLine, iOfflineKLineCount);
				if (!bGetOfflineKLineSnapshotOk)
				{
					// ���ѻ��ļ��ж��������ݣ� ֱ�ӷ��͸������������
					CMmiReqNode *pMmiReqNodeOut = NULL;
					if (RequestDataOnlyFromCommunication(pMmiCommReq, pMmiReqNodeOut, iCommunicationId))
					{
						aMmiReqNodes.Add(pMmiReqNodeOut);
					}
				}
				else
				{
					CMmiReqMerchKLine Req = *pMmiReqMerchKLine;
					if (ERTYSpecifyTime == pMmiReqMerchKLine->m_eReqTimeType)
					{
						if (Req.m_TimeStart > Req.m_TimeEnd)
						{
							CGmtTime TimeTemp	= Req.m_TimeStart;
							Req.m_TimeStart		= Req.m_TimeEnd;
							Req.m_TimeEnd		= TimeTemp;
						}
						
						if (Req.m_TimeStart < TimeStartOfflineKLine)
						{
							if (Req.m_TimeEnd < TimeStartOfflineKLine)  
							{
								// ��ȫ�ڲ����ѻ��������棬 �������� ����
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}		
							else if (Req.m_TimeEnd > TimeEndOfflineKLine)
							{
								// �м�һ���ڱ��ش���
								{
									Req.m_TimeStart = TimeStartOfflineKLine;
									Req.m_TimeEnd = TimeEndOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �������
								{
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �Ҷ�����
								{
									Req.m_TimeStart = TimeEndOfflineKLine;
									Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
							else	// 
							{
								// �ұ�һ���ڱ��ش���
								{
									Req.m_TimeStart = TimeStartOfflineKLine;
									Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �������
								{
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else if (Req.m_TimeStart >= TimeStartOfflineKLine && Req.m_TimeStart <= TimeEndOfflineKLine)
						{
							if (Req.m_TimeEnd > TimeEndOfflineKLine)
							{
								// ���һ���ڱ��ش���
								{
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeStart;
									Req.m_TimeEnd = TimeEndOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �Ҷ�����
								{
									Req.m_TimeStart = TimeEndOfflineKLine;
									Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
							else
							{
								// ��ȫ���ѻ��������棬 ������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
						}
						else // if (Req.m_TimeStart > TimeEndOfflineKLine)
						{
							// ��ȫ�ڲ����ѻ���������
							// �˴�Ϊ�˱����ѻ��ļ��е����ݵ������ԣ� �᳢�Խ������������ѻ����ݴ��
							Req.m_eReqTimeType = ERTYSpecifyTime;
							Req.m_TimeStart = TimeEndOfflineKLine;
							Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeEnd;
							
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
					}
					else if (ERTYFrontCount == pMmiReqMerchKLine->m_eReqTimeType)
					{
	DO_KLINE_FRONT_COUNT:
						// ���ֻ�������������
						if ((unsigned)Req.m_iFrontCount % KReqKLineGrowCount != 0)	
						{
							Req.m_iFrontCount += (KReqKLineGrowCount - ((unsigned)Req.m_iFrontCount % KReqKLineGrowCount));
						}

						// �ָ������
						if (Req.m_TimeSpecify < TimeStartOfflineKLine)
						{
							// ��ȫ�ڲ����ѻ��������棬 �������� ����
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineKLine && Req.m_TimeSpecify <= TimeEndOfflineKLine)
						{
							// �����ж����������ڱ��ش��ڵ�
							CArray<CKLine, CKLine> aKLines;
							m_OfflineDataManager.ReadOfflineKLines(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_eKLineTypeBase, EOKTVipData, Req.m_TimeSpecify, -Req.m_iFrontCount, aKLines);
							if (aKLines.GetSize() >= Req.m_iFrontCount)	
							{
								// ���������㹻������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// �Ҷ��ڱ���
								{
									Req.m_eReqTimeType = ERTYSpecifyTime;
									Req.m_TimeStart = TimeStartOfflineKLine;
									Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeSpecify;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}

								// �����������
								{
									Req.m_eReqTimeType = ERTYFrontCount;
									Req.m_TimeSpecify = TimeStartOfflineKLine;
									Req.m_iFrontCount = pMmiReqMerchKLine->m_iFrontCount - aKLines.GetSize();

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else
						{
							// �ر�˵����
							// ���ｫ����ֳ�������
							// 1. ��������ʱ��ͱ���ʱ��֮��Ŀ�ȱ����
							// 2. ���Ա����ļ������ʱ����ԭ��������ʱ������
							// ���������ܿ��ܻ����һ�����˷ѣ� ��Ϊ��ȷ��1��������ж������ݷ��أ� ������������һ�ε��������Ǽ��ٵģ� ���ԣ� �˷�Ҳ�����޵�
							// ����������ܳ�����ñ����ѻ�������
							
							// 1
							if (Req.m_TimeSpecify > TimeEndOfflineKLine)
							{
								Req.m_eReqTimeType = ERTYSpecifyTime;
								Req.m_TimeStart = TimeEndOfflineKLine;
								Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeSpecify;

								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							
							// 2
							Req.m_eReqTimeType = ERTYFrontCount;
							Req.m_TimeSpecify = TimeEndOfflineKLine;
							Req.m_iFrontCount = pMmiReqMerchKLine->m_iFrontCount;
							goto DO_KLINE_FRONT_COUNT;
						}
					}
					else if (ERTYCount == pMmiReqMerchKLine->m_eReqTimeType)
					{
	DO_REQ_KLINE_COUNT:
						// ���ֻ�������������
						if ((unsigned)Req.m_iCount % KReqKLineGrowCount != 0)	
						{
							Req.m_iCount += (KReqKLineGrowCount - ((unsigned)Req.m_iCount % KReqKLineGrowCount));
						}

						// �ָ������
						if (Req.m_TimeSpecify > TimeEndOfflineKLine)
						{
							// ��ȫ�ڲ����ѻ��������棬 �������� ����
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineKLine && Req.m_TimeSpecify <= TimeEndOfflineKLine)
						{
							// �����ж����������ڱ��ش��ڵ�
							CArray<CKLine, CKLine> aKLines;
							m_OfflineDataManager.ReadOfflineKLines(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_eKLineTypeBase, EOKTVipData, Req.m_TimeSpecify, Req.m_iCount, aKLines);
							if (aKLines.GetSize() >= Req.m_iCount)	
							{
								// ���������㹻������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// ����ڱ���
								{
									Req.m_eReqTimeType = ERTYSpecifyTime;
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeSpecify;
									Req.m_TimeEnd = TimeEndOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}

								// �Ҷ���������
								{
									Req.m_eReqTimeType = ERTYCount;
									Req.m_TimeSpecify = TimeEndOfflineKLine;
									Req.m_iCount = pMmiReqMerchKLine->m_iCount - aKLines.GetSize();

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else
						{
							// �ر�˵����
							// ���ｫ����ֳ�������
							// 1. ��������ʱ��ͱ���ʱ��֮��Ŀ�ȱ����
							// 2. ���Ա����ļ�����ǰʱ����ԭ����ָ��ʱ������
							// ���������ܿ��ܻ����һ�����˷ѣ� ��Ϊ��ȷ��1��������ж������ݷ��أ� ������������һ�ε��������Ǽ��ٵģ� ���ԣ� �˷�Ҳ�����޵�
							// ����������ܳ�����ñ����ѻ�������
							
							// 1
							if (Req.m_TimeSpecify < TimeStartOfflineKLine)
							{
								Req.m_eReqTimeType = ERTYSpecifyTime;
								Req.m_TimeStart = pMmiReqMerchKLine->m_TimeSpecify;
								Req.m_TimeEnd = TimeStartOfflineKLine;

								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							
							// 2
							Req.m_eReqTimeType = ERTYCount;
							Req.m_TimeSpecify = TimeStartOfflineKLine;
							Req.m_iCount = pMmiReqMerchKLine->m_iCount;
							goto DO_REQ_KLINE_COUNT;
						}
					}
				}
			}
			else if (ECTReqMerchTimeSales == pMmiCommReq->m_eCommType)
			{
				CMmiReqMerchTimeSales *pMmiReqMerchTimeSales = (CMmiReqMerchTimeSales *)pMmiCommReq;
				
				CGmtTime TimeStartOfflineTicks, TimeEndOfflineTicks;
				int32 iOfflineTicksCount = 0;
				
				if (!m_OfflineDataManager.GetOfflineTicksSnapshot(pMmiReqMerchTimeSales->m_iMarketId, pMmiReqMerchTimeSales->m_StrMerchCode, TimeStartOfflineTicks, TimeEndOfflineTicks, iOfflineTicksCount))
				{
					// ���ѻ��ļ��ж��������ݣ� ֱ�ӷ��͸������������
					CMmiReqNode *pMmiReqNodeOut = NULL;
					if (RequestDataOnlyFromCommunication(pMmiCommReq, pMmiReqNodeOut, iCommunicationId))
					{
						aMmiReqNodes.Add(pMmiReqNodeOut);
					}
				}
				else
				{
					CMmiReqMerchTimeSales Req = *pMmiReqMerchTimeSales;
					if (ERTYSpecifyTime == pMmiReqMerchTimeSales->m_eReqTimeType)
					{
						if (Req.m_TimeStart > Req.m_TimeEnd)
						{
							CGmtTime TimeTemp	= Req.m_TimeStart;
							Req.m_TimeStart		= Req.m_TimeEnd;
							Req.m_TimeEnd		= TimeTemp;
						}
						
						if (Req.m_TimeStart < TimeStartOfflineTicks)
						{
							if (Req.m_TimeEnd < TimeStartOfflineTicks)  
							{
								// ��ȫ�ڲ����ѻ��������棬 �������� ����
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}		
							else if (Req.m_TimeEnd > TimeEndOfflineTicks)
							{
								// �м�һ���ڱ��ش���
								{
									Req.m_TimeStart = TimeStartOfflineTicks;
									Req.m_TimeEnd = TimeEndOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �������
								{
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �Ҷ�����
								{
									Req.m_TimeStart = TimeEndOfflineTicks;
									Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
							else	// 
							{
								// �ұ�һ���ڱ��ش���
								{
									Req.m_TimeStart = TimeStartOfflineTicks;
									Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �������
								{
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else if (Req.m_TimeStart >= TimeStartOfflineTicks && Req.m_TimeStart <= TimeEndOfflineTicks)
						{
							if (Req.m_TimeEnd > TimeEndOfflineTicks)
							{
								// ���һ���ڱ��ش���
								{
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeStart;
									Req.m_TimeEnd = TimeEndOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// �Ҷ�����
								{
									Req.m_TimeStart = TimeEndOfflineTicks;
									Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
							else
							{
								// ��ȫ���ѻ��������棬 ������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
						}
						else // if (Req.m_TimeStart > TimeEndOfflineTicks)
						{
							// ��ȫ�ڲ����ѻ���������
							// �˴�Ϊ�˱����ѻ��ļ��е����ݵ������ԣ� �᳢�Խ������������ѻ����ݴ��
							Req.m_eReqTimeType = ERTYSpecifyTime;
							Req.m_TimeStart = TimeEndOfflineTicks;
							Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeEnd;

							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
					}
					else if (ERTYFrontCount == pMmiReqMerchTimeSales->m_eReqTimeType)
					{
	DO_TIMESALE_FRONT_COUNT:
						// ���ֻ�������������
						if ((unsigned)Req.m_iFrontCount % KReqTimeSalesGrowCount != 0)	
						{
							Req.m_iFrontCount += (KReqTimeSalesGrowCount - ((unsigned)Req.m_iFrontCount % KReqTimeSalesGrowCount));
						}

						// �ָ������
						if (Req.m_TimeSpecify < TimeStartOfflineTicks)
						{
							// ��ȫ�ڲ����ѻ��������棬 �������� ����
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineTicks && Req.m_TimeSpecify <= TimeEndOfflineTicks)
						{
							// �����ж����������ڱ��ش��ڵ�
							CArray<CTick, CTick> aTicks;
							m_OfflineDataManager.ReadOfflineTicks(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_TimeSpecify, -Req.m_iFrontCount, aTicks);
							if (aTicks.GetSize() >= Req.m_iFrontCount)	
							{
								// ���������㹻������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// �Ҷ��ڱ���
								{
									Req.m_eReqTimeType = ERTYSpecifyTime;
									Req.m_TimeStart = TimeStartOfflineTicks;
									Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeSpecify;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}

								// �����������
								{
									Req.m_eReqTimeType = ERTYFrontCount;
									Req.m_TimeSpecify = TimeStartOfflineTicks;
									Req.m_iFrontCount = pMmiReqMerchTimeSales->m_iFrontCount - aTicks.GetSize();

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else
						{
							// �ر�˵����
							// ���ｫ����ֳ�������
							// 1. ��������ʱ��ͱ���ʱ��֮��Ŀ�ȱ����
							// 2. ���Ա����ļ������ʱ����ԭ��������ʱ������
							// ���������ܿ��ܻ����һ�����˷ѣ� ��Ϊ��ȷ��1��������ж������ݷ��أ� ������������һ�ε��������Ǽ��ٵģ� ���ԣ� �˷�Ҳ�����޵�
							// ����������ܳ�����ñ����ѻ�������
							
							// 1
							if (Req.m_TimeSpecify > TimeEndOfflineTicks)
							{
								Req.m_eReqTimeType = ERTYSpecifyTime;
								Req.m_TimeStart = TimeEndOfflineTicks;
								Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeSpecify;
								
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							
							// 2
							Req.m_eReqTimeType = ERTYFrontCount;
							Req.m_TimeSpecify = TimeEndOfflineTicks;
							Req.m_iFrontCount = pMmiReqMerchTimeSales->m_iFrontCount;
							goto DO_TIMESALE_FRONT_COUNT;
						}
					}
					else if (ERTYCount == pMmiReqMerchTimeSales->m_eReqTimeType)
					{
	DO_REQ_TIMESALE_COUNT:
						// ���ֻ�������������
						if ((unsigned)Req.m_iCount % KReqTimeSalesGrowCount != 0)	
						{
							Req.m_iCount += (KReqTimeSalesGrowCount - ((unsigned)Req.m_iCount % KReqTimeSalesGrowCount));
						}

						// �ָ������
						if (Req.m_TimeSpecify > TimeEndOfflineTicks)
						{
							// ��ȫ�ڲ����ѻ��������棬 �������� ����
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineTicks && Req.m_TimeSpecify <= TimeEndOfflineTicks)
						{
							// �����ж����������ڱ��ش��ڵ�
							CArray<CTick, CTick> aTicks;
							m_OfflineDataManager.ReadOfflineTicks(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_TimeSpecify, Req.m_iCount, aTicks);
							if (aTicks.GetSize() >= Req.m_iCount)	
							{
								// ���������㹻������
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// ����ڱ���
								{
									Req.m_eReqTimeType = ERTYSpecifyTime;
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeSpecify;
									Req.m_TimeEnd = TimeEndOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}

								// �Ҷ���������
								{
									Req.m_eReqTimeType = ERTYCount;
									Req.m_TimeSpecify = TimeEndOfflineTicks;
									Req.m_iCount = pMmiReqMerchTimeSales->m_iCount - aTicks.GetSize();
									
									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
							}
						}
						else
						{
							// �ر�˵����
							// ���ｫ����ֳ�������
							// 1. ��������ʱ��ͱ���ʱ��֮��Ŀ�ȱ����
							// 2. ���Ա����ļ�����ǰʱ����ԭ����ָ��ʱ������
							// ���������ܿ��ܻ����һ�����˷ѣ� ��Ϊ��ȷ��1��������ж������ݷ��أ� ������������һ�ε��������Ǽ��ٵģ� ���ԣ� �˷�Ҳ�����޵�
							// ����������ܳ�����ñ����ѻ�������
							
							// 1
							if (Req.m_TimeSpecify < TimeStartOfflineTicks)
							{
								Req.m_eReqTimeType = ERTYSpecifyTime;
								Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeSpecify;
								Req.m_TimeEnd = TimeStartOfflineTicks;

								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							
							// 2
							Req.m_eReqTimeType = ERTYCount;
							Req.m_TimeSpecify = TimeStartOfflineTicks;
							Req.m_iCount = pMmiReqMerchTimeSales->m_iCount;
							goto DO_REQ_TIMESALE_COUNT;
						}
					}
				}
			}
			else	// �������͵����� ֱ�ӷ���������
			{
				CMmiReqNode *pMmiReqNodeOut = NULL;
				if (RequestDataOnlyFromCommunication(pMmiCommReq, pMmiReqNodeOut, iCommunicationId))
				{
					aMmiReqNodes.Add(pMmiReqNodeOut);
				}
			}
		}
	}
}

void CDataManager::OnCommDataServiceConnected(int32 iServiceId)
{
	// ����֪ͨ
	NotifyDataServiceConnected(iServiceId);
}

void CDataManager::OnCommDataServiceDisconnected(int32 iServiceId)
{
	// ����֪ͨ
	NotifyDataServiceDisconnected(iServiceId);
}

void CDataManager::OnCommServerConnected(int32 iCommunicationId)
{
	// ����֪ͨ
	NotifyServerConnected(iCommunicationId);
}

void CDataManager::OnCommServerDisconnected(int32 iCommunicationId)
{
	TRACE(L"CDataManager: ����������: %d \n", iCommunicationId);
	// �����ѽ��ܶ��У� ������ڸ÷��������͵�ʵʱ���ݣ� ��ɾ���� ������ServerDisconnect��Ϣ��CommResponse��Ϣǰ��˳���������ϲ��ʵʱ���ݴ�����߼�����
	RemoveRealtimeDataRespOfSpecifyServer(iCommunicationId);

	// ����������ϵ��ѷ�����ɾ��. ��������������, �ٷ������ʱ��, ���� m_MmiReqListPtr ���˵�, ����û����ȥ��bug
	RemoveHistoryReqOfSpecifyServer(iCommunicationId);

	// ����֪ͨ
	NotifyServerDisconnected(iCommunicationId);
}

void CDataManager::OnCommServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType)
{
	//
	if ( ERCTWaitingForOnePack == eType || ERCTNoDataAtAll == eType || ERCTTooMuchTimeOutPack == eType )
	{
		_DLLTRACE(L"***D[%d] dataengine �յ� commengine ��֪ͨ: %d �ܾ�û���յ�����, ��Ҫ����! ����֪ͨ�ϲ� \n", eType, iCommunicationId);
	}
	
	//
	NotifyServerLongTimeNoRecvData(iCommunicationId, eType);
}

void CDataManager::OnCommResponse(int32 iCommSerialNo, int32 iCommunicationId, IN CMmiCommBase *pMmiCommResp)
{
	// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
	//lint --e{429} 

	ASSERT(NULL != pMmiCommResp);
	if (NULL == pMmiCommResp)
	{
		return;
	}
	CString strTime;
#ifdef TRACE_DLG
	GetLocalTime(strTime);
#endif

	// ����������У� ���ҿ��Ƿ����ҵ���Ӧ������
	// ����
	CMmiReqNode *pMmiReqNodeFound = NULL;
	{
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		
		for (int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0; i--)
		{
			CMmiReqNode *pMmiReqNode = m_MmiReqListPtr[i];
			
			// ��Ӧ����ˮ��
			if (iCommSerialNo == pMmiReqNode->m_iCommSerialNo)
			{
				// �ҵ���ɾ����������סʱ��̫��
				pMmiReqNodeFound = pMmiReqNode;
				m_MmiReqListPtr.RemoveAt(i);
				break;
			}
		}
	}
	
	// ����
	bool bAddInQueue = false;
	int iRespReqId = -1;
	{
		
		// �����ҵ����Ҳ���������ֱ���
		if (NULL == pMmiReqNodeFound)	// �Ҳ�����Ӧ����ˮ�� 
		{
			if ( pMmiCommResp->m_eCommType == ECTRespAddPushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTRespUpdatePushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTAnsRegisterPushPrice )
			{
				_DLLTRACE(L"***D dataengine ͨѶ��: %d �޷���ѯ�����¼�Ļ�Ӧ��##==>: %s! \n", iCommunicationId, pMmiCommResp->GetSummary().GetBuffer());
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			else
			{
				_DLLTRACE(L"***D dataengine ͨѶ��: %d �޷���ѯ�����¼�Ļ�Ӧ��##(%d): %s! \n", iCommunicationId, pMmiCommResp->m_eCommType, pMmiCommResp->GetSummary().GetBuffer());	
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			// �Լ�������ķ��������������ӹ�(��Ҫ������������ܲ�������Ӧ���)
			RepairResponseData(NULL, pMmiCommResp);
			
			// ����������ǿ��ܴ��ڵ�ʵʱ���ݵ�Ӧ��
			if (ECTPushPrice	== pMmiCommResp->m_eCommType ||
				ECTPushTick		== pMmiCommResp->m_eCommType ||
				ECTPushLevel2	== pMmiCommResp->m_eCommType ||
				ECTRespAddPushGeneralNormal == pMmiCommResp->m_eCommType ||
				ECTRespAddPushMerchTrendIndex == pMmiCommResp->m_eCommType ||
				ECTRespAddPushMinuteBS == pMmiCommResp->m_eCommType)
			{
				// 
	#ifdef TRACE_DLG
				if (NULL != m_hwndTrace)
				{
					CString strLogInfo;
					strLogInfo.Format(_T("%s: dataengine: �յ����������͡�%s��"),pMmiCommResp->GetSummary());
					m_vecLogInfo.push_back(strLogInfo);

/*					CString *pStrTrace = new CString;
					*pStrTrace = L"  ��dataengine: �յ����������� ��";
					*pStrTrace += pMmiCommResp->GetSummary();
*/		
				}
	#endif

 				// ��¼�ѻ�����				
				if ( ECTPushPrice == pMmiCommResp->m_eCommType)
				{
 					CMmiPushPrice*	pResp = (CMmiPushPrice*)pMmiCommResp;
 					
 					//
 					for ( int32 i = 0; i < pResp->m_RealtimePriceListPtr.GetSize(); i++ )
 					{
 						if ( NULL != pResp->m_RealtimePriceListPtr[i] )
 						{
							CRealtimePrice stRealtimePrice = *(pResp->m_RealtimePriceListPtr[i]);
 							T_OffLineRealtimePriceParam stOffLineRealTimePrice(stRealtimePrice);
 							PushOffLineRealtimePriceList(stOffLineRealTimePrice);
 						}						
 					}
 				}

				// ��ӵ�Ӧ���������
				bAddInQueue = true;
			}
			else if ( ECTKickOut == pMmiCommResp->m_eCommType )
			{
				// ������.
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: �����ˡ�%s��"),strTime,pMmiCommResp->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif	
				// ��ӵ�Ӧ���������
				bAddInQueue = true;
			}
			else if ( ECTRespPlugIn == pMmiCommResp->m_eCommType )
			{
				// ���ģ������Ͱ�
				CMmiCommBasePlugIn* pPlugIn = (CMmiCommBasePlugIn*)pMmiCommResp;

				if ( ECTPIRespAddPushMainMonitor == pPlugIn->m_eCommTypePlugIn 
					|| ECTPIRespAddPushTickEx == pPlugIn->m_eCommTypePlugIn				  
					|| ECTPIRespAddPushMerchIndexEx3 == pPlugIn->m_eCommTypePlugIn
					|| ECTPIRespAddPushMerchIndexEx5 == pPlugIn->m_eCommTypePlugIn
					|| ECTPIRespAddPushMerchIndexEx10 == pPlugIn->m_eCommTypePlugIn
					|| ECTPIRespAddPushBlock == pPlugIn->m_eCommTypePlugIn
					|| ECTPIRespAddPushMerchIndex == pPlugIn->m_eCommTypePlugIn 
					|| ECTPIRespAddPushShortMonitor == pPlugIn->m_eCommTypePlugIn
					|| ECTPIRespAddPushMainMasukura == pPlugIn->m_eCommTypePlugIn)
				{
					// ��ӵ�Ӧ���������
					bAddInQueue = true;
				}
			}
			else
			{
				// ��֪����������Ұ���� :)
				// ��������Ҫ�ϼӸ��٣� ���Ƿ��ǿͻ��˻��߷����������bug����
				// ASSERT(0);		
				// ���������������, 
				
				// zhangbo 0513 #���Դ��룬 �����ڼ����쳣�� ��ʽ���߲�������ִ���

#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: ���棺��Ӧ����Ҳ�����Ӧ�ķ��Ͱ��� ���Ҳ���ʵʱ�������ݣ� �ô���Ҳ����������һ�������Ӧ��ʵ��̫���ˣ� �������������ɾ���ˣ�����"),strTime,pMmiCommResp->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif
			}
		}
		else // �ҵ���Ӧ����ˮ��
		{
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse �յ�֪ͨ", m_hwndTrace);
			}

			// �鿴�Ƿ��Ѿ�Ӧ�������
			CMmiCommBase *pMmiCommReq = pMmiReqNodeFound->m_pMmiCommReq;
			ASSERT (NULL != pMmiCommReq);	// ���������������
			if (NULL == pMmiCommReq)
			{
				return;
			}
			// �Լ�������ķ��������������ӹ�(��Ҫ������������ܲ�������Ӧ���)
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse ׼���ӹ�����", m_hwndTrace);
			}

			RepairResponseData(pMmiCommReq, pMmiCommResp);

			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse ��ɼӹ�����", m_hwndTrace);
			}

			// �����ѻ�����
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse ׼�������ѻ�����", m_hwndTrace);
			}

			// ���ѻ����ݵ����� �����ݶ����ѻ�����ģ�鴦��
			if (ECTReqMerchKLine == pMmiCommReq->m_eCommType && ECTRespMerchKLine == pMmiCommResp->m_eCommType)
			{
				CMmiReqMerchKLine *pMmiReqMerchKLine = (CMmiReqMerchKLine *)pMmiCommReq;
				CMmiRespMerchKLine *pMmiRespMerchKLine = (CMmiRespMerchKLine *)pMmiCommResp;
				
				// 
				ASSERT(pMmiReqMerchKLine->m_iMarketId == pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId);
				ASSERT(pMmiReqMerchKLine->m_eKLineTypeBase == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase);
				ASSERT(pMmiReqMerchKLine->m_StrMerchCode.CompareNoCase(pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode) == 0);
			
				//
				T_OffLineKLineParam stOffLineKLineParam(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, pMmiRespMerchKLine->m_MerchKLineNode.m_KLines);
				PushOffLineKLineList(stOffLineKLineParam);	
			}
			else if (ECTReqMerchTimeSales == pMmiCommReq->m_eCommType && ECTRespMerchTimeSales == pMmiCommResp->m_eCommType)
			{
				CMmiReqMerchTimeSales *pMmiReqMerchTimeSales = (CMmiReqMerchTimeSales *)pMmiCommReq;
				CMmiRespMerchTimeSales *pMmiRespMerchTimeSales = (CMmiRespMerchTimeSales *)pMmiCommResp;
				
				// 
				ASSERT(pMmiReqMerchTimeSales->m_iMarketId == pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId);
				ASSERT(pMmiReqMerchTimeSales->m_StrMerchCode.CompareNoCase(pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode) == 0);
				
				//
				T_OffLineTickParam stOffLineTickParam(pMmiReqMerchTimeSales->m_iMarketId, pMmiReqMerchTimeSales->m_StrMerchCode, pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks);
				PushOffLineTickList(stOffLineTickParam);
			}
			else if ( ECTReqRealtimePrice == pMmiCommReq->m_eCommType && ECTRespRealtimePrice == pMmiCommResp->m_eCommType )
			{		 		
				// ��¼�ѻ���������
 				CMmiReqRealtimePrice*	pReq  = (CMmiReqRealtimePrice*)pMmiCommReq;
 				CMmiRespRealtimePrice*	pResp = (CMmiRespRealtimePrice*)pMmiCommResp;
				if (pReq && pResp)
				{
					// ....
				}
 
 				//
 				for ( int32 i = 0; i < pResp->m_RealtimePriceListPtr.GetSize(); i++ )
 				{
 					if ( NULL != pResp->m_RealtimePriceListPtr[i] )
 					{
						CRealtimePrice stRealtimePrice = *(pResp->m_RealtimePriceListPtr[i]);
 						T_OffLineRealtimePriceParam stOffLineRealTimePrice(stRealtimePrice);
 						PushOffLineRealtimePriceList(stOffLineRealTimePrice);
 					}						
 				}							
			}

			//
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse ��ɴ����ѻ�����", m_hwndTrace);
			}

			// �ȴ����������ʱ��ͬ��������, ���ٰ��Ĳ���. ��Ϊ���ÿ���������������ٰ�, ���ͬ����ʱ����ܲ����������ϵ���̨������
			if ( ECTRespNetTest != pMmiCommResp->m_eCommType )
			{
				if ( !m_bHasSyncServerTime )
				{
					_DLLTRACE(L"***D [%d] ͬ��������ʱ��", iCommunicationId);
				}				
				PreTranslateSycnTime(pMmiReqNodeFound->m_uiTimeRequest, timeGetTime(), pMmiCommResp->m_uiTimeServer);
			}

			// ĳ̨��������֤�ɹ���, ͬ��һ��ʱ��
			if ( ECTRespAuth == pMmiCommResp->m_eCommType )
			{
				m_bHasSyncServerTime = false;
				PreTranslateSycnTime(pMmiReqNodeFound->m_uiTimeRequest, timeGetTime(), pMmiCommResp->m_uiTimeServer);
				_DLLTRACE(L"***D [%d] ��������֤�ɹ�, ͬ��������ʱ��", iCommunicationId);					
			}
			
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse ֪ͨ�ϲ�", m_hwndTrace);
			}
	#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: �յ�commengineӦ�� %d, %s"), strTime, iCommSerialNo, pMmiCommResp->GetSummary());				
				m_vecLogInfo.push_back(strLogInfo);
	#endif	
			
			// ��ӵ�Ӧ���������
			// ����������Ϊ�ϲ��첽���ʵ���Ҫ�� ���������Ӧ��ռ䲢���ڱ�ģ�����ͷţ� ����Ҫ���ϲ��д��� �м�!!!
			iRespReqId = pMmiReqNodeFound->m_iMmiReqId;
			bAddInQueue = true;

			// ɾ�����������¼
			if ( pMmiCommResp->m_eCommType == ECTRespAddPushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTRespUpdatePushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTAnsRegisterPushPrice )
			{
				_DLLTRACE(L"***D dataengine ͨѶ��: %d �ϵİ���Ӧ##==>OK: %s  ��Ӧ��:%s! \n", pMmiReqNodeFound->m_iCommunicationId, pMmiReqNodeFound->m_pMmiCommReq->GetSummary().GetBuffer(), pMmiCommResp->GetSummary().GetBuffer());	
				pMmiReqNodeFound->m_pMmiCommReq->GetSummary().ReleaseBuffer();
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			else
			{
				_DLLTRACE(L"***D dataengine ͨѶ��: %d �ϵİ���Ӧ##OK: %s  ��Ӧ��(%d):%s! \n", pMmiReqNodeFound->m_iCommunicationId, pMmiReqNodeFound->m_pMmiCommReq->GetSummary().GetBuffer(), pMmiCommResp->m_eCommType, pMmiCommResp->GetSummary().GetBuffer());		
				pMmiReqNodeFound->m_pMmiCommReq->GetSummary().ReleaseBuffer();
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}

			DEL(pMmiReqNodeFound);
		}
	}

	if ( bAddInQueue )
	{
		CMmiRespNode *pNewRespNode			= new CMmiRespNode;
		pNewRespNode->m_iMmiRespId			= iRespReqId;
		pNewRespNode->m_iCommunicationId	= iCommunicationId;
		pNewRespNode->m_pMmiCommResp		= pMmiCommResp;
		AddResp(pNewRespNode);
		
		// ֪ͨ�а�����
		NotifyDataCommResponse();
	}
	else
	{
		TRACE(_T("dataengine�����ݰ�û�а취֪ͨ�ϲ�\r\n"));
		// ASSERT( 0 );
		// �а�����Ҫ���
		// ��й¶�Ű�
	}
}

void CDataManager::OnXTimer(int32 iTimerId)
{
	if (KTimerIdClearMmiReqList == iTimerId)
	{
		OnTimerClearMmiReqList();
	}
	else if (KTimerIdProcessOfflineReq == iTimerId)
	{
		OnTimerProcessOfflineReq();
	}
	else if (KTimerIdProcessMmiReqWaitingList == iTimerId)
	{
		OnTimerProcessMmiReqWaitingList();
	}
}

void CDataManager::OnTimerProcessMmiReqWaitingList()
{
	CString strTime;
#ifdef TRACE_DLG
	GetLocalTime(strTime);
#endif
	// �鿴��Щ�������Ҫ����
	CArray<CMmiReqWaitingNode *, CMmiReqWaitingNode *>	aReqWaitNeedSend;
	{
		RGUARD(LockSingle, m_LockReqWaitingList, LockReqWaitingList);
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		
		uint32 uiTimeNow = timeGetTime();
		for (int32 i = 0; i < m_aMmiReqWaitingListPtr.GetSize(); i++)
		{
			CMmiReqWaitingNode *pMmiReqWaitingNode = m_aMmiReqWaitingListPtr[i];
			CMmiCommBase *pReqWaiting = pMmiReqWaitingNode->m_pMmiCommReq;
			
			if (uiTimeNow - pMmiReqWaitingNode->m_uiTimeRequest < KReqWaitingTimeOutMSecond/2)
				continue;
			
			////////////////////////////////////////////////////////
			// ʱ�䵽�ˣ� ���Կ��Ƿ���������
			
			// �ӷ��Ͷ����п�����û��ͬ��Ʒ�� ͬ���͵���ʷ����
			bool32 bHasSameReqObject = false;
			for (int32 j = 0; j < m_MmiReqListPtr.GetSize(); j++)
			{
				CMmiReqNode *pReqNode = m_MmiReqListPtr[j];
				CMmiCommBase *ppp = pReqNode->m_pMmiCommReq;
				
				uint32 uiTimeDiff = uiTimeNow - pReqNode->m_uiTimeRequest;
				// �����ض�ʱ��Ĳ���Ϊ�ǿ��Կ��ǵ���ͬ��
				if ( uiTimeDiff<KiTimeForPrompt && IsSameReqObject(pReqWaiting, ppp))
				{
					bHasSameReqObject = true;
					break;
				}
			}
			
			// 
			if (bHasSameReqObject)	// �������ͬ���͵����ݣ� ��ʱ������
				continue;
			
			
			// �ӵȴ�������ȡ���������
			aReqWaitNeedSend.Add(pMmiReqWaitingNode);
			m_aMmiReqWaitingListPtr.RemoveAt(i, 1);
			i--;
		}
	}
	
	// ʵ�ʷ��Ͷ���
	{
		for ( int32 i=0; i<aReqWaitNeedSend.GetSize(); ++i )
		{
			CMmiReqWaitingNode *pMmiReqWaitingNode = aReqWaitNeedSend[i];
			CMmiCommBase *pReqWaiting = pMmiReqWaitingNode->m_pMmiCommReq;
			if (pReqWaiting)
			{
				// ....
			}
			
			// ���ڴ��������
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: ��ȡ�������� ��%s��"),strTime,pMmiReqWaitingNode->m_pMmiCommReq->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif
			
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			RequestDataImmediately(pMmiReqWaitingNode->m_pMmiCommReq, aMmiReqNodes, pMmiReqWaitingNode->m_eDataSource, pMmiReqWaitingNode->m_iCommunicationId);
			
			// ɾ���ȴ�����
			DEL(pMmiReqWaitingNode);
		}
		aReqWaitNeedSend.RemoveAll();
	}
}

void CDataManager::DealTimeOutInfomations(CArray<CPoint, CPoint>& aTimeOutReqPairs, CMmiReqNode* pReqNode)
{
	if ( NULL == pReqNode )
	{
		ASSERT(0);
		return;
	}

	// ��ͨѶ��
	bool32 bFindCommunication = false;
	CPoint* pTimeOutReqPairs = (CPoint*)aTimeOutReqPairs.GetData();

	for ( int32 i = 0; i < aTimeOutReqPairs.GetSize(); i++ )
	{
		if ( pTimeOutReqPairs[i].x == pReqNode->m_iCommunicationId )
		{
			// �ҵ���, ��ʱ��¼�� 1
			bFindCommunication = true;
			pTimeOutReqPairs[i].y += 1;
		}
	}

	// û���ҵ�, ���������¼
	if ( !bFindCommunication )
	{
		// 
		CPoint TimeOut;

		// Ĭ�� x ����ͨѶ��
		TimeOut.x = pReqNode->m_iCommunicationId;

		// Ĭ�� y ���泬ʱ������, ��ʼֵ 1
		TimeOut.y = 1;

		aTimeOutReqPairs.Add(TimeOut);
	}
}

void CDataManager::OnTimerClearMmiReqList()
{
	// ��ʱ��ReqNode
	CArray<int32, int32>	aReqNodeIdTimeout;
	// ��ʱͳ�Ƶ����ݶ�(Communicationid, TimeOutReqs):
	CArray<CPoint, CPoint> aTimeOutReqPairs;
	aTimeOutReqPairs.RemoveAll();
	// �г��� 5 ��û�лصİ�, ���������粻��
	CArray<int32, int32> aCommunicationsNeedPrompt;

	// ��ʱ����ѯ
	{
		// 
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		{
			// �Ӻ���ǰɨ�����е����� �г�ʱ���޳�
			
			uint32 uiTimeNow = timeGetTime();
			
			// �Ƿ���Ҫ����
			//bool32 bNeedReConnect	  = false;
			
			DWORD dwTime = timeGetTime();
			
			//
			for (int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0; i--)
			{
				CMmiReqNode *pMmiReqNode = m_MmiReqListPtr[i];
				if (NULL == pMmiReqNode)
				{
					ASSERT(0);	// ���������������
					m_MmiReqListPtr.RemoveAt(i, 1);
					continue;
				}
				
				// 
				if ( uiTimeNow - pMmiReqNode->m_uiTimeRequest >= KiTimeForPrompt )
				{
#ifdef _DEBUG
					TRACE(_T("CDataManager ͨѶ��: %d ��[������=%d, id=%d,%d, time=%d] �Ļ�Ӧʱ�䳬ʱ %d, ��ʾ! tag=%d\n")
						, pMmiReqNode->m_iCommunicationId, pMmiReqNode->m_pMmiCommReq->m_eCommType
						, pMmiReqNode->m_iMmiReqId, pMmiReqNode->m_iCommSerialNo
						, pMmiReqNode->m_uiTimeRequest, uiTimeNow - pMmiReqNode->m_uiTimeRequest
						, dwTime
						);
#endif
					
					// 
					bool32 bExist = false;
					
					//
					for ( int32 j = 0; j < aCommunicationsNeedPrompt.GetSize(); j++ )
					{
						if (pMmiReqNode->m_iCommunicationId == aCommunicationsNeedPrompt.GetAt(j))
						{
							bExist = true;
							break;
						}
					}
					
					//
					if ( !bExist )
					{
						aCommunicationsNeedPrompt.Add(pMmiReqNode->m_iCommunicationId);
					}
				}
				
				//
				if (uiTimeNow - pMmiReqNode->m_uiTimeRequest < KReqTimeOutMSecond)
					continue;
				
#ifdef _DEBUG
				TRACE(_T("CDataManager ͨѶ��: %d ��[������=%d, id=%d,%d, time=%d] �Ļ�Ӧʱ�䳬ʱ %d ��ɾ��, ��ʾ! tag=%d\n")
					, pMmiReqNode->m_iCommunicationId, pMmiReqNode->m_pMmiCommReq->m_eCommType
					, pMmiReqNode->m_iMmiReqId, pMmiReqNode->m_iCommSerialNo
					, pMmiReqNode->m_uiTimeRequest, uiTimeNow - pMmiReqNode->m_uiTimeRequest
					, dwTime
					);				
#endif
				
				// ��ӵ���ʱ֪ͨ����id
				aReqNodeIdTimeout.Add( m_MmiReqListPtr[i]->m_iMmiReqId );
				
				// ����һ��ͳ����Ϣ
				DealTimeOutInfomations(aTimeOutReqPairs, m_MmiReqListPtr[i]);
				
				// ɾ��������¼
				DEL(m_MmiReqListPtr[i]);
				m_MmiReqListPtr.RemoveAt(i , 1);			
			}
		}
	}

	// ����֪ͨ
	{
		// ����id��ʱ����
		int32 i=0;
		for ( i=0; i<aReqNodeIdTimeout.GetSize(); ++i )
		{
			int32 iTimeoutId = aReqNodeIdTimeout[i];
			NotifyDataRequestTimeOut( iTimeoutId );
		}

		// ĳ���������ж������ʱ֪ͨ
		for ( i = 0; i < aTimeOutReqPairs.GetSize(); i++ )
		{
			CPoint	TimeOutPair = aTimeOutReqPairs[i];
			
			// ���ͨѶ�ϵĳ�ʱ���Ѿ�̫����, ֪ͨ
			if ( TimeOutPair.y >= KiTimeOutReqsForPrompt )
			{
				// 
				_DLLTRACE(L"***D [%d] dataengine ͨѶ��: %d �ϵĳ�ʱ��: %d �� �Ѿ��������ֵ: %d, ������Ҫ�ϲ�����! \n", ERCTTooMuchTimeOutPack, TimeOutPair.x, TimeOutPair.y, KiTimeOutReqsForPrompt);				
				NotifyServerLongTimeNoRecvData(TimeOutPair.x, ERCTTooMuchTimeOutPack);	
				
				// �Ѿ������˾Ͳ�Ҫ��ʾ���粻˳��
				for ( int32 j = aCommunicationsNeedPrompt.GetSize() - 1; j >= 0; j-- )
				{
					if ( TimeOutPair.x == aCommunicationsNeedPrompt.GetAt(j) )
					{
						aCommunicationsNeedPrompt.RemoveAt(j);
						break;
					}
				}
			}
		}

		// ĳ���Ӵ������糬ʱ�����������Ӳ��Ǻ�����
		// ��ʾ���粻˳��
		for ( i = 0; i < aCommunicationsNeedPrompt.GetSize(); i++ )
		{
			TRACE(L"CDataManager ͨѶ��: %d ���а��Ļ�Ӧʱ�䳬�� 5 s, ��ʾ! \n", aCommunicationsNeedPrompt.GetAt(i));				
			NotifyServerLongTimeNoRecvData(aCommunicationsNeedPrompt.GetAt(i), ERCTPackTimeOut);
		}
	}
}

// ��ʱ�����ѻ����������, ʵ���첽֪ͨ��Ŀ��
// ��������Щ�������ˮ���б�Ϊ�գ� ���ҿ��ܴ����ѻ����ݵ�����
void CDataManager::OnTimerProcessOfflineReq()
{
	CString strTime;
#ifdef TRACE_DLG
	GetLocalTime(strTime);
#endif
	// �����ܴ���ġ�ʹ��new�����Ĵ���
	CArray<CMmiReqNode *, CMmiReqNode *>	aReqMaybeDone;
	
	{
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		for (int32 i = 0; i < m_MmiReqListPtr.GetSize(); i++)
		{
			// 
			CMmiReqNode *pMmiReqNode = m_MmiReqListPtr[i];
			if (EDSVipData != pMmiReqNode->m_eDataSource && EDSOfflineData != pMmiReqNode->m_eDataSource)
				continue;
			
			CMmiCommBase *pMmiCommReq = pMmiReqNode->m_pMmiCommReq;
			if (ECTReqMerchKLine == pMmiCommReq->m_eCommType)
			{
				CMmiReqNode *pNewNode = new CMmiReqNode;
				*pNewNode = *pMmiReqNode;
				CMmiCommBase *pNewMmiComm = NewCopyCommReqObject(pMmiCommReq);
				pNewNode->m_pMmiCommReq = pNewMmiComm;
				aReqMaybeDone.Add(pNewNode);
				continue;
			}
			else if (ECTReqMerchTimeSales == pMmiCommReq->m_eCommType)
			{
				CMmiReqNode *pNewNode = new CMmiReqNode;
				*pNewNode = *pMmiReqNode;
				CMmiCommBase *pNewMmiComm = NewCopyCommReqObject(pMmiCommReq);
				pNewNode->m_pMmiCommReq = pNewMmiComm;
				aReqMaybeDone.Add(pNewNode);
				continue;
			}
			else if (ECTReqRealtimePrice == pMmiCommReq->m_eCommType)
			{
				CMmiReqNode *pNewNode = new CMmiReqNode;
				*pNewNode = *pMmiReqNode;
				CMmiCommBase *pNewMmiComm = NewCopyCommReqObject(pMmiCommReq);
				pNewNode->m_pMmiCommReq = pNewMmiComm;
				aReqMaybeDone.Add(pNewNode);
				continue;
			}
			else
			{
				// �����������ѻ����ݶ��޷�����
				// NULL;
			}
		}
	}
	
	// ��ȡ�ѻ�����
	{
		for (int32 i = 0; i < aReqMaybeDone.GetSize(); i++)
		{
			// 
			CMmiReqNode *pMmiReqNode = aReqMaybeDone[i];
			if (EDSVipData != pMmiReqNode->m_eDataSource && EDSOfflineData != pMmiReqNode->m_eDataSource)
				continue;
			
			CMmiCommBase *pMmiCommReq = pMmiReqNode->m_pMmiCommReq;
			if (ECTReqMerchKLine == pMmiCommReq->m_eCommType)
			{
				CMmiReqMerchKLine *pMmiReqMerchKLine = (CMmiReqMerchKLine *)pMmiCommReq;
				ASSERT(ERTYSpecifyTime == pMmiReqMerchKLine->m_eReqTimeType || ERTYFrontCount == pMmiReqMerchKLine->m_eReqTimeType || ERTYCount == pMmiReqMerchKLine->m_eReqTimeType);

				CMmiRespMerchKLine *pMmiRespMerchKLine = new CMmiRespMerchKLine;
				pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId		= pMmiReqMerchKLine->m_iMarketId;
				pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode		= pMmiReqMerchKLine->m_StrMerchCode;
				pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase	= pMmiReqMerchKLine->m_eKLineTypeBase;

				E_OffKLineType eOffKLineType = EOKTVipData;
				if ( EDSOfflineData == pMmiReqNode->m_eDataSource )
				{
					eOffKLineType = EOKTOfflineData;
				}

				//
				if (ERTYSpecifyTime == pMmiReqMerchKLine->m_eReqTimeType)
				{
					m_OfflineDataManager.ReadOfflineKLines(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, 
						eOffKLineType, 
						pMmiReqMerchKLine->m_TimeStart, pMmiReqMerchKLine->m_TimeEnd, 
						pMmiRespMerchKLine->m_MerchKLineNode.m_KLines);
				}
				else if (ERTYFrontCount == pMmiReqMerchKLine->m_eReqTimeType)
				{
					m_OfflineDataManager.ReadOfflineKLines(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, 
						eOffKLineType, 
						pMmiReqMerchKLine->m_TimeSpecify, -pMmiReqMerchKLine->m_iFrontCount, 
						pMmiRespMerchKLine->m_MerchKLineNode.m_KLines);
				}
				else if (ERTYCount == pMmiReqMerchKLine->m_eReqTimeType)
				{
					m_OfflineDataManager.ReadOfflineKLines(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, 
						eOffKLineType, 
						pMmiReqMerchKLine->m_TimeSpecify, pMmiReqMerchKLine->m_iCount, 
						pMmiRespMerchKLine->m_MerchKLineNode.m_KLines);
				}

				// ������Ҫ�����ж�.���ݵĺϷ���
				RepairResponseData(NULL, pMmiRespMerchKLine);
				//
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: ȡ���ѻ����� ����"),strTime,pMmiRespMerchKLine->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif

				
				// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
				//lint --e{429} 

				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespMerchKLine;
				AddResp(pNewRespNode, true);
				
				// ɾ��������¼
				m_LockReqList.lock();
				{
					for ( int32 iDel=0; iDel<m_MmiReqListPtr.GetSize(); ++iDel )
					{
						CMmiReqNode *pReqSrc = m_MmiReqListPtr[iDel];
						if ( pReqSrc!=NULL 
							&& pReqSrc->m_iMmiReqId==pMmiReqNode->m_iMmiReqId )
						{
							DEL(pReqSrc);
							m_MmiReqListPtr.RemoveAt(iDel);
							break;
						}
					}
				}
				m_LockReqList.unlock();

				// ֪ͨ�ϲ��а�������
				NotifyDataCommResponse();
				continue;
			}
			else if (ECTReqMerchTimeSales == pMmiCommReq->m_eCommType)
			{
				CMmiReqMerchTimeSales *pMmiReqMerchTimeSales = (CMmiReqMerchTimeSales *)pMmiCommReq;
				ASSERT(ERTYSpecifyTime == pMmiReqMerchTimeSales->m_eReqTimeType || ERTYFrontCount == pMmiReqMerchTimeSales->m_eReqTimeType || ERTYCount == pMmiReqMerchTimeSales->m_eReqTimeType);

				CMmiRespMerchTimeSales *pMmiRespMerchTimeSales = new CMmiRespMerchTimeSales;
				pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId		= pMmiReqMerchTimeSales->m_iMarketId;
				pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode		= pMmiReqMerchTimeSales->m_StrMerchCode;
		
				if (ERTYSpecifyTime == pMmiReqMerchTimeSales->m_eReqTimeType)
				{
					m_OfflineDataManager.ReadOfflineTicks(pMmiReqMerchTimeSales->m_iMarketId, pMmiReqMerchTimeSales->m_StrMerchCode, 
						pMmiReqMerchTimeSales->m_TimeStart, pMmiReqMerchTimeSales->m_TimeEnd, 
						pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks);
				}
				else if (ERTYFrontCount == pMmiReqMerchTimeSales->m_eReqTimeType)
				{
					m_OfflineDataManager.ReadOfflineTicks(pMmiReqMerchTimeSales->m_iMarketId, pMmiReqMerchTimeSales->m_StrMerchCode,
						pMmiReqMerchTimeSales->m_TimeSpecify, -pMmiReqMerchTimeSales->m_iFrontCount, 
						pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks);
				}
				else if (ERTYCount == pMmiReqMerchTimeSales->m_eReqTimeType)
				{	
					m_OfflineDataManager.ReadOfflineTicks(pMmiReqMerchTimeSales->m_iMarketId, pMmiReqMerchTimeSales->m_StrMerchCode,
						pMmiReqMerchTimeSales->m_TimeSpecify, pMmiReqMerchTimeSales->m_iCount, 
						pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks);
				}

				// ������Ҫ�����ж�.���ݵĺϷ���
				RepairResponseData(NULL, pMmiRespMerchTimeSales);

				//

				// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
				//lint --e{429} 
				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespMerchTimeSales;
				AddResp(pNewRespNode);
				
				// ɾ��������¼
				m_LockReqList.lock();
				{
					for ( int32 iDel=0; iDel<m_MmiReqListPtr.GetSize(); ++iDel )
					{
						CMmiReqNode *pReqSrc = m_MmiReqListPtr[iDel];
						if ( pReqSrc!=NULL 
							&& pReqSrc->m_iMmiReqId==pMmiReqNode->m_iMmiReqId )
						{
							DEL(pReqSrc);
							m_MmiReqListPtr.RemoveAt(iDel);
							break;
						}
					}
				}
				m_LockReqList.unlock();

				// ֪ͨ�ϲ��а�������
				NotifyDataCommResponse();
				continue;
			}
			else if (ECTReqRealtimePrice == pMmiCommReq->m_eCommType)
			{
				// ֻ�� offline ����
				ASSERT(EDSOfflineData == pMmiReqNode->m_eDataSource);
			
				CMmiReqRealtimePrice* pReqRealtimePrice = (CMmiReqRealtimePrice*)pMmiCommReq;

				//
				CMmiRespRealtimePrice *pMmiRespRealtimePrice = new CMmiRespRealtimePrice();
				pMmiRespRealtimePrice->m_uiTimeServer = timeGetTime();


				CRealtimePrice* pRealtimePrice = new CRealtimePrice();
				m_OfflineDataManager.ReadOfflineRealtimePrice(pReqRealtimePrice->m_iMarketId, pReqRealtimePrice->m_StrMerchCode, *pRealtimePrice);
			
				pMmiRespRealtimePrice->m_RealtimePriceListPtr.Add(pRealtimePrice);
				
				//
				for ( int32 j = 0; j < pReqRealtimePrice->m_aMerchMore.GetSize(); j++ )
				{
					CMerchKey MerchKey = pReqRealtimePrice->m_aMerchMore.GetAt(j);

					//
					CRealtimePrice* p = new CRealtimePrice();
					m_OfflineDataManager.ReadOfflineRealtimePrice(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode, *p);
					pMmiRespRealtimePrice->m_RealtimePriceListPtr.Add(p);
				}

				// 
				// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
				//lint --e{429} 

				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespRealtimePrice;
				AddResp(pNewRespNode, true);
				
				// ɾ��������¼
				m_LockReqList.lock();
				{
					for ( int32 iDel=0; iDel<m_MmiReqListPtr.GetSize(); ++iDel )
					{
						CMmiReqNode *pReqSrc = m_MmiReqListPtr[iDel];
						if ( pReqSrc!=NULL 
							&& pReqSrc->m_iMmiReqId==pMmiReqNode->m_iMmiReqId )
						{
							DEL(pReqSrc);
							m_MmiReqListPtr.RemoveAt(iDel);
							break;
						}
					}
				}
				m_LockReqList.unlock();
				
				// ֪ͨ�ϲ��а�������
				NotifyDataCommResponse();
				continue;
			}
			else
			{
				// �����������ѻ����ݶ��޷�����
				// NULL;
			}
		}
	}

	// �����ʱ�������Դ
	{
		for ( int32 i=0; i<aReqMaybeDone.GetSize(); ++i )
		{
			DEL(aReqMaybeDone[i]);
		}
		aReqMaybeDone.RemoveAll();
	}
}

void CDataManager::NotifyDataRequestTimeOut(int32 iMmiReqId)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataRequestTimeOut(iMmiReqId);
		}
	}
}

void CDataManager::NotifyDataCommResponse()
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataCommResponse();
		}
	}
}

void CDataManager::NotifyDataSyncServerTimer(CGmtTime &TimeServer)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataSyncServerTime(TimeServer);
		}
	}
}

void CDataManager::NotifyDataServiceConnected(int32 iServiceId)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataServiceConnected(iServiceId);
		}
	}
}

void CDataManager::NotifyDataServiceDisconnected(int32 iServiceId)
{
	// TRACE(L"CDataManager: ���ݷ������ %d \n", iServiceId);

	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataServiceDisconnected(iServiceId);
		}
	}
}

void CDataManager::NotifyServerConnected(int32 iCommunicationId)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataServerConnected(iCommunicationId);
		}
	}
}

void CDataManager::NotifyServerDisconnected(int32 iCommunicationId)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataServerDisconnected(iCommunicationId);
		}
	}
}

void CDataManager::NotifyServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType)
{
	for (int32 i = 0; i < m_pDataManagerNotifyPtrList.GetSize(); i++)
	{
		CDataManagerNotify *pDataManagerNotify = m_pDataManagerNotifyPtrList[i];
		if (NULL != pDataManagerNotify)
		{
			pDataManagerNotify->OnDataServerLongTimeNoRecvData(iCommunicationId, eType);
		}
	}
}

int32 g_iIgnoreCount = 0;
int32 g_iReplaceCount = 0;
void CDataManager::PushReqIntoWaitingQueue(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId)
{
	CString strTime;
#ifdef TRACE_DLG
	GetLocalTime(strTime);
#endif
	ASSERT(NULL != pMmiCommReq);
	ASSERT(EDSOfflineData == eDataSource || EDSCommunication == eDataSource || EDSVipData == eDataSource || EDSAuto == eDataSource);

	// ��Ӽ�¼����������
	RGUARD(LockSingle, m_LockReqWaitingList, LockReqWaitingList);
	{
		uint32 uiFirstRemoveTime = timeGetTime();
		int32  iFirstRemoveIndex = -1;
		

		// ��������µ���������������е�����������Ƚϣ� ��֮ǰ����Щ�����ǿ��Թ��˵�
		bool32 bIgnoreReq = false;
		for (int32 i = m_aMmiReqWaitingListPtr.GetSize() - 1; i >= 0 ; i--)
		{
			CMmiReqWaitingNode *pReqNode = m_aMmiReqWaitingListPtr[i];
			CMmiCommBase *ppp = pReqNode->m_pMmiCommReq;
			
			CMmiCommBase::E_CompareResult eCompareResult = CMmiCommBase::ECREqual;
			if (eDataSource == pReqNode->m_eDataSource &&				// һ��Ҫ��ͬ�������� ���ܱ�֤�пɱ���
				iCommunicationId == pReqNode->m_iCommunicationId && 
				pMmiCommReq->CompareReq(ppp, eCompareResult))
			{
				if (CMmiCommBase::ECRMore == eCompareResult)	// ��ǰ���� > ���������� ɾ�������и�����
				{
					// �����������ʱ��, ����������
					uiFirstRemoveTime = m_aMmiReqWaitingListPtr[i]->m_uiTimeRequest;
					iFirstRemoveIndex = i;

					// ������������
					DEL(m_aMmiReqWaitingListPtr[i]);
					m_aMmiReqWaitingListPtr.RemoveAt(i, 1);

	#ifdef TRACE_DLG
					CString strLogInfo;
					strLogInfo.Format(_T("%s: dataengine: �Ż����滻��һ������ ��%s��"),strTime,pMmiCommReq->GetSummary());
					m_vecLogInfo.push_back(strLogInfo);
	#endif
				}
				else if (CMmiCommBase::ECRLess == eCompareResult || CMmiCommBase::ECREqual == eCompareResult) // ��ǰ���� <= ���������� �����󱻹���
				{
					bIgnoreReq = true;
					break;
				}
				else if (CMmiCommBase::ECRIntersection == eCompareResult)
				{
					// zhangbo 0822 #������
					//...
					// NULL;
				}
			}
		}
		
		// ��ӵ���������
		if (!bIgnoreReq)
		{
#ifdef TRACE_DLG
			CString strLogInfo;
			strLogInfo.Format(_T("%s: dataengine: ����������� ��%s��"), strTime,pMmiCommReq->GetSummary());
			m_vecLogInfo.push_back(strLogInfo);
#endif
			AppendWaitingReq(pMmiCommReq, eDataSource, iCommunicationId);

			// �޸��¼ӵļ�¼��ʱ��
			if (iFirstRemoveIndex >= 0)
			{
				CMmiReqWaitingNode *pNewWaitingNode = m_aMmiReqWaitingListPtr[m_aMmiReqWaitingListPtr.GetSize() - 1];
				pNewWaitingNode->m_uiTimeRequest = uiFirstRemoveTime;

				m_aMmiReqWaitingListPtr.InsertAt(iFirstRemoveIndex, pNewWaitingNode, 1);
				m_aMmiReqWaitingListPtr.RemoveAt(m_aMmiReqWaitingListPtr.GetSize() - 1, 1);
			}
		}
		else
		{
#ifdef TRACE_DLG
			CString strLogInfo;
			strLogInfo.Format(_T("%s: dataengine: �ð��������ˡ�%s��"), strTime,pMmiCommReq->GetSummary());
			m_vecLogInfo.push_back(strLogInfo);
#endif
		}
	}
}

void CDataManager::AppendWaitingReq(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId)
{
	ASSERT(NULL != pMmiCommReq);
	if (NULL == pMmiCommReq)
	{
		return;
	}
	ASSERT(EDSOfflineData == eDataSource || EDSCommunication == eDataSource || EDSVipData == eDataSource || EDSAuto == eDataSource);

	///////////////////////////////////////////////////////////////////
	// ������������
	// ����һ���������ݳ����� ��Ϊ���ݽ������������ݺܿ����ڸú���ִ����ͱ��ͷŵ���
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);

	if (NULL == pCommNew)
		return;
	
	// ����������¼
	CMmiReqWaitingNode *pMmiReqWaitingNode = new CMmiReqWaitingNode;
	m_aMmiReqWaitingListPtr.Add(pMmiReqWaitingNode);
	
	pMmiReqWaitingNode->m_eDataSource = eDataSource;
	pMmiReqWaitingNode->m_iCommunicationId = iCommunicationId;
	pMmiReqWaitingNode->m_pMmiCommReq = pCommNew;


	// t..fangz0510 
	if ( ECTReqMerchKLine == pMmiCommReq->m_eCommType )
	{
		_LogCheckTime(L"[Dataengine==>] CDataManager:: �����������", m_hwndTrace);
	}
}

bool32 CDataManager::RequestDataOnlyFromCommunication(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut, int32 iCommunicationId)
{
	pMmiReqNodeOut = NULL;

	
	ASSERT(NULL != pMmiCommReq);
	if (NULL == pMmiCommReq)
	{
		//return -1;
		return false;
	}
	if (NULL == m_pCommManager)
	{
		//return -1;
		return false;
	}
		

#ifdef TRACE_DLG
	CString strTime, strLogInfo;
	GetLocalTime(strTime);
	strLogInfo.Format(_T("%s: ����commengine���͡�%s��"),strTime, pMmiCommReq->GetSummary());
	m_vecLogInfo.push_back(strLogInfo);
#endif

	// t..fangz0520
	if ( ECTReqMerchKLine == pMmiCommReq->m_eCommType )
	{
		_LogCheckTime("[Dataengine==>] CDataManager:: ����commengine��������:", m_hwndTrace);
	}

	if ( ECTReqMerchTimeSales == pMmiCommReq->m_eCommType )
	{
		CMmiReqMerchTimeSales* ppp = (CMmiReqMerchTimeSales*)pMmiCommReq;

		if ( ppp->m_iFrontCount <= 0 )
		{
			ASSERT(0);
			TRACE(L"[datengine!!!]�ֱ��������: id = %d code = %s time = %d count = %d \n", ppp->m_iMarketId, ppp->m_StrMerchCode.GetBuffer(), ppp->m_TimeSpecify.GetTime(), ppp->m_iFrontCount);
			ppp->m_StrMerchCode.ReleaseBuffer();
		}
	}
	// 
	T_RequestRet stRequestRet;
	bool32 bRequestOk = m_pCommManager->RequestCommData(pMmiCommReq, stRequestRet, iCommunicationId);
	if (!bRequestOk)
		return false;

	// ����һ���������ݳ����� ��Ϊ���ݽ������������ݺܿ����ڸú���ִ����ͱ��ͷŵ���
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
	{
		ASSERT(0);		// ��Ӧ�ó����ִ���
		return false;
	}
	
	// ����������¼
	CMmiReqNode *pMmiReqNode = new CMmiReqNode;
	
	pMmiReqNode->m_iCommunicationId		= stRequestRet.iCommunicationId;
	pMmiReqNode->m_iCommSerialNo		= stRequestRet.iCommSerialNo;
	pMmiReqNode->m_eDataSource			= EDSCommunication;
	pMmiReqNode->m_iMmiReqId			= GetMmiReqId();
	pMmiReqNode->m_pMmiCommReq			= pCommNew;

	m_LockReqList.lock();
	m_MmiReqListPtr.Add(pMmiReqNode);
	m_LockReqList.unlock();
	
	//
	pMmiReqNodeOut = pMmiReqNode;
	return true;
}


bool32 CDataManager::RequestDataOnlyFromVipData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut)
{
	pMmiReqNodeOut = NULL;

	//
	ASSERT(NULL != pMmiCommReq);
	
	// ����һ���������ݳ����� ��Ϊ���ݽ������������ݺܿ����ڸú���ִ����ͱ��ͷŵ���
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
		return false;

#ifdef TRACE_DLG
	if (NULL != m_hwndTrace)
	{
		CString *pStrTrace = new CString;
		*pStrTrace = L"  ��dataengine: �����ѻ����� ��";
		*pStrTrace += pCommNew->GetSummary();
		::PostMessage(m_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// ����������¼
	CMmiReqNode *pMmiReqNode = new CMmiReqNode;
	
	pMmiReqNode->m_eDataSource = EDSVipData;
	pMmiReqNode->m_iMmiReqId = GetMmiReqId();
	pMmiReqNode->m_pMmiCommReq = pCommNew;

	m_LockReqList.lock();
	m_MmiReqListPtr.Add(pMmiReqNode);
	m_LockReqList.unlock();
	
	//
	pMmiReqNodeOut = pMmiReqNode;
	return true;
}

bool32 CDataManager::RequestDataOnlyFromOfflineData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut)
{
	pMmiReqNodeOut = NULL;

	//
	ASSERT(NULL != pMmiCommReq);
	
	// ����һ���������ݳ����� ��Ϊ���ݽ������������ݺܿ����ڸú���ִ����ͱ��ͷŵ���
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
		return false;

#ifdef TRACE_DLG
	if (NULL != m_hwndTrace)
	{
		CString *pStrTrace = new CString;
		*pStrTrace = L"  ��dataengine: �����ѻ����� ��";
		*pStrTrace += pCommNew->GetSummary();
		::PostMessage(m_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// ����������¼
	CMmiReqNode *pMmiReqNode = new CMmiReqNode;
	
	pMmiReqNode->m_eDataSource = EDSOfflineData;
	pMmiReqNode->m_iMmiReqId = GetMmiReqId();
	pMmiReqNode->m_pMmiCommReq = pCommNew;

	m_LockReqList.lock();
	m_MmiReqListPtr.Add(pMmiReqNode);
	m_LockReqList.unlock();
	
	//
	pMmiReqNodeOut = pMmiReqNode;
	return true;
}
// �Լ�������ķ��������������ӹ�(��Ҫ������������ܲ�������Ӧ���)
// �������ļ���Ӧ�����Ҫ������ ��ο�commengine�д����ر�˵������ע������
void CDataManager::RepairResponseData(CMmiCommBase *pMmiCommReq, CMmiCommBase *pMmiCommResp)
{
	ASSERT(NULL != pMmiCommResp);
	if (NULL == pMmiCommResp)
	{
		return;
	}
	
	if (NULL != pMmiCommReq && ECTReqReport == pMmiCommReq->m_eCommType)
	{
		if (ECTRespReport == pMmiCommResp->m_eCommType)
		{
			CMmiReqReport *pMmiReqReport		= (CMmiReqReport *)pMmiCommReq;
			CMmiRespReport *pMmiRespReport		= (CMmiRespReport *)pMmiCommResp;
			
			pMmiRespReport->m_iMarketId			= pMmiReqReport->m_iMarketId;	
			pMmiRespReport->m_eMerchReportField	= pMmiReqReport->m_eMerchReportField;	
			pMmiRespReport->m_bDescSort			= pMmiReqReport->m_bDescSort;	
			pMmiRespReport->m_iStart			= pMmiReqReport->m_iStart;	
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
	if (NULL != pMmiCommReq && ECTReqMerchKLine == pMmiCommReq->m_eCommType)
	{
		if (ECTRespMerchKLine == pMmiCommResp->m_eCommType)
		{
			CMmiReqMerchKLine *pMmiReqMerchKLine = (CMmiReqMerchKLine *)pMmiCommReq;
			CMmiRespMerchKLine *pMmiRespMerchKLine = (CMmiRespMerchKLine *)pMmiCommResp;
			
			pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase = pMmiReqMerchKLine->m_eKLineTypeBase;
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
	else if (NULL != pMmiCommReq && ECTReqMerchInfo == pMmiCommReq->m_eCommType)
	{
		if (ECTRespMerchInfo == pMmiCommResp->m_eCommType)
		{
			CMmiReqMerchInfo *pMmiReqMerchInfo = (CMmiReqMerchInfo *)pMmiCommReq;
			CMmiRespMerchInfo *pMmiRespMerchInfo = (CMmiRespMerchInfo *)pMmiCommResp;
			
			pMmiRespMerchInfo->m_iMarketId = pMmiReqMerchInfo->m_iMarketId;
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
	else if (NULL != pMmiCommReq && ECTReqPublicFile == pMmiCommReq->m_eCommType)
	{
		if (ECTRespPublicFile == pMmiCommResp->m_eCommType)
		{
			CMmiReqPublicFile *pMmiReqPublicFile = (CMmiReqPublicFile *)pMmiCommReq;
			CMmiRespPublicFile *pMmiRespPublicFile = (CMmiRespPublicFile *)pMmiCommResp;
			
			pMmiRespPublicFile->m_iMarketId = pMmiReqPublicFile->m_iMarketId;
			pMmiRespPublicFile->m_StrMerchCode = pMmiReqPublicFile->m_StrMerchCode;	
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
	else if (NULL != pMmiCommReq && ECTReqBroker == pMmiCommReq->m_eCommType)
	{
		if (ECTRespBroker == pMmiCommResp->m_eCommType)
		{
			CMmiReqBroker *pMmiReqBroker = (CMmiReqBroker *)pMmiCommReq;
			CMmiRespBroker *pMmiRespBroker = (CMmiRespBroker *)pMmiCommResp;
			
			pMmiRespBroker->m_iBreedId = pMmiReqBroker->m_iBreedId;
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
	else if (NULL != pMmiCommReq && ECTCancelAttendMerch == pMmiCommReq->m_eCommType)
	{
		if (ECTRespCancelAttendMerch == pMmiCommResp->m_eCommType)
		{
			CMmiCancelAttendMerch *pMmiCancelAttendMerch = (CMmiCancelAttendMerch *)pMmiCommReq;
			CMmiRespCancelAttendMerch *pMmiRespCancelAttendMerch = (CMmiRespCancelAttendMerch *)pMmiCommResp;
			if (pMmiCancelAttendMerch && pMmiRespCancelAttendMerch)
			{
				// .....
			}
			
			// zhangbo 20090620 #�����䣬 ��Э��
			//...
// 			pMmiRespCancelAttendMerch->m_iMarketId = pMmiCancelAttendMerch->m_iMarketId;
// 			pMmiRespCancelAttendMerch->m_StrMerchCode = pMmiCancelAttendMerch->m_StrMerchCode;
// 
// 			pMmiRespCancelAttendMerch->m_aMerchMore.Copy(pMmiCancelAttendMerch->m_aMerchMore);
		}
		else
		{
			// ���������ƥ��ܿ������յ��˴���Ӧ���, ������
			// NULL;
		}
	}
		
	
	// ר��Ϊ�����������Ϣ
	if (ECTRespError == pMmiCommResp->m_eCommType)
	{
		if (NULL != pMmiCommReq)
		{
			CMmiRespError *pMmiRespError = (CMmiRespError *)pMmiCommResp;
			pMmiRespError->m_eReqCommType = pMmiCommReq->m_eCommType;
		}
	}

	// ��K�����ݵ������Ժ���Ч�Դ���
	// 1. ʱ�侫ȷ��ʲô�̶�
	// 2. ʱ����Ӧ�����ε���
	// 3. �۸�С�ڵ���0
	if (ECTRespMerchKLine == pMmiCommResp->m_eCommType)
	{
		CMmiRespMerchKLine *pMmiRespMerchKLine = (CMmiRespMerchKLine *)pMmiCommResp;
		
		CKLine KLine;
		CGmtTime Time, TimeNext;
		for (int32 i = pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetSize() - 1; i >= 0; i--)
		{
			KLine = pMmiRespMerchKLine->m_MerchKLineNode.m_KLines[i]; 

			if ( 0.0 >= KLine.m_fPriceOpen 
			  || 0.0 >= KLine.m_fPriceLow 
			  || 0.0 >= KLine.m_fPriceHigh
			  || 0.0 >= KLine.m_fPriceClose)
			{
				// �۸��� 0, �Ƿ�
				ASSERT(0);
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
				continue;
			}

			if ( KLine.m_fPriceHigh < KLine.m_fPriceLow)
			{
				// ��߼�С����ͼ�, �Ƿ�
				ASSERT(0);
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
				continue;
			}

			if (KLine.m_TimeCurrent == 0)
			{
#ifdef TRACE_DLG
				if (NULL != m_hwndTrace)
				{
					CString *pStrTrace = new CString;
					*pStrTrace = L"  ��dataengine: ���棺�յ�K�����ݰ���ȫ0���ݵ�K�ߣ� �����⽫�����ѻ����ݲ����޸��Ĵ��󣡣���";
					*pStrTrace += pMmiCommResp->GetSummary();
					::PostMessage(m_hwndTrace, 0x458, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
				}
#endif
				ASSERT(0);	// zhangbo 0704 #�����ã� ��������Ӧ�ó����������
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
				continue;
			}

			// Լ��ʱ��
			Time = KLine.m_TimeCurrent;
			if (EKTBMinute == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase)
				SaveMinute(Time);
			if (EKTB5Min == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase)
				SaveMinute(Time);
			if (EKTBHour == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase)
				SaveMinute(Time);
			if (EKTBDay == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase)
				SaveDay(Time);
			if (EKTBMonth == pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase)
				SaveDay(Time);

			if (Time != KLine.m_TimeCurrent)
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.SetAt(i, KLine);
			
			// ʱ��˳��
			if (i < pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetSize() - 1)
			{
				if (Time >= TimeNext)	// ʱ�䲻����, ɾ����������¼
				{
#ifdef TRACE_DLG
					if (NULL != m_hwndTrace)
					{
						CString *pStrTrace = new CString;
						*pStrTrace = L"  ��dataengine: ���棺�յ���K��������ʱ��˳��ǵ�����״���� ��Ҫ����Ƿ���������ݵ����⣡����";
						*pStrTrace += pMmiCommResp->GetSummary();
						::PostMessage(m_hwndTrace, 0x458, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
					}
#endif
					ASSERT(0);	// zhangbo 0704 #�����ã� ��������Ӧ�ó����������

					pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
					continue;
				}
			}
			
			// ��¼�¸ñʵ�ʱ�� 
			TimeNext = Time;
		}
	}
	

	// ��Tick���ݵ������Ժ���Ч�Դ���
	// 1. ʱ����Ӧ�����ε���
	if (ECTRespMerchTimeSales == pMmiCommResp->m_eCommType)
	{
		CMmiRespMerchTimeSales *pMmiRespMerchTimeSales = (CMmiRespMerchTimeSales *)pMmiCommResp;
		 
		CTick Tick;
		CMsTime Time, TimeNext;
		for (int32 i = pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetSize() - 1; i >= 0 ; i--)
		{
			Tick = pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks[i];

			// 
			Time = Tick.m_TimeCurrent;

			// ʱ��˳��
			if (i < pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetSize() - 1)
			{
				if (Time > TimeNext)	// ʱ�䲻����, ɾ����������¼
				{
					// ASSERT(0);
					TRACE(L"%d %s ʱ�䲻����, ɾ����������¼: ��ǰʱ��: %d ��һ��ʱ��: %d\n", 
						pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId, pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode.GetBuffer(), Time.m_Time.GetTime(), TimeNext.m_Time.GetTime());

					pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode.ReleaseBuffer();

					pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.RemoveAt(i);
					continue;
				}
			}
			
			// ��¼�¸ñʵ�ʱ�� 
			TimeNext = Time;
		}
	}
}

void CDataManager::RemoveAllReq()
{
	RGUARD(LockSingle, m_LockReqList, LockReqList);
	{
		
		for (int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0; i--)
		{
			DEL(m_MmiReqListPtr[i]);
		}
		
		m_MmiReqListPtr.RemoveAll();
	}
}


void CDataManager::RemoveAllWaitingReq()
{
	RGUARD(LockSingle, m_LockReqWaitingList, LockReqWaitingList);
	{
		for (int32 i = m_aMmiReqWaitingListPtr.GetSize() - 1; i >= 0; i--)
		{
			DEL(m_aMmiReqWaitingListPtr[i]);
		}
		
		m_aMmiReqWaitingListPtr.RemoveAll();
	}
}

bool32 CDataManager::Lock(HANDLE hMutex)
{
	if (NULL != hMutex)
	{
		if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0)
			return true;
	}
	
	return false;
}

void CDataManager::UnLock(HANDLE hMutex)
{
	if (NULL != hMutex)
	{
		ReleaseMutex(hMutex);
	}
}

int32 CDataManager::GetRespCount()
{
	return m_MmiRespListPtr.GetSize();
}

CMmiRespNode* CDataManager::PeekAndRemoveFirstResp()
{
	CMmiRespNode *pMmiRespNode = NULL;
	RGUARD(LockSingle, m_LockAnsList, LockAnsList);
	{
		if (m_MmiRespListPtr.GetSize() > 0)
		{
			pMmiRespNode = m_MmiRespListPtr[0];
			m_MmiRespListPtr.RemoveAt(0, 1);
		}
	}
	
	return pMmiRespNode;
}

void CDataManager::RemoveAllResp()
{
	RGUARD(LockSingle, m_LockAnsList, LockAnsList);
	{
		for (int32 i = m_MmiRespListPtr.GetSize() - 1; i >= 0; i--)
		{
			DEL(m_MmiRespListPtr[i]);
		}
		
		m_MmiRespListPtr.RemoveAll();
	}
}

void CDataManager::AddResp(CMmiRespNode *pMmiRespNode, bool32 bOfflineData)
{
	RGUARD(LockSingle, m_LockAnsList, LockAnsList);
	{
		if (NULL != pMmiRespNode)
		{
			m_MmiRespListPtr.Add(pMmiRespNode);
		}
	}
}

// ������ʱ�� + ��ֵ / 2, ���������ؼ�¼�ķ�����ʱ��
// ���㷨��һ��ʮ�־�ȷ�� ��ΪҪ���ǵ������������ʱ�� ��Ҫ����socketengineģ��������ݿ��ܴ�����ʱ���, ���ǻ����������������ʵʱ������Ӧ����100��������, ���Խ���
void CDataManager::PreTranslateSycnTime(uint32 uiTimeRequest, uint32 uiTimeResponse, uint32 uiTimeServer)
{
	if (!m_bHasSyncServerTime)
	{
		uint32 uiAdd = (uiTimeResponse - uiTimeRequest) / 2;
		uiAdd = (uiAdd + 500) / 1000;
		
		//
		uiTimeServer += uiAdd;
		
		uint32 uiTimeNow = timeGetTime();
		uiTimeServer += (uiTimeNow - uiTimeResponse) / 1000;
		
		CGmtTime TimeServer(uiTimeServer);
		NotifyDataSyncServerTimer(TimeServer);
		
		_DLLTRACE(L"***D ʱ��ͬ��Ϊ %d", TimeServer.GetTime());
		m_bHasSyncServerTime = true;	
	}
}


void CDataManager::RemoveRealtimeDataRespOfSpecifyServer(int32 iCommunicationId)
{
	RGUARD(LockSingle, m_LockAnsList, LockAnsList);
	{
		for (int32 i = m_MmiRespListPtr.GetSize() - 1; i >= 0; i--)
		{
			CMmiRespNode *pMmiRespNode = m_MmiRespListPtr[i];
			if (pMmiRespNode->m_iCommunicationId != iCommunicationId)
				continue;

			if (ECTRespRealtimePrice	== pMmiRespNode->m_pMmiCommResp->m_eCommType ||
				ECTRespRealtimeLevel2	== pMmiRespNode->m_pMmiCommResp->m_eCommType ||
				
				ECTAnsRegisterPushPrice	== pMmiRespNode->m_pMmiCommResp->m_eCommType ||
				ECTAnsRegisterPushTick	== pMmiRespNode->m_pMmiCommResp->m_eCommType ||
				ECTAnsRegisterPushLevel2== pMmiRespNode->m_pMmiCommResp->m_eCommType)
			{
				DEL(m_MmiRespListPtr[i]);
				m_MmiRespListPtr.RemoveAt(i, 1);
			}
		}
	}
}

void CDataManager::RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId)
{
	RGUARD(LockSingle, m_LockReqList, LockReqList);
	{
		// TRACE(L"CDataManager: ���������ʷ�����¼  ǰ���� %d ������ \n", m_MmiReqListPtr.GetSize());

		for ( int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0 ; i-- )
		{
			CMmiReqNode* pMmiReqNode = m_MmiReqListPtr.GetAt(i);

			if ( NULL == pMmiReqNode )
			{
				continue;
			}

			// ��Ӧ�ķ�����ͨѶ��:
			if (iCommunicationId == pMmiReqNode->m_iCommunicationId )
			{
				// TRACE(L"CDataManager: ���������ʷ�����¼: %d %d \n", pMmiReqNode->m_iCommSerialNo, pMmiReqNode->m_iCommunicationId);

				DEL(pMmiReqNode);
				m_MmiReqListPtr.RemoveAt(i, 1);		
			}
		}
 
		// TRACE(L"CDataManager: ���������ʷ�����¼  ���� %d ������ \n", m_MmiReqListPtr.GetSize());
	}
}

void CDataManager::ForceRequestType(E_DataSource eDataSource)
{
	if ( EDSCounts == eDataSource )
	{
		return;
	}

	m_eForceDataSource = eDataSource;
}
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
// 超时
const uint32 KReqTimeOutMSecond						= 1000 * 30 * 4;		// 请求应答超时时间
const uint32 KReqWaitingTimeOutMSecond				= 20;				// 待发请求处理时间

////////////////////////////////////////////////////////////////////////
// 定时清理长时间不回应的请求
const int32 KTimerIdClearMmiReqList					= 1;
const int32 KTimerPeriodClearMmiReqList				= 1000 * 5;		// ms unit

// 超时包大于 3 个的时候, 通知上层长时间未收到数据, 重连
const uint32 KiTimeOutReqsForPrompt					= 3;

// 有超过 5 秒没回的包, 就提醒上层
const uint32 KiTimeForPrompt						= 1000 * 5;	

// 定时处理脱机数据请求
const int32 KTimerIdProcessOfflineReq				= 2;
const int32 KTimerPeriodProcessOfflineReq			= 30;			// ms unit

// 定时处理待发队列请求
const int32 KTimerIdProcessMmiReqWaitingList		= 3;
const int32 KTimerPeriodProcessMmiReqWaitingList	= 16;			// ms unit

// 按条请求时， 总是请求某个数的整数倍数量，多请求几条
const uint32 KReqKLineGrowCount						= 20;			// 请求K线的取整数量
const uint32 KReqTimeSalesGrowCount					= 20;			// 请求分笔的取整数据

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
	m_iLastMmiReqId			= 10001;	// zhangbo 0513 #测试， 故意将该值设置为一个比较大的值 区别于通讯请求号	
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

	// 启动脱机数据处理线程
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
	
	// 找到所有的, 剔除（可能有重复的情况）
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

	// K 线
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
	
	// 写脱机数据
	for ( int32 i = 0; i < iSizeKLine; i++ )
	{
		m_OfflineDataManager.WriteOfflineKLines(pOffLineKLine[i].m_iMarketId, pOffLineKLine[i].m_StrMerchCode, pOffLineKLine[i].m_eKLineTypeBase, EOKTVipData, pOffLineKLine[i].m_aKLines);
	}
	
	// 清空队列
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
	
	// 写脱机数据
	for ( int32 i = 0; i < iSizeTick; i++ )
	{
		m_OfflineDataManager.WriteOfflineTicks(pOffLineTick[i].m_iMarketId, pOffLineTick[i].m_StrMerchCode, pOffLineTick[i].m_aTicks);
	}
	
	// 清空
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
	
	// 写数据
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

// 请求数据， 该数据可能在脱机数据中， 也可能存在于服务器
// 返会处理结果， <0 表示请求失败 =0 丢入队列中 >0 已经丢入发送队列
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

	if (EDSCommunication == eDataSource)	// 对于必须向服务器请求的数据， 不需要经过待发队列， 直接丢给服务器
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
			if (ECTReqMerchKLine != pMmiCommReq->m_eCommType &&	ECTReqMerchTimeSales != pMmiCommReq->m_eCommType)// 仅历史请求需要丢入发送队列				
			{
				bRequestDataImmediately = true;		
			}
		}
	}
		

	// 
	if (bRequestDataImmediately)
	{
		// 立即发往服务器
		RequestDataImmediately(pMmiCommReq, aMmiReqNodes, eDataSource, iCommunicationId);

		if (aMmiReqNodes.GetSize() > 0)
			return 1;		// 已经发送了请求
		else
			return -1;		// 发送失败
	}
	else
	{
		PushReqIntoWaitingQueue(pMmiCommReq, eDataSource, iCommunicationId);
		return 0;	// 丢入队列中
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
		if (EDSCommunication == eDataSource)	// 仅向服务器请求数据
		{
			CMmiReqNode *pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromCommunication(pMmiCommReq, pMmiReqNodeOut, iCommunicationId))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else if (EDSVipData == eDataSource)	// 仅从vipdata数据中获取
		{
			CMmiReqNode *pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromVipData(pMmiCommReq, pMmiReqNodeOut))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else if ( EDSOfflineData == eDataSource ) // 从真正的脱机数据中取
		{
			CMmiReqNode* pMmiReqNodeOut = NULL;
			if (RequestDataOnlyFromOfflineData(pMmiCommReq, pMmiReqNodeOut))
			{
				aMmiReqNodes.Add(pMmiReqNodeOut);
			}
		}
		else// if (EDSAuto == eDataSource)		// 自动方式， 有脱机数据， 优先脱机数据， 脱机不够的， 再从服务器取来放在脱机数据中， 再从脱机数据中取出返回
		{
			if (ECTReqMerchKLine == pMmiCommReq->m_eCommType)
			{
				CMmiReqMerchKLine *pMmiReqMerchKLine = (CMmiReqMerchKLine *)pMmiCommReq;
				
				CGmtTime TimeStartOfflineKLine, TimeEndOfflineKLine;
				int32 iOfflineKLineCount = 0;
				
				bool32 bGetOfflineKLineSnapshotOk = m_OfflineDataManager.GetOfflineKLinesSnapshot(pMmiReqMerchKLine->m_iMarketId, pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_eKLineTypeBase, EOKTVipData, TimeStartOfflineKLine, TimeEndOfflineKLine, iOfflineKLineCount);
				if (!bGetOfflineKLineSnapshotOk)
				{
					// 从脱机文件中读不到数据， 直接发送该请求给服务器
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
								// 完全在不在脱机数据里面， 二级完美 ：）
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}		
							else if (Req.m_TimeEnd > TimeEndOfflineKLine)
							{
								// 中间一段在本地存在
								{
									Req.m_TimeStart = TimeStartOfflineKLine;
									Req.m_TimeEnd = TimeEndOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 左段请求
								{
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 右段请求
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
								// 右边一段在本地存在
								{
									Req.m_TimeStart = TimeStartOfflineKLine;
									Req.m_TimeEnd = pMmiReqMerchKLine->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 左段请求
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
								// 左边一段在本地存在
								{
									Req.m_TimeStart = pMmiReqMerchKLine->m_TimeStart;
									Req.m_TimeEnd = TimeEndOfflineKLine;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 右段请求
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
								// 完全在脱机数据里面， 完美！
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
						}
						else // if (Req.m_TimeStart > TimeEndOfflineKLine)
						{
							// 完全在不在脱机数据里面
							// 此处为了保持脱机文件中的数据的完整性， 会尝试将数据请求与脱机数据搭界
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
						// 保持基数倍数量请求
						if ((unsigned)Req.m_iFrontCount % KReqKLineGrowCount != 0)	
						{
							Req.m_iFrontCount += (KReqKLineGrowCount - ((unsigned)Req.m_iFrontCount % KReqKLineGrowCount));
						}

						// 分各种情况
						if (Req.m_TimeSpecify < TimeStartOfflineKLine)
						{
							// 完全在不在脱机数据里面， 二级完美 ：）
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineKLine && Req.m_TimeSpecify <= TimeEndOfflineKLine)
						{
							// 看看有多少数据是在本地存在的
							CArray<CKLine, CKLine> aKLines;
							m_OfflineDataManager.ReadOfflineKLines(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_eKLineTypeBase, EOKTVipData, Req.m_TimeSpecify, -Req.m_iFrontCount, aKLines);
							if (aKLines.GetSize() >= Req.m_iFrontCount)	
							{
								// 本地数据足够请求了
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// 右段在本地
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

								// 左端请求数据
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
							// 特别说明：
							// 这里将请求分成两部分
							// 1. 请求最新时间和本地时间之间的空缺部分
							// 2. 再以本地文件的最后时间做原来的最新时间请求
							// 这里的请求很可能会造成一定的浪费， 因为不确定1的请求会有多少数据返回， 大多数情况下这一段的数据量是极少的， 所以， 浪费也是有限的
							// 这样处理就能充分利用本地脱机数据了
							
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
						// 保持基数倍数量请求
						if ((unsigned)Req.m_iCount % KReqKLineGrowCount != 0)	
						{
							Req.m_iCount += (KReqKLineGrowCount - ((unsigned)Req.m_iCount % KReqKLineGrowCount));
						}

						// 分各种情况
						if (Req.m_TimeSpecify > TimeEndOfflineKLine)
						{
							// 完全在不在脱机数据里面， 二级完美 ：）
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineKLine && Req.m_TimeSpecify <= TimeEndOfflineKLine)
						{
							// 看看有多少数据是在本地存在的
							CArray<CKLine, CKLine> aKLines;
							m_OfflineDataManager.ReadOfflineKLines(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_eKLineTypeBase, EOKTVipData, Req.m_TimeSpecify, Req.m_iCount, aKLines);
							if (aKLines.GetSize() >= Req.m_iCount)	
							{
								// 本地数据足够请求了
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// 左段在本地
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

								// 右端请求数据
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
							// 特别说明：
							// 这里将请求分成两部分
							// 1. 请求最新时间和本地时间之间的空缺部分
							// 2. 再以本地文件的最前时间做原来的指定时间请求
							// 这里的请求很可能会造成一定的浪费， 因为不确定1的请求会有多少数据返回， 大多数情况下这一段的数据量是极少的， 所以， 浪费也是有限的
							// 这样处理就能充分利用本地脱机数据了
							
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
					// 从脱机文件中读不到数据， 直接发送该请求给服务器
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
								// 完全在不在脱机数据里面， 二级完美 ：）
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}		
							else if (Req.m_TimeEnd > TimeEndOfflineTicks)
							{
								// 中间一段在本地存在
								{
									Req.m_TimeStart = TimeStartOfflineTicks;
									Req.m_TimeEnd = TimeEndOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 左段请求
								{
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeStart;
									Req.m_TimeEnd = TimeStartOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 右段请求
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
								// 右边一段在本地存在
								{
									Req.m_TimeStart = TimeStartOfflineTicks;
									Req.m_TimeEnd = pMmiReqMerchTimeSales->m_TimeEnd;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 左段请求
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
								// 左边一段在本地存在
								{
									Req.m_TimeStart = pMmiReqMerchTimeSales->m_TimeStart;
									Req.m_TimeEnd = TimeEndOfflineTicks;

									CMmiReqNode *pMmiReqNodeOut = NULL;
									if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
									{
										aMmiReqNodes.Add(pMmiReqNodeOut);
									}
								}
								
								// 右段请求
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
								// 完全在脱机数据里面， 完美！
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
						}
						else // if (Req.m_TimeStart > TimeEndOfflineTicks)
						{
							// 完全在不在脱机数据里面
							// 此处为了保持脱机文件中的数据的完整性， 会尝试将数据请求与脱机数据搭界
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
						// 保持基数倍数量请求
						if ((unsigned)Req.m_iFrontCount % KReqTimeSalesGrowCount != 0)	
						{
							Req.m_iFrontCount += (KReqTimeSalesGrowCount - ((unsigned)Req.m_iFrontCount % KReqTimeSalesGrowCount));
						}

						// 分各种情况
						if (Req.m_TimeSpecify < TimeStartOfflineTicks)
						{
							// 完全在不在脱机数据里面， 二级完美 ：）
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineTicks && Req.m_TimeSpecify <= TimeEndOfflineTicks)
						{
							// 看看有多少数据是在本地存在的
							CArray<CTick, CTick> aTicks;
							m_OfflineDataManager.ReadOfflineTicks(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_TimeSpecify, -Req.m_iFrontCount, aTicks);
							if (aTicks.GetSize() >= Req.m_iFrontCount)	
							{
								// 本地数据足够请求了
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// 右段在本地
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

								// 左端请求数据
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
							// 特别说明：
							// 这里将请求分成两部分
							// 1. 请求最新时间和本地时间之间的空缺部分
							// 2. 再以本地文件的最后时间做原来的最新时间请求
							// 这里的请求很可能会造成一定的浪费， 因为不确定1的请求会有多少数据返回， 大多数情况下这一段的数据量是极少的， 所以， 浪费也是有限的
							// 这样处理就能充分利用本地脱机数据了
							
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
						// 保持基数倍数量请求
						if ((unsigned)Req.m_iCount % KReqTimeSalesGrowCount != 0)	
						{
							Req.m_iCount += (KReqTimeSalesGrowCount - ((unsigned)Req.m_iCount % KReqTimeSalesGrowCount));
						}

						// 分各种情况
						if (Req.m_TimeSpecify > TimeEndOfflineTicks)
						{
							// 完全在不在脱机数据里面， 二级完美 ：）
							CMmiReqNode *pMmiReqNodeOut = NULL;
							if (RequestDataOnlyFromCommunication(&Req, pMmiReqNodeOut, iCommunicationId))
							{
								aMmiReqNodes.Add(pMmiReqNodeOut);
							}
						}
						else if (Req.m_TimeSpecify >= TimeStartOfflineTicks && Req.m_TimeSpecify <= TimeEndOfflineTicks)
						{
							// 看看有多少数据是在本地存在的
							CArray<CTick, CTick> aTicks;
							m_OfflineDataManager.ReadOfflineTicks(Req.m_iMarketId, Req.m_StrMerchCode, Req.m_TimeSpecify, Req.m_iCount, aTicks);
							if (aTicks.GetSize() >= Req.m_iCount)	
							{
								// 本地数据足够请求了
								CMmiReqNode *pMmiReqNodeOut = NULL;
								if (RequestDataOnlyFromVipData(&Req, pMmiReqNodeOut))
								{
									aMmiReqNodes.Add(pMmiReqNodeOut);
								}
							}
							else
							{
								// 左段在本地
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

								// 右端请求数据
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
							// 特别说明：
							// 这里将请求分成两部分
							// 1. 请求最新时间和本地时间之间的空缺部分
							// 2. 再以本地文件的最前时间做原来的指定时间请求
							// 这里的请求很可能会造成一定的浪费， 因为不确定1的请求会有多少数据返回， 大多数情况下这一段的数据量是极少的， 所以， 浪费也是有限的
							// 这样处理就能充分利用本地脱机数据了
							
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
			else	// 其他类型的请求， 直接发往服务器
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
	// 挨个通知
	NotifyDataServiceConnected(iServiceId);
}

void CDataManager::OnCommDataServiceDisconnected(int32 iServiceId)
{
	// 挨个通知
	NotifyDataServiceDisconnected(iServiceId);
}

void CDataManager::OnCommServerConnected(int32 iCommunicationId)
{
	// 挨个通知
	NotifyServerConnected(iCommunicationId);
}

void CDataManager::OnCommServerDisconnected(int32 iCommunicationId)
{
	TRACE(L"CDataManager: 服务器断线: %d \n", iCommunicationId);
	// 查找已接受队列， 如果存在该服务器推送的实时数据， 则删除， 避免因ServerDisconnect消息与CommResponse消息前后顺序混乱造成上层对实时数据处理的逻辑混乱
	RemoveRealtimeDataRespOfSpecifyServer(iCommunicationId);

	// 这个服务器上的已发请求删掉. 否则会出现重连上, 再发请求的时候, 还被 m_MmiReqListPtr 过滤掉, 请求没发出去的bug
	RemoveHistoryReqOfSpecifyServer(iCommunicationId);

	// 挨个通知
	NotifyServerDisconnected(iCommunicationId);
}

void CDataManager::OnCommServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType)
{
	//
	if ( ERCTWaitingForOnePack == eType || ERCTNoDataAtAll == eType || ERCTTooMuchTimeOutPack == eType )
	{
		_DLLTRACE(L"***D[%d] dataengine 收到 commengine 的通知: %d 很久没有收到数据, 需要重连! 继续通知上层 \n", eType, iCommunicationId);
	}
	
	//
	NotifyServerLongTimeNoRecvData(iCommunicationId, eType);
}

void CDataManager::OnCommResponse(int32 iCommSerialNo, int32 iCommunicationId, IN CMmiCommBase *pMmiCommResp)
{
	// 全局释放的时候会释放，在函数内屏蔽429错误
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

	// 在请求队列中， 查找看是否能找到对应的请求
	// 查找
	CMmiReqNode *pMmiReqNodeFound = NULL;
	{
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		
		for (int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0; i--)
		{
			CMmiReqNode *pMmiReqNode = m_MmiReqListPtr[i];
			
			// 对应的流水号
			if (iCommSerialNo == pMmiReqNode->m_iCommSerialNo)
			{
				// 找到就删除，避免锁住时间太长
				pMmiReqNodeFound = pMmiReqNode;
				m_MmiReqListPtr.RemoveAt(i);
				break;
			}
		}
	}
	
	// 处理
	bool bAddInQueue = false;
	int iRespReqId = -1;
	{
		
		// 对能找到和找不到的情况分别处理
		if (NULL == pMmiReqNodeFound)	// 找不到对应的流水号 
		{
			if ( pMmiCommResp->m_eCommType == ECTRespAddPushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTRespUpdatePushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTAnsRegisterPushPrice )
			{
				_DLLTRACE(L"***D dataengine 通讯号: %d 无法查询请求记录的回应包##==>: %s! \n", iCommunicationId, pMmiCommResp->GetSummary().GetBuffer());
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			else
			{
				_DLLTRACE(L"***D dataengine 通讯号: %d 无法查询请求记录的回应包##(%d): %s! \n", iCommunicationId, pMmiCommResp->m_eCommType, pMmiCommResp->GetSummary().GetBuffer());	
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			// 对几个特殊的返回数据做后续加工(需要对照请求包才能补充完整应答包)
			RepairResponseData(NULL, pMmiCommResp);
			
			// 如果该请求是可能存在的实时数据的应答
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
					strLogInfo.Format(_T("%s: dataengine: 收到服务器推送【%s】"),pMmiCommResp->GetSummary());
					m_vecLogInfo.push_back(strLogInfo);

/*					CString *pStrTrace = new CString;
					*pStrTrace = L"  ★dataengine: 收到服务器推送 【";
					*pStrTrace += pMmiCommResp->GetSummary();
*/		
				}
	#endif

 				// 记录脱机行情				
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

				// 添加到应答包队列中
				bAddInQueue = true;
			}
			else if ( ECTKickOut == pMmiCommResp->m_eCommType )
			{
				// 被踢了.
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: 被踢了【%s】"),strTime,pMmiCommResp->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif	
				// 添加到应答包队列中
				bAddInQueue = true;
			}
			else if ( ECTRespPlugIn == pMmiCommResp->m_eCommType )
			{
				// 插件模块的推送包
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
					// 添加到应答包队列中
					bAddInQueue = true;
				}
			}
			else
			{
				// 不知道哪里来的野孩子 :)
				// 这种数据要严加跟踪， 看是否是客户端或者服务器处理的bug引起
				// ASSERT(0);		
				// 不允许发生这种情况, 
				
				// zhangbo 0513 #测试代码， 调试期间检测异常， 正式上线不理会这种错误

#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: 警告：该应答包找不到对应的发送包， 并且不是实时推送数据， 该错误也可能是由于一个请求的应答实在太慢了， 以至于请求包被删除了！！！"),strTime,pMmiCommResp->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif
			}
		}
		else // 找到对应的流水号
		{
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 收到通知", m_hwndTrace);
			}

			// 查看是否已经应答完成了
			CMmiCommBase *pMmiCommReq = pMmiReqNodeFound->m_pMmiCommReq;
			ASSERT (NULL != pMmiCommReq);	// 不允许发生这种情况
			if (NULL == pMmiCommReq)
			{
				return;
			}
			// 对几个特殊的返回数据做后续加工(需要对照请求包才能补充完整应答包)
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 准备加工数据", m_hwndTrace);
			}

			RepairResponseData(pMmiCommReq, pMmiCommResp);

			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 完成加工数据", m_hwndTrace);
			}

			// 处理脱机数据
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 准备处理脱机数据", m_hwndTrace);
			}

			// 有脱机数据的请求， 将数据丢给脱机数据模块处理
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
				// 记录脱机行情数据
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
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 完成处理脱机数据", m_hwndTrace);
			}

			// 先处理跟服务器时间同步的问题, 测速包的不算. 因为会跟每个服务器都发测速包, 最后同步的时间可能不是真正连上的那台服务器
			if ( ECTRespNetTest != pMmiCommResp->m_eCommType )
			{
				if ( !m_bHasSyncServerTime )
				{
					_DLLTRACE(L"***D [%d] 同步服务器时间", iCommunicationId);
				}				
				PreTranslateSycnTime(pMmiReqNodeFound->m_uiTimeRequest, timeGetTime(), pMmiCommResp->m_uiTimeServer);
			}

			// 某台服务器认证成功后, 同步一次时间
			if ( ECTRespAuth == pMmiCommResp->m_eCommType )
			{
				m_bHasSyncServerTime = false;
				PreTranslateSycnTime(pMmiReqNodeFound->m_uiTimeRequest, timeGetTime(), pMmiCommResp->m_uiTimeServer);
				_DLLTRACE(L"***D [%d] 服务器认证成功, 同步服务器时间", iCommunicationId);					
			}
			
			if ( ECTRespMerchKLine == pMmiCommResp->m_eCommType )
			{
				_LogCheckTime(L"[==>Dataengine] Datamanager::OnCommResponse 通知上层", m_hwndTrace);
			}
	#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: 收到commengine应答 %d, %s"), strTime, iCommSerialNo, pMmiCommResp->GetSummary());				
				m_vecLogInfo.push_back(strLogInfo);
	#endif	
			
			// 添加到应答包队列中
			// 严重声明：为上层异步访问的需要， 这里申请的应答空间并不在本模块中释放， 而是要到上层中处理， 切记!!!
			iRespReqId = pMmiReqNodeFound->m_iMmiReqId;
			bAddInQueue = true;

			// 删除这条请求记录
			if ( pMmiCommResp->m_eCommType == ECTRespAddPushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTRespUpdatePushGeneralNormal
				|| pMmiCommResp->m_eCommType == ECTAnsRegisterPushPrice )
			{
				_DLLTRACE(L"***D dataengine 通讯号: %d 上的包回应##==>OK: %s  回应包:%s! \n", pMmiReqNodeFound->m_iCommunicationId, pMmiReqNodeFound->m_pMmiCommReq->GetSummary().GetBuffer(), pMmiCommResp->GetSummary().GetBuffer());	
				pMmiReqNodeFound->m_pMmiCommReq->GetSummary().ReleaseBuffer();
				pMmiCommResp->GetSummary().ReleaseBuffer();
			}
			else
			{
				_DLLTRACE(L"***D dataengine 通讯号: %d 上的包回应##OK: %s  回应包(%d):%s! \n", pMmiReqNodeFound->m_iCommunicationId, pMmiReqNodeFound->m_pMmiCommReq->GetSummary().GetBuffer(), pMmiCommResp->m_eCommType, pMmiCommResp->GetSummary().GetBuffer());		
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
		
		// 通知有包过来
		NotifyDataCommResponse();
	}
	else
	{
		TRACE(_T("dataengine有数据包没有办法通知上层\r\n"));
		// ASSERT( 0 );
		// 有包不需要添加
		// 先泄露着吧
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
	// 查看哪些请求包需要发送
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
			// 时间到了， 可以考虑发送请求了
			
			// 从发送队列中看看有没有同商品， 同类型的历史请求
			bool32 bHasSameReqObject = false;
			for (int32 j = 0; j < m_MmiReqListPtr.GetSize(); j++)
			{
				CMmiReqNode *pReqNode = m_MmiReqListPtr[j];
				CMmiCommBase *ppp = pReqNode->m_pMmiCommReq;
				
				uint32 uiTimeDiff = uiTimeNow - pReqNode->m_uiTimeRequest;
				// 超过特定时间的不认为是可以考虑的相同包
				if ( uiTimeDiff<KiTimeForPrompt && IsSameReqObject(pReqWaiting, ppp))
				{
					bHasSameReqObject = true;
					break;
				}
			}
			
			// 
			if (bHasSameReqObject)	// 如果有相同类型的数据， 暂时不处理
				continue;
			
			
			// 从等待队列中取走这个数据
			aReqWaitNeedSend.Add(pMmiReqWaitingNode);
			m_aMmiReqWaitingListPtr.RemoveAt(i, 1);
			i--;
		}
	}
	
	// 实际发送动作
	{
		for ( int32 i=0; i<aReqWaitNeedSend.GetSize(); ++i )
		{
			CMmiReqWaitingNode *pMmiReqWaitingNode = aReqWaitNeedSend[i];
			CMmiCommBase *pReqWaiting = pMmiReqWaitingNode->m_pMmiCommReq;
			if (pReqWaiting)
			{
				// ....
			}
			
			// 现在处理该请求
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: 提取队列请求 【%s】"),strTime,pMmiReqWaitingNode->m_pMmiCommReq->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif
			
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			RequestDataImmediately(pMmiReqWaitingNode->m_pMmiCommReq, aMmiReqNodes, pMmiReqWaitingNode->m_eDataSource, pMmiReqWaitingNode->m_iCommunicationId);
			
			// 删除等待对象
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

	// 找通讯号
	bool32 bFindCommunication = false;
	CPoint* pTimeOutReqPairs = (CPoint*)aTimeOutReqPairs.GetData();

	for ( int32 i = 0; i < aTimeOutReqPairs.GetSize(); i++ )
	{
		if ( pTimeOutReqPairs[i].x == pReqNode->m_iCommunicationId )
		{
			// 找到了, 超时记录加 1
			bFindCommunication = true;
			pTimeOutReqPairs[i].y += 1;
		}
	}

	// 没有找到, 增加这个记录
	if ( !bFindCommunication )
	{
		// 
		CPoint TimeOut;

		// 默认 x 保存通讯号
		TimeOut.x = pReqNode->m_iCommunicationId;

		// 默认 y 保存超时包个数, 初始值 1
		TimeOut.y = 1;

		aTimeOutReqPairs.Add(TimeOut);
	}
}

void CDataManager::OnTimerClearMmiReqList()
{
	// 超时的ReqNode
	CArray<int32, int32>	aReqNodeIdTimeout;
	// 超时统计的数据对(Communicationid, TimeOutReqs):
	CArray<CPoint, CPoint> aTimeOutReqPairs;
	aTimeOutReqPairs.RemoveAll();
	// 有超过 5 秒没有回的包, 就提醒网络不畅
	CArray<int32, int32> aCommunicationsNeedPrompt;

	// 超时包查询
	{
		// 
		RGUARD(LockSingle, m_LockReqList, LockReqList);
		{
			// 从后往前扫描所有的请求， 有超时的剔除
			
			uint32 uiTimeNow = timeGetTime();
			
			// 是否需要重连
			//bool32 bNeedReConnect	  = false;
			
			DWORD dwTime = timeGetTime();
			
			//
			for (int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0; i--)
			{
				CMmiReqNode *pMmiReqNode = m_MmiReqListPtr[i];
				if (NULL == pMmiReqNode)
				{
					ASSERT(0);	// 不允许发生这种情况
					m_MmiReqListPtr.RemoveAt(i, 1);
					continue;
				}
				
				// 
				if ( uiTimeNow - pMmiReqNode->m_uiTimeRequest >= KiTimeForPrompt )
				{
#ifdef _DEBUG
					TRACE(_T("CDataManager 通讯号: %d 上[包类型=%d, id=%d,%d, time=%d] 的回应时间超时 %d, 提示! tag=%d\n")
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
				TRACE(_T("CDataManager 通讯号: %d 上[包类型=%d, id=%d,%d, time=%d] 的回应时间超时 %d 被删除, 提示! tag=%d\n")
					, pMmiReqNode->m_iCommunicationId, pMmiReqNode->m_pMmiCommReq->m_eCommType
					, pMmiReqNode->m_iMmiReqId, pMmiReqNode->m_iCommSerialNo
					, pMmiReqNode->m_uiTimeRequest, uiTimeNow - pMmiReqNode->m_uiTimeRequest
					, dwTime
					);				
#endif
				
				// 添加到超时通知请求id
				aReqNodeIdTimeout.Add( m_MmiReqListPtr[i]->m_iMmiReqId );
				
				// 处理一下统计信息
				DealTimeOutInfomations(aTimeOutReqPairs, m_MmiReqListPtr[i]);
				
				// 删除该条记录
				DEL(m_MmiReqListPtr[i]);
				m_MmiReqListPtr.RemoveAt(i , 1);			
			}
		}
	}

	// 具体通知
	{
		// 单个id超时请求
		int32 i=0;
		for ( i=0; i<aReqNodeIdTimeout.GetSize(); ++i )
		{
			int32 iTimeoutId = aReqNodeIdTimeout[i];
			NotifyDataRequestTimeOut( iTimeoutId );
		}

		// 某连接上面有多个包超时通知
		for ( i = 0; i < aTimeOutReqPairs.GetSize(); i++ )
		{
			CPoint	TimeOutPair = aTimeOutReqPairs[i];
			
			// 这个通讯上的超时包已经太多了, 通知
			if ( TimeOutPair.y >= KiTimeOutReqsForPrompt )
			{
				// 
				_DLLTRACE(L"***D [%d] dataengine 通讯号: %d 上的超时包: %d 个 已经超过最大值: %d, 可能需要上层重连! \n", ERCTTooMuchTimeOutPack, TimeOutPair.x, TimeOutPair.y, KiTimeOutReqsForPrompt);				
				NotifyServerLongTimeNoRecvData(TimeOutPair.x, ERCTTooMuchTimeOutPack);	
				
				// 已经重连了就不要提示网络不顺畅
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

		// 某连接存在网络超时包，可能连接不是很流畅
		// 提示网络不顺畅
		for ( i = 0; i < aCommunicationsNeedPrompt.GetSize(); i++ )
		{
			TRACE(L"CDataManager 通讯号: %d 上有包的回应时间超过 5 s, 提示! \n", aCommunicationsNeedPrompt.GetAt(i));				
			NotifyServerLongTimeNoRecvData(aCommunicationsNeedPrompt.GetAt(i), ERCTPackTimeOut);
		}
	}
}

// 定时处理脱机数据请求包, 实现异步通知的目的
// 仅处理那些请求包流水号列表为空， 并且可能存在脱机数据的请求
void CDataManager::OnTimerProcessOfflineReq()
{
	CString strTime;
#ifdef TRACE_DLG
	GetLocalTime(strTime);
#endif
	// 可能能处理的。使用new出来的处理
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
				// 其他的请求脱机数据都无法满足
				// NULL;
			}
		}
	}
	
	// 读取脱机数据
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

				// 这里需要加上判断.数据的合法性
				RepairResponseData(NULL, pMmiRespMerchKLine);
				//
#ifdef TRACE_DLG
				CString strLogInfo;
				strLogInfo.Format(_T("%s: dataengine: 取到脱机数据 【】"),strTime,pMmiRespMerchKLine->GetSummary());
				m_vecLogInfo.push_back(strLogInfo);
#endif

				
				// 全局释放的时候会释放，在函数内屏蔽429错误
				//lint --e{429} 

				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespMerchKLine;
				AddResp(pNewRespNode, true);
				
				// 删除该条记录
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

				// 通知上层有包返回了
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

				// 这里需要加上判断.数据的合法性
				RepairResponseData(NULL, pMmiRespMerchTimeSales);

				//

				// 全局释放的时候会释放，在函数内屏蔽429错误
				//lint --e{429} 
				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespMerchTimeSales;
				AddResp(pNewRespNode);
				
				// 删除该条记录
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

				// 通知上层有包返回了
				NotifyDataCommResponse();
				continue;
			}
			else if (ECTReqRealtimePrice == pMmiCommReq->m_eCommType)
			{
				// 只在 offline 里有
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
				// 全局释放的时候会释放，在函数内屏蔽429错误
				//lint --e{429} 

				CMmiRespNode *pNewRespNode			= new CMmiRespNode;
				pNewRespNode->m_iMmiRespId			= pMmiReqNode->m_iMmiReqId;
				pNewRespNode->m_iCommunicationId	= -1;
				pNewRespNode->m_pMmiCommResp		= pMmiRespRealtimePrice;
				AddResp(pNewRespNode, true);
				
				// 删除该条记录
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
				
				// 通知上层有包返回了
				NotifyDataCommResponse();
				continue;
			}
			else
			{
				// 其他的请求脱机数据都无法满足
				// NULL;
			}
		}
	}

	// 清除临时分配的资源
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
	// TRACE(L"CDataManager: 数据服务断线 %d \n", iServiceId);

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

	// 添加记录到待发队列
	RGUARD(LockSingle, m_LockReqWaitingList, LockReqWaitingList);
	{
		uint32 uiFirstRemoveTime = timeGetTime();
		int32  iFirstRemoveIndex = -1;
		

		// 拿这个最新的请求与待发队列中的其他请求相比较， 看之前的那些请求是可以过滤的
		bool32 bIgnoreReq = false;
		for (int32 i = m_aMmiReqWaitingListPtr.GetSize() - 1; i >= 0 ; i--)
		{
			CMmiReqWaitingNode *pReqNode = m_aMmiReqWaitingListPtr[i];
			CMmiCommBase *ppp = pReqNode->m_pMmiCommReq;
			
			CMmiCommBase::E_CompareResult eCompareResult = CMmiCommBase::ECREqual;
			if (eDataSource == pReqNode->m_eDataSource &&				// 一定要是同样的请求， 才能保证有可比性
				iCommunicationId == pReqNode->m_iCommunicationId && 
				pMmiCommReq->CompareReq(ppp, eCompareResult))
			{
				if (CMmiCommBase::ECRMore == eCompareResult)	// 当前请求 > 队列中请求， 删除队列中该请求
				{
					// 保留该请求的时间, 留给新请求
					uiFirstRemoveTime = m_aMmiReqWaitingListPtr[i]->m_uiTimeRequest;
					iFirstRemoveIndex = i;

					// 该请求被咔嚓了
					DEL(m_aMmiReqWaitingListPtr[i]);
					m_aMmiReqWaitingListPtr.RemoveAt(i, 1);

	#ifdef TRACE_DLG
					CString strLogInfo;
					strLogInfo.Format(_T("%s: dataengine: 优化：替换了一个请求 【%s】"),strTime,pMmiCommReq->GetSummary());
					m_vecLogInfo.push_back(strLogInfo);
	#endif
				}
				else if (CMmiCommBase::ECRLess == eCompareResult || CMmiCommBase::ECREqual == eCompareResult) // 当前请求 <= 队列中请求， 该请求被过滤
				{
					bIgnoreReq = true;
					break;
				}
				else if (CMmiCommBase::ECRIntersection == eCompareResult)
				{
					// zhangbo 0822 #待补充
					//...
					// NULL;
				}
			}
		}
		
		// 添加到待发队列
		if (!bIgnoreReq)
		{
#ifdef TRACE_DLG
			CString strLogInfo;
			strLogInfo.Format(_T("%s: dataengine: 丢入请求队列 【%s】"), strTime,pMmiCommReq->GetSummary());
			m_vecLogInfo.push_back(strLogInfo);
#endif
			AppendWaitingReq(pMmiCommReq, eDataSource, iCommunicationId);

			// 修改新加的记录的时间
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
			strLogInfo.Format(_T("%s: dataengine: 该包被丢弃了【%s】"), strTime,pMmiCommReq->GetSummary());
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
	// 丢到待发队列
	// 复制一份请求数据出来， 因为传递进来的请求数据很可能在该函数执行完就被释放掉了
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);

	if (NULL == pCommNew)
		return;
	
	// 保存该请求记录
	CMmiReqWaitingNode *pMmiReqWaitingNode = new CMmiReqWaitingNode;
	m_aMmiReqWaitingListPtr.Add(pMmiReqWaitingNode);
	
	pMmiReqWaitingNode->m_eDataSource = eDataSource;
	pMmiReqWaitingNode->m_iCommunicationId = iCommunicationId;
	pMmiReqWaitingNode->m_pMmiCommReq = pCommNew;


	// t..fangz0510 
	if ( ECTReqMerchKLine == pMmiCommReq->m_eCommType )
	{
		_LogCheckTime(L"[Dataengine==>] CDataManager:: 加入请求队列", m_hwndTrace);
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
	strLogInfo.Format(_T("%s: 丢给commengine发送【%s】"),strTime, pMmiCommReq->GetSummary());
	m_vecLogInfo.push_back(strLogInfo);
#endif

	// t..fangz0520
	if ( ECTReqMerchKLine == pMmiCommReq->m_eCommType )
	{
		_LogCheckTime("[Dataengine==>] CDataManager:: 丢给commengine发送请求:", m_hwndTrace);
	}

	if ( ECTReqMerchTimeSales == pMmiCommReq->m_eCommType )
	{
		CMmiReqMerchTimeSales* ppp = (CMmiReqMerchTimeSales*)pMmiCommReq;

		if ( ppp->m_iFrontCount <= 0 )
		{
			ASSERT(0);
			TRACE(L"[datengine!!!]分笔请求错误: id = %d code = %s time = %d count = %d \n", ppp->m_iMarketId, ppp->m_StrMerchCode.GetBuffer(), ppp->m_TimeSpecify.GetTime(), ppp->m_iFrontCount);
			ppp->m_StrMerchCode.ReleaseBuffer();
		}
	}
	// 
	T_RequestRet stRequestRet;
	bool32 bRequestOk = m_pCommManager->RequestCommData(pMmiCommReq, stRequestRet, iCommunicationId);
	if (!bRequestOk)
		return false;

	// 复制一份请求数据出来， 因为传递进来的请求数据很可能在该函数执行完就被释放掉了
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
	{
		ASSERT(0);		// 不应该出这种错误
		return false;
	}
	
	// 保存该请求记录
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
	
	// 复制一份请求数据出来， 因为传递进来的请求数据很可能在该函数执行完就被释放掉了
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
		return false;

#ifdef TRACE_DLG
	if (NULL != m_hwndTrace)
	{
		CString *pStrTrace = new CString;
		*pStrTrace = L"  ☆dataengine: 丢给脱机队列 【";
		*pStrTrace += pCommNew->GetSummary();
		::PostMessage(m_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// 保存该请求记录
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
	
	// 复制一份请求数据出来， 因为传递进来的请求数据很可能在该函数执行完就被释放掉了
	CMmiCommBase *pCommNew = NewCopyCommReqObject(pMmiCommReq);
	if (NULL == pCommNew)
		return false;

#ifdef TRACE_DLG
	if (NULL != m_hwndTrace)
	{
		CString *pStrTrace = new CString;
		*pStrTrace = L"  ☆dataengine: 丢给脱机队列 【";
		*pStrTrace += pCommNew->GetSummary();
		::PostMessage(m_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// 保存该请求记录
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
// 对几个特殊的返回数据做后续加工(需要对照请求包才能补充完整应答包)
// 具体是哪几个应答包需要修正， 请参考commengine中带“特别说明”的注释内容
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
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
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
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
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
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
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
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
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
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
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
			
			// zhangbo 20090620 #待补充， 新协议
			//...
// 			pMmiRespCancelAttendMerch->m_iMarketId = pMmiCancelAttendMerch->m_iMarketId;
// 			pMmiRespCancelAttendMerch->m_StrMerchCode = pMmiCancelAttendMerch->m_StrMerchCode;
// 
// 			pMmiRespCancelAttendMerch->m_aMerchMore.Copy(pMmiCancelAttendMerch->m_aMerchMore);
		}
		else
		{
			// 与请求包不匹配很可能是收到了错误应答包, 正常！
			// NULL;
		}
	}
		
	
	// 专门为错误包补充信息
	if (ECTRespError == pMmiCommResp->m_eCommType)
	{
		if (NULL != pMmiCommReq)
		{
			CMmiRespError *pMmiRespError = (CMmiRespError *)pMmiCommResp;
			pMmiRespError->m_eReqCommType = pMmiCommReq->m_eCommType;
		}
	}

	// 对K线数据的完整性和有效性处理
	// 1. 时间精确到什么程度
	// 2. 时间上应该依次递增
	// 3. 价格小于等于0
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
				// 价格是 0, 非法
				ASSERT(0);
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
				continue;
			}

			if ( KLine.m_fPriceHigh < KLine.m_fPriceLow)
			{
				// 最高价小于最低价, 非法
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
					*pStrTrace = L"  ★dataengine: 警告：收到K线数据包含全0数据的K线， 该问题将导致脱机数据不可修复的错误！！！";
					*pStrTrace += pMmiCommResp->GetSummary();
					::PostMessage(m_hwndTrace, 0x458, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
				}
#endif
				ASSERT(0);	// zhangbo 0704 #调试用， 服务器不应该出现这种情况
				pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
				continue;
			}

			// 约束时间
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
			
			// 时间顺序
			if (i < pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetSize() - 1)
			{
				if (Time >= TimeNext)	// 时间不合理, 删除掉这条记录
				{
#ifdef TRACE_DLG
					if (NULL != m_hwndTrace)
					{
						CString *pStrTrace = new CString;
						*pStrTrace = L"  ★dataengine: 警告：收到的K线数据有时间顺序非递增的状况， 需要检查是否服务器数据的问题！！！";
						*pStrTrace += pMmiCommResp->GetSummary();
						::PostMessage(m_hwndTrace, 0x458, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
					}
#endif
					ASSERT(0);	// zhangbo 0704 #调试用， 服务器不应该出现这种情况

					pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.RemoveAt(i);
					continue;
				}
			}
			
			// 记录下该笔的时间 
			TimeNext = Time;
		}
	}
	

	// 对Tick数据的完整性和有效性处理
	// 1. 时间上应该依次递增
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

			// 时间顺序
			if (i < pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetSize() - 1)
			{
				if (Time > TimeNext)	// 时间不合理, 删除掉这条记录
				{
					// ASSERT(0);
					TRACE(L"%d %s 时间不合理, 删除掉这条记录: 当前时间: %d 下一个时间: %d\n", 
						pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId, pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode.GetBuffer(), Time.m_Time.GetTime(), TimeNext.m_Time.GetTime());

					pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode.ReleaseBuffer();

					pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.RemoveAt(i);
					continue;
				}
			}
			
			// 记录下该笔的时间 
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

// 服务器时间 + 差值 / 2, 即算做本地记录的服务器时间
// 该算法不一定十分精确， 因为要考虑到服务器处理的时间差， 还要考虑socketengine模块接收数据可能存在在时间差, 但是基本上与服务器间真实时间的误差应该在100毫秒以内, 可以接受
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
		
		_DLLTRACE(L"***D 时间同步为 %d", TimeServer.GetTime());
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
		// TRACE(L"CDataManager: 断线清除历史请求记录  前共有 %d 个请求 \n", m_MmiReqListPtr.GetSize());

		for ( int32 i = m_MmiReqListPtr.GetSize() - 1; i >= 0 ; i-- )
		{
			CMmiReqNode* pMmiReqNode = m_MmiReqListPtr.GetAt(i);

			if ( NULL == pMmiReqNode )
			{
				continue;
			}

			// 对应的服务器通讯号:
			if (iCommunicationId == pMmiReqNode->m_iCommunicationId )
			{
				// TRACE(L"CDataManager: 断线清除历史请求记录: %d %d \n", pMmiReqNode->m_iCommSerialNo, pMmiReqNode->m_iCommunicationId);

				DEL(pMmiReqNode);
				m_MmiReqListPtr.RemoveAt(i, 1);		
			}
		}
 
		// TRACE(L"CDataManager: 断线清除历史请求记录  后共有 %d 个请求 \n", m_MmiReqListPtr.GetSize());
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
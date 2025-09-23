// OffLineData.cpp: implementation of the COffLineData class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <process.h>
#include <algorithm>
#include "OffLineData.h"
#include "ReportScheme.h"
#include "viewdata.h"
#include "AbsOfflineDataManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// 请求的片段, 一次处理这么多个, 完全处理好了再处理下一批. 避免同时大量的请求

// K 线
static const int32 s_KiDealSectionKLine			= 30;

// 行情
static const int32 s_KiDealSectionRealtimePrice	= 50;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COffLineData::COffLineData(CViewData* pViewData)
{
	ASSERT(NULL != pViewData);
	m_pViewData = pViewData;

	m_bStopWork		   = true;
	m_hThreadDownLoad  = NULL;
	m_hThreadWriteFile = NULL;
	m_pNotify		   = NULL;
	m_bTimeToNextSection= false;
	// m_bTimeToWrite		= false;
}

COffLineData::~COffLineData()
{
	
}

void COffLineData::ResetData()
{
	StopDownLoad();

	//
	if ( NULL != m_pViewData )
	{
		m_pViewData->DelViewDataListner(this);
	}
	
	//	
	m_bStopWork		   = true;
	m_hThreadDownLoad  = NULL;
	m_hThreadWriteFile = NULL;
	m_pNotify		   = NULL;

	//
	{
		RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
		m_aRequestIDs.clear();
	}
	
	//
	{
		RGUARD(LockSingle, m_LockDownLoadOffLineParam, LockDownLoadOffLineParam);
		m_DownLoadOffLineParam.Clear();
	}
	
	//
	{
		RGUARD(LockSingle, m_LockRealtimePriceToWrite, LockRealtimePriceToWrite);
		m_aRealtimePriceToWrite.RemoveAll();
	}
	
	//
	{
		RGUARD(LockSingle, m_LockKLinesToWrite, LockKLinesToWrite);
		m_aKLinesToWrite.RemoveAll();
	}
}

void COffLineData::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{
	// 保存数据, 给写文件线程记录
	
	// 看看是不是我们发的请求:
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), iReqId);

	if ( m_aRequestIDs.end() != itFind )
	{
		// 正确:
		CRealtimePrice stLocal = RealtimePrice;
		CArray<CRealtimePrice*, CRealtimePrice*> aData;
		aData.Add(&stLocal);

		SaveDownLoadRealTimePriceData(aData);
	}
}

void COffLineData::OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine)
{
	// 保存数据, 给写文件线程记录
	// 看看是不是我们发的请求:
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), iMmiReqId);
	
	if ( m_aRequestIDs.end() != itFind )
	{
		// 正确:
		SaveDownLoadKlineNodeData(iMmiReqId, pMmiRespMerchKLine->m_MerchKLineNode);
	}
	else
	{
		// TRACE(L"忽略的请求: %d %d-%s\n", iMmiReqId, pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId, pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode);
	}
}

//
CGmtTime COffLineData::GetLastTime(E_KLineTypeBase eType)
{
	if ( NULL == m_pViewData )
	{
		ASSERT(0);
		return 0;
	}

	CGmtTime TimeBegin,TimeEnd; 
	int32 iCount;

	// 找到 000001 的时间作为标准
	if ( m_pViewData->m_OfflineDataManager.GetOfflineKLinesSnapshot(0, L"000001", eType, EOKTOfflineData, TimeBegin, TimeEnd, iCount) )
	{
		return TimeEnd;
	}

	return 0;
}

void COffLineData::DownLoadRealTimePriceData(const T_DownLoadOffLine& stDownLoadOffLine)
{
	StartDownLoad(stDownLoadOffLine);
}

void COffLineData::DownLoadKLineData(const T_DownLoadOffLine& stDownLoadOffLine)
{
	StartDownLoad(stDownLoadOffLine);
}

void COffLineData::DownLoadF10Data(const T_DownLoadOffLine& stDownLoadOffLine)
{
	StartDownLoad(stDownLoadOffLine);
}

void COffLineData::StartDownLoad(const T_DownLoadOffLine& stDownLoadOffLine)
{
	RGUARD(LockSingle, m_LockDownLoadOffLineParam, LockDownLoadOffLineParam);
	m_DownLoadOffLineParam = stDownLoadOffLine;
	
	// TRACE(L"开始下载: Type = %d KLine = %d MerchCounts = %d Begin = %d End = %d\n %s - %s\n%s - %s\n", m_DownLoadOffLineParam.m_eReqType, m_DownLoadOffLineParam.m_eKLineBase, m_DownLoadOffLineParam.m_aMerchs.size(), m_DownLoadOffLineParam.m_TimeBeing.GetTime(), m_DownLoadOffLineParam.m_TimeEnd.GetTime(),
	//	m_DownLoadOffLineParam.m_aMerchs[0]->m_MerchInfo.m_StrMerchCode, m_DownLoadOffLineParam.m_aMerchs[0]->m_MerchInfo.m_StrMerchCnName,
	//	m_DownLoadOffLineParam.m_aMerchs[m_DownLoadOffLineParam.m_aMerchs.size() - 1]->m_MerchInfo.m_StrMerchCode, m_DownLoadOffLineParam.m_aMerchs[m_DownLoadOffLineParam.m_aMerchs.size() - 1]->m_MerchInfo.m_StrMerchCnName);		
	
	StopDownLoad();

	//
	m_bStopWork = false;

	//
	BeginThreadDownLoad();
	BeginThreadWriteOffLineData();
}

void COffLineData::StopDownLoad()
{
	m_bStopWork = true;

	//
	StopThreadDownLoad();
	StopThreadWriteOffLineData();
}

bool32 COffLineData::GetMerchsBySection(OUT vector<CMerch*>& aMerchs, bool32 bRealTime)
{
	aMerchs.clear();

	int32 iSection = s_KiDealSectionKLine;
	if ( bRealTime )
	{
		iSection = s_KiDealSectionRealtimePrice;
	}

	//
	if ( (int32)m_DownLoadOffLineParam.m_aMerchs.size() <= iSection )
	{
		// 不够了, 取完后本地就空了
		aMerchs = m_DownLoadOffLineParam.m_aMerchs;
		m_DownLoadOffLineParam.m_aMerchs.clear();

		return true;
	}
	
	// 拷贝数据, 减少本地的
	aMerchs.resize(iSection);
	
	CMerch** pNow = &(*aMerchs.begin());
	CMerch** pSrc = &(*m_DownLoadOffLineParam.m_aMerchs.begin());
	
	//
	memcpyex(pNow, pSrc, iSection * sizeof(CMerch*));

	//
	int32 iTimes = iSection;
	while (iTimes)
	{
		m_DownLoadOffLineParam.m_aMerchs.erase(m_DownLoadOffLineParam.m_aMerchs.begin());
		iTimes -= 1;
	}

	//
	return false;
}

void COffLineData::SaveDownLoadRealTimePriceData(const CArray<CRealtimePrice*, CRealtimePrice*>& aRealtimePriceListPtr)
{
	RGUARD(LockSingle, m_LockRealtimePriceToWrite, LockRealtimePriceToWrite);

	//
	int32 iSize = aRealtimePriceListPtr.GetSize();

	//
	for ( int32 i = 0; i < iSize; i++ )
	{
		CRealtimePrice* pRealtimePrice = aRealtimePriceListPtr[i];
		if ( NULL == pRealtimePrice )
		{
			continue;
		}

		//
		CRealtimePrice stRealtimePrice = *pRealtimePrice;
		m_aRealtimePriceToWrite.Add(stRealtimePrice);
	}

	// TRACE(L"收到行情数据回包: 商品个数: %d\n%d-%s --- %d-%s\n", iSize, m_aRealtimePriceToWrite[0].m_iMarketId, m_aRealtimePriceToWrite[0].m_StrMerchCode, m_aRealtimePriceToWrite[m_aRealtimePriceToWrite.GetSize() - 1].m_iMarketId, m_aRealtimePriceToWrite[m_aRealtimePriceToWrite.GetSize() - 1].m_StrMerchCode);
}

void COffLineData::SaveDownLoadKlineNodeData(int32 iReqID, const CMerchKLineNode& MerchKLineNode)
{
	RGUARD(LockSingle, m_LockKLinesToWrite, LockKLinesToWrite);

	//
	T_OffLineRespKLineData stDataSave;
	
	stDataSave.m_iRespID		= iReqID;
	stDataSave.m_iMarketId		= MerchKLineNode.m_iMarketId;
	stDataSave.m_StrMerchCode	= MerchKLineNode.m_StrMerchCode;
	stDataSave.m_eKLineTypeBase = MerchKLineNode.m_eKLineTypeBase;
	stDataSave.m_aKLines.Copy(MerchKLineNode.m_KLines);

	//
	m_aKLinesToWrite.Add(stDataSave);

	//
	// TRACE(L"收到K线数据回包: %d-%s 待记录商品个数: %d \n", stDataSave.m_iMarketId, stDataSave.m_StrMerchCode, m_aKLinesToWrite.GetSize());
}

bool32 COffLineData::BeginThreadDownLoad()
{
	//
	m_bTimeToNextSection = true;
	
	//
	m_hThreadDownLoad = (HANDLE)_beginthreadex(NULL, 0, CallBackThreadDownLoad, this, 0, NULL);	
	if ( NULL == m_hThreadDownLoad )
	{
		return false;
	}

	return true;
}

void COffLineData::StopThreadDownLoad()
{
	if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadDownLoad, INFINITE) )
	{
		DEL_HANDLE(m_hThreadDownLoad);
	}	
}

unsigned int COffLineData::CallBackThreadDownLoad(LPVOID lpParam)
{
	COffLineData* pThis = (COffLineData*)lpParam;

	while ( !pThis->m_bStopWork )
	{
		bool32 bFinished = pThis->ThreadDownLoad();		
		if ( bFinished )
		{
			break;
		}

		//
		Sleep(100);
	}

	// TRACE(L"请求数据线程退出\n");
	return 1;
}

bool32 COffLineData::ThreadDownLoad()
{
	// 具体的下载处理:
	RGUARD(LockSingle, m_LockDownLoadOffLineParam, LockDownLoadOffLineParam);
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);

	//
	if ( ECTReqRealtimePrice == m_DownLoadOffLineParam.m_eReqType )
	{	
		if ( !m_bTimeToNextSection )
		{
			return false;
		}
		
		// 
		m_bTimeToNextSection = false;
	
		// 设置监听
		m_aRequestIDs.clear();
		m_eListenType = ECTReqRealtimePrice;
		m_pViewData->AddViewDataListner(this);

		// 下载行情数据
		CMmiReqRealtimePrice Req;
		
		bool32 bValidFirstMerch = false;
		
		//
		vector<CMerch*> aMerchsNow;
		bool32 bFinished = GetMerchsBySection(aMerchsNow, true);

		for ( uint32 i = 0; i < aMerchsNow.size(); i++ )
		{
			CMerch* pMerch = aMerchsNow[i];
			if ( NULL == pMerch )
			{
				continue;
			}
			
			//
			if ( !bValidFirstMerch )
			{
				Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
				Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
				
				bValidFirstMerch = true;
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
				
				//
				Req.m_aMerchMore.Add(MerchKey);
			}			
			
			
			// 默认下载行情的时候下载财务数据 CFinanceData
			bool32 bFuture = CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);

			if ( !bFuture )
			{
				CMmiReqPublicFile ReqFinance;
				ReqFinance.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
				ReqFinance.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
				ReqFinance.m_ePublicFileType= EPFTF10;
				
				CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqFinanceNodes;
				m_pViewData->m_pDataManager->RequestData(&ReqFinance, aMmiReqFinanceNodes, EDSCommunication);			
			}			
			
		}
	
		//	
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		
		int32 iReVal = m_pViewData->m_pDataManager->RequestData(&Req, aMmiReqNodes, EDSCommunication);	
		
		if ( (iReVal < 0) || (1 != aMmiReqNodes.GetSize()) )
		{
			// 请求错误
			ASSERT(0);
			return true;
		}
		
		// 保存 ID 号
		int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
		m_aRequestIDs.push_back(iReqID);

		//
		if ( NULL != m_pNotify && bFinished )
		{
			m_pNotify->OnAllRequestSended(ECTReqRealtimePrice);
		}

		return bFinished;
	}
	else if ( ECTReqMerchKLine == m_DownLoadOffLineParam.m_eReqType )
	{
		if ( !m_bTimeToNextSection )
		{
			return false;
		}

		// 设置监听
		m_bTimeToNextSection = false;
		m_aRequestIDs.clear();

		m_eListenType = ECTReqMerchKLine;
		m_pViewData->AddViewDataListner(this);

		vector<CMerch*> aMerchsNow;
		bool32 bFinished = GetMerchsBySection(aMerchsNow, false);

		for ( uint32 i = 0; i < aMerchsNow.size(); i++ )
		{
			CMerch* pMerch = aMerchsNow[i];
			if ( NULL == pMerch )
			{
				continue;
			}
			
			//
			CMmiReqMerchKLine Req;
			
			Req.m_iMarketId		 = pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	 = pMerch->m_MerchInfo.m_StrMerchCode;
			Req.m_eKLineTypeBase = m_DownLoadOffLineParam.m_eKLineBase;
			Req.m_eReqTimeType	 = ERTYSpecifyTime;
			Req.m_TimeStart		 = m_DownLoadOffLineParam.m_TimeBeing;
			Req.m_TimeEnd		 = m_DownLoadOffLineParam.m_TimeEnd;

			//	
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			
			int32 iReVal = m_pViewData->m_pDataManager->RequestData(&Req, aMmiReqNodes, EDSCommunication);	
			
			if ( (iReVal < 0) || (1 != aMmiReqNodes.GetSize()) )
			{
				// 请求错误
				ASSERT(0);
				return true;
			}
			
			// 保存 ID 号
			int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
			m_aRequestIDs.push_back(iReqID);

			// TRACE(L"线程发送K线请求: 请求号:%d 商品: %d %s \n", iReqID, Req.m_iMarketId, Req.m_StrMerchCode);
		}

		//
		if ( NULL != m_pNotify && bFinished )
		{
			m_pNotify->OnAllRequestSended(ECTReqMerchKLine);
		}

		//
		return bFinished;
	}

	return true;
}

bool32 COffLineData::BeginThreadWriteOffLineData()
{
	//
	m_hThreadWriteFile = (HANDLE)_beginthreadex(NULL, 0, CallBackThreadWriteOffLineData, this, 0, NULL);
	
	if ( NULL == m_hThreadWriteFile )
	{
		return false;
	}

	return true;
}

void COffLineData::StopThreadWriteOffLineData()
{
	if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadWriteFile, INFINITE) )
	{
		DEL_HANDLE(m_hThreadWriteFile);
	}
}

unsigned int COffLineData::CallBackThreadWriteOffLineData(LPVOID lpParam)
{
	COffLineData* pThis = (COffLineData*)lpParam;

	while ( !pThis->m_bStopWork )
	{
		if (pThis->ThreadWriteOffLineData())
		{
			break;
		}

		Sleep(100);
	}

	// TRACE(L"写数据线程退出\n");
	return 1;
}

bool32 COffLineData::ThreadWriteOffLineData()
{
	bool32 bOK = false;

	E_CommType eReqType;

	{
		RGUARD(LockSingle, m_LockDownLoadOffLineParam, LockDownLoadOffLineParam);
		eReqType = m_DownLoadOffLineParam.m_eReqType;
	}

	//
	if ( ECTReqRealtimePrice == eReqType )
	{
		bOK = WriteRealtimePriceData();
	}
	else if ( ECTReqMerchKLine == eReqType )
	{
		bOK = WriteKLineData();
	}
	else if ( ECTReqMerchF10 == eReqType )
	{
		bOK = WriteF10Data();
	}	
	
	return bOK;
}

bool32 COffLineData::WriteRealtimePriceData()
{		

	//
	if ( NULL == m_pViewData )
	{
		return true;
	}

	//
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	RGUARD(LockSingle, m_LockRealtimePriceToWrite, LockRealtimePriceToWrite);

	for ( int32 i = 0; i < m_aRealtimePriceToWrite.GetSize(); i++ )
	{		
		//
		CRealtimePrice& stData = m_aRealtimePriceToWrite.GetAt(i);

		// 写数据
		// 底层动态库会自动记录行情数据的, 不用再写一次, 节省时间
		// m_pViewData->m_OfflineDataManager.WriteOfflineRealtimePrice(stData);
		// TRACE(L"写文件线程, 记录行情数据: %d-%s\n", stData.m_iMarketId, stData.m_StrMerchCode);

		// 通知:		
		if ( NULL != m_pNotify )
		{
			CMerch* pMerch = NULL;
			m_pViewData->m_MerchManager.FindMerch(stData.m_StrMerchCode, stData.m_iMarketId, pMerch);
			if ( NULL != pMerch )
			{				
				m_pNotify->OnDataDownLoading(pMerch, ECTReqRealtimePrice);
			}
			else
			{
				ASSERT(0);
			}			
		}
	}

	//
	if ( m_aRealtimePriceToWrite.GetSize() > 0 )
	{
		m_bTimeToNextSection = true;
		
		//
		m_aRealtimePriceToWrite.RemoveAll();
		
		// 清空请求ID队列
		m_aRequestIDs.clear();
		
		// 通知完成
		if ( m_DownLoadOffLineParam.m_aMerchs.empty() && NULL != m_pNotify )
		{
			m_pNotify->OnDataDownLoadFinished(ECTReqRealtimePrice);
			return true;
		}
	}

	return false;
}

bool32 COffLineData::WriteKLineData()
{
	//

	//
	if ( NULL == m_pViewData )
	{
		return true;
	}
	
	//
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	RGUARD(LockSingle, m_LockKLinesToWrite, LockKLinesToWrite);

	//
	T_OffLineRespKLineData* pKLineData = (T_OffLineRespKLineData*)m_aKLinesToWrite.GetData();	

	for( int32 i = 0; i < m_aKLinesToWrite.GetSize(); i++ )
	{
		T_OffLineRespKLineData* pDataNow = &pKLineData[i];

		//
		if ( NULL == pDataNow )
		{
			continue;
		}

		// 写数据
		m_pViewData->m_OfflineDataManager.WriteOfflineKLines(pDataNow->m_iMarketId, pDataNow->m_StrMerchCode, pDataNow->m_eKLineTypeBase, EOKTOfflineData, pDataNow->m_aKLines);

		// TRACE(L"写文件线程, 记录k线数据: %d-%s\n", pDataNow->m_iMarketId, pDataNow->m_StrMerchCode);

		// 通知
		if ( NULL != m_pNotify )
		{
			CMerch* pMerch = NULL;
			m_pViewData->m_MerchManager.FindMerch(pDataNow->m_StrMerchCode, pDataNow->m_iMarketId, pMerch);
			if ( NULL != pMerch )
			{
				m_pNotify->OnDataDownLoading(pMerch, ECTReqMerchKLine);
			}
			else
			{
				ASSERT(0);
			}
		}

		// 清除请求ID
		std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), pDataNow->m_iRespID);
		if ( m_aRequestIDs.end() != itFind )
		{
			m_aRequestIDs.erase(itFind);
		}
	}
	
	// 准备请求下一批
	if ( NULL != pKLineData && m_aRequestIDs.empty() )
	{
		m_bTimeToNextSection = true;	
	}

	// 清空这一批计算的 K线
	m_aKLinesToWrite.RemoveAll();

	// 全部计算完了, 通知一下
	if ( m_DownLoadOffLineParam.m_aMerchs.empty() && m_aRequestIDs.empty() && NULL != m_pNotify )
	{
		// TRACE(L"写文件线程, 记录K 线数据完毕\n");
		m_pNotify->OnDataDownLoadFinished(ECTReqMerchKLine);
		return true;
	}
	
	return false;
}

bool32 COffLineData::WriteF10Data()
{
	return true;
}

// IndexChsStkMideCore.cpp: implementation of the CIndexChsStkMideCore class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <process.h>
#include <algorithm>


#include "IoViewShare.h"
#include "MerchManager.h"
#include "IndexChsStkMideCore.h"
#include "ReportScheme.h"
#include "SaneIndex.h"

#include "dlgindexchoosestock.h"
#include "UserBlockManager.h"
#include "PathFactory.h"
#include "BlockManager.h"
#include "IoViewBase.h"
#include "IoViewKLine.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

static const int32 KiReqKLineNumsForIndexChsStk	= 300;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndexChsStkMideCore::CIndexChsStkMideCore()
{
	m_aMerchsToChoose.clear();
	m_aRequestIDs.clear();
	m_aChsStkResults.clear();
	m_aCalcParams.RemoveAll();

	m_pAbsCenterManager				= NULL;
	m_pDlgShow				= NULL;
	m_hThreadReq			= NULL;
	m_hThreadRecv			= NULL;
	m_hThreadCalc			= NULL;
	
	m_iErrTimesRecv			= 0;	
	m_bThreadReqExit		= true;
	m_bThreadRecvExit		= true;
	m_bThreadCalcExit		= true;

	m_uiUserCycle			= (UINT)-1;
	m_eTimeInterval			= ENTICount;
	m_eOffLineDataGetType	= EODGTCount;
	
	GetViewData();
}

CIndexChsStkMideCore::~CIndexChsStkMideCore()
{
	DEL_HANDLE(m_hThreadCalc);
	DEL_HANDLE(m_hThreadRecv);
	DEL_HANDLE(m_hThreadReq);
	
	if (m_pAbsCenterManager)
	{
		m_pAbsCenterManager->SetIndexChsStkMideCore(NULL);
	}
	
	//m_pDlgShow,m_pAbsCenterManager不能释放，其它地方有用，有释放 -cfj
	//lint --esym(1540, m_pDlgShow)
	//lint --esym(1540, m_pAbsCenterManager)
}

//////////////////////////////////////////////////////////////////////////
static bool32 GetMerchsByMarketName(IN CAbsCenterManager * pAbsCenterManager, IN CString StrMarketName, OUT CArray<CMerch*, CMerch*> &aMerchs)
{
    // 通过版块名称得到这个板块下面所有的商品
    if ( StrMarketName.GetLength() <= 0)
    {
        return false;
    }
    
    if (NULL == pAbsCenterManager)
    {
        return false;
    }
	
    aMerchs.RemoveAll();
    
    // 先查物理板块	
    CMarket * pMarket = NULL;
    pAbsCenterManager->GetMerchManager().FindMarket(StrMarketName,pMarket);
    
    if ( NULL != pMarket)
    {
		aMerchs.Copy(pMarket->m_MerchsPtr);
		return true;
    }

    // 再查系统板块:    
	if ( CSysBlockManager::Instance()->GetMerchsInBlock(StrMarketName, aMerchs) )
	{
		return true;	
	}
	
    // 再查用户板块:	
	if ( CUserBlockManager::Instance()->GetMerchsInBlock(StrMarketName, aMerchs) )
	{
		return true;
	}
    	
    return false;
}

void CIndexChsStkMideCore::SetChsStkParams(OUT int32& iMerchNums, IN E_OfflineDataGetType eOffLineDataGetType, IN const std::vector<CString>& aBlocks,  IN const std::vector<T_IndexChsStkCondition>& aConditions, IN E_NodeTimeInterval eTimeInterval, IN UINT uiUserCycle /* = 0 */)
{	
	// 设置参数:
	if ( NULL == m_pAbsCenterManager )
	{
		GetViewData();
	}

	iMerchNums = 0;
	
	// 1: 是否请求历史数据:
	m_eOffLineDataGetType = eOffLineDataGetType;

	// 2: 商品 (根据板块得到商品)
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		return;
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( NULL == pDoc )
	{
		return;
	}

	if ( NULL == m_pAbsCenterManager )
	{
		if ( !GetViewData() )
		{
			return;
		}	
	}

	// 清空关注商品
	m_pAbsCenterManager->RemoveAttendMerch(EA_Choose);

	for ( int32 i = 0; i < aBlocks.size(); i++ )
	{
		CString StrBlockName = aBlocks[i];

		CArray<CMerch*, CMerch*> aMerchs;
		GetMerchsByMarketName(m_pAbsCenterManager, StrBlockName, aMerchs);

		for ( int32 j = 0; j < aMerchs.GetSize(); j++ )
		{
			CMerch* pMerch = aMerchs.GetAt(j);
			if ( NULL != pMerch )
			{
				m_aMerchsToChoose.push_back(pMerch);
			}
			
			// 添加关注商品, 不要被定时器清除了
			CSmartAttendMerch SmartAttendMerch;
			SmartAttendMerch.m_pMerch = pMerch;
			SmartAttendMerch.m_iDataServiceTypes = EDSTKLine;
			
			m_pAbsCenterManager->AddAttendMerch(SmartAttendMerch,EA_Choose);			
		}
	}

	//
	iMerchNums = m_aMerchsToChoose.size();
	
	// 3: 条件
	m_aConditions = aConditions;

	// 4: 周期
	m_eTimeInterval = eTimeInterval;

	// 5: 自定义周期
	m_uiUserCycle	= uiUserCycle;
}

bool32 CIndexChsStkMideCore::GetViewData()
{
	//
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		return false;
	}
	
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( NULL == pDoc )
	{
		return false;
	}
	
	m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	return true;	
}

void CIndexChsStkMideCore::SetParentDlg(CDlgIndexChooseStock* pDlgShow)
{
	ASSERT(NULL != pDlgShow);
	m_pDlgShow = pDlgShow;
}

bool32 CIndexChsStkMideCore::RequestData(CMerch* pMerch, bool32 bReqAll)
{
	if ( NULL == pMerch )
	{
		return false;
	}
	
	//
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		return false;
	}
	
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( NULL == pDoc )
	{
		return false;
	}

	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	// 指定时间段请求	
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;
	
	int32 iScale = 1;
	
	if ( !GetTimeIntervalInfo(m_uiUserCycle, m_uiUserCycle, m_eTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale) )
	{
		//ASSERT(0);	
		return false;
	}
	
	if ( 0 == iScale )
	{
		//ASSERT(0);
		iScale = 1;
	}

	if ( bReqAll )
	{
		// 全部请求
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= eKLineTypeCompare;								// 使用原始K线周期
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType		= ERTYFrontCount;
		info.m_TimeSpecify		= m_pAbsCenterManager->GetServerTime();
		info.m_iFrontCount		= KiReqKLineNumsForIndexChsStk * iScale;
		
		//	
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		
		int32 iReVal = pDoc->m_pAbsDataManager->RequestData(&info, aMmiReqNodes, EDSCommunication);	
		
		if ( (iReVal < 0) || (1 != aMmiReqNodes.GetSize()) )
		{
			// 请求错误
			//ASSERT(0);
			return false;
		}

		// 保存 ID 号
		int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
		m_aRequestIDs.push_back(iReqID);

		TRACE(L"发请求==> 请求号: %d 商品名: %s ,共发请求 %d 个\n", iReqID, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aRequestIDs.size());
	}
	else
	{
		// 增量请求
		CGmtTime TimeNow	= m_pAbsCenterManager->GetServerTime();
	
		// 1: 得到本地数据的时间
		CGmtTime TimeLocalStart;
		CGmtTime TimeLocalEnd;
		int32	 iKLineCountLocal;

		if ( !pDoc->m_pAbsDataManager->GetOfflineKLinesSnapshot(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineTypeCompare, EOKTIndexData, TimeLocalStart, TimeLocalEnd, iKLineCountLocal) )
		{
			// 读取本地数据失败, 全部请求			
			return(RequestData(pMerch, true));
		}

		if ( TimeLocalEnd > TimeNow )
		{
			// 本地数据的时间可能有错误	,全部请求				
			return(RequestData(pMerch, true));
		}

		// 增量请求
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= eKLineTypeCompare;								// 使用原始K线周期
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType		= ERTYSpecifyTime;
		info.m_TimeStart		= TimeLocalEnd;
		info.m_TimeEnd			= TimeNow;
		
		//	
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		
		int32 iReVal = pDoc->m_pAbsDataManager->RequestData(&info, aMmiReqNodes, EDSCommunication);	
		
		if ( (iReVal < 0) || (1 != aMmiReqNodes.GetSize()) )
		{
			// 请求错误
			//ASSERT(0);		
			return false;
		}
			
		// 保存 ID 号
		int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
		m_aRequestIDs.push_back(iReqID);

		//
		TRACE(L"发请求==> 请求号: %d 商品名: %s ,共发请求 %d 个\n", iReqID, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aRequestIDs.size());
	}

	return true;
}

void CIndexChsStkMideCore::RequestWeightData()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}
	
	//
	for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
	{
		TRACE(L"请求除权数据 %d \n", i);

		if ( m_bThreadReqExit )
		{
			TRACE(L"请求线程退出0 \n");			
			return;
		}

		//
		CMerch* pMerch = m_aMerchsToChoose[i];
		if ( NULL == pMerch )
		{
			continue;
		}
		
		if ( !CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
		{
			CMmiReqPublicFile info;
			info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
			info.m_ePublicFileType	= EPFTWeight;
			info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;
			
			m_pAbsCenterManager->RequestViewData((CMmiCommBase*)&info);
			
			// 通知父窗口,改变进度条
			if ( NULL != m_pDlgShow )
			{
				CString* pStrMsg = new CString;			
				*pStrMsg = pMerch->m_MerchInfo.m_StrMerchCnName;
				
				m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg), LPARAM(CDlgIndexChooseStock::EPTWeight));				
			}
			
			Sleep(50);
		}
	}	
}

bool32 CIndexChsStkMideCore::ProcessRespKLine(const T_IndexChsStkRespData& stIndexChsStkRespData)
{
	RGUARD(LockSingle, m_LockCalcParm, LockCalcParm);
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);

	int32 iRespID = stIndexChsStkRespData.m_iRespID;

	TRACE(L"回请求<== 请求号: %d \n", iRespID);
	
	// 有没有这个 ID	 
	std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), iRespID);
	
	if ( itFind == m_aRequestIDs.end() )
	{		
		return false;
	}

	// 返回数据为空
	if ( stIndexChsStkRespData.m_aKLines.GetSize() <= 0 || stIndexChsStkRespData.m_aKLines.GetSize() >= 100000 )
	{		
		// 
		CString StrMerchCode = stIndexChsStkRespData.m_StrMerchCode;
		TRACE(L" 收到商品 %d %s 数据错误: K 线个数为: %d \n", stIndexChsStkRespData.m_iMarketId, StrMerchCode.GetBuffer(), stIndexChsStkRespData.m_aKLines.GetSize());
		m_aRequestIDs.erase(itFind);			
		return false;
	}

	// 文档类指针
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		return false;
	}
	
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( NULL == pDoc || !pDoc->m_pAbsDataManager)
	{
		return false;
	}
	
	// 有没有这个商品
	CMerch* pMerch = NULL;
	m_pAbsCenterManager->GetMerchManager().FindMerch(stIndexChsStkRespData.m_StrMerchCode, stIndexChsStkRespData.m_iMarketId, pMerch);
	if ( NULL == pMerch )
	{
		m_aRequestIDs.erase(itFind);		
		return false;
	}

	bool32 bFuture = CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);

	//
	if ( EODGTAll == m_eOffLineDataGetType )
	{
		// 给线程处理的参数
		T_IndexChsStkCalcParam stIndexChsStkCalcParam;
		
		stIndexChsStkCalcParam.m_iRespID = iRespID;		
		stIndexChsStkCalcParam.m_bFuture = bFuture;
		stIndexChsStkCalcParam.m_pMerch	 = pMerch;
		stIndexChsStkCalcParam.m_aKLines.Copy(stIndexChsStkRespData.m_aKLines);

		m_aCalcParams.Add(stIndexChsStkCalcParam);
	}
	else if ( EODGTAdd == m_eOffLineDataGetType )
	{
		// 保存本地,跟本地拼接起来,再把数据读出来,用于计算		
		pDoc->m_pAbsDataManager->WriteOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stIndexChsStkRespData.m_eKLineTypeBase, EOKTIndexData, stIndexChsStkRespData.m_aKLines);
		
		CArray<CKLine, CKLine> aKLines;
		pDoc->m_pAbsDataManager->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stIndexChsStkRespData.m_eKLineTypeBase, EOKTIndexData, aKLines);
		if ( aKLines.GetSize() <= 0 )
		{
			//ASSERT(0);
			m_aRequestIDs.erase(itFind);
			return false;
		}

		// 丢给线程处理
		if ( aKLines.GetSize() <= 0 )
		{
			//ASSERT(0);
			m_aRequestIDs.erase(itFind);	
			return false;
		}
		
		// 给线程处理的参数
		T_IndexChsStkCalcParam stIndexChsStkCalcParam;
		
		stIndexChsStkCalcParam.m_iRespID = iRespID;		
		stIndexChsStkCalcParam.m_bFuture = bFuture;
		stIndexChsStkCalcParam.m_pMerch	 = pMerch;
		stIndexChsStkCalcParam.m_aKLines.Copy(aKLines);		

		m_aCalcParams.Add(stIndexChsStkCalcParam);
	}
	else
	{
		//ASSERT(0);
	}

	return true;
}

bool32 CIndexChsStkMideCore::CombinKLine(IN CMerch* pMerch, IN const CArray<CKLine, CKLine>& aKLineSrc, OUT CArray<CKLine, CKLine>& aKlineDst)
{
	// 先清空数据
	aKlineDst.RemoveAll();

	if ( NULL == pMerch || aKLineSrc.GetSize() <= 0 )
	{
		return false;
	}

	//
	CMarketIOCTimeInfo MarketIOCTimeInfo;
	if (!pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), MarketIOCTimeInfo, pMerch->m_MerchInfo))
	{
		//ASSERT(0);
		return false;
	}

	switch (m_eTimeInterval)
	{
	case ENTIMinute:
		{
			aKlineDst.SetSize(aKLineSrc.GetSize());	
			memcpyex(aKlineDst.GetData(), aKLineSrc.GetData(), sizeof(CKLine) * aKLineSrc.GetSize());						
		}
		break;
	case ENTIMinute5:
		{
			aKlineDst.SetSize(aKLineSrc.GetSize());	
			memcpyex(aKlineDst.GetData(), aKLineSrc.GetData(), sizeof(CKLine) * aKLineSrc.GetSize());	
		}
		break;					
	case ENTIMinute15:
		{
			if (!CMerchKLineNode::CombinMinuteN(pMerch, 15, aKLineSrc, aKlineDst))
			{
				//ASSERT(0);
				return false;
			}	
		}
		break;
	case ENTIMinute30:
		{
			if (!CMerchKLineNode::CombinMinuteN(pMerch, 30, aKLineSrc, aKlineDst))				
			{
				//ASSERT(0);
				return false;
			}
		}
		break;
	case ENTIMinute60:
		{	
			aKlineDst.SetSize(aKLineSrc.GetSize());	
			memcpyex(aKlineDst.GetData(), aKLineSrc.GetData(), sizeof(CKLine) * aKLineSrc.GetSize());	
		}	
		break;
	case ENTIMinuteUser:
		{					
			if (!CMerchKLineNode::CombinMinuteN(pMerch, m_uiUserCycle, aKLineSrc, aKlineDst))				
			{
				//ASSERT(0);
				return false;
			}
		}	
		break;
	case ENTIDay:
		{
			aKlineDst.SetSize(aKLineSrc.GetSize());	
			memcpyex(aKlineDst.GetData(), aKLineSrc.GetData(), sizeof(CKLine) * aKLineSrc.GetSize());	
		}
		break;
	case ENTIWeek:
		{
			if (!CMerchKLineNode::CombinWeek(aKLineSrc, aKlineDst))
			{
				//ASSERT(0);
				return false;
			}
		}
		break;
	case ENTIDayUser:
		{
			if (!CMerchKLineNode::CombinDayN(aKLineSrc, m_uiUserCycle, aKlineDst))
			{
				//ASSERT(0);
				return false;
			}
		}
		break;
	case ENTIMonth:
		{
			aKlineDst.SetSize(aKLineSrc.GetSize());	
			memcpyex(aKlineDst.GetData(), aKLineSrc.GetData(), sizeof(CKLine) * aKLineSrc.GetSize());
		}
		break;			
		
	case ENTIQuarter:
		{
			if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 3, aKlineDst))
			{
				//ASSERT(0);
				return false;
			}
		}
		break;
	case ENTIYear:
		{
			if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 12, aKlineDst))
			{
				//ASSERT(0);
				return false;
			}
		}
		break;
	default:
		{
			return false;
		}		
	}	

	return true;
}

void CIndexChsStkMideCore::OnRecvTimeOut()
{
	StopChooseStock(true);
}

bool32 CIndexChsStkMideCore::BePassedCloseTime(CMerch* pMerch)
{
	//
	if ( NULL == pMerch || NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}
	
	// 
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	
	CMarketIOCTimeInfo RecentTradingDayTime;
	if ( pMerch->m_Market.GetRecentTradingDay(TimeNow, RecentTradingDayTime, pMerch->m_MerchInfo) )
	{
		CGmtTime TimeRecentClose(RecentTradingDayTime.m_TimeClose.m_Time);
		
		CGmtTime TimeNowDay = TimeNow;
		SaveDay(TimeNowDay);
		
		CGmtTime TimeRecentCloseDay = TimeRecentClose;
		SaveDay(TimeRecentCloseDay);
		
		if ( TimeNowDay != TimeRecentCloseDay )
		{
			// 日期不同, 直接返回			
			return true;				
		}
		else
		{
			// 日期相同, 
			if (TimeNow >= TimeRecentClose)	
			{				
				return true;
			}	
		}		
	}
	
	return false;
}

bool32 CIndexChsStkMideCore::BeginChooseStock()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}
	
	//
	if ( EODGTAll == m_eOffLineDataGetType || EODGTAdd == m_eOffLineDataGetType )
	{
		// 这两种情况才请求数据		
		BeginReqThread();	
		BeginRecvThread();
	}
	else if ( EODGTNone == m_eOffLineDataGetType )
	{
		// 直接本地取数据算:
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if ( NULL == pApp )
		{
			return false;
		} 
		
		CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
		if ( NULL == pDoc || !pDoc->m_pAbsDataManager)
		{
			return false;
		}
		
		for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
		{
			// 取商品
			CMerch* pMerch = m_aMerchsToChoose[i];
			
			if ( NULL == pMerch )
			{
				continue;
			}
			
			bool32 bFuture = CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
			
			// 得到基本周期
			E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
			E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;
			
			int32 iScale = 1;
			
			if ( !GetTimeIntervalInfo(m_uiUserCycle, m_uiUserCycle, m_eTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale) )
			{
				//ASSERT(0);	
				continue;
			}
			
			// 取 k 线数据
			CArray<CKLine, CKLine> aKLines;
			pDoc->m_pAbsDataManager->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineTypeCompare, EOKTIndexData, aKLines);
			if ( aKLines.GetSize() <= 0 || aKLines.GetSize() >= 100000 )
			{		
				// 
				TRACE(L"商品 %s %d 数据错误: K 线个数为: %d \n", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), aKLines.GetSize());
				continue;
			}

			// 取除权数据
			CArray<CWeightData,CWeightData&> aWeightData;
			UINT uiCrc32 = 0;
			
			if ( CIoViewBase::ReadWeightDataFromLocalFile(pMerch, aWeightData, uiCrc32) )
			{				
				pMerch->m_bHaveReadLocalWeightFile = true;
				
				// 设置CMerch 中相关字段的值				
				pMerch->m_uiWeightDataCRC = uiCrc32;
				
				int32 iSize = aWeightData.GetSize();
				pMerch->m_aWeightDatas.SetSize(iSize);				
				memcpyex(pMerch->m_aWeightDatas.GetData(), aWeightData.GetData(), sizeof(CWeightData) * iSize);					
			}
			
			// 丢给线程处理			
			// 给线程处理的参数
			// 本地数据,用数组的大小做 ID 索引号
			
			int32 iRespID = m_aCalcParams.GetSize();
			m_aRequestIDs.push_back(iRespID);
			
			T_IndexChsStkCalcParam stIndexChsStkCalcParam;
						
			stIndexChsStkCalcParam.m_iRespID = iRespID;		
			stIndexChsStkCalcParam.m_bFuture = bFuture;
			stIndexChsStkCalcParam.m_pMerch	 = pMerch;
			stIndexChsStkCalcParam.m_aKLines.Copy(aKLines);
			
			m_aCalcParams.Add(stIndexChsStkCalcParam);	
		}

		// 开始计算
		if ( m_aCalcParams.GetSize() > 0 )
		{			
			BeginCalcThread();
		}
		else
		{
			return false;
		}		
	}
	else 
	{
		// 错误:
		//ASSERT(0);
		return false;
	}
		 
	return true;
}

void CIndexChsStkMideCore::StopChooseStock(bool32 bErr /*= false*/)
{
	m_bThreadReqExit  = true;
	m_bThreadRecvExit = true;
	m_bThreadCalcExit = true;

	bool32 bThreadCalcExit = false;
	bool32 bThreadReqExit  = false;
	bool32 bThreadRecvExit = false;

	// 请求线程
	if ( NULL != m_hThreadReq )	
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadReq, INFINITE) )
		{
			// 退出了
			bThreadReqExit = true;
		}	
	}
	else
	{
		bThreadReqExit = true;
	}

	// 接收线程
	if ( NULL != m_hThreadRecv )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadRecv, INFINITE) )
		{
			// 退出了
			bThreadRecvExit = true;
		}
	}
	else
	{
		bThreadRecvExit = true;
	}

	// 计算线程
	if ( NULL != m_hThreadCalc )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadCalc, INFINITE) )
		{
			// 退出了
			bThreadCalcExit = true;
		}
	}
	else
	{
		bThreadCalcExit = true;
	}

	// 都退出了
	if ( bThreadReqExit && bThreadRecvExit && bThreadCalcExit )
	{
		if ( NULL != m_pDlgShow )
		{
			m_pDlgShow->SetIndexChsStkResult(m_aChsStkResults);
			m_pDlgShow->DisplayDatas();
			m_pDlgShow->ResetProgress();
			m_pDlgShow->StopCheckTimeOutTimer();
			m_pDlgShow->ChangeControlState(true);
			m_pDlgShow->UpdateData(FALSE);
		}
				
		OnChooseStockFinish();
	}

	// 
	if ( bErr )
	{
		// 避免没有窗口的时候还弹出来选股超时的提示框
		if ( NULL != m_pDlgShow && NULL != m_pDlgShow->GetSafeHwnd() )
		{			
			::MessageBox(m_pDlgShow->GetSafeHwnd(), L"数据下载超时...", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		}		
	}
}

void CIndexChsStkMideCore::OnChooseStockFinish()
{
	// 选股完毕,清空所有数据:
	m_aMerchsToChoose.clear();
	m_aRequestIDs.clear();
	m_aChsStkResults.clear();
	m_aCalcParams.RemoveAll();
	
	m_iErrTimesRecv			= 0;
//	m_pAbsCenterManager->SetIndexChsStkMideCore(NULL);

	m_hThreadReq			= NULL;
	m_bThreadReqExit		= true;

	m_hThreadRecv			= NULL;
	m_bThreadRecvExit		= true;

	m_hThreadCalc			= NULL;	
	m_bThreadCalcExit		= true;
	
	m_uiUserCycle			= (UINT)-1;
	m_eTimeInterval			= ENTICount;
	m_eOffLineDataGetType	= EODGTCount;			
}

void CIndexChsStkMideCore::OnRecvKLineData(int32 iMarketId, CString StrMerchCode, bool32 bFinished /*= false*/)
{
	// 通知父窗口, 更新进度条
	if ( NULL != m_pDlgShow )
	{
		CMerch* pMerch = NULL;
		if ( NULL != m_pAbsCenterManager )
		{
			m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch);
		}
		
		CString* pStrMsg = new CString;			
		
		if ( NULL != pMerch )
		{
			*pStrMsg = pMerch->m_MerchInfo.m_StrMerchCnName;
		}
		else
		{
			//ASSERT(0);
			*pStrMsg = L"";
		}
		
		m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg),  LPARAM(CDlgIndexChooseStock::EPTKLine));	
		
		if ( bFinished )
		{
			// 完成了,关掉定时器
			m_pDlgShow->StopCheckTimeOutTimer();
		}
	}
}

bool32 CIndexChsStkMideCore::BeIndexChsStkReqID(int32 iRespID)
{
	std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), iRespID);

	if ( itFind == m_aRequestIDs.end() )
	{
		return false;
	}

	return true;
}

bool32 CIndexChsStkMideCore::BeMeetCondition(int32 iCondition, UINT uiParam, IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes)
{
	int32 iNodeSize = aSaneNodes.GetSize();
	if ( iNodeSize <= 0 )
	{
		return false;
	}

	//
	if ( uiParam < 1 )
	{
		return false;
	}

	if ( iNodeSize < uiParam )
	{
		return false;
	}

	//
	T_SaneIndexNode* pNodes = (T_SaneIndexNode*)aSaneNodes.GetData();
	T_SaneIndexNode Node = pNodes[iNodeSize - uiParam];

	if ( 0 == iCondition )
	{
		/*
		// ？？天内发出买入开仓信号
		for ( int32 iIndexNode = iNodeSize - 1; iIndexNode >= (iNodeSize - uiParam); iIndexNode-- )
		{
			if ( iIndexNode < 0 || iIndexNode >= iNodeSize )
			{
				return false;
			}

			T_SaneIndexNode Node = pNodes[iIndexNode];
			
			if ( ESISDTOC == Node.m_eSaneIndexState 			  	  
			  || ESISKCDO == Node.m_eSaneIndexState)
			{
				return true;
			}
		}
		*/

		// 第 ?? 天发出买入开仓信号
		if ( ESISDTOC == Node.m_eSaneIndexState || ESISKCDO == Node.m_eSaneIndexState )
		{
			return true;
		}
	}
	else if ( 1 == iCondition )
	{
		/*
		// ？？天内发出卖出开仓信号
		for ( int32 iIndexNode = iNodeSize - 1; iIndexNode >= (iNodeSize - uiParam); iIndexNode-- )
		{
			if ( iIndexNode < 0 || iIndexNode >= iNodeSize )
			{
				return false;
			}

			T_SaneIndexNode Node = pNodes[iIndexNode];
			if ( ESISKTOC == Node.m_eSaneIndexState 			  
			  || ESISDCKO == Node.m_eSaneIndexState)			  
			{
				return true;
			}
		}
		*/

		// 第 ?? 天发出卖出开仓信号
		if ( ESISKTOC == Node.m_eSaneIndexState || ESISDCKO == Node.m_eSaneIndexState )			  
		{
			return true;
		}
	}
	else if ( 2 == iCondition )
	{
		/*
		// ？？天内发出清仓信号
		for ( int32 iIndexNode = iNodeSize - 1; iIndexNode >= (iNodeSize - uiParam); iIndexNode-- )
		{
			if ( iIndexNode < 0 || iIndexNode >= iNodeSize )
			{
				return false;
			}

			T_SaneIndexNode Node = pNodes[iIndexNode];
			if ( ESISDTCC == Node.m_eSaneIndexState 			  
			  || ESISKTCC == Node.m_eSaneIndexState
			  || ESISDCKO == Node.m_eSaneIndexState
			  || ESISKCDO == Node.m_eSaneIndexState)			  
			{
				return true;
			}
		}
		*/

		// ？？天内发出清仓信号
		if ( ESISDTCC == Node.m_eSaneIndexState 			  
			|| ESISKTCC == Node.m_eSaneIndexState
			|| ESISDCKO == Node.m_eSaneIndexState
			|| ESISKCDO == Node.m_eSaneIndexState)			  
		{
			return true;
		}
	}
		
	return false;
}

bool32 CIndexChsStkMideCore::FiltResultWithConditions(IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes)
{
	// 根据条件过滤结果:
	if ( m_aConditions.empty() )
	{
		return true;
	}

	if ( aSaneNodes.GetSize() <= 0 )
	{
		return false;
	}
	
	for ( int32 i = 0; i < m_aConditions.size(); i++ )
	{
		T_IndexChsStkCondition stIndexChsStkCondition = m_aConditions[i];
				
		if ( stIndexChsStkCondition.m_bNeedCalc )
		{
			if ( BeMeetCondition(i, stIndexChsStkCondition.m_uiParam, aSaneNodes) )
			{
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// 线程相关
bool32 CIndexChsStkMideCore::BeginReqThread()
{	
	m_bThreadReqExit = false;
	m_hThreadReq = CreateThread(NULL, 0, ThreadRequestData, this, 0, NULL);
	
	if ( NULL == m_hThreadReq )
	{
		return false;
	}
	
	return true;
}

DWORD WINAPI CIndexChsStkMideCore::ThreadRequestData(LPVOID pParam)
{
	CIndexChsStkMideCore* pThis = (CIndexChsStkMideCore*)pParam;
	if ( NULL == pThis )
	{
		return 0;
	}

	TRACE(L"请求线程开始 \n");		
		
	//
	if ( !pThis->m_bThreadReqExit )
	{
		pThis->ThreadRequestData();
	}
	
	TRACE(L"请求线程退出3 \n");

	
	// 开始计算线程
 	 if ( !pThis->m_bThreadReqExit )
 	 {
 	 	pThis->BeginCalcThread();
 	 }

	pThis->m_bThreadReqExit = true;

	return 0;
}

bool32 CIndexChsStkMideCore::ThreadRequestData()
{
 	// 先把除权的数据请求了:

 	RequestWeightData();
 
	if ( m_bThreadReqExit )
	{
		return false;
	}

	//
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	// 开始检验超时的计时器
	if ( NULL != m_pDlgShow )
	{
		m_pDlgShow->StartCheckTimeOutTimer();
	}

	////
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	m_pAbsCenterManager->SetIndexChsStkMideCore(this);
	m_pAbsCenterManager->SetIndexChsStkReqNums(m_aMerchsToChoose.size());

	//
	if ( EODGTAll == m_eOffLineDataGetType )
	{
		// 全部重新下载:
		for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
		{
			if ( m_bThreadReqExit )
			{		
				TRACE(L"请求线程退出1 \n");
				return false;
			}

			//
			CMerch* pMerch = m_aMerchsToChoose[i];			
			if ( NULL == pMerch )
			{
				continue;
			}

			//
			if ( !RequestData(pMerch, true) )
			{
			  m_pAbsCenterManager->SetIndexChsStkReqNums(m_aMerchsToChoose.size()-1);
				ASSERT(0);
			}
				
			//
			Sleep(50);
		}
	}
	else if ( EODGTAdd == m_eOffLineDataGetType )
	{
		// 增量下载:	
		for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
		{
			if ( m_bThreadReqExit )
			{
				TRACE(L"请求线程退出2 \n");	 
				return false;
			}

			//
			CMerch* pMerch = m_aMerchsToChoose[i];			
			if ( NULL == pMerch )
			{
				continue;
			}
			
			//
			if ( !RequestData(pMerch, false) )
			{
				m_pAbsCenterManager->SetIndexChsStkReqNums(m_aMerchsToChoose.size()-1);
				ASSERT(0);
			}	
			
			//
			Sleep(50);
		}
	}
	else
	{
		ASSERT(0);
	}

	return true;
}

bool32 CIndexChsStkMideCore::BeginRecvThread()
{
	m_bThreadRecvExit = false;
	m_hThreadRecv = CreateThread(NULL, 0, ThreadRecvData, this, 0, NULL);
	
	if ( NULL == m_hThreadRecv )
	{
		return false;
	}

	return true;
}

DWORD WINAPI CIndexChsStkMideCore::ThreadRecvData(LPVOID pParam)
{
	CIndexChsStkMideCore* pThis = (CIndexChsStkMideCore*)pParam;

	if ( NULL == pThis )
	{
		return 0;
	}

	//
	TRACE(L"接收线程开始 \n");
	
	//
	while (!pThis->m_bThreadRecvExit)
	{
		pThis->ThreadRecvData();
		Sleep(200);
	}

	pThis->m_bThreadRecvExit = true;
	
	//
	TRACE(L"接收线程退出 \n");

	return 0;
}

void CIndexChsStkMideCore::ThreadRecvData()
{		
	// 从ViewData 中取数据处理:
	if ( NULL != m_pAbsCenterManager )
	{
		// 取请求数据
		CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&> aIndexChsStkRespData;
		aIndexChsStkRespData.RemoveAll();

		m_pAbsCenterManager->GetRespMerchKLineForIndexChsStk(aIndexChsStkRespData); 
		T_IndexChsStkRespData* pData = (T_IndexChsStkRespData*)aIndexChsStkRespData.GetData();

		//
		for ( int32 i = 0; i < aIndexChsStkRespData.GetSize(); i++ )
		{
			if ( m_bThreadRecvExit )
			{
				break;
			}
			
			T_IndexChsStkRespData stIndexChsStkRespData = pData[i];
			
			// 处理数据
			ProcessRespKLine(stIndexChsStkRespData); 

			TRACE(L"请求的个数 %d, 取到的数据个数: %d.\n", m_aRequestIDs.size(), m_aCalcParams.GetSize());

			// 看是否处理完了
			if ( m_aRequestIDs.size() == m_aCalcParams.GetSize() )
			{			
				TRACE(L"请求的个数 %d, 取到的数据个数: %d. 退出线程\n", m_aRequestIDs.size(), m_aCalcParams.GetSize());
				m_pAbsCenterManager->SetIndexChsStkMideCore(NULL);
				m_bThreadRecvExit = true;
				break;
			}
		}
	}
}

bool32 CIndexChsStkMideCore::BeginCalcThread()
{	
	m_bThreadCalcExit = false;
	m_hThreadCalc = CreateThread(NULL, 0, ThreadCalcIndexValue, this, 0, NULL);
	
	if ( NULL == m_hThreadCalc )
	{
		return false;
	}
	
	return true;
}

DWORD WINAPI CIndexChsStkMideCore::ThreadCalcIndexValue(LPVOID pParam)
{
	CIndexChsStkMideCore* pThis = (CIndexChsStkMideCore*)pParam;
	if ( NULL == pThis )
	{
		return 0;
	}

	//
	TRACE(L"计算线程开始 \n");		
	
	//
	while( !pThis->m_bThreadCalcExit )
	{
		pThis->ThreadCalcIndexValue();
		Sleep(50);
	}

	pThis->m_bThreadCalcExit = true;
	//
	TRACE(L"计算线程退出1 \n");
	
	return 0;
}

void CIndexChsStkMideCore::ThreadCalcIndexValue()
{
	// 上锁
	RGUARD(LockSingle, m_LockCalcParm, LockCalcParm);
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	
	int32 iSize = m_aCalcParams.GetSize();
	
	for ( int32 i = iSize - 1; i >= 0; i-- )
	{
		CTime TimeBegin = GetTickCount();

		if ( m_bThreadCalcExit )
		{
			TRACE(L"计算线程退出0 \n");
			return;
		}
		
		// K 线数据
		T_IndexChsStkCalcParam stIndexChsStkCalcParam = m_aCalcParams.GetAt(i);
		
		// 找到ID 数组中对应的ID		
		std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), stIndexChsStkCalcParam.m_iRespID);
		
		if ( itFind == m_aRequestIDs.end() )
		{
			// 不应该出现这种情况
			//ASSERT(0);
			continue;
		}
		
		// 看数据是否有效
		if ( stIndexChsStkCalcParam.m_aKLines.GetSize() <= 0 )
		{
			// 无效数据
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		if ( NULL == stIndexChsStkCalcParam.m_pMerch )
		{
			// 无效数据
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		// 前复权		
		CArray<CKLine, CKLine> aKLinesWeighted;
		if ( !CMerchKLineNode::WeightKLine(stIndexChsStkCalcParam.m_aKLines, stIndexChsStkCalcParam.m_pMerch->m_aWeightDatas, true, aKLinesWeighted) )
		{
			// 无效数据
			// m_aRequestIDs.erase(itFind);
			// m_aCalcParams.RemoveAt(i);
			// continue;
			
			aKLinesWeighted.Copy(stIndexChsStkCalcParam.m_aKLines);			
		}
	
		if ( aKLinesWeighted.GetSize() <= 0 )
		{
			// 无效数据
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}

		// 合并K 线
		CArray<CKLine, CKLine> aKLinesFinal;
		if ( !CombinKLine(stIndexChsStkCalcParam.m_pMerch, aKLinesWeighted, aKLinesFinal) )
		{
			//ASSERT(0);
			// 无效数据
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}

		// 计算系统指标, new 出来的,用完要删掉
		T_IndexOutArray* pIndexMa = CalcIndexEMAForSane(aKLinesFinal);
		
		if ( NULL == pIndexMa )
		{
			// 无效数据
			DEL(pIndexMa);
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		// 计算金盾指标
		bool32 bPassedUpdateTime = true;
		bool32 bNeedTime = false;
		CGmtTime TimeToUpdate;

		//
		CIoViewKLine::GetKLineUpdateTime(stIndexChsStkCalcParam.m_pMerch, m_pAbsCenterManager, aKLinesFinal.GetAt(aKLinesFinal.GetSize()-1).m_TimeCurrent, m_eTimeInterval, m_uiUserCycle, bNeedTime, TimeToUpdate, bPassedUpdateTime);
		
		//
		CArray<T_SaneIndexNode, T_SaneIndexNode> aSaneIndexNodes;
		if ( !CalcHistorySaneIndex(stIndexChsStkCalcParam.m_bFuture, bPassedUpdateTime, pIndexMa, aKLinesFinal, aSaneIndexNodes) )
		{
			// 无效数据
			DEL(pIndexMa);
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;				 
		}
		
		DEL(pIndexMa);
		
		CTime TimeEnd = GetTickCount();

		long iTimeSpace = TimeEnd.GetTime() - TimeBegin.GetTime();
		TRACE(L"计算一个商品耗时: %d ms \n", iTimeSpace);
		// 看是否满足条件

		if ( FiltResultWithConditions(aSaneIndexNodes) )
		{			
			// 计算准确率,收益率等值
			int32 iKLineCalcReal = 0;
			int32 iTradeTimes	 = 0;
			int32 iGoodTimes	 = 0;
			int32 iBadTimes		 = 0;
			float fAccuracyRate	 = 0.;
			float fProfitability = 0.;
			float fProfit		 = 0.;
			 
			float fPriceLastClose = aKLinesFinal.GetAt(aKLinesFinal.GetSize() - 1).m_fPriceClose;

			// 
			int32 iStatisticaKLineNums = KiStatisticaKLineNums;
			
			int32 iParamKLineNums = 0;

			for ( int32 iIndexCondition = 0; iIndexCondition < m_aConditions.size(); iIndexCondition++ )
			{
				if ( m_aConditions[iIndexCondition].m_bNeedCalc )
				{
					// 判断统计K 线的根数. 取最大值用于统计
					iParamKLineNums = iParamKLineNums >= m_aConditions[iIndexCondition].m_uiParam ? iParamKLineNums : m_aConditions[iIndexCondition].m_uiParam;
				}
			}

			if ( 0 != iParamKLineNums )
			{
				iStatisticaKLineNums = iParamKLineNums;
			}

			//
			if ( StatisticaSaneValues2(iStatisticaKLineNums, fPriceLastClose, iKLineCalcReal, aSaneIndexNodes, iTradeTimes, iGoodTimes, iBadTimes, fAccuracyRate, fProfitability, fProfit) )
			{
				// 保存结果
				T_IndexChsStkResult stIndexChsStkResult;
				stIndexChsStkResult.m_pMerch			= stIndexChsStkCalcParam.m_pMerch;
				stIndexChsStkResult.m_fAccuracyRate		= fAccuracyRate;
				stIndexChsStkResult.m_fProfitability	= fProfitability; 
				
				m_aChsStkResults.push_back(stIndexChsStkResult);						
			}
			else
			{
				// 无效数据			
				m_aRequestIDs.erase(itFind);
				m_aCalcParams.RemoveAt(i);
				continue;
			}			
		}
				
		// 保存文件, 保存的是原始K 线序列.不要保存复权后或者合并后的K 线数据
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if ( NULL != pApp )
		{
			CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
			
			if ( NULL != pDoc && pDoc->m_pAbsDataManager)
			{				
				E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
				E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;
				
				int32 iScale = 1;
				
				if ( GetTimeIntervalInfo(m_uiUserCycle, m_uiUserCycle, m_eTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale) )
				{
					pDoc->m_pAbsDataManager->WriteOfflineKLines(stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_iMarketId, stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_StrMerchCode, eKLineTypeCompare, EOKTIndexData, stIndexChsStkCalcParam.m_aKLines);		
				}				
			}
		}
	
		// 通知父窗口,改变进度条
		if ( NULL != m_pDlgShow )
		{
			CString* pStrMsg = new CString;			
			*pStrMsg = stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_StrMerchCnName;
			
			m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg), LPARAM(CDlgIndexChooseStock::EPTCalc));				
		}	
		
		// 算完了,清除这条记录					
		m_aCalcParams.RemoveAt(i);
		m_aRequestIDs.erase(itFind);
		
		//
		TRACE(L"计算商品 %s 成功, 还剩 %d 个 \n", stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aCalcParams.GetSize());			
	}
		
	// 全部算完了
	if ( m_aRequestIDs.empty() && m_bThreadReqExit )
	{
		TRACE(L"计算完成! \n");		
		if ( NULL != m_pDlgShow )
		{						
			// 通知父窗口计算完成
			m_pDlgShow->SetIndexChsStkResult(m_aChsStkResults);
			
			//
			CString* pStrMsg = new CString;			
			*pStrMsg = L"完成";
			m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg), LPARAM(CDlgIndexChooseStock::EPTFinish));
						
			m_bThreadCalcExit = true; 
		}	
		
		return;
	}		
}

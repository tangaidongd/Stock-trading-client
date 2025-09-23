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
	
	//m_pDlgShow,m_pAbsCenterManager�����ͷţ������ط����ã����ͷ� -cfj
	//lint --esym(1540, m_pDlgShow)
	//lint --esym(1540, m_pAbsCenterManager)
}

//////////////////////////////////////////////////////////////////////////
static bool32 GetMerchsByMarketName(IN CAbsCenterManager * pAbsCenterManager, IN CString StrMarketName, OUT CArray<CMerch*, CMerch*> &aMerchs)
{
    // ͨ��������Ƶõ��������������е���Ʒ
    if ( StrMarketName.GetLength() <= 0)
    {
        return false;
    }
    
    if (NULL == pAbsCenterManager)
    {
        return false;
    }
	
    aMerchs.RemoveAll();
    
    // �Ȳ�������	
    CMarket * pMarket = NULL;
    pAbsCenterManager->GetMerchManager().FindMarket(StrMarketName,pMarket);
    
    if ( NULL != pMarket)
    {
		aMerchs.Copy(pMarket->m_MerchsPtr);
		return true;
    }

    // �ٲ�ϵͳ���:    
	if ( CSysBlockManager::Instance()->GetMerchsInBlock(StrMarketName, aMerchs) )
	{
		return true;	
	}
	
    // �ٲ��û����:	
	if ( CUserBlockManager::Instance()->GetMerchsInBlock(StrMarketName, aMerchs) )
	{
		return true;
	}
    	
    return false;
}

void CIndexChsStkMideCore::SetChsStkParams(OUT int32& iMerchNums, IN E_OfflineDataGetType eOffLineDataGetType, IN const std::vector<CString>& aBlocks,  IN const std::vector<T_IndexChsStkCondition>& aConditions, IN E_NodeTimeInterval eTimeInterval, IN UINT uiUserCycle /* = 0 */)
{	
	// ���ò���:
	if ( NULL == m_pAbsCenterManager )
	{
		GetViewData();
	}

	iMerchNums = 0;
	
	// 1: �Ƿ�������ʷ����:
	m_eOffLineDataGetType = eOffLineDataGetType;

	// 2: ��Ʒ (���ݰ��õ���Ʒ)
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

	// ��չ�ע��Ʒ
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
			
			// ��ӹ�ע��Ʒ, ��Ҫ����ʱ�������
			CSmartAttendMerch SmartAttendMerch;
			SmartAttendMerch.m_pMerch = pMerch;
			SmartAttendMerch.m_iDataServiceTypes = EDSTKLine;
			
			m_pAbsCenterManager->AddAttendMerch(SmartAttendMerch,EA_Choose);			
		}
	}

	//
	iMerchNums = m_aMerchsToChoose.size();
	
	// 3: ����
	m_aConditions = aConditions;

	// 4: ����
	m_eTimeInterval = eTimeInterval;

	// 5: �Զ�������
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

	// ָ��ʱ�������	
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
		// ȫ������
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= eKLineTypeCompare;								// ʹ��ԭʼK������
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
			// �������
			//ASSERT(0);
			return false;
		}

		// ���� ID ��
		int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
		m_aRequestIDs.push_back(iReqID);

		TRACE(L"������==> �����: %d ��Ʒ��: %s ,�������� %d ��\n", iReqID, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aRequestIDs.size());
	}
	else
	{
		// ��������
		CGmtTime TimeNow	= m_pAbsCenterManager->GetServerTime();
	
		// 1: �õ��������ݵ�ʱ��
		CGmtTime TimeLocalStart;
		CGmtTime TimeLocalEnd;
		int32	 iKLineCountLocal;

		if ( !pDoc->m_pAbsDataManager->GetOfflineKLinesSnapshot(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineTypeCompare, EOKTIndexData, TimeLocalStart, TimeLocalEnd, iKLineCountLocal) )
		{
			// ��ȡ��������ʧ��, ȫ������			
			return(RequestData(pMerch, true));
		}

		if ( TimeLocalEnd > TimeNow )
		{
			// �������ݵ�ʱ������д���	,ȫ������				
			return(RequestData(pMerch, true));
		}

		// ��������
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= eKLineTypeCompare;								// ʹ��ԭʼK������
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
			// �������
			//ASSERT(0);		
			return false;
		}
			
		// ���� ID ��
		int32 iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
		m_aRequestIDs.push_back(iReqID);

		//
		TRACE(L"������==> �����: %d ��Ʒ��: %s ,�������� %d ��\n", iReqID, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aRequestIDs.size());
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
		TRACE(L"�����Ȩ���� %d \n", i);

		if ( m_bThreadReqExit )
		{
			TRACE(L"�����߳��˳�0 \n");			
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
			
			// ֪ͨ������,�ı������
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

	TRACE(L"������<== �����: %d \n", iRespID);
	
	// ��û����� ID	 
	std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), iRespID);
	
	if ( itFind == m_aRequestIDs.end() )
	{		
		return false;
	}

	// ��������Ϊ��
	if ( stIndexChsStkRespData.m_aKLines.GetSize() <= 0 || stIndexChsStkRespData.m_aKLines.GetSize() >= 100000 )
	{		
		// 
		CString StrMerchCode = stIndexChsStkRespData.m_StrMerchCode;
		TRACE(L" �յ���Ʒ %d %s ���ݴ���: K �߸���Ϊ: %d \n", stIndexChsStkRespData.m_iMarketId, StrMerchCode.GetBuffer(), stIndexChsStkRespData.m_aKLines.GetSize());
		m_aRequestIDs.erase(itFind);			
		return false;
	}

	// �ĵ���ָ��
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
	
	// ��û�������Ʒ
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
		// ���̴߳���Ĳ���
		T_IndexChsStkCalcParam stIndexChsStkCalcParam;
		
		stIndexChsStkCalcParam.m_iRespID = iRespID;		
		stIndexChsStkCalcParam.m_bFuture = bFuture;
		stIndexChsStkCalcParam.m_pMerch	 = pMerch;
		stIndexChsStkCalcParam.m_aKLines.Copy(stIndexChsStkRespData.m_aKLines);

		m_aCalcParams.Add(stIndexChsStkCalcParam);
	}
	else if ( EODGTAdd == m_eOffLineDataGetType )
	{
		// ���汾��,������ƴ������,�ٰ����ݶ�����,���ڼ���		
		pDoc->m_pAbsDataManager->WriteOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stIndexChsStkRespData.m_eKLineTypeBase, EOKTIndexData, stIndexChsStkRespData.m_aKLines);
		
		CArray<CKLine, CKLine> aKLines;
		pDoc->m_pAbsDataManager->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stIndexChsStkRespData.m_eKLineTypeBase, EOKTIndexData, aKLines);
		if ( aKLines.GetSize() <= 0 )
		{
			//ASSERT(0);
			m_aRequestIDs.erase(itFind);
			return false;
		}

		// �����̴߳���
		if ( aKLines.GetSize() <= 0 )
		{
			//ASSERT(0);
			m_aRequestIDs.erase(itFind);	
			return false;
		}
		
		// ���̴߳���Ĳ���
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
	// ���������
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
			// ���ڲ�ͬ, ֱ�ӷ���			
			return true;				
		}
		else
		{
			// ������ͬ, 
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
		// �������������������		
		BeginReqThread();	
		BeginRecvThread();
	}
	else if ( EODGTNone == m_eOffLineDataGetType )
	{
		// ֱ�ӱ���ȡ������:
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
			// ȡ��Ʒ
			CMerch* pMerch = m_aMerchsToChoose[i];
			
			if ( NULL == pMerch )
			{
				continue;
			}
			
			bool32 bFuture = CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
			
			// �õ���������
			E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
			E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;
			
			int32 iScale = 1;
			
			if ( !GetTimeIntervalInfo(m_uiUserCycle, m_uiUserCycle, m_eTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale) )
			{
				//ASSERT(0);	
				continue;
			}
			
			// ȡ k ������
			CArray<CKLine, CKLine> aKLines;
			pDoc->m_pAbsDataManager->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineTypeCompare, EOKTIndexData, aKLines);
			if ( aKLines.GetSize() <= 0 || aKLines.GetSize() >= 100000 )
			{		
				// 
				TRACE(L"��Ʒ %s %d ���ݴ���: K �߸���Ϊ: %d \n", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), aKLines.GetSize());
				continue;
			}

			// ȡ��Ȩ����
			CArray<CWeightData,CWeightData&> aWeightData;
			UINT uiCrc32 = 0;
			
			if ( CIoViewBase::ReadWeightDataFromLocalFile(pMerch, aWeightData, uiCrc32) )
			{				
				pMerch->m_bHaveReadLocalWeightFile = true;
				
				// ����CMerch ������ֶε�ֵ				
				pMerch->m_uiWeightDataCRC = uiCrc32;
				
				int32 iSize = aWeightData.GetSize();
				pMerch->m_aWeightDatas.SetSize(iSize);				
				memcpyex(pMerch->m_aWeightDatas.GetData(), aWeightData.GetData(), sizeof(CWeightData) * iSize);					
			}
			
			// �����̴߳���			
			// ���̴߳���Ĳ���
			// ��������,������Ĵ�С�� ID ������
			
			int32 iRespID = m_aCalcParams.GetSize();
			m_aRequestIDs.push_back(iRespID);
			
			T_IndexChsStkCalcParam stIndexChsStkCalcParam;
						
			stIndexChsStkCalcParam.m_iRespID = iRespID;		
			stIndexChsStkCalcParam.m_bFuture = bFuture;
			stIndexChsStkCalcParam.m_pMerch	 = pMerch;
			stIndexChsStkCalcParam.m_aKLines.Copy(aKLines);
			
			m_aCalcParams.Add(stIndexChsStkCalcParam);	
		}

		// ��ʼ����
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
		// ����:
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

	// �����߳�
	if ( NULL != m_hThreadReq )	
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadReq, INFINITE) )
		{
			// �˳���
			bThreadReqExit = true;
		}	
	}
	else
	{
		bThreadReqExit = true;
	}

	// �����߳�
	if ( NULL != m_hThreadRecv )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadRecv, INFINITE) )
		{
			// �˳���
			bThreadRecvExit = true;
		}
	}
	else
	{
		bThreadRecvExit = true;
	}

	// �����߳�
	if ( NULL != m_hThreadCalc )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadCalc, INFINITE) )
		{
			// �˳���
			bThreadCalcExit = true;
		}
	}
	else
	{
		bThreadCalcExit = true;
	}

	// ���˳���
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
		// ����û�д��ڵ�ʱ�򻹵�����ѡ�ɳ�ʱ����ʾ��
		if ( NULL != m_pDlgShow && NULL != m_pDlgShow->GetSafeHwnd() )
		{			
			::MessageBox(m_pDlgShow->GetSafeHwnd(), L"�������س�ʱ...", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		}		
	}
}

void CIndexChsStkMideCore::OnChooseStockFinish()
{
	// ѡ�����,�����������:
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
	// ֪ͨ������, ���½�����
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
			// �����,�ص���ʱ��
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
		// �������ڷ������뿪���ź�
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

		// �� ?? �췢�����뿪���ź�
		if ( ESISDTOC == Node.m_eSaneIndexState || ESISKCDO == Node.m_eSaneIndexState )
		{
			return true;
		}
	}
	else if ( 1 == iCondition )
	{
		/*
		// �������ڷ������������ź�
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

		// �� ?? �췢�����������ź�
		if ( ESISKTOC == Node.m_eSaneIndexState || ESISDCKO == Node.m_eSaneIndexState )			  
		{
			return true;
		}
	}
	else if ( 2 == iCondition )
	{
		/*
		// �������ڷ�������ź�
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

		// �������ڷ�������ź�
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
	// �����������˽��:
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
// �߳����
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

	TRACE(L"�����߳̿�ʼ \n");		
		
	//
	if ( !pThis->m_bThreadReqExit )
	{
		pThis->ThreadRequestData();
	}
	
	TRACE(L"�����߳��˳�3 \n");

	
	// ��ʼ�����߳�
 	 if ( !pThis->m_bThreadReqExit )
 	 {
 	 	pThis->BeginCalcThread();
 	 }

	pThis->m_bThreadReqExit = true;

	return 0;
}

bool32 CIndexChsStkMideCore::ThreadRequestData()
{
 	// �Ȱѳ�Ȩ������������:

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

	// ��ʼ���鳬ʱ�ļ�ʱ��
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
		// ȫ����������:
		for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
		{
			if ( m_bThreadReqExit )
			{		
				TRACE(L"�����߳��˳�1 \n");
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
		// ��������:	
		for ( int32 i = 0; i < m_aMerchsToChoose.size(); i++ )
		{
			if ( m_bThreadReqExit )
			{
				TRACE(L"�����߳��˳�2 \n");	 
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
	TRACE(L"�����߳̿�ʼ \n");
	
	//
	while (!pThis->m_bThreadRecvExit)
	{
		pThis->ThreadRecvData();
		Sleep(200);
	}

	pThis->m_bThreadRecvExit = true;
	
	//
	TRACE(L"�����߳��˳� \n");

	return 0;
}

void CIndexChsStkMideCore::ThreadRecvData()
{		
	// ��ViewData ��ȡ���ݴ���:
	if ( NULL != m_pAbsCenterManager )
	{
		// ȡ��������
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
			
			// ��������
			ProcessRespKLine(stIndexChsStkRespData); 

			TRACE(L"����ĸ��� %d, ȡ�������ݸ���: %d.\n", m_aRequestIDs.size(), m_aCalcParams.GetSize());

			// ���Ƿ�������
			if ( m_aRequestIDs.size() == m_aCalcParams.GetSize() )
			{			
				TRACE(L"����ĸ��� %d, ȡ�������ݸ���: %d. �˳��߳�\n", m_aRequestIDs.size(), m_aCalcParams.GetSize());
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
	TRACE(L"�����߳̿�ʼ \n");		
	
	//
	while( !pThis->m_bThreadCalcExit )
	{
		pThis->ThreadCalcIndexValue();
		Sleep(50);
	}

	pThis->m_bThreadCalcExit = true;
	//
	TRACE(L"�����߳��˳�1 \n");
	
	return 0;
}

void CIndexChsStkMideCore::ThreadCalcIndexValue()
{
	// ����
	RGUARD(LockSingle, m_LockCalcParm, LockCalcParm);
	RGUARD(LockSingle, m_LockRequestIDs, LockRequestIDs);
	
	int32 iSize = m_aCalcParams.GetSize();
	
	for ( int32 i = iSize - 1; i >= 0; i-- )
	{
		CTime TimeBegin = GetTickCount();

		if ( m_bThreadCalcExit )
		{
			TRACE(L"�����߳��˳�0 \n");
			return;
		}
		
		// K ������
		T_IndexChsStkCalcParam stIndexChsStkCalcParam = m_aCalcParams.GetAt(i);
		
		// �ҵ�ID �����ж�Ӧ��ID		
		std::list<int32>::iterator itFind = std::find(m_aRequestIDs.begin(), m_aRequestIDs.end(), stIndexChsStkCalcParam.m_iRespID);
		
		if ( itFind == m_aRequestIDs.end() )
		{
			// ��Ӧ�ó����������
			//ASSERT(0);
			continue;
		}
		
		// �������Ƿ���Ч
		if ( stIndexChsStkCalcParam.m_aKLines.GetSize() <= 0 )
		{
			// ��Ч����
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		if ( NULL == stIndexChsStkCalcParam.m_pMerch )
		{
			// ��Ч����
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		// ǰ��Ȩ		
		CArray<CKLine, CKLine> aKLinesWeighted;
		if ( !CMerchKLineNode::WeightKLine(stIndexChsStkCalcParam.m_aKLines, stIndexChsStkCalcParam.m_pMerch->m_aWeightDatas, true, aKLinesWeighted) )
		{
			// ��Ч����
			// m_aRequestIDs.erase(itFind);
			// m_aCalcParams.RemoveAt(i);
			// continue;
			
			aKLinesWeighted.Copy(stIndexChsStkCalcParam.m_aKLines);			
		}
	
		if ( aKLinesWeighted.GetSize() <= 0 )
		{
			// ��Ч����
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}

		// �ϲ�K ��
		CArray<CKLine, CKLine> aKLinesFinal;
		if ( !CombinKLine(stIndexChsStkCalcParam.m_pMerch, aKLinesWeighted, aKLinesFinal) )
		{
			//ASSERT(0);
			// ��Ч����
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}

		// ����ϵͳָ��, new ������,����Ҫɾ��
		T_IndexOutArray* pIndexMa = CalcIndexEMAForSane(aKLinesFinal);
		
		if ( NULL == pIndexMa )
		{
			// ��Ч����
			DEL(pIndexMa);
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;
		}
		
		// ������ָ��
		bool32 bPassedUpdateTime = true;
		bool32 bNeedTime = false;
		CGmtTime TimeToUpdate;

		//
		CIoViewKLine::GetKLineUpdateTime(stIndexChsStkCalcParam.m_pMerch, m_pAbsCenterManager, aKLinesFinal.GetAt(aKLinesFinal.GetSize()-1).m_TimeCurrent, m_eTimeInterval, m_uiUserCycle, bNeedTime, TimeToUpdate, bPassedUpdateTime);
		
		//
		CArray<T_SaneIndexNode, T_SaneIndexNode> aSaneIndexNodes;
		if ( !CalcHistorySaneIndex(stIndexChsStkCalcParam.m_bFuture, bPassedUpdateTime, pIndexMa, aKLinesFinal, aSaneIndexNodes) )
		{
			// ��Ч����
			DEL(pIndexMa);
			m_aRequestIDs.erase(itFind);
			m_aCalcParams.RemoveAt(i);
			continue;				 
		}
		
		DEL(pIndexMa);
		
		CTime TimeEnd = GetTickCount();

		long iTimeSpace = TimeEnd.GetTime() - TimeBegin.GetTime();
		TRACE(L"����һ����Ʒ��ʱ: %d ms \n", iTimeSpace);
		// ���Ƿ���������

		if ( FiltResultWithConditions(aSaneIndexNodes) )
		{			
			// ����׼ȷ��,�����ʵ�ֵ
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
					// �ж�ͳ��K �ߵĸ���. ȡ���ֵ����ͳ��
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
				// ������
				T_IndexChsStkResult stIndexChsStkResult;
				stIndexChsStkResult.m_pMerch			= stIndexChsStkCalcParam.m_pMerch;
				stIndexChsStkResult.m_fAccuracyRate		= fAccuracyRate;
				stIndexChsStkResult.m_fProfitability	= fProfitability; 
				
				m_aChsStkResults.push_back(stIndexChsStkResult);						
			}
			else
			{
				// ��Ч����			
				m_aRequestIDs.erase(itFind);
				m_aCalcParams.RemoveAt(i);
				continue;
			}			
		}
				
		// �����ļ�, �������ԭʼK ������.��Ҫ���渴Ȩ����ߺϲ����K ������
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
	
		// ֪ͨ������,�ı������
		if ( NULL != m_pDlgShow )
		{
			CString* pStrMsg = new CString;			
			*pStrMsg = stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_StrMerchCnName;
			
			m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg), LPARAM(CDlgIndexChooseStock::EPTCalc));				
		}	
		
		// ������,���������¼					
		m_aCalcParams.RemoveAt(i);
		m_aRequestIDs.erase(itFind);
		
		//
		TRACE(L"������Ʒ %s �ɹ�, ��ʣ %d �� \n", stIndexChsStkCalcParam.m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aCalcParams.GetSize());			
	}
		
	// ȫ��������
	if ( m_aRequestIDs.empty() && m_bThreadReqExit )
	{
		TRACE(L"�������! \n");		
		if ( NULL != m_pDlgShow )
		{						
			// ֪ͨ�����ڼ������
			m_pDlgShow->SetIndexChsStkResult(m_aChsStkResults);
			
			//
			CString* pStrMsg = new CString;			
			*pStrMsg = L"���";
			m_pDlgShow->PostMessage(UM_Index_Choose_Stock_Progress, WPARAM(pStrMsg), LPARAM(CDlgIndexChooseStock::EPTFinish));
						
			m_bThreadCalcExit = true; 
		}	
		
		return;
	}		
}

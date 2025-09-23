#include "stdafx.h"
#include "ShareFun.h"
#include "memdc.h"
#include "io.h"
#include "PathFactory.h"
#include "MerchManager.h"
#include "UserBlockManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "GGTongView.h"
#include "IoViewManager.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "IoViewTimeSale.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "DlgBlockSelect.h"
#include "DlgBlockSet.h"
#include "IoViewReportSelect.h"
#include "LogFunctionTime.h"
#include "PluginFuncRight.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_VALUE  (-1)

const int32 KTimerIdRequestChooseStock		=	7842;
const int32 KTimerPeriodRequestChooseStock	= 60*2*1000;

//xml
static	const char KStrXmlKey[]	= "ReportSelectStock";
static	const char KStrXmlAttriReqIndexSelectStockFlagName[] = "IndexSelectFlag";
static	const char KStrXmlAttriReqHotSelectFlagName[]		 = "HotSelectFlag";
static	const char KStrXmlAttriCurrentSelectTypeName[]		 = "CurSelType";

IMPLEMENT_DYNCREATE(CIoViewReportSelect, CIoViewReport)		

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewReportSelect, CIoViewReport)
//{{AFX_MSG_MAP(CIoViewReportSelect)
ON_WM_CREATE()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

CIoViewReportSelect::CIoViewReportSelect()
:CIoViewReport()
{
	m_bIsStockSelectedReport = true;
	m_pMmiCurrentReq = &m_mmiIndexSelectStock;

	m_bIoViewActiveChanging = false;

	m_bPromptedByLackOfRight = false;

	m_mmiSelectStockReq.m_uTypes = ECSTShort;	// 短线
	m_mmiPushSelectStockReq.m_uType = ECSTShort;
	
	m_dwLastIndexSelectStockFlag    = INVALID_VALUE;

	m_eReportType = ERST_COMPLETE_RESULT;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewReportSelect::~CIoViewReportSelect()
{
	if ( NULL != m_pSubject )
	{
		m_pSubject->DelObserver(this);
	}

	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
}

int CIoViewReportSelect::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iRet = CIoViewReport::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return iRet;
	}

	// 额外添加一个条件选股

	OpenSelBlock(T_BlockDesc::EBTSelect);

	SetTimer(KTimerIdRequestChooseStock, KTimerPeriodRequestChooseStock, NULL);
	
	return iRet;
}

void CIoViewReportSelect::PostNcDestroy()
{
	//delete this; // 由于归IoViewManager删除，这里就不做处理了
}

void CIoViewReportSelect::RequestViewData()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	
	// 由于现在仅仅在当前为选股时发送请求，所以如果当期不为select，然后切换到select时，需要发出请求 以获取最新的结果 TODO
	if ( TabIsValid(iTab, eType) && ( eType == T_BlockDesc::EBTSelect || T_BlockDesc::EBTMerchSort == eType) )
	{
		// 根据现在的标志，申请不同的市场做显示调试数据, 具体各种请求标志对应市场中不同的排序表头
		// 短线 中线 买入精选 请求
		// 战略选股请求
		// 大盘选股请求
		// 热门强势请求	

		typedef	map<CStockSelectManager::E_IndexSelectStock, E_ChooseStockType> ReqTypeMap;

		ReqTypeMap mapReqType;
		mapReqType[ CStockSelectManager::ShortLine ] = ECSTShort;
		mapReqType[ CStockSelectManager::MidLine ] = ECSTMid;
		mapReqType[ CStockSelectManager::BuySelect ] = ECSTMrjx;
		mapReqType[ CStockSelectManager::StrategyJGLT ] = ECSTJglt;
		mapReqType[ CStockSelectManager::StrategyXPJS ] = ECSTXpjs;
		mapReqType[ CStockSelectManager::StrategyYJZT ] = ECSTZjzt;
		mapReqType[ CStockSelectManager::StrategyNSQL ] = ECSTNsql;
		mapReqType[ CStockSelectManager::StrategyCDFT ] = ECSTCdft;
		mapReqType[ CStockSelectManager::StrategySLXS ] = ECSTSlxs;
		mapReqType[ CStockSelectManager::StrategyHFFL ] = ECSTHffl;
		mapReqType[ CStockSelectManager::StrategySHJD ] = ECSTShjd;
		mapReqType[ CStockSelectManager::IndexStrategy ] = ECSTDpcl;		// 大盘策略只发一种就可以了，服务器决定发什么过来
		mapReqType[ CStockSelectManager::RedStrategy ] = ECSTDpcl;			
		mapReqType[ CStockSelectManager::GreenStrategy ] = ECSTDpcl;
		mapReqType[ CStockSelectManager::BlueStrategy ] = ECSTDpcl;
		mapReqType[ CStockSelectManager::YellowStrategy ] = ECSTDpcl;
		mapReqType[ CStockSelectManager::FallStrategy ] = ECSTDpcl;

		mapReqType[ CStockSelectManager::StrategyKTXJ ] = ECSTKtxj;
		mapReqType[ CStockSelectManager::StrategyZDSD ] = ECSTZdsd;
		mapReqType[ CStockSelectManager::StrategyDXQS ] = ECSTDxqs;
		mapReqType[ CStockSelectManager::StrategyQZHQ ] = ECSTQzhq;
		mapReqType[ CStockSelectManager::StrategyBDCZ ] = ECSTBdcz;



		// 由于需要取∩关系，所有选择flag中都有的才能算选中
		//	所以设定n种接收类型的数组，保存选股返回结果，每次发送请求时清空所有数据，回来时保存，等待最后一个请求的数据返回时，运算∩
		//	将结果赋予显示结果集

		m_aIndexSelectFlag.RemoveAll();		// 清空所有结果集
		ClearResultMap();
		m_mapMerchsHot.clear();
		m_aHotSelect.RemoveAll();

		CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();

		if ( eCurSel == CStockSelectManager::IndexShortMidSelect )	// 短 中 买 策略 大盘
		{
			DWORD dwIndexSelectFlag = CStockSelectManager::Instance().GetIndexSelectStockFlag();
			DWORD dwReqFlag = 0;
			m_mmiSelectStockReq.m_uTypes = m_mmiPushSelectStockReq.m_uType = 0;
			for ( ReqTypeMap::const_iterator it = mapReqType.begin(); it != mapReqType.end() ; it++ )
			{
				if ( (dwIndexSelectFlag & it->first) == it->first )
				{
					m_mmiSelectStockReq.m_uTypes |= it->second;
					m_mmiPushSelectStockReq.m_uType |= it->second;
					dwReqFlag |= it->second;
				}
			}
			m_aIndexSelectFlag.Add(dwReqFlag);
			RequestViewData(m_mmiSelectStockReq);		// 统一一次请求的数据
			RequestViewData(m_mmiPushSelectStockReq);
		}
		else if ( eCurSel == CStockSelectManager::HotSelect )	// 大单选股 - 按照需求，客户端进行筛选
		{
			DWORD dwHotSelectFlag = CStockSelectManager::Instance().GetHotSelectStockFlag();
			if ( (dwHotSelectFlag & CStockSelectManager::HotTimeSale) != 0 )
			{
				m_aHotSelect.Add(ERSPIExBigChange);
			}
			if ( dwHotSelectFlag & CStockSelectManager::HotForce )
			{
				m_aHotSelect.Add(ERSPIBigChange);
			}
			if ( dwHotSelectFlag & CStockSelectManager::HotCapital )
			{
				m_aHotSelect.Add(ERSPIBigBuyChange);
			}

			for ( int i=0; i < m_aHotSelect.GetSize() ; i++ )
			{
				CMmiReqMerchSort req;
				req.m_iMarketId = -1;	// 沪深A
				req.m_iStart = 0;
				req.m_iCount = 15; //4; // 多请求一些数据，筛选涨的商品
				req.m_eReportSortType = m_aHotSelect[i];
				req.m_bDescSort = TRUE;

				RequestViewData(req);
			}
		}
		else
		{
			return;
		}
	}
}

void CIoViewReportSelect::RequestViewData( CMmiCommBase &req )
{
	int eRightDeny;
	if ( CStockSelectManager::Instance().IsCurrentReqUnderUserRight(&eRightDeny) )	// 有权限才发
	{
		DoRequestViewData(req);
	}
	else
	{
		PromptLackRight(eRightDeny);
	}
}

void CIoViewReportSelect::OnVDataReportUpdate( int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs )
{
	CIoViewReport::OnVDataReportUpdate(iMarketId, eMerchReportField, bDescSort, iPosStart, iOrgMerchCount, aMerchs);
}

void CIoViewReportSelect::OnVDataForceUpdate()
{
	CIoViewReport::OnVDataForceUpdate();
	
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if (!pFrame)
	{
		return;
	}
	
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	
	DWORD dwIndexSelectFlag = CStockSelectManager::Instance().GetIndexSelectStockFlag();

	if ( TabIsValid(iTab, eType) && m_dwLastIndexSelectStockFlag != dwIndexSelectFlag )
	{
		if ( ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType) && !m_bIoViewActiveChanging )
		{
			if (pFrame->IsEnableRequestData())
			{
				m_dwLastIndexSelectStockFlag = dwIndexSelectFlag;
			}
			
			RequestViewData();
			//RequestViewDataCurrentVisibleRow();
		}
	}
}

void CIoViewReportSelect::OnIoViewActive()
{
	m_bIoViewActiveChanging = true;
	CIoViewReport::OnIoViewActive();
	//if ( m_iCurTab == 0 )
	//{
	//	// 不需要请求排行，只请求实时报价就可以了
	//	// RequestViewData();
	//	// RequestViewDataCurrentVisibleRow();
	//}
	m_bIoViewActiveChanging = false;
}

void CIoViewReportSelect::OnFixedRowClickCB( CCellID& cell )
{
	//if ( cell.IsValid() && m_GridCtrl.IsValid(cell) )
	//{
	//	int32 iTab;
	//	T_BlockDesc::E_BlockType eType;
	//	
	//	if ( TabIsValid(iTab, eType) )
	//	{
	//		if ( eType == T_BlockDesc::EBTSelect )
	//		{
	//			// 短 中 不处理, 资金 由新协议处理
	//			CString StrHeadName = m_ReportHeadInfoList.GetAt(cell.col).m_StrHeadNameCn;
	//			
	//			if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHShortLineSelect)
	//				|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMidLineSelect)
	//				/*|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHCapitalFlow )*/)
	//			{
	//				//return;	// 现在可以排序
	//			}
	//		}
	//	}
	//}

	CIoViewReport::OnFixedRowClickCB(cell);
}

void CIoViewReportSelect::OnChooseStockData( const CMmiRespCRTEStategyChooseStock *pResp )
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();
	
	if ( !TabIsValid(iTab, eType) || eType != T_BlockDesc::EBTSelect )
	{
		return;	// 非选股或当前不需要运算集合了	- 由于过滤掉了，所以在切换回来时，商品就不是最新的了
	}
	
	// 修复选股数据显示不全问题
	if(pResp->m_aMerchs.GetSize() > 0)
	{
		ChangeGridSize();
	}

	if ( NULL != pResp && IsAttendResp(pResp->m_uTypes) )
	{
		CArray<CMerch *, CMerch *>	aMerchs;
		for ( int i=0; i < pResp->m_aMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = NULL;
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(pResp->m_aMerchs[i].m_StrMerchCode, pResp->m_aMerchs[i].m_iMarketId, pMerch))
			{
				pMerch->m_uiSelStockTime  = pResp->m_aMerchs.GetAt(i).m_uSelectTime;
				pMerch->m_fSelPrice       = pResp->m_aMerchs.GetAt(i).m_fSelPrice;
				aMerchs.Add(pMerch);
			}
		}

		AddMerchsToResultMap(pResp->m_uTypes, aMerchs);		// 添加到集合 - 如果满足运算条件，则会计算运算结果，并添加到显示集合
	}
}

void CIoViewReportSelect::OnTimer( UINT nIDEvent )
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( KTimerIdRequestChooseStock == nIDEvent )
	{
		int32 iTab;
		T_BlockDesc::E_BlockType eType;
		if ( TabIsValid(iTab, eType) && (T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType) )	// 选股需要发选取请求
		{
			// 注意这个请求应该是比较吃网速的，而且也没做收盘优化(!!服务器有可能收盘后重启而在收盘时产生新数据)
			RequestViewData();
		}
	}
	
	CIoViewReport::OnTimer(nIDEvent);
}

void CIoViewReportSelect::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	CIoViewReport::OnVDataPluginResp(pResp);

	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespMerchSort )
	{
		OnMerchSortResp((const CMmiRespMerchSort *)pRespPlugin);
	}
}

void CIoViewReportSelect::OnMerchSortResp( const CMmiRespMerchSort *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	// 仅计算数据，不显示
// 	int32 iTab;
// 	T_BlockDesc::E_BlockType eType;
// 	

// 	if ( !TabIsValid(iTab, eType) || eType != T_BlockDesc::EBTSelect || m_eCurrentSelect != HotSelect )
// 	{
// 		return;	// 非选股或当前不需要运算集合了	
// 	}

	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();
	if ( eCurSel != CStockSelectManager::HotSelect )
	{
		return;	// 非热门xx，不处理
	}
	
	CArray<CMerch *, CMerch *>	aMerchsMy;
	for ( int i=0; i < m_aHotSelect.GetSize() ; i++ )
	{
		E_ReportSortEx eRSE = m_aHotSelect[i];
		if ( (pResp->m_iMarketId == -1 /*|| pResp->m_iMarketId == 65535*/)
			&& pResp->m_eReportSortType == eRSE
			&& pResp->m_iStart == 0
			&& pResp->m_aMerchs.GetSize() == 15  //4
			&& pResp->m_bDescSort == TRUE
			)
		{
			ConvertMerchKeysToMerchs(pResp->m_aMerchs, aMerchsMy, true);

			// 哪些是涨的？
			// 需要建立一个临时队列，请求实时报价，然后在筛选涨股，这样由于实时不够实时的原因
			// 可能涨股会出现越来越少的问题
			m_mapMerchsHot[GetHotSelectTempArrayIndex(eRSE)].Copy(aMerchsMy);	// 临时队列
			
			// 修复选股数据显示不全问题
			if (pResp->m_aMerchs.GetSize() > 0)
			{
				ChangeGridSize();
			}

			AddMerchArrayToSmartAttend(aMerchsMy, true);		// 发送临时队列数据请求

			break;
		}
	}
}

bool32 CIoViewReportSelect::IsAttendResp( DWORD dwTypes )
{
	for (int i=0; i < m_aIndexSelectFlag.GetSize() ; i++)
	{
		if ( dwTypes == m_aIndexSelectFlag[i] )
		{
			return true;
		}
	}
	return false;
}

void CIoViewReportSelect::ClearResultMap()
{
	m_mapSelectResult.clear();
}

bool32 CIoViewReportSelect::AddMerchsToResultMap( u32 uTypes, const MerchArray &aMerchs )
{
	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();
	ASSERT( IsAttendResp(uTypes) );
	ASSERT( eCurSel == CStockSelectManager::IndexShortMidSelect );

	m_mapSelectResult[uTypes].Copy(aMerchs);

	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	
	if ( !TabIsValid(iTab, eType) || ( eType != T_BlockDesc::EBTSelect && eType != T_BlockDesc::EBTMerchSort) )
	{
		return true;	// 非选股不需要运算集合了	
	}

	bool32 bMyDataOk = true;
	int i;
	for ( i=0; i < m_aIndexSelectFlag.GetSize(); i++ )
	{
		if ( m_mapSelectResult.count(m_aIndexSelectFlag[i]) <= 0 )
		{
			bMyDataOk = false;
			break;
		}
	}

	if ( bMyDataOk )
	{
		// 运算结果集
		MerchCountMap merchsCount;
		MerchArray	  aMerchsFirst;
		for ( SelectResultMap::const_iterator it=m_mapSelectResult.begin(); it != m_mapSelectResult.end(); it++ )
		{
			if ( !IsAttendResp((E_ChooseStockType)it->first) )
			{
				ASSERT( 0 ); // 应当在req时，全部清除了的
				continue;
			}
			for ( int i=0; i < it->second.GetSize() ; i++ )
			{
				MerchCountMap::iterator itCount = merchsCount.find(it->second[i]);
				if ( itCount != merchsCount.end() )
				{
					itCount->second++;
				}
				else
				{
					merchsCount[ it->second[i] ] = 1;
				}
			}

			if ( it == m_mapSelectResult.begin() )
			{
				aMerchsFirst.Copy( it->second );
			}
		}

		// 商品出现次数等于attend的数目
		MerchArray aMerchsMy;
		const DWORD dwCount = m_aIndexSelectFlag.GetSize();
// 		for ( MerchCountMap::const_iterator itCount=merchsCount.begin(); itCount != merchsCount.end() ; itCount++ )
// 		{
// 			if ( itCount->second == dwCount )
// 			{
// 				aMerchsMy.Add(itCount->first);
// 			}
// 		}
		// 序列插入
		for ( i=0; i < aMerchsFirst.GetSize() ; i++ )
		{
			MerchCountMap::const_iterator itCount = merchsCount.find(aMerchsFirst[i]);
			if ( itCount != merchsCount.end() && itCount->second == dwCount )
			{
				aMerchsMy.Add( itCount->first );
			}
		}

		// 设置商品数据
		m_aTabInfomations[iTab].m_Block.m_aMerchs.Copy(aMerchsMy);	// 设置商品列表源
		if ( m_aTabInfomations[iTab].m_iDataIndexBegin < 0 || m_aTabInfomations[iTab].m_iDataIndexEnd >= aMerchsMy.GetSize() )	// 重置开始-结束数据
		{
			m_aTabInfomations[iTab].m_iDataIndexBegin = 0;
			m_aTabInfomations[iTab].m_iDataIndexEnd = aMerchsMy.GetSize() - 1;
		}
		SetTabParams(iTab);		// 现在有数据了，重设参数 - 如果有排序该怎么处理？？
		bool32 bSort = m_bRequestViewSort;
		//ReCreateTabWnd();
		//TabChange();
		CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
		ReSetGridCellDataByMerchs(aMerchsMy);
// 		ReSetGridContent();
		UpdateTableContent(TRUE, NULL, false);
		m_GridCtrl.SetSelectedRange(rangeSel);

		
		// 			
		// 			ReSetGridCellDataByMerchs(aMerchs);
		// 			UpdateTableContent(TRUE, NULL, false);
		//
		m_bRequestViewSort = bSort;
		if ( !RequestViewDataSort() )
		{
			RequestViewDataCurrentVisibleRow();	
		}
	}

	return true;
}

void CIoViewReportSelect::AddMerchArrayToSmartAttend( const MerchArray &aMerchs, bool32 bRequestNow/* = false*/, bool32 bNeedF10/*=false*/, bool32 bNeedChooseStockData/* = false*/ )
{
	// 查看smartmerch中有该商品没
	CArray<CSmartAttendMerch, CSmartAttendMerch &> aSmartMerchs;
	int32 i = 0;
	typedef map<CMerch *, int32>	MerchIntMap;
	MerchIntMap merchExistMap;
	for ( i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( aMerchs[i] == NULL )
		{
			continue;
		}
		merchExistMap[ aMerchs[i] ] = -1;
	}
	for ( i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
	{
		MerchIntMap::iterator it = merchExistMap.find(m_aSmartAttendMerchs[i].m_pMerch);
		if ( it != merchExistMap.end() )
		{
			merchExistMap[ m_aSmartAttendMerchs[i].m_pMerch ] = i;	// 保存其在smart中索引
		}
	}

	UINT uDataSevice = EDSTPrice;
	if ( bNeedF10 )
	{
		uDataSevice |= EDSTGeneral;
	}
	for ( MerchIntMap::iterator it = merchExistMap.begin() ; it != merchExistMap.end() ; it++ )
	{
		if ( it->second == -1 )
		{
			CSmartAttendMerch Smart;
			Smart.m_pMerch = it->first;
			Smart.m_iDataServiceTypes = uDataSevice;
			it->second = m_aSmartAttendMerchs.Add(Smart);
		}
	}

	if ( bRequestNow )
	{
		// 实时报价数据
		{
			CMmiReqRealtimePrice Req;
			for ( MerchIntMap::const_iterator it = merchExistMap.begin(); it != merchExistMap.end() ; it++ )
			{
				if ( NULL == it->first )
				{
					continue;
				}
				
				if ( Req.m_StrMerchCode.IsEmpty() )
				{
					Req.m_iMarketId			= it->first->m_MerchInfo.m_iMarketId;
					Req.m_StrMerchCode		= it->first->m_MerchInfo.m_StrMerchCode;						
				}
				else
				{
					CMerchKey MerchKey;
					MerchKey.m_iMarketId	= it->first->m_MerchInfo.m_iMarketId;
					MerchKey.m_StrMerchCode = it->first->m_MerchInfo.m_StrMerchCode;
					
					Req.m_aMerchMore.Add(MerchKey);					
				}
			}
			
			DoRequestViewData(Req);
		}
		
		
		{
			bool32 bNeedIndexReq = false;
			for ( i=0; i < m_ReportHeadInfoList.GetSize() && bNeedChooseStockData; i++ )
			{
				CString StrHeadName = m_ReportHeadInfoList.GetAt(i).m_StrHeadNameCn;
				
				if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHShortLineSelect)
					|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMidLineSelect)
					)
				{
					bNeedIndexReq = true;
					break;
				}
			}
			if ( bNeedIndexReq ) 
			{
				// 增加选股数据的发送请求
				CMmiReqMerchIndex	reqIndex;			// 单个数据
				CMmiReqPushPlugInMerchData	reqPush;	// 推送数据
				
				for ( MerchIntMap::const_iterator it = merchExistMap.begin(); it != merchExistMap.end() ; it++ )
				{
					if ( NULL == it->first )
					{
						continue;
					}
					
					if ( reqIndex.m_StrMerchCode.IsEmpty() )
					{
						reqIndex.m_iMarketId		= it->first->m_MerchInfo.m_iMarketId;
						reqIndex.m_StrMerchCode		= it->first->m_MerchInfo.m_StrMerchCode;						
						reqPush.m_iMarketID			= it->first->m_MerchInfo.m_iMarketId;
						reqPush.m_StrMerchCode		= it->first->m_MerchInfo.m_StrMerchCode;
						reqPush.m_uType             = ECSTChooseStock;
					}
					else
					{
						CMerchKey MerchKey;
						MerchKey.m_iMarketId	= it->first->m_MerchInfo.m_iMarketId;
						MerchKey.m_StrMerchCode = it->first->m_MerchInfo.m_StrMerchCode;
						
						reqIndex.m_aMerchMore.Add(MerchKey);					
						
						T_ReqPushMerchData PushData;
						PushData.m_iMarket = it->first->m_MerchInfo.m_iMarketId;
						PushData.m_StrCode = it->first->m_MerchInfo.m_StrMerchCode;
						PushData.m_uType   = ECSTChooseStock;
						reqPush.m_aReqMore.Add(PushData);
					}
				}
				
				DoRequestViewData(reqIndex);
				DoRequestViewData(reqPush);
			}
		}
		
		{
			// 主力增仓
			bool32 bNeedMainMasukuraReq = false;
			for ( i=0; i < m_ReportHeadInfoList.GetSize(); i++ )
			{
				CString StrHeadName = m_ReportHeadInfoList.GetAt(i).m_StrHeadNameCn;

				if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayMasukuraProportion)
					|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayRanked)
					|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayRise) )
				{
					bNeedMainMasukuraReq = true;
					break;
				}
			}
			if ( bNeedMainMasukuraReq ) 
			{
				// 增加主力增仓数据的发送请求
				CMmiReqMainMasukura	reqMainMasukura;	// 单个数据
				CMmiReqPushPlugInMerchData	reqPush;	// 推送数据

				for ( int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
				{
					CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
					if ( NULL == SmartAttendMerch.m_pMerch )
					{
						continue;
					}

					if ( 0 == m_aSmartAttendMerchs.GetSize() )
					{
						reqMainMasukura.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
						reqMainMasukura.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
						reqPush.m_iMarketID			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
						reqPush.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
						reqPush.m_uType             = ECSTMainMasukura;
					}
					else
					{
						CMerchKey MerchKey;
						MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
						MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;

						reqMainMasukura.m_aMerchMore.Add(MerchKey);					

						T_ReqPushMerchData PushData;
						PushData.m_iMarket = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
						PushData.m_StrCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
						PushData.m_uType   = ECSTMainMasukura;
						reqPush.m_aReqMore.Add(PushData);
					}
				}

				//	如果没有关心的商品，就不发通信了
				if (0 != m_aSmartAttendMerchs.GetSize())
				{
					DoRequestViewData(reqMainMasukura);
					DoRequestViewData(reqPush);
				}	
			}
		}
		
		
		{
			// 请求财务数据
			CMmiReqPublicFile	reqF10;
			reqF10.m_ePublicFileType = EPFTF10;
			for ( MerchIntMap::const_iterator it = merchExistMap.begin(); it != merchExistMap.end() && bNeedF10 ; it++  )
			{
				if ( NULL == it->first ) // 不关注，不处理
				{
					continue;
				}
				
				
				reqF10.m_iMarketId			= it->first->m_MerchInfo.m_iMarketId;
				reqF10.m_StrMerchCode		= it->first->m_MerchInfo.m_StrMerchCode;						
				
				if ( !DoRequestViewData(reqF10) )
				{
					break;		// 如果不能请求就算了
				}
			}
		}
		
	}
}

void CIoViewReportSelect::UpdatePushMerchs()
{
	CIoViewReport::UpdatePushMerchs();

	// 是否有选股的临时队列数据
	for ( int i=0; i < m_aHotSelect.GetSize() ; i++ )
	{
		SelectResultMap::const_iterator it = m_mapMerchsHot.find(GetHotSelectTempArrayIndex(m_aHotSelect[i]));	// 临时数据常量
		if ( it != m_mapMerchsHot.end() )
		{
			AddMerchArrayToSmartAttend(it->second);	// 仅需要实时报价
		}
	}
}

DWORD CIoViewReportSelect::GetHotSelectTempArrayIndex( E_ReportSortEx eRankType )
{
	return eRankType + 1000;
}

bool32 CIoViewReportSelect::ConvertHotSelectTempArrayToShowArray( E_ReportSortEx eRankType )
{
	int32 iTab = 0;
	bool32 bCurrentTab = false;
	if ( !GetChooseStockTab(iTab, &bCurrentTab) )		// 是否存在选股tab
	{
		return false;
	}

	// 合并商品 - 更改为∩
	E_ReportSortEx eRSE = eRankType;

	// 筛选涨价格商品
	int32 i=0;
	const DWORD dwTmpIndex = GetHotSelectTempArrayIndex(eRSE);
	SelectResultMap::const_iterator it = m_mapMerchsHot.find(dwTmpIndex);
	ASSERT( it != m_mapMerchsHot.end() );
	MerchArray aMerchsMy;
	for ( i=0; it != m_mapMerchsHot.end() && i < it->second.GetSize() ; i++ )
	{
		CMerch *pMerch = it->second[i];
		if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
		{
			continue;
		}

		E_ReportType eRT = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
		ASSERT( eRT == ERTStockCn );
		if ( pMerch->m_pRealtimePrice->m_fPriceNew > pMerch->m_pRealtimePrice->m_fPricePrevClose )
		{
			aMerchsMy.Add( pMerch );
			if ( aMerchsMy.GetSize() >= 4 )
			{
				break;
			}
		}
	}
	m_mapMerchsHot.erase(dwTmpIndex);	// !!移出该临时队列
	it = m_mapMerchsHot.end();
	m_mapMerchsHot[eRSE].Copy(aMerchsMy);
	
	bool32 bMyDataOk = true;
	for ( int j=0; j < m_aHotSelect.GetSize() ; j++ )
	{
		if ( m_mapMerchsHot.count(m_aHotSelect[j]) <= 0 )
		{
			bMyDataOk  = false;
			break;
		}
	}
	
	if ( bMyDataOk  )	// 全部返回
	{
		// 合并
		MerchCountMap merchsCount;
		MerchArray	  aMerchsFirst;
		for ( SelectResultMap::const_iterator itor = m_mapMerchsHot.begin(); itor != m_mapMerchsHot.end() ; itor++ )
		{
			//ASSERT( m_aHotSelect.GetSize() == m_mapMerchsHot.size() );		// 临时队列 + 正式
			for ( int k=0; k < itor->second.GetSize() ; k++ )		// 假如同一个队列中存在重复的，就会出现判断失误的情况
			{
				CMerch *pMerch = itor->second[k];
				MerchCountMap::iterator itCount = merchsCount.find(pMerch);
				if ( itCount != merchsCount.end() )
				{
					itCount->second++;
				}
				else
				{
					merchsCount[pMerch] = 1;
				}
			}
			if ( itor == m_mapMerchsHot.begin() )
			{
				aMerchsFirst.Copy(itor->second);
			}
		}
		
		const DWORD	dwCount = m_aHotSelect.GetSize();
		MerchArray	aMerchsShow;
// 		for ( MerchCountMap::const_iterator itCount = merchsCount.begin(); itCount != merchsCount.end() ; itCount++ )
// 		{
// 			if ( itCount->second == dwCount && itCount->first != NULL )
// 			{
// 				aMerchsShow.Add( itCount->first );
// 			}
// 		}
		// 按照序列插入 - 虽然效率低
		for ( i=0; i < aMerchsFirst.GetSize() ; i++ )
		{
			MerchCountMap::const_iterator itCount = merchsCount.find(aMerchsFirst[i]);
			if ( itCount != merchsCount.end() && itCount->second == dwCount )
			{
				aMerchsShow.Add( itCount->first );
			}
		}
		
		// 变更数据
		
		m_aTabInfomations[iTab].m_Block.m_aMerchs.Copy(aMerchsShow);	// 设置商品列表源
		if ( m_aTabInfomations[iTab].m_iDataIndexBegin < 0 || m_aTabInfomations[iTab].m_iDataIndexEnd >= aMerchsMy.GetSize() )	// 重置开始-结束数据
		{
			m_aTabInfomations[iTab].m_iDataIndexBegin = 0;
			m_aTabInfomations[iTab].m_iDataIndexEnd = aMerchsShow.GetSize() - 1;
		}

		if ( bCurrentTab )
		{
			// 变更显示
			bool32 bSort = m_bRequestViewSort;
			SetTabParams(iTab);		// 现在有数据了，重设参数
			CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
			//ReCreateTabWnd();
			//TabChange();
			ReSetGridCellDataByMerchs(aMerchsShow);
//			ReSetGridContent();
			UpdateTableContent(TRUE, NULL, false);
			m_GridCtrl.SetSelectedRange(rangeSel);
			// 			
			// 			ReSetGridCellDataByMerchs(aMerchs);
			// 			UpdateTableContent(TRUE, NULL, false);
			// 			
			m_bRequestViewSort = bSort;
			if ( !RequestViewDataSort() )
			{
				RequestViewDataCurrentVisibleRow();	
			}
		}
	}
	return true;
}

void CIoViewReportSelect::OnVDataRealtimePriceUpdate( IN CMerch *pMerch )
{
	if ( pMerch == NULL/* || NULL == pMerch->m_pRealtimePrice*/ )	// NULL的real time price也会传进来，小心
	{
		// 这个情况是错误的参数
		return;
	}

	bool32 bIsStock = pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn;	// 仅支持沪深a

	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();

	int32 iTab;
	bool32 bCurrentTab;
	if ( NULL != pMerch->m_pRealtimePrice && GetChooseStockTab(iTab, &bCurrentTab) && bIsStock && eCurSel == CStockSelectManager::HotSelect )
	{
		// 检查选股临时队列中的商品
		bool32 bInTmpCalc = false;
		for ( int32 i=0; i < m_aHotSelect.GetSize() ; i++ )
		{
			DWORD dwTmpIndex = GetHotSelectTempArrayIndex(m_aHotSelect[i]);
			SelectResultMap::const_iterator it = m_mapMerchsHot.find(dwTmpIndex);
			if ( it != m_mapMerchsHot.end() )
			{
				bInTmpCalc = true;		// 正在临时列表运算当中

				int32 j=0;
				for ( j=0; j < it->second.GetSize() ; j++ )
				{
					if ( it->second[j]->m_pRealtimePrice == NULL )
					{
						break;
					}
				}
				if ( j >= it->second.GetSize() )	// 这个运算完毕会清除临时队列，所以不必要判断是不是相关商品的更新
				{
					ConvertHotSelectTempArrayToShowArray(m_aHotSelect[i]);	// 该临时队列所有数据都已经准备好了，简单判断
				}
			}
			else
			{
				//ASSERT( 0 ); // - 有可能列表没回来或者回来完毕已删除
			}
		}
		
		bool32 bRise = pMerch->m_pRealtimePrice->m_fPriceNew > pMerch->m_pRealtimePrice->m_fPricePrevClose;
		if ( !bInTmpCalc && !bRise )
		{
			// 正式显示状态情况下，如果发现有非涨股票出现在热门XX选股中，立即删除该股并重新显示
			CArray<CMerch *, CMerch *> aMerchsShow;
			aMerchsShow.Copy(m_aTabInfomations[iTab].m_Block.m_aMerchs);
			for ( int32 i=aMerchsShow.GetSize()-1; i >= 0 ; i-- )
			{
				if ( aMerchsShow[i] == pMerch )
				{
					aMerchsShow.RemoveAt(i);
					ResetShowData(aMerchsShow);
				}
			}
		}
	}

	CIoViewReport::OnVDataRealtimePriceUpdate(pMerch);
}

bool32 CIoViewReportSelect::GetChooseStockTab( OUT int32 &iTab, OUT bool32 *pbIsCurrent /*= NULL*/ )
{
	if ( NULL != pbIsCurrent )
	{
		*pbIsCurrent = false;
	}

	T_BlockDesc::E_BlockType eType;
	if ( TabIsValid(iTab, eType) && (eType == T_BlockDesc::EBTSelect || eType == T_BlockDesc::EBTMerchSort) )
	{
		if ( NULL != pbIsCurrent )
		{
			*pbIsCurrent = true;
		}
		return true;
	}

	for ( int32 i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		if ( m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTSelect || m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTMerchSort )
		{
			iTab = i;
			return true;
		}
	}
	return false;
}

void CIoViewReportSelect::ResetShowData( const MerchArray &aMerchsShow )
{
	int32 iTab;
	bool32 bCurrent;
	if ( !GetChooseStockTab(iTab, &bCurrent) )
	{
		return;
	}

	m_aTabInfomations[iTab].m_Block.m_aMerchs.Copy(aMerchsShow);	// 设置商品列表源
	int32 iMerchShowCount = m_aTabInfomations[iTab].m_iDataIndexEnd - m_aTabInfomations[iTab].m_iDataIndexBegin + 1;
	if ( m_aTabInfomations[iTab].m_iDataIndexBegin < 0 )	// 重置开始-结束数据
	{
		m_aTabInfomations[iTab].m_iDataIndexBegin = 0;
		m_aTabInfomations[iTab].m_iDataIndexEnd = aMerchsShow.GetSize() - 1;
	}
	else if ( m_aTabInfomations[iTab].m_iDataIndexEnd >= aMerchsShow.GetSize() )
	{
		m_aTabInfomations[iTab].m_iDataIndexEnd = aMerchsShow.GetSize() - 1;
		m_aTabInfomations[iTab].m_iDataIndexBegin = m_aTabInfomations[iTab].m_iDataIndexEnd - iMerchShowCount +1;
		if ( m_aTabInfomations[iTab].m_iDataIndexBegin < 0 || m_aTabInfomations[iTab].m_iDataIndexBegin > m_aTabInfomations[iTab].m_iDataIndexEnd )
		{
			m_aTabInfomations[iTab].m_iDataIndexBegin = 0;
		}
	}
	if ( bCurrent )
	{
		SetTabParams(iTab);		// 现在有数据了，重设参数 - 如果有排序该怎么处理？？
		bool32 bSort = m_bRequestViewSort;
		CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
		//ReCreateTabWnd();
		//TabChange();
		ReSetGridContent();
		UpdateTableContent(TRUE, NULL, false);
		m_GridCtrl.SetSelectedRange(rangeSel);
		// 			
		// 			ReSetGridCellDataByMerchs(aMerchs);
		// 			UpdateTableContent(TRUE, NULL, false);
		//
		m_bRequestViewSort = bSort;
		if ( !RequestViewDataSort() )
		{
			RequestViewDataCurrentVisibleRow();	
		}	
	}
}

bool32 CIoViewReportSelect::FromXmlExtra( TiXmlElement *pTiXmlElement )
{
	// 两种初始化途径 - instance 和 createobject
	if (NULL == pTiXmlElement)
		return false;

	// 不解析任何选股资料

	// 当前选择，两种选择的参数
// 	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::IndexShortMidSelect;
// 	DWORD			dwIndex	= CStockSelectManager::ShortLine;
// 	DWORD			dwHot	= CStockSelectManager::HotTimeSale;
// 
// 	TiXmlElement *pMyEle = pTiXmlElement->FirstChildElement(KStrXmlKey);
// 	if ( NULL != pMyEle )
// 	{
// 		const char *pcValue = NULL;
// 		pcValue = pMyEle->Attribute(KStrXmlAttriCurrentSelectTypeName);
// 		if ( NULL != pcValue )
// 		{
// 			int iCurSel = atoi(pcValue);
// 			if ( iCurSel >= CStockSelectManager::IndexShortMidSelect && iCurSel <= CStockSelectManager::HotSelect )
// 			{
// 				eCurSel = (CStockSelectManager::E_CurrentSelect)iCurSel;
// 			}
// 		}
// 		
// 		pcValue = pMyEle->Attribute(KStrXmlAttriReqIndexSelectStockFlagName);
// 		if ( NULL != pcValue )
// 		{
// 			dwIndex = (DWORD)atol(pcValue);
// 		}
// 		pcValue = pMyEle->Attribute(KStrXmlAttriReqHotSelectFlagName);
// 		if ( NULL != pcValue )
// 		{
// 			dwHot = (DWORD)atol(pcValue);
// 		}
// 
// 		CStockSelectManager::Instance().SetIndexSelectStockFlag(dwIndex);
// 		CStockSelectManager::Instance().SetHotSelectStockFlag(dwHot);
// 		CStockSelectManager::Instance().SetCurrentReqType(eCurSel);
// 	}

	return true;
}

CString CIoViewReportSelect::ToXmlExtra()
{
	// 不保存任何选股有关资料，一切默认
	CString StrRet = L"";
// 	StrRet.Format(_T("<%s %s=\"%d\" %s=\"%u\" %s=\"%u\" />\n"),
// 		CString(KStrXmlKey),
// 		CString(KStrXmlAttriCurrentSelectTypeName), CStockSelectManager::Instance().GetCurrentReqType(),
// 		CString(KStrXmlAttriReqIndexSelectStockFlagName), CStockSelectManager::Instance().GetIndexSelectStockFlag(),
// 		CString(KStrXmlAttriReqHotSelectFlagName),	CStockSelectManager::Instance().GetHotSelectStockFlag()
// 		);

	return StrRet;
}

void CIoViewReportSelect::OnStockSelectTypeChanged()
{
//	// 如果当前是激活视图，则用户的意思为切换
//	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
//	if ( NULL != pMainFrame && this == pMainFrame->FindActiveIoView() )
	// 当前打开不是智能选股标签，需要先打开智能选股标签

	CStockSelectManager::E_CurrentSelect eCurSel = CStockSelectManager::Instance().GetCurrentReqType();

	if (CStockSelectManager::HotSelect == eCurSel)
	{
		OpenSelBlock(T_BlockDesc::EBTMerchSort);
	}
	else
	{
		OpenSelBlock(T_BlockDesc::EBTSelect);
	}

	// 需要先清空数据吗？
	if ( (eCurSel == CStockSelectManager::HotSelect && CStockSelectManager::Instance().GetHotSelectStockFlag() == 0)
		|| (eCurSel == CStockSelectManager::IndexShortMidSelect && CStockSelectManager::Instance().GetIndexSelectStockFlag() == 0) )
	{
		MerchArray aMerchs;
		ResetShowData(aMerchs);
	}

	RequestViewData();
}

void CIoViewReportSelect::PromptLackRight(int eRightDeny)
{
	if ( !m_bPromptedByLackOfRight )
	{
		CStockSelectManager::Instance().PromptLackRight(eRightDeny);
		m_bPromptedByLackOfRight = true;
	}
}

void CIoViewReportSelect::OpenSelBlock(const T_BlockDesc::E_BlockType &eType)
{
	T_BlockDesc blockDesc;
	blockDesc.m_eType			= eType;
	blockDesc.m_StrBlockName	= _T("条件选股");
	blockDesc.m_iMarketId		= CBlockInfo::GetInvalidId();
	OpenBlock(blockDesc);	
}

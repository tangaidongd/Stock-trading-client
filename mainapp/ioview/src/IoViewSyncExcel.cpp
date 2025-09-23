#include "stdafx.h"
#include "ShareFun.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "PathFactory.h"
#include "IoViewShare.h"
#include "IoViewSyncExcel.h"
#include "DlgSyncExcel.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

//const int32 KUpdatePushMerchsTimerId	= 100005;		// 每隔 5 秒钟, 重新计算一次当前的推送商品
//const int32 KTimerPeriodPushMerchs		= 1000 * 5;

const int32 KCheckExcelExistTimerId		= 100006;		// 每隔 20 秒钟, 扫描excel是否被关闭了
const int32 KCheckExcelExistPeriod		= 1000 * 20;

const DWORD	KExcelSuspendedWait			= 200;			// excel陷入挂起后，等待 n ms才能进行cell设置

const DWORD VBA_E_IGNORE = 0x800AC472;					// excel异常号

CIoViewSyncExcel *CIoViewSyncExcel::m_sthis = NULL;

IMPLEMENT_DYNCREATE(CIoViewSyncExcel, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewSyncExcel, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewSyncExcel)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_MESSAGE(CSyncExcelThread::EUMT_ExcelSheetError, OnSheetError)
ON_MESSAGE(CSyncExcelThread::EUMT_ExcelAllSheetError, OnAllSheetError)
ON_MESSAGE(CSyncExcelThread::EUMT_ExcelAppInit, OnExcelAppInit)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////
// 
// extern "C" const GUID __declspec(selectany) IID_IWorkbookEvents =
//     {0x00024412,0x0001,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
// 
// BEGIN_INTERFACE_MAP(CIoViewSyncExcel, CIoViewBase)
// 	INTERFACE_PART(CIoViewSyncExcel, IID_IWorkbookEvents, MyWorkBookEventsSink)
// END_INTERFACE_MAP()
// 
// ULONG FAR EXPORT CIoViewSyncExcel::XMyWorkBookEventsSink::AddRef()
// {
//     METHOD_PROLOGUE(CIoViewSyncExcel, MyWorkBookEventsSink)
// 	return pThis->ExternalAddRef();
// }
// 
// ULONG FAR EXPORT CIoViewSyncExcel::XMyWorkBookEventsSink::Release()
// {
//     METHOD_PROLOGUE(CIoViewSyncExcel, MyWorkBookEventsSink)
// 		return pThis->ExternalRelease();
// }
// 
// HRESULT FAR EXPORT CIoViewSyncExcel::XMyWorkBookEventsSink::QueryInterface(
// 														   REFIID iid, void FAR* FAR* ppvObj)
// {
//     METHOD_PROLOGUE(CIoViewSyncExcel, MyWorkBookEventsSink)
// 		return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
// }
// 
// void FAR EXPORT CIoViewSyncExcel::XMyWorkBookEventsSink::OnBeforeClose(BOOL *pCancel)
// {
//     METHOD_PROLOGUE(CIoViewSyncExcel, MyWorkBookEventsSink)
// 	OnBookBeforeClose(pCancel);
// }


CIoViewSyncExcel & CIoViewSyncExcel::Instance()
{
	if(m_sthis == NULL) m_sthis = new CIoViewSyncExcel;
	ASSERT(m_sthis);

	if ( !IsWindow(m_sthis->m_hWnd) )
	{
		CRect rect(0,0,0,0);
		if ( !m_sthis->Create(WS_CHILD|WS_VISIBLE, AfxGetMainWnd(), rect, (UINT)(-1)) )
		{
			ASSERT( 0 );
		}
	}
	ASSERT( IsWindow(m_sthis->m_hWnd) );
	
	if ( NULL == m_sthis->GetCenterManager() )
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		if ( NULL != pDoc )
		{
			m_sthis->SetCenterManager(pDoc->m_pAbsCenterManager);
		}
	}
	
	ASSERT( m_sthis->GetCenterManager() != NULL );
	
	return *m_sthis;
}

void CIoViewSyncExcel::DelInstance()
{
	if(m_sthis)
	{
		DEL(m_sthis);
	}
}

CIoViewSyncExcel::CIoViewSyncExcel()
:CIoViewBase()
{

}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewSyncExcel::~CIoViewSyncExcel()
{
	
}

BOOL CIoViewSyncExcel::PreTranslateMessage(MSG* pMsg)
{
	return CIoViewBase::PreTranslateMessage(pMsg);
}

int CIoViewSyncExcel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//
//	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	SetTimer(KCheckExcelExistTimerId, KCheckExcelExistPeriod, NULL);
	
	MoveWindow(0,0,0,0);
	return 0;
}

void CIoViewSyncExcel::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	if ( cx > 0 && cy > 0 )
	{
		MoveWindow(0,0,0,0);
	}
}

// 通知视图改变关注的商品
void CIoViewSyncExcel::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewSyncExcel::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewSyncExcel::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	UpdateMerchData(pMerch);
}

// 当物理市场排行发生变化
// 当物理市场排行发生变化
void CIoViewSyncExcel::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	// 与排行无关
}

void CIoViewSyncExcel::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		if ( m_aAttendBlocks[i].m_eHeadType == eReportType )
		{
			UpdateBlockData(m_aAttendBlocks[i]);
		}
	}
	//UpdateData();	// 更新与eReportType有关的数据 - 现全更新
}

void CIoViewSyncExcel::OnTimer(UINT nIDEvent) 
{
	//if ( nIDEvent == KUpdatePushMerchsTimerId )
	//{
	//	RequestViewData();
	//}
	if ( nIDEvent == KCheckExcelExistTimerId )
	{
		// 检查所有的block - sheet
		CSyncExcelThread::Instance().CheckExcelStatus();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewSyncExcel::RequestViewData(const CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts )
{
	CMmiReqRealtimePrice Req;
	bool32 bF10 = false;
	
	for ( int32 i = 0; i < aRequestSmarts.GetSize(); i++ )
	{
		const CSmartAttendMerch &SmartAttendMerch = aRequestSmarts[i];
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}

		bF10 = bF10 || (SmartAttendMerch.m_iDataServiceTypes&EDSTGeneral);
		
		if ( 0 == i )
		{
			Req.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
		}
		else
		{
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
			
			Req.m_aMerchMore.Add(MerchKey);					
		}
	}

	if(NULL == m_pAbsCenterManager)
	{
		return;
	}
	
	m_pAbsCenterManager->RequestViewData(&Req);

	int32 i = 0;
	bool32 bNeedIndexReq = IsAttendMerchIndexData();	
	if ( bNeedIndexReq ) 
	{
		// 增加选股数据的发送请求
		CMmiReqMerchIndex	reqIndex;			// 单个数据
		CMmiReqPushPlugInMerchData	reqPush;	// 推送数据
		
		for ( i = 0; i < aRequestSmarts.GetSize(); i++ )
		{
			const CSmartAttendMerch &SmartAttendMerch = aRequestSmarts[i];
			if ( NULL == SmartAttendMerch.m_pMerch )
			{
				continue;
			}
			
			if ( 0 == i )
			{
				reqIndex.m_iMarketId		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqIndex.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
				reqPush.m_iMarketID			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqPush.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				reqPush.m_uType             = ECSTChooseStock;
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				
				reqIndex.m_aMerchMore.Add(MerchKey);					
				
				T_ReqPushMerchData PushData;
				PushData.m_iMarket = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				PushData.m_StrCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				PushData.m_uType   = ECSTChooseStock;
				reqPush.m_aReqMore.Add(PushData);
			}
		}
		
		m_pAbsCenterManager->RequestViewData(&reqIndex);
		m_pAbsCenterManager->RequestViewData(&reqPush);
		//DoRequestViewData(reqIndex);
		//DoRequestViewData(reqPush);
	}
	
	// 请求财务数据
	if ( bF10 )
	{
		CMmiReqPublicFile	reqF10;
		reqF10.m_ePublicFileType = EPFTF10;
		for ( i= 0; i < aRequestSmarts.GetSize(); i++ )
		{
			const CSmartAttendMerch &SmartAttendMerch = aRequestSmarts[i];
			if ( NULL == SmartAttendMerch.m_pMerch )
			{
				continue;
			}
			
			reqF10.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			reqF10.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
			
			m_pAbsCenterManager->RequestViewData(&reqF10);
		}
	}
}

void CIoViewSyncExcel::RequestViewData()
{
	m_aSmartAttendMerchs.RemoveAll();
	for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		AddAttendMerchs(m_aAttendBlocks[i].m_aMerchs, false); // 会将新的商品添加到m_aSmartAttendMerchs，现在不发请求
	}
	RequestViewData(m_aSmartAttendMerchs); // 请求一起发送
}

void CIoViewSyncExcel::OnIoViewColorChanged()
{
	
}

void CIoViewSyncExcel::OnIoViewFontChanged()
{
	
}

bool32 CIoViewSyncExcel::InitialBlockHeader( const T_Block &block )
{
	// 更新excel中的表头信息
	CStringArray aCols;
	if ( GetBlockHeaderList(&aCols, block.m_eHeadType) < 1 )
	{
		return false;
	}

	for ( long i=0; i < aCols.GetSize() ; i++ )
	{
		SetExcelCellValue(block.m_StrName, 1, (USHORT)(i+1), aCols[i], false, 0, true);
	}
	
	return true;
}

bool32 CIoViewSyncExcel::UpdateMerchData( CMerch *pMerch, bool32 bForce /*= false*/ )
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return false;
	}

	CRealtimePrice	rtPrice = *pMerch->m_pRealtimePrice;
	for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		UpdateBlockMerchData(m_aAttendBlocks[i], pMerch, bForce);
	}
	return true;	
}

bool32 CIoViewSyncExcel::UpdateBlockMerchData( const T_Block &block, CMerch *pMerch, bool32 bForce /*= false*/ , int32 index)
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return false;
	}

	int iPos = index;
	if (-1 == iPos)
	{
		for ( iPos=0; iPos < block.m_aMerchs.GetSize() ; iPos++  )
		{
			if ( block.m_aMerchs[iPos] == pMerch )
			{
				break;
			}
		}
	}

	int32 iBegin = -1, iNum=0;
	for (int k=0; k < block.m_aMerchs.GetSize() ; k++ )
	{
		if ( block.m_aMerchs[k] == pMerch )
		{
			iNum++;
			if (-1==iBegin)
			{
				iBegin = k;
			}
		}
	}

	if ( iPos >= block.m_aMerchs.GetSize() )
	{
		return false;	// 不是该block关注的merch
	}

	if ( !bForce )
	{
		if ( IsPriceCached(block.m_StrName, pMerch) )
		{
			return true; // 非强制更新 - 数据没有更新，则不用更新了 - 数据缓冲是否按照block的来
		}
	}

	// 得到最新价结构
	CMerch *pMerchRow = pMerch;
	CRealtimePrice RealtimePrice = *pMerch->m_pRealtimePrice;
	
	float fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
	if (ERTFuturesCn == pMerch->m_Market.m_MarketInfo.m_eMarketReportType)	// 国内期货使用昨结算作为参考价格
	{
		fPrevReferPrice = RealtimePrice.m_fPricePrevAvg;
	}
	
	// 
	CMerchExtendData ExtendData;
	if (NULL != pMerch && NULL != pMerch->m_pMerchExtendData)
	{
		ExtendData = *pMerch->m_pMerchExtendData;
	}	

	// 财务数据
	CFinanceData	FinanceData;
	if ( NULL != pMerch->m_pFinanceData )
	{
		FinanceData = *pMerch->m_pFinanceData;
	}

	// 选股标志
	bool32 bUseMerchIndex = false;
	MerchIndexMap::const_iterator it = m_mapMerchIndex.find(pMerch);
	T_RespMerchIndex RespMerchIndex;
	if ( it != m_mapMerchIndex.end() )
	{
		bUseMerchIndex = true;
		RespMerchIndex = it->second;
	}
	CStringArray aMerchIndexSelectStr; // 选股 红三角 绿三角 橙圆 蓝方
	aMerchIndexSelectStr.Add(_T("▲"));
	aMerchIndexSelectStr.Add(_T("▲"));
	aMerchIndexSelectStr.Add(_T("●"));
	aMerchIndexSelectStr.Add(_T("■"));
	CArray<COLORREF, COLORREF> aMerchIndexColors;
	aMerchIndexColors.Add(RGB(250,0,0));
	aMerchIndexColors.Add(RGB(0,255,0));
	aMerchIndexColors.Add(RGB(255,255,0));
	aMerchIndexColors.Add(RGB(0,0,255));
	ASSERT( aMerchIndexColors.GetSize() == aMerchIndexSelectStr.GetSize() && aMerchIndexSelectStr.GetSize() == ESSCount );
	
	// ...fangz0926 补丁 成交量其他的颜色
	COLORREF clrCurHold = RGB(128, 128, 0);
	COLORREF clrRise = RGB(225,0,0);//GetIoViewColor(ESCRise);
	COLORREF clrFall = RGB(0,127,0);//GetIoViewColor(ESCFall);
	COLORREF clrKeep = RGB(127,127,127);//GetIoViewColor(ESCKeep);
	COLORREF clrDefault = RGB(0,0,0);
	
	COLORREF clr = RGB(0,0,0);
	COLORREF clrForce = CLR_INVALID; // 强制颜色，如果其设定了的话，clr必须取该值
	bool32 bUseColor = false;		// 是否使用涨跌色
	bool32 bTrimSymbol = false;		// 是否显示前面+ - 符号，+号总不显示
	
	CArray<T_HeadInfo,T_HeadInfo> aHeadInfo;
	if ( GetBlockHeaderList(aHeadInfo, block.m_eHeadType) < 1 )
	{
		return false;
	}

	bool32 bIsFuture = CReportScheme::IsFuture(GetMerchKind(pMerch));
	bool32 bShowTranslate = !bIsFuture;
	bShowTranslate = false;	// 全部都显示真实数据
	
	// 跟据列表头显示的内容， 显示具体的数据
	for (long iCol = 0; iCol < aHeadInfo.GetSize() ; iCol++)
	{
		bUseColor = false;
		bTrimSymbol = false;
		clr = clrDefault;
		clrForce = CLR_INVALID;

		// 表头项( 序号, 名称, 代码....)
		CReportScheme::E_ReportHeader ERHType;
		ERHType = CReportScheme::Instance()->GetReportHeaderEType(aHeadInfo[iCol].m_StrHeadNameCn);
		
		// 商品类型 (国内期货, 国内证券, 外汇...)
		E_ReportType eMerchKind = GetMerchKind(pMerch);	
		
		CString StrCell;
		float fVal = 0.0f;

		//
		switch ( ERHType )
		{
		case CReportScheme::ERHRowNo:
			{
				StrCell.Format(_T("%d"), iPos+1);
			}
			break;
		case CReportScheme::ERHMerchCode:
			{
				StrCell = pMerch->m_MerchInfo.m_StrMerchCode;
			}
			break;
		case CReportScheme::ERHMerchName:
			{
				if (((iBegin==iPos) && (1==iNum)) || (iBegin!=iPos))
				{
					if (pMerch->m_StrMerchFakeName.IsEmpty())
					{
						StrCell = pMerch->m_MerchInfo.m_StrMerchCnName;
					}
					else
					{
						StrCell = pMerch->m_StrMerchFakeName;
					}
				}
				else
				{
					StrCell = pMerch->m_MerchInfo.m_StrMerchCnName;
				}
			}
			break;
		case CReportScheme::ERHPricePrevClose:			
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);	// 总是显示持平
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHPricePrevBalance:
			{
				// 昨结收
				if (CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = Float2SymbolString(RealtimePrice.m_fPricePrevAvg, RealtimePrice.m_fPricePrevAvg, pMerch->m_MerchInfo.m_iSaveDec);
				}
				else
				{
					StrCell = _T("-");
				}					
			}
			break;
		case CReportScheme::ERHPriceOpen:
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceOpen, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHPriceNew:
			{
				
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceNew, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHPriceBalance:			// 结算价: 今天的均价
			{					
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceAvg, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHPriceHigh:
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceHigh, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);	
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHPriceLow:
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceLow, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHVolumeCur:
			{
				StrCell = Float2String(RealtimePrice.m_fVolumeCur, 0, bShowTranslate);
				bUseColor = true;
				clr = clrCurHold;
			}
			break;
		case CReportScheme::ERHVolumeTotal:
			{
				StrCell = Float2String(RealtimePrice.m_fVolumeTotal, 0, bShowTranslate);
				bUseColor = true;
				clr = clrCurHold;
			}
			break;
		case CReportScheme::ERHAmount:			
			{
				StrCell = Float2String(RealtimePrice.m_fAmountTotal, 0, bShowTranslate);
				bUseColor = true;
				clr = clrCurHold;
			}
			break;
		case CReportScheme::ERHRiseFall:			// 涨跌
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerch->m_MerchInfo.m_iSaveDec);	
				bUseColor = true;
				bTrimSymbol = false;	// 显示-
			}
			break;
		case CReportScheme::ERHRange:				// 幅度%
			{
				float   fRisePercent = 0.0;
				
				if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
				{
					fRisePercent = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
					StrCell = Float2SymbolString(fRisePercent, 0, 2);
					bUseColor = true;
					bTrimSymbol = false; // 显示-
				}
			}
			break;
		case CReportScheme::ERHSwing:				// 振幅
			{
				float fValue = 0.;
				
				if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
				{
					fValue = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
					StrCell = Float2SymbolString(fValue, fValue, 2, false, true, false);
					bUseColor = true;	// 正常颜色
					bTrimSymbol = true;
				}	
			}
			break;
		case CReportScheme::ERHTime:				// 时间
			{
				CTime TimeCurrent(RealtimePrice.m_TimeCurrent.m_Time.GetTime());
				StrCell.Format(L"%02d:%02d:%02d", 
					/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
					TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
			}
			break;
		case CReportScheme::ERHBuyPrice:			// 买入价(买1)
			{
				StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHSellPrice:			// 卖出价(卖1)
			{
				StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerch->m_MerchInfo.m_iSaveDec);
				bUseColor = true;
				bTrimSymbol = true;
			}
			break;
		case CReportScheme::ERHBuyAmount:			// 买入量(买1)
			{
				StrCell = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
			}
			break;
		case CReportScheme::ERHSellAmount:			// 卖出量(卖1)
			{
				StrCell = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
			}
			break;
		case CReportScheme::ERHRate:			  // 委比=〖(委买手数-委卖手数)÷(委买手数+委卖手数)〗×100%
			{
				float fRate = 0.0;
				int32 iBuyVolums  = 0;
				int32 iSellVolums = 0;
				
				for ( int32 i = 0 ; i < 5 ; i++)
				{
					iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
					iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
				}
				
				if ( 0 != (iBuyVolums + iSellVolums) )
				{
					fRate = (float)(iBuyVolums - iSellVolums)*(float)100 / (iBuyVolums + iSellVolums);
				}
				
				StrCell = Float2SymbolString(fRate, 0.0, 2, false, true, false);
				bUseColor = true;
				bTrimSymbol = false;	// 显示符号-
			}
			break;
		case CReportScheme::ERHHold:				// 持仓
			{										
				if (CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = Float2String(RealtimePrice.m_fHoldTotal, 0, bShowTranslate);
					bUseColor = true;
					clr = clrCurHold;
				}
				else
				{
					StrCell = _T("-");
				}
			}
			break;				
		case CReportScheme::ERHDifferenceHold:		// 持仓差(日增仓=持仓量-昨持仓)
			{				
				if (CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = Float2String(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev, 0, bShowTranslate);
					bUseColor = true;
					bTrimSymbol = false; // 显示符号
					clr = clrCurHold; 
				}
				else
				{
					StrCell = _T("-");
				}
			}
			break;
		case CReportScheme::ERHBuild:			 // 日开仓=（成交量+日增仓）/2
			{	
				float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
				float fBuild = ( RealtimePrice.m_fVolumeTotal + fAddPerDay ) / 2;
				
				StrCell = Float2String(fBuild, 0, bShowTranslate);
				
				if (!CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = _T("-");
				}
			}
			break;
		case CReportScheme::ERHClear:			// 日平仓=（成交量-日增仓）/2
			{					
				float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
				float fClear = ( RealtimePrice.m_fVolumeTotal - fAddPerDay ) / 2;
				
				StrCell = Float2String(fClear, 0, bShowTranslate);
				
				if (!CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = _T("-");
				}
			}
			break;
		case CReportScheme::ERHAddPer:			// 单笔增仓=持仓量-前一笔持仓量
			{										
				StrCell = Float2String( RealtimePrice.m_fHoldCur, 0, bShowTranslate);
				
				if (!CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = _T("-");
				}
			}
			break;
		case CReportScheme::ERHBuildPer:		// 单笔开仓=（现量+单笔增仓）/2
			{
				float fBuildPer = (RealtimePrice.m_fVolumeCur + RealtimePrice.m_fHoldCur) / 2;
				
				StrCell = Float2String( fBuildPer, 0, bShowTranslate);
				
				if (!CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = _T("-");
				}
			}
			break;
		case CReportScheme::ERHClearPer:		// 单笔平仓=（现量-单笔增仓）/2
			{
				float fClearPer = (RealtimePrice.m_fVolumeCur - RealtimePrice.m_fHoldCur) / 2;
				
				StrCell = Float2String( fClearPer, 0, bShowTranslate);
				
				if (!CReportScheme::IsFuture(eMerchKind))
				{
					StrCell = _T("-");
				}					
			}
			break;
		case CReportScheme::ERHBuyVolumn:
			{
				StrCell = Float2String(RealtimePrice.m_fBuyVolume, 0, bShowTranslate);
			}
			break;
		case CReportScheme::ERHSellVolumn:
			{
				StrCell = Float2String(RealtimePrice.m_fSellVolume, 0, bShowTranslate);
			}
			break;		
		case CReportScheme::ERHShortLineSelect:		// 短线选股 红三角 绿三角 橙圆 蓝方
			{
				if ( bUseMerchIndex )
				{
					if ( RespMerchIndex.m_usShort >=0 && RespMerchIndex.m_usShort < ESSCount )
					{
						USHORT usFlag = RespMerchIndex.m_usShort % ESSCount;
						clrForce = (aMerchIndexColors[usFlag]);
						StrCell = (aMerchIndexSelectStr[usFlag]);
					}	
				}
			}
			break;	
		case CReportScheme::ERHMidLineSelect:		// 中线选股
			{
				if ( bUseMerchIndex )
				{
					if ( RespMerchIndex.m_usMid >=0 && RespMerchIndex.m_usMid < ESSCount )
					{
						USHORT usFlag = RespMerchIndex.m_usMid % ESSCount;
						clrForce = (aMerchIndexColors[usFlag]);
						StrCell = (aMerchIndexSelectStr[usFlag]);
					}
				}
			}
			break;	
		case CReportScheme::ERHCapitalFlow:		// 资金流向
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fCapticalFlow, 0.0, 2, bShowTranslate);
				bUseColor = true;
			}
			break;	
		case CReportScheme::ERHChangeRate:		// 换手率
			{
				StrCell = Float2String(RealtimePrice.m_fTradeRate, 2, bShowTranslate);
			}
			break;	
		case CReportScheme::ERHMarketWinRate:	// 市盈率
			{
				StrCell = Float2String(RealtimePrice.m_fPeRate, 2, bShowTranslate);
			}
			break;	
		case CReportScheme::ERHVolumeRate:		// 量比
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fVolumeRate, 1.0, 2, bShowTranslate);
			}
			break;	
		case CReportScheme::ERHSpeedRiseFall:		// 快速涨跌
			{
				StrCell = Float2SymbolString(RealtimePrice.m_fRiseRate, 0.0, 2, bShowTranslate);
			}
			break;	
		case CReportScheme::ERHBuySellRate:			// 内外比
			{
				float fBuySellRate = 0.0;
				if ( RealtimePrice.m_fSellVolume != 0.0 )
				{
					fBuySellRate = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
				}
				StrCell = Float2String(fBuySellRate, 2, bShowTranslate);
			}
			break;	
		case CReportScheme::ERHAllCapital:				// 总股本
			{
				fVal = FinanceData.m_fAllCapical;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHCircAsset:				// 流通股本
			{
				fVal = FinanceData.m_fCircAsset;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHAllAsset:				// 总资产
			{
				fVal = FinanceData.m_fAllAsset;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHFlowDebt:				// 流动负债
			{
				fVal = FinanceData.m_fFlowDebt;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHPerFund:				// 每股公积金
			{
				fVal = FinanceData.m_fPerFund;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHBusinessProfit:				// 营业利益
			{
				fVal = FinanceData.m_fBusinessProfit;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHPerNoDistribute:				// 每股未分配
			{
				fVal = FinanceData.m_fPerNoDistribute;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHPerIncomeYear:				// 每股收益(年)
			{
				fVal = FinanceData.m_fPerIncomeYear;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
				if ( RealtimePrice.m_uiSeason <= 3 )
				{
					//lint --e(569)
					TCHAR chSeason = _T('㈠');
					chSeason += (TCHAR)RealtimePrice.m_uiSeason;
					StrCell += chSeason;
				}
			}
			break;
		case CReportScheme::ERHPerPureAsset:				// 每股净资产
			{
				fVal = FinanceData.m_fPerPureAsset;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHChPerPureAsset:				// 调整每股净资产
			{
				fVal = FinanceData.m_fChPerPureAsset;
				
				StrCell = Float2String(fVal, 2, false, bShowTranslate);
			}
			break;
		case CReportScheme::ERHDorRightRate:				// 股东权益比
			{
				fVal = FinanceData.m_fDorRightRate;
				
				StrCell = Float2String(fVal, 2, bShowTranslate);
			}
			break;
		case CReportScheme::ERHCircMarketValue:				// 流通市值
			{
				fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, false);
			}
			break;
		case CReportScheme::ERHAllMarketValue:				// 总市值
			{
				fVal = FinanceData.m_fAllCapical * RealtimePrice.m_fPriceNew;
				fVal /= 10000.0;
				
				StrCell = Float2String(fVal, 2, false);
			}
			break;
		case CReportScheme::ERHPowerDegree:				// 强弱度
			{
				CString StrValue = _T(" -");
				if ( NULL != m_pAbsCenterManager && m_pAbsCenterManager->GetMerchPowerValue(pMerch, fVal) )
				{
					StrCell = Float2String(fVal * 100, 2, false, false);
				}
			}
			break;
			case CReportScheme::ERHPriceSell5:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceSell4:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceSell3:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceSell2:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceSell1:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceBuy5:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceBuy4:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceBuy3:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceBuy2:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHPriceBuy1:
				{
					StrCell = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
					bUseColor = true;
					bTrimSymbol = true;
				}
				break;
			case CReportScheme::ERHVolSell5:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astSellPrices[4].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolSell4:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astSellPrices[3].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolSell3:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astSellPrices[2].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolSell2:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astSellPrices[1].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolSell1:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolBuy5:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astBuyPrices[4].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolBuy4:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astBuyPrices[3].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolBuy3:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astBuyPrices[2].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolBuy2:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astBuyPrices[1].m_fVolume, 0, bShowTranslate);
				}
				break;
			case CReportScheme::ERHVolBuy1:
				{
					bUseColor = true;
					clr = clrCurHold;
					StrCell = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
				}
				break;
		default:
			{
				float fdefault = 0.0;					
				StrCell = Float2SymbolString(fdefault,0.0,1,false);
			}
			break;
		}
		
		// 设置excel值
		if ( CLR_INVALID != clrForce )
		{
			bUseColor = true;
			clr = clrForce;	// 被强制设定了
		}
		else if ( StrCell.GetLength() > 1 ) // symbol不显示+号，其它的显示
		{
			if ( bUseColor )
			{
				if ( StrCell.GetAt(0) == _T('+') )
				{
					clr = clrRise;
					bTrimSymbol = true;	// +号不显示
				}
				else if ( StrCell.GetAt(0) == _T('-') )
				{
					clr = clrFall;
				}
				else
				{
					if ( StrCell.GetAt(0) != _T(' '))	// 其它任何都不能截断
					{
						bTrimSymbol = false;
					}
					bUseColor = true; // 持平使用默认色
					if ( clr != clrCurHold )		// 如果不是持仓色就是持平了
					{
						clr = clrKeep;
					}
				}
			}
			if ( bTrimSymbol )
			{
				StrCell.TrimLeft(_T(" +-"));
			}
		}
		SetExcelCellValue(block.m_StrName, (USHORT)(iPos+2), (USHORT)(iCol+1), StrCell, bUseColor, clr, false);		// 此处必须要缓冲
	}
		
	CachePrice(block.m_StrName, pMerch);	// 缓存该数据
		
	return true;
}

bool32 CIoViewSyncExcel::AddBlockShowMerch( const T_Block &block, int32 iPos, const MerchArray &aNewMerchs )
{
	// 设置强制显示数据 - 利用临时block显示数据
	T_Block blockTmp(block);
	blockTmp.m_aMerchs.Append(aNewMerchs);
	int i;
	for ( i=0; i < aNewMerchs.GetSize() ; i++ )
	{
		UpdateBlockMerchData(blockTmp, aNewMerchs[i], true, i);
	}

	// 添加一条空的数据
	int iColCount = GetBlockHeaderList(NULL, blockTmp.m_eHeadType);
	for ( i=0; i < iColCount ; i++ )
	{
		SetExcelCellValue(blockTmp.m_StrName, (USHORT)(blockTmp.m_aMerchs.GetSize()+2), (USHORT)(i+1), _T(""), false, 0, false);
	}
	
	// 开始添加尾部数据 - 需要清除部分数据 - 交给excel线程处理
	

	// 显示
	CSyncExcelThread::Instance().ShowExcelApp();
	return true;
}

bool32 CIoViewSyncExcel::UpdateBlockData( const T_Block &block )
{
	if ( !ClearBlockShowData(block) )
	{
		return false;
	}

	if ( !InitialBlockHeader(block) )
	{
		return false;
	}

	// 初始化所有商品数据
	for ( int i=0; i < block.m_aMerchs.GetSize() ; i++ )
	{
		if ( block.m_aMerchs[i] != NULL )
		{
			UpdateBlockMerchData(block, block.m_aMerchs[i], true, i);
		}
	}
	return true;
}

bool32 CIoViewSyncExcel::ClearBlockShowData( const T_Block &block )
{
	// 强制对所有格子初始化
	// 表头 + 商品 + 一个空行
	CStringArray aCols;
	GetBlockHeaderList(&aCols, block.m_eHeadType);
	CSyncExcelThread::Instance().ClearCellRange(block.m_StrName, block.m_aMerchs.GetSize()+2, aCols.GetSize());
	return true;
}

bool32 CIoViewSyncExcel::UpdateData()
{
	for ( int i=0; i< m_aAttendBlocks.GetSize() ; i++ )
	{
		if (!UpdateBlockData(m_aAttendBlocks[i])) // 会不会导致用户关闭了的又重新打开呢？
		{
			return false;
		}
	}
	return true;
}

// 添加流程:
//		1. 调用excel线程初始化excel资源，将板块加入等待队列
//		2. excel资源初始化完成，Post ExcelAppInit消息，通知初始化结果
//			2.1 结果OK，将等待队列的板块添加进excel中，初始化这些板块数据
//			2.2 结果Fail，报错提示
void CIoViewSyncExcel::AddSyncBlock( const T_Block &block )
{
	// 检查与block相关的sheet是否存在，不存在需要删除该block信息
	bool32 bExistSheet = false;
	TRY 
	{
		bExistSheet =  CSyncExcelThread::Instance().CheckSheetExist(block.m_StrName);
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	if ( !bExistSheet )
	{
		RemoveSyncBlock(block.m_StrName);
	}
	// 检查已经存在的block
	MerchArray aMerchsDiff;
	int iFind = FindExistBlockIndex(m_aAttendBlocks, block, aMerchsDiff);
	
	if ( iFind < m_aAttendBlocks.GetSize() && aMerchsDiff.GetSize() > 0 )
	{ // 有不同的商品 - 加入商品，更新显示 - excel异常怎么办？？
		if ( !AddBlockShowMerch(m_aAttendBlocks[iFind], m_aAttendBlocks[iFind].m_aMerchs.GetSize(), aMerchsDiff) )
		{
			MessageBox(_T("添加同步板块[") + block.m_StrName + _T("]失败！"), _T("同步失败"), MB_OK|MB_ICONWARNING);
			return;
		}
		m_aAttendBlocks[iFind].m_aMerchs.Append(aMerchsDiff);
		AddAttendMerchs(aMerchsDiff);

		return; // 添加完毕
	}

	if ( m_aAttendBlocks.GetSize() <= iFind )
	{ // 新的block - Excel显示, 数据申请 - 碰上excel异常怎么办？？ - 这里仅发出初始化请求
		// 假如还在等待怎么办？
		InitialExcelResource();
		
		int iBlockInWait = FindExistBlockIndex(m_aWaitSyncBlocks, block, aMerchsDiff);
		if ( iBlockInWait < m_aWaitSyncBlocks.GetSize() )
		{
			m_aWaitSyncBlocks[iBlockInWait].m_aMerchs.Append(aMerchsDiff);
		}
		else
		{
			m_aWaitSyncBlocks.Add(block);	// 加入等待队列
		}
	}
}

void CIoViewSyncExcel::RemoveSyncBlock( const CString &StrBlockName, bool32 bRemoveExcelSheet /*= false*/ )
{
	//
	bool bDel = false;
	for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		if ( StrBlockName == m_aAttendBlocks[i].m_StrName )
		{
			m_aAttendBlocks.RemoveAt(i);
			bDel = true;
			break;
		}
	}

	if ( bDel )
	{
		m_mapRealtimePrices.erase(StrBlockName);	// 取消cache数据
	}

	if ( bDel )
	{
		// 重组attend merchs
		m_aSmartAttendMerchs.RemoveAll();
		for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
		{
			AddAttendMerchs(m_aAttendBlocks[i].m_aMerchs, false);	// 不需要在这个时候发出请求
		}
	}

	if ( bDel && bRemoveExcelSheet ) // 需要删除excel sheet?
	{
		ASSERT( 0 );	// 暂时不支持
	}
}

bool32 CIoViewSyncExcel::ResetSyncBlockArray( const BlockArray &aBlocks )
{
	// 清除以前的 - 显示现在的
	// 如果是某些商品被移除了怎么办？这样excel中会有部分商品使不能刷新的
	int i=0;
	m_aWaitSyncBlocks.RemoveAll();	// 清空等待的

	for ( i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		int j=0;
		for ( j=0; j < aBlocks.GetSize() ; j++ )
		{
			if ( m_aAttendBlocks[i].m_StrName == aBlocks[j].m_StrName )
			{
				ClearBlockShowData(m_aAttendBlocks[i]);	// 这个是异步的 - 只有要更新的才需要清除，删除的不需要清除
			}
		}
	}

	RemoveAllSyncBlock();

	for ( i=0; i < aBlocks.GetSize() ; i++ )
	{
		AddSyncBlock(aBlocks[i]);
	}

	return false;	
}

void CIoViewSyncExcel::RemoveAllSyncBlock()
{
	int iSizeOld = m_aAttendBlocks.GetSize();
	while ( m_aAttendBlocks.GetSize() > 0 )
	{
		RemoveSyncBlock(m_aAttendBlocks[0].m_StrName);
		if ( iSizeOld == m_aAttendBlocks.GetSize() )
		{
			ASSERT( 0 );  // 应该有删除的
			break;
		}
	}
	//UnInitialExcelResource();	// 释放资源
}

int32 CIoViewSyncExcel::GetBlockHeaderList( CStringArray *pStrCols, E_ReportType eHeadType )
{
	if ( pStrCols != NULL )
	{
		pStrCols->RemoveAll();
	}
	CArray<T_HeadInfo,T_HeadInfo> aHeaderInfos;
    GetBlockHeaderList(aHeaderInfos, eHeadType);
	for ( int i=0; i < aHeaderInfos.GetSize() && pStrCols ; i++ )
	{
		pStrCols->Add(aHeaderInfos[i].m_StrHeadNameCn);
	}
	return aHeaderInfos.GetSize();
}

int32 CIoViewSyncExcel::GetBlockHeaderList( CArray<T_HeadInfo,T_HeadInfo> &aHeaderInfos, E_ReportType eHeadType )
{
	int32 iFixCol = 0;
	CReportScheme::Instance()->GetReportHeadInfoList(eHeadType, aHeaderInfos, iFixCol);
	return aHeaderInfos.GetSize();
}

void CIoViewSyncExcel::AddAttendMerchs( const CArray<CMerch *, CMerch *> &aMerchs, bool bRequestViewData/* = true */)
{
	// 注意：此处要求block必须先添加进板块列表
	UINT uFlag = EDSTPrice;
	if ( IsAttendFinanceData() )
	{
		uFlag |= EDSTGeneral;	// 报价|F10
	}
	CArray<CSmartAttendMerch, CSmartAttendMerch&> aSmart;
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( NULL == aMerchs[i] )
		{
			continue;
		}
		CMerch *pMerchAdd = aMerchs[i];
		int j=0;
		for ( j=0; j < m_aSmartAttendMerchs.GetSize() ; j++ )
		{
			CMerch *pMerchExist = m_aSmartAttendMerchs[j].m_pMerch;
			if ( pMerchAdd == pMerchExist || 
				(pMerchAdd->m_MerchInfo.m_iMarketId == pMerchExist->m_MerchInfo.m_iMarketId &&
				 pMerchAdd->m_MerchInfo.m_StrMerchCode == pMerchExist->m_MerchInfo.m_StrMerchCode) )
			{
				break;
			}
		}
		if ( j >= m_aSmartAttendMerchs.GetSize() )
		{
			CSmartAttendMerch smart;
			smart.m_pMerch = pMerchAdd;
			smart.m_iDataServiceTypes = uFlag;		
			aSmart.Add(smart);
		}
	}

	if ( aSmart.GetSize() > 0 )
	{
		m_aSmartAttendMerchs.Append(aSmart);
		if ( bRequestViewData )
		{
			RequestViewData(aSmart);	// 申请数据
		}
	}
}

bool32 CIoViewSyncExcel::IsPriceCached( const CString &StrBlock, CMerch *pMerch )
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return false;
	}

	BlockRealTimePriceMap::iterator itBlock = m_mapRealtimePrices.find(StrBlock);
	if ( itBlock != m_mapRealtimePrices.end() )
	{
		MerchRealtimePriceMap::iterator itMerch = itBlock->second.find(pMerch);
		if ( itMerch != itBlock->second.end() )
		{
			return *pMerch->m_pRealtimePrice == itMerch->second;
		}
	}
	return false;
}

void CIoViewSyncExcel::CachePrice( const CString &StrBlock, CMerch *pMerch )
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return;
	}

	m_mapRealtimePrices[StrBlock][pMerch] = *pMerch->m_pRealtimePrice;
}

void CIoViewSyncExcel::RemoveCache( const CString &StrBlock )
{
	m_mapRealtimePrices.erase(StrBlock);
}

void CIoViewSyncExcel::RemoveCache( const CString &StrBlock, CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}
	
	BlockRealTimePriceMap::iterator itBlock = m_mapRealtimePrices.find(StrBlock);
	if ( itBlock != m_mapRealtimePrices.end() )
	{
		itBlock->second.erase(pMerch);
	}
}

int CIoViewSyncExcel::FindExistBlockIndex( const BlockArray &aBlocks, const T_Block &newBlock, OUT MerchArray &aNewMerchs )
{
	int iFind = 0;
	aNewMerchs.RemoveAll();

	for ( iFind=0; iFind < aBlocks.GetSize() ; iFind++ )
	{
		if ( aBlocks[iFind].m_StrName == newBlock.m_StrName ) // 是否要对表头类型加以限制
		{
			// 同一个block - 商品是否相同
			for ( int i=0; i < newBlock.m_aMerchs.GetSize() ; i++ )
			{
				if ( NULL == newBlock.m_aMerchs[i] )
				{
					ASSERT( 0 );
					continue;
				}
				CMerch *pMerchArg = newBlock.m_aMerchs[i];
				int j=0;
				for ( j=0; j < aBlocks[iFind].m_aMerchs.GetSize(); j++ )
				{
					if ( NULL == aBlocks[iFind].m_aMerchs[j] )
					{
						ASSERT( 0 );
						continue;
					}
					CMerch *pMerchExist = aBlocks[iFind].m_aMerchs[j];
					if ( pMerchExist == pMerchArg || (pMerchExist->m_MerchInfo.m_iMarketId == pMerchArg->m_MerchInfo.m_iMarketId
						&& pMerchExist->m_MerchInfo.m_StrMerchCode == pMerchArg->m_MerchInfo.m_StrMerchCode) ) 
					{ // 相同的商品
						break;
					}
				}
				if ( j >= aBlocks[iFind].m_aMerchs.GetSize() )
				{ // 该商品不在存在的block里面
					aNewMerchs.Add(pMerchArg);
				}
			}

			break;	// 找到了
		}
	}

	return  iFind;
}

void CIoViewSyncExcel::SetExcelCellValue( const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache )
{
	CSyncExcelThread::Instance().SetCellValue(StrSheet, nRow, nCol, StrValue, bUseColor, clr, bIgnoreCache);
}

void CIoViewSyncExcel::InitialExcelResource()
{
	CSyncExcelThread::Instance().InitializeExcelResource(Instance().m_hWnd);	// 发出初始化请求
}

void CIoViewSyncExcel::UnInitialExcelResource()
{
	CSyncExcelThread::Instance().UninitializeExcelResource();	
	// 等待线程退出？
}

LRESULT CIoViewSyncExcel::OnSheetError( WPARAM w, LPARAM l )
{
	LPCTSTR lpszSheet = (LPCTSTR)w;
	if ( NULL != lpszSheet && _tcslen(lpszSheet) > 0 )
	{
		RemoveSyncBlock(CString(lpszSheet));
	}
	return 1;
}

LRESULT CIoViewSyncExcel::OnAllSheetError( WPARAM w, LPARAM l )
{
	RemoveAllSyncBlock();
	UnInitialExcelResource(); // 释放资源
	return 1;
}

// 调用Excel线程初始化excel资源 - excel线程初始化资源后Post EUMT_ExcelAppInit到这里 通知其初始化结果
LRESULT CIoViewSyncExcel::OnExcelAppInit( WPARAM w, LPARAM l )
{
	bool32 bInitOK = (bool32)w;
	
	BlockArray	aFailBlocks;

	if ( bInitOK )	// 成功 - 将等待加入的加入
	{
		for ( int i=0; i < m_aWaitSyncBlocks.GetSize() ; i++ )
		{
			if ( CSyncExcelThread::Instance().AddSheet(m_aWaitSyncBlocks[i].m_StrName) && UpdateBlockData(m_aWaitSyncBlocks[i]) )
			{
				m_aAttendBlocks.Add(m_aWaitSyncBlocks[i]);
				AddAttendMerchs(m_aWaitSyncBlocks[i].m_aMerchs);
			}
			else
			{
				aFailBlocks.Add(m_aWaitSyncBlocks[i]);
			}
		}
	}
	else
	{
		aFailBlocks.Copy(m_aWaitSyncBlocks);
	}

	m_aWaitSyncBlocks.RemoveAll();		// 清空等待队列

	if ( aFailBlocks.GetSize() > 0 )
	{
		CString	StrMsg;
		StrMsg = _T("同步板块添加失败:\r\n");
		for ( int i=0; i < aFailBlocks.GetSize() ; i++ )
		{
			StrMsg += _T("    ") + aFailBlocks[i].m_StrName;
		}

		MessageBox(StrMsg, _T("同步失败"), MB_OK|MB_ICONWARNING);
	}

	return 1;
}

void CIoViewSyncExcel::UserManageSyncBlock()
{
	BlockArray aBlocks;
	CDlgSyncExcel dlg(AfxGetMainWnd());
	
	if ( dlg.SetSyncExcel(m_aAttendBlocks, aBlocks) )
	{
		ResetSyncBlockArray(aBlocks);
	}
}

void CIoViewSyncExcel::PostNcDestroy()
{
	// 防止ioview base中的delete this
	//NULL;
}

void CIoViewSyncExcel::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp || pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (const CMmiCommBasePlugIn *)pResp;
	
	switch ( pRespPlugin->m_eCommTypePlugIn )
	{
	case ECTPIRespMerchIndex:
	case ECTPIRespAddPushMerchIndex:
		{
			// 推送回包跟请求的回包一样的. 只是类型不同
			const CMmiRespMerchIndex *pRespIndex = (const CMmiRespMerchIndex *)pRespPlugin;
			for ( int i=0; i < pRespIndex->m_aMerchIndexList.GetSize() ; i++ )
			{
				CMerch *pMerch = NULL;
				const T_RespMerchIndex &merchIndex = pRespIndex->m_aMerchIndexList[i];
				if (m_pAbsCenterManager && (!m_pAbsCenterManager->GetMerchManager().FindMerch(merchIndex.m_StrCode, merchIndex.m_uiMarket, pMerch)))
				{
					continue;
				}
				
				m_mapMerchIndex[pMerch] = merchIndex;
				for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
				{
					if ( IsAttendMerchIndexData(m_aAttendBlocks[i]) )
					{
						UpdateBlockMerchData(m_aAttendBlocks[i], pMerch, true);
					}
				}
				//UpdateMerchData(pMerch, true);		// 对于选股标志请求，使用强制更新方式 - 有性能方面的损失
			}
			
		}
		break;
	default:
		break;
	}
}

void CIoViewSyncExcel::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
	if ( EPFTF10 == ePublicFileType  )	// F10数据更新
	{
		for ( int i=0; i < m_aAttendBlocks.GetSize() ; i++ )
		{
			if ( IsAttendFinanceData(m_aAttendBlocks[i]) )
			{
				UpdateBlockMerchData(m_aAttendBlocks[i], pMerch, true);
			}
		}
		//UpdateMerchData(pMerch, true);		// 对于财务数据，使用强制更新方式 - 有性能方面的损失
	}
}

bool32 CIoViewSyncExcel::IsAttendFinanceData(const T_Block &block)
{
	CArray<T_HeadInfo,T_HeadInfo> aHeaderInfos;
	if ( GetBlockHeaderList(aHeaderInfos, block.m_eHeadType) < 1 )
	{
		return false;
	}

	for ( int i=0; i < aHeaderInfos.GetSize() ; i++ )
	{
		if ( aHeaderInfos[i].m_eReportHeader >= CReportScheme::ERHAllCapital && aHeaderInfos[i].m_eReportHeader <= CReportScheme::ERHAllMarketValue )
		{
			return true;
		}
	}
	return false;
}

bool32 CIoViewSyncExcel::IsAttendFinanceData()
{
	for ( int32 i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		if ( IsAttendFinanceData(m_aAttendBlocks[i]) )
		{
			return true;
		}
	}
	return false;
}

bool32 CIoViewSyncExcel::IsAttendMerchIndexData(const T_Block &block)
{
	CArray<T_HeadInfo,T_HeadInfo> aHeaderInfos;
	if ( GetBlockHeaderList(aHeaderInfos, block.m_eHeadType) < 1 )
	{
		return false;
	}
	
	for ( int i=0; i < aHeaderInfos.GetSize() ; i++ )
	{
		if ( aHeaderInfos[i].m_eReportHeader >= CReportScheme::ERHShortLineSelect && aHeaderInfos[i].m_eReportHeader <= CReportScheme::ERHMidLineSelect )
		{
			return true;
		}
	}
	return false;
}

bool32 CIoViewSyncExcel::IsAttendMerchIndexData()
{
	for ( int32 i=0; i < m_aAttendBlocks.GetSize() ; i++ )
	{
		if ( IsAttendMerchIndexData(m_aAttendBlocks[i]) )
		{
			return true;
		}
	}
	return false;
}

// 
// BOOL CIoViewSyncExcel::OnBookBeforeClose( BOOL *pCancel )
// {
// 	if ( pCancel )
// 	{
// 		*pCancel = TRUE;
// 	}
// 	return TRUE;
// }

//////////////////////////////////////////////////////////////////////////
// 辅组同步
class CCSExcelLock
{
public:
	CCSExcelLock(CRITICAL_SECTION &cs, bool bLock=true)
	{
		m_pCS = &cs;
		m_bLock = false;
		if ( bLock )
		{
			Lock();
		}
	}
	CCSExcelLock(CRITICAL_SECTION *pcs, bool bLock=true)
	{
		ASSERT( NULL != pcs );
		m_pCS = pcs;
		m_bLock = false;
		if ( bLock )
		{
			Lock();
		}
	}
	~CCSExcelLock()
	{
		if ( m_bLock )
		{
			UnLock();
		}
	}

	void Lock()
	{
		if ( NULL != m_pCS )
		{
			if ( !m_bLock )
			{
				::EnterCriticalSection(m_pCS);
				m_bLock = true;
			}
		}
	}
	//lint -sem(CCSExcelLock::UnLock,cleanup)
	void UnLock()
	{
		if ( NULL != m_pCS )
		{
			if ( m_bLock )
			{
				::LeaveCriticalSection(m_pCS);
				m_bLock = false;
			}
			m_pCS = NULL;
		}
	}

protected:
private:
	bool m_bLock;
	CRITICAL_SECTION *m_pCS;
};

/////////////////////////////////////////////////////////////////////////////
// 单独线程处理excel数据插入 CSyncExcelThread
// CSynExcelThread

CString	CSyncExcelThread::m_sStrDlgFilePath;

IMPLEMENT_DYNCREATE(CSyncExcelThread, CWinThread)

CSyncExcelThread::CSyncExcelThread()
{
	m_nSheetErrorMsg = EUMT_ExcelSheetError;
	m_nAllSheetErrorMsg = EUMT_ExcelAllSheetError;
	m_hwndMsgOwner = NULL;

	m_bExcelSuspended	=	false;
	m_nExcelSuspendedWaitTime	=	0;

	::InitializeCriticalSection(&m_csLockCellsToSet);
	::InitializeCriticalSection(&m_csLockSheetCache);
	::InitializeCriticalSection(&m_csLockAsynStringParam);
	
	m_hEvtPumpMsgEnd = CreateEvent(NULL, FALSE, FALSE, NULL);	// 默认自动 - 消息未处理
	ASSERT( NULL != m_hEvtPumpMsgEnd );
	m_hEvtWaitMsgEnter = CreateEvent(NULL, FALSE, TRUE, NULL); // 默认自动 - 允许进入等待
	ASSERT( NULL != m_hEvtWaitMsgEnter );
}

CSyncExcelThread::~CSyncExcelThread()
{
	::DeleteCriticalSection(&m_csLockCellsToSet);
	::DeleteCriticalSection(&m_csLockSheetCache);
	::DeleteCriticalSection(&m_csLockAsynStringParam);
	
	DEL_HANDLE(m_hEvtWaitMsgEnter);
	DEL_HANDLE(m_hEvtPumpMsgEnd);
}

BOOL CSyncExcelThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	HRESULT hr = CoInitialize(NULL);
	ASSERT( SUCCEEDED(hr) );
	return TRUE;
}

int CSyncExcelThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	DoUninitializeExcel();

	CoUninitialize();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSyncExcelThread, CWinThread)
	ON_THREAD_MESSAGE(EUMT_AddSheet, OnAddSheet)
	ON_THREAD_MESSAGE(EUMT_CheckSheetExist, OnCheckSheetExist)
	ON_THREAD_MESSAGE(EUMT_InitializeExcel, OnIntializeExcelResource)
	ON_THREAD_MESSAGE(EUMT_SetCellValue, OnSetCellValue)
	ON_THREAD_MESSAGE(EUMT_CheckExcelStatus, OnCheckExcelStatus)
	ON_THREAD_MESSAGE(EUMT_ClearCellRange, OnClearCellRange)
	ON_THREAD_MESSAGE(EUMT_ShowExcelApp, OnShowExcelApp)
END_MESSAGE_MAP()

CSyncExcelThread *CSyncExcelThread::m_spExcelThread = NULL;
CSyncExcelThread & CSyncExcelThread::Instance()
{
	if ( NULL != m_spExcelThread )
	{
		DWORD dwExit = 0;
		if ( GetExitCodeThread(m_spExcelThread->m_hThread, &dwExit) && dwExit == STILL_ACTIVE )
		{
			return *m_spExcelThread;
		}
		// delete
		m_spExcelThread->m_bAutoDelete = TRUE;
		delete m_spExcelThread;
		m_spExcelThread = NULL;
	}
	if ( m_spExcelThread == NULL )
	{
		m_spExcelThread = (CSyncExcelThread *)AfxBeginThread(RUNTIME_CLASS(CSyncExcelThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_spExcelThread->m_bAutoDelete = FALSE;
		m_spExcelThread->ResumeThread();
	}

	ASSERT( NULL != m_spExcelThread );
	return *m_spExcelThread;
}

bool32 CSyncExcelThread::AddSheet( const CString &StrSheet )
{
	bool32 bRet = false;
	if ( GetCurrentThreadId() != m_nThreadID )
	{
		// 其它线程调用 - 简单处理同步
		SendSyncThreadMessage(EUMT_AddSheet, (WPARAM)(LPCTSTR)StrSheet, (LPARAM)&bRet);
		return bRet;
	}
	else
	{
		// 自线程调用
		ASSERT( 0 ); // 不应当调用
		bRet = DoAddSheet(StrSheet);
	}
	return bRet;
}

bool32 CSyncExcelThread::CheckSheetExist( const CString &StrSheet )
{
	bool32 bRet = false;
	if ( GetCurrentThreadId() != m_nThreadID )
	{
		// 其它线程调用 - 简单处理同步
		SendSyncThreadMessage(EUMT_CheckSheetExist, (WPARAM)(LPCTSTR)StrSheet, (LPARAM)&bRet);
		return bRet;
	}
	else
	{
		// 自线程调用
		ASSERT( 0 ); // 不应当调用
		bRet = DoCheckSheetExist(StrSheet);
	}
	return bRet;
}

bool32 CSyncExcelThread::DoAddSheet( LPCTSTR lpszSheet )
{
	if ( NULL == lpszSheet || _tcslen(lpszSheet) == 0 )
	{
		return false;
	}
	_Worksheet sheet;
	bool32 bIsCreate = false;
	bool bRet = GetExcelWorkSheet(lpszSheet, sheet, true, &bIsCreate);
	if ( bRet )
	{
		TRY 
		{
			OnShowExcelApp(0, 0);
			sheet.Activate();
		}
		CATCH_ALL(e)
		{
			
		}
		END_CATCH_ALL
		
		CCSExcelLock lockCache(&m_csLockSheetCache);
		if ( bIsCreate )
		{
			m_mapSheetCache[lpszSheet].clear();		// 添加&清空一个缓冲记录
		}
		else
		{
			m_mapSheetCache[lpszSheet]; // 如果没有的话，新加一个记录
		}
		lockCache.UnLock();
	}
	return bRet;
}

bool32 CSyncExcelThread::DoCheckSheetExist( LPCTSTR lpszSheet )
{
	if ( NULL == lpszSheet || _tcslen(lpszSheet) == 0 )
	{
		return false;
	}
	_Worksheet sheet;
	bool bRet = GetExcelWorkSheet(lpszSheet, sheet, false);		// 虽然有PRC_E_REJECT，但是这个调用是在添加同步板块前，所以只能返回失败了
	sheet.ReleaseDispatch();
	return bRet;
}

void CSyncExcelThread::CheckExcelStatus()
{
	PostThreadMessage(EUMT_CheckExcelStatus, 0, 0);
}

void CSyncExcelThread::SetCellValue( const T_CellValue &cell, bool32 bIgnoreCache /*= false*/ )
{
	// 保存该cell与ignore cache值，通知线程设置excel
	if ( !AppendCellValue(cell, bIgnoreCache) )
	{
		return;
	}
	PostThreadMessage(EUMT_SetCellValue, 0, 0);
}

void CSyncExcelThread::SetCellValue( const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache /*= false*/ )
{
	T_CellValue cell;
	cell.m_StrSheet = StrSheet;
	cell.m_nRow = nRow;
	cell.m_nCol = nCol;
	cell.m_StrValue = StrValue;
	cell.m_bUseColor = bUseColor;
	cell.m_clr = clr;
	SetCellValue(cell, bIgnoreCache);
}

void CSyncExcelThread::ClearCellRange(const CString &StrSheet, USHORT nRowCount, USHORT nColCount )
{
	DWORD dwKey = (DWORD)(LPCTSTR)StrSheet;
	SaveAsynStringParam(dwKey, StrSheet);
	PostThreadMessage(EUMT_ClearCellRange, dwKey,  MAKELPARAM(nColCount, nRowCount));
}

void CSyncExcelThread::InitializeExcelResource(HWND hwndMsgOwner, DWORD dwMsgSheetError/* = EUMT_ExcelSheetError*/, DWORD dwMsgAllSheetError /*= EUMT_ExcelAllSheetError*/)
{
	m_hwndMsgOwner = hwndMsgOwner;
	m_nSheetErrorMsg = dwMsgSheetError;
	m_nAllSheetErrorMsg = dwMsgAllSheetError;

	if ( GetCurrentThreadId() != m_nThreadID )
	{
		// 其它线程调用 - 简单处理同步 - 改为消息AppInit回送结果
		//SendSyncThreadMessage(EUMT_InitializeExcel, 0, (LPARAM)&bRet);
		//return bRet;
		PostThreadMessage(EUMT_InitializeExcel, 0, NULL);
	}
	else
	{
		ASSERT( 0 );
		// 自线程调用 - 应当调用Doxxx
		//bRet = DoInitializeExcelResource();
	}
	//return bRet;
	return;
}

void CSyncExcelThread::UninitializeExcelResource()
{
	// 外部调用 - 退出 调用释放资源
	if ( GetCurrentThreadId() != m_nThreadID )
	{
		m_bAutoDelete = FALSE;
		PostThreadMessage(WM_QUIT, 0, 0);	// 退出吧
		WaitForSingleObject(m_hThread, INFINITE);
		m_bAutoDelete = TRUE;
		Delete();
	}
}

void CSyncExcelThread::Delete()
{
	if ( m_spExcelThread == this )
	{
		m_spExcelThread = NULL;
	}
	CWinThread::Delete();
}

void CSyncExcelThread::SendSyncThreadMessage( DWORD dwMsg, WPARAM w, LPARAM lpResult )
{
	ASSERT( GetCurrentThreadId() != m_nThreadID ); // 不同线程调用

	DWORD dw = WaitForSingleObject(m_hEvtWaitMsgEnter, INFINITE);
	TRY 
	{
		if ( dw == WAIT_OBJECT_0 )
		{
			m_msgSync.hwnd = NULL;
			m_msgSync.wParam = w;
			m_msgSync.message = dwMsg;
			m_msgSync.lParam = lpResult;
			ResetEvent(m_hEvtPumpMsgEnd);

			bool bPost = true;
			if ( !PostThreadMessage(dwMsg, w, lpResult) )
			{
				Sleep(10);
				if ( !PostThreadMessage(dwMsg, w, lpResult) )
				{
					bPost = false;
				}
			}

			if ( bPost )
			{
				dw = WaitForSingleObject(m_hEvtPumpMsgEnd, INFINITE);
				//if ( dw != WAIT_OBJECT_0 )
				//{
				//	m_lresult = 0;	// 简单错误处理
				//}
			}
		}
	}
	CATCH_ALL(e)
	{
		TRACE(_T("SendSyncThreadMessage fail\r\n"));
		ASSERT( 0 );
	}
	END_CATCH_ALL

	SetEvent(m_hEvtWaitMsgEnter);	// 完成消息等待

	return;
}

BOOL CSyncExcelThread::PumpMessage()
{
	BOOL b;

	BOOL bRet = CWinThread::PumpMessage();

	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	
	if ( m_msgSync.message == pState->m_msgCur.message && 
		 m_msgSync.wParam == pState->m_msgCur.wParam &&
		 m_msgSync.lParam == pState->m_msgCur.lParam &&
		 m_msgSync.hwnd == pState->m_msgCur.hwnd ) // 消息相等 - 简单判断
	{
		b = SetEvent(m_hEvtPumpMsgEnd);
		ASSERT( b );
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// Excel处理
bool32 CSyncExcelThread::DoInitializeExcelResource()
{
	TRY 
	{
		if ( NULL != m_sExcelWorkSheets.m_lpDispatch )
		{
			TRY 
			{
				m_sExcelWorkSheets.GetCount();		// 查看是否已经被关闭了 - 如果关闭，这个调用会异常
				return true;	// 以前的还可以用
			}
			CATCH_ALL(e)
			{
				SCODE code = 0;
				if ( e->IsKindOf(RUNTIME_CLASS(COleException)) )
				{
					code = ((COleException *)e)->m_sc;
				}
				else if ( e->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
				{
					code = ((COleDispatchException *)e)->m_scError;
				}

				// 无法获取该sheet - 该sheet出现异常 - 该异常有可能是RPC_E_CALL_REJECTED，需要等待excel能够接受数据才能开始数据传输
				// 此判断条件暂时没想到合适的修改，先屏蔽掉此错误
				//lint --e(650)
				if (/* RPC_E_CALL_REJECTED ==  code ||*/ VBA_E_IGNORE == code )
				{ // 对于正忙的错误，选择提示失败，但是不释放以前的资源
					MessageBox(GetForegroundWindow(), _T("Excel正处于不能响应通信状态, 请尝试:\r\n")
									 _T("  1.取消Excel中正在进行的编辑操作\r\n")
									 _T("  2.关闭Excel中弹出的对话框"), _T("Excel正忙..."), MB_OK);
					return false;
				}
			}
			END_CATCH_ALL
		}
		
		DoUninitializeExcel();		// 释放以前的资源
		ASSERT( NULL == m_sExcelWorkSheets.m_lpDispatch );
		return NULL != GetExcelWorkSheets(); // 创建新的资源 - 如果以前有关注的sheet，需要通知ioview sheet已经改变 OR 在新创建的资源上重新同步？？ - 未实现
	}
	CATCH_ALL(eAll)
	{
		TRACE(_T("Excel initial excel resource fail\r\n"));
		return false;
	}
	END_CATCH_ALL
		
	ASSERT( 0 );
	return false;
}

void CSyncExcelThread::DoUninitializeExcel()
{
	TRY
	{
		// 释放excel资源
		if ( NULL != m_sExcelWorkBook.m_lpDispatch )
		{
			TRY 
			{
				m_sExcelWorkBook.Save();  // 尝试保存关闭
				m_sExcelWorkBook.Close(_variant_t(false), vtMissing, vtMissing);

				if ( NULL != m_sExcelApp.m_lpDispatch )	// 关闭后隐藏
				{
					TRY 
					{
						m_sExcelApp.SetVisible(FALSE);
						m_sExcelApp.SetUserControl(FALSE);
					}
					CATCH_ALL(eApp1)
					{
						TRACE(_T("Excel app disappear fail\r\n"));
					}
					END_CATCH_ALL
				}
			}
			CATCH_ALL(e)
			{
				TRACE(_T("Excel save error\r\n"));
			}
			END_CATCH_ALL
		}

		m_sExcelWorkSheets.ReleaseDispatch();
		m_sExcelWorkBook.ReleaseDispatch();
		m_sExcelWorkBooks.ReleaseDispatch();
		
		if ( NULL != m_sExcelApp.m_lpDispatch )
		{
			TRY 
			{
				m_sExcelApp.Quit();
			}
			CATCH_ALL(e)
			{
				TRACE(_T("Excel quit error\r\n"));
				TRY 
				{
					m_sExcelApp.SetVisible(TRUE);		// 如果不能退出 - 尝试将控制权交给用户
					m_sExcelApp.SetUserControl(TRUE);
				}
				CATCH_ALL(eSub)
				{
					TRACE(_T("Excel quit set to user fail\r\n"));
				}
				END_CATCH_ALL
			}
			END_CATCH_ALL
		}
		
		m_sExcelApp.ReleaseDispatch();
	}
	CATCH_ALL(e)
	{
		TRACE(_T("Excel error in uninitialize \r\n"));
	}
	END_CATCH_ALL
}

_Application * CSyncExcelThread::GetExcelApplication()
{
	if ( NULL == m_sExcelApp.m_lpDispatch )
	{
		COleException e;
		if ( !m_sExcelApp.CreateDispatch(_T("Excel.Application"), &e) )
		{
#ifdef DEBUG
			ASSERT( 0 );
#else
			MessageBox(GetActiveWindow(), _T("启动Excel程序失败."), L"Excel启动失败", MB_ICONWARNING);
#endif
		}
		//else
		//{
		//	m_sExcelApp.SetUserControl(FALSE);
		//	m_sExcelApp.SetVisible(TRUE);
		//}
	}
	if ( NULL != m_sExcelApp.m_lpDispatch )
	{
		return &m_sExcelApp;
	}
	return NULL;
}

Workbooks* CSyncExcelThread::GetExcelWorkBooks()
{
	_Application *pExcelApp = GetExcelApplication();
	if ( NULL != pExcelApp )
	{
		if ( NULL == m_sExcelWorkBooks.m_lpDispatch )
		{
			m_sExcelWorkBooks.AttachDispatch(pExcelApp->GetWorkbooks());
		}
	}
	if ( NULL != m_sExcelWorkBooks.m_lpDispatch )
	{
		return &m_sExcelWorkBooks;
	}
	ASSERT( 0 );
	return NULL;
}

_Workbook* CSyncExcelThread::GetExcelWorkBook()
{
	_Application *pExcelApp = GetExcelApplication();
	Workbooks    *pWorkBooks = GetExcelWorkBooks();

	if ( NULL == pExcelApp || NULL == pWorkBooks )
	{
		return NULL;	// 无法完成同步操作
	}

	if ( NULL != m_sExcelWorkBook.m_lpDispatch )
	{
		return &m_sExcelWorkBook;	// 已经初始化好了
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	// 请用户选择一个合适的文件同步，不默认打开任何文件
	// 备份当前路径
	CString StrFileName(_T("同步到Excel数据"));
	CString StrFilePath = CPathFactory::GetPrivateExcelPath(pDoc->m_pAbsCenterManager->GetUserName());
	StrFilePath += StrFileName;	
	StrFilePath += L".xls";

	// 得到Excel 文件夹路径
	TCHAR TStrFilePath[MAX_PATH];
	lstrcpy(TStrFilePath, StrFilePath);
	_tcheck_if_mkdir(TStrFilePath);

	// 判断文件是否存在,不在就新建
	bool32 bFileExist = true;
	if ( -1 == _taccess(StrFilePath,0) )
	{			
		bFileExist = false;
	}

	// 备份当前路径
	TCHAR TStrFilePathBk[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1, TStrFilePathBk);
	
	TRY 
	{
		if ( bFileExist )
		{
			// 获取用户选择的文件路径 - 有可能是不存在的文件
			TRY{
				// 设置为Excel 路径
				::SetCurrentDirectory(TStrFilePath);
				
				CString StrExcelPath(TStrFilePath);
				StrExcelPath.Replace(_T('/'), _T('\\'));
				CFileDialog	dlg(TRUE, _T("xls"), StrFileName+_T(".xls"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Microsoft Office Excel工作簿(*.xls)|*.xls||"),NULL);
				if(GetVersion()<0x80000000)
				{
					dlg.m_ofn.lStructSize=88;
				}
				else
				{
					dlg.m_ofn.lStructSize=76;
				}
				if ( m_sStrDlgFilePath.IsEmpty() )
				{
					m_sStrDlgFilePath = StrExcelPath;
				}
				dlg.m_ofn.lpstrInitialDir = m_sStrDlgFilePath;
				
				StrFilePath.Empty();
				if ( dlg.DoModal() == IDOK )
				{
					StrFilePath = dlg.GetPathName();
					int iFileNameStart = StrFilePath.GetLength() - dlg.GetFileName().GetLength();
					m_sStrDlgFilePath = StrFilePath.Left(iFileNameStart);
				}
			}
			CATCH_ALL(e)
			{
				
			}
			END_CATCH_ALL
			// 还原当前路径
			::SetCurrentDirectory(TStrFilePathBk);
		}
		
		// 最后验证文件是否存在
		CFileFind findFile;
		if ( !findFile.FindFile(StrFilePath) && !StrFilePath.IsEmpty() )
		{
			// 不存在 - 创建新的excel文件
			//m_sExcelWorkBook.AttachDispatch(pExcelApp->GetNewWorkbook());
			m_sExcelWorkBook.AttachDispatch(pWorkBooks->Add(vtMissing));
			ASSERT( m_sExcelWorkBook.m_lpDispatch != NULL );
			// 使用绝对文件路径
			CString StrTmp(StrFilePath);
			StrTmp.TrimLeft();
			CString StrAbsPath(TStrFilePathBk);
			if ( StrTmp.Left(1) == _T(".") )
			{
				if ( StrAbsPath.Right(1) != _T("\\") )
				{
					StrAbsPath += _T("\\");
				}
				StrTmp.Replace(_T("/"), _T("\\"));
				StrAbsPath += StrTmp.Mid(StrTmp.Find(_T("\\"))+1); // 将.相对路径变成绝对路径
				StrFilePath = StrAbsPath;
			}
			else
			{
				StrAbsPath = StrFilePath;	// 不是相对路径吗？
			}
			m_sExcelWorkBook.SaveAs(_variant_t(StrAbsPath), vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, 0, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing);
		}
		else if ( StrFilePath.IsEmpty() )
		{
			return NULL;		// 没有选择同步文件
		}
		else
		{	// 打开
			m_sExcelWorkBook.AttachDispatch(m_sExcelWorkBooks.Open(StrFilePath, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, _variant_t(true), vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing));
			ASSERT( NULL != m_sExcelWorkBook.m_lpDispatch );
		}
		
	}
	CATCH_ALL(eAll)
	{
		CString StrErr;
		if ( eAll )
		{
			eAll->GetErrorMessage(StrErr.GetBuffer(1024), 1024);
			StrErr.ReleaseBuffer();
		}
		m_sExcelWorkBook.ReleaseDispatch();
		CString StrTmp;
		StrTmp.Format(_T("初始化Excel文件[%s]失败：%s"), StrFilePath.GetBuffer(), StrErr.GetBuffer());
		::MessageBox(NULL, StrTmp, _T("错误"), MB_OK | MB_ICONWARNING);
	}
	END_CATCH_ALL
	
	
	if ( NULL != m_sExcelWorkBook.m_lpDispatch )
	{
		// 建立关闭连接点 - 以后实现
		//m_sExcelWorkBook.m_lpDispatch->QueryInterface()
		
		return &m_sExcelWorkBook;
	}

//	if ( NULL != pExcelApp && !StrFilePath.IsEmpty() )
//	{
//#ifndef DEBUG
//		::MessageBox(NULL, _T("打开Excel文件[") + StrFilePath + _T("]失败！"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
//#endif
//	}
	return NULL;
}

Worksheets* CSyncExcelThread::GetExcelWorkSheets()
{
	if ( NULL == m_sExcelWorkSheets.m_lpDispatch )
	{
		_Workbook *pBook = GetExcelWorkBook();
		if ( NULL != pBook )
		{
			m_sExcelWorkSheets.AttachDispatch( pBook->GetWorksheets() );
		}
	}

	if ( NULL != m_sExcelWorkSheets.m_lpDispatch )
	{
		return &m_sExcelWorkSheets;
	}
	
	return NULL;
}

Worksheets* CSyncExcelThread::GetExcelExistWorkSheets()
{
	if ( NULL != m_sExcelWorkSheets.m_lpDispatch )
	{
		return &m_sExcelWorkSheets;
	}
	
	return NULL;
}

bool CSyncExcelThread::GetExcelWorkSheet( const CString &StrBlock, OUT _Worksheet &sheet, bool bCanCreate/* = false*/, bool32 *pbIsCreate/* = NULL*/, SCODE *pResult/* = NULL*/ )
{
	if ( StrBlock.IsEmpty() )
	{
		return false;
	}

	if ( NULL != pResult )
	{
		*pResult = 0;
	}

	if ( NULL != pbIsCreate )
	{
		*pbIsCreate = false;
	}

	TRY 
	{
		// 从打开的里面查找
		Worksheets *pSheets = GetExcelExistWorkSheets();
		if ( NULL == pSheets )
		{
			return false;
		}
		
		long lCount = 0;
		TRY 
		{
			lCount = pSheets->GetCount();	// 有可能这个Sheets已经被用户从外面关闭掉了
		}
		CATCH_ALL(eSub)
		{
			SCODE code = 0;
			if ( eSub->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
			{
				COleDispatchException *pe = (COleDispatchException *)eSub;
				code = pe->m_scError;
			}
			else if ( eSub->IsKindOf(RUNTIME_CLASS(COleException)) )
			{
				COleException *pe = (COleException *)eSub;
				code = pe->m_sc;
			}

			if ( NULL != pResult )
			{
				*pResult = code;
			}

			// 此判断条件暂时没想到合适的修改，先屏蔽掉此错误
			//lint --e(650)
			if (/* code != RPC_E_CALL_REJECTED &&*/ code != VBA_E_IGNORE )	// 忙 - 不需要释放资源
			{
				DoUninitializeExcel();		// 可能被关闭了或者出现其它异常 - 释放所有的资源 - 还需要通知instance取消所有block
			}
			return false;		// 不能创建，返回失败
		}
		END_CATCH_ALL
		for ( long i=0; i < lCount ; i++ )
		{
			_Worksheet sheetTmp;
			bool bContinue =false;
			TRY 
			{
				sheetTmp.AttachDispatch(pSheets->GetItem(_variant_t((long)(i+1))));
			}
			CATCH_ALL(eSheet)
			{
				if ( NULL != pResult )		// 记录最后一个异常的code
				{
					if ( eSheet->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
					{
						COleDispatchException *pe = (COleDispatchException *)eSheet;
						*pResult = pe->m_scError;
					}
					else if ( eSheet->IsKindOf(RUNTIME_CLASS(COleException)) )
					{
						COleException *pe = (COleException *)eSheet;
						*pResult = pe->m_sc;
					}
				}
				bContinue = true;
			}
			END_CATCH_ALL

			if ( bContinue )
			{
				continue;
			}

			if ( NULL != sheetTmp.m_lpDispatch )
			{
				if ( sheetTmp.GetName() == StrBlock )	// 选取同名的sheet - block的名字很重要, 小心重复
				{
					if ( NULL != m_sExcelApp.m_lpDispatch && !m_sExcelApp.GetVisible() )
					{
						m_sExcelApp.SetVisible(TRUE);
						sheet.Activate();	// 是第一次就要激活当前sheet
					}
					sheet = sheetTmp;	// 已经AddRef了
					//sheet.Activate();  // 已经在的就不需要active
					return true;
				}
			}
		}

		if ( bCanCreate )
		{ // 创建新的sheet
			sheet.AttachDispatch(pSheets->Add(vtMissing, vtMissing, vtMissing, vtMissing));
			ASSERT( NULL != sheet.m_lpDispatch );
			if ( NULL != sheet.m_lpDispatch )
			{
				if ( NULL != m_sExcelApp.m_lpDispatch && !m_sExcelApp.GetVisible() )
				{
					m_sExcelApp.SetVisible(TRUE);
				}
				sheet.Activate();
				sheet.SetName(StrBlock);
				if ( NULL != pbIsCreate )
				{
					*pbIsCreate = true;
				}
				return true;
			}
		}
	}
	CATCH_ALL(e)
	{
		SCODE code = 0;
		if ( e->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
		{
			COleDispatchException *pe = (COleDispatchException *)e;
			code = pe->m_scError;
		}
		else if ( e->IsKindOf(RUNTIME_CLASS(COleException)) )
		{
			COleException *pe = (COleException *)e;
			code = pe->m_sc;
		}
		
		if ( NULL != pResult )
		{
			*pResult = code;
		}
		
		CString StrErr;
		e->GetErrorMessage(StrErr.GetBuffer(1024), 1024);
		StrErr.ReleaseBuffer();
		TRACE(_T("Sheet get Error: %s\r\n"), StrErr.GetBuffer());
	}
	END_CATCH_ALL
	
	ASSERT( !bCanCreate || NULL != sheet.m_lpDispatch );
	
	return false;
}

bool32 CSyncExcelThread::GetExcelCellPosString( UINT iRow, UINT iCol, CString &StrOut )
{
	StrOut.Empty();
	if (iRow < 1 || iCol < 1)
	{
		return false;
	}
	else
	{
		iCol--;		// 列调整为0起点
		// 1,1 = A1
		// 行 1...n 列 A-Z AA-AZ BA-BZ .. ZA-ZZ AAA-AAZ...
		// 由于目前excel不会出现到IV以上，so使用的是修改的取值算法
		CString StrCol;
		UINT iColQuo = iCol;
		UINT iColRem;
		do 
		{
			iColRem = iColQuo%26;
			iColQuo = iColQuo/26;
			if ( iColQuo > 0 || StrCol.IsEmpty() )
			{
				StrCol.Insert(0, _T('A') + iColRem);
			}
			else	// ==0 && strcol not empty 代表10位以上的权值位, 即最大权值位, 最大权值位比>=1, 且用A代替1, excel不会出现26*25列以上
			{
				StrCol.Insert(0, _T('A') + iColRem -1);
			}
		} while ( iColQuo > 0 );
		
		StrOut.Format(_T("%s%d"), StrCol.GetBuffer(), iRow);
		return true;
	}	
}

// CSynExcelThread 消息处理程序
void CSyncExcelThread::OnAddSheet( WPARAM w, LPARAM l )
{
	bool32 bRet = DoAddSheet((LPCTSTR)w);
	if ( l != NULL )
	{
		*(bool32 *)l = bRet;
	}
}

void CSyncExcelThread::OnCheckSheetExist( WPARAM w, LPARAM l )
{
	bool32 bRet = DoCheckSheetExist((LPCTSTR)w);
	if ( l != NULL )
	{
		*(bool32 *)l = bRet;
	}
}

void CSyncExcelThread::OnCheckExcelStatus( WPARAM w, LPARAM l )
{
	bool32 bRet = DoCheckExcelStatus();	
	if ( l != NULL )
	{
		*(bool32 *)l = bRet;
	}
}

void CSyncExcelThread::OnIntializeExcelResource( WPARAM w, LPARAM l )
{
	bool32 bRet = DoInitializeExcelResource();
	if ( l != NULL )
	{
		*(bool32 *)l = bRet;
	}

	if ( IsWindow(m_hwndMsgOwner) )
	{
		PostMessage(m_hwndMsgOwner, EUMT_ExcelAppInit, (WPARAM)bRet, NULL);
	}
}

void CSyncExcelThread::OnSetCellValue( WPARAM/* w*/, LPARAM/* l*/ )
{
	if ( m_bExcelSuspended )
	{
		if ( timeGetTime() - m_nExcelSuspendedWaitTime < KExcelSuspendedWait )
		{
			return;		// 还在等待excel能接受数据
		}
	}
	// 具体设置excel单元格
	SheetMap mapSheet;
	PeekNeedUpdateCellValue(mapSheet);

	if ( mapSheet.empty() )
	{
		return;
	}

	//TRACE(_T("SetCell[%d]sheet\r\n"), mapSheet.size());

	int iUpdatedSheet = 0;
	SCODE code = 0;

	for ( SheetMap::iterator itSheet = mapSheet.begin(); itSheet != mapSheet.end(); ++itSheet )
	{
		//TRACE(_T("SetCell[%d]cell\r\n"), itSheet->second.size());
		//bool32 bContinue = false;
		code = 0;
		TRY 
		{
			_Worksheet sheet;
			if ( !GetExcelWorkSheet(itSheet->first, sheet, false, NULL, &code) )
			{
				// 无法获取该sheet - 该sheet出现异常 - 该异常有可能是RPC_E_CALL_REJECTED，需要等待excel能够接受数据才能开始数据传输
				if ( RPC_E_CALL_REJECTED !=  code )
				{
					DoSheetError(itSheet->first);
				}
				else
				{
					// 将更新失败的数据还给更新区 OR 让数据丢失？
					RestoreToUpdate(itSheet);
				}
				//bContinue = true;
			}
			else
			{
				// 更新该sheet下所有需要更新的sheet
				CString StrCellPos;
				for ( CellMap::iterator itCell = itSheet->second.begin(); itCell != itSheet->second.end(); ++itCell )
				{
					if ( itCell->second.IsValid() )
					{
						bool32 bNew = false;
						GetExcelCellPosString(itCell->second.m_nRow, itCell->second.m_nCol, StrCellPos);
						if ( itCell->second.m_nCol > 'Z'-'A' )
						{
							TRACE(_T("Setcell[%s] : %s\r\n"), StrCellPos.GetBuffer(), itCell->second.GetDebugString().GetBuffer());
						}
						//TRACE(_T("Setcell : %s\r\n"), itCell->second.GetDebugString());
						if ( IsCellNeedUpdate(itCell->second, &bNew) )
						{
							Range excelCell(sheet.GetRange(_variant_t(StrCellPos), vtMissing));
							if ( bNew )	// 新cell需要清空原来excel的值
							{
								excelCell.Clear();
							}
							excelCell.SetItem(_variant_t((long)1), _variant_t((long)1), _variant_t(itCell->second.m_StrValue));

							if ( itCell->second.m_bUseColor )
							{
								FontExecl excelFont(excelCell.GetFont());
								excelFont.SetColor(_variant_t((long)itCell->second.m_clr));
							}
							CacheCell(itCell->second);
						}
					}
					else
					{
						ASSERT( 0 );
					}
				}
				iUpdatedSheet++;
			}
		}
		CATCH_ALL(eSub)
		{
			code = 0;
			if ( eSub->IsKindOf(RUNTIME_CLASS(COleException)) )
			{
				COleException *pe = (COleException *)eSub;
				code = pe->m_sc;
			}
			else if ( eSub->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
			{
				COleDispatchException *pe = (COleDispatchException *)eSub;
				code = pe->m_scError;
			}

			if ( RPC_E_CALL_REJECTED != code && VBA_E_IGNORE != (DWORD)code )		// 800AC472 = VBA_E_IGNORE，excel不能响应该数据请求
			{
				DoSheetError(itSheet->first); // 是否需要视为sheet异常？？
			}
			else
			{
				// 还给更新区 OR 丢失数据？
				RestoreToUpdate(itSheet);
			}
		}
		END_CATCH_ALL
	}
	
	if ( 0 == iUpdatedSheet && RPC_E_CALL_REJECTED != code && VBA_E_IGNORE != (DWORD)code )
	{
		// 没有一个sheet得到更新 - 检查excel状态，如果出现异常，则通知
		DoCheckExcelStatus();
	}
	else if ( 0 == iUpdatedSheet )
	{
		// 这两个异常需要将状态设置为等待excel
		m_bExcelSuspended = true;
		m_nExcelSuspendedWaitTime = timeGetTime();
	}
	else
	{
		m_bExcelSuspended = false;
	}
}

bool32 CSyncExcelThread::AppendCellValue( const T_CellValue &cell, bool32 bIgnoreCache )
{
	if ( !cell.IsValid() )
	{
		return false;
	}

	CCSExcelLock lockCache(&m_csLockSheetCache, true);	
	SheetMap::iterator itSheet = m_mapSheetCache.find(cell.m_StrSheet);
	bool bHasSheet = itSheet != m_mapSheetCache.end();
	lockCache.UnLock();
	if ( !bHasSheet )
	{
		return false;	// 缓存区中无sheet信息，则不存在该sheet
	}

	CCSExcelLock lockCell(&m_csLockCellsToSet, true);
	DWORD dwCellIndex = MakeCellIndex(cell.m_nRow, cell.m_nCol);
	m_mapCellsToSet[cell.m_StrSheet][dwCellIndex] = cell;
	m_mapCellsToSet[cell.m_StrSheet][dwCellIndex].m_bIgnoreCache = bIgnoreCache;

	//TRACE(_T("Append cell[%d-%ud]: %s\r\n"), m_mapCellsToSet[cell.m_StrSheet].size(), dwCellIndex, cell.GetDebugString());
	lockCell.UnLock();

	return true;
}

void CSyncExcelThread::RestoreToUpdate( SheetMap::iterator itSheet )
{
	CCSExcelLock lockCell(&m_csLockCellsToSet, true);

	SheetMap::iterator itSetSheet = m_mapCellsToSet.find(itSheet->first);
	if ( itSetSheet == m_mapCellsToSet.end() )
	{
		m_mapCellsToSet.insert(*itSheet);		// 不存在该内容 - 直接添加
	}
	else
	{		// 存在内容，则选择没有的添加
		for ( CellMap::iterator itCell = itSheet->second.begin(); itCell != itSheet->second.end() ; ++itCell )
		{
			DWORD dwIndex = MakeCellIndex(itCell->second.m_nRow, itCell->second.m_nCol);
			if ( 0 == itSetSheet->second.count(dwIndex) )
			{
				itSetSheet->second.insert(*itCell);
			}
		}
	}
	
	//TRACE(_T("Restore cell[%s]: %d\r\n"), itSheet->first, itSheet->second.size());
	lockCell.UnLock();
}

void CSyncExcelThread::PeekNeedUpdateCellValue( OUT SheetMap &sheetData )
{
	CCSExcelLock lockCell(&m_csLockCellsToSet, true);
	sheetData = m_mapCellsToSet;
	m_mapCellsToSet.clear();
	lockCell.UnLock();
}

DWORD CSyncExcelThread::MakeCellIndex( USHORT nRow, USHORT nCol )
{
	ASSERT( nRow > 0 && nCol > 0 ); // HI:Y(row) LO:X(col)
	return (((DWORD)nRow)<<16) | (nCol&0xffff);
}

void CSyncExcelThread::SaveAsynStringParam( DWORD dwKey, LPCTSTR lpszStr )
{
	CCSExcelLock lockParam(&m_csLockAsynStringParam);
	CString Str;
	if ( NULL != lpszStr )
	{
		Str = lpszStr;
	}
	m_mapAsyncStringParam[dwKey] = Str;
	lockParam.UnLock();
}

bool32 CSyncExcelThread::ReadAsynStringParam( DWORD dwKey, CString &StrParam )
{
	bool32 bOk =false;
	CCSExcelLock lockParam(&m_csLockAsynStringParam);
	if ( m_mapAsyncStringParam.count(dwKey) )
	{
		StrParam = m_mapAsyncStringParam[dwKey];
		m_mapAsyncStringParam.erase(dwKey);
		bOk = true;
	}
	lockParam.UnLock();
	return bOk;
}

bool32 CSyncExcelThread::IsCellNeedUpdate( const T_CellValue &cell, OUT bool32 *pCellIsNew )
{
	if ( !cell.IsValid() )
	{
		return false;	// 非法cell，不要update
	}

	CCSExcelLock lockCache(&m_csLockSheetCache, true);
	bool32 bUpdate = false;
	SheetMap::iterator itSheet = m_mapSheetCache.find(cell.m_StrSheet);
	if ( itSheet != m_mapSheetCache.end() )
	{
		DWORD dwKey = MakeCellIndex(cell.m_nRow, cell.m_nCol);
		CellMap::iterator itCell = itSheet->second.find(dwKey);
		if ( itCell != itSheet->second.end() )
		{
			if ( NULL != pCellIsNew )
			{
				*pCellIsNew = false;
			}
			// 比较
			if ( cell.m_bIgnoreCache || cell != itCell->second )
			{
				bUpdate = true;
			}
		}
		else
		{
			if ( NULL != pCellIsNew )
			{
				*pCellIsNew = true;
			}
			bUpdate = true;
		}
	}
	lockCache.UnLock();
	return bUpdate;
}

void CSyncExcelThread::CacheCell( const T_CellValue &cell )
{
	if ( !cell.IsValid() )
	{
		return;
	}

	CCSExcelLock lockCache(&m_csLockSheetCache);
	SheetMap::iterator itSheet = m_mapSheetCache.find(cell.m_StrSheet);
	if ( itSheet != m_mapSheetCache.end() )
	{
		DWORD dwKey = MakeCellIndex(cell.m_nRow, cell.m_nCol);
		itSheet->second[dwKey] = cell;
	}
	lockCache.UnLock();

	//TRACE(_T("Cache cell: %s\r\n"), cell.GetDebugString());
}

void CSyncExcelThread::RemoveCacheSheet( const CString &StrSheet )
{
	CCSExcelLock lockCache(&m_csLockSheetCache);
	m_mapSheetCache.erase(StrSheet);
	lockCache.UnLock();
}

void CSyncExcelThread::DoSheetError( const CString &StrSheet )
{
	// 移除该sheet缓存和更新数据
	CCSExcelLock lockCell(&m_csLockCellsToSet);
	m_mapCellsToSet.erase(StrSheet);
	lockCell.UnLock();

	CCSExcelLock lockCache(&m_csLockSheetCache);
	m_mapSheetCache.erase(StrSheet);
	lockCache.UnLock();

	// 通知消息窗口
	SendMessage(m_hwndMsgOwner, m_nSheetErrorMsg, (WPARAM)(LPCTSTR)StrSheet, 0);
}

void CSyncExcelThread::DoAllSheetError()
{
	// 清除缓存与更新
	CCSExcelLock lockCell(&m_csLockCellsToSet);
	m_mapCellsToSet.clear();
	lockCell.UnLock();

	CCSExcelLock lockCache(&m_csLockSheetCache);
	m_mapSheetCache.clear();
	lockCache.UnLock();

	// 释放excel资源
	DoUninitializeExcel();
	
	// 通知消息窗口
	PostMessage(m_hwndMsgOwner, m_nAllSheetErrorMsg, 0, 0);
}

bool32 CSyncExcelThread::DoCheckExcelStatus()
{
	// 检查所有的block - sheet
	bool32 bOk = true;
	CStringArray aSheets;
	CCSExcelLock lockCache(&m_csLockSheetCache);
	for ( SheetMap::iterator itSheet = m_mapSheetCache.begin() ; itSheet != m_mapSheetCache.end() ; ++itSheet )
	{
		aSheets.Add(itSheet->first);
	}
	lockCache.UnLock();

	int iExist = 0;
	SCODE code = 0;
	for ( int i=0; i < aSheets.GetSize() ; i++ )
	{
		bool32 bSheetOK = false;
		TRY 
		{
			_Worksheet sheet;
			code = 0;
			if ( GetExcelWorkSheet(aSheets[i], sheet, false, NULL, &code) )
			{
				sheet.ReleaseDispatch();
				iExist++;
				bSheetOK = true;
			}
		}
		CATCH_ALL(e)
		{
			//NULL;	// 这里不会有异常导致到这里
		}
		END_CATCH_ALL

		if ( !bSheetOK && RPC_E_CALL_REJECTED != code )	// 非excel忙错误
		{
			DoSheetError(aSheets[i]);
		}

		if ( !bSheetOK )
		{
			bOk =false;
		}
	}
	
	if ( 0 == iExist )
	{		
		// 检查是否存在excel - 如果不可见，则释放excel资源
		TRY 
		{
			if ( NULL != m_sExcelApp.m_lpDispatch )
			{
				if ( !m_sExcelApp.GetVisible() )
				{
					DoAllSheetError();
					bOk = false;
				}
			}
		}
		CATCH_ALL(e)
		{
			code = 0;
			if ( e->IsKindOf(RUNTIME_CLASS(COleDispatchException)) )
			{
				COleDispatchException *pe = (COleDispatchException *)e;
				code = pe->m_scError;
			}
			else if ( e->IsKindOf(RUNTIME_CLASS(COleException)) )
			{
				COleException *pe = (COleException *)e;
				code = pe->m_sc;
			}
			
			if ( RPC_E_CALL_REJECTED != code )
			{
				DoAllSheetError();
			}
			bOk = false;
		}
		END_CATCH_ALL
	}
	else
	{
		// 有excel sheet可用了 - 判断是否有忙时未更新的数据
		CCSExcelLock lockCell(&m_csLockCellsToSet);
		if ( !m_mapCellsToSet.empty() )
		{
			MSG msg;
			if ( !PeekMessage(&msg, (HWND)-1, EUMT_SetCellValue, EUMT_SetCellValue, PM_NOREMOVE) )
			{
				PostThreadMessage(EUMT_SetCellValue, 0, 0);	// 没有更新消息的话，发出更新消息
			}
		}
		lockCell.UnLock();
	}

	return bOk;
}

void CSyncExcelThread::OnClearCellRange( WPARAM w, LPARAM l )
{
	// LO:col HI:row
	CString StrSheet;
	if ( !ReadAsynStringParam((DWORD)w, StrSheet) )
	{
		return;
	}
	USHORT nRowCount = (USHORT)((l >> 16) & 0xffff);
	USHORT nColCount = (USHORT)(l & 0xffff);
	if ( nRowCount < 1 || nColCount < 1 )
	{
		return;
	}

	TRY 
	{
		_Worksheet sheet;
		if ( !GetExcelWorkSheet(StrSheet, sheet, false, NULL, NULL) )
		{
			// 不处理
		}
		else
		{
			// 清空指定范围数据
			CString StrLeftTop;
			CString StrRightBottom;
			GetExcelCellPosString(1, 1, StrLeftTop);
			GetExcelCellPosString(nRowCount, nColCount, StrRightBottom); // 含右底
			Range	excelRange(sheet.GetRange(_variant_t(StrLeftTop), _variant_t(StrRightBottom)));
			excelRange.Clear();
			
			// 如果没出错，开始清空缓存
			CCSExcelLock lockCache(&m_csLockSheetCache);
			SheetMap::iterator itSheet = m_mapSheetCache.find(StrSheet);
			if ( itSheet != m_mapSheetCache.end() )
			{
				for ( CellMap::iterator itCell = itSheet->second.begin(); itCell != itSheet->second.end() ; ++itCell )
				{
					itCell->second.m_StrValue.Empty();	// 清空原来的显示数据就可以了
				}
			}
			lockCache.UnLock();
		}
	}
	CATCH_ALL(e)
	{
		// 不做错误处理
	}
	END_CATCH_ALL
}

void CSyncExcelThread::ShowExcelApp()
{
	PostThreadMessage(EUMT_ShowExcelApp, 0, 0);
}

void CSyncExcelThread::OnShowExcelApp( WPARAM w, LPARAM l )
{
	TRY 
	{
		if ( NULL != m_sExcelApp.m_lpDispatch )	// 激活前端显示
		{
			m_sExcelApp.SetVisible(TRUE);
			m_sExcelApp.SetUserControl(TRUE);
			
			HWND hwnd = (HWND)m_sExcelApp.GetHwnd();
			if ( IsWindow(hwnd) )
			{
				BOOL b = SetForegroundWindow(hwnd);
				ASSERT( b );
			}
		}
	}
	CATCH_ALL(e)
	{
		
	}
	END_CATCH_ALL
		
}

//////////////////////////////////////////////////////////////////////////
// 辅助结构

CSyncExcelThread::T_CellValue::T_CellValue()
{
	m_nRow = m_nCol = 0;
	m_bUseColor = false;
	m_clr = RGB(0,0,0);

	m_bIgnoreCache = false;
}

bool32 CSyncExcelThread::T_CellValue::operator==( const T_CellValue &cell ) const
{
	bool32 bSame = m_nRow == cell.m_nRow &&
		m_nCol == cell.m_nCol &&
		m_StrSheet == cell.m_StrSheet &&
		m_StrValue == cell.m_StrValue &&
		m_bUseColor == cell.m_bUseColor &&
		(m_bUseColor ? m_clr == cell.m_clr : true);
	return bSame;
}

bool32 CSyncExcelThread::T_CellValue::IsValid() const
{
	return m_nRow > 0 && m_nCol > 0;
}

CString CSyncExcelThread::T_CellValue::GetDebugString() 
{
	CString s;
#ifdef DEBUG
	s.Format(_T("CellValue: %s[%d,%d] - %s"), m_StrSheet.GetBuffer(), m_nRow, m_nCol, m_StrValue.GetBuffer());
#endif
	return s;
}

#pragma warning(disable: 4786)
#include "stdafx.h"
#include "ShareFun.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "UserBlockManager.h"
#include "PathFactory.h"
#include "IoViewShare.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "IoViewStarry.h"
#include "IoViewManager.h"
#include "GGTongView.h"
#include "dlgchoosestockblock.h"
#include "BlockConfig.h"
#include "XLTraceFile.h"
#include <set>

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int32 KUpdatePushMerchsTimerId		= 100005;	// 每隔 n 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs			= 1000 * 60 * 5;

const int32 KUpdateXYAxisTimerId			= 100006;	// 商品数据变化引发更新坐标轴
const int32 KUpdateXYAxisTimerPeriod		= 200;		// n ms后更新坐标轴

const int32 KDrawChartMerchTimerId			= 100007;
const int32 KDrawChartMerchTimerPeriod		= 1000;		// n ms后更新数据变化了的商品

const int32 KDrawBlinkUserStockTimerId		= 100008;
const int32 KDrawBlinkUserStockTimerPeriod	= 750;		// n ms后绘制自选股

const int32 KReqNextTimerId					= 100009;	// 延时请求下一个数据
const int32 KReqNextTimerPeriod				= 150;		// n ms

const int32 KTimerIdCheckReqTimeOut			= 100010;	// 检查请求是否超时
const int32 KTimerPeriodCheckReqTimeOut		= 1000*60;		// n ms

const int32 KTimerIdSyncReq					= 100011;	// 同步数据的请求, n时间请求一次数据
const int32 KTimerPeriodSyncReq				= 1000*2;	// n ms

const int32 KTimerIdReqAttendMerch			= 100012;	// 请求要关注的商品数据
const int32 KTimerPeriodReqAttendMerch		= 200;	

const UINT  KNIDBlockSelTab = 0x9998;
const UINT  KNIDXYSelTab = 0x9999;

const int32 KFixedGridRow = 10;	// 固定行

const int32	KMaxMerchRadius  = 5;
const int32 KMinMerchRadius  = 2;

static	const int32 KChartProper = 80;
static	const int32 KChartTopSkip = KMaxMerchRadius * 2;
static	const int32 KChartRightSkip = KMaxMerchRadius * 2;

// 坐标轴字段
static const CIoViewStarry::AxisType KAxisTypes[] = 
{
	CReportScheme::ERHPriceNew,					// 最新价
	CReportScheme::ERHVolumeTotal,				// 总成交量
	CReportScheme::ERHAmount,					// 金额
	CReportScheme::ERHRange,					// 幅度
	CReportScheme::ERHSwing,					// 振幅
	CReportScheme::ERHRate,						// 委比
	CReportScheme::ERHVolumeRate,				// 量比

	CReportScheme::ERHBuySellRate,				// 内外比
	CReportScheme::ERHChangeRate,				// 换手率
	CReportScheme::ERHMarketWinRate,			// 市盈率

	CReportScheme::ERHAllCapital,				// 总股本
	CReportScheme::ERHCircAsset,				// 流通股本
	CReportScheme::ERHAllAsset,				// 总资产
	CReportScheme::ERHFlowDebt,				// 流动负债
	CReportScheme::ERHPerFund,					// 每股公积金
	
	CReportScheme::ERHBusinessProfit,			// 营业利益
	CReportScheme::ERHPerNoDistribute,			// 每股未分配
	CReportScheme::ERHPerIncomeYear,			// 每股收益(年)
	CReportScheme::ERHPerPureAsset,			// 每股净资产
	CReportScheme::ERHChPerPureAsset,			// 调整每股净资产
	
	CReportScheme::ERHDorRightRate,			// 股东权益比// 
	CReportScheme::ERHCircMarketValue,			// 流通市值
	CReportScheme::ERHAllMarketValue,			// 总市值
};
static const int32 KAxisTypesCount = sizeof(KAxisTypes) / sizeof(KAxisTypes[0]);

// XML字段
const char* KStrViewStarryBlockIdName		= "BLOCKID";		// 打开的板块所属类别
const char* KStrViewStarryBlockName			= "BLOCKNAME";		// 打开的板块名称 - 现假设板块名称是唯一的
const char* KStrViewStarryXAxis				= "XAXIS";			// 当前激活x轴 - 目前为ReportHeader
const char* KStrViewStarryYAxis				= "YAXIS";			// 当前激活y轴 - 目前为ReportHeader
const char* KStrViewStarryXAxisUser			= "XAXIS_USER";		// 用户自定义X轴 - report header
const char* KStrViewStarryYAxisUser			= "YAXIS_USER";		// 自定义Y - report header
const char* KStrViewStarryRadius			= "RADIUS";			// 圆半径
const char* KStrViewStarryCoordinate		= "COORDINATE";		// 坐标 - 普通/对数

IMPLEMENT_DYNCREATE(CIoViewStarry, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewStarry, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewStarry)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_PAINT()
ON_WM_SETTINGCHANGE()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONUP()
ON_WM_ERASEBKGND()
ON_WM_SETCURSOR()
ON_WM_CONTEXTMENU()
ON_NOTIFY_RANGE(TCN_SELCHANGE, KNIDBlockSelTab, KNIDXYSelTab, OnSelChange)
ON_MESSAGE(UM_STARRY_INITIALIZE_ALLMERCHS, OnDoInitializeAllRealTimePrice)
ON_MESSAGE_VOID(UM_STARRY_UPDATEXY, OnUpdateXYAxis)
ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
ON_COMMAND_RANGE(IDM_IOVIEW_STARRY_START, IDM_IOVIEW_STARRY_END, OnMenuStarry)
//}}AFX_MSG_MAP
ON_NOTIFY(GVN_SELCHANGED,0x20207,OnMerchSelectChange)
ON_NOTIFY(NM_DBLCLK, 0x20207, OnGridDblClick)
END_MESSAGE_MAP()

CIoViewStarry::CIoViewStarry()
:CIoViewBase()
{
	m_fMinX = m_fMaxX = m_fMinY = m_fMaxY = 0.0;
	m_fScaleX = m_fScaleY = 1.0;
	m_eCoordinate = COOR_Normal;
	m_bValidMinMaxX = m_bValidMinMaxY = false;

	m_iRadius = KMaxMerchRadius;

	m_RectBottomX.SetRectEmpty();
	m_RectLeftY.SetRectEmpty();
	m_RectChart.SetRectEmpty();
	m_RectGrid.SetRectEmpty();

	m_RectLastZoomUpdate.SetRectEmpty();

	m_axisUserCur.m_StrName = _T("自定义");

	m_bMerchsRealTimeInitialized = false;

	m_ptLastMouseMove = CPoint(-1,-1);
	m_iBlockId = 0;

	m_bInZoom = false;
	m_ptZoomStart = m_ptZoomEnd = CPoint(-1,-1);

	m_bUseCacheRealTimePrices	= true;		// 使用缓存的实时报价计算 & 绘制

	m_bIsIoViewActive = false;

	m_bBlinkUserStock = false;
	m_bBlinkUserStockDrawSpecifyColor = false;

	m_pMerchLastMouse = NULL;
	m_pMerchLastSelected = NULL;

	m_bFirstReqEnd = false;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewStarry::~CIoViewStarry()
{
// 	if ( NULL != m_pSubject )
// 	{
// 		m_pSubject->DelObserver(this);
// 	}
	if (m_dcMemPaintDraw.GetSafeHdc())
	{
		m_dcMemPaintDraw.DeleteDC();
	}

	if (m_dcMemTempDraw.GetSafeHdc())
	{
		m_dcMemTempDraw.DeleteDC();
	}

	if (m_bmpImage.GetSafeHandle())
	{
		m_bmpImage.DeleteObject();
	}
	
	if ( NULL != CBlockConfig::PureInstance() )
	{
		CBlockConfig::PureInstance()->RemoveListener(this);
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}
}

BOOL CIoViewStarry::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if ( VK_ADD == pMsg->wParam && IsCtrlPressed())
		{
			// 增大半径
			ChangeRadiusByKey(true);
			return TRUE;
		}
		else if ( VK_SUBTRACT == pMsg->wParam && IsCtrlPressed())
		{
			// 缩小半径
			ChangeRadiusByKey(false);
			return TRUE;
		}
		else if ( VK_ESCAPE == pMsg->wParam)
		{
			// 如果被Zoom了，则还原
			if ( m_bInZoom )
			{
				CacelZoom();
				return TRUE;
			}
			else if ( IsPtInChart(m_ptZoomStart) )
			{
				CancelZoomSelect(m_ptLastMouseMove, true);
				return TRUE;
			}
			// 其它的由base处理
		}
		else if ( VK_SPACE == pMsg->wParam )
		{
			if ( m_bInZoom )
			{
				CacelZoom();
				return TRUE;
			}
			else if ( IsPtInChart(m_ptZoomStart) )
			{
				CancelZoomSelect(m_ptLastMouseMove, true);
			}
		}	
	}
	return CIoViewBase::PreTranslateMessage(pMsg);
}

// 通知视图改变关注的商品
void CIoViewStarry::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 立即请求该商品的数据，反应该商品的最新坐标
		bool32 bNeedReq = true;
		for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; ++i )
		{
			if ( m_aSmartAttendMerchs[i].m_pMerch == pMerch )
			{
				bNeedReq = false;
				break;
			}
		}
		if ( bNeedReq )
		{
			SetTimer(KTimerIdReqAttendMerch, KTimerPeriodReqAttendMerch, NULL);
		}
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

//
void CIoViewStarry::OnVDataForceUpdate()
{
	RequestViewData();	// 发出现有关心的请求
	RequestBlockQueueNext();
	//RequestBlockViewDataByQueue();	// 商品数据序列重新发送 - 定时更新的没更新就不更新了，不理
}

void CIoViewStarry::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	//DWORD dwTime = timeGetTime();

	//ASSERT( NULL != pMerch->m_pRealtimePrice );

	if ( NULL != pMerch && NULL != pMerch->m_pRealtimePrice )		// 由于这个视图只能获取一次数据的更新，所以该数据必须是有效的
	{
		m_mapMerchValues[pMerch].m_realTimePrice = *pMerch->m_pRealtimePrice;

		if ( m_aMerchsNeedInitializedAll.GetSize() > 0 || !m_bMerchsRealTimeInitialized )		// 初始化行情数据状态
		{
			RemoveAttendMerch(pMerch);
			
			int32 iLeave = m_aSmartAttendMerchs.GetSize();
			int32 iTotal = m_aMerchsNeedInitializedAll.GetSize();
			if ( iLeave == 0 )
			{
				m_dlgWait.CancelDlg(IDOK);
			}
			else
			{
				m_dlgWait.SetProgress(iTotal-iLeave, iTotal);
			}
		}
		else 
		{
			// 数据正常更新情况下
			
			RemoveAttendMerch(pMerch);	// 移出更新 - 定时器会保持关注

			m_mapMerchLastReqTimes[pMerch] = m_pAbsCenterManager->GetServerTime();	// 更新最后请求时间

			// 如果是当前的商品，则对画面产生影响
			if ( m_MerchsCurrent.count(pMerch) )
			{
				if ( !m_bFirstReqEnd )
				{
					// 如果是该板块的第一次请求数据，则立即请求下一次的商品，否则交给sync计时器去请求下一个
					if ( m_aSmartAttendMerchs.GetSize() < 3 )
					{
						// 等到大部分商品都回来了，才发下一次的请求，频繁发请求导致阻塞界面
						RequestBlockQueueNextAsync();	// 如果在板块第一次请求时，由于是空的，所以不必担心有数据请求
					}
				}
				
				UpdateSelectedMerchValue(pMerch);	// 更新下选择商品的数据
				
				bool32 bChange = false;
				CalcXYMinMaxAxis(pMerch, true, &bChange);
				if ( !bChange )
				{
					m_aMerchsNeedDraw.Add(pMerch);
					SetTimer(KDrawChartMerchTimerId, KDrawChartMerchTimerPeriod, NULL);	// 定时绘制
				}
			}
		}
	}

	//TRACE(_T("星空RealTime: %d ms\r\n"), timeGetTime()-dwTime);
}

// 当物理市场排行发生变化
// 当物理市场排行发生变化
void CIoViewStarry::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	// 与排行无关
}

void CIoViewStarry::OnVDataGridHeaderChanged(E_ReportType eReportType)
{

}

void CIoViewStarry::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();	

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	m_wndTabBlock.SetBkGround(false, clrBk, 0, 0);
	m_wndTabXYSet.SetBkGround(false, clrBk, 0, 0);

	UpdateAllContent();	
}

void CIoViewStarry::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	UpdateAllContent();
}

void CIoViewStarry::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewStarry::FromXml( TiXmlElement *pTiXmlElement )
{
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif
	if (NULL == pTiXmlElement)
		return false;
	
	// 判读是不是IoView的节点
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// 不是描述自己的业务节点
		return false;

	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);
	
	// block
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryBlockIdName);
	if ( NULL == pcAttrValue )
	{
		m_iBlockId = 0;
	}
	else
	{
		m_iBlockId = atoi(pcAttrValue);
	}

	CString StrBlockName;
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryBlockName);
	if ( NULL != pcAttrValue )
	{
		::MultiByteToWideChar(CP_UTF8, 0, pcAttrValue, -1, StrBlockName.GetBuffer(1000), 1000);
		StrBlockName.ReleaseBuffer();
	}

	// 先按id查找，如果找到了就不进行名称查找了
	ASSERT( CBlockConfig::Instance()->IsInitialized() );
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
	//ASSERT( NULL != pBlock );
	if ( NULL == pBlock && !StrBlockName.IsEmpty() )
	{
		CBlockConfig::BlockArray	aBlocks;
		CBlockConfig::Instance()->GetBlocksByName(StrBlockName, aBlocks);
		if ( aBlocks.GetSize() > 0 )
		{
			pBlock = aBlocks[0];
		}
	}
	else
	{
		// 保持空还是取自选股？
	}

	if ( NULL != pBlock )
	{
		m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
		pBlock->m_blockInfo.FillBlock(m_block);
		ASSERT( m_block.m_StrName == StrBlockName );
	}
	
	// xy轴
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryXAxis);
	if ( NULL != pcAttrValue )
	{
		m_axisCur.m_eXType = (AxisType) atoi(pcAttrValue);
	}
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryYAxis);
	if ( NULL != pcAttrValue )
	{
		m_axisCur.m_eYType = (AxisType) atoi(pcAttrValue);
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryXAxisUser);
	if ( NULL != pcAttrValue )
	{
		m_axisUserCur.m_eXType = (AxisType) atoi(pcAttrValue);
	}
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryYAxisUser);
	if ( NULL != pcAttrValue )
	{
		m_axisUserCur.m_eYType = (AxisType) atoi(pcAttrValue);
	}

	// 半径
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryRadius);
	if ( NULL != pcAttrValue )
	{
		m_iRadius = atoi(pcAttrValue);
		if ( m_iRadius < KMinMerchRadius )
		{
			m_iRadius = KMinMerchRadius;
		}
		else if ( m_iRadius > KMaxMerchRadius )
		{
			m_iRadius = KMaxMerchRadius;
		}
	}
	
	// 坐标类型
	E_Coordinate eCoor = m_eCoordinate;
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryCoordinate);
	if ( NULL != pcAttrValue )
	{
		eCoor = (E_Coordinate)atoi(pcAttrValue);
	}

	// 完成xml，调整参数影响
	int i=0;
	for ( i=0 ; i < m_aBlockCollectionIds.GetSize() && NULL != pBlock ; i++ )
	{
		if ( m_aBlockCollectionIds[i] == pBlock->m_blockCollection.m_iBlockCollectionId )
		{
			m_wndTabBlock.SetCurtab(i);
			break;
		}
	}

	for ( i=0; i < m_aXYTypes.GetSize() ; i++ )
	{
		if ( m_aXYTypes[i] == m_axisCur )
		{
			m_wndTabXYSet.SetCurtab(i);
			break;
		}
	}
	if ( i >= m_aXYTypes.GetSize() )
	{
		m_wndTabXYSet.SetCurtab(m_aXYTypes.GetSize()-1);		// 剩下的全部归为自定义
	}

	SetCoordinate(eCoor);

	SetNewBlock(m_block);

	m_wndTabBlock.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);
	m_wndTabXYSet.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);

#ifdef _DEBUG
	TRACE(_T("星空图fromXml: %d ms\r\n"), timeGetTime()-dwTime);
#endif

	return true;
}

CString CIoViewStarry::ToXml()
{
	//
	CString StrThis;
	
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" ", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1",
		CString(KStrViewStarryBlockIdName).GetBuffer(), m_iBlockId,
		CString(KStrViewStarryBlockName).GetBuffer(), m_block.m_StrName.GetBuffer(),
		CString(KStrViewStarryXAxis).GetBuffer(), m_axisCur.m_eXType,
		CString(KStrViewStarryYAxis).GetBuffer(), m_axisCur.m_eYType,
		CString(KStrViewStarryXAxisUser).GetBuffer(), m_axisUserCur.m_eXType,
		CString(KStrViewStarryYAxisUser).GetBuffer(), m_axisUserCur.m_eYType,
		CString(KStrViewStarryRadius).GetBuffer(), m_iRadius,
		CString(KStrViewStarryCoordinate).GetBuffer(), m_eCoordinate);

	StrThis += SaveColorsToXml();
	StrThis += SaveFontsToXml();
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewStarry::GetDefaultXML()
{
	CString StrThis;
	// 
	// 额外添加block class, name, axis x,y axis user x, y , radius, coordinate
	T_XYAxisType axis;
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1",
		CString(KStrViewStarryBlockIdName).GetBuffer(), 0,
		CString(KStrViewStarryBlockName).GetBuffer(), _T(""),
		CString(KStrViewStarryXAxis).GetBuffer(), axis.m_eXType,
		CString(KStrViewStarryYAxis).GetBuffer(), axis.m_eYType,
		CString(KStrViewStarryXAxisUser).GetBuffer(), axis.m_eXType,
		CString(KStrViewStarryYAxisUser).GetBuffer(), axis.m_eYType,
		CString(KStrViewStarryRadius).GetBuffer(), KMaxMerchRadius,
		CString(KStrViewStarryCoordinate).GetBuffer(), COOR_Normal );
	
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewStarry::OnIoViewActive()
{
	SetFocus();
	m_bIsIoViewActive = true;
	

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetHotkeyTarget(this);

	// 这里不申请数据 - 定时器会申请
	
	Invalidate(TRUE);
}

void CIoViewStarry::OnIoViewDeactive()
{
	m_bIsIoViewActive = false;

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	Invalidate(TRUE);
}

void CIoViewStarry::LockRedraw()
{
	if ( !m_bLockRedraw )
	{
		SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
		::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
		::SendMessage(m_wndTabBlock.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
		::SendMessage(m_wndTabXYSet.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
		m_bLockRedraw = true;
	}
}

void CIoViewStarry::UnLockRedraw()
{
	if ( m_bLockRedraw )
	{
		SendMessage(WM_SETREDRAW, TRUE, 0);
		::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
		::SendMessage(m_wndTabBlock.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
		::SendMessage(m_wndTabXYSet.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
		Invalidate(TRUE);
		m_bLockRedraw = false;
	}
}

int CIoViewStarry::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iCreate = CIoViewBase::OnCreate(lpCreateStruct);
	if ( iCreate == -1 )
	{
		return -1;
	}

	//ModifyStyle(0, WS_CLIPCHILDREN);		// 子窗口clip

	InitialIoViewFace(this);	// 这个应该移到ioview base里面调用

	//创建Tab 条 // 初始化tab条
	m_wndTabBlock.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,KNIDBlockSelTab);
	m_wndTabBlock.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_wndTabBlock.SetUserCB(this);
	m_wndTabBlock.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	ASSERT( CBlockConfig::Instance()->IsInitialized() );
	CBlockConfig::Instance()->GetCollectionIdArray(m_aBlockCollectionIds);
	// 删除期货板块 & 空的
	int i;
	for ( i=m_aBlockCollectionIds.GetSize()-1; i >= 0 ; --i )
	{
		int32 iColId = m_aBlockCollectionIds[i];
		CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(iColId);
		if ( NULL == pCol 
			|| pCol->m_aBlocks.GetSize() <= 0 
			|| pCol->m_aBlocks[0] == NULL 
			|| (pCol->m_aBlocks[0]->m_blockInfo.m_iType != CBlockInfo::typeNormalBlock
			&& pCol->m_aBlocks[0]->m_blockInfo.m_iType != CBlockInfo::typeMarketClassBlock
			&& pCol->m_aBlocks[0]->m_blockInfo.m_iType != CBlockInfo::typeUserBlock)
			)
		{
			m_aBlockCollectionIds.RemoveAt(i);
		}
	}
	ASSERT( m_aBlockCollectionIds.GetSize() > 0 );
	for ( i=0; i < m_aBlockCollectionIds.GetSize() ; i++ )
	{
		CString Str;
		CBlockConfig::Instance()->GetCollectionName(m_aBlockCollectionIds[i], Str);
		if ( Str.GetLength() > 2 && Str.Right(2) == _T("板块") )
		{
			Str.Delete(Str.GetLength()-2, 2);
		}
		m_wndTabBlock.Addtab(Str + _T("▲"), Str, Str);
	}

	m_wndTabXYSet.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,KNIDXYSelTab);
	m_wndTabXYSet.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_wndTabXYSet.SetUserCB(this);
	m_wndTabXYSet.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	m_wndTabXYSet.Addtab(_T("价盘"), _T("价盘"), _T("价格/流通股本"));
	m_wndTabXYSet.Addtab(_T("价资"), _T("价资"), _T("价格/每股净资产"));
	m_wndTabXYSet.Addtab(_T("涨量比"), _T("涨量比"), _T("涨幅/量比"));
	m_wndTabXYSet.Addtab(_T("涨盘"), _T("涨盘"), _T("涨幅/流通股本"));
	m_wndTabXYSet.Addtab(_T("涨换"), _T("涨换"), _T("涨幅/换手率"));
	m_wndTabXYSet.Addtab(_T("自定义▲"), _T("自定义"), _T("自定义"));
	T_XYAxisType	xyType;
	m_aXYTypes.Add(xyType);	// 价盘
	xyType.m_eYType = CReportScheme::ERHPriceNew;
	xyType.m_eXType = CReportScheme::ERHPerPureAsset;
	xyType.m_StrName = _T("价资");		
	m_aXYTypes.Add(xyType);		// 价资
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHVolumeRate;
	xyType.m_StrName = _T("涨量比"); // 涨量比
	m_aXYTypes.Add(xyType);
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHCircAsset;
	xyType.m_StrName = _T("涨盘");		
	m_aXYTypes.Add(xyType);		// 涨盘
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHChangeRate;
	xyType.m_StrName = _T("涨换");		
	m_aXYTypes.Add(xyType);		// 涨换

	m_aXYTypes.Add(T_XYAxisType());	// 自定义默认选默认
	m_axisCur = m_aXYTypes[0];		// 默认选择
	m_axisUserCur = m_axisCur;		// 默认自定义选择
	m_axisUserCur.m_StrName = _T("自定义");

	ASSERT( m_aXYTypes.GetSize() == m_wndTabXYSet.GetCount() );
	
	// 创建数据表格 - block
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 0x20205);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrl.SetBorderColor(GetIoViewColor(ESCChartAxisLine)); // border显示有问题
	
	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(false);
	m_GridCtrl.EnableSelection(FALSE);
	m_GridCtrl.SetColumnCount(2);
	
	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// 表zoom描述
	m_GridCtrlZoomDesc.Create(CRect(0, 0, 0, 0), this, 0x20206);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrlZoomDesc.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrlZoomDesc.SetBorderColor(GetIoViewColor(ESCChartAxisLine));
	
	// 显示表格线的风格
	m_GridCtrlZoomDesc.ShowGridLine(false);
	m_GridCtrlZoomDesc.EnableSelection(FALSE);

	m_GridCtrlZoomDesc.SetColumnCount(1);
	
	// 设置字体
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, TRUE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(TRUE, FALSE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(TRUE, TRUE)->SetFont(pFontSmall);

	// 表zoom商品显示
	m_GridCtrlZoomMerchs.Create(CRect(0, 0, 0, 0), this, 0x20207);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrlZoomMerchs.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrlZoomMerchs.SetBorderColor(GetIoViewColor(ESCChartAxisLine));
	
	// 显示表格线的风格
	m_GridCtrlZoomMerchs.ShowGridLine(false);
	m_GridCtrlZoomMerchs.EnableSelection(TRUE);
	m_GridCtrlZoomMerchs.SetSingleRowSelection(TRUE);
	m_GridCtrlZoomMerchs.SetColumnCount(2);
	
	// 设置字体
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	m_wndScrollV.Create(WS_CHILD|WS_VISIBLE, this, CRect(0,0,0,0), 0x20208);
	m_GridCtrlZoomMerchs.SetScrollBar(NULL, &m_wndScrollV);
	m_wndScrollV.SetOwner(&m_GridCtrlZoomMerchs);
	
	EmptyGridContent();

	T_Block block;
	SetNewBlock(block);		// 空block

	CBlockConfig::Instance()->AddListener(this);

	PostMessage(UM_STARRY_INITIALIZE_ALLMERCHS);		// 刷新所有商品行情信息，目前由于推送没有注销的办法，所以这里会要修改

	return iCreate;
}

void CIoViewStarry::OnSize( UINT nType, int cx, int cy )
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CIoViewStarry::RecalcLayout()
{
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
	int32 iWidth = rcClient.Width();
	int32 iHeight = rcClient.Height();
	
	// 
	CRect rcGridOld = m_RectGrid;

	// 清空几个矩形
	m_RectLeftY.SetRectEmpty();
	m_RectChart.SetRectEmpty();
	m_RectBottomX.SetRectEmpty();
	m_RectGrid.SetRectEmpty();
	
	// 右侧给grid
	int32 iGridWidth = 160;
	m_RectGrid = rcClient;
	m_RectGrid.left = m_RectGrid.right - iGridWidth;
	{ // 划分Grid空间
		ASSERT( m_GridCtrl.GetRowCount() > 0 );
		int32 iGridBlockHeight = 200;
		if ( m_GridCtrl.GetRowCount() > 0 )
		{
			iGridBlockHeight = m_GridCtrl.GetRowCount() * m_GridCtrl.GetRowHeight(0);
		}
		int32 iGridZoomDescHeight = 65;
		if ( m_GridCtrlZoomDesc.GetRowCount() > 0 )
		{
			iGridZoomDescHeight = m_GridCtrlZoomDesc.GetRowCount() * m_GridCtrlZoomDesc.GetRowHeight(0);
		}
		
		CRect rcGrid(m_RectGrid); // 顶
		m_GridCtrl.MoveWindow(rcGrid.left, rcGrid.top, iGridWidth, iGridBlockHeight);
		m_GridCtrl.ExpandColumnsToFit();
		
		rcGrid.top += iGridBlockHeight;	// 底
		int32 iTopDesc = rcGrid.Height() > iGridZoomDescHeight ? rcGrid.bottom - iGridZoomDescHeight : rcGrid.top;
		m_GridCtrlZoomDesc.MoveWindow(rcGrid.left, iTopDesc, iGridWidth, iGridZoomDescHeight);
		m_GridCtrlZoomDesc.ExpandColumnsToFit();
		
		rcGrid.bottom = iTopDesc;
		m_GridCtrlZoomMerchs.MoveWindow(rcGrid);
	}
	
	iWidth -= iGridWidth;	
	rcClient.right -= iGridWidth;
	if ( iWidth < 0 )
	{
		m_wndTabXYSet.MoveWindow(CRect(0,0,0,0));
		m_wndTabBlock.MoveWindow(CRect(0,0,0,0));
	}
	else if ( iWidth > KChartRightSkip )
	{
		int32 iTabHeight = m_wndTabBlock.GetFitHorW();
		int32 iTabBlockSeperatePos = rcClient.left + iWidth/2; // 留一个像素画分割线 grid的border
		int32 iTabWidth1 = iTabBlockSeperatePos - rcClient.left;
		int32 iTabWidth2 = rcClient.right - iTabBlockSeperatePos-1;

		CSize sizeTabBlock = m_wndTabBlock.GetWindowWishSize();
		CSize sizeTabXYSet = m_wndTabXYSet.GetWindowWishSize();
		CRect rcTabBlock(rcClient.left, rcClient.bottom-iTabHeight, rcClient.left + iTabWidth1, rcClient.bottom);
		CRect rcTabXYSet(iTabBlockSeperatePos, rcClient.bottom-iTabHeight, iTabBlockSeperatePos+iTabWidth2, rcClient.bottom);
		if ( sizeTabBlock.cx + sizeTabXYSet.cx <= iTabWidth1 + iTabWidth2 )	// 决定两个tab分割比例
		{
			rcTabBlock.right = rcTabBlock.left + sizeTabBlock.cx;
			rcTabXYSet.left = rcTabXYSet.right -  sizeTabXYSet.cx;
		}
		else
		{
			// 先满足需求小的，剩下的全给大的
			if ( sizeTabXYSet.cx < iTabWidth2 )	// 有空余
			{
				rcTabXYSet.left = rcTabXYSet.right -sizeTabXYSet.cx;
				rcTabBlock.right = rcTabXYSet.left - 2;
			}
			else if ( sizeTabBlock.cx < iTabWidth1 )
			{
				rcTabBlock.right = rcTabBlock.left + sizeTabBlock.cx;
				rcTabXYSet.left = rcTabBlock.right + 2;
			}
			// 如果都是特大，就平分
		}
		m_wndTabBlock.MoveWindow(rcTabBlock);
		m_wndTabXYSet.MoveWindow(rcTabXYSet);	// 需要调整
		
		iHeight -= iTabHeight;
		rcClient.bottom -= iTabHeight;
		if ( iHeight > KChartTopSkip )
		{
			int32 iYWidth = 62;
			int32 iXHeight = 25;
			
			int32 iYRealWidth = iWidth > iYWidth ? iYWidth : iWidth;
			int32 iXRealHeight = iHeight > iXHeight ? iXHeight : iHeight;
			m_RectLeftY.SetRect(rcClient.left, rcClient.top, rcClient.left + iYRealWidth, rcClient.bottom - iXRealHeight);
			rcClient.left += iYRealWidth;
			
			m_RectBottomX.SetRect(rcClient.left, rcClient.bottom - iXRealHeight, rcClient.right, rcClient.bottom);
			rcClient.bottom -= iXRealHeight;
			
			m_RectChart = rcClient;
			
			// 矩形需要进行skip微调 - 但是绘制时需要撑到边
			m_RectLeftY.top += KChartTopSkip;
			m_RectChart.top += KChartTopSkip;
			m_RectChart.right -= KChartRightSkip;
			m_RectBottomX.right -= KChartRightSkip;
		}
	}
	
	if ( !rcGridOld.EqualRect(m_RectGrid) )	// 如果变化了则计算
	{
		CalcDrawMerchs(m_block.m_aMerchs);	// 重算绘制点
	}
}

void CIoViewStarry::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		if ( IsFinanceDataType(m_axisCur.m_eXType) && IsFinanceDataType(m_axisCur.m_eYType) )	// 都是财务数据就不需要动态更新了
		{
		}
		else if ( m_lstMerchsWaitToRequest.IsEmpty() )
		{
			// 仅当以前的数据已经请求完毕了才重新更新
			//RequestBlockViewData();	// 重新申请数据
			// 取上证，看现在时间是否为开收盘时间
			// 如果是收盘时间，则查看板块列表中的前n个商品，
			// 看是这些商品的更新时间是否过了今日收盘时间
			// 过了，则不发送这次的timer更新
			bool32 bUpdate = true;
			CMerch *pMerch = NULL;
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000001"), 0, pMerch)
				&& NULL != pMerch )
			{
				CMarketIOCTimeInfo IOCTime;
				CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
				if ( pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo)
					&& TimeNow > IOCTime.m_TimeClose.m_Time+CGmtTimeSpan(0,0,2,0) )
				{
					CGmtTime TimeEnd = IOCTime.m_TimeClose.m_Time+CGmtTimeSpan(0,0,2,0);
					int32 iTmpMerchSize = m_block.m_aMerchs.GetSize();
					int32 iMaxMerchCount = min(15, iTmpMerchSize);
					bool32 bAllHasData = true;	// 默认所有商品都有数据了
					for ( int32 i=0; i < iMaxMerchCount ; ++i )
					{
						MerchReqTimeMap::iterator it = m_mapMerchLastReqTimes.find(m_block.m_aMerchs[i]);
						if ( it == m_mapMerchLastReqTimes.end()
							|| TimeEnd > it->second )
						{
							// 没有发送过该商品请求的记录或者该请求在收盘之前发送的，则认为本次应当发送请求
							bAllHasData = false;
							break;
						}
					}
					bUpdate = !bAllHasData;
				}
			}
			if ( bUpdate )
			{
				// 需要更新，则开始同步
				m_bFirstReqEnd = true;
				RequestBlockViewDataByQueue();	// 序列请求发送
				SetTimer(KTimerIdSyncReq, KTimerPeriodSyncReq, NULL);
			}
		}
	}
	else if ( KTimerIdSyncReq == nIDEvent )
	{
		// 同步请求
		RequestBlockQueueNext();
		if ( m_lstMerchsWaitToRequest.IsEmpty() )
		{
			KillTimer(nIDEvent);
		}
	}
	else if ( KUpdateXYAxisTimerId == nIDEvent )
	{
		if ( !m_bInZoom )	// 缩放状态不允许自动计算坐标轴
		{
			RefreshBlock();	// 坐标轴已经变化，重计算&显示
		}
		KillTimer(nIDEvent);
	}
	else if ( KDrawChartMerchTimerId == nIDEvent )
	{
		if ( m_aMerchsNeedDraw.GetSize() > 0 )
		{
			CalcDrawMerchs(m_aMerchsNeedDraw);
			Invalidate(TRUE);	// 让paint绘制 - paint有针对新更新商品的位置计算
		}
		else
		{
// 			for ( int32 i=0; i < m_aMerchsNeedDraw.GetSize() ; i++ )
// 			{
// 				DrawMerch(m_aMerchsNeedDraw[i], NULL);
// 			}
		}
		m_aMerchsNeedDraw.RemoveAll();
		KillTimer(nIDEvent);
	}
	else if ( KDrawBlinkUserStockTimerId == nIDEvent )
	{
		m_bBlinkUserStockDrawSpecifyColor = !m_bBlinkUserStockDrawSpecifyColor;
		DrawBlinkUserStock();
	}
	else if ( KReqNextTimerId == nIDEvent )
	{
		RequestBlockQueueNext();
	}
	else if ( KTimerIdCheckReqTimeOut == nIDEvent )
	{
		// 这个定时消息过来，证明reqnext已经有一定时间没有调用了
		bool32 bReqTimeOut = !m_lstMerchsWaitToRequest.IsEmpty();
		RequestBlockQueueNextAsync();	// 调用next
		KillTimer(KTimerIdCheckReqTimeOut);
		if ( bReqTimeOut )
		{
			TRACE(_T("星空图商品数据请求超时发生\r\n"));
		}
	}
	else if ( KTimerIdReqAttendMerch == nIDEvent )
	{
		if ( NULL != m_pMerchXml && IsMerchNeedReqData(m_pMerchXml) )
		{
			AddAttendMerch(m_pMerchXml, true);
		}
		KillTimer(KTimerIdReqAttendMerch);
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewStarry::DrawActiveFlag( CDC &dc )
{
	COLORREF clrActive = GetIoViewColor( m_bIsIoViewActive ? ESCVolume : ESCBackground );
	dc.FillSolidRect(3,2,2,2, clrActive);
}

void CIoViewStarry::OnPaint()
{
#ifdef _DEBUG
	CString StrLogFile(_T(""));

	//StrLogFile = _T("Starry.log");

	XLTraceFile::GetXLTraceFile(StrLogFile).TraceWithTimestamp(_T("\r\n\r\n###     Start Paint     ###"), TRUE);
#endif

	CPaintDC dc(this);

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	CDC &dcMem = BeginMainDrawDC(dc);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	dcMem.SetBkColor(clrBk);
	dcMem.FillSolidRect(rc, clrBk);

	if ( m_aMerchsNeedDraw.GetSize() > 0 )
	{
		CalcDrawMerchs(m_aMerchsNeedDraw);
		m_aMerchsNeedDraw.RemoveAll();		// 进入paint就不需要绘制等待绘制的商品了
	}

	DrawActiveFlag(dcMem);
	DrawXAxis(dcMem);
	DrawYAxis(dcMem);
	DrawChart(dcMem);

	EndMainDrawDC(dc);
#ifdef _DEBUG
	XLTraceFile::GetXLTraceFile(StrLogFile).TraceWithTimestamp(_T("   end main draw"), TRUE);
#endif
	DrawMouseMove(m_ptLastMouseMove);	// 绘制临时

	UpdateBlockPaintMerchCount();
#ifdef _DEBUG
	XLTraceFile::GetXLTraceFile(StrLogFile).TraceWithTimestamp(_T("~~~    End Paint    ~~~"), TRUE);
	XLTraceFile::GetXLTraceFile(StrLogFile).Trace0(_T("\r\n\r\n"));
#endif
}

bool32 CIoViewStarry::AskUserBlockSelect(OUT T_Block &block, OUT int32 &iBlockId, int32 iCurTab, CPoint ptScr )
{
	if ( iCurTab < 0 || iCurTab >= m_aBlockCollectionIds.GetSize() )
	{
		return false;
	}

	CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aBlockCollectionIds[iCurTab]);
	if ( NULL == pCollection )
	{
		return false;
	}

	// 获取所有板块
	CBlockConfig::BlockArray aBlocks;
	pCollection->GetValidBlocks(aBlocks);
	if ( aBlocks.GetSize() <= 0 )
	{
		return false;
	}
	
	CMenu menu;
	menu.CreatePopupMenu();
	
	for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
	{
		menu.AppendMenu(MF_STRING, (UINT)(i+1), aBlocks[i]->m_blockInfo.m_StrBlockName);
		if ( aBlocks[i]->m_blockInfo.m_iBlockId == m_iBlockId )
		{
			menu.CheckMenuItem(i+1, MF_BYCOMMAND |MF_CHECKED);
		}
	}
	
	MultiColumnMenu(menu);
	if ( ptScr.x < 0 || ptScr.y < 0 )
	{
		GetCursorPos(&ptScr);
	}
	
	int32 iSel = (int32)menu.TrackPopupMenu(TPM_NONOTIFY |TPM_RETURNCMD |TPM_BOTTOMALIGN |TPM_LEFTALIGN, ptScr.x, ptScr.y, AfxGetMainWnd());
	if ( iSel > 0 && iSel <= aBlocks.GetSize() )
	{
		iSel--;
		iBlockId = aBlocks[iSel]->m_blockInfo.m_iBlockId;
		return aBlocks[iSel]->m_blockInfo.FillBlock(block);
	}
	
	return false;
}

void CIoViewStarry::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewStarry::OnLButtonDown2( CPoint pt, int32 iTab )
{
	if ( NULL == m_pAbsCenterManager )
	{
		return false;
	}

	const MSG *pMsg = GetCurrentMessage();
	if ( pMsg->message != WM_LBUTTONDOWN )
	{
		return true;
	}

	if ( pMsg->hwnd == m_wndTabBlock.m_hWnd )
	{
		// block选择
		
		//if ( iTab == 0 )	// 物理板块
		{
			
			T_Block block;
			CRect rc;
			if ( m_wndTabBlock.GetTabRect(rc, iTab) )
			{
				m_wndTabBlock.ClientToScreen(&rc);
				int32 iBlockId = m_iBlockId;
				if ( AskUserBlockSelect(block, iBlockId, iTab, rc.TopLeft()) )
				{
					m_iBlockId = iBlockId;
					SetNewBlock(block);
					return false;
				}
			}
		}
	}
	else if ( pMsg->hwnd == m_wndTabXYSet.m_hWnd ) 
	{
		// xy
		if ( iTab == m_aXYTypes.GetSize() - 1 )	// 自定义坐标轴
		{
			int32 iSelCur = m_wndTabXYSet.GetCurtab();
			if ( iSelCur == iTab )	// 这种情况下，不会有tabsel，所以这里处理
			{
				CRect rcTab(0,0,0,0);
				if ( !m_wndTabXYSet.GetTabRect(rcTab, iTab) )
				{
					return true;	// 错误了
				}
				m_wndTabXYSet.ClientToScreen(&rcTab);
				rcTab.top += 1;

				SelectUserAxisType(rcTab.TopLeft(), m_axisUserCur);
				ChangeAxisType(m_axisUserCur.m_eXType, m_axisUserCur.m_eYType);
			}
		}
		return false;
	}
	else
	{
		ASSERT( 0 );
	}

	return true; // 不切换
}

void CIoViewStarry::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( eInitializeType == BeforeInitialize )
	{
		// 等到初始化完成后处理
	}
	else if ( eInitializeType == Initialized )
	{
		// 更新板块配置信息
		CBlockConfig::Instance()->GetCollectionIdArray(m_aBlockCollectionIds);
		
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
		if ( NULL != pBlock )
		{
			pBlock->m_blockInfo.FillBlock(m_block);
			SetNewBlock(m_block, true);
		}
		// 如果不存在该板块是清除还是保留以前的信息？

		RecreateBlockTab();
	}
}

void CIoViewStarry::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	if ( BlockModified == eNotifyType && iBlockId == m_iBlockId )
	{
		// 板块数据变更 - 重设
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		ASSERT( NULL != pBlock );
		if ( NULL != pBlock )
		{
			pBlock->m_blockInfo.FillBlock(m_block);
			SetNewBlock(m_block, true);	// 等待定时器刷新数据, 财务数据需要强制刷新
		}
	}
	else if ( BlockDeleted == eNotifyType && iBlockId == m_iBlockId )
	{
		// 板块被删除了 - 怎么办
		m_block.m_aMerchs.RemoveAll();
		SetNewBlock(m_block, false);	// 仅清除商品数据
	}

	if ( CBlockConfig::Instance()->IsUserBlock(iBlockId) )
	{
		UpdateUserBlockMerchs();
		if ( m_bBlinkUserStock )
		{
			DrawBlinkUserStock();
		}
	}
}

// 
// void CIoViewStarry::OnUserBlockUpdate( CSubjectUserBlock::E_UserBlockUpdate eUpdateType )
// {
// 	// 商品变化，板块信息变化，板块被删除
// 	CBlockClassManager::UpdateUserBlock();
// 
// 	T_Block blockOld = m_block;
// 
// 	if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
// 	{
// 		CArray<T_Block, T_Block &> aBlocks;
// 		CUserBlockManager::Instance()->GetBlocks(aBlocks);
// 		for ( int32 j = 0; j < aBlocks.GetSize(); j++ )
// 		{
// 			T_Block &stBlock = aBlocks.GetAt(j);
// 			
// 			if ( stBlock.m_bChangeName && stBlock.m_StrNameOld == m_block.m_StrName )
// 			{
// 				m_block.m_StrName = stBlock.m_StrName;	// 名称变更 - 变更现在关注板块的名称
// 				break;
// 			}
// 		}
// 	}
// 
// 	if ( NULL != CUserBlockManager::Instance()->GetBlock(m_block.m_StrName) )	// 现在关注的板块是自定义的 - 有可能该板块已经被删除了
// 	{
// 		CBlockClassManager::FillUserBlock(m_block.m_StrName, m_block);
// 		// 看是否有商品增加或者删除 - 需要细节处理，现在简单调用new block，不过会发送大量请求
// 		SetNewBlock(m_block, false);		// 此时不发送数据，等待计时器去同步新加入的商品
// 	}
// 	else
// 	{
// 		if ( m_block.m_eHeadType == ERTCustom )	// 板块被删除了 - 保持显示吧
// 		{
// 		}
// 		UpdateUserBlockMerchs();		// 更新一下哪些是自定义商品
// 		if ( m_bBlinkUserStock )
// 		{
// 			DrawBlinkUserStock();
// 		}
// 	}
// }

BOOL CIoViewStarry::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewStarry::OnSelChange(UINT nId, NMHDR* pNMHDR, LRESULT* pResult )
{
	if ( NULL != pResult )
	{
		*pResult = TRUE;
	}

	if ( !m_bMerchsRealTimeInitialized )	// 没有初始化完成时的sel消息是不需要处理的
	{
		return;
	}

	if ( KNIDBlockSelTab == nId )
	{
	}
	else if ( KNIDXYSelTab == nId )
	{
		int32 iSel = m_wndTabXYSet.GetCurtab();
		if ( iSel >=0 && iSel < m_aXYTypes.GetSize() )
		{
			if ( m_aXYTypes.GetSize()-1 == iSel )	// 自定义 - 由其它tab切换到这里
			{
				m_axisCur.m_StrName = m_axisUserCur.m_StrName;
				ChangeAxisType(m_axisUserCur.m_eXType, m_axisUserCur.m_eYType);	// 先进行默认的切换, 然后弹出菜单处理

				CRect rcTab(0,0,0,0);
				if ( !m_wndTabXYSet.GetTabRect(rcTab, iSel) )
				{
					return;	// 错误了
				}
				m_wndTabXYSet.ClientToScreen(&rcTab);
				
				rcTab.top += 1;
				SelectUserAxisType(rcTab.TopLeft(), m_axisUserCur);
				
				ChangeAxisType(m_axisUserCur.m_eXType, m_axisUserCur.m_eYType);
			}
			else
			{
				m_axisCur.m_StrName = m_aXYTypes[iSel].m_StrName;
				ChangeAxisType(m_aXYTypes[iSel].m_eXType, m_aXYTypes[iSel].m_eYType);
			}
		}
	}
}

void CIoViewStarry::SetNewBlock( const T_Block &block, bool32 bRequestData/* = true*/ )
{
// 	if ( m_block.m_StrName == block.m_StrName && m_block.m_aMerchs.GetSize() == block.m_aMerchs.GetSize() )
// 	{
// 		return;
// 	}
	
	T_Block blockTemp = block;	// 备份该block，防止相同的m_block清除数据
	m_block = block;	// 重算坐标，重绘，更新grid
	m_block.m_aMerchs.RemoveAll();
	int i=0;
	// 过滤国内商品 指数 ，其它的不要
	for ( i=0; i < blockTemp.m_aMerchs.GetSize() ; i++ )
	{
		CMerch *pMerch = blockTemp.m_aMerchs[i];
		E_ReportType eRT = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
		if ( NULL != pMerch && (eRT == ERTStockCn || eRT == ERTExp || eRT == ERTStockHk) )
		{
			m_block.m_aMerchs.Add( pMerch );
		}
	}
	
	for ( i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_block.m_aMerchs[i] != NULL )	// 将这个视图的xml merch设置一下
		{
			OnVDataMerchChanged(m_block.m_aMerchs[i]->m_MerchInfo.m_iMarketId, m_block.m_aMerchs[i]->m_MerchInfo.m_StrMerchCode, m_block.m_aMerchs[i]);
			break;
		}
	}

	// 更新当前商品集合
	m_MerchsCurrent.clear();
	for ( i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_block.m_aMerchs[i] != NULL )	
		{
			m_MerchsCurrent.insert(m_block.m_aMerchs[i]);
		}
	}

	m_aMerchsNeedDraw.RemoveAll();	// 待绘制的点清除

	m_bFirstReqEnd = false;
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);	// 重置板块定时更新
	KillTimer(KTimerIdSyncReq);	// 结束同步timer
	KillTimer(KTimerIdCheckReqTimeOut);	// 结束超时检查timer
	m_lstMerchsWaitToRequest.RemoveAll();	// 以前的请求数据应当清除
	if ( m_bMerchsRealTimeInitialized && bRequestData )	// 初始化全局数据时不能申请数据，此时数据会被丢弃- 貌似不能在过早的时间申请数据，否则返回NULL real price
	{
		RequestBlockViewData();	// 申请数据 - 因为没有做初始化了
	}

	UpdateUserBlockMerchs();

	CancelZoomSelect(CPoint(-1,-1), false);
	CacelZoom();		// 取消缩放

	UpdateSelectedMerchs();	// 取消选择的商品列表

	RefreshBlock();

	SetChildFrameTitle();
}

bool32 CIoViewStarry::SetNewBlock( int32 iBlockId, bool32 bReOpen /*= false*/, bool32 bRequestData /*= true*/ )
{
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( NULL != pBlock )
	{
		if ( !bReOpen && iBlockId == m_iBlockId )
		{
			return true;	// 已经打开
		}

		m_iBlockId = iBlockId;
		T_Block block;
		pBlock->m_blockInfo.FillBlock(block);
		SetNewBlock(block, bRequestData);
		return true;
	}
	return false;
}

void CIoViewStarry::RefreshBlock()
{
	CalcXYAllMinMaxAxis(true);	// 重算坐标 & 刷新显示
}

void CIoViewStarry::CalcXYAllMinMaxAxis(bool32 bNotifyChange/* = false*/)
{
	double fxMin, fxMax, fyMin, fyMax;
	fxMin = fxMax = fyMin = fyMax = 0;	// 默认0

	fyMin = fxMin = FLT_MAX;			// 设置一对无效值
	fyMax = fxMax = -FLT_MAX;

	for ( int i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_block.m_aMerchs[i]->m_pRealtimePrice != NULL )
		{
			CMerch *pMerch = m_block.m_aMerchs[i];
			double fx, fy;
			if ( CalcMerchXYValue(pMerch, &fx, &fy) )
			{
				fxMax = max(fxMax, fx);
				fxMin = min(fxMin, fx);
				fyMax = max(fyMax, fy);
				fyMin = min(fyMin, fy);
				
// 				if ( fx < fxMin )
// 				{
// 					fxMin = fx;	// 最小值改变
// 				}
// 				if ( fx > fxMax )
// 				{
// 					fxMax = fx;
// 				}
// 				
// 				if ( fy < fyMin )
// 				{
// 					fyMin = fy;
// 				}
// 				if ( fy > fyMax )
// 				{
// 					fyMax = fy;
// 				}
			}
		}
	}

	if ( fyMin > fyMax )
	{
		fyMin = fyMax = 0.0f;	// 没有有效的值, 设置默认值
		m_bValidMinMaxY = false;
	}
	else
	{
		m_bValidMinMaxY = true;
	}
	if ( fxMin > fxMax )
	{
		fxMin = fxMax = 0.0f;
		m_bValidMinMaxX = false;
	}
	else
	{
		m_bValidMinMaxX = true;
	}

	UpdateXYMinMax(fxMin, fxMax, fyMin, fyMax, bNotifyChange);
}

void CIoViewStarry::CalcXYMinMaxAxis( CMerch *pMerch, bool32 bNotifyChange/* = false*/, bool32 *pbMinMaxChanged /*= NULL*/ )
{
	double fxMin, fxMax, fyMin, fyMax;
	if ( m_bValidMinMaxX )
	{
		fxMin = m_fMinX;
		fxMax = m_fMaxX;
	}
	else
	{
		fxMin = FLT_MAX;
		fxMax = -FLT_MAX;
	}
	if ( m_bValidMinMaxY )
	{
		fyMin = m_fMinY;
		fyMax = m_fMaxY;
	}
	else
	{
		fyMin = FLT_MAX;
		fyMax = -FLT_MAX;
	}

	double fx, fy;
	if ( CalcMerchXYValue(pMerch, &fx, &fy) )
	{
		bool32 bChange = false;
		if ( fx < m_fMinX )
		{
			fxMin = fx;
			bChange = true;
		}
		if ( fx > m_fMaxX )
		{
			fxMax = fx;
			bChange = true;
		}

		if ( fy < m_fMinY )
		{
			fyMin = fy;
			bChange = true;
		}
		if ( fy > m_fMaxY )
		{
			fyMax = fy;
			bChange = true;
		}

		if ( fyMin > fyMax )
		{
			fyMin = fyMax = 0.0f;	// 没有有效的值, 设置默认值
			m_bValidMinMaxY = false;
		}
		else
		{
			m_bValidMinMaxY = true;
		}
		if ( fxMin > fxMax )
		{
			fxMin = fxMax = 0.0f;
			m_bValidMinMaxX = false;
		}
		else
		{
			m_bValidMinMaxX = true;
		}
		
		if ( NULL != pbMinMaxChanged )
		{
			*pbMinMaxChanged = bChange;
		}

		if ( bNotifyChange && bChange )
		{
			// 由于这里可能有n个商品在短时间内导致update产生，所以使用一个timer延时处理
			SetTimer(KUpdateXYAxisTimerId, KUpdateXYAxisTimerPeriod, NULL);
		}
	}
}

void CIoViewStarry::UpdateXYMinMax( double fxMin, double fxMax, double fyMin, double fyMax, bool32 bNotifyChange/* = false*/ )
{
	ASSERT( fxMin <= fxMax && fyMin <= fyMax );
	ASSERT( !_isnan(fxMin) && !_isnan(fxMax) && !_isnan(fyMin) && !_isnan(fyMax) );
	ASSERT( _finite(fxMin) && _finite(fxMax) && _finite(fyMin) && _finite(fyMax) );

	m_fMinX = fxMin;
	m_fMaxX = fxMax;
	m_fMinY = fyMin;
	m_fMaxY = fyMax;

	// 重算基本单位 - 现在只有1w
	m_fScaleY = m_fScaleX = 1.0;
	while ( (m_fMaxY / m_fScaleY) >= 10000.0 )
	{
		m_fScaleY *= 10000.0;
	}
	while ( (m_fMaxX / m_fScaleX) >= 10000.0 )
	{
		m_fScaleX *= 10000.0;
	}
// 	if ( m_fMaxY >= 10000.0 )
// 	{
// 		m_fScaleY = 10000.0;
// 	}
// 	if ( m_fMaxX >= 10000.0 )
// 	{
// 		m_fScaleX = 10000.0;
// 	}

	m_mapMerchRect.clear();
    CalcDrawMerchs(m_block.m_aMerchs);		// 有可能部分数据不在显示范围内
	//TRACE(_T("星空画面点计算: %d\r\n"), iCalc);

	if ( bNotifyChange )
	{
		UpdateAllContent();	// 完成计算，刷新显示
	}
}

void CIoViewStarry::SetCoordinate( E_Coordinate eCoor, bool32 bNotifyChange /*= true*/ )
{
	if ( eCoor >= COOR_END )
	{
		return;
	}

	if ( m_eCoordinate != eCoor )
	{
		m_eCoordinate = eCoor;
		if ( bNotifyChange )
		{
			CalcDrawMerchs(m_block.m_aMerchs);		// 重算商品位置
			UpdateAllContent();			// 只需要重新绘制就可以了
		}
	}
}

bool32 CIoViewStarry::CalcXYValueByClientPoint( IN CPoint pt, OUT double *pfx, OUT double *pfy )
{
	switch (m_eCoordinate)
	{
	case COOR_Normal:
		return CalcXYValueByClientPointNoraml(pt, pfx, pfy);
	case COOR_Logarithm:
		return CalcXYValueByClientPointLogarithm(pt, pfx, pfy);
	}
	ASSERT( 0 );
	return false;
}


bool32 CIoViewStarry::CalcClientPointByXYValue( IN double fx, IN double fy, OUT CPoint &ptClient )
{
	switch (m_eCoordinate)
	{
	case COOR_Normal:
		return CalcClientPointByXYValueNormal(fx, fy, ptClient);
	case COOR_Logarithm:
		return CalcClientPointByXYValueLogarithm(fx, fy, ptClient);
	}
	ASSERT( 0 );
	return false;
}

bool32 CIoViewStarry::CalcClientPointByXYValueNormal( IN double fx, IN double fy, OUT CPoint &ptClient )
{
	// 线性计算
	bool bRet = true;
	ptClient.x = -1;
	ptClient.y = -1;
	
	if ( fx >= m_fMinX && fx <= m_fMaxX )
	{
		if ( m_fMaxX - m_fMinX > 0 )
		{
			ptClient.x = (LONG)(m_RectBottomX.left + m_RectBottomX.Width()*(fx - m_fMinX)/ (m_fMaxX-m_fMinX));
		}
		else
		{
			ptClient.x = m_RectBottomX.left;
		}
	}
	else
	{
		bRet = false;
	}
	
	if ( fy >= m_fMinY && fy <= m_fMaxY )
	{
		if ( m_fMaxY - m_fMinY > 0 )
		{
			ptClient.y = (LONG)(m_RectLeftY.bottom - m_RectLeftY.Height()*(fy - m_fMinY)/ (m_fMaxY-m_fMinY));
		}
		else
		{
			ptClient.y = m_RectLeftY.bottom;
		}
	}
	else
	{
		bRet = false;
	}
	
	return bRet;
}

bool32 CIoViewStarry::CalcXYValueByClientPointNoraml( IN CPoint pt, OUT double *pfx, OUT double *pfy )
{
	// 此为线性计算方法
	bool32 bRet = true;	// 仅两个都成功才true
	if ( pfx != NULL ) // 计算X轴
	{
		*pfx = m_fMinX;
		if ( pt.x >= m_RectBottomX.left && pt.x <= m_RectBottomX.right )
		{
			if ( m_RectBottomX.Width() > 0 )
			{
				double fWidth = (double)m_RectBottomX.Width();
				*pfx =  m_fMinX + ((pt.x - m_RectBottomX.left)*(m_fMaxX - m_fMinX)/ fWidth);
			}
			else
			{
				*pfx = m_fMinX;
			}
		}
		else
		{
			bRet = false;
		}
	}
	
	if ( pfy != NULL )
	{
		*pfy = m_fMinY;
		if ( pt.y >= m_RectLeftY.top && pt.y <= m_RectLeftY.bottom )
		{
			if ( m_RectLeftY.Height() > 0 )
			{
				double fHeight = (double)m_RectLeftY.Height();
				*pfy =  m_fMinY + ((m_RectLeftY.bottom - pt.y)*(m_fMaxY - m_fMinY)/ fHeight);
			}
			else
			{
				*pfy = m_fMinY;
			}
		}
		else
		{
			bRet = false;
		}
	}
	
	return bRet;
}

bool32 CIoViewStarry::CalcClientPointByXYValueLogarithm( IN double fx, IN double fy, OUT CPoint &ptClient )
{
	bool bRet = true;
	ptClient.x = -1;
	ptClient.y = -1;
	
	if ( fx >= m_fMinX && fx <= m_fMaxX )
	{
		double fSign1 = 1.0;
		double f1;
		if ( m_fMinX < 0.0 )
		{
			fSign1 = -1.0;
			f1 = -(m_fMinX - 1.0);
			f1 = log10f(f1);
		}
		else
		{
			fSign1 = 1.0;
			f1 = m_fMinX + 1.0;
			f1 = log10f(f1);
		}
		
		double fSign2 = 1.0;
		double f2;
		if ( m_fMaxX < 0.0 )
		{
			fSign2 = -1.0;
			f2 = -(m_fMaxX - 1.0);
			f2 = log10f(f2);
		}
		else
		{
			fSign2 = 1.0;
			f2 = m_fMaxX + 1.0;
			f2 = log10f(f2);
		}

		double fSign3 = 1.0;
		double f3;
		if ( fx < 0.0 )
		{
			fSign3 = -1.0;
			f3 = -(fx - 1.0);
			f3 = log10f(f3);
		}
		else
		{
			fSign3 = 1.0;
			f3 = fx + 1.0;
			f3 = log10f(f3);
		}
		
		double fLength = fSign2*f2 - fSign1*f1;
		//ASSERT( fLength > 0.0 );

		if ( fLength > 0 )
		{
			ptClient.x = (LONG)(m_RectBottomX.left + m_RectBottomX.Width()*(f3*fSign3 - f1*fSign1)/ (fLength));
		}
		else
		{
			ptClient.x = m_RectBottomX.left;
		}
	}
	else
	{
		bRet = false;
	}
	
	if ( fy >= m_fMinY && fy <= m_fMaxY )
	{
		double fSign1 = 1.0;
		double f1;
		if ( m_fMinY < 0.0 )
		{
			fSign1 = -1.0;
			f1 = -(m_fMinY - 1.0);
			f1 = log10f(f1);
		}
		else
		{
			fSign1 = 1.0;
			f1 = m_fMinY + 1.0;
			f1 = log10f(f1);
		}
		
		double fSign2 = 1.0;
		double f2;
		if ( m_fMaxY < 0.0 )
		{
			fSign2 = -1.0;
			f2 = -(m_fMaxY - 1.0);
			f2 = log10f(f2);
		}
		else
		{
			fSign2 = 1.0;
			f2 = m_fMaxY + 1.0;
			f2 = log10f(f2);
		}
		
		double fSign3 = 1.0;
		double f3;
		if ( fy < 0.0 )
		{
			fSign3 = -1.0;
			f3 = -(fy - 1.0);
			f3 = log10f(f3);
		}
		else
		{
			fSign3 = 1.0;
			f3 = fy + 1.0;
			f3 = log10f(f3);
		}
		
		double fLength = fSign2*f2 - fSign1*f1;

		if ( fLength > 0 )
		{
			ptClient.y = (LONG)(m_RectLeftY.bottom - m_RectLeftY.Height()*(f3*fSign3 - f1*fSign1)/ (fLength));
		}
		else
		{
			ptClient.y = m_RectLeftY.bottom;
		}
	}
	else
	{
		bRet = false;
	}
	
	return bRet;
}

bool32 CIoViewStarry::CalcXYValueByClientPointLogarithm( IN CPoint pt, OUT double *pfx, OUT double *pfy )
{
	// 此为对数计算 (x2 - xMin) / (xMax-xMin) = (log(f2) - log(fxMin)) / (log(fMax)-log(fMin)) - 暂时使用这个来代替
	// 由于参数必须要大于0，所以做调整
	
	bool32 bRet = true;	// 仅两个都成功才true
	if ( pfx != NULL ) // 计算X轴
	{
		*pfx = m_fMinX;
		if ( pt.x >= m_RectBottomX.left && pt.x <= m_RectBottomX.right )
		{
			double fSign1 = 1.0;
			double f1;
			if ( m_fMinX < 0.0 )
			{
				fSign1 = -1.0;
				f1 = -(m_fMinX - 1.0);
				f1 = log10f(f1);
			}
			else
			{
				fSign1 = 1.0;
				f1 = m_fMinX + 1.0;
				f1 = log10f(f1);
			}

			double fSign2 = 1.0;
			double f2;
			if ( m_fMaxX < 0.0 )
			{
				fSign2 = -1.0;
				f2 = -(m_fMaxX - 1.0);
				f2 = log10f(f2);
			}
			else
			{
				fSign2 = 1.0;
				f2 = m_fMaxX + 1.0;
				f2 = log10f(f2);
			}

			//ASSERT( fLength > 0.0 );

			if ( m_RectBottomX.Width() > 0 )
			{
				double fWidth = (double)m_RectBottomX.Width();
				*pfx =  fSign1*f1 + ((pt.x - m_RectBottomX.left)*(fSign2*f2 - fSign1*f1)/ fWidth);
				if ( *pfx < 0.0 )
				{
					*pfx = -powf(10, -*pfx) + 1.0;
				}
				else
				{
					*pfx = powf(10, *pfx) - 1.0;
				}
				//ASSERT( *pfx >= m_fMinX && *pfx <= m_fMaxX );	// 这个会被用来计算某点的值，可能会略微超出
				if ( *pfx < m_fMinX )
				{
					*pfx = m_fMinX;
				}
				else if ( *pfx > m_fMaxX )
				{
					*pfx = m_fMaxX;
				}
			}
			else
			{
				*pfx = m_fMinX;
			}
		}
		else
		{
			bRet = false;
		}
	}
	
	if ( pfy != NULL )
	{
		*pfy = m_fMinY;
		if ( pt.y >= m_RectLeftY.top && pt.y <= m_RectLeftY.bottom )
		{
			double fSign1 = 1.0;
			double f1;
			double fM = m_fMinY;
			if ( fM < 0.0 )
			{
				fSign1 = -1.0;
				f1 = -(fM - 1.0);
				f1 = log10f(f1);
			}
			else
			{
				fSign1 = 1.0;
				f1 = fM + 1.0;
				f1 = log10f(f1);
			}
			
			double fSign2 = 1.0;
			double f2;
			fM = m_fMaxY;
			if ( fM < 0.0 )
			{
				fSign2 = -1.0;
				f2 = -(fM - 1.0);
				f2 = log10f(f2);
			}
			else
			{
				fSign2 = 1.0;
				f2 = fM + 1.0;
				f2 = log10f(f2);
			}
			
			//ASSERT( fLength > 0.0 );

			if ( m_RectLeftY.Height() > 0 )
			{
				double fHeight = (double)m_RectLeftY.Height();
				*pfy =  fSign1*f1 + ((m_RectLeftY.bottom - pt.y)*(fSign2*f2 - fSign1*f1)/ fHeight);
				if ( *pfy < 0.0 )
				{
					*pfy = -powf(10, -*pfy) + 1.0;
				}
				else
				{
					*pfy = powf(10, *pfy) - 1.0;
				}
				//ASSERT( *pfy >= m_fMinY && *pfy <= m_fMaxY );
				if ( *pfy < m_fMinY )
				{
					*pfy = m_fMinY;
				}
				else if ( *pfy > m_fMaxY )
				{
					*pfy = m_fMaxY;
				}
				
			}
			else
			{
				*pfy = m_fMinY;
			}
		}
		else
		{
			bRet = false;
		}
	}
	
	return bRet;
}

bool32 CIoViewStarry::GetDivededValueByClientPoint( IN CPoint pt, OUT double *pfx, OUT double *pfy )
{
	if ( CalcXYValueByClientPoint(pt, pfx, pfy) )
	{
		if ( pfx != NULL && m_fScaleX != 0.0 )
		{
			*pfx = (*pfx) / m_fScaleX;
		}

		if ( pfy != NULL && m_fScaleY != 0.0 )
		{
			*pfy = (*pfy) / m_fScaleY;
		}
		return true;
	}
	return false;
}

bool32 CIoViewStarry::CalcMerchXYValue(CMerch *pMerch, OUT double *pfXValue, OUT double *pfYValue)
{
	if ( NULL == pMerch || !m_bMerchsRealTimeInitialized || NULL == pMerch->m_pRealtimePrice )	// 没有初始化完成则不允许计算
	{
		return false;
	}

	if ( m_bUseCacheRealTimePrices )	// 看这个商品有没有载入real price记录
	{
		if ( m_mapMerchValues.count(pMerch) == 0 )
		{
			return false;	// 没有经过vdata update的不计算
		}
	}

	bool32 bValidFlt = true;		// 注意非法的float值
	if ( NULL != pfXValue )
	{
		*pfXValue = CalcMerchValueByAxisType(pMerch, GetXAxisType());
		bValidFlt = _finite(*pfXValue);
		//bValidFlt = !_isnan(*pfXValue);
		//ASSERT( bValidFlt );
	}
	if ( NULL != pfYValue )
	{
		*pfYValue = CalcMerchValueByAxisType(pMerch, GetYAxisType());
		bValidFlt = _finite(*pfYValue) && bValidFlt;
		//bValidFlt = !_isnan(*pfYValue) && bValidFlt;
		//ASSERT( bValidFlt );
	}

	return bValidFlt;
}

bool32 CIoViewStarry::GetMerchRealTimePrice( CMerch *pMerch, OUT CRealtimePrice &RealTimePrice )
{
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ));	// 暂时只允许国内证券
	// 商品类型 (国内期货, 国内证券, 外汇...)
	if ( m_bUseCacheRealTimePrices )	// 缓存
	{
		MerchValueMap::iterator it = m_mapMerchValues.find(pMerch);
		if ( m_mapMerchValues.end() != it )
		{
			RealTimePrice = it->second.m_realTimePrice;
			return true;
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else	// 直接
	{
		ASSERT( NULL != pMerch && pMerch->m_pRealtimePrice != NULL );
		if ( NULL != pMerch && pMerch->m_pRealtimePrice != NULL )
		{
			RealTimePrice = *pMerch->m_pRealtimePrice;
			return true;
		}
	}
	return false;
}

bool32 CIoViewStarry::GetMerchFinanceData( CMerch *pMerch, OUT CFinanceData &FinanceData )
{
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk  ));	// 暂时只允许国内证券
	// 商品类型 (国内期货, 国内证券, 外汇...)
	// 总是不执行该分支，先注释掉
	//if ( 0 && m_bUseCacheRealTimePrices )	// 缓存 - 取消财务数据缓存
	//{
	//	
	//}
	//else	// 直接
	{
		ASSERT( NULL != pMerch && pMerch->m_pFinanceData != NULL );
		if ( NULL != pMerch && pMerch->m_pFinanceData != NULL )
		{
			FinanceData = *pMerch->m_pFinanceData;
			return true;
		}
	}
	return false;
}

double CIoViewStarry::CalcMerchValueByAxisType( CMerch *pMerch, AxisType axisType, bool32 *pBCalced /*= NULL*/ )
{
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ));	// 暂时只允许国内证券
	// 商品类型 (国内期货, 国内证券, 外汇...)
	CRealtimePrice RealtimePrice;
	GetMerchRealTimePrice(pMerch, RealtimePrice);
	CFinanceData  FinanceData;
	memset((void*)(&FinanceData), 0, sizeof(FinanceData));
	GetMerchFinanceData(pMerch, FinanceData);
// 
// 	CReportScheme::ERHPriceNew,					// 最新价
// 		CReportScheme::ERHVolumeTotal,				// 总成交量
// 		CReportScheme::ERHAmount,					// 金额
// 		CReportScheme::ERHRange,					// 幅度
// 		CReportScheme::ERHSwing,					// 振幅
// 		CReportScheme::ERHRate,						// 委比
// 		CReportScheme::ERHVolumeRate,				// 量比
// 		
// 		CReportScheme::ERHBuySellRate,				// 内外比
// 		CReportScheme::ERHChangeRate,				// 换手率
// 		CReportScheme::ERHMarketWinRate,			// 市盈率
// 		
// 		CReportScheme::ERHAllCapital,				// 总股本
// 		CReportScheme::ERHCircAsset,				// 流通股本
// 		CReportScheme::ERHAllAsset,				// 总资产
// 		CReportScheme::ERHFlowDebt,				// 流动负债
// 		CReportScheme::ERHPerFund,					// 每股公积金
// 		
// 		CReportScheme::ERHBusinessProfit,			// 营业利益
// 		CReportScheme::ERHPerNoDistribute,			// 每股未分配
// 		CReportScheme::ERHPerIncomeYear,			// 每股收益(年)
// 		CReportScheme::ERHPerPureAsset,			// 每股净资产
// 		CReportScheme::ERHChPerPureAsset,			// 调整每股净资产
// 		
// 		CReportScheme::ERHDorRightRate,			// 股东权益比// 
// 		CReportScheme::ERHCircMarketValue,			// 流通市值
// 	CReportScheme::ERHAllMarketValue,			// 总市值
	
	double fVal = 0.0;
	double fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
	if ( NULL != pBCalced )
	{
		*pBCalced = true;
	}
	//
	switch ( axisType )
	{
	case CReportScheme::ERHPriceNew:
		{
			fVal = RealtimePrice.m_fPriceNew;
		}
		break;
	case CReportScheme::ERHVolumeTotal:
		{
			fVal = RealtimePrice.m_fVolumeTotal;
		}
		break;
	case CReportScheme::ERHAmount:			
		{
			fVal = RealtimePrice.m_fAmountTotal;
		}
		break;
	case CReportScheme::ERHRange:				// 幅度%
		{
			if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				fVal = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			}
		}
		break;
	case CReportScheme::ERHSwing:				// 振幅
		{
			if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
			{
				fVal = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
			}	
		}
		break;
	case CReportScheme::ERHRate:			  // 委比=〖(委买手数-委卖手数)÷(委买手数+委卖手数)〗×100%
		{
			int32 iBuyVolums  = 0;
			int32 iSellVolums = 0;
			
			for ( int32 i = 0 ; i < 5 ; i++)
			{
				iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
				iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
			}
			
			if ( 0 != (iBuyVolums + iSellVolums) )
			{
				fVal = (double)(iBuyVolums - iSellVolums)*(double)100 / (iBuyVolums + iSellVolums);
			}
		}
		break;
	case CReportScheme::ERHVolumeRate:				// 量比
		{
			fVal = RealtimePrice.m_fVolumeRate;	
		}
		break;
	case CReportScheme::ERHBuySellRate:				// 内外比
		{
			if ( RealtimePrice.m_fSellVolume != 0.0 )
			{
				fVal = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
			}
		}
		break;
	case CReportScheme::ERHChangeRate:				// 换手率
		{
			fVal = RealtimePrice.m_fTradeRate;
		}
		break;
	case CReportScheme::ERHMarketWinRate:				// 换手率
		{
			fVal = RealtimePrice.m_fPeRate;
		}
		break;
	case CReportScheme::ERHCapitalFlow:				// 资金流向
		{
			fVal = RealtimePrice.m_fCapticalFlow;
		}
		break;
	case CReportScheme::ERHAllCapital:				// 总股本
		{
			fVal = FinanceData.m_fAllCapical;
		}
		break;
	case CReportScheme::ERHCircAsset:				// 流通股本
		{
			fVal = FinanceData.m_fCircAsset;
		}
		break;
	case CReportScheme::ERHAllAsset:				// 总资产
		{
			fVal = FinanceData.m_fAllAsset;
		}
		break;
	case CReportScheme::ERHFlowDebt:				// 流动负债
		{
			fVal = FinanceData.m_fFlowDebt;
		}
		break;
	case CReportScheme::ERHPerFund:				// 每股公积金
		{
			fVal = FinanceData.m_fPerFund;
		}
		break;
	case CReportScheme::ERHBusinessProfit:				// 营业利益
		{
			fVal = FinanceData.m_fBusinessProfit;
		}
		break;
	case CReportScheme::ERHPerNoDistribute:				// 每股未分配
		{
			fVal = FinanceData.m_fPerNoDistribute;
		}
		break;
	case CReportScheme::ERHPerIncomeYear:				// 每股收益(年)
		{
			fVal = FinanceData.m_fPerIncomeYear;
		}
		break;
	case CReportScheme::ERHPerPureAsset:				// 每股净资产
		{
			fVal = FinanceData.m_fPerPureAsset;
		}
		break;
	case CReportScheme::ERHChPerPureAsset:				// 调整每股净资产
		{
			fVal = FinanceData.m_fChPerPureAsset;
		}
		break;
	case CReportScheme::ERHDorRightRate:				// 股东权益比
		{
			fVal = FinanceData.m_fDorRightRate;
		}
		break;
	case CReportScheme::ERHCircMarketValue:				// 流通市值
		{
			fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
		}
		break;
	case CReportScheme::ERHAllMarketValue:				// 总市值
		{
			fVal = FinanceData.m_fAllCapical * RealtimePrice.m_fPriceNew;
		}
		break;
	default:
		{
			ASSERT( 0 );
			if ( NULL != pBCalced )
			{
				*pBCalced = true;
			}
		}
		break;
	}
	
	return fVal;	
}

CIoViewStarry::AxisType CIoViewStarry::GetXAxisType(OUT CString *pStrName/* = NULL*/)
{
	if ( NULL != pStrName )
	{
		*pStrName = CReportScheme::GetReportHeaderCnName(m_axisCur.m_eXType);
	}

	return  m_axisCur.m_eXType;
}

CIoViewStarry::AxisType CIoViewStarry::GetYAxisType(OUT CString *pStrName/* = NULL*/)
{
	if ( NULL != pStrName )
	{
		*pStrName = CReportScheme::GetReportHeaderCnName(m_axisCur.m_eYType);
	}
	
	return  m_axisCur.m_eYType;
}

CString CIoViewStarry::GetStarryType()
{
	return  m_axisCur.m_StrName;
}

CString CIoViewStarry::GetUnitString( double fScale )
{
	if ( fScale == 1.0 )
	{
		return _T("");
	}
	else if ( 10000.0 == fScale )
	{
		return _T("[X 万]");
	}
	else if ( 10000.0 * 10000.0 == fScale )
	{
		return _T("[X 亿]");
	}
	else if ( 10000.0 * 10000.0 * 10000.0 == fScale )
	{
		return _T("[X 万亿]");
	}
	else
	{
		CString Str;
		Str.Format(_T("[X%.0f]"), fScale);
		return Str;
	}
}

void CIoViewStarry::UpdateAllContent()
{
	UpdateBlockGrid();
	m_wndTabBlock.Invalidate(TRUE);
	m_wndTabXYSet.Invalidate(TRUE);
	m_GridCtrlZoomDesc.Refresh();
	m_GridCtrlZoomMerchs.Refresh();
	Invalidate(TRUE);	// 已经计算好了，重绘
}

void CIoViewStarry::UpdateBlockGrid()
{
	CString StrText;
	// 星类型
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(GetStarryType());

	//-- block名称
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(m_block.m_StrName);

	// 品种数量
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText.Format(_T("%d"), m_block.m_aMerchs.GetSize());
	pCell->SetText(StrText);

	// 画面星数量
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText.Format(_T("%d"), m_block.m_aMerchs.GetSize());
	pCell->SetText(StrText);

	m_GridCtrl.Refresh();
}

void CIoViewStarry::UpdateBlockPaintMerchCount()
{
	CString StrText;
	// 画面星数量
	CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText.Format(_T("%d"), m_mapMerchRect.size());
	pCell->SetText(StrText);
	
	m_GridCtrl.Refresh();
}

void CIoViewStarry::EmptyGridContent()
{
	m_pMerchLastMouse = NULL;
	m_pMerchLastSelected = NULL;
	m_RectLastZoomUpdate.SetRectEmpty();

	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.SetRowCount(KFixedGridRow);
	m_GridCtrl.SetColumnCount(2);

	m_GridCtrl.SetColumnWidth(0, 80);
	m_GridCtrl.SetColumnWidth(1, 80);
	m_GridCtrl.SetColumnResize(TRUE);
	
	CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
	pCell->SetText(L"星空类型");
	
	//--
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"当前集合");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"品种数量");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"画面星星");
	
	//--
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"当前品种");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"品种代码");
	
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"现价");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"涨幅");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"X轴坐标");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"Y轴坐标");
	
	//-- zoom商品描述
	m_GridCtrlZoomDesc.DeleteNonFixedRows();
	m_GridCtrlZoomDesc.SetRowCount(3);
	m_GridCtrlZoomDesc.SetColumnCount(1);
	
	m_GridCtrlZoomDesc.SetColumnWidth(0, 160);
	m_GridCtrlZoomDesc.SetColumnResize(TRUE);
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(0, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"X轴: ");
	
	
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(1, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"Y轴: ");
	
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(2, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"共筛选: ");

	//-- zoom商品
	m_GridCtrlZoomMerchs.DeleteNonFixedRows();
	m_GridCtrlZoomMerchs.SetColumnCount(2);
	m_GridCtrlZoomMerchs.SetColumnWidth(0, 80);
	m_GridCtrlZoomMerchs.SetColumnWidth(1, 80);

	int32 i = 0;
	for ( i = 0; i < m_GridCtrl.GetColumnCount(); i++)
 	{
		CGridCellSys * pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(0,i);
		pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		
		pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(3,i);
		pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

		pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(9,i);
		pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 	}

	for (i = 0; i < m_GridCtrlZoomDesc.GetColumnCount(); i++)
	{
		CGridCellSys * pTmpCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(0,i);
		pTmpCell->SetCellRectDrawFlag(DR_TOP_SOILD);
 	}

	m_GridCtrl.AutoSizeRows();
	m_GridCtrlZoomDesc.AutoSizeRows();
	m_GridCtrlZoomMerchs.AutoSizeRows();

	m_GridCtrl.Refresh();
	m_GridCtrlZoomDesc.Refresh();
	m_GridCtrlZoomMerchs.Refresh();
}

void CIoViewStarry::UpdateMouseMerch( CPoint ptMouse )
{
	CMerch *pMerch = NULL;
	
	pMerch = GetMouseMerch(ptMouse);
	if ( pMerch == m_pMerchLastMouse )
	{
		return;
	}

	m_pMerchLastMouse = pMerch;
	if ( NULL != m_pMerchLastMouse )	// 将xml merch更换
	{
		OnVDataMerchChanged(m_pMerchLastMouse->m_MerchInfo.m_iMarketId, m_pMerchLastMouse->m_MerchInfo.m_StrMerchCode, m_pMerchLastMouse);
	}

	CString		StrMerchName;
	CString		StrMerchCode;
	CString		StrMerchPrice;
	CString		StrMerchRiseRate;
	CString		StrMerchXValue;
	CString		StrMerchYValue;
	if ( NULL != pMerch && NULL != pMerch->m_pRealtimePrice )
	{
		int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
		double fPrePrice = pMerch->m_pRealtimePrice->m_fPricePrevClose;		// 这里引用的是最新real，是否改为缓存?
		StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
		StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
		double fPriceNew, fRiseRate;
		fPriceNew = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHPriceNew);
		fRiseRate = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHRange);
		StrMerchPrice = Float2SymbolString(fPriceNew, fPrePrice, iSaveDec);
		StrMerchRiseRate = Float2SymbolString(fRiseRate, 0.0, iSaveDec, false, false, true);
		double fx, fy;
		if ( CalcMerchXYValue(pMerch, &fx, &fy) )
		{
			StrMerchXValue = Float2String(fx, iSaveDec, true, false);
			StrMerchYValue = Float2String(fy, iSaveDec, true, false);
		}
	}

	// 商品名
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(StrMerchName);

	// 商品代码
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(StrMerchCode);

	// 商品价
	m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol *pSymbolCell = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
	pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSNone);
	pSymbolCell->SetText(StrMerchPrice);

	// 商品涨幅
	m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
	pSymbolCell = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
	pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSFall);
	pSymbolCell->SetText(StrMerchRiseRate);

	// 商品X
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(StrMerchXValue);

	// 商品Y
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(StrMerchYValue);

	m_GridCtrl.Refresh();
}

void CIoViewStarry::OnMouseMove( UINT nFlags, CPoint point )
{
	UpdateMouseMerch(point);

	UpdateSelectedRange(point);

	DrawMouseMove(point);

	CRect rcZoom;
	if ( GetZoomRect(rcZoom) && rcZoom.PtInRect(point) )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));		// 手型
	}
	else if ( IsPtInChart(point) )
	{
		::SetCursor(NULL);
	}

	TRACKMOUSEEVENT tme = {0};
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;
	_TrackMouseEvent(&tme);

	CIoViewBase::OnMouseMove(nFlags, point);
}

LRESULT CIoViewStarry::OnMouseLeave( WPARAM w, LPARAM l )
{
	DrawMouseMove(CPoint(-1,-1));	
	return Default();
}

LRESULT CIoViewStarry::OnDoInitializeAllRealTimePrice( WPARAM w, LPARAM l )
{
	// 查找所有商品中没有realtimeprice的商品 Or 不管怎么样，刷新所有商品？
// 	m_aMerchsNeedInitializedAll.RemoveAll();
// 	CArray<CMarket *, CMarket *>	aMarkets;
// 	for ( int32 i=0; i < m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() ; i++ )
// 	{
// 		CBreed *pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
// 		if ( NULL == pBreed )
// 		{
// 			continue;
// 		}
// 		for ( int32 j=0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
// 		{
// 			if ( pBreed->m_MarketListPtr[j] == NULL )
// 			{
// 				continue;
// 			}
// 			if ( pBreed->m_MarketListPtr[j]->m_MarketInfo.m_eMarketReportType == ERTStockCn ||
// 				pBreed->m_MarketListPtr[j]->m_MarketInfo.m_eMarketReportType == ERTExp )
// 			{
// 				CMarket *pMarket = pBreed->m_MarketListPtr[j];
// 				for ( int32 k=0; k < pMarket->m_MerchsPtr.GetSize() ; k++ )
// 				{
// 					if ( NULL != pMarket->m_MerchsPtr[k] && NULL == pMarket->m_MerchsPtr[k]->m_pRealtimePrice )
// 					{
// 						m_aMerchsNeedInitializedAll.Add(pMarket->m_MerchsPtr[k]);
// 					}
// 				}
// //				m_aMerchsAll.Append(pMarket->m_MerchsPtr);
// 			}
// 		}
// 	}

	UpdateWindow();

	// 初始化不做任何事情
	if ( m_aMerchsNeedInitializedAll.GetSize() > 0 )
	{
		AddAttendMerchs(m_aMerchsNeedInitializedAll, true);	// 全部：）
		
		if ( m_dlgWait.ShowTimeWaitDlg(true, 30, true, _T("刷新行情数据"), _T("请稍等...")) != IDOK )
		{
			MessageBox(_T("刷新行情数据失败！\r\n您可能会有部分商品数据不能即时浏览"), _T("警告信息"), MB_OK|MB_ICONWARNING);
		}

		ClearAttendMerchs();	// 清除关心数据
		m_aMerchsNeedInitializedAll.RemoveAll();
	}

	m_bMerchsRealTimeInitialized = true;

	RefreshBlock();	// 计算&刷新显示
	// 重新关注当前需要关注的 - TODO
	
	RequestBlockViewData();
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);	// 开始定时推送更新板块商品信息

	return 1;
}

void CIoViewStarry::RequestViewData()
{
	RequestViewData(m_aSmartAttendMerchs); // 请求一起发送	
}

void CIoViewStarry::RequestViewData(CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts)
{
// 	if ( !IsWindowVisible() )
// 	{
// 		return;
// 	}

	CMmiReqRealtimePrice Req;

	CMmiReqPublicFile	ReqFinance;
	ReqFinance.m_ePublicFileType = EPFTF10;

	//static int32 siReqFi = 0;
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	for ( int32 i = 0; i < aRequestSmarts.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = aRequestSmarts.GetAt(i);
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}

		m_mapMerchReqWaitEchoTimes[ SmartAttendMerch.m_pMerch ] = TimeNow;	// 记录请求的时间
		//m_mapMerchLastReqTimes[ SmartAttendMerch.m_pMerch ] = TimeNow;	// 数据回来了再更新
		
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

		// 总是请求财务数据
		if ( SmartAttendMerch.m_pMerch->m_pFinanceData ==NULL || SmartAttendMerch.m_pMerch->m_pFinanceData->m_fAllCapical <= 0.0  )
		{
			// 需要申请其财务数据
			aRequestSmarts[i].m_iDataServiceTypes |= (int)EDSTGeneral;
			ReqFinance.m_iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			ReqFinance.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
			RequestViewData(ReqFinance);

			//TRACE(_T("星空请求财务(%d): %d,%s %s\r\n"),siReqFi++, ReqFinance.m_iMarketId, ReqFinance.m_StrMerchCode, SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCnName);
		}
		else
		{
			// 已有财务数据，由于ViewData的PublicFile模式不会对无更新的进行调用，所以在这里调用下
			// 如果加关注标志并请求数据，就有可能没有机会取消这个关注标志，从而导致申请队列堵塞
			// 要不就是把所有的财务数据在初始化时机申请下来，以后就不管财务数据更新
			// 这样存在财务数据更新不了的bug
			// xl 0505 如果过滤viewdata会给与一次通知
			// OnVDataPublicFileUpdate(aRequestSmarts[i].m_pMerch, EPFTF10);
			//aRequestSmarts[i].m_iDataServiceTypes |= (int)EDSTGeneral;	// 不关注， 只请求数据，这样结果是没通知，但是数据会更新，更换时会显示反应出来
			ReqFinance.m_iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			ReqFinance.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
			RequestViewData(ReqFinance);
		}
	}
	
	if ( aRequestSmarts.GetSize() > 0 )
	{
		RequestViewData(Req);
	}
#ifdef _DEBUG
	{
		CTime TimeDis(TimeNow.GetTime());
		TRACE(_T("星空实际请求数据(%04d-%02d-%02d %02d:%02d:%02d): %d 个\r\n")
			, TimeDis.GetYear(), TimeDis.GetMonth(), TimeDis.GetDay()
 			, TimeDis.GetHour(), TimeDis.GetMinute(), TimeDis.GetSecond(), aRequestSmarts.GetSize());
	}
#endif
}

void CIoViewStarry::RequestViewData( CMmiCommBase &req )
{
	DoRequestViewData(req);
}

void CIoViewStarry::RequestBlockViewData()
{
// 	if ( !IsWindowVisible() )
// 	{
// 		return;
// 	}

	ClearAttendMerchs();	// 仅申请该block的数据
	//AddAttendMerchs(m_block.m_aMerchs, true);
	RequestBlockViewDataByQueue();
}

void CIoViewStarry::AddAttendMerchs( const MerchArray &aMerchs, bool bRequestViewData /*= true */ )
{
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
			smart.m_iDataServiceTypes = EDSTPrice;
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

void CIoViewStarry::AddAttendMerch( CMerch *pMerch, bool bRequestViewData /*= true */ )
{
	CArray<CSmartAttendMerch, CSmartAttendMerch&> aSmart;
	if ( NULL == pMerch )
	{
		return;
	}
	CMerch *pMerchAdd = pMerch;
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
		smart.m_iDataServiceTypes = EDSTPrice;
		aSmart.Add(smart);
		
		m_aSmartAttendMerchs.Append(aSmart);	// 对这个商品保持关注
		if ( bRequestViewData )
		{
			RequestViewData(aSmart);	// 仅申请新商品数据
		}
	}
}

void CIoViewStarry::ClearAttendMerchs()
{
	m_aSmartAttendMerchs.RemoveAll();
}

void CIoViewStarry::RemoveAttendMerch( CMerch *pMerch, E_DataServiceType eSerivceRemove /*= EDSTPrice*/ )
{
	int j=0;
	for ( j=0; j < m_aSmartAttendMerchs.GetSize() ; j++ )
	{
		CMerch *pMerchExist = m_aSmartAttendMerchs[j].m_pMerch;
		if ( pMerch == pMerchExist || 
			(pMerch->m_MerchInfo.m_iMarketId == pMerchExist->m_MerchInfo.m_iMarketId &&
			pMerch->m_MerchInfo.m_StrMerchCode == pMerchExist->m_MerchInfo.m_StrMerchCode) )
		{
			m_aSmartAttendMerchs[j].m_iDataServiceTypes &= ~(int)eSerivceRemove;
			if ( m_aSmartAttendMerchs[j].m_iDataServiceTypes == 0 )
			{
				m_aSmartAttendMerchs.RemoveAt(j);
			}
			return;
		}
	}
	
	m_mapMerchReqWaitEchoTimes.erase(pMerch);	// 移出时间
}

int CIoViewStarry::GetProperChartGridCellSize(OUT double &cx, OUT double &cy, OUT int32 *pColCount/* = NULL*/, OUT int32 *pRowCount /*= NULL*/)
{
	CRect rcChart = m_RectChart;
	cx = cy = 0;

	if ( NULL != pColCount )
	{
		*pColCount = 0;
	}

	if ( NULL != pRowCount )
	{
		*pRowCount = 0;
	}

	//rcChart.right -= KChartRightSkip;
	//rcChart.top -= KChartTopSkip;
	if ( rcChart.Width() <=0 || rcChart.Height() <= 0 )
	{
		return 0;	// 太窄或太矮
	}

	int32 iRowCount = rcChart.Height() / KChartProper;
	int32 iRowLeftHeight = rcChart.Height() % KChartProper;

	if ( iRowCount == 0 )
	{
		cy = rcChart.Height();
		iRowCount = 1;
	}
	else
	{
		double fRowLeftPer = iRowLeftHeight / (double)iRowCount;
		cy = fRowLeftPer + KChartProper;	// 如果row很多导致剩余太多怎么办？
	}

	int32 iColCount = rcChart.Width() / KChartProper;
	int32 iColLeftHeight = rcChart.Width() % KChartProper;
	
	if ( iColCount == 0 )
	{
		cx = rcChart.Width();
		iColCount = 1;
	}
	else
	{
		double fColLeftPer = iColLeftHeight / (double)iColCount;
		cx = fColLeftPer + KChartProper;	// 如果row很多导致剩余太多怎么办？
	}
	
	if ( NULL != pColCount )
	{
		*pColCount = iRowCount;
	}
	
	if ( NULL != pRowCount )
	{
		*pRowCount = iColCount;
	}
	
	return iRowCount * iColCount;
}

void CIoViewStarry::DrawXAxis( CDC &dc )
{
	if ( m_RectBottomX.Width() <=0 || m_RectBottomX.Height() <= 0 )
	{
		return;
	}
	
	// x轴坐标分割点分为5段以下(han)
	const int32	iMaxCount = 5;
	int32 iWidthPer = m_RectBottomX.Width() / iMaxCount;

	double cx, cy;
	cx = cy = 0;
	if ( GetProperChartGridCellSize(cx, cy) <= 0 )
	{
		return;	// 不需要绘制
	}

	int iSaveDC = dc.SaveDC();
	dc.SetTextColor(GetIoViewColor(ESCText));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetIoViewFontObject(ESFSmall));

	//cx 
	int32 iDivideCount = iMaxCount;
	if ( iWidthPer < cx )
	{
		iDivideCount = (int32)(m_RectBottomX.Width() / cx);
		if ( iDivideCount == 0 )
		{
			iDivideCount = 1;
		}
		iWidthPer = (int32)(cx);
	}
	
	// 计算矩形
	CArray<CRect, const CRect &>	aRects;
	int32 i=0;
	for ( i = 0; i < iDivideCount ; i++  )	
	{
		int32 iPos = m_RectBottomX.left + i*iWidthPer;
		aRects.Add(CRect(iPos, m_RectBottomX.top+KMaxMerchRadius, iPos+iWidthPer, m_RectBottomX.bottom));	// 所有top下调一个半径
	}

	// 绘制 
	for ( i = 0; i < aRects.GetSize()-1 ; i++ )	// 最后一个是显示单位的
	{
		double fx = 0;
		if ( GetDivededValueByClientPoint(aRects[i].TopLeft(), &fx, NULL) )
		{
			CString Str;
			Str.Format(_T("%0.2f"), fx);
			dc.DrawText(Str, aRects[i], DT_LEFT |DT_SINGLELINE);
		}
	}

	if ( aRects.GetSize() > 0 )	// 单位
	{
		CRect &rcScale = aRects[aRects.GetSize()-1];
		rcScale.right = m_RectBottomX.right;
		CString Str;
		CString StrType;
		GetXAxisType(&StrType);
		if ( m_fScaleX != 1.0 )
		{
			//Str.Format(_T("%s[X%0.0f]"), StrType, m_fScaleX);
			Str = StrType + GetUnitString(m_fScaleX);
		}
		else
		{
			Str.Format(_T("%s"), StrType.GetBuffer());
		}
		dc.SetTextColor(GetIoViewColor(ESCVolume));
		dc.DrawText(Str, rcScale, DT_RIGHT |DT_SINGLELINE);
	}

	dc.RestoreDC(iSaveDC);
}

void CIoViewStarry::DrawYAxis( CDC &dc )
{
	if ( m_RectLeftY.Width() <=0 || m_RectLeftY.Height() <= 0 )
	{
		return;
	}
	
	// y轴坐标分割点分为10段以下(han)
	const int32	iMaxCount = 10;
	double fHeightPer = m_RectLeftY.Height() / (double)iMaxCount;
	
	double cx, cy;
	cx = cy = 0;
	if ( GetProperChartGridCellSize(cx, cy) <= 0 )
	{
		return;	// 不需要绘制
	}
	
	int iSaveDC = dc.SaveDC();
	dc.SetTextColor(GetIoViewColor(ESCText));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetIoViewFontObject(ESFSmall));
	
	//cx 
	int32 iDivideCount = iMaxCount;
	if ( fHeightPer < 20 )	// 不能小于20
	{
		iDivideCount = m_RectLeftY.Height() / 20;
		if ( iDivideCount == 0 )
		{
			iDivideCount = 1;
		}
		fHeightPer = m_RectLeftY.Height() / (double)iDivideCount;
	}
	
	// 计算矩形 - 从低计算到顶
	CArray<CRect, const CRect &>	aRects;
	int32 i = 0;
	for ( i = 0; i < iDivideCount ; i++  )	
	{
		int32 iPos = m_RectLeftY.bottom - (int32)(i*fHeightPer);
		aRects.Add(CRect(m_RectLeftY.left, (int)(iPos - fHeightPer), m_RectLeftY.right, iPos));
	}
	
	// 绘制 
	for ( i = 0; i < aRects.GetSize()-1 ; i++ )	// 最后一个是显示单位的
	{
		double fy = 0;
		if ( GetDivededValueByClientPoint(aRects[i].BottomRight(), NULL, &fy) )
		{
			CRect rc = aRects[i];
			rc.right -= KChartRightSkip;	// 空10个像素
			CString Str;
			Str.Format(_T("%0.2f"), fy);
			dc.DrawText(Str, rc, DT_RIGHT |DT_BOTTOM |DT_SINGLELINE);
		}
	}
	
	if ( aRects.GetSize() > 0 )	// 单位
	{
		CRect &rcScale = aRects[aRects.GetSize()-1];
		rcScale.top = m_RectLeftY.top;
		rcScale.right -= KMaxMerchRadius;
		CString Str;
		CString StrType;
		GetYAxisType(&StrType);
		if ( m_fScaleY != 1.0 )
		{
			Str = StrType + GetUnitString(m_fScaleY);
		}
		else
		{
			Str.Format(_T("%s"), StrType.GetBuffer());
		}

		dc.SetTextColor(GetIoViewColor(ESCVolume));
		dc.DrawText(Str, rcScale, DT_RIGHT |DT_WORDBREAK |DT_TOP);
	}
	
	dc.RestoreDC(iSaveDC);
}

void CIoViewStarry::DrawChart( CDC &dc )
{
	if ( m_RectChart.Width() <=0 || m_RectChart.Height() <= 0 )
	{
		return;
	}

	double cx, cy;
	cx = cy = 0;
	if ( !GetProperChartGridCellSize(cx, cy) )
	{
		return;
	}

	ASSERT( cx > 0 && cy > 0 );
	// 从左到右，从底到顶
	
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);

	CPen penBorder;
	CPen penDot;
	COLORREF clrLine = GetIoViewColor(ESCChartAxisLine);
	penBorder.CreatePen(PS_SOLID, 0, clrLine);
	penDot.CreatePen(PS_DOT, 0, clrLine);

	CRect rcChart(m_RectChart);

	CRect rcClient(0,0,0,0);	// grid border问题
	GetClientRect(rcClient);
	CPoint ptSeperatorTop, ptSeperatorBottom;	
	ptSeperatorTop.x = rcChart.right + KChartRightSkip - 1;
	ptSeperatorTop.y = rcClient.top;
	ptSeperatorBottom.x = ptSeperatorTop.x;
	ptSeperatorBottom.y = rcClient.bottom;
	dc.SelectObject(&penBorder);
	dc.MoveTo(ptSeperatorTop);
	dc.LineTo(ptSeperatorBottom);

	// 边框两线撑到底
	dc.SelectObject(&penBorder);
	dc.MoveTo(rcChart.left, rcChart.bottom);
	dc.LineTo(rcChart.right + KChartRightSkip, rcChart.bottom);
	dc.MoveTo(rcChart.left, rcChart.bottom);
	dc.LineTo(rcChart.left, rcChart.top - KChartTopSkip);

	// 虚线格子
	dc.SelectObject(&penDot);
	double fPos = rcChart.bottom - cy;
	double fEnd = rcChart.top + KChartProper;
	while ( fPos >= fEnd ) // 第一条不画，最后一条单独画
	{
		dc.MoveTo(rcChart.left, (int)fPos);
		dc.LineTo(rcChart.right, (int)fPos);
		fPos -= cy;
	}
	// 最后一行
	dc.MoveTo(rcChart.left, rcChart.top);
	dc.LineTo(rcChart.right, rcChart.top);
	
	fPos = rcChart.left + cx;
	fEnd = rcChart.right - KChartProper;	// cx或者cy一定会大于Proper
	while ( fPos <= fEnd ) // 第一条不画，最后一条单独画
	{
		dc.MoveTo((int)fPos, rcChart.bottom);
		dc.LineTo((int)fPos, rcChart.top);
		fPos += cx;
	}
	// 最后一列
	dc.MoveTo(rcChart.right, rcChart.bottom);
	dc.LineTo(rcChart.right, rcChart.top);

	// 画最后的点 - -, 现在画所有点, 待处理
	COLORREF	clrRise = GetIoViewColor(ESCRise);
	COLORREF	clrFall = GetIoViewColor(ESCFall);
	COLORREF	clrKeep = GetIoViewColor(ESCKeep);
	CBrush		brhRise, brhFall, brhKeep;
	brhRise.CreateSolidBrush(clrRise);
	brhFall.CreateSolidBrush(clrFall);
	brhKeep.CreateSolidBrush(clrKeep);
	CPen		penRise, penFall, penKeep;
	COLORREF	clrBlack = GetIoViewColor(ESCBackground);
	penRise.CreatePen(PS_SOLID, 0, clrBlack);
	penFall.CreatePen(PS_SOLID, 0, clrBlack);
	penKeep.CreatePen(PS_SOLID, 0, clrBlack);

	MerchArray aMerchs;
	aMerchs.Copy(m_block.m_aMerchs);

	CRect rcRadius(0,0,0,0);
	for ( int32 i=0; i < aMerchs.GetSize() && m_bMerchsRealTimeInitialized ; i++ )
	{
		if ( GetMerchDrawRect(aMerchs[i], rcRadius) )		// 是否考虑计算与绘制分离？？ - 数据大时会上升到100ms + 实现分离
		{
			//m_mapMerchRect[aMerchs[i]] = rcRadius;
			
			CBrush *pBrh = &brhKeep;
			CPen   *pPen = &penKeep;
			if ( NULL != aMerchs[i]->m_pRealtimePrice )
			{
				double fNew, fPrev;
				fNew = aMerchs[i]->m_pRealtimePrice->m_fPriceNew;
				fPrev = aMerchs[i]->m_pRealtimePrice->m_fPricePrevClose;
				if ( fNew > fPrev )
				{
					pBrh = &brhRise;
					pPen = &penRise;
				}
				else if ( fNew < fPrev && fNew != 0.0 )
				{
					pBrh = &brhFall;
					pPen = &penFall;
				}
			}
			
			dc.SelectObject(pPen);
			dc.SelectObject(pBrh);
			dc.RoundRect(rcRadius, CPoint(rcRadius.Width(), rcRadius.Height()));
			
		}
	}

	//if ( m_bBlinkUserStock && m_bBlinkUserStockDrawSpecifyColor ) // 闪烁自选股最后画 - 总画，避免出现不一致
	{
		if ( !m_bBlinkUserStock )
		{
			m_bBlinkUserStockDrawSpecifyColor = false;
		}
		COLORREF clrUserStock = RGB(255,255,0);
		for ( int i=0; i < m_aUserStocks.GetSize() ; i++ )
		{
			DrawMerchAlreadyDrawed(m_aUserStocks[i], dc, m_iRadius, m_bBlinkUserStockDrawSpecifyColor, clrUserStock);	
		}
	}

	dc.RestoreDC(iSaveDC);
}

void CIoViewStarry::DrawMerchAlreadyDrawed( CMerch *pMerch, CDC &dc, int32 iRadius, bool32 bSpecifyColor /* = false*/, COLORREF clrSpecify/* = RGB(255,255,0)*/ )
{
	MerchRectMap::const_iterator it = m_mapMerchRect.find(pMerch);
	if ( it == m_mapMerchRect.end() )
	{
		return;
	}
	CRect rcDrawed = it->second;
	
	// 设置矩形大小
	CPoint ptCenter = rcDrawed.CenterPoint();
	rcDrawed.SetRect(ptCenter.x-iRadius, ptCenter.y-iRadius, ptCenter.x+iRadius, ptCenter.y+iRadius);

	int iSaveDC = dc.SaveDC();
	
	if ( !bSpecifyColor )
	{
		COLORREF	clrRise = GetIoViewColor(ESCRise);
		COLORREF	clrFall = GetIoViewColor(ESCFall);
		COLORREF	clrKeep = GetIoViewColor(ESCKeep);
		COLORREF	clrBlack = GetIoViewColor(ESCBackground);
		CBrush		brhRise, brhFall, brhKeep, brhBk;
		brhRise.CreateSolidBrush(clrRise);
		brhFall.CreateSolidBrush(clrFall);
		brhKeep.CreateSolidBrush(clrKeep);
		brhBk.CreateSolidBrush(clrBlack);
		CPen		penRise, penFall, penKeep;
		penRise.CreatePen(PS_SOLID, 0, clrBlack);
		penFall.CreatePen(PS_SOLID, 0, clrBlack);
		penKeep.CreatePen(PS_SOLID, 0, clrBlack);
		
		CBrush *pBrh = &brhKeep;
		CPen   *pPen = &penKeep;
		if ( NULL != pMerch->m_pRealtimePrice )
		{
			double fNew, fPrev;
			fNew = pMerch->m_pRealtimePrice->m_fPriceNew;
			fPrev = pMerch->m_pRealtimePrice->m_fPricePrevClose;
			if ( fNew > fPrev )
			{
				pBrh = &brhRise;
				pPen = &penRise;
			}
			else if ( fNew < fPrev && fNew != 0.0 )
			{
				pBrh = &brhFall;
				pPen = &penFall;
			}
		}
		dc.SelectObject(pPen);
		dc.SelectObject(pBrh);
		dc.RoundRect(rcDrawed, CPoint(rcDrawed.Width(), rcDrawed.Height()));
	}
	else
	{
		CPen penEdge;
		COLORREF	clrBlack = GetIoViewColor(ESCBackground);
		penEdge.CreatePen(PS_SOLID, 0, clrBlack);
		CBrush	brh;
		brh.CreateSolidBrush(clrSpecify);
		dc.SelectObject(&penEdge);
		dc.SelectObject(&brh);
		dc.RoundRect(rcDrawed, rcDrawed.CenterPoint());
	}
	
	dc.RestoreDC(iSaveDC);
}

void CIoViewStarry::DrawMerch(CMerch *pMerch, CDC *pDC )
{
	// 暂不适用
	return;

	CDC *pDrawDC = pDC;
	if ( NULL == pDrawDC )
	{
		pDrawDC = new CClientDC(this);
	}
	int iSaveDC = pDrawDC->SaveDC();
	
	COLORREF	clrRise = GetIoViewColor(ESCRise);
	COLORREF	clrFall = GetIoViewColor(ESCFall);
	COLORREF	clrKeep = GetIoViewColor(ESCKeep);
	COLORREF	clrBlack = GetIoViewColor(ESCBackground);
	CBrush		brhRise, brhFall, brhKeep, brhBk;
	brhRise.CreateSolidBrush(clrRise);
	brhFall.CreateSolidBrush(clrFall);
	brhKeep.CreateSolidBrush(clrKeep);
	brhBk.CreateSolidBrush(clrBlack);
	CPen		penRise, penFall, penKeep;
	penRise.CreatePen(PS_SOLID, 0, clrBlack);
	penFall.CreatePen(PS_SOLID, 0, clrBlack);
	penKeep.CreatePen(PS_SOLID, 0, clrBlack);

	int32 xPos, yPos;
	
	if ( TestMerchInChart(pMerch, xPos, yPos) )
	{
		// 先抹除以前画的
		MerchRectMap::iterator it = m_mapMerchRect.find(pMerch);
		if ( it != m_mapMerchRect.end() )
		{
			pDrawDC->FillRect(it->second, &brhBk);
			for ( MerchRectMap::const_iterator itRelated = m_mapMerchRect.begin(); itRelated != m_mapMerchRect.end() ; itRelated++ )
			{
				CRect rcIntersect(0,0,0,0);
				if (  itRelated->first != it->first && rcIntersect.IntersectRect(itRelated->second, it->second) )
				{
					if ( NULL != itRelated->first && NULL != itRelated->first->m_pRealtimePrice )
					{
						CBrush *pBrh = &brhKeep;
						CPen   *pPen = &penKeep;
						double fNew, fPrev;
						fNew = itRelated->first->m_pRealtimePrice->m_fPriceNew;
						fPrev = itRelated->first->m_pRealtimePrice->m_fPricePrevClose;
						if ( fNew > fPrev )
						{
							pBrh = &brhRise;
							pPen = &penRise;
						}
						else if ( fNew < fPrev )
						{
							pBrh = &brhFall;
							pPen = &penFall;
						}
						pDrawDC->SelectObject(pPen);
						pDrawDC->SelectObject(pBrh);
						CRgn rgnClipOrg;
						rgnClipOrg.CreateRectRgn(0,0,0,0);
						int iRes = ::GetClipRgn(pDrawDC->m_hDC, (HRGN)rgnClipOrg.m_hObject);
						CRgn rgnRect;
						rgnRect.CreateRectRgnIndirect(rcIntersect);
						pDrawDC->SelectClipRgn(&rgnRect, RGN_AND);
						pDrawDC->RoundRect(itRelated->second, itRelated->second.CenterPoint());
						if ( iRes <= 0 )
						{
							pDrawDC->SelectClipRgn(NULL);
						}
						else
						{
							pDrawDC->SelectClipRgn(&rgnClipOrg);
						}
					}
				}
			}
		}

		CRect rcRadius(xPos-m_iRadius, yPos-m_iRadius, xPos+m_iRadius, yPos+m_iRadius);
		m_mapMerchRect[pMerch] = rcRadius;		// 新显示的总是会显示
		
		CBrush *pBrh = &brhKeep;
		CPen   *pPen = &penKeep;
		if ( NULL != pMerch->m_pRealtimePrice )
		{
			double fNew, fPrev;
			fNew = pMerch->m_pRealtimePrice->m_fPriceNew;
			fPrev = pMerch->m_pRealtimePrice->m_fPricePrevClose;
			if ( fNew > fPrev )
			{
				pBrh = &brhRise;
				pPen = &penRise;
			}
			else if ( fNew < fPrev )
			{
				pBrh = &brhFall;
				pPen = &penFall;
			}
		}
		
		pDrawDC->SelectObject(pPen);
		pDrawDC->SelectObject(pBrh);
		pDrawDC->RoundRect(rcRadius, rcRadius.CenterPoint());
	}

	pDrawDC->RestoreDC(iSaveDC);
	if ( pDC == NULL )
	{
		DEL(pDrawDC);
	}
}

bool32 CIoViewStarry::TestMerchInChart( CMerch *pMerch, OUT int32 &xClient, OUT int32 &yClient )
{
	xClient = yClient = -1;
	double fx, fy;
	if ( CalcMerchXYValue(pMerch, &fx, &fy) )
	{
		CPoint pt;
		if ( CalcClientPointByXYValue(fx, fy, pt) )
		{
			xClient = pt.x;
			yClient = pt.y;
			return true;
		}
	}
	return false;
}

bool32 CIoViewStarry::CalcMerchRect( CMerch *pMerch, OUT CRect &RectMerch )
{
	CPoint ptCenter;
	if ( TestMerchInChart(pMerch, ptCenter.x, ptCenter.y) )
	{
		RectMerch.SetRect(ptCenter.x-m_iRadius, ptCenter.y-m_iRadius, ptCenter.x+m_iRadius, ptCenter.y+m_iRadius);
		return true;
	}
	else
	{
		RectMerch.SetRectEmpty();
	}
	return false;
}

CMerch * CIoViewStarry::GetMouseMerch( CPoint pt )
{
	for (MerchRectMap::reverse_iterator it = m_mapMerchRect.rbegin(); it != m_mapMerchRect.rend() ; it++ )
	{
		if ( it->second.PtInRect(pt) )
		{
			return it->first;
		}
	}
	return NULL;
}

void CIoViewStarry::DrawZoomSelect( CDC &dc, CPoint ptEnd )
{
	COLORREF clrZoomFrame = GetIoViewColor(ESCKeep);	// 框选 - 暂使用平盘
	
	CRect rcZoom;
	bool32 bDrawZoom = true;
	if ( !GetZoomRect(rcZoom) )	// 先获取选定的，失败在计算
	{
		bDrawZoom = CalcZoomRect(ptEnd, rcZoom);
	}
	if ( bDrawZoom )
	{
		CBrush brhZoom;
		brhZoom.CreateSolidBrush(clrZoomFrame);
		dc.FrameRect(rcZoom, &brhZoom);
	}
}

void CIoViewStarry::DrawMouseMove( CPoint ptMouse )
{
	CDC &dcMem = BeginTempDraw();	// 还原镜像

	int iSavedcMem = dcMem.SaveDC();

	if ( (ptMouse.x < 0 || ptMouse.y < 0 ) && !IsPtInChart(m_ptZoomEnd) )	// 如果没有确定就清除
	{
		CancelZoomSelect(ptMouse, false);
	}
	else
	{
		DrawZoomSelect(dcMem, ptMouse);	// 绘制矩形
	}
	DrawMerchAlreadyDrawed(m_pMerchLastSelected, dcMem, m_iRadius*2);

	if ( ptMouse.x < 0 || ptMouse.y < 0 )
	{
		dcMem.RestoreDC(iSavedcMem);
		EndTempDraw();
		m_ptLastMouseMove = ptMouse;
		return;
	}
	
	m_ptLastMouseMove = ptMouse;
	
	int32 yClient, xClient;
	CPoint ptInChart;
	ConvertIntoChartPoint(ptMouse, ptInChart);
	xClient = ptInChart.x;
	yClient = ptInChart.y;
	
	COLORREF clrLineNormal = GetIoViewColor(ESCText);
	COLORREF clrLineEdge = RGB(0,255,255);
	COLORREF clrSlider = RGB(10,100,166);

	COLORREF clrLineX, clrLineY;
	clrLineX = (xClient == m_RectChart.left || xClient == m_RectChart.right) ? clrLineEdge : clrLineNormal;
	clrLineY = (yClient == m_RectChart.top  || yClient == m_RectChart.bottom) ? clrLineEdge : clrLineNormal;
	
	CPen penLineX, penLineY;
	penLineX.CreatePen(PS_SOLID, 0, clrLineX);
	penLineY.CreatePen(PS_SOLID, 0, clrLineY);

	dcMem.SelectObject(&penLineX);
	dcMem.MoveTo(xClient, m_RectChart.top);
	dcMem.LineTo(xClient, m_RectChart.bottom);

	dcMem.SelectObject(&penLineY);
	dcMem.MoveTo(m_RectChart.left, yClient);
	dcMem.LineTo(m_RectChart.right, yClient);

	// 滑块
	double fx, fy;
	CString StrX, StrY;
	CPoint ptAdjust(xClient, yClient);
	if ( GetDivededValueByClientPoint(ptAdjust, &fx, &fy) )
	{
		StrX.Format(_T("%0.2f"), fx);
		StrY.Format(_T("%0.2f"), fy);

		dcMem.SelectObject(GetIoViewFontObject(ESFSmall));
		CSize sizeX = dcMem.GetTextExtent(StrX);
		CSize sizeY = dcMem.GetTextExtent(StrY);

		CRect rcX, rcY;
		rcX = m_RectBottomX;
		rcX.top += 1;
		rcX.left = xClient;
		rcX.right = rcX.left + sizeX.cx + 10;	// + n个像素
		rcX.bottom = rcX.top + sizeX.cy + 5;
		rcY = m_RectLeftY;
		rcY.top = yClient;
		rcY.bottom = rcY.top + sizeY.cy + 5;
		rcY.left = rcY.right - sizeY.cx - 10;

		if ( rcX.right > m_RectBottomX.right )
		{
			rcX.left = m_RectBottomX.right - rcX.Width();
			rcX.right = m_RectBottomX.right;
		}

		
		dcMem.SetTextColor(GetIoViewColor(ESCText));
		dcMem.SetBkMode(TRANSPARENT);
		dcMem.FillSolidRect(rcX, clrSlider);
		dcMem.FillSolidRect(rcY, clrSlider);
		dcMem.DrawText(StrX, rcX, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
		dcMem.DrawText(StrY, rcY, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
	} // 没有值处理？
	
	dcMem.RestoreDC(iSavedcMem);
	EndTempDraw();
}

void CIoViewStarry::OnRButtonDown( UINT nFlags, CPoint point )
{
	CIoViewBase::OnRButtonDown(nFlags, point);
}

void CIoViewStarry::OnUpdateXYAxis()
{
	RefreshBlock();
}

void CIoViewStarry::ChangeAxisType( AxisType eNewXType, AxisType eNewYType )
{
	if ( eNewXType != m_axisCur.m_eXType || eNewYType != m_axisCur.m_eYType )
	{
		m_axisCur.m_eXType = eNewXType;
		m_axisCur.m_eYType = eNewYType;

		// 以前没有财务数据，现在有财务数据了，咋办？ - 现在总是请求财务数据
		RefreshBlock();	// 重计算&刷新
	}
}

CDC& CIoViewStarry::BeginMainDrawDC( CDC &dcSrc )
{
	if ( NULL == m_dcMemPaintDraw.m_hDC )
	{
		m_dcMemPaintDraw.CreateCompatibleDC(&dcSrc);
		int32 iWidth = dcSrc.GetDeviceCaps(HORZRES);
		int32 iHeight = dcSrc.GetDeviceCaps(VERTRES);
		m_bmpImage.DeleteObject();
		m_bmpImage.CreateCompatibleBitmap(&dcSrc, iWidth, iHeight);
		m_dcMemPaintDraw.SelectObject(&m_bmpImage);
	}
	
	return m_dcMemPaintDraw;
}

void CIoViewStarry::MyDCBitBlt( CDC &dcDst, CDC &dcSrc )
{
	if ( NULL == dcSrc.m_hDC || NULL == dcDst.m_hDC )
	{
		return;
	}
	
	CRect rc;
	GetClientRect(rc);
	
	CRect rcBlockTab, rcXYSetTab, rcGrid;
	m_wndTabBlock.GetWindowRect(rcBlockTab);
	ScreenToClient(&rcBlockTab);
	m_wndTabXYSet.GetWindowRect(rcXYSetTab);
	ScreenToClient(&rcXYSetTab);
	
	CRgn rgnBlockTab, rgnXYSetTab, rgnGrid;
	rgnBlockTab.CreateRectRgnIndirect(rcBlockTab);
	rgnXYSetTab.CreateRectRgnIndirect(rcXYSetTab);
	rgnGrid.CreateRectRgnIndirect(m_RectGrid);
	
	CRgn rgnExtClip;
	int iRes2;
	rgnExtClip.CreateRectRgn(0,0,0,0);
	iRes2 = rgnExtClip.CombineRgn(&rgnBlockTab, &rgnXYSetTab, RGN_OR);
	iRes2 = rgnExtClip.CombineRgn(&rgnExtClip, &rgnGrid, RGN_OR);
	
	CRgn rgnClipOrg;
	rgnClipOrg.CreateRectRgn(0,0,0,0);
	int iRes = GetClipRgn(dcDst.m_hDC, (HRGN)rgnClipOrg.m_hObject);
	
	CRgn rgnFull;
	rgnFull.CreateRectRgnIndirect(rc);
	iRes2 = dcDst.SelectClipRgn(&rgnExtClip, RGN_DIFF);
	dcDst.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcSrc, rc.left, rc.top, SRCCOPY);
	if ( iRes > 0 )
	{
		dcDst.SelectClipRgn(&rgnClipOrg);
	}
	else
	{
		dcDst.SelectClipRgn(NULL);
	}
}


void CIoViewStarry::EndMainDrawDC( CDC &dcDst )
{
	MyDCBitBlt(dcDst, m_dcMemPaintDraw);
}

CDC& CIoViewStarry::BeginTempDraw()
{
	if ( NULL == m_dcMemTempDraw.m_hDC )
	{
		CClientDC dc(this);
		m_dcMemTempDraw.CreateCompatibleDC(&dc);
		int32 iWidth = dc.GetDeviceCaps(HORZRES);
		int32 iHeight = dc.GetDeviceCaps(VERTRES);
		m_bmpTempDraw.DeleteObject();
		m_bmpTempDraw.CreateCompatibleBitmap(&dc, iWidth, iHeight);
		m_dcMemTempDraw.SelectObject(&m_bmpTempDraw);
	}

	if ( NULL == m_dcMemPaintDraw.m_hDC )
	{
		return m_dcMemTempDraw;
	}
	
	MyDCBitBlt(m_dcMemTempDraw, m_dcMemPaintDraw);
	return m_dcMemTempDraw;
}

void CIoViewStarry::EndTempDraw()
{
	CClientDC dc(this);
	MyDCBitBlt(dc, m_dcMemTempDraw);
}

void CIoViewStarry::OnSettingChange( UINT uFlags, LPCTSTR lpszSection )
{
	m_dcMemTempDraw.DeleteDC();
	m_dcMemPaintDraw.DeleteDC();
	Invalidate(TRUE);
}

void CIoViewStarry::ChangeRadiusByKey( bool32 bAdd )
{
	int32 iOld = m_iRadius;
	if ( bAdd )
	{
		m_iRadius++;
	}
	else
	{
		m_iRadius--;
	}

	if ( m_iRadius < KMinMerchRadius )
	{
		m_iRadius = KMinMerchRadius;
	}
	else if ( m_iRadius > KMaxMerchRadius )
	{
		m_iRadius = KMaxMerchRadius;
	}

	if ( iOld != m_iRadius )
	{
		CalcDrawMerchs(m_block.m_aMerchs);
		m_aMerchsNeedDraw.RemoveAll();
		Invalidate(TRUE);
	}
}

void CIoViewStarry::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	DoDbClick(point);

	CIoViewBase::OnLButtonDblClk(nFlags, point);
}

void CIoViewStarry::OnLButtonDown( UINT nFlags, CPoint point )
{
	if ( IsPtInChart(m_ptZoomStart) )	// 有开始的选定
	{
		CRect rcZoom;
		if ( GetZoomRect(rcZoom) )	// 结束位置已经确定 - 在区域中按下，则为确认缩放，否则取消这次选定
		{
			if ( rcZoom.PtInRect(point) )
			{
				EnsureZoom();
			}
			else
			{
				CancelZoomSelect(point);
			}
		}
		else	// 确定结束的位置
		{
			EnsureZoomSelect(point);
		}	
	}
	else
	{ // 设定开始位置
		ConvertIntoChartPoint(point, m_ptZoomStart);
		m_ptZoomEnd = CPoint(-1,-1);
	}
	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewStarry::OnLButtonUp( UINT nFlags, CPoint point )
{
	if ( IsPtInChart(m_ptZoomStart) && !IsPtInChart(m_ptZoomEnd) && IsPtInChart(point) )		// 有开始选定, 但是没有确定结束位置, 确定结束位置
	{
		EnsureZoomSelect(point);
	}
	CIoViewBase::OnLButtonUp(nFlags, point);
}

void CIoViewStarry::ConvertIntoChartPoint( IN CPoint pt, OUT CPoint &ptInChart )
{
	ptInChart = pt;
	if ( ptInChart.y < m_RectChart.top )
	{
		ptInChart.y = m_RectChart.top;
	}
	else if ( ptInChart.y > m_RectChart.bottom )
	{
		ptInChart.y = m_RectChart.bottom;
	}
	
	if ( ptInChart.x < m_RectChart.left )
	{
		ptInChart.x = m_RectChart.left;
	}
	else if ( ptInChart.x > m_RectChart.right )
	{
		ptInChart.x = m_RectChart.right;
	}
}

bool32 CIoViewStarry::IsPtInChart( CPoint pt )
{
	CRect rcChart(m_RectChart);
	rcChart.right += 1;
	rcChart.bottom += 1;
	return rcChart.PtInRect(pt);		// 右/底边不算，所以+1补充
}

bool32 CIoViewStarry::GetZoomRect( OUT CRect &rcZoom )
{
	CPoint ptLT, ptRB;
	ptLT.x = MIN(m_ptZoomStart.x, m_ptZoomEnd.x);
	ptLT.y = MIN(m_ptZoomStart.y, m_ptZoomEnd.y);
	ptRB.x = MAX(m_ptZoomStart.x, m_ptZoomEnd.x);
	ptRB.y = MAX(m_ptZoomStart.y, m_ptZoomEnd.y);
	
	rcZoom.SetRect(ptLT, ptRB);

	return IsPtInChart(ptLT) && IsPtInChart(ptRB);
}

bool32 CIoViewStarry::CalcZoomRect( CPoint ptLast, OUT CRect &rcZoom )
{
	CPoint point;
	ConvertIntoChartPoint(ptLast, point);
	CPoint ptLT, ptRB;
	ptLT.x = MIN(m_ptZoomStart.x, point.x);
	ptLT.y = MIN(m_ptZoomStart.y, point.y);
	ptRB.x = MAX(m_ptZoomStart.x, point.x);
	ptRB.y = MAX(m_ptZoomStart.y, point.y);
	
	rcZoom.SetRect(ptLT, ptRB);

	return IsPtInChart(m_ptZoomStart);
}

void CIoViewStarry::EnsureZoom()
{
	CRect rcZoom;
	m_bInZoom = GetZoomRect(rcZoom);
	if ( m_bInZoom )
	{
		double fxMin, fxMax, fyMin, fyMax;
		if ( CalcXYValueByClientPoint(rcZoom.TopLeft(), &fxMin, &fyMax) && CalcXYValueByClientPoint(rcZoom.BottomRight(), &fxMax, &fyMin) )
		{
			CancelZoomSelect(CPoint(-1,-1), false);
			UpdateXYMinMax(fxMin, fxMax, fyMin, fyMax, true);
		}
		else
		{
			CacelZoom();
		}
	}
	else
	{
		CacelZoom();
	}
}

void CIoViewStarry::CacelZoom()
{
	m_ptZoomStart = CPoint(-1, -1);
	m_ptZoomEnd = CPoint(-1, -1);
	m_bInZoom = false;
	RefreshBlock();
}

void CIoViewStarry::CancelZoomSelect( CPoint ptMouse, bool32 bDraw/* = true */) 
{
	m_ptZoomStart = CPoint(-1,-1);
	m_ptZoomEnd = CPoint(-1,-1);
	if ( bDraw )
	{
		DrawMouseMove(ptMouse);		// 临时dc要重绘, 反应变化
	}
}

void CIoViewStarry::EnsureZoomSelect( CPoint ptMouse )
{
	ConvertIntoChartPoint(ptMouse, m_ptZoomEnd);
	CRect rcZoom;
	if ( GetZoomRect(rcZoom) && rcZoom.Width() > KMinMerchRadius && rcZoom.Height() > KMinMerchRadius )
	{
		// 获取具体框选的商品
		UpdateSelectedMerchs();
		DrawMouseMove(ptMouse);
	}
	else
	{
		CancelZoomSelect(ptMouse);
	}
}

void CIoViewStarry::UpdateSelectedRange( CPoint ptMouse )
{
	CString StrXRange(_T("X轴: ")), StrYRange(_T("Y轴: ")), StrMerchsCount;
	MerchArray	aMerchs;
	CRect rcZoom;
	if ( GetZoomRect(rcZoom) || CalcZoomRect(ptMouse, rcZoom) )
	{
		if ( m_RectLastZoomUpdate == rcZoom )
		{
			return;	// 不需要初始化
		}
		
		for ( MerchRectMap::const_iterator it = m_mapMerchRect.begin() ; it != m_mapMerchRect.end() ; it++ )
		{
			if ( rcZoom.PtInRect(it->second.CenterPoint()) )
			{
				aMerchs.Add(it->first);
			}
		}
		
		double fxMin, fxMax, fyMin, fyMax;
		if ( GetDivededValueByClientPoint(rcZoom.TopLeft(), &fxMin, &fyMax) && GetDivededValueByClientPoint(rcZoom.BottomRight(), &fxMax, &fyMin) )
		{
			StrXRange.Format(_T("X轴: %0.2f —— %0.2f"), fxMin, fxMax);
			StrYRange.Format(_T("Y轴: %0.2f —— %0.2f"), fyMin, fyMax);
		}
	}
	else
	{
		rcZoom.SetRectEmpty();
		if ( m_RectLastZoomUpdate == rcZoom )
		{
			return;	// 不需要初始化
		}
	}
	
	m_RectLastZoomUpdate = rcZoom;
	
	StrMerchsCount.Format(_T("共筛选: %d 个商品"), aMerchs.GetSize());
	
	// x范围
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(0, 0);
	pCell->SetText(StrXRange);
	
	//-- y范围
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(1, 0);
	pCell->SetText(StrYRange);
	
	// 筛选品种数量
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(2, 0);
	pCell->SetText(StrMerchsCount);
	
	m_GridCtrlZoomDesc.Refresh();
}

int32 CIoViewStarry::InsertUpdateSelectedMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return -1;
	}

	int32 i = 0;
	int32 iRowCount = m_GridCtrlZoomMerchs.GetRowCount();
	for ( i=0; i < iRowCount ; i++ )
	{
		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
		if ( pCell != NULL && pMerch == (CMerch *)pCell->GetData() )
		{
			m_GridCtrlZoomMerchs.SetFocusCell(i, 0);
			m_GridCtrlZoomMerchs.SetSelectedRange(i, 0, i, 1);
			m_GridCtrlZoomMerchs.EnsureVisible(i, 0);
			UpdateSelectedMerchValue(pMerch);
			return i;	// 已经有该商品，选择并更新下就可以了
		}
	}

	// 没有 - 新插入
	if ( m_mapMerchRect.count(pMerch) == 0 )	// 不可见的不允许添加
	{
		return -1;	
	}

	int32 iRowNew = m_GridCtrlZoomMerchs.InsertRow(pMerch->m_MerchInfo.m_StrMerchCnName);
	ASSERT( -1 != iRowNew );
	if ( -1 != iRowNew )
	{
		if ( m_GridCtrlZoomMerchs.SetItemData(iRowNew, 0, (LPARAM)pMerch) )
		{
			m_aZoomMerch.Add(pMerch);
			m_GridCtrlZoomMerchs.SetFocusCell(iRowNew, 0);
			m_GridCtrlZoomMerchs.SetSelectedRange(iRowNew, 0, iRowNew, 1);
			m_GridCtrlZoomMerchs.EnsureVisible(iRowNew, 0);
			UpdateSelectedMerchValue(pMerch);
		}
		else
		{
			ASSERT( 0 );
		}
	}
	return iRowNew;
}

void CIoViewStarry::UpdateSelectedMerchValue( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}

	int32 i = 0;
	int32 iRowCount = m_GridCtrlZoomMerchs.GetRowCount();
	for ( i=0; i < iRowCount ; i++ )
	{
		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
		if ( pCell != NULL && pMerch == (CMerch *)pCell->GetData() )
		{
			// 更新数据
			CRealtimePrice RealTimePrice;
			if ( !GetMerchRealTimePrice(pMerch, RealTimePrice) )
			{
				ASSERT( 0 );		// 既然显示了，就不应当出现没有的情况
				return;
			}
			CString		StrMerchName;
			CString		StrMerchPrice;
			int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
			double fPrePrice = RealTimePrice.m_fPricePrevClose;		// 这里引用的是最新real，是否改为缓存?
			StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
			double fPriceNew;
			fPriceNew = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHPriceNew);
			StrMerchPrice = Float2SymbolString(fPriceNew, fPrePrice, iSaveDec);
			
			// 商品名
			CGridCellSys * pTmpCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
			pTmpCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pTmpCell->SetText(StrMerchName);
			pTmpCell->SetData((LPARAM)pMerch);
			
			// 商品价
			m_GridCtrlZoomMerchs.SetCellType(i, 1, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pSymbolCell = (CGridCellSymbol *)m_GridCtrlZoomMerchs.GetCell(i, 1);
			pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSNone);
			pSymbolCell->SetText(StrMerchPrice);
			
			m_GridCtrlZoomMerchs.RedrawRow(i);

			return;
		}
	}
}

void CIoViewStarry::UpdateSelectedMerchs()
{
	m_aZoomMerch.RemoveAll();

	CRect rcZoom;
	if ( GetZoomRect(rcZoom) )
	{
		for ( MerchRectMap::const_iterator it = m_mapMerchRect.begin() ; it != m_mapMerchRect.end() ; it++ )
		{
			if ( rcZoom.PtInRect(it->second.CenterPoint()) )
			{
				m_aZoomMerch.Add(it->first);
			}
		}
		
	}
	else
	{
		rcZoom.SetRectEmpty();
	}
	
	// 插入筛选商品
	const int32 iMerchShowSize = m_aZoomMerch.GetSize();	// 需不需要设置最大插入数量

	{
		CAutoLockWindowUpdate lockUpdate(&m_GridCtrlZoomMerchs);
		
		m_GridCtrlZoomMerchs.DeleteNonFixedRows();
		m_GridCtrlZoomMerchs.SetRowCount(iMerchShowSize);
		m_GridCtrlZoomMerchs.SetColumnCount(2);
		
		m_pMerchLastSelected = NULL;		// 商品重插入，清空选择商品
		
		m_GridCtrlZoomMerchs.InsertRowBatchBegin();
		for ( int i=0; i < iMerchShowSize ; i++ )
		{
			CMerch *pMerch = m_aZoomMerch[i];
			if ( NULL != pMerch && NULL != pMerch->m_pRealtimePrice )
			{
				CString		StrMerchName;
				CString		StrMerchPrice;
				int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
				double fPrePrice = pMerch->m_pRealtimePrice->m_fPricePrevClose;		// 这里引用的是最新real，是否改为缓存?
				StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
				double fPriceNew;
				fPriceNew = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHPriceNew);
				StrMerchPrice = Float2SymbolString(fPriceNew, fPrePrice, iSaveDec);
				
				// 商品名
				CGridCellSys * pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetText(StrMerchName);
				pCell->SetData((LPARAM)pMerch);
				
				// 商品价
				m_GridCtrlZoomMerchs.SetCellType(i, 1, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pSymbolCell = (CGridCellSymbol *)m_GridCtrlZoomMerchs.GetCell(i, 1);
				pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSNone);
				pSymbolCell->SetText(StrMerchPrice);
			}
		}
		m_GridCtrlZoomMerchs.InsertRowBatchEnd();
	}
	if ( m_GridCtrlZoomMerchs.GetRowCount() > 0 )
	{
		m_GridCtrlZoomMerchs.EnsureVisible(0,0);
	}

	m_GridCtrlZoomMerchs.AutoSizeRows();
	m_GridCtrlZoomMerchs.Refresh();
}

BOOL  CIoViewStarry::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CRect rcZoom;
	if ( GetZoomRect(rcZoom) && rcZoom.PtInRect(m_ptLastMouseMove) )
	{
		return TRUE;
	}
	else if ( IsPtInChart(m_ptLastMouseMove) )
	{
		return TRUE;
	}
	return CIoViewBase::OnSetCursor(pWnd, nHitTest, message);
}

void CIoViewStarry::OnMerchSelectChange( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	NM_GRIDVIEW *pNMGrid = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pResult )
	{
		*pResult = TRUE;
	}

	if ( NULL != pNMGrid )
	{
		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(pNMGrid->iRow, 0);
		if ( NULL != pCell )
		{
			CMerch *pMerch = (CMerch *)pCell->GetData();
			MerchRectMap::iterator it = m_mapMerchRect.find(pMerch);
			if ( it != m_mapMerchRect.end() )
			{
				DoMerchSelected(pMerch, true);
			}
		}
	}
}

void CIoViewStarry::MultiColumnMenu( IN CMenu &menu )
{
	ASSERT( menu.m_hMenu != NULL && IsMenu(menu.m_hMenu) );
	const int32 iColHeight = 15;
	
	const int32 iColCount = menu.GetMenuItemCount() / iColHeight;
	
	for ( int32 i=0; i <= iColCount; i++ )
	{
		int32 iPos = (i+1) * iColHeight;
		MENUITEMINFO miInfo;
		memset(&miInfo, 0, sizeof(miInfo));
		miInfo.cbSize = sizeof(miInfo);
		miInfo.fMask  = 0x00000100; // MIIM_FTYPE;
		if ( menu.GetMenuItemInfo(iPos, &miInfo, TRUE) )
		{
			miInfo.fType |= MFT_MENUBARBREAK;
		    ::SetMenuItemInfo(menu.m_hMenu, iPos, TRUE, &miInfo);
		}
	}	
}

int32 CIoViewStarry::SelectUserAxisType( CPoint ptMenu, INOUT T_XYAxisType &axisType )
{
	int32 iRet = 0;		// 返回值: 用户选择X(1), Y(2), 不选择(0)

	CStringArray aNames;
	for ( int i = 0; i < KAxisTypesCount ; i++ )
	{
		CString Str = CReportScheme::GetReportHeaderCnName(KAxisTypes[i]);
		if ( !Str.IsEmpty() )
		{
			aNames.Add( Str );
		}
	}
				
	ASSERT( aNames.GetSize() == KAxisTypesCount );
	if ( aNames.GetSize() > 0 )
	{
		CMenu menu;
		menu.CreatePopupMenu();
		CMenu menuX;
		menuX.CreatePopupMenu();
		int i=0;
		for ( i=0; i < aNames.GetSize() ; i++ )
		{
			menuX.AppendMenu(MF_STRING, i+1, aNames[i]);
			if ( KAxisTypes[i] == axisType.m_eXType )
			{
				menuX.CheckMenuItem(i+1, MF_BYCOMMAND |MF_CHECKED);
			}
		}
		MultiColumnMenu(menuX);
		
		CMenu menuY;
		menuY.CreatePopupMenu();
		const int32 iMenuYStart = 1000;
		for ( i=0; i < aNames.GetSize() ; i++ )
		{
			menuY.AppendMenu(MF_STRING, iMenuYStart+i+1, aNames[i]);
			if ( KAxisTypes[i] == axisType.m_eYType )
			{
				menuY.CheckMenuItem(iMenuYStart+i+1, MF_BYCOMMAND |MF_CHECKED);
			}
		}
		MultiColumnMenu(menuY);
		
		menu.AppendMenu(MF_POPUP |MF_STRING, (UINT)menuX.m_hMenu, _T("设定X轴"));
		menu.AppendMenu(MF_POPUP |MF_STRING, (UINT)menuY.m_hMenu, _T("设定Y轴"));
		int32 iSel = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_BOTTOMALIGN |TPM_RETURNCMD |TPM_NONOTIFY, ptMenu.x, ptMenu.y, AfxGetMainWnd());
		if ( iSel > 0 )
		{
			iSel--;
			if (iSel >=0 && iSel < KAxisTypesCount )
			{
				axisType.m_eXType = KAxisTypes[iSel];
				iRet = 1;
			}
			else if(iSel >= iMenuYStart)
			{
				axisType.m_eYType = KAxisTypes[iSel - iMenuYStart];
				iRet = 2;
			}
		}
	}
	return iRet;
}

void CIoViewStarry::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin;
	GetWindowRect(rcWin);
	if ( !rcWin.PtInRect(pos) )
	{
		pos = rcWin.TopLeft();	// 左上
	}

	CRect rcTabXY(0,0,0,0), rcTabBlock(0,0,0,0);
	m_wndTabXYSet.GetWindowRect(rcTabXY);
	m_wndTabBlock.GetWindowRect(rcTabBlock);
	if ( rcTabBlock.PtInRect(pos) || rcTabXY.PtInRect(pos) )	// tab下不能响应
	{
		return;
	}

	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_CHG);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	pPopMenu->RemoveMenu(_T("自定义1"));
	pPopMenu->RemoveMenu(_T("自定义2"));
	pPopMenu->RemoveMenu(_T("自定义3"));
	pPopMenu->RemoveMenu(_T("自定义4"));
	
	//
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);
	pPopMenu->AppendODMenu(L"返回", MF_STRING, IDM_IOVIEWBASE_RETURN);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	// 取消缩放 - 闪烁自选股 -/ 导出板块 -/ 普通坐标 - 对数坐标 -/ 增大星 - 减小星 
	pPopMenu->AppendODMenu(L"取消缩放 SPACE", MF_STRING, IDM_IOVIEW_STARRY_CANCELZOOM);
	if ( !m_bInZoom )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_CANCELZOOM, MF_BYCOMMAND |MF_DISABLED);
	}
	pPopMenu->AppendODMenu(L"闪烁自选", MF_STRING, IDM_IOVIEW_STARRY_BLINKUSER);
	if ( m_bBlinkUserStock )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_BLINKUSER, MF_BYCOMMAND |MF_CHECKED);
	}

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"导出板块", MF_STRING, IDM_IOVIEW_STARRY_EXPORTBLOCK);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"普通坐标", MF_STRING, IDM_IOVIEW_STARRY_COR_NORMAL);
	if ( m_eCoordinate == COOR_Normal )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_COR_NORMAL, MF_BYCOMMAND |MF_CHECKED);
	}
	pPopMenu->AppendODMenu(L"对数坐标", MF_STRING, IDM_IOVIEW_STARRY_COR_LOGARITHM);
	if ( m_eCoordinate == COOR_Logarithm )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_COR_LOGARITHM, MF_BYCOMMAND |MF_CHECKED);
	}

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"增大星星 CTRL NUM+", MF_STRING, IDM_IOVIEW_STARRY_STARRY_ADD);
	if ( KMaxMerchRadius == m_iRadius )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_STARRY_ADD, MF_BYCOMMAND |MF_DISABLED);
	}
	pPopMenu->AppendODMenu(L"减小星星 CTRL NUM-", MF_STRING, IDM_IOVIEW_STARRY_STARRY_SUB);
	if ( KMinMerchRadius == m_iRadius )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_STARRY_SUB, MF_BYCOMMAND |MF_DISABLED);
	}

// 	pPopMenu->AppendODMenu(_T("导出到Excel"),MF_STRING,IDM_USER1);		// 是否需要, 上千个商品开销不起
// 	pPopMenu->AppendODMenu(_T("同步到Excel"),MF_STRING,IDM_USER2);		// 
// 
// 	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// 
	
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	// 插入内容:
	CNewMenu* pIoViewPopMenu = pPopMenu->AppendODPopupMenu(L"插入内容");
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// 关闭内容:
	pPopMenu->AppendODMenu(L"关闭内容", MF_STRING, IDM_IOVIEWBASE_CLOSECUR);	

	// 内容切换:	
	pPopMenu->AppendODMenu(L"内容切换 TAB", MF_STRING, IDM_IOVIEWBASE_TAB);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	
	// 选择商品
	pPopMenu->AppendODMenu(L"选择商品", MF_STRING, IDM_IOVIEWBASE_MERCH_CHANGE);

	pPopMenu->AppendODMenu(L"",MF_SEPARATOR);

	// 全屏/恢复
	pPopMenu->AppendODMenu(L"全屏/恢复 F7", MF_STRING, IDM_IOVIEWBASE_F7);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// 字体放大
	pPopMenu->AppendODMenu(L"字体放大 CTRL+↑", MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER);

	// 字体缩小
	pPopMenu->AppendODMenu(L"字体缩小 CTRL+↓", MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// 分割窗口
	CNewMenu* pSplitWndMenu = pPopMenu->AppendODPopupMenu(L"分割窗口");
	pSplitWndMenu->AppendODMenu(L"添加上视图", MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP);
	pSplitWndMenu->AppendODMenu(L"添加下视图", MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM);
	pSplitWndMenu->AppendODMenu(L"添加左视图", MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT);
	pSplitWndMenu->AppendODMenu(L"添加右视图", MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT);

	// 关闭窗口
	pPopMenu->AppendODMenu(L"关闭窗口", MF_STRING, IDM_IOVIEWBASE_CLOSE);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	
	// 风格设置
	pPopMenu->AppendODMenu(L"风格设置", MF_STRING, ID_SETTING);

	// 版面布局
	pPopMenu->AppendODMenu(L"版面布局", MF_STRING, ID_LAYOUT_ADJUST);

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pos.x, pos.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewStarry::OnMenuStarry( UINT nId )
{
	switch (nId)
	{
	case IDM_IOVIEW_STARRY_CANCELZOOM:
		CacelZoom();
		break;
	case IDM_IOVIEW_STARRY_BLINKUSER:
		BlinkUserStock();
		break;
	case IDM_IOVIEW_STARRY_COR_NORMAL:
		SetCoordinate(COOR_Normal);
		break;
	case IDM_IOVIEW_STARRY_COR_LOGARITHM:
		SetCoordinate(COOR_Logarithm);
		break;
	case IDM_IOVIEW_STARRY_EXPORTBLOCK: // 导出当前显示的星星
		{
			if ( m_mapMerchRect.size() <= 0 )
			{
				MessageBox(L"没有可导出的商品", L"导出板块", MB_ICONWARNING);
				return;
			}
			
			// 打开板块对话框:
			CDlgChooseStockBlock Dlg;
			
			if ( IDOK == Dlg.DoModal() ) 
			{
				T_BlockDesc Block = Dlg.m_BlockFinal;

				T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(Block.m_StrBlockName);
				if ( pBlock == NULL )
				{
					return;
				}
				CString StrTip;
				StrTip.Format(_T("板块[%s]中的数据将被清空，是否继续？"), Block.m_StrBlockName.GetBuffer());
				if ( pBlock->m_aMerchs.GetSize() > 0 && IDNO == MessageBox(StrTip, _T("请选择"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) )
				{
					return;
				}
				
				// 导出到板块中:
				for ( MerchRectMap::const_iterator it = m_mapMerchRect.begin() ; it != m_mapMerchRect.end() ; it++ )
				{
					if ( NULL == it->first )
					{
						continue;
					}
					CUserBlockManager::Instance()->AddMerchToUserBlock(it->first, Block.m_StrBlockName, false);
				}
				
				// 商品多的时候, 提高性能, 只通知和保存文件一次.
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
				
				// 打开相应版块
				CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
				pMainFrame->OnSpecifyBlock(Block);
			}
		}
		break;
	case IDM_IOVIEW_STARRY_STARRY_ADD:
		ChangeRadiusByKey(true);
		break;
	case IDM_IOVIEW_STARRY_STARRY_SUB:
		ChangeRadiusByKey(false);
		break;
	}
}

void CIoViewStarry::DoCmdUIUpdate( CCmdUI *pUI )
{
	if ( NULL == pUI )
	{
		return;
	}
	//pUI->Enable(TRUE);
	switch (pUI->m_nID)
	{
	case IDM_IOVIEW_STARRY_CANCELZOOM:
		{
			if ( !m_bInZoom )
			{
				pUI->Enable(FALSE);
			}
		}
		break;
	case IDM_IOVIEW_STARRY_BLINKUSER:
		break;
	case IDM_IOVIEW_STARRY_COR_NORMAL:
		break;
	case IDM_IOVIEW_STARRY_COR_LOGARITHM:
		break;
	case IDM_IOVIEW_STARRY_EXPORTBLOCK:
		{
			if ( m_mapMerchRect.size() <= 0 )
			{
				pUI->Enable(FALSE);
			}
		}
		break;
	case IDM_IOVIEW_STARRY_STARRY_ADD:
		{
			if ( KMaxMerchRadius == m_iRadius )
			{
				pUI->Enable(FALSE);
			}
		}
		break;
	case IDM_IOVIEW_STARRY_STARRY_SUB:
		{
			if ( KMinMerchRadius == m_iRadius )
			{
				pUI->Enable(FALSE);
			}
		}
		break;
	}
}

void CIoViewStarry::BlinkUserStock()
{
	if ( !m_bBlinkUserStock )	// 启用blink
	{
		m_bBlinkUserStock = true;
		m_bBlinkUserStockDrawSpecifyColor = true;
		SetTimer(KDrawBlinkUserStockTimerId, KDrawBlinkUserStockTimerPeriod, NULL);
	}
	else
	{
		KillTimer(KDrawBlinkUserStockTimerId);
		m_bBlinkUserStockDrawSpecifyColor = false;
		DrawBlinkUserStock();	// 还原显示
		m_bBlinkUserStock = false;
	}
}

void CIoViewStarry::UpdateUserBlockMerchs()
{
	m_aUserStocks.RemoveAll();
	typedef std::set<CMerch *> SetMerch;
	SetMerch setMerch;
	CArray<T_Block, T_Block &> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);
	int i = 0;
	for ( i=0; i < aBlocks.GetSize() ; i++ )
	{
		T_Block &block = aBlocks[i];
		for ( int j=0; j < block.m_aMerchs.GetSize() ; j++ )
		{
			setMerch.insert(block.m_aMerchs[j]);
		}
	}
	for ( i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
	{
		CMerch *pMerch = m_block.m_aMerchs[i];
		if ( setMerch.count(pMerch) )
		{
			m_aUserStocks.Add(pMerch);
		}
	}
}

void CIoViewStarry::DrawBlinkUserStock(CDC *pDC/* = NULL*/)
{
	if ( NULL == pDC )
	{
		CDC &dcMem = m_dcMemPaintDraw;		// 直接使用main dc
		COLORREF clrUserStock = RGB(255,255,0);
		for ( int i=0; i < m_aUserStocks.GetSize() ; i++ )
		{
			DrawMerchAlreadyDrawed(m_aUserStocks[i], dcMem, m_iRadius, m_bBlinkUserStockDrawSpecifyColor, clrUserStock);
		}
		if ( m_aUserStocks.GetSize() > 0 )
		{
			DrawMouseMove(m_ptLastMouseMove);
		}
	}
	//InvalidateRect(m_RectChart);
}

LRESULT CIoViewStarry::OnMsgHotKey( WPARAM w, LPARAM l )
{
	CHotKey *pHotKey = (CHotKey *)w;
	if (NULL == pHotKey)
	{
		return 0;
	}
	if ( pHotKey->m_eHotKeyType != EHKTMerch )
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), UM_HOTKEY, w, l);	// 非商品信息交给mainframe处理
		return 0;
	}
	
	CHotKey HotKey = *pHotKey;
	DEL(pHotKey);
	pHotKey = &HotKey;
	
	if (EHKTMerch == pHotKey->m_eHotKeyType)
	{
		CString StrHotKey ,StrName, StrMerchCode;
		int32   iMarketId;
		StrHotKey		= pHotKey->m_StrKey;
		StrMerchCode	= pHotKey->m_StrParam1;
		StrName			= pHotKey->m_StrSummary;
		iMarketId		= pHotKey->m_iParam1;
		
		CMerch* pMerch = NULL;
		
		if ( m_pAbsCenterManager && m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
		{
			if ( NULL == pMerch )
			{
				return 0;
			}
			
			// 添加到选择商品列表

			if ( InsertUpdateSelectedMerch(pMerch) != -1 )
			{
				DoMerchSelected(pMerch, true);
			}
			
			//
			CMPIChildFrame* pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			CGGTongView* pGGTView  = GetParentGGtongView();
			pFrame->SetActiveGGTongView(pGGTView);
			((CMainFrame*)AfxGetMainWnd())->SetHotkeyTarget(this);		
		}
	}
	// 板块快捷未处理
	return 1;
}

void CIoViewStarry::DoMerchSelected(CMerch *pMerch,  bool32 bNeedDraw /*= true*/ )
{
	m_pMerchLastSelected = pMerch;
	if ( bNeedDraw )
	{
		DrawMouseMove(m_ptLastMouseMove);	// 反应变化
	}
	// 更新一下xml merch
	if ( NULL != m_pMerchLastSelected )
	{
		OnVDataMerchChanged(m_pMerchLastSelected->m_MerchInfo.m_iMarketId, m_pMerchLastSelected->m_MerchInfo.m_StrMerchCode, m_pMerchLastSelected);
	}
}

void CIoViewStarry::DoDbClick( CPoint ptClick )
{
	DoDbClick(GetMouseMerch(ptClick));
}

void CIoViewStarry::DoDbClick( CMerch *pMerchClick )
{
	if ( pMerchClick != NULL )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->OnShowMerchInChart(pMerchClick, this);
	}
}

void CIoViewStarry::OnGridDblClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NM_GRIDVIEW *pNMGrid = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pResult && pNMGrid->iRow < m_GridCtrlZoomMerchs.GetFixedRowCount() )
	{
		*pResult = TRUE;
	}
	
	if ( NULL != pNMGrid )
	{
		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(pNMGrid->iRow, 0);
		if ( NULL != pCell )
		{
			CMerch *pMerch = (CMerch *)pCell->GetData();
			MerchRectMap::iterator it = m_mapMerchRect.find(pMerch);
			if ( it != m_mapMerchRect.end() )
			{
				DoDbClick(pMerch);
			}
		}
	}
}

bool32 CIoViewStarry::CalcDrawMerch( CMerch *pMerch )
{
	CRect rcRadius;
	//ASSERT( m_mapRealTimePrices.count(pMerch) );
	if ( CalcMerchRect(pMerch, rcRadius) )		// 数据变更时，调用该函数计算商品坐标等	
	{
		m_mapMerchRect[pMerch] = rcRadius;
		ASSERT( m_mapMerchRect.size() <= m_block.m_aMerchs.GetSize() );
		return true;
	}
	return false;
}

int32 CIoViewStarry::CalcDrawMerchs( const MerchArray &aMerchs )
{
	int32 iCalc = 0;
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( CalcDrawMerch(aMerchs[i]) )
		{
			iCalc++;
		}
	}
	return iCalc;
}

bool32 CIoViewStarry::GetMerchDrawRect( CMerch *pMerch, OUT CRect &rcMerch )
{
	MerchRectMap::iterator it = m_mapMerchRect.find(pMerch);
	if ( it != m_mapMerchRect.end() )
	{
		rcMerch = it->second;
		return true;
	}
	return false;
}

void CIoViewStarry::RequestBlockViewDataByQueue()
{
	m_lstMerchsWaitToRequest.RemoveAll();

	DWORD dwTime = timeGetTime();

	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

	int32 iIgnored = 0;

	if ( m_block.m_aMerchs.GetSize() <= 200 )
	{
		// 数据量小的直接请求就是，不考虑过滤
		for ( int i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_block.m_aMerchs[i];
			m_lstMerchsWaitToRequest.AddTail(pMerch);
		}
	}
	else
	{
		// 数据量大，则过滤掉部分数据
		for ( int i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_block.m_aMerchs[i];
			MerchValueMap::iterator itValue = m_mapMerchValues.find(pMerch);
			bool32 bAdd = true;
			if ( itValue != m_mapMerchValues.end() )
			{
				// 有这个商品的数据
				// 如果这个商品是在若干时间内请求过的，则这一次不再请求
				// 可能会有时间跨度刚好跨天了的问题 - 
				MerchReqTimeMap::iterator itLastTime = m_mapMerchLastReqTimes.find(pMerch);
				if ( itLastTime != m_mapMerchLastReqTimes.end()
					&& TimeNow < itLastTime->second+CGmtTimeSpan(0,0,1, 0) )
				{
					// 没有超过若干请求间隔时间的
					if ( !IsNeedFinanceDataType()
						|| (NULL!=pMerch->m_pFinanceData&&pMerch->m_pFinanceData->m_fAllCapical>100) )
					{
						// 不需要财务数据，或者已经有了财务数据，那就过滤这次的数据请求
						// 可能出现财务数据更新了，但是这边不允许这次请求，导致星空图里面的财务数据永远得不到更新
						bAdd = false;
						++iIgnored;
					}
				}
			}
			if ( bAdd )
			{
				m_lstMerchsWaitToRequest.AddTail(pMerch);
			}
		}
	}
	
	RequestBlockQueueNext();

	TRACE(_T("星空请求板块第一次数据(%d/%d): %d ms\n"), iIgnored, m_block.m_aMerchs.GetSize(), timeGetTime()-dwTime);
}

void CIoViewStarry::RequestBlockQueueNext()
{
	KillTimer(KReqNextTimerId);

	const int32 iMaxAttendMerchSize = 20;	// 保持关注的商品不大于

	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	MerchArray aMerchs;
	int32 i = 0;
	for ( i = 0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
	{
		MerchReqTimeMap::iterator it = m_mapMerchReqWaitEchoTimes.find(m_aSmartAttendMerchs[i].m_pMerch);
		if ( it != m_mapMerchReqWaitEchoTimes.end() )
		{
			if ( TimeNow - it->second > CGmtTimeSpan(0, 0, 1, 0) )
			{
				// 这个商品的请求时间超时了, 不在关心这个商品
				// 只有等待下一轮的循环才可以有机会请求数据
				aMerchs.Add(m_aSmartAttendMerchs[i].m_pMerch);
			}
			else
			{
				break;	// 后面的时间比它短
			}
		}
	}
	for ( i = 0 ; i < aMerchs.GetSize() ; i++ )
	{
		RemoveAttendMerch(aMerchs[i]);		
	}
	
	int32 iAdded = 0;
	MerchArray aMerchsNeedReq;
	int32 iAttendSize = m_aSmartAttendMerchs.GetSize();
	while ( iAttendSize < iMaxAttendMerchSize && !m_lstMerchsWaitToRequest.IsEmpty() )
	{
		CMerch *pMerch = m_lstMerchsWaitToRequest.RemoveHead();

		// 只发送在开收盘内且星空图接收的数据并不是在收盘后得到的
		if ( IsMerchNeedReqData(pMerch) )
		{
			aMerchsNeedReq.Add(pMerch);
			++iAdded;
			++iAttendSize;
		}
	}
	AddAttendMerchs(aMerchsNeedReq, true);

	if ( iAdded > 0 )
	{
		SetTimer(KTimerIdCheckReqTimeOut, KTimerPeriodCheckReqTimeOut, NULL);	// 准备检查数据超时
	}

	if ( iAdded > 0 && m_lstMerchsWaitToRequest.IsEmpty() )
	{
		m_bFirstReqEnd = true;
	}

 	CTime TimeDis(TimeNow.GetTime());
 	TRACE(_T("星空请求数据(%04d-%02d-%02d %02d:%02d:%02d)剩余: %d 个\r\n")
 		, TimeDis.GetYear(), TimeDis.GetMonth(), TimeDis.GetDay()
 		, TimeDis.GetHour(), TimeDis.GetMinute(), TimeDis.GetSecond(), m_lstMerchsWaitToRequest.GetCount());
}

void CIoViewStarry::RecreateBlockTab()
{
	bool32 bOldInit = m_bMerchsRealTimeInitialized;		// 防止切换响应
	m_bMerchsRealTimeInitialized = false;
	m_wndTabBlock.DeleteAll();

	ASSERT( m_aBlockCollectionIds.GetSize() > 0 );
	for ( int i=0; i < m_aBlockCollectionIds.GetSize() ; i++ )
	{
		CString Str;
		CBlockConfig::Instance()->GetCollectionName(m_aBlockCollectionIds[i], Str);
		if ( Str.GetLength() > 2 && Str.Right(2) == _T("板块") )
		{
			Str.Delete(Str.GetLength()-2, 2);
		}
		m_wndTabBlock.Addtab(Str + _T("▲"), Str, Str);
	}	

	// 根据id选择当前的block
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
	if ( NULL != pBlock )
	{
		for ( int i=0; i < m_aBlockCollectionIds.GetSize() ; i++ )
		{
			if ( m_aBlockCollectionIds[i] == pBlock->m_blockCollection.m_iBlockCollectionId )
			{
				m_wndTabBlock.SetCurtab(i);
			}
		}
	}

	RecalcLayout();
	Invalidate(TRUE);

	m_bMerchsRealTimeInitialized = bOldInit;
}

void CIoViewStarry::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
	if ( EPFTF10 != ePublicFileType )
	{
		return;
	}

	if ( NULL == pMerch || NULL == pMerch->m_pFinanceData )
	{
		ASSERT( 0 );
		return;
	}


	RemoveAttendMerch(pMerch, EDSTGeneral);		// 取消对财务数据的关注

	if ( IsNeedFinanceDataType() )	// 当前需要财务数据
	{
		bool32 bChange = false;
		CalcXYMinMaxAxis(pMerch, true, &bChange);		// 看是否该商品的数据可以改变坐标轴最大最小值，或者仅仅只需要更新下显示
		if ( !bChange )
		{
			m_aMerchsNeedDraw.Add(pMerch);
			SetTimer(KDrawChartMerchTimerId, KDrawChartMerchTimerPeriod, NULL);	// 定时绘制
		}
	}
}

bool32 CIoViewStarry::IsFinanceDataType( CReportScheme::E_ReportHeader eHeader )
{
	if ( eHeader >= CReportScheme::ERHAllCapital && eHeader <= CReportScheme::ERHDorRightRate )	// 目前提供的财富数据范围 - 流通市值 总市值 需要实时报价，归为报价处理
	{
		return true;
	}
	return false;
}

void CIoViewStarry::RequestBlockQueueNextAsync()
{
	SetTimer(KReqNextTimerId, KReqNextTimerPeriod, NULL);
}

bool32 CIoViewStarry::IsNeedFinanceDataType()
{
	return IsFinanceDataType(m_axisCur.m_eXType) || IsFinanceDataType(m_axisCur.m_eYType);
}

bool32 CIoViewStarry::IsMerchNeedReqData( CMerch *pMerch )
{
	if ( NULL == pMerch || NULL == m_pAbsCenterManager)
	{
		return false;
	}
	CMarketIOCTimeInfo IOCTime;
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	if ( pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo) )
	{
		// 没有发送过该商品请求的记录或者该请求在今日收盘之前发送的，则认为本次应当发送请求
		MerchReqTimeMap::iterator it = m_mapMerchLastReqTimes.find(pMerch);
		if ( it == m_mapMerchLastReqTimes.end() )
		{
			return true;
		}

		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeStart = IOCTime.m_aOCTimes[i];
			CGmtTime TimeEnd = IOCTime.m_aOCTimes[i+1]+CGmtTimeSpan(0,0,2,0);
			if ( TimeStart <= it->second && TimeEnd > it->second  )
			{
				return true;	// 位于开收盘之内
			}
		}
		return false;
	}
	else
	{
		ASSERT( 0 );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

T_MerchAxisNode::T_MerchAxisNode()
{
	m_pMerch = NULL;
	m_eHeader = CReportScheme::ERHPriceNew;
}

T_MerchAxisNode::operator double()
{
	ASSERT( NULL != m_pMerch && (m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp  ));	// 暂时只允许国内证券
	// 商品类型 (国内期货, 国内证券, 外汇...)
	ASSERT( NULL != m_pMerch && m_pMerch->m_pRealtimePrice != NULL );
	CRealtimePrice RealtimePrice;
	if ( NULL != m_pMerch && m_pMerch->m_pRealtimePrice != NULL )
	{
		RealtimePrice = *m_pMerch->m_pRealtimePrice;
	}
	
	double fVal = 0.0;
	double fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
	//
	switch ( m_eHeader )
	{
	case CReportScheme::ERHPriceNew:
		{
			fVal = RealtimePrice.m_fPriceNew;
		}
		break;
	case CReportScheme::ERHVolumeTotal:
		{
			fVal = RealtimePrice.m_fVolumeTotal;
		}
		break;
	case CReportScheme::ERHAmount:			
		{
			fVal = RealtimePrice.m_fAmountTotal;
		}
		break;
	case CReportScheme::ERHRange:				// 幅度%
		{
			if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				fVal = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			}
		}
		break;
	case CReportScheme::ERHSwing:				// 振幅
		{
			if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
			{
				fVal = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
			}	
		}
		break;
	case CReportScheme::ERHRate:			  // 委比=〖(委买手数-委卖手数)÷(委买手数+委卖手数)〗×100%
		{
			int32 iBuyVolums  = 0;
			int32 iSellVolums = 0;
			
			for ( int32 i = 0 ; i < 5 ; i++)
			{
				iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
				iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
			}
			
			if ( 0 != (iBuyVolums + iSellVolums) )
			{
				fVal = (double)(iBuyVolums - iSellVolums)*(double)100 / (iBuyVolums + iSellVolums);
			}
		}
		break;
	case CReportScheme::ERHVolumeRate:				// 量比
		{
			fVal = RealtimePrice.m_fVolumeRate;	
		}
		break;
	default:
		break;
	}
	
	return fVal;
}


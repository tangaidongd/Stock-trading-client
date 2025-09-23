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

const int32 KUpdatePushMerchsTimerId		= 100005;	// ÿ�� n ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs			= 1000 * 60 * 5;

const int32 KUpdateXYAxisTimerId			= 100006;	// ��Ʒ���ݱ仯��������������
const int32 KUpdateXYAxisTimerPeriod		= 200;		// n ms�����������

const int32 KDrawChartMerchTimerId			= 100007;
const int32 KDrawChartMerchTimerPeriod		= 1000;		// n ms��������ݱ仯�˵���Ʒ

const int32 KDrawBlinkUserStockTimerId		= 100008;
const int32 KDrawBlinkUserStockTimerPeriod	= 750;		// n ms�������ѡ��

const int32 KReqNextTimerId					= 100009;	// ��ʱ������һ������
const int32 KReqNextTimerPeriod				= 150;		// n ms

const int32 KTimerIdCheckReqTimeOut			= 100010;	// ��������Ƿ�ʱ
const int32 KTimerPeriodCheckReqTimeOut		= 1000*60;		// n ms

const int32 KTimerIdSyncReq					= 100011;	// ͬ�����ݵ�����, nʱ������һ������
const int32 KTimerPeriodSyncReq				= 1000*2;	// n ms

const int32 KTimerIdReqAttendMerch			= 100012;	// ����Ҫ��ע����Ʒ����
const int32 KTimerPeriodReqAttendMerch		= 200;	

const UINT  KNIDBlockSelTab = 0x9998;
const UINT  KNIDXYSelTab = 0x9999;

const int32 KFixedGridRow = 10;	// �̶���

const int32	KMaxMerchRadius  = 5;
const int32 KMinMerchRadius  = 2;

static	const int32 KChartProper = 80;
static	const int32 KChartTopSkip = KMaxMerchRadius * 2;
static	const int32 KChartRightSkip = KMaxMerchRadius * 2;

// �������ֶ�
static const CIoViewStarry::AxisType KAxisTypes[] = 
{
	CReportScheme::ERHPriceNew,					// ���¼�
	CReportScheme::ERHVolumeTotal,				// �ܳɽ���
	CReportScheme::ERHAmount,					// ���
	CReportScheme::ERHRange,					// ����
	CReportScheme::ERHSwing,					// ���
	CReportScheme::ERHRate,						// ί��
	CReportScheme::ERHVolumeRate,				// ����

	CReportScheme::ERHBuySellRate,				// �����
	CReportScheme::ERHChangeRate,				// ������
	CReportScheme::ERHMarketWinRate,			// ��ӯ��

	CReportScheme::ERHAllCapital,				// �ܹɱ�
	CReportScheme::ERHCircAsset,				// ��ͨ�ɱ�
	CReportScheme::ERHAllAsset,				// ���ʲ�
	CReportScheme::ERHFlowDebt,				// ������ծ
	CReportScheme::ERHPerFund,					// ÿ�ɹ�����
	
	CReportScheme::ERHBusinessProfit,			// Ӫҵ����
	CReportScheme::ERHPerNoDistribute,			// ÿ��δ����
	CReportScheme::ERHPerIncomeYear,			// ÿ������(��)
	CReportScheme::ERHPerPureAsset,			// ÿ�ɾ��ʲ�
	CReportScheme::ERHChPerPureAsset,			// ����ÿ�ɾ��ʲ�
	
	CReportScheme::ERHDorRightRate,			// �ɶ�Ȩ���// 
	CReportScheme::ERHCircMarketValue,			// ��ͨ��ֵ
	CReportScheme::ERHAllMarketValue,			// ����ֵ
};
static const int32 KAxisTypesCount = sizeof(KAxisTypes) / sizeof(KAxisTypes[0]);

// XML�ֶ�
const char* KStrViewStarryBlockIdName		= "BLOCKID";		// �򿪵İ���������
const char* KStrViewStarryBlockName			= "BLOCKNAME";		// �򿪵İ������ - �ּ�����������Ψһ��
const char* KStrViewStarryXAxis				= "XAXIS";			// ��ǰ����x�� - ĿǰΪReportHeader
const char* KStrViewStarryYAxis				= "YAXIS";			// ��ǰ����y�� - ĿǰΪReportHeader
const char* KStrViewStarryXAxisUser			= "XAXIS_USER";		// �û��Զ���X�� - report header
const char* KStrViewStarryYAxisUser			= "YAXIS_USER";		// �Զ���Y - report header
const char* KStrViewStarryRadius			= "RADIUS";			// Բ�뾶
const char* KStrViewStarryCoordinate		= "COORDINATE";		// ���� - ��ͨ/����

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

	m_axisUserCur.m_StrName = _T("�Զ���");

	m_bMerchsRealTimeInitialized = false;

	m_ptLastMouseMove = CPoint(-1,-1);
	m_iBlockId = 0;

	m_bInZoom = false;
	m_ptZoomStart = m_ptZoomEnd = CPoint(-1,-1);

	m_bUseCacheRealTimePrices	= true;		// ʹ�û����ʵʱ���ۼ��� & ����

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
			// ����뾶
			ChangeRadiusByKey(true);
			return TRUE;
		}
		else if ( VK_SUBTRACT == pMsg->wParam && IsCtrlPressed())
		{
			// ��С�뾶
			ChangeRadiusByKey(false);
			return TRUE;
		}
		else if ( VK_ESCAPE == pMsg->wParam)
		{
			// �����Zoom�ˣ���ԭ
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
			// ��������base����
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

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewStarry::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// ��ǰ��Ʒ��Ϊ�յ�ʱ��
	if (NULL != pMerch)
	{
		// �����������Ʒ�����ݣ���Ӧ����Ʒ����������
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
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
}

//
void CIoViewStarry::OnVDataForceUpdate()
{
	RequestViewData();	// �������й��ĵ�����
	RequestBlockQueueNext();
	//RequestBlockViewDataByQueue();	// ��Ʒ�����������·��� - ��ʱ���µ�û���¾Ͳ������ˣ�����
}

void CIoViewStarry::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	//DWORD dwTime = timeGetTime();

	//ASSERT( NULL != pMerch->m_pRealtimePrice );

	if ( NULL != pMerch && NULL != pMerch->m_pRealtimePrice )		// ���������ͼֻ�ܻ�ȡһ�����ݵĸ��£����Ը����ݱ�������Ч��
	{
		m_mapMerchValues[pMerch].m_realTimePrice = *pMerch->m_pRealtimePrice;

		if ( m_aMerchsNeedInitializedAll.GetSize() > 0 || !m_bMerchsRealTimeInitialized )		// ��ʼ����������״̬
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
			// �����������������
			
			RemoveAttendMerch(pMerch);	// �Ƴ����� - ��ʱ���ᱣ�ֹ�ע

			m_mapMerchLastReqTimes[pMerch] = m_pAbsCenterManager->GetServerTime();	// �����������ʱ��

			// ����ǵ�ǰ����Ʒ����Ի������Ӱ��
			if ( m_MerchsCurrent.count(pMerch) )
			{
				if ( !m_bFirstReqEnd )
				{
					// ����Ǹð��ĵ�һ���������ݣ�������������һ�ε���Ʒ�����򽻸�sync��ʱ��ȥ������һ��
					if ( m_aSmartAttendMerchs.GetSize() < 3 )
					{
						// �ȵ��󲿷���Ʒ�������ˣ��ŷ���һ�ε�����Ƶ������������������
						RequestBlockQueueNextAsync();	// ����ڰ���һ������ʱ�������ǿյģ����Բ��ص�������������
					}
				}
				
				UpdateSelectedMerchValue(pMerch);	// ������ѡ����Ʒ������
				
				bool32 bChange = false;
				CalcXYMinMaxAxis(pMerch, true, &bChange);
				if ( !bChange )
				{
					m_aMerchsNeedDraw.Add(pMerch);
					SetTimer(KDrawChartMerchTimerId, KDrawChartMerchTimerPeriod, NULL);	// ��ʱ����
				}
			}
		}
	}

	//TRACE(_T("�ǿ�RealTime: %d ms\r\n"), timeGetTime()-dwTime);
}

// �������г����з����仯
// �������г����з����仯
void CIoViewStarry::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	// �������޹�
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
	
	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// ���������Լ���ҵ��ڵ�
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

	// �Ȱ�id���ң�����ҵ��˾Ͳ��������Ʋ�����
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
		// ���ֿջ���ȡ��ѡ�ɣ�
	}

	if ( NULL != pBlock )
	{
		m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
		pBlock->m_blockInfo.FillBlock(m_block);
		ASSERT( m_block.m_StrName == StrBlockName );
	}
	
	// xy��
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

	// �뾶
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
	
	// ��������
	E_Coordinate eCoor = m_eCoordinate;
	pcAttrValue = pTiXmlElement->Attribute(KStrViewStarryCoordinate);
	if ( NULL != pcAttrValue )
	{
		eCoor = (E_Coordinate)atoi(pcAttrValue);
	}

	// ���xml����������Ӱ��
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
		m_wndTabXYSet.SetCurtab(m_aXYTypes.GetSize()-1);		// ʣ�µ�ȫ����Ϊ�Զ���
	}

	SetCoordinate(eCoor);

	SetNewBlock(m_block);

	m_wndTabBlock.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);
	m_wndTabXYSet.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);

#ifdef _DEBUG
	TRACE(_T("�ǿ�ͼfromXml: %d ms\r\n"), timeGetTime()-dwTime);
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
	// �������block class, name, axis x,y axis user x, y , radius, coordinate
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

	// ���ﲻ�������� - ��ʱ��������
	
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

	//ModifyStyle(0, WS_CLIPCHILDREN);		// �Ӵ���clip

	InitialIoViewFace(this);	// ���Ӧ���Ƶ�ioview base�������

	//����Tab �� // ��ʼ��tab��
	m_wndTabBlock.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,KNIDBlockSelTab);
	m_wndTabBlock.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_wndTabBlock.SetUserCB(this);
	m_wndTabBlock.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	ASSERT( CBlockConfig::Instance()->IsInitialized() );
	CBlockConfig::Instance()->GetCollectionIdArray(m_aBlockCollectionIds);
	// ɾ���ڻ���� & �յ�
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
		if ( Str.GetLength() > 2 && Str.Right(2) == _T("���") )
		{
			Str.Delete(Str.GetLength()-2, 2);
		}
		m_wndTabBlock.Addtab(Str + _T("��"), Str, Str);
	}

	m_wndTabXYSet.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,KNIDXYSelTab);
	m_wndTabXYSet.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_wndTabXYSet.SetUserCB(this);
	m_wndTabXYSet.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	m_wndTabXYSet.Addtab(_T("����"), _T("����"), _T("�۸�/��ͨ�ɱ�"));
	m_wndTabXYSet.Addtab(_T("����"), _T("����"), _T("�۸�/ÿ�ɾ��ʲ�"));
	m_wndTabXYSet.Addtab(_T("������"), _T("������"), _T("�Ƿ�/����"));
	m_wndTabXYSet.Addtab(_T("����"), _T("����"), _T("�Ƿ�/��ͨ�ɱ�"));
	m_wndTabXYSet.Addtab(_T("�ǻ�"), _T("�ǻ�"), _T("�Ƿ�/������"));
	m_wndTabXYSet.Addtab(_T("�Զ����"), _T("�Զ���"), _T("�Զ���"));
	T_XYAxisType	xyType;
	m_aXYTypes.Add(xyType);	// ����
	xyType.m_eYType = CReportScheme::ERHPriceNew;
	xyType.m_eXType = CReportScheme::ERHPerPureAsset;
	xyType.m_StrName = _T("����");		
	m_aXYTypes.Add(xyType);		// ����
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHVolumeRate;
	xyType.m_StrName = _T("������"); // ������
	m_aXYTypes.Add(xyType);
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHCircAsset;
	xyType.m_StrName = _T("����");		
	m_aXYTypes.Add(xyType);		// ����
	xyType.m_eYType = CReportScheme::ERHRange;
	xyType.m_eXType = CReportScheme::ERHChangeRate;
	xyType.m_StrName = _T("�ǻ�");		
	m_aXYTypes.Add(xyType);		// �ǻ�

	m_aXYTypes.Add(T_XYAxisType());	// �Զ���Ĭ��ѡĬ��
	m_axisCur = m_aXYTypes[0];		// Ĭ��ѡ��
	m_axisUserCur = m_axisCur;		// Ĭ���Զ���ѡ��
	m_axisUserCur.m_StrName = _T("�Զ���");

	ASSERT( m_aXYTypes.GetSize() == m_wndTabXYSet.GetCount() );
	
	// �������ݱ�� - block
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 0x20205);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrl.SetBorderColor(GetIoViewColor(ESCChartAxisLine)); // border��ʾ������
	
	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(false);
	m_GridCtrl.EnableSelection(FALSE);
	m_GridCtrl.SetColumnCount(2);
	
	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// ��zoom����
	m_GridCtrlZoomDesc.Create(CRect(0, 0, 0, 0), this, 0x20206);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrlZoomDesc.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrlZoomDesc.SetBorderColor(GetIoViewColor(ESCChartAxisLine));
	
	// ��ʾ����ߵķ��
	m_GridCtrlZoomDesc.ShowGridLine(false);
	m_GridCtrlZoomDesc.EnableSelection(FALSE);

	m_GridCtrlZoomDesc.SetColumnCount(1);
	
	// ��������
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, FALSE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(FALSE, TRUE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(TRUE, FALSE)->SetFont(pFontSmall);
	m_GridCtrlZoomDesc.GetDefaultCell(TRUE, TRUE)->SetFont(pFontSmall);

	// ��zoom��Ʒ��ʾ
	m_GridCtrlZoomMerchs.Create(CRect(0, 0, 0, 0), this, 0x20207);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrlZoomMerchs.SetBkColor(GetIoViewColor(ESCBackground));
	//m_GridCtrlZoomMerchs.SetBorderColor(GetIoViewColor(ESCChartAxisLine));
	
	// ��ʾ����ߵķ��
	m_GridCtrlZoomMerchs.ShowGridLine(false);
	m_GridCtrlZoomMerchs.EnableSelection(TRUE);
	m_GridCtrlZoomMerchs.SetSingleRowSelection(TRUE);
	m_GridCtrlZoomMerchs.SetColumnCount(2);
	
	// ��������
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrlZoomMerchs.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	m_wndScrollV.Create(WS_CHILD|WS_VISIBLE, this, CRect(0,0,0,0), 0x20208);
	m_GridCtrlZoomMerchs.SetScrollBar(NULL, &m_wndScrollV);
	m_wndScrollV.SetOwner(&m_GridCtrlZoomMerchs);
	
	EmptyGridContent();

	T_Block block;
	SetNewBlock(block);		// ��block

	CBlockConfig::Instance()->AddListener(this);

	PostMessage(UM_STARRY_INITIALIZE_ALLMERCHS);		// ˢ��������Ʒ������Ϣ��Ŀǰ��������û��ע���İ취�����������Ҫ�޸�

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

	// ��ռ�������
	m_RectLeftY.SetRectEmpty();
	m_RectChart.SetRectEmpty();
	m_RectBottomX.SetRectEmpty();
	m_RectGrid.SetRectEmpty();
	
	// �Ҳ��grid
	int32 iGridWidth = 160;
	m_RectGrid = rcClient;
	m_RectGrid.left = m_RectGrid.right - iGridWidth;
	{ // ����Grid�ռ�
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
		
		CRect rcGrid(m_RectGrid); // ��
		m_GridCtrl.MoveWindow(rcGrid.left, rcGrid.top, iGridWidth, iGridBlockHeight);
		m_GridCtrl.ExpandColumnsToFit();
		
		rcGrid.top += iGridBlockHeight;	// ��
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
		int32 iTabBlockSeperatePos = rcClient.left + iWidth/2; // ��һ�����ػ��ָ��� grid��border
		int32 iTabWidth1 = iTabBlockSeperatePos - rcClient.left;
		int32 iTabWidth2 = rcClient.right - iTabBlockSeperatePos-1;

		CSize sizeTabBlock = m_wndTabBlock.GetWindowWishSize();
		CSize sizeTabXYSet = m_wndTabXYSet.GetWindowWishSize();
		CRect rcTabBlock(rcClient.left, rcClient.bottom-iTabHeight, rcClient.left + iTabWidth1, rcClient.bottom);
		CRect rcTabXYSet(iTabBlockSeperatePos, rcClient.bottom-iTabHeight, iTabBlockSeperatePos+iTabWidth2, rcClient.bottom);
		if ( sizeTabBlock.cx + sizeTabXYSet.cx <= iTabWidth1 + iTabWidth2 )	// ��������tab�ָ����
		{
			rcTabBlock.right = rcTabBlock.left + sizeTabBlock.cx;
			rcTabXYSet.left = rcTabXYSet.right -  sizeTabXYSet.cx;
		}
		else
		{
			// ����������С�ģ�ʣ�µ�ȫ�����
			if ( sizeTabXYSet.cx < iTabWidth2 )	// �п���
			{
				rcTabXYSet.left = rcTabXYSet.right -sizeTabXYSet.cx;
				rcTabBlock.right = rcTabXYSet.left - 2;
			}
			else if ( sizeTabBlock.cx < iTabWidth1 )
			{
				rcTabBlock.right = rcTabBlock.left + sizeTabBlock.cx;
				rcTabXYSet.left = rcTabBlock.right + 2;
			}
			// ��������ش󣬾�ƽ��
		}
		m_wndTabBlock.MoveWindow(rcTabBlock);
		m_wndTabXYSet.MoveWindow(rcTabXYSet);	// ��Ҫ����
		
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
			
			// ������Ҫ����skip΢�� - ���ǻ���ʱ��Ҫ�ŵ���
			m_RectLeftY.top += KChartTopSkip;
			m_RectChart.top += KChartTopSkip;
			m_RectChart.right -= KChartRightSkip;
			m_RectBottomX.right -= KChartRightSkip;
		}
	}
	
	if ( !rcGridOld.EqualRect(m_RectGrid) )	// ����仯�������
	{
		CalcDrawMerchs(m_block.m_aMerchs);	// ������Ƶ�
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
		if ( IsFinanceDataType(m_axisCur.m_eXType) && IsFinanceDataType(m_axisCur.m_eYType) )	// ���ǲ������ݾͲ���Ҫ��̬������
		{
		}
		else if ( m_lstMerchsWaitToRequest.IsEmpty() )
		{
			// ������ǰ�������Ѿ���������˲����¸���
			//RequestBlockViewData();	// ������������
			// ȡ��֤��������ʱ���Ƿ�Ϊ������ʱ��
			// ���������ʱ�䣬��鿴����б��е�ǰn����Ʒ��
			// ������Щ��Ʒ�ĸ���ʱ���Ƿ���˽�������ʱ��
			// ���ˣ��򲻷�����ε�timer����
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
					bool32 bAllHasData = true;	// Ĭ��������Ʒ����������
					for ( int32 i=0; i < iMaxMerchCount ; ++i )
					{
						MerchReqTimeMap::iterator it = m_mapMerchLastReqTimes.find(m_block.m_aMerchs[i]);
						if ( it == m_mapMerchLastReqTimes.end()
							|| TimeEnd > it->second )
						{
							// û�з��͹�����Ʒ����ļ�¼���߸�����������֮ǰ���͵ģ�����Ϊ����Ӧ����������
							bAllHasData = false;
							break;
						}
					}
					bUpdate = !bAllHasData;
				}
			}
			if ( bUpdate )
			{
				// ��Ҫ���£���ʼͬ��
				m_bFirstReqEnd = true;
				RequestBlockViewDataByQueue();	// ����������
				SetTimer(KTimerIdSyncReq, KTimerPeriodSyncReq, NULL);
			}
		}
	}
	else if ( KTimerIdSyncReq == nIDEvent )
	{
		// ͬ������
		RequestBlockQueueNext();
		if ( m_lstMerchsWaitToRequest.IsEmpty() )
		{
			KillTimer(nIDEvent);
		}
	}
	else if ( KUpdateXYAxisTimerId == nIDEvent )
	{
		if ( !m_bInZoom )	// ����״̬�������Զ�����������
		{
			RefreshBlock();	// �������Ѿ��仯���ؼ���&��ʾ
		}
		KillTimer(nIDEvent);
	}
	else if ( KDrawChartMerchTimerId == nIDEvent )
	{
		if ( m_aMerchsNeedDraw.GetSize() > 0 )
		{
			CalcDrawMerchs(m_aMerchsNeedDraw);
			Invalidate(TRUE);	// ��paint���� - paint������¸�����Ʒ��λ�ü���
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
		// �����ʱ��Ϣ������֤��reqnext�Ѿ���һ��ʱ��û�е�����
		bool32 bReqTimeOut = !m_lstMerchsWaitToRequest.IsEmpty();
		RequestBlockQueueNextAsync();	// ����next
		KillTimer(KTimerIdCheckReqTimeOut);
		if ( bReqTimeOut )
		{
			TRACE(_T("�ǿ�ͼ��Ʒ��������ʱ����\r\n"));
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
		m_aMerchsNeedDraw.RemoveAll();		// ����paint�Ͳ���Ҫ���Ƶȴ����Ƶ���Ʒ��
	}

	DrawActiveFlag(dcMem);
	DrawXAxis(dcMem);
	DrawYAxis(dcMem);
	DrawChart(dcMem);

	EndMainDrawDC(dc);
#ifdef _DEBUG
	XLTraceFile::GetXLTraceFile(StrLogFile).TraceWithTimestamp(_T("   end main draw"), TRUE);
#endif
	DrawMouseMove(m_ptLastMouseMove);	// ������ʱ

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

	// ��ȡ���а��
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
		// blockѡ��
		
		//if ( iTab == 0 )	// ������
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
		if ( iTab == m_aXYTypes.GetSize() - 1 )	// �Զ���������
		{
			int32 iSelCur = m_wndTabXYSet.GetCurtab();
			if ( iSelCur == iTab )	// ��������£�������tabsel���������ﴦ��
			{
				CRect rcTab(0,0,0,0);
				if ( !m_wndTabXYSet.GetTabRect(rcTab, iTab) )
				{
					return true;	// ������
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

	return true; // ���л�
}

void CIoViewStarry::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( eInitializeType == BeforeInitialize )
	{
		// �ȵ���ʼ����ɺ���
	}
	else if ( eInitializeType == Initialized )
	{
		// ���°��������Ϣ
		CBlockConfig::Instance()->GetCollectionIdArray(m_aBlockCollectionIds);
		
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
		if ( NULL != pBlock )
		{
			pBlock->m_blockInfo.FillBlock(m_block);
			SetNewBlock(m_block, true);
		}
		// ��������ڸð����������Ǳ�����ǰ����Ϣ��

		RecreateBlockTab();
	}
}

void CIoViewStarry::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	if ( BlockModified == eNotifyType && iBlockId == m_iBlockId )
	{
		// ������ݱ�� - ����
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		ASSERT( NULL != pBlock );
		if ( NULL != pBlock )
		{
			pBlock->m_blockInfo.FillBlock(m_block);
			SetNewBlock(m_block, true);	// �ȴ���ʱ��ˢ������, ����������Ҫǿ��ˢ��
		}
	}
	else if ( BlockDeleted == eNotifyType && iBlockId == m_iBlockId )
	{
		// ��鱻ɾ���� - ��ô��
		m_block.m_aMerchs.RemoveAll();
		SetNewBlock(m_block, false);	// �������Ʒ����
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
// 	// ��Ʒ�仯�������Ϣ�仯����鱻ɾ��
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
// 				m_block.m_StrName = stBlock.m_StrName;	// ���Ʊ�� - ������ڹ�ע��������
// 				break;
// 			}
// 		}
// 	}
// 
// 	if ( NULL != CUserBlockManager::Instance()->GetBlock(m_block.m_StrName) )	// ���ڹ�ע�İ�����Զ���� - �п��ܸð���Ѿ���ɾ����
// 	{
// 		CBlockClassManager::FillUserBlock(m_block.m_StrName, m_block);
// 		// ���Ƿ�����Ʒ���ӻ���ɾ�� - ��Ҫϸ�ڴ������ڼ򵥵���new block�������ᷢ�ʹ�������
// 		SetNewBlock(m_block, false);		// ��ʱ���������ݣ��ȴ���ʱ��ȥͬ���¼������Ʒ
// 	}
// 	else
// 	{
// 		if ( m_block.m_eHeadType == ERTCustom )	// ��鱻ɾ���� - ������ʾ��
// 		{
// 		}
// 		UpdateUserBlockMerchs();		// ����һ����Щ���Զ�����Ʒ
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

	if ( !m_bMerchsRealTimeInitialized )	// û�г�ʼ�����ʱ��sel��Ϣ�ǲ���Ҫ�����
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
			if ( m_aXYTypes.GetSize()-1 == iSel )	// �Զ��� - ������tab�л�������
			{
				m_axisCur.m_StrName = m_axisUserCur.m_StrName;
				ChangeAxisType(m_axisUserCur.m_eXType, m_axisUserCur.m_eYType);	// �Ƚ���Ĭ�ϵ��л�, Ȼ�󵯳��˵�����

				CRect rcTab(0,0,0,0);
				if ( !m_wndTabXYSet.GetTabRect(rcTab, iSel) )
				{
					return;	// ������
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
	
	T_Block blockTemp = block;	// ���ݸ�block����ֹ��ͬ��m_block�������
	m_block = block;	// �������꣬�ػ棬����grid
	m_block.m_aMerchs.RemoveAll();
	int i=0;
	// ���˹�����Ʒ ָ�� �������Ĳ�Ҫ
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
		if ( m_block.m_aMerchs[i] != NULL )	// �������ͼ��xml merch����һ��
		{
			OnVDataMerchChanged(m_block.m_aMerchs[i]->m_MerchInfo.m_iMarketId, m_block.m_aMerchs[i]->m_MerchInfo.m_StrMerchCode, m_block.m_aMerchs[i]);
			break;
		}
	}

	// ���µ�ǰ��Ʒ����
	m_MerchsCurrent.clear();
	for ( i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_block.m_aMerchs[i] != NULL )	
		{
			m_MerchsCurrent.insert(m_block.m_aMerchs[i]);
		}
	}

	m_aMerchsNeedDraw.RemoveAll();	// �����Ƶĵ����

	m_bFirstReqEnd = false;
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);	// ���ð�鶨ʱ����
	KillTimer(KTimerIdSyncReq);	// ����ͬ��timer
	KillTimer(KTimerIdCheckReqTimeOut);	// ������ʱ���timer
	m_lstMerchsWaitToRequest.RemoveAll();	// ��ǰ����������Ӧ�����
	if ( m_bMerchsRealTimeInitialized && bRequestData )	// ��ʼ��ȫ������ʱ�����������ݣ���ʱ���ݻᱻ����- ò�Ʋ����ڹ����ʱ���������ݣ����򷵻�NULL real price
	{
		RequestBlockViewData();	// �������� - ��Ϊû������ʼ����
	}

	UpdateUserBlockMerchs();

	CancelZoomSelect(CPoint(-1,-1), false);
	CacelZoom();		// ȡ������

	UpdateSelectedMerchs();	// ȡ��ѡ�����Ʒ�б�

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
			return true;	// �Ѿ���
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
	CalcXYAllMinMaxAxis(true);	// �������� & ˢ����ʾ
}

void CIoViewStarry::CalcXYAllMinMaxAxis(bool32 bNotifyChange/* = false*/)
{
	double fxMin, fxMax, fyMin, fyMax;
	fxMin = fxMax = fyMin = fyMax = 0;	// Ĭ��0

	fyMin = fxMin = FLT_MAX;			// ����һ����Чֵ
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
// 					fxMin = fx;	// ��Сֵ�ı�
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
		fyMin = fyMax = 0.0f;	// û����Ч��ֵ, ����Ĭ��ֵ
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
			fyMin = fyMax = 0.0f;	// û����Ч��ֵ, ����Ĭ��ֵ
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
			// �������������n����Ʒ�ڶ�ʱ���ڵ���update����������ʹ��һ��timer��ʱ����
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

	// ���������λ - ����ֻ��1w
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
    CalcDrawMerchs(m_block.m_aMerchs);		// �п��ܲ������ݲ�����ʾ��Χ��
	//TRACE(_T("�ǿջ�������: %d\r\n"), iCalc);

	if ( bNotifyChange )
	{
		UpdateAllContent();	// ��ɼ��㣬ˢ����ʾ
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
			CalcDrawMerchs(m_block.m_aMerchs);		// ������Ʒλ��
			UpdateAllContent();			// ֻ��Ҫ���»��ƾͿ�����
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
	// ���Լ���
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
	// ��Ϊ���Լ��㷽��
	bool32 bRet = true;	// ���������ɹ���true
	if ( pfx != NULL ) // ����X��
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
	// ��Ϊ�������� (x2 - xMin) / (xMax-xMin) = (log(f2) - log(fxMin)) / (log(fMax)-log(fMin)) - ��ʱʹ�����������
	// ���ڲ�������Ҫ����0������������
	
	bool32 bRet = true;	// ���������ɹ���true
	if ( pfx != NULL ) // ����X��
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
				//ASSERT( *pfx >= m_fMinX && *pfx <= m_fMaxX );	// ����ᱻ��������ĳ���ֵ�����ܻ���΢����
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
	if ( NULL == pMerch || !m_bMerchsRealTimeInitialized || NULL == pMerch->m_pRealtimePrice )	// û�г�ʼ��������������
	{
		return false;
	}

	if ( m_bUseCacheRealTimePrices )	// �������Ʒ��û������real price��¼
	{
		if ( m_mapMerchValues.count(pMerch) == 0 )
		{
			return false;	// û�о���vdata update�Ĳ�����
		}
	}

	bool32 bValidFlt = true;		// ע��Ƿ���floatֵ
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
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ));	// ��ʱֻ�������֤ȯ
	// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
	if ( m_bUseCacheRealTimePrices )	// ����
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
	else	// ֱ��
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
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk  ));	// ��ʱֻ�������֤ȯ
	// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
	// ���ǲ�ִ�и÷�֧����ע�͵�
	//if ( 0 && m_bUseCacheRealTimePrices )	// ���� - ȡ���������ݻ���
	//{
	//	
	//}
	//else	// ֱ��
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
	ASSERT( NULL != pMerch && (pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ||pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockHk ));	// ��ʱֻ�������֤ȯ
	// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
	CRealtimePrice RealtimePrice;
	GetMerchRealTimePrice(pMerch, RealtimePrice);
	CFinanceData  FinanceData;
	memset((void*)(&FinanceData), 0, sizeof(FinanceData));
	GetMerchFinanceData(pMerch, FinanceData);
// 
// 	CReportScheme::ERHPriceNew,					// ���¼�
// 		CReportScheme::ERHVolumeTotal,				// �ܳɽ���
// 		CReportScheme::ERHAmount,					// ���
// 		CReportScheme::ERHRange,					// ����
// 		CReportScheme::ERHSwing,					// ���
// 		CReportScheme::ERHRate,						// ί��
// 		CReportScheme::ERHVolumeRate,				// ����
// 		
// 		CReportScheme::ERHBuySellRate,				// �����
// 		CReportScheme::ERHChangeRate,				// ������
// 		CReportScheme::ERHMarketWinRate,			// ��ӯ��
// 		
// 		CReportScheme::ERHAllCapital,				// �ܹɱ�
// 		CReportScheme::ERHCircAsset,				// ��ͨ�ɱ�
// 		CReportScheme::ERHAllAsset,				// ���ʲ�
// 		CReportScheme::ERHFlowDebt,				// ������ծ
// 		CReportScheme::ERHPerFund,					// ÿ�ɹ�����
// 		
// 		CReportScheme::ERHBusinessProfit,			// Ӫҵ����
// 		CReportScheme::ERHPerNoDistribute,			// ÿ��δ����
// 		CReportScheme::ERHPerIncomeYear,			// ÿ������(��)
// 		CReportScheme::ERHPerPureAsset,			// ÿ�ɾ��ʲ�
// 		CReportScheme::ERHChPerPureAsset,			// ����ÿ�ɾ��ʲ�
// 		
// 		CReportScheme::ERHDorRightRate,			// �ɶ�Ȩ���// 
// 		CReportScheme::ERHCircMarketValue,			// ��ͨ��ֵ
// 	CReportScheme::ERHAllMarketValue,			// ����ֵ
	
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
	case CReportScheme::ERHRange:				// ����%
		{
			if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				fVal = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			}
		}
		break;
	case CReportScheme::ERHSwing:				// ���
		{
			if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
			{
				fVal = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
			}	
		}
		break;
	case CReportScheme::ERHRate:			  // ί��=��(ί������-ί������)��(ί������+ί������)����100%
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
	case CReportScheme::ERHVolumeRate:				// ����
		{
			fVal = RealtimePrice.m_fVolumeRate;	
		}
		break;
	case CReportScheme::ERHBuySellRate:				// �����
		{
			if ( RealtimePrice.m_fSellVolume != 0.0 )
			{
				fVal = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
			}
		}
		break;
	case CReportScheme::ERHChangeRate:				// ������
		{
			fVal = RealtimePrice.m_fTradeRate;
		}
		break;
	case CReportScheme::ERHMarketWinRate:				// ������
		{
			fVal = RealtimePrice.m_fPeRate;
		}
		break;
	case CReportScheme::ERHCapitalFlow:				// �ʽ�����
		{
			fVal = RealtimePrice.m_fCapticalFlow;
		}
		break;
	case CReportScheme::ERHAllCapital:				// �ܹɱ�
		{
			fVal = FinanceData.m_fAllCapical;
		}
		break;
	case CReportScheme::ERHCircAsset:				// ��ͨ�ɱ�
		{
			fVal = FinanceData.m_fCircAsset;
		}
		break;
	case CReportScheme::ERHAllAsset:				// ���ʲ�
		{
			fVal = FinanceData.m_fAllAsset;
		}
		break;
	case CReportScheme::ERHFlowDebt:				// ������ծ
		{
			fVal = FinanceData.m_fFlowDebt;
		}
		break;
	case CReportScheme::ERHPerFund:				// ÿ�ɹ�����
		{
			fVal = FinanceData.m_fPerFund;
		}
		break;
	case CReportScheme::ERHBusinessProfit:				// Ӫҵ����
		{
			fVal = FinanceData.m_fBusinessProfit;
		}
		break;
	case CReportScheme::ERHPerNoDistribute:				// ÿ��δ����
		{
			fVal = FinanceData.m_fPerNoDistribute;
		}
		break;
	case CReportScheme::ERHPerIncomeYear:				// ÿ������(��)
		{
			fVal = FinanceData.m_fPerIncomeYear;
		}
		break;
	case CReportScheme::ERHPerPureAsset:				// ÿ�ɾ��ʲ�
		{
			fVal = FinanceData.m_fPerPureAsset;
		}
		break;
	case CReportScheme::ERHChPerPureAsset:				// ����ÿ�ɾ��ʲ�
		{
			fVal = FinanceData.m_fChPerPureAsset;
		}
		break;
	case CReportScheme::ERHDorRightRate:				// �ɶ�Ȩ���
		{
			fVal = FinanceData.m_fDorRightRate;
		}
		break;
	case CReportScheme::ERHCircMarketValue:				// ��ͨ��ֵ
		{
			fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
		}
		break;
	case CReportScheme::ERHAllMarketValue:				// ����ֵ
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
		return _T("[X ��]");
	}
	else if ( 10000.0 * 10000.0 == fScale )
	{
		return _T("[X ��]");
	}
	else if ( 10000.0 * 10000.0 * 10000.0 == fScale )
	{
		return _T("[X ����]");
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
	Invalidate(TRUE);	// �Ѿ�������ˣ��ػ�
}

void CIoViewStarry::UpdateBlockGrid()
{
	CString StrText;
	// ������
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(GetStarryType());

	//-- block����
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(m_block.m_StrName);

	// Ʒ������
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText.Format(_T("%d"), m_block.m_aMerchs.GetSize());
	pCell->SetText(StrText);

	// ����������
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText.Format(_T("%d"), m_block.m_aMerchs.GetSize());
	pCell->SetText(StrText);

	m_GridCtrl.Refresh();
}

void CIoViewStarry::UpdateBlockPaintMerchCount()
{
	CString StrText;
	// ����������
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
	pCell->SetText(L"�ǿ�����");
	
	//--
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"��ǰ����");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"Ʒ������");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"��������");
	
	//--
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"��ǰƷ��");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"Ʒ�ִ���");
	
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"�ּ�");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"�Ƿ�");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText( L"X������");
	
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"Y������");
	
	//-- zoom��Ʒ����
	m_GridCtrlZoomDesc.DeleteNonFixedRows();
	m_GridCtrlZoomDesc.SetRowCount(3);
	m_GridCtrlZoomDesc.SetColumnCount(1);
	
	m_GridCtrlZoomDesc.SetColumnWidth(0, 160);
	m_GridCtrlZoomDesc.SetColumnResize(TRUE);
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(0, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"X��: ");
	
	
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(1, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"Y��: ");
	
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(2, 0);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetFont(pFontSmall);
	pCell->SetText(L"��ɸѡ: ");

	//-- zoom��Ʒ
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
	if ( NULL != m_pMerchLastMouse )	// ��xml merch����
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
		double fPrePrice = pMerch->m_pRealtimePrice->m_fPricePrevClose;		// �������õ�������real���Ƿ��Ϊ����?
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

	// ��Ʒ��
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(StrMerchName);

	// ��Ʒ����
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	pCell->SetText(StrMerchCode);

	// ��Ʒ��
	m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol *pSymbolCell = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
	pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSNone);
	pSymbolCell->SetText(StrMerchPrice);

	// ��Ʒ�Ƿ�
	m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
	pSymbolCell = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
	pSymbolCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pSymbolCell->SetShowSymbol(CGridCellSymbol::ESSFall);
	pSymbolCell->SetText(StrMerchRiseRate);

	// ��ƷX
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(StrMerchXValue);

	// ��ƷY
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
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));		// ����
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
	// ����������Ʒ��û��realtimeprice����Ʒ Or ������ô����ˢ��������Ʒ��
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

	// ��ʼ�������κ�����
	if ( m_aMerchsNeedInitializedAll.GetSize() > 0 )
	{
		AddAttendMerchs(m_aMerchsNeedInitializedAll, true);	// ȫ������
		
		if ( m_dlgWait.ShowTimeWaitDlg(true, 30, true, _T("ˢ����������"), _T("���Ե�...")) != IDOK )
		{
			MessageBox(_T("ˢ����������ʧ�ܣ�\r\n�����ܻ��в�����Ʒ���ݲ��ܼ�ʱ���"), _T("������Ϣ"), MB_OK|MB_ICONWARNING);
		}

		ClearAttendMerchs();	// �����������
		m_aMerchsNeedInitializedAll.RemoveAll();
	}

	m_bMerchsRealTimeInitialized = true;

	RefreshBlock();	// ����&ˢ����ʾ
	// ���¹�ע��ǰ��Ҫ��ע�� - TODO
	
	RequestBlockViewData();
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);	// ��ʼ��ʱ���͸��°����Ʒ��Ϣ

	return 1;
}

void CIoViewStarry::RequestViewData()
{
	RequestViewData(m_aSmartAttendMerchs); // ����һ����	
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

		m_mapMerchReqWaitEchoTimes[ SmartAttendMerch.m_pMerch ] = TimeNow;	// ��¼�����ʱ��
		//m_mapMerchLastReqTimes[ SmartAttendMerch.m_pMerch ] = TimeNow;	// ���ݻ������ٸ���
		
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

		// ���������������
		if ( SmartAttendMerch.m_pMerch->m_pFinanceData ==NULL || SmartAttendMerch.m_pMerch->m_pFinanceData->m_fAllCapical <= 0.0  )
		{
			// ��Ҫ�������������
			aRequestSmarts[i].m_iDataServiceTypes |= (int)EDSTGeneral;
			ReqFinance.m_iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			ReqFinance.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
			RequestViewData(ReqFinance);

			//TRACE(_T("�ǿ��������(%d): %d,%s %s\r\n"),siReqFi++, ReqFinance.m_iMarketId, ReqFinance.m_StrMerchCode, SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCnName);
		}
		else
		{
			// ���в������ݣ�����ViewData��PublicFileģʽ������޸��µĽ��е��ã����������������
			// ����ӹ�ע��־���������ݣ����п���û�л���ȡ�������ע��־���Ӷ�����������ж���
			// Ҫ�����ǰ����еĲ��������ڳ�ʼ��ʱ�������������Ժ�Ͳ��ܲ������ݸ���
			// �������ڲ������ݸ��²��˵�bug
			// xl 0505 �������viewdata�����һ��֪ͨ
			// OnVDataPublicFileUpdate(aRequestSmarts[i].m_pMerch, EPFTF10);
			//aRequestSmarts[i].m_iDataServiceTypes |= (int)EDSTGeneral;	// ����ע�� ֻ�������ݣ����������û֪ͨ���������ݻ���£�����ʱ����ʾ��Ӧ����
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
		TRACE(_T("�ǿ�ʵ����������(%04d-%02d-%02d %02d:%02d:%02d): %d ��\r\n")
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

	ClearAttendMerchs();	// �������block������
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
			RequestViewData(aSmart);	// ��������
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
		
		m_aSmartAttendMerchs.Append(aSmart);	// �������Ʒ���ֹ�ע
		if ( bRequestViewData )
		{
			RequestViewData(aSmart);	// ����������Ʒ����
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
	
	m_mapMerchReqWaitEchoTimes.erase(pMerch);	// �Ƴ�ʱ��
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
		return 0;	// ̫խ��̫��
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
		cy = fRowLeftPer + KChartProper;	// ���row�ܶർ��ʣ��̫����ô�죿
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
		cx = fColLeftPer + KChartProper;	// ���row�ܶർ��ʣ��̫����ô�죿
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
	
	// x������ָ���Ϊ5������(han)
	const int32	iMaxCount = 5;
	int32 iWidthPer = m_RectBottomX.Width() / iMaxCount;

	double cx, cy;
	cx = cy = 0;
	if ( GetProperChartGridCellSize(cx, cy) <= 0 )
	{
		return;	// ����Ҫ����
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
	
	// �������
	CArray<CRect, const CRect &>	aRects;
	int32 i=0;
	for ( i = 0; i < iDivideCount ; i++  )	
	{
		int32 iPos = m_RectBottomX.left + i*iWidthPer;
		aRects.Add(CRect(iPos, m_RectBottomX.top+KMaxMerchRadius, iPos+iWidthPer, m_RectBottomX.bottom));	// ����top�µ�һ���뾶
	}

	// ���� 
	for ( i = 0; i < aRects.GetSize()-1 ; i++ )	// ���һ������ʾ��λ��
	{
		double fx = 0;
		if ( GetDivededValueByClientPoint(aRects[i].TopLeft(), &fx, NULL) )
		{
			CString Str;
			Str.Format(_T("%0.2f"), fx);
			dc.DrawText(Str, aRects[i], DT_LEFT |DT_SINGLELINE);
		}
	}

	if ( aRects.GetSize() > 0 )	// ��λ
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
	
	// y������ָ���Ϊ10������(han)
	const int32	iMaxCount = 10;
	double fHeightPer = m_RectLeftY.Height() / (double)iMaxCount;
	
	double cx, cy;
	cx = cy = 0;
	if ( GetProperChartGridCellSize(cx, cy) <= 0 )
	{
		return;	// ����Ҫ����
	}
	
	int iSaveDC = dc.SaveDC();
	dc.SetTextColor(GetIoViewColor(ESCText));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetIoViewFontObject(ESFSmall));
	
	//cx 
	int32 iDivideCount = iMaxCount;
	if ( fHeightPer < 20 )	// ����С��20
	{
		iDivideCount = m_RectLeftY.Height() / 20;
		if ( iDivideCount == 0 )
		{
			iDivideCount = 1;
		}
		fHeightPer = m_RectLeftY.Height() / (double)iDivideCount;
	}
	
	// ������� - �ӵͼ��㵽��
	CArray<CRect, const CRect &>	aRects;
	int32 i = 0;
	for ( i = 0; i < iDivideCount ; i++  )	
	{
		int32 iPos = m_RectLeftY.bottom - (int32)(i*fHeightPer);
		aRects.Add(CRect(m_RectLeftY.left, (int)(iPos - fHeightPer), m_RectLeftY.right, iPos));
	}
	
	// ���� 
	for ( i = 0; i < aRects.GetSize()-1 ; i++ )	// ���һ������ʾ��λ��
	{
		double fy = 0;
		if ( GetDivededValueByClientPoint(aRects[i].BottomRight(), NULL, &fy) )
		{
			CRect rc = aRects[i];
			rc.right -= KChartRightSkip;	// ��10������
			CString Str;
			Str.Format(_T("%0.2f"), fy);
			dc.DrawText(Str, rc, DT_RIGHT |DT_BOTTOM |DT_SINGLELINE);
		}
	}
	
	if ( aRects.GetSize() > 0 )	// ��λ
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
	// �����ң��ӵ׵���
	
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);

	CPen penBorder;
	CPen penDot;
	COLORREF clrLine = GetIoViewColor(ESCChartAxisLine);
	penBorder.CreatePen(PS_SOLID, 0, clrLine);
	penDot.CreatePen(PS_DOT, 0, clrLine);

	CRect rcChart(m_RectChart);

	CRect rcClient(0,0,0,0);	// grid border����
	GetClientRect(rcClient);
	CPoint ptSeperatorTop, ptSeperatorBottom;	
	ptSeperatorTop.x = rcChart.right + KChartRightSkip - 1;
	ptSeperatorTop.y = rcClient.top;
	ptSeperatorBottom.x = ptSeperatorTop.x;
	ptSeperatorBottom.y = rcClient.bottom;
	dc.SelectObject(&penBorder);
	dc.MoveTo(ptSeperatorTop);
	dc.LineTo(ptSeperatorBottom);

	// �߿����߳ŵ���
	dc.SelectObject(&penBorder);
	dc.MoveTo(rcChart.left, rcChart.bottom);
	dc.LineTo(rcChart.right + KChartRightSkip, rcChart.bottom);
	dc.MoveTo(rcChart.left, rcChart.bottom);
	dc.LineTo(rcChart.left, rcChart.top - KChartTopSkip);

	// ���߸���
	dc.SelectObject(&penDot);
	double fPos = rcChart.bottom - cy;
	double fEnd = rcChart.top + KChartProper;
	while ( fPos >= fEnd ) // ��һ�����������һ��������
	{
		dc.MoveTo(rcChart.left, (int)fPos);
		dc.LineTo(rcChart.right, (int)fPos);
		fPos -= cy;
	}
	// ���һ��
	dc.MoveTo(rcChart.left, rcChart.top);
	dc.LineTo(rcChart.right, rcChart.top);
	
	fPos = rcChart.left + cx;
	fEnd = rcChart.right - KChartProper;	// cx����cyһ�������Proper
	while ( fPos <= fEnd ) // ��һ�����������һ��������
	{
		dc.MoveTo((int)fPos, rcChart.bottom);
		dc.LineTo((int)fPos, rcChart.top);
		fPos += cx;
	}
	// ���һ��
	dc.MoveTo(rcChart.right, rcChart.bottom);
	dc.LineTo(rcChart.right, rcChart.top);

	// �����ĵ� - -, ���ڻ����е�, ������
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
		if ( GetMerchDrawRect(aMerchs[i], rcRadius) )		// �Ƿ��Ǽ�������Ʒ��룿�� - ���ݴ�ʱ��������100ms + ʵ�ַ���
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

	//if ( m_bBlinkUserStock && m_bBlinkUserStockDrawSpecifyColor ) // ��˸��ѡ����� - �ܻ���������ֲ�һ��
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
	
	// ���þ��δ�С
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
	// �ݲ�����
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
		// ��Ĩ����ǰ����
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
		m_mapMerchRect[pMerch] = rcRadius;		// ����ʾ�����ǻ���ʾ
		
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
	COLORREF clrZoomFrame = GetIoViewColor(ESCKeep);	// ��ѡ - ��ʹ��ƽ��
	
	CRect rcZoom;
	bool32 bDrawZoom = true;
	if ( !GetZoomRect(rcZoom) )	// �Ȼ�ȡѡ���ģ�ʧ���ڼ���
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
	CDC &dcMem = BeginTempDraw();	// ��ԭ����

	int iSavedcMem = dcMem.SaveDC();

	if ( (ptMouse.x < 0 || ptMouse.y < 0 ) && !IsPtInChart(m_ptZoomEnd) )	// ���û��ȷ�������
	{
		CancelZoomSelect(ptMouse, false);
	}
	else
	{
		DrawZoomSelect(dcMem, ptMouse);	// ���ƾ���
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

	// ����
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
		rcX.right = rcX.left + sizeX.cx + 10;	// + n������
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
	} // û��ֵ����
	
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

		// ��ǰû�в������ݣ������в��������ˣ�զ�죿 - �������������������
		RefreshBlock();	// �ؼ���&ˢ��
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
	if ( IsPtInChart(m_ptZoomStart) )	// �п�ʼ��ѡ��
	{
		CRect rcZoom;
		if ( GetZoomRect(rcZoom) )	// ����λ���Ѿ�ȷ�� - �������а��£���Ϊȷ�����ţ�����ȡ�����ѡ��
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
		else	// ȷ��������λ��
		{
			EnsureZoomSelect(point);
		}	
	}
	else
	{ // �趨��ʼλ��
		ConvertIntoChartPoint(point, m_ptZoomStart);
		m_ptZoomEnd = CPoint(-1,-1);
	}
	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewStarry::OnLButtonUp( UINT nFlags, CPoint point )
{
	if ( IsPtInChart(m_ptZoomStart) && !IsPtInChart(m_ptZoomEnd) && IsPtInChart(point) )		// �п�ʼѡ��, ����û��ȷ������λ��, ȷ������λ��
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
	return rcChart.PtInRect(pt);		// ��/�ױ߲��㣬����+1����
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
		DrawMouseMove(ptMouse);		// ��ʱdcҪ�ػ�, ��Ӧ�仯
	}
}

void CIoViewStarry::EnsureZoomSelect( CPoint ptMouse )
{
	ConvertIntoChartPoint(ptMouse, m_ptZoomEnd);
	CRect rcZoom;
	if ( GetZoomRect(rcZoom) && rcZoom.Width() > KMinMerchRadius && rcZoom.Height() > KMinMerchRadius )
	{
		// ��ȡ�����ѡ����Ʒ
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
	CString StrXRange(_T("X��: ")), StrYRange(_T("Y��: ")), StrMerchsCount;
	MerchArray	aMerchs;
	CRect rcZoom;
	if ( GetZoomRect(rcZoom) || CalcZoomRect(ptMouse, rcZoom) )
	{
		if ( m_RectLastZoomUpdate == rcZoom )
		{
			return;	// ����Ҫ��ʼ��
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
			StrXRange.Format(_T("X��: %0.2f ���� %0.2f"), fxMin, fxMax);
			StrYRange.Format(_T("Y��: %0.2f ���� %0.2f"), fyMin, fyMax);
		}
	}
	else
	{
		rcZoom.SetRectEmpty();
		if ( m_RectLastZoomUpdate == rcZoom )
		{
			return;	// ����Ҫ��ʼ��
		}
	}
	
	m_RectLastZoomUpdate = rcZoom;
	
	StrMerchsCount.Format(_T("��ɸѡ: %d ����Ʒ"), aMerchs.GetSize());
	
	// x��Χ
	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(0, 0);
	pCell->SetText(StrXRange);
	
	//-- y��Χ
	pCell = (CGridCellSys *)m_GridCtrlZoomDesc.GetCell(1, 0);
	pCell->SetText(StrYRange);
	
	// ɸѡƷ������
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
			return i;	// �Ѿ��и���Ʒ��ѡ�񲢸����¾Ϳ�����
		}
	}

	// û�� - �²���
	if ( m_mapMerchRect.count(pMerch) == 0 )	// ���ɼ��Ĳ��������
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
			// ��������
			CRealtimePrice RealTimePrice;
			if ( !GetMerchRealTimePrice(pMerch, RealTimePrice) )
			{
				ASSERT( 0 );		// ��Ȼ��ʾ�ˣ��Ͳ�Ӧ������û�е����
				return;
			}
			CString		StrMerchName;
			CString		StrMerchPrice;
			int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
			double fPrePrice = RealTimePrice.m_fPricePrevClose;		// �������õ�������real���Ƿ��Ϊ����?
			StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
			double fPriceNew;
			fPriceNew = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHPriceNew);
			StrMerchPrice = Float2SymbolString(fPriceNew, fPrePrice, iSaveDec);
			
			// ��Ʒ��
			CGridCellSys * pTmpCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
			pTmpCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pTmpCell->SetText(StrMerchName);
			pTmpCell->SetData((LPARAM)pMerch);
			
			// ��Ʒ��
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
	
	// ����ɸѡ��Ʒ
	const int32 iMerchShowSize = m_aZoomMerch.GetSize();	// �費��Ҫ��������������

	{
		CAutoLockWindowUpdate lockUpdate(&m_GridCtrlZoomMerchs);
		
		m_GridCtrlZoomMerchs.DeleteNonFixedRows();
		m_GridCtrlZoomMerchs.SetRowCount(iMerchShowSize);
		m_GridCtrlZoomMerchs.SetColumnCount(2);
		
		m_pMerchLastSelected = NULL;		// ��Ʒ�ز��룬���ѡ����Ʒ
		
		m_GridCtrlZoomMerchs.InsertRowBatchBegin();
		for ( int i=0; i < iMerchShowSize ; i++ )
		{
			CMerch *pMerch = m_aZoomMerch[i];
			if ( NULL != pMerch && NULL != pMerch->m_pRealtimePrice )
			{
				CString		StrMerchName;
				CString		StrMerchPrice;
				int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
				double fPrePrice = pMerch->m_pRealtimePrice->m_fPricePrevClose;		// �������õ�������real���Ƿ��Ϊ����?
				StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
				double fPriceNew;
				fPriceNew = CalcMerchValueByAxisType(pMerch, CReportScheme::ERHPriceNew);
				StrMerchPrice = Float2SymbolString(fPriceNew, fPrePrice, iSaveDec);
				
				// ��Ʒ��
				CGridCellSys * pCell = (CGridCellSys *)m_GridCtrlZoomMerchs.GetCell(i, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetText(StrMerchName);
				pCell->SetData((LPARAM)pMerch);
				
				// ��Ʒ��
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
	int32 iRet = 0;		// ����ֵ: �û�ѡ��X(1), Y(2), ��ѡ��(0)

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
		
		menu.AppendMenu(MF_POPUP |MF_STRING, (UINT)menuX.m_hMenu, _T("�趨X��"));
		menu.AppendMenu(MF_POPUP |MF_STRING, (UINT)menuY.m_hMenu, _T("�趨Y��"));
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
		pos = rcWin.TopLeft();	// ����
	}

	CRect rcTabXY(0,0,0,0), rcTabBlock(0,0,0,0);
	m_wndTabXYSet.GetWindowRect(rcTabXY);
	m_wndTabBlock.GetWindowRect(rcTabBlock);
	if ( rcTabBlock.PtInRect(pos) || rcTabXY.PtInRect(pos) )	// tab�²�����Ӧ
	{
		return;
	}

	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_CHG);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	pPopMenu->RemoveMenu(_T("�Զ���1"));
	pPopMenu->RemoveMenu(_T("�Զ���2"));
	pPopMenu->RemoveMenu(_T("�Զ���3"));
	pPopMenu->RemoveMenu(_T("�Զ���4"));
	
	//
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);
	pPopMenu->AppendODMenu(L"����", MF_STRING, IDM_IOVIEWBASE_RETURN);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	// ȡ������ - ��˸��ѡ�� -/ ������� -/ ��ͨ���� - �������� -/ ������ - ��С�� 
	pPopMenu->AppendODMenu(L"ȡ������ SPACE", MF_STRING, IDM_IOVIEW_STARRY_CANCELZOOM);
	if ( !m_bInZoom )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_CANCELZOOM, MF_BYCOMMAND |MF_DISABLED);
	}
	pPopMenu->AppendODMenu(L"��˸��ѡ", MF_STRING, IDM_IOVIEW_STARRY_BLINKUSER);
	if ( m_bBlinkUserStock )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_BLINKUSER, MF_BYCOMMAND |MF_CHECKED);
	}

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"�������", MF_STRING, IDM_IOVIEW_STARRY_EXPORTBLOCK);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"��ͨ����", MF_STRING, IDM_IOVIEW_STARRY_COR_NORMAL);
	if ( m_eCoordinate == COOR_Normal )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_COR_NORMAL, MF_BYCOMMAND |MF_CHECKED);
	}
	pPopMenu->AppendODMenu(L"��������", MF_STRING, IDM_IOVIEW_STARRY_COR_LOGARITHM);
	if ( m_eCoordinate == COOR_Logarithm )
	{
		pPopMenu->CheckMenuItem(IDM_IOVIEW_STARRY_COR_LOGARITHM, MF_BYCOMMAND |MF_CHECKED);
	}

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	pPopMenu->AppendODMenu(L"�������� CTRL NUM+", MF_STRING, IDM_IOVIEW_STARRY_STARRY_ADD);
	if ( KMaxMerchRadius == m_iRadius )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_STARRY_ADD, MF_BYCOMMAND |MF_DISABLED);
	}
	pPopMenu->AppendODMenu(L"��С���� CTRL NUM-", MF_STRING, IDM_IOVIEW_STARRY_STARRY_SUB);
	if ( KMinMerchRadius == m_iRadius )
	{
		pPopMenu->EnableMenuItem(IDM_IOVIEW_STARRY_STARRY_SUB, MF_BYCOMMAND |MF_DISABLED);
	}

// 	pPopMenu->AppendODMenu(_T("������Excel"),MF_STRING,IDM_USER1);		// �Ƿ���Ҫ, ��ǧ����Ʒ��������
// 	pPopMenu->AppendODMenu(_T("ͬ����Excel"),MF_STRING,IDM_USER2);		// 
// 
// 	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// 
	
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	// ��������:
	CNewMenu* pIoViewPopMenu = pPopMenu->AppendODPopupMenu(L"��������");
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// �ر�����:
	pPopMenu->AppendODMenu(L"�ر�����", MF_STRING, IDM_IOVIEWBASE_CLOSECUR);	

	// �����л�:	
	pPopMenu->AppendODMenu(L"�����л� TAB", MF_STRING, IDM_IOVIEWBASE_TAB);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	
	// ѡ����Ʒ
	pPopMenu->AppendODMenu(L"ѡ����Ʒ", MF_STRING, IDM_IOVIEWBASE_MERCH_CHANGE);

	pPopMenu->AppendODMenu(L"",MF_SEPARATOR);

	// ȫ��/�ָ�
	pPopMenu->AppendODMenu(L"ȫ��/�ָ� F7", MF_STRING, IDM_IOVIEWBASE_F7);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// ����Ŵ�
	pPopMenu->AppendODMenu(L"����Ŵ� CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER);

	// ������С
	pPopMenu->AppendODMenu(L"������С CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// �ָ��
	CNewMenu* pSplitWndMenu = pPopMenu->AppendODPopupMenu(L"�ָ��");
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT);

	// �رմ���
	pPopMenu->AppendODMenu(L"�رմ���", MF_STRING, IDM_IOVIEWBASE_CLOSE);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
	
	// �������
	pPopMenu->AppendODMenu(L"�������", MF_STRING, ID_SETTING);

	// ���沼��
	pPopMenu->AppendODMenu(L"���沼��", MF_STRING, ID_LAYOUT_ADJUST);

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
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
	case IDM_IOVIEW_STARRY_EXPORTBLOCK: // ������ǰ��ʾ������
		{
			if ( m_mapMerchRect.size() <= 0 )
			{
				MessageBox(L"û�пɵ�������Ʒ", L"�������", MB_ICONWARNING);
				return;
			}
			
			// �򿪰��Ի���:
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
				StrTip.Format(_T("���[%s]�е����ݽ�����գ��Ƿ������"), Block.m_StrBlockName.GetBuffer());
				if ( pBlock->m_aMerchs.GetSize() > 0 && IDNO == MessageBox(StrTip, _T("��ѡ��"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) )
				{
					return;
				}
				
				// �����������:
				for ( MerchRectMap::const_iterator it = m_mapMerchRect.begin() ; it != m_mapMerchRect.end() ; it++ )
				{
					if ( NULL == it->first )
					{
						continue;
					}
					CUserBlockManager::Instance()->AddMerchToUserBlock(it->first, Block.m_StrBlockName, false);
				}
				
				// ��Ʒ���ʱ��, �������, ֻ֪ͨ�ͱ����ļ�һ��.
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
				
				// ����Ӧ���
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
	if ( !m_bBlinkUserStock )	// ����blink
	{
		m_bBlinkUserStock = true;
		m_bBlinkUserStockDrawSpecifyColor = true;
		SetTimer(KDrawBlinkUserStockTimerId, KDrawBlinkUserStockTimerPeriod, NULL);
	}
	else
	{
		KillTimer(KDrawBlinkUserStockTimerId);
		m_bBlinkUserStockDrawSpecifyColor = false;
		DrawBlinkUserStock();	// ��ԭ��ʾ
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
		CDC &dcMem = m_dcMemPaintDraw;		// ֱ��ʹ��main dc
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
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), UM_HOTKEY, w, l);	// ����Ʒ��Ϣ����mainframe����
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
			
			// ��ӵ�ѡ����Ʒ�б�

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
	// �����δ����
	return 1;
}

void CIoViewStarry::DoMerchSelected(CMerch *pMerch,  bool32 bNeedDraw /*= true*/ )
{
	m_pMerchLastSelected = pMerch;
	if ( bNeedDraw )
	{
		DrawMouseMove(m_ptLastMouseMove);	// ��Ӧ�仯
	}
	// ����һ��xml merch
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
	if ( CalcMerchRect(pMerch, rcRadius) )		// ���ݱ��ʱ�����øú���������Ʒ�����	
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
		// ������С��ֱ��������ǣ������ǹ���
		for ( int i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_block.m_aMerchs[i];
			m_lstMerchsWaitToRequest.AddTail(pMerch);
		}
	}
	else
	{
		// ������������˵���������
		for ( int i=0; i < m_block.m_aMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_block.m_aMerchs[i];
			MerchValueMap::iterator itValue = m_mapMerchValues.find(pMerch);
			bool32 bAdd = true;
			if ( itValue != m_mapMerchValues.end() )
			{
				// �������Ʒ������
				// ��������Ʒ��������ʱ����������ģ�����һ�β�������
				// ���ܻ���ʱ���ȸպÿ����˵����� - 
				MerchReqTimeMap::iterator itLastTime = m_mapMerchLastReqTimes.find(pMerch);
				if ( itLastTime != m_mapMerchLastReqTimes.end()
					&& TimeNow < itLastTime->second+CGmtTimeSpan(0,0,1, 0) )
				{
					// û�г�������������ʱ���
					if ( !IsNeedFinanceDataType()
						|| (NULL!=pMerch->m_pFinanceData&&pMerch->m_pFinanceData->m_fAllCapical>100) )
					{
						// ����Ҫ�������ݣ������Ѿ����˲������ݣ��Ǿ͹�����ε���������
						// ���ܳ��ֲ������ݸ����ˣ�������߲�����������󣬵����ǿ�ͼ����Ĳ���������Զ�ò�������
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

	TRACE(_T("�ǿ��������һ������(%d/%d): %d ms\n"), iIgnored, m_block.m_aMerchs.GetSize(), timeGetTime()-dwTime);
}

void CIoViewStarry::RequestBlockQueueNext()
{
	KillTimer(KReqNextTimerId);

	const int32 iMaxAttendMerchSize = 20;	// ���ֹ�ע����Ʒ������

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
				// �����Ʒ������ʱ�䳬ʱ��, ���ڹ��������Ʒ
				// ֻ�еȴ���һ�ֵ�ѭ���ſ����л�����������
				aMerchs.Add(m_aSmartAttendMerchs[i].m_pMerch);
			}
			else
			{
				break;	// �����ʱ�������
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

		// ֻ�����ڿ����������ǿ�ͼ���յ����ݲ����������̺�õ���
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
		SetTimer(KTimerIdCheckReqTimeOut, KTimerPeriodCheckReqTimeOut, NULL);	// ׼��������ݳ�ʱ
	}

	if ( iAdded > 0 && m_lstMerchsWaitToRequest.IsEmpty() )
	{
		m_bFirstReqEnd = true;
	}

 	CTime TimeDis(TimeNow.GetTime());
 	TRACE(_T("�ǿ���������(%04d-%02d-%02d %02d:%02d:%02d)ʣ��: %d ��\r\n")
 		, TimeDis.GetYear(), TimeDis.GetMonth(), TimeDis.GetDay()
 		, TimeDis.GetHour(), TimeDis.GetMinute(), TimeDis.GetSecond(), m_lstMerchsWaitToRequest.GetCount());
}

void CIoViewStarry::RecreateBlockTab()
{
	bool32 bOldInit = m_bMerchsRealTimeInitialized;		// ��ֹ�л���Ӧ
	m_bMerchsRealTimeInitialized = false;
	m_wndTabBlock.DeleteAll();

	ASSERT( m_aBlockCollectionIds.GetSize() > 0 );
	for ( int i=0; i < m_aBlockCollectionIds.GetSize() ; i++ )
	{
		CString Str;
		CBlockConfig::Instance()->GetCollectionName(m_aBlockCollectionIds[i], Str);
		if ( Str.GetLength() > 2 && Str.Right(2) == _T("���") )
		{
			Str.Delete(Str.GetLength()-2, 2);
		}
		m_wndTabBlock.Addtab(Str + _T("��"), Str, Str);
	}	

	// ����idѡ��ǰ��block
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


	RemoveAttendMerch(pMerch, EDSTGeneral);		// ȡ���Բ������ݵĹ�ע

	if ( IsNeedFinanceDataType() )	// ��ǰ��Ҫ��������
	{
		bool32 bChange = false;
		CalcXYMinMaxAxis(pMerch, true, &bChange);		// ���Ƿ����Ʒ�����ݿ��Ըı������������Сֵ�����߽���ֻ��Ҫ��������ʾ
		if ( !bChange )
		{
			m_aMerchsNeedDraw.Add(pMerch);
			SetTimer(KDrawChartMerchTimerId, KDrawChartMerchTimerPeriod, NULL);	// ��ʱ����
		}
	}
}

bool32 CIoViewStarry::IsFinanceDataType( CReportScheme::E_ReportHeader eHeader )
{
	if ( eHeader >= CReportScheme::ERHAllCapital && eHeader <= CReportScheme::ERHDorRightRate )	// Ŀǰ�ṩ�ĲƸ����ݷ�Χ - ��ͨ��ֵ ����ֵ ��Ҫʵʱ���ۣ���Ϊ���۴���
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
		// û�з��͹�����Ʒ����ļ�¼���߸������ڽ�������֮ǰ���͵ģ�����Ϊ����Ӧ����������
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
				return true;	// λ�ڿ�����֮��
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
	ASSERT( NULL != m_pMerch && (m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTStockCn ||m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTExp  ));	// ��ʱֻ�������֤ȯ
	// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
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
	case CReportScheme::ERHRange:				// ����%
		{
			if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				fVal = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			}
		}
		break;
	case CReportScheme::ERHSwing:				// ���
		{
			if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
			{
				fVal = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
			}	
		}
		break;
	case CReportScheme::ERHRate:			  // ί��=��(ί������-ί������)��(ί������+ί������)����100%
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
	case CReportScheme::ERHVolumeRate:				// ����
		{
			fVal = RealtimePrice.m_fVolumeRate;	
		}
		break;
	default:
		break;
	}
	
	return fVal;
}


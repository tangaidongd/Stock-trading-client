#pragma warning(disable:4786)
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
#include "IoViewTimeSale.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "DlgBlockSelect.h"
#include "DlgBlockSet.h"
#include "ColorStep.h"
#include "DlgPhaseSort.h"
#include "IoViewPhaseSort.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  ID_CALCMSG_START  (UM_CALCMSG+EPSCM_Start)
#define  ID_CALCMSG_POS  (UM_CALCMSG+EPSCM_Pos)
#define  ID_CALCMSG_END  (UM_CALCMSG+EPSCM_End)
#define  ID_CALCMSG_REQUST_DATA  (UM_CALCMSG+EPSCM_RequestData)

#define  ID_GRDID_CTRL 12366

// xml
const char *KStrXMLIOPSTimeStart		= ("TimeStart");
const char *KStrXMLIOPSTimeEnd			= ("TimeEnd");
const char *KStrXMLIOPSPhaseSortType	= ("SortType");
const char *KStrXMLIOPSPreWeight		= ("PreWeight");
const char *KStrXMLIOPSBlockId			= ("BlockId");

const int32 KWatchTimeChangeTimerId						= 100006;	// 每隔 n 秒钟, 重新查看时间是否跨天
const int32 KWatchTimeChangeTimerPeriod					= 60*1000;					

// 固定 Tab 个数和项目. 留最后一个Tab 作为灵活显示前面没有的板块

static const int32 KiIDTabAll			= -1;   // 全集
static const int32 KiIDTabUserOwn		= -2;	// 自选股集合
static const int32 KiIDTabChgable		= -3;	// 最后一个Tab 可以自己改变的
static const int32 KiIDTabFenLei		= -4;	// 分类板块

static const int32 KiIDTabAllExcept		= 2;	// 全集里去掉这个板块

// 标签页数组定义
static const T_SimpleTabInfo  s_KaTabInfos[] = 
{
	T_SimpleTabInfo(KiIDTabFenLei,	2003,	0,		L"所有分类▲",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabAll,		4075,	1,		L"板块集合▲",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabUserOwn,	-1,		2,		L"自选板块▲",		ETITCollection,		T_BlockDesc::EBTUser),
	T_SimpleTabInfo(2000,			2000,	3,		L"沪深Ａ股",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2005,			2005,	4,		L"中小企业",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2006,			2006,	5,		L"创业板",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2007,			2007,	6,		L"沪深Ｂ股",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2008,			2008,	7,		L"沪深基金",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2010,			2010,	8,		L"沪深权证",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
};

static const int32 s_KiTabInfoCount = sizeof(s_KaTabInfos)/sizeof(T_SimpleTabInfo);

	
IMPLEMENT_DYNCREATE(CIoViewPhaseSort, CIoViewBase)
CIoViewPhaseSort *CIoViewPhaseSort::m_spThis = NULL;
CIoViewPhaseSort::MerchRequestTimeMap CIoViewPhaseSort::s_mapMerchReqTime;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewPhaseSort, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewPhaseSort)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
ON_MESSAGE_VOID(UM_BLOCK_DOINITIALIZE, OnDoBlockReportInitialize)
ON_MESSAGE_VOID(UM_DOCALC, OnMsgDoCalc)
ON_MESSAGE(ID_CALCMSG_START, OnMsgCalcStart)
ON_MESSAGE(ID_CALCMSG_POS, OnMsgCalcPos)
ON_MESSAGE(ID_CALCMSG_END, OnMsgCalcEnd)
ON_MESSAGE(ID_CALCMSG_REQUST_DATA, OnMsgCalcReqData)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRDID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRDID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRDID_CTRL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRDID_CTRL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRDID_CTRL, OnGridKeyUpEnd)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRDID_CTRL, OnGridGetDispInfo)
ON_NOTIFY(GVN_ODCACHEHINT, ID_GRDID_CTRL, OnGridCacheHint)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewPhaseSort::CIoViewPhaseSort()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_RectTitle.SetRectEmpty();

	m_iSortColumn		= -1;
	
	m_aUserBlockNames.RemoveAll();

	m_PhaseOpenBlockParam.m_iBlockId = CBlockInfo::GetInvalidId();		// 给与无效化
	
	m_bBlockReportInitialized = false;		// 待初始化

	m_tmDisplay.tm_year = 0;

	m_iPreTab = m_iCurTab = -1;

	m_eStage = ES_Count;

	m_PhaseOpenBlockParam.m_TimeStart = m_PhaseOpenBlockParam.m_TimeEnd + CGmtTimeSpan(1,0,0,0);	// 默认参数无效化，避免默认就打开
	m_PhaseBlockParamXml.m_TimeStart = m_PhaseBlockParamXml.m_TimeEnd + CGmtTimeSpan(1,0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPhaseSort::~CIoViewPhaseSort()
{
// 	if ( NULL != m_pSubject )
// 	{
// 		m_pSubject->DelObserver(this);
// 	}
	
	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewPhaseSort::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

	if ( !IsWindowVisible() )
	{
		return;
	}

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	//
	UnLockRedraw();

	CRect rectClient;
	GetClientRect(&rectClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume2);

	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		clrBk = RGB(44, 0, 0);
	}
	else
	{
		// 灰度
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	CMemDC dc(&dcPaint, m_RectTitle);
	int32 iSaveDC = dc.SaveDC();

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	dc.SelectObject(pFontNormal);
	
	dc.FillSolidRect(m_RectTitle, clrBk);

	dc.SetBkColor(clrBk);
	dc.SetBkMode(TRANSPARENT);
	
	CRect rcDraw(m_RectTitle);
	rcDraw.left += 10;


	for ( int32 i=0; i < m_aTitleString.GetSize() ; i++ )
	{
		dc->SetTextColor(m_aTitleString[i].m_clr);
		dc->DrawText(m_aTitleString[i].m_StrTitle, rcDraw, DT_SINGLELINE |DT_LEFT |DT_VCENTER);
		CSize sizeText = dc->GetTextExtent(m_aTitleString[i].m_StrTitle);
		rcDraw.left += sizeText.cx;
	}

	dc.FillSolidRect(rectClient.left, m_RectTitle.bottom-1, rectClient.Width(), 1, clrAxis);

	if ( m_bActive )
	{
		dc.FillSolidRect(3, 2, 2, 2, clrVol);
	}

	dc.RestoreDC(iSaveDC);
}

BOOL CIoViewPhaseSort::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_F8 == pMsg->wParam && m_PhaseOpenBlockParam.m_TimeStart <= m_PhaseOpenBlockParam.m_TimeEnd )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			OpenBlock(BlockParam, true);	// 重新打开
			return TRUE;
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

int CIoViewPhaseSort::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//创建Tab 条
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	m_GuiTabWnd.SetUserCB(this);
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);

	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRDID_CTRL);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.SetTextColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	
	m_GridCtrl.SetDefCellWidth(60);
	m_GridCtrl.EnableBlink(FALSE);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetVirtualCompare(CompareRow);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));

	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息
	m_XSBVert.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 即时发送数据请求

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);

	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(false);
	m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE); 
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// 序 名
	m_GridCtrl.SetFixedRowCount(1);

	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	PostMessage(UM_BLOCK_DOINITIALIZE, 0, 0);

	// 这个时候可以初始化tab资源了的，板块在此时应当加载好了，如果没加载应当在do initialize中初始化板块和tab
	// 固定的tab页，现在加载
	InitializeTabs();	

	//
	InitializeTitleString();

	// 现在显示的时间，如果要显示的话
	SetTimer(KWatchTimeChangeTimerId, KWatchTimeChangeTimerPeriod , NULL);
	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	timeNow.GetLocalTm(&m_tmDisplay);

	// 板块侦听
	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewPhaseSort::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

BOOL CIoViewPhaseSort::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewPhaseSort::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	Invalidate(TRUE);
}

void CIoViewPhaseSort::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewPhaseSort::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( m_bBlockReportInitialized )	// 初始化完毕
	{
		TabChanged();
		InvalidateRect(m_RectTitle);
	}

	*pResult = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
//从WorkSpace的xml中读入Blocks,并不确保Blocks的内容在物理板块. Logic xml. user xml中存在

bool32 CIoViewPhaseSort::FromXml(TiXmlElement * pTiXmlElement)
{
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

	// 读取保存的当前参数，等待init
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSTimeStart);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_TimeStart = (time_t)atol(pcAttrValue);
	}
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSTimeEnd);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_TimeEnd = (time_t)atol(pcAttrValue);
	}
	
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSPhaseSortType);
	if ( NULL != pcAttrValue )
	{
		E_PhaseSortType eSort = (E_PhaseSortType)atoi(pcAttrValue);
		if ( eSort < EPST_Count )
		{
			m_PhaseBlockParamXml.m_ePhaseSortType = eSort;
		}
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSPreWeight);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_bDoPreWeight = 0 != atol(pcAttrValue);
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSBlockId);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_iBlockId = atol(pcAttrValue);
	}

	// 此时Tab信息是初始化好了的
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);

	return true;
}

CString CIoViewPhaseSort::ToXml()
{	
	//
	CString StrThis;
	int32 iTimeStart = m_PhaseOpenBlockParam.m_TimeStart.GetTime();
	int32 iTimeEnd   = m_PhaseOpenBlockParam.m_TimeEnd.GetTime();

	StrThis.Format( L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" ", 
					CString(GetXmlElementValue()).GetBuffer(),
					CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
					CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
					CString(KStrXMLIOPSTimeStart).GetBuffer(), iTimeStart,
					CString(KStrXMLIOPSTimeEnd).GetBuffer(), iTimeEnd,
					CString(KStrXMLIOPSPhaseSortType).GetBuffer(), m_PhaseOpenBlockParam.m_ePhaseSortType,
					CString(KStrXMLIOPSPreWeight).GetBuffer(), m_PhaseOpenBlockParam.m_bDoPreWeight,
					CString(KStrXMLIOPSBlockId).GetBuffer(), m_PhaseOpenBlockParam.m_iBlockId
				  );

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";


	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewPhaseSort::GetDefaultXML()
{
	CString StrThis;
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";

	return StrThis;
}

void CIoViewPhaseSort::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos(0, 0);
	GetCursorPos(&pos);
	DoTrackMenu(pos);
}

void CIoViewPhaseSort::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pGridView && pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
	{
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			CMainFrame *pMainFrame =DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pMainFrame->OnShowMerchInChart(pMerch, this);
			}
		}
	}
}

void CIoViewPhaseSort::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewPhaseSort::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向下键按到头了
	
}

void CIoViewPhaseSort::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向上键按到头了
	
}

// 通知视图改变关注的商品
void CIoViewPhaseSort::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewPhaseSort::OnVDataForceUpdate()
{
	if ( m_aSmartAttendMerchs.GetSize() > 0 )
	{
		RequestAttendMerchs(true);		// 现有数据重新请求下
	}
	TryRequestMoreNeedPriceMerchs();
}

void CIoViewPhaseSort::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	// 每次市场初始化也会用0数据来调用, 纠结, 还要请求财务数据吗？
	for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
	{
		if (m_pAbsCenterManager && pMerch == m_aSmartAttendMerchs[i].m_pMerch )
		{
			s_mapMerchReqTime[pMerch] = m_pAbsCenterManager->GetServerTime();	// 该请求更新了, 就算是市场初始化也计算在内

			m_aSmartAttendMerchs.RemoveAt(i);	// 尽量删除重复???
			if ( m_aSmartAttendMerchs.GetSize() <= 0 )
			{
				TryRequestMoreNeedPriceMerchs();	// 尝试请求更多的
			}

			if ( ES_WaitPrice == m_eStage )
			{
				// 正在等待行情数据
				const int32 iAttSize = m_aSmartAttendMerchs.GetSize() + m_aMerchsNeedPrice.GetSize();
				const int32 iMaxSize = m_ParamForDisplay.m_aMerchsToCalc.GetSize();
				
				m_DlgWait.SetProgress(iMaxSize-iAttSize, iMaxSize);
				TRACE(_T("%d-%d\r\n"), iMaxSize-iAttSize, iMaxSize);
				if ( iAttSize <= 0 )
				{
					// 完毕
					m_eStage = ES_WaitCalc;
					PostMessage(UM_DOCALC, 0, 0);	// 开始计算
				}
			}
			break;
		}
	}
}

// 当物理市场排行发生变化
// 当物理市场排行发生变化
void CIoViewPhaseSort::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	
}

void CIoViewPhaseSort::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	
}

void CIoViewPhaseSort::OnFixedRowClickCB(CCellID& cell)
{
	// 数据全申请，不做优化
    if (!m_GridCtrl.IsValid(cell) )
	{
		return;
	}

	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		return;
	}

	const int iCol = cell.col;
	ASSERT( iCol >= 0 && iCol < m_aColumnHeaders.GetSize() );
	E_PhaseHeaderType eHeader = m_aColumnHeaders[iCol].m_eHeaderType;
	if ( eHeader == EPHT_No )
	{
		// 还原排序
		if ( m_GridCtrl.GetSortColumn() >= 0 )
		{
			m_GridCtrl.SetSortColumn(-1);
			m_GridCtrl.ResetVirtualOrder();
			m_GridCtrl.Refresh();
		}
		return;
	}

	{
		m_iSortColumn = iCol;
		
		m_spThis = this;
		m_GridCtrl.SetHeaderSort(TRUE);

		m_iSortColumn = cell.col;
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		m_GridCtrl.Refresh();
	}

}

void CIoViewPhaseSort::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewPhaseSort::OnHScrollEnd()
{
	
}

void CIoViewPhaseSort::OnVScrollEnd()
{
	
}

void CIoViewPhaseSort::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewPhaseSort::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewPhaseSort::OnDestroy()
{	
	CBlockConfig::Instance()->RemoveListener(this);		// 停止侦听

	if ( m_ParamForCalc.m_eResultFlag == EPSCRF_Calculating )
	{
		ASSERT( 0 );
		CancelPhaseSortCalc(&m_ParamForCalc);	// 停止计算, 如果有的话
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewPhaseSort::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if (nIDEvent == KWatchTimeChangeTimerId )
	{
		if(m_pAbsCenterManager)
		{
			CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
			tm tmLocal;
			timeNow.GetLocalTm(&tmLocal);
			if ( m_tmDisplay.tm_yday != tmLocal.tm_yday )
			{
				m_tmDisplay = tmLocal;
				InvalidateRect(m_RectTitle);
			}
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewPhaseSort::SetChildFrameTitle()
{
	CString StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewPhaseSort::GetStdMenuEnable( MSG* pMsg )
{
	return false;
}

void CIoViewPhaseSort::LockRedraw()
{
	
}

void CIoViewPhaseSort::UnLockRedraw()
{
	
}

void CIoViewPhaseSort::OnDblClick(CMerch *pMerch)
{
	
}

void CIoViewPhaseSort::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	Invalidate();
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);	

	RecalcLayout();
	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewPhaseSort::OnLButtonDown2( CPoint pt, int32 iTab )
{
	// true拦截
	// 看击中的是哪个tab，前面3个是要选择block的，后面的是点击就产生效果
	const T_SimpleTabInfo *pTab = GetSimpleTabInfo(iTab);
	if ( NULL != pTab && pTab->m_eTabType == ETITCollection )
	{
		// 需要菜单
		CNewMenu menu;
		menu.CreatePopupMenu();

		CBlockLikeMarket *pBlockCur = GetCurrentBlock();
		
		typedef map<UINT, CBlockLikeMarket *> CmdToBlockMap;
		CmdToBlockMap mapIds;
		int32 iCmdBase = 10;
		switch ( pTab->m_iID )
		{
		case KiIDTabFenLei: // 分类
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetMarketClassBlocks(aBlocks);
				for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
				{
					menu.AppendMenu(MF_STRING, iCmdBase, aBlocks[i]->m_blockInfo.m_StrBlockName);
					if ( pBlockCur == aBlocks[i] )
					{
						menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
					}
					mapIds[iCmdBase++] = aBlocks[i];
				}

				// 加入我的自选
				T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
				if ( NULL != pBlockServer )
				{
					CBlockConfig::BlockArray aUserBlocks;
					CBlockConfig::Instance()->GetUserBlocks(aUserBlocks);
					CBlockLikeMarket *pBlockDefUser = NULL;
					for ( int32 i=0; i < aUserBlocks.GetSize() ; i++ )
					{
						if ( aUserBlocks[i]->m_blockInfo.m_StrBlockName == pBlockServer->m_StrName )
						{
							pBlockDefUser = aUserBlocks[i];
							break;
						}
					}
					if ( NULL != pBlockDefUser )
					{
						if ( menu.GetMenuItemCount() > 0 )
						{
							menu.AppendMenu(MF_SEPARATOR);
						}
						menu.AppendMenu(MF_STRING, iCmdBase, pBlockDefUser->m_blockInfo.m_StrBlockName);
						if ( pBlockCur == pBlockDefUser )
						{
							menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
						}
						mapIds[iCmdBase++] = pBlockDefUser;
					}
				}
			}
			break;
		case KiIDTabUserOwn:
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetUserBlocks(aBlocks);
				for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
				{
					menu.AppendMenu(MF_STRING, iCmdBase, aBlocks[i]->m_blockInfo.m_StrBlockName);
					if ( pBlockCur == aBlocks[i] )
					{
						menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
					}
					mapIds[iCmdBase++] = aBlocks[i];
				}
			}
			break;
		case KiIDTabAll: // 所有
			{
				// col - block
				// 显示全部
				CBlockConfig::IdArray aIdAll;
				CBlockConfig::Instance()->GetCollectionIdArray(aIdAll);
				//
				for ( int32 i = 0; i < aIdAll.GetSize(); i++ )
				{	
					//
					CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aIdAll[i]);
					if ( pCol->IsMarketClassBlockCollection() || pCol->IsUserBlockCollection() )
					{
						continue;
					}
					CBlockConfig::BlockArray aBlocks;
					pCol->GetValidBlocks(aBlocks);
					
					if ( aBlocks.GetSize() > 0 
						&& ( aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// 普通 || 分类
						|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )					
						)
					{
						CNewMenu *pColMenu = menu.AppendODPopupMenu(pCol->m_StrName);
						
						//
						ASSERT(NULL != pColMenu);
						
						//
						for ( int32 iBlock = 0; iBlock < aBlocks.GetSize(); iBlock++ )
						{						
							if ( NULL == aBlocks[iBlock] )
							{
								//ASSERT(0);
								continue;
							}
							
							//
							CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
							
							if ( StrItem.GetLength() > 7 )
							{
								StrItem = StrItem.Left(6) + _T("...");
							}
							
							//
							pColMenu->AppendODMenu(StrItem, MF_STRING, iCmdBase);
							if ( pBlockCur == aBlocks[iBlock] )
							{
								menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
							}
							mapIds[ iCmdBase++ ] = aBlocks[iBlock];
						}
						
						MultiColumnMenu(*pColMenu, 20);
					}
				}
			}
			break;
		}

		// 弹出菜单
		bool32 bSel = false;
		CPoint point = m_GuiTabWnd.GetLeftTopPoint(iTab);	
		int32 iRet = menu.TrackPopupMenu(TPM_BOTTOMALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD,
			point.x, point.y, AfxGetMainWnd());
		if ( iRet > 0 )
		{
			CmdToBlockMap::iterator it = mapIds.find(iRet);
			if ( it != mapIds.end() )
			{
				T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
				BlockParam.m_iBlockId = it->second->m_blockInfo.m_iBlockId;
				bSel = OpenBlock(BlockParam, true);	// 菜单选择的需重新打开
			}
		}
		return !bSel;	// 没有选择就拦截
	}
	return false;
}

bool32 CIoViewPhaseSort::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return true;	// 不能右键
}

void CIoViewPhaseSort::SetTab(int32 iCurTab)
{
	int32 iCur = m_GuiTabWnd.GetCurtab();
	m_GuiTabWnd.SetCurtab(iCurTab);
	if ( iCur == iCurTab )	// 此时tab不会发送消息，自己调用
	{
		TabChanged();
	}
}

void CIoViewPhaseSort::TabChanged()
{
	int32 iCurTab = m_GuiTabWnd.GetCurtab();
	const T_SimpleTabInfo *pTab = GetSimpleTabInfo(iCurTab);
	if ( NULL != pTab 
		&& pTab->m_eTabType == ETITEntity )
	{
		// 单个实体
		if ( NULL != CBlockConfig::Instance()->FindBlock(pTab->m_iID) )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			BlockParam.m_iBlockId = pTab->m_iID;
			OpenBlock( BlockParam, false );	// 不必重新打开
		}
	}
}

void CIoViewPhaseSort::SetRowHeightAccordingFont()
{
	// 根据列表头，确定每行行高
	float fFixedBase = 0.0;
	float fNonFixedBase = 0.0f;
	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		fFixedBase = max(fFixedBase, m_aColumnHeaders[i].m_fFixedRowHeightRatio);
		fNonFixedBase = max(fNonFixedBase, m_aColumnHeaders[i].m_fNonFixedRowHeightRatio);
	}

	// 重设字体还原行高
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	int32 iFixedHeight = m_GridCtrl.GetDefCellHeight();
	int32 iNonFHeight  = iFixedHeight;
	if ( fFixedBase > 0.0f )
	{
		iFixedHeight = (int32)(fFixedBase * iFixedHeight);
	}
	if ( fNonFixedBase > 0.0f )
	{
		iNonFHeight = (int32)(fNonFixedBase * iNonFHeight);
	}
	
	//if ( iFixedHeight != m_GridCtrl.GetDefCellHeight() )
	{
		// 变更，改变默认的与现有的
		CGridDefaultCell *pCell1 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(TRUE, TRUE));
		CGridDefaultCell *pCell2 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(TRUE, FALSE));
		if ( NULL != pCell1 )
		{
			pCell1->SetHeight(iFixedHeight);
		}
		if ( NULL != pCell2 )
		{
			pCell2->SetHeight(iFixedHeight);
		}

		for ( int32 i=0; i < m_GridCtrl.GetFixedRowCount() ; i++ )
		{
			m_GridCtrl.SetRowHeight(i, iFixedHeight);
		}
	}

	if ( iNonFHeight != m_GridCtrl.GetDefCellHeight() )
	{
		// 变更，改变默认的与现有的
		CGridDefaultCell *pCell1 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(FALSE, TRUE));
		CGridDefaultCell *pCell2 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(FALSE, FALSE));
		if ( NULL != pCell1 )
		{
			pCell1->SetHeight(iNonFHeight);
		}
		if ( NULL != pCell2 )
		{
			pCell2->SetHeight(iNonFHeight);
		}
		
		for ( int32 i=m_GridCtrl.GetFixedRowCount(); i < m_GridCtrl.GetRowCount() ; i++ )
		{
			m_GridCtrl.SetRowHeight(i, iNonFHeight);
		}
	}
}

void CIoViewPhaseSort::SetColWidthAccordingFont()
{
	// 使用列表自己的意愿调整？ TODO
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("HW髋测"));
	dc.SelectObject(pOldFont);
	const int32 iBaseCharWidth = (sizeText.cx)/4;
	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		float fRatio = m_aColumnHeaders[i].m_fPreferWidthByChar;
		int32 iWidth = (int32)(fRatio*iBaseCharWidth);
		if ( m_GridCtrl.GetColumnWidth(i) > 0 )
		{
			m_GridCtrl.SetColumnWidth(i, iWidth);
		}
	}
}

void CIoViewPhaseSort::UpdateTableHeader()
{
	m_GridCtrl.SetFixedColumnCount(3);	// 序号列 code,名称固定
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(m_aColumnHeaders.GetSize());	// 序号列, 名称...

	m_GridCtrl.SetSortColumn(-1);

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::UpdateTableAllContent()
{
	CCellID    cellId = m_GridCtrl.GetTopleftNonFixedCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();

	m_GridCtrl.DeleteNonFixedRows();	
	//m_GridCtrl.ResetVirtualOrder();
	
	m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_ParamForDisplay.m_aCalcResults.GetSize());
	//SetColWidthAccordingFont();
	//SetRowHeightAccordingFont();

	if ( !m_GridCtrl.IsValid(cellId) )
	{
		cellId.row = m_GridCtrl.GetFixedRowCount();
		cellId.col = m_GridCtrl.GetFixedColumnCount();
	}
	if ( m_GridCtrl.IsValid(cellId) )
	{
		m_GridCtrl.EnsureTopLeftCell(cellId);
	}
	if ( m_GridCtrl.IsValid(cellRange) )
	{
		m_GridCtrl.SetSelectedRange(cellRange);
	}
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnDoBlockReportInitialize()
{
	m_bBlockReportInitialized = true;		// 初始化完毕了，以后就是正式的操作了

	// 重装载一次板块数据
	if ( m_PhaseBlockParamXml.m_TimeStart <= m_PhaseBlockParamXml.m_TimeEnd
		&& m_PhaseBlockParamXml != m_PhaseOpenBlockParam
		&& m_PhaseOpenBlockParam.m_TimeEnd < m_PhaseOpenBlockParam.m_TimeStart )
	{
		// 在原始参数无效的情况下，开始打开xml中的板块
		// 现在阶段排行通过mainframe中的菜单 先调用页面文件 页面文件xml化 然后在mainframe调用open打开参数排行
		// 应该是不会进入这里的
		ASSERT( 0 );
		//OpenBlock(m_PhaseBlockParamXml, false);
		//ChangeTabToCurrentBlock();
	}
}

void CIoViewPhaseSort::InitializeTabs()
{
	int i = 0;
	bool32 bInitOld = m_bBlockReportInitialized;
	m_bBlockReportInitialized = false;				// 禁止tab响应

	m_GuiTabWnd.DeleteAll();
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		const T_SimpleTabInfo &tab = s_KaTabInfos[i];
		CString StrName = tab.m_StrShowName;
		m_GuiTabWnd.Addtab(StrName, StrName, StrName);
	}
	
	m_bBlockReportInitialized = bInitOld;
}

void CIoViewPhaseSort::SetTabByBlockId( int32 iBlockId )
{
	int i = 0;
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		if ( iBlockId == s_KaTabInfos[i].m_iIdDefault && s_KaTabInfos[i].m_eTabType == ETITEntity )		// 选中该tab
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				m_GuiTabWnd.SetCurtab(i);		// 当前选择是否变更??
				break;
			}
		}
	}
}

void CIoViewPhaseSort::ChangeTabToCurrentBlock()
{
	int i = 0;
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		if ( m_PhaseOpenBlockParam.m_iBlockId == s_KaTabInfos[i].m_iIdDefault && s_KaTabInfos[i].m_eTabType == ETITEntity )		// 选中该tab
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				bool32 bInit = m_bBlockReportInitialized;
				m_bBlockReportInitialized = false;	// 禁止响应
				m_GuiTabWnd.SetCurtab(i);		// 当前选择是否变更??
				m_bBlockReportInitialized = bInit;
				break;
			}
		}
	}
}

void CIoViewPhaseSort::RecalcLayout()
{
	m_RectTitle.SetRectEmpty();

	CRect rcClient;
	GetClientRect(rcClient);

	m_RectTitle = rcClient;

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeTitle = dc.GetTextExtent(_T("标题高度测试"));
	dc.SelectObject(pFontOld);

	m_RectTitle.bottom = m_RectTitle.top + sizeTitle.cy + 10;

	rcClient.top = m_RectTitle.bottom;

	CSize sizeTab = m_GuiTabWnd.GetWindowWishSize();
	if ( rcClient.Height() < sizeTab.cy )
	{
		m_GuiTabWnd.MoveWindow(0,0,0,0);
		m_GridCtrl.MoveWindow(0,0,0,0);
		m_XSBHorz.MoveWindow(0,0,0,0);
		return;
	}
	
	// 优先满足tab，但是srcoll不能小于一个特定值, 如果总宽度小于该值，则所有宽度都是scroll的
	int32 iMaxTabWidth = rcClient.Width() - 125;	// 
	iMaxTabWidth = iMaxTabWidth < 0 ? 0 : iMaxTabWidth;
	sizeTab.cx = MIN(iMaxTabWidth, sizeTab.cx);		// 最多分1/1
	m_GuiTabWnd.MoveWindow(rcClient.left, rcClient.bottom - sizeTab.cy, sizeTab.cx, sizeTab.cy);
	CRect rcHorz(rcClient.left + sizeTab.cx, rcClient.bottom - sizeTab.cy, rcClient.right, rcClient.bottom);
	m_XSBHorz.SetSBRect(rcHorz, TRUE);

	rcClient.bottom -= sizeTab.cy;

	m_GridCtrl.MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());	// 不给竖直滚动分配了
}

void CIoViewPhaseSort::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( Initialized == eInitializeType )
	{
		// 板块信息重建
		InitializeTabs();
	}
}

void CIoViewPhaseSort::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// 板块变更不理
	Invalidate(TRUE);	// 刷新下显示
}

void CIoViewPhaseSort::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// 尝试v mode
		
		GV_ITEM &item = pDisp->item;
		item.lParam = NULL;

		CGridCellNormalSys *pCell = DYNAMIC_DOWNCAST(CGridCellNormalSys, m_GridCtrl.GetDefaultVirtualCell());
		ASSERT( pCell != NULL );
		if ( NULL != pCell )
		{
			pCell->SetShowSymbol(0x7);	// 全部都显示
		}

		COLORREF clrText = GetIoViewColor(ESCKeep);
		COLORREF clrRise = GetIoViewColor(ESCRise);
		COLORREF clrFall = GetIoViewColor(ESCFall);
		item.crFgClr = clrText;	// 设置默认颜色
		if ( item.col >=0 && item.col < m_aColumnHeaders.GetSize() )
		{
			const T_ColumnHeader &header = m_aColumnHeaders[item.col];
			if ( item.row == 0 )
			{
				// 表头
				item.strText = header.m_StrHeader;
			}
			else if ( item.row <= m_ParamForDisplay.m_aCalcResults.GetSize() && item.row > 0 )
			{
				int32 iIndex = item.row -1;
				const PhaseSortCalcResultArray &aResults = m_ParamForDisplay.m_aCalcResults;
				const T_PhaseSortCalcReslt &Res = aResults[iIndex];
				CMerch *pMerch = m_ParamForDisplay.m_aMerchsToCalc[iIndex];
				int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
				if ( item.col == 0 )
				{
					item.lParam = (LPARAM)pMerch;	// 设置该商品作为0列的PARAM
				}
				switch (header.m_eHeaderType)
				{
				case EPHT_No:		// 序号
					// 当前在哪行
					{
						if ( m_GridCtrl.GetSortColumn() >= 0 )
						{
							int i = 0;
							for ( CGridCtrl::intlist::const_iterator it = m_GridCtrl.m_arRowOrder.begin(); it != m_GridCtrl.m_arRowOrder.end() ; it++, i++ )
							{
								if ( *it == item.row )
								{
									item.strText.Format(_T("%d"), i);
									break;
								}
							}
						}
						else
						{
							item.strText.Format(_T("%d"), item.row);
						}
					}
					break;
				case EPHT_Code:	// 代码
					item.strText = pMerch->m_MerchInfo.m_StrMerchCode;
					break;
				case EPHT_Name:
					item.strText = pMerch->m_MerchInfo.m_StrMerchCnName;
					{
						// 是否是自选股 & 当前板块是不是自选板块
						const T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerch);
						CBlockLikeMarket *pBlockCur = GetCurrentBlock();
						if ( NULL != pUserBlock && pUserBlock->m_clrBlock != COLORNOTCOUSTOM
							&& ( NULL == pBlockCur || !pBlockCur->IsUserBlock() ))
						{
							/*item.crFgClr = pUserBlock->m_clrBlock;*/
						}

						// 是否有标记
						GV_DRAWMARK_ITEM markItem;
						if ( InitDrawMarkItem(pMerch, markItem) )
						{
							item.markItem = markItem;
						}
					}
					break;
				case EPHT_PreClose:
					item.strText = Float2String(Res.fPreClose, iSaveDec, false, false);
					break;
				case EPHT_Close:
					item.strText = Float2String(Res.fClose, iSaveDec, false, false);
					break;
				case EPHT_High:
					item.strText = Float2String(Res.fHigh, iSaveDec, false, false);
					break;
				case EPHT_Low:
					item.strText = Float2String(Res.fLow, iSaveDec, false, false);
					break;
				case EPHT_Vol:
					item.strText = Float2String(Res.fVol*100, 0, true, false);	// 股
					item.crFgClr = GetIoViewColor(ESCVolume2);
					break;
				case EPHT_Amount:
					item.strText = Float2String(Res.fAmount, 0, true, false);
					item.crFgClr = GetIoViewColor(ESCAmount);
					break;
				case EPHT_MarketRate:
					item.strText = Float2String(Res.fMarketRate*100, 2, false, false);
					break;
				case EPHT_TradeRate:
					item.strText = Float2String(Res.fTradeRate*100, 2, false, false);
					break;
				case EPHT_VolChangeRate:
					item.strText = Float2String(Res.f5DayVolChangeRate*100, 2, false, false);
					break;

				case EPHT_RiseFall: // 涨跌幅度
					{
						// 两行
						float fRiseFall = Res.fClose - Res.fPreClose;
						float fRate = 0.0f;
						if ( Res.fPreClose != 0.0f )
						{
							fRate = fRiseFall/Res.fPreClose*100;
						}
						item.strText = Float2String(fRiseFall, iSaveDec, false, false) + _T("\r\n");
						item.strText += Float2String(fRate, iSaveDec, false, false, true);

						item.nFormat = DT_RIGHT;
						if ( fRate > 0.000001f )
						{
							item.crFgClr = clrRise;
						}
						else if ( fRate < -0.000001f )
						{
							item.crFgClr = clrFall;
						}
					}
					break;
				case EPHT_ShakeRate:	// 震荡幅度
					{
						// 两行
						float fShake = Res.fHigh - Res.fLow;
						float fRate = 0.0f;
						if ( Res.fLow != 0.0f )
						{
							fRate = fShake/Res.fLow*100;
						}
						item.strText = Float2String(fShake, iSaveDec, false, false) + _T("\r\n");
						item.strText += Float2String(fRate, iSaveDec, false, false, true);

						item.nFormat = DT_RIGHT;
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
		}
	}
	
	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}


bool CIoViewPhaseSort::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_spThis != NULL );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;

	const MerchArray &aMerchs = m_spThis->m_ParamForDisplay.m_aMerchsToCalc;
	const PhaseSortCalcResultArray &aResults = m_spThis->m_ParamForDisplay.m_aCalcResults;
	ASSERT( aMerchs.GetSize() == aResults.GetSize() );
	ASSERT( iIndex2 >= 0 && iIndex2 < aMerchs.GetSize() );
	ASSERT( iIndex1 >= 0 && iIndex1 < aMerchs.GetSize() );

	CMerch *pMerch1 = aMerchs[iIndex1];
	CMerch *pMerch2 = aMerchs[iIndex2];

	const T_PhaseSortCalcReslt &Res1 = aResults[iIndex1];
	const T_PhaseSortCalcReslt &Res2 = aResults[iIndex2];

	ASSERT( pMerch1 != NULL && pMerch2 != NULL );
	if ( NULL == pMerch1 || pMerch2 == NULL )
	{
		return false;	// 任意
	}

	BOOL bAscend = m_spThis->m_GridCtrl.GetSortAscending();

	const int iCol = m_spThis->m_GridCtrl.m_CurCol;
	ASSERT( iCol >= 0 && iCol < m_spThis->m_aColumnHeaders.GetSize() );
	E_PhaseHeaderType eHeader = m_spThis->m_aColumnHeaders[iCol].m_eHeaderType;
	bool bRet = false;
	float fValue1 = 0.0f;
	float fValue2 = fValue1;
	bool32 bNeedZeroEndCmp = true; // 是否要进行0要排在最后的比较
	switch (eHeader)
	{
	case EPHT_No:
		ASSERT( 0 );
		break;
	case EPHT_Code:	// 英文字母
		bRet = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCode, -1,
			pMerch2->m_MerchInfo.m_StrMerchCode, -1) == CSTR_LESS_THAN;
		bNeedZeroEndCmp = false;
		break;
	case EPHT_Name:		// 名称
		bRet = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCnName, -1,
			pMerch2->m_MerchInfo.m_StrMerchCnName, -1)  == CSTR_LESS_THAN;
		bNeedZeroEndCmp = false;
		break;
	case EPHT_PreClose:
		//bRet = Res1.fPreClose > Res2.fPreClose;
		fValue1 = Res1.fPreClose;
		fValue2 = Res2.fPreClose;
		break;
	case EPHT_Close:
		//bRet = Res1.fClose > Res2.fClose;
		fValue1 = Res1.fClose;
		fValue2 = Res2.fClose;
		break;
	case EPHT_High:
		//bRet = Res1.fHigh > Res2.fHigh;
		fValue1 = Res1.fHigh;
		fValue2 = Res2.fHigh;
		break;
	case EPHT_Low:
		//bRet = Res1.fLow > Res2.fLow;
		fValue1 = Res1.fLow;
		fValue2 = Res2.fLow;
		break;
	case EPHT_Vol:
		//bRet = Res1.fVol > Res2.fVol;
		fValue1 = Res1.fVol;
		fValue2 = Res2.fVol;
		break;
	case EPHT_Amount:
		//bRet = Res1.fAmount > Res2.fAmount;
		fValue1 = Res1.fAmount;
		fValue2 = Res2.fAmount;
		break;
	case EPHT_MarketRate:
		//bRet = Res1.fMarketRate > Res2.fMarketRate;
		fValue1 = Res1.fMarketRate;
		fValue2 = Res2.fMarketRate;
		break;
	case EPHT_TradeRate:
		//bRet = Res1.fTradeRate > Res2.fTradeRate;
		fValue1 = Res1.fTradeRate;
		fValue2 = Res2.fTradeRate;
		break;
	case EPHT_VolChangeRate:
		{
			// 0.0f是要排在最后面的
			fValue1 = Res1.f5DayVolChangeRate;
			fValue2 = Res2.f5DayVolChangeRate;
		}
		break;
	case EPHT_RiseFall: // 涨跌幅度
		{
			// 两行
			// 对于涨跌幅这个特殊的排序，0是要排最后的
			float fRiseFall1 = Res1.fClose - Res1.fPreClose;
			float fRiseFall2 = Res2.fClose - Res2.fPreClose;			
			if ( Res1.fPreClose != 0.0f )
			{
				fValue1 = fRiseFall1/Res1.fPreClose;
			}
			if ( Res2.fPreClose != 0.0f )
			{
				fValue2 = fRiseFall2/Res2.fPreClose;
			}
		}
		break;
	case EPHT_ShakeRate:	// 震荡幅度
		{
			// 两行
			float fShake1 = Res1.fHigh - Res1.fLow;
			float fShake2 = Res2.fHigh - Res2.fLow;
			if ( Res1.fLow != 0.0f )
			{
				fValue1 = fShake1/Res1.fLow*100;
			}
			if ( Res2.fLow != 0.0f )
			{
				fValue2 = fShake2/Res2.fLow*100;
			}
		}
		break;
	default:
		ASSERT( 0 );
	}

	if ( bNeedZeroEndCmp )
	{
		if ( fValue1 != 0.0f && fValue2 != 0.0f )
		{
			bRet = fValue1 < fValue2;
		}
		else if ( fValue1 == 0.0f )
		{
			bRet = !bAscend;	// 0.0f小于它
		}
		else
		{
			bRet = bAscend != FALSE;	// 性能警报- -
		}
	}

	return bRet;
}

void CIoViewPhaseSort::OnGridCacheHint( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	// 如果当前排序字段为非排序或者本地排序字段，则不需要理会，如果为服务器排序字段，则检查本地存在的排行数据是否足够
	// 不够则需要重发排行请求，等待数据回归，则进行刷新显示
	//GV_CACHEHINT *pCacheHint = (GV_CACHEHINT *)pNotifyStruct;
	//if ( NULL != pCacheHint && pCacheHint->range.IsValid() )	// 如果没有服务器数据需求，不需要处理
	//{
	//	

	//}

	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

void CIoViewPhaseSort::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcClient;
	GetWindowRect(rcClient);
	if ( !rcClient.PtInRect(pos) )
	{
		pos = rcClient.TopLeft();
		DoTrackMenu(pos);
		return;
	}
	
	if ( pWnd->GetSafeHwnd() == m_hWnd )
	{
		DoTrackMenu(pos);
		return;
	}

	CIoViewBase::OnContextMenu(pWnd, pos);
	return;	
}

void CIoViewPhaseSort::DoTrackMenu(CPoint pos)
{
	CNewMenu menu;
	menu.CreatePopupMenu();
	

	int32 iCmd = 1;
	int32 iSortTypeBase = -1;
	if ( m_ParamForDisplay.m_aCalcResults.GetSize() > 0 )
	{
		// 有数据可以设置

		menu.AppendMenu(MF_STRING, iCmd++, _T("打开商品"));
		menu.SetDefaultItem(1, FALSE);
		menu.AppendMenu(MF_SEPARATOR);

		// 网格线
		BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
		menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("隐藏网格线"): _T("显示网格线"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// 设置区间等参数
		menu.AppendMenu(MF_STRING, iCmd++, _T("设置排行参数"));
		menu.AppendMenu(MF_STRING, iCmd++, _T("重新计算   F8"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// 几个排行列类型
		iCmd++;
		iSortTypeBase = iCmd;
		menu.AppendMenu(MF_STRING, iCmd+EPST_RiseFallPecent, _T("涨跌幅度"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_TradeRate, _T("换手率排行"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_VolChangeRate, _T("量变幅度"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_ShakeRate, _T("震荡幅度"));
		menu.CheckMenuItem(iCmd+m_PhaseOpenBlockParam.m_ePhaseSortType, MF_CHECKED |MF_BYCOMMAND);
		iCmd += EPST_Count;
		menu.AppendMenu(MF_SEPARATOR);
	}
	else
	{
		// 设置区间等参数
		iCmd++;	// 跳过打开商品
		menu.AppendMenu(MF_STRING, iCmd++, _T("设置排行参数"));
		menu.AppendMenu(MF_SEPARATOR);
	}
	
	// 普通按钮
	AppendStdMenu(&menu);

	menu.LoadToolBar(g_awToolBarIconIDs);

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(menu);
	}

	int32 iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_RETURNCMD |TPM_NONOTIFY, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet > 0 && iRet < iCmd )
	{
		if ( iRet == 1 )
		{
			// 打开商品
			CMerch *pMerchXml = GetMerchXml();
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pMainFrame->OnShowMerchInChart(pMerchXml, this);
			}
		}
		else if ( 2 == iRet )
		{
			BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
			m_GridCtrl.ShowGridLine(bShowGrid ? GVL_NONE : GVL_BOTH);
			m_GridCtrl.Refresh();
		}
		else if (3 == iRet)
		{
			// 设置参数
			T_PhaseOpenBlockParam BlockParam;
			if ( CDlgPhaseSort::PhaseSortSetting(BlockParam, &m_PhaseOpenBlockParam)
				&& m_PhaseOpenBlockParam != BlockParam )
			{
				if ( !m_PhaseOpenBlockParam.IsCalcParamSame(BlockParam) )
				{
					OpenBlock(BlockParam, true);	// 需要重计算打开
				}
				else if ( m_PhaseOpenBlockParam.m_ePhaseSortType != BlockParam.m_ePhaseSortType )
				{
					SetSortColumnBySortType(BlockParam.m_ePhaseSortType);	// 更换排序类型
				}
			}
		}
		else if ( 4 == iRet )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			OpenBlock(BlockParam, true);	// 重新打开
		}
		else if ( iSortTypeBase > 0 )
		{
			// 改变排行类型
			E_PhaseSortType eSort = (E_PhaseSortType)(iRet-iSortTypeBase);
			if ( eSort < EPST_Count )
			{
				SetSortColumnBySortType(eSort);
			}
		}
	}
	else
	{
		// 普通功能
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(iRet, 0), 0);
	}
}

CMerch * CIoViewPhaseSort::GetMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();

	if ( m_GridCtrl.IsValid(rangeSel) )
	{
		pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0);
		if ( NULL != pMerch )
		{
			return pMerch;
		}
	}
	
	return m_pMerchXml;
}

void CIoViewPhaseSort::InitializeTitleString()
{
	m_aTitleString.RemoveAll();

	// 阶段排行分析--涨跌幅度 市场:自选股 区间:2010-03-14(三),2011-01-02(五) 点右键进行操作
	T_TitleString title;
	CString StrTitle;
	StrTitle.Format(_T("阶段排行分析 -- %s "), GetPhaseSortTypeName(m_PhaseOpenBlockParam.m_ePhaseSortType).GetBuffer());
	title.m_StrTitle = StrTitle;
	title.m_clr = RGB(250,50,50);
	m_aTitleString.Add(title);

	COLORREF clrVol = GetIoViewColor(ESCVolume);
	COLORREF clrText = GetIoViewColor(ESCText);
	title.m_StrTitle = _T("市场: ");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		title.m_StrTitle = _T("       ");
	}
	else
	{
		title.m_StrTitle = pBlock->m_blockInfo.m_StrBlockName;
	}
	title.m_StrTitle += _T(" ");
	title.m_clr = clrVol;
	m_aTitleString.Add(title);

	CTime TimeStart(m_PhaseOpenBlockParam.m_TimeStart.GetTime());
	CTime TimeEnd(m_PhaseOpenBlockParam.m_TimeEnd.GetTime());
	title.m_StrTitle = _T("区间: ");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
	title.m_StrTitle.Format(_T("%04d-%02d-%02d(%s),%04d-%02d-%02d(%s) "),
		TimeStart.GetYear(), TimeStart.GetMonth(), TimeStart.GetDay(), GetWeekString(TimeStart).GetBuffer(),
		TimeEnd.GetYear(), TimeEnd.GetMonth(), TimeEnd.GetDay(), GetWeekString(TimeEnd).GetBuffer());
	title.m_clr = clrVol;
	m_aTitleString.Add(title);

	title.m_StrTitle = _T("点右键进行操作");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
}

void CIoViewPhaseSort::InitializeColumnHeader( E_PhaseSortType eType )
{
	CArray<E_PhaseHeaderType, E_PhaseHeaderType> aHeaders;
	switch (eType)
	{
	case EPST_RiseFallPecent:	// 涨跌
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_ShakeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_TradeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_VolChangeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_VolChangeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_RiseFall);
		}
		break;
	default:
		ASSERT( 0 );
	}

	m_aColumnHeaders.RemoveAll();
	for ( int32 i=0; i < aHeaders.GetSize() ; i++ )
	{
		m_aColumnHeaders.Add( T_ColumnHeader(aHeaders[i]) );
	}
}

bool32 CIoViewPhaseSort::OpenBlock(const T_PhaseOpenBlockParam &BlockParam, bool32 bReOpen, bool32 bAskUserSetting/* = false*/)
{
	if ( !bReOpen && m_PhaseOpenBlockParam.IsCalcParamSame(BlockParam) )
	{
		return true;	// 不需要重复打开
	}

	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(BlockParam.m_iBlockId);
	if ( NULL != pBlock
		&& BlockParam.m_TimeStart <= BlockParam.m_TimeEnd
		&& BlockParam.m_ePhaseSortType < EPST_Count )
	{
		ASSERT( m_ParamForCalc.m_eResultFlag != EPSCRF_Calculating );
		CancelPhaseSortCalc(&m_ParamForCalc);		// 终止计算

		m_PhaseOpenBlockParam = BlockParam;
		m_PhaseOpenBlockParam.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;

		m_PhaseBlockParamXml = m_PhaseOpenBlockParam;		// 将xml的结果重置，防止init时再次打开

		// 赋值计算参数
		m_ParamForCalc.m_TimeStart	= BlockParam.m_TimeStart;
		m_ParamForCalc.m_TimeEnd	= BlockParam.m_TimeEnd;
		m_ParamForCalc.m_bDoPreWeight = BlockParam.m_bDoPreWeight;
		m_ParamForCalc.m_aMerchsToCalc.Copy(pBlock->m_blockInfo.m_aSubMerchs);

		// 其它参数
		m_ParamForCalc.m_hWndMsg = m_hWnd;
		m_ParamForCalc.m_uMsgBase = UM_CALCMSG;
		m_ParamForCalc.m_pAbsDataManager = AfxGetDocument()->m_pAbsDataManager;

		// 显示数据初始化
		m_ParamForDisplay.CopyResult(m_ParamForCalc);
		m_ParamForDisplay.m_aCalcResults.SetSize(m_ParamForDisplay.m_aMerchsToCalc.GetSize());
		ZeroMemory(m_ParamForDisplay.m_aCalcResults.GetData(), m_ParamForDisplay.m_aCalcResults.GetSize()*sizeof(T_PhaseSortCalcReslt));

		//ChangeTabToCurrentBlock();

		// 切换显示
		InitializeTitleString();
		InitializeColumnHeader(m_PhaseOpenBlockParam.m_ePhaseSortType);	// 初始化列表头
		UpdateTableHeader();	// 表头初始化
		SetRowHeightAccordingFont();	// 初始化行高
		UpdateTableAllContent();	// 表格数据重刷新
		m_GridCtrl.SetSortColumn(3);	// 第4列必然排序

		DWORD dwTime = timeGetTime();
		m_mapExpAmount.clear();
		RedrawWindow();	// 此时应当显示一次


		if ( m_ParamForCalc.m_aMerchsToCalc.GetSize() > 0 )	// 有商品可计算
		{
			// 以请求行情数据开始
			RequestExpKline();	// 请求指数K线

			if ( m_PhaseOpenBlockParam.m_bDoPreWeight )
			{
				//RequestWeightData();	// 请求除权数据 // 不请求除权数据，有就有，没有就没有
			}

			int32 iReq = RequestAllPriceData();
			TRACE(_T("阶段排行发送权重,指数K线,实时报价: %d ms\r\n"), timeGetTime() - dwTime);
			int32 iRes = -1;
			if ( iReq > 0 )
			{
				m_eStage = ES_WaitPrice;
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("刷新行情数据..."), _T("刷新行情数据"));
			}
			else
			{
				m_eStage = ES_WaitCalc;
				//--- wangyongxue 2016/09/29 使用PostMessage经常抛出失败，改用SendMessage
				//				PostMessage(UM_DOCALC, 0, 0);	// 没有行情请求，直接计算
				SendMessage(UM_DOCALC, 0, 0);	
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("正在计算..."), _T(""));	// 显示正在计算
			}
			// 取消数据关注
			m_aMerchsNeedPrice.RemoveAll();
			m_aSmartAttendMerchs.RemoveAll();
			if ( iRes == IDCANCEL && ES_WaitPrice == m_eStage )
			{
				// 取消行情刷新，一样的要开始计算
				m_eStage = ES_WaitCalc;
				PostMessage(UM_DOCALC, 0, 0);
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("开始计算..."), _T(""));	// 显示开始计算
			}

			E_Stage eStateOld = m_eStage;
			m_eStage = ES_Count;	// 状态清除
			if ( iRes == IDCANCEL )
			{
				CancelPhaseSortCalc(&m_ParamForCalc);	// 中断计算
			}
			ClearPhaseSortCalcParamResource(&m_ParamForCalc);	// 清理资源

			if ( ES_WaitPrice != eStateOld )
			{
				m_TimeLastReqPrice = m_pAbsCenterManager->GetServerTime(); // 数据请求完毕

				ASSERT( ES_WaitCalc == eStateOld );
				// 计算完毕，查看计算结果
				if ( m_ParamForCalc.m_eResultFlag == EPSCRF_UserCancel )
				{
					MessageBox(_T("用户取消计算，部分数据没有计算！"), _T("提示"));
				}
				else if ( m_ParamForCalc.m_eResultFlag == EPSCRF_Error )
				{
					MessageBox(_T("计算出错！！"), _T("提示"));
				}
				
				// 不管什么结果，拷贝结果集
				m_ParamForDisplay.CopyResult(m_ParamForCalc);

				// 重新排序
				CCellID cell(0, 3);	// 第四列必然是排序列
				m_GridCtrl.SetSortAscending(TRUE);	// 从x序开始
				OnFixedRowClickCB(cell);
				Invalidate();	// 刷新显示
			}
		}

		return true;
	}
	else if ( bAskUserSetting )
	{
		// 这个时候应当是没有初始化参数的，所以该调出对话框询问用户怎么配置 TODO
		T_PhaseOpenBlockParam TmpParam = BlockParam;
		if ( CDlgPhaseSort::PhaseSortSetting(TmpParam, &BlockParam) )
		{
			return OpenBlock(TmpParam, bReOpen, false);		// 不要在询问了
		}
	}
	return false;
}

int32 CIoViewPhaseSort::RequestAllPriceData(bool32 bForce /*= false*/)
{
	if(NULL == m_pAbsCenterManager)
	{
		return 0;
	}
	// 超过若干时间，请求所有行情数据, 第一次必请求
	// 请求所有股票的行情数据
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CGmtTimeSpan SpanDiscard(0, 0, 30, 0);	// 若干分钟后，该请求失效, 需要重新请求
	
	m_aSmartAttendMerchs.RemoveAll();
	m_aMerchsNeedPrice.SetSize(0, m_ParamForDisplay.m_aMerchsToCalc.GetSize());
	const MerchArray &aMerchSrc = m_ParamForDisplay.m_aMerchsToCalc;
	const int32 iSize = aMerchSrc.GetSize();
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchSrc[i];
		MerchRequestTimeMap::iterator it = s_mapMerchReqTime.find(pMerch);
		if ( it != s_mapMerchReqTime.end() && TimeNow - it->second < SpanDiscard )
		{
			continue;	// 该请求前面已经请求过了
		}
		if ( it != s_mapMerchReqTime.end()
			&& m_mapCircAssert.count(pMerch) > 0 )
		{
			// 有请求，且该商品有有效的RealTime值
			if ( !IsNowInTradeTimes(pMerch, true) )
			{
				continue;		// 非交易时间段内的过滤掉它
			}
		}

		m_aMerchsNeedPrice.Add(pMerch);	// 加入需要更新的队列
	}

// 	for ( int32 i=0; i < m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() ; i++ )
// 	{
// 		CBreed *pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
// 		if ( NULL == pBreed )
// 		{
// 			continue;
// 		}
// 
// 		for ( int32 j=0; j < pBreed->m_MarketListPtr.GetSize() ; j++ )
// 		{
// 			CMarket *pMarket = pBreed->m_MarketListPtr[j];
// 			if ( NULL != pMarket 
// 				&& (pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn
// 				|| pMarket->m_MarketInfo.m_eMarketReportType == ERTExp)
// 				)
// 			{
// 				// 所有国内股票&&指数的行情都需要
// 				m_aMerchsNeedPrice.Append(pMarket->m_MerchsPtr);
// 			}
// 		}
// 	}

	int32 iNeedSize= m_aMerchsNeedPrice.GetSize();
	TryRequestMoreNeedPriceMerchs();

	return iNeedSize;
}

int32 CIoViewPhaseSort::TryRequestMoreNeedPriceMerchs()
{
	const int32 iMaxMerchs = 20;	// 保证队列中只有这么多商品
	if ( m_aSmartAttendMerchs.GetSize() < iMaxMerchs && m_aMerchsNeedPrice.GetSize() > 0 )
	{
		CArray<CSmartAttendMerch, CSmartAttendMerch &> aTTtmp;
		const int32 iNeed = iMaxMerchs - m_aSmartAttendMerchs.GetSize();
		CSmartAttendMerch att;
		for ( int32 i=0; i < iNeed ; i++ )
		{
			if ( m_aMerchsNeedPrice.GetSize() <= 0 )
			{
				break;
			}
			att.m_iDataServiceTypes = EDSTPrice;
			att.m_pMerch = m_aMerchsNeedPrice[m_aMerchsNeedPrice.GetUpperBound()];
			aTTtmp.Add(att);

			m_aMerchsNeedPrice.RemoveAt( m_aMerchsNeedPrice.GetUpperBound() );	// 移除最尾端数据
		}

		RequestAttendMerchs(aTTtmp, true);	// 数据重视强制发出的
		m_aSmartAttendMerchs.Append( aTTtmp );	// 已经发出过了就加入队列
		return aTTtmp.GetSize();
	}
	return 0;
}

void CIoViewPhaseSort::RequestAttendMerchs(bool32 bForce)
{
	RequestAttendMerchs(m_aSmartAttendMerchs, bForce);
}

void CIoViewPhaseSort::RequestAttendMerchs( const CArray<CSmartAttendMerch, CSmartAttendMerch &> &aSmarts, bool32 bForce )
{
	CMmiReqRealtimePrice req;
	CSmartAttendMerch att;
	req.m_aMerchMore.SetSize(0, aSmarts.GetSize());
	
	if ( aSmarts.GetSize() > 0 )
	{
		req.m_iMarketId = aSmarts[0].m_pMerch->m_MerchInfo.m_iMarketId;
		req.m_StrMerchCode = aSmarts[0].m_pMerch->m_MerchInfo.m_StrMerchCode;
	}
	CMerchKey key;
	for ( int32 i=1; i < aSmarts.GetSize() ; i++ )
	{
		CMerch *pMerch = aSmarts[i].m_pMerch;
		key.m_iMarketId = pMerch->m_MerchInfo.m_iMarketId;
		key.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
		req.m_aMerchMore.Add(key);
	}
	
	DoRequestViewData(req, bForce);
}

void CIoViewPhaseSort::RequestExpKline()
{
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}
	// 请求几个指数特定的K线过来
	MerchArray aMerchsExp;
	GetSpecialAccordExpMerch(aMerchsExp);
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CGmtTime TimeStart = m_ParamForCalc.m_TimeStart;
	SaveDay(TimeStart);
	for ( int32 i=0; i < aMerchsExp.GetSize() ; i++ )
	{
		CMmiReqMerchKLine reqKline;
		reqKline.m_iMarketId = aMerchsExp[i]->m_MerchInfo.m_iMarketId;
		reqKline.m_StrMerchCode = aMerchsExp[i]->m_MerchInfo.m_StrMerchCode;
		
		reqKline.m_eKLineTypeBase = EKTBDay;	
		reqKline.m_eReqTimeType = ERTYSpecifyTime;
		reqKline.m_TimeStart = TimeStart;
		reqKline.m_TimeEnd = TimeNow;		// 保证K线连续
		
		DoRequestViewData(reqKline);		// 先请求日K线，K线数据先于实时报价数据过来，可以不响应处理
	}
}

void CIoViewPhaseSort::RequestWeightData()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( NULL == pDoc )
	{
		return;
	}
	CAbsDataManager *pAbsDataManager = pDoc->m_pAbsDataManager;
	if (!pAbsDataManager)
	{
		return;
	}

	CString StrFilePath;
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = NULL;

	for ( int32 i=0; i < m_ParamForDisplay.m_aMerchsToCalc.GetSize() ; i++ )
	{
		CMerch *pMerch = m_ParamForDisplay.m_aMerchsToCalc[i];
		if ( CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
		{
			continue;
		}

		// 请求除权数据
		// 1: 看本地有没有除权数据
		if ( pMerch->m_bHaveReadLocalWeightFile )
		{
			continue;
		}
		else if ( !pMerch->m_bHaveReadLocalWeightFile && 0 == pMerch->m_TimeLastUpdateWeightDatas.GetTime() )
		{
			// 简单判断
			// 取文件路径:
			if ( pAbsDataManager->GetPublishFilePath(EPFTWeight, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath)
				&& (hFind=::FindFirstFile(StrFilePath, &wfd)) == INVALID_HANDLE_VALUE )
			{
				DEL_HANDLE(hFind);
				continue;	// 有则证明有了
			}
		}
		
		// 2: 向服务器发请求		
		CMmiReqPublicFile info;
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		info.m_ePublicFileType	= EPFTWeight;
		info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;
		
		DoRequestViewData(info, true);	// 视图不过滤的请求
	}
}

void CIoViewPhaseSort::OnMsgDoCalc()
{
	// 有可能是从行情刷新过来的，也有可能是直接过来的
	// 启动计算
	// 赋值参数队列
	if ( ES_WaitCalc != m_eStage )
	{
		return;
	}
	const int32 iSize = m_ParamForCalc.m_aMerchsToCalc.GetSize();
	m_ParamForCalc.m_aDataParamToCalc.SetSize(iSize);
	PhaseSortCalcDataParamArray &aData = m_ParamForCalc.m_aDataParamToCalc;
	MerchArray &aMerchs = m_ParamForCalc.m_aMerchsToCalc;
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchs[i];
		aData[i].pMerch = pMerch;
		CalcPhaseRequestData(aData[i]);
	}

	// 开始启动计算
	if ( !StartPhaseSortCalc(&m_ParamForCalc) )
	{
		if ( m_DlgWait.GetSafeHwnd() != NULL )
		{
			m_DlgWait.CancelDlg(IDCANCEL);
		}
		ASSERT( 0 );	// 什么情况
		return;
	}
	
	m_DlgWait.SetTitle(_T("开始计算..."));
	m_DlgWait.SetProgress(0, m_ParamForDisplay.m_aMerchsToCalc.GetSize()+1);
}

LRESULT CIoViewPhaseSort::OnMsgCalcStart( WPARAM w, LPARAM l )
{
	//
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	m_DlgWait.SetTitle(_T("正在计算..."));
	return 1;
}

LRESULT CIoViewPhaseSort::OnMsgCalcPos( WPARAM w, LPARAM l )
{
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	MSG msg = *GetCurrentMessage();
	while (::PeekMessage(&msg, m_hWnd, UM_CALCMSG+EPSCM_Pos, UM_CALCMSG+EPSCM_Pos, TRUE))
	{
		// 仅取最后一个
	}
	w = msg.wParam;
	l = msg.lParam;
	CString Str;
	int32 iCurPos = (int32)l;
	const int32 iSize = m_ParamForDisplay.m_aMerchsToCalc.GetSize();
	if ( iCurPos >= 0 && iCurPos < m_ParamForDisplay.m_aMerchsToCalc.GetSize() )
	{
		CString StrMerchName;
		StrMerchName = m_ParamForDisplay.m_aMerchsToCalc[iCurPos]->m_MerchInfo.m_StrMerchCnName;
		m_DlgWait.SetTipText(StrMerchName);
	}
	if ( iSize > 0 )
	{
		int32 iPec = l*100/iSize;
		Str.Format(_T("%d%%"), iPec);
		m_DlgWait.AppendToShowTitle(Str);
		m_DlgWait.SetProgress(iPec, 100);
	}
	return 1;	
}

LRESULT CIoViewPhaseSort::OnMsgCalcEnd( WPARAM w, LPARAM l )
{
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	m_DlgWait.CancelDlg(IDOK);		// 结束了
	return 1;
}

LRESULT CIoViewPhaseSort::OnMsgCalcReqData( WPARAM w, LPARAM l )
{
	// 请求数据
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	T_PhaseSortCalcRequestData *pReq = (T_PhaseSortCalcRequestData *)l;
	T_PhaseSortCalcParam *pParam = (T_PhaseSortCalcParam *)w;
	ASSERT( pParam == &m_ParamForCalc && pReq != NULL && pReq->pMerch != NULL );
	if ( pParam == &m_ParamForCalc && pReq != NULL && pReq->pMerch != NULL )
	{
		CalcPhaseRequestData(*pReq);
	}
	return 1;
}

bool32 CIoViewPhaseSort::CalcPhaseRequestData( INOUT T_PhaseSortCalcRequestData &reqData )
{
	if ( NULL == reqData.pMerch )
	{
		return false;
	}

	CMerch *pMerch = reqData.pMerch;
	reqData.fCircAssert = reqData.fMarketAmount = 0.0f;
	if ( NULL != pMerch->m_pRealtimePrice )
	{
		reqData.m_RealTimePrice = *pMerch->m_pRealtimePrice;		// 存储它的实时报价
	}
	// 流通股本
	float fTradeRate = 0.0f;
	if ( NULL != pMerch->m_pFinanceData && 0.0f != pMerch->m_pFinanceData->m_fCircAsset )	// 财务数据中的
	{
		reqData.fCircAssert = pMerch->m_pFinanceData->m_fCircAsset;
		m_mapCircAssert[pMerch] = reqData.fCircAssert;		// 存储它
	}
	else if ( NULL != pMerch->m_pRealtimePrice && 0.0f != (fTradeRate=pMerch->m_pRealtimePrice->m_fTradeRate) )
	{
		// *100股 *100百分比
		reqData.fCircAssert =  pMerch->m_pRealtimePrice->m_fVolumeTotal/fTradeRate*10000;	// 反推流通股, server的换手率就是%了的
		
		m_mapCircAssert[pMerch] = reqData.fCircAssert;		// 存储它
	}
	else
	{
		MerchFloatDataMap::iterator it = m_mapCircAssert.find(pMerch);
		if ( it != m_mapCircAssert.end() )
		{
			reqData.fCircAssert = it->second;		// 使用存储的
		}
		else
		{
			// 是否尝试从财务数据文件中读取
			CString StrPath;
			if (!AfxGetDocument()->m_pAbsDataManager)
			{
				return false;
			}
			AfxGetDocument()->m_pAbsDataManager->GetPublishFilePath(EPFTF10, pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, StrPath);
			
			CStdioFile FileFinance;
			CFinanceData stFinance;
			if ( FileFinance.Open(StrPath, CFile::modeRead | CFile::typeBinary) )
			{
				int32 iSize = FileFinance.Read(&stFinance, sizeof(stFinance));
				if ( iSize == sizeof(stFinance) )
				{
					reqData.fCircAssert = stFinance.m_fCircAsset;
				}			
			}
		}
	}
	
	// 市场金额???计算K线吗？
	CMerch *pExpMerch = GetMerchAccordExpMerchStatic(pMerch);
	if ( NULL != pExpMerch )
	{
		MerchFloatDataMap::iterator it = m_mapExpAmount.find(pExpMerch);
		if ( it != m_mapExpAmount.end() )
		{
			reqData.fMarketAmount = it->second;
		}
		else
		{
			// 查找K线计算
			CGmtTime TimeStart = m_ParamForCalc.m_TimeStart;
			CGmtTime TimeEnd = m_ParamForCalc.m_TimeEnd;
			SaveDay(TimeStart);
			
			int32 iPos;
			CMerchKLineNode *pKlines = NULL;
			if ( pExpMerch->FindMerchKLineNode(EKTBDay, iPos, pKlines) && NULL != pKlines )
			{
				const int32 iPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKlines->m_KLines, TimeStart);
				const int32 iPosEnd = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKlines->m_KLines, TimeEnd);
				if ( iPosStart >= 0
					&& iPosEnd >= iPosStart
					&& iPosEnd < pKlines->m_KLines.GetSize() )
				{
					float fAmount = 0.0f;
					for ( int32 i=iPosStart; i <= iPosEnd ; i++ )
					{
						fAmount += pKlines->m_KLines[i].m_fAmount;
					}
					if ( fAmount > 0.0f )
					{
						m_mapExpAmount[pExpMerch] = fAmount;
						reqData.fMarketAmount = fAmount;
					}
				}
			}
		}
	}

	return reqData.fMarketAmount != 0.0f && reqData.fCircAssert != 0.0f;
}

const T_SimpleTabInfo* CIoViewPhaseSort::GetSimpleTabInfo( int32 iTabIndex )
{
	if ( iTabIndex >= 0 && iTabIndex < s_KiTabInfoCount )
	{
		return s_KaTabInfos + iTabIndex;
	}
	ASSERT( 0 );
	return NULL;
}

void CIoViewPhaseSort::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	CIoViewBase::GetAttendMarketDataArray(aAttends);	// 离线视图，当没有关注商品时，实际上也不需要关注该商品了
}

CBlockLikeMarket * CIoViewPhaseSort::GetCurrentBlock()
{
	return CBlockConfig::Instance()->FindBlock(m_PhaseOpenBlockParam.m_iBlockId);
}

void CIoViewPhaseSort::SetSortColumnBySortType( E_PhaseSortType eType )
{
	E_PhaseHeaderType eHeader = EPHT_Count;
	switch (eType)
	{
	case EPST_RiseFallPecent:		// 涨跌幅
		{
			eHeader = EPHT_RiseFall;
		}
		break;
	case EPST_TradeRate:		// 换手率
		{
			eHeader = EPHT_TradeRate;
		}
		break;
	case EPST_VolChangeRate:		// 量变
		{
			eHeader = EPHT_VolChangeRate;
		}
		break;
	case EPST_ShakeRate:		// 震荡
		{
			eHeader = EPHT_ShakeRate;
		}
		break;
	default:
		ASSERT( 0 );
		return;
	}
	m_PhaseOpenBlockParam.m_ePhaseSortType = eType;
	InitializeTitleString();
	InitializeColumnHeader(m_PhaseOpenBlockParam.m_ePhaseSortType);	// 初始化列表头
	UpdateTableHeader();	// 表头初始化

	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		if ( m_aColumnHeaders[i].m_eHeaderType == eHeader )
		{
			// 该列排序
			CCellID cell(0, i);
			m_GridCtrl.SetSortAscending(TRUE);	// 初始化排序
			OnFixedRowClickCB(cell);
			break;
		}
	}
	Invalidate();
	m_GridCtrl.Refresh();
}

//////////////////////////////////////////////////////////////////////////
// 
CArray<CIoViewPhaseSort::T_ColumnHeader, const CIoViewPhaseSort::T_ColumnHeader &> CIoViewPhaseSort::T_ColumnHeader::s_aInitHeaders;
void CIoViewPhaseSort::T_ColumnHeader::InitStatic()
{
	if ( s_aInitHeaders.GetSize() <= 0 )
	{
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_No,			_T("    "),		false, 3.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Code,		_T("代码"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Name,		_T("名称"), false, 5.8f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_RiseFall,	_T("涨跌幅度"), false, 7.3f, 1.0f, 1.5f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_TradeRate,	_T("换手率%"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_VolChangeRate, _T("5日量变%"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_ShakeRate,	_T("震荡幅度"), false, 7.3f, 1.0f, 1.5f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_PreClose,	_T("前收盘"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_High,		_T("最高"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Low,		_T("最低"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Close,		_T("收盘"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Vol,		_T("成交量"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Amount,		_T("总金额"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_MarketRate, _T("市场比"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Count, _T("")) );		// 无效值
	}
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader( E_PhaseHeaderType eHeaderType, LPCTSTR pszName, bool32 bSort, float fWidthByChar, float fFixedRowHeight, float fNonFixedRowHeight )
{
	m_eHeaderType = eHeaderType;
	m_StrHeader = pszName;
	m_bNeedSort = bSort;
	m_fPreferWidthByChar = fWidthByChar;
	m_fFixedRowHeightRatio = fFixedRowHeight;
	m_fNonFixedRowHeightRatio = fNonFixedRowHeight;
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader( E_PhaseHeaderType eHeaderType )
{
	if ( !Initialize(eHeaderType) )
	{
		Initialize(EPHT_Count);
	}
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader()
{
	m_eHeaderType = EPHT_Count;
	m_fPreferWidthByChar = 0.;
	m_fFixedRowHeightRatio = 0.;
	m_fNonFixedRowHeightRatio = 0.;

	m_bNeedSort = false;
}

bool32 CIoViewPhaseSort::T_ColumnHeader::Initialize( E_PhaseHeaderType eHeaderType )
{
	InitStatic();

	for ( int32 i=0; i < s_aInitHeaders.GetSize() ; i++ )
	{
		if ( s_aInitHeaders[i].m_eHeaderType == eHeaderType )
		{
			*this = s_aInitHeaders[i];
			return true;
		}
	}

	return false;
}

bool32 CIoViewPhaseSort::T_ColumnHeader::Initialize( const CString &StrHeader )
{
	InitStatic();
	
	for ( int32 i=0; i < s_aInitHeaders.GetSize() ; i++ )
	{
		if ( s_aInitHeaders[i].m_StrHeader == StrHeader )
		{
			*this = s_aInitHeaders[i];
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool32 T_PhaseOpenBlockParam::IsCalcParamSame( const T_PhaseOpenBlockParam &pa1 ) const
{
	return m_iBlockId == pa1.m_iBlockId
		&& m_TimeStart == pa1.m_TimeStart
		&& m_TimeEnd == pa1.m_TimeEnd
		&& ((m_bDoPreWeight && pa1.m_bDoPreWeight)|| (!m_bDoPreWeight&&!pa1.m_bDoPreWeight))
		   ;
}

bool32 operator==( const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2 )
{
	return memcmp((void*)(&pa1), (void*)(&pa2), sizeof(T_PhaseOpenBlockParam)) == 0;
}

bool32 operator!=( const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2 )
{
	return !(pa1==pa2);
}

CString GetPhaseSortTypeName( E_PhaseSortType eType )
{
	switch (eType)
	{
	case EPST_RiseFallPecent:	// 涨跌
		{
			return _T("涨跌幅度");
		}
		//break;
	case EPST_ShakeRate:
		return _T("震荡幅度");
	case EPST_TradeRate:
		return _T("换手率排行");
	case EPST_VolChangeRate:
		return _T("量变幅度");
	default:
		//ASSERT( 0 );
		break;
	}

	return _T("");
}

T_PhaseOpenBlockParam::T_PhaseOpenBlockParam()
: m_TimeEnd(time(NULL))
{
	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
	m_bDoPreWeight = true;	// 虽然暂时不计算除权值
	m_ePhaseSortType = EPST_RiseFallPecent;
	m_TimeStart = m_TimeEnd - CGmtTimeSpan(30, 0, 0, 0);
	SaveDay(m_TimeStart);
}

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
#include "GridCtrlNormal.h"
#include "XLTraceFile.h"
#include "IoViewTimeSaleStatistic.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int32 KUpdatePushMerchsTimerId	= 100005;			// ÿ�� n ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs		= 1000 * 60 *2;

const int32	KSortAsyncTimerId			= 1006;
const int32	KSortAsyncTimerPeriod		= 50;
#define ID_GRID_CONTROL	10205

#define COLO_NO_NAME_CODE  RGB(240,248,136)

namespace
{
#define  INVALID_ID  -1
#define GRIDCTRL_MOVEDOWN_SIZE	    50

#define MID_BUTTON_PAIHANG_START	10001
#define MID_BUTTON_PAIHANG_PROMPT	10002
#define MID_BUTTON_PAIHANG_ONE		10003
#define MID_BUTTON_PAIHANG_THREE	10004
#define MID_BUTTON_PAIHANG_FIVE		10005
#define MID_BUTTON_PAIHANG_TEN		10006
#define MID_BUTTON_PAIHANG_END		10010
}

//////////////////////////////////////////////////////////////////////////
// ��ͷ�ֶ�
const T_TimeSaleStatisticHeader KTimeSaleStatisticHeaders[]	=	
{
	T_TimeSaleStatisticHeader(_T("����"),	ETSSHCode,		ERSPIEnd),		// 
	T_TimeSaleStatisticHeader(_T("����"),	ETSSHName,		ERSPIEnd),		// 
	T_TimeSaleStatisticHeader(_T("����\n(Ԫ)"), ETSSHPriceNew,	ERSPIEnd),		// ���¼�������Ҫ���������

	T_TimeSaleStatisticHeader(_T("����\n������\n(���)"), ETSSHVolNetSuper, ERSPIExBigNetBuyVolume),		// 

	T_TimeSaleStatisticHeader(_T("����\n������\n(���)"), ETSSHVolBuySuper, ERSPIExBigBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("����\n������\n(���)"), ETSSHVolSellSuper, ERSPIExBigSellVol),		// 

	T_TimeSaleStatisticHeader(_T("��\n������\n(���)"), ETSSHVolBuyBig, ERSPIBigBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("��\n������\n(���)"), ETSSHVolSellBig, ERSPIBigSellVol),		// 

	T_TimeSaleStatisticHeader(_T("�е�\n������\n(���)"), ETSSHVolBuyMid, ERSPIMidBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("�е�\n������\n(���)"), ETSSHVolSellMid, ERSPIMidSellVol),		// 

	T_TimeSaleStatisticHeader(_T("С��\n������\n(���)"), ETSSHVolBuySmall, ERSPISmallBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("С��\n������\n(���)"), ETSSHVolSellSmall, ERSPISmallSellVol),		// 

	T_TimeSaleStatisticHeader(_T("��С��\n������\n(���)"), ETSSHVolBuyMidSmall, ERSPIMnSBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("��С��\n������\n(���)"), ETSSHVolSellMidSmall, ERSPIMnSSellVol),		// 

	T_TimeSaleStatisticHeader(_T("����\n�����\n(��Ԫ)"), ETSSHAmountBuySuper, ERSPIExBigBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("����\n������\n(��Ԫ)"), ETSSHAmountSellSuper, ERSPIExBigSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("����\n����\n(��Ԫ)"), ETSSHNetAmountSuper, ERSPIExBigNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("��\n�����\n(��Ԫ)"), ETSSHAmountBuyBig, ERSPIBigBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("��\n������\n(��Ԫ)"), ETSSHAmountSellBig, ERSPIBigSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("��\n����\n(��Ԫ)"), ETSSHNetAmountBig, ERSPIBigNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("�е�\n�����\n(��Ԫ)"), ETSSHAmountBuyMid, ERSPIMidBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("�е�\n������\n(��Ԫ)"), ETSSHAmountSellMid, ERSPIMidSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("�е�\n����\n(��Ԫ)"), ETSSHNetAmountMid, ERSPIMidNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("С��\n�����\n(��Ԫ)"), ETSSHAmountBuySmall, ERSPISmallBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("С��\n������\n(��Ԫ)"), ETSSHAmountSellSmall, ERSPISmallSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("С��\n����\n(��Ԫ)"), ETSSHNetAmountSmall, ERSPISmallNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("����\n�ܽ��\n(��Ԫ)"), ETSSHAmountTotalBuy, ERSPITotalBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("����\n�ܽ��\n(��Ԫ)"), ETSSHAmountTotalSell, ERSPITotalSellAmount),		// 

	T_TimeSaleStatisticHeader(_T("����\n������\n(%)"), ETSSHTradeRateSuper, ERSPIExBigChange),		// 
	T_TimeSaleStatisticHeader(_T("��\n������\n(%)"), ETSSHTradeRateBig, ERSPIBigChange),		// 
	T_TimeSaleStatisticHeader(_T("�����\n������\n(%)"), ETSSHTradeRateBuyBig, ERSPIBigBuyChange),		// 

	T_TimeSaleStatisticHeader(_T("�ɽ�����"), ETSSHTradeCount, ERSPITradeCounts),		// 
	T_TimeSaleStatisticHeader(_T("ÿ�ʽ��\n(��Ԫ)"), ETSSHAmountPerTrans, ERSPIAmountPerTrade),		// 
	T_TimeSaleStatisticHeader(_T("ÿ�ʹ���\n(��)"), ETSSHStocksPerTrans, ERSPIStockPerTrade),		// 

	T_TimeSaleStatisticHeader(_T("�ʽ�����\n(��Ԫ)"), ETSSHCapitalFlow, ERSPIEnd),		// �ʽ�����������������
};

const int32 KTimeSaleStatisticHeadersCount = sizeof(KTimeSaleStatisticHeaders) / sizeof(KTimeSaleStatisticHeaders[0]);

//////////////////////////////////////////////////////////////////////////
CIoViewTimeSaleStatistic *CIoViewTimeSaleStatistic::m_pThis = NULL;
	
IMPLEMENT_DYNCREATE(CIoViewTimeSaleStatistic, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTimeSaleStatistic, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewTimeSaleStatistic)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_WM_HSCROLL()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
ON_MESSAGE(UM_DOTRACEMENU, OnDoTrackMenu)
ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChanged)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CONTROL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CONTROL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRID_CONTROL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRID_CONTROL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRID_CONTROL, OnGridKeyUpEnd)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRID_CONTROL, OnGridGetDispInfo)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// ׼���Ż� fangz20100514

CIoViewTimeSaleStatistic::CIoViewTimeSaleStatistic()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);

	m_MmiRequestSys.m_iMarketId = -1;
	m_MmiRequestSys.m_iStart = 0;
	m_MmiRequestSys.m_iCount = 0;
	m_MmiRequestSys.m_eReportSortType = ERSPIEnd;
	m_MmiRequestSys.m_bDescSort = TRUE;
	
	m_aUserBlockNames.RemoveAll();

	m_iMaxGridVisibleRow = 0;

	m_bInitialized = false;

	m_iPeriod = 1;
	m_pCurrentXSBHorz = NULL;
	m_pImgBtn = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSaleStatistic::~CIoViewTimeSaleStatistic()
{
	// MainFrame �п�ݼ�
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
	DEL(m_pImgBtn);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewTimeSaleStatistic::OnPaint()
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

	RedrawCycleBtn(&dcPaint);
}

void CIoViewTimeSaleStatistic::OnMouseWheel(short zDelta)
{
	//m_GridCtrl.SendMessage(WM_MOUSEWHEEL, MAKELONG(0, zDelta*120), 0);
}

BOOL CIoViewTimeSaleStatistic::PreTranslateMessage(MSG* pMsg)
{
	//if ( WM_MOUSEWHEEL == pMsg->message )
	//{
	//	// ���ع���
	//	short zDelta = HIWORD(pMsg->wParam);
	//	zDelta /= 120;

	//	// �Լ�����
	//	//OnMouseWheel(zDelta);
	//	//return TRUE;
	//}

	return CControlBase::PreTranslateMessage(pMsg);		// Խ��IoViewBase�ļ��̴���
}

int CIoViewTimeSaleStatistic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ASSERT( NULL != m_pAbsCenterManager );
	if ( NULL == m_pAbsCenterManager )	// �Լ�ע��
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//����Tab ��
// 	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
// 	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
// 	// m_GuiTabWnd.SetBkGround(true,GetIoViewColor(ESCBackground),IDB_GUITAB_NORMAL,IDB_GUITAB_SELECTED);
// 	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
// 	m_GuiTabWnd.SetUserCB(this);
// 	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);

	// �����������
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CONTROL);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetDoubleBuffering(TRUE);
	//m_GridCtrl.SetVirtualCompare(*this);
	m_GridCtrl.EnableBlink(FALSE);			// ��ֹ��˸
	m_GridCtrl.SetTextColor(COLO_NO_NAME_CODE);
	m_GridCtrl.SetFixedTextColor(COLO_NO_NAME_CODE);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_LEFT | DT_WORDBREAK| DT_NOPREFIX |DT_VCENTER);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_WORDBREAK| DT_NOPREFIX |DT_VCENTER);

	// �����໥֮��Ĺ���
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ
	m_XSBVert.AddMsgListener(m_hWnd);	// ����������Ϣ - ���Ŀ�����Ʒ

	SetXSBHorz(&m_XSBHorz);

	// ���ñ�ͷ
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);

	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetListMode(TRUE);
	m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// �� ��
	m_GridCtrl.SetFixedRowCount(1);

	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// ��ͷ�̶������Գ�ʼ����ͷ��
	UpdateTableHeader();

	//pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);

	PostMessage(UM_DOINITIALIZE, 0, 0);

	// ���ʱ����Գ�ʼ��tab��Դ�˵ģ�����ڴ�ʱӦ�����غ��ˣ����û����Ӧ����do initialize�г�ʼ������tab
	//InitializeTabs();	

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	CreateBtnList();

	if(m_mapBtnCycleList.size() > 0)
	{
		int iBtnID = MID_BUTTON_PAIHANG_ONE;
		switch(m_iPeriod)
		{
		case 1:
			iBtnID = MID_BUTTON_PAIHANG_ONE;
			break;

		case 3:
			iBtnID = MID_BUTTON_PAIHANG_THREE;
			break;

		case 5:
			iBtnID = MID_BUTTON_PAIHANG_FIVE;
			break;

		case 10:
			iBtnID = MID_BUTTON_PAIHANG_TEN;
			break;

		default:
			break;
		}

		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iBtnID)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}


	return 0;
}

void CIoViewTimeSaleStatistic::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

BOOL CIoViewTimeSaleStatistic::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowVisible())
	{
		if ( 13 == nChar && 1 == nRepCnt)
		{
			if( 28 == nFlags || 284 == nFlags)				
			{
				// �س���
				CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
				if ( rangeSel.IsValid() 
					&& rangeSel.GetMinRow() >= m_GridCtrl.GetFixedRowCount()
					&& rangeSel.GetMinRow() < m_GridCtrl.GetRowCount() )
				{
					CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0);
					OnDblClick(pMerch);
					return TRUE;
				}
			}			
		}

		return FALSE;
	}

	return FALSE;
}

void CIoViewTimeSaleStatistic::OnIoViewActive()
{
	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	m_bActive = IsActiveInFrame();

	Invalidate(TRUE);
}

void CIoViewTimeSaleStatistic::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewTimeSaleStatistic::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos;
	GetCursorPos(&pos);
	PostMessage(UM_DOTRACEMENU, MAKEWPARAM(pos.x, pos.y), 0);
}

void CIoViewTimeSaleStatistic::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL == pGridView || pGridView->iRow < m_GridCtrl.GetFixedRowCount() )
	{
		return;
	}
	CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(pGridView->iRow, 0);
	OnDblClick(pMerch);
}

void CIoViewTimeSaleStatistic::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewTimeSaleStatistic::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���¼�����ͷ��
}

void CIoViewTimeSaleStatistic::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���ϼ�����ͷ��
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewTimeSaleStatistic::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewTimeSaleStatistic::OnVDataForceUpdate()
{
	if ( m_tabInfo.IsSort() )
	{
		RequestSortData(true);		// ǿ��������������
	}
	RequestViewDataCurrentVisibleRow();
}

void CIoViewTimeSaleStatistic::OnFixedRowClickCB(CCellID& cell)
{
    if (!m_GridCtrl.IsValid(cell))
	{
		return;
	}
	// ��Ų����򣬽��������ݣ��������������������0��ʼ��ֵ
	// ���ñ���������	
	if ( cell.col ==0 || cell.col == 1 )	// ���� ����
	{
		if ( m_tabInfo.IsSort() )	// �����ǰ������ ��������ձ�� (��ģʽ�Ƿ���Բ��ã�)
		{
			//m_GridCtrl.DeleteNonFixedRows(); // ɾ�����ݣ�������λ����0
			m_GridCtrl.SetRowCount(m_tabInfo.m_aMerchs.GetSize() + m_GridCtrl.GetFixedRowCount());
			m_tabInfo.ClearSort();		// �µ������Ѿ��滻
			m_MmiRequestSys.m_eReportSortType = ERSPIEnd;
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		}

		m_pThis = this;
		m_GridCtrl.SetVirtualCompare(CompareRow);
		m_GridCtrl.SetHeaderSort(TRUE);
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		
		m_GridCtrl.Refresh();

		RequestViewDataCurrentVisibleRow();
	}
	else
	{
		// Ӧ���ڴ˷������󣬵����������������
		m_GridCtrl.SetHeaderSort(FALSE);	// �����н��ñ������

		//m_GridCtrl.SetSortAscending(m_tabInfo.ChangeSortType(cell.col));
		//m_GridCtrl.SetSortColumn(cell.col);
		
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(cell.col);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(cell.col);
			if ( eHeaderShow == ETSSHPriceNew )		// ���¼�����? �������¼ۻ���a�������
			{
				// 
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
				if ( NULL == pBlock )
				{
					return; // û�л���a������
				}
				CBlockLikeMarket *pBlockNow = GetCurrentBlock();
				if ( NULL == pBlockNow )
				{
					return;
				}
				BOOL bChangedCol = FALSE;
				m_MmiRequestBlockSys.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
				if ( pBlockNow == pBlock )
				{
					// ����A��û�ı�Ļ�һ��������
				}
				else if ( pBlockNow->m_blockInfo.m_mapSubMerchMarkets.GetCount() == 1 )
				{
					// ����A�����ĳ���г�,���ڵ�����Ǳ�Ȼ�������г����˴��Ժ�����Ҫ���ģ�ͨ���г�������
				}
				m_MmiRequestBlockSys.m_iStart = 0;					
				// ��ʵ�ϣ�����������Ǳ������л���a����Ʒ���У����Ƿ�������Ʒ�����п��ܴ��ڱ�����Ʒ���ϣ��������������Ǹ��鷳
				//m_MmiRequestBlockSys.m_iCount = 5000;	
				m_MmiRequestBlockSys.m_iCount = pBlock->m_blockInfo.m_aSubMerchs.GetSize();	// ������Ʒ����ʵ�����п������� - �����Ʒ����
				m_MmiRequestBlockSys.m_eMerchReportField = EMRFPriceNew;
				m_MmiRequestBlockSys.m_bDescSort = !m_tabInfo.ChangeSortType(cell.col, &bChangedCol);
				if ( bChangedCol )
				{
					m_GridCtrl.ResetVirtualOrder(); // ������ģʽ�µ������� - ��Ȼ�Ѿ�����������ڵȴ����ص�ʱ�򣬻��ǻ�����û����������ѽڵ�����
					m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
					m_GridCtrl.Refresh();
				}
				RequestSortData();
			}
			return;		// �������� �������κδ���
		}

		// ��������
		BOOL bChangedCol = FALSE;
		m_MmiRequestSys.m_iMarketId = m_tabInfo.m_iBlockId;
		m_MmiRequestSys.m_eReportSortType = eHeaderSort;
		m_MmiRequestSys.m_iStart = m_tabInfo.m_iStartRow;
		m_MmiRequestSys.m_iCount = m_iMaxGridVisibleRow;
		m_MmiRequestSys.m_bDescSort = !m_tabInfo.ChangeSortType(cell.col, &bChangedCol);

		if ( bChangedCol )	// �ı�������Ӧ�����ÿ�������
		{
			m_GridCtrl.ResetVirtualOrder(); // ������ģʽ�µ������� - ��Ȼ�Ѿ�����������ڵȴ����ص�ʱ�򣬻��ǻ�����û����������ѽڵ�����
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.Refresh();
		}

		RequestSortData();
	}
}

void CIoViewTimeSaleStatistic::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewTimeSaleStatistic::OnHScrollEnd()
{
	
}

void CIoViewTimeSaleStatistic::OnVScrollEnd()
{
	
}

void CIoViewTimeSaleStatistic::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewTimeSaleStatistic::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewTimeSaleStatistic::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewTimeSaleStatistic::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{	
		if ( m_tabInfo.IsSort() )
		{
			RequestSortData();
		}
		else
		{
			RequestViewDataCurrentVisibleRow();	
		}
	}
	else if ( KSortAsyncTimerId == nIDEvent )
	{
		UpdateVisibleMerchs();
		KillTimer(nIDEvent);
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewTimeSaleStatistic::RequestViewDataCurrentVisibleRow()
{
	CMmiReqPeriodTickEx			reqHis;
	CMmiReqPushPlugInMerchData	req;		// ֱ��������������

	{
		for ( MerchMap::iterator it = m_mapLastPushMerchs.begin() ; it != m_mapLastPushMerchs.end() ; it++ )
		{
			it->second--;		// ����0 or -- ��
		}
	}
	
	
	// ѡ������Ʒ
	CCellRange cellRange = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( cellRange.IsValid() )
	{
		int32 iMaxRow = cellRange.GetMaxRow();
		//iMaxRow += max(15, cellRange.GetRowSpan());		// ������һ�㣬�����������ݼ���
		iMaxRow += 5;
		if ( iMaxRow > m_tabInfo.m_aMerchs.GetSize() )
		{
			iMaxRow = m_tabInfo.m_aMerchs.GetSize();
		}
		
		m_aSmartAttendMerchs.RemoveAll();		// ���ʵʱ����
		CMmiReqRealtimePrice reqRealPrice;
		
		for ( int i=cellRange.GetMinRow(); i <= iMaxRow ; i++ )
		{
			CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(i, 0);
			ASSERT( NULL ==pMerch || pMerch == CheckMerchInBlock(pMerch) );	// �п��ܱ��û�ж�Ӧ��Ʒ����Ϊ�������Ʒ��û����
			if ( pMerch == NULL )
			{
				continue;
			}
			
			MerchMap::iterator itMerch = m_mapLastPushMerchs.find(pMerch);
			if ( itMerch != m_mapLastPushMerchs.end() )
			{
				itMerch->second++;
			}
			else
			{
				m_mapLastPushMerchs[pMerch] = 1;
			}
			
			bool32 bReqHis = m_mapMerchData.count(pMerch) <= 0;	// ֻ����û�и���Ʒ����ʷ�����²�������ʷ
			
			if ( req.m_StrMerchCode.IsEmpty() )
			{
				req.m_iMarketID = pMerch->m_MerchInfo.m_iMarketId;	// ����A - X
				req.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				req.m_uType = ECSTTickEx;
				
				reqHis.m_iMarketId = req.m_iMarketID;				// ��һ���ͼ���ȥ��
				reqHis.m_StrMerchCode = req.m_StrMerchCode;
				reqHis.m_uiPeriod = m_iPeriod;
			}
			else
			{
				T_ReqPushMerchData	rpmd;
				rpmd.m_iMarket = pMerch->m_MerchInfo.m_iMarketId;
				rpmd.m_StrCode = pMerch->m_MerchInfo.m_StrMerchCode;
				rpmd.m_uType = ECSTTickEx;
				req.m_aReqMore.Add( rpmd );
	
				//if ( bReqHis )			// ����Ҫ���͵���ʷ��������
				{
					CPeriodMerchKey key;
					key.m_iMarketId = rpmd.m_iMarket;
					key.m_StrMerchCode = rpmd.m_StrCode;
					key.m_uiPeriod = m_iPeriod;
					reqHis.m_aMerchMore.Add(key);
				}
			}
			
			// ʵʱ���۹�ע
			CSmartAttendMerch smartMerch;
			smartMerch.m_iDataServiceTypes = EDSTPrice;
			smartMerch.m_pMerch = pMerch;
			m_aSmartAttendMerchs.Add(smartMerch);
			
			if ( reqRealPrice.m_StrMerchCode.IsEmpty() )
			{
				reqRealPrice.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
				reqRealPrice.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;						
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				
				reqRealPrice.m_aMerchMore.Add(MerchKey);					
			}
		}
		
		if ( !req.m_StrMerchCode.IsEmpty() )
		{
			//	CString StrTrace;
			//	StrTrace.Format(_T("Tickex his: %d"), 1 + reqHis.m_aMerchMore.GetSize());
			//	XLTraceFile::GetXLTraceFile(_T("tickex.log")).TraceWithTimestamp(StrTrace, TRUE);
			if(m_iPeriod == 1)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushTickEx;
			}
			else if(m_iPeriod == 3)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx3;
			}
			else if(m_iPeriod == 5)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx5;
			}
			else if(m_iPeriod == 10)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx10;
			}
			RequestData(reqHis);
			RequestData(req);
			RequestData(reqRealPrice);
		}
	}

	// ɾ�����õ�����???
	if ( 0 )
	{
		CMmiReqRemovePushPlugInMerchData reqDel(ECTPIReqDeletePushMerchData);
		reqDel.m_eType = ECSTTickEx;
		MerchMap mapMerch = m_mapLastPushMerchs;
		for ( MerchMap::iterator it = mapMerch.begin() ; it != mapMerch.end() ; it++ )
		{
			if ( it->second < 1 )	// Ӧ��ɾ��
			{
				CMerchKey key;
				key.m_iMarketId		= it->first->m_MerchInfo.m_iMarketId;
				key.m_StrMerchCode	= it->first->m_MerchInfo.m_StrMerchCode;
				reqDel.m_aMerchs.Add(key);
				m_mapLastPushMerchs.erase(it->first);
			}
		}
		if ( reqDel.m_aMerchs.GetSize() > 0 )
		{
			//TRACE(_T("Delete Tickex: %d\r\n"), reqDel.m_aMerchs.GetSize());
			RequestData(reqDel);		// ɾ�����ͻ�ɾ�����й�ע�����Ʒ�����͵����ͣ�����������ͼ��Ҫ~~
		}
	}
}

void CIoViewTimeSaleStatistic::RequestSortData(bool32 bForce/* = false*/)
{
	if ( m_tabInfo.IsSort() )
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(m_tabInfo.m_iSortColumn);
			if ( eHeaderShow == ETSSHPriceNew )		// ���¼�����? �������¼ۻ���a�������
			{
				RequestData(m_MmiRequestBlockSys, bForce);
			}
		}
		else
		{
			m_MmiRequestSys.m_iMarketId = -1;	// ǿ�� ����a
			// 		int32 iBlockDefId = 0;
			// 		if ( CBlockConfig::Instance()->GetDefaultMarketClassBlockId(iBlockDefId) && m_tabInfo.m_iBlockId == iBlockDefId )
			// 		{
			// 	  		m_MmiRequestSys.m_iStart = m_tabInfo.m_iStartRow;
			//   		m_MmiRequestSys.m_iCount = m_iMaxGridVisibleRow;
			// 		}
			// 		else
			{
				m_MmiRequestSys.m_iStart = 0;
				m_MmiRequestSys.m_iCount = -1;
			}
			m_MmiRequestSys.m_uiPeriod = m_iPeriod;
		}
		
		RequestData(m_MmiRequestSys, bForce);
	}
}

void CIoViewTimeSaleStatistic::SetChildFrameTitle()
{
	
}

void CIoViewTimeSaleStatistic::OnDblClick(CMerch *pMerch)
{
	if ( pMerch != NULL )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewTimeSaleStatistic::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	
	SetColWidthAccordingFont();
	RecalcLayout();
}

void CIoViewTimeSaleStatistic::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewTimeSaleStatistic::OnLButtonDown2( CPoint pt, int32 iTab )
{
	return false;
}

bool32 CIoViewTimeSaleStatistic::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return false;
}

void CIoViewTimeSaleStatistic::SetRowHeightAccordingFont()
{
	m_GridCtrl.AutoSizeRows();
}

void CIoViewTimeSaleStatistic::SetColWidthAccordingFont()
{
	// ����ֵ�д����ΪʲôҪ�̶����������Զ��ų���������ʲôҵ���߼���
	// ��Ĭ�����ֵ��70�޸�Ϊ95���أ������ʾ��ȫ
	const int32 iConstColWidth = 95;
	CClientDC dc(this);
	// ʹ�ù̶�cell��font
	CFont *pFontNormal =  GetIoViewFontObject(ESFNormal);//m_GridCtrl.GetDefaultCell(TRUE, TRUE)->GetFontObject();// 
	CFont *pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeText = dc.GetTextExtent(_T("���Ը߿�"));
	dc.SelectObject(pOldFont);
	int32 iColWidth = (int32)(sizeText.cx * 1.15f);
	iColWidth = max(iColWidth, iConstColWidth);
	//m_GridCtrl.AutoSizeColumns();
	//m_GridCtrl.ExpandColumnsToFit();
	for ( int i=0; i < m_GridCtrl.GetColumnCount() ; i++ )	// ������Сcol
	{
		if ( m_GridCtrl.GetColumnWidth(i) > 0 /*&& m_GridCtrl.GetColumnWidth(i) < iColWidth*/ )
		{
			if ( i == 11 || i == 12 || i == 13 )
			{
				m_GridCtrl.SetColumnWidth(i, iColWidth + 20);
			}
			else
			{
				m_GridCtrl.SetColumnWidth(i, iColWidth);
			}
		}
	}

	//CRect rcGrid;
	//m_GridCtrl.GetClientRect(rcGrid);
	//int iLeftWidth = rcGrid.Width() - m_GridCtrl.GetVirtualWidth();
	//int iCol = 0;
	//if ( iLeftWidth > 0 && (iCol=m_GridCtrl.GetColumnCount()-1) >= 0 )
	//{
	//	m_GridCtrl.SetColumnWidth( iCol, m_GridCtrl.GetColumnWidth(iCol) + iLeftWidth );
	//}
}

void CIoViewTimeSaleStatistic::UpdateTableHeader()
{
	HeaderArray	aHeaders;
	GetColumnHeaders(aHeaders);
		
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.SetColumnCount(aHeaders.GetSize());	// �����, ����

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();	// ���ڹ̶��иߵ����⣬����ֻ���ڽ��б�ͷʱ���ã�����n��
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::UpdateTableRowContent( CMerch *pMerch, bool32 bBlink )
{
	// TODO
	ASSERT( 0 );	// û��ʵ�� - ������ݸ��º�Ƶ��������ʵ��

	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::UpdateTableAllContent()
{
	//m_GridCtrl.DeleteNonFixedRows();	

	// ��ʱ�Ѿ�û����������Ϣ
	ASSERT( m_tabInfo.m_eSort == T_Tab::SortNone );

	m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_tabInfo.m_aMerchs.GetSize());

// 	m_GridCtrl.InsertRowBatchBegin();
// 	int32 i=0; 
// 	CString StrNo;
// 	for ( i=0; i < m_aMerchs.GetSize() ; i++ )
// 	{
// 		if ( m_aMerchs[i] == NULL )
// 		{
// 			continue;;
// 		}
// 		CMerch *pMerch = m_aMerchs[i];
// 		int32 iRow = m_GridCtrl.InsertRowBatchForOne(pMerch->m_MerchInfo.m_StrMerchCode);
// 		if ( iRow >= 0 )
// 		{
// 			m_GridCtrl.SetItemData(iRow, 0, (LPARAM)pMerch);
// 			m_GridCtrl.SetItemText(iRow, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
// 
// 			UpdateTableMerchSimple(pMerch, false);
// 		}
// 	}
// 	m_GridCtrl.InsertRowBatchEnd();

	//SetRowHeightAccordingFont();
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::OnDoInitialize()
{
	m_bInitialized = true;		// ������һ����־�����ڻ�����ʵ�ʳ�ʼ��

	int32 iBlockId = m_tabInfo.m_iBlockId;

	// 
	CBlockLikeMarket	*pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL != pBlock )
	{
		iBlockId = pBlock->m_blockInfo.m_iBlockId;
	}

	OpenBlock(iBlockId);		// ��װ��һ�ΰ������
}

void CIoViewTimeSaleStatistic::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);

	// ����������� - ���Լ���  - ��Ҫ�����㹻������
	int32 iOldRowCount = m_iMaxGridVisibleRow;
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("�и߶Ȳ���"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// �����˲�֪�����ٸ�
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}
	m_tabInfo.m_iRowCount = m_iMaxGridVisibleRow;


	CRect rcOld(0,0,0,0);
	m_GridCtrl.GetClientRect(rcOld);
	rcClient.top += GRIDCTRL_MOVEDOWN_SIZE;
	m_GridCtrl.MoveWindow(rcClient);

	//if ( rcOld.Width() < rcClient.Width() )
	//{
	//	SetColWidthAccordingFont();
	//	m_GridCtrl.ExpandColumnsToFit(TRUE);
	//}

	if ( iOldRowCount < m_iMaxGridVisibleRow )
	{
		AppendShowMerchs(m_iMaxGridVisibleRow - iOldRowCount);
	}

	ShowOrHideXHorzBar();
}

void CIoViewTimeSaleStatistic::OpenBlock( int32 iBlockId )
{
	int32 iBlockOldId = m_tabInfo.m_iBlockId;
	CBlockLikeMarket *pBlock = GetBlock(iBlockId);
	if ( pBlock == NULL )
	{
		return;	// do nothing
	}

	m_tabInfo.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
	m_tabInfo.m_StrName = pBlock->m_blockInfo.m_StrBlockName;
	// ��䵱ǰ��Ʒ����
	m_tabInfo.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
	m_tabInfo.ResetShowData();
	m_tabInfo.m_iStartRow = 0;
	m_tabInfo.m_iRowCount = m_iMaxGridVisibleRow;

	m_GridCtrl.SetSortColumn(-1);
	m_GridCtrl.DeleteNonFixedRows();	

	UpdateTableAllContent();

	// Ĭ�������һ���������ֶ�
	E_ReportSortEx eSort = ERSPIEnd;
	for ( int32 i=0; i < m_GridCtrl.GetColumnCount() ; i++ )
	{
		eSort = GetColumnSortHeader(i);
		if ( eSort != ERSPIEnd )
		{
			// ���������������ʾ����
			//for ( int j=0; j < m_tabInfo.m_aMerchsVisible.GetSize() ; j++ )
			//{
			//	m_tabInfo.m_aMerchsVisible[j] = NULL;  // Ҫ�ǳ�ʱ�䲻��������̫�ѿ���
			//}

			CCellID idCell;
			idCell.row = 0;
			idCell.col = i;
			OnFixedRowClickCB(idCell);

			if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount()
				&& iBlockId != iBlockOldId )
			{
				// �л�����л�����һ��λ��
				m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
				m_GridCtrl.SetSelectedRange(m_GridCtrl.GetFixedRowCount(), 0, m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetColumnCount()-1);
				m_GridCtrl.SetFocusCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			}
			return; // ������Ͳ���������ʾ
			//break;
		}
	}

	if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount()
		&& iBlockId != iBlockOldId )
	{
		// �л�����л�����һ��λ��
		m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		m_GridCtrl.SetSelectedRange(m_GridCtrl.GetFixedRowCount(), 0, m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetColumnCount()-1);
		m_GridCtrl.SetFocusCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
	}
	
	RequestViewDataCurrentVisibleRow();		// �������� - �Ƿ�������ز����������
}

void CIoViewTimeSaleStatistic::AppendShowMerchs(int32 iMerchAppCount)		// ֻ������Ч���ݺ���ӣ�����Ч��������²�������κ�
{
	if ( iMerchAppCount <= 0 || GetCurrentBlock() == NULL )
	{
		return;
	}

	// ������������Ϳ�����
// 	if ( m_tabInfo.IsSort() )
// 	{
// 		RequestSortData();		// ������Ҫ������ - ����ȫ�������Ѿ�����
// 	}
// 	else
	{
		RequestViewDataCurrentVisibleRow();	// ������ֻҪ��ʾ���ݾͿ�����
	}

// 
// 	// ��ȡ���ʵ���Ҫ��ʾ����Ʒ���п�����Ҫ�����������������Ȼ������ʾ
// 	// ����Ҫ������ѡȡ��Ʒ�����е���Ʒ����ʾ
// 	
// 	if ( m_tabInfo.m_eSort != T_Tab::SortNone )	// ������
// 	{
// 		// ƫ��һ����������Ԥ�ƿɼ��е����� ���ı����ڵ�ƫ�� ���ݻ������ٴ���, �ڵȴ������п��ܷ����κθı䵱ǰ״̬�����飬��Ҫ����
// 	}
// 	else	// �����г�����Ʒ��������ʾ
// 	{
// 		CMerch *pMerchLast = NULL;
// 		CCellRange cellRange = m_GridCtrl.GetCellRange();
// 		CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(m_GridCtrl.GetRowCount()-1, 0);
// 		
// 		CMarket *pMarket = GetCurrentMarket();
// 		int32 i=0;
// 		for ( i=0; i < pMarket->m_MerchsPtr.GetSize() ; i++ )
// 		{
// 			if ( pMarket->m_MerchsPtr[i] == pMerch )
// 			{
// 				break;
// 			}
// 		}
// 		int32 iMerchApped = 0;
// 		i++;	// 
// // 		for (  ; i < pMarket->m_MerchsPtr.GetSize() && iMerchApped < iMerchAppCount ; i++ )
// // 		{
// // 			CMerch *pMerchMarket = pMarket->m_MerchsPtr[i];
// // 			if ( NULL == pMerchMarket )
// // 			{
// // 				continue;
// // 			}
// // 			int32 iRow = m_GridCtrl.InsertRow(pMerchMarket->m_MerchInfo.m_StrMerchCode);
// // 			if ( iRow >= 0 )
// // 			{
// // 				m_GridCtrl.SetItemData(iRow, 0, (LPARAM)pMerchMarket);
// // 				m_GridCtrl.SetItemText(iRow, 1, pMerchMarket->m_MerchInfo.m_StrMerchCnName);
// // 				
// // 				UpdateTableMerchSimple(pMerchMarket, false);
// // 			}
// // 			iMerchApped++;
// // 		}
// 		iMerchApped = m_aMerchs.GetSize() - i;
// 		if ( iMerchApped > 0 )
// 		{
// 			//m_GridCtrl.SetRowCount(m_GridCtrl.GetRowCount() + iMerchApped);
// 		}
// 		if ( iMerchApped > 0 )
// 		{
// 			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);	// ͷ������
// 		}
// 	}
	
}

void CIoViewTimeSaleStatistic::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	HeaderArray	aHeaders;
	GetColumnHeaders(aHeaders);		// �������ϸ���Ҫ

	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// ����v mode
		// ������ܴ���ΪȦ���õ�����Ʒ��������Ʒ���飬�ж��پ�ֻ��grid��ʾ���٣���ô������֣��������ع��֣����������ݣ����ݻ�����������
		ASSERT( pDisp->item.col >= 0 && pDisp->item.col < aHeaders.GetSize() );
		
		int iRow = pDisp->item.row;
		int iCol = pDisp->item.col;

		pDisp->item.lParam = NULL;		// ��ʼ��

		CGridCellNormalSys *pCell = DYNAMIC_DOWNCAST(CGridCellNormalSys, m_GridCtrl.GetDefaultVirtualCell());
		if ( NULL != pCell )
		{
			pCell->SetShowSymbol(CGridCellSymbol::ESSNone);
			pCell->SetParentActiveFlag(false);
			pCell->SetDrawParentActiveParams(-1, -1, m_ColorActive);
			if ( pDisp->item.row == 0 && 0 == pDisp->item.col )
			{
				pCell->SetParentActiveFlag(m_bActive);
				pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);
			}
		}
		else
		{
			ASSERT( 0 );
		}

		// ͷ��
		if ( pDisp->item.row == 0 )
		{
			// ��ɫ
			// ĳЩ�ֶ�ֻ��ʾһ��
			pDisp->item.nFormat |= DT_VCENTER;
			pDisp->item.crFgClr = GetIoViewColor(ESCVolume);
			pDisp->item.lParam = aHeaders[pDisp->item.col].m_eHeaderShow;		// ����ͷ����Ϣ
			pDisp->item.strText = aHeaders[pDisp->item.col].m_StrName;
			if ( pDisp->item.col >= 2 )
			{
				pDisp->item.nFormat |= DT_RIGHT;	// �����ֶ��Ҷ���
			}
		}
		else
		{
			const MerchArray &aMerchShow = m_tabInfo.m_aMerchsVisible;
			//ASSERT( pDisp->item.row > 0 && pDisp->item.row <= aMerchShow.GetSize() );
			if ( pDisp->item.row < 0 || pDisp->item.row > aMerchShow.GetSize() )
			{
				return;		// �ڰ���л�ʱ������getitem������
			}

			int iIndex = iRow - 1;

			// ���ݲ���
			// ��Ҫ����DATA��TEXT�������������ͼ��������Ĭ�ϵģ� �����δ֪��Ʒ���º��Ѵ�������
			CMerch *pMerchShow = aMerchShow[iIndex];
			pDisp->item.lParam = (LPARAM)pMerchShow;	// ����ΪNULL
			E_TimeSaleStatisticHeader eHeaderShow = aHeaders[pDisp->item.col].m_eHeaderShow;
			
			// ��ʹ��Ĭ������
			DataMap::const_iterator it = m_mapMerchData.find(pMerchShow);
			if ( NULL != pMerchShow )
			{
				T_TimeSaleStatisticData dataShow;
				CRealtimePrice			realPrice;
				if ( it != m_mapMerchData.end() )
				{
					dataShow = it->second;
				} // û��������ô�� TODO
				if ( NULL != pMerchShow->m_pRealtimePrice )
				{
					realPrice = *pMerchShow->m_pRealtimePrice;
				}

				pDisp->item.strText = _T(" -");
				//pDisp->item.crFgClr = GetIoViewColor(ESCText);
				//TRACE(_T("%d-%d text clr: %08X\r\n"), pDisp->item.row, pDisp->item.col, pDisp->item.crFgClr);
				

				if ( eHeaderShow >= ETSSHVolBuySuper && eHeaderShow <= ETSSHVolSellMidSmall )
				{
					pDisp->item.crFgClr = GetIoViewColor(ESCVolume2);
					//pDisp->item.crFgClr = RGB(192,192,0);
				}
				else if ( eHeaderShow == ETSSHAmountPerTrans || (eHeaderShow >= ETSSHAmountBuySuper && eHeaderShow <= ETSSHAmountTotalSell) )
				{
					pDisp->item.crFgClr = GetIoViewColor(ESCAmount);
				}

				const float cf1W = 10000.0;

				switch (eHeaderShow)
				{
				case ETSSHCode:
					pDisp->item.strText = pMerchShow->m_MerchInfo.m_StrMerchCode;
					break;
				case ETSSHName:
					{
						T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerchShow);
						if ( NULL != pUserBlock && pUserBlock->m_clrBlock != COLORNOTCOUSTOM )
						{
							pDisp->item.crFgClr = pUserBlock->m_clrBlock;
						}
						pDisp->item.strText = pMerchShow->m_MerchInfo.m_StrMerchCnName;

						// �Ƿ��б��
						GV_DRAWMARK_ITEM markItem;
						if ( InitDrawMarkItem(pMerchShow, markItem) )
						{
							pDisp->item.markItem = markItem;
						}
					}
					break;
				case ETSSHPriceNew: // ���¼�ȡRealTimePrice�е�
					{
						if ( realPrice.m_fPriceNew != 0.0 )
						{
							if ( realPrice.m_fPriceNew > realPrice.m_fPricePrevClose )
							{
								pDisp->item.crFgClr = GetIoViewColor(ESCRise);
							}
							else if ( realPrice.m_fPriceNew < realPrice.m_fPricePrevClose )
							{
								pDisp->item.crFgClr = GetIoViewColor(ESCFall);
							}
						}
						pDisp->item.strText = Float2String(realPrice.m_fPriceNew, 2);
					}
					break;
				case ETSSHNetAmountSuper:
					{
						float fNet = (dataShow.m_tickEx.m_fExBigBuyAmount-dataShow.m_tickEx.m_fExBigSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolNetSuper:
					{
						float fNet = (dataShow.m_tickEx.m_fExBigBuyVol-dataShow.m_tickEx.m_fExBigSellVol) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolBuySuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigSellVol / cf1W,  2);
					break;
				case ETSSHNetAmountBig:
					{
						float fNet = (dataShow.m_tickEx.m_fBigBuyAmount-dataShow.m_tickEx.m_fBigSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;

				case ETSSHVolBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigSellVol / cf1W,  2);
					break;
				case ETSSHVolBuyMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidSellVol / cf1W,  2);
					break;
				case ETSSHVolBuySmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallSellVol / cf1W,  2);
					break;
				case ETSSHNetAmountMid:
					{
						float fNet = (dataShow.m_tickEx.m_fMidBuyAmount-dataShow.m_tickEx.m_fMidSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHNetAmountSmall:
					{
						float fNet = (dataShow.m_tickEx.m_fSmallBuyAmount-dataShow.m_tickEx.m_fSmallSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolBuyMidSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMnSBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellMidSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMnSSellVol / cf1W,  2);
					break;
				case ETSSHAmountBuySuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuyMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuySmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallSellAmount / cf1W,  2);
					break;
				case ETSSHAmountTotalBuy:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAllBuyAmount / cf1W,  2);
					// ��ɫΪ��ɫ
					pDisp->item.crFgClr = GetIoViewColor(ESCRise);
					break;
				case ETSSHAmountTotalSell:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAllSellAmount / cf1W,  2);
					// ��ɫΪ��ɫ
					pDisp->item.crFgClr = GetIoViewColor(ESCFall);
					break;
				case ETSSHAmountPerTrans:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAmountPerTrans / cf1W,  2);
					break;
				case ETSSHStocksPerTrans:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fStocksPerTrans,  2);
					break;
				
				case ETSSHTradeRateSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigChange * 100.0, 2);
					break;
				case ETSSHTradeRateBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigChange * 100.0, 2);
					break;
				case ETSSHTradeRateBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyChange * 100.0, 2);
					break;
				case ETSSHTradeCount:
					pDisp->item.strText.Format(_T(" %d"), dataShow.m_tickEx.m_uiTradeTimes);
					break;
				case ETSSHCapitalFlow:
					{
						float fFlow = dataShow.m_tickEx.m_fAllBuyAmount - dataShow.m_tickEx.m_fAllSellAmount;
						pDisp->item.strText = Float2SymbolString(fFlow, 0.0, 2, false, true);
					}
					break;

				default:
					// n���ֶβ��ܴ���
					;
				}

				// symbol����-
				if ( pDisp->item.strText == _T("-") )
				{
					pDisp->item.strText = _T(" -");
				}

				if ( pDisp->item.col >= 2 )
				{
					pDisp->item.nFormat |= DT_RIGHT;	// �����ֶ��Ҷ���
				}
			}
		}
	}

	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

bool CIoViewTimeSaleStatistic::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_pThis != NULL );
	const MerchArray &aMerchs = m_pThis->m_tabInfo.m_aMerchsVisible;
	ASSERT( aMerchs.GetSize() >= iRow1 && aMerchs.GetSize() >= iRow2  );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;

	CMerch *pMerch1 = aMerchs[iIndex1];
	CMerch *pMerch2 = aMerchs[iIndex2];

	// Ӧ��ֻ�д�������Ʋ��ܽ�����������������ܽ���, ���ƺʹ�������ʱ��������Ʒ��������Ʒ����Ӧ������NULL�����

	ASSERT( NULL != pMerch1 && NULL != pMerch2 );
	if ( NULL == pMerch1 || NULL == pMerch2 )
	{
		return false;
	}

	const int iCol = m_pThis->m_GridCtrl.m_CurCol;
	if ( iCol == 0 )	// ����
	{
		//return pMerch1->m_MerchInfo.m_StrMerchCode > pMerch2->m_MerchInfo.m_StrMerchCode;
		return CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCode, -1
			, pMerch2->m_MerchInfo.m_StrMerchCode, -1) == CSTR_LESS_THAN;
	}
	else if ( iCol == 1 )
	{
		return CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCnName, -1
			, pMerch2->m_MerchInfo.m_StrMerchCnName, -1) == CSTR_LESS_THAN;
	}
	ASSERT( 0 );
	return true;
}

BOOL CIoViewTimeSaleStatistic::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewTimeSaleStatistic::OnVDataRealtimePriceUpdate( IN CMerch *pMerch )
{
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}
	for ( int i=rangeVisible.GetMinRow(); i < rangeVisible.GetMaxRow() ; i++ )
	{
		CMerch *pMerchRow = (CMerch *)m_GridCtrl.GetItemData(i, 0);
		if ( pMerchRow == pMerch )
		{
			//m_GridCtrl.RedrawRow(i);
			// ��ʵ��ֻҪ����ʵʱ���۱��Ϳ����� - Ŀǰʵʱ���۱����2��
			for ( int j=0; j < m_GridCtrl.GetColumnCount() ; j++ )
			{
				if ( GetColumnShowHeader(j) == ETSSHPriceNew )
				{
					m_GridCtrl.RedrawCell(i, j);
					break;
				}
			}
			return;
		}
	}
}

void CIoViewTimeSaleStatistic::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespPeriodMerchSort )
	{
		OnMerchSortResp((const CMmiRespPeriodMerchSort *)pRespPlugin);
	}
	else if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespPeriodTickEx
	         || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushTickEx
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx3 
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx5
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx10 )
	{
		OnTickExResp((const CMmiRespPeriodTickEx *)pResp);
	}
}

void CIoViewTimeSaleStatistic::OnTickExResp( const CMmiRespPeriodTickEx *pResp )
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}
	
	int i = 0;
	// ����������Ʒ������, ����пɼ��ģ�ˢ����ʾ
	MerchArray	aMerchsUpdate;
	CArray<T_TickEx, const T_TickEx &>	aTickexesUpdate;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		
		if ( CheckMerchInBlock(pMerch) != NULL )
		{
			aMerchsUpdate.Add(pMerch);
			aTickexesUpdate.Add(tick);
		}
	}

	ASSERT( aMerchsUpdate.GetSize() == aTickexesUpdate.GetSize() );

	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		T_TimeSaleStatisticData dataMy;
		dataMy.m_tickEx = aTickexesUpdate[i];
		m_mapMerchData[ aMerchsUpdate[i] ] = dataMy;		// ������ʾ������

		//TRACE(_T("Merch Tick ex: %s\r\n"), aMerchsUpdate[i]->m_MerchInfo.m_StrMerchCnName);
	}

	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		if ( IsMerchInMerchArray(aMerchsUpdate[i], m_tabInfo.m_aMerchsVisible)  )
		{
			m_GridCtrl.Refresh();	// ��Ҫˢ����ʾ
			break;
		}
	}
}

void CIoViewTimeSaleStatistic::OnMerchSortResp( const CMmiRespPeriodMerchSort *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	if ( m_tabInfo.IsSort()
		&& (pResp->m_iMarketId == -1 /*|| pResp->m_iMarketId == 65535*/)		// ǿ��	-	������ȫ�����أ�ֻ������
		&& pResp->m_eReportSortType == m_MmiRequestSys.m_eReportSortType
		&& pResp->m_bDescSort == m_MmiRequestSys.m_bDescSort
		&& pResp->m_iPeriod == m_MmiRequestSys.m_uiPeriod
		)
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			return;	// ������Ҫ������
		}
		// ɸѡ��Ʒ
		int i = 0;
		MerchArray aMerchMy, aMerchSrc, aMerchOrg;
		ConvertMerchKeysToMerchs(pResp->m_aMerchs, aMerchSrc, true);
		aMerchMy.SetSize(0, aMerchSrc.GetSize());
		aMerchOrg.Copy(m_tabInfo.m_aMerchs);
		for ( i=0; i < aMerchSrc.GetSize() ; i++ )
		{
			for ( int32 j=0; j < aMerchOrg.GetSize() ; j++ )
			{
				if ( aMerchSrc[i] == aMerchOrg[j] )
				{
					aMerchMy.Add(aMerchSrc[i]);
					aMerchOrg.RemoveAt(j);
					break;
				}
			}
		}
		//ASSERT( aMerchOrg.GetSize() == 0 );

		// ������ƷȨ��ԭ�򣬿��ܷ���������Ʒ�����Ϳͻ�������������ʵ�ʿͻ��˵õ��Ŀ���Ҫ���ڷ�������
		// ���ÿ�����Ʒ
		MerchArray aMerchVisible;
		aMerchVisible.Copy(aMerchMy);
		aMerchVisible.Append(aMerchOrg);	// ��ʣ�����Ʒ�ӽ�ȥ����

		ASSERT( aMerchVisible.GetSize() == m_tabInfo.m_aMerchs.GetSize() );
		m_tabInfo.m_aMerchsVisible.Copy(aMerchVisible);

		m_GridCtrl.SetSortColumn( m_tabInfo.m_iSortColumn );
		m_GridCtrl.SetSortAscending( !pResp->m_bDescSort );

		RequestViewDataCurrentVisibleRow();		// �����������

		m_GridCtrl.Refresh();
	}
}

void CIoViewTimeSaleStatistic::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
	
}

CMerch* CIoViewTimeSaleStatistic::CheckMerchInBlock( CMerch *pMerch )
{
	for ( int i=0; i < m_tabInfo.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_tabInfo.m_aMerchs[i] == pMerch )
		{
			return pMerch;
		}
	}
	return NULL;
}

void CIoViewTimeSaleStatistic::GetColumnHeaders( OUT HeaderArray &aHeaders )
{
	
	// 	CStringArray aColNames;
	// // 	��������������ɣ�
	// // 		��������������ɣ�
	// // 		������������ɣ�
	// // 		������������ɣ�
	// // 		�е�����������ɣ�
	// // 		�е�����������ɣ�
	// // 		С������������ɣ�
	// // 		С������������ɣ�
	// // 		��С������������ɣ�
	// // 		��С������������ɣ�
	// // 		����������Ԫ��
	// // 		�����������Ԫ��
	// // 		���󵥾���
	// // 		��������Ԫ��
	// // 		���������Ԫ��
	// // 		�󵥾���
	// // 		�е�������Ԫ��
	// // 		�е��������Ԫ��
	// // 		�е�����
	// // 		С��������Ԫ��
	// // 		С���������Ԫ��
	// // 		С������
	// // 		�����ܶ��Ԫ��
	// // 		�����ܶ��Ԫ��
	// // 		���󵥻�����%
	// // 		�󵥻�����%
	// // 		�ɽ�����
	// // 		ÿ�ʽ���Ԫ��
	// // 		ÿ�ʹ������ɣ�
	// // 		�ʽ�����
	// 	aColNames.Add(_T("����"));
	// 	aColNames.Add(_T("����"));
	// 	aColNames.Add(_T("����\n(Ԫ)"));
	// 	aColNames.Add(_T("����\n������\n(���)"));
	// 	aColNames.Add(_T("����\n������\n(���)"));
	// 	aColNames.Add(_T("��\n������\n(���)"));
	// 	aColNames.Add(_T("��\n������\n(���)"));
	// 	aColNames.Add(_T("��С��\n������\n(���)"));
	// 	aColNames.Add(_T("��С��\n������\n(���)"));
	// 	aColNames.Add(_T("�ʽ�����\n(��Ԫ)"));
	// 	aColNames.Add(_T("�������\n(%)"));
	// 	aColNames.Add(_T("�ɽ����\n(��Ԫ)"));
	// 	aColNames.Add(_T("����\n������\n(%)"));
	// 	aColNames.Add(_T("��\n������\n(%)"));
	// 	aColNames.Add(_T("�����\n������\n(%)"));
	// 	aColNames.Add(_T("ÿ�ʹ���\n(��)"));
// 	aColNames.Add(_T("ÿ�ʽ��\n(��Ԫ)"));
	static CArray<E_TimeSaleStatisticHeader, E_TimeSaleStatisticHeader>	aEHeaders;
	if ( aEHeaders.GetSize() == 0 )
	{
		aEHeaders.Add(ETSSHCode);
		aEHeaders.Add(ETSSHName);
		aEHeaders.Add(ETSSHPriceNew);
		
		aEHeaders.Add(ETSSHNetAmountSuper);
		aEHeaders.Add(ETSSHVolNetSuper);
		aEHeaders.Add(ETSSHVolBuySuper);
		aEHeaders.Add(ETSSHVolSellSuper);
		
		aEHeaders.Add(ETSSHNetAmountBig);
		aEHeaders.Add(ETSSHVolBuyBig);
		aEHeaders.Add(ETSSHVolSellBig);

		aEHeaders.Add(ETSSHNetAmountMid);
		aEHeaders.Add(ETSSHNetAmountSmall);
		aEHeaders.Add(ETSSHVolBuyMidSmall);
		aEHeaders.Add(ETSSHVolSellMidSmall);
		
		aEHeaders.Add(ETSSHAmountTotalBuy);
		aEHeaders.Add(ETSSHAmountTotalSell);
		aEHeaders.Add(ETSSHAmountPerTrans);
		aEHeaders.Add(ETSSHStocksPerTrans);
		
		aEHeaders.Add(ETSSHTradeRateSuper);
		aEHeaders.Add(ETSSHTradeRateBig);
	}
	
	aHeaders.RemoveAll();
	for ( int i=0; i < aEHeaders.GetSize() ; i++ )
	{
		E_TimeSaleStatisticHeader eHeader = aEHeaders[i];
		for ( int j=0; j < KTimeSaleStatisticHeadersCount ; j++ )
		{
			if ( KTimeSaleStatisticHeaders[j].m_eHeaderShow == eHeader )
			{
				aHeaders.Add(KTimeSaleStatisticHeaders[j]);
				break;
			}
		}
	}
}

E_ReportSortPlugIn CIoViewTimeSaleStatistic::GetColumnSortHeader( int iCol )
{
	if ( !m_GridCtrl.IsValid(0, iCol) )
	{
		return ERSPIEnd;
	}

	E_TimeSaleStatisticHeader eHeaderShow = (E_TimeSaleStatisticHeader)m_GridCtrl.GetItemData(0, iCol);
	for ( int i=0; i < KTimeSaleStatisticHeadersCount ; i++ )
	{
		if ( KTimeSaleStatisticHeaders[i].m_eHeaderShow == eHeaderShow )
		{
			return KTimeSaleStatisticHeaders[i].m_eHeaderSort;
		}
	}
	return ERSPIEnd;
}

E_TimeSaleStatisticHeader CIoViewTimeSaleStatistic::GetColumnShowHeader( int iCol )
{
	if ( !m_GridCtrl.IsValid(0, iCol) )
	{
		return ETSSHCount;
	}
	
	E_TimeSaleStatisticHeader eHeaderShow = (E_TimeSaleStatisticHeader)m_GridCtrl.GetItemData(0, iCol);

	return eHeaderShow;
}

bool32 CIoViewTimeSaleStatistic::IsMerchInMerchArray( CMerch *pMerch, const MerchArray &aMerchs )
{
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( aMerchs[i] == pMerch )
		{
			return true;
		}
	}
	return false;
}

LRESULT CIoViewTimeSaleStatistic::OnScrollPosChanged( WPARAM w, LPARAM l )
{
	HWND hwnd = (HWND)w;
	CXScrollBar *pHorz = GetXSBHorz();
	if ( m_XSBVert.m_hWnd == hwnd )
	{
		KillTimer(KSortAsyncTimerId);
		SetTimer(KSortAsyncTimerId, KSortAsyncTimerPeriod, NULL);		// �ؼ��������Ʒ
	}
	else if ( pHorz != NULL && pHorz->GetSafeHwnd() == hwnd )
	{
		// �����Ƿ���ʾ/����ˮƽ������
		ShowOrHideXHorzBar();
	}
	return 1;
}

void CIoViewTimeSaleStatistic::UpdateVisibleMerchs()
{
	KillTimer(KSortAsyncTimerId);

	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( rangeVisible.GetMinRow() != m_tabInfo.m_iStartRow ) // ������
	{
		int32 iOldStart = m_tabInfo.m_iStartRow;
		m_tabInfo.m_iStartRow = rangeVisible.GetMinRow() - m_GridCtrl.GetFixedRowCount() - 9;	// �������, �����Ԥ��������Ʒ����
		if ( m_tabInfo.m_iStartRow < 0  )
		{
			m_tabInfo.m_iStartRow = 0;
		}

// 		if ( m_tabInfo.IsSort()
// 			&& (rangeVisible.GetMaxRow() >= m_tabInfo.m_iStartRow + m_iMaxGridVisibleRow		// �������һ��������ʾ���ݵĽ���)
// 				|| rangeVisible.GetMinRow() < iOldStart	// �Ϲ�
// 				) )
// 		{
// 			RequestSortData();
// 		}

		//if ( m_tabInfo.IsSort() )		// ������㲻ͣ�ĸ��£�so
		//{
		//	RequestSortData(); // �����Ѿ�����ȫ��
		//}

		RequestViewDataCurrentVisibleRow();
	}
}

CBlockLikeMarket* CIoViewTimeSaleStatistic::GetCurrentBlock()
{
	return GetBlock(m_tabInfo.m_iBlockId);
}

CBlockLikeMarket* CIoViewTimeSaleStatistic::GetBlock( int32 iBlockId )
{
	// ��������û�л���A�����ݣ���ô���أ�
	return CBlockConfig::Instance()->FindBlock(iBlockId);
}

void CIoViewTimeSaleStatistic::RequestData( CMmiCommBase &req, bool32 bForce /*= false*/ )
{
	DoRequestViewData(req);
}

void CIoViewTimeSaleStatistic::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin;
	GetWindowRect(rcWin);
	if ( !rcWin.PtInRect(pos) )
	{
		pos = rcWin.TopLeft();
	}

	// DoTrackMenu(pos);
}

void CIoViewTimeSaleStatistic::DoTrackMenu(CPoint pos)
{
	// ѡ�������
	CBlockConfig::IdArray	aBlockIds;
	int32 i = 0;
	//CBlockLikeMarket *pBlockCur = CBlockConfig::Instance()->FindBlock(m_tabInfo.m_iBlockId);
	CBlockLikeMarket *pBlockSHSZA = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockSHSZA )
	{
		return;
	}
	ASSERT( pBlockSHSZA->m_blockInfo.m_aSubBlockIds.GetSize() > 0 );
	aBlockIds.Add(pBlockSHSZA->m_blockInfo.m_iBlockId);
	aBlockIds.Append(pBlockSHSZA->m_blockInfo.m_aSubBlockIds);
	for ( i=0; i < aBlockIds.GetSize() ; i++ )
	{
		if ( aBlockIds[i] == m_tabInfo.m_iBlockId )
		{
			aBlockIds.RemoveAt(i);
			break;
		}
	}
	if ( aBlockIds.GetSize() <= 0 )
	{
		return;		// û���򷵻�
	}
	
	CNewMenu menu;
	menu.CreatePopupMenu();
	int32 iCmd = 1;
	menu.AppendMenu(MF_STRING, iCmd++, _T("����Ʒ"));
	menu.SetDefaultItem(1, FALSE);

	BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("����������") : _T("��ʾ������"));
	
	menu.AppendMenu(MF_SEPARATOR);
	for ( i=0; i < aBlockIds.GetSize() ; i++ )
	{
		menu.AppendMenu(MF_STRING, iCmd +i, CBlockConfig::Instance()->GetBlockName(aBlockIds[i]));
	}

	CWnd *pWndParent = GetParent();
	if ( NULL != pWndParent && pWndParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
	{
		AppendStdMenu(&menu);
	}
	
	int iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_NONOTIFY |TPM_RETURNCMD, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet == 1 )
	{
		CMerch *pMerch = NULL;
		CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
		if ( cellRange.IsValid() && (pMerch = (CMerch *)m_GridCtrl.GetItemData(cellRange.GetMinRow(), 0)) != NULL )
		{
			OnDblClick(pMerch);
		}
	}
	else if ( iRet == 2 )
	{
		m_GridCtrl.ShowGridLine(!bShowGrid);
		m_GridCtrl.Refresh();
	}
	else if ( iRet >= iCmd )
	{
		iRet -= iCmd;
		if ( iRet >=0 && iRet < aBlockIds.GetSize() )
		{
			OpenBlock(aBlockIds[iRet]);
		}
		else
		{
			iRet += iCmd;	// ��ԭid
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, iRet, 0);
		}
	}
}

LRESULT CIoViewTimeSaleStatistic::OnDoTrackMenu( WPARAM w, LPARAM l )
{
	CPoint pos;
	pos.x = LOWORD(w);
	pos.y = HIWORD(w);
	DoTrackMenu(pos);
	return 1;
}

CMerch * CIoViewTimeSaleStatistic::GetMerchXml()
{
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	CMerch *pMerch = NULL;
	if ( rangeSel.IsValid() && (pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != NULL )
	{
		return pMerch;
	}
	return CIoViewBase::GetMerchXml();
}

CXScrollBar* CIoViewTimeSaleStatistic::SetXSBHorz( CXScrollBar *pHorz /*= NULL*/ )
{
	CXScrollBar *pOld = m_pCurrentXSBHorz;
	if ( NULL == pHorz )
	{
		pHorz = &m_XSBHorz;
	}
	
	if ( IsWindow(pHorz->GetSafeHwnd()) )
	{
		if ( m_pCurrentXSBHorz != pHorz )
		{
			m_pCurrentXSBHorz = pHorz;
			m_pCurrentXSBHorz->SetOwner(&m_GridCtrl);
			m_GridCtrl.SetScrollBar(m_pCurrentXSBHorz, &m_XSBVert);	// ��ֱ�����ṩ����������, ˮƽ������Ҫ����
			m_pCurrentXSBHorz->AddMsgListener(m_hWnd);	// ������������ʾ�������ع�����
			ShowOrHideXHorzBar();
		}
	}
	else
	{
		ASSERT( 0 );
	}
	return pOld;
}

CXScrollBar* CIoViewTimeSaleStatistic::GetXSBHorz() const
{
	return m_pCurrentXSBHorz;
}

void CIoViewTimeSaleStatistic::OnVDataReportInBlockUpdate( int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs )
{
	bool32 bNeedSortData = FALSE;
	if ( m_tabInfo.IsSort() )
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(m_tabInfo.m_iSortColumn);
			if ( eHeaderShow == ETSSHPriceNew )		// ���¼�����? �������¼ۻ���a�������
			{
				bNeedSortData = true;
			}
		}
	}
	if ( bNeedSortData
		&& iBlockId == m_MmiRequestBlockSys.m_iBlockId
		&& eMerchReportField == m_MmiRequestBlockSys.m_eMerchReportField
		&& bDescSort == m_MmiRequestBlockSys.m_bDescSort
		&& iPosStart == m_MmiRequestBlockSys.m_iStart
		&& iOrgMerchCount >= m_MmiRequestBlockSys.m_iCount	// ���������Ҳ��Ϊ��Ҫ���
		)
	{
		// ��ʵ�ϣ�����������Ǳ������л���a����Ʒ���У����Ƿ�������Ʒ�����п��ܴ��ڱ�����Ʒ���ϣ��������������Ǹ��鷳
		// ɸѡ��Ʒ
		int i = 0;
		MerchArray aMerchMy, aMerchSrc, aMerchOrg;
		aMerchSrc.Copy(aMerchs);
		aMerchMy.SetSize(0, aMerchSrc.GetSize());
		aMerchOrg.Copy(m_tabInfo.m_aMerchs);
		for ( i=0; i < aMerchSrc.GetSize() ; i++ )
		{
			for ( int32 j=0; j < aMerchOrg.GetSize() ; j++ )
			{
				if ( aMerchSrc[i] == aMerchOrg[j] )
				{
					aMerchMy.Add(aMerchSrc[i]);
					aMerchOrg.RemoveAt(j);
					break;
				}
			}
		}
		//ASSERT( aMerchOrg.GetSize() == 0 );
		if ( aMerchOrg.GetSize() > 0 )
		{
			TRACE(_T("�����д������ݲ������У�%d. ����ԭ��Ϊ��������Ʒ�뱾����Ʒ��һ��!!\r\n"), aMerchOrg.GetSize());
		}
		
		// ������ƷȨ��ԭ�򣬿��ܷ���������Ʒ�����Ϳͻ�������������ʵ�ʿͻ��˵õ��Ŀ���Ҫ���ڷ�������
		// ���ÿ�����Ʒ
		MerchArray aMerchVisible;
		aMerchVisible.Copy(aMerchMy);
		aMerchVisible.Append(aMerchOrg);	// ��ʣ�����Ʒ�ӽ�ȥ����
		
		ASSERT( aMerchVisible.GetSize() == m_tabInfo.m_aMerchs.GetSize() );
		m_tabInfo.m_aMerchsVisible.Copy(aMerchVisible);
		
		m_GridCtrl.SetSortColumn( m_tabInfo.m_iSortColumn );
		m_GridCtrl.SetSortAscending( !bDescSort );
		
		RequestViewDataCurrentVisibleRow();		// �����������
		
		m_GridCtrl.Refresh();
	}
}

void CIoViewTimeSaleStatistic::LockRedraw()
{
	if ( !m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->LockRedraw();
					}
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
				}
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = true;
	}
}

void CIoViewTimeSaleStatistic::UnLockRedraw()
{
	if ( m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
			Invalidate();
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);		// ����redraw���û�ʹhide��Ϊshow��so��Ҫע��
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->UnLockRedraw();
					}
				}
				
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = false;
	}
}

bool32 CIoViewTimeSaleStatistic::ShowOrHideXHorzBar()
{
	if ( GetXSBHorz() != NULL )
	{
		CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
		if ( !IsWindowVisible()
			|| (rangeVisible.IsValid() 
			&& rangeVisible.GetColSpan() == m_GridCtrl.GetColumnCount() - m_GridCtrl.GetFixedColumnCount()) )
		{
			GetXSBHorz()->ShowWindow(SW_HIDE);
			return false;
		}
		else
		{
			GetXSBHorz()->ShowWindow(SW_SHOW);
			return true;
		}
	}
	return false;
}

void CIoViewTimeSaleStatistic::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ShowOrHideXHorzBar();
}

void CIoViewTimeSaleStatistic::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	T_AttendMarketData data;
	if ( NULL != pBlock )
	{
		// �������е���Ʒ�ҳ����е��г�- -������ĳ���
		CBlockInfo::E_BlockType eBlockType = pBlock->m_blockInfo.GetBlockType();
		switch ( eBlockType )
		{
		case CBlockInfo::typeMarketClassBlock:
		case CBlockInfo::typeNormalBlock:
		case CBlockInfo::typeUserBlock:
		case CBlockInfo::typeHSASubBlock:
			{
				// ȫ��Ʒ����
				typedef CMap<int32, int32, int32, int32> MarketMap;
				MarketMap ms;
				POSITION pos = pBlock->m_blockInfo.m_mapSubMerchMarkets.GetStartPosition();
				while ( NULL != pos )
				{
					int32 iMarket, iValue;
					pBlock->m_blockInfo.m_mapSubMerchMarkets.GetNextAssoc(pos, iMarket, iValue);
					
					data.m_iMarketId = iMarket;
					data.m_iEDSTypes = EDSTGeneral;
					
					aAttends.Add(data);
				}
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
}

CMerch* CIoViewTimeSaleStatistic::GetNextMerch( CMerch* pMerchNow, bool32 bPre )
{
	CMerch *pMerchFind = NULL;
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		return CIoViewBase::GetNextMerch(pMerchNow, bPre);
	}
	// ȡǰһ����Ʒ
	for ( int32 i=m_tabInfo.m_aMerchsVisible.GetUpperBound() ; i >=0; --i )
	{
		if ( pMerchNow == m_tabInfo.m_aMerchsVisible[i] )
		{
			if ( bPre && i > 0 )
			{
				pMerchFind = m_tabInfo.m_aMerchsVisible[i-1];
			}
			else if ( !bPre && i < m_tabInfo.m_aMerchsVisible.GetUpperBound() )
			{
				pMerchFind = m_tabInfo.m_aMerchsVisible[i+1];
			}
			break;
		}
	}
	if ( NULL == pMerchFind && !m_tabInfo.IsSort() && m_GridCtrl.GetSortColumn() == -1 )
	{
		// �����������޷���ȡ����Ʒ����һ�����򰴰����˳��ȡ
		int32 iMerchCount = m_tabInfo.m_aMerchs.GetSize();
		for ( int32 i=0; i < iMerchCount ; ++i )
		{
			if ( pMerchNow == m_tabInfo.m_aMerchs[i] )
			{
				if ( bPre )
				{
					pMerchFind = m_tabInfo.m_aMerchs[ (i-1+iMerchCount)%iMerchCount ];
				}
				else
				{
					pMerchFind = m_tabInfo.m_aMerchs[ (i+1)%iMerchCount ];
				}
				break;
			}
		}
	}
	if ( NULL == pMerchFind )
	{
		return CIoViewBase::GetNextMerch(pMerchNow, bPre);
	}
	return pMerchFind;
}

void CIoViewTimeSaleStatistic::OnEscBackFrameMerch( CMerch *pMerch )
{
	// ȡǰһ����Ʒ
	for ( int32 i=0 ; i < m_tabInfo.m_aMerchsVisible.GetSize(); i++ )
	{
		if ( pMerch == m_tabInfo.m_aMerchsVisible[i] )
		{
			CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
			int32 iRow = m_GridCtrl.GetFixedRowCount()+i;
			m_GridCtrl.EnsureVisible(iRow, rangeVisible.GetMinCol());
			CCellID cellFocus = m_GridCtrl.GetFocusCell();
			if ( cellFocus.IsValid() )
			{
				m_GridCtrl.SetFocusCell(iRow, cellFocus.col);
			}
			m_GridCtrl.SetSelectedRange(iRow, 0, iRow, m_GridCtrl.GetColumnCount()-1);
			break;
		}
	}
}

void CIoViewTimeSaleStatistic::RefreshView( int32 iPeriod )
{
	m_iPeriod = iPeriod;

	if ( m_tabInfo.IsSort() )
	{
		RequestSortData();
	}
	else
	{
		RequestViewDataCurrentVisibleRow();	
	}
}

BOOL CIoViewTimeSaleStatistic::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator itHome = m_mapBtnCycleList.find(iID);
	if (m_mapBtnCycleList.end() != itHome)
	{
		switch(iID)
		{
		case  MID_BUTTON_PAIHANG_ONE:
			m_iPeriod = 1;
			break;

		case  MID_BUTTON_PAIHANG_THREE:
			m_iPeriod = 3;
			break;

		case  MID_BUTTON_PAIHANG_FIVE:
			m_iPeriod = 5;
			break;

		case  MID_BUTTON_PAIHANG_TEN:
			m_iPeriod = 10;
			break;

		default:
			break;
		}
	}

	RefreshView(m_iPeriod);

	return CWnd::OnCommand(wParam, lParam);
}

void CIoViewTimeSaleStatistic::OnLButtonUp( UINT nFlags, CPoint point )
{
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonUp();
	}	
}

void CIoViewTimeSaleStatistic::OnLButtonDown( UINT nFlags, CPoint point )
{
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonDown();


		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iButton)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}
}

void CIoViewTimeSaleStatistic::CreateBtnList()
{
	m_mapBtnCycleList.clear();

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom = GRIDCTRL_MOVEDOWN_SIZE;

	CRect rcBtn(rcClient);

	int iDistance = 15;

	m_pImgBtn = Image::FromFile(_T("image//subbtn.png"));


	rcBtn.top	 =  (GRIDCTRL_MOVEDOWN_SIZE - m_pImgBtn->GetHeight() / 3) / 2;;
	int iBtnWidth = m_pImgBtn->GetWidth();
	// ��Ϊ��5����ť���Գ���5
	rcBtn.left   =500;//rcClient.left +  (rcClient.Width() - (iBtnWidth + iDistance)* 5)/2;
	rcBtn.right  = rcBtn.left + iBtnWidth;
	rcBtn.bottom = rcBtn.top + m_pImgBtn->GetHeight() / 3;

	btnPrompt.CreateButton(L"��ѡ����:", rcBtn, this, NULL, 0, MID_BUTTON_PAIHANG_PROMPT);

	//��ʾ��ť��ʾ
	btnPrompt.SetTextBkgColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextFrameColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextColor(RGB(190,191,191), RGB(190,191,191), RGB(190,191,191));

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_ONE, L"����");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_THREE, L"3��");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_FIVE, L"5��");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_TEN, L"10��");
}

void CIoViewTimeSaleStatistic::RedrawCycleBtn( CPaintDC *pPainDC )
{
	//����gridctrlλ��
	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom  = rcClient.top+ GRIDCTRL_MOVEDOWN_SIZE + 1;

	CDC memDC;
	memDC.CreateCompatibleDC(pPainDC);
	CBitmap bmp;
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();
	bmp.CreateCompatibleBitmap(pPainDC, rcClient.Width(), rcClient.Height());
	//memDC.FillSolidRect(rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height(),RGB(37,40,45));
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);

	Graphics graphics(memDC.GetSafeHdc());

	SolidBrush  brush(Color(37,40,45));
	graphics.FillRectangle(&brush, rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height());

	btnPrompt.DrawButton(&graphics);
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.DrawButton(&graphics);
	}

	pPainDC->BitBlt(rcClient.left ,rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	pPainDC->SelectClipRgn(NULL);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void CIoViewTimeSaleStatistic::AddNCButton( LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption )
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextBkgColor(RGB(25,25,25), RGB(146,96,0), RGB(25,25,25));
	btnControl.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetTextColor(RGB(190,191,191), RGB(255,255,255), RGB(255,255,255));
	m_mapBtnCycleList[nID] = btnControl;
}

int CIoViewTimeSaleStatistic::TNCButtonHitTest( CPoint point )
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

// bool CIoViewTimeSaleStatistic::operator()( int iRow1, int iRow2 )
// {
// 	return CompareRow(iRow1, iRow2);
// }


//////////////////////////////////////////////////////////////////////////
//

CIoViewTimeSaleStatistic::T_TimeSaleStatisticData::T_TimeSaleStatisticData()
{
	// TickEx��ʼ�� TODO	
}

#include "stdafx.h"
#include "memdc.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "DialogExportSaleData.h"
#include "FontFactory.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
const int32 KShowTimeSaleCount		 = 10000;
const int32 KNormalShowTimeSaleCount = 50;

const int32 KMinRequestTimeSaleCount = 500;    // ����޷�ȷ����ʾ�������ݴ�С����ѡ�����ֵ����������

static const int32 KForceRequestTimeSalePage = INT_MAX - 1; // ǿ�����뵱ǰ�����������µ���ʷ���� - �Է�ֹ����������ʧ�Ĳ������±��۳�Ϊ����ʷ���ۣ���������û������

const int32 KTimerIdRefresh			= 6005;
const int32 KTimerPeriodRefresh		= 300;
//////////////////////////////////////////////////////////////////////

void TestRecordData(CArray<CTick, CTick>& aTick);

// 
CShowTick::CShowTick()
{
	m_eTickType			= ETTTrade;
	m_fPrice			= 0.;
	m_fVolume			= 0.;
	m_fHold				= 0.;
	m_eTradeKind		= CTick::ETKUnKnown;

	m_TimePrev			= CMsTime(0);
	m_fHoldTotalPrev	= 0.;
	m_fPricePre			= 0.;
	m_StrProperty		= L"-";
}

const CShowTick& CShowTick::operator=(const CTick &Tick)
{
	m_TimeCurrent	= Tick.m_TimeCurrent;		// ��ʱ��Ϊ��Ʒ����ʱ����ʱ�䣬 ��������ֶθ�ֵ�� ������Ϊ0ֵ�� ϵͳ����Ը��ֶεĴ�������ù����� �������ȷ�ȵ�����	
	m_eTickType		= Tick.m_eTickType;			// ����
	m_fPrice		= Tick.m_fPrice;			// �۸�
	m_fVolume		= Tick.m_fVolume;			// ����
	m_fHold			= Tick.m_fHold;				// �ֲܳ���
	m_eTradeKind	= Tick.m_eTradeKind;		// ��������Ϣ
	
	m_TimePrev		= CMsTime(0);
	m_fHoldTotalPrev= 0.;
	m_fPricePre		= 0.;
	m_StrProperty	= L"-";

	return *this;	
}

void CShowTick::Calculate()
{
	// 
	float fHoldAdd		= m_fHold - m_fHoldTotalPrev;	// ���ʲֲ�
	
	// ����������ʾ����
	m_StrProperty = L"-";
	if (fHoldAdd == 0.)	// ��¼����
	{
		if (CTick::ETKSell == m_eTradeKind)	
		{
			m_StrProperty = L"�໻";	// �����ǰ�ĳɽ�Ϊ���̣����¼��ǰ����Ϊ��ͷ���֡��������ϼ��Ϊ���໻����
		}
		else if (CTick::ETKBuy == m_eTradeKind)	
		{
			m_StrProperty = L"�ջ�";	// �����ǰ�ĳɽ�Ϊ���̣����¼��ǰ����Ϊ��ͷ���֡��������ϼ��Ϊ���ջ�����
		}
	}
	else if (m_fVolume == fHoldAdd)	
	{
		m_StrProperty = L"˫��";		// ���ʳɽ���=���ʲֲ���¼Ϊ˫�߿��֡��������ϼ��Ϊ��˫������
	}
	else if (m_fVolume == -fHoldAdd)
	{
		m_StrProperty = L"˫ƽ";		// ���ʳɽ���=-���ʲֲ���¼Ϊ˫��ƽ�֡��������ϼ��Ϊ��˫ƽ����
	}
	else if (fHoldAdd < m_fVolume)
	{
		if (fHoldAdd > 0)
		{
			if (CTick::ETKSell == m_eTradeKind)	
			{
				m_StrProperty = L"�࿪";	// �����ǰ�ĳɽ�Ϊ���̣����¼��ǰ����Ϊ��ͷ���֡��������ϼ��Ϊ���࿪����
			}
			else if (CTick::ETKBuy == m_eTradeKind)	
			{
				m_StrProperty = L"�տ�";	// �����ǰ�ĳɽ�Ϊ���̣����¼��ǰ����Ϊ��ͷ���֡��������ϼ��Ϊ���տ�����
			}
		}
		else if (fHoldAdd < 0)
		{
			if (CTick::ETKSell == m_eTradeKind)	
			{
				m_StrProperty = L"��ƽ";	// �����ǰ�ĳɽ�Ϊ���̣����¼Ϊ��ͷƽ�֡��������ϼ��Ϊ����ƽ����
			}
			else if (CTick::ETKBuy == m_eTradeKind)	
			{
				m_StrProperty = L"��ƽ";	// �����ǰ�ĳɽ�Ϊ���̣����¼Ϊ��ͷƽ�֡��������ϼ��Ϊ����ƽ����
			}
		}
	}
}


/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTimeSale, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTimeSale, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewTimeSale)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EXPORT_DATA,OnExportData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSale::CIoViewTimeSale()
:CIoViewBase()
{
	m_iSaveDec			= 2;

	m_aTicksShow.SetSize(0, KShowTimeSaleCount);
	m_eTimeSaleHeadType = ETHTCount;

	//
	m_bShowFull			= false;			
	m_bShowNewestFull	= true;	
	m_bRequestedTodayFirstData = false;

	m_iPerfectWidth		= 0;		
	m_iGridFullRow		= 0;				
	m_iGridFullCol		= 0;			
	m_iDataFullNums		= 0;		
	m_iGridFullColUints	= 0;

	m_iFullBeginIndex	= -1;
	m_iFullEndIndex		= -1;
	m_bActive			= false;

	m_bHistoryTimeSale = false;

	m_bSetTimer		   = FALSE;
	m_bMerchChange	   = FALSE;
	m_TimePre		   = ::time(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSale::~CIoViewTimeSale()
{
}

CIoViewTimeSale::E_TimeSaleHeadType CIoViewTimeSale::GetMerchTimeSaleHeadType(E_ReportType eReportType)
{
	if (ERTFuturesCn == eReportType)
		return CIoViewTimeSale::ETHTFuture;

	return ETHTSimple;
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
BOOL CIoViewTimeSale::PreTranslateMessage(MSG* pMsg)
{
	if ( m_bHistoryTimeSale )
	{
		// ��ʷʱ����������Щ��Ϣ
		return CControlBase::PreTranslateMessage(pMsg);
	}

	if ( WM_LBUTTONDBLCLK == pMsg->message )
	{
		// ˫����ʱ��,�����ͼ:
		CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
		CGGTongView* pGGTongView	= GetParentGGtongView();
		
		//
		if ( NULL != pMainFrame && NULL != pGGTongView )
		{
			pMainFrame->OnProcessF7(pGGTongView);
			return TRUE;
		}
	}
	else if ( WM_KEYDOWN == pMsg->message )
	{
		if ( !m_bShowFull )
		{
			// ����״̬�£�����������¼�����Ĭ�Ͻ��۽���ʾ���ٽ���
			if ( VK_UP == pMsg->wParam )
			{
				CCellID idTopLeft = m_GridCtrl.GetTopleftNonFixedCell();
				if ( m_GridCtrl.IsValid(idTopLeft) )
				{
					m_GridCtrl.SetFocusCell(idTopLeft);
				}
			}
			else if ( VK_DOWN == pMsg->wParam )
			{
				CCellRange rangeVis = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
				if ( m_GridCtrl.IsValid(rangeVis) )
				{
					m_GridCtrl.SetFocusCell(rangeVis.GetMaxRow(), rangeVis.GetMinCol());
				}
			}
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewTimeSale::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
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
	
	if ( m_GridCtrl.GetSafeHwnd() && !m_bShowFull )
	{
		m_GridCtrl.ShowWindow(SW_SHOW);	
		m_GridCtrlFull.ShowWindow(SW_HIDE);
		m_GridCtrl.RedrawWindow();
	}

	if ( m_GridCtrlFull.GetSafeHwnd() && m_bShowFull )
	{
		m_GridCtrlFull.ShowWindow(SW_SHOW);
		m_GridCtrl.ShowWindow(SW_HIDE);
		m_GridCtrlFull.RedrawWindow();
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewTimeSale::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewTimeSale::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

void CIoViewTimeSale::OnLButtonDblClk(UINT nHitTest, CPoint point)
{
	CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
	CGGTongView* pGGTongView	= GetParentGGtongView();
	
	//
	if ( NULL != pMainFrame && NULL != pGGTongView )
	{
		pMainFrame->OnProcessF7(pGGTongView);
	}

	CStatic::OnNcLButtonDblClk(nHitTest, point);
}
///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewTimeSale::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt;
	pt.x = point.x;
	pt.y = point.y + 1;

	CStatic::OnMouseMove(nFlags, point);
}

BOOL CIoViewTimeSale::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	// ...fangz1204 ÿ�ζ���������? ��ʱ��ô����һ����
	// DefWindowProc OnMouseWheel��Ϊ���������ڷ���WM_MOUSEWHEEL������FALSE
	// ����Grid�ؼ����ص���DefWindowProc��FALSE������CWnd::WindowProc���ٴε���DefWindowProc
	// �������ε��ò�����
	static bool32 bProcess = true;
	bProcess = !bProcess;

	// ��ǰ����������ʾ.������ʱ��ҳ,���������ʾ����
	
	if ( m_bShowFull && !bProcess )
	{		
		// 
		int32  iDir		 = zDelta / 120;	
		bool32 bNextPage = true;
		
		if ( iDir > 0 )
		{
			bNextPage = false;
		}

		//
		if ( -1 == m_iFullBeginIndex || -1 == m_iFullEndIndex )
		{
			// ��û������ʼ��,������
			return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
		}

		if ( m_bShowNewestFull && bNextPage )
		{
			// �Ѿ�������һҳ,���·�.������
			return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
		}

		// 

		// ����zDelta Զ���û��Ĺ����� ���� Ϊ������OnPageScroll���� ��Ϊ���ϣ���ת�·���
		OnPageScroll( - iDir );

		// ��������
		
		return TRUE;
	}
	//else if ( !bProcess && !m_bShowFull )
	//{
	//	// �Ƕ���ģʽ
	//}

	return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
}

void CIoViewTimeSale::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	SetRowHeightAccordingFont();

	if ( !m_bShowFull )
	{
		m_GridCtrl.DeleteNonFixedRows();
		SetTableData();
	}

	// Full����������С�Ļ����������ݲ���
	OnPageScroll(0);

	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewTimeSale::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);	
}

void CIoViewTimeSale::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewTimeSale::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	//
	InitialIoViewFace(this);

	// �������ݹ�����
 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
 	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_HIDE);

	// �����ֱ����ݱ��
	{
		m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 20205);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.SetColumnCount(5);
		m_GridCtrl.SetColumnResize(FALSE);
		
		// ���ñ��ͼ��
		m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
		m_GridCtrl.SetImageList(&m_ImageList);
		
		// �����໥֮��Ĺ���
		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
		
		// ���ñ����
		m_GridCtrl.EnableSelection(false);
		m_GridCtrl.ShowGridLine(false);
		m_GridCtrl.SetDonotScrollToNonExistCell(GVL_BOTH);
	}

	// ���������ʾ�ı��
	{
		m_GridCtrlFull.Create(CRect(0, 0, 0, 0), this, 20206);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrlFull.SetColumnCount(1);
		m_GridCtrlFull.SetRowCount(1);
		m_GridCtrlFull.SetColumnResize(FALSE);
		
		// ���ñ��ͼ��
		m_GridCtrlFull.SetImageList(&m_ImageList);
		
		// ���ñ����
		m_GridCtrlFull.EnableSelection(false);
		m_GridCtrlFull.ShowGridLine(false);
	}

	m_GridCtrl.EnablePolygonCorner(false);
	m_GridCtrlFull.EnablePolygonCorner(false);

	SetShowType(false);

	// ����Ĭ�ϵı�ͷ
	SetTimeGridHead(ETHTSimple);
	SetRowHeightAccordingFont();
	
	return 0;
}

// ���ñ�ͷ
void CIoViewTimeSale::SetTimeGridHead(E_TimeSaleHeadType eTimeSaleHeadType)
{
	if (NULL == m_GridCtrl.GetSafeHwnd())
		return;

	if ( !m_bShowFull )
	{
		// ������ʾ�ı��
		m_GridCtrl.DeleteAllItems();
		
		if (ETHTSimple == eTimeSaleHeadType)
		{
			m_GridCtrl.SetHeaderSort(FALSE);
			m_GridCtrl.SetFixedRowCount(0);
			m_GridCtrl.SetColumnCount(3);
		}
		else if (ETHTFuture == eTimeSaleHeadType)
		{
			m_GridCtrl.SetHeaderSort(FALSE);
			m_GridCtrl.SetFixedRowCount(0);
			m_GridCtrl.SetColumnCount(5);
		}
	}
	else
	{
		// ��󻯵ı��:
		CRect RectGrid;
		//
		if ( m_iGridFullColUints <= 1 )
		{
			return;
		}

		if (ETHTSimple == eTimeSaleHeadType)
		{
			int iCol = NoShowNewvol()? 2 : 3;
			for ( int32 i = 0 ; i < m_iGridFullColUints; i++ )
			{
				CGridCellSys *pCell = NULL;
				
				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 0);
				pCell->SetText(L" ʱ��");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 1);
				pCell->SetText(L"�۸� ");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				if (iCol == 3)
				{
					pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 2);
					pCell->SetText(L"����");
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);		
					pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
				}
			}	
			m_GridCtrlFull.ExpandColumnsToFit();
		}
		else if (ETHTFuture == eTimeSaleHeadType)
		{
			for ( int32 i = 0 ; i < m_iGridFullColUints; i++ )
			{				
				CGridCellSys *pCell = NULL;
				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 0);
				pCell->SetText(L" ʱ��");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 1);
				pCell->SetText(L"�۸�");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 2);
				pCell->SetText(L"����");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 3);
				pCell->SetText(L"�ֲ�");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 4);
				pCell->SetText(L"���� ");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);		
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
			}
			m_GridCtrlFull.ExpandColumnsToFit();
		}
	}
	
	//
	m_eTimeSaleHeadType = eTimeSaleHeadType;	
}

void CIoViewTimeSale::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
			
	CRect rect = m_rectClient;

	if ( m_rectClient.Width() <= 0 || m_rectClient.Height() <= 0 )
	{
		return;
	}

 	CRect RectSB(rect);
	CRect RectGridCtrl(rect);
	CRect RectGridFull(rect);
	//
	m_GridCtrl.MoveWindow(RectGridCtrl);
	m_GridCtrl.ExpandColumnsToFit();
	//	
	bool32 bShowFullBk		= m_bShowFull;

	//
	m_GridCtrlFull.MoveWindow(RectGridFull);
	CalcFullGridParams();
	m_GridCtrlFull.ExpandColumnsToFit();

	//--- wangyongxue 2016/06/14 ��ȫ��״̬�¶�ֻ��ʾһ������
	if ( m_iGridFullColUints <= 1 )
	{
		SetShowType(false);
	}
	else
	{
		SetShowType(true);
	}

	// �������Ҫ��ձ��,������ֵ

	// ���������� m_iFullBeginIndex\endIndex\showNewest����OnPageSroll����, SetTableData������������

	// 1: ��ʼ�����ʾ,����������ʾ:
	if ( bShowFullBk && !m_bShowFull )
	{
		SetTimeGridHead(m_eTimeSaleHeadType);
		SetTableData();
	}

	// 2: ��ʼ������ʾ,���������ʾ:
	if ( !bShowFullBk && m_bShowFull )
	{
		SetTimeGridHead(m_eTimeSaleHeadType);
		// ������ҳβ
		OnPageScroll(INT_MAX);
	}

	// 3: ��ʼ�����ʾ,���ڻ��������ʾ,������ʾ�Ĳ�������:
	if ( bShowFullBk && m_bShowFull )
	{	
		SetTimeGridHead(m_eTimeSaleHeadType);
		OnPageScroll(0);
	}
}
 
BOOL CIoViewTimeSale::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	static bool32 bProcess = true;
	bProcess = !bProcess;

	if ( IsWindowVisible() && VK_F1 == nChar && !bProcess )
	{
		CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
		CGGTongView* pGGTongView	= GetParentGGtongView();
		
		//
		if ( NULL != pMainFrame && NULL != pGGTongView )
		{
			pMainFrame->OnProcessF7(pGGTongView);
			return TRUE;
		}		
	}
	else if ( VK_HOME == nChar && !bProcess )
	{
		// ��һҳ, ��ʾ���ϵ�
		if ( m_bShowFull )
		{			
			OnPageScroll(INT_MIN);
				
			return TRUE;
		}
	}
	else if ( VK_END == nChar && !bProcess )
	{
		// ���һҳ, ��ʾ���µ�
		if ( m_bShowFull )
		{			
			OnPageScroll(INT_MAX);
			return TRUE;
		}
	}

	return FALSE;
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewTimeSale::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	CalcFullGridParams();
	
	// ��ǰ��Ʒ��Ϊ�յ�ʱ��
	if (NULL != pMerch)
	{
		// ���ù�ע����Ʒ��Ϣ
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTTimeSale | EDSTTick | EDSTKLine | EDSTPrice;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);

		// �������ǰ��ʾ����
		DeleteTableContent();

		// �����ǰ����
		m_aTicksShow.SetSize(0);

		// ȷ��������ʱ���Լ�
		m_iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

		// �����Ҫ�� �ı����
		E_TimeSaleHeadType eTimeSaleHeadType = GetMerchTimeSaleHeadType(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
		if (eTimeSaleHeadType != m_eTimeSaleHeadType)
		{
			SetTimeGridHead(eTimeSaleHeadType);
		}

		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		bool32 bTradingDayOk = false;
		if ( m_bHistoryTimeSale )
		{
			bTradingDayOk = pMerch->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerch->m_MerchInfo);
		}
		else
		{
			bTradingDayOk = pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo);
		}
		if ( bTradingDayOk )
		{
			m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			m_TrendTradingDayInfo.RecalcHold(*pMerch);
		}
		
		// 
		m_bMerchChange = TRUE;
		OnVDataMerchTimeSalesUpdate(pMerch);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
}

//
void CIoViewTimeSale::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewTimeSale::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	if (!::IsWindow(m_GridCtrl.GetSafeHwnd())) 
		return;

	// 
	CMerch &Merch = *pMerch;
	if (NULL == Merch.m_pMerchTimeSales)
	{
		return;
	}
		
	if ( 0 == Merch.m_pMerchTimeSales->m_Ticks.GetSize() )
	{
		return;
	}

	// TestRecordData(Merch.m_pMerchTimeSales->m_Ticks);

	// �Ƚ����ݱ仯�� ���ڽ���������������ж�
	int32 iPosDayStart = CMerchTimeSales::QuickFindTickWithBigOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit);
	if (iPosDayStart < 0 || iPosDayStart >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	if ( iPosDayStart > 0 )
	{
		// ������ǰ�����ݶ������ˣ�������������ǰ�����������ݣ���������ͱ�־�������Ѿ����뵽��ǰ������
		// ���ڴ�����ʷ�ֱʣ��������Ҳ�ǲ�ȷ����, ��ʱȥ������ xl 110219
		// m_bRequestedTodayFirstData = true;
	}
	else
	{
		m_bRequestedTodayFirstData = false;
	}

	int32 iPosDayEnd = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd);
	if (iPosDayEnd < 0 || iPosDayEnd >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	if (iPosDayEnd < iPosDayStart)
	{
		TestRecordData(Merch.m_pMerchTimeSales->m_Ticks);
		return;
	}

	// �Ż��������������£� ���¼���������ʵʱ�ֱ����ݸ�������ģ� ����������������ж�

	CTick *pTickShow	= (CTick *)m_aTicksShow.GetData();
	int32 iCountShow	= m_aTicksShow.GetSize();

	CTick *pTickSrc		= (CTick *)Merch.m_pMerchTimeSales->m_Ticks.GetData();
	pTickSrc		   += iPosDayStart;
	int32 iCountSrc		= iPosDayEnd - iPosDayStart + 1;

	// ȫ������
	{
  		// 
 		if (iCountSrc > KShowTimeSaleCount + 1000)
 		{
			// ��Ծ��Ʒ�����10062�����
			TRACE(_T("��Ʒtick����������\r\n"));
 		}
 		
		// ֱ�Ӹ��ƾ�����
 		m_aTicksShow.SetSize(iCountSrc);
 		pTickShow = (CTick *)m_aTicksShow.GetData();
 		iCountShow = m_aTicksShow.GetSize();
		
		memcpyex(pTickShow, pTickSrc, iCountShow * sizeof(CTick));

		CGmtTime timeNow = ::time(NULL);
		if (((timeNow.GetTime()-m_TimePre.GetTime()) < KTimerPeriodRefresh) && !m_bMerchChange)
		{
			m_TimePre = timeNow;
			if (!m_bSetTimer)
			{
				m_bSetTimer = TRUE;
				SetTimer(KTimerIdRefresh, KTimerPeriodRefresh, NULL);
			}
			return;
		}

		m_bMerchChange = FALSE;
		m_TimePre = timeNow;
		// 
		SetTableData();
	}
}

void CIoViewTimeSale::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// ��Ҫ�������ռ�
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
		float fBackupPricePrevHold	= m_TrendTradingDayInfo.GetPrevHold();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice() || fBackupPricePrevHold != m_TrendTradingDayInfo.GetPrevHold())
		{
			SetTableData();
		}
	}
}

void CIoViewTimeSale::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// ��Ҫ�������ռ�
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
		float fBackupPricePrevHold	= m_TrendTradingDayInfo.GetPrevHold();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice() || fBackupPricePrevHold != m_TrendTradingDayInfo.GetPrevHold())
		{
			SetTableData();
		}
	}
}

void CIoViewTimeSale::SetTableData()
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}

	//
 	if ( GetParentGGTongViewDragFlag() )
 	{
 		LockRedraw();		
 		return;
 	}
 	else
 	{
 		UnLockRedraw();		
 	}

	if ( m_bShowFull )
	{
		// ���������������: (�����ʾ)
		if ( !::IsWindow(m_GridCtrl.GetSafeHwnd()) )
		{
			return;
		}
			
		m_GridCtrlFull.DeleteNonFixedRows();
		
		m_GridCtrlFull.SetRowCount(m_iGridFullRow);
		m_GridCtrlFull.SetColumnCount(m_iGridFullCol);

		int32 iTickSize = m_aTicksShow.GetSize();

		if ( iTickSize > 0 )
		{
			COLORREF clrRise	= GetIoViewColor(ESCRise);
			COLORREF clrFall	= GetIoViewColor(ESCFall);
			COLORREF clrKeep	= GetIoViewColor(ESCKeep);
			COLORREF clrVolume	= GetIoViewColor(ESCVolume);
			
			CTick *pTickShow = (CTick *)m_aTicksShow.GetData();
			
			if ( iTickSize < m_iDataFullNums )
			{
				// ������ݵĸ�������һҳ��ʾ,�������Ͻǿ�ʼ������ʾ
				int32 iRowIndex		= 1;
				int32 iColUintIndex	= 0;

				// ��ʼ����������				
				m_iFullBeginIndex	= 0;
				m_iFullEndIndex		= iTickSize - 1;
				m_bShowNewestFull	= true;
				

				if ( !BeValidFullGridIndex() )
				{
					return;
				}

				for ( int32 i = m_iFullBeginIndex; i <= m_iFullEndIndex; i++ )
				{
					CShowTick ShowTick;
					ShowTick = pTickShow[i];
					
					if ( i > 0 )
					{
						ShowTick.m_TimePrev		  = pTickShow[i - 1].m_TimeCurrent;
						ShowTick.m_fHoldTotalPrev = pTickShow[i - 1].m_fHold;
						ShowTick.m_fPricePre	  = pTickShow[i - 1].m_fPrice;
					}
					else
					{
						ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
						ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();
					}

					ShowTick.Calculate();

					//
					SetRowValue(ShowTick, iRowIndex, iColUintIndex, clrRise, clrFall, clrKeep, clrVolume);
					
					iRowIndex++;
					
					if ( iRowIndex == m_iGridFullRow )
					{
						iRowIndex		= 1;
						iColUintIndex   = iColUintIndex + 1;

						if ( iColUintIndex >= m_iGridFullColUints )
						{
							break;
						}
					}
				}
			}
			else
			{
				// �������½ǿ�ʼ
				int32 iRowIndex		= m_iGridFullRow - 1;
				int32 iColUintIndex	= m_iGridFullColUints -1;

				if ( -1 == m_iFullBeginIndex || -1 == m_iFullEndIndex || m_bShowNewestFull )
				{
					// ��һ�ε�ʱ��, ��������ʵ����һ�����ݵ�ʱ��
					m_iFullEndIndex		= iTickSize - 1;
					m_iFullBeginIndex	= m_iFullEndIndex - m_iDataFullNums + 1;
					m_bShowNewestFull	= true;
				}

				if ( !BeValidFullGridIndex() )
				{
					return;
				}

				//TRACE(_T("----------ShowTimeSale: %d - %d\r\n"), m_iFullBeginIndex, m_iFullEndIndex);
				for (int32 j = m_iFullEndIndex; j >= m_iFullBeginIndex; j-- )
				{
					
					// ��������������ʾ
					CShowTick ShowTick;
					ShowTick = pTickShow[j];
					
					if ( j > 0 )
					{
						ShowTick.m_TimePrev		  = pTickShow[j - 1].m_TimeCurrent;
						ShowTick.m_fHoldTotalPrev = pTickShow[j - 1].m_fHold;
						ShowTick.m_fPricePre	  = pTickShow[j - 1].m_fPrice;
					}
					else
					{
						ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
						ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();

					}
										
					ShowTick.Calculate();
					
					//
					SetRowValue(ShowTick, iRowIndex, iColUintIndex, clrRise, clrFall, clrKeep, clrVolume);
	
					iRowIndex--;

					if ( iRowIndex == 0 )
					{
						iRowIndex		= m_iGridFullRow - 1;
						iColUintIndex   = iColUintIndex - 1;					

						if ( iColUintIndex < 0 )
						{
							break;
						}
					}
				}
			}			
		}
		
		// ���ø���					
		for ( int32 iRow = 0; iRow < m_GridCtrlFull.GetRowCount(); iRow++ )
		{
			for ( int32 iCol = 0; iCol < m_GridCtrlFull.GetColumnCount(); iCol++ )
			{
				CGridCellSys* pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(iRow, iCol);

				if ( iRow == (m_GridCtrlFull.GetRowCount() - 1) )
				{
					// ���һ������Ӻ���
					pCell->SetCellRectDrawFlag( DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
				}

				if ( 0 == m_iGridFullColUints )
				{
					continue;
				}

				if ( 0 == ((iCol + 1) % ((m_GridCtrlFull.GetColumnCount() / m_iGridFullColUints))) )
				{
					// ��Ԫ���ұ߼��Ϻ���
					if ( 0 != iRow )
					{
						pCell->SetCellRectDrawFlag( DR_RIGHT_SOILD | pCell->GetCellRectDrawFlag());
					}					
				}
			}
		}

		//
		m_GridCtrlFull.ExpandColumnsToFit(TRUE);
		m_GridCtrlFull.Refresh();
		m_GridCtrlFull.Invalidate();	
		
		//
		SetRowHeightAccordingFont();
	}
	else
	{
		// ���������������: (������ʾ)
		// ����ǰ�����µ���ʾ�����޸�Ϊ���µ���ʾ����
		if ( !::IsWindow(m_GridCtrl.GetSafeHwnd()) )
			return;
		
		if ( m_XSBVert.GetSafeHwnd() && m_XSBVert.IsWindowVisible() )
		{
			m_XSBVert.ShowWindow(SW_HIDE);
		}
		
		m_GridCtrl.DeleteNonFixedRows();
		if (m_aTicksShow.GetSize() > 0)
		{
			COLORREF clrRise = GetIoViewColor(ESCRise);
			COLORREF clrFall = GetIoViewColor(ESCFall);
			COLORREF clrKeep = GetIoViewColor(ESCKeep);
			COLORREF clrVolume = GetIoViewColor(ESCVolume);
			
			//
			CTick *pTickShow = (CTick *)m_aTicksShow.GetData();
			int32 iFullSize  = m_aTicksShow.GetSize();

			//
			CArray<CTick, CTick> aTicksNormalShow;
			aTicksNormalShow.SetSize(KNormalShowTimeSaleCount);
			CTick* pTickNormalShow = (CTick *)aTicksNormalShow.GetData();

			// �������µ�100 ������,������ʾ:
			if ( iFullSize > KNormalShowTimeSaleCount )
			{
				pTickShow += ( iFullSize - KNormalShowTimeSaleCount );					
				memcpyex(pTickNormalShow, pTickShow, sizeof(CTick)*KNormalShowTimeSaleCount);				

				//
				m_GridCtrl.SetRowCount(aTicksNormalShow.GetSize() + 1);
			}
			else
			{
				memcpyex(pTickNormalShow, pTickShow, sizeof(CTick)*iFullSize);		
				
				//
				aTicksNormalShow.SetSize(iFullSize);
				m_GridCtrl.SetRowCount(iFullSize + 1);
			}			
			
			//
			for (int32 i = 0; i < aTicksNormalShow.GetSize(); i++)
			{
				// ��������������ʾ
				CShowTick ShowTick;
				ShowTick = pTickNormalShow[i];
				
				if (i > 0)
				{
					ShowTick.m_TimePrev		  = pTickNormalShow[i - 1].m_TimeCurrent;
					ShowTick.m_fHoldTotalPrev = pTickNormalShow[i - 1].m_fHold;
					ShowTick.m_fPricePre	  = pTickNormalShow[i - 1].m_fPrice;
				}
				else
				{
					ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
					ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();
				}
				
				ShowTick.Calculate();
				//SetRowValue(ShowTick, iRowCount - i - 1, 0, clrRise, clrFall, clrKeep, clrVolume);
				SetRowValue(ShowTick, i + 1, 0, clrRise, clrFall, clrKeep, clrVolume);	// Ԥ��һ���У�

				for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
				{
					for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
					{				
						CGridCellBase* pCell = m_GridCtrl.GetCell(i, j);
						if (NULL != pCell)
						{
							if (0 == j)
							{
								pCell->SetTextPadding(CRect(15,0,0,0));
							}

							if (m_GridCtrl.GetColumnCount()- 1 == j)
							{
								pCell->SetTextPadding(CRect(0,0,15,0));
							}
						}
					}
				}
			}
		}
		
		//
		//if ( m_XSBVert.GetSafeHwnd() && !m_XSBVert.IsWindowVisible() )
		//{
		//	m_XSBVert.ShowWindow(SW_SHOW);
		//}

		m_GridCtrl.DeleteRow(0);	// ����ʾ��ͷ - ��Ȼ����û�б�ͷ��������setRowValue��ʱ��Ԥ����һ���յı�ͷ

		m_GridCtrl.ExpandColumnsToFit(TRUE);
		//
		SetRowHeightAccordingFont();
		// �������һ�����������ݣ�������Ҫ����
		if ( m_GridCtrl.GetRowCount() > 0 )
		{
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
			m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);
		}
		//
		m_GridCtrl.Refresh();
	//	m_GridCtrl.Invalidate();		
	}	

	//
	SetActiveCell();
	Invalidate();
}

void CIoViewTimeSale::SetRowValue(const CShowTick &Tick, int32 iRowIndex, int32 iColUintIndex, COLORREF clrRise, COLORREF clrFall, COLORREF clrKeep, COLORREF clrVolume)
{
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}
	
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
	}
	else
	{
		UnLockRedraw();
	}
	//
	if ( m_bShowFull )
	{
		if (iColUintIndex < 0 || iColUintIndex >= m_iGridFullColUints )
		{
			return;
		}
	}
	else
	{
		if (iRowIndex < 1 || iRowIndex >= m_GridCtrl.GetRowCount() || 0 != iColUintIndex )
		{
			return;
		}		
	}
	//
	COLORREF clrProperty = clrKeep;
	CString  StrFlag     = _T("");

	if (CTick::ETKBuy == Tick.m_eTradeKind)
	{
		clrProperty = clrFall;
		StrFlag     = "��";
	}
	else if (CTick::ETKSell == Tick.m_eTradeKind)
	{
		clrProperty = clrRise;
		StrFlag     = "��";
	}

	// 
	CGridCtrlSys* pGrid = NULL;
	
	if (m_bShowFull)
	{
		pGrid = &m_GridCtrlFull;
	}
	else
	{
		pGrid = &m_GridCtrl;
	}

	if ( NULL == pGrid )
	{
		return;
	}

	
	//  ���ӱ������ұ߿�ļ��
	int iColumn = pGrid->GetColumnCount();
	ASSERT(iColumn > 0);
	for (int32 i=0; i<pGrid->GetRowCount(); i++)
	{		
		CGridCellBase* pCell = m_GridCtrl.GetCell(i, 0);
		if (NULL != pCell)
		{
			pCell->SetTextPadding(CRect(5,0,0,0));
		}

		pCell = m_GridCtrl.GetCell(i, iColumn - 1);
		if (NULL != pCell)
		{
			pCell->SetTextPadding(CRect(0,0,5,0));
		}
	}

	LOGFONT fontNumber;
	memset(&fontNumber, 0, sizeof(fontNumber));
	_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontNumber.lfHeight  = -14;
	fontNumber.lfWeight = 560;
	//
	if (ETHTSimple == m_eTimeSaleHeadType)
	{
		int32 iColIndex = 0 + iColUintIndex * (NoShowNewvol()?2:3);
		bool32 bTimeChange = false;

		// ��1�� ʱ��
		{
			//
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			CString StrTime;
			CTime TimeCurrent(Tick.m_TimeCurrent.m_Time.GetTime());
			CTime TimePre(Tick.m_TimePrev.m_Time.GetTime());
			
			if ( TimeCurrent < 0 || TimePre < 0 )
			{
				return;
			}
			
			LOGFONT* pFontOld = pCell->GetFont();
			LOGFONT* pFontSmall = GetIoViewFont(ESFSmall);
			if ( NULL == pFontSmall )
			{
				pFontSmall = pFontOld;
			}
			
			// ��ʾ ʱ��, ʱ����ͬ,��ʾ��
			if (TimeCurrent.GetHour() == TimePre.GetHour() && TimeCurrent.GetMinute() == TimePre.GetMinute() )
			{							
				// pCell->SetFont(pFontSmall);						
				// StrTime.Format(L"      :%02d", TimeCurrent.GetSecond());										
				StrTime.Format(L"%02d:%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());					
			}
			else
			{
				StrTime.Format(L"%02d:%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());					
				
				//bTimeChange = true;
				
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}										
			}				

			//
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrTime);
			iColIndex++;
		}
		 
		// ��2�� �۸�
		{
			pGrid->SetCellType(iRowIndex, iColIndex, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)pGrid->GetCell(iRowIndex, iColIndex);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
			pCellSymbol->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());				
					}					
				}	
				else
				{
					pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());				
				}				
			}

			// ��ȡ�۸� �����ռ۱Ƚ��ж���������

			CString StrPrice = Float2SymbolString(Tick.m_fPrice, Tick.m_fPricePre, m_iSaveDec);
			if(StrPrice == L"-")
			{
				StrPrice = L"";
			}
			else
			{
				if(abs(Tick.m_fPrice - Tick.m_fPricePre) > 0.000001f)
				{
					StrPrice += StrFlag;
				}
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPrice);
			iColIndex++;
		}

		if(m_bShowFull)	// ȫ���������
		{
			// ��3�� ����
			if(!NoShowNewvol())
			{
				CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

				if ( bTimeChange )
				{
					if ( m_bShowFull )
					{
						if ( 1 != iRowIndex )
						{
							pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());						
						}					
					}	
					else
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());				
					}				
				}

				CString StrVolume = Float2String(Tick.m_fVolume, 0, true);		
				pCell->SetTextClr(clrProperty);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolume);

			}
		}
		else	// ��ȫ��ʱ
		{
			// ��3�� ����
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());						
					}					
				}	
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());				
				}				
			}

			CString StrVolume = Float2String(Tick.m_fVolume, 0, true);		
			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);

		}

	}
	else if (ETHTFuture == m_eTimeSaleHeadType)
	{
		bool32 bTimeChange = false;
		int32 iColIndex = 0 + iColUintIndex * 5;
		// ��1�� ʱ��
		{
			//
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			CString StrTime;
			CTime TimeCurrent(Tick.m_TimeCurrent.m_Time.GetTime());
			CTime TimePre(Tick.m_TimePrev.m_Time.GetTime());
			
			if ( TimeCurrent < 0 || TimePre < 0 )
			{
				return;
			}

			LOGFONT* pFontOld = pCell->GetFont();
			LOGFONT* pFontSmall = GetIoViewFont(ESFSmall);
			if ( NULL == pFontSmall )
			{
				pFontSmall = pFontOld;
			}
	
			// ��ʾ ʱ��, ʱ����ͬ,��ʾ��
			if (TimeCurrent.GetHour() == TimePre.GetHour() && TimeCurrent.GetMinute() == TimePre.GetMinute() )
			{				
				// pCell->SetFont(pFontSmall);		 			
				// StrTime.Format(L"      :%02d", TimeCurrent.GetSecond());										
				StrTime.Format(L"%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute());
			}
			else
			{
				//bTimeChange = true;
				StrTime.Format(L"%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute());
				
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}
			}				

			pCell->SetFont(&fontNumber);
			pCell->SetText(StrTime);
			iColIndex++;
		}
		
		// ��2�� �۸�
		{
			pGrid->SetCellType(iRowIndex, iColIndex, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)pGrid->GetCell(iRowIndex, iColIndex);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);		
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());
				}				
			}
			// ��ȡ�۸� �����ռ۱Ƚ��ж���������
			CString StrPrice = Float2SymbolString(Tick.m_fPrice, Tick.m_fPricePre, m_iSaveDec);
			if(StrPrice == L"-")
			{
				StrPrice = L"";
			}
			else
			{
				if(abs(Tick.m_fPrice - Tick.m_fPricePre) > 0.000001f)
				{
					StrPrice += L" " + StrFlag;
				}	
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPrice);
			iColIndex++;
		}
		
		// ��3�� ����
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}
			
			CString StrVolume = Float2String(Tick.m_fVolume, 0, true);
			if(StrVolume == L"-")
			{
				StrVolume = L" ";
			}
			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);
			iColIndex++;
		}
		
		// ��4�� �ֲ�
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}

			CString StrHoldAdd = Float2String(Tick.m_fHold - Tick.m_fHoldTotalPrev, 0, false, false);
			pCell->SetTextClr(clrVolume);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrHoldAdd);
			iColIndex++;
		}
		
		// ��5�� ����
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}

			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(Tick.m_StrProperty+L" ");
		}
	}
}

void CIoViewTimeSale::SetShowType(bool32 bShowFull)
{
	if(bShowFull)	// �����ж�һ��m_GridCtrl����m_GridCtrlFull���ѡ��
	{
		CRect RectGrid, RectMainWnd;
		m_GridCtrl.GetClientRect(&RectGrid);
		AfxGetMainWnd()->GetClientRect(&RectMainWnd);
	
		if(RectGrid.Width() < RectMainWnd.Width() / 2)	// ���б���С�������ʱ������ʾ����
		{
			bShowFull = false;
			//m_GridCtrl.SetColumnCount(NoShowNewvol()? 2 : 3);
		}
	}

	if ( m_bShowFull == bShowFull )
	{
		return;
	}

	m_bShowFull = bShowFull;
	
	if ( m_bShowFull )
	{
		m_GridCtrl.ShowWindow(SW_HIDE);
		m_XSBVert.ShowWindow(SW_HIDE);	
		m_GridCtrlFull.ShowWindow(SW_SHOW);
	}
	else
	{
		m_GridCtrl.ShowWindow(SW_SHOW);
		// m_XSBVert.ShowWindow(SW_SHOW);		
		m_GridCtrlFull.ShowWindow(SW_HIDE);
	}
}

void CIoViewTimeSale::SetActiveCell()
{
	//
	CGridCtrlSys* pGridCtrl = NULL;
	if ( m_bShowFull )
	{
		pGridCtrl = &m_GridCtrlFull;
	}
	else
	{
		pGridCtrl = &m_GridCtrl;
	}
	
	if ( NULL != pGridCtrl )
	{
		if ( pGridCtrl->GetRowCount() >= 1 && pGridCtrl->GetColumnCount() >= 1 )
		{
			if ( m_bShowFull )
			{
				// ����ģʽ�� 0,0
				CGridCellSys* pCell = (CGridCellSys*)pGridCtrl->GetCell(0, 0);
				if ( NULL != pCell )
				{
					pCell->SetParentActiveFlag(m_bActive);
					pCell->SetDrawParentActiveParams(0, 0, m_ColorActive);					
				}
			}
			else
			{
				// ���У�����
				// ����ÿ�����ݸ��¶����������룬so���ؿ�����ǰ�����
				CGridCellSys* pCell = (CGridCellSys*)pGridCtrl->GetCell(m_GridCtrl.GetRowCount()-1, 0);
				if ( NULL != pCell )
				{
					pCell->SetParentActiveFlag(m_bActive);
					pCell->SetDrawParentActiveParams(0, 0, m_ColorActive);					
				}
			}
		}
	}
}

void CIoViewTimeSale::UpDateFullGridNewestData()
{
	ASSERT( 0 );  //�������û�����ù�

	if ( !m_bShowFull || !m_bShowNewestFull )
	{
		// ���������ʾ,�������¼۲�����ʾ֮�е�ʱ��,ֱ�ӷ���:
		return;
	}

	if ( m_aTicksShow.GetSize() <= 0 )
	{
		return;
	}

	//
	if ( m_aTicksShow.GetSize() >= m_iDataFullNums )
	{
		// ������ʾ: ��������������
		OnPageScroll(INT_MAX);
	}
	else
	{
		// û����ʾ��: ����һ������ʾҳβ
		OnPageScroll(INT_MAX);
	}
	
	if ( !BeValidFullGridIndex() )
	{
		// ����ֵ����
		return;
	}

	// 
	RedrawWindow();
}

void CIoViewTimeSale::CalcFullGridParams()
{
	m_iPerfectWidth		= 0;	
	m_iGridFullRow		= 0;		
	m_iGridFullCol		= 0;		
	m_iDataFullNums		= 0;
	m_iGridFullColUints	= 0;
	//
	CClientDC dc(this);

	CRect RectGrid;
	m_GridCtrlFull.GetClientRect(&RectGrid);
		
	// �����и�
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont* pOldFont = dc.SelectObject(pFontNormal);
	CSize size = dc.GetTextExtent(L"123:456:789");

	if ( size.cy <= 0 )
	{
		return;
	}

	// ���ʱ��Ҫ����SetHead. �����Լ�������,�������Ա����
	E_TimeSaleHeadType eTimeSaleHeadType = m_eTimeSaleHeadType;

	if ( NULL != m_pMerchXml )
	{
		eTimeSaleHeadType = GetMerchTimeSaleHeadType(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);
	}	
	// 
	if ( ETHTSimple == eTimeSaleHeadType )
	{
		int iCol = NoShowNewvol()?2:3;
		m_iPerfectWidth		= iCol * size.cx;
		m_iGridFullColUints	= (RectGrid.Width() / m_iPerfectWidth);
		m_iGridFullCol		= m_iGridFullColUints * iCol; 
	}
	else if ( ETHTFuture == eTimeSaleHeadType )
	{
		m_iPerfectWidth		= 5 * size.cx;
		m_iGridFullColUints	= (RectGrid.Width() / m_iPerfectWidth);
		m_iGridFullCol		= m_iGridFullColUints * 5; 
	}

	//
	int32 iRowHeight0 = size.cy + 6;
	m_iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
	m_iGridFullRow	 += 1;

	// 
	m_iDataFullNums  = (m_iGridFullRow - 1) * m_iGridFullColUints; 
	
	//
	dc.SelectObject(pOldFont);

	if (m_iGridFullColUints <= 1)
	{
		SetShowType(false);
	}
	else
	{
		SetShowType(true);
	}

	//
	if ( m_iGridFullRow > 0 && m_iGridFullCol > 0 )
	{
		m_GridCtrlFull.SetRowCount(m_iGridFullRow);
		m_GridCtrlFull.SetColumnCount(m_iGridFullCol);

		m_GridCtrlFull.SetHeaderSort(FALSE);
		m_GridCtrlFull.SetFixedRowCount(1);		
	}	
}

bool32 CIoViewTimeSale::BeValidFullGridIndex()
{
	// ��������
	if ( m_iFullBeginIndex < 0 || m_iFullBeginIndex >= m_aTicksShow.GetSize() )
	{		
		return false;
	}
	
	if ( m_iFullEndIndex < 0 || m_iFullEndIndex >= m_aTicksShow.GetSize() )
	{
		return false;
	}
	
	if ( m_iFullBeginIndex > m_iFullEndIndex )
	{
		return false;
	}
	
	if ( m_aTicksShow.GetSize() >= m_iDataFullNums )
	{
		if ( (m_iFullEndIndex - m_iFullBeginIndex + 1) != m_iDataFullNums )
		{
			return false;
		}
	}

	return true;
}

void CIoViewTimeSale::SetRowHeightAccordingFont()
{
	CClientDC dc(this);
	
	// �����и�
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont* pOldFont = dc.SelectObject(pFontNormal);
	CSize size = dc.GetTextExtent(L"һ��");
	int32 iRowHeight0 = size.cy + 6;
	dc.SelectObject(pOldFont);

 	int32 iRowHeight = size.cy + 6; 
 
	int32 i = 0;
	for ( i = m_GridCtrl.GetRowCount() - 1; i > 0; i-- )
	{
		//
		m_GridCtrl.SetRowHeight(i, iRowHeight);
	}
	
	//
	for (i = m_GridCtrlFull.GetRowCount() - 1; i > 0; i-- )
	{
		//
		m_GridCtrlFull.SetRowHeight(i, iRowHeight);
	}
	
	if ( m_GridCtrl.GetRowCount() > 0 )
	{
		m_GridCtrl.SetRowHeight(0, iRowHeight0);
	}	

	if ( m_GridCtrlFull.GetRowCount() > 0 )
	{
		m_GridCtrlFull.SetRowHeight(0, iRowHeight0);
	}	
}

bool32 CIoViewTimeSale::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
		return false;
	
	// ��ȡ��ҵ����ͼ���е�����
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);
	SetRowHeightAccordingFont();

	//
	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = pcAttrValue;
	}

	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}
	
	// ��Ʒ�����ı�
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	return true;
}

CString CIoViewTimeSale::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(),
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		StrMarketId.GetBuffer());
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewTimeSale::GetDefaultXML()
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
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewTimeSale::DeleteTableContent()
{
	if ( NULL != m_GridCtrl.GetSafeHwnd() && !m_bShowFull )
	{
		m_GridCtrl.DeleteNonFixedRows();		
	}		

	if ( NULL != m_GridCtrlFull.GetSafeHwnd() && m_bShowFull )
	{
		m_GridCtrlFull.DeleteNonFixedRows();
	}

	SetRowHeightAccordingFont();
	RedrawWindow();
}

void CIoViewTimeSale::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	//TRACE(_T("active :%d\r\n"), m_bActive);
	//
	SetActiveCell();
	if ( m_bShowFull )
	{
		if ( m_GridCtrlFull.GetSafeHwnd() )
		{
			m_GridCtrlFull.RedrawWindow();
		}
	}
	else
	{
		if ( m_GridCtrl.GetSafeHwnd() )
		{
			m_GridCtrl.RedrawWindow();
		}
	}

	//
	if ( NULL != m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.SetFocus();
	}
	
	SetChildFrameTitle();
}

void CIoViewTimeSale::OnIoViewDeactive()
{
	m_bActive = false;

	//
	SetActiveCell();

	if ( m_bShowFull )
	{
		if ( m_GridCtrlFull.GetSafeHwnd() )
		{
			m_GridCtrlFull.RedrawWindow();
		}
	}
	else
	{
		if ( m_GridCtrl.GetSafeHwnd() )
		{
			m_GridCtrl.RedrawWindow();
		}
	}
}

void  CIoViewTimeSale::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}


int32 CIoViewTimeSale::CalcMaxVisibleTimeSaleCount()
{
	if ( !IsWindow(m_GridCtrl.GetSafeHwnd()) || !IsWindow(m_GridCtrlFull.GetSafeHwnd()) )
	{
		return 0;
	}
	
	if ( m_GridCtrl.IsWindowVisible() )
	{
		// ��ͨģʽ
		CClientDC dc(this);
		
		CRect RectGrid;
		m_GridCtrl.GetClientRect(&RectGrid);
		
		// �����и�
		CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
		CFont* pOldFont = dc.SelectObject(pFontNormal);
		CSize size = dc.GetTextExtent(L"���");
		
		if ( size.cy <= 0 )
		{
			return 0;
		}
		
		//
		int32 iGridFullRow;
		int32 iRowHeight0 = size.cy + 6;
		iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
		iGridFullRow	 += 1;
		
		// 
		//
		dc.SelectObject(pOldFont);
		
		return iGridFullRow;
		//return KNormalShowTimeSaleCount;
	}
	else
	{
		// FullShow
		CClientDC dc(this);
		
		CRect RectGrid;
		m_GridCtrlFull.GetClientRect(&RectGrid);
		
		// �����и�
		CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
		CFont* pOldFont = dc.SelectObject(pFontNormal);
		CSize size = dc.GetTextExtent(L"һ������");
		
		if ( size.cy <= 0 )
		{
			return 0;
		}
		
		// ���ʱ��δ֪. �����Լ�������,�������Ա����
		E_TimeSaleHeadType eTimeSaleHeadType = m_eTimeSaleHeadType;
		
		if ( NULL != m_pMerchXml )
		{
			eTimeSaleHeadType = GetMerchTimeSaleHeadType(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);
		}	
		
		int32 iPerfectWidth,  iGridFullCol;
		int32 iGridFullColUnits = 0;
		int32 iGridFullRow, iDataFullNums;
		//
		if ( ETHTSimple == eTimeSaleHeadType )
		{
			int iCol = NoShowNewvol()? 2:3;
			iPerfectWidth		= iCol * size.cx;
			iGridFullColUnits	= (RectGrid.Width() / iPerfectWidth);
			iGridFullCol		= iGridFullColUnits * iCol; 
		}
		else if ( ETHTFuture == eTimeSaleHeadType )
		{
			iPerfectWidth		= 5 * size.cx;
			iGridFullColUnits	= (RectGrid.Width() / iPerfectWidth);
			iGridFullCol		= iGridFullColUnits * 5; 
		}
		
		//
		int32 iRowHeight0 = size.cy + 6;
		iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
		iGridFullRow	 += 1;
		
		// 
		iDataFullNums  = (iGridFullRow - 1) * iGridFullColUnits; 
		//
		dc.SelectObject(pOldFont);
		
		return iDataFullNums;
	}
	
	return 0;
}

void CIoViewTimeSale::OnPageScroll( int32 iScrollPage )
{
	// �����0�򣬿����������Ƿ��㣬����������һ��
	// ����� KForceRequestTimeSalePage ��ǿ�����뵱ǰ������������
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	if ( !m_bShowFull )
	{
		// ��ͨ ����Ϊ����
		// ������������ǰ�Ĵ�������Լ��л�
	}
	else
	{
		//if ( !m_GridCtrlFull.IsWindowVisible() || !IsWindowVisible() )
		//{
		//	ASSERT( 0 );  // ��Ӧ�ó��ֵģ������������У��ܱ���
		//}

		// Full ����Ϊ����
		CGmtTime TimeInit	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeEnd	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;//TimeInit + CGmtTimeSpan(0, 23, 59, 59);
		
		int32 iMaxVisibleDataCount = m_iDataFullNums;
		if ( iMaxVisibleDataCount < 1 )
		{
			// ������ʱ��û�еõ���ʼ���Ļ�������һ�� - Ӧ����size��ŵ��õ�
			ASSERT( 0 );
			iMaxVisibleDataCount = CalcMaxVisibleTimeSaleCount();
		}

		int32 iRequestDataCount = iMaxVisibleDataCount;//MAX(iMaxVisibleDataCount, KMinRequestTimeSaleCount);
		if ( iRequestDataCount <= 0 )
		{
			ASSERT( 0 );
			iMaxVisibleDataCount = 1;
			iRequestDataCount = KMinRequestTimeSaleCount;
		}

		// ����ԭ������merch��Tick�����л��棬������UpdateTimeSale�����ȫ���Ļ������ݹ�����������ǲ�����������������Ļ�
		//		���ݾͻ������·�ҳ��ʱ����ֶϲ㣬�����ж��Ƿ���Ҫ��������ʧ�ܣ�����Ҳ�����ж��Ƿ񷭵�������ҳ
		//      ����Ҫ�������µ����ݣ���Ϊÿ��MerchChangeʱ��������һ�����µ����ݣ���ǰ�����е����ݶ�����ǰ��ʱ����
		if ( KForceRequestTimeSalePage == iScrollPage )
		{
			// ǿ���������� - ���ı���ʾ����
			int iTickSize = m_aTicksShow.GetSize();
			iRequestDataCount = MAX(iRequestDataCount, iTickSize);
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYFrontCount;
			info.m_TimeSpecify	= TimeEnd;
			info.m_iFrontCount  = iRequestDataCount;
			
			RequestTimeSaleData(info);
			return; // �ȴ����ݻ�����Update�������SetTable
		}
		else if ( INT_MIN == iScrollPage )
		{
			// ҳ��
			// �������� ���� - �п����������ݶ�����һҳ��ʾ��������ҪSetTableData��֮�и�������Newest��־
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYSpecifyTime;
			info.m_TimeStart	= TimeInit;
			info.m_TimeEnd      = TimeEnd;
			
			RequestTimeSaleData(info);
			
			m_iFullBeginIndex = 0;
			m_iFullEndIndex   = m_iFullBeginIndex + iMaxVisibleDataCount - 1;
			m_bShowNewestFull = false;
			SetTableData();
			return;
		}
		else if ( INT_MAX == iScrollPage || m_aTicksShow.GetSize() < iRequestDataCount )
		{
			// ҳβ(���ݲ���Ҳ��ҳβ��)
			if ( m_aTicksShow.GetSize() < iRequestDataCount )
			{
				// ���ݲ��㣬����ҳβ n ������
				CMmiReqMerchTimeSales info;
				info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
				info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
				
				info.m_eReqTimeType	= ERTYFrontCount;
				info.m_TimeSpecify	= TimeEnd;
				info.m_iFrontCount  = iRequestDataCount;
				
				RequestTimeSaleData(info);
			}
			
			//m_iFullEndIndex = ??
			m_bShowNewestFull = true;
			// ����ֵ��SetTableData�и�������
			m_iFullBeginIndex = -1;
			m_iFullEndIndex   = -1;
			SetTableData();
			return;
		}
		else
		{
			// �����ڵ�λ�ÿ�ʼ��
			// ���ڼ��豾�������������ݣ������£���
			// 0 ���� - ��������������m_iFullEndIndexΪ��������

			int32 iStartPos = m_iFullBeginIndex + iScrollPage * iMaxVisibleDataCount;
			int32 iEndPos   = iStartPos + iMaxVisibleDataCount - 1;

			if ( 0 == iScrollPage )
			{
				// 0 - �������� m_iFullEndIndex Ϊ��������
				iEndPos = m_iFullEndIndex;
				iStartPos = iEndPos - iMaxVisibleDataCount + 1;
			}
			
			if ( iEndPos > m_aTicksShow.GetSize()-1 )
			{
				// β�����ݲ��� - ��Ϊ�Ѿ��������µ����ݣ�����ֻ��������ʾ
				iEndPos = m_aTicksShow.GetSize() - 1;
				iStartPos = iEndPos - iMaxVisibleDataCount + 1;
			}
			else if ( iStartPos < 0 )
			{
				// ͷ�����ݲ��� - ������ʱ������㣬��ǰ���� �������� + �������� �� - ��Ϊ�ֱʵ�ʱ�侫ȷ����������

				int32 iNeedDataCount = -iStartPos;
				iStartPos += iNeedDataCount;		// 0
				iEndPos	  += iNeedDataCount;        // iMaxVisibleDataCount;
				
				if ( !m_bRequestedTodayFirstData )
				{
					// TRACE(_T("---------TimeSale request header data\r\n"));

					CMmiReqMerchTimeSales info;
					info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
					info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
					
					info.m_eReqTimeType	= ERTYFrontCount;
					info.m_TimeSpecify	= TimeEnd;
					info.m_iFrontCount  = iRequestDataCount + m_aTicksShow.GetSize();
					
					RequestTimeSaleData(info);
				}
				else
				{
					// TRACE(_T("---------TimeSale DONOT request header data\r\n"));
					// �������ǰ�����Ѿ�������ˣ�����Ҫ�������ˣ�����һ����ʾ����Ϳ�����
				}
			}
			else
			{
				// Ӧ�������������������
			}

			if ( iEndPos > m_aTicksShow.GetSize()-1 )
			{
				iEndPos = m_aTicksShow.GetSize() - 1;
			}
			m_iFullBeginIndex = iStartPos;
			m_iFullEndIndex   = iEndPos;
			if ( iStartPos <= m_aTicksShow.GetSize() && iEndPos >= m_aTicksShow.GetSize()-1 )
			{
				m_bShowNewestFull = true;
			}
			else
			{
				m_bShowNewestFull = false;
			}

			SetTableData();
		}
	}
}

void CIoViewTimeSale::RequestViewData()
{
	if (NULL == m_pMerchXml)
		return;

// 	if ( !IsWindowVisible() )
// 	{
// 		//TRACE(_T("---------Ignore Invisible request!!\r\n"));
// 		return;
// 	}
	
	CGmtTime TimeInit	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
	CGmtTime TimeEnd	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
	
	CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
	if (!m_bHistoryTimeSale && (TimeServer < TimeInit || TimeServer > TimeEnd))	// ����ͬһ�죬 ��Ҫ������ʱ������ռ۵�
	{
		// 
		m_iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		
		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo))
		{
			m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

			// �ж����������Ƿ����ڻ�õ��ٽ��������ڽ���ʱ�ε����ݣ����ǣ�����ո�����
			if ( m_aTicksShow.GetSize() > 0 )
			{
				// �ֱʵ�ʱ��Ӧ�����ڵ����г��ĳ�ʼ��ʱ�� ����С�� ��Endʱ��
				if ( RecentTradingDay.m_TimeInit >  m_aTicksShow[0].m_TimeCurrent || RecentTradingDay.m_TimeEnd < m_aTicksShow[0].m_TimeCurrent )
				{
					// ������ʱ������������� ���� ������������
					// �������ǰ��ʾ����
					DeleteTableContent();
					
					// �����ǰ����
					m_aTicksShow.SetSize(0);
				}
			}
		}
	}
	
	// ����ָ��ʱ�����K�ߣ� �Ա��ȡ��ǰ��ʾ����ͼ�����ռۣ� �񿪼�
	{
		ASSERT(m_TrendTradingDayInfo.m_bInit);
		
		// 
		CGmtTime TimeDay = m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);
		
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;		// Ϊ��ֹ���죬��������&����
		info.m_TimeStart		= TimeDay;
		//info.m_TimeEnd		= TimeDay;
		info.m_iFrontCount		= 2;			
		DoRequestViewData(info);
	}
	
	// ��RealtimePrice����, Ϊȡ���ռ�
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
	// �������Ʒ������
	{
		if ( m_bShowFull )
		{
			// �ж�һ���Ƿ���Ҫ�������� xl 0601 ǿ������������ʷ����
			OnPageScroll(KForceRequestTimeSalePage);
		}
		else
		{
			// ��ͨ����£���������100������
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYFrontCount;
			info.m_TimeSpecify	= TimeEnd;
			info.m_iFrontCount  = KNormalShowTimeSaleCount;
			
			DoRequestViewData(info);
		}
		
	}
	
	// ��RealtimeTick����
	{
		CMmiReqRealtimeTick Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
}

void CIoViewTimeSale::RequestTimeSaleData( CMmiReqMerchTimeSales &req )
{
	DoRequestViewData(req);
}

void CIoViewTimeSale::SetHistoryTime( const CGmtTime &TimeAttend )
{
	m_bHistoryTimeSale	= true;
	m_TimeHistory	= TimeAttend;
	CMarketIOCTimeInfo RecentTradingDay;
	if ( NULL != m_pMerchXml 
		&& m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo)
		)
	{
		m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
		m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
		m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

		if ( m_GridCtrl.m_hWnd != NULL )
		{
			m_GridCtrl.EnsureVisible(0, 0);		// ���ܱ�֤���ǵ�һ���ɼ�
			DeleteTableContent();		// �����������
		}
	}
	// ������ʱ����ֻʣ��merchchangedʱ��
}

bool32 CIoViewTimeSale::GetCurrentPerfectWidth( OUT int32 &iWidth )
{
	if ( m_iPerfectWidth != 0 )
	{
		iWidth = m_iPerfectWidth;
		return true;
	}
	// �Ƿ��Լ��㣿��
	return false;
}

void TestRecordData(CArray<CTick, CTick>& aTick)
{
	return;
	static long lIndex = 0;
	
	FILE* pFile = NULL;
	if ( lIndex == 0 )
	{
		pFile = fopen("c:\\Tick.txt", "w");
	}
	else
	{
		pFile = fopen("c:\\Tick.txt", "aw+");
	}
	
	if ( NULL == pFile )
	{
		return;			
	}
	
	int32 iMax = 1000;
	
	fprintf(pFile, "================== beging[%d] ================== \r\n", lIndex);
	//
	for ( int32 i = aTick.GetSize() - 1; i >= 0; i--)
	{
		CTick Tick = aTick[i];	
		
		CString StrLog = L"[%05d]ʱ��: [%ld]%02d:%02d �۸�: %.2f ����: %.0f \r\n";
		//StrLog.Format(L"[%05d]ʱ��: [%ld]%02d:%02d �۸�: %.2f ����: %.0f \r\n", i, Tick.m_TimeCurrent.m_Time.GetTime(), Tick.m_TimeCurrent.m_Time.GetMinute(), Tick.m_TimeCurrent.m_Time.GetSecond(), Tick.m_fPrice, Tick.m_fVolume);
		//
		char* pStrLog = NULL;
		USES_CONVERSION;
		//
		pStrLog = _W2A(StrLog);
		
		fprintf(pFile, pStrLog, i, Tick.m_TimeCurrent.m_Time.GetTime(), Tick.m_TimeCurrent.m_Time.GetMinute(), Tick.m_TimeCurrent.m_Time.GetSecond(), Tick.m_fPrice, Tick.m_fVolume);

		iMax--;
		if ( iMax <= 0 )
		{
			break;
		}
	}
	
	fprintf(pFile, "================== end[%d] ================== \r\n", lIndex);	
	fclose(pFile);

	lIndex++;
}

void CIoViewTimeSale::DoShowStdPopupMenu()
{
	CNewMenu Menu;
	Menu.LoadMenu(IDR_MENU_TIMESALE);
	Menu.LoadToolBar(g_awToolBarIconIDs);
	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));

	pPopMenu->EnableMenuItem(ID_EXPORT_DATA,true);
	CMenu* pTempMenu = pPopMenu->GetSubMenu(L"��������");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	CIoViewBase::AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	CPoint pt;
	GetCursorPos(&pt);
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
	pPopMenu->DestroyMenu();	
}

void CIoViewTimeSale::OnExportData()
{
	if (m_pMerchXml != NULL)
	{
		CDialogExportSaleData dlg;
		dlg.SetMerchInfo(m_pMerchXml->m_MerchInfo);
		dlg.DoModal();
	}
}

bool32 CIoViewTimeSale::NoShowNewvol()
{
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);
	if ((ERTMony == eMerchKind) && 0 == CConfigInfo::Instance()->m_bShowNewvol)	// �ж��Ƿ��ǡ������̡�
	{
		return true;
	}
	return false;
}

void CIoViewTimeSale::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KTimerIdRefresh )
	{
		KillTimer(KTimerIdRefresh);
		SetTableData();
		m_bSetTimer = FALSE;
	}
	CIoViewBase::OnTimer(nIDEvent);
}
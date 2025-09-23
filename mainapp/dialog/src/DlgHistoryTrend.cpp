#include "StdAfx.h"

#include "DlgHistoryTrend.h"

#include "IoViewKLine.h"
#include "MerchManager.h"
#include "NewMenu.h"

#include "IoViewTrend.h"
#include "IoViewTimeSale.h"



#include "facescheme.h"

#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
const UINT KIdTrend = 0x105;
const UINT KIdTimeSale = 0x106;

const int32 KIAxisWidth = 2;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDlgHistoryTrend, CDialogEx)


CDlgHistoryTrend::CDlgHistoryTrend( CWnd *pParent /*= NULL*/ )
:BaseDailog(IDD, pParent)
{
	m_pAttendMerch = NULL;
	m_pIoViewTimeSale = NULL;
	m_pIoViewTrend = NULL;
	m_sizeTimeSale = CSize(0, 0);

	m_bAttendTimeChanged = false;
	m_ptLastDrawSplit = 0;
	m_RectAxis.SetRectEmpty();
}


CDlgHistoryTrend::~CDlgHistoryTrend()
{
	delete m_pIoViewTimeSale;
	//m_pIoViewTimeSame = NULL;
	delete m_pIoViewTrend;
	//m_pIoViewTrend = NULL;
}


void CDlgHistoryTrend::DoDataExchange(CDataExchange* pDX)
{
	BaseDailog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHistoryTrend)
	//}}AFX_DATA_MAP
}

BOOL CDlgHistoryTrend::OnInitDialog()
{
	BaseDailog::OnInitDialog();
	
	ASSERT( NULL == m_pIoViewTrend );
	ASSERT( NULL == m_pIoViewTimeSale );
	ASSERT( NULL != m_pAttendMerch );

	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );

	m_pIoViewTimeSale = new CIoViewTimeSale();
	m_pIoViewTimeSale->SetCenterManager(pAbsCenterManager);
	m_pIoViewTimeSale->Create(SS_NOTIFY |WS_CHILD |WS_VISIBLE |WS_TABSTOP, this, CRect(0,0,0,0), KIdTimeSale);
	m_pIoViewTimeSale->SetHistoryFlag(true);
	m_pIoViewTimeSale->SetHistoryTime(m_TimeHistory);

	m_pIoViewTrend = new CIoViewTrend();
	m_pIoViewTrend->SetCenterManager(pAbsCenterManager);
	m_pIoViewTrend->Create(SS_NOTIFY |WS_CHILD |WS_VISIBLE |WS_TABSTOP, this, CRect(0,0,0,0), KIdTrend);
	m_pIoViewTrend->SetHistoryFlag(true);
	m_pIoViewTrend->SetHistoryTime(m_TimeHistory);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnViewMerchChanged(m_pIoViewTimeSale, m_pAttendMerch);	// 商品变更并请求数据, group应当是-1
	pFrame->OnViewMerchChanged(m_pIoViewTrend, m_pAttendMerch);

	m_pIoViewTrend->SetFocus();

	ModifyStyle(WS_MINIMIZEBOX, WS_MAXIMIZEBOX);
	SetWindowText(m_pIoViewTrend->GetHistroyTitle(m_TimeHistory));
	MoveWindow(CRect(0, 0, 700, 500));
	CenterWindow();
	RecalcLayout();
	//
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgHistoryTrend, CDialogEx)
//{{AFX_MSG_MAP(CDlgHistoryTrend)
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
ON_WM_SETCURSOR()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgHistoryTrend::OnSize( UINT nType, int cx, int cy )
{
	BaseDailog::OnSize(nType, cx, cy);

	RecalcLayout();

	Invalidate(TRUE);		// dlg 和 static都没有CS_HREDRAW...
}

void CDlgHistoryTrend::RecalcLayout()
{
	// oninitialdlg 时机晚于此
// 	ASSERT( NULL != m_pIoViewTrend );
// 	ASSERT( NULL != m_pIoViewTimeSame );
	m_RectAxis.SetRectEmpty();
	if ( NULL != m_pIoViewTimeSale && NULL != m_pIoViewTrend )
	{
		CRect rcClient(0,0,0,0);
		GetClientRect(rcClient);
		// 双击最大化分笔判定
		if ( m_pIoViewTrend->IsWindowVisible() || !m_pIoViewTimeSale->IsWindowVisible() )	// initdialog 两者都是隐藏的
		{
			// timesale固定宽度，其它的留给trend
			if ( m_sizeTimeSale.cx == 0 )
			{
				int32 iTSWidth = 0;
				if ( !m_pIoViewTimeSale->GetCurrentPerfectWidth(iTSWidth) )
				{
					iTSWidth = 200;
				}
				m_sizeTimeSale.cx = iTSWidth;
			}
			CRect rcTM(rcClient);
			rcTM.left = rcClient.right - m_sizeTimeSale.cx;
			if ( rcTM.left < rcClient.left )
			{
				rcTM.left = rcClient.left;
			}
			m_pIoViewTimeSale->MoveWindow(rcTM);
			CRect rcTrend(rcClient);
			
			m_RectAxis = rcTM;
			m_RectAxis.left = rcTM.left - KIAxisWidth;
			m_RectAxis.right = rcTM.left;
			
			rcTrend.right = m_RectAxis.left;
			m_pIoViewTrend->MoveWindow(rcTrend);
		}
		else
		{
			// 分笔最大化
			m_pIoViewTimeSale->MoveWindow(rcClient);
		}
	}
}

void CDlgHistoryTrend::PostNcDestroy()
{
	// 此时应当删除两个业务视图 - 因为现在已经不能响应数据了
	delete m_pIoViewTimeSale;
	m_pIoViewTimeSale = NULL;
	delete m_pIoViewTrend;
	m_pIoViewTrend = NULL;

	BaseDailog::PostNcDestroy();
}

void CDlgHistoryTrend::OnDestroy()
{
	BaseDailog::OnDestroy();
}

void CDlgHistoryTrend::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
	rcClient.right = rcClient.right - m_sizeTimeSale.cx;
	rcClient.left = rcClient.right - KIAxisWidth;
	dc.FillSolidRect(rcClient, CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine));
}

BOOL CDlgHistoryTrend::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

int CDlgHistoryTrend::ShowHistoryTrend( CIoViewKLine *pIoViewKLine, const CKLine &KLineSrc )
{
	CMerch *pMerchAttend = NULL;
	if ( NULL != pIoViewKLine && (pMerchAttend=pIoViewKLine->GetMerchXml()) != NULL && KLineSrc.m_TimeCurrent.GetYear() > 1900 )
	{
		CDlgHistoryTrend dlg(AfxGetMainWnd());
		dlg.m_pAttendMerch = pMerchAttend;
		dlg.m_TimeHistory = KLineSrc.m_TimeCurrent;
		
		return dlg.DoModal();
	}
	return IDCANCEL;
}

BOOL CDlgHistoryTrend::PreTranslateMessage( MSG* pMsg )
{
	// 还是使用page up + down 吧
	// 时间轴跳动，根据商品现有日k线数据滚动
	
	if ( WM_KEYDOWN == pMsg->message && NULL != m_pAttendMerch )
	{
		int32 iAdjust = 0;
		switch ( pMsg->wParam )
		{
		case VK_PRIOR:
			iAdjust = -1;
			break;
		case VK_NEXT:
			iAdjust = +1;
			break;
		}

		if ( iAdjust != 0 )
		{
			int32 iPosFound;
			CMerchKLineNode* pKLineRequest = NULL;
			m_pAttendMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineRequest);
			if ( NULL != pKLineRequest )
			{
				int32 iKLinePos = CMerchKLineNode::QuickFindKLineByTime(pKLineRequest->m_KLines, m_TimeHistory);
				if ( iKLinePos >= 0 && iKLinePos < pKLineRequest->m_KLines.GetSize() )
				{
					int32 iKSize = pKLineRequest->m_KLines.GetSize()-1;
					int32 iKLineNewPos = iKLinePos + iAdjust;
					iKLineNewPos = max(iKLineNewPos, 0);
					iKLineNewPos = min(iKLineNewPos, iKSize);
					if ( iKLineNewPos != iKLinePos )
					{
						ChangeAttendTimeStart( pKLineRequest->m_KLines[iKLineNewPos].m_TimeCurrent );
						m_bAttendTimeChanged = true;
						return TRUE;
					}
				}
				else
				{
					ASSERT( 0 );	// 不应该找不到
				}
			}

			return TRUE;	// 拦截该消息
		}
	}
	if ( WM_KEYUP == pMsg->message && NULL != m_pAttendMerch )
	{
		switch ( pMsg->wParam )
		{
		case VK_PRIOR:
		case VK_NEXT:
			if ( m_bAttendTimeChanged )
			{
				m_bAttendTimeChanged = false;
				ChangeAttendTimeEnd();
			}
			return TRUE;
		}
	}

	bool32 bCaptureMsg = false;
	if ( WM_LBUTTONDOWN == pMsg->message )
	{
		CWnd *pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);
		if ( pWnd != NULL )
		{
			CPoint	ptDown(0,0);
			ptDown = GetMessagePos();
			//pWnd->ClientToScreen(&ptDown);
			ScreenToClient(&ptDown);
			bCaptureMsg = DoLButtonDown(ptDown);
			if ( !bCaptureMsg )
			{
				pWnd->SetFocus();	// Trend的focus会被分笔抢去，为什么呢？？
			}
		}
	}
	else if ( WM_LBUTTONUP == pMsg->message )
	{
		CWnd *pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);
		if ( pWnd == this ) // 仅接收自己的
		{
			CPoint	ptUp(0,0);
			ptUp = GetMessagePos();
			//pWnd->ClientToScreen(&ptUp);
			ScreenToClient(&ptUp);
			bCaptureMsg = DoLButtonUp(ptUp);
		}
	}
	else if ( WM_MOUSEMOVE == pMsg->message )
	{
		//if ( GetCapture() == this )
		{
			CWnd *pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);
			if ( pWnd != NULL )
			{
				CPoint	ptUp(0,0);
				ptUp = GetMessagePos();
				//pWnd->ClientToScreen(&ptUp);
				ScreenToClient(&ptUp);
				bCaptureMsg = DoMouseMove(ptUp);
			}
		}
	}
	else if ( WM_LBUTTONDBLCLK == pMsg->message )
	{
		if ((m_pIoViewTrend!=NULL) && (m_pIoViewTimeSale!=NULL))
		{
			CWnd *pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);
			if ( pWnd == m_pIoViewTimeSale || m_pIoViewTimeSale->IsChild(pWnd) )	// 双击最大
			{
				if ( m_pIoViewTrend->IsWindowVisible() )
				{
					m_pIoViewTrend->ShowWindow(SW_HIDE);
					RecalcLayout();
					Invalidate();
				}
				else
				{
					m_pIoViewTrend->ShowWindow(SW_SHOW);
					RecalcLayout();
					Invalidate();
				}
				bCaptureMsg = true;
			}
		}
	}
	if ( bCaptureMsg )
	{
		return TRUE;
	}
	return BaseDailog::PreTranslateMessage(pMsg);
}

void CDlgHistoryTrend::ChangeAttendTimeStart( const CGmtTime &TimeAttend )
{
	m_TimeHistory = TimeAttend;
	if ( NULL != m_pIoViewTrend )
	{
		m_pIoViewTrend->SetHistoryTime(TimeAttend);
		//m_pIoViewTrend->RequestViewData();
		SetWindowText( m_pIoViewTrend->GetHistroyTitle(m_TimeHistory) );
	}

	if ( NULL != m_pIoViewTimeSale )
	{
		m_pIoViewTimeSale->SetHistoryTime(TimeAttend);
		//m_pIoViewTimeSale->RequestViewData();
	}
}

void CDlgHistoryTrend::ChangeAttendTimeEnd()
{
	if ( NULL != m_pIoViewTrend )
	{
		m_pIoViewTrend->RequestViewData();
	}
	
	if ( NULL != m_pIoViewTimeSale )
	{
		m_pIoViewTimeSale->RequestViewData();
	}
}

bool32 CDlgHistoryTrend::DoLButtonDown( CPoint pt )
{
	if ( NULL != m_pIoViewTimeSale )
	{
		CRect rcTS(m_RectAxis);
		rcTS.InflateRect(1, 0);
		if ( rcTS.PtInRect(pt) )
		{
			SetCapture();
			
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			m_ptLastDrawSplit = CPoint(-1, -1);
			return true;
		}
	}
	return false;
}

bool32 CDlgHistoryTrend::DoLButtonUp( CPoint pt )
{
	if ( GetCapture() == this )
	{
		ReleaseCapture();
		SetCursor(NULL);

		CRect rcClient(0,0,0,0);
		GetClientRect(rcClient);
		m_sizeTimeSale.cx = max(rcClient.right - pt.x, 1);

		RecalcLayout();

		Invalidate(TRUE);
		return true;
	}
	return false;
}

bool32 CDlgHistoryTrend::DoMouseMove( CPoint pt )
{
	if ( GetCapture() == this )
	{
		CRect rcClient(0,0,0,0);
		GetClientRect(rcClient);
		CClientDC dc(this);
		int32 iSaveDC = dc.SaveDC();
		CPen pen;
		pen.CreatePen(PS_SOLID, KIAxisWidth, RGB(255,255,255));
		dc.SelectObject(&pen);
		dc.SetROP2(R2_NOT);
		if ( m_ptLastDrawSplit.x != -1 )
		{
			dc.MoveTo( m_ptLastDrawSplit.x, rcClient.top );
			dc.LineTo( m_ptLastDrawSplit.x, rcClient.bottom );
		}

		m_ptLastDrawSplit = pt;
		dc.MoveTo( m_ptLastDrawSplit.x, rcClient.top );
		dc.LineTo( m_ptLastDrawSplit.x, rcClient.bottom );

		dc.RestoreDC(iSaveDC);

		return true;
	}
	else
	{
		m_ptLastDrawSplit = CPoint(-1, -1);
		CRect rcTS(m_RectAxis);
		rcTS.InflateRect(1, 0);
		if ( rcTS.PtInRect(pt) )
		{
			m_ptLastDrawSplit = pt;
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
	}
	return false;
}

BOOL CDlgHistoryTrend::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	if ( GetCapture() == this || m_ptLastDrawSplit.x != -1 )
	{
		return TRUE;
	}
	else
	{
		
	}
	return BaseDailog::OnSetCursor(pWnd, nHitTest, message);
}

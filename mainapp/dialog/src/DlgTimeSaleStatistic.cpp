#include "StdAfx.h"
#include "DlgTimeSaleStatistic.h"
#include "MerchManager.h"
#include "NewMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTimeSaleStatistic *CDlgTimeSaleStatistic::m_spThis = NULL;

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTabWndCtrl, CTabCtrl)

BEGIN_MESSAGE_MAP(CTabWndCtrl, CTabCtrl)
//{{AFX_MSG_MAP(CTabWndCtrl)
ON_WM_SIZE()
ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabWndCtrl::OnSelChange( NMHDR *pHdr, LRESULT *pResult )
{
	int iSel = GetCurSel();
	CWnd *pWndSel = NULL;
	for ( int i=0; i < GetItemCount() && iSel != -1 ; i++ )
	{
		TCITEM item = {0};
		item.mask = TCIF_PARAM;
		if ( GetItem(i, &item) )
		{
			CWnd *pWnd = (CWnd *)item.lParam;
			if ( IsWindow(pWnd->GetSafeHwnd()) && i != iSel )
			{
				pWnd->ShowWindow(SW_HIDE);
			}
			else if ( i == iSel )
			{
				pWndSel = pWnd;
			}
		}
	}

	if (pWndSel != NULL)
	{
		if ( IsWindow(pWndSel->GetSafeHwnd()) )
		{
			pWndSel->ShowWindow(SW_SHOW);
		}
	}

	return FALSE;	// parent will receive notify
}

void CTabWndCtrl::OnSize( UINT nType, int cx, int cy )
{
	CTabCtrl::OnSize(nType, cx, cy);
	RecalcLayout();
}

void CTabWndCtrl::RecalcLayout(CRect rcInflate/*=CRect(0,0,0,0)*/)
{
	CRect rc(0,0,0,0), rcItem(0,0,0,0);
	GetClientRect(rc);
	if ( GetItemCount() > 0 )
	{
		GetItemRect(0, rcItem);
		CRect rcChild = rc;
		rcChild.top = rcItem.bottom + 1;

		rcChild.InflateRect(rcInflate);

		for ( int i=0; i < GetItemCount() ; i++ )
		{
			TCITEM item = {0};
			item.mask = TCIF_PARAM;
			if ( GetItem(i, &item) )
			{
				CWnd *pWnd = (CWnd *)item.lParam;
				if ( IsWindow(pWnd->GetSafeHwnd()) )
				{
					if ( pWnd->GetParent() != this )
					{
						CRect rcTmp(rcChild);
						MapWindowPoints(pWnd->GetParent(), &rcTmp);
						pWnd->MoveWindow(rcTmp);
					}
					else
					{
						pWnd->MoveWindow(rcChild);
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDlgTimeSaleStatistic, CDialog)

CDlgTimeSaleStatistic::CDlgTimeSaleStatistic( CWnd *pParent /*= NULL*/ )
:CDialog(IDD, pParent)
{
	
}

void CDlgTimeSaleStatistic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTimeSaleStatistic)
	DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
	//}}AFX_DATA_MAP
}

BOOL CDlgTimeSaleStatistic::OnInitDialog()
{
	CDialog::OnInitDialog();

	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc )
	{
		return FALSE;
	}

	m_wndTimeSale.SetCenterManager(pDoc->m_pAbsCenterManager);
	m_wndRank.SetCenterManager(pDoc->m_pAbsCenterManager);
	
	m_wndTimeSale.Create(WS_VISIBLE |WS_CHILD|SS_NOTIFY, &m_ctrlTab, CRect(0,0,0,0), 0x203);

	m_wndRank.Create(WS_CHILD |SS_NOTIFY, &m_ctrlTab, CRect(0,0,0,0), 0x204);

	CRect rcClient;
	GetWindowRect(rcClient);
	//m_wndTimeSale.MoveWindow(rcClient);

	m_ctrlTab.InsertItem(TCIF_TEXT |TCIF_PARAM, 0,  _T("大单统计"), 0, (LPARAM)&m_wndTimeSale);
	m_ctrlTab.InsertItem(TCIF_TEXT |TCIF_PARAM, 1,  _T("大单排行榜"), 0, (LPARAM)&m_wndRank);
	m_ctrlTab.SetCurSel(0);

	RecalcLayout();
	ShowTitle();

	//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	//ShowWindow(SW_SHOWMAXIMIZED);
	//
	//m_wndTimeSale.ShowWindow(SW_SHOW);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgTimeSaleStatistic, CDialog)
//{{AFX_MSG_MAP(CDlgTimeSaleStatistic)
ON_WM_SIZE()
ON_WM_CLOSE()
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgTimeSaleStatistic::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	RecalcLayout();

	Invalidate(TRUE);		// dlg 和 static都没有CS_HREDRAW...
}

void CDlgTimeSaleStatistic::OnSelChange( NMHDR *pNMHeader, LRESULT *pResult )
{
	int iSel = m_ctrlTab.GetCurSel();
	
	ShowTitle();
	if ( iSel == 0 )
	{
		m_wndRank.OnIoViewDeactive();
		m_wndTimeSale.OnIoViewActive();
	}
	else
	{
		m_wndTimeSale.OnIoViewDeactive();
		m_wndRank.OnIoViewActive();
	}
	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

void CDlgTimeSaleStatistic::RecalcLayout()
{
	if ( m_ctrlTab.m_hWnd == NULL )
	{
		return;
	}

	CRect rc(0,0,0,0), rcTab;
	GetClientRect(rc);
	rcTab = rc;
	m_ctrlTab.AdjustRect(FALSE, &rcTab);
	m_ctrlTab.MoveWindow(rc, TRUE);
}

void CDlgTimeSaleStatistic::ShowTitle()
{
	CString StrTitle;
	int iSel = m_ctrlTab.GetCurSel();
	if ( -1 != iSel )
	{
		TCITEM item = {0};
		item.mask = TCIF_TEXT;
		item.pszText = StrTitle.GetBuffer(1024);
		item.cchTextMax = 1024;
		if ( m_ctrlTab.GetItem(iSel, &item) )
		{
			StrTitle.ReleaseBuffer();
			SetWindowText(StrTitle);
		}
		else
		{
			StrTitle.ReleaseBuffer();
		}
	}
}

void CDlgTimeSaleStatistic::ShowDlgTimeSaleStatistic()
{
	if ( !IsWindow(m_spThis->GetSafeHwnd()) )
	{
		ASSERT( m_spThis == NULL );
		m_spThis = new CDlgTimeSaleStatistic(AfxGetMainWnd());
		m_spThis->Create(IDD, AfxGetMainWnd());
	}
	ASSERT( IsWindow(m_spThis->GetSafeHwnd()) );
	m_spThis->CenterWindow();
	m_spThis->ShowWindow(SW_SHOW);
}

void CDlgTimeSaleStatistic::OnClose()
{
	DestroyWindow();
}

CDlgTimeSaleStatistic::~CDlgTimeSaleStatistic()
{
	if ( m_spThis == this )
	{
		m_spThis = NULL;
	}
}

void CDlgTimeSaleStatistic::PostNcDestroy()
{
	delete this;
}

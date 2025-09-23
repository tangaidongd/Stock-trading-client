// tabsheet.cpp : implementation file
//
#include "stdafx.h"
//#include "GGTong.h"
#include "tabsheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSheet

CTabSheet::CTabSheet()
{
	m_nNumOfPages = 0;
	m_nCurrentPage = 0;
	m_pPreCreate = NULL;

	memset(m_pPages, 0, sizeof(m_pPages));
	memset(m_IDD, 0, sizeof(m_IDD));
	memset(m_Title, 0, sizeof(m_Title));
}

CTabSheet::~CTabSheet()
{
	m_pPreCreate = NULL;
}
void CTabSheet::AddPreCreate ( CTabDialogPreCreateCallback* pPreCreate )
{
	m_pPreCreate = pPreCreate;
}

BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	//{{AFX_MSG_MAP(CTabSheet)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSheet message handlers

BOOL CTabSheet::AddPage(LPCTSTR title, CDialog *pDialog,UINT ID)
{
	if( MAXPAGE == m_nNumOfPages )
		return FALSE;

	if (m_nNumOfPages >= 0 && m_nNumOfPages < sizeof(m_pPages)/sizeof(m_pPages[0]))
	{
		m_pPages[m_nNumOfPages] = pDialog;
	}
	if (m_nNumOfPages >= 0 && m_nNumOfPages < sizeof(m_IDD)/sizeof(m_IDD[0]))
	{
		m_IDD[m_nNumOfPages] = ID;
	}
	if (m_nNumOfPages >= 0 && m_nNumOfPages < sizeof(m_Title)/sizeof(m_Title[0]))
	{
		m_Title[m_nNumOfPages] = title;
	}
	m_nNumOfPages++;
	return TRUE;
}

void CTabSheet::SetRect()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-2;
	nYc=tabRect.bottom-nY-2;
	
	m_pPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for( int nCount=1; nCount < m_nNumOfPages; nCount++ )
		m_pPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);

}

void CTabSheet::Show()
{
	int i = 0;
	for(  i=0; i < m_nNumOfPages; i++ )
	{
		if ( NULL != m_pPreCreate )
		{
			m_pPreCreate->PreCreate(m_Title[i],i);
		}
		m_pPages[i]->Create( m_IDD[i], this );
		InsertItem( i, m_Title[i] );
	}

	m_pPages[0]->ShowWindow(SW_SHOW);
	for( i=1; i < m_nNumOfPages; i++)
		m_pPages[i]->ShowWindow(SW_HIDE);

	SetRect();

}

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_nCurrentPage != GetCurFocus())
	{
		if (m_nCurrentPage >= 0 && m_nCurrentPage < sizeof(m_pPages)/sizeof(m_pPages[0]))
		{
			m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		}		
		m_nCurrentPage=GetCurFocus();
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
	}
}

int CTabSheet::SetCurSel(int nItem)
{
	if( nItem < 0 || nItem >= m_nNumOfPages)
		return -1;

	int ret = m_nCurrentPage;

	if(m_nCurrentPage != nItem )
	{
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage = nItem;
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
		CTabCtrl::SetCurSel(nItem);
	}

	return ret;
}

int CTabSheet::GetCurSel()
{
	return CTabCtrl::GetCurSel();
}

CDialog * CTabSheet::GetPage(int32 iIndex)
{
	ASSERT(iIndex<MAXPAGE-1);
	return m_pPages[iIndex];
}

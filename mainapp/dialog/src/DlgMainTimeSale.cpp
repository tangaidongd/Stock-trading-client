#include "StdAfx.h"

#include "DlgMainTimeSale.h"

#include "MerchManager.h"
#include "NewMenu.h"

#include "tinyxml.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CDlgMainTimeSale *CDlgMainTimeSale::m_spThis = NULL;

IMPLEMENT_DYNCREATE(CDlgMainTimeSale, CDialog)


CDlgMainTimeSale::CDlgMainTimeSale( CWnd *pParent /*= NULL*/ )
:CDialog(IDD, pParent)
{
	m_pMainTimeSaleSrc = NULL;
}


void CDlgMainTimeSale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMainTimeSale)
	//}}AFX_DATA_MAP
}

BOOL CDlgMainTimeSale::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (NULL == m_pMainTimeSaleSrc)
	{
		return TRUE;
	}
	
	if ( IsWindow(m_pMainTimeSaleSrc->GetSafeHwnd()) )
	{
		m_wndTimeSale.SetCenterManager(m_pMainTimeSaleSrc->GetCenterManager());
	}
	else
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		if ( NULL != pDoc )
		{
			m_wndTimeSale.SetCenterManager(pDoc->m_pAbsCenterManager);
		}
	}
	m_wndTimeSale.EnableDrawTitle(false);
	m_wndTimeSale.Create(WS_VISIBLE |WS_CHILD|SS_NOTIFY, this, CRect(0,0,0,0), 0x203);

	if ( IsWindow(m_pMainTimeSaleSrc->GetSafeHwnd()) )
	{
		CString StrXml = m_wndTimeSale.ToXml();
		int iLen = ::WideCharToMultiByte(CP_UTF8, 0, StrXml, -1, NULL, NULL, NULL, NULL);
		if ( iLen > 0 )
		{
			char *pStrUtf8 = new char[iLen];
			::WideCharToMultiByte(CP_UTF8, 0, StrXml, -1, pStrUtf8, iLen, NULL, NULL);
			ASSERT( strlen(pStrUtf8) == iLen-1 );
			TiXmlDocument	tiDoc;
			tiDoc.Parse(pStrUtf8);
			m_wndTimeSale.FromXml(tiDoc.FirstChildElement());
			delete []pStrUtf8;
		}
	}

	RecalcLayout();
	//
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgMainTimeSale, CDialog)
//{{AFX_MSG_MAP(CDlgMainTimeSale)
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgMainTimeSale::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	RecalcLayout();

	Invalidate(TRUE);		// dlg 和 static都没有CS_HREDRAW...
}

void CDlgMainTimeSale::RecalcLayout()
{
	if ( m_wndTimeSale.m_hWnd == NULL )
	{
		return;
	}

	CRect rc(0,0,0,0);
	GetClientRect(rc);
	m_wndTimeSale.MoveWindow(rc, TRUE);
	m_wndTimeSale.Invalidate(TRUE);
	m_wndTimeSale.OpenTab(m_wndTimeSale.GetCurTab());
}

int CDlgMainTimeSale::ShowPopupMainTimeSale( CIoViewMainTimeSale *pMainTmSrc /*= NULL*/ )
{
	if ( !IsWindow(m_spThis->GetSafeHwnd()) )
	{
		ASSERT( NULL == m_spThis );
		m_spThis = new CDlgMainTimeSale();
		m_spThis->m_pMainTimeSaleSrc = pMainTmSrc;
		m_spThis->Create(IDD, AfxGetMainWnd());
	}
	if ( IsWindow(m_spThis->GetSafeHwnd()) )
	{
		if ( !m_spThis->IsWindowVisible() )
		{
			m_spThis->CenterWindow();
		}
		m_spThis->ShowWindow(SW_SHOWNORMAL);
		m_spThis->SetForegroundWindow();
		m_spThis->SetFocus();
	}
	else
	{
		delete m_spThis;
		m_spThis = NULL;
		return IDCANCEL;
	}

	return IDOK;
}

void CDlgMainTimeSale::PostNcDestroy()
{
	if ( m_spThis == this )
	{
		m_spThis = NULL;
	}
	delete this;
}

void CDlgMainTimeSale::OnDestroy()
{
	// 销毁还是隐藏呢？
	CDialog::OnDestroy();
}

void CDlgMainTimeSale::OnPaint()
{
	CPaintDC dc(this);
}

BOOL CDlgMainTimeSale::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

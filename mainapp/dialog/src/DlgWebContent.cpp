// DlgNews.cpp : implementation file
//
#include "stdafx.h"

#include "DlgWebContent.h"
#include <wininet.h>
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTodayComment dialog
CDlgWebContent::CDlgWebContent(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWebContent::IDD, pParent)
{
	m_pParent = pParent;
	m_pWndCef = NULL;
}

CDlgWebContent::~CDlgWebContent()
{
	//m_WebBrowser.DestroyWindow();
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
}

void CDlgWebContent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWebContent)

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgWebContent, CDialog)
	//{{AFX_MSG_MAP(CDlgWebContent)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
//	ON_WM_ACTIVATE()
//	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWebContent message handlers

BOOL CDlgWebContent::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);

	CRect rect(0, 0 , 840, 610);
	this->MoveWindow(&rect);
	this->CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWebContent::SetTitleUrl( const CString &StrTitle, const CString &StrUrl)
{
	m_strTitle = StrTitle;
	m_strUrl = StrUrl;

	if ( NULL != m_hWnd )
	{
		SetWindowText(StrTitle);
	}
}

void CDlgWebContent::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	if(NULL != m_pWndCef)
	{
		m_pWndCef->MoveWindow(0, 0, cx, cy);
	}
}

void CDlgWebContent::OnOK()
{
	// 检查一下值
	
	CDialog::OnOK();
}

void CDlgWebContent::ShowDlgIEWithSize(const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag /* = 0 */)
{
	if ( 1 == bFlag )
	{
		if ( NULL == m_hWnd )
		{
			return;
		}
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		return;
	}
	
	CRect rtShow = rect;
	if (rect.IsRectEmpty())
	{
		rtShow = CRect(0,0,800,600);		
	}

	SetTitleUrl(StrTitle, StrUrl);
	MoveWindow(&rtShow);
	ShowWeb(rtShow);	// 显示浏览器内容
	CenterWindow();
	ShowWindow(SW_NORMAL);
}

void CDlgWebContent::OnClickClose()
{
	//m_WebBrowser.DestroyWindow();
}

void CDlgWebContent::ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag/* = 0*/)
{
	if( 1 == bFlag )
	{
		if ( NULL == m_hWnd )
		{
			return;
		}
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		return;
	}

	CRect rcShow;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcShow, SPIF_SENDCHANGE);	// 不包括任务栏在内
	int screenWidth = rcShow.Width();
	rcShow.left = screenWidth * 3 / 16;
	rcShow.right = rcShow.left + screenWidth * 5 / 8;
	rcShow.top = 103;
	rcShow.bottom -= 60;


	SetTitleUrl(StrTitle, StrUrl);
	MoveWindow(&rcShow);
	ShowWindow(SW_NORMAL);
	CenterWindow();
	ShowWeb(rcShow);	// 显示浏览器内容
}

void CDlgWebContent::ShowWeb(const CRect& rc)
{
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
	m_pWndCef = new CWndCef;
	m_pWndCef->SetUrl(m_strUrl);
	m_pWndCef->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,
						CRect(0, 0, rc.Width(), rc.Height()), this, 7506);
}

// void CDlgWebContent::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
// {
// 	CDialog::OnActivate(nState, pWndOther, bMinimized);  
// 	if (WA_INACTIVE == nState)  
// 	{  
// 
// 		CPoint pt(0,0);
// 		GetCursorPos(&pt);
// 		if (m_parentBtnRect.PtInRect(pt))
// 		{
// 			return;
// 		}
// 		PostMessage(WM_CLOSE, NULL, NULL);  
// 	}  
// }

void CDlgWebContent::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL CDlgWebContent::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDlgWebContent::SetUserDlgInfo(CRect &parentRct)
{
	m_parentBtnRect = parentRct;
}

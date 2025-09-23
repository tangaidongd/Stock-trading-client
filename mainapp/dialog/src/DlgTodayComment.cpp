// DlgNews.cpp : implementation file
//
#include "stdafx.h"

#include "DlgTodayComment.h"
#include <wininet.h>
#include <WindowsX.h>
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CDlgTodayComment dialog
CDlgTodayCommentIE::CDlgTodayCommentIE(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTodayCommentIE::IDD, pParent)
{
	m_pWndCef = NULL;
}

CDlgTodayCommentIE::~CDlgTodayCommentIE()
{
	//m_WebBrowser.DestroyWindow();
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
}

void CDlgTodayCommentIE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTodayComment)

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTodayCommentIE, CDialogEx)
	//{{AFX_MSG_MAP(CDlgTodayComment)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
//	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTodayComment message handlers

BOOL CDlgTodayCommentIE::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_strTitle);

	CRect rect(0, 0 , 840, 610);
	this->MoveWindow(&rect);
	this->CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTodayCommentIE::SetTitleUrl( const CString &StrTitle, const CString &StrUrl)
{
	m_strTitle = StrTitle;
	m_strUrl = StrUrl;

	if ( NULL != m_hWnd )
	{
		SetWindowText(StrTitle);
	}
}

void CDlgTodayCommentIE::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);

	if(NULL != m_pWndCef)
    {
        if(IsRestored(m_hWnd) || IsZoomed())
        {
            m_pWndCef->MoveWindow(0, 0, cx, cy);
        }
        else
        {
            // 减去标题栏的高度
            m_pWndCef->MoveWindow(0, 0, cx, cy - 33);
        }
	}
}

void CDlgTodayCommentIE::OnOK()
{
	// 检查一下值
	
	CDialogEx::OnOK();
}

void CDlgTodayCommentIE::ShowDlgIEWithSize(const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag /* = 0 */, Color clrCefBk)
{
	static CDlgTodayCommentIE s_dlg;
	
	if ( 1 == bFlag )
	{
		if ( NULL == s_dlg.m_hWnd )
		{
			return;
		}
		::SendMessage(s_dlg.m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		//::PostMessage(s_dlg.m_hWnd,WM_CLOSE,0,0);
		return;
	}
	
	CRect rtShow = rect;
	if (rect.IsRectEmpty())
	{
		rtShow = CRect(0,0,800,600);		
	}

	if ( s_dlg.m_hWnd == NULL )
	{
		s_dlg.Create(IDD, AfxGetMainWnd());
	}
	
	s_dlg.SetTitleUrl(StrTitle, StrUrl);	 
	s_dlg.MoveWindow(&rtShow);
	s_dlg.CenterWindow();
	s_dlg.ShowWeb(rtShow, clrCefBk);	// 显示浏览器内容
	s_dlg.ShowWindow(SW_NORMAL);
}

void CDlgTodayCommentIE::OnClickClose()
{
	//m_WebBrowser.DestroyWindow();
}

void CDlgTodayCommentIE::ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag/* = 0*/)
{
	static CDlgTodayCommentIE s_dlg;

	if( 1 == bFlag )
	{
		if ( NULL == s_dlg.m_hWnd )
		{
			return;
		}
		::SendMessage(s_dlg.m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		return;
	}

	CRect rcShow;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcShow, SPIF_SENDCHANGE);	// 不包括任务栏在内
	int screenWidth = rcShow.Width();
	rcShow.left = screenWidth * 3 / 16;
	rcShow.right = rcShow.left + screenWidth * 5 / 8;
	rcShow.top = 103;
	rcShow.bottom -= 60;

	if( s_dlg.m_hWnd == NULL )
	{
		s_dlg.Create(IDD, AfxGetMainWnd());
	}

	s_dlg.SetTitleUrl(StrTitle, StrUrl);
	s_dlg.MoveWindow(&rcShow);
	s_dlg.ShowWindow(SW_NORMAL);
	s_dlg.ShowWeb(rcShow);	// 显示浏览器内容
	
}

void CDlgTodayCommentIE::ShowWeb(const CRect& rc, Color clrCefBk)
{
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
	m_pWndCef = new CWndCef;
//	m_pWndCef->SetUrl(m_strUrl);
	m_pWndCef->SetCefBkColor(clrCefBk);
	m_pWndCef->CreateWndCef(7506,this, CRect(0, 0, rc.Width()-2*m_nFrameWidth, rc.Height() - 31),m_strUrl);
	//m_pWndCef->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,
	//					CRect(0, 0, rc.Width()-2*m_nFrameWidth, rc.Height() - 33), this, 7506);
}

void CDlgTodayCommentIE::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}

BOOL CDlgTodayCommentIE::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

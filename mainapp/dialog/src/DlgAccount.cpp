// DlgNews.cpp : implementation file
//
#include "stdafx.h"

#include "DlgAccount.h"
#include <wininet.h>
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTodayComment dialog
CDlgAccount::CDlgAccount(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAccount::IDD, pParent)
{
	m_pParent = pParent;
	m_pWndCef = NULL;
	m_pImgBk = NULL;
}

CDlgAccount::~CDlgAccount()
{
	//m_WebBrowser.DestroyWindow();
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
}

void CDlgAccount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAccount)

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAccount, CDialog)
	//{{AFX_MSG_MAP(CDlgAccount)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
//	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAccount message handlers

BOOL CDlgAccount::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExStyle=GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^0x80000);
	}

	SetLayeredWindowAttributes(RGB(0, 0, 0), 255, 0x1);
	SetWindowText(m_strTitle);

	m_pImgBk = Image::FromFile(L"image//userInfoBk.png");

	CRect rect(0, 0 , 840, 610);
	this->MoveWindow(&rect);
	this->CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAccount::SetTitleUrl( const CString &StrTitle, const CString &StrUrl)
{
	m_strTitle = StrTitle;
	m_strUrl = StrUrl;

	if ( NULL != m_hWnd )
	{
		SetWindowText(StrTitle);
	}
}

void CDlgAccount::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	if(NULL != m_pWndCef)
	{
		m_pWndCef->MoveWindow(0, 6, cx, cy);
	}
}

void CDlgAccount::OnOK()
{
	// 检查一下值
	
	CDialog::OnOK();
}

void CDlgAccount::ShowDlgIEWithSize(const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag /* = 0 */)
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
	ShowWindow(SW_NORMAL);
}

void CDlgAccount::OnClickClose()
{
	//m_WebBrowser.DestroyWindow();
}

void CDlgAccount::ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag/* = 0*/)
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
	ShowWeb(rcShow);	// 显示浏览器内容
}

void CDlgAccount::ShowWeb(const CRect& rc)
{
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
	m_pWndCef = new CWndCef;
	m_pWndCef->SetCefBkColor(RGB(255,255,255));
	m_pWndCef->CreateWndCef(7506,this,CRect(0, 6, rc.Width(), rc.Height()),m_strUrl);
	
//	m_pWndCef->SetUrl(m_strUrl);
//	m_pWndCef->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,
//						CRect(0, 0, rc.Width(), rc.Height()), this, 7506);
}

void CDlgAccount::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);  
	if (WA_INACTIVE == nState)  
	{  

		CPoint pt(0,0);
		GetCursorPos(&pt);
		if (m_parentBtnRect.PtInRect(pt))
		{
			return;
		}
		PostMessage(WM_CLOSE, NULL, NULL);  
	}  
}

void CDlgAccount::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	if(m_pImgBk)
	{
		RectF destRect;
		destRect.X = 0;
		destRect.Y = 0;
		destRect.Width  = rcWindow.Width();
		destRect.Height = rcWindow.Height();
		graphics.DrawImage(m_pImgBk, destRect, 0, 0, m_pImgBk->GetWidth()-1, m_pImgBk->GetHeight(), UnitPixel);
	}

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

BOOL CDlgAccount::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDlgAccount::SetUserDlgInfo(CRect &parentRct)
{
	m_parentBtnRect = parentRct;
}

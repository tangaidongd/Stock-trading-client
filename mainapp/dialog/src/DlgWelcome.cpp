// DlgWelcome.cpp : implementation file
//
#include "stdafx.h"
#include "DlgWelcome.h"

#include "pathfactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWelcome dialog

CDlgWelcome::CDlgWelcome(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWelcome::IDD, pParent)
{
	m_bShowUrl   = FALSE;
	m_hIcon      = NULL;
	m_pImgWelcom = NULL;
}

void CDlgWelcome::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWelcome)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgWelcome, CDialogEx)
	//{{AFX_MSG_MAP(CDlgWelcome)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWelcome message handlers

BOOL CDlgWelcome::OnInitDialog()
{
	int32 iWidth = 740, iHeight = 525;
	CString StrName = L"欢迎";
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();

	if (NULL == pApp)
	{
		return TRUE;
	}

	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"欢迎" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;
			break;
		}
	}

	m_pImgWelcom = Image::FromFile(CPathFactory::GetImageWelcomePath() );

//--- wangyongxue 使用本地图片进行显示
// 	if (0 < pApp->m_pConfigInfo->m_StrWelcomeUrl.GetLength())
// 	{
// 		m_bShowUrl = TRUE;
// 	}
// 	else
// 	{
// 		m_bShowUrl = FALSE;
// 	}

	SetWindowText(StrName);

	MoveWindow(0, 0, iWidth, iHeight);

	CRect rect,rcWnd;
	CenterWindow();

	if (m_bShowUrl)
	{
		CRect rectWeb;
		GetClientRect(rectWeb);
		m_wndCef.CreateWndCef(9877,this, rectWeb,pApp->m_pConfigInfo->m_StrWelcomeUrl);
		//m_wndCef.SetUrl(pApp->m_pConfigInfo->m_StrWelcomeUrl);
		//m_wndCef.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rectWeb, this, 9877);
	}

	return TRUE; 
}

void CDlgWelcome::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	DEL(m_pImgWelcom);
	delete this;
}

void CDlgWelcome::OnPaint()
{
	CPaintDC dc(this);
	if (m_bShowUrl)
	{
		return;
	}

	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(&rc); 

	rc.top = 0;
	rc.left = 0;
	rc.right = rc.Width() ;
	rc.bottom = rc.Height();

	if ( m_pImgWelcom )
	{
		Graphics g(dc.m_hDC);
		DrawImage(g, m_pImgWelcom, rc, 1, 0, true);
	}
}

void CDlgWelcome::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

HCURSOR CDlgWelcome::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// 屏蔽按键消息
BOOL CDlgWelcome::PreTranslateMessage(MSG* pMsg) 
{
	if ((WM_LBUTTONUP == pMsg->message) && !m_bShowUrl)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		
		CRect rtClient;
		GetClientRect(rtClient);

		if (rtClient.PtInRect(pt))
		{
			CDialog::OnOK();
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg); 
}

void CDlgWelcome::OnLButtonDown(UINT nFlags, CPoint point)
{
	ScreenToClient(&point);
	
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString StrAdvsCode = pApp->m_pConfigInfo->m_StrWelcomeAdvsCode;
	CString StrAdvsUrl = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode,EAUTLink);
	if(!StrAdvsUrl.IsEmpty())
		ShellExecute(0, L"open", StrAdvsUrl, NULL, NULL, SW_NORMAL);
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

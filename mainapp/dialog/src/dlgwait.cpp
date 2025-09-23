// dlgwait.cpp : implementation file
//

#include "stdafx.h"

#include "dlgwait.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog


CDlgWait::CDlgWait(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWait::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWait)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CDlgWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWait)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWait, CDialog)
	//{{AFX_MSG_MAP(CDlgWait)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWait message handlers
void CDlgWait::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
					   // Do not call CDialog::OnPaint() for painting messages
}
void CDlgWait::OnClose() 
{
	CMainFrame* pWnd    = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgWait	= NULL;
	
	DestroyWindow();
}
void CDlgWait::PostNcDestroy() 
{	
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CDlgWait::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE);	
	// ShowCursor(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CDlgWait::OnEraseBkgnd(CDC* pDC) 
{
	DrawShowText();
	return TRUE;
}
BOOL CDlgWait::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}
void CDlgWait::DrawShowText()
{
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect,RGB(0,0,0));
	dc.SetTextColor(RGB(255,0,0));
	
	LOGFONT lg;
	memset(&lg,0,sizeof(LOGFONT));
	_tcscpy(lg.lfFaceName,L"宋体");  
	
   	lg.lfHeight  = -12;
   	lg.lfWeight  = 580;
   	lg.lfCharSet = 0;
	lg.lfOutPrecision = 3;
	
	CFont  font;
	font.CreateFontIndirect(&lg);
	CFont * pOldFont = dc.SelectObject(&font);
	
	dc.TextOut(rect.Width()/2,rect.Height()/2,L"页面载入中...");

	dc.SelectObject(pOldFont);
	font.DeleteObject();
}

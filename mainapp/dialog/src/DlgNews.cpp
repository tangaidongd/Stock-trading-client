// DlgNews.cpp : implementation file
//

#include "stdafx.h"

#include "DlgNews.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNews dialog


CDlgNews::CDlgNews(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNews::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNews)
	m_strNewsText = _T("");
	//}}AFX_DATA_INIT

	m_clrText = GetSysColor(COLOR_BTNTEXT);
	m_clrBackground = GetSysColor(COLOR_3DFACE);

	m_nTimerID = 0;

	m_pView = NULL;
}


void CDlgNews::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNews)
	DDX_Control(pDX, IDC_EDIT_NEWS, m_ctrlNews);
	DDX_Control(pDX, IDC_STATIC_PICPOS, m_ctrPicPos);
	DDX_Text(pDX, IDC_EDIT_NEWS, m_strNewsText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNews, CDialogEx)
	//{{AFX_MSG_MAP(CDlgNews)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNews message handlers

BOOL CDlgNews::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	int32 iWidth = 672, iHeight = 448;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL != pApp)
	{
		int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
		for (int32 i=0; i<iSize; i++)
		{
			CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
			if (L"Jpzx" == wnd.m_strID)
			{
				iWidth = wnd.m_iWidth;
				iHeight = wnd.m_iHeight;
				break;
			}
		}
	}
	CRect rcWnd(0, 0, iWidth, iHeight);
	MoveWindow(rcWnd);
	CenterWindow();
	
	m_ctrPicPos.ShowWindow(SW_HIDE);
	m_ctrlNews.ShowWindow(SW_HIDE);
	
	if( !m_strTitle.IsEmpty() )
	{
		SetWindowText(m_strTitle);
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	//pBut->GetClientRect( &rc );
	//pBut->ClientToScreen( &rc );
	//this->ScreenToClient( &rc );
	
	if( m_pView == NULL )
		m_pView = new CNewsShowView ;
	if( !m_pView->GetSafeHwnd() )
	{
		if(!m_pView->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcClient,this,0))
			return FALSE ;
	}

	m_pView->OnInitialUpdate();
	m_pView->Invalidate(FALSE);
	m_pView->ShowWindow(SW_SHOW);

	m_pView->SetNewsConext(m_strTitle,m_strTime,m_strNewsText);

	if( m_nTimerID == 0 )
	{
		m_nTimerID = SetTimer(10001,100,NULL);
	}
/*	m_Font.DeleteObject();
	m_FontNode.m_stLogfont.lfHeight = -16;
	m_Font.CreateFontIndirect(&m_FontNode.m_stLogfont);
	m_ctrlNews.SetFont(&m_Font);

	CRect rcClient;
	GetClientRect(&rcClient);
	m_ctrlNews.MoveWindow(rcClient);

	m_ctrlNews.SetSel( -1,0,FALSE );*/

	UpdateData(FALSE);

	m_pView->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNews::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

/*	CRect rtClient;
	GetClientRect(rtClient);
	
	int32 iSaveDc = dc.SaveDC();
	
	//	
	//CFont* pFont = GetIoViewFontObject(ESFNormal);
	//CFont* pFontOld = dc->SelectObject(pFont);
	
	//
	//COLORREF clrText	= GetIoViewColor(ESCText);
	COLORREF clrBkGroud	= m_clrBackground;
	//COLORREF clrLine = GetIoViewColor(ESCChartAxisLine);
	
	//
	dc.FillSolidRect(rtClient, clrBkGroud);
	//dc->SetTextColor(clrText);
	//dc->SetBkMode(TRANSPARENT);
	//
	
	//CPen pen(PS_SOLID, 1, clrLine);
	//CPen* pPenOld = dc->SelectObject(&pen);
	
	//
	//dc.SelectObject(pFontOld);
	//dc.SelectObject(pPenOld);
	//pen.DeleteObject();
	
	dc.RestoreDC(iSaveDc);*/
}

void CDlgNews::OnDestroy() 
{
	if( m_pView && m_pView->GetSafeHwnd() )
	{
		m_pView->DestroyWindow();
	}
	
	m_pView = NULL; 

	CDialogEx::OnDestroy();

	//m_Font.DeleteObject();
}

HBRUSH CDlgNews::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
/*	if (nCtlColor == CTLCOLOR_STATIC && m_bNeedColorText)
	{	
		switch(pWnd->GetDlgCtrlID()) 
		{ 
		case IDC_EDIT_NEWS: 
			pDC->SetBkMode(TRANSPARENT); 
			pDC->SetTextColor(m_clrText); 
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH); 
		default: 
			break; 
		} 
	}*/
	return hbr;
}

void CDlgNews::OnTimer(UINT nIDEvent) 
{	
	if( m_nTimerID == nIDEvent )
	{
		if( m_pView )
			m_pView->SetFocus();

		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	CDialogEx::OnTimer(nIDEvent);
}
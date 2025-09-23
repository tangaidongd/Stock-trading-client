// DlgNews.cpp : implementation file
//

#include "stdafx.h"
#include "DlgJPNews.h"
#include "PathFactory.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WM_NCMOUSELEAVE			
#define GGT_WM_NCMOUSELEAVE		(0x02A2)
#else
#define GGT_WM_NCMOUSELEAVE		(WM_NCMOUSELEAVE)
#endif

#ifndef TME_NONCLIENT
#define GGT_TME_NONCLIENT		(0x0010)
#else
#define GGT_TME_NONCLIENT		(TME_NONCLIENT)
#endif
/////////////////////////////////////////////////////////////////////////////
// CDlgJPNews dialog
#define INVALID_ID		-1
#define FRAME_WIDTH		 1
#define HIDE_WIDTH		 0
#define TITLE_HEIGHT	28
#define SYSBTN_WIDTH	29
#define SYSBTN_HEIGHT	26
#define ID_WEBCONTROL	1900	// web控件ID
#define ID_TABCONTROL	1901    // TAB控件ID
#define TAB_WIDTH		134
#define PREBTN_WIDTH	24
#define LOGOTOTAB		20		// tab距离Logo的距离

CDlgJPNews::CDlgJPNews(CWnd* pParent /*=NULL*/) : CDialog(CDlgJPNews::IDD, pParent)
{
	m_pGuiTabWnd	= NULL;
	m_pLogoImg		= NULL;
	m_pTabImg		= NULL;
	m_pSysMenuImg	= NULL;
	m_pBkImg		= NULL;
	m_pPreImg		= NULL;
	m_pNextImg		= NULL;
	m_pLogoImg      = NULL;
	m_pTabImg       = NULL;
	m_rectCaption		= CRect(0,0,0,0);
	m_rectLeftBoard		= CRect(0,0,0,0);
	m_rectRightBoard	= CRect(0,0,0,0);
	m_rectBottomBoard	= CRect(0,0,0,0);
	m_rectBorder		= CRect(0,0,0,0);
	m_iXButtonHovering  = INVALID_ID;
	m_bShowBtn			= FALSE;
	m_iTabCnt			= 0;
	
}

CDlgJPNews::~CDlgJPNews()
{
	DEL(m_pGuiTabWnd);

	DEL(m_pSysMenuImg);
	DEL(m_pLogoImg);
	DEL(m_pTabImg);
	DEL(m_pBkImg);
	DEL(m_pPreImg);
	DEL(m_pNextImg);

	map<CString, CWndJPNews*>::iterator iter;
	for (iter=m_mapJPNews.begin(); iter!=m_mapJPNews.end(); ++iter)
	{
		if(iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	m_mapJPNews.clear();

	DEL(m_pBkImg);
	DEL(m_pSysMenuImg);
	DEL(m_pLogoImg);
	DEL(m_pTabImg);
	DEL(m_pPreImg);
	DEL(m_pNextImg);
}

void CDlgJPNews::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgJPNews)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgJPNews, CDialog)
	//{{AFX_MSG_MAP(CDlgJPNews)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_MESSAGE(GGT_WM_NCMOUSELEAVE, OnNcMouseLeave)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_NCPAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TCN_SELCHANGE, ID_TABCONTROL, OnSelChange)
	ON_NOTIFY(WM_DELETETAB, ID_TABCONTROL, OnTabDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgJPNews message handlers

BOOL CDlgJPNews::OnInitDialog() 
{
	CRect rcWnd;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWnd, SPIF_SENDCHANGE);	// 不包括任务栏在内
	int screenWidth = rcWnd.Width();
	rcWnd.left = screenWidth * 3 / 16;
	rcWnd.right = rcWnd.left + screenWidth * 5 / 8;
	rcWnd.top = 103;
	rcWnd.bottom -= 64;
	MoveWindow(rcWnd);

	GetClientRect(rcWnd);
	m_rectBorder.top    = TITLE_HEIGHT-2;
	m_rectBorder.left   = rcWnd.left;
	m_rectBorder.right  = rcWnd.right - FRAME_WIDTH;
	m_rectBorder.bottom = TITLE_HEIGHT;
	GetDlgItem(IDC_STATIC_TOP)->MoveWindow(m_rectBorder);

	return TRUE;
}

int CDlgJPNews::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)	
	{
		return -1;
	}

	m_pSysMenuImg = Image::FromFile(_T("image//Tab_SysMenu.png"));
	m_pLogoImg	  = Image::FromFile(CPathFactory::GetImageTabLogoPath());
	m_pTabImg	  = Image::FromFile(CPathFactory::GetImageTabOutPath());
	m_pBkImg	  = Image::FromFile(CPathFactory::GetImageTabTitleBKPath());
	m_pPreImg	  = Image::FromFile(L"image//LeftScroll.png");
	m_pNextImg	  = Image::FromFile(L"image//RightScroll.png");

	m_pGuiTabWnd = new CGuiTabWnd(); 
	m_pGuiTabWnd->Create(WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,0), this, ID_TABCONTROL);
	m_pGuiTabWnd->SetTabBkColor(RGB(50,50,50), RGB(30,30,30));
	m_pGuiTabWnd->SetTabTextColor(RGB(188,188,188), RGB(244,144,0));
	m_pGuiTabWnd->SetPngBkGround(m_pTabImg, m_pTabImg, 3, IDB_TABBK_NEWS);
	m_pGuiTabWnd->SetLayoutStyle(CGuiTabWnd::ELSGraphic);
	m_pGuiTabWnd->SetGraphicStyle(CGuiTabWnd::EGSStyle2);

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);
	CRect rcControl;
	rcControl.left   = rcWnd.right - SYSBTN_WIDTH;
	rcControl.top    = 0;
	rcControl.right  = rcWnd.right;
	rcControl.bottom = rcControl.top + SYSBTN_HEIGHT;

	// 添加标题栏上的最大，最小，关闭按钮
	CNCButton* pButton= AddTitleButton(rcControl, m_pSysMenuImg, 2, ID_MIN_BTN);
	pButton->SetBtnBkgColor(RGB(44,50,55));
	pButton= AddTitleButton(rcControl, m_pSysMenuImg, 2, ID_MAX_BTN);
	pButton->SetBtnBkgColor(RGB(44,50,55));
	pButton= AddTitleButton(rcControl, m_pSysMenuImg, 2, ID_CLOSE_BTN);
	pButton->SetBtnBkgColor(RGB(44,50,55));

	int iLogoImgWidth = 0;
	if(m_pLogoImg != NULL)
	{
		iLogoImgWidth = m_pLogoImg->GetWidth();
	}

	rcControl.left	= iLogoImgWidth + LOGOTOTAB;
	rcControl.right = rcControl.left + PREBTN_WIDTH;
	AddTitleButton(rcControl, m_pPreImg, 3, ID_PRE_BTN);
	AddTitleButton(rcControl, m_pNextImg, 3, ID_NEXT_BTN);

	return 0;
}

void CDlgJPNews::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(&rcClient);

	int iLogoImgWidth = 0;
	if(m_pLogoImg != NULL)
	{
		iLogoImgWidth = m_pLogoImg->GetWidth();
	}

	int iShowWidth = rcClient.Width() - iLogoImgWidth - LOGOTOTAB - 130;
	m_iTabCnt = iShowWidth / (TAB_WIDTH-HIDE_WIDTH);

	SetTabSize();

	m_rectBorder.left   = rcClient.left;
	m_rectBorder.right  = rcClient.right - FRAME_WIDTH;
	m_rectBorder.top	= TITLE_HEIGHT - 2;
	m_rectBorder.bottom = TITLE_HEIGHT;

	CalcNcSize();
	if ((SIZE_MAXIMIZED == nType) || (SIZE_RESTORED == nType))
	{
		OnNcPaint();
	}

	// 遍历所有的窗口
	map<CString, CWndJPNews*>::iterator iter;
	for (iter=m_mapJPNews.begin(); iter!=m_mapJPNews.end(); ++iter)
	{
		CWndJPNews* pWnd = iter->second;
		pWnd->MoveWindow(GetBrowerRect());
	}
}

void CDlgJPNews::OnClose()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	DestroyWindow();
	DEL(pWnd->m_pDlgJPNews)
}

BOOL CDlgJPNews::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

HBRUSH CDlgJPNews::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetBkMode(TRANSPARENT);
		int32 iID = pWnd->GetDlgCtrlID();
		if (IDC_STATIC_TOP == iID)
		{
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	return hbr;
}

LRESULT CDlgJPNews::OnNcHitTest(CPoint point) 
{	
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = TButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		switch (iButton)
		{
		case ID_CLOSE_BTN:
			{
				return HTCLOSE;
			}
		case ID_MAX_BTN:
			{
				return HTMAXBUTTON;
			}
		case ID_MIN_BTN:
			{
				return HTMINBUTTON;
			}
		default:
			break;
		}
	}

	if (m_rectCaption.PtInRect(pt))
	{
		return HTCAPTION;
	}

	return CDialog::OnNcHitTest(point);
}

void CDlgJPNews::OnNcPaint()
{
	CWindowDC dc(this);

	int iWidthBk = 0;
	int iHeightBk = 0;
	if(m_pBkImg != NULL)
	{
		iWidthBk = m_pBkImg->GetWidth();
		iHeightBk = m_pBkImg->GetHeight();
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	int iLogoImgWidth = 0;
	if(m_pLogoImg != NULL)
	{
		iLogoImgWidth = m_pLogoImg->GetWidth();
	}

	CRect rcClient;
	rcClient.left   = m_rectLeftBoard.right;
	rcClient.top    = m_rectCaption.bottom;
	rcClient.right  = m_rectRightBoard.left;
	rcClient.bottom = m_rectBottomBoard.top;

	// 剪除掉客户区
	dc.ExcludeClipRect(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	// 边框
	Pen pen(Color(255,70,70,70), 2*FRAME_WIDTH);
	graphics.DrawLine(&pen, m_rectLeftBoard.left, m_rectLeftBoard.top, m_rectLeftBoard.right, m_rectLeftBoard.bottom);
	graphics.DrawLine(&pen, m_rectRightBoard.left, m_rectRightBoard.top, m_rectRightBoard.right, m_rectRightBoard.bottom);
	graphics.DrawLine(&pen, m_rectBottomBoard.left, m_rectBottomBoard.top, m_rectBottomBoard.right, m_rectBottomBoard.bottom);

	RectF destRect;
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width  = (REAL)rcWindow.Width();
	destRect.Height = (REAL)iHeightBk;
	graphics.DrawImage(m_pBkImg, destRect, 1, 0, (REAL)(iWidthBk-3), (REAL)iHeightBk, UnitPixel);

	destRect.X = 5;
	destRect.Y = 0;
	destRect.Width  = (REAL)iLogoImgWidth;
	destRect.Height = (REAL)iHeightBk;
	graphics.DrawImage(m_pLogoImg, destRect, 0, 0, (REAL)iLogoImgWidth, (REAL)iHeightBk, UnitPixel);


	// 系统按钮位置
	int iBegPos = rcWindow.right - SYSBTN_WIDTH;
	int iTop = 0;
	int iBottom = SYSBTN_HEIGHT;

	CRect rcClose(iBegPos, iTop, iBegPos+SYSBTN_WIDTH, iBottom);
	m_mapTitleBtn[ID_CLOSE_BTN].SetRect(rcClose);

	CRect rcMax(iBegPos-SYSBTN_WIDTH, iTop, iBegPos, iBottom);
	m_mapTitleBtn[ID_MAX_BTN].SetRect(rcMax);

	CRect rcMin(iBegPos-SYSBTN_WIDTH*2, iTop, iBegPos-SYSBTN_WIDTH, iBottom);
	m_mapTitleBtn[ID_MIN_BTN].SetRect(rcMin);

	// 向前，向后按钮
	iBegPos = iLogoImgWidth + LOGOTOTAB;
	iTop += 2;
	CRect rcPre(iBegPos, iTop, iBegPos+PREBTN_WIDTH, iBottom);
	m_mapTitleBtn[ID_PRE_BTN].SetRect(rcPre);

	CRect rcNext(iBegPos+PREBTN_WIDTH, iTop, iBegPos+PREBTN_WIDTH*2, iBottom);
	m_mapTitleBtn[ID_NEXT_BTN].SetRect(rcNext);

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;

	// 遍历标题栏上所有按钮
	for (iter=m_mapTitleBtn.begin(); iter!=m_mapTitleBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		CRect rcControl;
		btnControl.GetRect(rcControl);

		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		int index = 0;
		int iD = btnControl.GetControlId();

		if (ID_MIN_BTN == iD) //系统按钮 1 min, 2 max, 3 restore, 4 close
		{
			index = 1;
			btnControl.SetSysIndex(index);
			btnControl.DrawSysButton(&graphics, index);
		}
		else if (ID_MAX_BTN == iD)
		{
			if (IsZoomed())
			{
				index = 3;
			}
			else
			{
				index = 2;
			}
			btnControl.SetSysIndex(index);
			btnControl.DrawSysButton(&graphics, index);
		}
		else if (ID_CLOSE_BTN == iD)
		{
			index = 4;
			btnControl.SetSysIndex(index);
			btnControl.DrawSysButton(&graphics, index);
		}
		else
		{
			if (!m_bShowBtn)
			{
				continue;
			}
			btnControl.DrawButton(&graphics);
		}
	}

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);	

	if(m_pGuiTabWnd != NULL)
	{
		m_pGuiTabWnd->RedrawWindow();
	}

	CWnd *pWnd = GetDlgItem(IDC_STATIC_TOP);
	if (pWnd->GetSafeHwnd())
	{
		pWnd->MoveWindow(m_rectBorder, FALSE);
		pWnd->RedrawWindow();
	}
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CDlgJPNews::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	if ((HTLEFT <= nHitTest) && (HTBOTTOMRIGHT >= nHitTest || 
		HTCAPTION == nHitTest))
	{
		CDialog::OnNcMouseMove(nHitTest, point);
	}

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -= rcWindow.left;
	point.y -= rcWindow.top;

	int iButton = TButtonHitTest(point);
	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapTitleBtn[m_iXButtonHovering].MouseLeave();			
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{
			m_iXButtonHovering = iButton;
			m_mapTitleBtn[m_iXButtonHovering].MouseHover();
		}
	}

	TRACKMOUSEEVENT tme;
	tme.cbSize	  = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags   = TME_LEAVE | GGT_TME_NONCLIENT;
	_TrackMouseEvent(&tme);
}

LRESULT CDlgJPNews::OnNcMouseLeave( WPARAM w, LPARAM l )
{
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapTitleBtn[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = INVALID_ID;

	}

	return 0; 
}

void CDlgJPNews::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + TITLE_HEIGHT;

	// 当前点是否在标题栏上
	if (rcWindow.PtInRect(point))
	{
		point.x -= rcWindow.left;
		point.y -= rcWindow.top;
		int iButton = TButtonHitTest(point);

		// 在按钮上双击,调用单击函数
		if (INVALID_ID != iButton)
		{
			m_mapTitleBtn[iButton].LButtonDown();
			return;
		}

		// 双击标题栏上除按钮以外的其它区域
		if (IsZoomed())
		{
			ShowWindow(SW_RESTORE);
		}
		else
		{
			ShowWindow(SW_MAXIMIZE);
		}
		return;
	}
}

void CDlgJPNews::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = TButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapTitleBtn[iButton].LButtonDown();
		return;
	}

	if ( HTMINBUTTON == nHitTest || HTMAXBUTTON == nHitTest || HTCLOSE == nHitTest || HTSYSMENU == nHitTest )
	{
		return;
	}

	CDialog::OnNcLButtonDown(nHitTest, point);
}

BOOL CDlgJPNews::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(NULL == m_pGuiTabWnd)
	{
		return CDialog::OnCommand(wParam, lParam);
	}

	int indexBegin = 0;
	int iCurTab = m_pGuiTabWnd->GetCurtab();
	int iTabCnt = m_pGuiTabWnd->GetCount();

	if (ID_PRE_BTN == wParam)
	{
		if ((iCurTab+m_iTabCnt-2) < iTabCnt)
		{
			indexBegin = iCurTab - 1;
			m_pGuiTabWnd->SetShowBeginIndex(indexBegin, indexBegin + m_iTabCnt-1);
		}

		m_pGuiTabWnd->SetCurtab(iCurTab - 1);
	}

	if (ID_NEXT_BTN == wParam)
	{
		if ((iCurTab-m_iTabCnt+3) > 0)
		{
			indexBegin = iCurTab-m_iTabCnt + 3;
			m_pGuiTabWnd->SetShowBeginIndex(indexBegin, indexBegin + m_iTabCnt-1);
		}
		m_pGuiTabWnd->SetCurtab(iCurTab + 1);
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDlgJPNews::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	int32 iFrameWidth = GetSystemMetrics(SM_CXFRAME)-GetSystemMetrics(SM_CXBORDER);
	lpMMI->ptMaxSize.y = GetSystemMetrics(SM_CYMAXIMIZED)-iFrameWidth*2;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgJPNews::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurTab = m_pGuiTabWnd->GetCurtab();
	CString StrVal = m_pGuiTabWnd->GetTabsValue(iCurTab);

	if(m_mapJPNews[StrVal]->GetSafeHwnd() == NULL)
	{
		CWndJPNews *pWnd = new CWndJPNews;
		m_mapJPNews[StrVal] = pWnd;

		pWnd->m_strUrl = m_pGuiTabWnd->GetTabsValue(iCurTab);
		if(!pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,GetBrowerRect(),this,15000+iCurTab))
		{
			return;	
		}
	}

	// 遍历所有de窗口
	map<CString, CWndJPNews*>::iterator iter;
	for (iter=m_mapJPNews.begin(); iter!=m_mapJPNews.end(); ++iter)
	{
		CWndJPNews* pWnd = iter->second;
		pWnd->ShowWindow(SW_HIDE);
	}
	m_mapJPNews[StrVal]->ShowWindow(SW_SHOW);

	//
	int iTabCnt = m_pGuiTabWnd->GetCount();
	if ((iTabCnt-1 == iCurTab) && m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(TRUE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(FALSE, TRUE);
	}
	else if ((0 == iCurTab) && m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(FALSE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(TRUE, TRUE);
	}
	else if (m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(TRUE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(TRUE, TRUE);
	}

	*pResult = 0;
}

void CDlgJPNews::OnTabDelete(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(NULL != m_pGuiTabWnd)
	{
		if (0 >= m_pGuiTabWnd->GetCount())
		{
			DestroyWindow();
		}
		else
		{
			SetTabSize();
			OnSelChange(pNMHDR, pResult);
		}
	}

	*pResult = 0;
}

CGuiTabWnd *CDlgJPNews::GetTabWnd()
{
	return m_pGuiTabWnd;
}

void CDlgJPNews::SetTabSize()
{
	if(NULL == m_pGuiTabWnd)
	{
		return;
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcTabWnd(rcClient);
	int iCurTab = m_pGuiTabWnd->GetCurtab();
	int iTabCnt = m_pGuiTabWnd->GetCount();
	int iShowCnt = iTabCnt;
	int indexBegin = 0;

	int iLogoImgWidth = 0;
	if(m_pLogoImg != NULL)
	{
		iLogoImgWidth = m_pLogoImg->GetWidth();
	}

	if (m_iTabCnt < iTabCnt)
	{
		indexBegin = iTabCnt - m_iTabCnt + 1;
		iShowCnt = m_iTabCnt - 1;
		rcTabWnd.left += (iLogoImgWidth + LOGOTOTAB + PREBTN_WIDTH*2);
		m_bShowBtn = TRUE;

		if ((iCurTab+m_iTabCnt-1) < iTabCnt)
		{
			indexBegin = iCurTab;
		}

		m_pGuiTabWnd->SetShowBeginIndex(indexBegin, indexBegin + m_iTabCnt-1);
	}
	else
	{
		rcTabWnd.left += (iLogoImgWidth + LOGOTOTAB);
		m_bShowBtn = FALSE;
		m_pGuiTabWnd->SetShowBeginIndex(indexBegin, iTabCnt);
	}

	m_pGuiTabWnd->SetALingTabs(CGuiTabWnd::ALN_TOP);
	int iHeightTabBar = m_pGuiTabWnd->GetFitHorW();

	rcTabWnd.bottom	= rcTabWnd.top + iHeightTabBar;
	rcTabWnd.right  = rcTabWnd.left + iShowCnt*(TAB_WIDTH-HIDE_WIDTH) + HIDE_WIDTH;
	m_pGuiTabWnd->MoveWindow(rcTabWnd);	

	if ((iTabCnt-1 == iCurTab) && m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(TRUE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(FALSE, TRUE);
	}
	else if((0 == iCurTab) && m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(FALSE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(TRUE, TRUE);
	}
	else if (m_bShowBtn)
	{
		m_mapTitleBtn[ID_PRE_BTN].EnableButton(TRUE, TRUE);
		m_mapTitleBtn[ID_NEXT_BTN].EnableButton(TRUE, TRUE);
	}
}

// 获取当前鼠标所在的按钮
int CDlgJPNews::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历标题栏上所有按钮
	for (iter=m_mapTitleBtn.begin(); iter!=m_mapTitleBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		CRect rcRect(0,0,0,0); 
		btnControl.GetRect(rcRect);

		// 点point是否在按钮区域内
		if (rcRect.PtInRect(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

void CDlgJPNews::TitleBtnChange()
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapTitleBtn.begin(); iter!=m_mapTitleBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 按钮是否需要弹起
		if (!btnControl.IsNormal())
		{
			btnControl.MouseLeave();
		}
		m_iXButtonHovering = INVALID_ID;
	}
}

CNCButton *CDlgJPNews::AddTitleButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	m_mapTitleBtn[nID] = btnControl;
	return &m_mapTitleBtn[nID];
}

void CDlgJPNews::CalcNcSize()
{	
	CRect rcWnd;
	GetWindowRect(rcWnd);

	m_rectCaption.top	 = 0;
	m_rectCaption.left   = 0;
	m_rectCaption.right  = rcWnd.Width();
	if ( IsIconic() )
	{
		//最小化时，全部都是Nc Area
		m_rectCaption.bottom = rcWnd.Height();

		// 左边框
		m_rectLeftBoard.SetRectEmpty();

		// 右边框	
		m_rectRightBoard.SetRectEmpty();

		// 底边框
		m_rectBottomBoard.SetRectEmpty();
	}
	else
	{
		int32 iHeightBk = 0;
		if(m_pBkImg != NULL)
		{
			iHeightBk = m_pBkImg->GetHeight();
		}

		m_rectCaption.bottom = m_rectCaption.top + iHeightBk;		

		// 左边框
		m_rectLeftBoard.left   = m_rectCaption.left;
		m_rectLeftBoard.right  = m_rectCaption.left + FRAME_WIDTH;
		m_rectLeftBoard.top	   = m_rectCaption.bottom;
		m_rectLeftBoard.bottom = rcWnd.Height();

		// 右边框	
		m_rectRightBoard.right  = m_rectCaption.right;
		m_rectRightBoard.left   = m_rectRightBoard.right - FRAME_WIDTH;
		m_rectRightBoard.top    = m_rectLeftBoard.top;
		m_rectRightBoard.bottom = m_rectLeftBoard.bottom;

		// 底边框
		m_rectBottomBoard		 = m_rectCaption;
		m_rectBottomBoard.top	 = rcWnd.Height() - FRAME_WIDTH;
		m_rectBottomBoard.bottom = rcWnd.Height();
	}
}

BOOL CDlgJPNews::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgJPNews::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = TButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		switch (iButton)
		{
		case ID_CLOSE_BTN:
			{
				SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
			}
			break;
		case ID_MAX_BTN:
			{
				if (IsZoomed())
				{
					SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
					m_mapTitleBtn[iButton].MouseLeave();
				}
				else
				{
					SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
					m_mapTitleBtn[iButton].MouseLeave();
				}
			}
			break;
		case ID_MIN_BTN:
			{
				CRect rcNews;
				GetWindowRect(&rcNews);
				BYTE bIsMinimize = false;

				if(rcNews.Height() < 100)
				{
					bIsMinimize = true;
				}

				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				m_mapTitleBtn[iButton].MouseLeave();

				if(bIsMinimize)
				{
					SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				}
				else
				{	// 改变最小化时的位置
					CRect rcWnd, rcParentClient;
					GetWindowRect(&rcWnd);
					GetParent()->GetClientRect(&rcParentClient);
					::SetWindowPos(m_hWnd, wndTop, rcParentClient.right - rcWnd.Width(), rcParentClient.bottom - rcWnd.Height(), 0, 0, SWP_NOSIZE);
				}
			}
			break;
		default:
			{
				m_mapTitleBtn[iButton].LButtonUp();
			}
			break;
		}
	}
	else
	{
		CDialog::OnNcLButtonUp(nHitTest, point);
	}
}

CRect CDlgJPNews::GetBrowerRect()
{
	CRect rect;
	GetClientRect(&rect);
	CRect rcWeb  =  rect;
	rcWeb.top	 += TITLE_HEIGHT;
	rcWeb.left	 += (FRAME_WIDTH - 2);
	rcWeb.bottom -= FRAME_WIDTH;

	return rcWeb;
}
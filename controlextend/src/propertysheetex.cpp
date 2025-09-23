// propertysheetex.cpp : implementation file
//

#include "stdafx.h"
#include "propertysheetex.h"
#include "GdiPlusTS.h"
		//XL0002
#include "ColorStep.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 
const UINT KTimerIdRefreshPropertySheetCaption	    = 123454;
const UINT KTimerPeriodRefreshPropertySheetCaption  = 200;

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetEX

IMPLEMENT_DYNAMIC(CPropertySheetEX, CPropertySheet)

CPropertySheetEX::CPropertySheetEX(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	ConstructPropertySheet();
}

CPropertySheetEX::CPropertySheetEX(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	ConstructPropertySheet();
}

CPropertySheetEX::~CPropertySheetEX()
{
	if (NULL != m_pImageButtons)
	{
		delete  m_pImageButtons;
		m_pImageButtons = NULL;
	}
	if (NULL != m_pImageCatptionBig)
	{
		delete  m_pImageCatptionBig;
		m_pImageCatptionBig = NULL;
	}
	if (NULL != m_pImageCatptionSmall)
	{
		delete  m_pImageCatptionSmall;
		m_pImageCatptionSmall = NULL;
	}
}

//XL0002
BEGIN_MESSAGE_MAP(CPropertySheetEX, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetEX)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_NCACTIVATE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE(GGT_WM_NCMOUSELEAVE, OnNcMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetEX message handlers
void CPropertySheetEX::ConstructPropertySheet()
{
	m_rectClose		= CRect(0,0,0,0);
	m_rectCaption	= CRect(0,0,0,0);
	
	m_rectLeftBoard  = CRect(0,0,0,0);
	m_rectRightBoard = CRect(0,0,0,0);
	m_rectBottomBoard= CRect(0,0,0,0);
	
	m_bNcMouseMoving= false;
	m_eCurrentShowState = ECSSNormal;

	m_pImageButtons = NULL;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_DLG_BTN, L"PNG", m_pImageButtons))
	{
		//ASSERT(0);
		m_pImageButtons = NULL;
	}	
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_DIALOG_CAPTION_BIG, L"PNG", m_pImageCatptionBig))
	{
		//ASSERT(0);
		m_pImageCatptionBig = NULL;
	}
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_DIALOG_CAPTION_SMALL, L"PNG", m_pImageCatptionSmall))
	{
		//ASSERT(0);
		m_pImageCatptionSmall = NULL;
	}	
}

void CPropertySheetEX::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	// Do not call CPropertySheet::OnPaint() for painting messages
}

void CPropertySheetEX::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);
	
	CalcNcSize();
	//OnNcPaint(); xiali
}

void CPropertySheetEX::CalcNcSize()
{
	CRect rc;
	GetWindowRect(rc);
	
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR= DATE=2010/04/10]
	//DESCRIPTION:	边框换成WindowRect与ClientRect来重新计算 - 保留先
	// m_rectCaption.top	 = 0;
	// 	m_rectCaption.left   = 0;
	// 	m_rectCaption.right  = rc.Width();
	// 	m_rectCaption.bottom = m_rectCaption.top + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	// 	
	// 	// 左边框	
	// 	m_rectLeftBoard.left  = rc.left;
	// 	m_rectLeftBoard.right = m_rectLeftBoard.left + GetSystemMetrics(SM_CXFRAME);
	// 	m_rectLeftBoard.top	  = rc.top + m_rectCaption.Height();
	// 	m_rectLeftBoard.bottom= rc.bottom;
	// 
	// 	// 右边框	
	// 
	// 	m_rectRightBoard.right = rc.right;
	// 	m_rectRightBoard.left  = m_rectRightBoard.right - GetSystemMetrics(SM_CXFRAME);
	// 	m_rectRightBoard.top   = m_rectLeftBoard.top;
	// 	m_rectRightBoard.bottom= m_rectLeftBoard.bottom;
	// 	
	// 	ScreenToClient(&m_rectRightBoard);
	// 	
	// 	m_rectRightBoard.right += GetSystemMetrics(SM_CXFRAME);
	// 	m_rectRightBoard.left  += GetSystemMetrics(SM_CXFRAME);
	// 	m_rectRightBoard.top   += GetSystemMetrics(SM_CXFRAME);
	//  	m_rectRightBoard.bottom+= GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXFRAME);
	// 
	// 	// 底边框
	// 	int iCaptionHight = m_rectCaption.Height();
	// 	m_rectBottomBoard = rc;
	// 	m_rectBottomBoard.top = m_rectBottomBoard.bottom - GetSystemMetrics(SM_CXFRAME);
	// 	
	// 	ScreenToClient(&m_rectBottomBoard);
	// 	
	// 	m_rectBottomBoard.right  += GetSystemMetrics(SM_CXFRAME);
	// 	m_rectBottomBoard.left   += GetSystemMetrics(SM_CXFRAME);
	// 	
	// 	m_rectBottomBoard.bottom += iCaptionHight;
	// 	m_rectBottomBoard.top	 += iCaptionHight;

	CRect	rcClient;
	GetClientRect(rcClient);
	ClientToScreen(&rcClient);
	
	m_rectCaption.top	 = 0;
	m_rectCaption.left   = 0;
	m_rectCaption.right  = rc.Width();
	if ( IsIconic() )
	{
		m_rectCaption.bottom	=	rc.Height();

		m_rectLeftBoard.SetRectEmpty();
		m_rectRightBoard.SetRectEmpty();
		m_rectBottomBoard.SetRectEmpty();
	}
	else
	{
		m_rectCaption.bottom = m_rectCaption.top + (rcClient.top - rc.top);
		
		// 左边框	
		m_rectLeftBoard.left  = m_rectCaption.left;
		m_rectLeftBoard.right = m_rectCaption.left + rcClient.left - rc.left;
		m_rectLeftBoard.top	  = m_rectCaption.bottom;
		m_rectLeftBoard.bottom= m_rectCaption.bottom + rcClient.Height();
		
		// 右边框	
		m_rectRightBoard.right = m_rectCaption.right;
		m_rectRightBoard.left  = m_rectLeftBoard.right + rcClient.Width();
		m_rectRightBoard.top   = m_rectLeftBoard.top;
		m_rectRightBoard.bottom= m_rectLeftBoard.bottom;
		
		// 底边框	
		m_rectBottomBoard = m_rectCaption;
		m_rectBottomBoard.top = m_rectCaption.bottom + rcClient.Height();
		m_rectBottomBoard.bottom	=	m_rectBottomBoard.top + rc.bottom - rcClient.bottom;
	}
//#BUG_MARK_END [NO=XL0002]

	// 按钮
	int iButtonWidth  = 18;
	int iButtonHeigth = 18;
	
	m_rectClose = m_rectCaption;

	m_rectClose.right -= 5;
	//m_rectClose.top   += 4; //使用下面的替换固定值
	int	iBtnTop	=	m_rectClose.top + (m_rectCaption.Height() - iButtonHeigth)/2;
	if ( iBtnTop > m_rectClose.top )
	{
		//如果有多余的高度，则至中
		m_rectClose.top += iBtnTop;
	}
	m_rectClose.left   = m_rectClose.right - iButtonWidth;
	m_rectClose.bottom = m_rectClose.top   + iButtonHeigth;
}

void CPropertySheetEX::OnNcPaint() 
{
	CWindowDC dc(this);	
	HDC hDC = dc.GetSafeHdc();
	int32 iStretchMode = dc.SetStretchBltMode(COLORONCOLOR);  //win经典视图下，中间有个黑线，调整stretchblt模式0001797
	
	if ( NULL == hDC )
	{
		return;
	}

	// 设置clipRgn，防止闪烁
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	ScreenToClient(&rectWindow);
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if ( NULL != pThreadState 
		&& pThreadState->m_lastSentMsg.message == WM_NCPAINT
		&& pThreadState->m_lastSentMsg.wParam != 1 )
	{
		// 设置ClipRegion，以防止闪烁
		// 全屏坐标转换为window坐标
		CRect rc;
		GetWindowRect(rc);
		HRGN rgnClip;
		rgnClip = CreateRectRgn(0,0,0,0);
		CombineRgn(rgnClip, (HRGN)pThreadState->m_lastSentMsg.wParam, rgnClip, RGN_COPY);
		OffsetRgn(rgnClip, -rc.left, -rc.top);
		ExtSelectClipRgn(dc.m_hDC, rgnClip, RGN_AND);
		DeleteObject(rgnClip);
	}

	COLORREF	clrSilver = RGB(170,170,170);
	COLORREF    clrWhite  = RGB(230,230,230);
	COLORREF	clrText   = RGB(255,255,0);
	
	if ( !m_bActive )
	{
		clrSilver = RGB(200,200,200);
		clrWhite  = RGB(240,240,240);
		clrText   = RGB(127,127,127);
	}
	CColorStep step(clrWhite, clrSilver, m_rectCaption.Height()+1);
	step.SetColorSmooth(true);
	
	for (int32 i=m_rectCaption.top ; i< m_rectCaption.bottom; i++)
	{	
		dc.FillSolidRect(m_rectCaption.left, i, m_rectCaption.Width(), 1, step.NextColor());
	}
	
	if ( !m_bNcMouseMoving )
	{
		//step.ResetStep();			
		for (int32 i=m_rectLeftBoard.left ; i< m_rectLeftBoard.right; i++)
		{	
			dc.FillSolidRect(i, m_rectLeftBoard.top, 1, m_rectLeftBoard.Height(), step.GetColor());
		}
		
		// 右边框
		//step.ResetStep();
		for (int32 i=m_rectRightBoard.left ; i<m_rectRightBoard.right; i++)
		{
			dc.FillSolidRect(i, m_rectRightBoard.top, 1, m_rectRightBoard.Height(), step.GetColor());
		}
		
		// 底边框
		for (int32 i=m_rectBottomBoard.top ; i<m_rectBottomBoard.bottom; i++)
		{
			dc.FillSolidRect(m_rectBottomBoard.left, i, m_rectBottomBoard.Width(), 1, step.NextColor());
		}
	}

	HDC hdc = dc.GetSafeHdc();		
	Graphics GraphicsButtons(hdc);

	// 关闭按钮
	DrawSCButtons(dc, NULL);

	//if ( IsIconic() )
	{
		HRGN hRgn = CreateRectRgn(0,0,0,0);
		int iErr = GetWindowRgn(hRgn);
		if ( ERROR != iErr && NULLREGION != iErr )
		{
			CBrush brush;
			brush.CreateSolidBrush(clrSilver);
			FrameRgn(dc.m_hDC, hRgn, (HBRUSH)brush.m_hObject, 1, 1);
		}
		DeleteObject(hRgn);
	}

	// 标题文字
	CString StrWinText;
	GetWindowText(StrWinText);
	
	CRect rectText;
	rectText = m_rectCaption;
	
	rectText.left += 5;	
	rectText.top  += 8;

	LOGFONT lg;
	memset(&lg,0,sizeof(LOGFONT));
	_tcscpy(lg.lfFaceName,L"宋体");  

   	lg.lfHeight  = -14;
   	lg.lfWeight  = 580;
   	lg.lfCharSet = 0;
 	lg.lfOutPrecision = 3;

	CFont font;
	font.CreateFontIndirect(&lg);
	CFont * pOldFont = dc.SelectObject(&font);
	
	dc.SetBkMode(TRANSPARENT);
	
	dc.SetTextColor(clrText);	
	
	dc.DrawText(StrWinText,rectText,DT_LEFT);	
	
	dc.SelectObject(pOldFont);
	dc.SetStretchBltMode(iStretchMode);
	font.DeleteObject();
}

int CPropertySheetEX::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(WS_SYSMENU,0);
	return 0;
}

BOOL CPropertySheetEX::OnNcActivate(BOOL bActive) 
{
	// TODO: Add your message handler code here and/or call default	
	m_bNcMouseMoving = false;
	m_bActive = bActive;
	OnNcPaint();
	//return CPropertySheet::OnNcActivate(bActive);
	return true;
}

void CPropertySheetEX::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	CPoint	pointTemp	=	point;
	ScreenToClient(&pointTemp);
	if (!IsIconic())
	{
		pointTemp.y += m_rectCaption.Height();
	}
	pointTemp.x += m_rectLeftBoard.Width();

	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	将m_bNcMouseMoving为false，让OnNcPaint正确绘制标题栏
	E_CurrentShowState	eTemp	=	m_eCurrentShowState;
	m_bNcMouseMoving	=	false;

	TRACKMOUSEEVENT	trackMouse	=	{0};
	trackMouse.cbSize		=	sizeof(trackMouse);
	trackMouse.dwFlags		=	GGT_TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT是加上ncclient的意思
	trackMouse.hwndTrack	=	m_hWnd;
	BOOL bMouseTrack = _TrackMouseEvent( &trackMouse );
	ASSERT( bMouseTrack );
	//#BUG_MARK_END [NO=XL0002]

	if (m_rectClose.PtInRect(pointTemp))
	{
		m_bNcMouseMoving = true;

// 		KillTimer(KTimerIdRefreshPropertySheetCaption);
// 		SetTimer(KTimerIdRefreshPropertySheetCaption, KTimerPeriodRefreshPropertySheetCaption, NULL);

		m_eCurrentShowState = ECSSForcusClose;		
	}
	else
	{
		m_eCurrentShowState = ECSSNormal;		
	}
	
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR= DATE=2010/04/10]
	//DESCRIPTION:	节约绘图
	if (eTemp != m_eCurrentShowState)
	{
		OnNcPaint();
	}
	//OnNcPaint();
	//#BUG_MARK_END [NO=XL0002]
	
	CPropertySheet::OnNcMouseMove(nHitTest, point);
}

LRESULT CPropertySheetEX::OnNcHitTest(CPoint point)
{
	CPoint	pointTemp	=	point;
	ScreenToClient(&pointTemp);
	if ( !IsIconic() )
	{
		pointTemp.y	+=	m_rectCaption.Height();
	}
	pointTemp.x	+=	m_rectLeftBoard.left;

	UINT uHT	=	HTNOWHERE;
	if ( m_rectClose.PtInRect(pointTemp) )
	{
		uHT = HTCLOSE;
	}
	else
	{
		uHT	= CPropertySheet::OnNcHitTest(point);
		if ( HTCLOSE == uHT)
		{
			uHT = HTCAPTION;
		}
	}
	return uHT;
}


//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/13]
//DESCRIPTION:	
LRESULT CPropertySheetEX::OnNcMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//清除所有显示的标志
	
	if ( m_bNcMouseMoving || ECSSNormal != m_eCurrentShowState )
	{
		m_bNcMouseMoving = false;
		m_eCurrentShowState	=	ECSSNormal;
		SendMessage(WM_NCPAINT, 0, 0);
	}
	
	//TRACE(_T("NcMouseLeave\r\n"));
	//CMDIChildWnd::OnNcMouseLeave();
	
	return	Default();
}
//#BUG_MARK_END [NO=XL0002]

void CPropertySheetEX::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{		
	CPoint pt = point;
	ScreenToClient(&pt);

	if (!IsIconic())
	{
		pt.y += m_rectCaption.Height();
	}

	pt.x += m_rectLeftBoard.Width();

	E_CurrentShowState eTempShowState = m_eCurrentShowState;
	m_bNcMouseMoving	=	false;

	if (m_rectClose.PtInRect(pt))
	{		
		// SendMessage(WM_DESTROY,0,0);
		//SendMessage(WM_COMMAND,IDOK,0);
		//return;
		m_eCurrentShowState	=	ECSSPressClose;
		m_bNcMouseMoving	=	true;
	}

	if ( eTempShowState != m_eCurrentShowState )
	{
		SendMessage(WM_NCPAINT, 0, 0);
	}

	if ( HTCLOSE == nHitTest || HTMAXBUTTON == nHitTest || HTMINBUTTON == nHitTest )
	{
		return;
	}
	
	CPropertySheet::OnNcLButtonDown(nHitTest, point);
}

void CPropertySheetEX::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	CWindowDC dc(this);

	CPoint	pointTemp	=	point;
	ScreenToClient(&pointTemp);
	if (!IsIconic())
	{
		pointTemp.y += m_rectCaption.Height();
	}
	pointTemp.x	+=	m_rectLeftBoard.Width();

	//重置一下就可以了，因为只有WM_CLOSE发送
	m_eCurrentShowState = ECSSNormal;
	m_bNcMouseMoving	=	false;

	if (m_rectClose.PtInRect(pointTemp))
	{
		//PostMessage(WM_CLOSE, 0,0);
		SendMessage(WM_SYSCOMMAND, SC_CLOSE,0);
		//貌似sheet WM_CLOSE, 发送了有响应，但是不关闭 ... 发送SC_CLOSE吧
	}

	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	貌似可以不用主动绘制
	//OnNcPaint();
	//#BUG_MARK_END [NO=XL0002]
	
	CPropertySheet::OnNcLButtonUp(nHitTest,point);
}

void CPropertySheetEX::SetWindowTextEx(const CString& StrText)
{
	SetWindowText(StrText);
	SendMessage(WM_NCPAINT, 0, 0);
}

void CPropertySheetEX::OnTimer(UINT nIDEvent)
{
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	暂时没有必要调用定时器来绘制
	if ( nIDEvent == KTimerIdRefreshPropertySheetCaption )
	{
		KillTimer(KTimerIdRefreshPropertySheetCaption);
		// 		
		// 		m_eCurrentShowState = ECSSNormal;
		// 		m_bNcMouseMoving	= false;
		// 		
		// 		SendMessage(WM_NCPAINT, 0, 0);
	}
	//#BUG_MARK_END [NO=XL0002]
}

HBRUSH CPropertySheetEX::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertySheet::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetTextColor(RGB(0,0,255));
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CPropertySheetEX::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertySheet::OnClose();
}

void CPropertySheetEX::DrawSCButtons( CWindowDC &dc, Graphics *pGraphics )
{
	CRect rcBtn = m_rectClose;

	CFont MyFont;
	LOGFONT logfont;
	logfont.lfHeight = -rcBtn.Height();
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_NORMAL;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	
	_tcscpy(logfont.lfFaceName,_T("Marlett"));
	
	MyFont.CreateFontIndirect (&logfont);
	
	CFont* pOldFont = dc.SelectObject (&MyFont); 
	int OldMode = dc.SetBkMode(TRANSPARENT);  
	COLORREF clrOld = dc.SetTextColor(RGB(0,0,0));
	
	

	TCHAR chMin, chMax, chClo, chRestore;
	chMin = 48;
	chMax = 49;
	chRestore = 50;
	chClo = 114;
	
	// 关闭按钮
	if ( NULL != pGraphics )
	{
		if ( ECSSNormal == m_eCurrentShowState)
		{
			//TRACE(_T("Draw normal close\r\n"));
			DrawImage(*pGraphics, m_pImageButtons, m_rectClose, 12, 3, false);
		}
		else if ( ECSSForcusClose == m_eCurrentShowState)
		{
			//TRACE(_T("Draw focus close\r\n"));
			DrawImage(*pGraphics, m_pImageButtons, m_rectClose, 12, 7, false);
		}
		else if ( ECSSPressClose == m_eCurrentShowState )
		{
			//TRACE(_T("Draw press close\r\n"));
			DrawImage(*pGraphics, m_pImageButtons, m_rectClose, 12, 11, false);
		}
	}
	else
	{
		COLORREF clrText = RGB(90,90,70);
		rcBtn = m_rectClose;
		if ( ECSSForcusClose == m_eCurrentShowState)
		{
			clrText = RGB(127,0,0);
		}
		else if ( ECSSPressClose == m_eCurrentShowState )
		{
			clrText = RGB(127,0,0);
			rcBtn.left += 2;
			rcBtn.top += 2;
		}
		dc.SetTextColor(clrText);
		dc.DrawText(&chClo, 1, rcBtn, DT_VCENTER| DT_CENTER| DT_SINGLELINE);
	}
	
	

	dc.SetTextColor(clrOld);
	dc.SetBkMode(OldMode);
	dc.SelectObject(pOldFont);
}

// DialogEx.cpp : implementation file
//
#include "stdafx.h"
#include "DialogEx.h"
//#include "facescheme.h"
#include "resource.h"
#include "GdiPlusTS.h"
#include "ShareFun.h"
		//XL0002

#include "ColorStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT KTimerIdRefreshDlgCaption	  = 123454;
const UINT KTimerPeriodRefreshDlgCaption  = 200;

/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog
IMPLEMENT_DYNCREATE(CDialogEx, CDialog)

CDialogEx::CDialogEx(UINT nIDTemplate,CWnd * pParent):CDialog(nIDTemplate,pParent)
{
	Construct();
}

CDialogEx::CDialogEx()
{
	Construct();
}

CDialogEx::~CDialogEx()
{
	DEL(m_pImageButtons);
	DEL(m_pImageCatptionBig);
	DEL(m_pImageCatptionSmall);
	DeleteObject(m_BrushBack);
	DeleteObject(m_BrushEditBack);
}

void CDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEx)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//XL0002
BEGIN_MESSAGE_MAP(CDialogEx, CDialog)
	//{{AFX_MSG_MAP(CDialogEx)
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(GGT_WM_NCMOUSELEAVE, OnNcMouseLeave)
	ON_MESSAGE(WM_SETTEXT, OnSetWindowText)
	ON_MESSAGE(WM_GETTEXT, OnGetWindowText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetWindowTextLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEx message handlers

void CDialogEx::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CalcNcSize();

	// 是否调用RedrawWindow?
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_NOERASE);
	OnNcPaint();
}

void CDialogEx::OnClose()
{
	OnClickClose();	
	CDialog::OnClose();
}

void CDialogEx::CalcNcSize()
{
	DWORD dwStyle = GetExStyle();

	if (dwStyle & WS_EX_TOOLWINDOW)
	{
		m_bToolWindow    = true;
		m_bOnlyShowClose = true;
	}
	else
	{
		m_bToolWindow	 = false;			
	}

	DWORD	dwStyle2	=	GetStyle();
	if ( (WS_MAXIMIZEBOX & dwStyle2) || (WS_MINIMIZEBOX & dwStyle2) )
	{
		m_bOnlyShowClose	=	false;
		if ( WS_MAXIMIZEBOX & dwStyle2 )
		{
			m_bHaveMaxBox	=	true;
		}
		else
		{
			m_bHaveMaxBox	=	false;
		}
	}
	else
	{
		m_bOnlyShowClose	=	true;
	}
		
	CRect rc;
	GetWindowRect(rc);

	CRect	rcClient;
	GetClientRect(rcClient);
	ClientToScreen(&rcClient);
	
	m_rectCaption.top	 = 0;
	m_rectCaption.left   = 0;
	m_rectCaption.right  = rc.Width();
	if ( IsIconic() )
	{
		//最小化时，全部都是Nc Area
		m_rectCaption.bottom = rc.Height();
		
		m_rectLeftBoard.SetRectEmpty();
		
		// 右边框	
		m_rectRightBoard.SetRectEmpty();
		
		// 底边框
		m_rectBottomBoard.SetRectEmpty();
	}
	else
	{
		m_rectCaption.bottom = m_rectCaption.top + (rcClient.top - rc.top);
		
		
		// 左边框
		
		m_rectLeftBoard.left  = m_rectCaption.left;
		m_rectLeftBoard.right = m_rectCaption.left + rcClient.left - rc.left;
		m_rectLeftBoard.top	  = m_rectCaption.top;
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

	// 按钮 - 是否需要固定大小？
	int iButtonWidth  = 18;
	int iButtonHeigth = 18;
	if ( m_bToolWindow )
	{
		iButtonHeigth = iButtonWidth = 15;
	}

	int iCaptionH = m_rectCaption.Height();
	iButtonHeigth = min(iCaptionH, iButtonHeigth);
	iButtonHeigth = max(0, iButtonHeigth);
	
	m_rectClose = m_rectCaption;
	m_rectClose.right -= 5;
	
	int	iBtnTop	=	m_rectClose.top + (m_rectCaption.Height() - iButtonHeigth)/2;
	if ( iBtnTop > m_rectClose.top )
	{
		//如果有多余的高度，则至中
		m_rectClose.top += iBtnTop;
	}
	//#BUG_MARK_END [NO=XL0002]
	
	m_rectClose.left   = m_rectClose.right - iButtonWidth;
	m_rectClose.bottom = m_rectClose.top   + iButtonHeigth;

	//  ...fangz1012 所有对话框都只要关闭按钮
	//	...xiali0413 可以根据MIN/MAX BOX的值来调节是否显示 MIN/MAX BOX
 	if ( !m_bToolWindow && !m_bOnlyShowClose)
 	{
 		m_rectMax		 = m_rectClose;
 		m_rectMax.right -= iButtonWidth;
 		m_rectMax.left  -= iButtonWidth;
 		
 		m_rectMin		 = m_rectMax;
 		m_rectMin.right -= iButtonWidth;
 		m_rectMin.left  -= iButtonWidth;
 	}
 	else
 	{
		m_rectMax.SetRectEmpty();
		m_rectMin.SetRectEmpty();
	}
}

void CDialogEx::OnNcPaint() 
{
	HWND hWnd = this->GetSafeHwnd();
	if ( !::IsWindow(hWnd) )
	{
		return;
	}
	

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

	DrawMyNcPaint(dc);
	
	dc.SetStretchBltMode(iStretchMode);
}

int CDialogEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nFrameWidth = 1;//GetSystemMetrics(SM_CXFRAME)-GetSystemMetrics(SM_CXBORDER);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	rcWnd.left -= m_nFrameWidth;
	rcWnd.right += m_nFrameWidth;
	rcWnd.bottom += 6;//15;
	MoveWindow(rcWnd);

	ModifyStyle(WS_CAPTION,0);
	ModifyStyle(WS_SYSMENU,0);
	//ModifyStyle(WS_BORDER,0);
	
	return 0;
}

BOOL CDialogEx::OnNcActivate(BOOL bActive) 
{
	// TODO: Add your message handler code here and/or call default	
//	m_bActive = bActive;
	m_bNcMouseMoving = false;
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	由于事实上这个对话框暂时没有activate与deactivate区别 - 可能看不出来呵，后面可能有比较大区别, 还是NcPaint吧
//	OnNcPaint();
	//SendMessage(WM_NCPAINT, 0, 0);
	//#BUG_MARK_END [NO=XL0002]
	//return CDialog::OnNcActivate(bActive);
	return true;
}

void CDialogEx::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	ScreenToClient(&point);
	//if (!IsIconic())
	//{
		point.y += m_rectCaption.Height();
	//}
	point.x += m_rectLeftBoard.Width();

	//TRACE(_T("NC MouseMove: chg:%d,%d   org:%d,%d\r\n"), point.x, point.y, po2.x, po2.y);
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	保存当前显示标志，避免重绘
	//				m_bNcMouseMoving重置false，如果需要，赋值true
	E_CurrentShowState	eTemp	=	m_eCurrentShowState;
	m_eCurrentShowState	=	ECSSNormal;
	m_bNcMouseMoving	=	false;

	TRACKMOUSEEVENT	trackMouse	=	{0};
	trackMouse.cbSize		=	sizeof(trackMouse);
	trackMouse.dwFlags		=	GGT_TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT是加上ncclient的意思
	trackMouse.hwndTrack	=	m_hWnd;
	BOOL bMouseTrack = _TrackMouseEvent( &trackMouse );
	ASSERT( bMouseTrack );
	
	DWORD dwStyle	=	GetStyle();
	//#BUG_MARK_END [NO=XL0002]

	if (m_rectClose.PtInRect(point))
	{
		//TRACE(_T("NC MouseMove: close\r\n"));
		m_eCurrentShowState = ECSSForcusClose;
		m_bNcMouseMoving	=	true;
	}
	else if ( m_rectMax.PtInRect(point))
	{
		if ( !m_bToolWindow && (WS_MAXIMIZEBOX & dwStyle) )	//使用style来控制显示 - 非ToolWindow && MaxBox选择了
		{		
			m_eCurrentShowState = ECSSForcusMax;
			m_bNcMouseMoving	=	true;
			//TRACE(_T("NC MouseMove: max\r\n"));
		}
	}
	else if ( m_rectMin.PtInRect(point))
	{
		if ( !m_bToolWindow && (WS_MINIMIZEBOX & dwStyle)) //非ToolWindow && MinBox选择了
		{
			m_eCurrentShowState = ECSSForcusMin;
			m_bNcMouseMoving	=	true;
			//TRACE(_T("NC MouseMove: min\r\n"));
		}
	}
	else
	{
		//移出去了
		//TRACE(_T("NC MouseMove: none\r\n"));
		m_eCurrentShowState = ECSSNormal;		
		m_bNcMouseMoving	=	false;
	}
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	重绘，有改变
	if (eTemp != m_eCurrentShowState)
	{
		CRect rcBtn(m_rectCaption);
		rcBtn.left = m_rectMin.left - 5;
		CRect rcWin(0,0,0,0);
		GetWindowRect(rcWin);
		rcBtn.OffsetRect(rcWin.TopLeft());
		ScreenToClient(&rcBtn);
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcBtn);
		RedrawWindow(NULL, &rgn, RDW_FRAME |RDW_INVALIDATE |RDW_NOINTERNALPAINT |RDW_UPDATENOW);
		//OnNcPaint();
	}
	//#BUG_MARK_END [NO=XL0002]

	CDialog::OnNcMouseMove(nHitTest, point);
}


//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/13]
//DESCRIPTION:	
LRESULT CDialogEx::OnNcMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//清除所有显示的标志
	
	if ( m_bNcMouseMoving || ECSSNormal != m_eCurrentShowState )
	{
		//如果先前有focus标记的话 或者 不是常规状态，都需要重置显示状态
		//其实可以改变m_bNcMouseMoving的意义为控制NcPaint是否需要刷新全部显示？ - 暂不改变，全部重绘吧
		m_bNcMouseMoving = false;
		m_eCurrentShowState	=	ECSSNormal;
		OnNcPaint();
		//SendMessage(WM_NCPAINT, 0, 0);
	}
	
	//TRACE(_T("NcMouseLeave\r\n"));
	//CMDIChildWnd::OnNcMouseLeave();
	
	return	Default();
}
//#BUG_MARK_END [NO=XL0002]

void CDialogEx::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{	
	HWND hWnd = this->GetSafeHwnd();
	if ( !::IsWindow(hWnd) )
	{
		return;
	}

	CPoint pt = point;
	ScreenToClient(&pt);

	//if (!IsIconic())
	//{
		pt.y += m_rectCaption.Height();
	//}

	pt.x += m_rectLeftBoard.Width();

	E_CurrentShowState	eShowStateTemp	=	m_eCurrentShowState;
	m_bNcMouseMoving	=	true;
	
	//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/13]
	//DESCRIPTION:	使用style控制
	DWORD dwStyle	=	GetStyle();
	//#BUG_MARK_END [NO=XL00002]

	//按理：应该把鼠标L按下与鼠标L弹起标记做响应的记录，只有在同一个按钮下的按下与弹起理解为一个button command(Windows标准按钮的表现) - 暂未实现
	//现在实现是判断NcLButtonUp
	//参数nHitTest是否有用？- i don't know
	if (m_rectClose.PtInRect(pt))
	{
		//Close以down为开始 - 还是Up吧
// 		SendMessage(WM_CLOSE,0,0);		
// 		return;
		m_eCurrentShowState = ECSSPressClose;
	}
	else if ( m_rectMax.PtInRect(pt) && !m_bToolWindow )
	{
		//非ToolWindo && MaxBox, 准备响应该press
		if ( (WS_MAXIMIZEBOX & dwStyle) )
		{
			m_eCurrentShowState = ECSSPressMax;									
		}
	}
	else if ( m_rectMin.PtInRect(pt) && !m_bToolWindow)
	{
		//非ToolWindow && MinBox，
		if ( (WS_MINIMIZEBOX & dwStyle) )
		{
			m_eCurrentShowState = ECSSPressMin;				
		}
	}
	else
	{
		//没有按到任何有用的
		m_bNcMouseMoving	=	false;
	}
	
	if ( eShowStateTemp != m_eCurrentShowState )
	{
		OnNcPaint();
		//SendMessage(WM_NCPAINT, 0, 0);
	}

	if ( ECSSNormal != m_eCurrentShowState )
	{
		//不能让基类系统获取该消息，因为系统会画它自己的按钮
		return;
	}
	CDialog::OnNcLButtonDown(nHitTest, point);	
}


//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/13]
//DESCRIPTION:	
LRESULT CDialogEx::OnNcHitTest(CPoint point)
{
	CPoint	pointTemp	=	point;
	ScreenToClient(&pointTemp);
	//if (!IsIconic())
	//{
		pointTemp.y += m_rectCaption.Height();
	//}
	pointTemp.x += m_rectLeftBoard.Width();

	UINT uHT	=	HTNOWHERE;
	if ( m_rectClose.PtInRect(pointTemp) )
	{
		uHT	=	HTCLOSE;
	}
	else if ( m_rectMax.PtInRect(pointTemp) )
	{
		uHT	=	HTMAXBUTTON;
	}
	else if ( m_rectMin.PtInRect(pointTemp) )
	{
		uHT	=	HTMINBUTTON;
	}
	else if ( m_rectCaption.PtInRect(pointTemp) )
	{
		uHT	=	HTCAPTION;
	}
	else
	{
		//让基类自己说在哪里吧
		uHT	=	CDialog::OnNcHitTest(point);
		//基类不能决定HTClose,HTMaxButton,HTMinButton位置, 不能弹出菜单，因为菜单风格不一致
		if ( HTMINBUTTON==uHT || HTMAXBUTTON==uHT || HTCLOSE==uHT || HTSYSMENU==uHT )
		{
			uHT	=	HTCAPTION;	//是caption还是nowhere ?
		}
	}

	return uHT;
}
//#BUG_MARK_END [NO=XL0002]

void CDialogEx::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	HWND hWnd = this->GetSafeHwnd();
	if ( !::IsWindow(hWnd) )
	{
		return;
	}

	CWindowDC dc(this);
	
	CPoint	pointTemp	=	point;
	ScreenToClient(&pointTemp);
	//if (!IsIconic())
	//{
		pointTemp.y += m_rectCaption.Height();
	//}

	pointTemp.x	+=	m_rectLeftBoard.Width();

	DWORD	dwStyle	=	GetStyle();
	//下次如果系统觉得有必要绘制NcPaint，则会全部绘制，设置m_bNcMouseMoving = false;
	m_bNcMouseMoving	=	false;

	if ( m_rectMax.PtInRect(pointTemp) && !m_bToolWindow && (WS_MAXIMIZEBOX & dwStyle) )
	{
		if ( IsZoomed() )
		{
			PostMessage(WM_SYSCOMMAND,SC_RESTORE,0);
		}
		else
		{			
			PostMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);			
		}		
	}
	else if ( m_rectMin.PtInRect(pointTemp) && !m_bToolWindow && (WS_MINIMIZEBOX & dwStyle) )
	{
		// 最小化的时候,整个程序都最小化了 - 根据有没有TopMost标志来看
		if ( IsIconic() )
		{
			PostMessage(WM_SYSCOMMAND,SC_RESTORE,0);
		}
		else
		{			
			PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);			
		}
	}
	else if ( m_rectClose.PtInRect(pointTemp) )
	{
		PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	}
	else
	{
		CDialog::OnNcLButtonUp(nHitTest,point);
	}
}

void CDialogEx::SetWindowTextEx(const CString& StrText)
{
	SetWindowText(StrText);
	SendMessage(WM_NCPAINT, 0, 0);
}

void CDialogEx::OnTimer(UINT nIDEvent)
{


 	if ( nIDEvent == KTimerIdRefreshDlgCaption )
 	{
		KillTimer(KTimerIdRefreshDlgCaption);
		//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/10]
		//DESCRIPTION:	如果没有绘制问题，可以取消该Timer响应
		// 		
		// 		m_eCurrentShowState = ECSSNormal;
		// 		m_bNcMouseMoving	= false;
		// 		
		// 		SendMessage(WM_NCPAINT, 0, 0);
		//#BUG_MARK_END [NO=XL0002]
 	}

}

void CDialogEx::OnPaint() 
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(rcClient,GetBackColor());
}

HBRUSH CDialogEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC && m_bNeedColorText)
	{	
		pDC->SetTextColor(RGB(0,0,255));
	}

	if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_BrushEditBack;
	}

	if (CTLCOLOR_LISTBOX != nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_BrushBack;
	}
	
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);;
}

void CDialogEx::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp )
{
//	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);

	DWORD dwExStyle = GetExStyle();
	if ( !(dwExStyle & WS_EX_TOOLWINDOW) )
	{ 
	}  

	CRect &rc  = (CRect&)lpncsp->rgrc[0]; 
	rc.top += 30;//(TITLE_HEIGHT + m_nFrameWidth);
	rc.left   += m_nFrameWidth;   
	rc.bottom -= m_nFrameWidth;   
	rc.right  -= m_nFrameWidth; 
}

void CDialogEx::DrawNcClient( CWindowDC &dc )
{
	// clientArea是肯定要排除在外的 - 一切以dc为CWindowDC为准，一切所需要的面积都已经算好了(base window org 0)
	int iSave = dc.SaveDC();
	
	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(&rcClient);
	CRgn rgnClient;
	rgnClient.CreateRectRgnIndirect(rcClient);
	dc.SelectClipRgn(&rgnClient, RGN_DIFF);

	CRect rcWin;
	GetWindowRect(rcWin);
	rcClient.OffsetRect(-rcWin.left, -rcWin.top);

	dc.RestoreDC(iSave);
}

void CDialogEx::DrawSCButtons( CWindowDC &dc, Graphics *pGraphics )
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
	if ( m_bToolWindow || m_bOnlyShowClose)
	{
		// 关闭按钮
		//vista下没有看见X图标，不知道是否与gdi+的drawimage有关？是否需要手动绘制？xiali $DATE=2010/04/10$ - 目前貌似是对话框没处理好的原因
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
			COLORREF clrText =  RGB(255,255,255);//RGB(90,90,70);
			if(m_bNeedChangeColor)
			{
				if(!m_bNeedDrawSpliter)
				{
					clrText = RGB(32,28,28);
				}	
			}
			rcBtn = m_rectClose;
			if ( ECSSForcusClose == m_eCurrentShowState)
			{
				if(m_bNeedChangeColor)
				{
					if(!m_bNeedDrawSpliter)
					{
					   clrText = RGB(80,69,69);
					}
					else
					{
						clrText = RGB(255, 255, 255);
					}
				}
				else
				{
					clrText = RGB(127,0,0);
				}			
			}
			else if ( ECSSPressClose == m_eCurrentShowState )
			{
				if(m_bNeedChangeColor)
				{
					if(!m_bNeedDrawSpliter)
					{
						clrText = RGB(80,69,69);
					}
					else
					{
						clrText = RGB(255, 255, 255);
					}
				}
				else
				{
					clrText = RGB(127,0,0);
				}
				rcBtn.left += 2;
				rcBtn.top += 2;
			}

			dc.SetTextColor(clrText);
			dc.DrawText(&chClo, 1, rcBtn, DT_VCENTER| DT_CENTER| DT_SINGLELINE);
		}
	}
	else
	{
		
		// 正常:
		// 后面如果要添加 Disable 状态的话，需要修改这里代码以支持
		int		iCloseIndex	=	3;	//normal close
		int		iMaxIndex	=	1;	//normal max
		int		iMinIndex	=	0;	//normal min

		COLORREF clrMin, clrMax, clrClo;
		if(m_bNeedChangeColor)
		{
			clrMin = clrMax = clrClo = RGB(32,28,28);
		}
		else
		{
			clrMin = clrMax = clrClo = RGB(90,90,70);
		}
		
		CRect rcMin(m_rectMin), rcMax(m_rectMax), rcClo(m_rectClose);
		COLORREF clrFocus = RGB(235,235,235);
		COLORREF clrDisable = GetSysColor(COLOR_GRAYTEXT);

		DWORD dwStyle = GetStyle();
		if ( !(dwStyle&WS_MAXIMIZEBOX) )
		{
			clrMax = clrDisable;
		}
		if ( !(dwStyle&WS_MINIMIZEBOX) )
		{
			clrMin = clrDisable;
		}
		
		if ( IsIconic() )
		{
			//最小了
			iMinIndex	=	2;	//恢复
			chMin = chRestore;
		}
		else if ( IsZoomed() )
		{
			iMaxIndex	=	2;	//恢复
			chMax = chRestore;
		}
		
		if ( ECSSNormal == m_eCurrentShowState)
		{		
		}
		else if ( ECSSForcusClose == m_eCurrentShowState)
		{
			iCloseIndex	+=	4;		//focus 占第二组, based 第一组
			if(m_bNeedChangeColor)
			{
				if(!m_bNeedDrawSpliter)
				{
					clrClo = RGB(80,69,69);
				}
				else
				{
					clrClo = RGB(255, 255, 255);
				}
			}
			else
			{
				clrClo = RGB(127,0,0);
			}
		}
		else if ( ECSSPressClose == m_eCurrentShowState )
		{
			iCloseIndex	+=	4*2;	//press第三组
			if(m_bNeedChangeColor)
			{
				if(!m_bNeedDrawSpliter)
				{
					clrClo = RGB(80,69,69);
				}
				else
				{
					clrClo = RGB(255, 255, 255);
				}
			}
			else
			{
				clrClo = RGB(127,0,0);
			}
			rcClo.left += 2;
			rcClo.top += 2;
		}
		else if ( ECSSForcusMax == m_eCurrentShowState)
		{
			iMaxIndex	+= 4;
			clrMax = clrFocus;
		}
		else if ( ECSSPressMax == m_eCurrentShowState)
		{
			iMaxIndex	+=	4*2;
			clrMax = clrFocus;
			rcMax.left += 2;
			rcMax.top += 2;
		}
		else if ( ECSSForcusMin == m_eCurrentShowState)
		{
			iMinIndex	+=	4;
			clrMin = clrFocus;
		}
		else if ( ECSSPressMin == m_eCurrentShowState)
		{
			iMinIndex	+=	4*2;
			clrMin = clrFocus;
			rcMin.left += 2;
			rcMin.top += 2;
		}

		if ( NULL != pGraphics )
		{
			DrawImage(*pGraphics, m_pImageButtons, m_rectClose, 12, iCloseIndex, false);
			DrawImage(*pGraphics, m_pImageButtons, m_rectMax, 12, iMaxIndex, false);
			DrawImage(*pGraphics, m_pImageButtons, m_rectMin, 12, iMinIndex, false);
		}
		else
		{
			dc.SetTextColor(clrClo);
			dc.DrawText(&chClo, 1, rcClo, DT_VCENTER| DT_CENTER| DT_SINGLELINE);
			dc.SetTextColor(clrMax);
			dc.DrawText(&chMax, 1, rcMax, DT_VCENTER| DT_CENTER| DT_SINGLELINE);
			dc.SetTextColor(clrMin);
			dc.DrawText(&chMin, 1, rcMin, DT_VCENTER| DT_CENTER| DT_SINGLELINE);
		}

		
	}

	dc.SetTextColor(clrOld);
	dc.SetBkMode(OldMode);
	dc.SelectObject(pOldFont);
	MyFont.DeleteObject();
}

LRESULT CDialogEx::OnSetWindowText( WPARAM w, LPARAM l )
{
	LPCTSTR	pBuf = (LPCTSTR)l;
	m_StrWindowText.Empty();
	if ( NULL != pBuf )
	{
		m_StrWindowText = pBuf;
	} 
	m_bInitializedText = true;		// 已经获取了实际的文字
	RedrawWindow(NULL, NULL, RDW_FRAME |RDW_INVALIDATE |RDW_NOINTERNALPAINT |RDW_UPDATENOW);
	return TRUE;
}


LRESULT CDialogEx::OnGetWindowText( WPARAM w, LPARAM l )
{
	if ( !m_bInitializedText )
	{
		return Default();
	}
	
	TCHAR *pBuf = (TCHAR *)l;
	int32  iLen = (int32)w;
	if ( NULL != pBuf && iLen > 0 )
	{
		int32 iTextLength = m_StrWindowText.GetLength();
		int32 iCopyLen = min(iLen-1, iTextLength);
		if ( iCopyLen > 0 )
		{
			_tcsncpy(pBuf, m_StrWindowText, iCopyLen);
		}
		pBuf[iCopyLen] = _T('\0');
		return iCopyLen;
	}
	return 0;
}

LRESULT CDialogEx::OnGetWindowTextLength( WPARAM w, LPARAM l )
{
	if ( !m_bInitializedText )
	{
		return Default();
	}
	return m_StrWindowText.GetLength();
}

void CDialogEx::Construct()
{
	m_rectMin		= CRect(0,0,0,0);
	m_rectMax		= CRect(0,0,0,0);
	m_rectClose		= CRect(0,0,0,0);
	m_rectCaption	= CRect(0,0,0,0);
	
	m_rectLeftBoard  = CRect(0,0,0,0);
	m_rectRightBoard = CRect(0,0,0,0);
	m_rectBottomBoard= CRect(0,0,0,0);
	
	m_bNcMouseMoving= false;
	m_bToolWindow	= false;
	m_bHaveMaxBox   = false;
	m_bNeedColorText= true;
	
	m_bOnlyShowClose= true;

	m_bNeedChangeColor = false;
	m_CaptionBKColor = RGB(0, 0, 0);
	m_FrameColor = RGB(0, 0, 0);
	m_CaptionColor = RGB(255, 255, 255);
	m_bNeedDrawSpliter = false;
	
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
	
	m_eCurrentShowState = ECSSNormal;

	m_bInitializedText = false;
	m_bHelpDlg = FALSE;

	COLORREF clrBack = GetBackColor();
	m_BrushBack = CreateSolidBrush(clrBack);
	m_BrushEditBack = CreateSolidBrush(RGB(255,255,255));
}

COLORREF CDialogEx::GetBackColor()
{
    return RGB(225,225,225); 	
}

void CDialogEx::DrawMyNcPaint( CWindowDC &dc )
{
	COLORREF	clrSilver = RGB(44, 50, 55);// 0x3e342c;
	COLORREF    clrWhite  = RGB(44, 50, 55);//0x3e342c;
	if (m_bHelpDlg)
	{
		clrSilver = RGB(83,169,255);
		clrWhite = RGB(83,169,255);
	}

	COLORREF	clrText   = RGB(255,255,255);//RGB(90,63,153);
	if(m_bNeedChangeColor)
	{
		clrText = m_CaptionColor;
	}
	
// 	if ( !m_bActive )
// 	{
// 		clrSilver = RGB(200,200,200);
// 		clrWhite  = RGB(240,240,240);
// 		//clrText   = RGB(255,255,0);
// 		clrText = RGB(127,127,127);
// 	}
	if(m_bNeedChangeColor)
	{
		CColorStep stepChange(m_CaptionBKColor, m_CaptionBKColor, m_rectCaption.Height()+1);
		stepChange.SetColorSmooth(true);

		CColorStep stepFrame(m_FrameColor, m_FrameColor, m_rectCaption.Height()+1);
		stepFrame.SetColorSmooth(true);

		dc.FillSolidRect(m_rectCaption.left , m_rectCaption.top, m_rectCaption.Width(), 1, stepFrame.GetColor());
		for (int32 i=m_rectCaption.top + 1  ; i< m_rectCaption.bottom -1; i++)
		{	
			dc.FillSolidRect(m_rectCaption.left +1 , i, m_rectCaption.Width() - 2, 1, stepChange.NextColor());
		}

		if(m_bNeedDrawSpliter)
		{
			// 分割线
			dc.FillSolidRect(m_rectCaption.left+m_nFrameWidth, m_rectCaption.bottom-1, m_rectCaption.right-2*m_nFrameWidth, 1, stepFrame.GetColor());
			CPen pSolid;
			pSolid.CreatePen(PS_SOLID, 1, stepFrame.GetColor());
			CPen * pOldPen = dc.SelectObject(&pSolid);
			dc.MoveTo(CPoint(m_rectCaption.right - 25, m_rectCaption.top));
			dc.LineTo(CPoint(m_rectCaption.right - 25, m_rectCaption.bottom));
			dc.SelectObject(pOldPen);
		}


		if ( !m_bNcMouseMoving )
		{
			int32 i = 0;
			//step.ResetStep();	
			for (i=m_rectLeftBoard.left ; i< m_rectLeftBoard.right; i++)
			{	
				dc.FillSolidRect(i, m_rectLeftBoard.top, 1, m_rectLeftBoard.Height(), stepFrame.GetColor());
			}

			// 右边框
			//step.ResetStep();
			for ( i=m_rectRightBoard.left ; i<m_rectRightBoard.right; i++)
			{
				dc.FillSolidRect(i, m_rectRightBoard.top, 1, m_rectRightBoard.Height(), stepFrame.GetColor());
			}

			// 底边框
			for ( i=m_rectBottomBoard.top ; i<m_rectBottomBoard.bottom; i++)
			{
				dc.FillSolidRect(m_rectBottomBoard.left, i, m_rectBottomBoard.Width(), 1, stepFrame.NextColor());
			}
		}
	}
	else
	{
		CColorStep step(clrWhite, clrSilver, m_rectCaption.Height()+1);
		step.SetColorSmooth(true);

		CColorStep stepFrame(RGB(74, 75, 77), RGB(74, 75, 77), m_rectCaption.Height()+1);
		stepFrame.SetColorSmooth(true);



		for (int32 i=m_rectCaption.top ; i< m_rectCaption.bottom; i++)
		{	
			dc.FillSolidRect(m_rectCaption.left, i, m_rectCaption.Width(), 1, step.NextColor());
		}

		// 分割线
		dc.FillSolidRect(m_rectCaption.left+m_nFrameWidth, m_rectCaption.bottom-1, m_rectCaption.right-2*m_nFrameWidth, 1, 0x362d26);

		if ( !m_bNcMouseMoving )
		{
			int32 i = 0;
			//step.ResetStep();	
			for (i=m_rectLeftBoard.left ; i< m_rectLeftBoard.right; i++)
			{	
				dc.FillSolidRect(i, m_rectLeftBoard.top, 1, m_rectLeftBoard.Height(), stepFrame.GetColor());
			}

			// 右边框
			//step.ResetStep();
			for ( i=m_rectRightBoard.left ; i<m_rectRightBoard.right; i++)
			{
				dc.FillSolidRect(i, m_rectRightBoard.top, 1, m_rectRightBoard.Height(), stepFrame.GetColor());
			}

			// 底边框
			for ( i=m_rectBottomBoard.top ; i<m_rectBottomBoard.bottom; i++)
			{
				dc.FillSolidRect(m_rectBottomBoard.left, i, m_rectBottomBoard.Width(), 1, stepFrame.NextColor());
			}
		}
	}


	HDC hdc = dc.GetSafeHdc();		
	Graphics GraphicsButtons(hdc);
	
	// 按钮

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

	//////////////////////////////////////////////////////////////////////////
	// 程序图标
	// HICON hIcon =  AfxGetApp()->LoadIcon(IDR_MAINFRAME_SMALL);
	// dc.DrawIcon(0,0,hIcon);
	//////////////////////////////////////////////////////////////////////////

	// 标题文字
	CString StrWinText;
	GetWindowText(StrWinText);
	CRect rectText;
	rectText = m_rectCaption;
	if ( m_rectMin.left <= 0 )
	{
		rectText.right = m_rectClose.left - 5;
	}
	else
	{
		rectText.right = m_rectMin.left - 5;
	}
	rectText.left += 5;
// 	if (m_bToolWindow)
// 	{
// 		rectText.top  += 5;
// 	}
// 	else
// 	{
// 		rectText.top  += 6;
// 	}
	rectText.top = m_rectClose.top + 2;

	LOGFONT lg;
	memset(&lg,0,sizeof(LOGFONT));
	_tcscpy(lg.lfFaceName,L"宋体");  

	if ( m_bToolWindow )
	{
		lg.lfHeight  = -12;
	}
	else
	{
		lg.lfHeight  = -14;
	}
   	lg.lfWeight  = 400;
   	lg.lfCharSet = 0;
 	lg.lfOutPrecision = 3;

	CFont  font;
	font.CreateFontIndirect(&lg);
	CFont * pOldFont = dc.SelectObject(&font);
	dc.SetBkMode(TRANSPARENT);
	
	dc.SetTextColor(clrText);
	
	dc.DrawText(StrWinText,rectText, DT_LEFT |DT_WORD_ELLIPSIS);	
	dc.SelectObject(pOldFont);
	font.DeleteObject();
}

void CDialogEx::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	// DESCRIPTION:	先让基类的先计算，然后再修改
	CDialog::OnGetMinMaxInfo(lpMMI);
	
	CRect rc;
	BOOL bInfo = SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	ASSERT( bInfo );
	if ( bInfo && !(GetStyle() & WS_CAPTION) )
	{
		lpMMI->ptMaxPosition.x	+=	rc.left;
		lpMMI->ptMaxPosition.y	+=	rc.top;
	}
//	lpMMI->ptMaxSize.x	=	GetSystemMetrics(SM_CXMAXIMIZED);
	lpMMI->ptMaxSize.y	=	GetSystemMetrics(SM_CYMAXIMIZED);

	int32 iFrameWidth = GetSystemMetrics(SM_CXFRAME)-GetSystemMetrics(SM_CXBORDER);
	lpMMI->ptMaxSize.y += (m_nFrameWidth-iFrameWidth);

	// 4是XP下默认的frame高，vista下面会比4要大
	lpMMI->ptMaxSize.y	-=	3;
}

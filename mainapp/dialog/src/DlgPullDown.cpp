// DlgPullDown.cpp : implementation file
//

#include "stdafx.h"

#include "DlgPullDown.h"
#include "PathFactory.h"
#include "StockSelectManager.h"
#include "IoViewCowBear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgPullDown dialog
#define INVALID_ID -1

CDlgPullDown::CDlgPullDown(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPullDown::IDD, pParent)
{
	m_iXButtonHovering = INVALID_ID;
	CRect rcTmp(0,0,0,0);
	m_parentBtnRect = rcTmp;
	m_pParent = pParent;

	m_pPrevChild = NULL;
	m_nWidth = 70;
	m_nHeight = 0;
	m_pHoverBtn = NULL;
}


void CDlgPullDown::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPullDown)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPullDown, CDialog)
	//{{AFX_MSG_MAP(CDlgPullDown)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPullDown message handlers
void CDlgPullDown::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	if (NULL != pNewWnd->GetSafeHwnd() && IsWindowVisible())
	{
		CWnd* pParent = GetParent();
		if (NULL != pParent)
		{
			if (pParent->IsKindOf(RUNTIME_CLASS(CDlgPullDown)))
			{
				CDlgPullDown *pDlg = (CDlgPullDown*)pParent;
 				OnOK();
 				pDlg->OnOK();
			}
			else
			{
				if (NULL != pNewWnd &&  this != pNewWnd->GetParent())
				{
					OnOK();
				}				
			}
		}		
	}
}

int CDlgPullDown::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CDlgPullDown::OnPaint() 
{
	CPaintDC dc(this);

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
	
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	buttonContainerType::iterator iter;
	CRect rcControl;
	
	// 遍历工具栏上所有按钮	
	FOR_EACH(m_mapMoreBtn, iter)
	{
		CNCButton &btnControl = *iter;
		btnControl.GetRect(rcControl);
		
		// 为每个按钮设置下选股状态
		btnControl.SetSelectStockFlagAndDaPanStatus(GetIndexSelectStockFlag(), GetHotSelectStockFlag()
			, GetCurrentCowBearState());
		
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		btnControl.DrawButton(&graphics);
	}
	
	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	//
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CDlgPullDown::OnMouseMove(UINT nFlags, CPoint point) 
{
	int iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			GetCNCButton(m_mapMoreBtn, m_iXButtonHovering).MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}
		
		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			CNCButton &btn = GetCNCButton(m_mapMoreBtn, m_iXButtonHovering);
			m_pHoverBtn = &btn;
			btn.MouseHover();
			if (m_pPrevChild)
			{
				m_pPrevChild->ShowWindow(SW_HIDE);
			}
			CWnd *pChildMenu = btn.GetChildMenu();
			if (pChildMenu)
			{
				pChildMenu->ShowWindow(SW_SHOW);
				m_pPrevChild = pChildMenu;
			}
		}
	}
	
	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof(csTME);
	csTME.dwFlags   = TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;
	::_TrackMouseEvent (&csTME);
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgPullDown::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		GetCNCButton(m_mapMoreBtn, m_iXButtonHovering).MouseLeave();
		m_iXButtonHovering = INVALID_ID;
	}
 	m_pHoverBtn = NULL;
	
	return 0;       
}   

void CDlgPullDown::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		GetCNCButton(m_mapMoreBtn, iButton).LButtonDown();
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgPullDown::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		GetCNCButton(m_mapMoreBtn, iButton).LButtonUp();
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CDlgPullDown::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( NULL != m_pParent)
	{
		m_pParent->PostMessage(WM_COMMAND, wParam);
	}

	OnOK();
	return TRUE;
}

int CDlgPullDown::TButtonHitTest(CPoint point)
{
	buttonContainerType::iterator iter;
	
	// 遍历标题栏上所有按钮
	FOR_EACH(m_mapMoreBtn, iter)	
	{
		CNCButton &btnControl = *iter;
		
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}
	
	return INVALID_ID;
}

void CDlgPullDown::SetShowButtons(const buttonContainerType& mapBtnDst, CRect &parentRct)
{
	m_mapMoreBtn.clear();
	m_mapMoreBtn = mapBtnDst;

	m_parentBtnRect = parentRct;
}

void CDlgPullDown::OnRButtonUp(UINT nFlags, CPoint point)
{
	int iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		GetCNCButton(m_mapMoreBtn, iButton).RButtonUp();
	}
}

void CDlgPullDown::SetParentTitle(CString strTitle)
{
	m_strParentTitle = strTitle;
}

CString CDlgPullDown::GetParentTitle()
{
	return m_strParentTitle;
}

void CDlgPullDown::IncrementHeightAndWidth(int nIncrement, int nWidth)
{
	m_nHeight += nIncrement;
	m_nWidth = nWidth;
}

int CDlgPullDown::GetHeight()
{
	return m_nHeight;
}

int CDlgPullDown::GetWidth()
{
	return m_nWidth;
}

CRect CDlgPullDown::GetRect()
{
	return m_parentBtnRect;
}

BOOL CDlgPullDown::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

DWORD CDlgPullDown::GetIndexSelectStockFlag() const
{
	return CStockSelectManager::Instance().GetIndexSelectStockFlag();
}

DWORD CDlgPullDown::GetHotSelectStockFlag() const
{
	return CStockSelectManager::Instance().GetHotSelectStockFlag();
}

E_DpnxState CDlgPullDown::GetCurrentCowBearState() const
{
	return CCowBearState::Instance().GetCurrentCowBearState();
}


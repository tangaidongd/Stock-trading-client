// *************************************************
// *  CHButton - Class for using Hyperlink Buttons *
// *  Version 1.3                                  *    
// *  (C) Copyright 2005 By Giorgi Moniava         *                      
// *************************************************

/*
This class supports following public methods : 

	* void SetCursor(HCURSOR curCursor);
	  Sets Cursor of the Hyperlink Button 

	* void SetToolTip(LPCSTR strText, BOOL bActive = TRUE);
	  Sets the ToolTip text for the Hyperlink button

	* void SetHoverTextColor(COLORREF clrTextColor,BOOL bShowColor = TRUE);
	  Sets the Hyperlink button text color when mouse is over the button

	* void SetUnderline(UINT nUnderline);
	  Adds or removes the Underline from the text

			[nUnderline]   [description]

			 HS_HOVER      Sets the underline when mouse is over the button
			 HS_ALWAYS	   Sets the Underline always
			 HS_NONE	   Removes the Underline if existed	
		
	* void SetTextColor(COLORREF clrColor);_
      Sets the text color of the Hyperlink button

	* void SetIcon(HICON Icon);
	  Sets the icon to the button

*/


#include "stdafx.h"
#include "HButton.h"
#include ".\hbutton.h"


// CHButton

IMPLEMENT_DYNAMIC(CHButton, CButton)
CHButton::CHButton()
: m_nIconSize(0)
{
	m_nIconSize=16;
	m_bIcon = FALSE;
	m_nUnderline = HS_NONE;
	m_bMouseOver = FALSE;
	m_bCursor = FALSE;
	m_clrTextColor = RGB(255,255,255);
	m_bUnderline = TRUE;
	m_bHoverColor = FALSE;
	m_clrBkColor =RGB(255, 255, 255);
	//m_clrBkColor =::GetSysColor(COLOR_3DFACE);

	m_BackColor = TRANS_BACK;
	m_TextColor = RGB(255, 255, 255);
	m_Cursor = NULL;
	m_Icon = NULL;
	m_clrHoverColor = RGB(255, 255, 255);
}

CHButton::~CHButton()
{
	m_Brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CHButton, CButton)
ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_RBUTTONUP()
	//
	ON_WM_CTLCOLOR()
	//
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CHButton::PreSubclassWindow(void)
{
	CButton::PreSubclassWindow();
}

void CHButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//Get DC for drawing
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC); 
	//Get button state
	UINT state = lpDrawItemStruct->itemState;
	//Get button Rectangle
	CRect oldRect = lpDrawItemStruct->rcItem;

	//Get Button Tex
	CString strText;
	GetWindowText(strText);
	//Get text dimensions
	CSize Extent = pDC->GetTextExtent(strText);

	// Underline Font if neccessary
	if((m_bMouseOver && (m_nUnderline==HS_HOVER) ) ||(m_nUnderline==HS_ALWAYS)) //屏蔽下划线
	{
		LOGFONT lgfnt;
		CFont *t = GetFont();
		CFont *p = new CFont;
		t->GetLogFont(&lgfnt);
		lgfnt.lfUnderline = 1;
		p->CreateFontIndirect(&lgfnt);
		pDC->SelectObject(p);
		delete p;
	}
	

    //Draw Bk COlor
	//pDC->FillSolidRect(oldRect,m_clrBkColor);
	
	// Get 1/2 of Button's Text length and height
	// Modify them if icon is Set
	int tx,ty;
	if (m_bIcon)tx = Extent.cx/2+m_nIconSize/2;
	else     	tx = Extent.cx/2;
	ty = Extent.cy/2;


	//get button coordinates
	CRect rect1 ;
	::GetWindowRect(m_hWnd, &rect1 );
	//Convert them to Parent windows client coordinates
	GetParent()->ScreenToClient(&rect1);

	//Get centerpoint coordinates of rect1
	int cx,cy;
	cx=rect1.CenterPoint().x;
	cy = rect1.CenterPoint().y;

	//initialize NEW Hyperlink Button rectangle
	 CRect newRect(cx-tx-2,cy-ty-2,cx+tx+2,cy+ty+2);
		
	//set text Hyperlink color
	pDC->SetTextColor(m_clrTextColor);

	//Set  Hyperlink text color when mouse is over the button
	if(m_bHoverColor&m_bMouseOver)	pDC->SetTextColor(m_clrHoverColor);


	//Draw the icon if neccessary
	if(m_bIcon)
	{
		DrawIconEx(pDC->GetSafeHdc(),0,0,m_Icon,m_nIconSize,m_nIconSize,0, NULL, DI_NORMAL | DI_COMPAT );
		oldRect.left =2*m_nIconSize;
	}


	//Drawing the Hyperlink Text
	if (!strText.IsEmpty())  
	{

		//Initialize the buttons Left Top point coordinate where text should be drawn
		CPoint pt( oldRect.CenterPoint().x-tx,oldRect.CenterPoint().y - ty);

		pDC->SetBkMode(TRANSPARENT);//Transparent Background

		if (state & ODS_DISABLED)
		{
			pDC->SetTextColor(RGB(192, 192, 192));
			pDC->TextOut(pt.x, pt.y, strText);  
			pDC->SetTextColor(m_clrTextColor);
			// pDC->DrawState(pt, Extent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL); // draw disabled button
		}
		else
			pDC->TextOut(pt.x, pt.y, strText); // Draw Text , when button enabled

		pDC->SetBkMode(TRANSPARENT);//Transparent Background
	}
	//Relesase Device Context
	ReleaseDC(pDC);

	//Resize the button window , so that it fits the Hyperlink control Dimensions
	MoveWindow(newRect);
		
}


void CHButton::OnMouseMove(UINT nFlags,CPoint point)
{
	if(m_bCursor) ::SetCursor(m_Cursor);

	if(!m_bMouseOver)
	{
		m_bMouseOver = TRUE;
		Invalidate();

		// using struct TRACKMOUSEEVENT to track mouse 
		TRACKMOUSEEVENT event;
		event.cbSize = sizeof(TRACKMOUSEEVENT);
		event.dwFlags = TME_LEAVE;
		event.hwndTrack  = m_hWnd;
		_TrackMouseEvent(&event); // Sends WM_MOUSELEAVE message , when mouse leaves the button area
	}

	CButton::OnMouseMove(nFlags,point);
}


void CHButton::SetCursor(HCURSOR curCursor)
{
	m_Cursor = curCursor;
	m_bCursor = TRUE;
	Invalidate();
}

void CHButton::OnLButtonDown(UINT nFlags, CPoint point)
{
		if(m_bCursor) ::SetCursor(m_Cursor);
		CButton::OnLButtonDown(nFlags,point);
}

void CHButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
 if(m_bCursor) ::SetCursor(m_Cursor);
 CButton::OnLButtonDblClk(nFlags,point);
}

void CHButton::OnRButtonDown(UINT nFlags, CPoint point)
{
 if(m_bCursor)::SetCursor(m_Cursor);
 CButton::OnRButtonDown(nFlags,point);
}

void CHButton::OnRButtonDblClk(UINT nFlags, CPoint point)
{
 if(m_bCursor)::SetCursor(m_Cursor);
 CButton::OnRButtonDblClk(nFlags,point);
}

void CHButton::OnLButtonUp(UINT nFlags, CPoint point)
{
 if(m_bCursor)::SetCursor(m_Cursor);
 CButton::OnLButtonUp(nFlags,point);
}

void CHButton::OnRButtonUp(UINT nFlags, CPoint point)
{
 if(m_bCursor)::SetCursor(m_Cursor);
 CButton::OnRButtonUp(nFlags,point);
}

void CHButton::SetHoverTextColor(COLORREF clrTextColor,BOOL bShowColor)
{
	m_clrHoverColor = clrTextColor;
	m_bHoverColor = bShowColor;
	Invalidate();
}

LRESULT CHButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver=FALSE;
	CRect rt;
	GetWindowRect(&rt);
	GetParent()->ScreenToClient(&rt);
	GetParent()->InvalidateRect(rt, TRUE);
	Invalidate();
	return LRESULT();
}

void CHButton::SetUnderline(UINT nUnderline)
{
	m_nUnderline = nUnderline;
	Invalidate();
}

void CHButton::SetTextColor(COLORREF clrColor)
{
	m_clrTextColor = clrColor;
	Invalidate();
}
void CHButton::SetToolTip(LPCTSTR strText, BOOL bActive)
{
	if (strText == NULL) return;

	//create ToolTip
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this);
		m_ToolTip.Activate(FALSE);
	}
	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rect; 
		GetClientRect(rect);
		m_ToolTip.AddTool(this, strText, rect, 1);
	}
	// Set text for tooltip
	m_ToolTip.UpdateTipText(strText, this, 1);
	m_ToolTip.Activate(bActive);

}
BOOL CHButton::PreTranslateMessage(MSG* pMsg) 
{
	
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this);
		m_ToolTip.Activate(FALSE);
	}
	m_ToolTip.RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}
void CHButton::SetIcon(HICON Icon)
{
	m_Icon  = Icon;
	m_bIcon = TRUE;
	//m_nIconSize
	Invalidate();
}

//HBRUSH CHButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
HBRUSH CHButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
#if 0
	HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
//	if (nCtlColor==CTLCOLOR_STATIC)
		
	{
		pDC->SetTextColor(RGB(255,0,0));  //字体颜色
		pDC->SetBkColor(RGB(0, 0, 255));   //字体背景色  
		
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
#endif

	m_Brush.DeleteObject();
	
    if (m_BackColor == TRANS_BACK) {
        m_Brush.CreateStockObject(HOLLOW_BRUSH);
        pDC->SetBkMode(TRANSPARENT);
    }
    else {
        m_Brush.CreateSolidBrush(m_BackColor);
        pDC->SetBkColor(m_BackColor);
    }
	
    pDC->SetTextColor(m_TextColor);
	
    return (HBRUSH)m_Brush;
}

void CHButton::SetButtonFont(const TCHAR *pFontName, int nSize /*= 12*/ )
{
    VERIFY(m_font.CreateFont(
        nSize,                     // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,				   // nWeight
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        pFontName));               // lpszFacename

    SetFont(&m_font);
}

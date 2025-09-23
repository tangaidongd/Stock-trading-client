// ListEx.cpp : implementation file
//

#include "stdafx.h"
//#include "ggtong.h"
#include "ListEx.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  INVALID_ID -1
//#define  IDC_HAND MAKEINTRESOURCE(32649)	//手型光标
/////////////////////////////////////////////////////////////////////////////

// CListEx
CListEx::CListEx(int nRows, int nCols)
{
	RegisterWindowClass();

	m_nRowsPerWheelNotch = 1;

	if (0 > nRows)
	{
		m_iRows = 0;
	}
	else
	{
		m_iRows	= nRows;
	}

	if (0 >= nCols)
	{
		m_iCols = 1;
	}
	else
	{
		m_iCols	= nCols;
	}
	m_iRowHeight	= 30;
	m_iActiveCol	= 0;

	m_crfBk			= RGB(10, 10, 10);
	m_crfRowNor1	= RGB(13, 13, 13);
	m_crfRowNor2	= RGB(0, 0, 0);
	m_crfRowHot		= RGB(52, 52, 48);
	m_crfText		= RGB(183, 183, 183);
	m_crfHotText	= RGB(144,100,0);

	m_bCross		= TRUE;
	m_bAllRowSel	= FALSE;
	m_bHotRowBk		= FALSE;
	m_bHotRowColor	= TRUE;

	m_pXSBHorz		= NULL;
	m_pXSBVert		= NULL;

	m_iXButtonHovering = INVALID_ID;
	m_bOriginalScroll  = FALSE;
	m_nVScrollMax      = 0;  
    m_nHScrollMax      = 0;
	m_dwDonotScrollToNonExistCell = GVL_NONE;
}

CListEx::~CListEx()
{
	m_font.DeleteObject();
	for (int i=0; i<m_iCols; i++)
	{
		m_arColFonts[i].DeleteObject();
	}

	int nRows = m_rowData.GetSize();// 获取行数
	for (int nRow = 0 ; nRow < nRows ; ++nRow)
	{
		ListItem *pListItem = m_rowData.GetAt(nRow);
		int nCols = 0;
		if (NULL != pListItem)
		{
			nCols = pListItem->m_ColData.GetSize();// 获取列数
			// 删除列
			for (int nCol = 0 ; nCol < nCols ; ++nCol)
			{
				ListCol *pColData = pListItem->m_ColData.GetAt(nCol);
				if (NULL != pColData)
				{
					delete pColData;
					pColData = NULL;
				}
			}
			// 删除行
			if (pListItem)
			{
				pListItem->m_ColData.RemoveAll();
				delete pListItem;
				pListItem = NULL;
			}
		}		
	}
	m_rowData.RemoveAll();

	//int nsize = m_ColData.GetSize();
	//for( int i = 0; i < nsize ;i++ )
	//{
	//	ListCol *pListCol = m_ColData.GetAt(i);
	//	if (pListCol)
	//	{
	//		delete pListCol;
	//		pListCol = NULL;
	//	}
	//}
}


BEGIN_MESSAGE_MAP(CListEx, CWnd)
	//{{AFX_MSG_MAP(CListEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListEx message handlers


int CListEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(m_font.CreateFont(
	-15,                        // nHeight
	0,                         // nWidth
	0,                         // nEscapement
	0,                         // nOrientation
	FW_NORMAL,                 // nWeight
	FALSE,                     // bItalic
	FALSE,                     // bUnderline
	0,                         // cStrikeOut
	ANSI_CHARSET,              // nCharSet
	OUT_DEFAULT_PRECIS,        // nOutPrecision
	CLIP_DEFAULT_PRECIS,       // nClipPrecision
	DEFAULT_QUALITY,           // nQuality
	DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	gFontFactory.GetExistFontName(L"微软雅黑")));		   // lpszFacename//...

	TRY
    {
        m_arRowHeights.SetSize(m_iRows);    // initialize row heights
        m_arColWidths.SetSize(m_iCols);     // initialize column widths
		m_arColFonts.SetSize(m_iCols);
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        return FALSE;
    }
    END_CATCH

	CRect rcWnd;
	GetClientRect(rcWnd);
	int iWidth    = rcWnd.Width();
	int iColWidth = iWidth;			// 默认的列宽
	if (0 != m_iCols)
	{
		iColWidth = iWidth / m_iCols; 
	}

	int i = 0;
	for (i=0; i<m_iRows; i++)
	{
		ListItem *pListItem = new ListItem;
		for (int j=0; j<m_iCols; j++)
		{
			ListCol	*pListCol = new ListCol;
			pListCol->m_eColStatus = Normal;
			pListCol->m_strColVal = L"";
			pListItem->m_ColData.Add(pListCol);
		}
		pListItem->m_eColStatus = Normal;
		m_rowData.Add(pListItem);
		m_arRowHeights[i] = m_iRowHeight;
	}

	for (i=0; i<m_iCols; i++)
	{
		m_arColWidths[i] = iColWidth;
		LOGFONT lf;
		m_font.GetLogFont(&lf);   // 这里取得LOGFONT结构	
		m_arColFonts[i].CreateFontIndirect(&lf);
	}

	m_idTopLeftCell.row = 0;
	m_idTopLeftCell.col = 0;
	
	return 0;
}

void CListEx::OnHScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    int iScrollPos = GetScrollPos32(SB_HORZ);
	ListCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rcClient;
    GetClientRect(rcClient);

    switch (nSBCode)
    {
    case SB_LINERIGHT:
        if (iScrollPos < m_nHScrollMax)
        {
			while (idTopLeft.col < (GetColumnCount()-1)
				&& 1 > GetColumnWidth( idTopLeft.col))
            {
                idTopLeft.col++;
            }
            int ixScroll = GetColumnWidth(idTopLeft.col);
            SetScrollPos32(SB_HORZ, iScrollPos + ixScroll);
            if (GetScrollPos32(SB_HORZ) == iScrollPos)
                break; 

            InvalidateRect(rcClient);
        }
        break;

    case SB_LINELEFT:
        if (0 < iScrollPos)
        {
			int iColToUse = idTopLeft.col - 1;
            while(1 > GetColumnWidth( iColToUse))
            {
                iColToUse--;
            }
			
            int ixScroll = GetColumnWidth(iColToUse);
            SetScrollPos32(SB_HORZ, __max(0, iScrollPos - ixScroll));
            InvalidateRect(rcClient);
        }
        break;

    case SB_PAGERIGHT:
        if (iScrollPos < m_nHScrollMax)
        {
            int iOffset = rcClient.Width();
            int iPos = min(m_nHScrollMax, iScrollPos + iOffset);
            SetScrollPos32(SB_HORZ, iPos);
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_PAGELEFT:
        if (0 < iScrollPos)
        {
            int iOffset = -rcClient.Width();
            int iPos = __max(0, iScrollPos + iOffset);
            SetScrollPos32(SB_HORZ, iPos);
            rcClient.left = rcClient.left;
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
			 SetScrollPos32(SB_HORZ, GetScrollPos32(SB_HORZ, TRUE));
			 m_idTopLeftCell.row = -1;
			 ListCellID idNewTopLeft = GetTopleftNonFixedCell();
			 if (idNewTopLeft != idTopLeft)
			 {
				 InvalidateRect(rcClient);
			 }
        }
        break;
        
    case SB_LEFT:
        if (0 < iScrollPos)
        {
            SetScrollPos32(SB_HORZ, 0);
            Invalidate();
        }
        break;
        
    case SB_RIGHT:
        if (iScrollPos < m_nHScrollMax)
        {
            SetScrollPos32(SB_HORZ, m_nHScrollMax);
            Invalidate();
        }
        break;        
        
    default: 
        break;
    }
}

void CListEx::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    int iScrollPos = GetScrollPos32(SB_VERT);
	ListCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rcClient;
    GetClientRect(rcClient);

	switch (nSBCode)
    {
    case SB_LINEDOWN:
        if (iScrollPos < m_nVScrollMax)
        {
			while(idTopLeft.row < (GetRowCount()-1)
				&& 1 > GetRowHeight(idTopLeft.row))
            {
                idTopLeft.row++;
            }

            int iyScroll = GetRowHeight(idTopLeft.row);
         // SetScrollPos32(SB_VERT, iScrollPos + iyScroll);
			SetScrollPos32(SB_VERT, __min(m_nVScrollMax, iScrollPos + iyScroll));
            if (GetScrollPos32(SB_VERT) == iScrollPos)
                break;   
	
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_LINEUP:
        if (0 < iScrollPos)
        {
			int iRowToUse = idTopLeft.row - 1;

            while(1 > GetRowHeight(iRowToUse))
            {
                iRowToUse--;
            }

            int iyScroll = GetRowHeight(iRowToUse);
            SetScrollPos32(SB_VERT, __max(0, iScrollPos - iyScroll));
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_PAGEDOWN:
        if (iScrollPos < m_nVScrollMax)
        {
			int iCurStep = iScrollPos + rcClient.Height();
            iScrollPos = min(m_nVScrollMax, iCurStep);
            SetScrollPos32(SB_VERT, iScrollPos);
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_PAGEUP:
        if (0 < iScrollPos)
        {
            int iOffset = -rcClient.Height();
            int iPos = __max(0, iScrollPos + iOffset);
            SetScrollPos32(SB_VERT, iPos);
            InvalidateRect(rcClient);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
			SetScrollPos32(SB_VERT, GetScrollPos32(SB_VERT, TRUE));
            m_idTopLeftCell.row = -1;
            ListCellID idNewTopLeft = GetTopleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            { 
                InvalidateRect(rcClient);
            }
        }
        break;
        
    case SB_TOP:
        if (0 < iScrollPos)
        {
            SetScrollPos32(SB_VERT, 0);
            Invalidate();
        }
        break;
        
    case SB_BOTTOM:
        if (iScrollPos < m_nVScrollMax)
        {
            SetScrollPos32(SB_VERT, m_nVScrollMax);
            Invalidate();
        }
        break;
    default: 
        break;
    }

}

void CListEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	short zDelta;
	switch (nChar)
	{	
	case VK_DOWN:   
		zDelta = -120;  
		break;
		
	case VK_UP:     
		zDelta = 120;  
        break;  
	default:
		return;
	}
	
	CPoint point;
	GetCursorPos(&point);
	OnMouseWheel(nFlags, zDelta, point);
}

void CListEx::OnSize(UINT nType, int cx, int cy)
{  
    static BOOL bAlreadyInsideThisProcedure = FALSE;
    if (bAlreadyInsideThisProcedure)
        return;
	
    if (!::IsWindow(m_hWnd))
        return;
	
    bAlreadyInsideThisProcedure = TRUE;
	
    CWnd::OnSize(nType, cx, cy);

    ResetScrollBars();
    bAlreadyInsideThisProcedure = FALSE;
}

BOOL CListEx::OnEraseBkgnd(CDC* pDC) 
{
 	return TRUE;
}

void CListEx::OnPaint() 
{
	CPaintDC dc(this);
	CRect m_rcClient;
	GetClientRect(m_rcClient);

	// 一行显示不全，显示省略号...
	if (1 < m_iCols)
	{
		CFont* pFontOld = dc.SelectObject(&m_arColFonts[m_iActiveCol]);
		
		for (int32 i=0; i<m_iRows; i++)
		{
			CString strVal = m_rowData[i]->m_ColData[m_iActiveCol]->m_strTmp;
			CSize sizeText	= dc.GetTextExtent(strVal);
			int32 iCnt = strVal.GetLength();
			if (0 >= iCnt)
			{
				dc.SelectObject(pFontOld);
				break;
			}
			uint32 uiWidth = sizeText.cx / iCnt;
			if (sizeText.cx > (LONG)m_arColWidths[m_iActiveCol])
			{
				int32 iShow = m_arColWidths[m_iActiveCol]/uiWidth - 3;
				strVal = strVal.Left(iShow);
				strVal += L"...";
				
				m_rowData[i]->m_ColData[m_iActiveCol]->m_strColVal = strVal;
			}
			else
			{
				m_rowData[i]->m_ColData[m_iActiveCol]->m_strColVal = m_rowData[i]->m_ColData[m_iActiveCol]->m_strTmp;
			}
		}
		dc.SelectObject(pFontOld);
	}

	CBitmap bmp;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	memDC.FillSolidRect(0, 0, m_rcClient.Width(), m_rcClient.Height(), m_crfBk);

	ListCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row, minVisibleCol = idTopLeft.col;

    CRect VisRect;
    ListCellID idBottomRight = GetVisibleNonFixedCellRange(VisRect, FALSE, FALSE);
    int maxVisibleRow = idBottomRight.row, maxVisibleCol = idBottomRight.col;
	
	for (int i=minVisibleRow; i<= maxVisibleRow; i++)
	{
		ListItem *pListItem = m_rowData[i];
		int iRowIndex = i - minVisibleRow;

		if (m_bHotRowBk)
		{
			if (Normal == pListItem->m_eColStatus)
			{
				if (!m_bCross)
				{
					if (iRowIndex % 2)
					{
						memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor1);
					}
					else
					{
						memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor2);
					}
				}
				else
				{
					memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor1);
				}
			}
			else
			{
				memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowHot);
			}
		}
		else
		{
			if (!m_bCross)
			{
				if (iRowIndex % 2)
				{
					memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor1);
				}
				else
				{
					memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor2);
				}
			}
			else
			{
				memDC.FillSolidRect(0, m_iRowHeight*iRowIndex, m_rcClient.Width(), m_iRowHeight, m_crfRowNor1);
			}
		}

		for (int j=minVisibleCol; j<=maxVisibleCol; j++)
		{
			CFont* def_font = memDC.SelectObject(&m_arColFonts[j]);
			ListCol *pListCol = m_rowData[i]->m_ColData[j];
			if (m_bHotRowColor)
			{
				if (Normal == pListCol->m_eColStatus)
				{
					memDC.SetTextColor(m_crfText);
				}
				else
				{
					memDC.SetTextColor(m_crfHotText);
				}
			}
			else
			{
				memDC.SetTextColor(m_crfText);
			}

			CRect rcDraw;
			int iRow = 0, iCol = 0;
			iRow = iRowIndex;
			iCol = j - minVisibleCol;
			GetCellRect(rcDraw, iRow, iCol);

			CRect temp = rcDraw;
			int height = memDC.DrawText(pListCol->m_strColVal, temp, DT_CALCRECT);

			if (1 == j)
			{
				rcDraw.DeflateRect(0, (rcDraw.Height()-height)/2);
			}
			else
			{
				rcDraw.DeflateRect(8, (rcDraw.Height()-height)/2);
			}
				
			memDC.DrawText(pListCol->m_strColVal, rcDraw, DT_LEFT);
			memDC.SelectObject(def_font);
		}
	}

 	dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

BOOL CListEx::RegisterWindowClass()
{
    WNDCLASS wndcls;
    //HINSTANCE hInst = AfxGetInstanceHandle();
    HINSTANCE hInst = AfxGetResourceHandle();
	
    if (!(::GetClassInfo(hInst, LISTCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
#ifndef _WIN32_WCE_NO_CURSOR
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
#else
        wndcls.hCursor          = 0;
#endif
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = LISTCTRL_CLASSNAME;
		
        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }
	
    return TRUE;
}

BOOL CListEx::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle) 
{
	ASSERT(pParentWnd->GetSafeHwnd());
	
	m_iID = nID;
	CWnd* pWnd = this;
    if (!pWnd->Create(LISTCTRL_CLASSNAME, NULL, dwStyle | SS_NOTIFY, rect, pParentWnd, nID))
        return FALSE;
	
	// 不显示边框
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	
	return TRUE;
}

BOOL CListEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint point = pt;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -= rcWindow.left;
	point.y -= (rcWindow.top);
	int iScrollPos = GetScrollPos32(SB_VERT);

    if (-1 == m_nRowsPerWheelNotch)
    {
        int nPagesScrolled = zDelta / 120;
		
        if (0 < nPagesScrolled)
		{
            for (int i = 0; i < nPagesScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
			}
		}
        else
		{
            for (int i = 0; i > nPagesScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
			}
		}
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;
		
        if (0 < nRowsScrolled)
		{
            for (int i = 0; i < nRowsScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
			}

			if (0 < iScrollPos)
			{
				point.y -= m_iRowHeight;
			}
		}
        else
		{
            for (int i = 0; i > nRowsScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
			}

			if (iScrollPos < m_nVScrollMax)
			{
				point.y += m_iRowHeight;
			}
		}
    }

 	OnMouseMove(nFlags, point);

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CListEx::OnMouseMove(UINT nFlags, CPoint point) 
{ 
//	SetFocus();
	GetParent()->SendMessage(WM_LISTIME_FOCUS, 0, m_iID);

	CRect rcClient;
	GetClientRect(rcClient);
 	int iButton = TButtonHitTest(point);

	// 改变光标形状
	if (INVALID_ID != iButton)
	{
		SetCursor(LoadCursor(NULL, IDC_HAND)); 	
	}

 	if (iButton != m_iXButtonHovering)
 	{
		ListCellID idTopLeft = GetTopleftNonFixedCell();
		int minVisibleRow = idTopLeft.row;

 		if (INVALID_ID != m_iXButtonHovering)
		{
			for (int i=0; i<=m_iActiveCol; i++)
			{
				m_rowData[m_iXButtonHovering]->m_ColData[i]->m_eColStatus = Normal;
			}
			m_rowData[m_iXButtonHovering]->m_eColStatus = Normal;

			CRect rcDraw;
			int iCol = m_iActiveCol;
			int iRow = m_iXButtonHovering - minVisibleRow;
			GetCellRect(rcDraw, iRow, iCol);
			rcDraw.left = rcClient.left;

			CRect rcTmp = rcDraw;
			rcTmp.right = rcClient.right;
			if (m_bAllRowSel)
			{
				InvalidateRect(rcTmp, FALSE);
			}
			else
			{
				InvalidateRect(rcDraw, FALSE);
			}

 	 		m_iXButtonHovering = INVALID_ID;
		}
 		
 		if (INVALID_ID != iButton)
 		{
			m_iXButtonHovering = iButton;
			for (int i=0; i<=m_iActiveCol; i++)
			{
				m_rowData[m_iXButtonHovering]->m_ColData[i]->m_eColStatus = MouseOver;
			}
			m_rowData[m_iXButtonHovering]->m_eColStatus = MouseOver;

			CRect rcDraw;
			int iCol = m_iActiveCol;
			int iRow = m_iXButtonHovering - minVisibleRow;
			GetCellRect(rcDraw, iRow, iCol);
			rcDraw.left = rcClient.left;

			CRect rcTmp = rcDraw;
			rcTmp.right = rcClient.right;
			if (m_bAllRowSel)
			{
				InvalidateRect(rcTmp, FALSE);
			}
			else
			{
				InvalidateRect(rcDraw, FALSE);
			}
 		}
 	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags   = TME_LEAVE;
	csTME.hwndTrack = m_hWnd ; 
	::_TrackMouseEvent (&csTME);

	CWnd::OnMouseMove(nFlags, point);
}

LRESULT CListEx::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		ListCellID idTopLeft = GetTopleftNonFixedCell();
		int minVisibleRow = idTopLeft.row;

		for (int i=0; i<=m_iActiveCol; i++)
		{
			m_rowData[m_iXButtonHovering]->m_ColData[i]->m_eColStatus = Normal;
		}
		m_rowData[m_iXButtonHovering]->m_eColStatus = Normal;

		CRect rcClient;
		GetClientRect(rcClient);

		CRect rcDraw;
		int iCol = m_iActiveCol;
		int iRow = m_iXButtonHovering - minVisibleRow;
		GetCellRect(rcDraw, iRow, iCol);
		rcDraw.left = rcClient.left;

		CRect rcTmp = rcDraw;
		rcTmp.right = rcClient.right;
		if (m_bAllRowSel)
		{
			InvalidateRect(rcTmp, FALSE);
		}
		else
		{
			InvalidateRect(rcDraw, FALSE);
		}

		m_iXButtonHovering = INVALID_ID;
	}
	
	return 0;       
}  

int CListEx::TButtonHitTest(CPoint point)
{
	ListCellID idTopLeft = GetTopleftNonFixedCell();
	int minVisibleRow = idTopLeft.row ;
	
	CRect rcVis;
	ListCellID idBottomRight = GetVisibleNonFixedCellRange(rcVis, FALSE, FALSE);
	int maxVisibleRow = idBottomRight.row;

	CClientDC dc(this);
	CFont* pFontOld = dc.SelectObject(&m_arColFonts[m_iActiveCol]);
	
	for (int i=minVisibleRow; i<=maxVisibleRow; i++)
	{		
		CRect rcClient, rcTmp;
		GetClientRect(rcClient);
		int iCol = m_iActiveCol;
		int iRow = i - minVisibleRow;
		GetCellRect(rcTmp, iRow, iCol);
		
		CString strVal = m_rowData[i]->m_ColData[m_iActiveCol]->m_strColVal;
		CSize sizeText	= dc.GetTextExtent(strVal);
		rcTmp.right = rcTmp.left + sizeText.cx;

		if(rcTmp.PtInRect(point))
		{
			return i;
		}
	}
	dc.SelectObject(pFontOld);
	
	return INVALID_ID;
}

void CListEx::SetItemValue(int iRowIndex, int iColIndex, CString strVal, LPARAM	m_lParam)
{
	if ((iRowIndex >= m_iRows) || (iColIndex >= m_iCols))
	{
		return;
	}
	m_rowData[iRowIndex]->m_ColData[iColIndex]->m_strColVal = strVal;
	m_rowData[iRowIndex]->m_ColData[iColIndex]->m_strTmp = strVal;
	m_rowData[iRowIndex]->m_ColData[iColIndex]->m_lParam = m_lParam;
}

void CListEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		GetParent()->SendMessage(WM_LISTITME_CLICK, iButton, m_iID);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CListEx::OnLButtonUp(UINT nFlags, CPoint point) 
{	  
// 	int iButton = TButtonHitTest(point);
// 	if (INVALID_ID != iButton)
// 	{
// 		GetParent()->SendMessage(WM_LISTITME_CLICK, iButton, m_iID);
// 	}
		
	CWnd::OnLButtonUp(nFlags, point);
}

void CListEx::IsCross(bool bflag)
{
	m_bCross = bflag;
}

void CListEx::IsAllRowSel(bool bflag)
{
	m_bAllRowSel = bflag;
}

void CListEx::IsHotRowBK(bool bflag)
{
	m_bHotRowBk = bflag;
}

void CListEx::IsHotRowColor(bool bflag)
{
	m_bHotRowColor = bflag;
}

void CListEx::SetListBkColor(COLORREF crfBk, COLORREF crfRowNor1, COLORREF crfRowNor2, COLORREF crfRowHot)
{
	m_crfBk		 = crfBk;
	m_crfRowNor1 = crfRowNor1;
	m_crfRowNor2 = crfRowNor2;
	m_crfRowHot	 = crfRowHot;
}

void CListEx::SetListTextColor(COLORREF crfText, COLORREF crfHotText)
{
	m_crfText	 = crfText;
	m_crfHotText = crfHotText;
}

void CListEx::GetCellRect(CRect &rct, int iRow, int iCol)
{
	rct.left = 0;
	int i = 0;
	for (i=0; i<iCol; i++)
	{
		rct.left += m_arColWidths[i];
	}

	rct.top = 0;
	for (i=0; i<iRow; i++)
	{
		rct.top += m_arRowHeights[i];
	}

	rct.right  = rct.left + m_arColWidths[iCol];
	rct.bottom = rct.top + m_arRowHeights[iRow];
}

void CListEx::SetScrollBar(CXScrollBar *pXSBHorz, CXScrollBar *pXSBVert)
{
	m_pXSBHorz = pXSBHorz;
	m_pXSBVert = pXSBVert;
	
	ResetScrollBars();
}

int CListEx::GetScrollPos32(int nBar, BOOL bGetTrackPos /* = FALSE */)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
	
	if ( TRUE == m_bOriginalScroll ) //使用原始滚动条
	{	
		if (bGetTrackPos)
		{
			if (GetScrollInfo(nBar, &si, SIF_TRACKPOS))
			{
				return si.nTrackPos;
			}
		}
		else
		{
			if (GetScrollInfo(nBar, &si, SIF_POS))
			{
				return si.nPos;
			}
		}
	}
	else 
	{
		if (SB_HORZ == nBar)
		{
			if (NULL != m_pXSBHorz)
			{
				return m_pXSBHorz->GetScrollPos();
			}
		}
		else if (SB_VERT == nBar)
		{
			if (NULL != m_pXSBVert)
			{
				return m_pXSBVert->GetScrollPos();
			}
		}
	}
	
    return 0;
}

BOOL CListEx::SetScrollPos32(int nBar, int nPos, BOOL bRedraw /* = TRUE */)
{
	m_idTopLeftCell.row = -1;

	if ( TRUE == m_bOriginalScroll )  //使用原始滚动条
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask  = SIF_POS;
		si.nPos   = nPos;
		return SetScrollInfo(nBar, &si, bRedraw);
	}
	
	if (SB_HORZ == nBar)
	{
		if (NULL != m_pXSBHorz)
		{
			m_pXSBHorz->SetScrollPos(nPos, bRedraw);
			return TRUE;
		}
	}
	
    if (SB_VERT == nBar)
	{
		if (NULL != m_pXSBVert)
		{
			m_pXSBVert->SetScrollPos(nPos, bRedraw);
			return TRUE;
		}
	}
	
	return FALSE;
}

void CListEx::ResetScrollBars()
{
	m_idTopLeftCell.row = -1;

    if (!::IsWindow(GetSafeHwnd())) 
	{
        return;
	}
    
    CRect rcClient;
    GetClientRect(rcClient);
    
    if (rcClient.left == rcClient.right || rcClient.top == rcClient.bottom)
        return;
    
    if (rcClient.left >= rcClient.right || rcClient.top >= rcClient.bottom)
    {
        EnableScrollBars(SB_BOTH, FALSE);
        return;
    }

    CRect rcVisible(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
    CRect rcVirtual(rcClient.left, rcClient.top, GetVirtualWidth(), GetVirtualHeight());
	
	if (rcVisible.Height() < rcVirtual.Height())
	{
		EnableScrollBars(SB_VERT, TRUE); 
		m_nVScrollMax = rcVirtual.Height() - rcClient.Height() - 1;
	}
	else
	{
		EnableScrollBars(SB_VERT, FALSE); 
		m_nVScrollMax = 0;
	}
	
	if (rcVisible.Width() < rcVirtual.Width())
	{
		EnableScrollBars(SB_HORZ, TRUE); 
		m_nHScrollMax = rcVirtual.Width() - 1;
	}
	else
	{
//		EnableScrollBars(SB_HORZ, FALSE); 
// 		m_nHScrollMax = 0;
		EnableScrollBars(SB_HORZ, TRUE); 
		m_nHScrollMax = rcVirtual.Width() - 1;
	}

	if ( (GVL_HORZ & m_dwDonotScrollToNonExistCell) && 0 < m_nHScrollMax)
    {
		// 限制了水平不存在列的滚动
		// 从总的宽度中除去可见部分宽度
		m_nHScrollMax -= rcVisible.Width();
		if (0 >= m_nHScrollMax)
		{
			EnableScrollBars(SB_HORZ, FALSE);
			m_nHScrollMax = 0;
		}
    }
    if ( (GVL_VERT & m_dwDonotScrollToNonExistCell) && 0 < m_nVScrollMax)
    {
		// 限制了竖直不存在行的滚动
		m_nVScrollMax -= rcVisible.Height();
		if (0 >= m_nVScrollMax)
		{
			EnableScrollBars(SB_VERT, FALSE);
			m_nVScrollMax = 0;
		}
    }

    ASSERT(INT_MAX > m_nVScrollMax && INT_MAX > m_nHScrollMax); 
	
	if ( TRUE == m_bOriginalScroll ) //使用原始的滚动条
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = (m_nHScrollMax>0)? m_nHScrollMax : 0;
		si.nMin = 0;
		si.nMax = m_nHScrollMax;
		SetScrollInfo(SB_HORZ, &si, TRUE);
		
		si.fMask |= SIF_DISABLENOSCROLL;
		si.nPage = (m_nVScrollMax>0)? m_nVScrollMax : 0;
		si.nMin = 0;
		si.nMax = m_nVScrollMax;
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
	else
	{
		if (NULL != m_pXSBHorz)
		{
			m_pXSBHorz->SetScrollRange(0, m_nHScrollMax, TRUE);

			if ( GetScrollPos32(SB_HORZ) > m_nHScrollMax )
			{
				SetScrollPos32(SB_HORZ, m_nHScrollMax);
			}
		}

		if (NULL != m_pXSBVert)
		{
			m_pXSBVert->SetScrollRange(0, m_nVScrollMax, TRUE);

			if ( GetScrollPos32(SB_VERT) >= m_nVScrollMax )
			{
				SetScrollPos32(SB_VERT, m_nVScrollMax);
			}
		}
	}
}

void CListEx::EnableScrollBars(int nBar, BOOL bEnable /*=TRUE*/)
{
    if (bEnable)
    {
        if (!IsVisibleHScroll() && (SB_HORZ == nBar || SB_BOTH == nBar))
        {
            m_nBarState |= GVL_HORZ;
            
			if (NULL != m_pXSBHorz)
			{
				m_pXSBHorz->EnableWindow(bEnable);
			}
        }
        
        if (!IsVisibleVScroll() && (SB_VERT == nBar || SB_BOTH == nBar))
        {
            m_nBarState |= GVL_VERT;
			
            if (NULL != m_pXSBVert)
			{
				m_pXSBVert->EnableWindow(bEnable);
			}
        }
		
		if ( !IsWindow(m_pXSBHorz->GetSafeHwnd()) )	// 暂时限定为要一个窗口，其实也可是抽象的东东
		{
			m_nBarState &= ~GVL_HORZ;
		}
		if ( !IsWindow(m_pXSBVert->GetSafeHwnd()) )
		{
			m_nBarState &= ~GVL_VERT;
		}
    }
    else
    {
        if ( IsVisibleHScroll() && (SB_HORZ == nBar || SB_BOTH == nBar))
        {
            m_nBarState &= ~GVL_HORZ; 
			
			if (NULL != m_pXSBHorz)
			{
				m_pXSBHorz->EnableWindow(bEnable);
			}
        }
        
        if ( IsVisibleVScroll() && (SB_VERT == nBar || SB_BOTH == nBar))
        {
            m_nBarState &= ~GVL_VERT;
            
			if (NULL != m_pXSBVert)
			{
				m_pXSBVert->EnableWindow(bEnable);
			}
        }
    }
}

BOOL CListEx::SetColumnWidth(int nCol, int width)
{
  //   ASSERT(nCol >= 0 && nCol < m_iCols && width >= 0);
    if (nCol < 0 || nCol >= m_iCols || width < 0)
	{
        return FALSE;
	}
	
    m_arColWidths[nCol] = width;
    ResetScrollBars();
	
    return TRUE;
}

BOOL CListEx::SetRowHeight(int height)
{
    if (height < 0)
        return FALSE;
	
 //   m_arRowHeights[nRow] = height;
	m_iRowHeight = height;
    ResetScrollBars();
	
    return TRUE;
}

long CListEx::GetVirtualWidth() const
{
    long lVirtualWidth = 0;
    int iColCount = GetColumnCount();
    for (int i = 0; i < iColCount; i++)
	{
        lVirtualWidth += m_arColWidths[i];
	}
	
    return lVirtualWidth;
}

long CListEx::GetVirtualHeight() const
{
    long lVirtualHeight = 0;
    int iRowCount = GetRowCount();
    for (int i = 0; i < iRowCount; i++)
	{
        lVirtualHeight += m_arRowHeights[i];
	}
	
    return lVirtualHeight;
}

int CListEx::GetRowHeight(int nRow) const
{
    ASSERT(nRow >= 0 && nRow < m_iRows);
    if (nRow < 0 || nRow >= m_iRows)
	{
        return -1;
	}
	
    return m_arRowHeights[nRow];
}

int CListEx::GetColumnWidth(int nCol) const
{
    ASSERT(nCol >= 0 && nCol < m_iCols);
    if (nCol < 0 || nCol >= m_iCols)
	{
        return -1;
	}
	
    return m_arColWidths[nCol];
}

ListCellID CListEx::GetTopleftNonFixedCell(BOOL bForceRecalculation /*=FALSE*/)
{
    if (m_idTopLeftCell.IsValid() && !bForceRecalculation)
	{
        return m_idTopLeftCell;
	}
	
    int nVertScroll = GetScrollPos32(SB_VERT), 
        nHorzScroll = GetScrollPos32(SB_HORZ);
	
    m_idTopLeftCell.col = 0;
    int nRight = 0;
    while (nRight < nHorzScroll && m_idTopLeftCell.col < (GetColumnCount()-1))
	{
        nRight += GetColumnWidth(m_idTopLeftCell.col++);
	}
	
    m_idTopLeftCell.row = 0;
    int nTop = 0;
    while (nTop < (nVertScroll) && m_idTopLeftCell.row < (GetRowCount()-1))
	{
        nTop += GetRowHeight(m_idTopLeftCell.row++);
	}
	
    return m_idTopLeftCell;
}

ListCellID CListEx::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/,
												  BOOL bContainFragment/*=TRUE*/)
{
    int i;
    CRect rcClient;
    GetClientRect(rcClient);
	
    ListCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);
	
    // calc bottom
    int bottom = rcClient.top;
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rcClient.bottom)
        {
			if ( !bContainFragment && bottom > rcClient.bottom )
			{
				bottom -= GetRowHeight(i);
				i--;	// 不允许包含部分行的话剔除最后一行
			}
			else
			{
				bottom = rcClient.bottom;
			}
            break;
        }
    }
	int iMaxRowIndex = GetRowCount() - 1;
    int maxVisibleRow = min(i, iMaxRowIndex);
	
    // calc right
    int right = rcClient.left;
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rcClient.right)
        {
// 			if ( !bContainFragment && right > rect.right )
// 			{
// 				right -= GetColumnWidth(i);
// 				i--;	// 不允许包含部分列的话剔除最后一列
// 			}
// 			else
			{
				right = rcClient.right;
			}
            break;
        }
    }
	int iMaxColumnIndex = GetColumnCount() - 1;
    int maxVisibleCol = min(i, iMaxColumnIndex);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }
	
    return ListCellID(maxVisibleRow, maxVisibleCol);
}

int CListEx::InsertRow(int nRow)
{
 	if (nRow > GetRowCount() || nRow < 0)
	{
         return -1;
	}

	m_iRows += 1; 
	m_arRowHeights.InsertAt(nRow, (UINT)0);
	m_arRowHeights[nRow] = m_iRowHeight;
	
	ListItem *pListItem = new ListItem;
	pListItem->m_eColStatus = Normal;
	for (int j=0; j<m_iCols; j++)
	{
		ListCol	*pListCol = new ListCol;
		pListCol->m_eColStatus = Normal;
		pListCol->m_strColVal = "";
		pListItem->m_ColData.Add(pListCol);
	}
	m_rowData.InsertAt(nRow, pListItem);
	m_arRowHeights[nRow] = m_iRowHeight;
	ResetScrollBars();

	return nRow;
}

int CListEx::InsertColumn(int nColumn)
{
	m_iCols += 1;

	for (int row = 0; row < m_iRows; row++)
	{
		ListItem* pRow = m_rowData[row];
		if (!pRow)
		{
			return FALSE;	
		}
		
		ListCol	*pListCol = new ListCol;
		pListCol->m_eColStatus = Normal;
		pListCol->m_strColVal = L"";  
		pRow->m_ColData.InsertAt(nColumn, pListCol);
    }

	CRect rcWnd;
	GetClientRect(rcWnd);
	int iWidth = rcWnd.Width();
	int iColWidth = iWidth / m_iCols; // 默认的列宽
	m_arColWidths[nColumn] = iColWidth;

	LOGFONT lf;
	m_font.GetLogFont(&lf);   // 这里取得LOGFONT结构	
	m_arColFonts[nColumn].CreateFontIndirect(&lf);

	ResetScrollBars();

	return nColumn;
}

BOOL CListEx::DeleteRow(int nRow)
{
	if (nRow < 0 || nRow >= GetRowCount())
        return FALSE;

	m_rowData.RemoveAt(nRow);
	m_arRowHeights.RemoveAt(nRow);
	m_iRows -= 1; 

	ResetScrollBars();

	return TRUE;
}

BOOL CListEx::DeleteAllItems()
{
//	m_arColWidths.RemoveAll();
//    m_arRowHeights.RemoveAll();
//	m_arColFonts.RemoveAll();
	m_rowData.RemoveAll();
//	m_iRows = m_iCols = 0;
	m_iRows = 0;
    ResetScrollBars();

	return TRUE;
}

BOOL CListEx::DeleteColumn(int nColumn)
{
	if (nColumn < 0 || nColumn >= GetColumnCount())
	{
        return FALSE;
	}

	for (int row = 0; row < m_iRows; row++)
	{
		ListItem* pRow = m_rowData[row];
		if (!pRow)
		{
			return FALSE;		
		}
        
		pRow->m_ColData.RemoveAt(nColumn);
    }

	m_arColWidths.RemoveAt(nColumn);
	m_arColFonts[nColumn].DeleteObject();
	m_arColFonts.RemoveAt(nColumn);
	m_iCols -= 1;
	ResetScrollBars();

	return TRUE;
}

void CListEx::SetColsFont(int nColumn, const LOGFONT* plf)
{
	if (nColumn < 0 || nColumn >= GetColumnCount())
	{
        return;	
	}

	if (NULL != plf)
	{
		m_arColFonts[nColumn].DeleteObject();
		m_arColFonts[nColumn].Detach();
		m_arColFonts[nColumn].CreateFontIndirect(plf);
	}
}

void CListEx::SetColsActive(int nColumn)
{
	if (nColumn < 0 || nColumn >= GetColumnCount())
	{
        return;
	}

	m_iActiveCol = nColumn;
}

int CListEx::GetControlID()
{
	return m_iID;
}

CString CListEx::GetItemText(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_iRows || nCol < 0 || nCol >= m_iCols)
	{
        return L"";
	}

	return m_rowData[nRow]->m_ColData[nCol]->m_strTmp;
}

LPARAM CListEx::GetData(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_iRows || nCol < 0 || nCol >= m_iCols)
	{
        return 0;
	}
	
	return m_rowData[nRow]->m_ColData[nCol]->m_lParam;
}

int32 CListEx::GetCurrentRow()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	int iButton = TButtonHitTest(point);
	return iButton;
}

bool32  CListEx::IsRowVisible(int nRow)
{
	CRect rcVis;
	ListCellID idBottomRight = GetVisibleNonFixedCellRange(rcVis, FALSE, TRUE);
	int maxVisibleRow = idBottomRight.row;
	if (nRow == maxVisibleRow)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CListEx::IsSetFocus(bool32 bFocus)
{
	if (bFocus)
	{
		SetFocus();
	}
}
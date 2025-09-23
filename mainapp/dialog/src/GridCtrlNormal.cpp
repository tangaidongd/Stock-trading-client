#include "StdAfx.h"
#include "GridCtrlNormal.h"

#include "GridCtrl.h"

#include "IoViewBase.h"
#include "DrawPolygon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern void DrawDotLine (CDC* pDC, CPoint pt1, CPoint pt2, COLORREF clr );

IMPLEMENT_DYNCREATE(CGridCellNormal, CGridCell)

CGridCellNormal::CGridCellNormal()
:CGridCell()
{
}

CGridCellNormal::~CGridCellNormal()
{
	
}

BOOL CGridCellNormal::Draw( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*= TRUE*/ )
{
	// Note - all through this function we totally brutalise 'rect'. Do not
    // depend on it's value being that which was passed in.
	
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;                             //  though cell is hidden

    //TRACE3("Drawing %scell %d, %d\n", IsFixed()? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
    if (!pDefaultCell)
        return FALSE;

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;

    TextClr = (GetTextClr() == CLR_DEFAULT)? pDefaultCell->GetTextClr() : GetTextClr();
    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pDefaultCell->GetBackClr();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

	// Draw cell background and highlighting (if necessary)
    if ( IsFocused() || IsDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(GetState() & GVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_HIGHLIGHT);
            TextClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
            bEraseBkgnd = TRUE;
        }

		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}

        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            TRY 
            {
                CBrush brush(TextBkClr);
                pDC->FillRect(rect, &brush);
            } 
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        if (pGrid->GetFrameFocusCell())
        {
                // Use same color as text to outline the cell so that it shows
                // up if the background is black.
            TRY 
            {
                CBrush brush(TextClr);
                pDC->FrameRect(rect, &brush);
            }
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }
        pDC->SetTextColor(TextClr);

        // Adjust rect after frame draw if no grid lines
        if(pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

		//rect.DeflateRect(0,1,1,1);  - Removed by Yogurt
    }
    else if ((GetState() & GVIS_SELECTED))
    {
		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}
		else
		{
			TextBkClr = ::GetSysColor(COLOR_HIGHLIGHT);
		}

		rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		pDC->FillSolidRect(rect, TextBkClr);
		rect.right--; rect.bottom--;
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
    else
    {
		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}

        if (bEraseBkgnd)
        {
            rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;
        }
        pDC->SetTextColor(TextClr);
    }

    // Draw lines only when wanted
    if (IsFixed()/* && pGrid->GetGridLines() != GVL_NONE*/)
    {
        CCellID FocusCell = pGrid->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
                            (FocusCell.row == nRow || FocusCell.col == nCol);

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        if (bHiliteFixed)
        {
            rect.right++; rect.bottom++;
            pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
            rect.DeflateRect(1,1);
        }
        else
        {
            CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
                darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
                *pOldPen = pDC->GetCurrentPen();

            pDC->SelectObject(&lightpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.left, rect.bottom);

            pDC->SelectObject(&darkpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.left, rect.bottom);

            pDC->SelectObject(pOldPen);
            rect.DeflateRect(1,1);
        }
    }
// 	
// 	{
// 	
// 		CPen lightpen(PS_SOLID, 1,  ::GetSysColor(0x00ff00));
//             
// 		CPen *pOldPen = pDC->SelectObject(&lightpen);
//         pDC->MoveTo(rect.right, rect.top);
//         pDC->LineTo(rect.left, rect.top);
//         pDC->LineTo(rect.left, rect.bottom);
// 
//         pDC->SelectObject(pOldPen);
//         rect.DeflateRect(1,1);
// 
// 
// 	}
// 


    // Draw Text and image
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    if (!pDC->m_bPrinting)
#endif
    {
        CFont *pFont = GetFontObject();
		ASSERT(pFont);
        if (pFont)
            pDC->SelectObject(pFont);
    }

    //rect.DeflateRect(GetMargin(), 0); - changed by Yogurt
    rect.DeflateRect(GetMargin(), GetMargin());    
    rect.right++;    
    rect.bottom++;

    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();

            /*
            // removed by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;
            if( nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
                && nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin())  )
            {
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            */
            // Added by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;            
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top;            
            if ((nImageWidth + rect.left <= rect.right) && (nImageHeight + rect.top <= rect.bottom))                
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);

            //rect.left += nImageWidth+GetMargin();
        }
    }

	BOOL bDrawedSort = FALSE;
    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0 /*&& pGrid->BeDrawSortArrow()*/)
    {
        CSize size = pDC->GetTextExtent(_T("M"));
        int nOffset = 2;

        // Base the size of the triangle on the smaller of the column
        // height or text height with a slight offset top and bottom.
        // Otherwise, it can get drawn outside the bounds of the cell.
        size.cy -= (nOffset * 2);

        if (size.cy >= rect.Height())
            size.cy = rect.Height() - (nOffset * 2);

        size.cx = size.cy;      // Make the dimensions square

        // Kludge for vertical text
        BOOL bVertical = (GetFont()->lfEscapement == 900);

        // Only draw if it'll fit!
        //if (size.cx + rect.left < rect.right + (int)(2*GetMargin())) - changed / Yogurt
        if (size.cx + rect.left < rect.right)
        {
            int nTriangleBase = rect.bottom - nOffset - size.cy;    // Triangle bottom right
            //int nTriangleBase = (rect.top + rect.bottom - size.cy)/2; // Triangle middle right
            //int nTriangleBase = rect.top + nOffset;                 // Triangle top right

            //int nTriangleLeft = rect.right - size.cx;                 // Triangle RHS
            //int nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            //int nTriangleLeft = rect.left;                            // Triangle LHS

			BOOL bLeftTriangle = (GetFormat() & DT_RIGHT || GetFormat() & DT_CENTER);		// ������룬��������ߣ��������ұ� 

            int nTriangleLeft;
            if (bVertical)
                nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            else
			{
				if ( bLeftTriangle )
				{
					nTriangleLeft = rect.left;
				}
				else
				{
					nTriangleLeft = rect.right - size.cx;               // Triangle RHS
				}
			}
            CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
            CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
            if (pGrid->GetSortAscending())
            {
                // Draw triangle pointing upwards
                  CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                  pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);
                  pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + 1 );
                  pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + size.cy + 1);
                  pDC->LineTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);
  
                  pDC->SelectObject(&penShadow);
                  pDC->MoveTo( nTriangleLeft, nTriangleBase + size.cy );
                  pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase );
                  pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase + size.cy );
                  pDC->LineTo( nTriangleLeft, nTriangleBase + size.cy );
                  pDC->SelectObject(pOldPen);
            }
            else
            {
                // Draw triangle pointing downwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + size.cy + 1 );
                pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + 1, nTriangleBase + 1 );
    
                pDC->SelectObject(&penShadow);
                pDC->MoveTo( nTriangleLeft, nTriangleBase );
                pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase );
                pDC->LineTo( nTriangleLeft, nTriangleBase );
                pDC->SelectObject(pOldPen);
            }
            
            if (!bVertical)
			{
				if ( bLeftTriangle )
				{
					rect.left += size.cy;
				}
				else
				{
					rect.right -= size.cy;
				}
			}
			bDrawedSort = TRUE;
        }
    }

    // We want to see '&' characters so use DT_NOPREFIX
    GetTextRect(rect);
    rect.right++;    
    rect.bottom++;

	DWORD dwFmt = GetFormat() |DT_NOPREFIX;
	if ( bDrawedSort )
	{
		dwFmt |= DT_WORD_ELLIPSIS;
	}
    DrawText(pDC->m_hDC, GetText(), -1, rect, dwFmt);

	//	������������ö�����Ľǣ�����û���б�ͷ������Ƶ�һ������ͷβ���������������ν�
	if (NULL != m_pGrid && m_pGrid->HavePolygonConer())
	{

		if (m_pGrid->GetFixedRowCount() == 0)
		{
			//	���Ͻ����к�
			CCellID cellID = m_pGrid->GetTopleftNonFixedCell();			
			if(cellID.IsValid())
			{
				if (nRow == cellID.row)
				{
					//	���Ƶ�һ��
					if(0 == nCol)
					{
						//	�����б����Ͻ�
						rect.left = 0;
						rect.top = 0;
						DrawPolygonBorder(pDC, rect, true);				
					}
					//	�������һ��
					if (m_pGrid->GetColumnCount() - 1== nCol)
					{
						//	�����б����Ͻǽ�
						CRect rt;
						m_pGrid->GetClientRect(&rt);
						rect.top = 0;
						rect.right = rt.right;
						DrawPolygonBorder(pDC, rect, false);						 

					}

				}
			}			

		}
	}
    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

CSize CGridCellNormal::GetTextExtent( LPCTSTR szText, CDC* pDC /*= NULL*/ )
{
	CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);
	
    BOOL bReleaseDC = FALSE;
    if ( szText == NULL)
    {
		return CGridCell::GetTextExtent(szText, pDC);
    }
	else if ( pDC == NULL )
	{
		if ( pGrid != NULL )
		{
			pDC = pGrid->GetDC();
			bReleaseDC = TRUE;
		}
		if ( pDC == NULL )
		{
			return CGridCell::GetTextExtent(szText, pDC);
		}
	}

    CFont *pOldFont = NULL,
		*pFont = GetFontObject();
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);
	
    CSize size;
    int nFormat = GetFormat();
	
    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = szText;
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;
			
            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }

		if ( str == szText )	// û���ҵ�\n ��ʹ��Ĭ�Ͽ�������㣬��������ǰ���ܿ��
		{
			int iDefWidth = pGrid->GetDefCellWidth();
			if ( iDefWidth > 0 )
			{
				nMaxWidth = iDefWidth;
			}
		}
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth+1, 0);
        pDC->DrawText(szText, -1, rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
        size = pDC->GetTextExtent(szText, (int)_tcslen(szText));
	
    // Removed by Yogurt
    //TEXTMETRIC tm;
    //pDC->GetTextMetrics(&tm);
    //size.cx += (tm.tmOverhang);
	
    if (pOldFont)
        pDC->SelectObject(pOldFont);
    
    size += CSize(2*GetMargin(), 2*GetMargin());
	
    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }
	else
	{
		if ( size.cx < pGrid->GetDefCellWidth() )		// ����С��Ĭ��ֵ
		{
			size.cx = pGrid->GetDefCellWidth();
		}
	}
    
    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);
	
    return size;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGridCtrlNormal, CGridCtrl)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CGridCtrlNormal, CGridCtrl)
//{{AFX_MSG_MAP(CGridCtrlNormal)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGridCtrlNormal::CGridCtrlNormal( int nRows /*= 0*/, int nCols /*= 0*/, int nFixedRows /*= 0*/, int nFixedCols /*= 0*/ )
:CGridCtrl(nRows, nCols, nFixedRows, nFixedCols)
{
	ASSERT( m_pRtcDefault != NULL );

	m_pDefaultVirtualModeCell = (CGridCellBase *)m_pRtcDefault->CreateObject();	// ����һ��Ĭ�ϵ�virtual��Ԫ�񣬻���������ʼ��
	m_pDefaultVirtualModeCell->SetGrid(this);
}

CGridCtrlNormal::~CGridCtrlNormal()
{
	delete m_pDefaultVirtualModeCell;
	m_pDefaultVirtualModeCell = NULL;
}

BOOL CGridCtrlNormal::SetDefaultCellType( CRuntimeClass* pRuntimeClass )
{
	if ( !CGridCtrl::SetDefaultCellType(pRuntimeClass) )
	{
		return FALSE;
	}
	ASSERT( m_pRtcDefault != NULL );
	
	delete m_pDefaultVirtualModeCell;
	m_pDefaultVirtualModeCell = (CGridCellBase *)m_pRtcDefault->CreateObject();
	m_pDefaultVirtualModeCell->SetGrid(this);
	return TRUE;
}

CGridCellBase* CGridCtrlNormal::GetCell( int nRow, int nCol ) const
{
	ASSERT( m_pDefaultVirtualModeCell != NULL );
	CGridCellBase *pCell = CGridCtrl::GetCell(nRow, nCol);
	if ( pCell != NULL && GetVirtualMode() )
	{
		*m_pDefaultVirtualModeCell = *pCell;
		return m_pDefaultVirtualModeCell;
	}
	return pCell;
}

CCellRange CGridCtrlNormal::GetVisibleNonFixedCellRange( LPRECT pRect /* = NULL */, BOOL bForceRecalculation /* = FALSE */ )
{
	return CGridCtrl::GetVisibleNonFixedCellRange(pRect, bForceRecalculation);
}




BOOL CGridCtrlNormal::DeleteNonFixedRows()
{
	if ( CGridCtrl::DeleteNonFixedRows() )
	{
		SetScrollPos32(SB_VERT, 0, FALSE);
		SetScrollPos32(SB_HORZ, 0, FALSE);
		return TRUE;
	}
	return FALSE;
}

void CGridCtrlNormal::EnsureVisible( int nRow, int nCol )
{
	if (!m_bAllowDraw)
        return;

	if ( !IsValid(nRow, nCol) )
	{
		return;
	}
	
	CGridCtrl::EnsureVisible(nRow, nCol);
}

void CGridCtrlNormal::EnsureVisible( const CCellID &cellId )
{
	if ( cellId.IsValid() )
	{
		EnsureVisible(cellId.row, cellId.col);
	}
}

bool32 CGridCtrlNormal::EnsureBottomRightCell( int nRow, int nCol )
{
	if (!m_bAllowDraw || !IsValid(nRow, nCol) )
        return false;
	
    CRect rectWindow;
	
    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    CWnd* pFocusWnd = GetFocus();	// ����ΪNull
	
    CCellRange VisibleCells = GetVisibleNonFixedCellRange();
	
    int iRight  = VisibleCells.GetMaxCol() - nCol;
    int bottom    = VisibleCells.GetMaxRow() - nRow;
	
    
    while( iRight > 0 )	// �Ҳ໹�пɼ���, ������
    {
        SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
        iRight--;
    }
	while ( iRight < 0 )	// �Լ����������Ҳ���, �Ҳ����
	{
		SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
		iRight++;
	}
	
	
    while( bottom > 0 )	// �²໹�пɼ��ģ��Ϲ���
    {
        SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        bottom--;
    }
	while( bottom < 0 )	// �Լ����������²��ˣ��¹���
	{
		SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
		bottom++;
	}

	// Move one more if we only see a snall bit of the cell
    CRect rectCell;
    if (!GetCellRect(nRow, nCol, rectCell))
    {
		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
			pFocusWnd->SetFocus(); 
        return false;
    }
	
    GetClientRect(rectWindow);

	CRect rectNonFixed(rectWindow);
	rectNonFixed.left += GetFixedColumnWidth();
	rectNonFixed.top += GetFixedRowHeight();

	// ��ֹ�ײ���n��������أ����Ϲ���
	if ( rectWindow.bottom - rectCell.bottom > rectCell.Height() )
	{
		while ( rectWindow.bottom - rectCell.bottom > rectCell.Height()
			&& IsVisibleVScroll() 
			)
		{
			CRect rectCellOld(rectCell);	// ��ʱ����ܳ��ֹ���һ�β��������
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
			if ( !GetCellRect(nRow, nCol, rectCell) )
			{
				::SetFocus(pFocusWnd->GetSafeHwnd());
				return false;
			}
		}
			
	}

	// �Ҳ��ж�������
// 	if ( rectWindow.right - rectCell.right > 0
// 		&& VisibleCells.GetMinCol() > GetFixedColumnCount()+1 )
// 	{
// 		while ( rectWindow.bottom - rectCell.bottom > rectCell.Height()
// 			&& IsVisibleVScroll() 
// 			)
// 		{
// 			CRect rectCellOld(rectCell);
// 			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
// 			if ( !GetCellRect(nRow, nCol, rectCell) )
// 			{
// 				::SetFocus(pFocusWnd->GetSafeHwnd());
// 				return false;
// 			}
// 			if ( rectCell.top == rectCellOld.top )	// �����͹�����
// 			{
// 				return true;	// ֻ�ܵ�����������
// 			}
// 		}
// 		
// 	}
	
	::SetFocus(pFocusWnd->GetSafeHwnd());

	EnsureVisible(nRow, nCol);		// ����ǰ�ĺ�����������΢����
	
	return true;
}

bool32 CGridCtrlNormal::EnsureTopLeftCell( int nRow, int nCol )
{
	if (!m_bAllowDraw || !IsValid(nRow, nCol) )
        return false;

// 	EnsureVisible(nRow, nCol);
// 	return true;
	
    CRect rectWindow;
	
    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    CWnd* pFocusWnd = GetFocus();	// ����ΪNull
	
    CCellRange VisibleCells = GetVisibleNonFixedCellRange();
	
    int iLeft  = VisibleCells.GetMinCol() - nCol;
    int up    = VisibleCells.GetMinRow() - nRow;
	
    int iColumnStart;
    int iRowStart;
	
    iColumnStart = VisibleCells.GetMaxCol() + 1;
    while( iLeft > 0 )	// ��������� - ��֤�Լ��ɼ�
    {
        if( GetColumnWidth( nCol ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
		
        iLeft--;
    }
	iColumnStart = nCol-1;		// ��ߵ����Ҫ���ص�
	while ( iLeft < 0 )	// �������Ҳ�
	{
		if ( GetColumnWidth(iColumnStart) > 0 )
			SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
		iLeft++;
	}
	
	
    iRowStart = nRow - 1;
    while( up > 0 )	// �����ڶ��� ��֤�Լ��ɼ�
    {
        if( GetRowHeight( nRow ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        up--;
    }
	iRowStart = nRow - 1;
	while( up < 0 )	// �������±� ���Լ�����һ�����ɼ�
	{
		if ( GetRowHeight( iRowStart ) > 0 )
			SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
		up++;
	}

	EnsureVisible(nRow, nCol);
	
    // Move one more if we only see a snall bit of the cell
//     CRect rectCell;
//     if (!GetCellRect(nRow, nCol, rectCell))
//     {
// 		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
// 			pFocusWnd->SetFocus(); 
//         return false;
//     }
// 	
//     GetClientRect(rectWindow);
// 	
//     // The previous fix was fixed properly by Martin Richter 
//     while (rectCell.right > rectWindow.right
//         && rectCell.left > GetFixedColumnWidth()
//         && IsVisibleHScroll() // Junlin Xu: added to prevent infinite loop
//         )
//     {
//         SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
//         if (!GetCellRect(nRow, nCol, rectCell))
//         {
//             // pFocusWnd->SetFocus();  // focus����ΪNULL
// 			::SetFocus(pFocusWnd->GetSafeHwnd());
//             return false;
//         }
//     }
// 	
//     while (rectCell.bottom > rectWindow.bottom
//         && rectCell.top > GetFixedRowHeight()
//         && IsVisibleVScroll() // Junlin Xu: added to prevent infinite loop
//         )
//     {
//         SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
//         if (!GetCellRect(nRow, nCol, rectCell))
//         {
//             //pFocusWnd->SetFocus(); 
// 			::SetFocus(pFocusWnd->GetSafeHwnd());
//             return false;
//         }
//     }
// 	
//     // restore focus to whoever owned it
//     //pFocusWnd->SetFocus(); 
	
	::SetFocus(pFocusWnd->GetSafeHwnd());
	
	return true;
}

bool32 CGridCtrlNormal::EnsureTopLeftCell( const CCellID &cell )
{
	return EnsureTopLeftCell(cell.row, cell.col);
}

bool32 CGridCtrlNormal::EnsureBottomRightCell( const CCellID &cellId )
{
	if ( cellId.IsValid() )
	{
		return EnsureBottomRightCell(cellId.row, cellId.col);
	}
	return false;
}

void CGridCtrlNormal::ResetVirtualOrder()
{
	m_arRowOrder.resize(m_nRows);  
    for (int i = 0; i < m_nRows; i++)
	{
		m_arRowOrder[i] = i;	
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGridCtrlNormalSys, CGridCtrlSys)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CGridCtrlNormalSys, CGridCtrlSys)
//{{AFX_MSG_MAP(CGridCtrlNormal)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGridCtrlNormalSys::CGridCtrlNormalSys( int nRows /*= 0*/, int nCols /*= 0*/, int nFixedRows /*= 0*/, int nFixedCols /*= 0*/ )
:CGridCtrlSys(nRows, nCols, nFixedRows, nFixedCols)
{
	ASSERT( m_pRtcDefault != NULL );
	
	m_pDefaultVirtualModeCell = (CGridCellBase *)m_pRtcDefault->CreateObject();	// ����һ��Ĭ�ϵ�virtual��Ԫ�񣬻���������ʼ��
	m_pDefaultVirtualModeCell->SetGrid(this);
}

CGridCtrlNormalSys::~CGridCtrlNormalSys()
{
	delete m_pDefaultVirtualModeCell;
	m_pDefaultVirtualModeCell = NULL;
}

BOOL CGridCtrlNormalSys::SetDefaultCellType( CRuntimeClass* pRuntimeClass )
{
	if ( !CGridCtrlSys::SetDefaultCellType(pRuntimeClass) )
	{
		return FALSE;
	}
	ASSERT( m_pRtcDefault != NULL );
	
	delete m_pDefaultVirtualModeCell;
	m_pDefaultVirtualModeCell = (CGridCellBase *)m_pRtcDefault->CreateObject();
	m_pDefaultVirtualModeCell->SetGrid(this);
	return TRUE;
}

CGridCellBase* CGridCtrlNormalSys::GetCell( int nRow, int nCol ) const
{
	ASSERT( m_pDefaultVirtualModeCell != NULL );
	CGridCellBase *pCell = CGridCtrlSys::GetCell(nRow, nCol);
	if ( pCell != NULL && GetVirtualMode() )
	{
		*m_pDefaultVirtualModeCell = *pCell;	// �������ֻ᲻�Ḳ�ǣ� - ���ᣬ�����Ļ����Ͳ�����=��ֱ�ӵ�������
		return m_pDefaultVirtualModeCell;
	}
	return pCell;
}

void CGridCtrlNormalSys::ExpandColumnsToFit( BOOL bExpandFixed /*= TRUE*/ )
{
	CGridCtrlSys::ExpandColumnsToFit(bExpandFixed);
}

void CGridCtrlNormalSys::ExpandRowsToFit( BOOL bExpandFixed /*= TRUE*/ )
{
	CGridCtrlSys::ExpandRowsToFit(bExpandFixed);
}

bool32 CGridCtrlNormalSys::EnsureTopLeftCell( const CCellID &cell )
{
	if ( !cell.IsValid() )
	{
		return false;
	}

	return EnsureTopLeftCell(cell.row, cell.col);
}

bool32 CGridCtrlNormalSys::EnsureTopLeftCell( int nRow, int nCol )
{
	if (!m_bAllowDraw || !IsValid(nRow, nCol) )
        return false;

// 	EnsureVisible(nRow, nCol);
// 	return true;
	
    CRect rectWindow;
	
    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    CWnd* pFocusWnd = GetFocus();	// ����ΪNull
	
    CCellRange VisibleCells = GetVisibleNonFixedCellRange();
	
    int iLeft  = VisibleCells.GetMinCol() - nCol;
    int up    = VisibleCells.GetMinRow() - nRow;
	
    int iColumnStart;
    int iRowStart;
	
    iColumnStart = VisibleCells.GetMaxCol() + 1;
    while( iLeft > 0 )	// ��������� - ��֤�Լ��ɼ�
    {
        if( GetColumnWidth( nCol ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
		
        iLeft--;
    }
	iColumnStart = nCol-1;		// ��ߵ����Ҫ���ص�
	while ( iLeft < 0 )	// �������Ҳ�
	{
		if ( GetColumnWidth(iColumnStart) > 0 )
			SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
		iLeft++;
	}
	
	
    iRowStart = nRow - 1;
    while( up > 0 )	// �����ڶ��� ��֤�Լ��ɼ�
    {
        if( GetRowHeight( nRow ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        up--;
    }
	iRowStart = nRow - 1;
	while( up < 0 )	// �������±� ���Լ�����һ�����ɼ�
	{
		if ( GetRowHeight( iRowStart ) > 0 )
			SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
		up++;
	}

	EnsureVisible(nRow, nCol);
	
    // Move one more if we only see a snall bit of the cell
//     CRect rectCell;
//     if (!GetCellRect(nRow, nCol, rectCell))
//     {
// 		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
// 			pFocusWnd->SetFocus(); 
//         return false;
//     }
// 	
//     GetClientRect(rectWindow);
// 	
//     // The previous fix was fixed properly by Martin Richter 
//     while (rectCell.right > rectWindow.right
//         && rectCell.left > GetFixedColumnWidth()
//         && IsVisibleHScroll() // Junlin Xu: added to prevent infinite loop
//         )
//     {
//         SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
//         if (!GetCellRect(nRow, nCol, rectCell))
//         {
//             // pFocusWnd->SetFocus();  // focus����ΪNULL
// 			::SetFocus(pFocusWnd->GetSafeHwnd());
//             return false;
//         }
//     }
// 	
//     while (rectCell.bottom > rectWindow.bottom
//         && rectCell.top > GetFixedRowHeight()
//         && IsVisibleVScroll() // Junlin Xu: added to prevent infinite loop
//         )
//     {
//         SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
//         if (!GetCellRect(nRow, nCol, rectCell))
//         {
//             //pFocusWnd->SetFocus(); 
// 			::SetFocus(pFocusWnd->GetSafeHwnd());
//             return false;
//         }
//     }
// 	
//     // restore focus to whoever owned it
//     //pFocusWnd->SetFocus(); 
	
	::SetFocus(pFocusWnd->GetSafeHwnd());
	
	return true;
}

CCellRange CGridCtrlNormalSys::GetVisibleNonFixedCellRange( LPRECT pRect /*= NULL*/, BOOL bForceRecalculation /*= FALSE */, BOOL bContainFragment/*=TRUE*/ )
{
	return CGridCtrlSys::GetVisibleNonFixedCellRange(pRect, bForceRecalculation, bContainFragment);
}

BOOL CGridCtrlNormalSys::DeleteNonFixedRows()
{
	if ( CGridCtrlSys::DeleteNonFixedRows() )
	{
		SetScrollPos32(SB_VERT, 0, FALSE);
		SetScrollPos32(SB_HORZ, 0, FALSE);
		return TRUE;
	}
	return FALSE;
}

void CGridCtrlNormalSys::EnsureVisible( int nRow, int nCol )
{
	if (!m_bAllowDraw)
        return;

	if ( !IsValid(nRow, nCol) )
	{
		return;
	}
	
	CGridCtrlSys::EnsureVisible(nRow, nCol);
}

void CGridCtrlNormalSys::EnsureVisible( const CCellID &cellId )
{
	if ( cellId.IsValid() )
	{
		EnsureVisible(cellId.row, cellId.col);
	}
}

bool32 CGridCtrlNormalSys::EnsureBottomRightCell( int nRow, int nCol )
{
	if (!m_bAllowDraw || !IsValid(nRow, nCol) )
        return false;
	
    CRect rectWindow;
	
    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    CWnd* pFocusWnd = GetFocus();	// ����ΪNull
	
    CCellRange VisibleCells = GetVisibleNonFixedCellRange();
	
    int iRight  = VisibleCells.GetMaxCol() - nCol;
    int bottom = VisibleCells.GetMaxRow() - nRow;
	
    
    while( iRight > 0 )	// �Ҳ໹�пɼ���, ������
    {
        SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
        iRight--;
    }
	while ( iRight < 0 )	// �Լ����������Ҳ���, �Ҳ����
	{
		SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
		iRight++;
	}
	
	
    while( bottom > 0 )	// �²໹�пɼ��ģ��Ϲ���
    {
        SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        bottom--;
    }
	while( bottom < 0 )	// �Լ����������²��ˣ��¹���
	{
		SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
		bottom++;
	}

	// Move one more if we only see a snall bit of the cell
    CRect rectCell;
    if (!GetCellRect(nRow, nCol, rectCell))
    {
		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
			pFocusWnd->SetFocus(); 
        return false;
    }
	
    GetClientRect(rectWindow);

	CRect rectNonFixed(rectWindow);
	rectNonFixed.left += GetFixedColumnWidth();
	rectNonFixed.top += GetFixedRowHeight();

	// ��ֹ�ײ���n��������أ����Ϲ���
	if ( rectWindow.bottom - rectCell.bottom > rectCell.Height() )
	{
		while ( rectWindow.bottom - rectCell.bottom > rectCell.Height()
			&& IsVisibleVScroll() 
			)
		{
			CRect rectCellOld(rectCell);	// ��ʱ����ܳ��ֹ���һ�β��������
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
			if ( !GetCellRect(nRow, nCol, rectCell) )
			{
				::SetFocus(pFocusWnd->GetSafeHwnd());
				return false;
			}
		}
			
	}

	// �Ҳ��ж�������
// 	if ( rectWindow.right - rectCell.right > 0
// 		&& VisibleCells.GetMinCol() > GetFixedColumnCount()+1 )
// 	{
// 		while ( rectWindow.bottom - rectCell.bottom > rectCell.Height()
// 			&& IsVisibleVScroll() 
// 			)
// 		{
// 			CRect rectCellOld(rectCell);
// 			SendMessage(WM_VSCROLL, SB_LINEUP, 0);
// 			if ( !GetCellRect(nRow, nCol, rectCell) )
// 			{
// 				::SetFocus(pFocusWnd->GetSafeHwnd());
// 				return false;
// 			}
// 			if ( rectCell.top == rectCellOld.top )	// �����͹�����
// 			{
// 				return true;	// ֻ�ܵ�����������
// 			}
// 		}
// 		
// 	}
	
	::SetFocus(pFocusWnd->GetSafeHwnd());

	EnsureVisible(nRow, nCol);		// ����ǰ�ĺ�����������΢����
	
	return true;
}

bool32 CGridCtrlNormalSys::EnsureBottomRightCell( const CCellID &cellId )
{
	if ( cellId.IsValid() )
	{
		return EnsureBottomRightCell(cellId.row, cellId.col);
	}
	return false;
}

void CGridCtrlNormalSys::ResetVirtualOrder()
{
	m_arRowOrder.resize(m_nRows);  
    for (int i = 0; i < m_nRows; i++)
	{
		m_arRowOrder[i] = i;	
	}
}


//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNCREATE(CGridCellNormalSys, CGridCellSymbol)

BOOL CGridCellNormalSys::Draw( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*= TRUE*/ )
{
	// �����ַ����׸��ַ���+��-���жϼ۸�����/��	
	COLORREF ClrText = GetTextClr();  // ���û��ָ����ɫ����ʹ��ԭ���ģ����ָ������ɫ����ʹ��ָ����
	COLORREF clrCell = GetTextClr();
	
	CIoViewBase * pIoViewParent = NULL; 
	CWnd * pParent = GetGrid()->GetParent();
	
	if ( pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
	{
		pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);
	}

	if ( NULL != pIoViewParent)
	{
		ClrText = pIoViewParent->GetIoViewColor(ESCKeep);
	}
	else
	{
		ClrText = CFaceScheme::Instance()->GetSysColor(ESCKeep);
	}


	bool32 bSkipFirstChar = false;

	CString StrTextBackup = GetText();
	if (StrTextBackup.GetLength() > 0)
	{
		if (StrTextBackup[0] == L'+')
		{			
			if ( NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCRise);
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCRise);
			}						
			if (!(m_iShowSymbol & ESSRise))
				bSkipFirstChar = true;
		}
		else if (StrTextBackup[0] == L'-')
		{
			if ( NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCFall);
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCFall);
			}
			
			if (!(m_iShowSymbol & ESSFall))
				bSkipFirstChar = true;
		}
		else
		{
			if ( NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCKeep);	
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCKeep);
			}						
			if (!(m_iShowSymbol & ESSKeep))
			{
				if (StrTextBackup[0] == L' ')
				{
					bSkipFirstChar = true;
				}
			}
		}		
	}
	
	if (bSkipFirstChar)
	{
		CString StrText = StrTextBackup.Right(StrTextBackup.GetLength() - 1);
		
		//SetTextOnly(StrText);
		m_strText = StrText;
	}	
	
	if ( clrCell == CLR_DEFAULT )// ûָ����ɫ����ʹ�ø��ݷ����жϵ�
	{
		SetTextClr(ClrText);
	}
	
	//BOOL bResult = CGridCellSys::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd); // ʹ���������
	BOOL bResult = DoDraw(pDC, nRow, nCol, rect, bEraseBkgnd); // ʹ���������

	if (bSkipFirstChar)
	{
		//SetTextOnly(StrTextBackup);
		m_strText = StrTextBackup;
	}

	//	������������ö�����Ľǣ�����û���б�ͷ������Ƶ�һ������ͷβ���������������ν�
	DrawVisbleFirstRowPolygonConner(pDC, rect, nRow, nCol);

	
	return bResult;
}

BOOL CGridCellNormalSys::DoDraw( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*= TRUE*/ )
{
	// Note - all through this function we totally brutalise 'rect'. Do not
    // depend on it's value being that which was passed in.
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);
	
    if (!pGrid || !pDC)
        return FALSE;
	
    if( rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;                             //  though cell is hidden

	bool32 bDrawSelectAndFocus = (pGrid->GetSafeHwnd() == GetFocus()) || pGrid->IsShowSelectWhenLoseFocus();		// �������ѡ��

	CIoViewBase * pIoViewParent = NULL; 
	CWnd * pParent = GetGrid()->GetParent();
	pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);
	
	if (IsFixedRow())
	{
		COLORREF colBk;

		if (NULL != pIoViewParent)
		{			
			colBk = pIoViewParent->GetIoViewColor(ESCGridFixedBk);
		}
		else
		{
			colBk = CFaceScheme::Instance()->GetSysColor(ESCGridFixedBk); 
		}
		SetBackClr(colBk);
	}


	CRect rectGrid;
	pGrid->GetClientRect(&rectGrid);
	
	bool32 bTooSmallCellForText = false;
	if ( pGrid->IsAutoHideFragmentaryCell() && (rect.left < rectGrid.left || rect.right > rectGrid.right || rect.top < rectGrid.top || rect.bottom > rectGrid.bottom) )
	{
		COLORREF colBk = GetBackClr();
		
		if ( NULL != pIoViewParent && colBk == CLR_DEFAULT )
		{
			colBk = pIoViewParent->GetIoViewColor(ESCBackground); 
		}

		// ����ط�Ҫ����д TODO
		if ( pGrid->GetGridLines() == GVL_NONE )
		{
			CRect rectDraw = rect;
			rectDraw.left --;
			rectDraw.right++;
			//rectDraw.top--;
			rectDraw.bottom++;
			pDC->FillSolidRect(rectDraw, colBk);
			
			return true;
		}
		else
		{
			bTooSmallCellForText = true;
		}
	}

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

	DWORD nDrawSelectedStyle = pGrid->GetDrawSelectedCellStyle();

    // Set up text and background colours
    COLORREF TextClr, TextBkClr, TextNormalBkClr, ClrSelected;
	const CSize	 SizeText  = GetTextExtent(GetText(), pDC);

	if (  NULL != pIoViewParent)
	{
		TextClr = (GetTextClr() == CLR_DEFAULT) ? pIoViewParent->GetIoViewColor(m_eDefaultTextColor) : GetTextClr(); 
	}
	else
	{
		TextClr = (GetTextClr() == CLR_DEFAULT) ? CFaceScheme::Instance()->GetSysColor(m_eDefaultTextColor) : GetTextClr();
	}
	
	if (GetBackClr() == CLR_DEFAULT)
	{
		if (  NULL != pIoViewParent)
		{
			TextBkClr = pIoViewParent->GetIoViewColor(m_eDefaultTextBkColor);
		}
		else
		{
			TextBkClr = CFaceScheme::Instance()->GetSysColor(m_eDefaultTextBkColor);
		}
	}
	else
	{
		bEraseBkgnd = TRUE;
		TextBkClr = GetBackClr();
	}
	TextNormalBkClr = TextBkClr;
	ClrSelected		= TextBkClr;
	if ( bDrawSelectAndFocus && (GetState() & GVIS_SELECTED) )
	{
		if (  NULL != pIoViewParent)
		{
			ClrSelected = pIoViewParent->GetIoViewColor(ESCGridSelected);
		}
		else
		{
			ClrSelected = CFaceScheme::Instance()->GetSysColor(ESCGridSelected);	
		}
		if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
		{
			// ���߻���������Ϊ��ͨ����ɫ
		}
		else
		{
			// Ĭ�ϻ�����select�£�����Ϊselectɫ
			TextBkClr = ClrSelected;
		}
	}

	//if (  !bTooSmallCellForText )
	{
		// Draw cell background and highlighting (if necessary)
		if ( bDrawSelectAndFocus && (IsFocused() || IsDropHighlighted()) )	// �н���
		{
			// Always draw even in list mode so that we can tell where the
			// cursor is at.  Use the highlight colors though.
			if(GetState() & GVIS_SELECTED)
			{
				// 			CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
				// 			CMPIChildFrame * pParentFrame = (CMPIChildFrame *)GetGrid()->GetParentFrame();
				// 			
				// 			if (bMpiChildFrame && (*pMainFrame) != (*pParentFrame))
				// 			{				
				// 				TextBkClr = pParentFrame->GetFrameColor(ESCGridSelected);
				// 			}
				// 			else
				// 			{
				// 				TextBkClr = CFaceScheme::Instance()->GetSysColor(ESCGridSelected);
				//          }
				bEraseBkgnd = TRUE;
			}
			
			// zhangbo 0402 # for blink
			if ( !bTooSmallCellForText && (GetState() & GVIS_BLINK && m_iBlinkStep > 0) )
			{
				COLORREF clrBkOld = TextBkClr;
				if ( clrBkOld == 0xee0000 )
				{
					clrBkOld = RGB(127,127,127);
				}
				CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
				bEraseBkgnd = TRUE;
			}
			
			rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
			if (bEraseBkgnd)
			{
				TRY 
				{
					CBrush brush(TextBkClr);
					if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
					{
						// ����ɫ
						pDC->FillRect(rect, &brush);
						
						// �ײ���һ����
						CRect rcTmp(rect);
						rcTmp.bottom -= 1;
						rcTmp.top = rcTmp.bottom-1;
						pDC->FillSolidRect(rcTmp, ClrSelected);
						pDC->SetBkColor(TextBkClr);
					}
					else
					{
						pDC->FillRect(rect, &brush);
					}
				}
				CATCH(CResourceException, e)
				{
					//e->ReportError();
				}
				END_CATCH
			}
			
			// Don't adjust frame rect if no grid lines so that the
			// whole cell is enclosed.
			if(pGrid->GetGridLines() != GVL_NONE)
			{
				rect.right--;
				rect.bottom--;
			}
			
			if (pGrid->GetFrameFocusCell() && pGrid->IsSelectable() && GVSDS_BOTTOMLINE != nDrawSelectedStyle )
			{
				// Use same color as text to outline the cell so that it shows
				// up if the background is black.
				TRY 
				{
					CBrush brush(TextClr);
// 					pDC->FrameRect(rect, &brush);
				}
				CATCH(CResourceException, e)
				{
					//e->ReportError();
				}
				END_CATCH
			}
			
			// Adjust rect after frame draw if no grid lines
			if(pGrid->GetGridLines() == GVL_NONE)
			{
				rect.right--;
				rect.bottom--;
			}
			
			//rect.DeflateRect(0,1,1,1);  - Removed by Yogurt
		}
		else if ( bDrawSelectAndFocus && (GetState() & GVIS_SELECTED))	// ��ѡ��
		{			
			CString StrTest;
			if ( pGrid->GetVirtualMode() )		// ��ģʽ�²�Ҫ����getitemtext - ��ʵ������gettext xl 0810
			{
				StrTest = GetText();
			}
			else
			{
				StrTest	=  pGrid->GetItemText(nRow,nCol);
			}
			
			// zhangbo 0402 # for blink
			if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
			{
				COLORREF clrBkOld = TextBkClr;
				if ( clrBkOld == 0xee0000 )
				{
					clrBkOld = RGB(127,127,127);
				}
				CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
				bEraseBkgnd = TRUE;
			}
			else
			{		
				bEraseBkgnd = TRUE;
			}
			
			rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
			if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
			{
				// ����ɫ
				pDC->FillSolidRect(rect, TextBkClr);
				// �ײ���һ����
				CRect rcTmp(rect);
				rcTmp.bottom -= 1;
				rcTmp.top = rcTmp.bottom-1;
				pDC->FillSolidRect(rcTmp, ClrSelected);
				pDC->SetBkColor(TextBkClr);
			}
			else
			{
				pDC->FillSolidRect(rect, TextBkClr);
			}
			rect.right--; rect.bottom--;
		}
		else
		{
			// zhangbo 0402 # for blink
			if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
			{
				COLORREF clrBkOld = TextBkClr;
				if ( clrBkOld == 0xee0000 )
				{
					clrBkOld = RGB(127,127,127);
				}
				CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
				bEraseBkgnd = TRUE;
			}
			
			if (bEraseBkgnd)
			{
				rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
				CBrush brush(TextBkClr);
				pDC->FillRect(rect, &brush);
				rect.right--; rect.bottom--;
			}
		}

	}

	
    // fix��һ��Ҫ��ʾ�ָ���
    if ( IsFixed() )
    {
      //  CCellID FocusCell = pGrid->GetFocusCell();
		
        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
  //      BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
		//	(FocusCell.row == nRow || FocusCell.col == nCol);
		//
		//bool32 bValid = pGrid->IsValid(FocusCell);		

        CPen PenGridLine;

		if ( NULL != pIoViewParent)
		{
			PenGridLine.CreatePen(PS_SOLID, 1, pIoViewParent->GetIoViewColor(ESCGridLine));
		}
		else
		{
			PenGridLine.CreatePen(PS_SOLID, 1, CFaceScheme::Instance()->GetSysColor(ESCGridLine));
		}

		if ( GVL_NONE != GetGrid()->GetGridLines() )
		{
 			CPen *pOldPen = pDC->GetCurrentPen();
			CRect rectDraw = rect;
		
			if ( pGrid->IsDrawFixedCellGridLineAsNormal() )	// ������������߻���
			{
				rectDraw.InflateRect(1, 1, 0, 0);		// ��Ҫ�ʵ�����
				if ( rectDraw.left < rectGrid.left )
				{
					rectDraw.left = rectGrid.left;
				}
				if ( rectDraw.top < rectGrid.top )
				{
					rectDraw.top = rectGrid.top;
				}
			}
			
			pDC->SelectObject(&PenGridLine);
			pDC->MoveTo(rectDraw.left, rectDraw.top);
			pDC->LineTo(rectDraw.left, rectDraw.bottom);
			pDC->LineTo(rectDraw.right, rectDraw.bottom);
			pDC->LineTo(rectDraw.right, rectDraw.top);
			pDC->LineTo(rectDraw.left, rectDraw.top);
			
			pDC->SelectObject(pOldPen);
			
				rect = rectDraw;	// rect��Ҫ���
		}
				
		rect.DeflateRect(1,1);

		if ( bTooSmallCellForText )
		{
			return false;		// ���治��Ҫ������
		}
    }

    // Draw Text and image
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    if (!pDC->m_bPrinting)
#endif
    {
        CFont *pFont = GetFontObject();
		ASSERT(pFont);
        if (pFont)
            pDC->SelectObject(pFont);
    }
	
    //rect.DeflateRect(GetMargin(), 0); - changed by Yogurt
    rect.DeflateRect(GetMargin(), GetMargin());    
    rect.right++;    
    rect.bottom++;
	
	// ����ͼƬ
	CRect RectText = rect;
	CRect RectImage= rect;
	CRect RectTextByDC = rect;
	
	GetTextRect(RectTextByDC);
	RectText = RectTextByDC;
	RectImage.left = RectText.right;
	
    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();
			
            /*
            // removed by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;
            if( nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
			&& nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin())  )
            {
			pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            */
            // Added by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top;            
			//            if ((nImageWidth + RectImage.left <= RectImage.right) && (nImageHeight + RectImage.top <= RectImage.bottom))                
			



			CPoint PtTopLeft;			
			DWORD dFormat = GetFormat();
			bool32 bNotLeft = (dFormat&DT_RIGHT)|(dFormat&DT_CENTER);
			
			if ( !bNotLeft )
			{			
				RectText.left  += (nImageWidth + 1);
				RectText.right += (nImageWidth + 1);
				
				PtTopLeft.x = rect.left + 1;
				PtTopLeft.y = RectImage.top + (RectImage.Height() - nImageHeight) / 2;				
								
			}
			else
			{
				PtTopLeft.x = RectImage.left + (RectImage.Width() - nImageWidth) / 2;			
				PtTopLeft.y = RectImage.top + (RectImage.Height() - nImageHeight) / 2;				
			}
			
			pGrid->GetImageList()->Draw(pDC, GetImage(), PtTopLeft, ILD_NORMAL);
			
            //rect.left += nImageWidth+GetMargin();
        }
    }
	
    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0 && !bTooSmallCellForText /* && pGrid->BeDrawSortArrow()*/)
    {
		// ��gridsys��֮ͬ�� - ���򻭵Ĳ�ͬ
		// ���ֵĳߴ� - ��������󣬽������rect���޳�
		//CSize size = pDC->GetTextExtent(GetText());
		CSize size = SizeText;//GetTextExtent(GetText(), pDC);
		
		CPen  penArrow(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&penArrow);
		CPoint ptUp, ptDown, ptLeft, ptRight;

		// ��gridsys��֮ͬ�� - ���򻭵Ĳ�ͬ, �����Ȳ�������ʵ������ử����ʾ��������
		if ( GetFormat() & DT_CENTER )
		{
			// �ж��� - �����
			ptUp.x = rect.CenterPoint().x - (size.cx / 2) - 2; 
			if ( ptUp.x + 3 > RectText.left )
			{
				RectText.left = ptUp.x + 3;		// ���������ų�
			}
		}
		else if ( GetFormat() & DT_RIGHT )
		{
			// �Ҷ��� - �����
			ptUp.x = rect.right - size.cx - 3;
			if ( ptUp.x + 3 > RectText.left )
			{
				RectText.left = ptUp.x + 3;		// ���������ų�
			}
		}
		else 
		{
			// ����� - ���ұ�
			ptUp.x = rect.left + size.cx + 3;
			if ( ptUp.x - 3 < RectText.right )
			{
				RectText.right = ptUp.x - 3;		// ���������ų�
			}
		}

		ptUp.y = rect.top + (rect.Height() - 14) /2;
		ptDown.y = rect.bottom - 1;
		if ( ptDown.y - ptUp.y > 14 ) // ���Ʋ���̫��, ���߶���
		{
			ptDown.y = ptUp.y + 14;
		}
		
		// 
		if ( (pGrid->GetSortAscending()) )
		{
			//
			ptDown.x = ptUp.x;
			
			//
			ptLeft.x = ptUp.x - 2;
			ptLeft.y = ptUp.y + 2;
			//
			ptRight.x= ptUp.x + 2 +1 ;
			ptRight.y= ptLeft.y;
			
			
			pDC->MoveTo(ptUp);
			pDC->LineTo(ptDown);
			
			CPoint ptUp1,ptDown1;
			ptUp1.x = ptUp.x + 1;
			ptUp1.y = ptUp.y;
			ptDown1.x = ptDown.x + 1;
			ptDown1.y = ptDown.y;
			
			pDC->MoveTo(ptUp1);
			pDC->LineTo(ptDown1);
			
			pDC->MoveTo(ptUp);
			pDC->LineTo(ptLeft);
			pDC->LineTo(ptRight);
			pDC->LineTo(ptUp1);
			pDC->LineTo(ptUp);
		}
		else
		{
			//
			ptDown.x = ptUp.x;
			//
			ptLeft.x = ptUp.x - 2;
			ptLeft.y = ptDown.y - 2;
			//
			ptRight.x= ptUp.x + 2 + 1 ;
			ptRight.y= ptLeft.y;				
			
			pDC->MoveTo(ptUp);
			pDC->LineTo(ptDown);
			
			CPoint ptUp1,ptDown1;
			ptUp1.x = ptUp.x + 1;
			ptUp1.y = ptUp.y;
			ptDown1.x = ptDown.x + 1;
			ptDown1.y = ptDown.y;
			
			pDC->MoveTo(ptUp1);
			pDC->LineTo(ptDown1);
			
			pDC->MoveTo(ptDown);
			pDC->LineTo(ptLeft);
			pDC->LineTo(ptRight);
			pDC->LineTo(ptDown1);
			pDC->LineTo(ptDown);
		}
		
		pDC->SelectObject(pOldPen);
		penArrow.DeleteObject();
    }

    // We want to see '&' characters so use DT_NOPREFIX
    RectText.right++;    
    RectText.bottom++;
	
	if ( EDTNS_HorzPercentStick == m_eDTNS )
	{
		// ����
		pDC->SetTextColor(TextClr);
		pDC->SetBkMode(TRANSPARENT);
		
		CString StrText = GetText();
		LPTSTR pStop;
		double dRadio = _tcstod(StrText, &pStop);
		dRadio = min(1.0, dRadio);
		dRadio = max(0.0, dRadio);

		DWORD dwFmt = GetFormat();

		CRect rcDraw(RectText);
		rcDraw.InflateRect(0, -rcDraw.Height()/4);	// ���ٸ߶�
		if ( DT_RIGHT&dwFmt )	// ��֧��left&right
		{
			rcDraw.left = (int)(rcDraw.right - (1.0-dRadio)*RectText.Width());
			if ( rcDraw.left >= rcDraw.right )
			{
				rcDraw.left = rcDraw.right-1;
			}
		}
		else
		{
			rcDraw.right = (int)(rcDraw.left + RectText.Width()*dRadio);
			if ( rcDraw.right <= rcDraw.left )
			{
				rcDraw.right = rcDraw.left+1;
			}
		}
		
		pDC->FillSolidRect(rcDraw, TextClr);
	}
	else
	{
		// ����
		pDC->SetTextColor(TextClr);
		pDC->SetBkMode(TRANSPARENT);
		
		if ( !bTooSmallCellForText )
		{
			DWORD dwFmt = GetFormat();
			if ( 0 != (dwFmt & DT_VCENTER) && 0 == (dwFmt & DT_SINGLELINE) )		// �ǵ�����Ҫ����ֱ���룬������������
			{
				if ( rect.Height() > SizeText.cy + 1 )
				{
					RectText.OffsetRect(0, (rect.Height()-SizeText.cy)/2);
				}
			}
			//DrawText(pDC->m_hDC, GetText(), -1, RectText, GetFormat() | DT_NOPREFIX);

			// Ԥ�����λ��, ������ǻ����ұ�
			CString StrText = GetText();
			CSize sizeText = GetTextExtent(StrText, pDC);
			CSize sizeMark = GetDrawMarkTextExtent(pDC);
			CRect RectDrawText(RectText);
			RectDrawText.DeflateRect(m_rtTextPadding);
			RectDrawText.right -= sizeMark.cx;
			if ( RectDrawText.right < RectDrawText.left
				|| ((!(GetFormat()&DT_RIGHT)&&!(GetFormat()&DT_CENTER))&&RectDrawText.Width()<sizeText.cx) )
			{
				RectDrawText.right = RectText.right;	// ���̫С���������ʱ�����ط�����ǣ��������
			}
			//DrawText(pDC->m_hDC, GetText(), -1, RectText, GetFormat() | DT_NOPREFIX);
			DrawText(pDC->m_hDC, GetText(), -1, RectDrawText, GetFormat() | DT_NOPREFIX);
			
			const GV_DRAWMARK_ITEM &markItem = GetDrawMarkItem();
			if ( EDMF_None != markItem.eDrawMarkFlag )
			{
				// ���Ʊ������, ������ˮƽ����
				CRect rcMarkItem(RectText);
				rcMarkItem.InflateRect(0, GetMargin());
				
				DWORD nFormat = GetFormat();
				COLORREF clrMark = markItem.clrMark;
				DWORD nMarkFmt = DT_SINGLELINE |DT_LEFT;
				if ( DT_CENTER & nFormat )	// ����, ����뻭�������ұ�
				{
					rcMarkItem.left = RectDrawText.right - (RectDrawText.Width()-sizeText.cx)/2;
				}
				else if ( DT_RIGHT & nFormat ) // ����, �Ҷ��뻭�������ұ�
				{
					rcMarkItem.left = rcMarkItem.right - sizeMark.cx;
				}
				else // ���� ����뻭�������ұ�
				{
					rcMarkItem.left += sizeText.cx;
				}
				
				if ( EDMF_Bottom == markItem.eDrawMarkFlag )
				{
					nMarkFmt |= DT_BOTTOM;
				}
				else
				{
					nMarkFmt |= DT_TOP;
				}
				
				if ( clrMark == CLR_DEFAULT )
				{
					if ( NULL != pIoViewParent )
					{
						clrMark = pIoViewParent->GetIoViewColor(ESCAmount);
					}
					else
					{
						clrMark = CFaceScheme::Instance()->GetSysColor(ESCAmount);
					}
				}
				COLORREF clrOld = pDC->SetTextColor(clrMark);
				CFont *pOldFont2 = NULL;
				CFont FontMark;
				InitDrawMarkFont(FontMark);
				pOldFont2 = pDC->SelectObject(&FontMark);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(markItem.strMark, rcMarkItem, nMarkFmt);
				pDC->SelectObject(pOldFont2);
				pDC->SetTextColor(clrOld);
			}
		}
	}
	
	//
	CPen PenLine;	
	COLORREF clrLine = m_ClrRectCellLineClr;
	
	if ( (COLORREF)-1 == clrLine )
	{
		if (  NULL != pIoViewParent)
		{
			clrLine = pIoViewParent->GetIoViewColor(ESCChartAxisLine);			
		}
		else
		{
			clrLine = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);			
		}
	}
	
	PenLine.CreatePen(PS_SOLID, 1, clrLine);
	CPen * pOldpen = pDC->GetCurrentPen();
	pDC->SelectObject(PenLine);
	
  	if ( CheckFlag(m_uiCellRectFlag, DR_TOP_DOT) )
  	{
  		// ������
  		DrawDotLine(pDC, CPoint(rect.left - 3, rect.top - 1), CPoint(rect.right + 3, rect.top - 1), clrLine);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_TOP_SOILD) )
  	{
  		// ��ʵ��	
  		pDC->MoveTo(rect.left - 3, rect.top - 1);
  		pDC->LineTo(rect.right + 3, rect.top - 1);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_DOT) )
  	{
  		// ������
  		DrawDotLine(pDC, CPoint(rect.left - 2, rect.bottom + 1), CPoint(rect.right + 2, rect.bottom + 1), clrLine);
  	}
	
	if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_SOILD) )
	{
		// ��ʵ��
		pDC->MoveTo(rect.left - 3, rect.bottom + 1);
		pDC->LineTo(rect.right + 3, rect.bottom + 1);
	}
	
  	if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_DOT) )
  	{
  		// ������
  		DrawDotLine(pDC, CPoint(rect.left - 1, rect.top - 2), CPoint(rect.left-1, rect.bottom + 2), clrLine);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_SOILD) )
  	{
  		// ��ʵ��
  		pDC->MoveTo(rect.left - 1, rect.top - 2);
  		pDC->LineTo(rect.left - 1, rect.bottom + 2);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_DOT) )
  	{
  		// ������
		if ( IsFixed() )
		{
			DrawDotLine(pDC, CPoint(rect.right + 2, rect.top - 2), CPoint(rect.right + 2, rect.bottom + 2), clrLine);
		}
		else
		{
  			DrawDotLine(pDC, CPoint(rect.right + 1, rect.top - 2), CPoint(rect.right + 1, rect.bottom + 2), clrLine);
		}
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_SOILD) )
  	{
  		// ��ʵ��
		if ( IsFixed() )
		{
			if ( nRow == 0 )
			{
				pDC->MoveTo(rect.right + 2, rect.top - 1);
			}
			else
			{
				pDC->MoveTo(rect.right + 2, rect.top - 3);
			}
  			pDC->LineTo(rect.right + 2, rect.bottom + 2);
		}
		else
		{
  			pDC->MoveTo(rect.right + 1, rect.top - 3);
  			pDC->LineTo(rect.right + 1, rect.bottom + 2);
		}
  	}
	
	pDC->SelectObject(pOldpen);	
	PenLine.DeleteObject();

	// ����
  	CPoint ptTopLeft(rect.left + m_iXposParentActive, rect.top + m_iYposParentActive);
	CPoint ptBottomRight(ptTopLeft.x + 2, ptTopLeft.y + 2);	
	
	CRect rectActive(ptTopLeft, ptBottomRight);

	if ( m_bParentActive )
 	{
 		pDC->FillSolidRect(rectActive, m_ClrParentActive);
 	}
	
    pDC->RestoreDC(nSavedDC);
    
	return TRUE;
}

CGridCellNormalSys::CGridCellNormalSys()
{
	m_eDTNS = EDTNS_Text;
}


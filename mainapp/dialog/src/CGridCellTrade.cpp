#include "stdafx.h"
#include "GridCellSys.h"
#include "IoViewManager.h"
#include "GridCtrl.h"
#include "facescheme.h"

#include "MPIChildFrame.h"
#include "CGridCellTrade.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellTrade, CGridCell)

CGridCellTrade::CGridCellTrade()
:CGridCell()
{
	m_crfSelBk = RGB(0x2C,0x2C,0x34);//::GetSysColor(COLOR_HIGHLIGHT);
	m_crfSelText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	
}

CGridCellTrade::~CGridCellTrade()
{
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellTrade::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0 || rect.Height() <= 0)
        return FALSE;   

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
    if (!pDefaultCell)
        return FALSE;

	

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
            TextBkClr = m_crfSelBk;
            TextClr = TextClr;
            bEraseBkgnd = TRUE;
        }

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
			TextBkClr = m_crfSelBk;
		}

		rect.right++; 
		rect.bottom++;    // FillRect doesn't draw RHS or bottom
		pDC->FillSolidRect(rect, TextBkClr);
		rect.right--;
		rect.bottom--;
		pDC->SetTextColor(TextClr/*m_crfSelText*/);
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

	if (IsFixedRow())
	{
		COLORREF colBk = pGrid->GetDefaultCell(TRUE, FALSE)->GetBackClr();
		COLORREF colText = pGrid->GetDefaultCell(TRUE, FALSE)->GetTextClr();
		rect.right++; rect.bottom++; 
		pDC->FillSolidRect(rect, colBk);
		rect.right--;
		rect.bottom--;
        pDC->SetTextColor(colText);
	}
	else if (IsFixedCol())
	{
		COLORREF colBk = pGrid->GetDefaultCell(FALSE, TRUE)->GetBackClr();
		COLORREF colText = pGrid->GetDefaultCell(FALSE, TRUE)->GetTextClr();
		rect.right++; rect.bottom++; 
		pDC->FillSolidRect(rect, colBk);
		rect.right--;
		rect.bottom--;
        pDC->SetTextColor(colText);
	}
	else if (IsFixed())
	{
		COLORREF colBk = pGrid->GetDefaultCell(TRUE, TRUE)->GetBackClr();
		COLORREF colText = pGrid->GetDefaultCell(TRUE, TRUE)->GetTextClr();
		rect.right++; rect.bottom++; 
		pDC->FillSolidRect(rect, colBk);
		rect.right--;
		rect.bottom--;
        pDC->SetTextColor(colText);
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

			BOOL bLeftTriangle = (GetFormat() & DT_RIGHT || GetFormat() & DT_CENTER);		// 非左对齐，都画在左边，否则画在右边 

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
           
			
			DrawArrow(pDC, pGrid->GetSortAscending(), rect);
			
            
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

	HGDIOBJ hOldFont = NULL;
	if (m_bUnderLine)
	{
		LOGFONT *plf = GetFont();
        plf->lfUnderline = TRUE;//具有下划线的文字
		if (m_font.m_hObject == NULL)
		{
			 m_font.CreateFontIndirect(plf);
			 hOldFont = pDC->SelectObject(&m_font);
		}
		else
		{
			hOldFont = pDC->SelectObject(&m_font);
		}
	}

	CPen pen;
	CPen *oldPen;
	pen.CreatePen(PS_SOLID,1,RGB(242,240,235));
	oldPen = pDC->SelectObject(&pen);
	if (m_bVSeparatoLine)
	{
		pDC->MoveTo(rect.right-2,rect.top-2);
		pDC->LineTo(rect.right-2,rect.bottom);
	}
	if (m_bHSeparatoLine)
	{
		pDC->MoveTo(rect.left, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom);
	}
	pDC->SelectObject(oldPen);
	
	rect.left += 5;// 改变文本的起始绘制坐标，目的：防止压倒表格线了
	rect.right -= 5;// 同理
    DrawText(pDC->m_hDC, GetText(), -1, rect, dwFmt);

	if (hOldFont != NULL)
		pDC->SelectObject(hOldFont);

    pDC->RestoreDC(nSavedDC);

    return TRUE;
}


void	CGridCellTrade::DrawArrow(CDC* pDC, BOOL bAscending, const CRect& rect)
{
	if (NULL == pDC)
	{
		return;
	}
	
	CRect RectText;
	RectText.SetRectEmpty();
	GetTextRect(RectText);
	
	CSize size = GetTextExtent(GetText(), pDC);//GetTextExtent(GetText(), pDC);

	CPen  penArrow(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penArrow);
	CPoint ptUp, ptDown, ptLeft, ptRight;

	// 与gridsys不同之处 - 排序画的不同, 如果宽度不够，则实际排序会画在显示区域以外
	if ( GetFormat() & DT_CENTER )
	{
		// 中对齐 - 靠左侧
		ptUp.x = rect.CenterPoint().x - (size.cx / 2) - 2; 
		if ( ptUp.x + 3 > RectText.left )
		{
			RectText.left = ptUp.x + 3;		// 文字区域排除
		}
	}
	else if ( GetFormat() & DT_RIGHT )
	{
		// 右对齐 - 画左边
		ptUp.x = rect.right - size.cx - 3;
		if ( ptUp.x + 3 > RectText.left )
		{
			RectText.left = ptUp.x + 3;		// 文字区域排除
		}
	}
	else 
	{
		// 左对齐 - 画右边
		ptUp.x = rect.left + size.cx + 3;
		if ( ptUp.x - 3 < RectText.right )
		{
			RectText.right = ptUp.x - 3;		// 文字区域排除
		}
	}

	ptUp.y = rect.top + (rect.Height() - 14) /2;
	ptDown.y = rect.bottom - 1;
	if ( ptDown.y - ptUp.y > 14 ) // 限制不能太长, 顶边对齐
	{
		ptDown.y = ptUp.y + 14;
	}

	// 
	if (bAscending)
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
CSize CGridCellTrade::GetTextExtent( LPCTSTR szText, CDC* pDC /*= NULL*/ )
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

		if ( str == szText )	// 没有找到\n 则使用默认宽度来计算，而不是以前的总宽度
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
		if ( size.cx > pGrid->GetDefCellWidth() )		// 不能小于默认值
		{
			size.cx = pGrid->GetDefCellWidth();
		}
	}
    
    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);
	
    return size;
}

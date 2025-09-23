// GridCellCheck.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for a combobox cell type of the
// grid control.
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// Parts of the code contained in this file are based on the original
// CInPlaceList from http://www.codeguru.com/listview
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
// History:
// 23 Jul 2001 - Complete rewrite
// 13 Mar 2004 - GetCellExtent and GetCheckPlacement fixed by Yogurt
//             - Read-only now honoured - Yogurt
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCell.h"
#include "GridCtrl.h"
#include "GridCellCheck.h"
#include "IoViewRelative.h"
#include "MPIChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellCheck, CGridCellSys)

CGridCellCheck::CGridCellCheck() : CGridCellSys()
{
    m_bChecked = FALSE;
    //m_Rect.IsRectNull();
}

CSize CGridCellCheck::GetCellExtent(CDC* pDC)
{
    // Using SM_CXHSCROLL as a guide to the size of the checkbox
    int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();	
    CSize	cellSize = GetCellExtent(pDC);	
    cellSize.cx += nWidth;	
    cellSize.cy = max (cellSize.cy, nWidth);	
    return  cellSize;
}

// i/o:  i=dims of cell rect; o=dims of text rect
BOOL CGridCellCheck::GetTextRect( LPRECT pRect)
{
    //BOOL bResult = GetTextRect(pRect); //CGridCellSys
	BOOL bResult = CGridCellSys::GetTextRect(pRect);
    if (bResult)
    {
        int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
        
		pRect->left += nWidth;
        
		if (pRect->left > pRect->right)
        {
			pRect->left = pRect->right;
		} 
    }
    return bResult;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellCheck::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    BOOL bResult = CGridCellSys::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

#ifndef _WIN32_WCE
    // Store the cell's dimensions for later
    m_Rect = rect;

    CRect CheckRect = GetCheckPlacement();
    rect.left = CheckRect.right;
	
    // enough room to draw?
    // if (CheckRect.Width() < rect.Width() && CheckRect.Height() < rect.Height()) {
	
    // Do the draw 
	pDC->DrawFrameControl(GetCheckPlacement(), DFC_BUTTON, 
		(m_bChecked)? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK);

	CIoViewBase * pParentIoView = (CIoViewBase *)GetGrid()->GetParent();
	CRect rectdraw = rect ;
	rectdraw.left = CheckRect.right;

	if ( NULL != pParentIoView)
	{
		CFont * pOldFont = pDC->SelectObject(pParentIoView->GetIoViewFontObject(ESFNormal));

		if ((GetState() & GVIS_SELECTED))	// 被选中
		{			
			pDC->FillSolidRect(rectdraw, pParentIoView->GetIoViewColor(ESCGridSelected));
		}
		else
		{
			pDC->FillSolidRect(rectdraw, pParentIoView->GetIoViewColor(ESCBackground));
		}
		
		pDC->SetTextColor(pParentIoView->GetIoViewColor(ESCText));
		
		// 文字居中显示:
		CSize size = pDC->GetTextExtent(GetText());
		int32 iFontHeight = size.cy;
		
		CRect RectText = rect;
		RectText.top = (rect.Height() - iFontHeight) / 2  + rect.top; 
		
		pDC->DrawText(GetText(),RectText,GetFormat() | DT_NOPREFIX |DT_CENTER | DT_BOTTOM | DT_VCENTER);			
		pDC->SelectObject(pOldFont);
	}
	else
	{
		CFont * pOldFont = pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFNormal));

		if ((GetState() & GVIS_SELECTED))	// 被选中
		{			
			pDC->FillSolidRect(rectdraw, CFaceScheme::Instance()->GetSysColor(ESCGridSelected));
		}
		else
		{
			pDC->FillSolidRect(rectdraw, CFaceScheme::Instance()->GetSysColor(ESCBackground));
		}

		pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCText));
		
		// 文字居中显示:
		CSize size = pDC->GetTextExtent(GetText());
		int32 iFontHeight = size.cy;
		
		CRect RectText = rect;
		RectText.top = (rect.Height() - iFontHeight) / 2  + rect.top; 
		
		pDC->DrawText(GetText(),rectdraw,GetFormat() | DT_NOPREFIX |DT_CENTER | DT_BOTTOM | DT_VCENTER);		
		pDC->SelectObject(pOldFont);
	}



    // }
#endif
    return bResult;
}

void CGridCellCheck::OnClick(CPoint PointCellRelative)
{
	PointCellRelative += m_Rect.TopLeft();
    CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);	
	
	//if (!GetGrid()->IsCellEditable(cell))		
	//	return;
	
	if (! GetCheckPlacement().PtInRect(PointCellRelative))	
		return;
	
	if (GetGrid()->GetParent()->IsKindOf(RUNTIME_CLASS(CIoViewRelative)))
	{
		m_bChecked = !m_bChecked;
		CGridCellCheck * pCellFirst =(CGridCellCheck *)GetGrid()->GetCell(0,0);
		ASSERT(NULL != pCellFirst);
		// 如果是(0,0) 这个cell [关系]  特殊处理.全选或者全不选所有子row 
		
		if (cell.row==0 && cell.col == 0)
		{	
			if ( pCellFirst->GetCheck())
			{	
				int32 iRow = GetGrid()->GetRowCount();
				for (int32 i=1;i<iRow;i++)
				{
					CGridCellCheck * pCell =(CGridCellCheck *)GetGrid()->GetCell(i,0);
					ASSERT(NULL != pCell);
					pCell->SetCheck(true);
					
					CIoViewRelative * pIoViewRealtive =(CIoViewRelative *)GetGrid()->GetParent();
					ASSERT(NULL != pIoViewRealtive);
					pIoViewRealtive->SendInfomation(true,i,0);		
				}			
			}
			else 
			{
				for (int32 i=1;i<GetGrid()->GetRowCount();i++)
				{
					CGridCellCheck * pCell =(CGridCellCheck *)GetGrid()->GetCell(i,0);
					ASSERT(NULL != pCell);
					pCell->SetCheck(false);
					
					CIoViewRelative * pIoViewRealtive =(CIoViewRelative *)GetGrid()->GetParent();
					ASSERT(NULL != pIoViewRealtive);
					pIoViewRealtive->SendInfomation(false,i,0);		
				}
			}																	
		}
		if (cell.row>=1)
		{		
			CIoViewRelative * pIoViewRealtive =(CIoViewRelative *)GetGrid()->GetParent();							
			ASSERT(NULL != pIoViewRealtive);
			pIoViewRealtive->SendInfomation(m_bChecked,cell.row,cell.col);							
		}
	}
	else
	{
		// 非IoViewRelative 视图,不做特殊处理
		m_bChecked = !m_bChecked;
		GetGrid()->InvalidateRect(m_Rect);
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CGridCellCheck::SetCheck(BOOL bChecked /*=TRUE*/)
{
	BOOL bTemp = m_bChecked;
	m_bChecked = bChecked;
	if (!m_Rect.IsRectEmpty())
		GetGrid()->InvalidateRect(m_Rect);

	return bTemp;
}

BOOL CGridCellCheck::GetCheck()
{
	return m_bChecked;
}

//////////////////////////////////////////////////////////////////////
// Protected implementation
//////////////////////////////////////////////////////////////////////

// Returns the dimensions and placement of the checkbox in client coords.
CRect CGridCellCheck::GetCheckPlacement()
{
	int nWidth = GetSystemMetrics(SM_CXHSCROLL);
	CRect place = m_Rect + CSize(GetMargin(), GetMargin());
    place.right = place.left + nWidth;
    place.bottom = place.top + nWidth;

	/* for centering
	int nDiff = (place.Width() - nWidth)/2;
	if (nDiff > 0)
	{
		place.left += nDiff;
		place.right = place.left + nWidth;
	}
	nDiff = (place.Height() - nWidth)/2;
	if (nDiff > 0)
	{
		place.top += nDiff;
		place.bottom = place.top + nWidth;
	}
    */
   // if (m_Rect.Height() < nWidth + 2 * static_cast<int> (GetMargin())) 
   // {		
        place.top = m_Rect.top + (m_Rect.Height() - nWidth) / 2;	    
        place.bottom = place.top + nWidth;	
   // }
	return place;
}
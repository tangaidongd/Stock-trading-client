// TradeListCtrl.cpp : 实现文件
//

#include "stdafx.h"

#include "TradeListCtrl.h"




// CTradeListCtrl

IMPLEMENT_DYNAMIC(CTradeListCtrl, CListCtrl)

CTradeListCtrl::CTradeListCtrl()
{
	m_nRowHeight = 0;
	m_bAlterBkColor = TRUE;
	m_bTextEllipsis = FALSE;
	m_bDrawGridLines = FALSE;
}

CTradeListCtrl::~CTradeListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTradeListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_MEASUREITEM_REFLECT()
	ON_MESSAGE(LVM_GETHEADER, OnGetHeaderCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_INSERTITEM, OnLvnInsertItem)
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnLvnDeleteItem)
	ON_NOTIFY_REFLECT_EX(LVN_DELETEALLITEMS, OnLvnDeleteAllItem)
END_MESSAGE_MAP()


void CTradeListCtrl::OnDestroy()
{
	//m_wndTradeHeader.Detach();

	CListCtrl::OnDestroy();
}

void CTradeListCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpItem )
{
	if ( m_nRowHeight > 0 )
	{
		lpItem->itemHeight = m_nRowHeight;	
	}
}

int CTradeListCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iRet = CListCtrl::OnCreate(lpCreateStruct);
	HWND hWnd = (HWND) ::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0);
	if ( NULL!=hWnd )
	{
		CWnd *pWnd = CWnd::FromHandlePermanent(hWnd);
		ASSERT( NULL==pWnd );
		m_wndTradeHeader.SubclassWindow(hWnd);
	}
	return iRet;
}

BOOL CTradeListCtrl::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= LVS_OWNERDRAWFIXED;
	return CListCtrl::PreCreateWindow(cs);
}

void CTradeListCtrl::DrawItem( /*_In_*/ LPDRAWITEMSTRUCT lpDraw )
{
	CDC dc;
	dc.Attach(lpDraw->hDC);
	dc.SaveDC();

	BOOL bSelected	= lpDraw->itemState&ODS_SELECTED;	
	const int iRow	= lpDraw->itemID;
	const DWORD_PTR dwItemData = lpDraw->itemData;
	DWORD dwExStyle = GetExtendedStyle();
	BOOL bDrawGrid	= IsDrawGridLines();
	BOOL bSysDrawGrid = (dwExStyle&LVS_EX_GRIDLINES);
	BOOL bGrid		= bSysDrawGrid || bDrawGrid;	// 无论是自己绘制还是系统绘制

	LVITEM lvItem	= {0};
	lvItem.iItem	= iRow;
	lvItem.mask		= LVIF_STATE |LVIF_IMAGE;
	lvItem.stateMask = (UINT)-1;
	GetItem(&lvItem);

	CImageList *pImageList = GetImageList(LVSIL_SMALL);

	BOOL bFocusState = (lvItem.state&LVIS_FOCUSED) && GetFocus() == this;

	COLORREF clrBk		= GetRowDataBkColor(dwItemData, iRow);
	COLORREF clrText	= GetRowDataTextColor(dwItemData, iRow);
	if ( bSelected )
	{
// 		if ( !bFocusState )
// 		{
// 			clrBk	= GetSysColor(COLOR_BTNFACE);
// 			clrText	= GetSysColor(COLOR_BTNTEXT);
// 		}
// 		else
		{
			clrBk	= GetSysColor(COLOR_HIGHLIGHT);
			clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
	}

	CRect rcItem(lpDraw->rcItem);
	if ( bGrid )
	{
		// 网格，作出让步
		rcItem.InflateRect(0, 0, -1, -1);
	}

	
	dc.FillSolidRect(&lpDraw->rcItem, clrBk);
	if ( bFocusState && !bSelected )
	{
		dc.DrawFocusRect(rcItem);
//		rcItem.InflateRect(-1, -1);
	}

	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	const int iColCount = pHeaderCtrl->GetItemCount();
	LVCOLUMN lvCol = {0};
	lvCol.mask = LVCF_SUBITEM |LVCF_WIDTH |LVCF_FMT |LVCF_ORDER;
	CString StrSubItem;
	
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(clrBk);
	dc.SetTextColor(clrText);

	CRect rcSubItem(lpDraw->rcItem);
	rcSubItem.right = rcSubItem.left;

	RowItemDataColorMap::iterator itRowBkClr	= m_mapRowDataBkClr.find(dwItemData);
	RowItemDataColorMap::iterator itRowTextClr	= m_mapRowDataTextClr.find(dwItemData);
	BOOL bHasRowBk		= itRowBkClr!=m_mapRowDataBkClr.end();
	BOOL bHasRowText	= itRowTextClr!=m_mapRowDataTextClr.end();

	CPen	PenGrid;
	PenGrid.CreatePen(PS_SOLID, 0, RGB(192,192,192));
	dc.SelectObject(&PenGrid);

	UnionRowDataMap::iterator itUnionRow = m_mapUnionRow.find(dwItemData);
	BOOL bUnionRow = itUnionRow!=m_mapUnionRow.end() && !itUnionRow->second.empty();

	for ( int i=0; i < iColCount ; ++i )
	{
		int iCol = i;
		if ( !GetColumn(iCol, &lvCol) )
		{
			continue;
		}

		rcSubItem.right = rcSubItem.left + lvCol.cx;
		
		// 合并列判断
		if ( bUnionRow )
		{
			// 是否是开始的列
			UnionColMap::iterator itUnionCol = itUnionRow->second.find(iCol);
			if ( itUnionCol!=itUnionRow->second.end() && itUnionCol->second > 1 )
			{
				// 这个列横跨了几个数量的列，剩下的列(被合并的)直接跨过去，将不绘制被合并的列
				LVCOLUMN lvColHide = lvCol;
				++i;
				for (; i < iCol+itUnionCol->second ; )
				{
					if ( GetColumn(i, &lvColHide) )
					{
						rcSubItem.right += lvColHide.cx;
					}
					++i ;
				}
				--i;	// 第二次循环要求回退
			}
		}
		// 后面将不要引用i，只能使用icol代表现在使用的列

		if ( rcSubItem.left < rcItem.right )
		{
			CRect rcText(rcSubItem);

			// 即使绘制网格，这里也做出了让步了
			rcText.InflateRect(-2, 0);

			// 图片
			if ( NULL!=pImageList 
				&& 0==iCol 
				&& lvItem.iImage >=0 && lvItem.iImage < pImageList->GetImageCount() )
			{
				// 第一列的图片
				IMAGEINFO imageInfo = {0};
				if ( pImageList->GetImageInfo( lvItem.iImage, &imageInfo ) 
					&& imageInfo.rcImage.right-imageInfo.rcImage.left < rcText.Width() )
				{
					CRect rcImage(rcText);
					rcImage.right = rcImage.left + imageInfo.rcImage.right - imageInfo.rcImage.left;
					pImageList->Draw(&dc, lvItem.iImage, rcImage.TopLeft(), ILD_NORMAL);
					rcText.left = rcImage.right + 1;
				}
			}

			if ( rcText.left < rcText.right )
			{
				const int iColumn = lvCol.iOrder;
				StrSubItem = GetItemText(iRow, iColumn);
				UINT nFmt = DT_SINGLELINE |DT_VCENTER;
				if ( lvCol.fmt&LVCFMT_RIGHT )
				{
					nFmt |= DT_RIGHT;
				}
				else if ( lvCol.fmt&LVCFMT_CENTER )
				{
					nFmt |= DT_CENTER;
				}
				else
				{
					nFmt |= DT_LEFT;
				}
				if ( m_bTextEllipsis )
				{
					nFmt |= DT_WORD_ELLIPSIS;
				}
				if ( !m_bTextEllipsis )
				{
					CSize sizeText = dc.GetTextExtent(StrSubItem);
					if ( sizeText.cx > rcText.Width() )
					{
						nFmt &= ~(DT_RIGHT|DT_CENTER);	// 不够强制左对齐
					}
				}
				
				BOOL bHasColColor = FALSE;
				if ( bHasRowBk && !bSelected )
				{
					ColumnColorMap::iterator itCol = itRowBkClr->second.m_mapColClr.find(iColumn);
					if ( itCol!=itRowBkClr->second.m_mapColClr.end() && itCol->second != CLR_DEFAULT )
					{
						dc.FillSolidRect(rcSubItem, itCol->second);
						bHasColColor = TRUE;
					}
				}
				
				if ( bHasRowText && !bSelected )
				{
					ColumnColorMap::iterator itCol = itRowTextClr->second.m_mapColClr.find(iColumn);
					if ( itCol!=itRowTextClr->second.m_mapColClr.end() && itCol->second != CLR_DEFAULT )
					{
						dc.SetTextColor(itCol->second);
						bHasColColor = TRUE;
					}
				}
				
				dc.DrawText(StrSubItem, rcText, nFmt);
				
				if ( bHasColColor )
				{
					dc.SetBkColor(clrBk);
					dc.SetTextColor(clrText);
				}
			}

			if ( bDrawGrid )
			{
				// 网格线绘制
				// 网格线仅绘制预留的右侧&底侧部分，其它部分不需要绘制
				dc.MoveTo(rcSubItem.left, rcSubItem.bottom-1);
				dc.LineTo(rcSubItem.right-1, rcSubItem.bottom-1);
				dc.LineTo(rcSubItem.right-1, rcSubItem.top-1);
			}
		}
		rcSubItem.left = rcSubItem.right;
	}

	dc.RestoreDC(-1);
	dc.Detach();
}

BOOL CTradeListCtrl::OnLvnInsertItem( NMHDR *pHdr, LRESULT *pResult )
{	
	return FALSE;
}

BOOL CTradeListCtrl::OnLvnDeleteItem( NMHDR *pHdr, LRESULT *pResult )
{
	NMLISTVIEW *pHdrList = (NMLISTVIEW *)pHdr;
	if ( NULL!=pHdrList )
	{
		//TRACE(_T("delete %d,%d from %d\r\n"), pHdrList->iItem, pHdrList->iSubItem, pHdr->idFrom);
		
	}
	return FALSE;
}

BOOL CTradeListCtrl::OnLvnDeleteAllItem( NMHDR *pHdr, LRESULT *pResult )
{
	NMLISTVIEW *pHdrList = (NMLISTVIEW *)pHdr;
	if ( NULL!=pHdrList )
	{
		m_mapRowDataBkClr.clear();
		m_mapRowDataTextClr.clear();
		m_mapUnionRow.clear();
	}
	return FALSE;
}
// 
// void CTradeListCtrl::SetSingleRowBkColor( int iRow, COLORREF clrBk )
// {
// 	m_mapClrRowBk[iRow] = clrBk;
// }
// 
// void CTradeListCtrl::SetSingleRowTextColor( int iRow, COLORREF clrText )
// {
// 	m_mapClrRowText[iRow] = clrText;
// }
// 
// COLORREF CTradeListCtrl::GetSingleRowBkColor(int iRow) const
// {
// 	COLORREF clr = CLR_DEFAULT;
// 	if ( m_mapClrRowBk.Lookup(iRow, clr) )
// 	{
// 	}
// 	if ( clr == CLR_DEFAULT )
// 	{
// 		clr = GetBkColor();
// 		if ( IsAlterBkColor() )
// 		{
// 			if ( iRow%2 )
// 			{
// 				clr = GetGradiantColor(clr);
// 			}
// 		}
// 	}
// 	return clr;
// }
// 
// COLORREF CTradeListCtrl::GetSingleRowTextColor(int iRow) const
// {
// 	COLORREF clr = CLR_DEFAULT;
// 	if ( m_mapClrRowText.Lookup(iRow, clr) )
// 	{
// 	}
// 	if ( clr == CLR_DEFAULT )
// 	{
// 		return GetTextColor();
// 	}
// 	return clr;
// }

BOOL CTradeListCtrl::UnionColumn( DWORD dwRowData, int iStartColumn, int iUnionCount )
{
	if ( iUnionCount <= 1 )
	{
		// 取消合并操作
		return UnUnionColumn(dwRowData, iStartColumn);
	}

	UnionRowDataMap::iterator itRow = m_mapUnionRow.find(dwRowData);
	if ( itRow != m_mapUnionRow.end() )
	{
		UnionColMap::iterator itCol = itRow->second.find(iStartColumn);
		if ( itCol != itRow->second.end() )
		{
			if ( itCol->second >= iUnionCount )
			{
				// <= 原来的值，更新该值返回
				itCol->second = iUnionCount;
				return TRUE;
			}

			// >原来的值，判断是否与其它合并列冲突
			// TODO: 判断
			itCol->second = iUnionCount;
			return TRUE;
		}

		// 以前没有这个列，判断是否与其它和并列冲突
		// TODO: 判断
		itRow->second[iStartColumn] = iUnionCount;
		return TRUE;
	}

	m_mapUnionRow[dwRowData][iStartColumn] = iUnionCount;
	return TRUE;
}

BOOL CTradeListCtrl::UnUnionColumn( DWORD dwRowData, int iStartColumn )
{
	UnionRowDataMap::iterator itRow = m_mapUnionRow.find(dwRowData);
	if ( itRow != m_mapUnionRow.end() )
	{
		UnionColMap::iterator itCol = itRow->second.find(iStartColumn);
		if ( itCol != itRow->second.end() )
		{
			itRow->second.erase(iStartColumn);
			return TRUE;
		}

		// 如果位于某部分合并列中间，则需要打断合并列
		// TODO: 实现
		ASSERT( 0 );	// 未实现部分
	}
	return FALSE;
}

BOOL CTradeListCtrl::UnUnionColumn( DWORD dwRowData )
{
	m_mapUnionRow.erase(dwRowData);
	return TRUE;
}

LRESULT CTradeListCtrl::OnGetHeaderCtrl( WPARAM w, LPARAM l )
{
	LRESULT lRes = DefWindowProc(LVM_GETHEADER, w, l);
	HWND hWnd = (HWND)lRes;
	CWnd *pWnd = CWnd::FromHandlePermanent(hWnd);
	ASSERT( NULL==pWnd || pWnd->IsKindOf(RUNTIME_CLASS(CTradeHeaderCtrl)) );
	ASSERT( NULL==m_wndTradeHeader.m_hWnd || hWnd == m_wndTradeHeader.m_hWnd );
	if ( NULL==m_wndTradeHeader.m_hWnd )
	{
		m_wndTradeHeader.SubclassWindow(hWnd);
	}
	return lRes;
}

void CTradeListCtrl::OnPaint()
{
	Default();
}

void CTradeListCtrl::SetRowDataBkColor( DWORD_PTR dwItemData, COLORREF clrBk )
{
	m_mapRowDataBkClr[dwItemData].m_clrRow = clrBk;
}

void CTradeListCtrl::SetRowDataTextColor( DWORD_PTR dwItemData, COLORREF clrText )
{
	m_mapRowDataTextClr[dwItemData].m_clrRow = clrText;
}

COLORREF CTradeListCtrl::GetRowDataBkColor( DWORD_PTR dwItemData, int iRowNow ) const
{
	COLORREF clr = CLR_DEFAULT;
	RowItemDataColorMap::const_iterator it = m_mapRowDataBkClr.find(dwItemData);
	if ( it != m_mapRowDataBkClr.end() )
	{
		clr = it->second.m_clrRow;
	}
	if ( clr == CLR_DEFAULT )
	{
		clr = GetBkColor();
		if ( IsAlterBkColor() )
		{
			if ( iRowNow%2 )
			{
				clr = GetGradiantColor(clr);
			}
		}
	}
	return clr;
}

COLORREF CTradeListCtrl::GetRowDataTextColor( DWORD_PTR dwItemData, int /*iRowNow*/ ) const
{
	COLORREF clr = CLR_DEFAULT;
	RowItemDataColorMap::const_iterator it = m_mapRowDataTextClr.find(dwItemData);
	if ( it != m_mapRowDataTextClr.end() )
	{
		clr = it->second.m_clrRow;
	}
	if ( clr == CLR_DEFAULT )
	{
		clr = GetTextColor();
	}
	return clr;
}

void CTradeListCtrl::SetRowDataColBkColor( DWORD_PTR dwItemData, int iCol, COLORREF clrBk )
{
	m_mapRowDataBkClr[dwItemData].m_mapColClr[iCol] = clrBk;
}

void CTradeListCtrl::SetRowDataColTextColor( DWORD_PTR dwItemData, int iCol, COLORREF clrText )
{
	m_mapRowDataTextClr[dwItemData].m_mapColClr[iCol] = clrText;
}

COLORREF CTradeListCtrl::GetRowDataColBkColor( DWORD_PTR dwItemData, int iCol, int iRowNow ) const
{
	COLORREF clr = CLR_DEFAULT;
	RowItemDataColorMap::const_iterator it = m_mapRowDataBkClr.find(dwItemData);
	if ( it != m_mapRowDataBkClr.end() )
	{
		ColumnColorMap::const_iterator itCol = it->second.m_mapColClr.find(iCol);
		if ( itCol != it->second.m_mapColClr.end() )
		{
			clr = itCol->second;
		}
	}
	if ( clr == CLR_DEFAULT )
	{
		clr = GetRowDataBkColor(dwItemData, iRowNow);
	}
	return clr;
}

COLORREF CTradeListCtrl::GetRowDataColTextColor( DWORD_PTR dwItemData, int iCol, int iRowNow ) const
{
	COLORREF clr = CLR_DEFAULT;
	RowItemDataColorMap::const_iterator it = m_mapRowDataTextClr.find(dwItemData);
	if ( it != m_mapRowDataTextClr.end() )
	{
		ColumnColorMap::const_iterator itCol = it->second.m_mapColClr.find(iCol);
		if ( itCol != it->second.m_mapColClr.end() )
		{
			clr = itCol->second;
		}
	}
	if ( clr == CLR_DEFAULT )
	{
		clr = GetRowDataTextColor(dwItemData, iRowNow);
	}
	return clr;
}

void CTradeListCtrl::RemoveRowDataBkColor( DWORD_PTR dwItemData )
{
	m_mapRowDataBkClr.erase(dwItemData);
}

void CTradeListCtrl::RemoveRowDataTextColor( DWORD_PTR dwItemData )
{
	m_mapRowDataTextClr.erase(dwItemData);
}

COLORREF CTradeListCtrl::GetGradiantColor( COLORREF clr ) const
{
	
	int r = GetRValue(clr);
	int g = GetGValue(clr);
	int b = GetBValue(clr);
	const int iScale = 35;
	int r2 = r - iScale;
	if ( r2 < 0 )
	{
		r2 = r + iScale;
	}
	int g2 = g - iScale;
	if ( g2 < 0 )
	{
		g2 = g + iScale;
	}
	int b2 = b - iScale;
	if ( b2 < 0 )
	{
		b2 = b + iScale;
	}
	clr = RGB(r2, g2, b2);

	return clr;
}





// CTradeListCtrl 消息处理程序


//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNAMIC(CTradeHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CTradeHeaderCtrl, CHeaderCtrl)
	ON_MESSAGE(HDM_LAYOUT, OnHdmLayout)
END_MESSAGE_MAP()

CTradeHeaderCtrl::CTradeHeaderCtrl()
{

}

CTradeHeaderCtrl::~CTradeHeaderCtrl()
{

}

LRESULT CTradeHeaderCtrl::OnHdmLayout( WPARAM w, LPARAM l )
{
	BOOL lResult = (BOOL)DefWindowProc(HDM_LAYOUT, w, l);	
	return lResult;
}

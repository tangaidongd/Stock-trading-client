#ifndef _GRID_CELL_TRADE_H_
#define _GRID_CELL_TRADE_H_


#include "GridCell.h"

class CGridCellTrade : public CGridCell
{
    DECLARE_DYNCREATE(CGridCellTrade)
	
public:
    CGridCellTrade();    
	 virtual ~CGridCellTrade();
	
	// from CGrideCell
public:
    virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	void			DrawArrow(CDC* pDC, BOOL bAscending, const CRect& rect);
	virtual CSize	GetTextExtent(LPCTSTR szText, CDC* pDC = NULL);	
// 	void SetUnderLine(BOOL bUnderLine);

private:
	COLORREF	m_crfSelBk;		// 选中项的背景色
	COLORREF	m_crfSelText;	// 选中项文本的颜色
	
};

#endif //_GRID_CELL_TRADE_H_

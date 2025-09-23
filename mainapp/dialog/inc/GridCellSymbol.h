#ifndef _GRID_CELL_SYMBOL_H_
#define _GRID_CELL_SYMBOL_H_


#include "GridCellSys.h"

class CGridCellSymbol : public CGridCellSys
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellSymbol)

public:
	enum E_ShowSymbol
	{
		ESSNone			= 0x00,		// 不显示符号
		ESSRise			= 0x01,		// 显示价格上升符号 '+'
		ESSKeep			= 0x02,		// 显示价格持平符号 ' '
		ESSFall			= 0x04,		// 显示价格下跌符号 '-'
	};

public:
    CGridCellSymbol();             

	// from CGrideCell
public:
    virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

public:
	void			SetShowSymbol(int32 iShowSymbol) { m_iShowSymbol = iShowSymbol; }

private:
	void			SetTextOnly(LPCTSTR szText)   { m_strText = szText; }    
    
protected:
	int32			m_iShowSymbol;	// E_ShowSymbol 或值
};

#endif //_GRID_CELL_SYMBOL_H_




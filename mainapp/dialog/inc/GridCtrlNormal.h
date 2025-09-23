#ifndef _GRID_CTRL_NORMAL_H_
#define _GRID_CTRL_NORMAL_H_


#include "GridCell.h"
#include "GridCtrl.h"
#include "GridCtrlSys.h"
#include "GridCellSymbol.h"
#include "CGridCellTrade.h"

// 画单元格的样式

class CGridCellNormal : public CGridCell
{
    DECLARE_DYNCREATE(CGridCellNormal)

public:
    CGridCellNormal();
    virtual ~CGridCellNormal();

// Attributes
public:
    //void operator=(const CGridCellNormal& cell);

	virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	
	virtual CSize	GetTextExtent(LPCTSTR szText, CDC* pDC = NULL);			// 

public:

protected:
	
protected:
};


// 画单元格的样式

class CGridCellNormalSys : public CGridCellSymbol
{
    DECLARE_DYNCREATE(CGridCellNormalSys)
		
public:
	enum E_DrawTextNormalStyle
	{
		EDTNS_Text = 0,		// 绘制实际的文字
		EDTNS_HorzPercentStick,	// 将文字转换为比例值(最大为1)，绘制水平方向的比例图
	};

	CGridCellNormalSys();

	// Attributes
public:
    //void operator=(const CGridCellNormal& cell);
	
	virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

	BOOL			DoDraw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE); // 实际上代码几乎与gridsys一样

	void			SetAutoHideWhenTooSmallForText(bool32 bHide=TRUE);

	void			SetDrawTextNormalStyle(E_DrawTextNormalStyle eStyle) { m_eDTNS = eStyle; }
	E_DrawTextNormalStyle			GetDrawTextNormalStyle() const { return m_eDTNS; }
	
public:
protected:
	
	E_DrawTextNormalStyle	m_eDTNS;
protected:
};

//////////////////////////////////////////////////////////////////////////

class CGridCtrlNormal : public CGridCtrl
{
	DECLARE_DYNCREATE(CGridCtrlNormal)

public:
	CGridCtrlNormal(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);
	~CGridCtrlNormal();
	
public:
	
	virtual	CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!

	BOOL			SetDefaultCellType( CRuntimeClass* pRuntimeClass);		// 设置默认的单元格类型
	CGridCellBase  *GetDefaultVirtualCell() { return m_pDefaultVirtualModeCell; }

	CCellRange		GetVisibleNonFixedCellRange(LPRECT pRect  = NULL , BOOL bForceRecalculation = FALSE);

	BOOL			DeleteNonFixedRows();
	void			EnsureVisible(int nRow, int nCol);
	void			EnsureVisible(const CCellID &cellId);
	
	bool32			EnsureTopLeftCell(const CCellID &cell);
	bool32			EnsureTopLeftCell(int nRow, int nCol);
	
	bool32			EnsureBottomRightCell(int nRow, int nCol);
	bool32			EnsureBottomRightCell(const CCellID &cellId);

	void			ResetVirtualOrder();		// 虚模式下重置行排序

	////////////////////////////
	// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrlNormal)
protected:
    //}}AFX_VIRTUAL
	

private:
	
	CGridCellBase	*m_pDefaultVirtualModeCell;
	
protected:
	// Generated message map functions
    //{{AFX_MSG(CGridCtrlNormal)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// 做成template的？
class CGridCtrlNormalSys : public CGridCtrlSys
{
	DECLARE_DYNCREATE(CGridCtrlNormalSys)
		
public:
	CGridCtrlNormalSys(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);
	~CGridCtrlNormalSys();
	
public:
	
	virtual	CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!
	
	BOOL			SetDefaultCellType( CRuntimeClass* pRuntimeClass);		// 设置默认的单元格类型
	CGridCellBase  *GetDefaultVirtualCell() { return m_pDefaultVirtualModeCell; }


	
	virtual void	ExpandColumnsToFit(BOOL bExpandFixed = TRUE);		// 这两个还原为GridCtrl原来的
	virtual void	ExpandRowsToFit(BOOL bExpandFixed = TRUE);

	CCellRange		GetVisibleNonFixedCellRange(LPRECT pRect  = NULL, BOOL bForceRecalculation  = FALSE, BOOL bContainFragment =TRUE );

	BOOL			DeleteNonFixedRows();
	void			EnsureVisible(int nRow, int nCol);
	void			EnsureVisible(const CCellID &cellId);

	bool32			EnsureTopLeftCell(const CCellID &cell);
	bool32			EnsureTopLeftCell(int nRow, int nCol);

	bool32			EnsureBottomRightCell(int nRow, int nCol);
	bool32			EnsureBottomRightCell(const CCellID &cellId);

	void			ResetVirtualOrder();		// 虚模式下重置行排序
	
	////////////////////////////
	// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrlNormal)
protected:
    //}}AFX_VIRTUAL
	
	
private:
	
	CGridCellBase	*m_pDefaultVirtualModeCell;
	
protected:
	// Generated message map functions
    //{{AFX_MSG(CGridCtrlNormal)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#endif //_GRID_CTRL_NORMAL_H_




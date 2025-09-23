#ifndef _GRID_CTRL_SYS_H_
#define _GRID_CTRL_SYS_H_


#include "facescheme.h"

#include "GridCtrl.h"


class CGridCtrlSys : public CGridCtrl
{
	DECLARE_DYNCREATE(CGridCtrlSys)
	
public:
	CGridCtrlSys(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);
	~CGridCtrlSys();

	
public:
	virtual COLORREF	GetGridBkColor() const;
	virtual COLORREF	GetGridLineColor() const;
	
	virtual void	ExpandColumnsToFit(BOOL bExpandFixed = TRUE);
	virtual void	ExpandRowsToFit(BOOL bExpandFixed = TRUE);

////////////////////////////
	// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrlSys)
    protected:
    //}}AFX_VIRTUAL


	// Generated message map functions
    //{{AFX_MSG(CGridCtrlSys)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif


#ifndef _GRID_CELL_SYS_H_
#define _GRID_CELL_SYS_H_


#include "GridCellBase.h"
#include "facescheme.h"

// 画单元格的样式
#define DR_NONE				0x00000000
#define DR_LEFT_DOT			0x00000001
#define DR_LEFT_SOILD		0x00000002
#define DR_RIGHT_DOT        0x00000004
#define DR_RIGHT_SOILD      0x00000008
#define DR_TOP_DOT          0x00000010
#define DR_TOP_SOILD        0x00000020
#define DR_BOTTOM_DOT       0x00000040
#define DR_BOTTOM_SOILD     0x00000080

class CGridCellSys : public CGridCellBase
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellSys)

public:
    CGridCellSys();
    virtual ~CGridCellSys();

// Attributes
public:
    void operator=(const CGridCellSys& cell);

    virtual void	SetText(LPCTSTR szText)        { m_strText = szText; }
	virtual void    SetBlinkStep(int32 iBlinkStep) { m_iBlinkStep = iBlinkStep; }
	virtual void	SetBlinkText(LPCTSTR szText, bool32 bBlinkOnlyChange = true);		// zhangbo 0520 #修改默认值为true， 仅改变时闪烁
    virtual void	SetImage(int nImage)           { m_nImage = nImage;   }                        
    virtual void	SetData(LPARAM lParam)         { m_lParam = lParam;   }      
    virtual void	SetGrid(CGridCtrl* pGrid)      { m_pGrid = pGrid;     }                          
	//virtual void	SetState(const DWORD nState);  -  use base class version 
	
    virtual void	SetFormat(DWORD nFormat)       { m_nFormat = nFormat; }                      
    virtual void	SetTextClr(COLORREF clr)       { m_crFgClr = clr;     }                          
    virtual void	SetBackClr(COLORREF clr)       { m_crBkClr = clr;     }                          
    virtual void	SetFont(const LOGFONT* plf);
    virtual void	SetMargin(UINT nMargin)        { m_nMargin = nMargin; }
    virtual CWnd*	GetEditWnd() const             { return m_pEditWnd;   }
    virtual void	SetCoords(int /*nRow*/, int /*nCol*/) {}  // don't need to know the row and
                                                            // column for base implementation
    virtual LPCTSTR	GetText() const             { return (m_strText.IsEmpty())? _T("") : LPCTSTR(m_strText); }
	virtual int32   GetBlinkStep() const        { return m_iBlinkStep; }
    virtual int		GetImage() const            { return m_nImage;  }
    virtual LPARAM	GetData() const             { return m_lParam;  }
    virtual CGridCtrl*	GetGrid() const             { return m_pGrid;   }
    // virtual DWORD    GetState() const - use base class
    virtual DWORD	GetFormat() const;
    virtual COLORREF	GetTextClr() const          { return m_crFgClr; } // TODO: change to use default cell
    virtual COLORREF    GetBackClr() const          { return m_crBkClr; }
    virtual LOGFONT*    GetFont() const;
    virtual CFont*	GetFontObject() const;
    virtual UINT	GetMargin() const;
	virtual LPCTSTR GetTipText() const;
	virtual LPCTSTR GetTipTitle() const;

    virtual BOOL	IsEditing() const           { return m_bEditing; }
    virtual BOOL	IsDefaultFont() const       { return (m_plfFont == NULL); }
    virtual void	Reset();

	void			DrawBottomLine(bool32 bDraw, COLORREF clr = -1);			// fangz0605 Cell 底部画线
	void			DrawLeftLine  (bool32 bDraw, COLORREF clr = -1);			// fangz0605 Cell 左边画线
	void			DrawRightLine (bool32 bDraw, COLORREF clr = -1);			// fangz0605 Cell 左边画线
	void			SetRectLineDot(bool32 bDotLine);							// 设置边框画线风格

	UINT			GetCellRectDrawFlag();
	void			SetCellRectDrawFlag(UINT uiFlag);
// editing cells
public:
	void			SetDefaultTextColor(E_SysColor eTextColor);
	void			SetDefaultTextBkColor(E_SysColor eTextBkColor);
	void			SetDefaultFont(E_SysFont eFont);

	virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	void			DrawVisbleFirstRowPolygonConner(CDC* pDC, CRect rect, int nRow, int nCol);
	virtual BOOL	GetTextRect( LPRECT pRect);
	virtual CSize	GetTextExtent(LPCTSTR szText, CDC* pDC = NULL);
	
    virtual BOOL	Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void	EndEdit();

public:
	void			SetParentActiveFlag(bool32 bActive)	{ m_bParentActive = bActive; }			
	void			SetDrawParentActiveParams(int32 iXpos, int32 iYpos, COLORREF ClrActive);

protected:
    virtual void	OnEndEdit();

protected:
    CString			m_strText;      // Cell text (or binary data if you wish...)
    LPARAM			m_lParam;       // 32-bit value to associate with item
    int				m_nImage;       // Index of the list view item抯 icon
    DWORD			m_nFormat;
    COLORREF		m_crFgClr;
    COLORREF		m_crBkClr;
    LOGFONT*		m_plfFont;
    UINT			m_nMargin;

    BOOL			m_bEditing;     // Cell being edited?

    CGridCtrl*		m_pGrid;        // Parent grid control
    CWnd*			m_pEditWnd;
	
	bool32			m_bDrawBottomLine;
	bool32			m_bDrawLeftLine;
	bool32			m_bDrawRightLine;
	bool32			m_bRectCellLineDot;				// 边框是否虚线
	
	COLORREF		m_ClrRectCellLineClr;
	UINT			m_uiCellRectFlag;				// 画边框的样式

	E_SysColor	m_eDefaultTextColor;	// 默认颜色
	E_SysColor	m_eDefaultTextBkColor;
	E_SysFont	m_eDefaultFont;
	
	bool32			m_bParentActive;
	int32			m_iXposParentActive;
	int32			m_iYposParentActive;
	COLORREF		m_ClrParentActive;
};

#endif




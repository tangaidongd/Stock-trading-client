#if !defined(AFX_LISTEX_H__2A8BEAB9_3896_4E88_9B99_16CFF927176F__INCLUDED_)
#define AFX_LISTEX_H__2A8BEAB9_3896_4E88_9B99_16CFF927176F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListEx.h : header file
//
#define LISTCTRL_CLASSNAME    _T("MFCListCtrl")  // Window class name
/////////////////////////////////////////////////////////////////////////////
// CListEx window

#define WM_LISTITME_CLICK 3400
#define WM_LISTIME_FOCUS  3401

#include <map>
#include <vector>
using namespace std;
#include "typedef.h"
#include "dllexport.h"
#include "XScrollBar.h"

#define GVL_NONE                0L      // Neither
#define GVL_HORZ                1L      // Horizontal line or scrollbar
#define GVL_VERT                2L      // Vertical line or scrollbar
#define GVL_BOTH                3L      // Both

struct CONTROL_EXPORT ListCellID
{
	// Attributes
public:
	int row, col;
	
	// Operations
public:
	explicit ListCellID(int nRow = -1, int nCol = -1) : row(nRow), col(nCol) {}
	
	int IsValid() const { return (row >= 0 && col >= 0); }
	int operator==(const ListCellID& rhs) const { return (row == rhs.row && col == rhs.col); }
	int operator!=(const ListCellID& rhs) const { return !operator==(rhs); }
};

class CONTROL_EXPORT CListEx : public CWnd
{
public:
	enum ItemStatus{Normal, MouseOver, Press};

	struct ListCol
	{
		ItemStatus	m_eColStatus;
		CString		m_strColVal;
		CString		m_strTmp;
		LPARAM		m_lParam;  
	};

	struct ListItem 
	{
		ItemStatus	m_eColStatus;
		//lint -sem(CTypedPtrArray::Add, custodial(1))
		//lint -sem(CTypedPtrArray::InsertAt, custodial(2))
		CTypedPtrArray<CPtrArray, ListCol*> m_ColData;
	};

public:
    CListEx(int nRows = 0, int nCols = 3);

public:
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD /*| WS_BORDER */| WS_TABSTOP | WS_VISIBLE);
	void SetItemValue(int iRowIndex, int iColIndex, CString strVal,  LPARAM	m_lParam=0);
	int  TButtonHitTest(CPoint point);

	// 相邻两行的颜色是否相同
	void IsCross(bool bflag);

	// 是否整行选中
	void IsAllRowSel(bool bflag);

	// 高亮行是否显示特别背景
	void IsHotRowBK(bool bflag);

	// 高亮行文本颜色是否改变
	void IsHotRowColor(bool bflag);
	void SetListBkColor(COLORREF crfBk, COLORREF crfRowNor1, COLORREF crfRowNor2, COLORREF crfRowHot);
	void SetListTextColor(COLORREF crfText, COLORREF crfHotText);

	void SetScrollBar(CXScrollBar *pXSBHorz, CXScrollBar *pXSBVert);
	int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

	// 设置列的宽度
	BOOL SetColumnWidth(int nCol, int width);
	BOOL SetRowHeight(int height);
	int  GetRowHeight(int nRow) const;
	int  GetColumnWidth(int nCol) const;
	long GetVirtualWidth() const;
    long GetVirtualHeight() const;
	int  GetRowCount() const                    { return m_iRows; }
    int  GetColumnCount() const                 { return m_iCols; }

	// 获得指定行列的区域
	void GetCellRect(CRect &rct, int iRow, int iCol);
	ListCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
	ListCellID GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE, BOOL bContainFragment = TRUE);

	// 插入一行
	int  InsertRow(int nRow = 0);
	int  InsertColumn(int nColumn = -1);

	// 删除指定行
	BOOL DeleteRow(int nRow);
	BOOL DeleteAllItems();
	BOOL DeleteColumn(int nColumn);

	// 设置指定列的字体
	void SetColsFont(int nColumn, const LOGFONT* plf);

	// 设置指定列能够响应鼠标消息
	void SetColsActive(int nColumn);

	int GetControlID();
	CString GetItemText(int nRow, int nCol) const;
    LPARAM	GetData(int nRow, int nCol) const;
	int32	GetCurrentRow();
	bool32  IsRowVisible(int nRow);
	void	IsSetFocus(bool32 bFocus);

protected:
    BOOL RegisterWindowClass();

	void ResetScrollBars();
    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
	BOOL IsVisibleVScroll() { return ( (m_nBarState & GVL_VERT) > 0); } 
    BOOL IsVisibleHScroll() { return ( (m_nBarState & GVL_HORZ) > 0); }

public:

private:
	int			m_iRows;		// 列表的行数
	int			m_iCols;		// 列数
	int			m_iRowHeight;	// 行的高度
	int			m_iActiveCol;
	int			m_iID;			// 控件ID

	COLORREF	m_crfBk;		// 背景颜色
	COLORREF	m_crfRowNor1;	// 正常状态行的背景颜色1 (相邻两行的颜色不同)
	COLORREF	m_crfRowNor2;	// 正常状态行的背景颜色2
	COLORREF	m_crfRowHot;	// 高亮行的背景颜色

	COLORREF	m_crfText;		// 文本的颜色
	COLORREF	m_crfHotText;	// 高亮文本的颜色

	BOOL		m_bCross;		// 相邻两行的颜色是否相同
	BOOL		m_bAllRowSel;	// 是否整行选中
	BOOL		m_bHotRowBk;	// 高亮行是否显示特别背景
	BOOL		m_bHotRowColor;	// 高亮行文本颜色是否改变

	int			m_iXButtonHovering;

	CUIntArray  m_arRowHeights;
	CUIntArray  m_arColWidths;
	CArray<CFont, CFont> m_arColFonts;

	//lint -sem(CTypedPtrArray::InsertAt, custodial(2))
	//lint -sem(CTypedPtrArray::Add, custodial(1))
	CTypedPtrArray<CPtrArray, ListItem*> m_rowData;

private:
	CXScrollBar		*m_pXSBHorz;
	CXScrollBar		*m_pXSBVert;
	
	BOOL            m_bOriginalScroll;
	int				m_nBarState;
	int				m_nVScrollMax, m_nHScrollMax;
	DWORD			m_dwDonotScrollToNonExistCell;	 // 禁止滚动到不存在的表格处, 禁止方向可以设置，无则不禁止
	ListCellID		m_idCurrentCell, m_idTopLeftCell;
	int				m_nRowsPerWheelNotch;

	CFont			m_font;  // 字体

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTEX_H__2A8BEAB9_3896_4E88_9B99_16CFF927176F__INCLUDED_)

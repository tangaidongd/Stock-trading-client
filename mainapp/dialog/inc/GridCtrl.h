/////////////////////////////////////////////////////////////////////////////
// GridCtrl.h : header file
//
// MFC Grid Control - main header
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
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
// For use with CGridCtrl v2.20+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable: 4786)

#include "CellRange.h"
#include "GridCell.h"
#include <afxtempl.h>
#include <vector>
#include "GridCellBase.h"
#include "ShareFun.h"

#include "XTipWnd.h"

class CXScrollBar;


///////////////////////////////////////////////////////////////////////////////////
// Defines - these determine the features (and the final size) of the final code
///////////////////////////////////////////////////////////////////////////////////

//#define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
//#define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
//#define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines

#ifdef _WIN32_WCE
#   define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
#   define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
#   define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines
#   define GRIDCONTROL_NO_PRINTING    // Do not use printing routines
#   ifdef WCE_NO_PRINTING			  // Older versions of CE had different #def's
#       define _WIN32_WCE_NO_PRINTING
#   endif
#   ifdef WCE_NO_CURSOR
#       define _WIN32_WCE_NO_CURSOR
#   endif
#endif  // _WIN32_WCE

// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define GRIDCTRL_CLASSNAME    _T("MFCGridCtrl")  // Window class name
#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls


///////////////////////////////////////////////////////////////////////////////////
// Conditional includes
///////////////////////////////////////////////////////////////////////////////////

#ifndef GRIDCONTROL_NO_TITLETIPS
#   include "TitleTip.h"
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
#   include "GridDropTarget.h"
#   undef GRIDCONTROL_NO_CLIPBOARD     // Force clipboard functions on
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
#   include <afxole.h>
#endif


///////////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////////

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

// Backwards compatibility for pre 2.20 grid versions
#define DDX_GridControl(pDX, nIDC, rControl)  DDX_Control(pDX, nIDC, rControl)     


///////////////////////////////////////////////////////////////////////////////////
// Structures
///////////////////////////////////////////////////////////////////////////////////

// This structure sent to Grid's parent in a WM_NOTIFY message
typedef struct tagNM_GRIDVIEW {
    NMHDR hdr;
    int   iRow;
    int   iColumn;
	int	  iParam1;
	int	  iParam2;
} NM_GRIDVIEW;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_DISPINFO {
    NMHDR   hdr;
    GV_ITEM item;
} GV_DISPINFO;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_CACHEHINT {
    NMHDR      hdr;
    CCellRange range;
} GV_CACHEHINT;

// storage typedef for each row in the grid
typedef CTypedPtrArray<CObArray, CGridCellBase*> GRID_ROW;

// For virtual mode callback
typedef BOOL (CALLBACK* GRIDCALLBACK)(GV_DISPINFO *, LPARAM);

///////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////

// Grid line/scrollbar selection
#define GVL_NONE                0L      // Neither
#define GVL_HORZ                1L      // Horizontal line or scrollbar
#define GVL_VERT                2L      // Vertical line or scrollbar
#define GVL_BOTH                3L      // Both

// Autosizing option
#define GVS_DEFAULT             0
#define GVS_HEADER              1       // Size using column fixed cells data only
#define GVS_DATA                2       // Size using column non-fixed cells data only
#define GVS_BOTH                3       // Size using column fixed and non-fixed

// Cell Searching options
#define GVNI_FOCUSED            0x0001
#define GVNI_SELECTED           0x0002
#define GVNI_DROPHILITED        0x0004
#define GVNI_READONLY           0x0008
#define GVNI_FIXED              0x0010
#define GVNI_MODIFIED           0x0020

#define GVNI_ABOVE              LVNI_ABOVE
#define GVNI_BELOW              LVNI_BELOW
#define GVNI_TOLEFT             LVNI_TOLEFT
#define GVNI_TORIGHT            LVNI_TORIGHT
#define GVNI_ALL                (LVNI_BELOW|LVNI_TORIGHT|LVNI_TOLEFT)
#define GVNI_AREA               (LVNI_BELOW|LVNI_TORIGHT)

// Hit test values (not yet implemented)
#define GVHT_DATA               0x0000
#define GVHT_TOPLEFT            0x0001
#define GVHT_COLHDR             0x0002
#define GVHT_ROWHDR             0x0004
#define GVHT_COLSIZER           0x0008
#define GVHT_ROWSIZER           0x0010
#define GVHT_LEFT               0x0020
#define GVHT_RIGHT              0x0040
#define GVHT_ABOVE              0x0080
#define GVHT_BELOW              0x0100

// Messages sent to the grid's parent (More will be added in future)
#define GVN_BEGINDRAG           LVN_BEGINDRAG        // LVN_FIRST-9
#define GVN_BEGINLABELEDIT      LVN_BEGINLABELEDIT   // LVN_FIRST-5
#define GVN_BEGINRDRAG          LVN_BEGINRDRAG
#define GVN_COLUMNCLICK         LVN_COLUMNCLICK
#define GVN_DELETEITEM          LVN_DELETEITEM
#define GVN_ENDLABELEDIT        LVN_ENDLABELEDIT     // LVN_FIRST-6
#define GVN_SELCHANGING         LVN_ITEMCHANGING
#define GVN_SELCHANGED          LVN_ITEMCHANGED
#define GVN_GETDISPINFO         LVN_GETDISPINFO 
#define GVN_ODCACHEHINT         LVN_ODCACHEHINT 
#define GVN_COLWIDTHCHANGED		(LVN_FIRST - 0x200)
#define GVN_KEYDOWNEND			(LVN_FIRST - 0x201)
#define GVN_KEYUPEND			(LVN_FIRST - 0x202)

#define GVSDS_DEFAULT			(0)		// ѡ��Ԫ��Ĭ�ϻ��Ʒ�ʽ
#define GVSDS_BOTTOMLINE		(1)		// ѡ��Ԫ��ײ������߻��Ʒ�ʽ

#define WM_DISENTRUST			(WM_USER+1)

class CGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

typedef bool (*PVIRTUALCOMPARE)(int, int);

class CGridCtrlCB
{
protected:
	virtual void OnFixedRowClickCB(CCellID& cell) = 0;
	virtual void OnFixedColumnClickCB(CCellID& cell) = 0;
    virtual void OnHScrollEnd() = 0;
    virtual void OnVScrollEnd() = 0;
	virtual void OnCtrlMove( int32 x, int32 y ) = 0;
	virtual bool32 OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ) = 0;
	virtual void OnGridMouseMove(CPoint pt){}
	virtual bool32 BeBlindSelect() { return false; }
friend class CGridCtrl;
};

class CGridCtrl : public CWnd
{
    DECLARE_DYNCREATE(CGridCtrl)
    friend class CGridCell;
    friend class CGridCellBase;
    friend class CIoViewRelative;
	friend class CIoViewReport;
	friend class CIoViewReportSelect;		// ����ʱ��ô����

// Construction
public:
    CGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD /*| WS_BORDER*/ | WS_TABSTOP | WS_VISIBLE);

///////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    int  GetRowCount() const                    { return m_nRows; }
    int  GetColumnCount() const                 { return m_nCols; }
    int  GetFixedRowCount() const               { return m_nFixedRows; }
    int  GetFixedColumnCount() const            { return m_nFixedCols; }
    BOOL SetRowCount(int nRows = 10);
    BOOL SetColumnCount(int nCols = 10);
    BOOL SetFixedRowCount(int nFixedRows = 1);
    BOOL SetFixedColumnCount(int nFixedCols = 1);

	void SetFullDragAble( BOOL bDragAble );
	
    int  GetRowHeight(int nRow) const;
    BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    BOOL SetColumnWidth(int col, int width);

    BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p);
    BOOL GetCellOrigin(const CCellID& cell, LPPOINT p);
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect);
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect);

    BOOL GetTextRect(const CCellID& cell, LPRECT pRect);
    BOOL GetTextRect(int nRow, int nCol, LPRECT pRect);

    CCellID GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck = TRUE);

    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;
    long GetVirtualWidth() const;
    long GetVirtualHeight() const;

    CSize GetTextExtent(int nRow, int nCol, LPCTSTR str);
    // EFW - Get extent of current text in cell
    inline CSize GetCellTextExtent(int nRow, int nCol)  { return GetTextExtent(nRow, nCol, GetItemText(nRow,nCol)); }

	void	 ShowGridLine(BOOL bShow)	
	{
//		m_bShowGridLine = bShow; 
		if (!bShow)
		{
			SetGridLines(GVL_NONE);
		}
		else
		{
			SetGridLines(GVL_BOTH);
		}
		Refresh() ; Invalidate();		
	}
//	BOOL	 BeShowGridLine()					  { return m_bShowGridLine; }
    void     SetGridBkColor(COLORREF clr)         { m_crGridBkColour = clr;           }	
    virtual COLORREF GetGridBkColor() const               { return m_crGridBkColour;          }
    void     SetGridLineColor(COLORREF clr)       { m_crGridLineColour = clr;         }
    virtual COLORREF GetGridLineColor() const             { return m_crGridLineColour;        }

	BOOL	IsBlinkEnable() const { return m_bEnableBlink; }
	void	EnableBlink(BOOL bBlink = TRUE) {  m_bEnableBlink = bBlink; }

	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT) { m_crTTipBackClr = clr;  }
	COLORREF GetTitleTipBackClr()				            { return m_crTTipBackClr; }
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT) { m_crTTipTextClr = clr;  }
	COLORREF GetTitleTipTextClr()				            { return m_crTTipTextClr; }

    // ***************************************************************************** //
    // These have been deprecated. Use GetDefaultCell and then set the colors
    void     SetTextColor(COLORREF clr)      { m_cellDefault.SetTextClr(clr);        }
    COLORREF GetTextColor()                  { return m_cellDefault.GetTextClr();    }
    void     SetTextBkColor(COLORREF clr)    { m_cellDefault.SetBackClr(clr);        }
    COLORREF GetTextBkColor()                { return m_cellDefault.GetBackClr();    }
    void     SetFixedTextColor(COLORREF clr) { m_cellFixedRowDef.SetTextClr(clr); 
                                               m_cellFixedColDef.SetTextClr(clr); 
                                               m_cellFixedRowColDef.SetTextClr(clr); }
    COLORREF GetFixedTextColor() const       { return m_cellFixedRowDef.GetTextClr(); }
    void     SetFixedBkColor(COLORREF clr)   { m_cellFixedRowDef.SetBackClr(clr); 
                                               m_cellFixedColDef.SetBackClr(clr); 
                                               m_cellFixedRowColDef.SetBackClr(clr); }
	void	 SetFixColBkColor(COLORREF clr)	 { m_cellFixedColDef.SetBackClr(clr); }	// lcq add 20131225 �������������ֶα���

    COLORREF GetFixedBkColor() const         { return m_cellFixedRowDef.GetBackClr(); }
    void     SetGridColor(COLORREF clr)      { SetGridLineColor(clr);                }
    COLORREF GetGridColor()                  { return GetGridLineColor();            }
    void     SetBkColor(COLORREF clr)        { SetGridBkColor(clr);                  }
	void	 SetBorderColor(COLORREF clr, BOOL OnlyTopLeft = TRUE);  	 
    COLORREF GetBkColor()                    { return GetGridBkColor();              }
  
    void     SetDefCellMargin( int nMargin)  { m_cellDefault.SetMargin(nMargin); 
                                               m_cellFixedRowDef.SetMargin(nMargin); 
                                               m_cellFixedColDef.SetMargin(nMargin); 
                                               m_cellFixedRowColDef.SetMargin(nMargin); }
    int      GetDefCellMargin() const        { return m_cellDefault.GetMargin();     }

    int      GetDefCellHeight() const        { return m_cellDefault.GetHeight();     }
    void     SetDefCellHeight(int nHeight)   { m_cellDefault.SetHeight(nHeight); 
                                               m_cellFixedRowDef.SetHeight(nHeight); 
                                               m_cellFixedColDef.SetHeight(nHeight); 
                                               m_cellFixedRowColDef.SetHeight(nHeight); }
    int      GetDefCellWidth() const         { return m_cellDefault.GetWidth();     }
    void     SetDefCellWidth(int nWidth)     { m_cellDefault.SetWidth(nWidth); 
                                               m_cellFixedRowDef.SetWidth(nWidth); 
                                               m_cellFixedColDef.SetWidth(nWidth); 
                                               m_cellFixedRowColDef.SetWidth(nWidth); }

	bool	HavePolygonConer() {return m_bEnablePolygonCorner;}
    // ***************************************************************************** //

    int GetSelectedCount() const                  { return (int)m_SelectedCellMap.GetCount(); }

    CCellID SetFocusCell(CCellID cell);
    CCellID SetFocusCell(int nRow, int nCol);
    CCellID GetFocusCell() const                  { return m_idCurrentCell;           }


    void SetVirtualMode(BOOL bVirtual);
    BOOL GetVirtualMode() const                   { return m_bVirtualMode;            }
    void SetCallbackFunc(GRIDCALLBACK pCallback, 
                         LPARAM lParam)           { m_pfnCallback = pCallback; m_lParam = lParam; }
    GRIDCALLBACK GetCallbackFunc()                { return m_pfnCallback;             }


    void SetImageList(CImageList* pList)          { m_pImageList = pList;             }
    CImageList* GetImageList() const              { return m_pImageList;              }

    void SetGridLines(int nWhichLines = GVL_BOTH);
    int  GetGridLines() const                     { return m_nGridLines;              }
    void SetEditable(BOOL bEditable = TRUE)       { m_bEditable = bEditable;          }
    BOOL IsEditable() const                       { return m_bEditable;               }
    void SetListMode(BOOL bEnableListMode = TRUE);
    BOOL GetListMode() const                      { return m_bListMode;               }
    void SetSingleRowSelection(BOOL bSing = TRUE) { m_bSingleRowSelection = bSing;    }
    BOOL GetSingleRowSelection()                  { return m_bSingleRowSelection & m_bListMode; }
    void SetSingleColSelection(BOOL bSing = TRUE) { m_bSingleColSelection = bSing;    }
    BOOL GetSingleColSelection()                  { return m_bSingleColSelection;     }
    void EnableSelection(BOOL bEnable = TRUE)     { ResetSelectedRange(); m_bEnableSelection = bEnable; ResetSelectedRange(); }
    BOOL IsSelectable() const                     { return m_bEnableSelection;        }
    void SetFixedColumnSelection(BOOL bSelect)    { m_bFixedColumnSelection = bSelect;}
    BOOL GetFixedColumnSelection()                { return m_bFixedColumnSelection;   }
    void SetFixedRowSelection(BOOL bSelect)       { m_bFixedRowSelection = bSelect;   }
    BOOL GetFixedRowSelection()                   { return m_bFixedRowSelection;      }
    void EnableDragAndDrop(BOOL bAllow = TRUE)    { m_bAllowDragAndDrop = bAllow;     }
    BOOL GetDragAndDrop() const                   { return m_bAllowDragAndDrop;       }
    void SetRowResize(BOOL bResize = TRUE)        { m_bAllowRowResize = bResize;      }
    BOOL GetRowResize() const                     { return m_bAllowRowResize;         }
    void SetColumnResize(BOOL bResize = TRUE)     { m_bAllowColumnResize = bResize;   }
    BOOL GetColumnResize() const                  { return m_bAllowColumnResize;      }
    void SetHeaderSort(BOOL bSortOnClick = TRUE)  { m_bSortOnClick = bSortOnClick;    }
    BOOL GetHeaderSort() const                    { return m_bSortOnClick;            }
    void SetHandleTabKey(BOOL bHandleTab = TRUE)  { m_bHandleTabKey = bHandleTab;     }
    BOOL GetHandleTabKey() const                  { return m_bHandleTabKey;           }
    void SetDoubleBuffering(BOOL bBuffer = TRUE)  { m_bDoubleBuffer = bBuffer;        }
    BOOL GetDoubleBuffering() const               { return m_bDoubleBuffer;           }
    void EnableTitleTips(BOOL bEnable = TRUE)     { m_bTitleTips = bEnable;           }
    BOOL GetTitleTips()                           { return m_bTitleTips;              }
    void SetSortColumn(int nCol);
    int  GetSortColumn() const                    { return m_nSortColumn;             }
    void SetSortAscending(BOOL bAscending)        { m_bAscending = bAscending;        }
    BOOL GetSortAscending() const                 { return m_bAscending;              }
    void SetTrackFocusCell(BOOL bTrack)           { m_bTrackFocusCell = bTrack;       }
    BOOL GetTrackFocusCell()                      { return m_bTrackFocusCell;         }
    void SetFrameFocusCell(BOOL bFrame)           { m_bFrameFocus = bFrame;           }
    BOOL GetFrameFocusCell()                      { return m_bFrameFocus;             }
    void SetAutoSizeStyle(int nStyle = GVS_BOTH)  { m_nAutoSizeColumnStyle = nStyle;  }
    int  GetAutoSizeStyle()                       { return m_nAutoSizeColumnStyle; }

    void EnableHiddenColUnhide(BOOL bEnable = TRUE){ m_bHiddenColUnhide = bEnable;    }
    BOOL GetHiddenColUnhide()                     { return m_bHiddenColUnhide;        }
    void EnableHiddenRowUnhide(BOOL bEnable = TRUE){ m_bHiddenRowUnhide = bEnable;    }
    BOOL GetHiddenRowUnhide()                     { return m_bHiddenRowUnhide;        }

    void EnableColumnHide(BOOL bEnable = TRUE)    { m_bAllowColHide = bEnable;        }
    BOOL GetColumnHide()                          { return m_bAllowColHide;           }
    void EnableRowHide(BOOL bEnable = TRUE)       { m_bAllowRowHide = bEnable;        }
    BOOL GetRowHide()                             { return m_bAllowRowHide;           }
	BOOL BeDrawSortArrow()						  { return m_bNeedDrawSortArrow;	  }

	// linhc 0908
	void SetHideTextFlag(bool bOnlyHideText)	{ m_bOnlyHideText = bOnlyHideText; }
	bool GetHideTextFlag()						{ return m_bOnlyHideText; }

	void SetColsEditableFlag(bool32 bSomeColsEditable)	{ m_bSomeColsEditable = bSomeColsEditable; }
	void SetEditableCols(CArray<int32,int32> &aEditableCols);

	void SetUserCB ( CGridCtrlCB* pCB );
//	void SetBaseLines ( CArray<int32,int32>& rows );
//	void SetBaseLineColor ( COLORREF color );


	void SetAutoHideFragmentaryCell(BOOL bHide = TRUE) { m_bAutoHideFragmentaryCell = bHide; }
	BOOL IsAutoHideFragmentaryCell() const			{ return m_bAutoHideFragmentaryCell; }

	void SetShowSelectWhenLoseFocus(BOOL bShow = TRUE) { m_bShowSelectWhenLoseFocus = bShow; }
	BOOL IsShowSelectWhenLoseFocus() const			{ return m_bShowSelectWhenLoseFocus; }

	void SetDrawFixedCellGridLineAsNormal(BOOL bAs = FALSE) { m_bDrawFixedCellGridLineAsNormal = bAs; };
	BOOL IsDrawFixedCellGridLineAsNormal() const	{ return m_bDrawFixedCellGridLineAsNormal; }
	
	void SetDonotScrollToNonExistCell(DWORD dwGVL = GVL_NONE) { m_dwDonotScrollToNonExistCell = dwGVL; }
	DWORD GetDonotScrollToNonExistCell() const { return m_dwDonotScrollToNonExistCell; }

	void SetInitSortAscending(BOOL bInitAscending = TRUE) { m_bInitSortAscending = bInitAscending; }	
	BOOL GetInitSortAscending() const { return m_bInitSortAscending; }

	void SetRowHeightFixed(BOOL bFixed = FALSE) { m_bRowHeightFixed = bFixed; }
	BOOL IsRowHeightFixed() const { return m_bRowHeightFixed; }

	void SetDrawSelectedCellStyle(DWORD nStyle) { m_nDrawSelectedCellStyle = nStyle; }
	DWORD GetDrawSelectedCellStyle() const { return m_nDrawSelectedCellStyle; }

	void SetRemoveSelectClickBlank(BOOL bRemove=FALSE) { m_bRemoveSelectClickBlank = bRemove;}	//lcq add 20130117����հ�����ȡ���ϴ�ѡ����
	void SetListHeaderCanClick(BOOL bCan=TRUE) {m_bListHeaderCanNotClick = bCan;}			//lcq add 20130117��ͷ���Ա�ѡ��,ѡ����ʱ��ͷ�᰼��ȥ
///////////////////////////////////////////////////////////////////////////////////
// default Grid cells. Use these for setting default values such as colors and fonts
///////////////////////////////////////////////////////////////////////////////////

	void	EnablePolygonCorner(bool bEnable){m_bEnablePolygonCorner = bEnable;}
	
	void    SetNeedShowFixSpliter(bool bNeed) { m_bNeedShowFixRowSpliter = bNeed;}    //�Ƿ���ʾ�̶��еķָ���
	BOOL    GetNeedShowFixSpliter() { return m_bNeedShowFixRowSpliter; }
public:
    CGridCellBase* GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol) const;

///////////////////////////////////////////////////////////////////////////////////
// Grid cell Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    virtual	CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!

    void SetModified(BOOL bModified = TRUE, int nRow = -1, int nCol = -1);
    BOOL GetModified(int nRow = -1, int nCol = -1);
    BOOL IsCellFixed(int nRow, int nCol);

    BOOL   SetItem(const GV_ITEM* pItem);
    BOOL   GetItem(GV_ITEM* pItem);
    BOOL   SetItemText(int nRow, int nCol, LPCTSTR str);
    // The following was virtual. If you want to override, use 
    //  CGridCellBase-derived class's GetText() to accomplish same thing
    CString GetItemText(int nRow, int nCol) const;

    // EFW - 06/13/99 - Added to support printf-style formatting codes.
    // Also supports use with a string resource ID
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 210)
    BOOL   SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...);
    BOOL   SetItemTextFmtID(int nRow, int nCol, UINT nID, ...);
#endif

    BOOL   SetItemData(int nRow, int nCol, LPARAM lParam);
    LPARAM GetItemData(int nRow, int nCol) const;
    BOOL   SetItemImage(int nRow, int nCol, int iImage);
    int    GetItemImage(int nRow, int nCol) const;
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;
    BOOL   SetItemFormat(int nRow, int nCol, UINT nFormat);
    UINT   GetItemFormat(int nRow, int nCol) const;
    BOOL   SetItemBkColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemBkColour(int nRow, int nCol) const;
    BOOL   SetItemFgColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemFgColour(int nRow, int nCol) const;
    BOOL SetItemFont(int nRow, int nCol, const LOGFONT* lf);
	BOOL SetItemTextPadding(int nRow, int nCol, const CRect rtTextPadding);
    const LOGFONT* GetItemFont(int nRow, int nCol);

    BOOL IsItemEditing(int nRow, int nCol);

    BOOL SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass);
    BOOL SetDefaultCellType( CRuntimeClass* pRuntimeClass);

	void SetScrollBar(CXScrollBar *pXSBHorz, CXScrollBar *pXSBVert);
	
	int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

///////////////////////////////////////////////////////////////////////////////////
// Operations
///////////////////////////////////////////////////////////////////////////////////
public:
    int  InsertColumn(LPCTSTR strHeading, UINT nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE,
                      int nColumn = -1);
    
	void InsertRowBatchBegin();
	void InsertRowBatchEnd();
	int	 InsertRowBatchForOne ( LPCTSTR strHeading, int nRow = -1 );

	int  InsertRow(LPCTSTR strHeading, int nRow = -1);
	
    BOOL DeleteColumn(int nColumn);
    BOOL DeleteRow(int nRow);
    BOOL DeleteNonFixedRows();
    BOOL DeleteAllItems();

	void ClearCells(CCellRange Selection);

    BOOL AutoSizeRow(int nRow, BOOL bResetScroll = TRUE);
    BOOL AutoSizeColumn(int nCol, UINT nAutoSizeStyle = GVS_DEFAULT, BOOL bResetScroll = TRUE);
    void AutoSizeRows();
    void AutoSizeColumns(UINT nAutoSizeStyle = GVS_DEFAULT);
	// һ�����������Ӧ�п�ķ���,��Ϊ�Դ�����������ⲻ���ϴ˴�Ҫ��,�ֲ������Ķ�,����Ӱ�������ط����ִ���
	void AdjustColumnsWidth();

	//lxp add �����������п�
	void AutoSizeColumnslv(const float &flv =1.0,UINT nAutoSizeStyle =GVS_DEFAULT);
	BOOL AutoSizeColumnlv(const float &flv,int nCol, UINT nAutoSizeStyle = GVS_DEFAULT, BOOL bResetScroll = TRUE);
	CSize  GetColumuExtent(const LOGFONT* pLogFont,bool bSingle = false);
    void AutoSize(UINT nAutoSizeStyle = GVS_DEFAULT);
    virtual void ExpandColumnsToFit(BOOL bExpandFixed = TRUE);
    void ExpandLastColumn();
    virtual void ExpandRowsToFit(BOOL bExpandFixed = TRUE);
    void ExpandToFit(BOOL bExpandFixed = TRUE);

    void Refresh();
    void AutoFill();   // Fill grid with blank cells

    void EnsureVisible(CCellID &cell)       { EnsureVisible(cell.row, cell.col); }
    void EnsureVisible(int nRow, int nCol);
    BOOL IsCellVisible(int nRow, int nCol);
    BOOL IsCellVisible(CCellID cell);
    BOOL IsCellEditable(int nRow, int nCol) const;
    BOOL IsCellEditable(CCellID &cell) const;
    BOOL IsCellSelected(int nRow, int nCol) const;
    BOOL IsCellSelected(CCellID &cell) const;

    // SetRedraw stops/starts redraws on things like changing the # rows/columns
    // and autosizing, but not for user-intervention such as resizes
    void SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = NULL);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = NULL);
    BOOL RedrawRow(int row);
    BOOL RedrawColumn(int col);

#ifndef _WIN32_WCE
    BOOL Save(LPCTSTR filename, TCHAR chSeparator = _T(','));
    BOOL Load(LPCTSTR filename, TCHAR chSeparator = _T(','));
#endif

///////////////////////////////////////////////////////////////////////////////////
// Cell Ranges
///////////////////////////////////////////////////////////////////////////////////
 public:
    CCellRange GetCellRange() const;
    CCellRange GetSelectedCellRange() const;
	const CMap<DWORD,DWORD, CCellID, CCellID&> &GetSelectedCellMap()const { return m_SelectedCellMap; }
	POSITION   GetFirstSelectedRowPosition() const;
	int		   GetNextSelectedRow(INOUT POSITION &pos) const;
    void SetSelectedRange(const CCellRange& Range, BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                          BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
	void SetSelectedSingleRow(int iRow);
    BOOL IsValid(int nRow, int nCol) const;
    BOOL IsValid(const CCellID& cell) const;
    BOOL IsValid(const CCellRange& range) const;

///////////////////////////////////////////////////////////////////////////////////
// Clipboard, drag and drop, and cut n' paste operations
///////////////////////////////////////////////////////////////////////////////////
#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void CutSelectedText();
    virtual COleDataSource* CopyTextFromGrid();
    virtual BOOL PasteTextToGrid(CCellID cell, COleDataObject* pDataObject, BOOL bSelectPastedCells=TRUE);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
 public:
    virtual void OnBeginDrag();
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void OnEditCut();
    virtual void OnEditCopy();
    virtual void OnEditPaste();
#endif
    virtual void OnEditSelectAll();

///////////////////////////////////////////////////////////////////////////////////
// Misc.
///////////////////////////////////////////////////////////////////////////////////
public:
    CCellID GetNextItem(CCellID& cell, int nFlags) const;

	BOOL SortItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortTextItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data = 0);

	void SetCompareFunction(PFNLVCOMPARE pfnCompare);

	// in-built sort functions
	static int CALLBACK pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
//	static int CALLBACK pfnCellSymbolCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

///////////////////////////////////////////////////////////////////////////////////
// Printing
///////////////////////////////////////////////////////////////////////////////////
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
public:
    void Print(CPrintDialog* pPrntDialog = NULL);

    // EFW - New printing support functions
    void EnableWysiwygPrinting(BOOL bEnable = TRUE) { m_bWysiwygPrinting = bEnable;     }
    BOOL GetWysiwygPrinting()                       { return m_bWysiwygPrinting;        }

    void SetShadedPrintOut(BOOL bEnable = TRUE)     {   m_bShadedPrintOut = bEnable;    }
    BOOL GetShadedPrintOut(void)                    {   return m_bShadedPrintOut;       }

    // Use -1 to have it keep the existing value
    void SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
        int nLeftMargin, int nRightMargin, int nTopMargin,
        int nBottomMargin, int nGap);

    void GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
        int &nLeftMargin, int &nRightMargin, int &nTopMargin,
        int &nBottomMargin, int &nGap);

///////////////////////////////////////////////////////////////////////////////////
// Printing overrides for derived classes
///////////////////////////////////////////////////////////////////////////////////
public:
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

#endif // #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)

// Implementation
public:
    virtual ~CGridCtrl();

public:
	BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect);

protected:
    BOOL RegisterWindowClass();
    BOOL Initialise();
    void SetupDefaultCells();

    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage, int nParam1 = 0, int nParam2 = 0) const;
    LRESULT SendDisplayRequestToParent(GV_DISPINFO* pDisplayInfo) const;
    LRESULT SendCacheHintToParent(const CCellRange& range) const;

    BOOL InvalidateCellRect(const int row, const int col);
    BOOL InvalidateCellRect(const CCellID& cell);
    BOOL InvalidateCellRect(const CCellRange& cellRange);
    void EraseBkgnd(CDC* pDC);

    
    BOOL SetCell(int nRow, int nCol, CGridCellBase* pCell);

    int  SetMouseMode(int nMode) { int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int  GetMouseMode() const    { return m_MouseMode; }

    BOOL MouseOverRowResizeArea(CPoint& point);
    BOOL MouseOverColumnResizeArea(CPoint& point);

	void UpdateSelectedRows() const;	// ����ѡ����е�����
public:
    CCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
protected:
    CCellRange GetUnobstructedNonFixedCellRange(BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE, BOOL bContainFragment=TRUE);
    CCellRange GetVisibleFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE);

    BOOL IsVisibleVScroll() { return ( (m_nBarState & GVL_VERT) > 0); } 
    BOOL IsVisibleHScroll() { return ( (m_nBarState & GVL_HORZ) > 0); }
    void ResetSelectedRange();
    void ResetScrollBars();
    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
   

    BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                   int low, int high);

	// fangz 1023# ��ѡ������,���һ�в��μ�.����һ���趨�����Row ��Χ�ĺ���

	// void SetSortRowRange(int lowRow,int highRow);	

	//////////////////////////////////////////////////////////////////////////
    CPoint GetPointClicked(int nRow, int nCol, const CPoint& point);

	void ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText);

	void OnTimerLButtonDown();
	void OnTimerShowTextChange();
	void OnTimerShowTips();

	void BlindKeyFocusCell(bool32 bBlind);

// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrl)
    protected:
    virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMove( int x, int y );
    //}}AFX_VIRTUAL

protected:
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // Printing
	virtual void PrintFixedRowCells(int nStartColumn, int nStopColumn, int& row, CRect& rect,
                                    CDC *pDC, BOOL& bFirst);
    virtual void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Drag n' drop
    virtual CImageList* CreateDragImage(CPoint *pHotSpot);    // no longer necessary
#endif

public:
    // Mouse Clicks
	//     virtual void  OnFixedColumnClick(CCellID& cell);
	//     virtual void  OnFixedRowClick(CCellID& cell);
	virtual void  OnFixedRowClick(CCellID& cell,CPoint PointCellRelative);
	virtual void  OnFixedColumnClick(CCellID& cell,CPoint PointCellRelative);
	
	void EnableOriginalScroll(BOOL bEnable = FALSE) { m_bOriginalScroll = TRUE; }; //2013-7-19 cym add ��ʾԭ�й�����

    void SetIgnoreHeader(bool32 bIgnore)    { m_bIgnoreHeader = bIgnore; }
protected:
    // Editing
    virtual void  OnEditCell(int nRow, int nCol, CPoint point, UINT nChar);
    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
	virtual BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);
    virtual void  EndEditing();
	virtual BOOL  PreCreateWindow(CREATESTRUCT& cs);
    // Drawing
    virtual void  OnDraw(CDC* pDC);

    // CGridCellBase Creation and Cleanup
    virtual CGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);

// Attributes
protected:
    // General attributes
    COLORREF    m_crFixedTextColour, m_crFixedBkColour;
    COLORREF    m_crGridBkColour, m_crGridLineColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;
    COLORREF    m_crTTipBackClr, m_crTTipTextClr;                 // Titletip colours - FNA
    
    BOOL        m_bVirtualMode;
    LPARAM      m_lParam;                                           // lParam for callback
    GRIDCALLBACK m_pfnCallback;                                     // The callback function

    int         m_nGridLines;
    BOOL        m_bEditable;
    BOOL        m_bModified;
    BOOL        m_bAllowDragAndDrop;
    BOOL        m_bListMode;
    BOOL        m_bSingleRowSelection;
    BOOL        m_bSingleColSelection;
    BOOL        m_bAllowDraw;
    BOOL        m_bEnableSelection;
    BOOL        m_bFixedRowSelection, m_bFixedColumnSelection;
    BOOL        m_bSortOnClick;
	
	// fangz0602 ����������ͷ��ʱ��Ż������ͷ
	BOOL		m_bNeedDrawSortArrow;
	
	// 
	bool32		m_bBlindKeyFocusCell;
	// xl 0807 ��ģʽ��ɨ��blink��Ҫ���⿪�������Դ�����blink
	BOOL		m_bEnableBlink;

	BOOL		m_bAutoHideFragmentaryCell;		// ����ֻ����ʾһ���ֵĵ�Ԫ��

	BOOL		m_bShowSelectWhenLoseFocus;		// ʧȥ����ʱ���Ƿ���ʾѡ��

	BOOL		m_bDrawFixedCellGridLineAsNormal;	// ���̶����ӵı���߻���Ϊһ������ʽ��or��ǰ�Ĺ̶���ʽ

	DWORD		m_dwDonotScrollToNonExistCell;	// ��ֹ�����������ڵı��, ��ֹ����������ã����򲻽�ֹ

	BOOL		m_bInitSortAscending;

	BOOL		m_bRowHeightFixed;				// �и߶Ƚ�ʹ�ù̶��߶�

    BOOL        m_bHandleTabKey;
    BOOL        m_bDoubleBuffer;
    BOOL        m_bTitleTips;
    int         m_nBarState;
    BOOL        m_bWysiwygPrinting;
    BOOL        m_bHiddenColUnhide, m_bHiddenRowUnhide;
    BOOL        m_bAllowColHide, m_bAllowRowHide;
    BOOL        m_bAutoSizeSkipColHdr;
    BOOL        m_bTrackFocusCell;
    BOOL        m_bFrameFocus;
//	BOOL		m_bShowGridLine;
    UINT        m_nAutoSizeColumnStyle;
	COLORREF	m_clrBorder;
	BOOL		m_bOnlyDrawTopLeft;
    // Cell size details
    int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CUIntArray  m_arRowHeights, m_arColWidths;
    int         m_nVScrollMax, m_nHScrollMax;

	bool		m_bEnablePolygonCorner;		//	���ö�����Ľ�
    // Fonts and images
    CRuntimeClass*   m_pRtcDefault; // determines kind of Grid Cell created by default
    CGridDefaultCell m_cellDefault;  // "default" cell. Contains default colours, font etc.
    CGridDefaultCell m_cellFixedColDef, m_cellFixedRowDef, m_cellFixedRowColDef;
    CFont       m_PrinterFont;  // for the printer
    CImageList* m_pImageList;

    // Cell data
    CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;

    // Mouse operations such as cell selection
    int         m_MouseMode;
    BOOL        m_bLMouseButtonDown, m_bRMouseButtonDown;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    CCellID     m_LeftClickDownCell, m_SelectionStartCell;
    CCellID     m_idCurrentCell, m_idTopLeftCell;
    INT_PTR     m_nTimerID;
    int         m_nTimerInterval;
    int         m_nResizeCaptureRange;
    BOOL        m_bAllowRowResize, m_bAllowColumnResize;
    int         m_nRowsPerWheelNotch;
    CMap<DWORD,DWORD, CCellID, CCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;
	mutable CList<int, int>		m_SelectedRowList;
	
	// ���������пɱ༭
	bool32			m_bSomeColsEditable;
	CArray<int32,int32> m_aEditCols;	
	
	//////////////////////////////////////////////////////////////////////////
	// fangz 1023#

	// int			m_nLowSortRow;
	// int			m_nHightSortRow;

	// linhc 0908
	bool		m_bOnlyHideText;

	//////////////////////////////////////////////////////////////////////////
#ifndef GRIDCONTROL_NO_TITLETIPS
    CTitleTip   m_TitleTip;             // Title tips for cells
#endif
public:
	CXTipWnd m_TipWnd;
    // Drag and drop
    CCellID     m_LastDragOverCell;
#ifndef GRIDCONTROL_NO_DRAGDROP
    CGridDropTarget m_DropTarget;       // OLE Drop target for the grid
#endif

    // Printing information
    CSize       m_CharSize;
    int         m_nPageHeight;
    CSize       m_LogicalPageSize,      // Page size in gridctrl units.
                m_PaperSize;            // Page size in device units.
    // additional properties to support Wysiwyg printing
    int         m_nPageWidth;
    int         m_nPrintColumn;
    int         m_nCurrPrintRow;
    int         m_nNumPages;
    int         m_nPageMultiplier;

    // sorting
    int          m_bAscending;
    int          m_nSortColumn;
	PFNLVCOMPARE m_pfnCompare;

    // EFW - Added to support shaded/unshaded printout.  If true, colored
    // cells will print as-is.  If false, all text prints as black on white.
    BOOL        m_bShadedPrintOut;

    // EFW - Added support for user-definable margins.  Top and bottom are in 
    // lines.  Left, right, and gap are in characters (avg width is used).
    int         m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
                m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;

	DWORD		m_nDrawSelectedCellStyle;		// ����ѡ��Ԫ�ķ�ʽ 0-Ĭ�ϣ���ǰ����Ӱ������ʽ 1-�±�һ�������ߵķ�ʽ

	BOOL		m_bRemoveSelectClickBlank;		// ����հ�����ȡ���ϴ�ѡ�е���
	BOOL		m_bListHeaderCanNotClick;		// ��ͷ���Ա�ѡ��
private:
	CXScrollBar		*m_pXSBHorz;
	CXScrollBar		*m_pXSBVert;

	BOOL            m_bOriginalScroll;
	HCURSOR			m_hHandCursor;

    bool32          m_bIgnoreHeader;
    bool32          m_bNeedShowFixRowSpliter;
protected:
    void SelectAllCells();
    void SelectColumns(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectRows(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectCells(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void OnSelecting(const CCellID& currentCell);

    // Generated message map functions
    //{{AFX_MSG(CGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif
#ifndef _WIN32_WCE
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);    // EFW - Added
    afx_msg void OnSysColorChange();
#endif
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg void OnCaptureChanged(CWnd *pWnd);
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
#endif
#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#endif
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnImeChar(WPARAM wCharCode, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

    enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                       MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                       MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                       MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                       MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
                       MOUSE_PREPARE_DRAG, MOUSE_DRAGGING
#endif
    };
//      for sort in virtual mode, and column order, save and load layer
public:
	typedef std::vector<int> intlist;
	void Reorder(int From, int To);
	void SetVirtualCompare(PVIRTUALCOMPARE VirtualCompare) { m_pfnVirtualCompare = VirtualCompare;}
	int m_CurCol;
	void AllowReorderColumn(bool b=true) { m_AllowReorderColumn = b;}
	void EnableDragRowMode(bool b=true) { m_bDragRowMode = b; if(b) EnableDragAndDrop(); } // to change row order
	int GetLayer(int** pLayer); //  gives back the number of ints of the area (do not forget to delete *pLayer)
	void SetLayer(int* pLayer); // coming from a previous GetLayer (ignored if not same number of column, or the same revision number)
	void ForceQuitFocusOnTab(bool b=true) { m_QuitFocusOnTab = b;} // use only if GetParent() is a CDialog
	void AllowSelectRowInFixedCol(bool b=true) { m_AllowSelectRowInFixedCol = b;} // 
//    allow acces?
	intlist m_arRowOrder, m_arColOrder;
	static CGridCtrl* m_This;
protected:
	virtual void AddSubVirtualRow(int Num, int Nb);
	bool m_bDragRowMode;
	int m_CurRow;
private:
	void ResetVirtualOrder();
	PVIRTUALCOMPARE m_pfnVirtualCompare;
	static bool NotVirtualCompare(int c1, int c2);
	bool m_InDestructor;
	bool m_AllowReorderColumn;
	bool m_QuitFocusOnTab;
	bool m_AllowSelectRowInFixedCol;
	// cuipeng 2008-05-29
	CGridCtrlCB* m_pUserCB;
	BOOL	m_bFullDragAble;
//	CArray<int32,int32> m_aBaseLines;
//	COLORREF m_clrBaseLine;

};

// Returns the default cell implementation for the given grid region
inline CGridCellBase* CGridCtrl::GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol) const
{ 
    if (bFixedRow && bFixedCol) return (CGridCellBase*) &m_cellFixedRowColDef;
    if (bFixedRow)              return (CGridCellBase*) &m_cellFixedRowDef;
    if (bFixedCol)              return (CGridCellBase*) &m_cellFixedColDef;
    return (CGridCellBase*) &m_cellDefault;
}

inline CGridCellBase* CGridCtrl::GetCell(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return NULL;

    if (GetVirtualMode())
    {
        CGridCellBase* pCell = GetDefaultCell(nRow < m_nFixedRows, nCol < m_nFixedCols);
        static GV_DISPINFO gvdi;
        gvdi.item.row     = nRow;
        gvdi.item.col     = nCol;
        gvdi.item.mask    = 0xFFFFFFFF;
        gvdi.item.nState  = 0;
        gvdi.item.nFormat = pCell->GetFormat();
        gvdi.item.iImage  = pCell->GetImage();
        gvdi.item.crBkClr = pCell->GetBackClr();
        gvdi.item.crFgClr = pCell->GetTextClr();
        gvdi.item.lParam  = pCell->GetData();
        memcpyex(&gvdi.item.lfFont, pCell->GetFont(), sizeof(LOGFONT));
        gvdi.item.nMargin = pCell->GetMargin();
        gvdi.item.strText.Empty();
		gvdi.item.markItem = pCell->GetDrawMarkItem();
		gvdi.item.strTip	= pCell->GetTipText();
		gvdi.item.strTipTitle = pCell->GetTipTitle();
		gvdi.item.rtTextPadding = pCell->GetTextPadding();
        // Fix the state bits
        if (IsCellSelected(nRow, nCol))   gvdi.item.nState |= GVIS_SELECTED;
        if (nRow < GetFixedRowCount())    gvdi.item.nState |= (GVIS_FIXED | GVIS_FIXEDROW);
        if (nCol < GetFixedColumnCount()) gvdi.item.nState |= (GVIS_FIXED | GVIS_FIXEDCOL);
        if (GetFocusCell() == CCellID(nRow, nCol)) gvdi.item.nState |= GVIS_FOCUSED;
		if(!m_InDestructor)
		{
			gvdi.item.row = m_arRowOrder[nRow];
			gvdi.item.col = m_arColOrder[nCol];

			if (m_pfnCallback)
				m_pfnCallback(&gvdi, m_lParam);
			else
				SendDisplayRequestToParent(&gvdi);
			gvdi.item.row = nRow;        
			gvdi.item.col = nCol;
		}
        static CGridCell cell;
        cell.SetState(gvdi.item.nState);
        cell.SetFormat(gvdi.item.nFormat);
        cell.SetImage(gvdi.item.iImage);
        cell.SetBackClr(gvdi.item.crBkClr);
        cell.SetTextClr(gvdi.item.crFgClr);
        cell.SetData(gvdi.item.lParam);
        cell.SetFont(&(gvdi.item.lfFont));
        cell.SetMargin(gvdi.item.nMargin);
        cell.SetText(gvdi.item.strText);
        cell.SetGrid((CGridCtrl*)this);
		cell.SetDrawMarkItem(gvdi.item.markItem);
		cell.SetTipText(gvdi.item.strTip);
		cell.SetTipTitle(gvdi.item.strTipTitle);
		cell.SetTextPadding(gvdi.item.rtTextPadding);
        return (CGridCellBase*) &cell;
    }

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return NULL;

	int iFactCol = m_arColOrder[nCol];
    return pRow->GetAt(iFactCol);
}

inline BOOL CGridCtrl::SetCell(int nRow, int nCol, CGridCellBase* pCell)
{
    if (GetVirtualMode())
        return FALSE;

    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

	if (NULL != pCell)
		pCell->SetCoords(nRow, nCol); 
    pRow->SetAt(nCol, pCell);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)

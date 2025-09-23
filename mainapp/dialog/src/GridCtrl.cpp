// GridCtrl.cpp : implementation file
//
// MFC Grid Control v2.26
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// The code contained in this file was based on the original
// WorldCom Grid control written by Joe Willcoxson,
//        mailto:chinajoe@aol.com
//        http://users.aol.com/chinajoe
// (These addresses may be out of date) The code has gone through 
// so many modifications that I'm not sure if there is even a single 
// original line of code. In any case Joe's code was a great 
// framework on which to build.
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
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
//  History:
//  --------
//  This control is constantly evolving, sometimes due to new features that I
//  feel are necessary, and sometimes due to existing bugs. Where possible I
//  have credited the changes to those who contributed code corrections or
//  enhancements (names in brackets) or code suggestions (suggested by...)
//
//          1.0 - 1.13   20 Feb 1998 - 6 May 1999
//                          First release version. Progressed from being a basic
//                          grid based on the original WorldCom Grid control
//                          written by Joe Willcoxson (mailto:chinajoe@aol.com,
//                          http://users.aol.com/chinajoe) to something a little
//                          more feature rich. Rewritten so many times I doubt
//                          there is a single line of Joe's code left. Many, many,
//                          MANY people sent in bug reports and fixes. Thank you
//                          all.
//
//          2.0         1 Feb 2000
//                          Rewritten to make the grid more object oriented, in
//                          that the CGridCell class now takes care of cell-specific
//                          tasks. This makes the code more robust, but more
//                          importantly it allows the simple insertion of other
//                          types of cells.
//                          
//          2.10       11 Mar 2000 - Ken Bertelson and Chris Maunder
//                          - Additions for virtual CGridCell support of embedded tree 
//                            & cell buttons implementation
//                          - Optional WYSIWYG printing
//                          - Awareness of hidden (0 width/height) rows and columns for 
//                            key movements,  cut, copy, paste, and autosizing
//                          - CGridCell can make title tips display any text rather than 
//                            cell text only
//                          - Minor vis bug fixes
//                          - CGridCtrl now works with CGridCellBase instead of CGridCell
//                            This is a taste of things to come.
//
//          2.20       30 Jul 2000 - Chris Maunder
//                          - Font storage optimised (suggested by Martin Richter)
//                          - AutoSizeColumn works on either column header, data or both
//                          - EnsureVisible. The saga continues... (Ken)
//                          - Rewrote exception handling
//                          - Added TrackFocusCell and FrameFocusCell properties, as well as
//                            ExpandLastColumn (suggested by Bruce E. Stemplewski).
//                          - InsertColumn now allows you to insert columns at the end of the
//                            column range (David Weibel)
//                          - Shift-cell-selection more intuitive
//                          - API change: Set/GetGridColor now Set/GetGridLineColor
//                          - API change: Set/GetBkColor now Set/GetGridBkColor
//                          - API change: Set/GetTextColor, Set/GetTextBkColor depricated 
//                          - API change: Set/GetFixedTextColor, Set/GetFixedBkColor depricated 
//                          - Stupid DDX_GridControl workaround removed.
//                          - Added "virtual mode" via Set/GetVirtualMode
//                          - Added SetCallbackFunc to allow callback functions in virtual mode
//                          - Added Set/GetAutoSizeStyle
//                          - AutoSize() bug fixed
//                          - added GVIS_FIXEDROW, GVIS_FIXEDCOL states
//                          - added Get/SetFixed[Row|Column]Selection
//                          - cell "Get" methods now const'd. Sorry folks...
//                          - GetMouseScrollLines now uses win98/W2K friendly code
//                          - WS_EX_CLIENTEDGE style now implicit
//
//                          [ Only the latest version and major version changes will be shown ]
////
//          2.25       13 Mar 2004 - Chris Maunder
//                                 - Minor changes so it will compile in VS.NET (inc. Whidbey)
//                                 - Fixed minor bug in EnsureVisible - Junlin Xu
//                                 - Changed AfxGetInstanceHandle for AfxGetResourceHandle in RegisterWindowClass
//					               - Various changes thanks to Yogurt
//
//          2.26       13 Dec 2005 - Pierre Couderc
//                                 - Added sort in Virtual mode
//                                 - Change row/column order programatically or via drag and drop
//                                 - Added save/restore layer (for undoing row/column order changes)
//
// TODO:   1) Implement sparse grids (super easy now)
//         2) Fix it so that as you drag select, the speed of selection increases
//            with time.
//         3) Scrolling is still a little dodgy (too much grey area). I know there
//            is a simple fix but it's been a low priority
//
// ISSUES: 1) WindowFromPoint seems to do weird things in W2K. Causing problems for
//            the rigt-click-on-titletip code.
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MemDC.h"
#include "GridCtrl.h"
#include <algorithm>
#include "float.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "GridCtrlSys.h"
#include "ReportScheme.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "FontFactory.h"
#include "coding.h"
#include "DrawPolygon.h"

// OLE stuff for clipboard operations
#include <afxadv.h>            // For CSharedFile
//#include <afxconv.h>           // For LPTSTR -> LPSTR macros
#include <set>
using std::set;

#include "gridcellsymbol.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Spit out some messages as a sanity check for programmers
#ifdef GRIDCONTROL_NO_TITLETIPS
#pragma message(" -- CGridCtrl: No titletips for cells with large data")
#endif
#ifdef GRIDCONTROL_NO_DRAGDROP
#pragma message(" -- CGridCtrl: No OLE drag and drop")
#endif
#ifdef GRIDCONTROL_NO_CLIPBOARD
#pragma message(" -- CGridCtrl: No clipboard support")
#endif
#ifdef GRIDCONTROL_NO_PRINTING
#pragma message(" -- CGridCtrl: No printing support")
#endif

// 定时器
const int KTimerIdLButtonDown = 1;

// cell闪烁
const int KTimerIdShowTextChange = 2;
const int KTimerPeriodShowTextChange = 500;	// ms

// Tips 提示
const int KTimerIdShowTips       = 3;
const int KTimerPeriodShowTips   = 1000 * 1; 

extern CGridCtrlNormal	*m_gGridEntrust;


IMPLEMENT_DYNCREATE(CGridCtrl, CWnd)


// Get the number of lines to scroll with each mouse wheel notch
// Why doesn't windows give us this function???
UINT GetMouseScrollLines()
{
    int nScrollLines = 3;            // reasonable default

#ifndef _WIN32_WCE
    // Do things the hard way in win95
    OSVERSIONINFO VersionInfo;
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&VersionInfo) || 
        (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && VersionInfo.dwMinorVersion == 0))
    {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
            0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
        {
            TCHAR szData[128];
            DWORD dwKeyDataType;
            DWORD dwDataBufSize = sizeof(szData);
            
            if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
                (LPBYTE)szData, &dwDataBufSize) == ERROR_SUCCESS)
            {
                nScrollLines = _tcstoul(szData, NULL, 10);
            }
            RegCloseKey(hKey);
        }
    }
    // win98 or greater
    else
           SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
#endif

    return nScrollLines;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

CGridCtrl::CGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols)
:m_pXSBHorz(NULL),
m_pXSBVert(NULL)
{
    RegisterWindowClass();

#if !defined(GRIDCONTROL_NO_DRAGDROP) || !defined(GRIDCONTROL_NO_CLIPBOARD)
    _AFX_THREAD_STATE* pState = AfxGetThreadState();
    if (!pState->m_bNeedTerm && !AfxOleInit())
        AfxMessageBox(_T("OLE initialization failed. Make sure that the OLE libraries are the correct version"));
#endif

    // Store the system colours in case they change. The gridctrl uses
    // these colours, and in OnSysColorChange we can check to see if
    // the gridctrl colours have been changed from the system colours.
    // If they have, then leave them, otherwise change them to reflect
    // the new system colours.
    m_crWindowText        = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour      = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace            = ::GetSysColor(COLOR_3DFACE);
    m_crShadow            = ::GetSysColor(COLOR_3DSHADOW);
    m_crGridLineColour    = RGB(192,192,192);

    m_nRows               = 0;
    m_nCols               = 0;
    m_nFixedRows          = 0;
    m_nFixedCols          = 0;
	m_InDestructor        = false;

    m_bVirtualMode        = FALSE;
    m_pfnCallback         = NULL;

    m_nVScrollMax         = 0;          // Scroll position
    m_nHScrollMax         = 0;
    m_nRowsPerWheelNotch  = GetMouseScrollLines(); // Get the number of lines
                                                   // per mouse wheel notch to scroll
    m_nBarState           = GVL_NONE;
    m_MouseMode           = MOUSE_NOTHING;
    m_nGridLines          = GVL_BOTH;
    m_bEditable           = TRUE;
    m_bListMode           = FALSE;
    m_bSingleRowSelection = FALSE;
    m_bSingleColSelection = FALSE;
    m_bLMouseButtonDown   = FALSE;
    m_bRMouseButtonDown   = FALSE;
    m_bAllowDraw          = TRUE;       // allow draw updates
    m_bEnableSelection    = TRUE;
    m_bFixedColumnSelection = TRUE;
    m_bFixedRowSelection  = TRUE;
    m_bAllowRowResize     = TRUE;
    m_bAllowColumnResize  = TRUE;
    m_bSortOnClick        = FALSE;      // Sort on header row click
    m_bHandleTabKey       = TRUE;
#ifdef _WIN32_WCE
    m_bDoubleBuffer       = FALSE;      // Use double buffering to avoid flicker?
#else
    m_bDoubleBuffer       = TRUE;       // Use double buffering to avoid flicker?
#endif
    m_bTitleTips          = TRUE;       // show cell title tips

    m_bWysiwygPrinting    = FALSE;      // use size-to-width printing

    m_bHiddenColUnhide    = TRUE;       // 0-width columns can be expanded via mouse
    m_bHiddenRowUnhide    = TRUE;       // 0-Height rows can be expanded via mouse

    m_bAllowColHide       = TRUE;       // Columns can be contracted to 0-width via mouse
    m_bAllowRowHide       = TRUE;       // Rows can be contracted to 0-height via mouse

    m_bAscending          = TRUE;       // sorting stuff
    m_nSortColumn         = -1;
	m_pfnCompare		  = NULL;
	m_pfnVirtualCompare   = NULL;
    m_nAutoSizeColumnStyle = GVS_BOTH;  // Autosize grid using header and data info

    m_nTimerID            = 0;          // For drag-selection
    m_nTimerInterval      = 25;         // (in milliseconds)
    m_nResizeCaptureRange = 13;          // When resizing columns/row, the cursor has to be
                                        // within +/-3 pixels of the dividing line for
                                        // resizing to be possible
    m_pImageList          = NULL;       // Images in the grid
    m_bAllowDragAndDrop   = FALSE;      // for drag and drop - EFW - off by default
    m_bTrackFocusCell     = TRUE;       // Track Focus cell?
    m_bFrameFocus         = TRUE;       // Frame the selected cell?
//	m_bShowGridLine		  = TRUE;
	m_AllowReorderColumn  = false;
	m_QuitFocusOnTab	  = false;
	m_AllowSelectRowInFixedCol = false;
	m_bSomeColsEditable		   = false;
	m_bNeedDrawSortArrow  = FALSE;
	m_bDragRowMode = TRUE; // allow to drop a line over another one to change row order
    m_pRtcDefault = RUNTIME_CLASS(CGridCell);
	m_clrBorder			  = 0x123456;
	m_bOnlyDrawTopLeft	  = FALSE;
	m_bNeedShowFixRowSpliter = TRUE;
	//m_aBaseLines.RemoveAll();
	//m_clrBaseLine = RGB(0,0,0);

	//////////////////////////////////////////////////////////////////////////
	//	m_nLowSortRow		= -1;
	//	m_nHightSortRow		= -1;
	//////////////////////////////////////////////////////////////////////////

    SetupDefaultCells();
    SetGridBkColor(m_crShadow);

    // Set up the initial grid size
    SetRowCount(nRows);
    SetColumnCount(nCols);
    SetFixedRowCount(nFixedRows);
    SetFixedColumnCount(nFixedCols);

    SetTitleTipTextClr(CLR_DEFAULT);  //FNA
    SetTitleTipBackClr(CLR_DEFAULT); 

    // set initial selection range (ie. none)
    m_SelectedCellMap.RemoveAll();
    m_PrevSelectedCellMap.RemoveAll();


	//... fangz0424# testtips
	m_TipWnd.Create(this);

	// linhc 0908
	m_bOnlyHideText	= false;

	m_bEnableBlink = TRUE;			// 默认开启闪烁

	m_bAutoHideFragmentaryCell = TRUE;
	m_bShowSelectWhenLoseFocus = TRUE;
	m_bDrawFixedCellGridLineAsNormal = FALSE;
	m_dwDonotScrollToNonExistCell = GVL_NONE;

	m_bInitSortAscending = TRUE;

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // EFW - Added to support shaded/unshaded printout and
    // user-definable margins.
    m_bShadedPrintOut = TRUE;
    SetPrintMarginInfo(2, 2, 4, 4, 1, 1, 1);
#endif
	m_pUserCB = NULL;
	m_bFullDragAble = FALSE;

	m_bBlindKeyFocusCell = false;

	m_bRowHeightFixed = FALSE;
	m_nDrawSelectedCellStyle = 0;

	m_bOriginalScroll = FALSE;

	m_bRemoveSelectClickBlank = FALSE;
	m_bListHeaderCanNotClick = TRUE;
	m_hHandCursor = AfxGetApp()->LoadCursor(IDC_CUR_HAND);

    m_bIgnoreHeader = true;

	m_bEnablePolygonCorner = false;
}

// m_pRtcDefault, m_pImageList, m_pXSBHorz, m_pXSBVert, m_pUserCB, 这些指针其它地方有释放
//lint --e{1579}
CGridCtrl::~CGridCtrl()
{
	// zhangbo 0402 # for blink
//	KillTimer(KTimerIdLButtonDown);
//	KillTimer(KTimerIdShowTextChange);

	//... fangz0424 testtips
	m_TipWnd.DestroyWindow();
	//
	m_InDestructor = true;
    DeleteAllItems();

#ifndef GRIDCONTROL_NO_TITLETIPS
    if (m_bTitleTips && ::IsWindow(m_TitleTip.GetSafeHwnd())) 
        m_TitleTip.DestroyWindow();
#endif

	CWnd::DestroyWindow();

#if !defined(GRIDCONTROL_NO_DRAGDROP) || !defined(GRIDCONTROL_NO_CLIPBOARD)
    // BUG FIX - EFW
    COleDataSource *pSource = COleDataSource::GetClipboardOwner();
    if(pSource)
        COleDataSource::FlushClipboard();
#endif

	if (m_hHandCursor)
	{
		::DestroyCursor(m_hHandCursor);
	}
}

// Register the window class if it has not already been registered.
BOOL CGridCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    //HINSTANCE hInst = AfxGetInstanceHandle();
    HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, GRIDCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
#ifndef _WIN32_WCE_NO_CURSOR
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
#else
        wndcls.hCursor          = 0;
#endif
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = GRIDCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CGridCtrl::Initialise()
{
    // Stop re-entry problems
    static BOOL bInProcedure = FALSE;
    if (bInProcedure)
        return FALSE;
    bInProcedure = TRUE;

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.SetParentWnd(this);
#endif

	// This would be a good place to register the droptarget but
	// unfortunately this causes problems if you are using the 
	// grid in a view. 
	// Moved from OnSize.
//#ifndef GRIDCONTROL_NO_DRAGDROP
//    m_DropTarget.Register(this);
//#endif

    if (::IsWindow(m_hWnd))
        ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// Kludge: Make sure the client edge shows
	// This is so horrible it makes my eyes water.
	CRect rect;
	GetWindowRect(rect);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
		pParent->ScreenToClient(rect);
	rect.InflateRect(1,1);	MoveWindow(rect);
	rect.DeflateRect(1,1);  MoveWindow(rect);
 
    bInProcedure = FALSE;
    return TRUE;
}

// creates the control - use like any other window create control
BOOL CGridCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

	CWnd* pWnd = this;
    if (!pWnd->Create(GRIDCTRL_CLASSNAME, NULL, dwStyle | SS_NOTIFY, rect, pParentWnd, nID))
        return FALSE;

	// 不显示边框
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	
    //Initialise(); - called in PreSubclassWnd

    // The number of rows and columns will only be non-zero if the constructor
    // was called with non-zero initialising parameters. If this window was created
    // using a dialog template then the number of rows and columns will be 0 (which
    // means that the code below will not be needed - which is lucky 'cause it ain't
    // gonna get called in a dialog-template-type-situation.

    TRY
    {
        m_arRowHeights.SetSize(m_nRows);    // initialize row heights
        m_arColWidths.SetSize(m_nCols);     // initialize column widths
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        return FALSE;
    }
    END_CATCH

    int i;
    for (i = 0; i < m_nRows; i++)
        m_arRowHeights[i] = m_cellDefault.GetHeight();
    for (i = 0; i < m_nCols; i++)
        m_arColWidths[i] = m_cellDefault.GetWidth();

    return TRUE;
}
void CGridCtrl::SetUserCB ( CGridCtrlCB* pCB )
{
	m_pUserCB = pCB;
}
void CGridCtrl::SetFullDragAble( BOOL bDragAble )
{
	m_bFullDragAble = bDragAble;
}
BOOL CGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if ( m_TipWnd.IsWindowVisible()
		&& pMsg->message != WM_LBUTTONDOWN )
	{
		m_TipWnd.PreTranslateMessage(pMsg);
	}

	if ( m_bFullDragAble && WM_LBUTTONDOWN == pMsg->message )
	{
		SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
		return TRUE;
	}
	if ( m_bFullDragAble && 
		(WM_LBUTTONDBLCLK == pMsg->message ||
		 WM_NCLBUTTONDBLCLK == pMsg->message ) )
	{
		return TRUE;
	}

	if ( pMsg->message == WM_MOUSEMOVE)
	{
		if (m_bLMouseButtonDown)
		{
			CPoint point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			OnMouseMove(0, point);

			int iHeight = GetFixedRowHeight();
			
			if (point.y < iHeight)
			{
				return TRUE;
			}

		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}
void CGridCtrl::OnMove( int x, int y )
{
	if ( NULL != m_pUserCB )
	{
		m_pUserCB->OnCtrlMove(x,y);
	}
	CWnd::OnMove(x,y);
}

LRESULT CGridCtrl::OnNcHitTest(CPoint point) 
{
	if ( m_bFullDragAble )
	{
		CRect rtClient;
		GetClientRect(&rtClient);
		ClientToScreen(&rtClient);
		return rtClient.PtInRect(point) ? HTCAPTION : CWnd::OnNcHitTest(point);
	}
	else
	{
		return CWnd::OnNcHitTest(point);
	}
}
void CGridCtrl::SetupDefaultCells()
{
    m_cellDefault.SetGrid(this);            // Normal editable cell
    m_cellFixedColDef.SetGrid(this);        // Cell for fixed columns
    m_cellFixedRowDef.SetGrid(this);        // Cell for fixed rows
    m_cellFixedRowColDef.SetGrid(this);     // Cell for area overlapped by fixed columns/rows

    m_cellDefault.SetTextClr(m_crWindowText);   
    m_cellDefault.SetBackClr(m_crWindowColour); 
    m_cellFixedColDef.SetTextClr(m_crWindowText);
    m_cellFixedColDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowDef.SetTextClr(m_crWindowText);
    m_cellFixedRowDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowColDef.SetTextClr(m_crWindowText);
    m_cellFixedRowColDef.SetBackClr(m_cr3DFace);
}

void CGridCtrl::PreSubclassWindow()
{
    CWnd::PreSubclassWindow();

    //HFONT hFont = ::CreateFontIndirect(m_cellDefault.GetFont());
    //OnSetFont((LPARAM)hFont, 0);
    //DeleteObject(hFont);

    Initialise();
}

int CGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(KTimerIdShowTextChange, KTimerPeriodShowTextChange, 0);
	return 0;
}

// Sends a message to the parent in the form of a WM_NOTIFY message with
// a NM_GRIDVIEW structure attached
LRESULT CGridCtrl::SendMessageToParent(int nRow, int nCol, int nMessage, int nParam1, int nParam2) const
{
    if (!IsWindow(m_hWnd))
        return 0;

    NM_GRIDVIEW nmgv;
    nmgv.iRow         = nRow;
    nmgv.iColumn      = nCol;
	nmgv.iParam1	  = nParam1;
	nmgv.iParam2	  = nParam2;
    nmgv.hdr.hwndFrom = m_hWnd;
    nmgv.hdr.idFrom   = GetDlgCtrlID();
    nmgv.hdr.code     = nMessage;

    CWnd *pOwner = GetParent();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
    else
        return 0;
}

// Send a request to the parent to return information on a given cell
LRESULT CGridCtrl::SendDisplayRequestToParent(GV_DISPINFO* pDisplayInfo) const
{
    if (!IsWindow(m_hWnd))
        return 0;

    // Fix up the message headers
    pDisplayInfo->hdr.hwndFrom = m_hWnd;
    pDisplayInfo->hdr.idFrom   = GetDlgCtrlID();
    pDisplayInfo->hdr.code     = GVN_GETDISPINFO;

    // Send the message
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, pDisplayInfo->hdr.idFrom, (LPARAM)pDisplayInfo);
    else
        return 0;
}

// Send a hint to the parent about caching information
LRESULT CGridCtrl::SendCacheHintToParent(const CCellRange& range) const
{
    if (!IsWindow(m_hWnd))
        return 0;

    GV_CACHEHINT CacheHint;

    // Fix up the message headers
    CacheHint.hdr.hwndFrom = m_hWnd;
    CacheHint.hdr.idFrom   = GetDlgCtrlID();
    CacheHint.hdr.code     = GVN_ODCACHEHINT;

    CacheHint.range = range;

    // Send the message
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, CacheHint.hdr.idFrom, (LPARAM)&CacheHint);
    else
        return 0;
}

#define LAYER_SIGNATURE (0x5FD4E64)
int CGridCtrl::GetLayer(int** pLayer) // used to save and restore order of columns
{ //  gives back the size of the area (do not forget to delete pLayer)
	int Length = 2+GetColumnCount()*2;
	int *Layer = new int[Length];	// the caller is supposed to delete it
	Layer[0]= LAYER_SIGNATURE;
	Layer[1]= GetColumnCount();
	memcpyex(&Layer[2], &m_arColOrder[0], GetColumnCount()*sizeof(int));
	memcpyex(&Layer[2+GetColumnCount()], &m_arColWidths[0], GetColumnCount()*sizeof(int));
	*pLayer = Layer;
	return Length;
}
void CGridCtrl::SetLayer(int* pLayer)
{ // coming from a previous GetLayer (ignored if not same number of column, or the same revision number)
	if(pLayer[0] != LAYER_SIGNATURE) return;
	if(pLayer[1] != GetColumnCount()) return;
/*	TRACE("  %d == %d \n",m_arColOrder[0],pLayer[2]);
	TRACE("  %d == %d \n",m_arColOrder[1],pLayer[3]);
	TRACE("  %d == %d \n",m_arColOrder[2],pLayer[4]);
	TRACE("  %d == %d \n",m_arColWidths[0],pLayer[2+3]);
	TRACE("  %d == %d \n",m_arColWidths[1],pLayer[3+3]);
	TRACE("  %d == %d \n",m_arColWidths[2],pLayer[4+3]);
	TRACE("  %d == %d \n",GetColumnCount(),3);

	
	ASSERT(m_arColOrder[0]==pLayer[2]);
	ASSERT(m_arColOrder[1]==pLayer[3]);
	ASSERT(m_arColOrder[2]==pLayer[4]);
	ASSERT(m_arColWidths[0]==pLayer[2+3]);
	ASSERT(m_arColWidths[1]==pLayer[3+3]);
	ASSERT(m_arColWidths[2]==pLayer[4+3]);
	ASSERT(GetColumnCount()==3);
*/	memcpyex(&m_arColOrder[0],&pLayer[2], GetColumnCount()*sizeof(int));
	memcpyex(&m_arColWidths[0],&pLayer[2+GetColumnCount()], GetColumnCount()*sizeof(int));
}

BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
//EFW - Added ON_WM_RBUTTONUP
//{{AFX_MSG_MAP(CGridCtrl)
	ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
	ON_WM_MOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
    ON_WM_TIMER()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
	ON_WM_DESTROY()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_SYSKEYDOWN()
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
    ON_WM_RBUTTONDOWN()	// lcq add 20140117 不知道 _WIN32_WCE 是谁要用的 ，没改变源码，重新写了下
	//}}AFX_MSG_MAP
#ifndef _WIN32_WCE_NO_CURSOR
    ON_WM_SETCURSOR()
#endif
#ifndef _WIN32_WCE
    ON_WM_RBUTTONUP()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_CAPTURECHANGED()
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
#endif
#ifdef _WIN32_WCE
#if (_WIN32_WCE >= 210)
    ON_WM_SETTINGCHANGE()
#endif
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    ON_WM_MOUSEWHEEL()
#endif
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
    ON_MESSAGE(WM_IME_CHAR, OnImeChar)
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_INPLACE_CONTROL, OnEndInPlaceEdit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::OnPaint()
{
	if (!IsWindowVisible() || IsIconic())
	{
		return;
	}
    CPaintDC dc(this);      // device context for painting

    if (m_bDoubleBuffer)    // Use a memory DC to remove flicker
    {
		CRect rtClient;
		GetClientRect(&rtClient);
        CMemDC MemDC(&dc, rtClient);
        OnDraw(&MemDC);
    }
    else                    // Draw raw - this helps in debugging vis problems.
        OnDraw(&dc);
}

BOOL CGridCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE;    // Don't erase the background.
}

// Custom background erasure. This gets called from within the OnDraw function,
// since we will (most likely) be using a memory DC to stop flicker. If we just
// erase the background normally through OnEraseBkgnd, and didn't fill the memDC's
// selected bitmap with colour, then all sorts of vis problems would occur
void CGridCtrl::EraseBkgnd(CDC* pDC)
{
    CRect  VisRect, ClipRect, rect;
    CBrush FixedRowColBack(GetDefaultCell(TRUE, TRUE)->GetBackClr()),
           FixedRowBack(GetDefaultCell(TRUE, FALSE)->GetBackClr()),
           FixedColBack(GetDefaultCell(FALSE, TRUE)->GetBackClr()),
           TextBack(GetDefaultCell(FALSE, FALSE)->GetBackClr());
    CBrush Back(GetGridBkColor()); 

    //CBrush Back(GetTextBkColor());

    if (pDC->GetClipBox(ClipRect) == ERROR)
        return;
    GetVisibleNonFixedCellRange(VisRect);

    int nFixedColumnWidth = GetFixedColumnWidth();
    int nFixedRowHeight = GetFixedRowHeight();

    // Draw Fixed row/column background
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < nFixedRowHeight)
        pDC->FillRect(CRect(ClipRect.left, ClipRect.top, 
                      nFixedColumnWidth, nFixedRowHeight),
                      &FixedRowColBack);

    // Draw Fixed columns background
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < VisRect.bottom)
        pDC->FillRect(CRect(ClipRect.left, ClipRect.top, 
                      nFixedColumnWidth, VisRect.bottom),
                      &FixedColBack);
        
    // Draw Fixed rows background
    if (ClipRect.top < nFixedRowHeight && 
        ClipRect.right > nFixedColumnWidth && ClipRect.left < VisRect.right)
        pDC->FillRect(CRect(nFixedColumnWidth-1, ClipRect.top,
                      VisRect.right, nFixedRowHeight),
                      &FixedRowBack);

    // Draw non-fixed cell background
    if (rect.IntersectRect(VisRect, ClipRect)) 
    {
        CRect CellRect(__max(nFixedColumnWidth, rect.left), 
                       __max(nFixedRowHeight, rect.top),
                       rect.right, rect.bottom);
        pDC->FillRect(CellRect, &TextBack);
    }

    // Draw right hand side of window outside grid
    if (VisRect.right < ClipRect.right) 
        pDC->FillRect(CRect(VisRect.right, ClipRect.top, 
                      ClipRect.right, ClipRect.bottom),
                      &Back);

    // Draw bottom of window below grid
    if (VisRect.bottom < ClipRect.bottom && ClipRect.left < VisRect.right) 
        pDC->FillRect(CRect(ClipRect.left, VisRect.bottom,
                      VisRect.right, ClipRect.bottom),
                      &Back);
}


void CGridCtrl::OnSize(UINT nType, int cx, int cy)
{  
    static BOOL bAlreadyInsideThisProcedure = FALSE;
    if (bAlreadyInsideThisProcedure)
        return;

    if (!::IsWindow(m_hWnd))
        return;

	// This is not the ideal place to register the droptarget
#ifndef GRIDCONTROL_NO_DRAGDROP
	m_DropTarget.Register(this);
#endif

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = TRUE;

    EndEditing();        // destroy any InPlaceEdit's
    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = FALSE;
}

UINT CGridCtrl::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS; // DLGC_WANTALLKEYS; //

    if (m_bHandleTabKey && !IsCTRLpressed())
        nCode |= DLGC_WANTTAB;

    return nCode;
}

#ifndef _WIN32_WCE
// If system colours change, then redo colours
void CGridCtrl::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	if (GetDefaultCell(FALSE, FALSE)->GetTextClr() == m_crWindowText)                   // Still using system colours
		GetDefaultCell(FALSE, FALSE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
	if (GetDefaultCell(FALSE, FALSE)->GetBackClr() == m_crWindowColour)
		GetDefaultCell(FALSE, FALSE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

	if (GetDefaultCell(TRUE, FALSE)->GetTextClr() == m_crWindowText)                   // Still using system colours
		GetDefaultCell(TRUE, FALSE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
	if (GetDefaultCell(TRUE, FALSE)->GetBackClr() == m_crWindowColour)
		GetDefaultCell(TRUE, FALSE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

	if (GetDefaultCell(FALSE, TRUE)->GetTextClr() == m_crWindowText)                   // Still using system colours
		GetDefaultCell(FALSE, TRUE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
	if (GetDefaultCell(FALSE, TRUE)->GetBackClr() == m_crWindowColour)
		GetDefaultCell(FALSE, TRUE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

	if (GetDefaultCell(TRUE, TRUE)->GetTextClr() == m_crWindowText)                   // Still using system colours
		GetDefaultCell(TRUE, TRUE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
	if (GetDefaultCell(TRUE, TRUE)->GetBackClr() == m_crWindowColour)
		GetDefaultCell(TRUE, TRUE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

	//    if (GetGridBkColor() == m_crShadow)
	SetGridBkColor(::GetSysColor(COLOR_3DSHADOW));

	m_crWindowText   = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
	m_cr3DFace       = ::GetSysColor(COLOR_3DFACE);
	m_crShadow       = ::GetSysColor(COLOR_3DSHADOW);
}
#endif

#ifndef _WIN32_WCE_NO_CURSOR
// If we are drag-selecting cells, or drag and dropping, stop now
void CGridCtrl::OnCaptureChanged(CWnd *pWnd)
{
    if (pWnd->GetSafeHwnd() == GetSafeHwnd())
        return;

    // kill timer if active
    if (m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Kill drag and drop if active
    if (m_MouseMode == MOUSE_DRAGGING)
        m_MouseMode = MOUSE_NOTHING;
#endif
}
#endif

#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
// If system settings change, then redo colours
void CGridCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CWnd::OnSettingChange(uFlags, lpszSection);

    if (GetDefaultCell(FALSE, FALSE)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(FALSE, FALSE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(FALSE, FALSE)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(FALSE, FALSE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(TRUE, FALSE)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(TRUE, FALSE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(TRUE, FALSE)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(TRUE, FALSE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(FALSE, TRUE)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(FALSE, TRUE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(FALSE, TRUE)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(FALSE, TRUE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(TRUE, TRUE)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(TRUE, TRUE)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(TRUE, TRUE)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(TRUE, TRUE)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetGridBkColor() == m_crShadow)
        SetGridBkColor(::GetSysColor(COLOR_3DSHADOW));

    m_crWindowText   = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace       = ::GetSysColor(COLOR_3DFACE);
    m_crShadow       = ::GetSysColor(COLOR_3DSHADOW);

    m_nRowsPerWheelNotch = GetMouseScrollLines(); // Get the number of lines
}
#endif

// For drag-selection. Scrolls hidden cells into view
// TODO: decrease timer interval over time to speed up selection over time
void CGridCtrl::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case KTimerIdLButtonDown:
		{
			OnTimerLButtonDown();
		}
		break;
	case KTimerIdShowTextChange:
		{
			if ( m_bEnableBlink )
			{
				if ( !GetVirtualMode() )
				{
					OnTimerShowTextChange();
				}
				else		// 虚模式下不处理blink 
				{
					// - 虚模式下有可能行列太多，需要指定不同的闪烁规则，现在由于报价表等需要闪烁的都不
					//	 是虚模式，只扫描可见单元 xl 0807
					//KillTimer(nIDEvent);
					OnTimerShowTextChange();
				}
			}
			else
			{
				KillTimer(nIDEvent);
			}
		}
		break;
	case KTimerIdShowTips:
		{
			KillTimer(KTimerIdShowTips);
			OnTimerShowTips();
		}
		break;
	}
}

void CGridCtrl::OnTimerLButtonDown()
{
    CPoint pt, origPt;

#ifdef _WIN32_WCE
    if (m_MouseMode == MOUSE_NOTHING)
        return;
    origPt = GetMessagePos();
#else
    if (!GetCursorPos(&origPt))
        return;
#endif

	if ( NULL != m_pUserCB && m_pUserCB->BeBlindSelect() )
	{
		return;
	}

    ScreenToClient(&origPt);

    CRect rect;
    GetClientRect(rect);

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth = GetFixedColumnWidth();

    pt = origPt;
    if (pt.y > rect.bottom)
    {
        //SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        SendMessage(WM_KEYDOWN, VK_DOWN, 0);

        if (pt.x < rect.left)
            pt.x = rect.left;
        if (pt.x > rect.right)
            pt.x = rect.right;
        pt.y = rect.bottom;
        OnSelecting(GetCellFromPt(pt));
    }
    else if (pt.y < nFixedRowHeight)
    {
        //SendMessage(WM_VSCROLL, SB_LINEUP, 0);
        SendMessage(WM_KEYDOWN, VK_UP, 0);

        if (pt.x < rect.left)
            pt.x = rect.left;
        if (pt.x > rect.right)
            pt.x = rect.right;
        pt.y = nFixedRowHeight + 1;
        OnSelecting(GetCellFromPt(pt));
    }

    pt = origPt;
    if (pt.x > rect.right)
    {
        // SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        SendMessage(WM_KEYDOWN, VK_RIGHT, 0);

        if (pt.y < rect.top)
            pt.y = rect.top;
        if (pt.y > rect.bottom)
            pt.y = rect.bottom;
        pt.x = rect.right;
        OnSelecting(GetCellFromPt(pt));
    }
    else if (pt.x < nFixedColWidth)
    {
        //SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
        SendMessage(WM_KEYDOWN, VK_LEFT, 0);

        if (pt.y < rect.top)
            pt.y = rect.top;
        if (pt.y > rect.bottom)
            pt.y = rect.bottom;
        pt.x = nFixedColWidth + 1;
        OnSelecting(GetCellFromPt(pt));
    }
}

void CGridCtrl::OnTimerShowTextChange()
{
	ASSERT( m_bEnableBlink );
	// 是否只扫描可见单元格就可以了，不可见单元格让它保持闪烁状态，当它可见的时候闪烁。。。
	if ( GetVirtualMode() )	// 虚模式只扫描可见单元
	{
		CCellRange cellRange = GetVisibleNonFixedCellRange();
		for ( int iRow = cellRange.GetMinRow(); iRow < cellRange.GetMaxRow() ; iRow++ )
		{
			for ( int iCol = cellRange.GetMinCol(); iCol < cellRange.GetMaxCol() ; iCol++ )
			{
				if (GetItemState(iRow, iCol) & GVIS_BLINK)
				{
					CGridCellBase *pCell = GetCell(iRow, iCol);
					if (NULL != pCell)
					{
						if (pCell->m_iBlinkStep > 0)
						{
							pCell->m_iBlinkStep--;
							
							if (pCell->m_iBlinkStep > BLINK_STEP_COUNT)
							{
								pCell->m_iBlinkStep = BLINK_STEP_COUNT;
							}
							
							InvalidateCellRect(iRow, iCol);
						}
					}
				}
			}
		}
	}
	else	// 原来的
	{
		for (int nRow = 0; nRow < GetRowCount(); nRow++)
		{
			for (int nCol = 0; nCol < GetColumnCount(); nCol++)
			{
				if (GetItemState(nRow, nCol) & GVIS_BLINK)
				{
					CGridCellBase *pCell = GetCell(nRow, nCol);
					if (NULL != pCell)
					{
						if (pCell->m_iBlinkStep > 0)
						{
							pCell->m_iBlinkStep--;
							
							if (pCell->m_iBlinkStep > BLINK_STEP_COUNT)
							{
								pCell->m_iBlinkStep = BLINK_STEP_COUNT;
							}
							
							InvalidateCellRect(nRow, nCol);
						}
					}
				}
			}
		}
	}
	

	SetTimer(KTimerIdShowTextChange, KTimerPeriodShowTextChange, 0);
}

void CGridCtrl::OnTimerShowTips()
{
#ifndef GRIDCONTROL_NO_TITLETIPS

	CGridCellBase* pCell = NULL;
	CCellID idCurrentCell;
	idCurrentCell = GetCellFromPt(m_LastMousePoint);
	pCell = GetCell(idCurrentCell.row, idCurrentCell.col);

	m_bTitleTips = TRUE;
	CRect rtClient;
	GetClientRect(&rtClient);
	int32 iSensitiveWidth = 10;
	int32 iSensitiveHeight = 10;
	if ( pCell && pCell->GetGrid()->GetColumnCount() > 0 && pCell->GetGrid()->GetRowCount() > 0)
	{
		int32 iWidth   = pCell->GetGrid()->GetColumnWidth(0);
		iSensitiveWidth = iWidth/5;

		int32 iHeight = pCell->GetGrid()->GetRowHeight(0);
		iSensitiveHeight = iHeight/5;
	}			
	if (m_LastMousePoint.x <= (rtClient.left+iSensitiveWidth) || m_LastMousePoint.x >= (rtClient.right-iSensitiveWidth) || m_LastMousePoint.y <= (rtClient.top+iSensitiveHeight) || m_LastMousePoint.y >= (rtClient.bottom - iSensitiveHeight))
	{
		m_bTitleTips = false;
		m_TipWnd.Hide();
	}
	else
	{
		m_bTitleTips = true;
	}
	if (m_bTitleTips)
	{
		CRect TextRect, CellRect;
		if (pCell)
		{
			if (/*GetItemState(idCurrentCell.row, idCurrentCell.col)*/pCell->GetState() & GVIS_SHOWTIPS)
			{						
				LPCTSTR szTipText = pCell->GetTipText(); 
				if (!m_bRMouseButtonDown
					&& szTipText && szTipText[0]
				&& !pCell->IsEditing()
					&& GetCellRect( idCurrentCell.row, idCurrentCell.col, &TextRect)
					&& pCell->GetTipTextRect( &TextRect)
					&& GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect) 
					)
				{
					m_TipWnd.Show(m_LastMousePoint, pCell->GetTipText(), pCell->GetTipTitle());
				}										
			}
			else
			{
				m_TipWnd.Hide();						
			}
		}
	}

#endif
}

// move about with keyboard
void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

#ifndef GRIDCONTROL_NO_TITLETIPS
	m_TipWnd.Hide();
#endif

	if ( !IsValid(m_idCurrentCell) )
	{
		m_idCurrentCell = GetTopleftNonFixedCell(FALSE);	// 无效点默认设置为左上角点
	}
    if (!IsValid(m_idCurrentCell))
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }
    CCellID next = m_idCurrentCell;
    BOOL bChangeLine = FALSE;
	BOOL bHorzScrollAction = FALSE;
	BOOL bVertScrollAction = FALSE;

    if (IsCTRLpressed())
    {
        switch (nChar)
        {
        case 'A':
            OnEditSelectAll();
            break;
        case 'k':		// This is ctrl+ on french keyboard, may need to be better processed for other locales
            AutoSizeColumns();
			Invalidate();
            break;
#ifndef GRIDCONTROL_NO_CLIPBOARD
        case 'X':
            OnEditCut();
            break;
        case VK_INSERT:
        case 'C':
            OnEditCopy();
            break;
        case 'V':
            OnEditPaste();
            break;
#endif
        }
    }

#ifndef GRIDCONTROL_NO_CLIPBOARD
    if (IsSHIFTpressed() &&(nChar == VK_INSERT))
        OnEditPaste();
#endif

    BOOL bFoundVisible = TRUE;	// 默认有对应表格
    int iOrig;

    if (nChar == VK_DELETE)
    {
		CutSelectedText();
    }
    else if (nChar == VK_DOWN)
    {
        // don't let user go to a hidden row
        bFoundVisible = FALSE;
        iOrig = next.row;
        next.row++;
        while( next.row < GetRowCount())
        {
            if( GetRowHeight( next.row) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            next.row++;
        }

		// xl 0416 与ondraw一致的显示可见判断
		if ( bFoundVisible && !IsVisibleVScroll() )
		{
			CCellRange rangeVis = GetVisibleNonFixedCellRange();	// 与显示一致的调用
			if ( !rangeVis.InRange(next) && !IsCellVisible(next) )
			{
				next.row = iOrig;	// 没有非隐藏行或者不允许滚动到不可见行
			}
		}
		else if ( !bFoundVisible )
		{
			next.row = iOrig;
		}
//         if( !bFoundVisible
// 			|| (!IsCellVisible(next)&& !IsVisibleVScroll()) )	// 没有非隐藏行或者不允许滚动到不可见行
//             next.row = iOrig;

		if ( next == m_idCurrentCell )
		{
			SendMessageToParent(next.row, next.col, (int)GVN_KEYDOWNEND);
		}
    }
    else if (nChar == VK_UP)
    {
        // don't let user go to a hidden row
        bFoundVisible = FALSE;
        iOrig = next.row;
        next.row--;
        while( next.row >= m_nFixedRows)
        {
            if( GetRowHeight( next.row) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            next.row--;
        }
		// xl 0416 与ondraw一致的显示可见判断
		if ( bFoundVisible && !IsVisibleVScroll() )
		{
			CCellRange rangeVis = GetVisibleNonFixedCellRange();	// 与显示一致的调用
			if ( !rangeVis.InRange(next) && !IsCellVisible(next) )
			{
				next.row = iOrig;	// 没有非隐藏行或者不允许滚动到不可见行
			}
		}
		else if ( !bFoundVisible )
		{
			next.row = iOrig;
		}

//         if( !bFoundVisible
// 			|| (!IsCellVisible(next)&&!IsVisibleVScroll()) ) // 没有非隐藏行或者不允许滚动到不可见行
//             next.row = iOrig;

		if ( next == m_idCurrentCell )
		{
			SendMessageToParent(next.row, next.col, (int)GVN_KEYUPEND);
		}
    }
    else if (nChar == VK_RIGHT || (nChar == VK_TAB && !IsSHIFTpressed()) )
    {
		if( (nChar == VK_TAB) &&  m_QuitFocusOnTab )
		{
			CDialog* p= (CDialog*) GetParent();
			if(p) p->NextDlgCtrl();
			return;

		}
        // don't let user go to a hidden column
        bFoundVisible = FALSE;
        iOrig = next.col;
		/*next.col++ ;*/
		//--- wangyongxue 2016/06/14 优化->右移操作
		for (int i=iOrig; i<m_nCols; i++)
		{
			if(!IsCellVisible(next.row, i))
			{
				next.col = i;
				break;
			}
			else
			{
				next.col++ ;
			}
		}
// 		if (next.col >= m_nCols)
// 		{
// 			m_pXSBHorz->SetScrollPos(m_nHScrollMax);
// 		}
        if (nChar == VK_TAB)
        {
			// If we're at the end of a row, go down a row till we find a non-hidden row
            if (next.col == (GetColumnCount()) && next.row < (GetRowCount() - 1))
            {
				int iRowOld = next.row;
				next.row += 2;
				while( next.row < GetRowCount())
				{
					if( GetRowHeight(next.row) > 0)
					{
						bFoundVisible = TRUE;
						break;
					}
					next.row++; 
				}
				if ( !IsCellVisible(next.row, iOrig) && !IsVisibleVScroll() )
				{
					// 不允许 不能滚动到的不可见行
					next.row = iRowOld;
				}
				else
				{
					next.col = m_nFixedCols;	// Place focus on first non-fixed column
					bChangeLine = TRUE;
				}
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        }

		// We're on a non-hidden row, so look across for the next non-hidden column
        while( next.col < GetColumnCount())
        {
            if( GetColumnWidth( next.col) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            next.col++;
        }

		// If nothing worked then don't bother
        if( !bFoundVisible
			|| (!IsCellVisible(next)&&!IsVisibleHScroll()) ) // 找不到非隐藏行或者不允许水平滚动
            next.col = iOrig;
    }
    else if (nChar == VK_LEFT || (nChar == VK_TAB && IsSHIFTpressed()) )
    {
        // don't let user go to a hidden column
        bFoundVisible = FALSE;
        iOrig = next.col;
		/*next.col--;*/
		//--- wangyongxue 2016/06/14 优化<-左移操作
		for (int i=m_idCurrentCell.col; i>=2; i--)
		{
			if(!IsCellVisible(next.row, i))
			{
				next.col = i;
				break;
			}
			else
			{
				next.col-- ;
			}
		}

        if (nChar == VK_TAB)
        {
            if (next.col == (GetFixedColumnCount()-1) && next.row > GetFixedRowCount())
            {
				int iRowOld = next.row;
                next.row--;
				while( next.row > GetFixedRowCount())
				{
					if( GetRowHeight(next.row) > 0)
					{
						bFoundVisible = TRUE;
						break;
					}
					next.row--; 
				}
				
				if ( !IsCellVisible(next.row, iOrig) && !IsVisibleVScroll() )
				{
					next.row = iRowOld;	// 不允许滚动到不能显示的行
				}
				else
				{
					next.col = GetColumnCount() - 1; 
					bChangeLine = TRUE;
				}
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        }

        while( next.col >= m_nFixedCols)
        {
            if( GetColumnWidth( next.col) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            next.col--;
        }
        if( !bFoundVisible
			|| (!IsCellVisible(next)&&!IsVisibleHScroll()) ) // 找不到非隐藏行或者不允许水平滚动
            next.col = iOrig;
    }
    else if (nChar == VK_NEXT)
    {
        CCellID idOldTopLeft = GetTopleftNonFixedCell();
        SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
		bVertScrollAction = TRUE;
        CCellID idNewTopLeft = GetTopleftNonFixedCell();

        int increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment)
        {
            next.row += increment;
            if (next.row >(GetRowCount() - 1))
                next.row = GetRowCount() - 1;
        }
        else
		{
			if ( IsCellVisible(GetRowCount()-1, next.col) || IsVisibleVScroll() )
			{
				next.row = GetRowCount() - 1;	// 允许垂直滚动则取最后一行
			}
			else
			{
				// 最后一行不可见且不允许垂直滚动
				CRect rc;
				GetClientRect(&rc);
				int iHeight = GetFixedRowHeight();
				int iLastVisibleRow = -1;
				for ( int iRow=GetFixedRowCount(); iRow < GetRowCount()-1 ; iRow++ )
				{
					int i = GetRowHeight(iRow);
					iHeight += i;
					if ( iHeight >= rc.bottom )
					{
						break;
					}
					if ( i > 0 )
					{
						iLastVisibleRow = iRow;
					}
				}
				if ( iLastVisibleRow > next.row )
				{
					next.row = iLastVisibleRow;	// 最后一个可见行
				}
			}
		}
    }
    else if (nChar == VK_PRIOR)
    {
        CCellID idOldTopLeft = GetTopleftNonFixedCell();
        SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
		bVertScrollAction = TRUE;
        CCellID idNewTopLeft = GetTopleftNonFixedCell();
            
        int increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment) 
        {
            next.row += increment;
            if (next.row < m_nFixedRows) 
                next.row = m_nFixedRows;
        }
        else
            next.row = m_nFixedRows;
    }
    else if (nChar == VK_HOME)
    {
        // Home and Ctrl-Home work more like Excel
        //  and don't let user go to a hidden cell
        if (IsCTRLpressed())
        {
            SendMessage(WM_VSCROLL, SB_TOP, 0);
            SendMessage(WM_HSCROLL, SB_LEFT, 0);
			bVertScrollAction = TRUE;
			bHorzScrollAction = TRUE;
            next.row = m_nFixedRows;
            next.col = m_nFixedCols;
        }
        else
        {
            SendMessage(WM_HSCROLL, SB_LEFT, 0);
 			bHorzScrollAction = TRUE;
            next.col = m_nFixedCols;
        }
        // adjust column to avoid hidden columns and rows
        while( next.col < GetColumnCount() - 1)
        {
            if( GetColumnWidth( next.col) > 0)
                break;
            next.col++;
        }
        while( next.row < GetRowCount() - 1)
        {
            if( GetRowHeight( next.row) > 0)
                break;
            next.row++;
        }
    }
    else if (nChar == VK_END)
    {
        // End and Ctrl-End work more like Excel
        //  and don't let user go to a hidden cell
        if (IsCTRLpressed())
        {
            SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
			bHorzScrollAction = TRUE;
			bVertScrollAction = TRUE;
            next.row = GetRowCount() - 1;
            next.col = GetColumnCount() - 1;
        }
        else
        {
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
			bHorzScrollAction = TRUE;
            next.col = GetColumnCount() - 1;
        }
        // adjust column to avoid hidden columns and rows
		BOOL bEnableV = IsVisibleVScroll();
		BOOL bEnableH = IsVisibleHScroll();
		if ( bEnableH )
		{
			while( next.col > m_nFixedCols + 1)
			{
				if( GetColumnWidth( next.col) > 0)
					break;
				next.col--;
			}
		}
		else
		{
			// 不能水平滚动, 找到最后的可见列
			int iLastCol = -1;
			int iWidth = GetFixedColumnWidth();
			CRect rc;
			GetClientRect(rc);
			for ( int i=GetFixedColumnCount(); i < GetColumnCount() ; i++ )
			{
				int iColW = GetColumnWidth(i);
				iWidth += iColW;
				if ( iWidth >= rc.right )
				{
					break;
				}
				if ( iColW > 0 )
				{
					iLastCol = i;
				}
			}
			if ( iLastCol > m_idCurrentCell.col )
			{
				next.col = iLastCol;
			}
			else
			{
				ASSERT( next.col == m_idCurrentCell.col );	// 应当是最后一列了
				next.col = m_idCurrentCell.col;	// 只能还原了
			}
		}
        
		if ( bEnableV )
		{
			while( next.row > m_nFixedRows + 1)
			{
				if( GetRowHeight( next.row) > 0)
					break;
				next.row--;
			}
		}
		else
		{
			// 不能垂直滚动, 找到最后的可见行
			CRect rc;
			GetClientRect(&rc);
			int iHeight = GetFixedRowHeight();
			int iLastVisibleRow = -1;
			for ( int iRow=GetFixedRowCount(); iRow < GetRowCount() ; iRow++ )
			{
				int i = GetRowHeight(iRow);
				iHeight += i;
				if ( iHeight >= rc.bottom )
				{
					break;
				}
				if ( i > 0 )
				{
					iLastVisibleRow = iRow;
				}
			}
			if ( iLastVisibleRow > m_idCurrentCell.row )
			{
				next.row = iLastVisibleRow;	// 最后一个可见行
			}
			else
			{
				ASSERT( iLastVisibleRow == m_idCurrentCell.row );	// 此时应该是处于最后一行了
				iLastVisibleRow = m_idCurrentCell.row;
			}
		}
    }
    else if (nChar == VK_F2)
    {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), VK_LBUTTON);
    }
    else
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    if (/*next != m_idCurrentCell && */!m_bBlindKeyFocusCell && IsValid(next))
    {
        // While moving with the Cursorkeys the current ROW/CELL will get selected
        // OR Selection will get expanded when SHIFT is pressed
        // Cut n paste from OnLButtonDown - Franco Bez
        // Added check for NULL mouse mode - Chris Maunder.
        if (m_MouseMode == MOUSE_NOTHING)
        {
            m_PrevSelectedCellMap.RemoveAll();
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            if (!IsSHIFTpressed() || nChar == VK_TAB)
                m_SelectionStartCell = next;

			// Notify parent that selection is changing - Arthur Westerman/Scot Brennecke 
            SendMessageToParent(next.row, next.col, (int)GVN_SELCHANGING);
			OnSelecting(next);
			SendMessageToParent(next.row, next.col, (int)GVN_SELCHANGED);

            m_MouseMode = MOUSE_NOTHING;
        }

        SetFocusCell(next);

        if (!IsCellVisible(next))
        {

            switch (nChar)
            {
            case VK_RIGHT:  
                SendMessage(WM_HSCROLL, SB_LINERIGHT, 0); 
				bHorzScrollAction = TRUE;
                break;
                
            case VK_LEFT:   
                SendMessage(WM_HSCROLL, SB_LINELEFT, 0);  
				bHorzScrollAction = TRUE;
                break;
                
            case VK_DOWN:   
                SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  
				bVertScrollAction = TRUE;
                break;
                
            case VK_UP:     
                SendMessage(WM_VSCROLL, SB_LINEUP, 0);    
				bVertScrollAction = TRUE;
                break;                
                
            case VK_TAB:    
                if (IsSHIFTpressed())
                {
                    if (bChangeLine) 
                    {
                        SendMessage(WM_VSCROLL, SB_LINEUP, 0);
						bVertScrollAction = TRUE;
                        SetScrollPos32(SB_HORZ, m_nHScrollMax);
                        break;
                    }
                    else 
					{
                        SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
						bHorzScrollAction = TRUE;
					}
                }
                else
                {
                    if (bChangeLine) 
                    {
                        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
						bVertScrollAction = TRUE;
                        SetScrollPos32(SB_HORZ, 0);
                        break;
                    }
                    else 
					{
						SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
						bHorzScrollAction = TRUE;
					}
                }
                break;
            }
            EnsureVisible(next); // Make sure cell is visible
            Invalidate();
        }
        EnsureVisible(next); // Make sure cell is visible

		if (bHorzScrollAction)
			SendMessage(WM_HSCROLL, SB_ENDSCROLL, 0);
		if (bVertScrollAction)
			SendMessage(WM_VSCROLL, SB_ENDSCROLL, 0);
    }

	if ( m_bBlindKeyFocusCell )
	{
		m_bBlindKeyFocusCell = !m_bBlindKeyFocusCell;
	}
}

void CGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGridCtrl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef GRIDCONTROL_USE_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
	m_TipWnd.Hide();
#endif

    CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

// Instant editing of cells when keys are pressed
void CGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // EFW - BUG FIX
    if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && nChar != VK_ESCAPE)
    {
		CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
		if (!pApp->m_bBindHotkey)
		{
			if (!m_bHandleTabKey || (m_bHandleTabKey && nChar != VK_TAB))
				OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), nChar);			
		}
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

// Added by KiteFly
LRESULT CGridCtrl::OnImeChar(WPARAM wCharCode, LPARAM)
{
    // EFW - BUG FIX
    if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && wCharCode != VK_ESCAPE) 
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), (UINT)wCharCode);
    return 0;
}

// Callback from any CInPlaceEdits that ended. This just calls OnEndEditCell,
// refreshes the edited cell and moves onto next cell if the return character
// from the edit says we should.
void CGridCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
    GV_ITEM     *pgvItem = &pgvDispInfo->item;

    // In case OnEndInPlaceEdit called as window is being destroyed
    if (!IsWindow(GetSafeHwnd()))
        return;

    OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->strText);
    //InvalidateCellRect(CCellID(pgvItem->row, pgvItem->col));

    switch (pgvItem->lParam)
    {
    case VK_TAB:
    case VK_DOWN:
    case VK_UP:
    case VK_RIGHT:
    case VK_LEFT:
    case VK_NEXT:
    case VK_PRIOR:
    case VK_HOME:
    case VK_END:
        OnKeyDown((UINT)pgvItem->lParam, 0, 0);
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), (UINT)pgvItem->lParam);
    }

    *pResult = 0;
}

// Handle horz scrollbar notifications
void CGridCtrl::OnHScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEditing();

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
	m_TipWnd.Hide();
#endif

    int scrollPos = GetScrollPos32(SB_HORZ);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
    case SB_LINERIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            // may have contiguous hidden columns.  Blow by them
            while (idTopLeft.col < (GetColumnCount()-1)
                    && GetColumnWidth( idTopLeft.col) < 1 )
            {
                idTopLeft.col++;
            }
            int xScroll = GetColumnWidth(idTopLeft.col);
            SetScrollPos32(SB_HORZ, scrollPos + xScroll);
            if (GetScrollPos32(SB_HORZ) == scrollPos)
                break;          // didn't work

            rect.left = GetFixedColumnWidth();
            //rect.left = GetFixedColumnWidth() + xScroll;
            //ScrollWindow(-xScroll, 0, rect);
            //rect.left = rect.right - xScroll;
            InvalidateRect(rect);
        }
        break;

    case SB_LINELEFT:
        if (scrollPos > 0 && idTopLeft.col > GetFixedColumnCount())
        {
            int iColToUse = idTopLeft.col-1;
            // may have contiguous hidden columns.  Blow by them
            while(  iColToUse > GetFixedColumnCount()
                    && GetColumnWidth( iColToUse) < 1 )
            {
                iColToUse--;
            }

            int xScroll = GetColumnWidth(iColToUse);
            SetScrollPos32(SB_HORZ, __max(0, scrollPos - xScroll));
            rect.left = GetFixedColumnWidth();
            //ScrollWindow(xScroll, 0, rect);
            //rect.right = rect.left + xScroll;
            InvalidateRect(rect);
        }
        break;

    case SB_PAGERIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            rect.left = GetFixedColumnWidth();
            int offset = rect.Width();
            int pos = min(m_nHScrollMax, scrollPos + offset);
            SetScrollPos32(SB_HORZ, pos);
            rect.left = GetFixedColumnWidth();
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGELEFT:
        if (scrollPos > 0)
        {
            rect.left = GetFixedColumnWidth();
            int offset = -rect.Width();
            int pos = __max(0, scrollPos + offset);
            SetScrollPos32(SB_HORZ, pos);
            rect.left = GetFixedColumnWidth();
            InvalidateRect(rect);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            SetScrollPos32(SB_HORZ, GetScrollPos32(SB_HORZ, TRUE));
            m_idTopLeftCell.row = -1;
            CCellID idNewTopLeft = GetTopleftNonFixedCell();
			// modify by tangad 内容不足一版面不刷新页面
			CRect rectWidth;
			GetClientRect(rectWidth);
            if (idNewTopLeft != idTopLeft && GetVirtualWidth() >= rectWidth.Width())
            {
                rect.left = GetFixedColumnWidth();
                InvalidateRect(rect);
            }
        }
        break;
        
    case SB_LEFT:
        if (scrollPos > 0)
        {
            SetScrollPos32(SB_HORZ, 0);
            Invalidate();
        }
        break;
        
    case SB_RIGHT:
        if (scrollPos < m_nHScrollMax)
        {
            SetScrollPos32(SB_HORZ, m_nHScrollMax);
            Invalidate();
        }
        break;        
        
    default: 
        break;
    }
	if ( NULL != m_pUserCB )
	{
		m_pUserCB->OnHScrollEnd();
	}
}

// Handle vert scrollbar notifications
void CGridCtrl::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEditing();

#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
	m_TipWnd.Hide();
#endif

    // Get the scroll position ourselves to ensure we get a 32 bit value
    int scrollPos = GetScrollPos32(SB_VERT);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
    case SB_LINEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
            // may have contiguous hidden rows.  Blow by them
            while(  idTopLeft.row < (GetRowCount()-1)
                    && GetRowHeight( idTopLeft.row) < 1 )
            {
                idTopLeft.row++;
            }

            int yScroll = GetRowHeight(idTopLeft.row);
            SetScrollPos32(SB_VERT, scrollPos + yScroll);
            if (GetScrollPos32(SB_VERT) == scrollPos)
                break;          // didn't work

            rect.top = GetFixedRowHeight();
            //rect.top = GetFixedRowHeight() + yScroll;
            //ScrollWindow(0, -yScroll, rect);
            //rect.top = rect.bottom - yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_LINEUP:
        if (scrollPos > 0 && idTopLeft.row > GetFixedRowCount())
        {
            int iRowToUse = idTopLeft.row-1;
            // may have contiguous hidden rows.  Blow by them
            while(  iRowToUse > GetFixedRowCount()
                    && GetRowHeight( iRowToUse) < 1 )
            {
                iRowToUse--;
            }

            int yScroll = GetRowHeight( iRowToUse);
            SetScrollPos32(SB_VERT, __max(0, scrollPos - yScroll));
            rect.top = GetFixedRowHeight();
            //ScrollWindow(0, yScroll, rect);
            //rect.bottom = rect.top + yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
            rect.top = GetFixedRowHeight();
			int iScrollPosTmp = scrollPos + rect.Height();
            scrollPos = min(m_nVScrollMax, iScrollPosTmp);
            SetScrollPos32(SB_VERT, scrollPos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEUP:
        if (scrollPos > 0)
        {
            rect.top = GetFixedRowHeight();
            int offset = -rect.Height();
            int pos = __max(0, scrollPos + offset);
            SetScrollPos32(SB_VERT, pos);
            rect.top = GetFixedRowHeight();
            InvalidateRect(rect);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            SetScrollPos32(SB_VERT, GetScrollPos32(SB_VERT, TRUE));
            m_idTopLeftCell.row = -1;
            CCellID idNewTopLeft = GetTopleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            {
                rect.top = GetFixedRowHeight();
                InvalidateRect(rect);
            }
        }
        break;
        
    case SB_TOP:
        if (scrollPos > 0)
        {
            SetScrollPos32(SB_VERT, 0);
            Invalidate();
        }
        break;
        
    case SB_BOTTOM:
        if (scrollPos < m_nVScrollMax)
        {
            SetScrollPos32(SB_VERT, m_nVScrollMax);
            Invalidate();
        }
		break;
    default: 
        break;
    }
	if ( NULL != m_pUserCB )
	{
		m_pUserCB->OnVScrollEnd();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl implementation functions
// void CGridCtrl::SetBaseLines( CArray<int32,int32>& rows )
// {
// 	m_aBaseLines.RemoveAll();
// 	m_aBaseLines.Copy(rows);
// }
// void CGridCtrl::SetBaseLineColor( COLORREF color )
// {
// 	m_clrBaseLine = color;
//}
void CGridCtrl::SetBorderColor(COLORREF clr, BOOL OnlyTopLeft)
{
	m_clrBorder = clr;
	m_bOnlyDrawTopLeft = OnlyTopLeft;
}
void CGridCtrl::OnDraw(CDC* pDC)
{	
    if (!m_bAllowDraw)
        return;

    CRect clipRect;
    if (pDC->GetClipBox(&clipRect) == ERROR)
        return;

    EraseBkgnd(pDC);            // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.
#ifdef _DEBUG
	LARGE_INTEGER iStartCount;
	QueryPerformanceCounter(&iStartCount);
#endif

    CRect rect;
    int row, col;
    CGridCellBase* pCell;

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth  = GetFixedColumnWidth();

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();

    if (GetVirtualMode())
        SendCacheHintToParent(VisCellRange);
	
    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;
        rect.right = -1;

        for (col = 0; col < m_nFixedCols; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            pCell = GetCell(row, col);
            if (pCell)
			{
				pCell->SetCoords(row,col);
				//////////////////////////////////////////////////////////////////////////
				// fangz0714  原pCell->Draw(pDC, row, col, rect, FALSE);
				// 使固定列与普通列没有区别
                pCell->Draw(pDC, row, col, rect, TRUE);
			}
        }
    }

    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = -1;
        for (col = 0; col < m_nFixedCols; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;            // gone past cliprect
            if (rect.right < clipRect.left)
                continue;         // Reached cliprect yet?

            pCell = GetCell(row, col);
            if (pCell)
			{
				pCell->SetCoords(row,col);
				//////////////////////////////////////////////////////////////////////////
				// fangz0714  原pCell->Draw(pDC, row, col, rect, FALSE);
				// 使固定列与普通列没有区别
                pCell->Draw(pDC, row, col, rect, TRUE);
			}
        }
    }

    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            if (pCell)
			{
				pCell->SetCoords(row,col);
                pCell->Draw(pDC, row, col, rect, true);
			}
        }
    }

    // draw rest of non-fixed cells
    rect.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            // TRACE(_T("Cell %d,%d type: %s\n"), row, col, pCell->GetRuntimeClass()->m_lpszClassName);
            if (pCell)
			{
				pCell->SetCoords(row,col);
                pCell->Draw(pDC, row, col, rect, TRUE);
			}
        }
    }

	// fangz 0904 # 表格线颜色( 正常显示就显示表格色,不显示表格线的时候,就用背景色)
	
	CIoViewBase * pIoViewParent = NULL; 
	
	CWnd * pParent = GetParent();
	
	pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);
// 	CRuntimeClass * pRunTime = pParent->GetRuntimeClass();
// 	
// 	for ( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
// 	{
// 		if ( pRunTime == CIoViewManager::GetIoViewObject(i)->m_pIoViewClass)
// 		{
// 			pIoViewParent = (CIoViewBase*)pParent;
// 			break;
// 		}
// 	}
	
	if( NULL != pIoViewParent )
	{
		m_crGridLineColour = pIoViewParent->GetIoViewColor(ESCGridLine);
	}	
	else
	{
		m_crGridLineColour = CFaceScheme::Instance()->GetSysColor(ESCGridLine);		
	}
	
	CPen pen;	
	pen.CreatePen(PS_SOLID, 0, m_crGridLineColour);
	pDC->SelectObject(&pen);
	
// 	// draw vertical lines (drawn at ends of cells)
 	if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
 	{
 		// 非固定项
 		int x = nFixedColWidth;
 		for (col = minVisibleCol; col <= maxVisibleCol; col++)
 		{
 			if (GetColumnWidth(col) <= 0) continue;
 			
 			x += GetColumnWidth(col);
 			pDC->MoveTo(x-1, nFixedRowHeight);
 			pDC->LineTo(x-1, VisRect.bottom);
 		}
 	}
 	
 	// draw horizontal lines (drawn at bottom of each cell)
 	if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
 	{
 		int y = nFixedRowHeight;
 		for (row = minVisibleRow; row <= maxVisibleRow; row++)
 		{
 			if (GetRowHeight(row) <= 0) continue;
 			
 			y += GetRowHeight(row);
 			pDC->MoveTo(nFixedColWidth, y-1);
 			pDC->LineTo(VisRect.right,  y-1);
 		}
 	}

/*
 	if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
 	{
 		int x = 0;
		
		// 最左边一根
		pDC->MoveTo(x, 0);
 		pDC->LineTo(x, VisRect.bottom);
 		
		for (col = 0; col <= maxVisibleCol; col++)
 		{
 			if (GetColumnWidth(col) <= 0) continue;
 	
			x += GetColumnWidth(col);			
 			pDC->MoveTo(x-1, 0);
 			pDC->LineTo(x-1, VisRect.bottom);
 		}
 	}
	
	// draw horizontal lines (drawn at bottom of each cell)
	if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
	{
		int y = 0;
		
		pDC->MoveTo(0, y);
		pDC->LineTo(VisRect.right, y);
		
		for (row = 0; row <= maxVisibleRow; row++)
		{
			if (GetRowHeight(row) <= 0) continue;
			
			y += GetRowHeight(row);
			pDC->MoveTo(0, y-1);
			pDC->LineTo(VisRect.right,  y-1);
		}
	}
*/

// 		// 每个cell 的高度不是完全一样的,不能用height * row 得到总高度;
//  	if (m_aBaseLines.GetSize()>0)
//  	{		
//  		int32 i;
//  		CRect rectGrid;
//  		int32 rowheight = 0 ;
//  		GetClientRect(&rectGrid);
//  		for ( i = 0; i < m_aBaseLines.GetSize(); i ++ )
//  		{
//  			CPen *penOld,penRed;
//  			penRed.CreatePen(PS_SOLID,1,m_clrBaseLine);
//  			penOld = pDC->SelectObject(&penRed);
//  			if (GetRowHeight(m_aBaseLines[i]>0))
//  			{
//  				rowheight = GetRowHeight(m_aBaseLines[i]);
//  			}
//  			int32 y = nFixedRowHeight;
// 
// 			for (int32 j=0;j<m_aBaseLines[i];j++)
// 			{
// 				y += GetRowHeight(j);
// 			}
//  			pDC->MoveTo(nFixedColWidth,y-1);
//  			pDC->LineTo(VisRect.right,y-1);
//  			pDC->SelectObject(penOld);
//  			penRed.DeleteObject();
//  		}
//  	}
    pDC->SelectStockObject(NULL_PEN);

	if (m_clrBorder !=  0x123456)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		CPen * pOldpen,penBoard;
		penBoard.CreatePen(PS_SOLID,1,m_clrBorder);
		pOldpen = pDC->SelectObject(&penBoard);
		if (m_bOnlyDrawTopLeft)
		{
			BOOL bHorz = IsVisibleHScroll();
			BOOL bVert = IsVisibleVScroll();
			if (bHorz && bVert)
			{
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.left, rectClient.bottom);
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.right, rectClient.top);
			}
			else if (bHorz && !bVert)
			{
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.left, rectClient.bottom);
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.right, rectClient.top);
				pDC->MoveTo(rectClient.right-1, rectClient.top);
				pDC->LineTo(rectClient.right-1, rectClient.bottom);
			}
			else if (!bHorz && bVert)
			{
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.left, rectClient.bottom);
				pDC->MoveTo(rectClient.left, rectClient.top);
				pDC->LineTo(rectClient.right, rectClient.top);
				pDC->MoveTo(rectClient.left, rectClient.bottom-1);
				pDC->LineTo(rectClient.right, rectClient.bottom-1);
			}
			else
			{
				pDC->MoveTo(rectClient.TopLeft());
				pDC->LineTo(rectClient.right-1,rectClient.top);
				pDC->LineTo(rectClient.right-1,rectClient.bottom-1);
				pDC->LineTo(rectClient.left,rectClient.bottom-1);
				pDC->LineTo(rectClient.left,rectClient.top);
			}
		}
		else
		{
			pDC->MoveTo(rectClient.TopLeft());
			pDC->LineTo(rectClient.right-1,rectClient.top);
			pDC->LineTo(rectClient.right-1,rectClient.bottom-1);
			pDC->LineTo(rectClient.left,rectClient.bottom-1);
			pDC->LineTo(rectClient.left,rectClient.top);
		}
		pDC->SelectObject(pOldpen);
		penBoard.DeleteObject();
				
	}
    // Let parent know it can discard it's data if it needs to.
    if (GetVirtualMode())
       SendCacheHintToParent(CCellRange(-1,-1,-1,-1));

#ifdef _DEBUG
	LARGE_INTEGER iEndCount;
	QueryPerformanceCounter(&iEndCount);
	// TRACE1("Draw counter ticks: %d\n", iEndCount.LowPart-iStartCount.LowPart);
#endif

	
	if(m_bEnablePolygonCorner)
	{		
		CRect rt;
		GetClientRect(&rt);		
		DrawPolygonBorder(pDC, rt, true);
		DrawPolygonBorder(pDC, rt, false);	
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////
// CGridCtrl Cell selection stuff

// Is a given cell designation valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(int nRow, int nCol) const
{
    return (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
}

BOOL CGridCtrl::IsValid(const CCellID& cell) const
{
    return IsValid(cell.row, cell.col);
}

// Is a given cell range valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridCtrl::IsValid(const CCellRange& range) const
{
    return (range.GetMinRow() >= 0 && range.GetMinCol() >= 0 &&
        range.GetMaxRow() >= 0 && range.GetMaxCol() >= 0 &&
        range.GetMaxRow() < m_nRows && range.GetMaxCol() < m_nCols &&
        range.GetMinRow() <= range.GetMaxRow() && range.GetMinCol() <= range.GetMaxCol());
}

// Enables/Disables redraw for certain operations like columns auto-sizing etc,
// but not for user caused things such as selection changes.
void CGridCtrl::SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars /* = FALSE */)
{
//    TRACE(_T("%s: Setting redraw to %s\n"),
//             GetRuntimeClass()->m_lpszClassName, bAllowDraw? _T("TRUE") : _T("FALSE"));

    if (bAllowDraw && !m_bAllowDraw)
    {
        m_bAllowDraw = TRUE;
        Refresh();
    }

    m_bAllowDraw = bAllowDraw;
    if (bResetScrollBars)
        ResetScrollBars();
}

// Forces a redraw of a cell immediately (using a direct DC construction,
// or the supplied dc)
BOOL CGridCtrl::RedrawCell(const CCellID& cell, CDC* pDC /* = NULL */)
{
    return RedrawCell(cell.row, cell.col, pDC);
}

BOOL CGridCtrl::RedrawCell(int nRow, int nCol, CDC* pDC /* = NULL */)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

	if ( m_hWnd == NULL )
	{
		return FALSE;
	}

	// xl 0416 与ondraw一致的显示可见判断
	if (!m_bAllowDraw)
        return FALSE;
//     if (!m_bAllowDraw || !IsCellVisible(nRow, nCol))
//         return FALSE;

    CRect rect;
    if (!GetCellRect(nRow, nCol, rect))
        return FALSE;

    if (!pDC)
    {
        pDC = GetDC();
        if (pDC)
            bMustReleaseDC = TRUE;
    }

    if (pDC)
    {
        // Redraw cells directly
        if (nRow < m_nFixedRows || nCol < m_nFixedCols)
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);
        }
        else
        {
            CGridCellBase* pCell = GetCell(nRow, nCol);
            if (pCell)
                bResult = pCell->Draw(pDC, nRow, nCol, rect, TRUE);

            // Since we have erased the background, we will need to redraw the gridlines
            CPen pen;
            pen.CreatePen(PS_SOLID, 0, m_crGridLineColour);

            CPen* pOldPen = (CPen*) pDC->SelectObject(&pen);
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                pDC->MoveTo(rect.left,    rect.bottom);
                pDC->LineTo(rect.right + 1, rect.bottom);
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                pDC->MoveTo(rect.right, rect.top);
                pDC->LineTo(rect.right, rect.bottom + 1);
            }
            pDC->SelectObject(pOldPen);
        }
    }
	else
	{
		rect.right++;
		rect.bottom++;
        InvalidateRect(rect, TRUE);     // Could not get a DC - invalidate it anyway
	}
    // and hope that OnPaint manages to get one

    if (bMustReleaseDC)
        ReleaseDC(pDC);

    return bResult;
}

// redraw a complete row
BOOL CGridCtrl::RedrawRow(int row)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int col = 0; col < GetColumnCount(); col++)
        bResult = RedrawCell(row, col, pDC) && bResult;
    if (pDC)
        ReleaseDC(pDC);

    return bResult;
}

// redraw a complete column
BOOL CGridCtrl::RedrawColumn(int col)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int row = 0; row < GetRowCount(); row++)
        bResult = RedrawCell(row, col, pDC) && bResult;
    if (pDC)
        ReleaseDC(pDC);

    return bResult;
}


// Sets the currently selected cell, returning the previous current cell
CCellID CGridCtrl::SetFocusCell(int nRow, int nCol)
{
    return SetFocusCell(CCellID(nRow, nCol));
}

CCellID CGridCtrl::SetFocusCell(CCellID cell)
{
	if (!m_bListHeaderCanNotClick)
	{
		return CCellID(-1,-1);
	}
    if (cell == m_idCurrentCell)
        return m_idCurrentCell;

    CCellID idPrev = m_idCurrentCell;

    // EFW - Bug Fix - Force focus to be in a non-fixed cell
    if (cell.row != -1 && cell.row < GetFixedRowCount())
        cell.row = GetFixedRowCount();
    if (cell.col != -1 && cell.col < GetFixedColumnCount())
        cell.col = GetFixedColumnCount();

    m_idCurrentCell = cell;

    if (IsValid(idPrev))
    {
        SetItemState(idPrev.row, idPrev.col,
            GetItemState(idPrev.row, idPrev.col) & ~GVIS_FOCUSED);
        RedrawCell(idPrev); // comment to reduce flicker

        if (GetTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, idPrev.col);
        if (GetTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(idPrev.row, col);
    }

    if (IsValid(m_idCurrentCell))
    {
        SetItemState(m_idCurrentCell.row, m_idCurrentCell.col,
            GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);
		
       RedrawCell(m_idCurrentCell); // comment to reduce flicker

        if (GetTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                RedrawCell(row, m_idCurrentCell.col);
        if (GetTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                RedrawCell(m_idCurrentCell.row, col);

        // EFW - New addition.  If in list mode, make sure the selected
        // row highlight follows the cursor.
        // Removed by C Maunder 27 May
        //if (m_bListMode)
        //{
        //    m_PrevSelectedCellMap.RemoveAll();
        //    m_MouseMode = MOUSE_SELECT_ROW;
        //    OnSelecting(m_idCurrentCell);

            // Leave this off so that you can still drag the highlight around
            // without selecting rows.
            // m_MouseMode = MOUSE_NOTHING;
        //}

	}	

    return idPrev;
}

void CGridCtrl::SetSelectedSingleRow( int iRow )
{
	// 如果超出范围，则还是以前的自己处理
	if ( GetSingleRowSelection() )
	{
		SelectRows(CCellID(iRow,0));
	}
	else
	{
		if ( GetFocusCell().row == iRow )
		{
			m_SelectionStartCell = GetFocusCell();
		}
		else
		{
			m_SelectionStartCell = CCellID(iRow, 0);
		}
		ResetSelectedRange();
		SelectRows(CCellID(iRow, 0));
	}
}

// Sets the range of currently selected cells
void CGridCtrl::SetSelectedRange(const CCellRange& Range,
                                 BOOL bForceRepaint /* = FALSE */, BOOL bSelectCells/*=TRUE*/)
{
    SetSelectedRange(Range.GetMinRow(), Range.GetMinCol(),
                     Range.GetMaxRow(), Range.GetMaxCol(),
                     bForceRepaint, bSelectCells);
}

void CGridCtrl::SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                                 BOOL bForceRepaint /* = FALSE */, BOOL bSelectCells/*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

	CWaitCursor wait; // Thomas Haase 

    CDC* pDC = NULL;
    if (bForceRepaint)
        pDC = GetDC();

	// Only redraw visible cells
    CCellRange VisCellRange, FixedVisCellRange;
	if (IsWindow(GetSafeHwnd()))

	{
		VisCellRange = GetVisibleNonFixedCellRange();
		FixedVisCellRange = GetVisibleFixedCellRange();
	}
   
    // EFW - Bug fix - Don't allow selection of fixed rows

// 	int Left= (m_AllowSelectRowInFixedCol ? 0 : GetFixedColumnCount());
// 
//     if(nMinRow >= 0 && nMinRow < GetFixedRowCount())
//         nMinRow = GetFixedRowCount();
//     if(nMaxRow >= 0 && nMaxRow < GetFixedRowCount())
//         nMaxRow = GetFixedRowCount();
//     if(nMinCol >= 0 && nMinCol < Left)
//         nMinCol = GetFixedColumnCount();
//     if(nMaxCol >= 0 && nMaxCol < Left)
//        nMaxCol = GetFixedColumnCount();

    // If we are selecting cells, then first clear out the list of currently selected cells, then
    if (bSelectCells)
    {
        POSITION pos;

        // Unselect all previously selected cells
        for (pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
        {
            DWORD key;
            CCellID cell;
            m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);

            // Reset the selection flag on the cell
            if (IsValid(cell))
            {
                // This will remove the cell from the m_SelectedCellMap map
                SetItemState(cell.row, cell.col,
                    GetItemState(cell.row, cell.col) & ~GVIS_SELECTED);

                // If this is to be reselected, continue on past the redraw
                if (nMinRow <= cell.row && cell.row <= nMaxRow &&
                    nMinCol <= cell.col && cell.col <= nMaxCol)
                    continue;

				if ( (VisCellRange.IsValid() && VisCellRange.InRange( cell )) || FixedVisCellRange.InRange( cell ) )
				{
					if (bForceRepaint && pDC)                    // Redraw NOW
						RedrawCell(cell.row, cell.col, pDC);
					else
						InvalidateCellRect(cell);                // Redraw at leisure
				}
            }
            else
            {
                m_SelectedCellMap.RemoveKey( key);  // if it's not valid, get rid of it!
            }
        }

        // if we are selecting cells, and there are previous selected cells to be retained 
        // (eg Ctrl is being held down) then copy them to the newly created list, and mark 
        // all these cells as selected
        // Note that if we are list mode, single row selection, then we won't be adding 
        // the previous cells. Only the current row of cells will be added (see below)
        if (!GetSingleRowSelection() &&
            nMinRow >= 0 && nMinCol >= 0 && nMaxRow >= 0 && nMaxCol >= 0)
        {
            for (pos = m_PrevSelectedCellMap.GetStartPosition(); pos != NULL; /* nothing */)
            {
                DWORD key;
                CCellID cell;
                m_PrevSelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);

                if (!IsValid(cell))
                    continue;

                int nState = GetItemState(cell.row, cell.col);

                // Set state as Selected. This will add the cell to m_SelectedCellMap
                SetItemState(cell.row, cell.col, nState | GVIS_SELECTED);

				if (( VisCellRange.IsValid() && VisCellRange.InRange( cell ))  || FixedVisCellRange.InRange( cell )  )
				{
					// Redraw (immediately or at leisure)
					if (bForceRepaint && pDC)
					    RedrawCell(cell.row, cell.col, pDC);
					else
					    InvalidateCellRect(cell);
				}
            }
        }
    }

    // Now select/deselect all cells in the cell range specified. If selecting, and the cell 
    // has already been marked as selected (above) then ignore it. If we are deselecting and
    // the cell isn't selected, then ignore
    if (nMinRow >= 0 && nMinCol >= 0 && nMaxRow >= 0 && nMaxCol >= 0 &&
        nMaxRow < m_nRows && nMaxCol < m_nCols &&
        nMinRow <= nMaxRow && nMinCol <= nMaxCol)
    {
        for (int row = nMinRow; row <= nMaxRow; row++)
            for (int col = nMinCol; col <= nMaxCol; col++)
            {
                BOOL bCellSelected = IsCellSelected(row, col);
                if (bSelectCells == bCellSelected)
                    continue;    // Already selected or deselected - ignore

                // Set the selected state. This will add/remove the cell to m_SelectedCellMap
                if (bSelectCells)
                    SetItemState(row, col, GetItemState(row, col) | GVIS_SELECTED);
                else
				{
                    SetItemState(row, col, GetItemState(row, col) & ~GVIS_SELECTED);
					// 必须从前面选中状态清除
					m_PrevSelectedCellMap.RemoveKey(MAKELONG(row, col));
				}

				if ( (VisCellRange.IsValid() && VisCellRange.InRange(row, col))  || FixedVisCellRange.InRange(row, col) )
				{
	                // Redraw (immediately or at leisure)
	                if (bForceRepaint && pDC)
	                    RedrawCell(row, col, pDC);
	                else
	                    InvalidateCellRect(row, col);
				}
            }
    }
    //    TRACE(_T("%d cells selected.\n"), m_SelectedCellMap.GetCount());

    if (pDC != NULL)
        ReleaseDC(pDC);

	// 统计selectedrow信息
	UpdateSelectedRows();
}

// selects all cells
void CGridCtrl::SelectAllCells()
{
    if (!m_bEnableSelection)
        return;

	if ( GetSingleRowSelection() )
	{
		return;
	}

	if ( GetListMode() )
	{
		SetSelectedRange(m_nFixedRows, 0, GetRowCount()-1, GetColumnCount()-1);
	}
	else
	{
		SetSelectedRange(m_nFixedRows, m_nFixedCols, GetRowCount()-1, GetColumnCount()-1);
	}
}

// selects columns
void CGridCtrl::SelectColumns(CCellID currentCell, 
                              BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.col == m_idCurrentCell.col) return;
    if (currentCell.col < m_nFixedCols)
        return;
    if (!IsValid(currentCell))
        return;

    if (GetSingleColSelection())
        SetSelectedRange(GetFixedRowCount(), currentCell.col,
                         GetRowCount()-1,    currentCell.col,
                         bForceRedraw, bSelectCells);
    else
        SetSelectedRange(GetFixedRowCount(),
                         min(m_SelectionStartCell.col, currentCell.col),
                         GetRowCount()-1,
                         max(m_SelectionStartCell.col, currentCell.col),
                         bForceRedraw, bSelectCells);
}

// selects rows 
void CGridCtrl::SelectRows(CCellID currentCell, 
                           BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.row; == m_idCurrentCell.row) return;
    if (currentCell.row < m_nFixedRows)
        return;
    if (!IsValid(currentCell))
        return;
	//int Left = ( m_AllowSelectRowInFixedCol ? 0 : GetFixedColumnCount());
	int Left = 0;
    if (GetSingleRowSelection())
        SetSelectedRange(currentCell.row, Left ,
                         currentCell.row, GetColumnCount()-1, 
                         bForceRedraw, bSelectCells);
    else
        SetSelectedRange(min(m_SelectionStartCell.row, currentCell.row),
                         Left,
                         __max(m_SelectionStartCell.row, currentCell.row),
                         GetColumnCount()-1,
                         bForceRedraw, bSelectCells);
}

// selects cells
void CGridCtrl::SelectCells(CCellID currentCell, 
                            BOOL bForceRedraw /*=FALSE*/, BOOL bSelectCells /*=TRUE*/)
{
    if (!m_bEnableSelection)
        return;

    int row = currentCell.row;
    int col = currentCell.col;
    if (row < m_nFixedRows || col < m_nFixedCols)
        return;
    if (!IsValid(currentCell))
        return;

    // Prevent unnecessary redraws
    //if (currentCell == m_LeftClickDownCell)  return;
    //else if (currentCell == m_idCurrentCell) return;

    SetSelectedRange(min(m_SelectionStartCell.row, row),
                     min(m_SelectionStartCell.col, col),
                     __max(m_SelectionStartCell.row, row),
                     __max(m_SelectionStartCell.col, col),
                     bForceRedraw, bSelectCells);
}

// Called when mouse/keyboard selection is a-happening.
void CGridCtrl::OnSelecting(const CCellID& currentCell)
{
    if (!m_bEnableSelection)
        return;

    switch (m_MouseMode)
    {
    case MOUSE_SELECT_ALL:
        SelectAllCells();
        break;
    case MOUSE_SELECT_COL:
        SelectColumns(currentCell, FALSE);
        break;
    case MOUSE_SELECT_ROW:
        SelectRows(currentCell, TRUE);
        break;
    case MOUSE_SELECT_CELLS:
        SelectCells(currentCell, FALSE);
        break;
    }

    // EFW - Bug fix [REMOVED CJM: this will cause infinite loop in list mode]
    // SetFocusCell(max(currentCell.row, m_nFixedRows), max(currentCell.col, m_nFixedCols));
}

void CGridCtrl::ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText)
{
    if (!IsCellEditable(nRow, nCol))
        return;

    if (SendMessageToParent(nRow, nCol, (int)GVN_BEGINLABELEDIT) >= 0)
    {
        CString strCurrentText = GetItemText(nRow, nCol);
        if (strCurrentText != strText)
        {
            SetItemText(nRow, nCol, strText);
            if (ValidateEdit(nRow, nCol, strText) && 
                SendMessageToParent(nRow, nCol, (int)GVN_ENDLABELEDIT) >= 0)
            {
                SetModified(TRUE, nRow, nCol);
                RedrawCell(nRow, nCol);
            }
            else
            {
                SetItemText(nRow, nCol, strCurrentText);
            }
        }
    }
}

void CGridCtrl::ClearCells(CCellRange Selection)
{
    for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
    {
        for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
        {
            // don't clear hidden cells
            if ( m_arRowHeights[row] > 0 && m_arColWidths[col] > 0 )
            {
                ValidateAndModifyCellContents(row, col, _T(""));
            }
		}
	}
    Refresh();
}

#ifndef GRIDCONTROL_NO_CLIPBOARD

////////////////////////////////////////////////////////////////////////////////////////
// Clipboard functions

// Deletes the contents from the selected cells
void CGridCtrl::CutSelectedText()
{
    if (!IsEditable())
        return;

    for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
    {
		DWORD key;
        CCellID cell;
        m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
        ValidateAndModifyCellContents(cell.row, cell.col, _T(""));
    }
}

// Copies text from the selected cells to the clipboard
COleDataSource* CGridCtrl::CopyTextFromGrid()
{
    CCellRange Selection = GetSelectedCellRange();
    if (!IsValid(Selection))
        return NULL;

    if (GetVirtualMode())
        SendCacheHintToParent(Selection);

    // Write to shared file (REMEBER: CF_TEXT is ANSI, not UNICODE, so we need to convert)
    CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

    // Get a tab delimited string to copy to cache
    CString str;
    CGridCellBase *pCell;
    for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
    {
        // don't copy hidden cells
        if( m_arRowHeights[row] <= 0 )
            continue;

        str.Empty();
        for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
        {
            // don't copy hidden cells
            if( m_arColWidths[col] <= 0 )
                continue;

            pCell = GetCell(row, col);
            if (pCell &&(pCell->GetState() & GVIS_SELECTED))
            {
                // if (!pCell->GetText())
                //    str += _T(" ");
                // else 
                str += pCell->GetText();
            }
            if (col != Selection.GetMaxCol()) 
                str += _T("\t");
        }

        if (row != Selection.GetMaxRow()) 
            str += _T("\r\n");
        
        sf.Write(_Unicode2MultiChar(str).c_str(), str.GetLength());
        str.ReleaseBuffer();
    }
    
    char c = '\0';
    sf.Write(&c, 1);

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(-1,-1,-1,-1));

    DWORD dwLen = (DWORD) sf.GetLength();
    HGLOBAL hMem = sf.Detach();
    if (!hMem)
        return NULL;

    hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    if (!hMem)
        return NULL;

    // Cache data
    COleDataSource* pSource = new COleDataSource();
    pSource->CacheGlobalData(CF_TEXT, hMem);

    return pSource;
}

// Pastes text from the clipboard to the selected cells
BOOL CGridCtrl::PasteTextToGrid(CCellID cell, COleDataObject* pDataObject, 
								BOOL bSelectPastedCells /*=TRUE*/)
{
    if (!IsValid(cell) || !IsCellEditable(cell) || !pDataObject->IsDataAvailable(CF_TEXT))
        return FALSE;

    // Get the text from the COleDataObject
    HGLOBAL hmem = pDataObject->GetGlobalData(CF_TEXT);
    CMemFile sf((BYTE*) ::GlobalLock(hmem), (UINT)::GlobalSize(hmem));

    // CF_TEXT is ANSI text, so we need to allocate a char* buffer
    // to hold this.
    LPTSTR szBuffer = new TCHAR[::GlobalSize(hmem)];
    if (!szBuffer)
        return FALSE;

    sf.Read(szBuffer, (UINT)::GlobalSize(hmem));
    ::GlobalUnlock(hmem);

    // Now store in generic TCHAR form so we no longer have to deal with
    // ANSI/UNICODE problems
    CString strText = szBuffer;
    delete []szBuffer;

    // Parse text data and set in cells...
    strText.LockBuffer();
    CString strLine = strText;
    int nLine = 0;

    // Find the end of the first line
	CCellRange PasteRange(cell.row, cell.col,-1,-1);
    int nIndex;
    do
    {
        int nColumn = 0;
        nIndex = strLine.Find(_T("\n"));

        // Store the remaining chars after the newline
        CString strNext = _T("");
		if (nIndex >= 0)
		{
			strNext = strLine.Mid(nIndex + 1);
		}

        // Remove all chars after the newline
        if (nIndex >= 0)
            strLine = strLine.Left(nIndex);

        int nLineIndex = strLine.FindOneOf(_T("\t,"));
        CString strCellText = (nLineIndex >= 0)? strLine.Left(nLineIndex) : strLine;

        // skip hidden rows
        int iRowVis = cell.row + nLine;
        while( iRowVis < GetRowCount())
        {
            if( GetRowHeight( iRowVis) > 0)
                break;
            nLine++;
            iRowVis++;
        }

        while (!strLine.IsEmpty())
        {
            // skip hidden columns
            int iColVis = cell.col + nColumn;
            while( iColVis < GetColumnCount())
            {
                if( GetColumnWidth( iColVis) > 0)
                    break;
                nColumn++;
                iColVis++;
            }

            CCellID TargetCell(iRowVis, iColVis);
            if (IsValid(TargetCell))
            {
                strCellText.TrimLeft();
                strCellText.TrimRight();

                ValidateAndModifyCellContents(TargetCell.row, TargetCell.col, strCellText);

                // Make sure cell is not selected to avoid data loss
                SetItemState(TargetCell.row, TargetCell.col,
                    GetItemState(TargetCell.row, TargetCell.col) & ~GVIS_SELECTED);

				if (iRowVis > PasteRange.GetMaxRow()) PasteRange.SetMaxRow(iRowVis);
				if (iColVis > PasteRange.GetMaxCol()) PasteRange.SetMaxCol(iColVis);
            }

			if (nLineIndex >= 0)
			{
				strLine = strLine.Mid(nLineIndex + 1);
			}
			else
			{
				strLine = _T("");
			}

            nLineIndex = strLine.FindOneOf(_T("\t,"));
            strCellText = (nLineIndex >= 0)? strLine.Left(nLineIndex) : strLine;

            nColumn++;
        }

        strLine = strNext;
        nLine++;
    } while (nIndex >= 0);

    strText.UnlockBuffer();

	if (bSelectPastedCells)
		SetSelectedRange(PasteRange, TRUE);
	else
	{
		ResetSelectedRange();
		Refresh();
	}

    return TRUE;
}
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP

// Start drag n drop
void CGridCtrl::OnBeginDrag()
{
    COleDataSource* pSource = NULL;
    if (!m_bAllowDragAndDrop && m_CurCol==-1)
        return;

    if (m_CurCol>=0)	pSource = new COleDataSource();
    if (!pSource && m_bAllowDragAndDrop) pSource = CopyTextFromGrid();
    if (pSource)
    {
        SendMessageToParent(GetSelectedCellRange().GetTopLeft().row,
            GetSelectedCellRange().GetTopLeft().col,
            (int)GVN_BEGINDRAG);

        m_MouseMode = MOUSE_DRAGGING;
        m_bLMouseButtonDown = FALSE;

        DROPEFFECT dropEffect = pSource->DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE);

        if (dropEffect & DROPEFFECT_MOVE)
            CutSelectedText();

        if (pSource)
            delete pSource;    // Did not pass source to clipboard, so must delete

    }
}

// Handle drag over grid
DROPEFFECT CGridCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState,
                                 CPoint point)
{
	// Find which cell we are over and drop-highlight it
	CCellID cell = GetCellFromPt(point, FALSE);
	bool Valid;
	// Any text data available for us?
	if(m_CurCol==-1)
	{
		if(m_bDragRowMode)
		{
			Valid = cell.col>=GetFixedColumnCount() && cell.row>=GetFixedRowCount();
		}
		else
		{
			if (!m_bAllowDragAndDrop || !IsEditable() || !pDataObject->IsDataAvailable(CF_TEXT))
				return DROPEFFECT_NONE;
			Valid = IsValid(cell)!=0;
		}
	}
	else
	{
		Valid = cell.col>=GetFixedColumnCount() &&   cell.row<GetFixedRowCount() ;
	}


	// If not valid, set the previously drop-highlighted cell as no longer drop-highlighted
	if (!Valid)
	{
		OnDragLeave();
		m_LastDragOverCell = CCellID(-1,-1);
		return DROPEFFECT_NONE;
	}
	if(m_CurCol==-1)
	{
		if (!m_bDragRowMode && !IsCellEditable(cell))
			return DROPEFFECT_NONE;
	}

	// Have we moved over a different cell than last time?
	if (cell != m_LastDragOverCell)
	{
		// Set the previously drop-highlighted cell as no longer drop-highlighted
		if (IsValid(m_LastDragOverCell))
		{
			UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
			SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
				nState & ~GVIS_DROPHILITED);
			RedrawCell(m_LastDragOverCell);
		}

		m_LastDragOverCell = cell;

		// Set the new cell as drop-highlighted
		if (IsValid(m_LastDragOverCell))
		{
			UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
			SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
				nState | GVIS_DROPHILITED);
			RedrawCell(m_LastDragOverCell);
		}
	}

	// Return an appropraite value of DROPEFFECT so mouse cursor is set properly
	if (dwKeyState & MK_CONTROL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_MOVE;
}

// Something has just been dragged onto the grid
DROPEFFECT CGridCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState,
								  CPoint point)
{
	// Any text data available for us?
	m_LastDragOverCell = GetCellFromPt(point, m_CurCol>=0);
	bool Valid;
	if(m_CurCol==-1)
	{
		if (!m_bAllowDragAndDrop || !pDataObject->IsDataAvailable(CF_TEXT))
			return DROPEFFECT_NONE;

		// Find which cell we are over and drop-highlight it
		if (!IsValid(m_LastDragOverCell))
			return DROPEFFECT_NONE;

		if (!IsCellEditable(m_LastDragOverCell))
			return DROPEFFECT_NONE;
		Valid = IsValid(m_LastDragOverCell)!=0;

	}
	else
	{
		Valid = m_LastDragOverCell.row>=0 && m_LastDragOverCell.row<GetFixedRowCount() && m_LastDragOverCell.col>=GetFixedColumnCount();
	}

	if (Valid)
	{
		UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
		SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
			nState | GVIS_DROPHILITED);
		RedrawCell(m_LastDragOverCell);
	}

	// Return an appropraite value of DROPEFFECT so mouse cursor is set properly
	if (dwKeyState & MK_CONTROL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_MOVE;
}

// Something has just been dragged away from the grid
void CGridCtrl::OnDragLeave()
{
    // Set the previously drop-highlighted cell as no longer drop-highlighted
    if (IsValid(m_LastDragOverCell))
    {
        UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
        SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
            nState & ~GVIS_DROPHILITED);
        RedrawCell(m_LastDragOverCell);
    }
}

// Something has just been dropped onto the grid
BOOL CGridCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/,
					   CPoint /* point */)
{
	m_MouseMode = MOUSE_NOTHING;
	if(m_CurCol ==-1)
	{
		if (!m_bAllowDragAndDrop || (!IsCellEditable(m_LastDragOverCell) && !m_bDragRowMode))
			return FALSE;
	}

	OnDragLeave();
	if (m_CurCol>=0)
	{
		if(m_LastDragOverCell.col == m_CurCol || m_LastDragOverCell.row >= GetFixedRowCount()) 
			return FALSE;
		else
		{
			int New = m_arColOrder[m_CurCol];
			m_arColOrder.erase(m_arColOrder.begin()+m_CurCol);
			m_arColOrder.insert(m_arColOrder.begin()+m_LastDragOverCell.col, New);
			m_CurCol=-1;
			Invalidate();
			return TRUE;
		}
	}
	else
	{
		if(m_bDragRowMode)
		{
			Reorder(m_CurRow,m_LastDragOverCell.row);
			Invalidate();
			return TRUE;
		}
		else
		{
			return PasteTextToGrid(m_LastDragOverCell, pDataObject, FALSE);
		}
	}
}
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
void CGridCtrl::OnEditCut()
{
    if (!IsEditable())
        return;

    COleDataSource* pSource = CopyTextFromGrid();
    if (!pSource)
        return;

    pSource->SetClipboard();
    CutSelectedText();
}

void CGridCtrl::OnEditCopy()
{
    COleDataSource* pSource = CopyTextFromGrid();
    if (!pSource)
        return;

    pSource->SetClipboard();
}

void CGridCtrl::OnEditPaste()
{
    if (!IsEditable())
        return;

	CCellRange cellRange = GetSelectedCellRange();

	// Get the top-left selected cell, or the Focus cell, or the topleft (non-fixed) cell
	CCellID cell;
	if (cellRange.IsValid())
	{
		cell.row = cellRange.GetMinRow();
		cell.col = cellRange.GetMinCol();
	}
	else
	{
		cell = GetFocusCell();
		if (!IsValid(cell))
			cell = GetTopleftNonFixedCell();
		if (!IsValid(cell))
			return;
	}

	// If a cell is being edited, then call it's edit window paste function.
    if ( IsItemEditing(cell.row, cell.col) )
    {
        CGridCellBase* pCell = GetCell(cell.row, cell.col);
        ASSERT(pCell);
        if (!pCell) return;

		CWnd* pEditWnd = pCell->GetEditWnd();
		if ( pEditWnd && pEditWnd->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit*)pEditWnd)->Paste();
			return;
		}
    }

    // Attach a COleDataObject to the clipboard and paste the data to the grid
    COleDataObject obj;
    if (obj.AttachClipboard())
        PasteTextToGrid(cell, &obj);
}
#endif

void CGridCtrl::OnEditSelectAll()
{
    SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)GVN_SELCHANGING);
    SelectAllCells();
    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, (int)GVN_SELCHANGED);
}

#ifndef GRIDCONTROL_NO_CLIPBOARD
void CGridCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() > 0); // - Thomas Haase 
    //CCellRange Selection = GetSelectedCellRange();
    //pCmdUI->Enable(Selection.Count() && IsValid(Selection));
}

void CGridCtrl::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsEditable() && GetSelectedCount() > 0); // - Thomas Haase 
    //CCellRange Selection = GetSelectedCellRange();
    //pCmdUI->Enable(IsEditable() && Selection.Count() && IsValid(Selection));
}

void CGridCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    CCellID cell = GetFocusCell();

    BOOL bCanPaste = IsValid(cell) && IsCellEditable(cell) &&
        ::IsClipboardFormatAvailable(CF_TEXT);

    pCmdUI->Enable(bCanPaste);
}
#endif

void CGridCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bEnableSelection);
}

////////////////////////////////////////////////////////////////////////////////////////
// hittest-like functions

// TRUE if the mouse is over a row resize area
BOOL CGridCtrl::MouseOverRowResizeArea(CPoint& point)
{
    if (point.x >= GetFixedColumnWidth())
        return FALSE;

    CCellID idCurrentCell = GetCellFromPt(point);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start))
        return FALSE;

    int endy = start.y + GetRowHeight(idCurrentCell.row);

    if ((point.y - start.y < m_nResizeCaptureRange && idCurrentCell.row != 0) ||
        endy - point.y < m_nResizeCaptureRange)
    {
        return TRUE;
    }
    else
        return FALSE;
}

// TRUE if the mouse is over a column resize area. point is in Client coords
BOOL CGridCtrl::MouseOverColumnResizeArea(CPoint& point)
{
    if (point.y >= GetFixedRowHeight())
	{
#ifndef _WIN32_WCE_NO_CURSOR
//		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
        return FALSE;
	}
    CCellID idCurrentCell = GetCellFromPt(point);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start))
        return FALSE;

    int endx = start.x + GetColumnWidth(idCurrentCell.col);

    if ((point.x - start.x < m_nResizeCaptureRange && idCurrentCell.col != 0) ||
        endx - point.x < m_nResizeCaptureRange)
    {
        return TRUE;
    }
    else
        return FALSE;
}

// Get cell from point.
// point - client coordinates
// bAllowFixedCellCheck - if TRUE then fixed cells are checked
CCellID CGridCtrl::GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck /*=TRUE*/)
{
    CCellID cellID; // return value

    CCellID idTopLeft = GetTopleftNonFixedCell();
	if (!bAllowFixedCellCheck && !IsValid(idTopLeft))
		return cellID;

    // calculate column index
    int fixedColWidth = GetFixedColumnWidth();

    if (point.x < 0 || (!bAllowFixedCellCheck && point.x < fixedColWidth)) // not in window
        cellID.col = -1;
    else if (point.x < fixedColWidth) // in fixed col
    {
        int xpos = 0;
        int col = 0;
        while (col < m_nFixedCols)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
			col++;
        }

        cellID.col = col;
    }
    else    // in non-fixed col
    {
        int xpos = fixedColWidth;
		int col = idTopLeft.col; //m_nFixedCols;
        while ( col < GetColumnCount())
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
			col++;
        }

        if (col >= GetColumnCount())
            cellID.col = -1;
        else
            cellID.col = col;
    }

    // calculate row index
    int fixedRowHeight = GetFixedRowHeight();
    if (point.y < 0 || (!bAllowFixedCellCheck && point.y < fixedRowHeight)) // not in window
        cellID.row = -1;
    else if (point.y < fixedRowHeight) // in fixed col
    {
        int ypos = 0;
        int row = 0;
        while (row < m_nFixedRows) 
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
			row++;
        }
        cellID.row = row;
    }
    else
    {
        int ypos = fixedRowHeight;
		int row = idTopLeft.row; //m_nFixedRows;
        while ( row < GetRowCount() )
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
			row++;
        }

        if (row >= GetRowCount())
            cellID.row = -1;
        else
            cellID.row = row;
    }
    return cellID;
}

////////////////////////////////////////////////////////////////////////////////
// CGridCtrl cellrange functions

// Gets the first non-fixed cell ID
CCellID CGridCtrl::GetTopleftNonFixedCell(BOOL bForceRecalculation /*=FALSE*/)
{
    // Used cached value if possible
    if (m_idTopLeftCell.IsValid() && !bForceRecalculation)
        return m_idTopLeftCell;

    int nVertScroll = GetScrollPos32(SB_VERT), 
        nHorzScroll = GetScrollPos32(SB_HORZ);

    m_idTopLeftCell.col = m_nFixedCols;
    int nRight = 0;
    while (nRight < nHorzScroll && m_idTopLeftCell.col < (GetColumnCount()-1))
        nRight += GetColumnWidth(m_idTopLeftCell.col++);

    m_idTopLeftCell.row = m_nFixedRows;
    int nTop = 0;
    while (nTop < nVertScroll && m_idTopLeftCell.row < (GetRowCount()-1))
        nTop += GetRowHeight(m_idTopLeftCell.row++);

    //TRACE2("TopLeft cell is row %d, col %d\n",m_idTopLeftCell.row, m_idTopLeftCell.col);
    return m_idTopLeftCell;
}





// This gets even partially visible cells
CCellRange CGridCtrl::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/,
												  BOOL bContainFragment/*=TRUE*/)
{
    int i;
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
			if ( !bContainFragment && bottom > rect.bottom )
			{
				bottom -= GetRowHeight(i);
				i--;	// 不允许包含部分行的话剔除最后一行
			}
			else
			{
				bottom = rect.bottom;
			}
            break;
        }
    }

	int iRowCnt = GetRowCount() - 1;
    int maxVisibleRow = min(i, iRowCnt);

    // calc right
    int iRight = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        iRight += GetColumnWidth(i);
        if (iRight >= rect.right)
        {
			if ( !bContainFragment && iRight > rect.right )
			{
				iRight -= GetColumnWidth(i);
				i--;	// 不允许包含部分列的话剔除最后一列
			}
			else
			{
				iRight = rect.right;
			}
            break;
        }
    }

	int iColCnt = GetColumnCount() - 1;
    int maxVisibleCol = min(i, iColCnt);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = iRight;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}


CCellRange CGridCtrl::GetVisibleFixedCellRange(LPRECT pRect /*=NULL*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/)
{
    int i;
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
            bottom = rect.bottom;
            break;
        }
    }
	int iRowCnt = GetRowCount() - 1;
    int maxVisibleRow = min(i, iRowCnt);

    // calc right
    int iRight = 0;
    for (i = 0; i < GetFixedColumnCount(); i++)
    {
        iRight += GetColumnWidth(i);
        if (iRight >= rect.right)
        {
            iRight = rect.right;
            break;
        }
    }

	int iColCnt = GetColumnCount() - 1;
    int maxVisibleCol = min(i, iColCnt);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = iRight;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, 0, maxVisibleRow, maxVisibleCol);
}

// used by ResetScrollBars() - This gets only fully visible cells
CCellRange CGridCtrl::GetUnobstructedNonFixedCellRange(BOOL bForceRecalculation /*=FALSE*/)
{
    CRect rect;
    GetClientRect(rect);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int i;
    int bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
            break;
    }
	int iRowCnt = GetRowCount() - 1;
    int maxVisibleRow = min(i, iRowCnt);
    if (maxVisibleRow > 0 && bottom > rect.bottom)
        maxVisibleRow--;

    // calc right
    int iRight = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        iRight += GetColumnWidth(i);
        if (iRight >= rect.right)
            break;
    }
	int iColCnt = GetColumnCount() - 1;
    int maxVisibleCol = min(i, iColCnt);
    if (maxVisibleCol > 0 && iRight > rect.right)
        maxVisibleCol--;

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// Returns the minimum bounding range of the current selection
// If no selection, then the returned CCellRange will be invalid
CCellRange CGridCtrl::GetSelectedCellRange() const
{
    CCellRange Selection(GetRowCount(), GetColumnCount(), -1,-1);

    for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
    {
        DWORD key;
        CCellID cell;
        m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);

		int iSelMinRow = Selection.GetMinRow();
		int iSelMinCol = Selection.GetMinCol();
		int iSelMaxRow = Selection.GetMaxRow();
		int iSelMaxCol = Selection.GetMaxCol();
        Selection.SetMinRow( min(iSelMinRow, cell.row) );
        Selection.SetMinCol( min(iSelMinCol, cell.col) );
        Selection.SetMaxRow( __max(iSelMaxRow, cell.row) );
        Selection.SetMaxCol( __max(iSelMaxCol, cell.col) );
    }

    return Selection;
}

// Returns ALL the cells in the grid
CCellRange CGridCtrl::GetCellRange() const
{
    return CCellRange(0, 0, GetRowCount() - 1, GetColumnCount() - 1);
}

// Resets the selected cell range to the empty set.
void CGridCtrl::ResetSelectedRange()
{
	m_PrevSelectedCellMap.RemoveAll();

	// 视图显示上更新选择的删除动作
    SetSelectedRange(-1,-1,-1,-1);

    //SetFocusCell(-1,-1);

	m_SelectedRowList.RemoveAll();
}

// Get/Set scroll position using 32 bit functions
int CGridCtrl::GetScrollPos32(int nBar, BOOL bGetTrackPos /* = FALSE */)
{

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
	
	if ( TRUE == m_bOriginalScroll ) //2013-7-13 cym add 使用原始滚动条
	{	
		if (bGetTrackPos)
		{
			if (GetScrollInfo(nBar, &si, SIF_TRACKPOS))
				return si.nTrackPos;
		}
		else
		{
			if (GetScrollInfo(nBar, &si, SIF_POS))
				return si.nPos;
		}
	}
	else 
	{
		if (SB_HORZ == nBar)
		{
			if (NULL != m_pXSBHorz)
				return m_pXSBHorz->GetScrollPos();
		}
		else if (SB_VERT == nBar)
		{
			if (NULL != m_pXSBVert)
				return m_pXSBVert->GetScrollPos();
		}
	}

    return 0;
}

BOOL CGridCtrl::SetScrollPos32(int nBar, int nPos, BOOL bRedraw /* = TRUE */)
{
	m_idTopLeftCell.row = -1;

	if ( TRUE == m_bOriginalScroll )  //2013-7-13 cym add 使用原始滚动条
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask  = SIF_POS;
		si.nPos   = nPos;
		return SetScrollInfo(nBar, &si, bRedraw);
	}

	if (SB_HORZ == nBar)
	{
		if (NULL != m_pXSBHorz)
		{
			m_pXSBHorz->SetScrollPos(nPos, bRedraw);
			return TRUE;
		}
	}

    if (SB_VERT == nBar)
	{
		if (NULL != m_pXSBVert)
		{
			m_pXSBVert->SetScrollPos(nPos, bRedraw);
			return TRUE;
		}
	}

	return FALSE;
}

void CGridCtrl::EnableScrollBars(int nBar, BOOL bEnable /*=TRUE*/)
{
	// xl 修改该函数行为 - 使能滚动功能必须要有实际存在的ScrollBar
    if (bEnable)
    {
        if (!IsVisibleHScroll() && (nBar == SB_HORZ || nBar == SB_BOTH))
        {
            m_nBarState |= GVL_HORZ;
            
			if (NULL != m_pXSBHorz)
				m_pXSBHorz->EnableWindow(bEnable);
        }
        
        if (!IsVisibleVScroll() && (nBar == SB_VERT || nBar == SB_BOTH))
        {
            m_nBarState |= GVL_VERT;

            if (NULL != m_pXSBVert)
				m_pXSBVert->EnableWindow(bEnable);
        }

		if ( !IsWindow(m_pXSBHorz->GetSafeHwnd()) )	// 暂时限定为要一个窗口，其实也可是抽象的东东
		{
			m_nBarState &= ~GVL_HORZ;
		}

		if (NULL != m_pXSBVert)
		{
			if ( !IsWindow(m_pXSBVert->GetSafeHwnd()) )
			{
				m_nBarState &= ~GVL_VERT;
			}
		}
    }
    else
    {
        if ( IsVisibleHScroll() && (nBar == SB_HORZ || nBar == SB_BOTH))
        {
            m_nBarState &= ~GVL_HORZ; 

			if (NULL != m_pXSBHorz)
				m_pXSBHorz->EnableWindow(bEnable);
        }
        
        if ( IsVisibleVScroll() && (nBar == SB_VERT || nBar == SB_BOTH))
        {
            m_nBarState &= ~GVL_VERT;
            
			if (NULL != m_pXSBVert)
				m_pXSBVert->EnableWindow(bEnable);
        }
    }

}

// If resizing or cell counts/sizes change, call this - it'll fix up the scroll bars
void CGridCtrl::ResetScrollBars()
{
    // Force a refresh. 
    m_idTopLeftCell.row = -1;

    if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd())) 
        return;
    
    CRect rect;
    
    // This would have caused OnSize event - Brian 
    //EnableScrollBars(SB_BOTH, FALSE); 
    
    GetClientRect(rect);
    
    if (rect.left == rect.right || rect.top == rect.bottom)
        return;
    
    rect.left += GetFixedColumnWidth();
    rect.top += GetFixedRowHeight();
    
    
    if (rect.left >= rect.right || rect.top >= rect.bottom)
    {
        EnableScrollBars(SB_BOTH, FALSE);
        return;
    }
    
    CRect VisibleRect(GetFixedColumnWidth(), GetFixedRowHeight(), 
		              rect.right, rect.bottom);
    CRect VirtualRect(GetFixedColumnWidth(), GetFixedRowHeight(),
		              GetVirtualWidth(), GetVirtualHeight());
    
    // Removed to fix single row scrollbar problem (Pontus Goffe)
    // CCellRange visibleCells = GetUnobstructedNonFixedCellRange();
    // if (!IsValid(visibleCells)) return;
        
    //TRACE(_T("Visible: %d x %d, Virtual %d x %d.  H %d, V %d\n"), 
    //      VisibleRect.Width(), VisibleRect.Height(),
    //      VirtualRect.Width(), VirtualRect.Height(),
    //      IsVisibleHScroll(), IsVisibleVScroll());

//     // If vertical scroll bar, horizontal space is reduced
//     if (VisibleRect.Height() < VirtualRect.Height())
//         VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
//     // If horz scroll bar, vert space is reduced
//     if (VisibleRect.Width() < VirtualRect.Width())
//         VisibleRect.bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
    
    // Recheck vertical scroll bar
    //if (VisibleRect.Height() < VirtualRect.Height())
    // VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);

	
	if (VisibleRect.Height() < VirtualRect.Height())
	{
		EnableScrollBars(SB_VERT, TRUE); 
		m_nVScrollMax = VirtualRect.Height() - 1;
	}
	else
	{
		EnableScrollBars(SB_VERT, FALSE); 
		m_nVScrollMax = 0;
	}
	
	if (VisibleRect.Width() < VirtualRect.Width())
	{
		EnableScrollBars(SB_HORZ, TRUE); 
		m_nHScrollMax = VirtualRect.Width() - 1;
		// modify by tangad 滚动条最多只能滚到最右边的数据
		if (m_nHScrollMax - rect.Width() >= 0)
		{
			m_nHScrollMax -= rect.Width();
		}
	}
	else
	{
		//EnableScrollBars(SB_HORZ, FALSE); 
		//m_nHScrollMax = 0;
		
		// 0001772 水平滚动条在能够全屏显示完，但是用户隐藏了部分列时失去滚动功能，现在修改为任何时候都保持滚动功能
		// 这个问题与virtual width的计算有关，vertical就没事
		EnableScrollBars(SB_HORZ, TRUE); 
		m_nHScrollMax = VirtualRect.Width() - 1;
	}

	if ( (GVL_HORZ & m_dwDonotScrollToNonExistCell) && m_nHScrollMax > 0 )
    {
		// 限制了水平不存在列的滚动
		// 从总的宽度中除去可见部分宽度
		m_nHScrollMax -= VisibleRect.Width();
		if ( m_nHScrollMax <= 0 )
		{
			EnableScrollBars(SB_HORZ, FALSE);
			m_nHScrollMax = 0;
		}
    }
    if ( (GVL_VERT & m_dwDonotScrollToNonExistCell) && m_nVScrollMax > 0 )
    {
		// 限制了竖直不存在行的滚动
		m_nVScrollMax -= VisibleRect.Height();
		if ( m_nVScrollMax <= 0 )
		{
			EnableScrollBars(SB_VERT, FALSE);
			m_nVScrollMax = 0;
		}
    }

    ASSERT(m_nVScrollMax < INT_MAX && m_nHScrollMax < INT_MAX); // This should be fine

    /* Old code - CJM
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE;
    si.nPage = (m_nHScrollMax>0)? VisibleRect.Width() : 0;
    SetScrollInfo(SB_HORZ, &si, FALSE); 
    si.nPage = (m_nVScrollMax>0)? VisibleRect.Height() : 0;
    SetScrollInfo(SB_VERT, &si, FALSE);

    SetScrollRange(SB_VERT, 0, m_nVScrollMax, TRUE);
    SetScrollRange(SB_HORZ, 0, m_nHScrollMax, TRUE);
    */

    // New code - Paul Runstedler 
	
	if ( TRUE == m_bOriginalScroll ) //2013-7-13 cym add 使用原始的滚动条
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = (m_nHScrollMax>0)? VisibleRect.Width() : 0;
		si.nMin = 0;
		si.nMax = m_nHScrollMax;
		SetScrollInfo(SB_HORZ, &si, TRUE);
		
		si.fMask |= SIF_DISABLENOSCROLL;
		si.nPage = (m_nVScrollMax>0)? VisibleRect.Height() : 0;
		si.nMin = 0;
		si.nMax = m_nVScrollMax;
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
	else
	{
		if (NULL != m_pXSBHorz)
		{
			m_pXSBHorz->SetScrollRange(0, m_nHScrollMax, TRUE);
			if ( GetScrollPos32(SB_HORZ) > m_nHScrollMax )
			{
				SetScrollPos32(SB_HORZ, m_nHScrollMax);
			}
		}

		if (NULL != m_pXSBVert)
		{
			m_pXSBVert->SetScrollRange(0, m_nVScrollMax, TRUE);
			if ( GetScrollPos32(SB_VERT) > m_nVScrollMax )
			{
				SetScrollPos32(SB_VERT, m_nVScrollMax);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column position functions

// returns the top left point of the cell. Returns FALSE if cell not visible.
BOOL CGridCtrl::GetCellOrigin(int nRow, int nCol, LPPOINT p)
{
    int i;

    if (!IsValid(nRow, nCol))
        return FALSE;

    CCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = GetTopleftNonFixedCell();

    if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
        (nCol>= m_nFixedCols && nCol < idTopLeft.col))
        return FALSE;

    p->x = 0;
    if (nCol < m_nFixedCols)                      // is a fixed column
        for (i = 0; i < nCol; i++)
            p->x += GetColumnWidth(i);
        else 
        {                                        // is a scrollable data column
            for (i = 0; i < m_nFixedCols; i++)
                p->x += GetColumnWidth(i);
            for (i = idTopLeft.col; i < nCol; i++)
                p->x += GetColumnWidth(i);
        }
        
        p->y = 0;
        if (nRow < m_nFixedRows)                      // is a fixed row
            for (i = 0; i < nRow; i++)
                p->y += GetRowHeight(i);
            else 
            {                                        // is a scrollable data row
                for (i = 0; i < m_nFixedRows; i++)
                    p->y += GetRowHeight(i);
                for (i = idTopLeft.row; i < nRow; i++)
                    p->y += GetRowHeight(i);
            }
            
            return TRUE;
}

BOOL CGridCtrl::GetCellOrigin(const CCellID& cell, LPPOINT p)
{
    return GetCellOrigin(cell.row, cell.col, p);
}

// Returns the bounding box of the cell
BOOL CGridCtrl::GetCellRect(const CCellID& cell, LPRECT pRect)
{
    return GetCellRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetCellRect(int nRow, int nCol, LPRECT pRect)
{
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin))
        return FALSE;

    pRect->left   = CellOrigin.x;
    pRect->top    = CellOrigin.y;
	pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
	pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;


    //TRACE("Row %d, col %d: L %d, T %d, W %d, H %d:  %d,%d - %d,%d\n",
    //      nRow,nCol, CellOrigin.x, CellOrigin.y, GetColumnWidth(nCol), GetRowHeight(nRow),
    //      pRect->left, pRect->top, pRect->right, pRect->bottom);

    return TRUE;
}

BOOL CGridCtrl::GetTextRect(const CCellID& cell, LPRECT pRect)
{
    return GetTextRect(cell.row, cell.col, pRect);
}

BOOL CGridCtrl::GetTextRect(int nRow, int nCol, LPRECT pRect)
{
    CGridCellBase* pCell = GetCell( nRow, nCol);
    if( pCell == NULL)
        return FALSE;
    
    if( !GetCellRect( nRow, nCol, pRect) )
        return FALSE;

    return pCell->GetTextRect( pRect);
}

// Returns the bounding box of a range of cells
BOOL CGridCtrl::GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect)
{
    CPoint MinOrigin,MaxOrigin;

    if (!GetCellOrigin(cellRange.GetMinRow(), cellRange.GetMinCol(), &MinOrigin))
        return FALSE;
    if (!GetCellOrigin(cellRange.GetMaxRow(), cellRange.GetMaxCol(), &MaxOrigin))
        return FALSE;

    lpRect->left   = MinOrigin.x;
    lpRect->top    = MinOrigin.y;
    lpRect->right  = MaxOrigin.x + GetColumnWidth(cellRange.GetMaxCol()) - 1;
    lpRect->bottom = MaxOrigin.y + GetRowHeight(cellRange.GetMaxRow()) - 1;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// Grid attribute functions

LRESULT CGridCtrl::OnSetFont(WPARAM hFont, LPARAM /*lParam */)
{
    LRESULT result = Default();

    // Get the logical font
    LOGFONT lf;
    if (!GetObject((HFONT) hFont, sizeof(LOGFONT), &lf))
        return result;

    m_cellDefault.SetFont(&lf);
    m_cellFixedColDef.SetFont(&lf);
    m_cellFixedRowDef.SetFont(&lf);
    m_cellFixedRowColDef.SetFont(&lf);

    Refresh();

    return result;
}

LRESULT CGridCtrl::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //LOGFONT    lf;
    //m_cellDefault.GetFontObject()->GetLogFont(&lf);

    return (LRESULT) m_cellDefault.GetFontObject()->GetSafeHandle();
}

#ifndef _WIN32_WCE_NO_CURSOR
BOOL CGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT)
    {
        switch (m_MouseMode)
        {
        case MOUSE_OVER_COL_DIVIDE:
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
            break;
        case MOUSE_OVER_ROW_DIVIDE:
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
            break;
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_DRAGGING:
            break;
#endif
        default:
            if (!GetVirtualMode())
            {
                CPoint pt(GetMessagePos());
                ScreenToClient(&pt);
                CCellID cell = GetCellFromPt(pt);
                if (IsValid(cell))
                {
                    CGridCellBase* pCell = GetCell(cell.row, cell.col);
                    if (pCell)
					{
						if (this == m_gGridEntrust && cell.col == 0 && cell.row > 0 && m_hHandCursor != NULL)
							pCell->m_hCursor = m_hHandCursor;
						else
							pCell->m_hCursor = NULL;
                        return pCell->OnSetCursor();
					}
                }
            }

            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			return TRUE;
        }
     }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
#endif

////////////////////////////////////////////////////////////////////////////////////
// Row/Column count functions

BOOL CGridCtrl::SetFixedRowCount(int nFixedRows)
{
	if (m_nFixedRows == nFixedRows)
		return TRUE;

	ASSERT(nFixedRows >= 0);

	ResetSelectedRange();

	// Force recalculation
	m_idTopLeftCell.col = -1;

	if (nFixedRows > GetRowCount())
		if (!SetRowCount(nFixedRows))
			return FALSE;

	if (m_idCurrentCell.row < nFixedRows)
		SetFocusCell(-1, - 1);

	if (!GetVirtualMode())
	{
		if (nFixedRows > m_nFixedRows)
		{
			for (int i = m_nFixedRows; i < nFixedRows; i++)
				for (int j = 0; j < GetColumnCount(); j++)
				{
					SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED | GVIS_FIXEDROW);
					SetItemBkColour(i, j, CLR_DEFAULT );
					SetItemFgColour(i, j, CLR_DEFAULT );
				}
		}
		else
		{
			for (int i = nFixedRows; i < m_nFixedRows; i++)
			{
				int j;
				for (j = 0; j < GetFixedColumnCount(); j++)
					SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXEDROW );

				for (j = GetFixedColumnCount(); j < GetColumnCount(); j++)
				{
					SetItemState(i, j, GetItemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDROW) );
					SetItemBkColour(i, j, CLR_DEFAULT );
					SetItemFgColour(i, j, CLR_DEFAULT );
				}
			}
		}
	}

	m_nFixedRows = nFixedRows;

	Refresh();

	return TRUE;
}

BOOL CGridCtrl::SetFixedColumnCount(int nFixedCols)
{
    if (m_nFixedCols == nFixedCols)
        return TRUE;

    ASSERT(nFixedCols >= 0);

    if (nFixedCols > GetColumnCount())
        if (!SetColumnCount(nFixedCols))
            return FALSE;

    if (m_idCurrentCell.col < nFixedCols)
        SetFocusCell(-1, - 1);

    ResetSelectedRange();

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (!GetVirtualMode())
    {
        if (nFixedCols > m_nFixedCols)
        {
            for (int i = 0; i < GetRowCount(); i++)
                for (int j = m_nFixedCols; j < nFixedCols; j++)
                {
                    SetItemState(i, j, GetItemState(i, j) | GVIS_FIXED | GVIS_FIXEDCOL);
                    SetItemBkColour(i, j, CLR_DEFAULT );
                    SetItemFgColour(i, j, CLR_DEFAULT );
                }
        }
        else
        {
			{ // Scope limit i,j
	            for (int i = 0; i < GetFixedRowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
			            SetItemState(i, j, GetItemState(i, j) & ~GVIS_FIXEDCOL );
			}
			{// Scope limit i,j
	            for (int i = GetFixedRowCount(); i < GetRowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
			        {
				        SetItemState(i, j, GetItemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDCOL) );
					    SetItemBkColour(i, j, CLR_DEFAULT );
						SetItemFgColour(i, j, CLR_DEFAULT );
	                }
			}
        }
    }
        
    m_nFixedCols = nFixedCols;
        
    Refresh();
        
    return TRUE;
}

BOOL CGridCtrl::SetRowCount(int nRows)
{
    BOOL bResult = TRUE;

    ASSERT(nRows >= 0);
    if (nRows == GetRowCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nRows < m_nFixedRows)
        m_nFixedRows = nRows;

    if (m_idCurrentCell.row >= nRows)
        SetFocusCell(-1, - 1);

    int addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the GridCell objects
    // in each column within each row
    if (addedRows < 0)
    {
        if (!GetVirtualMode())
        {
            for (int row = nRows; row < m_nRows; row++)
            {
                // Delete cells
                for (int col = 0; col < m_nCols; col++)
                    DestroyCell(row, col);
            
                // Delete rows
                GRID_ROW* pRow = m_RowData[row];
                if (pRow)
				{
					delete pRow;
					m_RowData[row] = NULL;
				}

            }
        }
        m_nRows = nRows;
    }
    
    TRY
    {
        m_arRowHeights.SetSize(nRows);

        if (GetVirtualMode())
        {
            m_nRows = nRows;
            if (addedRows > 0)
            {
                int startRow = nRows - addedRows;
                for (int row = startRow; row < nRows; row++)
                    m_arRowHeights[row] = m_cellDefault.GetHeight();
            }
			ResetVirtualOrder();
        }
        else
        {
            // Change the number of rows.
            m_RowData.SetSize(nRows);

            // If we have just added rows, we need to construct new elements for each cell
            // and set the default row height
            if (addedRows > 0)
            {
                // initialize row heights and data
                int startRow = nRows - addedRows;
                for (int row = startRow; row < nRows; row++)
                {
                    m_arRowHeights[row] = m_cellDefault.GetHeight();

                    m_RowData[row] = new GRID_ROW;
                    m_RowData[row]->SetSize(m_nCols);
                    for (int col = 0; col < m_nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData[row];
                        if (pRow && !GetVirtualMode())
                            pRow->SetAt(col, CreateCell(row, col));
                    }
                    m_nRows++;
                }
            }
        }
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        bResult = FALSE;
    }
    END_CATCH

    SetModified();
    ResetScrollBars();
    Refresh();

    return bResult;
}

BOOL CGridCtrl::SetColumnCount(int nCols)
{
    BOOL bResult = TRUE;

    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nCols < m_nFixedCols)
        m_nFixedCols = nCols;

    if (m_idCurrentCell.col >= nCols)
        SetFocusCell(-1, - 1);

    int addedCols = nCols - GetColumnCount();

    // If we are about to lose columns, then we need to delete the GridCell objects
    // within each column
    if (addedCols < 0 && !GetVirtualMode())
    {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < GetColumnCount(); col++)
                DestroyCell(row, col);
    }

    TRY 
    {
        // Change the number of columns.
        m_arColWidths.SetSize(nCols);
    
        // Change the number of columns in each row.
        if (!GetVirtualMode())
            for (int i = 0; i < m_nRows; i++)
                if (m_RowData[i])
                    m_RowData[i]->SetSize(nCols);
        
        // If we have just added columns, we need to construct new elements for each cell
        // and set the default column width
        if (addedCols > 0)
        {
            int row, col;

            // initialized column widths
            int startCol = nCols - addedCols;
            for (col = startCol; col < nCols; col++)
                m_arColWidths[col] = m_cellFixedColDef.GetWidth();
        
            // initialise column data
            if (!GetVirtualMode())
            {
                for (row = 0; row < m_nRows; row++)
                    for (col = startCol; col < nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData[row];
                        if (pRow)
                            pRow->SetAt(col, CreateCell(row, col));
                    }
            }
        }
        // else    // check for selected cell ranges
        //    ResetSelectedRange();
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        bResult = FALSE;
    }
    END_CATCH

	m_arColOrder.resize(nCols);  // Reset Column Order
    for (int i = 0; i < nCols; i++)
	{
		m_arColOrder[i] = i;	
	}

    m_nCols = nCols;

    SetModified();
    ResetScrollBars();
    Refresh();

    return bResult;
}

// Insert a column at a given position, or add to end of columns (if nColumn = -1)
int CGridCtrl::InsertColumn(LPCTSTR strHeading,
                            UINT nFormat /* = DT_CENTER|DT_VCENTER|DT_SINGLELINE */,
                            int nColumn  /* = -1 */)
{
	ASSERT(!m_AllowReorderColumn); // function not implemented in case of m_AllowReorderColumn option
    if (nColumn >= 0 && nColumn < m_nFixedCols)
    {

        // TODO: Fix it so column insertion works for in the fixed column area
        ASSERT(FALSE);
        return -1;
    }

    // If the insertion is for a specific column, check it's within range.
    if (nColumn >= 0 && nColumn > GetColumnCount())
        return -1;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    ResetSelectedRange();

    // Gotta be able to at least _see_ some of the column.
    if (m_nRows < 1)
        SetRowCount(1);

    // Allow the user to insert after the last of the columns, but process it as a
    // "-1" column, meaning it gets flaged as being the last column, and not a regular
    // "insert" routine.
    if (nColumn == GetColumnCount())
        nColumn = -1;

    TRY
    {
        if (nColumn < 0)
        {
            nColumn = m_nCols;
            m_arColWidths.Add(0);
            if (!GetVirtualMode())
            {
                for (int row = 0; row < m_nRows; row++)
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (!pRow)
                        return -1;
                    pRow->Add(CreateCell(row, nColumn));
                }
            }
        } 
        else
        {
            m_arColWidths.InsertAt(nColumn, (UINT)0);
            if (!GetVirtualMode())
            {
                for (int row = 0; row < m_nRows; row++) 
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (!pRow)
                        return -1;
                    pRow->InsertAt(nColumn, CreateCell(row, nColumn));
                }
            }
        }
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        return -1;
    }
    END_CATCH

    m_nCols++;
    
    // Initialise column data
    SetItemText(0, nColumn, strHeading);
    for (int row = 0; row < m_nRows; row++) 
        SetItemFormat(row, nColumn, nFormat);
    
    // initialized column width
    m_arColWidths[nColumn] = GetTextExtent(0, nColumn, strHeading).cx;
    
    if (m_idCurrentCell.col != -1 && nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col++;
    
    ResetScrollBars();

    SetModified();
    
    return nColumn;
}
void CGridCtrl::InsertRowBatchBegin()
{
    // Force recalculation
    m_idTopLeftCell.col = -1;	
	ResetSelectedRange();

}
void CGridCtrl::InsertRowBatchEnd()
{
	ResetScrollBars();
	SetModified();
}
int CGridCtrl::InsertRowBatchForOne ( LPCTSTR strHeading, int nRow)
{
    if (nRow >= 0 && nRow < m_nFixedRows)
    {
        // TODO: Fix it so column insertion works for in the fixed row area
        ASSERT(FALSE);
        return -1;
    }

    // If the insertion is for a specific row, check it's within range.
    if (nRow >= 0 && nRow >= GetRowCount())
        return -1;

    // Force recalculation
    m_idTopLeftCell.col = -1;
	
    // Gotta be able to at least _see_ some of the row.
    if (m_nCols < 1)
        SetColumnCount(1);

    TRY
    {
        // Adding a row to the bottom
        if (nRow < 0)
        {
            nRow = m_nRows;
            m_arRowHeights.Add(0);
            if (!GetVirtualMode())
                m_RowData.Add(new GRID_ROW);
			else
				m_arRowOrder.push_back(m_nRows);

        }
        else
        {
            m_arRowHeights.InsertAt(nRow, (UINT)0);
            if (!GetVirtualMode())
                m_RowData.InsertAt(nRow, new GRID_ROW);
			else
			{
				ResetVirtualOrder();
			}
        }

        if (!GetVirtualMode())
            m_RowData[nRow]->SetSize(m_nCols);
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        return -1;
    }
    END_CATCH

    m_nRows++;

    // Initialise cell data
    if (!GetVirtualMode())
    {
        for (int col = 0; col < m_nCols; col++)
        {
            GRID_ROW* pRow = m_RowData[nRow];
            if (!pRow)
                return -1;
            pRow->SetAt(col, CreateCell(nRow, col));
        }
    }

    // Set row title
    SetItemText(nRow, 0, strHeading);

    // initialized row height
	if ( IsRowHeightFixed() )
	{
		m_arRowHeights[nRow] = m_cellDefault.GetHeight();
	}
	else
	{
		if (strHeading && strHeading[0])
			m_arRowHeights[nRow] = GetTextExtent(nRow, 0, strHeading).cy;
		else
			m_arRowHeights[nRow] = m_cellFixedRowDef.GetHeight();
	}

    if (m_idCurrentCell.row != -1 && nRow < m_idCurrentCell.row)
        m_idCurrentCell.row++;


    return nRow;
}

// Insert a row at a given position, or add to end of rows (if nRow = -1)
int CGridCtrl::InsertRow(LPCTSTR strHeading, int nRow /* = -1 */)
{
    if (nRow >= 0 && nRow < m_nFixedRows)
    {
        // TODO: Fix it so column insertion works for in the fixed row area
        ASSERT(FALSE);
        return -1;
    }

    // If the insertion is for a specific row, check it's within range.
    if (nRow >= 0 && nRow >= GetRowCount())
        return -1;

    // Force recalculation
    m_idTopLeftCell.col = -1;
	
	ResetSelectedRange();
	
    // Gotta be able to at least _see_ some of the row.
    if (m_nCols < 1)
        SetColumnCount(1);

    TRY
    {
        // Adding a row to the bottom
        if (nRow < 0)
        {
            nRow = m_nRows;
            m_arRowHeights.Add(0);
            if (!GetVirtualMode())
                m_RowData.Add(new GRID_ROW);
			else
				m_arRowOrder.push_back(m_nRows);

        }
        else
        {
            m_arRowHeights.InsertAt(nRow, (UINT)0);
            if (!GetVirtualMode())
                m_RowData.InsertAt(nRow, new GRID_ROW);
			else
			{
				ResetVirtualOrder();
			}
        }

        if (!GetVirtualMode())
            m_RowData[nRow]->SetSize(m_nCols);
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        return -1;
    }
    END_CATCH

    m_nRows++;

    // Initialise cell data
    if (!GetVirtualMode())
    {
        for (int col = 0; col < m_nCols; col++)
        {
            GRID_ROW* pRow = m_RowData[nRow];
            if (!pRow)
                return -1;
            pRow->SetAt(col, CreateCell(nRow, col));
        }
    }

    // Set row title
    SetItemText(nRow, 0, strHeading);

    // initialized row height
	if ( IsRowHeightFixed() )
	{
		m_arRowHeights[nRow] = m_cellDefault.GetHeight();	// 统一使用默认的
	}
	else
	{
		if (strHeading && strHeading[0])
			m_arRowHeights[nRow] = GetTextExtent(nRow, 0, strHeading).cy;
		else
			m_arRowHeights[nRow] = m_cellFixedRowDef.GetHeight();
	}

    if (m_idCurrentCell.row != -1 && nRow < m_idCurrentCell.row)
        m_idCurrentCell.row++;

	ResetScrollBars();

	SetModified();

    return nRow;
}

///////////////////////////////////////////////////////////////////////////////
// Cell creation stuff

BOOL CGridCtrl::SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass)
{
    if (GetVirtualMode())
        return FALSE;

    ASSERT(IsValid(nRow, nCol));
    if (!IsValid(nRow, nCol))
        return FALSE;

    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return FALSE;
    }

    CGridCellBase* pNewCell = (CGridCellBase*) pRuntimeClass->CreateObject();

    CGridCellBase* pCurrCell = GetCell(nRow, nCol);
    if (pCurrCell)
        *pNewCell = *pCurrCell;

    SetCell(nRow, nCol, pNewCell);
    delete pCurrCell;

    return TRUE;
}

BOOL CGridCtrl::SetDefaultCellType( CRuntimeClass* pRuntimeClass)
{
    ASSERT( pRuntimeClass != NULL );
    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return FALSE;
    }
    m_pRtcDefault = pRuntimeClass;
    return TRUE;
}

// Creates a new grid cell and performs any necessary initialisation
/*virtual*/ CGridCellBase* CGridCtrl::CreateCell(int nRow, int nCol)
{
    ASSERT(!GetVirtualMode());

    if (!m_pRtcDefault || !m_pRtcDefault->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        ASSERT( FALSE);
        return NULL;
    }
    CGridCellBase* pCell = (CGridCellBase*) m_pRtcDefault->CreateObject();
    if (!pCell)
        return NULL;

    pCell->SetGrid(this);
    pCell->SetCoords(nRow, nCol); 

    if (nCol < m_nFixedCols)
        pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDCOL);
    if (nRow < m_nFixedRows)
        pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDROW);
    
    pCell->SetFormat(pCell->GetDefaultCell()->GetFormat());

    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
/*virtual*/ void CGridCtrl::DestroyCell(int nRow, int nCol)
{
    // Should NEVER get here in virtual mode.
    ASSERT(!GetVirtualMode());

    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    SetItemState(nRow, nCol, 0);

    delete GetCell(nRow, nCol);
}

BOOL CGridCtrl::DeleteColumn(int nColumn)
{
    if (nColumn < 0 || nColumn >= GetColumnCount())
        return FALSE;

    ResetSelectedRange();

    if (!GetVirtualMode())
    {
        for (int row = 0; row < GetRowCount(); row++)
        {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow)
                return FALSE;

            DestroyCell(row, nColumn);
        
            pRow->RemoveAt(nColumn);
        }
    }
    m_arColWidths.RemoveAt(nColumn);
    m_nCols--;
    if (nColumn < m_nFixedCols)
        m_nFixedCols--;
    
    if (nColumn == m_idCurrentCell.col)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col--;
    
    ResetScrollBars();

    SetModified();

    return TRUE;
}


void CGridCtrl::AddSubVirtualRow(int Num, int Nb)
{
	ASSERT(Nb == -1 || Nb ==1); // only these vlaues are implemented now
	if(!GetVirtualMode()) return;
	for(int ind = 0; ind<m_nRows ; ind++)
		if(m_arRowOrder[ind]>Num) m_arRowOrder[ind]+=Nb;
	if(Nb>0)
		m_arRowOrder.insert(m_arRowOrder.begin()+Num,Num);
	else
		m_arRowOrder.erase(m_arRowOrder.begin()+Num);
}


BOOL CGridCtrl::DeleteRow(int nRow)
{
    if (nRow < 0 || nRow >= GetRowCount())
        return FALSE;

    ResetSelectedRange();

    if (!GetVirtualMode())
    {
        GRID_ROW* pRow = m_RowData[nRow];
        if (!pRow)
            return FALSE;

        for (int col = 0; col < GetColumnCount(); col++)
            DestroyCell(nRow, col);

        delete pRow;
        m_RowData.RemoveAt(nRow);
    }
    else
    	AddSubVirtualRow(nRow,-1);

    m_arRowHeights.RemoveAt(nRow);

    m_nRows--;
    if (nRow < m_nFixedRows)
        m_nFixedRows--;
    
    if (nRow == m_idCurrentCell.row)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nRow < m_idCurrentCell.row)
        m_idCurrentCell.row--;
    
    ResetScrollBars();

    SetModified();
    
    return TRUE;
}

// Handy function that removes all non-fixed rows
BOOL CGridCtrl::DeleteNonFixedRows()
{
    ResetSelectedRange();
    int nFixed = GetFixedRowCount();
    int nCount = GetRowCount();
    if (GetVirtualMode())
	{
		if(nCount != nFixed)
		{
			SetRowCount(nFixed);
			m_arRowOrder.resize(nFixed);
			m_arRowHeights.SetSize(nFixed);
			m_idCurrentCell.row = m_idCurrentCell.col = -1;
			ResetScrollBars();
			SetModified();
		}
	}
	else
	{
    // Delete all data rows
    for (int nRow = nCount; nRow >= nFixed; nRow--)
        DeleteRow(nRow);
	}
    return TRUE;
}

// Removes all rows, columns and data from the grid.
BOOL CGridCtrl::DeleteAllItems()
{
    ResetSelectedRange();

    m_arColWidths.RemoveAll();
    m_arRowHeights.RemoveAll();

    // Delete all cells in the grid
    if (!GetVirtualMode())
    {
        for (int row = 0; row < m_nRows; row++)
        {
            for (int col = 0; col < m_nCols; col++)
                DestroyCell(row, col);

            GRID_ROW* pRow = m_RowData[row];
            delete pRow;
        }

        // Remove all rows
        m_RowData.RemoveAll();
    }
	else
		m_arRowOrder.clear();


    m_idCurrentCell.row = m_idCurrentCell.col = -1;
    m_nRows = m_nFixedRows = m_nCols = m_nFixedCols = 0;

    ResetScrollBars();

    SetModified();

    return TRUE;
}

void CGridCtrl::AutoFill()
{
    if (!::IsWindow(m_hWnd))
        return;

    CRect rect;
    GetClientRect(rect);

    SetColumnCount(rect.Width() / m_cellDefault.GetWidth() + 1);
    SetRowCount(rect.Height() / m_cellDefault.GetHeight() + 1);
    SetFixedRowCount(1);
    SetFixedColumnCount(1);
    ExpandToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

// Set CListCtrl::GetNextItem for details
CCellID CGridCtrl::GetNextItem(CCellID& cell, int nFlags) const
{
    if ((nFlags & GVNI_ALL) == GVNI_ALL)
    {    // GVNI_ALL Search whole Grid beginning from cell
        //          First row (cell.row) -- ONLY Columns to the right of cell
        //          following rows       -- ALL  Columns
        int row = cell.row , col = cell.col + 1;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++)
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return CCellID(row, col);
            }
            // go to First Column
            col = GetFixedColumnCount();
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW && 
             (nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {   // GVNI_AREA Search Grid beginning from cell to Lower-Right of Grid
        //           Only rows starting with  cell.row and below
        //           All rows   -- ONLY Columns to the right of cell
        int row = cell.row;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            int col = cell.col + 1;
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++) 
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return CCellID(row, col);
            }
        }
    }
    else if ((nFlags & GVNI_ABOVE) == GVNI_ABOVE) 
    {
        for (int row = cell.row - 1; row >= GetFixedRowCount(); row--) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(row, cell.col);
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW)
    {
        for (int row = cell.row + 1; row < GetRowCount(); row++) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(row, cell.col);
        }
    } 
    else if ((nFlags & GVNI_TOLEFT) == GVNI_TOLEFT)
    {
        for (int col = cell.col - 1; col >= GetFixedColumnCount(); col--) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(cell.row, col);
        }
    }
    else if ((nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {
        for (int col = cell.col + 1; col < GetColumnCount(); col++) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return CCellID(cell.row, col);
        }
    }
    
    return CCellID(-1, -1);
}

// Sorts on a given column using the cell text
BOOL CGridCtrl::SortTextItems(int nCol, BOOL bAscending, LPARAM data /* = 0 */)
{
    return CGridCtrl::SortItems(pfnCellTextCompare, nCol, bAscending, data);
}

void CGridCtrl::SetCompareFunction(PFNLVCOMPARE pfnCompare)
{
	m_pfnCompare = pfnCompare;
}

// Sorts on a given column using the cell text and using the specified comparison
// function
/*
BOOL CGridCtrl::SortItems(int nCol, BOOL bAscending, LPARAM data)
{
    SetSortColumn(nCol);
    SetSortAscending(bAscending);
    ResetSelectedRange();
    SetFocusCell(-1, - 1);


	// 根据点击列的Cell 类型 设置排序类型,目前只增加这一种特殊类型的排序.
	if (GetCell(1,nCol))
	{
		if (GetCell(1,nCol)->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)))
		{
			m_pfnCompare = pfnCellSymbolCompare;
		}		
	}
	if (m_pfnCompare == NULL)
		return CGridCtrl::SortItems(pfnCellTextCompare, nCol, bAscending, data);
	else
	    return CGridCtrl::SortItems(m_pfnCompare, nCol, bAscending, data);
}
*/
BOOL CGridCtrl::SortItems(int nCol, BOOL bAscending, LPARAM data /* = 0 */)
{
    SetSortColumn(nCol);
    SetSortAscending(bAscending);
    ResetSelectedRange();
    SetFocusCell(-1, - 1);

	CGridCellBase* pCell = GetCell(1,nCol);
	if ( NULL != pCell && FLT_MAX != pCell->GetCompareFloat())
	{
		m_pfnCompare = pfnCellNumericCompare;
	}
	else
	{
		m_pfnCompare = pfnCellTextCompare;
	}
	return CGridCtrl::SortItems(m_pfnCompare, nCol, bAscending, data);
}

// Sorts on a given column using the supplied compare function (see CListCtrl::SortItems)
BOOL CGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending,
                          LPARAM data /* = 0 */)
{
    SetSortColumn(nCol);
    SetSortAscending(bAscending);
    ResetSelectedRange();
    SetFocusCell(-1, -1);
    return SortItems(pfnCompare, nCol, bAscending, data, GetFixedRowCount(), -1);
}

int CALLBACK CGridCtrl::pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//lint --e(522)
	UNUSED_ALWAYS(lParamSort);

 	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	CString StrCell1 =  pCell1->GetText();
	CString StrCell2 =  pCell2->GetText();

	//int returnvalue = _tcscmp(StrCell1, StrCell2);
	int returnvalue = CompareString(LOCALE_SYSTEM_DEFAULT, 0, StrCell1, -1, StrCell2, -1) - 2;
	ASSERT(returnvalue > -2 );
	return returnvalue;
}
/*
int CALLBACK CGridCtrl::pfnCellSymbolCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNUSED_ALWAYS(lParamSort);

 	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	CString StrCell1 =  pCell1->GetText();
	CString StrCell2 =  pCell2->GetText();

	
	if ( (StrCell1.GetAt(0)=='+'||StrCell1.GetAt(0)=='-') && (StrCell2.GetAt(0)=='+'||StrCell2.GetAt(0)=='-') )
	{
		StrCell1 = StrCell1.Right(StrCell1.GetLength()-1);
		StrCell2 = StrCell2.Right(StrCell2.GetLength()-1);
	}
	return _tcscmp(StrCell1, StrCell2);
}
int CALLBACK CGridCtrl::pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNUSED_ALWAYS(lParamSort);

	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	int nValue1 = _ttol(pCell1->GetText());
	int nValue2 = _ttol(pCell2->GetText());

	if (nValue1 < nValue2)
		return -1;
	else if (nValue1 == nValue2)
		return 0;
	else
		return 1;
}
*/
int CALLBACK CGridCtrl::pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//lint --e(522)
	UNUSED_ALWAYS(lParamSort);

	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	float fValue1 = pCell1->GetCompareFloat();
	float fValue2 = pCell2->GetCompareFloat();

	if (fValue1 < fValue2)
		return -1;
	else if (fValue1 == fValue2)
		return 0;
	else
		return 1;
}
CGridCtrl *  CGridCtrl::m_This;
// private recursive sort implementation
bool CGridCtrl::NotVirtualCompare(int c1, int c2)
{
	return ! CGridCtrl::m_This->m_pfnVirtualCompare(c1, c2);
}
BOOL CGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                          int low, int high)
{
    if (nCol >= GetColumnCount())
        return FALSE;

    if (high == -1)
        high = GetRowCount() - 1;
	
// 	if ( -1 != m_nLowSortRow)
// 	{
// 		low = m_nLowSortRow;
// 	}
// 
// 	if ( -1 != m_nHightSortRow )
// 	{
// 		high = m_nHightSortRow;
//	}
	
    int lo = low;
    int hi = high;
    
    if (hi <= lo)
        return FALSE;

    if (GetVirtualMode())
	{
		ASSERT(m_pfnVirtualCompare);
		m_CurCol = m_arColOrder[nCol];
		m_This = this;
		if ( bAscending )
		{
			std::stable_sort(m_arRowOrder.begin() + m_nFixedRows, m_arRowOrder.end(), 
                         m_pfnVirtualCompare);
		}
		else
		{
			std::stable_sort(m_arRowOrder.rbegin(), m_arRowOrder.rend()-m_nFixedRows, 
                         m_pfnVirtualCompare);
		}
		
		return TRUE;
	}
   
    
    //LPARAM midItem = GetItemData((lo + hi)/2, nCol);
	LPARAM pMidCell = (LPARAM) GetCell((lo + hi)/2, nCol);
    
    // loop through the list until indices cross
    while (lo <= hi)
    {
        // Find the first element that is greater than or equal to the partition 
        // element starting from the left Index.
        if (bAscending)
            while (lo < high  && pfnCompare( (LPARAM)GetCell(lo, nCol), (LPARAM) pMidCell, data) < 0)
                ++lo;
        else
            while (lo < high && pfnCompare((LPARAM)GetCell(lo, nCol), pMidCell, data) > 0)
				++lo;
                
		// Find an element that is smaller than or equal to  the partition 
		// element starting from the right Index.
		if (bAscending)
			while (hi > low && pfnCompare((LPARAM)GetCell(hi, nCol), pMidCell, data) > 0)
				--hi;
		else
			while (hi > low && pfnCompare((LPARAM)GetCell(hi, nCol), pMidCell, data) < 0)
				--hi;
                        
        // If the indexes have not crossed, swap if the items are not equal
        if (lo <= hi)
        {
			// swap only if the items are not equal
            if (pfnCompare((LPARAM)GetCell(lo, nCol), (LPARAM)GetCell(hi, nCol), data) != 0)
            {
				for (int col = 0; col < GetColumnCount(); col++)
                {
					CGridCellBase *pCell = GetCell(lo, col);
                    SetCell(lo, col, GetCell(hi, col));
                    SetCell(hi, col, pCell);
                }
                UINT nRowHeight = m_arRowHeights[lo];
                m_arRowHeights[lo] = m_arRowHeights[hi];
                m_arRowHeights[hi] = nRowHeight;
            }
                            
            ++lo;
            --hi;
         }
    }
    
    // If the right index has not reached the left side of array
    // must now sort the left partition.
    if (low < hi)
        SortItems(pfnCompare, nCol, bAscending, data, low, hi);
    
    // If the left index has not reached the right side of array
    // must now sort the right partition.
    if (lo < high)
        SortItems(pfnCompare, nCol, bAscending, data, lo, high);
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

BOOL CGridCtrl::SetItem(const GV_ITEM* pItem)
{
    if (!pItem || GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell)
        return FALSE;

    SetModified(TRUE, pItem->row, pItem->col);

    if (pItem->mask & GVIF_TEXT)
        pCell->SetText(pItem->strText);
    if (pItem->mask & GVIF_PARAM)
        pCell->SetData(pItem->lParam);
    if (pItem->mask & GVIF_IMAGE)
        pCell->SetImage(pItem->iImage);
    if (pItem->mask & GVIF_STATE)
        SetItemState(pItem->row, pItem->col, pItem->nState);
    if (pItem->mask & GVIF_FORMAT)
        pCell->SetFormat(pItem->nFormat);
    if (pItem->mask & GVIF_BKCLR)
        pCell->SetBackClr(pItem->crBkClr);
    if (pItem->mask & GVIF_FGCLR)
        pCell->SetTextClr(pItem->crFgClr);
    if (pItem->mask & GVIF_FONT)
        pCell->SetFont(&(pItem->lfFont));
    if( pItem->mask & GVIF_MARGIN)
        pCell->SetMargin( pItem->nMargin);
    
    return TRUE;
}

BOOL CGridCtrl::GetItem(GV_ITEM* pItem)
{
    if (!pItem)
        return FALSE;
    CGridCellBase* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell)
        return FALSE;

    if (pItem->mask & GVIF_TEXT)
        pItem->strText = GetItemText(pItem->row, pItem->col);
    if (pItem->mask & GVIF_PARAM)
        pItem->lParam  = pCell->GetData();;
    if (pItem->mask & GVIF_IMAGE)
        pItem->iImage  = pCell->GetImage();
    if (pItem->mask & GVIF_STATE)
        pItem->nState  = pCell->GetState();
    if (pItem->mask & GVIF_FORMAT)
        pItem->nFormat = pCell->GetFormat();
    if (pItem->mask & GVIF_BKCLR)
        pItem->crBkClr = pCell->GetBackClr();
    if (pItem->mask & GVIF_FGCLR)
        pItem->crFgClr = pCell->GetTextClr();
    if (pItem->mask & GVIF_FONT)
        memcpyex(&(pItem->lfFont), pCell->GetFont(), sizeof(LOGFONT));
    if( pItem->mask & GVIF_MARGIN)
        pItem->nMargin = pCell->GetMargin();

    return TRUE;
}

BOOL CGridCtrl::SetItemText(int nRow, int nCol, LPCTSTR str)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetText(str);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 210)
// EFW - 06/13/99 - Added to support printf-style formatting codes
BOOL CGridCtrl::SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...)
{
    if (GetVirtualMode())
        return FALSE;

    CString strText;

    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    // Format the message text
    va_start(argptr, szFmt);
    strText.FormatV(szFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

// EFW - 06/13/99 - Added to support string resource ID.  Supports
// a variable argument list too.
BOOL CGridCtrl::SetItemTextFmtID(int nRow, int nCol, UINT nID, ...)
{
    if (GetVirtualMode())
        return FALSE;

    CString strFmt, strText;
    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    // Format the message text
    va_start(argptr, nID);
    VERIFY(strFmt.LoadString(nID));
    strText.FormatV(strFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    SetModified(TRUE, nRow, nCol);
    return TRUE;
}
#endif

BOOL CGridCtrl::SetItemData(int nRow, int nCol, LPARAM lParam)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetData(lParam);
    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

LPARAM CGridCtrl::GetItemData(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return (LPARAM) 0;

    return pCell->GetData();
}

BOOL CGridCtrl::SetItemImage(int nRow, int nCol, int iImage)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetImage(iImage);
    SetModified(TRUE, nRow, nCol);
    return TRUE;
}

int CGridCtrl::GetItemImage(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return -1;

    return pCell->GetImage();
}

BOOL CGridCtrl::SetItemState(int nRow, int nCol, UINT state)
{
    BOOL bSelected = IsCellSelected(nRow, nCol);

    // If the cell is being unselected, remove it from the selected list
    if (bSelected && !(state & GVIS_SELECTED))
    {
        CCellID cell;
        DWORD key = MAKELONG(nRow, nCol);

        if (m_SelectedCellMap.Lookup(key, (CCellID&)cell))
            m_SelectedCellMap.RemoveKey(key);
    }

    // If cell is being selected, add it to the list of selected cells
    else if (!bSelected && (state & GVIS_SELECTED))
    {
        CCellID cell(nRow, nCol);
        m_SelectedCellMap.SetAt(MAKELONG(nRow, nCol), cell);
    }

    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    // Set the cell's state
    pCell->SetState(state);

    return TRUE;
}

UINT CGridCtrl::GetItemState(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
	ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetState();
}

BOOL CGridCtrl::SetItemFormat(int nRow, int nCol, UINT nFormat)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    pCell->SetFormat(nFormat);
    return TRUE;
}

UINT CGridCtrl::GetItemFormat(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetFormat();
}

BOOL CGridCtrl::SetItemBkColour(int nRow, int nCol, COLORREF cr /* = CLR_DEFAULT */)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    pCell->SetBackClr(cr);
    return TRUE;
}

COLORREF CGridCtrl::GetItemBkColour(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->GetBackClr();
}

BOOL CGridCtrl::SetItemFgColour(int nRow, int nCol, COLORREF cr /* = CLR_DEFAULT */)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;
    
    pCell->SetTextClr(cr);
    return TRUE;
}

COLORREF CGridCtrl::GetItemFgColour(int nRow, int nCol) const
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return 0;
    
    return pCell->GetTextClr();
}
BOOL CGridCtrl::SetItemTextPadding(int nRow, int nCol, const CRect rtTextPadding)
{
	if (GetVirtualMode())
		return FALSE;

	CGridCellBase* pCell = GetCell(nRow, nCol);
	ASSERT(pCell);
	if (!pCell)
		return FALSE;

	pCell->SetTextPadding(rtTextPadding);

	return TRUE;
}

BOOL CGridCtrl::SetItemFont(int nRow, int nCol, const LOGFONT* plf)
{
    if (GetVirtualMode())
        return FALSE;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;
    
    pCell->SetFont(plf);
    
    return TRUE;
}

const LOGFONT* CGridCtrl::GetItemFont(int nRow, int nCol)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) 
        return GetDefaultCell(nRow < GetFixedRowCount(), nCol < GetFixedColumnCount())->GetFont();
    
    return pCell->GetFont();
}

BOOL CGridCtrl::IsItemEditing(int nRow, int nCol)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return FALSE;

    return pCell->IsEditing();
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column size functions

long CGridCtrl::GetVirtualWidth() const
{
    long lVirtualWidth = 0;
    int iColCount = GetColumnCount();
    for (int i = 0; i < iColCount; i++)
        lVirtualWidth += m_arColWidths[i];

    return lVirtualWidth;
}

long CGridCtrl::GetVirtualHeight() const
{
    long lVirtualHeight = 0;
    int iRowCount = GetRowCount();
    for (int i = 0; i < iRowCount; i++)
        lVirtualHeight += m_arRowHeights[i];

    return lVirtualHeight;
}

int CGridCtrl::GetRowHeight(int nRow) const
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1;

    return m_arRowHeights[nRow];
}

int CGridCtrl::GetColumnWidth(int nCol) const
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1;

    return m_arColWidths[m_arColOrder[nCol]];
}

BOOL CGridCtrl::SetRowHeight(int nRow, int height)
{
    ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0)
        return FALSE;

    m_arRowHeights[nRow] = height;
    ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::SetColumnWidth(int nCol, int width)
{
    ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0)
        return FALSE;

    m_arColWidths[m_arColOrder[nCol]] = width;
    ResetScrollBars();

    return TRUE;
}

int CGridCtrl::GetFixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++)
        nHeight += GetRowHeight(i);

    return nHeight;
}

int CGridCtrl::GetFixedColumnWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++)
        nWidth += GetColumnWidth(i);

    return nWidth;
}

BOOL CGridCtrl::AutoSizeColumn(int nCol, UINT nAutoSizeStyle /*=GVS_DEFAULT*/, BOOL bResetScroll /*=TRUE*/)
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return FALSE;

    //  Skip hidden columns when autosizing
    if( GetColumnWidth( nCol) <=0 )
        return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return FALSE;

    int nWidth = 0;	

    ASSERT(nAutoSizeStyle <= GVS_BOTH);
    if (nAutoSizeStyle == GVS_DEFAULT)
        nAutoSizeStyle = GetAutoSizeStyle();

    int nStartRow = (nAutoSizeStyle & GVS_HEADER)? 0 : GetFixedRowCount();
    int nEndRow   = (nAutoSizeStyle & GVS_DATA)? GetRowCount()-1 : GetFixedRowCount()-1;

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(nStartRow, nCol, nEndRow, nCol));

    for (int nRow = nStartRow; nRow <= nEndRow; nRow++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);
        if (size.cx > nWidth)
			nWidth = size.cx;
    }

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(-1,-1,-1,-1));

	// 如果没有数据可调整，不应当重置为0的宽度，而应该保持原来的宽度 xl 0809
	if ( nWidth <= 0 )
	{
		nWidth = GetDefCellWidth();	// 取默认宽度
	}
    m_arColWidths[nCol] = nWidth;

    ReleaseDC(pDC);
    if (bResetScroll)
        ResetScrollBars();

    return TRUE;
}

BOOL CGridCtrl::AutoSizeRow(int nRow, BOOL bResetScroll /*=TRUE*/)
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return FALSE;

    //  Skip hidden rows when autosizing
    if( GetRowHeight( nRow) <=0 )
        return FALSE;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return FALSE;

    int nHeight = 0;		
    int nNumColumns = GetColumnCount();

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(nRow, 0, nRow, nNumColumns));

	if ( IsRowHeightFixed() )
	{
		nHeight = m_cellDefault.GetHeight();
	}
	else
	{
		for (int nCol = 0; nCol < nNumColumns; nCol++)
		{
			CGridCellBase* pCell = GetCell(nRow, nCol);
			if (pCell)
				size = pCell->GetCellExtent(pDC);
			if (size.cy > nHeight)
				nHeight = size.cy;
		}
	}

	// 由于以前存在高度，所以至少保持默认 xl 0810
	if ( nHeight <= 0 )
	{
		nHeight = GetDefCellHeight();
	}
    m_arRowHeights[nRow] = nHeight;

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(-1,-1,-1,-1));

    ReleaseDC(pDC);
    if (bResetScroll)
        ResetScrollBars();

    return TRUE;
}

void CGridCtrl::AutoSizeColumns(UINT nAutoSizeStyle /*=GVS_DEFAULT*/)
{
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Skip hidden columns when autosizing
        if( GetColumnWidth( nCol) > 0 )
            AutoSizeColumn(nCol, nAutoSizeStyle, FALSE);
    }
    ResetScrollBars();
}

void CGridCtrl::AdjustColumnsWidth()
{
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {   //  Skip hidden columns when autosizing
        if( GetColumnWidth( nCol) > 0 )
        {
			ASSERT(nCol >= 0 && nCol < m_nCols);
			if (nCol < 0 || nCol >= m_nCols)
				return ;
			
			CSize size;
			CDC* pDC = GetDC();
			if (!pDC)
				return ;
			
			int nWidth = 0;	
			
			if (GetVirtualMode())
				SendCacheHintToParent(CCellRange(0, nCol, GetRowCount() - 1, nCol));
			
			int nTempWidth = 0;
			for (int nRow = 0; nRow < GetRowCount(); nRow++)
			{
				CGridCellBase* pCell = GetCell(nRow, nCol);
				if(pCell)
				{
					CFont *pFont = GetFont();
					LOGFONT logFont;
					pFont->GetLogFont(&logFont);
					int nSizeFont = abs(logFont.lfHeight);
					if(nSizeFont == 0)
					{
						nSizeFont = 15;
					}

					CString strText = pCell->GetText();
					CFontFactory fontFactory;
					FontFamily fontFamily(fontFactory.GetExistFontName(L"微软雅黑"));//...
					Gdiplus::Font font(&fontFamily, nSizeFont, FontStyleRegular, UnitPixel);	//显示Name的字体
					
					Graphics graphics(this->m_hWnd);
					RectF rectF(0, 0, 0, nSizeFont);	//
					RectF boundRect;	//最后能够容纳文字的最适合矩形
					int codepointFitted = 0;	//文字的个数
					int linesFiled = 0;	//文字的行数
					graphics.MeasureString(strText, strText.GetLength(), &font, rectF, NULL, &boundRect, &codepointFitted, &linesFiled);
					nTempWidth = int(boundRect.Width + 1);
					if(nTempWidth > nWidth)
					{
						nWidth = nTempWidth;
					}
				}
			}
			nWidth += 10;
			
			if (GetVirtualMode())
				SendCacheHintToParent(CCellRange(-1,-1,-1,-1));
			
			// 如果没有数据可调整，不应当重置为0的宽度，而应该保持原来的宽度 xl 0809
			if ( nWidth <= 0 )
			{
				nWidth = GetDefCellWidth();	// 取默认宽度
			}
			m_arColWidths[nCol] = nWidth;
			
			ReleaseDC(pDC);
		}
    }
    ResetScrollBars();
}

void CGridCtrl::AutoSizeRows()
{
    int nNumRows = GetRowCount();
    for (int nRow = 0; nRow < nNumRows; nRow++)
    {
        //  Skip hidden rows when autosizing
        if( GetRowHeight( nRow) > 0 )
            AutoSizeRow(nRow, FALSE);
    }
    ResetScrollBars();
}

// sizes all rows and columns
// faster than calling both AutoSizeColumns() and AutoSizeRows()
void CGridCtrl::AutoSize(UINT nAutoSizeStyle /*=GVS_DEFAULT*/)
{
    CDC* pDC = GetDC();
    if (!pDC)
        return;

    int nNumColumns = GetColumnCount();

    int nCol, nRow;

    ASSERT(nAutoSizeStyle <= GVS_BOTH);
    if (nAutoSizeStyle == GVS_DEFAULT)
        nAutoSizeStyle = GetAutoSizeStyle();

    int nStartRow = (nAutoSizeStyle & GVS_HEADER)? 0 : GetFixedRowCount();
    int nEndRow   = (nAutoSizeStyle & GVS_DATA)? GetRowCount()-1 : GetFixedRowCount()-1;

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(nStartRow, 0, nEndRow, nNumColumns));

	if ( IsRowHeightFixed() )
	{
		// Row initialisation - only work on rows whose height is > 0
		for (nRow = nStartRow; nRow <= nEndRow; nRow++)
		{
			if( GetRowHeight( nRow) > 0 )
				m_arRowHeights[nRow] = m_cellDefault.GetHeight();	// 固定行高
		}
		
		CSize size;
		for (nCol = 0; nCol < nNumColumns; nCol++)
		{
			//  Don't size hidden columns or rows
			if( GetColumnWidth( nCol) > 0 )
			{
				// Skip columns that are hidden, but now initialize
				m_arColWidths[nCol] = 0;
				for (nRow = nStartRow; nRow <= nEndRow; nRow++)
				{
					if( GetRowHeight( nRow) > 0 )
					{
						size = CSize(0,0);
						CGridCellBase* pCell = GetCell(nRow, nCol);
						if (pCell)
							size = pCell->GetCellExtent(pDC);
						if (size.cx >(int) m_arColWidths[nCol])
							m_arColWidths[nCol] = size.cx;
					}
				}
			}
		}
	}
	else
	{
		// Row initialisation - only work on rows whose height is > 0
		for (nRow = nStartRow; nRow <= nEndRow; nRow++)
		{
			if( GetRowHeight( nRow) > 0 )
				m_arRowHeights[nRow] = 1;
		}
		
		CSize size;
		for (nCol = 0; nCol < nNumColumns; nCol++)
		{
			//  Don't size hidden columns or rows
			if( GetColumnWidth( nCol) > 0 )
			{
				// Skip columns that are hidden, but now initialize
				m_arColWidths[nCol] = 0;
				for (nRow = nStartRow; nRow <= nEndRow; nRow++)
				{
					if( GetRowHeight( nRow) > 0 )
					{
						size = CSize(0,0);
						CGridCellBase* pCell = GetCell(nRow, nCol);
						if (pCell)
							size = pCell->GetCellExtent(pDC);
						if (size.cx >(int) m_arColWidths[nCol])
							m_arColWidths[nCol] = size.cx;
						if (size.cy >(int) m_arRowHeights[nRow])
							m_arRowHeights[nRow] = size.cy;
					}
				}
			}
		}
	}

    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(-1,-1,-1,-1));

    ReleaseDC(pDC);

    ResetScrollBars();
    Refresh();
}

// Expands the columns to fit the screen space. If bExpandFixed is FALSE then fixed 
// columns will not be affected
void CGridCtrl::ExpandColumnsToFit(BOOL bExpandFixed /*=TRUE*/)
{
    if (bExpandFixed)
	{
		if (GetColumnCount() <= 0) return;
	} 
	else
	{
		if (GetColumnCount() <= GetFixedColumnCount()) return;
	}

    EnableScrollBars(SB_HORZ, FALSE);

    int col;
    CRect rect;
    GetClientRect(rect);

    int nFirstColumn = (bExpandFixed)? 0 : GetFixedColumnCount();

    int nNumColumnsAffected = 0;
    for (col = nFirstColumn; col < GetColumnCount(); col++)
    {
        if (m_arColWidths[col] > 0)
            nNumColumnsAffected++;
    }

    if (nNumColumnsAffected <= 0)
        return;

    long virtualWidth = GetVirtualWidth();
    int nDifference = rect.Width() -(int) virtualWidth;
    int nColumnAdjustment = nDifference / nNumColumnsAffected;

    for (col = nFirstColumn; col < GetColumnCount(); col++)
    {
        if (m_arColWidths[col] > 0)
            m_arColWidths[col] += nColumnAdjustment;    
    }

    if (nDifference > 0)
    {
        int leftOver = nDifference % nNumColumnsAffected;
        for (int nCount = 0, iCol = nFirstColumn; 
             (iCol < GetColumnCount()) && (nCount < leftOver); iCol++, nCount++)
        {
            if (m_arColWidths[iCol] > 0)
                m_arColWidths[iCol] += 1;
        }
    }
    else 
    {
        int leftOver = (-nDifference) % nNumColumnsAffected;
        for (int nCount = 0, iCol = nFirstColumn; 
             (iCol < GetColumnCount()) && (nCount < leftOver); iCol++, nCount++)
        {
            if (m_arColWidths[iCol] > 0)
                m_arColWidths[iCol] -= 1;
        }
    }

    Refresh();

    ResetScrollBars();
}

void CGridCtrl::ExpandLastColumn()
{
    if (GetColumnCount() <= 0)
        return;

    // Search for last non-hidden column
    int nLastColumn = GetColumnCount()-1;
    while (m_arColWidths[nLastColumn] <= 0)
        nLastColumn--;

    if (nLastColumn <= 0)
        return;

    EnableScrollBars(SB_HORZ, FALSE);

    CRect rect;
    GetClientRect(rect);

    long virtualWidth = GetVirtualWidth();
    int nDifference = rect.Width() -(int) virtualWidth;

    if (nDifference > 0)
    {
        //if (GetVirtualHeight() > rect.Height())
        //    nDifference -= GetSystemMetrics(SM_CXVSCROLL);

        m_arColWidths[ nLastColumn ] += nDifference;
        Refresh();
    }

    ResetScrollBars();
}

// Expands the rows to fit the screen space. If bExpandFixed is FALSE then fixed 
// rows will not be affected
void CGridCtrl::ExpandRowsToFit(BOOL bExpandFixed /*=TRUE*/)
{
    if (bExpandFixed)
	{
		if (GetRowCount() <= 0) return;
	} 
	else
	{
		if (GetRowCount() <= GetFixedRowCount()) return;
	}

    EnableScrollBars(SB_VERT, FALSE); 

    int row;
    CRect rect;
    GetClientRect(rect);
    
    int nFirstRow = (bExpandFixed)? 0 : GetFixedRowCount();

    int nNumRowsAffected = 0;
    for (row = nFirstRow; row < GetRowCount(); row++)
    {
        if (m_arRowHeights[row] > 0)
            nNumRowsAffected++;
    }

    if (nNumRowsAffected <= 0)
        return;

    long virtualHeight = GetVirtualHeight();
    int nDifference = rect.Height() -(int) virtualHeight;
    int nRowAdjustment = nDifference / nNumRowsAffected;
    
    for (row = nFirstRow; row < GetRowCount(); row++)
    {
        if (m_arRowHeights[row] > 0)
            m_arRowHeights[row] += nRowAdjustment;    
    }
    
    if (nDifference > 0)
    {
        int leftOver = nDifference % nNumRowsAffected;
        for (int nCount = 0, iRow = nFirstRow; 
             (iRow < GetRowCount()) && (nCount < leftOver); iRow++, nCount++)
        {
            if (m_arRowHeights[iRow] > 0)
                m_arRowHeights[iRow] += 1;
        }
    } 
    else 
    {
        int leftOver = (-nDifference) % nNumRowsAffected;
        for (int nCount = 0, iRow = nFirstRow; 
             (iRow < GetRowCount()) && (nCount < leftOver); iRow++, nCount++)
        {
            if (m_arRowHeights[iRow] > 0)
                m_arRowHeights[iRow] -= 1;
        }
    }

    Refresh();

    ResetScrollBars();
}

// Expands the cells to fit the screen space. If bExpandFixed is FALSE then fixed 
// cells  will not be affected
void CGridCtrl::ExpandToFit(BOOL bExpandFixed /*=TRUE*/)
{
    ExpandColumnsToFit(bExpandFixed);   // This will remove any existing horz scrollbar
    ExpandRowsToFit(bExpandFixed);      // This will remove any existing vert scrollbar
//    ExpandColumnsToFit(bExpandFixed);   // Just in case the first adjustment was with a vert
                                        // scrollbar in place
    Refresh();
}

/////////////////////////////////////////////////////////////////////////////////////
// Attributes

void CGridCtrl::SetVirtualMode(BOOL bVirtual)
{
    DeleteAllItems();
    m_bVirtualMode = bVirtual;

    // Force some defaults here.
    if (m_bVirtualMode)
    {
        SetEditable(FALSE);
        SetAutoSizeStyle(GVS_HEADER);
        SetFixedColumnSelection(FALSE);
        SetFixedRowSelection(FALSE);
		ResetVirtualOrder();
    }
}

void CGridCtrl::SetGridLines(int nWhichLines /*=GVL_BOTH*/) 
{
    m_nGridLines = nWhichLines;
    Refresh();
}

void CGridCtrl::SetListMode(BOOL bEnableListMode /*=TRUE*/)
{
    ResetSelectedRange();
    SetSortColumn(-1);
    m_bListMode = bEnableListMode;
    SetFixedRowSelection(FALSE);
    Refresh();
}

void CGridCtrl::SetSortColumn(int nCol)
{
    if (m_nSortColumn >= 0)
        InvalidateCellRect(0, m_nSortColumn);
    m_nSortColumn = nCol;
    if (nCol >= 0)
        InvalidateCellRect(0, nCol);
}

BOOL CGridCtrl::IsCellFixed(int nRow, int nCol)
{
    return (nRow < GetFixedRowCount() || nCol < GetFixedColumnCount());
}

void CGridCtrl::SetModified(BOOL bModified /*=TRUE*/, int nRow /*=-1*/, int nCol /*=-1*/)
{
    // Cannot guarantee sorting anymore...
    if (nCol < 0 || nCol == GetSortColumn())
        SetSortColumn(-1);

    if (nRow >= 0 && nCol >= 0)
    {
        if (bModified)
        {
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) | GVIS_MODIFIED);
            m_bModified = TRUE;
        }
        else
            SetItemState(nRow, nCol, GetItemState(nRow, nCol) & ~GVIS_MODIFIED);
    }
    else
        m_bModified = bModified;

    if (!m_bModified)
    {
        for (int row = 0; row < GetRowCount(); row++)
            for (int col = 0; col < GetColumnCount(); col++)
                SetItemState(row, col, GetItemState(row, col) & ~GVIS_MODIFIED);
    }
}

BOOL CGridCtrl::GetModified(int nRow /*=-1*/, int nCol /*=-1*/)
{
    if (nRow >= 0 && nCol >= 0)
        return ( (GetItemState(nRow, nCol) & GVIS_MODIFIED) == GVIS_MODIFIED );
    else
        return m_bModified;
}

/////////////////////////////////////////////////////////////////////////////////////
// GridCtrl cell visibility tests and invalidation/redraw functions

void CGridCtrl::Refresh()
{
    if (GetSafeHwnd() && m_bAllowDraw)
        Invalidate();
}

// EnsureVisible supplied by Roelf Werkman
void CGridCtrl::EnsureVisible(int nRow, int nCol)
{
    if (!m_bAllowDraw)
        return;

	if ( nRow == 0 && nCol == 0 )
	{
		SetScrollPos32(SB_VERT, 0, TRUE);
		SetScrollPos32(SB_HORZ, 0, TRUE);
		return;
	}

    CRect rectWindow;
    /*
    // set the scroll to the approximate position of row (Nigel Page-Jones)
    int nPos = (int)((float)nRow / GetRowCount() * 1000);
    float fPos = (float)nPos / 1000;
    SCROLLINFO scrollInfo;
    GetScrollInfo(SB_VERT, &scrollInfo);
    scrollInfo.nPos = (int)(scrollInfo.nMax * fPos);
    SetScrollInfo(SB_VERT, &scrollInfo, FALSE);
    
    GetClientRect(rectWindow);

    // redraw cells    if necessary (Nigel Page-Jones)
    CCellID idTopLeft = GetTopleftNonFixedCell(FALSE);
    CCellID idNewTopLeft = GetTopleftNonFixedCell(TRUE);
    if (idNewTopLeft != idTopLeft)
    {
        rectWindow.top = GetFixedRowHeight();
        InvalidateRect(rectWindow);
    }
    */

    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    CWnd* pFocusWnd = GetFocus();
	//ASSERT( NULL != pFocusWnd );

    //CCellRange VisibleCells = GetVisibleNonFixedCellRange();
	CCellRange VisibleCells = GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);	// 必须完整可见

    int iRight = nCol - VisibleCells.GetMaxCol();
    int iLeft  = VisibleCells.GetMinCol() - nCol;
    int down  = nRow - VisibleCells.GetMaxRow();
    int up    = VisibleCells.GetMinRow() - nRow;

    int iColumnStart;
    int iRowStart;

    iColumnStart = VisibleCells.GetMaxCol() + 1;
    while( iRight > 0 )
    {
        if( GetColumnWidth( iColumnStart ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );

        iRight--;
        iColumnStart++;
    }

    iColumnStart = VisibleCells.GetMinCol() - 1;
    while( iLeft > 0 )
    {
        if( GetColumnWidth( iColumnStart ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
        iLeft--;
        iColumnStart--;
    }

    iRowStart = VisibleCells.GetMaxRow() + 1;
    while( down > 0 )
    {
        if( GetRowHeight( iRowStart ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
        down--;
        iRowStart++;
    }

    iRowStart = VisibleCells.GetMinRow() - 1;
    while( up > 0 )
    {
        if( GetRowHeight( iRowStart ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        up--;
        iRowStart--;
    }

    // Move one more if we only see a snall bit of the cell
    CRect rectCell;
    if (!GetCellRect(nRow, nCol, rectCell))
    {
		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
			pFocusWnd->SetFocus(); 
        return;
    }

    GetClientRect(rectWindow);

	CRect rectNonFixed(rectWindow);
	rectNonFixed.left += GetFixedColumnWidth();
	rectNonFixed.top  += GetFixedRowHeight();

    // The previous fix was fixed properly by Martin Richter 
    while (rectCell.right > rectWindow.right
        && rectCell.left > GetFixedColumnWidth()
        && IsVisibleHScroll() // Junlin Xu: added to prevent infinite loop
		&& rectNonFixed.Width() > rectCell.Width()	// xl 0821 防止矩形区域不足导致的死循环
        )
    {
		CRect rcOld(rectCell);
        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        if (!GetCellRect(nRow, nCol, rectCell))
        {
            // pFocusWnd->SetFocus();  // focus可能为NULL
			::SetFocus(pFocusWnd->GetSafeHwnd());
            return;
        }
		if ( rcOld.EqualRect(rectCell) )		// 由于只是微调，如果滚动一次不动的话，就退出循环 - 如果这个没有效果，以后删除这个退出条件
		{
			return;
		}
    }

    while (rectCell.bottom > rectWindow.bottom
        && rectCell.top > GetFixedRowHeight()
        && IsVisibleVScroll() // Junlin Xu: added to prevent infinite loop
		&& rectNonFixed.Height() > rectCell.Height()	// xl 0821 防止矩形区域不足导致的死循环
        )
    {
		CRect rcOld(rectCell);
        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        if (!GetCellRect(nRow, nCol, rectCell))
        {
            //pFocusWnd->SetFocus(); 
			::SetFocus(pFocusWnd->GetSafeHwnd());
            return;
        }
		if ( rcOld.EqualRect(rectCell) )
		{
			return;
		}
    }

    // restore focus to whoever owned it
    //pFocusWnd->SetFocus(); 
	
	::SetFocus(pFocusWnd->GetSafeHwnd());
}

BOOL CGridCtrl::IsCellEditable(CCellID &cell) const
{
    return IsCellEditable(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellEditable(int nRow, int nCol) const
{
    BOOL bEdit = IsEditable() && ((GetItemState(nRow, nCol) & GVIS_READONLY) != GVIS_READONLY);
	if ( bEdit && m_bSomeColsEditable )
	{
		bEdit = FALSE;
		for ( int i=0; i < m_aEditCols.GetSize() ; ++i )
		{
			if ( nCol == m_aEditCols[i] )
			{
				bEdit = TRUE;
				break;
			}
		}
	}
	return bEdit;
}

BOOL CGridCtrl::IsCellSelected(CCellID &cell) const
{
    return IsCellSelected(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellSelected(int nRow, int nCol) const
{
    if (GetVirtualMode())
    {   
        if (!IsSelectable())
            return FALSE;

        CCellID cell;
        DWORD key = MAKELONG(nRow, nCol);

        return (m_SelectedCellMap.Lookup(key, (CCellID&)cell));       
    }
    else
        return IsSelectable() && ((GetItemState(nRow, nCol) & GVIS_SELECTED) == GVIS_SELECTED);
}

BOOL CGridCtrl::IsCellVisible(CCellID cell) 
{
    return IsCellVisible(cell.row, cell.col);
}

BOOL CGridCtrl::IsCellVisible(int nRow, int nCol)
{
    if (!IsWindow(m_hWnd))
        return FALSE;

    int x, y;

    CCellID TopLeft;
    if (nCol >= GetFixedColumnCount() || nRow >= GetFixedRowCount())
    {
        TopLeft = GetTopleftNonFixedCell();
        if (nCol >= GetFixedColumnCount() && nCol < TopLeft.col)
            return FALSE;
        if (nRow >= GetFixedRowCount() && nRow < TopLeft.row)
            return FALSE;
    }
    
    CRect rect;
    GetClientRect(rect);
    if (nCol < GetFixedColumnCount())
    {
        x = 0;
		if ( IsAutoHideFragmentaryCell() )
		{
			// 部分显示的列不计算它
			for (int i = 0; i <= nCol; i++) 
			{
				x += GetColumnWidth(i);    
				if (x > rect.right)
					return FALSE;
			}
		}
		else
		{
			for (int i = 0; i <= nCol; i++) 
			{
				if (x >= rect.right)
					return FALSE;
				x += GetColumnWidth(i);    
			}
		}
    } 
    else 
    {
        x = GetFixedColumnWidth();
		if ( IsAutoHideFragmentaryCell() )
		{
			for (int i = TopLeft.col; i <= nCol; i++) 
			{
				x += GetColumnWidth(i);    
				if (x > rect.right)
					return FALSE;
			}
		}
		else
		{
			for (int i = TopLeft.col; i <= nCol; i++) 
			{
				if (x >= rect.right)
					return FALSE;
				x += GetColumnWidth(i);    
			}
		}
    }
    
    if (nRow < GetFixedRowCount())
    {
        y = 0;
		if ( IsAutoHideFragmentaryCell() )
		{
			for (int i = 0; i <= nRow; i++) 
			{
				y += GetRowHeight(i);    
				if (y > rect.bottom)
					return FALSE;
			}
		}
		else
		{
			for (int i = 0; i <= nRow; i++) 
			{
				if (y >= rect.bottom)
					return FALSE;
				y += GetRowHeight(i);    
			}
		}
    } 
    else 
    {
        if (nRow < TopLeft.row)
            return FALSE;
        y = GetFixedRowHeight();
		if ( IsAutoHideFragmentaryCell() )
		{
			for (int i = TopLeft.row; i <= nRow; i++) 
			{
				y += GetRowHeight(i);    
				if (y > rect.bottom)
					return FALSE;
			}
		}
		else
		{
			for (int i = TopLeft.row; i <= nRow; i++) 
			{
				if (y >= rect.bottom)
					return FALSE;
				y += GetRowHeight(i);    
			}
		}
    }
    
    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellID& cell)
{
    return InvalidateCellRect(cell.row, cell.col);
}

BOOL CGridCtrl::InvalidateCellRect(const int row, const int col)
{
    if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
        return FALSE;

    if (!IsValid(row, col))
        return FALSE;

	// xl 0416 与ondraw一致的显示可见判断
//     if (!IsCellVisible(row, col))
//         return FALSE;

    CRect rect;
    if (!GetCellRect(row, col, rect))
        return FALSE;
    rect.right++;
    rect.bottom++;
    InvalidateRect(rect, TRUE);

    return TRUE;
}

BOOL CGridCtrl::InvalidateCellRect(const CCellRange& cellRange)
{
	ASSERT(IsValid(cellRange));
	if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
		return FALSE;

	// CCellRange visibleCellRange = GetVisibleNonFixedCellRange().Intersect(cellRange);
	CCellRange visibleCellRange = (cellRange);

	CRect rect;
	if (!GetCellRangeRect(visibleCellRange, rect))
		return FALSE;

	rect.right++;
	rect.bottom++;
	InvalidateRect(rect, TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl Mouse stuff

// Handles mouse wheel notifications
// Note - if this doesn't work for win95 then use OnRegisteredMouseWheel instead
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
BOOL CGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    if (m_nRowsPerWheelNotch == -1)
    {
        int nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0)
		{
            for (int i = 0; i < nPagesScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
			}
		}
        else
		{
            for (int i = 0; i > nPagesScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
			}
		}
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0)
		{
            for (int i = 0; i < nRowsScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
			}
		}
        else
		{
            for (int i = 0; i > nRowsScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
			}
		}
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
#endif // !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)

void CGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;
#endif

    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message hs been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bLMouseButtonDown
    // BOOL bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);

	if ( NULL != m_pUserCB )
	{
		m_pUserCB->OnGridMouseMove(point);
	}

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bLMouseButtonDown ||
        (m_bLMouseButtonDown && m_MouseMode == MOUSE_NOTHING))
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
          //  if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
#ifndef _WIN32_WCE_NO_CURSOR
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING)
        {
            CGridCellBase* pCell = NULL;
            CCellID idCurrentCell;
			idCurrentCell = GetCellFromPt(point);
            pCell = GetCell(idCurrentCell.row, idCurrentCell.col);
            if (!GetVirtualMode() || m_bTitleTips)
            {
                // Let the cell know that a big fat cursor is currently hovering
                // over it.    
                if (pCell)
                    pCell->OnMouseOver();
            }
			
			if (point != m_LastMousePoint)
			{
				m_TipWnd.Hide();
				SetTimer(KTimerIdShowTips, KTimerPeriodShowTips, 0);
			}
        }
		
/*
原始
#ifndef GRIDCONTROL_NO_TITLETIPS
            // Titletips anyone? anyone?
			//... fangz 0422# 根据不同cell 的风格,显示不同tip.
			//... for test

            if (m_bTitleTips)
            {
                CRect TextRect, CellRect;
	
					if ( pCell )
					{
						LPCTSTR szTipText = pCell->GetTipText();
						if (!m_bRMouseButtonDown
							&& szTipText && szTipText[0]
							&& !pCell->IsEditing()
							&& GetCellRect( idCurrentCell.row, idCurrentCell.col, &TextRect)
							&& pCell->GetTipTextRect( &TextRect)
							&& GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect) )
						{
							//						TRACE0("Showing TitleTip\n");
							m_TitleTip.Show(TextRect, pCell->GetTipText(),  0, CellRect,
								pCell->GetFont(),  GetTitleTipTextClr(), GetTitleTipBackClr());
						}
					}
				
            }
#endif

*/
        m_LastMousePoint = point;
        return;
    }
    if (!IsValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }
    // If the left mouse button is down, then process appropriately
    if (m_bLMouseButtonDown)
    {
        switch (m_MouseMode)
        {
        case MOUSE_SELECT_ALL:
            break;

        case MOUSE_SELECT_COL:
        case MOUSE_SELECT_ROW:
        case MOUSE_SELECT_CELLS:
            {
                CCellID idCurrentCell = GetCellFromPt(point);
                if (!IsValid(idCurrentCell))
                    return;

                if (idCurrentCell != GetFocusCell())
                {
                    OnSelecting(idCurrentCell);

                    // EFW - BUG FIX - Keep the appropriate cell row and/or
                    // column focused.  A fix in SetFocusCell() will place
                    // the cursor in a non-fixed cell as needed.
                    if((idCurrentCell.row >= m_nFixedRows &&
                      idCurrentCell.col >= m_nFixedCols) ||
                      m_MouseMode==MOUSE_SELECT_COL ||
                      m_MouseMode==MOUSE_SELECT_ROW)
                    {
                        SetFocusCell(idCurrentCell);
                    }
                }
                break;
            }

        case MOUSE_SIZING_COL:
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;

				int32 iRowHeight = GetRowHeight(0);

                CRect oldInvertedRect(m_LastMousePoint.x, rect.top,
                    m_LastMousePoint.x + 2, rect.top+iRowHeight);
                pDC->InvertRect(&oldInvertedRect);
                CRect newInvertedRect(point.x, rect.top, 
                    point.x + 2, rect.top+iRowHeight);
                pDC->InvertRect(&newInvertedRect);
                ReleaseDC(pDC);

				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
            }
            break;
            
        case MOUSE_SIZING_ROW:        
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;
                
                CRect oldInvertedRect(rect.left, m_LastMousePoint.y,
                    rect.right, m_LastMousePoint.y + 2);
                pDC->InvertRect(&oldInvertedRect);
                CRect newInvertedRect(rect.left, point.y, 
                    rect.right, point.y + 2);
                pDC->InvertRect(&newInvertedRect);
                ReleaseDC(pDC);
            }
            break;
            
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_PREPARE_EDIT:
        case MOUSE_PREPARE_DRAG:
            m_MouseMode = MOUSE_PREPARE_DRAG;
            OnBeginDrag();    
            break;
#endif
        }    
    }

    m_LastMousePoint = point;
	m_TipWnd.Hide();
}

// Returns the point inside the cell that was clicked (coords relative to cell top left)
CPoint CGridCtrl::GetPointClicked(int nRow, int nCol, const CPoint& point)
{
    CPoint PointCellOrigin;
    if( !GetCellOrigin( nRow, nCol, &PointCellOrigin)  )
        return CPoint( 0, 0);

    CPoint PointClickedCellRelative( point);
    PointClickedCellRelative -= PointCellOrigin;
    return PointClickedCellRelative;
}

void CGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    TRACE0("CGridCtrl::OnLButtonDblClk\n");

	m_TipWnd.Hide();

    CCellID cell = GetCellFromPt(point);
    if( !IsValid( cell) )
    {
        //ASSERT(FALSE);
        return;
    }

    // 过滤双击表头    
    if(m_bIgnoreHeader)
    {
        if (m_LeftClickDownCell.row == 0)
            return;
    }

#ifdef _WIN32_WCE
    if (MouseOverColumnResizeArea(point))
#else
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE)
#endif
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.x - start.x < m_nResizeCaptureRange)     // Clicked right of border
            cell.col--;

        //  ignore columns that are hidden and look left towards first visible column
        BOOL bFoundVisible = FALSE;
        while( cell.col >= 0)
        {
            if( GetColumnWidth( cell.col) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            cell.col--;
        }
        if( !bFoundVisible)
            return;

        AutoSizeColumn(cell.col, GetAutoSizeStyle());
        Invalidate();
    }
#ifdef _WIN32_WCE
    else if (MouseOverRowResizeArea(point))
#else
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE)
#endif
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.y - start.y < m_nResizeCaptureRange)     // Clicked below border
            cell.row--;

        //  ignore rows that are hidden and look up towards first visible row
        BOOL bFoundVisible = FALSE;
        while( cell.row >= 0)
        {
            if( GetRowHeight( cell.row) > 0)
            {
                bFoundVisible = TRUE;
                break;
            }
            cell.row--;
        }
        if( !bFoundVisible)
            return;
		
        AutoSizeRow(cell.row);
        Invalidate();
    }
    else if (m_MouseMode == MOUSE_NOTHING)
    {
        CPoint pointClickedRel;
        pointClickedRel = GetPointClicked( cell.row, cell.col, point);

        CGridCellBase* pCell = NULL;
        if (IsValid(cell))
            pCell = GetCell(cell.row, cell.col);

        // Clicked in the text area? Only then will cell selection work
        BOOL bInTextArea = FALSE;
        if (pCell)
        {
            CRect rectCell;
            if (GetCellRect(cell.row, cell.col, rectCell) && pCell->GetTextRect(rectCell))
                bInTextArea = rectCell.PtInRect(point);
        }

        if (cell.row >= m_nFixedRows && IsValid(m_LeftClickDownCell) && 
            cell.col >= m_nFixedCols && bInTextArea)
        {
            OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }
        else if (m_bListMode)
        {
            if (!IsValid(cell))
                return;
            if (cell.row >= m_nFixedRows && cell.col >= m_nFixedCols && bInTextArea)
                OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }

        if (IsValid(cell))
        {
            CGridCellBase* pCellBase = GetCell(cell.row, cell.col);
            if (pCellBase)
                pCellBase->OnDblClick(pointClickedRel);
            SendMessageToParent(cell.row, cell.col, (int)NM_DBLCLK);
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
	
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef GRIDCONTROL_USE_TITLETIPS
    // EFW - Bug Fix
    m_TitleTip.Hide();  // hide any titletips
	m_TipWnd.Hide();
#endif

	if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
	{
#ifndef _WIN32_WCE_NO_CURSOR
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
#endif
     }

    // TRACE0("CGridCtrl::OnLButtonDown\n");
    // CWnd::OnLButtonDown(nFlags, point);

    SetFocus();
	m_CurCol = -1;
    m_bLMouseButtonDown   = TRUE;
    m_LeftClickDownPoint  = point;
    m_LeftClickDownCell   = GetCellFromPt(point);
	m_bNeedDrawSortArrow  = false;
	
    if (!IsValid(m_LeftClickDownCell))
	{
		if (m_bRemoveSelectClickBlank)
		{
			// lcq add 20140117 点击非法区域，取消上次选中列
			ResetSelectedRange();
			SelectRows(CCellID(-1, -1));
			Refresh();
		}

		// fangz0928 非法区域,需要返回鼠标抬起的消息~要不鼠标状态会出错
		OnLButtonUp(0,point);
		
        return;
	}
	m_CurRow = m_LeftClickDownCell.row;

    // If the SHIFT key is not down, then the start of the selection area should be the 
    // cell just clicked. Otherwise, keep the previous selection-start-cell so the user
    // can add to their previous cell selections in an intuitive way. If no selection-
    // start-cell has been specified, then set it's value here and now.
    if ((nFlags & MK_SHIFT) != MK_SHIFT )
        m_SelectionStartCell = m_LeftClickDownCell;
    else
    {
        if (!IsValid(m_SelectionStartCell))
            m_SelectionStartCell = m_idCurrentCell;
    }

    EndEditing();

    // tell the cell about it 
    CGridCellBase* pCell = GetCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
    if (pCell)
        pCell->OnClickDown(GetPointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, point));

    // Clicked in the text area? Only then will cell selection work
    BOOL bInTextArea = FALSE;
    if (pCell)
    {
        CRect rectCell;
        if (GetCellRect(m_LeftClickDownCell.row, m_LeftClickDownCell.col, rectCell) &&
            pCell->GetTextRect(rectCell))
        {
            bInTextArea = rectCell.PtInRect(point);
        }
    }
	
    // If the user clicks on the current cell, then prepare to edit it.
    // (If the user moves the mouse, then dragging occurs)
    if (m_LeftClickDownCell == m_idCurrentCell && 
        !(nFlags & MK_CONTROL) && bInTextArea &&
        IsCellEditable(m_LeftClickDownCell))
    {
        m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    }
    // If the user clicks on a selected cell, then prepare to drag it.
    // (If the user moves the mouse, then dragging occurs)
    else if (IsCellSelected(m_LeftClickDownCell))
    {
        SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
		
        // If control is pressed then unselect the cell or row (depending on the list mode)
        if (nFlags & MK_CONTROL)
        {
            SetFocusCell(m_LeftClickDownCell);
            if (GetListMode())
                SelectRows(m_LeftClickDownCell, TRUE, FALSE);
            else
                SelectCells(m_LeftClickDownCell, TRUE, FALSE);
            return;
        }
#ifndef GRIDCONTROL_NO_DRAGDROP
        else if (m_bAllowDragAndDrop)
            m_MouseMode = MOUSE_PREPARE_DRAG;
#endif
    }
    else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE
		&& m_MouseMode != MOUSE_OVER_ROW_DIVIDE )
    {
		//...fangz0602# 这里选定为非固定项目.所以在选择时候,固定项目不会选中.
		
// 		if (m_LeftClickDownCell.row >= GetFixedRowCount() && 
// 			m_LeftClickDownCell.col >= GetFixedColumnCount())
// 		{
		// xl 0503
		SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
			// 正常单击事件选择处理后面有，这里不进行处理
// 			SelectRows(m_LeftClickDownCell, FALSE);
		// xl 0503 end
// 		}

		//else
		//	SetFocusCell(-1, -1);
		
        // SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
		
		//Don't set focus on any cell if the user clicked on a fixed cell - David Pritchard
        //if (GetRowCount() > GetFixedRowCount() && 
        //    GetColumnCount() > GetFixedColumnCount())
        //    SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
        //                 max(m_LeftClickDownCell.col, m_nFixedCols));
    }

    SetCapture();
    
    if (m_MouseMode == MOUSE_NOTHING)
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        // else if (m_MouseMode != MOUSE_NOTHING)
        //{
        //    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        //    m_MouseMode = MOUSE_NOTHING;
        //}
    }
    
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
    {
        m_MouseMode = MOUSE_SIZING_COL;

        // Kludge for if we are over the last column...
        if (GetColumnWidth(GetColumnCount()-1) < m_nResizeCaptureRange)
        {
            CRect VisRect;
            GetVisibleNonFixedCellRange(VisRect);
            if (abs(point.x - VisRect.right) < m_nResizeCaptureRange)
                m_LeftClickDownCell.col = GetColumnCount()-1;
        }

        CPoint start;
        if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start))
            return;

        if( !m_bHiddenColUnhide)
        {
            //  ignore columns that are hidden and look left towards first visible column
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellRightBorder = point.x - start.x >= m_nResizeCaptureRange;

            if( bIsCellRightBorder
                && m_LeftClickDownCell.col + 1 >= GetColumnCount() )
            {
                // clicked on last column's right border

                // if last column is visible, don't do anything
                if( m_LeftClickDownCell.col >= 0)
                    bLookForVisible = FALSE;
            }

            if( bLookForVisible)
            {
                // clicked on column divider other than last right border
                BOOL bFoundVisible = FALSE;
                int iOffset = 1;

                if( bIsCellRightBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.col - iOffset >= 0)
                {
                    if( GetColumnWidth( m_LeftClickDownCell.col - iOffset) > 0)
                    {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.col--;
                }
                if( !bFoundVisible)
                    return;
            }
        }


        CRect rect;
        GetClientRect(rect);
		int32 iRowHeight = GetRowHeight(0);
        CRect invertedRect(point.x, rect.top, point.x + 2, rect.top+iRowHeight);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        // If we clicked to the right of the colimn divide, then reset the click-down cell
        // as the cell to the left of the column divide - UNLESS we clicked on the last column
        // and the last column is teensy (kludge fix)
        if (point.x - start.x < m_nResizeCaptureRange)
        {
            if (m_LeftClickDownCell.col < GetColumnCount()-1 || 
                GetColumnWidth(GetColumnCount()-1) >= m_nResizeCaptureRange)
            {
                if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start))
                    return;
            }
        }

		// Allow a cell resize width no greater than that which can be viewed within
		// the grid itself
		int nMaxCellWidth = rect.Width()-GetFixedColumnWidth();
        rect.left  = start.x + 1;
		rect.right = rect.left + nMaxCellWidth;

        ClientToScreen(rect);
#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
    {
        m_MouseMode = MOUSE_SIZING_ROW;

        // Kludge for if we are over the last column...
        if (GetRowHeight(GetRowCount()-1) < m_nResizeCaptureRange)
        {
            CRect VisRect;
            GetVisibleNonFixedCellRange(VisRect);
            if (abs(point.y - VisRect.bottom) < m_nResizeCaptureRange)
                m_LeftClickDownCell.row = GetRowCount()-1;
        }

        CPoint start;
        if (!GetCellOrigin(m_LeftClickDownCell, &start))
            return;

        if( !m_bHiddenRowUnhide)
        {
            //  ignore rows that are hidden and look up towards first visible row
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellBottomBorder = point.y - start.y >= m_nResizeCaptureRange;

            if( bIsCellBottomBorder
                && m_LeftClickDownCell.row + 1 >= GetRowCount() )
            {
                // clicked on last row's bottom border

                // if last row is visible, don't do anything
                if( m_LeftClickDownCell.row >= 0)
                    bLookForVisible = FALSE;
            }

            if( bLookForVisible)
            {
                // clicked on row divider other than last bottom border
                BOOL bFoundVisible = FALSE;
                int iOffset = 1;

                if( bIsCellBottomBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.row - iOffset >= 0)
                {
                    if( GetRowHeight( m_LeftClickDownCell.row - iOffset) > 0)
                    {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.row--;
                }
                if( !bFoundVisible)
                    return;
            }
        }

        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        // If we clicked below the row divide, then reset the click-down cell
        // as the cell above the row divide - UNLESS we clicked on the last row
        // and the last row is teensy (kludge fix)
        if (point.y - start.y < m_nResizeCaptureRange)            // clicked below border
        {
            if (m_LeftClickDownCell.row < GetRowCount()-1 || 
                GetRowHeight(GetRowCount()-1) >= m_nResizeCaptureRange)
            {
                if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start))
                    return;
            }
        }

		int nMaxCellHeight = rect.Height()-GetFixedRowHeight();
        rect.top = start.y + 1;
		rect.bottom = rect.top + nMaxCellHeight;

        ClientToScreen(rect);

#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else
#ifndef GRIDCONTROL_NO_DRAGDROP
    if (m_MouseMode != MOUSE_PREPARE_DRAG) // not sizing or editing -- selecting
#endif
    {
		//...fangz602 正常的单击事件.
	    SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)GVN_SELCHANGING);

        // If Ctrl pressed, save the current cell selection. This will get added
        // to the new cell selection at the end of the cell selection process
        m_PrevSelectedCellMap.RemoveAll();
        if (nFlags & MK_CONTROL)
        {
            for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
            {
                DWORD key;
                CCellID cell;
                m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
                m_PrevSelectedCellMap.SetAt(key, cell);
            }
        }
        
		BOOL bSelectEnable = TRUE;
        if (m_LeftClickDownCell.row < GetFixedRowCount())
		{
			m_bNeedDrawSortArrow = true;

			if ( NULL != m_pUserCB )
			{
				m_pUserCB->OnFixedRowClickCB(m_LeftClickDownCell);
			}
            else
			{
				OnFixedRowClick(m_LeftClickDownCell,point);
			}
            if(m_AllowReorderColumn && m_LeftClickDownCell.col >=  GetFixedColumnCount())
			{
				ResetSelectedRange(); // TODO : This is not the better solution, as we do not see why clicking in column header should reset selection
											//but the state of selection is instable after drag (at least until someone debugs it), so better clear it allways.
				m_MouseMode = MOUSE_PREPARE_DRAG;
				m_CurCol = m_LeftClickDownCell.col;
			}
			bSelectEnable = FALSE;	// 固定行不允许选择到该行
		}
		else if (m_LeftClickDownCell.col < GetFixedColumnCount())
		{
			if ( NULL != m_pUserCB )
			{
				m_pUserCB->OnFixedColumnClickCB ( m_LeftClickDownCell );
			}
			else
			{
				OnFixedColumnClick(m_LeftClickDownCell,point);
			}
			if ( GetListMode() )
			{
				m_MouseMode = MOUSE_SELECT_ROW;
			}
			else
			{
				bSelectEnable = FALSE;	// 不允许选择该固定单元格
			}
		}
		else
		{
			// m_MouseMode = MOUSE_NOTHING;
			if ( NULL != m_pUserCB && !m_pUserCB->BeBlindSelect() ) 
			{
				m_nTimerID = SetTimer(KTimerIdLButtonDown, m_nTimerInterval, 0);
			}	        
		}
		
		if ( bSelectEnable )
		{
			m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
			
			OnSelecting(m_LeftClickDownCell);
			
			if ( GetListMode()
				&& !GetSingleRowSelection()
				&& ((nFlags & MK_SHIFT) == MK_SHIFT)
				&& m_SelectionStartCell.row != m_LeftClickDownCell.row )
			{
				// shift键选多行, 根据startrow重新调整selectedlst中顺序
				ASSERT( m_SelectedRowList.GetCount() > 1 );
				m_SelectedRowList.RemoveAll();
				int iStartRow = m_SelectionStartCell.row;
				int iEndRow = m_LeftClickDownCell.row;
				if ( iStartRow > iEndRow )
				{
					for ( int i=iStartRow; i >= iEndRow ; --i )
					{
						m_SelectedRowList.AddTail(i);
					}
				}
				else
				{
					for ( int i=iStartRow; i <= iEndRow ; ++i )
					{
						m_SelectedRowList.AddTail(i);
					}
				}
			}
		}
    }  
	//... fangz 0424# testtips
	m_TipWnd.Hide();
    m_LastMousePoint = point;
	
}

void CGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TRACE0("CGridCtrl::OnLButtonUp\n");
    CWnd::OnLButtonUp(nFlags, point);

    m_bLMouseButtonDown = FALSE;

#ifndef _WIN32_WCE_NO_CURSOR
    ClipCursor(NULL);
#endif
	
	if (m_LeftClickDownCell.col == 0)
	{
		CWnd *pParent = GetParent();
		if (pParent)
		{
			// 因为表头也是表格的一条记录（作为第0行），所以这里要屏蔽掉单击表头
			if (m_LeftClickDownCell.row > 0)
				pParent->SendMessage(WM_DISENTRUST, (WPARAM)this, 0);
		}
	}

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

    CPoint pointClickedRel;
    pointClickedRel = GetPointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point);

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current cell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT)
    {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, pointClickedRel, VK_LBUTTON);
    }
#ifndef GRIDCONTROL_NO_DRAGDROP
    // m_MouseMode == MOUSE_PREPARE_DRAG only if user clicked down on a selected cell
    // and then didn't move mouse before clicking up (releasing button)
    else if (m_MouseMode == MOUSE_PREPARE_DRAG) 
    {
        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick( GetPointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)NM_CLICK);
	    SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)GVN_SELCHANGING);
        ResetSelectedRange();
	    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, (int)GVN_SELCHANGED);
    }
#endif
    else if (m_MouseMode == MOUSE_SIZING_COL)
    {
        CRect rect;
        GetClientRect(rect);

		int32 iRowHeight = GetRowHeight(0);
        CRect invertedRect(m_LastMousePoint.x, rect.top, m_LastMousePoint.x + 2, rect.top+iRowHeight);
        
        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {   
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;

            int nColumnWidth = __max(point.x - start.x, m_bAllowColHide? 0 : 1);

			int32 iWidthBak = GetColumnWidth(m_LeftClickDownCell.col);

            SetColumnWidth(m_LeftClickDownCell.col, nColumnWidth);
            ResetScrollBars();
            Invalidate();

			int32 iWidthNew = GetColumnWidth(m_LeftClickDownCell.col);

			if (iWidthNew >= 0 && iWidthNew != iWidthBak)
				SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)GVN_COLWIDTHCHANGED, iWidthNew);
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW)
    {
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, m_LastMousePoint.y, rect.right, m_LastMousePoint.y + 2);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point  && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {
            CPoint start;
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;
            
            int nRowHeight = __max(point.y - start.y, m_bAllowRowHide? 0 : 1);

            SetRowHeight(m_LeftClickDownCell.row, nRowHeight);
            ResetScrollBars();
            Invalidate();
        }
    }
    else
    {
	    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, (int)GVN_SELCHANGED);

        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick( GetPointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, (int)NM_CLICK);
    }
    
    m_MouseMode = MOUSE_NOTHING;
    
#ifndef _WIN32_WCE_NO_CURSOR
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
    
    if (!IsValid(m_LeftClickDownCell))
        return;
    
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        pOwner->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED),
        (LPARAM) GetSafeHwnd());
}

void CGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{	
    if (!IsValid(GetCellFromPt(point)))
	{
		if (m_bRemoveSelectClickBlank)
		{
			// lcq add 20140117 点击非法区域，取消上次选中列
			ResetSelectedRange();
			SelectRows(CCellID(-1, -1));
			Refresh();
		}
	}
	
    CWnd::OnRButtonDown(nFlags, point);
}

#ifndef _WIN32_WCE
// void CGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
// {	
//     if (!IsValid(GetCellFromPt(point)))
// 	{
// 		// lcq add 点击非法区域，取消上次选中列
// 		ResetSelectedRange();
// 		SelectRows(CCellID(-1, -1));
// 	}
// 
//     CWnd::OnRButtonDown(nFlags, point);
// 	m_bRMouseButtonDown = TRUE;
// 	
// #ifdef GRIDCONTROL_USE_TITLETIPS
// 	TRACE0("Hiding TitleTip\n");
//     m_TitleTip.Hide();  // hide any titletips
// 		m_TipWnd.Hide();
// #endif
// }

// EFW - Added to forward right click to parent so that a context
// menu can be shown without deriving a new grid class.
void CGridCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonUp(nFlags, point);

	m_bRMouseButtonDown = FALSE;

    CCellID FocusCell;
    FocusCell = GetCellFromPt(point);

    EndEditing();        // Auto-destroy any InPlaceEdit's

    // If not a valid cell, pass -1 for row and column
    if(!IsValid(FocusCell))
        SendMessageToParent(-1, -1, (int)NM_RCLICK);
    else
    {
		// ...fangz0530 右键单击也选中整行
		// 多行模式下, 如果已经选择了多行，则不允许选择该行
		if ( GetSingleRowSelection() 
			|| !GetListMode()
			|| m_SelectedCellMap.GetCount() <= GetColumnCount() )
		{
			SetFocusCell(-1,-1);
			SetFocusCell(__max(FocusCell.row, m_nFixedRows),
            __max(FocusCell.col, m_nFixedCols));

			m_SelectionStartCell = FocusCell;	// 仅选择一行

			SelectRows(FocusCell, FALSE);
		}
		else
		{
			ASSERT( m_SelectedRowList.GetCount() > 1 );
		}


        // tell the cell about it
        CGridCellBase* pCell = GetCell(FocusCell.row, FocusCell.col);
        if (pCell)
            pCell->OnRClick( GetPointClicked( FocusCell.row, FocusCell.col, point) );

        SendMessageToParent(FocusCell.row, FocusCell.col, (int)NM_RCLICK);
    }
}
#endif

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing

// EFW - New print margin support functions
void CGridCtrl::SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
    int nLeftMargin, int nRightMargin, int nTopMargin,
    int nBottomMargin, int nGap)
{
    // If any parameter is -1, keep the existing setting
    if(nHeaderHeight > -1)
        m_nHeaderHeight = nHeaderHeight;
    if(nFooterHeight > -1)
        m_nFooterHeight = nFooterHeight;
    if(nLeftMargin > -1)
        m_nLeftMargin = nLeftMargin;
    if(nRightMargin > -1)
        m_nRightMargin = nRightMargin;
    if(nTopMargin > -1)
        m_nTopMargin = nTopMargin;
    if(nBottomMargin > -1)
        m_nBottomMargin = nBottomMargin;
    if(nGap > -1)
        m_nGap = nGap;
}

void CGridCtrl::GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
    int &nLeftMargin, int &nRightMargin, int &nTopMargin,
    int &nBottomMargin, int &nGap)
{
    nHeaderHeight = m_nHeaderHeight;
    nFooterHeight = m_nFooterHeight;
    nLeftMargin = m_nLeftMargin;
    nRightMargin = m_nRightMargin;
    nTopMargin = m_nTopMargin;
    nBottomMargin = m_nBottomMargin;
    nGap = m_nGap;
}

void CGridCtrl::Print(CPrintDialog* pPrntDialog /*=NULL*/)
{
    CDC dc;

    if (pPrntDialog == NULL)
    {
        CPrintDialog printDlg(FALSE);
        if (printDlg.DoModal() != IDOK)             // Get printer settings from user
            return;

        dc.Attach(printDlg.GetPrinterDC());         // attach a printer DC
    }
    else
        dc.Attach(pPrntDialog->GetPrinterDC());     // attach a printer DC
        
    dc.m_bPrinting = TRUE;

    CString strTitle;
    strTitle.LoadString(AFX_IDS_APP_TITLE);

    if( strTitle.IsEmpty() )
    {
        CWnd *pParentWnd = GetParent();
        while (pParentWnd)
        {
            pParentWnd->GetWindowText(strTitle);
            if (strTitle.GetLength())  // can happen if it is a CView, CChildFrm has the title
                break;
            pParentWnd = pParentWnd->GetParent();
        }
    }

    DOCINFO di;                                 // Initialise print doc details
    memset(&di, 0, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dc, &Info);                // Initialise printing
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++)
    {
        dc.StartPage();                         // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                    // Print page
        bPrintingOK = (dc.EndPage() > 0);       // end page
    }
    OnEndPrinting(&dc, &Info);                  // Clean up after printing

    if (bPrintingOK)
        dc.EndDoc();                            // end a print job
    else
        dc.AbortDoc();                          // abort job.

    dc.Detach();                                // detach the printer DC
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing overridables - for Doc/View print/print preview framework

// EFW - Various changes in the next few functions to support the
// new print margins and a few other adjustments.
void CGridCtrl::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
    // OnBeginPrinting() is called after the user has committed to
    // printing by OK'ing the Print dialog, and after the framework
    // has created a CDC object for the printer or the preview view.

    // This is the right opportunity to set up the page range.
    // Given the CDC object, we can determine how many rows will
    // fit on a page, so we can in turn determine how many printed
    // pages represent the entire document.

    ASSERT(pDC && pInfo);
    if (!pDC || !pInfo) return;

    // Get a DC for the current window (will be a screen DC for print previewing)
    CDC *pCurrentDC = GetDC();        // will have dimensions of the client area
    if (!pCurrentDC) return;

    CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
    CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));

    // Create the printer font
    int nFontSize = -10;
    CString strFontName = _T("Arial");
    m_PrinterFont.CreateFont(nFontSize, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                             OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, strFontName);

    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Get the average character width (in GridCtrl units) and hence the margins
    m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
    m_CharSize.cx /= 52;
    int nMargins = (m_nLeftMargin+m_nRightMargin)*m_CharSize.cx;

    // Get the page sizes (physical and logical)
    m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

    if( m_bWysiwygPrinting)
    {
        m_LogicalPageSize.cx = ScreenPixelsPerInch.cx * m_PaperSize.cx / PaperPixelsPerInch.cx * 3 / 4;
        m_LogicalPageSize.cy = ScreenPixelsPerInch.cy * m_PaperSize.cy / PaperPixelsPerInch.cy * 3 / 4;
    }
    else
    {
        m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

        m_LogicalPageSize.cx = GetVirtualWidth()+nMargins;
#ifdef _WIN32_WCE
        m_LogicalPageSize.cy = (m_LogicalPageSize.cx * m_PaperSize.cy) / m_PaperSize.cx;
#else
        m_LogicalPageSize.cy = MulDiv(m_LogicalPageSize.cx, m_PaperSize.cy, m_PaperSize.cx);
#endif
    }

    m_nPageHeight = m_LogicalPageSize.cy - GetFixedRowHeight()
                       - (m_nHeaderHeight+m_nFooterHeight + 2*m_nGap)*m_CharSize.cy;

    // Get the number of pages. Assumes no row is bigger than the page size.
    int nTotalRowHeight = 0;
    m_nNumPages = 1;
    for (int row = GetFixedRowCount(); row < GetRowCount(); row++)
    {
        nTotalRowHeight += GetRowHeight(row);
        if (nTotalRowHeight > m_nPageHeight) {
            m_nNumPages++;
            nTotalRowHeight = GetRowHeight(row);
        }
    }

    // now, figure out how many additional pages must print out if rows ARE bigger
    //  than page size
    int iColumnOffset = 0;
    int i1;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }
    m_nPageWidth = m_LogicalPageSize.cx - iColumnOffset
                    - nMargins;
    m_nPageMultiplier = 1;

    if( m_bWysiwygPrinting)
    {
        int iTotalRowWidth = 0;
        for( i1 = GetFixedColumnCount(); i1 < GetColumnCount(); i1++)
        {
            iTotalRowWidth += GetColumnWidth( i1);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPageMultiplier++;
                iTotalRowWidth = GetColumnWidth( i1);
            }
        }
        m_nNumPages *= m_nPageMultiplier;
    }

    // Set up the print info
    pInfo->SetMaxPage(m_nNumPages);
    pInfo->m_nCurPage = 1;                        // start printing at page# 1

    ReleaseDC(pCurrentDC);
    pDC->SelectObject(pOldFont);
}

void CGridCtrl::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
    if (!pDC || !pInfo)
        return;

    //CRect rcPage(pInfo->m_rectDraw);
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Set the page map mode to use GridCtrl units, and setup margin
    pDC->SetMapMode(MM_ANISOTROPIC);
    pDC->SetWindowExt(m_LogicalPageSize);
    pDC->SetViewportExt(m_PaperSize);
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, 0);

    // Header
    pInfo->m_rectDraw.top    = 0;
    pInfo->m_rectDraw.left   = 0;
    pInfo->m_rectDraw.right  = m_LogicalPageSize.cx - (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;
    pInfo->m_rectDraw.bottom = m_nHeaderHeight * m_CharSize.cy;
    PrintHeader(pDC, pInfo);
    pDC->OffsetWindowOrg(0, -m_nHeaderHeight * m_CharSize.cy);

    // Gap between header and column headings
    pDC->OffsetWindowOrg(0, -m_nGap * m_CharSize.cy);

    pDC->OffsetWindowOrg(0, -GetFixedRowHeight());

    // We need to find out which row to start printing for this page.
    int nTotalRowHeight = 0;
    UINT nNumPages = 1;
    m_nCurrPrintRow = GetFixedRowCount();


    // Not only the row, but we need to figure out column, too

    // Can print 4 pages, where page 1 and 2 represent the same rows but
    // with different WIDE columns.
    //
    // .......
    // .1 .2 .  If representing page 3  -->    iPageIfIgnoredWideCols = 2
    // .......                                 iWideColPageOffset = 0
    // .3 .4 .  If representing page 2  -->    iPageIfIgnoredWideCols = 1
    // .......                                 iWideColPageOffset = 1

    int iPageIfIgnoredWideCols = (int)pInfo->m_nCurPage / m_nPageMultiplier;
    int iWideColPageOffset = pInfo->m_nCurPage - ( iPageIfIgnoredWideCols * m_nPageMultiplier);
    if( iWideColPageOffset > 0)
        iPageIfIgnoredWideCols++;

    if( iWideColPageOffset == 0)
        iWideColPageOffset = m_nPageMultiplier;
    iWideColPageOffset--;

    // calculate current print row based on iPageIfIgnoredWideCols
    while(  m_nCurrPrintRow < GetRowCount()
            && (int)nNumPages < iPageIfIgnoredWideCols)
    {
        nTotalRowHeight += GetRowHeight(m_nCurrPrintRow);
        if (nTotalRowHeight > m_nPageHeight) {
            nNumPages++;
            if ((int)nNumPages == iPageIfIgnoredWideCols) break;
            nTotalRowHeight = GetRowHeight(m_nCurrPrintRow);
        }
        m_nCurrPrintRow++;
    }

    m_nPrintColumn = GetFixedColumnCount();
    int iTotalRowWidth = 0;
    int i1, i2;

    // now, calculate which print column to start displaying
    for( i1 = 0; i1 < iWideColPageOffset; i1++)
    {
        for( i2 = m_nPrintColumn; i2 < GetColumnCount(); i2++)
        {
            iTotalRowWidth += GetColumnWidth( i2);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPrintColumn = i2;
                iTotalRowWidth = 0;
                break;
            }
        }
    }


    PrintRowButtons( pDC, pInfo);   // print row buttons on each page
    int iColumnOffset = 0;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }

    // Print the column headings
    pInfo->m_rectDraw.bottom = GetFixedRowHeight();

    if( m_nPrintColumn == GetFixedColumnCount())
    {
        // have the column headings fcn draw the upper left fixed cells
        //  for the very first columns, only
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;
        m_nPrintColumn = 0;
        PrintColumnHeadings(pDC, pInfo);
        m_nPageWidth -= iColumnOffset;
        m_nPrintColumn = GetFixedColumnCount();

        pDC->OffsetWindowOrg( -iColumnOffset, -GetFixedRowHeight());
    }
    else
    {
        // changed all of this here to match above almost exactly same
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;

        // print from column 0 ... last column that fits on the current page
        PrintColumnHeadings(pDC, pInfo);
        
        m_nPageWidth -= iColumnOffset;
 
        pDC->OffsetWindowOrg( -iColumnOffset, -GetFixedRowHeight());
    }


    if (m_nCurrPrintRow >= GetRowCount()) return;

    // Draw as many rows as will fit on the printed page.
    // Clip the printed page so that there is no partially shown
    // row at the bottom of the page (the same row which will be fully
    // shown at the top of the next page).

    BOOL bFirstPrintedRow = TRUE;
    CRect rect;
    rect.bottom = -1;
    while (m_nCurrPrintRow < GetRowCount())
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(m_nCurrPrintRow) - 1;

        if (rect.bottom > m_nPageHeight) break;            // Gone past end of page

        rect.right = -1;

        // modified to allow printing of wide grids on multiple pages
        for (int col = m_nPrintColumn; col < GetColumnCount(); col++)
        {
            rect.left = rect.right+1;
            rect.right =    rect.left
                            + GetColumnWidth( col)
                            - 1;

            if( rect.right > m_nPageWidth)
                break;

            CGridCellBase* pCell = GetCell(m_nCurrPrintRow, col);
            if (pCell)
                pCell->PrintCell(pDC, m_nCurrPrintRow, col, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (col == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if (m_nCurrPrintRow == 0) {
                    pDC->MoveTo(rect.left-Overlap, rect.top);
                    pDC->LineTo(rect.right, rect.top);
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (bFirstPrintedRow)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if (col == 0) {
                    pDC->MoveTo(rect.left, rect.top-Overlap);
                    pDC->LineTo(rect.left, rect.bottom);
                }
            }

        }
        m_nCurrPrintRow++;
        bFirstPrintedRow = FALSE;
    }


    // Footer
    pInfo->m_rectDraw.bottom = m_nFooterHeight * m_CharSize.cy;
    pDC->SetWindowOrg( -m_nLeftMargin * m_CharSize.cx,
        -m_LogicalPageSize.cy + m_nFooterHeight * m_CharSize.cy);
    PrintFooter(pDC, pInfo);

    // SetWindowOrg back for next page
    pDC->SetWindowOrg(0,0);

    pDC->SelectObject(pOldFont);
}


// added by M.Fletcher 12/17/00
void CGridCtrl::PrintFixedRowCells(int nStartColumn, int nStopColumn, int& row, CRect& rect,
                                   CDC *pDC, BOOL& bFirst)
{
   // print all cells from nStartColumn to nStopColumn on row
   for (int col =nStartColumn; col < nStopColumn; col++)
   {
      rect.left = rect.right+1;
      rect.right =    rect.left + GetColumnWidth( col) - 1;

      if( rect.right > m_nPageWidth)
         break;
      
      CGridCellBase* pCell = GetCell(row, col);
      if (pCell)
         pCell->PrintCell(pDC, row, col, rect);
      
      if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
      {
         int Overlap = (col == 0)? 0:1;
         
         pDC->MoveTo(rect.left-Overlap, rect.bottom);
         pDC->LineTo(rect.right, rect.bottom);
         
         if (row == 0)
         {
            pDC->MoveTo(rect.left-Overlap, rect.top);
            pDC->LineTo(rect.right, rect.top);
          }
      }

      if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
      {
         int Overlap = (row == 0)? 0:1;

         pDC->MoveTo(rect.right, rect.top-Overlap);
         pDC->LineTo(rect.right, rect.bottom);
         
         if( bFirst)
         {
            pDC->MoveTo(rect.left-1, rect.top-Overlap);
            pDC->LineTo(rect.left-1, rect.bottom);
            bFirst = FALSE;
          }

       }

   } // end of column cells loop


} // end of CGridCtrl::PrintFixedRowCells

void CGridCtrl::PrintColumnHeadings(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.bottom = -1;

    BOOL bFirst = TRUE;
    BOOL bOriginal;
    

    // modified to allow column hdr printing of multi-page wide grids
    for (int row = 0; row < GetFixedRowCount(); row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row) - 1;

        rect.right = -1;
 
        // if printColumn > fixedcolumncount we are on page 2 or more
        // lets printout those fixed cell headings again the 1 or more that would be missed
        // added by M.Fletcher 12/17/00
        if(m_nPrintColumn>= GetFixedColumnCount())
        {
           bOriginal=bFirst;
           // lets print the missing fixed cells on left first out to last fixed column
           PrintFixedRowCells(0,GetFixedColumnCount(), row, rect, pDC, bFirst);
           bFirst=bOriginal;
        }

        // now back to normal business print cells in heading after all fixed columns
        PrintFixedRowCells(m_nPrintColumn, GetColumnCount(), row, rect, pDC, bFirst);
        
    } // end of Row Loop

    pDC->SelectObject(pOldFont);
} // end of CGridCtrl::PrintColumnHeadings



/*****************************************************************************
Prints line of row buttons on each page of the printout.  Assumes that
the window origin is setup before calling

*****************************************************************************/
void CGridCtrl::PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.right = -1;

    BOOL bFirst = TRUE;
    for( int iCol = 0; iCol < GetFixedColumnCount(); iCol++)
    {
        rect.left = rect.right+1;
        rect.right =    rect.left
                        + GetColumnWidth( iCol)
                        - 1;

        rect.bottom = -1;
        for( int iRow = m_nCurrPrintRow; iRow < GetRowCount(); iRow++)
        {
            rect.top = rect.bottom+1;
            rect.bottom = rect.top + GetRowHeight( iRow) - 1;

            if( rect.bottom > m_nPageHeight)
                break;

            CGridCellBase* pCell = GetCell(iRow, iCol);
            if (pCell)
                pCell->PrintCell(pDC, iRow, iCol, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (iCol == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if( bFirst) {
                    pDC->MoveTo(rect.left-Overlap, rect.top-1);
                    pDC->LineTo(rect.right, rect.top-1);
                    bFirst = FALSE;
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (iRow == 0)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if (iCol == 0) {
                    pDC->MoveTo(rect.left, rect.top-Overlap);
                    pDC->LineTo(rect.left, rect.bottom);
                }
            }

        }
    }
    pDC->SelectObject(pOldFont);
}

void CGridCtrl::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
    // print App title on top right margin
    CString strRight;
    strRight.LoadString(AFX_IDS_APP_TITLE);

    // print parent window title in the centre (Gert Rijs)
    CString strCenter;
    CWnd *pParentWnd = GetParent();
    while (pParentWnd)
    {
        pParentWnd->GetWindowText(strCenter);
        if (strCenter.GetLength())  // can happen if it is a CView, CChildFrm has the title
            break;
        pParentWnd = pParentWnd->GetParent();
    }

    CFont   BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    VERIFY(m_PrinterFont.GetLogFont(&lf));
    lf.lfWeight = FW_BOLD;
    VERIFY(BoldFont.CreateFontIndirect(&lf));

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect   rc(pInfo->m_rectDraw);
    if( !strCenter.IsEmpty() )
        pDC->DrawText( strCenter, &rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();

    // draw ruled-line across top
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.bottom);
    pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void CGridCtrl::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
    // page numbering on left
    CString strLeft;
    strLeft.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage() );

    // date and time on the right
    CString strRight;
    COleDateTime t = COleDateTime::GetCurrentTime();
    strRight = t.Format(_T("%c"));
    
    CRect rc(pInfo->m_rectDraw);

    // draw ruled line on bottom
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.top);
    pDC->LineTo(rc.right, rc.top);

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // EFW - Bug fix - Force text color to black.  It doesn't always
    // get set to a printable color when it gets here.
    pDC->SetTextColor(RGB(0, 0, 0));

    if( !strLeft.IsEmpty() )
        pDC->DrawText( strLeft, &rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();
}

void CGridCtrl::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    m_PrinterFont.DeleteObject();
}

#endif  // !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)


#ifndef _WIN32_WCE
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl persistance

BOOL CGridCtrl::Save(LPCTSTR filename, TCHAR chSeparator/*=_T(',')*/)
{
    CStdioFile File;
    CFileException ex;
	CString strSeparator(chSeparator);

    if (!File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText, &ex))
    {
        ex.ReportError();
        return FALSE;
    }

    TRY
    {
        int nNumColumns = GetColumnCount();
        for (int i = 0; i < GetRowCount(); i++)
        {
            for (int j = 0; j < nNumColumns; j++)
            {
                File.WriteString(GetItemText(i,j));

				CString StrWrite = strSeparator;
				if (j == (nNumColumns-1))
				{
					StrWrite = _T("\n");
				}

                File.WriteString(StrWrite);
            }
        }

        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to save grid list"));
        return FALSE;
    }
    END_CATCH

    return TRUE;
}

BOOL CGridCtrl::Load(LPCTSTR filename, TCHAR chSeparator/*=_T(',')*/)
{
    if (GetVirtualMode())
        return FALSE;

    TCHAR *token, *end;
    TCHAR buffer[1024];
    CStdioFile File;
    CFileException ex;

    if (!File.Open(filename, CFile::modeRead | CFile::typeText))
    {
        ex.ReportError();
        return FALSE;
    }

    DeleteAllItems();

    TRY
    {
        // Read Header off file
        File.ReadString(buffer, 1024);

        // Get first token
        for (token=buffer, end=buffer;
             *end && (*end != chSeparator) && (*end != _T('\n')); 
             end++)
            ;

        if ((*end == _T('\0')) && (token == end))
            token = NULL;

        *end = _T('\0');

        while (token)
        {
            InsertColumn(token);

            // Get next token
			token = ++end;
            for (; *end && (*end != chSeparator) && (*end != _T('\n')); end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');
        }

        // Read in rest of data
        int nItem = 1;
        while (File.ReadString(buffer, 1024))
        {
            // Get first token
            for (token=buffer, end=buffer;
              *end && (*end != chSeparator) && (*end != _T('\n')); end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');

            int nSubItem = 0;
            while (token)
            {
                if (!nSubItem)
                    InsertRow(token);
                else
                    SetItemText(nItem, nSubItem, token);

                // Get next token
				token = ++end;
                for (; *end && (*end != chSeparator) && (*end != _T('\n')); end++)
                    ;

                if ((*end == _T('\0')) && (token == end))
                    token = NULL;

                *end = _T('\0');

                nSubItem++;
            }
            nItem++;
        }

        AutoSizeColumns(GetAutoSizeStyle());
        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to load grid data"));
        return FALSE;
    }
    END_CATCH

    return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl overrideables

#ifndef GRIDCONTROL_NO_DRAGDROP
// This is no longer needed since I've changed to OLE drag and drop - but it's
// still cool code. :)
CImageList* CGridCtrl::CreateDragImage(CPoint *pHotSpot)
{
    CDC* pDC = GetDC();
    if (!pDC)
        return NULL;

    CRect rect;
    CCellID cell = GetFocusCell();
    if (!GetCellRect(cell.row, cell.col, rect))
        return NULL;
    
    // Translate coordinate system
    rect.BottomRight() = CPoint(rect.Width(), rect.Height());
    rect.TopLeft()     = CPoint(0, 0);
    *pHotSpot = rect.BottomRight(); 
    
    // Create a new imagelist (the caller of this function has responsibility
    // for deleting this list)
    CImageList* pList = new CImageList;
    if (!pList || !pList->Create(rect.Width(), rect.Height(), ILC_MASK, 1, 1))
    {    
        if (pList)
            delete pList;
        return NULL;
    }
    
    // Create mem DC and bitmap
    CDC MemDC;
    CBitmap bm;
    MemDC.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = MemDC.SelectObject(&bm);
    MemDC.SetWindowOrg(0, 0);
    
    // Draw cell onto bitmap in memDC
    CGridCellBase* pCell = GetCell(cell.row, cell.col);
    if (pCell)
        pCell->Draw(&MemDC, cell.row, cell.col, rect, FALSE);
    
    // Clean up
    MemDC.SelectObject(pOldBitmap);
    ReleaseDC(pDC);
    
    // Add the bitmap we just drew to the image list.
    pList->Add(&bm, GetDefaultCell(FALSE, FALSE)->GetBackClr());
    bm.DeleteObject();
	MemDC.DeleteDC();

    return pList;
}
#endif
// void CGridCtrl::SetSortRowRange(int lowRow,int highRow)
// {
// 	ASSERT( lowRow >=0 && lowRow < GetRowCount());
// 	ASSERT( highRow >=0 && highRow < GetRowCount());
// 
// 	m_nLowSortRow   = lowRow;
// 	m_nHightSortRow = highRow;
//}
void CGridCtrl::OnFixedRowClick(CCellID& cell,CPoint PointCellRelative)
{
    if (!IsValid(cell))
        return;	

	CGridCellBase * pCell = GetCell(cell.row,cell.col);
	if (pCell->IsKindOf(RUNTIME_CLASS(CGridCellCheck)))
	{
		CGridCellCheck * pCheckCell = (CGridCellCheck *)pCell;
		CPoint pt = GetPointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, PointCellRelative);
		pCheckCell->OnClick(pt);		
	}
	
	// L"序号"这一列不要排序
	
	int32 iColNum = -1;
	bool32 bFound  = false;
	int32 i = 0;
	for (int32 i = 0 ;i< GetColumnCount(); i++)
	{
		CString StrText =  GetItemText(0,i);
		if (StrText == CReportScheme::GetReportHeaderCnName(CReportScheme::ERHRowNo))
		{
			bFound   = true;
			iColNum  = i;
			break;
		}
	}

	if ( cell.col != iColNum ) 
	{	
		if (GetHeaderSort())
		{
			CWaitCursor waiter;
			if (cell.col == GetSortColumn())
				SortItems(cell.col, !GetSortAscending());
			else
				SortItems(cell.col, GetInitSortAscending());
			Invalidate();
		}
	}

	if (bFound)
	{
		CString StrRowNo;
		for (i = 1; i<GetRowCount();i++)
		{
			StrRowNo.Format(L"%d",i);
			GetCell(i,iColNum)->SetText(StrRowNo);
		}
		
	}
	
	// Did the user click on a fixed column cell (so the cell was within the overlap of
	// fixed row and column cells) - (fix by David Pritchard)

	// fangz0602 # for fixclick 不要计算fix 的选中
// 	if (GetFixedColumnSelection())
// 	{
// 		if (cell.col < GetFixedColumnCount())
// 		{
// 			m_MouseMode = MOUSE_SELECT_ALL;
// 			OnSelecting(cell);
// 		}
// 		else 
// 		{
// 			m_MouseMode = MOUSE_SELECT_COL;
// 			OnSelecting(cell);
// 		}
//	}
}

void CGridCtrl::OnFixedColumnClick(CCellID& cell,CPoint PointCellRelative)
{
    if (!IsValid(cell))
        return;

//    if (m_bListMode && (GetItemState(cell.row, m_nFixedCols) & GVNI_SELECTED))
//    {
//        OnEditCell(cell.row, cell.col, VK_LBUTTON);
//        return;
//    }

	// Did the user click on a fixed row cell (so the cell was within the overlap of
	// fixed row and column cells) - (fix by David Pritchard)
	
	CGridCellBase * pCell = GetCell(cell.row,cell.col);
	if (pCell->IsKindOf(RUNTIME_CLASS(CGridCellCheck)))
	{
		CGridCellCheck * pCheckCell = (CGridCellCheck *)pCell;
    	CPoint pt = GetPointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, PointCellRelative);
		pCheckCell->OnClick(pt);		
	}

    if (GetFixedRowSelection())
    {
        if (cell.row < GetFixedRowCount())
        {
            m_MouseMode = MOUSE_SELECT_ALL;
            OnSelecting(cell);
        }
        else
        {
            m_MouseMode = MOUSE_SELECT_ROW;
            OnSelecting(cell);
        }
    }
}

void CGridCtrl::SetEditableCols(CArray<int32,int32> &aEditableCols)
{
	m_aEditCols.RemoveAll();
	m_aEditCols.Copy(aEditableCols);
}

// Gets the extent of the text pointed to by str (no CDC needed)
// By default this uses the selected font (which is a bigger font)
CSize CGridCtrl::GetTextExtent(int nRow, int nCol, LPCTSTR str)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return CSize(0, 0);
    else
        return pCell->GetTextExtent(str);
}

// virtual
void CGridCtrl::OnEditCell(int nRow, int nCol, CPoint point, UINT nChar)
{
#ifndef GRIDCONTROL_NO_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
	m_TipWnd.Hide();
#endif

    // Can we do it?
    CCellID cell(nRow, nCol);
    if (!IsValid(cell) || !IsCellEditable(nRow, nCol))
        return;

    // Can we see what we are doing?
    EnsureVisible(nRow, nCol);
    if (!IsCellVisible(nRow, nCol))
        return;

    // Where, exactly, are we gonna do it??
    CRect rect;
    if (!GetCellRect(cell, rect))
        return;

    // Check we can edit...
    if (SendMessageToParent(nRow, nCol, (int)GVN_BEGINLABELEDIT) >= 0)
    {
        // Let's do it...
        CGridCellBase* pCell = GetCell(nRow, nCol);

		if ( m_bSomeColsEditable)
		{
			// 部分列可编辑
			bool bFind = false;
			for ( int i = 0 ; i < m_aEditCols.GetSize(); i++)
			{
				if ( nCol == m_aEditCols.GetAt(i))
				{
					bFind = true;
					break;
				}
			}
			
			if ( bFind && pCell )
			{
				pCell->Edit(nRow, nCol, rect, point, IDC_INPLACE_CONTROL, nChar);
			}
		}
		else
		{
			// 原来的处理
			if (pCell)
				pCell->Edit(nRow, nCol, rect, point, IDC_INPLACE_CONTROL, nChar);
		}
    }
}
// virtual
BOOL CGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	if ( m_bFullDragAble )
	{
		cs.style |= WS_CLIPSIBLINGS;
	}
	return CWnd::PreCreateWindow(cs);
}

// virtual
void CGridCtrl::EndEditing()
{
    CCellID cell = GetFocusCell();
    if (!IsValid(cell)) return;
    CGridCellBase *pCell = GetCell(cell.row, cell.col);
    if (pCell)
        pCell->EndEdit();
}

// virtual
void CGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
{
    CString strCurrentText = GetItemText(nRow, nCol);
	if ( NULL != m_pUserCB )
	{
		// 返回值为 false 的时候,把这个格子输入的内容设置为空		
		if ( !m_pUserCB->OnEditEndCB(nRow,nCol,strCurrentText,str))
		{
			// str = "";
			str = strCurrentText;
		}
	}

    if (strCurrentText != str)
    {
        SetItemText(nRow, nCol, str);
        if (ValidateEdit(nRow, nCol, str) && 
            SendMessageToParent(nRow, nCol, (int)GVN_ENDLABELEDIT) >= 0)
        {
            SetModified(TRUE, nRow, nCol);
            RedrawCell(nRow, nCol);
        }
        else
        {
            SetItemText(nRow, nCol, strCurrentText);
        }
    }

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (pCell)
        pCell->OnEndEdit();
}

// If this returns FALSE then the editing isn't allowed
// virtual
BOOL CGridCtrl::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return TRUE;

    return pCell->ValidateEdit(str);
}

// virtual
CString CGridCtrl::GetItemText(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols)
        return _T("");

    CGridCellBase* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
        return _T("");

    return pCell->GetText();
}

void CGridCtrl::ResetVirtualOrder()
{
	m_arRowOrder.resize(m_nRows);  
    for (int i = 0; i < m_nRows; i++)
	{
		m_arRowOrder[i] = i;	
	}
}


void CGridCtrl::Reorder(int From, int To)
{
		// Set line From just after Line To
	ASSERT(From>= GetFixedRowCount() && To>=GetFixedRowCount()-1 && From<m_nRows && To<m_nRows);
	int Value = m_arRowOrder[From];
	m_arRowOrder.erase(m_arRowOrder.begin()+From);
	int Offset = (From>=To ? 1:0);
	m_arRowOrder.insert(m_arRowOrder.begin()+To+Offset, Value);
}

void CGridCtrl::SetScrollBar(CXScrollBar *pXSBHorz, CXScrollBar *pXSBVert)
{
	m_pXSBHorz = pXSBHorz;
	m_pXSBVert = pXSBVert;

	ResetScrollBars();
}
void CGridCtrl::OnDestroy() 
{
	KillTimer(KTimerIdShowTextChange);	
	m_TipWnd.DestroyWindow();
	CWnd::OnDestroy();
}

void CGridCtrl::OnSetFocus( CWnd* pOldWnd )
{
	if ( !m_bShowSelectWhenLoseFocus )
	{
		CCellRange rangeSel = GetSelectedCellRange();
		if ( rangeSel.IsValid() )
		{
			InvalidateCellRect(rangeSel);
		}
	}
	CWnd::OnSetFocus(pOldWnd);
}

void CGridCtrl::OnKillFocus( CWnd* pNewWnd )
{
	if ( !m_bShowSelectWhenLoseFocus )
	{
		CCellRange rangeSel = GetSelectedCellRange();
		if ( rangeSel.IsValid() )
		{
			InvalidateCellRect(rangeSel);
		}
	}
	CWnd::OnKillFocus(pNewWnd);
}

void CGridCtrl::BlindKeyFocusCell(bool32 bBlind)
{
	m_bBlindKeyFocusCell = bBlind;
}

int CGridCtrl::GetNextSelectedRow(INOUT POSITION &pos) const
{
	CCellID cell(-1,-1);
	if ( NULL == pos )
	{
		return -1;
	}
	int nRow = m_SelectedRowList.GetNext(pos);
	if ( nRow >= GetFixedRowCount() && nRow < GetRowCount() )
	{
		return nRow;
	}
	return -1;
}

POSITION CGridCtrl::GetFirstSelectedRowPosition() const
{
	ASSERT( GetListMode() );
	UpdateSelectedRows();	// 更新选择的行信息
	return m_SelectedRowList.GetHeadPosition();
}

void CGridCtrl::UpdateSelectedRows() const
{
	if ( GetListMode() )
	{
		// 清除原队列中现在已经没有selected的行
		// 添加原队列中没有的到队尾(对于shift键添加的需要特殊处理)
		typedef set<int> RowSet;
		RowSet Rows;
		POSITION pos;
		DWORD dwKey;
		CCellID cellId;
		RowSet::iterator it;
		for ( pos=m_SelectedCellMap.GetStartPosition(); NULL != pos ; /*NULL*/ )
		{
			m_SelectedCellMap.GetNextAssoc(pos, dwKey, cellId);
			Rows.insert(cellId.row);
		}
		for ( pos=m_SelectedRowList.GetHeadPosition(); NULL != pos ;  )
		{
			POSITION posOld = pos;
			int iRow = m_SelectedRowList.GetNext(pos);
			it = Rows.find(iRow);
			if ( it == Rows.end() )
			{
				m_SelectedRowList.RemoveAt(posOld);
			}
			else
			{
				Rows.erase(it);
			}
		}
		for ( it=Rows.begin(); it != Rows.end() ; ++it )
		{
			m_SelectedRowList.AddTail(*it);
		}
	}
	else
	{
		m_SelectedRowList.RemoveAll();
	}
}

//lxp add 按比例调整列宽
BOOL CGridCtrl::AutoSizeColumnlv(const float &flv,int nCol, UINT nAutoSizeStyle/* = GVS_DEFAULT*/, BOOL bResetScroll/* = TRUE*/)
{
	ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return FALSE;
	
    //  Skip hidden columns when autosizing
    if( GetColumnWidth( nCol) <=0 )
        return FALSE;
	
    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return FALSE;
	
    int nWidth = 0;	
	
    ASSERT(nAutoSizeStyle <= GVS_BOTH);
    if (nAutoSizeStyle == GVS_DEFAULT)
        nAutoSizeStyle = GetAutoSizeStyle();
	
    int nStartRow = (nAutoSizeStyle & GVS_HEADER)? 0 : GetFixedRowCount();
    int nEndRow   = (nAutoSizeStyle & GVS_DATA)? GetRowCount()-1 : GetFixedRowCount()-1;
	
    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(nStartRow, nCol, nEndRow, nCol));
	
	int nOldWidth = 0;
	if( nOldWidth == 0 )
	{
		nOldWidth = GetColumnWidth(nCol);
		nOldWidth = int(nOldWidth * flv);
	}

    for (int nRow = nStartRow; nRow <= nEndRow; nRow++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);

        if (size.cx > nWidth)
            nWidth = size.cx;
    }
	
    if (GetVirtualMode())
        SendCacheHintToParent(CCellRange(-1,-1,-1,-1));
	
	// 如果没有数据可调整，不应当重置为0的宽度，而应该保持原来的宽度 xl 0809
	if ( nWidth <= 0 )
	{
		nWidth = GetDefCellWidth();	// 取默认宽度
	}
	
	if( nOldWidth > nWidth)
		nWidth = nOldWidth;

    m_arColWidths[nCol] = nWidth;
	
    ReleaseDC(pDC);
    if (bResetScroll)
        ResetScrollBars();
	
    return TRUE;
}

CSize  CGridCtrl::GetColumuExtent(const LOGFONT* pLogFont,bool bSingle)
{
	CSize size(0,0);
	ASSERT(pLogFont);
    if (!pLogFont) 
		return size;
	
	CFont FontTemp;
    FontTemp.DeleteObject();
    FontTemp.CreateFontIndirect(pLogFont);
		
    // Get the font size and hence the default cell size
    CDC* pDC = CDC::FromHandle(::GetDC(NULL));
    if (pDC)
    {
        CFont* pOldFont = pDC->SelectObject(&FontTemp);
		
		if( bSingle )
		{
			CString strText=_T("一");
			size = pDC->GetTextExtent(strText);
		}
		else
		{
			CString strText=_T("一二三四五六七八九");
			size = pDC->GetTextExtent(strText);
			size.cx = size.cy * 9;
			size.cy = size.cy +4;//(m_Size.cy * 3) / 2;  //lxp modify
		}
        pDC->SelectObject(pOldFont);
        ::ReleaseDC(NULL, pDC->GetSafeHdc());
    }

	return size;
}

void CGridCtrl::AutoSizeColumnslv(const float &flv,UINT nAutoSizeStyle /*=GVS_DEFAULT*/)
{
	int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Skip hidden columns when autosizing
        if( GetColumnWidth( nCol) > 0 )
            AutoSizeColumnlv(flv,nCol, nAutoSizeStyle, FALSE);
    }
    ResetScrollBars();
}
#include "StdAfx.h"

#include "GridCellSys.h"
#include "GridCtrlSys.h"

#include "MPIChildFrame.h"
#include "IoViewManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CGridCtrlSys, CGridCtrl)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CGridCtrlSys, CGridCtrl)
	//{{AFX_MSG_MAP(CGridCtrlSys)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CGridCtrlSys::CGridCtrlSys(int nRows, int nCols, int nFixedRows, int nFixedCols)
:CGridCtrl(nRows, nCols, nFixedRows, nFixedCols)
{
	m_nBarState           = GVL_NONE;
    m_MouseMode           = MOUSE_NOTHING;
    m_nGridLines          = /*GVL_BOTH*/GVL_NONE;	// zhangbo 0409 #set default
    m_bEditable           = /*TRUE*/FALSE;	// zhangbo 0409 #set default
    m_bListMode           = /*FALSE*/TRUE;	// zhangbo 0409 #set default
    m_bSingleRowSelection = /*FALSE*/TRUE;	// zhangbo 0409 #set default
    m_bSingleColSelection = /*FALSE*/TRUE;	// zhangbo 0409 #set default

    m_bLMouseButtonDown   = FALSE;
    m_bRMouseButtonDown   = FALSE;
    m_bAllowDraw          = TRUE;       // allow draw updates
    m_bEnableSelection    = TRUE;
    m_bFixedColumnSelection = TRUE;
    m_bFixedRowSelection  = TRUE;
    m_bAllowRowResize     = false;		// 不允许拉动列高
    m_bAllowColumnResize  = TRUE;
    m_bSortOnClick        = FALSE;      // Sort on header row click
    m_bHandleTabKey       = TRUE;
#ifdef _WIN32_WCE
    m_bDoubleBuffer       = FALSE;      // Use double buffering to avoid flicker?
#else
    m_bDoubleBuffer       = TRUE;       // Use double buffering to avoid flicker?
#endif
    m_bTitleTips          = /*TRUE*/FALSE; // zhangbo 0409 #set default      // show cell title tips

    m_bWysiwygPrinting    = FALSE;      // use size-to-width printing

    m_bHiddenColUnhide    = TRUE;       // 0-width columns can be expanded via mouse
    m_bHiddenRowUnhide    = TRUE;       // 0-Height rows can be expanded via mouse

    m_bAllowColHide       = /*TRUE*/FALSE;  // zhangbo 0409 #set default     // Columns can be contracted to 0-width via mouse
    m_bAllowRowHide       = /*TRUE*/FALSE;  // zhangbo 0409 #set default     // Rows can be contracted to 0-height via mouse

    m_bAscending          = TRUE;       // sorting stuff
    m_nSortColumn         = -1;
	m_pfnCompare		  = NULL;
    m_nAutoSizeColumnStyle = GVS_BOTH;  // Autosize grid using header and data info

    m_nTimerID            = 0;          // For drag-selection
    m_nTimerInterval      = 25;         // (in milliseconds)
    m_nResizeCaptureRange = 3;          // When resizing columns/row, the cursor has to be
                                        // within +/-3 pixels of the dividing line for
                                        // resizing to be possible
    m_pImageList          = NULL;       // Images in the grid
    m_bAllowDragAndDrop   = FALSE;      // for drag and drop - EFW - off by default
    m_bTrackFocusCell     = TRUE;       // Track Focus cell?
    m_bFrameFocus         = TRUE;       // Frame the selected cell?
	m_bDragRowMode = TRUE; // allow to drop a line over another one to change row order
    m_pRtcDefault = RUNTIME_CLASS(CGridCellSys);
}

CGridCtrlSys::~CGridCtrlSys()
{
	
}

COLORREF CGridCtrlSys::GetGridBkColor() const
{
	if (NULL != CFaceScheme::Instance())
	{
		CIoViewBase * pIoViewParent = NULL; 

		CWnd *pParent = GetParent();
		pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);
// 		CRuntimeClass * pRunTime = pParent->GetRuntimeClass();
// 
// 		// 为什么只有manager下的能够设置？？
// 		for ( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
// 		{
// 			if ( pRunTime == CIoViewManager::GetIoViewObject(i)->m_pIoViewClass)
// 			{
// 				pIoViewParent = (CIoViewBase*)pParent;
// 				break;
// 			}
// 		}	

// 		if (pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)))
// 		{
// 			pIoViewParent = (CIoViewBase *)pIoViewParent;
// 		}

		if ( NULL != pIoViewParent)
		{
			return pIoViewParent->GetIoViewColor(ESCBackground);
		}
		return CFaceScheme::Instance()->GetSysColor(ESCBackground);
	}

	return CGridCtrl::GetGridBkColor();
}

COLORREF CGridCtrlSys::GetGridLineColor() const
{
	if (NULL != CFaceScheme::Instance())
	{
		CIoViewBase * pIoViewParent = NULL; 
		
		CWnd * pParent = GetParent();

		pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);
// 		CRuntimeClass * pRunTime = pParent->GetRuntimeClass();
// 		
// 		for ( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
// 		{
// 			if ( pRunTime == CIoViewManager::GetIoViewObject(i)->m_pIoViewClass)
// 			{
// 				pIoViewParent = (CIoViewBase*)pParent;
// 				break;
// 			}
// 		}
		
// 		if (pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)))
// 		{
// 			pIoViewParent = (CIoViewBase *)pIoViewParent;
// 		}
		if ( NULL != pIoViewParent)
		{
			return pIoViewParent->GetIoViewColor(ESCGridLine);
		}
		return CFaceScheme::Instance()->GetSysColor(ESCGridLine);

	}
	return CGridCtrl::GetGridLineColor();
}

// 平分各列
void CGridCtrlSys::ExpandColumnsToFit(BOOL bExpandFixed /*=TRUE*/)
{
	int32 iNumCol = GetColumnCount();
	if (iNumCol <= 0)
		return;

 //   EnableScrollBars(SB_HORZ, FALSE);

    CRect rect;
    GetClientRect(rect);


	if (bExpandFixed)
	{
		int32 iWidthAve = rect.Width() / iNumCol;
		
		for (int32 i = 0; i < iNumCol; i++)
		{
			m_arColWidths[i] = iWidthAve;
		}
		
		// 剩下的空间挨个像素的加， 加到哪是哪
		if (iWidthAve > 0)
		{
			int32 iPixelsLeft = rect.Width() % iWidthAve;
			
			while (iPixelsLeft > 0)
			{
				for (int32 i = 0; i < iNumCol && iPixelsLeft > 0; i++, iPixelsLeft--)
				{
					m_arColWidths[i]++;
				}
			}
		}
	}
	else
	{
		// 固定项的不拉伸
		if (m_nFixedCols >= iNumCol)
		{
			return;
		}

		int32 iBeginIndex = m_nFixedCols;
		
		//
		int32 iWidthFixAll = 0;
		int32 i = 0;
		for (i = 0; i < m_nFixedCols; i++)
		{
			// 固定项目的宽度加起来
			iWidthFixAll += m_arColWidths[i];
		}

		//
		int32 iWidthLeft = rect.Width() - iWidthFixAll;
		if (iWidthLeft <= 0)
		{
			return;
		}

		//
		int32 iWidthAve = iWidthLeft / (iNumCol-m_nFixedCols);
		int32 iPixelsLeft = iWidthLeft % iWidthAve;

		if (iWidthAve <= 0)
		{
			return;
		}

		//
		for (i = iBeginIndex; i < iNumCol; i++, iPixelsLeft--)
		{
			m_arColWidths[i] = iWidthAve;

			if (iPixelsLeft > 0)
			{
				// m_arColWidths[i] += iPixelsLeft;
			}
		}
	}
	
 //   Refresh();
 //   ResetScrollBars();
}

// 平分各列
void CGridCtrlSys::ExpandRowsToFit(BOOL bExpandFixed /*=TRUE*/)
{
	int32 iNumRow = GetRowCount();
	if (iNumRow <= 0)
		return;

    EnableScrollBars(SB_VERT, FALSE);

    CRect rect;
    GetClientRect(rect);

	int32 iHeightAve = rect.Height() / iNumRow;

    for (int32 i = 0; i < iNumRow; i++)
    {
        m_arRowHeights[i] = iHeightAve;
    }

    // 剩下的空间挨个像素的加， 加到哪是哪
	if (iHeightAve > 0)
	{
		int32 iPixelsLeft = rect.Height() % iHeightAve;
	
		while (iPixelsLeft > 0)
		{
			for (int32 i = 0; i < iNumRow && iPixelsLeft > 0; i++, iPixelsLeft--)
			{
				m_arRowHeights[i]++;
			}
		}
	}
	
    Refresh();
    ResetScrollBars();
}




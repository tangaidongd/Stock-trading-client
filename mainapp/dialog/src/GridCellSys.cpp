#include "stdafx.h"

#include "GridCtrl.h"
#include "InPlaceEdit.h"
#include "facescheme.h"

#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "GridCellSys.h"
#include "ShareFun.h"
#include "DrawPolygon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridCellSys, CGridCellBase)

CGridCellSys::CGridCellSys()
{
	m_plfFont		  = NULL;
	m_bDrawBottomLine = false;
	m_bDrawLeftLine	  = false;
	m_bDrawRightLine  = false;
	m_bRectCellLineDot= false;
	m_ClrRectCellLineClr = (COLORREF)-1;
	m_uiCellRectFlag  = DR_NONE;
	m_lParam		  = NULL;

	m_bParentActive		= false;
	m_iXposParentActive = -1;
	m_iYposParentActive	= -1;
	m_ClrParentActive	= RGB(0, 0, 0);

	CGridCellSys::Reset();	
}

CGridCellSys::~CGridCellSys()
{
	delete m_plfFont;
	m_plfFont = NULL;
}

void CGridCellSys::SetBlinkText(LPCTSTR szText, bool32 bBlinkOnlyChange)
{
	if (bBlinkOnlyChange)
	{
		if (m_strText == szText)
			return;
	}

	SetState(GetState() | GVIS_BLINK);
	m_iBlinkStep = BLINK_STEP_COUNT;
	
	m_strText = szText; 
}

BOOL CGridCellSys::GetTextRect( LPRECT pRect)  // i/o:  i=dims of cell rect; o=dims of text rect
{
    if (GetImage() >= 0)
    {
        IMAGEINFO Info;
		
        CGridCtrl* pGrid = GetGrid();
        CImageList* pImageList = pGrid->GetImageList();
        
        if (pImageList && pImageList->GetImageInfo( GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            pRect->right -= (nImageWidth + GetMargin());
        }
    }
	
    return TRUE;
}

// By default this uses the selected font (which is a bigger font)
CSize CGridCellSys::GetTextExtent(LPCTSTR szText, CDC* pDC /*= NULL*/)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);
	
	CString StrText;
	if (NULL != szText)
		StrText = szText;
	
    BOOL bReleaseDC = FALSE;
    if (NULL == pDC)
	{
		pDC = pGrid->GetDC();
        if (NULL == pDC)
			return CSize(0, 0);
        
        bReleaseDC = TRUE;
    }
	
    CFont *pOldFont = NULL;
    CFont *pFont = GetFontObject();
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);
	
    CSize size;
    int nFormat = GetFormat();
	
    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = StrText;
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;
			
            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth+1, 0);
        pDC->DrawText(szText, -1, rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
        size = pDC->GetTextExtent(szText, (int)_tcslen(szText));
	
    // Removed by Yogurt
    //TEXTMETRIC tm;
    //pDC->GetTextMetrics(&tm);
    //size.cx += (tm.tmOverhang);
	
    if (pOldFont)
        pDC->SelectObject(pOldFont);
    
    size += CSize(2*GetMargin(), 2*GetMargin());
	
    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }
    
    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);
	
    return size;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Attributes
// 这个赋值操作符是调用基类的实现的，所以屏蔽1539错误成员变量没有被赋值-cfj
//lint --e{1539}
void CGridCellSys::operator=(const CGridCellSys& cell)
{
    if (this != &cell) 
	{
		CGridCellBase::operator=(cell);
	}
}

void CGridCellSys::Reset()
{
    CGridCellBase::Reset();
	
    m_strText.Empty();
    m_nImage   = -1;
    m_lParam   = NULL;           // BUG FIX J. Bloggs 20/10/03
    m_pGrid    = NULL;
    m_bEditing = FALSE;
    m_pEditWnd = NULL;
	
#ifdef _WIN32_WCE
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#else
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#endif
    m_crFgClr = CLR_DEFAULT;
    m_crBkClr = CLR_DEFAULT;
    m_nMargin = (UINT)1;			
	
    delete m_plfFont;
    m_plfFont = NULL;            // Cell font

	m_eDefaultTextColor = ESCText;
	m_eDefaultTextBkColor = ESCBackground;
	m_eDefaultFont	= ESFNormal;
}

void CGridCellSys::SetFont(const LOGFONT* plf)
{
    if (plf == NULL)
    {
        delete m_plfFont;
        m_plfFont = NULL;
    }
    else
    {
        if (!m_plfFont)
            m_plfFont = new LOGFONT;
        if (m_plfFont)
            memcpyex(m_plfFont, plf, sizeof(LOGFONT)); 
    }
}

LOGFONT* CGridCellSys::GetFont() const
{
    if (NULL == m_plfFont)
    {
		CIoViewBase * pIoViewParent = NULL; 
		
		CWnd * pParent = GetGrid()->GetParent();
		
		// 		if (pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)))
		// 		{
		// 			pIoViewParent = (CIoViewBase *)pIoViewParent;
		// 		}
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
		
		LOGFONT * pFont = NULL;
		if ( NULL != pIoViewParent)
		{
			if ( IsFixed() )
			{
				pFont = pIoViewParent->GetIoViewFont(m_eDefaultFont);
			}
			else
			{
				pFont = pIoViewParent->GetIoViewFont(m_eDefaultFont);
			}
			
		}
		else
		{
			if ( IsFixed() )
			{
				pFont = CFaceScheme::Instance()->GetSysFont(m_eDefaultFont);
			}
			else
			{
				pFont = CFaceScheme::Instance()->GetSysFont(m_eDefaultFont);
			}
			
		}
		ASSERT( NULL != pFont );
		return pFont;
    }
	
    return m_plfFont; 
}

CFont* CGridCellSys::GetFontObject() const
{
    // If the default font is specified, use the default cell implementation
    if (m_plfFont == NULL)
    {
		CIoViewBase * pIoViewParent = NULL; 
		
		CWnd * pParent = GetGrid()->GetParent();
		
// 		if (pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)))
// 		{
// 			pIoViewParent = (CIoViewBase *)pIoViewParent;
// 		}

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
		
		CFont * pFont = NULL;
		if ( NULL != pIoViewParent)
		{
			if ( IsFixed() )
			{
				pFont = pIoViewParent->GetIoViewFontObject(m_eDefaultFont);
			}
			else
			{
				pFont = pIoViewParent->GetIoViewFontObject(m_eDefaultFont);
			}
		}
		else
		{
			if ( IsFixed() )
			{
				pFont = CFaceScheme::Instance()->GetSysFontObject(m_eDefaultFont);
			}
			else
			{
				pFont = CFaceScheme::Instance()->GetSysFontObject(m_eDefaultFont);
			}
			
		}
		ASSERT( NULL != pFont );
		return pFont;
    }
    else
    {
        static CFont fontObj;
        fontObj.DeleteObject();
        fontObj.CreateFontIndirect(m_plfFont);
        return &fontObj;
    }
}

DWORD CGridCellSys::GetFormat() const
{
    return m_nFormat; 
}

UINT CGridCellSys::GetMargin() const           
{
	//    return m_nMargin; 
	return 2;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

BOOL CGridCellSys::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	pApp->m_bBindHotkey = false;

    if ( m_bEditing && m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{      
//        if (m_pEditWnd)
			m_pEditWnd->SendMessage ( WM_CHAR, nChar );    
    }  
	else  
	{   
		DWORD dwStyle = ES_LEFT;
		if (GetFormat() & DT_RIGHT) 
			dwStyle = ES_RIGHT;
		else if (GetFormat() & DT_CENTER) 
			dwStyle = ES_CENTER;
		
		m_bEditing = TRUE;
		
		// InPlaceEdit auto-deletes itself
		CGridCtrl* pGrid = GetGrid();
		m_pEditWnd = new CInPlaceEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    }
    return TRUE;
}

void CGridCellSys::EndEdit()
{
    if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
        ((CInPlaceEdit*)m_pEditWnd)->EndEdit();

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (pApp)
	{
		pApp->m_bBindHotkey =true;
	}
	
}

void CGridCellSys::OnEndEdit()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	
	if (!pApp->m_bBindHotkey )
	{
		pApp->m_bBindHotkey =true;
	}
    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

void CGridCellSys::SetDefaultTextColor(E_SysColor eTextColor)
{
	m_eDefaultTextColor = eTextColor;
}

void CGridCellSys::SetDefaultTextBkColor(E_SysColor eTextBkColor)
{
	m_eDefaultTextBkColor = eTextBkColor;
}

void CGridCellSys::SetDefaultFont(E_SysFont eFont)
{
	m_eDefaultFont = eFont;
}

void CGridCellSys::DrawBottomLine(bool32 bDraw, COLORREF clr)
{
	m_bDrawBottomLine	 = bDraw;
	m_ClrRectCellLineClr = clr;
}

void CGridCellSys::DrawLeftLine(bool32 bDraw, COLORREF clr)
{
	m_bDrawLeftLine		 = bDraw;
	m_ClrRectCellLineClr = clr;
}

void CGridCellSys::DrawRightLine(bool32 bDraw, COLORREF clr)
{
	m_bDrawRightLine	 = bDraw;
	m_ClrRectCellLineClr = clr;
}

void CGridCellSys::SetRectLineDot(bool32 bDotLine)
{
	m_bRectCellLineDot = bDotLine;
}

UINT CGridCellSys::GetCellRectDrawFlag()
{
	return m_uiCellRectFlag;
}

void CGridCellSys::SetCellRectDrawFlag(UINT uiFlag)
{
	m_uiCellRectFlag = uiFlag;
}

void DrawDotLine (CDC* pDC, CPoint pt1, CPoint pt2, COLORREF clr )
{
	// 只能画竖线或横线
	if ( NULL == pDC )
	{
		return;
	}

	//
	if ( pt1.y == pt2.y )
	{
		for ( int32 ix = pt1.x; ix < pt2.x; ix++ )
		{
			if ( 0 == ix % 2 )
			{
				pDC->SetPixel(ix, pt1.y, clr);
			}
		}
	}

	if ( pt1.x == pt2.x )
	{
		for ( int32 iy = pt1.y; iy < pt2.y; iy++ )
		{
			if ( 0 == iy % 2 )
			{
				pDC->SetPixel(pt1.x, iy, clr);
			}
		}
	}
}

void CGridCellSys::SetDrawParentActiveParams(int32 iXpos, int32 iYpos, COLORREF ClrActive)
{
	m_iXposParentActive = iXpos;
	m_iYposParentActive	= iYpos;
	m_ClrParentActive	= ClrActive;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellSys::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
	// Note - all through this function we totally brutalise 'rect'. Do not
    // depend on it's value being that which was passed in.
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);
	
    if (!pGrid || !pDC)
        return FALSE;
	
    if( rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;                             //  though cell is hidden

	DWORD nDrawSelectedStyle = pGrid->GetDrawSelectedCellStyle();
	bool32 bDrawSelectAndFocus = (pGrid->GetSafeHwnd() == GetFocus()) || pGrid->IsShowSelectWhenLoseFocus();		// 允许绘制选择

	CIoViewBase * pIoViewParent = NULL; 
	
	CWnd * pParent = GetGrid()->GetParent();
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
	
	if (IsFixedRow())
	{
		COLORREF colBk;

		if (NULL != pIoViewParent)
		{			
			colBk = pIoViewParent->GetIoViewColor(ESCGridFixedBk);
		}
		else
		{
			colBk = CFaceScheme::Instance()->GetSysColor(ESCGridFixedBk); 
		}
		SetBackClr(colBk);
	}


	CRect rectGrid;
	pGrid->GetClientRect(&rectGrid);

	//
	if (  pGrid->IsAutoHideFragmentaryCell() && (rect.left < rectGrid.left || rect.right > rectGrid.right || rect.top < rectGrid.top || rect.bottom > rectGrid.bottom) )
	{	
		CString StrText = GetText();
		//linhc 20100907 添加延长线
		if ( GetGrid()->GetHideTextFlag() )
		{   
			//隐藏单元格后画延长线
			COLORREF colBk = GetBackClr();
			//linhc	20100908 添加填充背景
			if ( NULL != pIoViewParent )
			{
				colBk = pIoViewParent->GetIoViewColor(ESCBackground); 
			}
			CRect rectDraw = rect;

			rectDraw.bottom++;
		    pDC->FillSolidRect(rectDraw, colBk);
              
			CPen PenLine;	
			COLORREF clrLine = m_ClrRectCellLineClr;
			
			if ( (COLORREF)-1 == clrLine )
			{
				if ( NULL != pIoViewParent)
				{
					clrLine = pIoViewParent->GetIoViewColor(ESCChartAxisLine);			
				}
				else
				{
					clrLine = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);			
				}
			}

			PenLine.CreatePen(PS_SOLID, 10, clrLine);
			CPen * pOldpen = pDC->GetCurrentPen();
			pDC->SelectObject(PenLine);
			
			if ( CheckFlag(m_uiCellRectFlag, DR_TOP_DOT) )
			{
				// 上虚线
				DrawDotLine(pDC, CPoint(rect.left, rect.top), CPoint(rect.right, rect.top), clrLine);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_TOP_SOILD) )
			{
				// 上实线	
				pDC->MoveTo(rect.left, rect.top);
				pDC->LineTo(rect.right, rect.top);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_DOT) )
			{
				// 下虚线
				DrawDotLine(pDC, CPoint(rect.left, rect.bottom), CPoint(rect.right, rect.bottom), clrLine);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_SOILD) )
			{
				// 下实线
				pDC->MoveTo(rect.left, rect.bottom);
				pDC->LineTo(rect.right, rect.bottom);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_DOT) )
			{
				// 左虚线
				DrawDotLine(pDC, CPoint(rect.left, rect.top), CPoint(rect.left, rect.bottom), clrLine);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_SOILD) )
			{
				// 左实线
				pDC->MoveTo(rect.left, rect.top);
				pDC->LineTo(rect.left, rect.bottom);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_DOT) )
			{
				// 右虚线
				DrawDotLine(pDC, CPoint(rect.right, rect.top), CPoint(rect.right, rect.bottom), clrLine);
			}
			
			if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_SOILD) )
			{
				// 右实线
				pDC->MoveTo(rect.right, rect.top);
				pDC->LineTo(rect.right, rect.bottom);
			}
			
			pDC->SelectObject(pOldpen);	
	        PenLine.DeleteObject();
			return true;
		}

        COLORREF colBk = GetBackClr();
		
		if ( NULL != pIoViewParent )
		{
			colBk = pIoViewParent->GetIoViewColor(ESCBackground); 
		}
		CRect rectDraw = rect;
		rectDraw.left --;
		if ( pGrid->GetGridLines()&GVL_VERT )
		{
			rectDraw.left++;	// 避免覆盖前一个的gridline
		}
		rectDraw.right++;
//		rectDraw.top--;		// 存在bottom++，可以不top--
		rectDraw.bottom++;
		pDC->FillSolidRect(rectDraw, colBk);

		// xl 0909 需要将fixed cell的格子填充下
		if (IsFixed())
		{
			//CCellID FocusCell = pGrid->GetFocusCell();
			
			// As above, always show current location even in list mode so
			// that we know where the cursor is at.
			// 没用到现在不判断-cfj
			//BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
			//	(FocusCell.row == nRow || FocusCell.col == nCol);		
			
			CPen PenGridLine;
			if ( NULL != pIoViewParent)
			{
				PenGridLine.CreatePen(PS_SOLID, 1, pIoViewParent->GetIoViewColor(ESCGridLine));
			}
			else
			{
				PenGridLine.CreatePen(PS_SOLID, 1, CFaceScheme::Instance()->GetSysColor(ESCGridLine));
			}
			
			if ( GVL_NONE != GetGrid()->GetGridLines() )
			{
				CPen *pOldPen = pDC->GetCurrentPen();
				CRect rectDrawTmp = rect;
				
				if ( pGrid->IsDrawFixedCellGridLineAsNormal() )	// 按照正常表格线绘制
				{
					rectDrawTmp.InflateRect(1, 1, 0, 0);		// 需要适当扩充
					if ( rectDrawTmp.left < rectGrid.left )
					{
						rectDrawTmp.left = rectGrid.left;
					}
					if ( rectDrawTmp.top < rectGrid.top )
					{
						rectDrawTmp.top = rectGrid.top;
					}
				}
				
				pDC->SelectObject(&PenGridLine);
				pDC->MoveTo(rectDrawTmp.left, rectDrawTmp.top);
				pDC->LineTo(rectDrawTmp.left, rectDrawTmp.bottom);
				pDC->LineTo(rectDrawTmp.right, rectDrawTmp.bottom);
				pDC->LineTo(rectDrawTmp.right, rectDrawTmp.top);
				pDC->LineTo(rectDrawTmp.left, rectDrawTmp.top);
				
				pDC->SelectObject(pOldPen);
				
				rect = rectDrawTmp;	// rect需要变更
			}
			
			rect.DeflateRect(1,1);
		}

		return true;	
	}

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Set up text and background colours
    COLORREF TextClr, TextBkClr, TextNormalBkClr, ClrSelected;

	if ( NULL != pIoViewParent)
	{
		TextClr = (GetTextClr() == CLR_DEFAULT) ?pIoViewParent->GetIoViewColor(m_eDefaultTextColor) : GetTextClr(); 
	}
	else
	{
		TextClr = (GetTextClr() == CLR_DEFAULT) ? CFaceScheme::Instance()->GetSysColor(m_eDefaultTextColor) : GetTextClr();
	}
	
	if (GetBackClr() == CLR_DEFAULT)
	{
		if ( NULL != pIoViewParent)
		{
			TextBkClr = pIoViewParent->GetIoViewColor(m_eDefaultTextBkColor);
		}
		else
		{
			TextBkClr = CFaceScheme::Instance()->GetSysColor(m_eDefaultTextBkColor);
		}
	}
	else
	{
		bEraseBkgnd = TRUE;
		TextBkClr = GetBackClr();
	}
	TextNormalBkClr = TextBkClr;
	ClrSelected		= TextBkClr;
	if ( bDrawSelectAndFocus && (GetState() & GVIS_SELECTED) )
	{
		if ( NULL != pIoViewParent)
		{
			ClrSelected = pIoViewParent->GetIoViewColor(ESCGridSelected);
		}
		else
		{
			ClrSelected = CFaceScheme::Instance()->GetSysColor(ESCGridSelected);	
		}

		if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
		{
			// 底线画法，背景为普通背景色
		}
		else
		{
			// 默认画法，select下，背景为select色
			TextBkClr = ClrSelected;
		}
	}

	// Draw cell background and highlighting (if necessary)
    if ( bDrawSelectAndFocus && (IsFocused() || IsDropHighlighted()) )	// 有焦点
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(GetState() & GVIS_SELECTED)
        {
			bEraseBkgnd = TRUE;
        }
		
		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}
		
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            TRY 
            {
                CBrush brush(TextBkClr);
				if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
				{
					// 背景色
					pDC->FillRect(rect, &brush);
					
					// 底部画一条线
					CRect rcTmp(rect);
					rcTmp.bottom -= 1;
					rcTmp.top = rcTmp.bottom-1;
					pDC->FillSolidRect(rcTmp, ClrSelected);
					pDC->SetBkColor(TextBkClr);
				}
				else
				{
					pDC->FillRect(rect, &brush);
				}
            }
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }
		
        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }
		
//         if (pGrid->GetFrameFocusCell() && pGrid->IsSelectable() && GVSDS_BOTTOMLINE != nDrawSelectedStyle )
//         {
//             // Use same color as text to outline the cell so that it shows
//             // up if the background is black.
// 			// 底部选择线不画focus
//             TRY 
//             {
//                 CBrush brush(TextClr);
//                 pDC->FrameRect(rect, &brush);
//             }
//             CATCH(CResourceException, e)
//             {
//                 //e->ReportError();
//             }
//             END_CATCH
//         }
		
        // Adjust rect after frame draw if no grid lines
        if(pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }
		
		//rect.DeflateRect(0,1,1,1);  - Removed by Yogurt
    }
    else if ( bDrawSelectAndFocus && ((GetState() & GVIS_SELECTED)) )	// 被选中
    {			
		CString StrTest;
		if ( pGrid->GetVirtualMode() )		// 虚模式下不要调用getitemtext - 其实都可以gettext xl 0810
		{
			StrTest = GetText();
		}
		else
		{
			StrTest	=  pGrid->GetItemText(nRow,nCol);
		}
		
		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}
		else
		{		
			bEraseBkgnd = TRUE;
		}
		
		rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		if ( GVSDS_BOTTOMLINE == nDrawSelectedStyle )
		{
			// 背景色
			pDC->FillSolidRect(rect, TextBkClr);
			// 底部画一条线
			CRect rcTmp(rect);
			rcTmp.bottom -= 1;
			rcTmp.top = rcTmp.bottom-1;
			pDC->FillSolidRect(rcTmp, ClrSelected);
			pDC->SetBkColor(TextBkClr);
		}
		else
		{
			pDC->FillSolidRect(rect, TextBkClr);
// 			COLORREF colBK;
// 			if (NULL != pIoViewParent)
// 			{			
// 				colBK = pIoViewParent->GetIoViewColor(ESCGridSelected);
// 			}
// 			else
// 			{
// 				colBK = CFaceScheme::Instance()->GetSysColor(ESCGridSelected); 
// 			}
// 			 CBrush brushBk(colBK/*CFaceScheme::Instance()->GetSysColor(ESCGridSelected)*/);
// 			 pDC->FillRect(rect, &brushBk);
// 			 CBrush brush(TextBkClr);
// 			 CRect rcTmp = rect;
// 			 rcTmp.left += 1;
// 			 rcTmp.top +=1;
// 			 rcTmp.right -= 1;
// 			 rcTmp.bottom -= 1;
// 			 pDC->FillRect(rcTmp, &brush);
		}
		rect.right--; rect.bottom--;
	}
    else
    {
		// zhangbo 0402 # for blink
		if (GetState() & GVIS_BLINK && m_iBlinkStep > 0)
		{
			COLORREF clrBkOld = TextBkClr;
			if ( clrBkOld == 0xee0000 )
			{
				clrBkOld = RGB(127,127,127);
			}
			CGridCellBase::GetSmoothColor(clrBkOld, 0xee0000, m_iBlinkStep, BLINK_STEP_COUNT, TextBkClr);
			bEraseBkgnd = TRUE;
		}
		
        if (bEraseBkgnd)
        {
            rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
			COLORREF colBK;
			if (NULL != pIoViewParent)
			{			
				colBK = pIoViewParent->GetIoViewColor(ESCBackground);
			}
			else
			{
				colBK = CFaceScheme::Instance()->GetSysColor(ESCBackground); 
			}
			CBrush brushBk/*(colBK/ *CFaceScheme::Instance()->GetSysColor(ESCBackground)* /)*/;
			//---wangyongxue 2016/06/13 标题栏去掉分割线
			COLORREF colBKLine;
			if (pIoViewParent)
			{
				colBKLine = pIoViewParent->GetIoViewColor(ESCGridFixedBk);
			}
			else
			{
				colBKLine = colBK;
			}
			
			if(IsFixedRow())
				brushBk.CreateSolidBrush(colBKLine);
			else
				brushBk.CreateSolidBrush(colBK);
			
			if(pGrid->GetNeedShowFixSpliter() == FALSE)
			{
				brushBk.CreateSolidBrush(TextBkClr);
			}

			pDC->FillRect(rect, &brushBk);
            CBrush brush(TextBkClr);
			CRect rcTmp = rect;
			rcTmp.left += 1;
			rcTmp.top +=1;
			rcTmp.right -= 1;
			rcTmp.bottom -= 1;
            pDC->FillRect(rcTmp, &brush);
            rect.right--; rect.bottom--;
        }
    }

    // fix项一定要显示分割线
    if (IsFixed())
    {
        //CCellID FocusCell = pGrid->GetFocusCell();
		
        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
		// 没用到现在不判断-cfj
		// BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
		//	(FocusCell.row == nRow || FocusCell.col == nCol);
	
		// 没用到现在不判断-cfj
		//bool32 bValid = pGrid->IsValid(FocusCell);		

        CPen PenGridLine;

		if ( NULL != pIoViewParent)
		{
			PenGridLine.CreatePen(PS_SOLID, 1, pIoViewParent->GetIoViewColor(ESCGridLine));
		}
		else
		{
			PenGridLine.CreatePen(PS_SOLID, 1, CFaceScheme::Instance()->GetSysColor(ESCGridLine));
		}

		if ( GVL_NONE != GetGrid()->GetGridLines() )
		{
 			CPen *pOldPen = pDC->GetCurrentPen();
			CRect rectDraw = rect;

			if ( pGrid->IsDrawFixedCellGridLineAsNormal() )	// 按照正常表格线绘制
			{
				rectDraw.InflateRect(1, 1, 0, 0);		// 需要适当扩充
				if ( rectDraw.left < rectGrid.left )
				{
					rectDraw.left = rectGrid.left;
				}
				if ( rectDraw.top < rectGrid.top )
				{
					rectDraw.top = rectGrid.top;
				}
			}
		
			pDC->SelectObject(&PenGridLine);
 			pDC->MoveTo(rectDraw.left, rectDraw.top);
 			pDC->LineTo(rectDraw.left, rectDraw.bottom);
 			pDC->LineTo(rectDraw.right, rectDraw.bottom);
 			pDC->LineTo(rectDraw.right, rectDraw.top);
 			pDC->LineTo(rectDraw.left, rectDraw.top);
 			
 			pDC->SelectObject(pOldPen);

			rect = rectDraw;	// rect需要变更
		}
				
		rect.DeflateRect(1,1);
    }

    // Draw Text and image
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    if (!pDC->m_bPrinting)
#endif
    {
        CFont *pFont = GetFontObject();
		ASSERT(pFont);
        if (pFont)
            pDC->SelectObject(pFont);
    }
	
    //rect.DeflateRect(GetMargin(), 0); - changed by Yogurt
    rect.DeflateRect(GetMargin(), GetMargin());    
    rect.right++;    
    rect.bottom++;
	
	// 绘制图片
	CRect RectText = rect;
	CRect RectImage= rect;
	
	GetTextRect(RectText);
	RectImage.left = RectText.right;
	
    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();
			
            /*
            // removed by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;
            if( nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
			&& nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin())  )
            {
			pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            */
            // Added by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top;            
			//            if ((nImageWidth + RectImage.left <= RectImage.right) && (nImageHeight + RectImage.top <= RectImage.bottom))                
			



			CPoint PtTopLeft;			
			DWORD dFormat = GetFormat();
			bool32 bNotLeft = (dFormat&DT_RIGHT)|(dFormat&DT_CENTER);
			
			if ( !bNotLeft )
			{			
				RectText.left  += (nImageWidth + 1);
				RectText.right += (nImageWidth + 1);
				
				PtTopLeft.x = rect.left + 1;
				PtTopLeft.y = RectImage.top + (RectImage.Height() - nImageHeight) / 2;				
								
			}
			else
			{
				PtTopLeft.x = RectImage.left + (RectImage.Width() - nImageWidth) / 2;			
				PtTopLeft.y = RectImage.top + (RectImage.Height() - nImageHeight) / 2;				
			}
			
			pGrid->GetImageList()->Draw(pDC, GetImage(), PtTopLeft, ILD_NORMAL);
			
            //rect.left += nImageWidth+GetMargin();
        }
    }
	
    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0/* && pGrid->BeDrawSortArrow()*/)
    {
		// 文字的尺寸
		//CSize size = pDC->GetTextExtent(GetText());
		CSize size = GetTextExtent(GetText(), pDC);
		
		int32 iArrowWidth = 6;
		CPen  penArrow(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&penArrow);
		CPoint ptUp, ptDown, ptLeft, ptRight;

		// 
 		if ( (pGrid->GetSortAscending()) )
 		{
 			if ( (rect.Width() - size.cx) >= iArrowWidth )
 			{
 				// 地方够大画箭头,贴着字的左边画:
 			
				if ( GetFormat() & DT_CENTER )
				{
					// 中对齐
					ptUp.x = rect.CenterPoint().x - (size.cx / 2) - 2; 
				}
				else if ( GetFormat() & DT_RIGHT )
				{
					// 右对齐
					ptUp.x = rect.right - size.cx - 3;
				}
				else 
				{
					// 左对齐
					ptUp.x = rect.left + size.cx + 3;
				}

 				ptUp.y = rect.top + (rect.Height() -  14) / 2;
 				//
 				ptDown.x = ptUp.x;
 				ptDown.y = rect.bottom ;
 				//
 				ptLeft.x = ptUp.x - 2;
 				ptLeft.y = ptUp.y + 2;
 				//
 				ptRight.x= ptUp.x + 2 +1 ;
 				ptRight.y= ptLeft.y;
 				
 			}
 			else
 			{
 				// 地方不够,贴着表格的左边画:
 				ptUp.x = rect.left + 3;
 				ptUp.y = rect.top + 6;
 				//
 				ptDown.x = ptUp.x;
 				ptDown.y = rect.bottom;
 				//
 				ptLeft.x = rect.left;
 				ptLeft.y = ptUp.y + 2;
 				//
 				ptRight.x= ptUp.x + 2 + 1;
 				ptRight.y= ptLeft.y;
 			}
 			//
			if ( ptDown.y - ptUp.y > 14 ) // 限制不能太长, 顶边对齐
			{
				ptDown.y = ptUp.y + 14;
				ptLeft.y = ptUp.y + 2;
				ptRight.y = ptLeft.y;
			}
 			pDC->MoveTo(ptUp);
 			pDC->LineTo(ptDown);
 			
			CPoint ptUp1,ptDown1;
			ptUp1.x = ptUp.x + 1;
			ptUp1.y = ptUp.y;
			ptDown1.x = ptDown.x + 1;
			ptDown1.y = ptDown.y;

			pDC->MoveTo(ptUp1);
 			pDC->LineTo(ptDown1);

 			pDC->MoveTo(ptUp);
 			pDC->LineTo(ptLeft);
 			pDC->LineTo(ptRight);
 			pDC->LineTo(ptUp1);
			pDC->LineTo(ptUp);
 		}
 		else
 		{
 			if ( (rect.Width() - size.cx) >= iArrowWidth )
 			{
 				// 地方够大画箭头,贴着字的左边画:
 				
				if ( GetFormat() & DT_CENTER )
				{
					// 中对齐
					ptUp.x = rect.CenterPoint().x - (size.cx / 2) - 2; 
				}
				else if ( GetFormat() & DT_RIGHT )
				{
					// 右对齐
					ptUp.x = rect.right - size.cx - 3;
				}
				else 
				{
					// 左对齐
					ptUp.x = rect.left + size.cx + 3;
				}

 				ptUp.y = rect.top + (rect.Height() -  14) / 2;
 				//
 				ptDown.x = ptUp.x;
 				ptDown.y = rect.bottom - 1;
 				//
 				ptLeft.x = ptUp.x - 2;
 				ptLeft.y = ptDown.y - 2;
 				//
 				ptRight.x= ptUp.x + 2 + 1 ;
 				ptRight.y= ptLeft.y;				
 			}
 			else
 			{
 				// 地方不够,贴着表格的左边画:
 				ptUp.x = rect.left + 3;
 				ptUp.y = rect.top + 6;
 				//
 				ptDown.x = ptUp.x;
 				ptDown.y = rect.bottom - 1;
 				//
 				ptLeft.x = ptUp.x - 2;
 				ptLeft.y = ptDown.y - 2;
 				//
 				ptRight.x= ptUp.x + 2 + 1;
 				ptRight.y= ptLeft.y;
 			}
 			//
			if ( ptDown.y - ptUp.y > 14 ) // 限制不能太长, 底边对齐
			{
				ptDown.y = ptUp.y + 14;
				ptLeft.y = ptDown.y - 2;
				ptRight.y = ptLeft.y;
			}
 			pDC->MoveTo(ptUp);
 			pDC->LineTo(ptDown);
 			
			CPoint ptUp1,ptDown1;
			ptUp1.x = ptUp.x + 1;
			ptUp1.y = ptUp.y;
			ptDown1.x = ptDown.x + 1;
			ptDown1.y = ptDown.y;

			pDC->MoveTo(ptUp1);
 			pDC->LineTo(ptDown1);

 			pDC->MoveTo(ptDown);
 			pDC->LineTo(ptLeft);
 			pDC->LineTo(ptRight);
 			pDC->LineTo(ptDown1);
			pDC->LineTo(ptDown);
 		}

 		pDC->SelectObject(pOldPen);
 		penArrow.DeleteObject();
    }

    // We want to see '&' characters so use DT_NOPREFIX
    RectText.right++;    
    RectText.bottom++;
		
	pDC->SetTextColor(TextClr);
	pDC->SetBkMode(TRANSPARENT);

	// 预留标记位置, 标记总是画在右边
	CString StrText = GetText();
	CSize sizeMark = GetDrawMarkTextExtent(pDC);
	CSize sizeText = GetTextExtent(StrText, pDC);
	CRect RectDrawText(RectText);
	RectDrawText.DeflateRect(m_rtTextPadding);
	RectDrawText.right -= sizeMark.cx;
	if ( RectDrawText.right < RectDrawText.left
		|| ((!(GetFormat()&DT_RIGHT)&&!(GetFormat()&DT_CENTER))&&RectDrawText.Width()<sizeText.cx) )
	{
		RectDrawText.right = RectText.right;	// 表格太小，不画标记
	}
    //DrawText(pDC->m_hDC, GetText(), -1, RectText, GetFormat() | DT_NOPREFIX);
	// 对于短线选股●太小的特殊处理
	if ( StrText == _T("●") )
	{
		CString StrPeText = _T("○") + StrText;
		CSize sizePeText = GetTextExtent(StrPeText, pDC);
		CRect rcT(RectDrawText);
		if ( rcT.Width() > sizePeText.cx/2 )
		{
			rcT.left = rcT.right - sizePeText.cx/2;
		}
		DrawText(pDC->m_hDC, StrPeText, -1, rcT, GetFormat() | DT_NOPREFIX);
	}
	else
	{
		DrawText(pDC->m_hDC, GetText(), -1, RectDrawText, GetFormat() | DT_NOPREFIX);
	}

	const GV_DRAWMARK_ITEM &markItem = GetDrawMarkItem();
	if ( EDMF_None != markItem.eDrawMarkFlag )
	{
		// 绘制标记文字, 仅考虑水平方向
		CRect rcMarkItem(RectText);
		rcMarkItem.InflateRect(0, GetMargin());
		DWORD nFormat = GetFormat();
		COLORREF clrMark = markItem.clrMark;
		DWORD nMarkFmt = DT_SINGLELINE |DT_LEFT;
		if ( DT_CENTER & nFormat )	// 居中, 左对齐画在文字右边
		{
			rcMarkItem.left = RectDrawText.right - (RectDrawText.Width()-sizeText.cx)/2;
		}
		else if ( DT_RIGHT & nFormat ) // 靠右, 右对齐画在文字右边
		{
			rcMarkItem.left = rcMarkItem.right - sizeMark.cx;
		}
		else // 靠左， 左对齐画在文字右边
		{
			rcMarkItem.left += sizeText.cx;
		}

		if ( EDMF_Bottom == markItem.eDrawMarkFlag )
		{
			nMarkFmt |= DT_BOTTOM;
		}
		else
		{
			nMarkFmt |= DT_TOP;
		}

		if ( clrMark == CLR_DEFAULT )
		{
			if ( NULL != pIoViewParent )
			{
				clrMark = pIoViewParent->GetIoViewColor(ESCAmount);
			}
			else
			{
				clrMark = CFaceScheme::Instance()->GetSysColor(ESCAmount);
			}
		}
		COLORREF clrOld = pDC->SetTextColor(clrMark);
		CFont *pOldFont2 = NULL;
		CFont FontMark;
		InitDrawMarkFont(FontMark);
		pOldFont2 = pDC->SelectObject(&FontMark);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(markItem.strMark, rcMarkItem, nMarkFmt);
		pDC->SelectObject(pOldFont2);
		pDC->SetTextColor(clrOld);
	}
	
	//
	CPen PenLine;	
	COLORREF clrLine = m_ClrRectCellLineClr;
	
	if ( (COLORREF)-1 == clrLine )
	{
		if ( NULL != pIoViewParent)
		{
			clrLine = pIoViewParent->GetIoViewColor(ESCChartAxisLine);			
		}
		else
		{
			clrLine = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);			
		}
	}
	
	PenLine.CreatePen(PS_SOLID, 1, clrLine);
	CPen * pOldpen = pDC->GetCurrentPen();
	pDC->SelectObject(PenLine);
	
  	if ( CheckFlag(m_uiCellRectFlag, DR_TOP_DOT) )
  	{
  		// 上虚线
  		DrawDotLine(pDC, CPoint(rect.left - 3, rect.top - 1), CPoint(rect.right + 3, rect.top - 1), clrLine);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_TOP_SOILD) )
  	{
  		// 上实线	
  		pDC->MoveTo(rect.left - 3, rect.top - 1);
  		pDC->LineTo(rect.right + 3, rect.top - 1);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_DOT) )
  	{
  		// 下虚线
  		DrawDotLine(pDC, CPoint(rect.left - 2, rect.bottom + 1), CPoint(rect.right + 2, rect.bottom + 1), clrLine);
  	}
	
	if ( CheckFlag(m_uiCellRectFlag, DR_BOTTOM_SOILD) )
	{
		// 下实线
		pDC->MoveTo(rect.left - 3, rect.bottom + 1);
		pDC->LineTo(rect.right + 3, rect.bottom + 1);
	}
	
  	if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_DOT) )
  	{
  		// 左虚线
  		DrawDotLine(pDC, CPoint(rect.left - 1, rect.top - 2), CPoint(rect.left-1, rect.bottom + 2), clrLine);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_LEFT_SOILD) )
  	{
  		// 左实线
  		pDC->MoveTo(rect.left - 1, rect.top - 2);
  		pDC->LineTo(rect.left - 1, rect.bottom + 2);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_DOT) )
  	{
  		// 右虚线
  		DrawDotLine(pDC, CPoint(rect.right + 1, rect.top - 2), CPoint(rect.right + 1, rect.bottom + 2), clrLine);
  	}
  	
  	if ( CheckFlag(m_uiCellRectFlag, DR_RIGHT_SOILD) )
  	{
  		// 右实线
  		pDC->MoveTo(rect.right + 1, rect.top - 3);
  		pDC->LineTo(rect.right + 1, rect.bottom + 2);
  	}
	
	pDC->SelectObject(pOldpen);	
	PenLine.DeleteObject();

	// 激活
  	CPoint ptTopLeft(rect.left + m_iXposParentActive, rect.top + m_iYposParentActive);
	CPoint ptBottomRight(ptTopLeft.x + 2, ptTopLeft.y + 2);	
	
	CRect rectActive(ptTopLeft, ptBottomRight);

	if ( m_bParentActive )
 	{
 		pDC->FillSolidRect(rectActive, m_ClrParentActive);
 	}
	
	//	如果父窗口启用多边形四角，并且没有列表头，则绘制第一行数据头尾两列左右两侧多边形角
	DrawVisbleFirstRowPolygonConner(pDC, rect, nRow, nCol);

    pDC->RestoreDC(nSavedDC);   
	return TRUE;
}
void	CGridCellSys::DrawVisbleFirstRowPolygonConner(CDC* pDC, CRect rect, int nRow, int nCol)
{
	if(NULL == pDC)
	{
		return;
	}
	//	如果父窗口启用多边形四角，并且没有列表头，则绘制第一行数据头尾两列左右两侧多边形角
	if (NULL != m_pGrid && m_pGrid->HavePolygonConer())
	{

		if (m_pGrid->GetFixedRowCount() == 0)
		{
			//	左上角行列号
			CCellID cellID = m_pGrid->GetTopleftNonFixedCell();			
			if(cellID.IsValid())
			{
				if (nRow == cellID.row)
				{
					//	绘制第一列
					if(0 == nCol)
					{
						//	覆盖列表左上角
						rect.left = 0;
						rect.top = 0;
						DrawPolygonBorder(pDC, rect, true);				
					}
					//	绘制最后一列
					if (m_pGrid->GetColumnCount() - 1== nCol)
					{
						//	覆盖列表右上角角
						CRect rt;
						m_pGrid->GetClientRect(&rt);
						rect.top = 0;
						rect.right = rt.right;
						DrawPolygonBorder(pDC, rect, false);
					}
				}
			}	
		}
	}
}

LPCTSTR CGridCellSys::GetTipText() const
{
	return m_StrTip;
}

LPCTSTR CGridCellSys::GetTipTitle() const
{
	return m_StrTipTitle;
}

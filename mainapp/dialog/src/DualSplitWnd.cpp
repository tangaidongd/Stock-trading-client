/*********************************************************
* Splitter Window Extension
* Version: 1.3
* Date: March 6, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* Copyright (C) 2002-03 by Michal Mecinski
*********************************************************/

#include "stdafx.h"
//#include <afximpl.h>
#include "memdc.h"

#include "DualSplitWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CX_BORDER   1
#define CY_BORDER   1


IMPLEMENT_DYNAMIC(CDualSplitWnd, CSplitterWnd);

// 与该类相关的xml配置中相关信息
static const char * KStrElementValue				= "DualSplitWnd";	
static const char * KStrElementAttrIsH				= "isH";				
static const char * KStrElementAttrScale			= "Scale";                  

CDualSplitWnd::CDualSplitWnd()
{
	m_cxSplitter	 = 4;    // must >=4 ,拖动时拖动条的宽度
	m_cySplitter	 = 4;   
	m_cxBorderShare  = 0;    // 按下鼠标时拖动条的偏移量
	m_cyBorderShare  = 0; 
	m_cxSplitterGap  = 5;    // splitter拖动条的宽度   
	m_cySplitterGap  = 5;  

	m_nPrev			 = -1;
	m_bChange		 = FALSE;
	m_dRatio		 = 0.5;
	m_dF9Ratio		 = 0.0;
	m_eDelChildSplit = EDCNONE;
	m_bChangeingSize = FALSE;
	
	m_rectToDel = CRect(0,0,0,0);
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bDragFull, 0);
	// fangz0906# for test
	if (!m_bDragFull)
	{
		m_bDragFull =! m_bDragFull;
	}	

	m_nTrackPos = 0;
}

CDualSplitWnd::~CDualSplitWnd()
{
	
}


BEGIN_MESSAGE_MAP(CDualSplitWnd, CSplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	// ON_WM_PAINT()
END_MESSAGE_MAP()


void CDualSplitWnd::OnInvertTracker(const CRect& rect)
{
// 	if (!m_bDragFull)
// 		CSplitterWnd::OnInvertTracker(rect);
	
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty()); 
	ASSERT((GetStyle()&WS_CLIPCHILDREN)==0);
	CRect rc=rect; 
	rc.InflateRect(2,2);
	CDC* pDC=GetDC(); 
	CBrush* pBrush=CDC::GetHalftoneBrush();
	HBRUSH hOldBrush=NULL;
	if(pBrush!=NULL) hOldBrush=(HBRUSH)SelectObject(pDC->m_hDC,pBrush->m_hObject);
	pDC->PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),BLACKNESS); 
	
	if(hOldBrush!=NULL) 
		SelectObject(pDC->m_hDC,hOldBrush);
	ReleaseDC(pDC); 	
}

void CDualSplitWnd::SetSplitCursor(int ht)
{
	if (m_bDragFull)	// use system cursors
	{
		if (ht >= vSplitterBar1 && ht <= vSplitterBar15)
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		else if (ht >= hSplitterBar1 && ht <= hSplitterBar15)
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		else if (ht >= splitterIntersection1 && ht <= splitterIntersection225)
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		else
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else
 		CSplitterWnd::SetSplitCursor(ht);
}

/*
void CDualSplitWnd::OnPaint()
{
	if ( m_nMaxRows < 1 || m_nMaxCols < 1 || m_nRows < 1 || m_nCols < 1)
	{
		return;
	}
	
	CSplitterWnd::OnPaint();
}
*/

void CDualSplitWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// only static splitter supported
	ASSERT(!(GetStyle() & SPLS_DYNAMIC_SPLIT));

	if (!m_bDragFull)	// normal dragging
	{
		CSplitterWnd::OnMouseMove(nFlags, point);
		if (m_bTracking)
			m_bChange = TRUE;
		return;
	}

	if (m_bTracking && GetCapture() != this)
		StopTracking(FALSE);
	
	if (m_bTracking)
	{
		if (point.y < m_rectLimit.top)
			point.y = m_rectLimit.top;
		else if (point.y > m_rectLimit.bottom)
			point.y = m_rectLimit.bottom;
		if (point.x < m_rectLimit.left)
			point.x = m_rectLimit.left;
		else if (point.x > m_rectLimit.right)
			point.x = m_rectLimit.right;
		
		if (m_htTrack == vSplitterBar1)
		{
			if (m_pRowInfo[0].nCurSize != point.y - m_nTrackPos)
			{
				m_pRowInfo[0].nIdealSize = point.y - m_nTrackPos;
				RecalcLayout();
				RedrawWindow();
				m_bChange = TRUE;
			}
			if (m_pRowInfo[0].nCurSize < VALIDAREASIZE ) 
			{
				// 上切分窗口尺寸过小,准备删除
				CRect rect = m_rectLimit;
				rect.bottom = rect.top + m_pRowInfo[0].nCurSize;
				
				CClientDC dc1(this);					
				CMemDC dc(&dc1,&rect);
				dc.FillSolidRect(&rect,RGB(0,0,255));
				
				m_rectToDel = rect;
				m_eDelChildSplit = EDCTOP;
			}
			else if ( m_pRowInfo[1].nCurSize < VALIDAREASIZE )
			{
				// 下切分窗口尺寸过小,准备删除
				CRect rect = m_rectLimit;
				rect.top = rect.bottom - m_pRowInfo[1].nCurSize;
				
				CClientDC dc1(this);
				//CMemDC dc(&dc1,&rect);
				dc1.FillSolidRect(&rect,RGB(0,0,255));

				m_rectToDel = rect;
				m_eDelChildSplit = EDCBOTTOM;

			}
			else
			{
				m_eDelChildSplit = EDCNONE;
				m_rectToDel = CRect(0,0,0,0);
			}
		}
		else if (m_htTrack == hSplitterBar1)
		{
			if (m_pColInfo[0].nIdealSize != point.x - m_nTrackPos)
			{
				m_pColInfo[0].nIdealSize = point.x - m_nTrackPos;
				RecalcLayout();
				RedrawWindow();
				m_bChange = TRUE;
			}
			if (m_pColInfo[0].nCurSize < VALIDAREASIZE ) 
			{
				// 左切分窗口尺寸过小,准备删除
				CRect rect = m_rectLimit;
				rect.right = rect.left + m_pColInfo[0].nCurSize;					

				CClientDC dc1(this);
				CMemDC dc(&dc1,&rect);
				dc.FillSolidRect(&rect,RGB(0,0,255));				

				m_rectToDel = rect;
				m_eDelChildSplit = EDCLEFT;

			}
			else if ( m_pColInfo[1].nCurSize < VALIDAREASIZE )
			{
				// 右切分窗口尺寸过小,准备删除
				
				CRect rect = m_rectLimit;
				rect.left = rect.right - m_pColInfo[1].nCurSize;

				CClientDC dc1(this);
				//CMemDC dc(&dc1,&rect);
				dc1.FillSolidRect(&rect,RGB(0,0,255));

				m_rectToDel = rect;
				m_eDelChildSplit = EDCRIGHT;
			}
			else
			{
				m_eDelChildSplit = EDCNONE;
				m_rectToDel = CRect(0,0,0,0);
			}
		}
	}
	else
	{
		int ht = HitTest(point);
		SetSplitCursor(ht);
	}
}

int CDualSplitWnd::HitTest(CPoint pt) const
{
	ASSERT_VALID(this);

	CRect RectWindow;
	GetWindowRect(&RectWindow);

	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);

	CRect rectInside;
	GetInsideRect(rectInside);

	if (m_bHasVScroll && m_nRows < m_nMaxRows &&
		CRect(rectInside.right, rectClient.top, rectClient.right,
		rectClient.top + m_cySplitter - 0/*afxData.bWin4*/).PtInRect(pt))
	{
		return vSplitterBox;
	}

	if (m_bHasHScroll && m_nCols < m_nMaxCols &&
		CRect(rectClient.left, rectInside.bottom,
		 rectClient.left + m_cxSplitter - 0/*afxData.bWin4*/,
		 rectClient.bottom).PtInRect(pt))
	{
		return hSplitterBox;
	}

	// for hit detect, include the border of splitters
	CRect rect;
	rect = rectClient;
	int col = 0;
	for (col = 0; col < m_nCols - 1; col++)
	{
		rect.left += m_pColInfo[col].nCurSize;
		rect.right = rect.left + m_cxSplitterGap;
		if (rect.PtInRect(pt))
			break;
		rect.left = rect.right;
	}

	rect = rectClient;
	int row = 0;
	for (row = 0; row < m_nRows - 1; row++)
	{
		rect.top += m_pRowInfo[row].nCurSize;
		rect.bottom = rect.top + m_cySplitterGap;
		if (rect.PtInRect(pt))
			break;
		rect.top = rect.bottom;
	}

	// row and col set for hit splitter (if not hit will be past end)
	if (col != m_nCols - 1)
	{
		if (row != m_nRows - 1)
			return splitterIntersection1 + row * 15 + col;
		return hSplitterBar1 + col;
	}

	if (row != m_nRows - 1)
		return vSplitterBar1 + row;

	return noHit;
}

void CDualSplitWnd::RecalcRatio()
{
	CRect RectClient;
	GetClientRect(RectClient);
	
	int32 cx = RectClient.Width();
	int32 cy = RectClient.Height();

	if (m_nCols==2 && m_nRows==1)	// vertical resize
	{
		int cxCur, cxMin;
		GetColumnInfo(0, cxCur, cxMin);

		if (m_bChange && m_nPrev>0)
		{
			m_dRatio = (double)cxCur / (double)m_nPrev;	// calc new ratio
			m_bChange = FALSE;
		}
	
		m_nPrev = cx;
	}
	else if (m_nCols==1 && m_nRows==2)	// horizontal resize
	{
		int cyCur, cyMin;
		GetRowInfo(0, cyCur, cyMin);

		if (m_bChange && m_nPrev>0)
		{
			m_dRatio = (double)cyCur / (double)m_nPrev;
			m_bChange = FALSE;
		}
	
		m_nPrev = cy;
	}
}

void CDualSplitWnd::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_pColInfo)
		return;

	// 重新计算比率
	RecalcRatio();

	// 调整布局
	if (m_nCols==2 && m_nRows==1)	// vertical resize
	{
		int cxMin = 0;
		SetColumnInfo(0, (int)(cx * m_dRatio), cxMin);
	}
	else if (m_nCols==1 && m_nRows==2)	// horizontal resize
	{
		int cyMin = 0;
		SetRowInfo(0, (int)(cy * m_dRatio), cyMin);
	}
	RecalcLayout();

	//
	CSplitterWnd::OnSize(nType, cx, cy);
}

void CDualSplitWnd::SetRadio(double dradio,bool32 bF9 /*= false*/)
{
	if (!bF9)
	{
		if (dradio <= 0.0 || dradio >= 1)
		{
			dradio = 0.5;
		}

		m_dRatio = dradio;
	}
	else
	{
		if ( 0.0 == m_dF9Ratio )
		{
			m_dF9Ratio = m_dRatio;
			m_dRatio   = dradio;
		}
		else
		{
			m_dRatio   = m_dF9Ratio;
			m_dF9Ratio = 0.0;
		}
	}

	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	int cy = rect.Height();
	
	if (m_nCols==2 && m_nRows==1)	// vertical resize
	{
		int cxMin = 0;
		SetColumnInfo(0, (int)(cx * m_dRatio), cxMin);
	}
	else if (m_nCols==1 && m_nRows==2)	// horizontal resize
	{
		int cyMin = 0;
		SetRowInfo(0, (int)(cy * m_dRatio), cyMin);
	}

	RecalcLayout();			
}

double CDualSplitWnd::GetF9Ratio(CWnd * pWnd) const
{
	double dRatio = 0.0;
	if (NULL == pWnd)
	{
		return dRatio;
	}
	CWnd * pWnd1 = NULL;
	CWnd * pWnd2 = NULL;
	if ( 2 == m_nCols && 1 == m_nRows)
	{
		pWnd1 = GetPane(0,0);
		pWnd2 = GetPane(0,1);
	}
	else if ( 1 == m_nCols && 2 == m_nRows)
	{
		pWnd1 = GetPane(0,0);
		pWnd2 = GetPane(1,0);		
	}
	else
	{
		return dRatio;
	}
	
	
	if ( pWnd == pWnd1 )
	{
		return KdF9Radion;
	}
	else if ( pWnd == pWnd2)
	{
		return ( double(1.0) - KdF9Radion);
	}
	else
	{
		return dRatio;
	}
}

void CDualSplitWnd::StartTracking(int ht)
{
	
	if (!m_bDragFull)
	{
		CSplitterWnd::StartTracking(ht);
		return;
	}
	
	m_dF9Ratio = 0.0;

	GetInsideRect(m_rectLimit);

	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	if (ht >= vSplitterBar1 && ht <= vSplitterBar15)
		m_nTrackPos = ptCursor.y - m_pRowInfo[0].nCurSize;
	else if (ht >= hSplitterBar1 && ht <= hSplitterBar15)
		m_nTrackPos = ptCursor.x - m_pColInfo[0].nCurSize;

	SetCapture();

	m_bTracking = TRUE;
	m_bChangeingSize = true;
	m_htTrack = ht;
	SetSplitCursor(ht);
}

void CDualSplitWnd::StopTracking(BOOL bAccept)
{
	if (!m_bDragFull)
		CSplitterWnd::StopTracking(bAccept);
	else
	{
		ReleaseCapture();
		m_bTracking = FALSE;
		m_bChangeingSize = false;
	}

	RecalcRatio();

	// 判断此时子窗口的大小,如果小于VALIDAREASIZE 就删除这个子窗口
	if (m_eDelChildSplit == EDCNONE)
		return;
	DelChildSplitWnd(m_eDelChildSplit);
	m_eDelChildSplit = EDCNONE;
}

void CDualSplitWnd::DelAllChildGGTongView(CDualSplitWnd * pDualSplitWnd)
{
	if (NULL == pDualSplitWnd)
		return;
	CWnd * pWnd1 = NULL;
	CWnd * pWnd2 = NULL;

	int32 iRowCounts = pDualSplitWnd->GetRowCount();
	int32 iColCounts = pDualSplitWnd->GetColumnCount();
	
	ASSERT(( iRowCounts == 1 && iColCounts == 2 )||( iRowCounts == 2 && iColCounts == 1 ));
	
	pWnd1 = pDualSplitWnd->GetPane(0,0);
	pWnd2 = iRowCounts == 2 ? pDualSplitWnd->GetPane(1,0) : pDualSplitWnd->GetPane(0,1);

	ASSERT( NULL!= pWnd1 && NULL!= pWnd2 );

	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());

	if (pWnd1->IsKindOf(RUNTIME_CLASS(CDualSplitWnd)))
	{
		DelAllChildGGTongView((CDualSplitWnd*)pWnd1);
	}
	else if (pWnd1->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		pParentFrame->DelSplit((CView *)pWnd1);
	}

	if (pWnd2->IsKindOf(RUNTIME_CLASS(CDualSplitWnd)))
	{
		DelAllChildGGTongView((CDualSplitWnd*)pWnd2);
	}
	else if (pWnd2->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		pParentFrame->DelSplit((CView *)pWnd2);
	}
}

void CDualSplitWnd::DelChildSplitWnd(E_DelChildsplit eDelChildsplit)
{
	if ( eDelChildsplit <= EDCNONE || eDelChildsplit > EDCCOUNTS)
		return;
	CWnd * pWndToDel = NULL;
	CWnd * pWndLeave = NULL;

	switch( eDelChildsplit )
	{
	case EDCLEFT:
		{
			pWndToDel = GetPane(0,0);				
			pWndLeave = GetPane(0,1);
		}
		break;
	case EDCRIGHT:
		{
			pWndToDel = GetPane(0,1);
			pWndLeave = GetPane(0,0);
		}
		break;
	case EDCTOP:
		{
			pWndToDel = GetPane(0,0);
			pWndLeave = GetPane(1,0);			
		}
		break;
	case EDCBOTTOM:
		{
			pWndToDel = GetPane(1,0);
			pWndLeave = GetPane(0,0);
		}		
		break;
	default:
		break;
	}
	
	ASSERT(NULL != pWndToDel && NULL != pWndLeave);

	if (pWndToDel->IsKindOf(RUNTIME_CLASS(CDualSplitWnd)))
	{
		// 遍历删除pWndToDel 下面的GGTongView;
		DelAllChildGGTongView((CDualSplitWnd *)pWndToDel);
	}
	else if ( pWndToDel->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		pParentFrame->DelSplit((CView *)pWndToDel);
	}
}

void CDualSplitWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{
	if(pDC==NULL) 
	{ 
		RedrawWindow(rect,NULL,RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	} 
	ASSERT_VALID(pDC);
	CRect rc=rect;

// 	if ( m_eDelChildSplit != EDCNONE )
// 	{
// 		pDC->FillSolidRect(&m_rectToDel,RGB(0,0,255));
// 	}	

	// ...fangz0411
	// CMPIChildFrame * pParentFrame = (CMPIChildFrame *)GetParentFrame();
	// COLORREF clr =  pParentFrame->GetFrameColor(ESCChartAxisLine);
	
	COLORREF clr =  CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);

	switch(nType) 
	{ 
	case splitBorder:
		//重画分割窗口边界,使之为红色 
		pDC->Draw3dRect(rc,clr,clr);
		rc.InflateRect(-CX_BORDER,-CY_BORDER); 
		pDC->Draw3dRect(rc,clr,clr); 
		
		break; 
 	case splitBox:
 		pDC->Draw3dRect(rc,RGB(0,0,0),RGB(0,0,0));
 		rc.InflateRect(-CX_BORDER,-CY_BORDER); 
 		pDC->Draw3dRect(rc,RGB(0,0,0),RGB(0,0,0));
 		rc.InflateRect(-CX_BORDER,-CY_BORDER);
 		pDC->FillSolidRect(rc,RGB(0,0,0)); 
 		pDC->Draw3dRect(rc,RGB(0,0,0),RGB(0,0,0));
		break; 
	case splitBar: 
		//重画分割条，使之为黑色 
		pDC->FillSolidRect(rc,RGB(0,0,0));
		rc.InflateRect(-5,-5); 
		pDC->Draw3dRect(rc,RGB(0,0,0),RGB(0,0,0)); 
		
		break; 
	default: 
		ASSERT(FALSE); 
	} 
	
}

CString CDualSplitWnd::ToXml(CGGTongView * pActiveView)
{
	ASSERT(NULL != pActiveView);
	CWnd * pWnd = GetPane(0,0);
	CWnd * pWndanother = NULL;

	CString StrThis;

	// 得到dualsplitwnd 的属性; ish , scale;
	CString  StrElmtAttrIsH,StrElmtAttrScale;
	if (GetColumnCount() == 2)
	{
		StrElmtAttrIsH = L"0";
	}
	else
	{
		StrElmtAttrIsH = L"1";
	}

	// 重新计算比率
	RecalcRatio();
	StrElmtAttrScale.Format(L"%d", (int32)(m_dRatio * 100));
	
	//   L"<%s     %s=\"%s\" %s=\"%s\" %s=\"%s\"  %s=\"%s\" >"

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\"   >  \n", 
					 CString(GetXmlElementValue()).GetBuffer(),
					 CString(GetXmlElmAttrDualSplitIsH()).GetBuffer(),
					 StrElmtAttrIsH.GetBuffer(),
					 CString(GetXmlElmAttrDualSplitScale()).GetBuffer(),
					 StrElmtAttrScale.GetBuffer());


	//
	int iCountColumn = GetColumnCount();   
	int iCountRow    = GetRowCount();         

	if ( iCountColumn == 2 )
	{
		pWndanother = GetPane(0,1);
	}
	else if ( iCountRow == 2)
	{
		pWndanother = GetPane(1,0);
	}

	if (pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		StrThis += ((CGGTongView*)pWnd)->ToXml(pActiveView);
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS(CDualSplitWnd)))
	{
		StrThis +=  ((CDualSplitWnd*)pWnd)->ToXml(pActiveView);
	}
	
	if (pWndanother != NULL)
	{
		if (pWndanother->IsKindOf(RUNTIME_CLASS(CGGTongView)))
		{
			StrThis += ((CGGTongView*)pWndanother)->ToXml(pActiveView);			
		}

		else if (pWndanother->IsKindOf(RUNTIME_CLASS(CDualSplitWnd)))
		{
			StrThis += ((CDualSplitWnd*)pWndanother)->ToXml(pActiveView);
		}
	}
	
	// 
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	//
	return StrThis;
}

bool32 CDualSplitWnd::FromXml(TiXmlElement *pTiXmlElement)
{
	return TRUE;
}

const char * CDualSplitWnd::GetXmlElementValue()
{
	return KStrElementValue;
}

const char * CDualSplitWnd::GetXmlElmAttrDualSplitIsH()
{
	return KStrElementAttrIsH; 
}

const char * CDualSplitWnd::GetXmlElmAttrDualSplitScale()
{
	return KStrElementAttrScale; 
}

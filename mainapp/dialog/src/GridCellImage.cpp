#include "stdafx.h"
#include "GridCellSys.h"
#include "IoViewManager.h"
#include "GridCtrl.h"
#include "facescheme.h"

#include "MPIChildFrame.h"
#include "GridCellImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellImage, CGridCell)

#define INVALID_ID -1

CGridCellImage::CGridCellImage()
:CGridCell()
{
	m_eCellStyle = ECSImage;
	m_iBtnHovering = INVALID_ID;
	m_pImgBtn = NULL;
	m_uImgCnt = 1;
	m_btnRect = (0,0,0,0);
}

CGridCellImage::~CGridCellImage()
{
	DEL(m_pImgBtn);
}

BOOL CGridCellImage::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
	CGridCtrl* pGrid = GetGrid();
	if (!pGrid || !pDC)
	{
		return FALSE;
	}

	if( rect.Width() <= 0 || rect.Height() <= 0)
	{
		return FALSE;   
	}

	CIoViewBase * pIoViewParent = NULL; 
	CWnd * pParent = GetGrid()->GetParent();
	pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);

	COLORREF cellSelClr;
	CPen PenGridLine;
	if ( NULL != pIoViewParent)
	{
		PenGridLine.CreatePen(PS_SOLID, 1, pIoViewParent->GetIoViewColor(ESCGridLine));
		cellSelClr = pIoViewParent->GetIoViewColor(ESCGridSelected);
	}
	else
	{
		PenGridLine.CreatePen(PS_SOLID, 1, CFaceScheme::Instance()->GetSysColor(ESCGridLine));
		cellSelClr = CFaceScheme::Instance()->GetSysColor(ESCGridSelected);
	}

	if(pGrid->GetGridLines() != GVL_NONE)
	{
		CPen *pOldPen = pDC->GetCurrentPen();

		pDC->SelectObject(&PenGridLine);
		pDC->MoveTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.right, rect.top);
		pDC->LineTo(rect.left, rect.top);

		pDC->SelectObject(pOldPen);
	}
	else
	{
		rect.right++;
		rect.bottom++;
	}

	if ((GetState() & GVIS_SELECTED))
	{
		pDC->FillSolidRect(rect, cellSelClr);
	}
	else
	{
		pDC->FillSolidRect(rect, GetGrid()->GetBkColor());
	}

	CRect rectGrid;
	pGrid->GetClientRect(&rectGrid);
	if ( m_pGrid->IsAutoHideFragmentaryCell() && (rect.left < rectGrid.left || rect.right > rectGrid.right || rect.top < rectGrid.top || rect.bottom > rectGrid.bottom) )
	{	
		return TRUE;
	}

	DrawButton(pDC, nRow, nCol, rect, bEraseBkgnd);

	return TRUE;
}

void CGridCellImage::DrawButton(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd )
{
	if (NULL != pDC && NULL != m_pGrid && bEraseBkgnd)
	{	
		Gdiplus::Graphics graphics(pDC->GetSafeHdc());

		int32 iX=0, iY=0, iWidth=0, iHeight=0;
		if (m_pImgBtn != NULL)
		{
			int32 iXSpace = rect.Width()-m_pImgBtn->GetWidth();
			int32 iYSpace = rect.Height()-m_pImgBtn->GetHeight()/m_uImgCnt;
			if (iXSpace < 0)
			{
				iXSpace = 0;
			}
			if (iYSpace < 0)
			{
				iYSpace = 0;
			}

			iX = rect.left + iXSpace / 2;
			iY = rect.top + iYSpace / 2;

			iWidth = m_pImgBtn->GetWidth();
			iHeight = m_pImgBtn->GetHeight()/m_uImgCnt;
		}

		CRect rtBtn(iX, iY, iX+iWidth, iY+iHeight);
		m_btnRect = rtBtn;
		m_btnCell.SetRect(&rtBtn);

		CPoint pt;
		GetCursorPos(&pt);
		m_pGrid->ScreenToClient(&pt);

		if (rtBtn.PtInRect(pt))
		{
			m_btnCell.MouseHover(false);
		}
		else
		{			
			m_btnCell.MouseLeave(false);
		}

		m_btnCell.DrawButton(&graphics);
	}
}

void CGridCellImage::OnMouseOver()
{
	if ( NULL != m_pGrid &&(ECSAddZiXuan == m_eCellStyle ||
		ECSImage == m_eCellStyle || ESCDelZiXuan == m_eCellStyle))
	{
		if (!m_btnRect.IsRectEmpty())
		{
			m_pGrid->Invalidate(false);
		}
	}
}

void CGridCellImage::SetImage(UINT uImgID, UINT uCnt, CELL_Style eStyle)
{
	m_uImgCnt = uCnt;
	m_eCellStyle = eStyle;

	if ( !ImageFromIDResource(AfxGetResourceHandle(), uImgID, L"PNG", m_pImgBtn))
	{
		ASSERT(0);
		m_pImgBtn = NULL;
	}

	if (!m_btnCell.GetCreate())
	{
		CRect rtBtn(0,0,0,0);	
		m_btnCell.CreateButton(L"", &rtBtn, NULL, m_pImgBtn, m_uImgCnt, 7777);
	}
}

void CGridCellImage::OnClickDown( CPoint PointCellRelative)
{
	if (ECSAddZiXuan == m_eCellStyle)
	{
		m_eCellStyle = ECSImage;

		// 加入到默认自选板块
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_BLOCK_ADDTOFIRST, 0);
	}
	else if (ESCDelZiXuan == m_eCellStyle)
	{
		// 从默认自选板块中删除
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_BLOCK_DELETEFROMFIRST, 0);
	}
	else if (ECSImage == m_eCellStyle)
	{

	}
}
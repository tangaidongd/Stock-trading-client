// staticsetcolor.cpp : implementation file
//

#include "stdafx.h"
#include "staticsetcolor.h"

//#include "dlgsystemface.h"
//#include "DlgBlockSet.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticSetColor

CStaticSetColor::CStaticSetColor()
{
	m_color			 = CLR_DEFAULT;		// 无效默认颜色
	m_bNeedDraw		 = false;
	m_bParentDlgFace = true;
	m_ectColorType = ECT_SingleColor;	// 单一颜色
}
CStaticSetColor::~CStaticSetColor()
{

}
BEGIN_MESSAGE_MAP(CStaticSetColor, CStatic)
	//{{AFX_MSG_MAP(CStaticSetColor)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSetColor message handlers

void CStaticSetColor::OnPaint() 
{
	CPaintDC dc(this);
	
	if (IsWindowEnabled())
	{
		DrawMyStatic(&dc);
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		dc.FillSolidRect(rect, (COLORREF)GetSysColor(COLOR_BTNFACE));
	}
}

void CStaticSetColor::SetColor(COLORREF clr)
{
	if ( clr != m_color )
	{
		DrawMyStatic();
	}
	m_color = clr;
}
void CStaticSetColor::SetText(COLORREF clr)
{	
	m_bNeedDraw = true;
	SetColor(clr);
	// 如果传入的颜色,和色块的颜色相同.则打勾
	if (clr == m_color && clr != CLR_DEFAULT) 
	{	
		SetWindowText(_T("√"));
		DrawMyStatic();
	}
	else
	{
		SetWindowText(_T(""));
	}
// 	else if ( m_color == CLR_DEFAULT )
// 	{
// 		CClientDC dc(this);
// 		CRect rect;
// 		dc.SetTextColor(RGB(0,0,0));
// 		GetClientRect(rect);			
// 		
// 		dc.DrawText(_T("选择颜色"), rect, DT_CENTER |DT_VCENTER |DT_SINGLELINE);
// 	}
}
void CStaticSetColor::SetDefaultText()
{
	m_bNeedDraw =false;
	SetWindowText(_T(""));
	DrawMyStatic();
}
COLORREF CStaticSetColor::GetColor(CPoint pt)
{
	if ( ECT_SingleColor != m_ectColorType || CLR_DEFAULT == m_color )
	{
		// pt 客户区坐标
		HDC   hDC =::GetDC(NULL);
		CRect rect(0,0,pt.x,pt.y);
		ClientToScreen(rect);	
		COLORREF color = ::GetPixel(hDC,rect.BottomRight().x,rect.BottomRight().y);
		CString StrTest;
		StrTest.Format(L"RGB(%u,%u,%u)",GetRValue(color), GetGValue(color), GetBValue(color));
		
		::ReleaseDC(m_hWnd, hDC);
		return color;
	}
	else
	{
		return m_color;
	}
}


void CStaticSetColor::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags,point);

	COLORREF clr =  GetColor(point);

	if (m_bParentDlgFace)
	{
		// 颜色设置对话框中单击的处理
		CWnd * pWnd = this->GetParent();
		if(pWnd)
			pWnd->PostMessage(UM_SETCOLOR,(WPARAM)clr,0);
		// 将当前的选择打上选中标记
		SetText(clr);
	}
	else
	{
		// 用户板块对话框中单击的处理,弹出颜色设置对话框,设置颜色:
		CColorDialog dlgColor;
		
		if (IDOK == dlgColor.DoModal())
		{
			COLORREF color = dlgColor.GetColor();	
			SetColor(color);

			CWnd * pWnd = this->GetParent();			
			if(pWnd)
				pWnd->PostMessage(UM_SETCOLOR,(WPARAM)color,0);
			DrawMyStatic();
		}
	}
}

BOOL CStaticSetColor::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CStaticSetColor::DrawMyStatic( CDC *pDC /*= NULL*/ )
{
	CDC *pDrawDC = pDC;
	if ( pDrawDC == NULL )
	{
		pDrawDC = new CClientDC(this);
	}

	CRect rect;
	GetClientRect(rect);
	
	m_bNeedDraw = false;
	
	if ( CLR_DEFAULT != m_color )
	{
		pDrawDC->FillSolidRect(rect,m_color);	
	}
	else
	{
		pDrawDC->FillSolidRect(rect, (COLORREF)GetSysColor(COLOR_BTNFACE));
	}
	
	// 直接绘制文字
	CString StrText;
	GetWindowText(StrText);
	if ( !StrText.IsEmpty() )
	{
		COLORREF clrReverse = ((~m_color) & (RGB(255,255,255)));
		if ( clrReverse == m_color )
		{
			clrReverse = RGB(0,0,0);
		}
		pDrawDC->SetTextColor(clrReverse);
		pDrawDC->SetBkMode(TRANSPARENT);
		pDrawDC->DrawText(StrText, rect, DT_CENTER | DT_VCENTER |DT_SINGLELINE);
	}

	if ( NULL == pDC )
	{
		delete pDrawDC;
		pDrawDC = NULL;
	}
}

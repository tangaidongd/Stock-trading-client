#include <stdafx.h>
#include "MyStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

COLORREF CMyStatic::t_unvclr=RGB(0,0,255);
COLORREF CMyStatic::t_vdclr=RGB(128,0,128);
HCURSOR  CMyStatic::t_hc=NULL;

IMPLEMENT_DYNAMIC(CMyStatic,CStatic)

BEGIN_MESSAGE_MAP(CMyStatic,CStatic)
  ON_WM_NCHITTEST()
  ON_WM_LBUTTONDOWN()
  ON_WM_CTLCOLOR_REFLECT()
  ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CMyStatic::CMyStatic()
{
	m_color=t_unvclr;
}

void CMyStatic::SetLinkObject(CString str)
{
	m_strlink=str;
}

void CMyStatic::OnLButtonDown(UINT nFlags,CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);	
}

LRESULT CMyStatic::OnNcHitTest(CPoint point)
{
	return HTCLIENT;
}

HBRUSH CMyStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	ASSERT(nCtlColor==CTLCOLOR_STATIC);
	DWORD dwStyle=GetStyle();
	HBRUSH hbr=NULL;
	if((dwStyle & 0xFF)<=SS_RIGHT)  //判断是否为文本
	{
		if(!(HFONT)m_font)
		{
			LOGFONT lf;
			GetFont()->GetObject(sizeof(lf),&lf);
			lf.lfUnderline = TRUE;  //可选择下划线字体
			m_font.CreateFontIndirect(&lf);
		}
		pDC->SelectObject(&m_font);  //应用字体
		pDC->SetTextColor(m_color);  //应用文本前景色
		pDC->SetBkMode(TRANSPARENT); //背景为透明

		hbr=(HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	return hbr;
}

BOOL CMyStatic::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message)
{
	if(t_hc==NULL)
	{
		t_hc=::LoadCursor(NULL,MAKEINTRESOURCE(32649));
	}
	::SetCursor(t_hc);

	return FALSE;
}

void CMyStatic::SetUnderline(BOOL bUnderline )
{
    //m_bUnderline = bUnderline;
	
    if (::IsWindow(GetSafeHwnd()))
    {
        LOGFONT lf;
        GetFont()->GetLogFont(&lf);
        lf.lfUnderline = bUnderline;
		
        m_font.DeleteObject();
        m_font.CreateFontIndirect(&lf);
        SetFont(&m_font);
		
        Invalidate();
    }
}
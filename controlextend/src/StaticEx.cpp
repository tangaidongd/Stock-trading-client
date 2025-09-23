// StaticEx.cpp : 实现文件
//

#include "stdafx.h"
#include "StaticEx.h"
#include "FontFactory.h"

// CStaticEx

IMPLEMENT_DYNAMIC(CStaticEx, CStatic)

CStaticEx::CStaticEx()
{
	m_clrBkColor = RGB(242,240,235);
	m_nAlign = DT_LEFT|DT_VCENTER|DT_SINGLELINE;
	m_clrText1 = RGB(100,100,100);
	m_clrText2 = RGB(100,100,100);
	LOGFONT logFont;
	::ZeroMemory(&logFont,sizeof(logFont));

	logFont.lfHeight = 18;
	logFont.lfWeight = FW_NORMAL;
	CFontFactory fontFactory;
	lstrcpy(logFont.lfFaceName,fontFactory.GetExistFontName(_T("微软雅黑")));
	m_font.CreateFontIndirect(&logFont);
}

CStaticEx::~CStaticEx()
{
	
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CStaticEx 消息处理程序



void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rtClient;
	GetClientRect(rtClient);

	HGDIOBJ hOldFont = NULL;
	if (m_font.m_hObject)
		hOldFont = dc.SelectObject(m_font.m_hObject);

	dc.FillSolidRect(rtClient, m_clrBkColor);

	dc.SetBkMode(TRANSPARENT);

	dc.SetTextColor(m_clrText1);
	dc.DrawText(m_strTitleText1,rtClient,m_nAlign);

	if (!m_strTitleText2.IsEmpty())
	{
		CSize txtSize = dc.GetTextExtent(m_strTitleText1);
		CRect rtText2(txtSize.cx+5,rtClient.top,rtClient.right,rtClient.bottom);
		dc.SetTextColor(m_clrText2);
		dc.DrawText(m_strTitleText2,rtText2,m_nAlign);
	}

	if (hOldFont)
		dc.SelectObject(hOldFont);
}

void CStaticEx::SetAlign( int nAlign )
{
	m_nAlign = nAlign;
}

void CStaticEx::SetTitleColor1( COLORREF clr )
{
	m_clrText1 = clr;
}

COLORREF CStaticEx::GetTitleColor1()
{
	return m_clrText1;
}

void CStaticEx::SetTitleColor2( COLORREF clr )
{
	m_clrText2 = clr;
}

COLORREF CStaticEx::GetTitleColor2()
{
	return m_clrText2;
}

void CStaticEx::SetTitleText1( CString strText )
{
	m_strTitleText1 = strText;
	CRect rtClient;
	GetClientRect(rtClient);
	InvalidateRect(rtClient);
}

void CStaticEx::GetTitleText1(CString &strText)
{
	strText = m_strTitleText1;
}

void CStaticEx::SetTitleText2( CString strText )
{
	m_strTitleText2 = strText;
	CRect rtClient;
	GetClientRect(rtClient);
	InvalidateRect(rtClient);
}

void CStaticEx::GetTitleText2(CString &strText)
{
	strText = m_strTitleText2;
}

void CStaticEx::SetBkColor(COLORREF clrColor)
{
	m_clrBkColor = clrColor;
}
// CheckBox.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckBox.h"
#include "FontFactory.h"
// CCheckBox

IMPLEMENT_DYNAMIC(CCheckBox, CButton)

CCheckBox::CCheckBox()
{
	m_bChecked = FALSE;
	m_clrText = RGB(100,100,100);
	m_clrBkText = RGB(242,240,235);

	LOGFONT logFont;
	::ZeroMemory(&logFont,sizeof(logFont));
	logFont.lfHeight = 18;
	logFont.lfWeight = FW_THIN;
	CFontFactory fontFactor;
	lstrcpy(logFont.lfFaceName,fontFactor.GetExistFontName(_T("微软雅黑")));
	m_font.CreateFontIndirect(&logFont);
	m_nStateCount = 1;
}

CCheckBox::~CCheckBox()
{
}


BEGIN_MESSAGE_MAP(CCheckBox, CButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CCheckBox::SetBkFromFile( const TCHAR *pFilePath, int nStateCount )
{
	ASSERT(pFilePath);
	if (!m_imgBK.IsNull())
		m_imgBK.Detach();
	m_nStateCount = nStateCount;
	m_imgBK.Load(pFilePath);
}

void CCheckBox::SetBkFromResource( int nResourceID, int nStateCount )
{
	if (!m_imgBK.IsNull())
		m_imgBK.Detach();
	m_nStateCount = nStateCount;
	m_imgBK.LoadFromResource(AfxGetResourceHandle(),nResourceID);
}


// CCheckBox 消息处理程序



void CCheckBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rtClient;
	GetClientRect(rtClient);

	CString strText;
	GetWindowText(strText);
	//
	if (!m_imgBK.IsNull())
	{
		int nHeight = m_imgBK.GetHeight()/m_nStateCount;
		if (m_bChecked)
			m_imgBK.Draw(dc.GetSafeHdc(),0,0,m_imgBK.GetWidth(),nHeight,0,0,m_imgBK.GetWidth(),nHeight);
		else
			m_imgBK.Draw(dc.GetSafeHdc(),0,0,m_imgBK.GetWidth(),nHeight,0,nHeight,m_imgBK.GetWidth(),nHeight);
	}

	HGDIOBJ hOldFont = NULL;
	if (m_font.m_hObject)
		dc.SelectObject(m_font.m_hObject);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_clrText);

	CRect rtText(m_imgBK.GetWidth()+3,0,rtClient.right,rtClient.bottom);
	dc.FillSolidRect(rtText,m_clrBkText);
	dc.DrawText(strText,rtText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	if (hOldFont)
		dc.SelectObject(hOldFont);
}

void CCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bChecked = !m_bChecked;

	CButton::OnLButtonUp(nFlags, point);
}

void CCheckBox::SetTitleColor( COLORREF clr )
{
	m_clrText = clr;
}

BOOL CCheckBox::GetChecked()
{
	return m_bChecked;
}

void CCheckBox::SetChecked( BOOL bChecked )
{
	m_bChecked = bChecked;
	Invalidate();
}

void CCheckBox::SetBkText( COLORREF clrBk )
{
	m_clrBkText = clrBk;
	Invalidate();
}



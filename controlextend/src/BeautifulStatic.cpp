// BeautifulStatic.cpp : implementation file
//

#include "stdafx.h"
#include "BeautifulStatic.h"
#include "FontFactory.h"

// CBeautifulStatic

IMPLEMENT_DYNAMIC(CBeautifulStatic, CStatic)

CBeautifulStatic::CBeautifulStatic()
{
	m_fontSize = 15;	// 字体默认大小为15
	m_strFontName = L"微软雅黑";	// 默认字体
	m_bkColor = RGB(0xF1, 0xF2, 0xEA);	// 背景的颜色
	m_textColor = Color(0xFF, 0x4D, 0x4D, 0x4D);	// 文字的默认颜色
	m_bTipMsgCenter = false;
}

CBeautifulStatic::~CBeautifulStatic()
{
}

BEGIN_MESSAGE_MAP(CBeautifulStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CBeautifulStatic message handlers

void CBeautifulStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	CBitmap bitmap;	//定义一个位图对象
	CDC MemeDc;	//首先定义一个显示设备对象
	//创建兼容设备DC，不过这时还不能绘图，因为没有地方画
	MemeDc.CreateCompatibleDC(&dc);	
	//建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小，也可以自己定义
	//（如：有滚动条时就要大于当前窗口的大小，在BitBlt时决定拷贝内存的哪部分到屏幕上）
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//将位图选入到内存显示设备中(只有选入了位图的内存显示设备才有地方绘图，画到指定的位图)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	//先用背景色将位图清除干净
	MemeDc.FillSolidRect(rect, m_bkColor); 
	//绘制文字
	CString strText;
	GetWindowText(strText);
	CFontFactory fontFactory;
	FontFamily fontFamily(fontFactory.GetExistFontName(m_strFontName.GetBuffer(0)));//...
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//显示Name的字体
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);	//垂直方向居中显示
	if (m_bTipMsgCenter)
		stringFormat.SetAlignment(StringAlignmentCenter);// 水平居中
	stringFormat.SetTrimming(StringTrimmingNone);
	Graphics graphics(MemeDc.m_hDC);
	RectF rectF(float(rect.left), float(rect.top), float(rect.Width()), float(rect.Height()));
	SolidBrush solidBrush(m_textColor);
	graphics.DrawString(strText, -1, &font, rectF, &stringFormat, &solidBrush);

	dc.BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);

	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
}

BOOL CBeautifulStatic::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void CBeautifulStatic::SetTextColor(COLORREF color)
{
	m_textColor = Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
	this->RedrawWindow();
}

void CBeautifulStatic::SetBKColor(COLORREF color)
{
	m_bkColor = color;
	this->RedrawWindow();
}

CRect CBeautifulStatic::AdjustStaticSize(int* nWidth)
{
	::SetWindowPos(this->GetSafeHwnd(), NULL, 0, 0, GetTextWidth(), m_fontSize+1, SWP_NOZORDER | SWP_NOMOVE);

	CRect rect;
	GetWindowRect(&rect);

	if(NULL != nWidth)
	{
		*nWidth = rect.Width();
	}

	Invalidate(true);

	return rect;
}

int CBeautifulStatic::AdjustStaticHeight()
{
	CRect rectStatic;
	GetClientRect(rectStatic);
	int nHeight = GetTextHeight();
	::SetWindowPos(this->GetSafeHwnd(), NULL, 0, 0, rectStatic.Width(), nHeight, SWP_NOZORDER | SWP_NOMOVE);

	Invalidate(true);

	return nHeight;
}
//
int CBeautifulStatic::GetTextWidth()
{
	CString strText;
	GetWindowText(strText);

	CFontFactory fontFactory;
	FontFamily fontFamily(fontFactory.GetExistFontName(m_strFontName.GetBuffer(0)));//...
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//显示Name的字体

	Graphics graphics(this->m_hWnd);
	RectF rectF(0, 0, 0, (Gdiplus::REAL)m_fontSize);	//
	RectF boundRect;	//最后能够容纳文字的最适合矩形
	int codepointFitted = 0;	//文字的个数
	int linesFiled = 0;	//文字的行数
	graphics.MeasureString(strText, strText.GetLength(), &font, rectF, NULL, &boundRect, &codepointFitted, &linesFiled);
	
	return int(boundRect.Width + 1);
}

int CBeautifulStatic::GetTextHeight()
{
	CString strText;
	GetWindowText(strText);

	CRect rectStatic;
	GetClientRect(rectStatic);
	
	CFontFactory fontFactory;
	FontFamily fontFamily(fontFactory.GetExistFontName(m_strFontName.GetBuffer(0)));//...
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//显示Name的字体

	Graphics graphics(this->m_hWnd);
	RectF rectF(0, 0, (Gdiplus::REAL)rectStatic.Width(), 0);	//
	RectF boundRect;	//最后能够容纳文字的最适合矩形
	int codepointFitted = 0;	//文字的个数
	int linesFiled = 0;	//文字的行数
	graphics.MeasureString(strText, strText.GetLength(), &font, rectF, NULL, &boundRect, &codepointFitted, &linesFiled);

	return int(boundRect.Height + m_fontSize);
}

void CBeautifulStatic::SetWindowText(LPCTSTR lpszString)
{
	CStatic::SetWindowText(lpszString);
	this->RedrawWindow();
}
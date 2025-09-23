// BeautifulStatic.cpp : implementation file
//

#include "stdafx.h"
#include "BeautifulStatic.h"
#include "FontFactory.h"

// CBeautifulStatic

IMPLEMENT_DYNAMIC(CBeautifulStatic, CStatic)

CBeautifulStatic::CBeautifulStatic()
{
	m_fontSize = 15;	// ����Ĭ�ϴ�СΪ15
	m_strFontName = L"΢���ź�";	// Ĭ������
	m_bkColor = RGB(0xF1, 0xF2, 0xEA);	// ��������ɫ
	m_textColor = Color(0xFF, 0x4D, 0x4D, 0x4D);	// ���ֵ�Ĭ����ɫ
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

	CBitmap bitmap;	//����һ��λͼ����
	CDC MemeDc;	//���ȶ���һ����ʾ�豸����
	//���������豸DC��������ʱ�����ܻ�ͼ����Ϊû�еط���
	MemeDc.CreateCompatibleDC(&dc);	
	//����һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С��Ҳ�����Լ�����
	//���磺�й�����ʱ��Ҫ���ڵ�ǰ���ڵĴ�С����BitBltʱ���������ڴ���Ĳ��ֵ���Ļ�ϣ�
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//��λͼѡ�뵽�ڴ���ʾ�豸��(ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	//���ñ���ɫ��λͼ����ɾ�
	MemeDc.FillSolidRect(rect, m_bkColor); 
	//��������
	CString strText;
	GetWindowText(strText);
	CFontFactory fontFactory;
	FontFamily fontFamily(fontFactory.GetExistFontName(m_strFontName.GetBuffer(0)));//...
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//��ʾName������
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);	//��ֱ���������ʾ
	if (m_bTipMsgCenter)
		stringFormat.SetAlignment(StringAlignmentCenter);// ˮƽ����
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
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//��ʾName������

	Graphics graphics(this->m_hWnd);
	RectF rectF(0, 0, 0, (Gdiplus::REAL)m_fontSize);	//
	RectF boundRect;	//����ܹ��������ֵ����ʺϾ���
	int codepointFitted = 0;	//���ֵĸ���
	int linesFiled = 0;	//���ֵ�����
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
	Gdiplus::Font font(&fontFamily, (Gdiplus::REAL)m_fontSize, FontStyleRegular, UnitPixel);	//��ʾName������

	Graphics graphics(this->m_hWnd);
	RectF rectF(0, 0, (Gdiplus::REAL)rectStatic.Width(), 0);	//
	RectF boundRect;	//����ܹ��������ֵ����ʺϾ���
	int codepointFitted = 0;	//���ֵĸ���
	int linesFiled = 0;	//���ֵ�����
	graphics.MeasureString(strText, strText.GetLength(), &font, rectF, NULL, &boundRect, &codepointFitted, &linesFiled);

	return int(boundRect.Height + m_fontSize);
}

void CBeautifulStatic::SetWindowText(LPCTSTR lpszString)
{
	CStatic::SetWindowText(lpszString);
	this->RedrawWindow();
}
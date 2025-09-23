#pragma once
#include "GdiPlusTS.h"
#include "dllexport.h"
#define COLOR_OUT_FRAME		Color(0xFF, 0xE6, 0xE6, 0xE6)	// 边框的颜色

// CBeautifulStatic

class CONTROL_EXPORT CBeautifulStatic : public CStatic
{
	DECLARE_DYNAMIC(CBeautifulStatic)

public:
	CBeautifulStatic();
	virtual ~CBeautifulStatic();
	void SetTextColor(COLORREF color);
	void SetBKColor(COLORREF color);
	void SetWindowText(LPCTSTR lpszString);
	CRect AdjustStaticSize(int* nWidth = NULL);	// 调整到合适宽度
	int  AdjustStaticHeight();	// 调整到合适高度
	inline void SetFontName(CString strFontName) {m_strFontName = strFontName;}
	inline void SetFontSize(int nFontSize){m_fontSize = nFontSize;}
	inline void SetTipMsgCenter(bool bCenter) { m_bTipMsgCenter = bCenter; }
private:
	int			m_fontSize;
	CString		m_strFontName;	// 文字字体名称
	Color		m_textColor;	// 文字的颜色
	COLORREF	m_bkColor;		// 背景色
	bool		m_bTipMsgCenter;// 提示文本是否居中
	int GetTextWidth();
	int GetTextHeight();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
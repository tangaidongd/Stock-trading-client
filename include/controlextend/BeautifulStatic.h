#pragma once
#include "GdiPlusTS.h"
#include "dllexport.h"
#define COLOR_OUT_FRAME		Color(0xFF, 0xE6, 0xE6, 0xE6)	// �߿����ɫ

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
	CRect AdjustStaticSize(int* nWidth = NULL);	// ���������ʿ��
	int  AdjustStaticHeight();	// ���������ʸ߶�
	inline void SetFontName(CString strFontName) {m_strFontName = strFontName;}
	inline void SetFontSize(int nFontSize){m_fontSize = nFontSize;}
	inline void SetTipMsgCenter(bool bCenter) { m_bTipMsgCenter = bCenter; }
private:
	int			m_fontSize;
	CString		m_strFontName;	// ������������
	Color		m_textColor;	// ���ֵ���ɫ
	COLORREF	m_bkColor;		// ����ɫ
	bool		m_bTipMsgCenter;// ��ʾ�ı��Ƿ����
	int GetTextWidth();
	int GetTextHeight();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
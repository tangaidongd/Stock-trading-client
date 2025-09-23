#pragma once
#include "staticsetcolor.h"

// CStaticEx

class CONTROL_EXPORT CStaticEx : public CStatic/*CStaticSetColor*/
{
	DECLARE_DYNAMIC(CStaticEx)

public:
	CFont m_font;
	int m_nAlign;
	COLORREF m_clrText1;
	COLORREF m_clrText2;
	CString m_strTitleText1;
	CString m_strTitleText2;

	COLORREF m_clrBkColor;

public:
	CStaticEx();
	virtual ~CStaticEx();

	void SetAlign(int nAlign);

	void SetTitleColor1(COLORREF clr);
	COLORREF GetTitleColor1();

	void SetTitleColor2(COLORREF clr);
	COLORREF GetTitleColor2();

	void SetTitleText1(CString strText);
	void GetTitleText1(CString &strText);

	void SetTitleText2(CString strText);
	void GetTitleText2(CString &strText);

	void SetBkColor(COLORREF clrColor);
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};



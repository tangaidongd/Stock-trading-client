#ifndef _MYSTATIC_H
#define _MYSTATIC_H

#include "dllexport.h"

class CONTROL_EXPORT CMyStatic:public CStatic
{
private:
	CString m_strlink;
public:
	DECLARE_DYNAMIC(CMyStatic)

	CMyStatic();
	void SetLinkObject(CString str);
	~CMyStatic(){}
public:
	static COLORREF t_unvclr;
	static COLORREF t_vdclr;
	static HCURSOR  t_hc;

public:
	void SetUnderline(BOOL bUnderline );
protected:
	CFont    m_font;
	COLORREF m_color;
	DECLARE_MESSAGE_MAP()
    
	afx_msg LRESULT   OnNcHitTest(CPoint point);
	afx_msg void   OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC,UINT nCtlColor);
	afx_msg BOOL   OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);
};

#endif _MYSTATIC_H
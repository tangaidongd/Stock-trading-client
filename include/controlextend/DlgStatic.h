#ifndef _DLGSTATIC_H_
#define _DLGSTATIC_H_
#include "typedef.h"
#include "dllexport.h"
// 不画背景的static - OnNcDestory自销毁，不要在栈上产生该控件
class CONTROL_EXPORT CDlgStatic :public CStatic
{
	DECLARE_DYNAMIC(CDlgStatic)

// Constructors
public:
	CDlgStatic();

	void	SetTransparentDrawText(bool32 bTransparent = true){m_bTransparentDrawText = bTransparent; };

	void	DrawTransparentBk(CDC *pDC);

	void SetTextFont(int nHeight, LPCTSTR fontName, BOOL bRedraw = true);
	void	SetMyTextColor(COLORREF clr);
private:
	bool32  m_bTransparentDrawText;

	COLORREF	m_crText;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;

	DWORD		m_dwOldSSStyle;		// 原来绘制style
	// Implementation
protected://限制堆上使用
	virtual ~CDlgStatic();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	virtual	void PostNcDestroy();
	// Implementation
protected:	
	
	// Generated message map functions
	//{{AFX_MSG(CDlgStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //_DLGSTATIC_H_
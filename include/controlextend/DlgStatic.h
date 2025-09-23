#ifndef _DLGSTATIC_H_
#define _DLGSTATIC_H_
#include "typedef.h"
#include "dllexport.h"
// ����������static - OnNcDestory�����٣���Ҫ��ջ�ϲ����ÿؼ�
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

	DWORD		m_dwOldSSStyle;		// ԭ������style
	// Implementation
protected://���ƶ���ʹ��
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
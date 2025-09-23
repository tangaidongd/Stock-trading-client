#ifndef _DLGCHECKBTN_H_
#define _DLGCHECKBTN_H_


#include <vector>
using std::vector;
#include "GdiPlusTS.h"
// ��ɾ�� - OwerDraw
#define BaseButton CButton
class CDlgCheckBtn : public BaseButton
{
	DECLARE_DYNAMIC(CDlgCheckBtn);
public:
	CDlgCheckBtn();
	~CDlgCheckBtn();

	void	SetTransparentDrawText(bool32 bTransparent = true){m_bTransparentDrawText = bTransparent; };

	virtual void PostNcDestroy();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

	int	    GetCheck()const;		// ���������BaseButton��Check�������BaseButton��ᵼ��OnGetCheck������
	void    SetCheck(int nCheck);	// ͬ�ϣ����ǵ�Check������checkboxһ��״̬

	void	DrawTransparentBk(CDC *pDC);
	
	void SetImageInfo(const UINT &unIdCheck,const UINT &unIdUnCheck);

	void	SetTextFont(int nHeight, LPCTSTR fontName, BOOL bRedraw = true);
	void	SetMyTextColor(COLORREF clr);
private:
	void	DrawMyButton();
	bool32  m_bTransparentDrawText;

	bool32	m_bDrawedFocus;
	bool32	m_bMouseOver;
	bool32	m_bLButtonDown;
	int 	m_iCheck;
	COLORREF m_clrTextColor;

	Image  *m_pImageUnCheck;
	Image  *m_pImageCheck;

	CFont		m_font;
	LOGFONT		m_lf;
	// Implementation
protected:	
	
	// Generated message map functions
	//{{AFX_MSG(CDlgCheckBtn)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetCheck(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetCheck(WPARAM w, LPARAM l);
	afx_msg BOOL    OnClicked();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);    // �˿ؼ����ᷢ�ͳ�BN_CLICK��Ϣ���κ���Ϣ��������
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCheckBtnOnlyOneCheckGroup
{
public:
	CCheckBtnOnlyOneCheckGroup(){}
	~CCheckBtnOnlyOneCheckGroup(){}

	void		AddCheckBtn(CButton *pBtn);
	void		RemoveCheckBtn(CButton *pBtn);

	void		OnSomeOneCheck(CButton *pBtn);
	void		ClearAllCheckStatus();

public:
	typedef vector<CButton *>	BtnVector;
	BtnVector	m_aBtn;
};

#endif //_DLGCHECKBTN_H_
#pragma once
#include "typedef.h"
#include <vector>
#include "dllexport.h"
//using std::vector;
using namespace std;
// CComboBoxCheck
typedef struct  tagITEMLIST
{
	HICON hIcon;		//图标
	CString strUrl;		//Url地址
	CString strTitle;	//标题(Url官网名称)
	COLORREF clrLeft;	//左边文本的颜色
	COLORREF clrRight;	//右边文本的颜色
	UINT     iItem;		//ID 号
}ItemList,*PItemList;

class CONTROL_EXPORT CComboBoxCheck : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxCheck)

public:
	CComboBoxCheck();
	virtual ~CComboBoxCheck();
	//lint -sem(std::vector::push_back, custodial(1))
	vector<struct  tagITEMLIST*>	m_vecItemList;
	typedef vector<struct  tagITEMLIST*>::iterator IterItem;

public:	

	//member function
	void InitControl(void);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxCheck)
public:
	virtual void    MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CComboBoxCheck)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave( WPARAM wParam,LPARAM lParam );
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchange();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void	UpdateRect(void);
	void OnNcPaint( CDC* pDC );
	void DrawContent( UINT iState,CRect rcClient, CDC* pDC,int itemID);

	BOOL	m_bOver;
	CBrush*	m_pBrsh;
	CPen	m_bgPen;
	CBrush* m_bgBrush;
	BOOL	m_bDown;
	CFont	m_font;

	int		m_selItem;	//select item flags
	BOOL	m_bFous;	//over fous
	int		m_preSelItem;	//pre select item flags

	CBitmap m_bMpPhone;

public:
	void AddItemEx(CString strContent);

};



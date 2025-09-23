#if !defined(AFX_SHORTCUTMENUTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_)
#define AFX_SHORTCUTMENUTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LeftToolBar.h : header file
//
#include "NCButton.h"
#include "XTipWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CShortCutMenuToolBar window


class CShortCutMenuToolBar : public CToolBar
{
// Construction
public:
	CShortCutMenuToolBar();
	virtual ~CShortCutMenuToolBar();

public:
	
/////////////////////////////////
    virtual BOOL    OnCommand(WPARAM wParam, LPARAM lParam);
	void			ControlDisStatus(bool bIsDis);			 // 


// Operations


	// Attributes
private:
	COLORREF m_clrTextNor;			     // 按钮文本的颜色
	COLORREF m_clrTextHot;
	COLORREF m_clrTextPress;
	std::map<int, CNCButton>			m_mapBtn;
	int	  m_iXButtonHovering;				// 标识鼠标进入按钮区域ID;

	Image	*m_pImgBk;				     // 工具栏背景图片
	int32   m_iToolBarWidth;            // 侧边栏背景图的宽度

	CWndCef			*m_pWndCef;				// 页面加载
	CString			m_StrCustomUrl;			// 网页
	CRect			m_RectWeb;				// web
	CXTipWnd		m_TipWnd;			// 按钮提示文字
	CPoint			m_PointLast;
		
	std::map<int, Image*>	m_mapSCBarImage;	// 工具栏图片：

private:
	void			InitFunctionButton();
	int				TButtonHitTest(CPoint point);
	int				FindKLineCycleId(unsigned int uiCycle);
	void			KLineCycleChange(int iKlineType);
	void			InitialToolBarBtns();
	void			InitialToolBarImage();
	void			ShowTips();
	void			LoadBtnImage();
	int				TButtonHitRect( CPoint point );
	void			SetCheckStatus( int iID );


	
	
	// Generated message map functions
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	//{{AFX_MSG(CShortCutMenuToolBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_)

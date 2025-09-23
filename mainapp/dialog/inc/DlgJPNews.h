#if !defined(AFX_DLGNEWS_H__3949C46A_9802_4CCC_93B5_E168BBFE3B90__INCLUDED_)
#define AFX_DLGNEWS_H__3949C46A_9802_4CCC_93B5_E168BBFE3B90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNews.h : header file
//

#include "GuiTabWnd.h"
#include "NCButton.h"
#include "client_handler.h"
#include "WndJPNews.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgJPNews dialog
#include <map>
#include <vector>
using namespace std;

class CDlgJPNews : public CDialog
{
// Construction
public:
	CDlgJPNews(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgJPNews();
	//
// Dialog Data
	//{{AFX_DATA(CDlgJPNews)
	enum { IDD = IDD_DIALOG_JP_NEWS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:

	// 获取当前鼠标所在的按钮
	int TButtonHitTest(CPoint point);
	
	// 在标题栏上添加按钮
	CNCButton *AddTitleButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	
	// 当鼠标离开标题栏时（按钮）状态改变
	void TitleBtnChange();

	void CalcNcSize();
	CGuiTabWnd *GetTabWnd();
	void SetTabSize();

	int GetTabCount() { return m_iTabCnt;}
//
private:
	CRect GetBrowerRect();	// 设置浏览器大小时用的一个函数

private:
	std::map<CString, CWndJPNews*> m_mapJPNews;
	//
	Image*  m_pBkImg;
	Image*	m_pSysMenuImg;
	Image*	m_pLogoImg;
	Image*	m_pTabImg;
	Image*	m_pPreImg;
	Image*	m_pNextImg;
	int		m_iXButtonHovering;
	BOOL	m_bShowBtn;
	int		m_iTabCnt;

	CRect   m_rectCaption;
	CRect   m_rectLeftBoard;
	CRect   m_rectRightBoard;
	CRect   m_rectBottomBoard;
	CRect   m_rectBorder;

	CGuiTabWnd			*m_pGuiTabWnd;
	map<int, CNCButton> m_mapTitleBtn;		// 标题按钮列表

	enum{
		ID_PRE_BTN = 100,
		ID_NEXT_BTN,
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgJPNews)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgJPNews)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcMouseLeave(WPARAM w, LPARAM l);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabDelete(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWS_H__3949C46A_9802_4CCC_93B5_E168BBFE3B90__INCLUDED_)

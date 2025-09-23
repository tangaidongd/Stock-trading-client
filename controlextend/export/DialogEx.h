#if !defined(AFX_DIALOGEX_H__600A75BF_98F7_408A_8250_C00371487135__INCLUDED_)
#define AFX_DIALOGEX_H__600A75BF_98F7_408A_8250_C00371487135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEx.h : header file
//
#include "typedef.h"
#include <gdiplus.h>
using namespace Gdiplus;
#include "dllexport.h"
/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog

class CONTROL_EXPORT CDialogEx : public CDialog
{
	DECLARE_DYNCREATE(CDialogEx)
public:
	enum E_CurrentShowState
	{
		ECSSNormal = 0,
			
		ECSSForcusMax,
		ECSSForcusMin,
		ECSSForcusClose,
		
		ECSSPressMax,
		ECSSPressMin,
		ECSSPressClose,
		//
		ECSSCount			
	};
	
// Construction
public:
	CDialogEx();   // standard constructor
	CDialogEx(UINT uIDTemplate,CWnd * pParent);
	~CDialogEx();
	void	CalcNcSize();
	void	SetWindowTextEx(const CString& StrText);

	virtual	void	DrawMyNcPaint(CWindowDC &dc);
	void	DrawNcClient(CWindowDC &dc);			// 目前未实现
	void	DrawSCButtons(CWindowDC &dc, Gdiplus::Graphics *pGraphics);			// ，如果pGra NULL，则绘制文字
	void    SetCaptionBKColor(COLORREF clr) { m_CaptionBKColor = clr; }
	void    SetFrameColor(COLORREF clr) { m_FrameColor = clr; }
	void    SetCaptionColor(COLORREF clr) { m_CaptionColor = clr; }
	void    SetNeedChangeColor(bool32 bNeed) { m_bNeedChangeColor = bNeed; }
	void    SetNeedDrawSpliter(bool32 bNeed) { m_bNeedDrawSpliter = bNeed; }
protected:
	void	Construct();
	virtual void OnClickClose(){}
	
	COLORREF GetBackColor();

	COLORREF m_CaptionBKColor;
	COLORREF m_FrameColor;
	COLORREF m_CaptionColor;
	bool32   m_bNeedChangeColor;
	bool32   m_bNeedDrawSpliter;

	CString	m_StrWindowText;
	bool32	m_bInitializedText;

	CRect	m_rectMin;
	CRect   m_rectMax;
	CRect	m_rectClose;
	CRect   m_rectCaption;
	
	CRect   m_rectLeftBoard;
	CRect   m_rectRightBoard;
	CRect   m_rectBottomBoard;

	Image*	m_pImageButtons;
	Image*	m_pImageCatptionBig;
	Image*	m_pImageCatptionSmall;

	bool32	m_bHelpDlg;

	HBRUSH	m_BrushBack;
	HBRUSH	m_BrushEditBack;
// Dialog Data
	// {{AFX_DATA(CDialogEx)
	// enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA
	
	BOOL	m_bActive;
	bool	m_bToolWindow;	
	bool	m_bHaveMaxBox;
	bool	m_bNeedColorText;
	bool32	m_bNcMouseMoving;
	int32	m_nFrameWidth;

	bool    m_bOnlyShowClose;	// ...fangz1012 只要关闭按钮了
	E_CurrentShowState m_eCurrentShowState;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEx)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnNcMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnSetWindowText(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetWindowText(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetWindowTextLength(WPARAM w, LPARAM l);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEX_H__600A75BF_98F7_408A_8250_C00371487135__INCLUDED_)

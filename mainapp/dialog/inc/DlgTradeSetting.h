#if !defined(AFX_TRADESETTING_H__INCLUDED_)
#define AFX_TRADESETTING_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTradeSetting.h : header file
#include "DlgTradeSetup.h"
#include "DlgTradeDownload.h"

#include <map>


using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgTradeSetting dialog



class CDlgTradeSetting : public CDialog
{
// Construction
public:
	CDlgTradeSetting( CWnd* pParent = NULL);   // standard constructor
	~CDlgTradeSetting();

public:
	CDialog        *m_pPages[2];        //定义的2，就是说能够存放的最多对话框，根据需求
	UINT            m_uIDD[2];           //每个对话框对应的IDD_XXX
	int             m_iNumOfPage;       //标示数组下标
	CRect           m_rcShowRect;       //画布

	CDlgTradeSetup       *m_pDlgTradeSetup;          // 主界面
	CDlgTradeDownload    *m_pDlgTradeDownload;       // 下载界面
	//函数
public:
	BOOL            AddPage(CDialog* pDialog, UINT ID);//添加进空对话框
	void            ShowPage(int CurrentPage);     //把你所需要显示的对话框显示出来
	
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// 鼠标是否在按钮上

	void			DrawTitleBar();

	Image		   *m_pImgCaption;
	Image          *m_pImgClose;
	
	std::map<int, CNCButton> m_mapBtn;
	int			             m_iXButtonHovering;			// 标识鼠标进入按钮区域
	bool32		   m_bShowWeb;					// 有网址的时候, 隐藏图片
	CRect		   m_rcCaption;				// 标题栏的区域
protected:	
	CFont		   m_fontStaticText;
	CFont		   m_fontCheckText;

public:
// Dialog Data
	//{{AFX_DATA(CDlgTradeSetting)
	enum { IDD = IDD_DIALOG_TRADE_SETTING };
		// NOTE: the ClassWizard will add data members here

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTradeSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTradeSetting)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADESETTING_H__INCLUDED_)

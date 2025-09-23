// GGTongView.h : interface of the CGGTongView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GGTONGVIEW_H__BEAA0FD9_B7BD_458A_AEE6_E3CE65096E64__INCLUDED_)
#define AFX_GGTONGVIEW_H__BEAA0FD9_B7BD_458A_AEE6_E3CE65096E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IoViewManager.h"

#include "MPIChildFrame.h"
#include "tinyxml.h"


#define SENSITIVESIZE		3
#define	SENSITIVETIMER		9779
#define VALIDAREASIZE		50
		 
class CGGTongView : public CView
{
public:
	enum E_DragDirection
	{
		EDDNONE = 0,
		EDDLEFT ,
		EDDRIGHT,
		EDDTOP,
		EDDBOTTOM
	};
protected: // create from serialization only
	CGGTongView();
	
public:
	virtual ~CGGTongView();
	
	DECLARE_DYNCREATE(CGGTongView)

// Attributes
public:
	CGGTongDoc* GetDocument();
// Operations
public:
	static const char *	GetXmlElementValue();
	static CString		GetDefaultXML(int32 iGroupID,bool32 bBlank, CIoViewBase *pIoView = NULL);
	CString				ToXml(CGGTongView * pActiveView);
	bool32				FromXml(CWnd *pParent,TiXmlElement *pElement);
	bool32				GetEmptyFlag();
	bool32				GetDragFlag();	
	bool32				GetMaxF7Flag();
	void				ClearAllDragFlags();
	void				DrawDragLine(CPoint pt,E_DragDirection eDragDirection);
	void				DealEnterKey();
	int32				FindIoView(bool32 bKline);				// 找除了自己,有KLine 或 Trend 没有?
	void				SetActiveCrossFlag(bool32 bActive) { m_bActiveCross = bActive ;}
	void				SetSubIoViewF7MaxFlag();
	void				SetBiSplitTrackDelFlag(bool32 bDel);
	void				DrawGGTongView(CDC* pDC);
	void				DrawGGTongFocusRect();

	bool32				IsLockedSplit() const;			// 锁定分割模式
	bool32				IsLockMinMaxInfo() const { return m_bLockMinMaxInfo; }

public:
	static bool32		GetDragMerchFlag()	{ return m_bDragMerch; }
private:
	void				AddSplit(CMPIChildFrame::E_SplitDirection eSD,double dRatio = 0.5 );
	
private:
	bool32				BeValidDragMerchBegin(CPoint pt);
	bool32				OnDragMerchEnd();
	void				ClearDragMerchFlags();
public:
	CIoViewManager		m_IoViewManager;
	bool32				m_bShowGtongVeiwBorder;				// 是否绘制边框
protected:
	bool32				m_bEmptyGGTongView;
	bool32				m_bPrepareDrag;						// 为了使用自定义的鼠标指针,给其他模块通知的信号,以免冲突
	bool32				m_bLButtonDown;
	bool32				m_bValidAction;
	bool32				m_bActiveCross;						// 处理ESC 键的时候,判断有没有十字光标
	//bool32				m_bF7MaxedView;							// 是否F7 全屏了	// F7实际的操作动作交给Split处理的，所以修改F7的判断交给split(frame)来处理
	bool32				m_bFromXML;							
	CPoint				m_ptDraging;						// 拖动的当前点坐标
	CPoint				m_ptDragPre;						// 拖动的前一点坐标,用于擦除痕迹	
	E_DragDirection		m_eDragDirection;

	bool32				m_bTracking;
	bool32				m_bDelByBiSplitTrack;
	bool32              m_bActiveFocus;						// 是否画focus

	bool32				m_bLockMinMaxInfo;					// 是否具有固定大小，目前仅最小值生效
	bool32				m_bCanF7;							// F7属性是否生效
	MINMAXINFO			m_mmiLockMinMaxInfo;				// 具体的大小

private:
	static  bool32		m_bDragMerchLBDown;
	static	bool32		m_bDragMerch;
	static  CGGTongView* m_pGGTViewDragBegin;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGGTongView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnActivateFrame(UINT nState, CFrameWnd* pFrameWnd);
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CGGTongView)
	afx_msg void OnWindowSplitAddRight();
	afx_msg void OnWindowSplitAddLeft();
	afx_msg void OnWindowSplitAddTop();
	afx_msg void OnWindowSplitAddBottom();
	afx_msg void OnWindowSplitAddTab();
	afx_msg void OnWindowSplitDel();
	afx_msg void OnLayOutAdjust();
	afx_msg void OnUpdateLayoutAdjust(CCmdUI *pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAddSplit(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnIsKindOfCGGTongView(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetActiveCrossFlag(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetBiSplitTrackDelFlag(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnToXml(WPARAM wParam,LPARAM lParam);
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnUpdateLockMinMax(CCmdUI *pCmdUI);
	afx_msg void OnLockMinMax();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

friend class CAdjustViewLayoutDlg;
friend class CIoViewBase;
friend class CMainFrame;
friend class CTabSplitWnd;
friend class CIoViewReport;
};

#ifndef _DEBUG  // debug version in GGTongView.cpp
inline CGGTongDoc* CGGTongView::GetDocument()
   { return (CGGTongDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GGTONGVIEW_H__BEAA0FD9_B7BD_458A_AEE6_E3CE65096E64__INCLUDED_)

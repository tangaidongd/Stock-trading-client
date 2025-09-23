/*********************************************************
* Multi-Page Interface
* Version: 1.2
* Date: September 2, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#pragma once

#include "TabSplitWnd.h"
#include "tinyxml.h"
#include "facescheme.h"
class CGGTongView;
class CIoViewBase;


// extended create context
struct CMPICreateContext : public CCreateContext
{
	int*	m_pSplitData;
	DWORD	m_dwViewParam;
};
// don't use these constants directly
// use the MPI_ macros from MPIDocTemplate.h
#define MPIT_VIEW		0
#define MPIT_HSPLIT		1
#define MPIT_VSPLIT		2
#define MPIT_TABS		3
#define MPIT_END		100

#define MPIS_BAR_LEFT	-1
#define MPIS_BAR_RIGHT	-2
#define MPIS_BAR_TOP	-1
#define MPIS_BAR_BOTTOM	-2

#define MPIBS_AUTO		-1

class CMPIChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMPIChildFrame)		
public:	
	enum E_CurrentShowState
	{
		ECSSNormal = 0,
			
		ECSSForcusMax,
		ECSSForcusMin,
		ECSSForcusClose,			
		ECSSForcusLayOut,
		ECSSForcusF7,
		
		ECSSPressMax,
		ECSSPressMin,
		ECSSPressClose,
		ECSSPressLayOut,
		ECSSPressF7,
		//
		ECSSCount			
	};
	enum E_SplitDirection
	{
		SD_TAB =0,
		SD_LEFT,
		SD_RIGHT,
		SD_TOP,
		SD_BOTTOM
	};
public:
	CMPIChildFrame();

public: 
	CString				ToXml(int32 iZIndex , const char * KpcFileName);
	bool32				FromXml (TiXmlElement *  pTiXmlElement );
	CString				ToXmlForWorkSpace(const char * KpcFileName);
	static CString		GetDefaultXML(bool32 bBlank,CRect RectPre, CIoViewBase *pIoView = NULL);
	static const char *	GetXmlElementValue();

	CString				SaveChildFrameFaceObjectToXml();
	bool32				GetChildFrameFaceObjectFromXml(IN TiXmlElement * pRootElement,OUT T_ChildFrameFaceObject& ChildFaceObject);
public:
	void				AddSplit(CView *pViewCur, CRuntimeClass *pNewViewClass, E_SplitDirection eSD, CMPICreateContext* pContext,double dRatio = 0.5);
	void				DelSplit(CView *pViewCur);

	// 锁定分割后，不允许任何的版面设置中的选项操作 - 锁定页面合适？
	// 切换窗口，关闭窗口，设置分组(手动), 关闭业务视图(手动), 新建业务视图(手动)
	bool32				IsLockedSplit() const;	// 是否锁定了分割
	void				SetLockSplit(bool32 bLock = false);	// 锁定分割
	void				SetF7AutoLock(bool32 bF7Lock);	// F7最大化自动锁定分割
	bool32				IsF7AutoLock() const;	// 是否是F7锁定

	CString				GetIdString() const;		// 标志该页面的String，不允许打开多个同id的页面(空字串为无id)
	void				SetIdString(const CString &StrId = _T(""));	// 设置页面id，空字串无id, 仅用于标志页面，其它情况下不要设置

	CGGTongView			*GetF7GGTongView() const;	// 获取当前被F7的窗口，可能为NULL
	void				SetF7GGTongViewXml(CGGTongView *pView);	// 设置在FromXml中应当被F7化的窗口

	int32				RemoveSplitMenuItem(CMenu &menu);

	// 隐藏模式下建议ioview不发送数据，不更新界面，等待非隐藏模式时在更新数据&显示
	void				SetHideMode(bool32 bHide) { m_bHideMode = bHide;  };		// 是否进入隐藏模式，
	bool32				IsHideMode() const { return m_bHideMode; }

	void				EnableRequestData(bool32 bEnable) { m_bEnableRequestData = bEnable; }		// 允许所有子视图商品请求, 如果设置此标志，子视图应当注意现在可能不适合请求商品数据
	bool32				IsEnableRequestData() const { return m_bEnableRequestData; }

	void				RecalcLayoutAsync();		// 延后的重计算大小

public:
	bool32				DoTabSplitElement(TiXmlElement *pElement);
	bool32				DoBiSplitElement(TiXmlElement *pElement, CWnd *pParent, int32 iDlgItemId);
	bool32				DoGGTongViewElement(TiXmlElement *pElement, CWnd *pParent, int32 iDlgItemId);

public:
	void				SetActiveGGTongView(CGGTongView *pGGTongView = NULL);
	void				ForceRefresh();
public:
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual void		ActivateFrame(int nCmdShow = -1);						
	virtual void		OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	virtual void		RecalcLayout(BOOL bNotify = TRUE);
public:	
	int32				GetGroupID();
	void				SetGroupID(int32 iID, bool32 bChangeSub = false, bool32 bChangeMerch = false);
protected:
	virtual BOOL		OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void		OnUpdateFrameTitle(BOOL bAddToTitle);

	void				ChangSubGroupId(CWnd *pWnd, int32 iId, bool32 bChangeMerch);
public:
	virtual ~CMPIChildFrame();

#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

public:
	bool32				GetMaxSizeFlag(){ return m_bMaxSize; }
	void				PostUpdateWnd();
public:
	void				SetChildFrameTitle(CString StrTitle,bool32 bFix = false);
	void				SetFixTitleFlag(bool32 bFix) { m_bFixTitle = bFix; }
	void				SetHistoryFlag(bool32 bHis)	 { m_bHistoryChildFrame = bHis; }

protected:
	BOOL				CreateClientHelper(int& nPos, CWnd* pParent, int nID, CMPICreateContext* pContext);
	void				CalcNcSize();

private:
	void				IntialImageResource();
	void				DrawCustomNcClient(bool32 bAllDraw = true);

	void				FindGGtongView(CWnd *pWnd, bool32 bActive);
///////////////////////////////////////////////////////////////////////////
// 颜色字体相关
	
public:
	
	void				SetStyleIndex(int32& iStyleIndex);
//	COLORREF			GetFrameColor(E_SysColor eSysColor);
//	LOGFONT *			GetFrameFont(E_SysFont eSysFont);
//	CFont *				GetFrameFontObject(E_SysFont eSysFont);

//	void				SetSysFontArray(CFontNode (&aSysFont)[ESFCount]); 
//	void				SetSysColorArray(COLORREF (&aSysColor)[ESCCount]);
	
//	void				SetSysFontArraySave(CArray<T_SysFontSave,T_SysFontSave>& aSysFontSave);
//	void				SetSysColorArraySave(CArray<T_SysColorSave,T_SysColorSave>& aSysColorSave);

//	void				ChangeFrameFont(bool32 bLarger);
protected:
	CRect				m_rectCaption;
	CRect				m_rectClose;
	CRect				m_rectMax;
	CRect				m_rectMin;
	CRect				m_rectLayOutAdjust;
	CRect				m_rectF7;
	
	CRect				m_rectBig;
	CRect				m_rectSmall;

	CRect				m_rectLast;

	bool32				m_bActive;
	bool32				m_bMaxSize;
	bool32				m_bMinSize;

	bool32				m_bHistoryChildFrame;

private:
	bool32				m_bFixTitle;						// 固定ChildFrame 标题.false 时随便修改.true 时.判断SetChildFrameTitle 的传入参数,如果传false 则标题不改,传true 有效.
	CString				m_StrTitle;

	bool32				m_bLockedSplit;
	bool32				m_bF7LockSplitOrg;
	bool32				m_bF7AutoLock;

	CGGTongView			*m_pF7GGTongViewXml;	// fromXml时该窗口应当被F7

	CString				m_StrId;			// 描述该页面的一个特殊字串，应保证同页面唯一性

	bool32				m_bHideMode;	// 是否在xml化时隐藏

	bool32				m_bEnableRequestData;	// 是否允许请求数据
				
	int32				m_iGroupID;	
	
public:
	int32				m_iShowFlag;
	int32				m_iStyleIndex;

	CArray<void*, void*> m_aTest;
	
	// CFontNode			m_aSysFont[ESFCount];
	// COLORREF			m_aSysColor[ESCCount];

	// CArray<T_SysFontSave,T_SysFontSave>		 m_aSysFontSave;
	// CArray<T_SysColorSave,T_SysColorSave>	 m_aSysColorSave;
public:	
	CMPICreateContext*		m_pContext;
	CTabSplitWnd			m_TabSplitWnd;
	bool32					m_bHaveTab;
protected:
	// CTypedPtrList<CPtrList, CWnd*> m_listSplitters;
	CArray<CWnd*, CWnd*>	m_listSplitters;
private:
	static CMPIChildFrame*  m_pThis;

	bool32			  m_bWindowTextValid;		// 标题
	CString			  m_StrWindowText;

private:
    Image*			   m_pImageButtons;
	
	Image*			   m_pButtonLayOut;

	Image*			   m_pCaptionActive;
	Image*			   m_pCaptionInActive;
	E_CurrentShowState m_eCurrentShowState;
	//bool32			   m_bNcMouseMove;   // 使用单独的函数控制
public:
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged( WINDOWPOS* lpwndpos );
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);	
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	//afx_msg void OnTabMenu(UINT nID);	
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg LRESULT OnChildFrameMaxSize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameMinSize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnNcMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetWindowText(WPARAM w, LPARAM l); // 拦截设置窗口标题消息 WM_SETTEXT
	afx_msg LRESULT OnGetWindowText(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetWindowTextLength(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetGroupID(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnDoBiSplitElement(WPARAM wParam,LPARAM lParam);
	afx_msg	LRESULT OnDoGGTongViewElement(WPARAM wParam,LPARAM lParam);
	afx_msg	LRESULT OnIsKindOfCMpiChildFram(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnIsHideMode(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnDelSplit(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnIsLockedSplit(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetF7AutoLock(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT On_IsF7AutoLock(WPARAM wParam,LPARAM lParam);
	//	virtual BOOL PreTranslateMessage(MSG* pMsg);


	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};










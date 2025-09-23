#pragma once

#include <afxpriv.h>
#include "dllexport.h"
// CMyDockContext
class CONTROL_EXPORT CMyDockContext : public CDockContext
{

public:
	CMyDockContext(CControlBar* pBar):CDockContext(pBar){ m_bEnableDrag = false; }

	virtual void StartDrag(CPoint pt);

	virtual	void	StartResize(int nHitTest, CPoint pt);

	void	EnableDrag(bool bEnable){ m_bEnableDrag = bEnable; }
	bool	IsEnableDrag() const { return m_bEnableDrag; }

	void	StretchDock(CPoint pt);
	BOOL	TrackDock();
	void	DrawResizeDockLine(BOOL bRemove=FALSE);
	void	CancelLoopDock();
	void	EndResizeDock();

private:
	bool m_bEnableDrag;
};

// CMyDlgBar

class CONTROL_EXPORT CMyDlgBar : public CDialogBar
{
	DECLARE_DYNAMIC(CMyDlgBar)

public:
	CMyDlgBar();
	virtual ~CMyDlgBar();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CSize	m_sizeMRU;
	CSize	m_sizeResizeTmp;
	CSize	m_sizeDockMRU;

	HCURSOR	m_hOldCursor;

private:
	BOOL	PtInResizeSep(CPoint ptInScreen);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt );
	afx_msg	BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


// CMyControlBar
class CONTROL_EXPORT CMyControlBar : public CControlBar
{
	DECLARE_DYNAMIC(CMyControlBar)

	// Construction
public:
	CMyControlBar();


	// Implementation
public:
	virtual ~CMyControlBar();

	virtual BOOL		Create(LPCTSTR lpszWindowName, DWORD dwStyle, CWnd* pParentWnd, UINT nId=AFX_IDW_CONTROLBAR_LAST);
	virtual void		OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);

	virtual BOOL PreTranslateMessage(MSG* pMsg);


	void			SetSizeDefault(CSize sizeDef);	// 默认大小
	CSize			GetSizeDefault() const { return m_sizeDefault; }

	CSize			GetCurrentSize();
	void			SetNewSize(CSize sizeNew);

	void			ToggleMyDock();		// 停靠/悬浮切换

	// 仅用户MDIFrameWnd
	CMDIFrameWnd	*GetParentMDIFrame();
	BOOL			Maximize();
	BOOL			IsMaximized();
	BOOL			RestoreSize();
	BOOL			Minimize();
	BOOL			IsMinimized();
	
	void		SetFixedHeight(bool bFiexd,const int iHeight);	//bFiexd=true固定高度，iHeight高度
private:
	CSize	m_sizeMRU;
	CSize	m_sizeResizeTmp;
	CSize	m_sizeDockMRU;

	HCURSOR	m_hOldCursor;

	CRect	m_rcLeft;		// 左侧区域

	CSize	m_sizeDefault;

	CSize	m_sizeLastRestore;
	
	int 			m_iFixedHeight;	//固定大小
	bool			m_bSetFixedSize;
private:
	BOOL	PtInResizeSep(CPoint ptInScreen);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg	BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void	OnPaint();
};

class CONTROL_EXPORT CMyDockBar : public CDockBar
{
	// Construction
public:
	/* explicit */ CMyDockBar(BOOL bFloating = FALSE) : CDockBar(bFloating){};   // TRUE if attached to CMiniDockFrameWnd

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:
	
};

class CONTROL_EXPORT CMyMiniDockFrameWnd : public CMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CMyMiniDockFrameWnd)

public:
	// Construction
	CMyMiniDockFrameWnd();

	// Operations

	// Implementation
public:
};

class CONTROL_EXPORT CMyContainerBar : public CMyControlBar
{
	DECLARE_DYNAMIC(CMyContainerBar)

	// Construction
public:
	CMyContainerBar();

	typedef CArray<CWnd *, CWnd *> WndPtrArray;

	CWnd		*GetCurWin();			// 没有则返回NULL
	void		AddWin(CWnd *pWnd);		// 新加入的窗口会设置成为该子窗口，并且大小与显示受控与此
	void		RemoveWin(CWnd *pWnd);	// 移出的窗口不会取消该父子关系，外部要转移其父子关系，不会自动显示其它窗口
	int			SetCurWin(CWnd *pWnd);	// 设置当前的显示窗口，其余窗口不可显示，如果为NULL，则隐藏所有窗口

	void		GetSubWndPtrs(WndPtrArray &wndPtrs){ wndPtrs.Copy(m_aWndPtrs); };

	// Implementation
public:
	virtual ~CMyContainerBar();

private:
	WndPtrArray		m_aWndPtrs;
	int				m_iCur;

	void		AdjustSubWinRect(CWnd *pWndSub, LPRECT pRect=NULL);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
};

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


	void			SetSizeDefault(CSize sizeDef);	// Ĭ�ϴ�С
	CSize			GetSizeDefault() const { return m_sizeDefault; }

	CSize			GetCurrentSize();
	void			SetNewSize(CSize sizeNew);

	void			ToggleMyDock();		// ͣ��/�����л�

	// ���û�MDIFrameWnd
	CMDIFrameWnd	*GetParentMDIFrame();
	BOOL			Maximize();
	BOOL			IsMaximized();
	BOOL			RestoreSize();
	BOOL			Minimize();
	BOOL			IsMinimized();
	
	void		SetFixedHeight(bool bFiexd,const int iHeight);	//bFiexd=true�̶��߶ȣ�iHeight�߶�
private:
	CSize	m_sizeMRU;
	CSize	m_sizeResizeTmp;
	CSize	m_sizeDockMRU;

	HCURSOR	m_hOldCursor;

	CRect	m_rcLeft;		// �������

	CSize	m_sizeDefault;

	CSize	m_sizeLastRestore;
	
	int 			m_iFixedHeight;	//�̶���С
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

	CWnd		*GetCurWin();			// û���򷵻�NULL
	void		AddWin(CWnd *pWnd);		// �¼���Ĵ��ڻ����ó�Ϊ���Ӵ��ڣ����Ҵ�С����ʾ�ܿ����
	void		RemoveWin(CWnd *pWnd);	// �Ƴ��Ĵ��ڲ���ȡ���ø��ӹ�ϵ���ⲿҪת���丸�ӹ�ϵ�������Զ���ʾ��������
	int			SetCurWin(CWnd *pWnd);	// ���õ�ǰ����ʾ���ڣ����ര�ڲ�����ʾ�����ΪNULL�����������д���

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

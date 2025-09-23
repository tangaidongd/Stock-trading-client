#pragma once

#if (_MFC_VER == 0x0600)
#include <winable.h>
#endif

//
// BALLOONMSG
//
// Written by Paul Roberts
//
// Uses a small, transparent window to launch the tooltip in
// a separate thread. This enables the tooltip to be used in
// almost any situation, without any big changes to the
// calling code (i.e. no need to route msgs via RelayEvent etc)
//
// Version 1:	Initial version, used non-tracking tooltips
// Version 2:	Changed to tracking tooltip and added extra TRACEs to help debug
//				cases where the balloons don't appear.
//

/////////////////////////////////////////////////////////////
// CBalloonMsgWnd
//
// A small transparent window that sits above the initial mouse
// position and is the parent for the tooltip.
class CBalloonMsgWnd : public CWnd
{
	DECLARE_DYNAMIC(CBalloonMsgWnd)

public:
	CToolTipCtrl	m_wndToolTip;
	CString			m_strHdr;
	CString			m_strBody;
	HICON			m_hIcon;
	BOOL			m_bReposition;
	CPoint			m_ptReposition;
	UINT_PTR		m_nTimer;
	UINT			m_nTimerCount;

protected:
	static CString	s_strWndClass;	// Custom window class, registered once and re-used
	LPVOID			m_pTTBuffer;	// Text for tooltip

public:
	CBalloonMsgWnd();
	virtual ~CBalloonMsgWnd();

protected:
	DECLARE_MESSAGE_MAP()
	LPCTSTR					GetWndClass();
	virtual BOOL			OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL			Create( CWnd* pParent );
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual afx_msg BOOL	OnToolTipText( UINT id, NMHDR* pNMHDR, LRESULT* pResult );
	virtual BOOL			PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL			OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void			OnTimer( UINT_PTR nIDEvent );
};

/////////////////////////////////////////////////////////////
// CBalloonMsgThread
//
// A separate user interface thread that provides a dedicated message queue
// for our tooltip.
// 
class CBalloonMsgThread : public CWinThread
{
	DECLARE_DYNCREATE(CBalloonMsgThread)
	
// Data members
public:
	DWORD				m_dwIDPrimaryThread;
	CBalloonMsgWnd		m_Wnd;
	GUITHREADINFO		m_GTI;					// To check focus etc in the primary thread
	BOOL				m_bExit;
	static BOOL			s_bExitAll;				// Flag use to cause all current balloons to close

protected:
	CBalloonMsgThread();						// protected constructor used by dynamic creation

public:
	virtual			~CBalloonMsgThread();
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual BOOL	OnIdle(LONG lCount);
	virtual void	RequestClose() { m_bExit = TRUE; }
	virtual void	GetWindowStates( LPGUITHREADINFO pGTI );
	
	//////////////////////////////////////////////////////////
	// Static methods to be called by client code
	static	void	RequestCloseAll()	{ s_bExitAll = TRUE; }
	static void		ResetCloseAll()		{ s_bExitAll = FALSE; }
	//////////////////////////////////////////////////////////

protected:
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////
// CBalloonMsg
class CBalloonMsg
{
public:
	// Statics used to control the behavior of the tooltip
	static	UINT	s_nInitialDelay;	// In Millisecs
	static	UINT	s_nAutoPop;			// Time before self-close; in millisecs
	static	UINT	s_nMaxTipWidth;		// In pixels, set to force linebreaks in tooltip content
	static	UINT	s_nToolBorder;		// Used to judge when the mouse has moved enough to pop the balloon
	static	UINT	s_nTimerStep;		// Elapse time in millisecs for our timer

private:
	// The static Show methods...
	static void	ShowFinal( LPCTSTR lpszHdr, LPCTSTR lpszBody, LPPOINT pPt = NULL, HICON hIcon = NULL );

public:	
	static void	Show( CString StrTitle, CString StrBody, HICON hIcon = NULL, LPPOINT pPt = NULL );
	static void	Show( UINT nIDStrHdr, UINT nIDStrBody, LPPOINT pPt = NULL, HICON hIcon = NULL );

	static void	ShowForCtrl( LPCTSTR lpszHdr, LPCTSTR lpszBody, CWnd* pCtrl, HICON hIcon = NULL );
	static void	ShowForCtrl( UINT nIDStrHdr, UINT nIDStrBody, CWnd* pCtrl, HICON hIcon = NULL );

	// And quick way to get rid of the balloon...
	static void	RequestCloseAll()	{ CBalloonMsgThread::RequestCloseAll(); }
	
	static bool IsBaloonTipEnabled();
};

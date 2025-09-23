#if !defined(AFX_COMMENTSTATUSBAR_H__)
#define AFX_COMMENTSTATUSBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommentStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommentStatusBar window
#include "facescheme.h"
#include "XTipWnd.h"
#include "resource.h"
#include "XTimer.h"
#include "AlarmCenter.h"
#include "WndCef.h"

class CCommentStatusBar : public CStatusBar//, public CXTimerAgent, public CAlarmNotify, public iAuthUserDataNotify
{

// Construction
public:
	CCommentStatusBar();
////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommentStatusBar)
	//}}AFX_VIRTUAL
	// Implementation

public:
	virtual ~CCommentStatusBar();
	// Generated message map functions
public:
	void  SetStatusTimer();
	//��ӵ������
	void  InsertAtComment(T_NewsInfo &stFirst);
	void  OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo);
	void  AddBatchComment(IN listNewsInfo& listTitles);
private:
	void  InitStatusBBSLogFont( OUT LOGFONT &lf );
	CSize GetProperBBSSizeShow();
	void  DrawRectBBS(CDC* pDC, CRect& rt, CRect &RectShow);
	int32 GetAdvIndexByPoint(CPoint pt);
	void  HitTestRectData(CPoint pt, int32 iType);
	void  RequestCommentContent(int iIndexId);
	void  RequesComments(int32 iType);



public:
	COLORREF	m_clrNorText;
	COLORREF	m_clrSelText;
	COLORREF	m_clrOtherText;

	//	����url��ɫ����
	CString		m_strBgColor;
	CString		m_strColor;
	CString		m_strHoverColor;

private:
	Image*		m_pImageBkGround;
	CRect       m_RectBBS;
	CRect       m_RectShow;
	CPoint		m_ptLastMouseMove;
	BOOL		m_bMouseTrack;
	BOOL        m_bRun;
	int			m_iIndexID;
	int			m_iSelIndexID;
	bool        m_bAddItemComment;
	DWORD       m_dwGetDataTickComment;
	int32		m_iInfoType;  // ��Ѷ������
	BOOL        m_bShowJp;	  // ��ʾ����
	BOOL		m_bShowInfo;  // ��ʾ��Ʒ��Ѷ
	CefRefPtr<ClientHandler> m_handler;
	CString		m_strUrl;
	// ����

	listNewsInfo m_aOrgAdvs;
	listNewsInfo m_aInfo; // ��Ʒ��Ѷ
	listNewsInfo m_aJP;   // ����
	struct T_OrgCommRect : public CRect
	{
		int32 iOrgIndex;		// ��Ӧ�Ĺ��index
	};
	CArray<T_OrgCommRect, const T_OrgCommRect &> m_aOrgAdvRects;		// ÿ���������
protected:
	//{{AFX_MSG(CCommentStatusBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point32);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point32);
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point32);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenu ( UINT nID );
	//afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseHover(WPARAM nFlags, LPARAM point);
	afx_msg LRESULT OnMouseLeave(WPARAM , LPARAM );
	afx_msg	void OnCefAfterCreated();

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMENTSTATUSBAR_H__)

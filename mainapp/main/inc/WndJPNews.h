#pragma once

#include "client_handler.h"
// CWndJPNews

class CWndJPNews : public CWnd
{
	DECLARE_DYNAMIC(CWndJPNews)

public:
	CWndJPNews();
	virtual ~CWndJPNews();

	void SetCefBkColor(Color clrBk);		// ÉèÖÃä¯ÀÀÆ÷±³¾°ÑÕÉ«

	//
	CString m_strUrl;
	CefRefPtr<ClientHandler> m_handler;
	Color   m_clrCefBk;		// ä¯ÀÀÆ÷±³¾°ÑÕÉ«

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
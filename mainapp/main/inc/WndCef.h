#pragma once

#include "client_handler.h"
// CWndCef

class CWndCef : public CWnd
{
	DECLARE_DYNAMIC(CWndCef)

public:
	CWndCef();
	virtual ~CWndCef();

	BOOL CreateWndCef(UINT nID,CWnd* pParentWnd,const RECT& rect, const CString& strUrl = L"");
	void SetUrl(const CString& strUrl, const CString& StrParam=_T(""));
	void OpenUrl(const CString& strUrl);
	void ReLoadCfmUrl(bool32 bActive);		// 版面隐藏和显示的时候改变网址
	void ReLoad();
	void GoBack();
	void GoForward();
    void SetNeedReloadURL(bool bNeed) { m_bNeedReloadURL = bNeed; } 
	void SetCefBkColor(Color clrBk);		// 设置浏览器背景颜色


	void SetWebFocusStatus(bool32 bFocusStatus);
	void TransferFunAndRefreshWeb(string strTransData);

	void FrameExecuteJavaScript(char* pstrFun);
private:
	CefRefPtr<ClientHandler> m_handler;
	CString m_strUrl;
    bool  m_bNeedReloadURL;
	Color m_clrCefBk;		// 浏览器背景颜色

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	void OnCefAfterCreated();
	afx_msg void OnDestroy();
};
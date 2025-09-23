#ifndef _IO_VIEW_F10_H_
#define _IO_VIEW_F10_H_

#include "IoViewBase.h"
#include "webbrowser2.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewF10
class CIoViewF10 : public CIoViewBase
{
	// Construction
public:
	CIoViewF10();
	virtual ~CIoViewF10();
	
	DECLARE_DYNCREATE(CIoViewF10)
		
		// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	// from CIoViewBase
public:
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	
	// from CIoViewBase
public:	
	virtual void    SetChildFrameTitle();
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	
	// from CView	
protected:
	virtual void	Draw();
	
	////////////////////////////////////////////////////////////
	//
protected:
	CWebBrowser2	m_WebBrowser2;
	
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewF10)
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewF10)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_

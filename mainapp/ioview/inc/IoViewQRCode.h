#ifndef _IO_VIEW_QRCODE_H_
#define _IO_VIEW_QRCODE_H_

#include "IoViewBase.h"

// CIoViewQRCode

class CIoViewQRCode : public CIoViewBase
{
public:
	CIoViewQRCode();
	virtual ~CIoViewQRCode();

	DECLARE_DYNCREATE(CIoViewQRCode)
	
	// from CIoViewBase:
public:
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML();
	virtual void	SetChildFrameTitle();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	
	virtual void	OnVDataForceUpdate(){}
	
	// 通知视图改变关注的商品
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}

private:
	Image	*m_pImage;
	CString  m_StrAdvsUrl;
public:
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewQRCode)
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewQRCode)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_QRCODE_H_



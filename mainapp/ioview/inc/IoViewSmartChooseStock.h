#ifndef _IO_VIEW_SMART_CHOOSE_STOCK_H_
#define _IO_VIEW_SMART_CHOOSE_STOCK_H_

#include "IoViewBase.h"
#include "IoViewReportSelect.h"
#include "NewDateTimeCtrl.h"

// CIoViewSmartChooseStock

class CIoViewSmartChooseStock : public CIoViewBase
{
public:
	CIoViewSmartChooseStock();
	virtual ~CIoViewSmartChooseStock();

	DECLARE_DYNCREATE(CIoViewSmartChooseStock)
	
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
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) {};

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}

private:
	void			RecalcLayout();

private:
	CIoViewReportSelect  m_ReportResult;                 // 选股结果
	CIoViewReportSelect  m_ReportFlilterResult;          // 筛选后的选股结果
	CNewDateTimeCtrl     m_TimePicker;                   // 查看历史结果的日期控件

public:
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewSmartChooseStock)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewQRCode)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_QRCODE_H_



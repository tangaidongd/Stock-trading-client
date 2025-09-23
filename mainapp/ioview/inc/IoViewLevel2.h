#ifndef _IO_VIEW_Level2_H_
#define _IO_VIEW_Level2_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "MerchManager.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewLevel2

#define SHOW_INFO_COUNTS    40
class CIoViewLevel2 : public CIoViewBase
{
public:
	enum E_ShowType
	{
		ESTBrokerCode = 0,
		ESTBrokerName,
		ESTCount
	};
public:
	CIoViewLevel2();
	virtual ~CIoViewLevel2();
	
	DECLARE_DYNCREATE(CIoViewLevel2)
		
		// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:	
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	void			SetShowType(CIoViewLevel2::E_ShowType eShowType);
	CIoViewLevel2::E_ShowType GetShowType();
	void			ChangeContentRealTime(CIoViewLevel2::E_ShowType e_ShowSignal);

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimeLevel2Update(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void    OnIoViewFontChanged();

// from CView	
protected:
	virtual void	Draw();
	void			SetRowHeightAccordingFont();								// 根据字体大小,设置行高

private:
	bool32		    CreateTable(E_ReportType eMerchKind);

protected:
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CGridCtrlSys	m_GridCtrl;
	CGridCtrlSys	m_GridCtrlBuy;
	CGridCtrlSys	m_GridCtrlSell;
	CImageList		m_ImageList;	
	CStaticSD		m_CtrlTitleChina;
	CStaticSD		m_CtrlTitleHongKongleft;
	CStaticSD		m_CtrlTitleHongKongright;
	CStaticSD		m_CtrlTitleHongKongSellleft;
	CStaticSD		m_CtrlTitleHongKongSellright;
	
	bool32          m_bChina;
	bool32          m_bHongKong;
	bool32			m_bHoerzShow;
	E_ShowType		m_eShowType;
	E_ReportType	m_eMerchKind;
	CArray<CLevel2Detail,CLevel2Detail> m_xxxBuyLevel2ListBackUp;
	CArray<CLevel2Detail,CLevel2Detail> m_yyySellLevelListBackUp;

	int32			m_iTitleHeight;
	bool32			m_bTracking;	
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewLevel2)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewLevel2)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_

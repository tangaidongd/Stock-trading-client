#ifndef _IO_VIEW_DK_MONEY_H_
#define _IO_VIEW_DK_MONEY_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CIoViewCapitalFlow

class CIoViewDKMoney : public CIoViewBase
{
// Construction
public:
	CIoViewDKMoney();
	virtual ~CIoViewDKMoney();

	DECLARE_DYNCREATE(CIoViewDKMoney)

	
	// from CControlBase
public:
	//virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData();
	virtual void	RequestData(){};
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

	virtual bool32	IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType);
	

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();


	virtual	bool32	IsPluginDataView() { return true; }
	virtual void    OnVDataPluginResp(const CMmiCommBase *pResp);

	void			OnDKMoneyResp( const CMmiRespDKMoney *pResp );
	void			OnTickExHistoryResp(const CMmiRespHistoryTickEx *pResp);
private:


private:
	void			DrawPillar(CDC &dc, const CRect &rcItem, float fValue, float fPercentage, CRect rcPercentPos,COLORREF clrStart, COLORREF clrEnd);					//画柱行
	void			DrawBottomText(CDC &dc, const CRect &rcItem, float fRise, float fFall, COLORREF clrRise, COLORREF clrFall);	// 画底部文字

	void			MemsetDKMoneyData();

	CRect			m_RectWindows;
	int32			m_iDataServiceType;

	CRect			m_RectTitle;		// 标题

	CRect			m_RectText;			// 下部分文字显示
	CRect			m_RectRedPillar;	// 红柱子
	CRect			m_RectGreenPillar;	// 绿柱子
	CRect			m_rcPercentPosRed;	// 红柱子百分比
	CRect			m_rcPercentPosGreen;	// 绿柱子百分比
	CRect			m_RectImage;		// 推荐策略

	
	CRect			m_RectButton;		// 一日 五日
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;

	CMmiRespDKMoney  m_RespDKMoney;


	CFont			m_Font14;			// 14高度宋体
	CFont			m_Font12;			// 12高度宋体




	
////////////////////////////////////////////////////////////
//
protected:
		

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewCapitalFlow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewCapitalFlow)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CAPITALFLOW_H_

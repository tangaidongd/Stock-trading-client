#ifndef _IOVIEW_FENJIABIAO_H_
#define _IOVIEW_FENJIABIAO_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlNormal.h"
#include "MerchManager.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewFenJiaBiao
class CIoViewFenJiaBiao : public CIoViewBase
{
public:
	CIoViewFenJiaBiao();
	virtual ~CIoViewFenJiaBiao();

	DECLARE_DYNCREATE(CIoViewFenJiaBiao)
	
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
	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	virtual void	RequestViewData();

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

public:
	void			SetHistoryFlag(bool32 bHistory) { m_bHistoryTimeSale = bHistory; }
	void			SetHistoryTime(const CGmtTime &TimeAttend);

	bool32			GetCurrentPerfectWidth(OUT int32 &iWidth);

private:
	void			RecalcLayout();

	int32			RecalcTableRowColumn();	// 重计算表格行列, 返回多个基本列(多个4列数据)

	void			DeleteTableContent(bool32 bDelRealData = false);			// 清空数据
	void			ClearShowData();	// 仅清除显示数据
	void			CalcTableData();				// 重计算所有的数据
	
	void			UpdateTableTickData(const CTick &tick);	// 更新该tick数据

	bool32			IsInMultiColumnMode();		// 当前是否处在多列模式

	//	
	void			RequestTimeSaleData(CMmiReqMerchTimeSales &req);

	bool32			DoShowFullSreen();
///////////////////////////////////////////////////////////////////////
//
protected:
	CXScrollBar				m_XSBVert;
	CGridCtrlNormalSys		m_GridCtrl;	// 虚表格

	CRect					m_RectTitle;	// 多列状态下绘制标题

private:
	CTrendTradingDayInfo	m_TrendTradingDayInfo;	// 需要昨收价来计算

	// 历史分价表显示??
	bool32					m_bHistoryTimeSale;
	CGmtTime				m_TimeHistory;

	CArray<CTick, CTick>	m_aTicksShow;		// 原始数据

	struct T_TickStatistic{
		float m_fPrice;			// 价格
		float m_fVolTotal;		// 该价格对应 成交量总 - 股票与期货商品
		float m_fAmountTotal;	// 该价格对应 成交额总 - 指数显示
		float m_fVolBuy;		// 成交量 买入			- 计算买入比例，指数不显示
		float m_fVolSell;		// 成交量 卖出
		float m_fAmountBuy;		// 买入额
		float m_fAmountSell;	// 卖出额

		E_SysColor	m_eColor;	// 附加的颜色数据，避免每次判断
	};

	CArray<T_TickStatistic, const T_TickStatistic &>	m_aPricesSort;			// 价格排序显示数据
	float					m_fVolTotal;			// 总成交量
	float					m_fAmountTotal;			// 总成交额
	float					m_fVolMax;				// 成交最大值
	float					m_fAmountMax;			// 成交额最大值

	int32					m_iPreferWidth;			// 一个基本列(4列)所需要的大小
	int32					m_iDecSave;				// 保存的小数点
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewFenJiaBiao)
	//}}AFX_VIRTUAL
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewFenJiaBiao)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_IOVIEW_FENJIABIAO_H_
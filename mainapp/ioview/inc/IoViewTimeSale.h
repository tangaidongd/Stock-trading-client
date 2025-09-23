#ifndef _IO_VIEW_TIMESALE_H_
#define _IO_VIEW_TIMESALE_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "MerchManager.h"
#include "ReportScheme.h"


class CShowTick : public CTick
{
public:
	CShowTick();
	~CShowTick()	{}

public:
	const CShowTick& operator=(const CTick &Tick);
	void			Calculate();

public:
	CMsTime			m_TimePrev;					// 上一笔的时间
	float			m_fHoldTotalPrev;			// 上一笔持仓
	float			m_fPricePre;				// 上一笔的价格
	CString			m_StrProperty;				// 性质
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSale

class CIoViewTimeSale : public CIoViewBase
{
public:
	enum E_TimeSaleHeadType
	{
		ETHTSimple = 0,		// 简单
		ETHTFuture,			// 期货
		// 
		ETHTCount
	};

public:
	CIoViewTimeSale();
	virtual ~CIoViewTimeSale();

	DECLARE_DYNCREATE(CIoViewTimeSale)
	
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
	virtual void	OnIoViewFontChanged();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual void    DoShowStdPopupMenu();
	// from CIoViewBase
public:
	virtual void	RequestViewData();

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

public:
	static E_TimeSaleHeadType GetMerchTimeSaleHeadType(E_ReportType eReportType);

	void			SetHistoryFlag(bool32 bHistory) { m_bHistoryTimeSale = bHistory; }
	void			SetHistoryTime(const CGmtTime &TimeAttend);

	bool32			GetCurrentPerfectWidth(OUT int32 &iWidth);

private:
	void			DeleteTableContent();
	void			SetTableData();
	void			SetRowValue(const CShowTick &Tick, int32 iRowIndex, int32 iColUintIndex, COLORREF clrRise, COLORREF clrFall, COLORREF clrKeep, COLORREF clrVolume);
	void			SetRowHeightAccordingFont();
	void			SetTimeGridHead(E_TimeSaleHeadType eTimeSaleHeadType);
	void			SetActiveCell();

	//	
	void			SetShowType(bool32 bShowFull);						// 改变显示的模式,正常显示或者最大化显示
	void			UpDateFullGridNewestData();							// 在最大化显示的时候,更新最新价
	void			CalcFullGridParams();								// 计算最大化显示时候的相关参数
	bool32			BeValidFullGridIndex();								// 判断索引是否合法

	void			RequestTimeSaleData(CMmiReqMerchTimeSales &req);
	void			OnPageScroll(int32 iScrollPage);					// 滚动页面，调整显示参数，适当申请数据 iScrollPage = INT_MAX 滚动到页尾, INT_MIN 页首，+1 下一页, -1 上一页
	int32			CalcMaxVisibleTimeSaleCount();						// 计算最大可视的分笔成交数据量

	bool32			NoShowNewvol();	// 是否显示“现手”
///////////////////////////////////////////////////////////////////////
//
protected:
	CXScrollBar				m_XSBVert;
	CXScrollBar				m_XSBHorz;
	CGridCtrlNormalSys		m_GridCtrl;
	CImageList				m_ImageList;	

private:
	int32					m_iSaveDec;
	CTrendTradingDayInfo	m_TrendTradingDayInfo;	// 

	// 历史分笔显示
	bool32					m_bHistoryTimeSale;
	CGmtTime				m_TimeHistory;

	CArray<CTick, CTick>	m_aTicksShow;
	E_TimeSaleHeadType		m_eTimeSaleHeadType;	

	// 
	bool32					m_bShowFull;			// 是否按照最大化规则显示 (只要视图宽度大于 m_iPerfectWidth 的两倍就这么显示)
	bool32					m_bShowNewestFull;		// 最大化的时候,是否显示最新数据
	bool32					m_bRequestedTodayFirstData;	  // 已经申请过今天最新的数据，在UpdateMainTimeSale中赋值
	CGridCtrlNormalSys		m_GridCtrlFull;			// 最大化的表格
	
	int32					m_iPerfectWidth;		// 最大化时,一个正好显示的列宽	
	int32					m_iGridFullRow;			// 最大化时显示的行数
	int32					m_iGridFullCol;			// 最大化时显示的列数
	int32					m_iGridFullColUints;	// 最大化时显示多少个单元列
	int32					m_iDataFullNums;		// 最大化时每一页的数据个数
	
	int32					m_iFullBeginIndex;		// 最大化时最左上角(第一个)的数据的索引值.
	int32					m_iFullEndIndex;		// 最大化时最左上角(第一个)的数据的索引值.

	bool32					m_bSetTimer;			// 是否设置了定时器
	bool32					m_bMerchChange;			// 商品切换
	CGmtTime				m_TimePre;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSale)
	//}}AFX_VIRTUAL
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSale)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExportData();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_
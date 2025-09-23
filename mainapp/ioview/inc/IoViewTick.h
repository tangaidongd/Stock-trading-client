#ifndef _IO_VIEW_TICK_H_
#define _IO_VIEW_TICK_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "TrendIndex.h"

class CMPIChildFrame;
class CIoViewTick;
class CDlgTrendIndexstatistics;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTick

class CIoViewTick : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewTick();
	virtual ~CIoViewTick();
	
	DECLARE_DYNCREATE(CIoViewTick)

	// from CControlBase
public:
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();	
// from CIoViewBase
public:	
	virtual void	SetChildFrameTitle();

	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);
	virtual void	OnIoViewActive();

	virtual void	OnKeyHome();
	virtual void	OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl();
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl();
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRight();
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	
	// 
	virtual void	ClearLocalData(bool32 bClearAll = true);

	// 获取X Slider text
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);  
public:
	////////////////////////////////////////////////////////////////////////////////////
	// CIoViewBase事件
	// 通知视图改变关注的商品
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	// 叠加视图相关
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);
	// 通知数据更新
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );
	/////////////////////////////////////////////////////////////////////////////////////
	// CRegionViewParam事件
	virtual bool32  OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32  OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );

	//Curve被删除,导致所绘制的Node被删除,应用层有必要清除Node的源数据.
	virtual void	OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes );
	virtual void	OnPickNode( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag );
	virtual void	OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y );
	virtual void	OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	virtual void	OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	virtual void	OnRegionIndexBtn (CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID );
	virtual void	OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart);
	virtual void	OnRegionCurvesNumChanged ( CChartRegion* pRegion, int32 iNum );
	virtual void	OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow);
	virtual void	OnCrossNoData (CString StrTime,bool32 bShow = true);
	virtual CString OnTime2String ( CGmtTime& Time );
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false);

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );

	virtual void	OnSliderId ( int32& id, int32 iJump );
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual	void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd);

public:
	// 成本堆积
	void			DrawNcp(IN CMemDCEx* pDC);
	
	// 历史分时相关
	void			GetCrossTick(OUT CTick & TickIn);

	//
	bool32			IsShowNewestTick(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList);	// 判断当前是否显示最新一条K线, 如果不是， 则传出当前显示的时间范围
	bool32			UpdateMainMerchTick(T_MerchNodeUserData &MerchNodeUserData);
	bool32			UpdateSubMerchTick(T_MerchNodeUserData &MerchNodeUserData);

	bool32			UpdateMainMerchTick2(T_MerchNodeUserData &MerchNodeUserData);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// ...fangz0729 十字光标的状态和位置应该K 线保存.不要放到ChartRegion 中.
	void			SetCrossTickIndex(int32 iPos)	{ m_iCurCrossTickIndex = iPos; }
	int32			GetCrossTickIndex()			{ return m_iCurCrossTickIndex; }

	void			SetRequestNodeFlag(bool32 bTrue){ m_bRequestNodesByRectView = bTrue;}

	// 多日tick
	void			SetTickMulitDay(int32 iMultiDay, bool32 bReqData = true);		// 设置多日分时
	bool32			GetTickMultiDayTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 获取多日分时的时间段
	bool32			IsTickZoomed();	// 是否被缩放了
	void			CancelZoom();	// 取消缩放
	bool32			GetZoomedTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 获取缩放状态下的时间段
	bool32			GetShowTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 根据当前状态获取显示的时间段
	
	void			FillMainVirtualKLineData(const CArray<CKLine, CKLine> &aSrc, OUT CArray<CKLine, CKLine> &aDst, OUT CArray<DWORD, DWORD> &aDstFlag);		// 填充主商品的虚假数据
	void			FillMainVirtualShowData();		// 填充主商品的虚假数据

	void			CalcKLineAvgValue(INOUT CArray<CKLine, CKLine> &aKlines);
	
	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CTick& Tick,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& rect);

	void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);

private:
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	
	void			UpdateTickStyle();
	bool32			OnZoomOut();
	bool32			OnZoomIn();

	void			RequestViewData();			// 判断是否有必要请求更多数据， 不一定请求

	void			RequestSequenceTickStart();	// 开始一段一段请求tick数据
	void			RequestSequenceTickNext();	// 继续一段一段请求tick数据

	void			UpdateTickMultiDayTitle();	// 更新tick的多日标题文字 
	float			GetTickPrevClose();			// 获取tick图的昨收价
	bool32			CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	void			FillAppendMainXPosNode(const CArray<CNodeData, CNodeData&> &aSrc, OUT CArray<CNodeData, CNodeData&> &aNewNodes);	// 根据时间段信息，填充末尾的占位节点
	bool32			FindPosInAllTimeIds(int32 iTimerId, OUT int32 &iPos);

private:
	bool32			m_bRequestNodesByRectView;	// 新建一个K 线的时候,根据当前显示区域的大小请求K 线.避免显示的不协调(过疏或过密)
	CTick			m_TickCrossNow;

	int32			m_iCurCrossTickIndex;		// 当前屏幕上十字光标显示位置
	int32			m_iNodeCountPerScreen;		// 当请屏幕上一屏显示K线条数

	int32			m_iTickMultiDay;				// 多日要求数
	// 在每次请求数据时，重新计算该数组
	CArray<CMarketIOCTimeInfo, const CMarketIOCTimeInfo &>	m_aTickMultiDayIOCTimes; // 每天的开收盘，第一天为当天，往上为前n天
	CGmtTime		m_TimeZoomStart;				// 缩放的开始时间, 范围总在选定的交易日的时间段内
	CGmtTime		m_TimeZoomEnd;					// 缩放的结束时间
	CArray<int32, int32>	m_aAllXTimeIds;			// 所有x轴(包含当前没显示的但是在限定的时间内)的时间节点

	CMmiReqMerchTimeSales	m_MmiReqTimeSales;			// 请求历史分笔

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTick)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTick)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMenu ( UINT nID );
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMySettingChanged();
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TICK_H_

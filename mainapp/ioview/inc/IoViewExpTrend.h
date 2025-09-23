#ifndef _IO_VIEW_EXP_TREND_H_
#define _IO_VIEW_EXP_TREND_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "IoViewShare.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"


class CMerch;
class CChartRegion;
class CIoViewExpTrend;
class CMPIChildFrame;
class CChartDrawer;

/////////////////////////////////////////////////////////////////////////////
// CIoViewExpTrend
const int32 KExpTrendHeartBeatTimerId = 10011;

class CIoViewExpTrend : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewExpTrend();
	virtual ~CIoViewExpTrend();
	
	DECLARE_DYNCREATE(CIoViewExpTrend)

/////////////////////////////////////////////////////
// from CControlBase
public:
	virtual	CString GetDefaultXML();	
	virtual void	DoFromXml();
	virtual void	SetChildFrameTitle();

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);			// 通知视图改变关注的商品
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);		// 叠加视图相关
	virtual void	OnVDataForceUpdate();															// 通知数据更新	
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);	
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);	
	virtual void	RequestViewData();
	
	virtual void	OnKeyHome();
	virtual void	OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl(){};
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl(){};
	virtual void	OnKeyRight();
	virtual void	OnKeyUp(){};
	virtual void	OnKeyDown(){};
	virtual void	OnIoViewActive();
	virtual void	UpdateAxisSize(bool32 bPreDraw = true);

	// 获取副图的标题栏按钮:
	virtual int32	GetSubRegionTitleButtonNums();
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex);

	//
	virtual void	ClearLocalData();
	
	// from CRegionViewParam
public:
	virtual void	OnRequestPreNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest );
	virtual void	OnRequestNextNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest );
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
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual CString OnTime2String ( CGmtTime& Time );
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false );

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );
	virtual void	OnSliderId ( int32& id, int32 iJump );//没用

//////////////////////////////////////////////////////////////////////////////
// 
public:
	CGmtTime		GetRequestNodeTime(IN CMerch *pMerch);

	bool32			ServerTimeInOpenCloseTime ( CMerch* pMerch );

	void			DrawAlarmMerchLine();

	void			GetDrawCrossInfoNoData(int32 &iOpenTime,int32 &iCloseTime);		// 无数据时候,画X 轴和详情窗口时用到

private:
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);

	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& kLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& rect);
	
	// 历史分时
public:
	void			SetHistoryTime(CGmtTime TimeHistory);
	void			SetHistoryFlag(bool32 bHistroy)	{ m_bHistoryTrend = bHistroy; }
	void			OnCtrlLeftRight(bool32 bLeft) ;
	CString			GetHistroyTitle(CGmtTime Time);


public:
	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);			

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	
	void			ValueYAsNodata(float fYMin, float fYMax, const CRect& rect,int32 iSkip, float fy,int32& iValue);

	// 新增画持仓线
	void			DrawHoldLine(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, IN CChartRegion* pRegion);

private:
	// 得到商品对应的大盘商品
	CMerch*			GetMerchAccordExpMerch(CMerch* pMerch);

public:
	CChartDrawer*		m_pTrendDrawer;
	CArray<CGmtTime,CGmtTime>	m_TimesCombin;
	CArray<int32,int32>	m_PosCombin;
	
	CGmtTime			m_CornerTime;

	CNodeSequence		*m_pHoldNodes;		// 持仓线数据
	
	//
	CMerch*				m_pMerchReal;
private:
	bool32				m_bHistoryTrend;
	CGmtTime			m_TimeHistory;

private:
	int32				m_iOpenTimeForNoData;
	int32				m_iCloseTimeForNoData;

	float				m_fMinHold;
	float				m_fMaxHold;
	
	// t..fangz1010
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkMain;	// 主图 y轴分割线备份,用于切换时或者无商品数据时绘图
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkSub;	// 副图 y轴分割线备份,用于切换时或者无商品数据时绘图

	

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewExpTrend)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewExpTrend)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenu ( UINT nID );
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_EXP_TREND_H_

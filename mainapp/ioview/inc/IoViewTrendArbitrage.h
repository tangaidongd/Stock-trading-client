#ifndef _IO_VIEW_TRENDARBITRAGE_H_
#define _IO_VIEW_TRENDARBITRAGE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "AxisYObject.h"

#include "ArbitrageManage.h"

class CMPIChildFrame;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTrendArbitrage

// 实现套利接口类~~
class CIoViewTrendArbitrage : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify, public CArbitrageNotify
{
// Construction
public:
	CIoViewTrendArbitrage();
	virtual ~CIoViewTrendArbitrage();
	
	DECLARE_DYNCREATE(CIoViewTrendArbitrage)

	// from CControlBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums(){};

	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();
	virtual CString CIoViewTrendArbitrage::ToXmlEleInChild();
	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return  EIVT_MultiMerch; }

	virtual	bool32	OnSpecialEsc();
// from CIoViewBase
public:	
	virtual void	SetChildFrameTitle();
	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);
	virtual void	OnIoViewActive();	
	virtual void 	OnIoViewDeactive();

	virtual void    OnKeyHome();
	virtual void    OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl();
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl();
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRight();
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	virtual void	OnKeySpace();

	// 获取副图的标题栏按钮:
	virtual int32   GetSubRegionTitleButtonNums();
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex);

	// 视图优化 K线获取X轴文字提示
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);

	// 
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd){};

	//
	virtual	void	ClearLocalData(bool32 bClearAll = true); 
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd){};

public:
	////////////////////////////////////////////////////////////////////////////////////
	// CIoViewBase事件
	// 通知视图改变关注的商品
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	
	// 叠加视图相关
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);

	// 通知数据更新
	virtual void	RequestViewData();
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	
	void			OnWeightTypeChange();
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch){};	// 信息地雷更新
	/////////////////////////////////////////////////////////////////////////////////////
	// CRegionViewParam事件
	virtual bool32  OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32  OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32	GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle);

	//Curve被删除,导致所绘制的Node被删除,应用层有必要清除Node的源数据.
	virtual void	OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes );
	virtual void	OnPickChart(CChartRegion *pRegion, int32 x, int32 y, int32 iFlag);
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
	virtual CString OnFloat2String ( float fValue,bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false );

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );

	virtual void	OnSliderId ( int32& id, int32 iJump );
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual	void	OnChartDBClick(CPoint ptClick, int32 iNodePos);	// 双击事件
	virtual bool32	OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix);

	virtual	void	OnIoViewColorChanged();

public:
	// 增加套利
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage);
	
	// 删除套利
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// 修改套利
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);


	// 设置当前套利
	bool32		SetArbitrage(IN const CArbitrage &arb);
	bool32		GetArbitrage(OUT CArbitrage &arb);

	// 计算套利
	void		CalcArbitrageAsyn();
	void		CancelCalcArbAsyn();
	bool32		CalcArbitrage();

	bool32		InitMainUserData();	// 初始化mainuserdata数据，所有关注的商品并不关心在userdata中的位置，但是main数据保存了合成后的差价数据
	T_MerchNodeUserData	*GetMainData(bool32 bCreateIfNotExist=false);	// main中存放的商品指针无意义
	T_MerchNodeUserData *GetMerchData(CMerch *pMerch, bool32 bCreateIfNotExist=false);	// 获取商品相应的数据
	bool32		ResetMerchUserData(T_MerchNodeUserData *pMerchData);	// 重置商品相关的数据

	void		TestSetArb(bool32 bAsk=false);

	void		UpdateMerchDataTradeTime(T_MerchNodeUserData *pMerchData);	// 更新交易时间
	void		UpdateAllMerchDataTradeTimes();
	void		CombineMainDataTradeTime();			// 合并main的交易时间，取相关商品的交集

	bool32			FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);			
	bool32			FillSubMerchTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);
	float			GetMerchTrendPrevClose(T_MerchNodeUserData &MerchNodeUserData);

	void		OnF5();	// 切换
public:
	 
	// 历史分时相关
	E_NodeTimeInterval GetTimeInterval();
	void			GetCrossKLine(OUT CKLine & KLineIn);

	//
	bool32			GetTrendOpenCloseTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 获取分时图的开收时间[开，收]

	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate = false);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate=true);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& Rect);

	void			AdjustCtrlFloatContent();	// 调整详情窗口内容

private:
	
	bool32			LoadAllIndex(CNewMenu* pMenu);
	int32			LoadOftenIndex(CNewMenu* pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	
	
	bool32			OnZoomOut(){ return false; };
	bool32			OnZoomIn(){ return false; };

public:

public:
	
private:

private:	
	
	CKLine			m_KLineCrossNow;
	CStringArray	m_aAllFormulaNames;			// 所有指标: 每个ID 对应数组中一个名字

	// t..fangz1010
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkMain;	// 主图 y轴分割线备份,用于切换时或者无商品数据时绘图
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkSub;	// 副图 y轴分割线备份,用于切换时或者无商品数据时绘图
	
	//////////////////////////////////////////////////////////////////////////
	

	// 套利 有套利数据更新，套利公式变更，套利删除(!)，当前所关注的套利变更等等接口
	CArbitrage		m_Arbitrage;		// 当前所关注的套利模型，只能关注一个~~
	// 套利所有数据以cmp存放cmp商品的位置存放 T_MerchUserData只用来存放数据，MainUserData用来存放实际的合成数据，与商品无关

private:
	
private:

public:
	
	//
	
	//
	bool32				AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);		// 增加该指标，依据主图、副图采取不同的行动

	void				RemoveShowIndex(const CString &StrIndexName);	// 删除指标，依据主图 副 采取不同
	void				DelCurrentIndex();
	void				ChangeIndexToMainRegion(const CString &StrIndexName);				// 变更主图指标类型
	void				ReplaceIndex(const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex = false, bool32 bChangeStock = false);	// 使用新index替换指定index
	// 改变指定位置的副图指标, 如果该位置不存在，则添加一个新的副图，并将该指标加入,
	//  bAddSameIfExist 指定所有副图中即使存在该指标，仍按上述规则添加，否则取消此次添加
	//  iSubRegionIndex -1 添加新的副图，任何有效副图index为指定位置
	void				AddIndexToSubRegion(const CString &StrIndexName, bool32 bAddSameIfExist = true, int32 iSubRegionIndex = -1); 
	void				AddIndexToSubRegion(const CString &StrIndexName, CChartRegion *pSubRegion);
	void				ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false); // 清空指定region的指标
	void				GetCurrentIndexNameArray(OUT CStringArray &aIndexNames); // 获取当前所有主副指标名称, 指标名称都是唯一的，so

	// 得到某个region 允许的常用指标数组
	void				GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame = true);
	void				ChangeToNextIndex(bool32 bPre);

	// 简单叠加商品
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void				RemoveCmpMerch(CMerch *pMerch){};
	virtual void				RemoveAllCmpMerch(){};
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// 显示区间变更
	// 主图商品的显示区域变更，集中到此函数(UpdateMainMerch时机以外的)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList);

	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);
	float				GetTrendPrevClose();		// 更改昨收价取法

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// 获取指定region下的指标线

	int32				CompareKLinesChange(const CKLine *pKLineBef, int32 iBefCount, const CKLine *pKLineAft, int32 iAftCount);		// 判断两端K 线的异同. 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化

public:

public:

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTrendArbitrage)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTrendArbitrage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TRENDARBITRAGE_H_

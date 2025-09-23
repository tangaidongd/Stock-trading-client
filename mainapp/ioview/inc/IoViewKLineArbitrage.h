#ifndef _IO_VIEW_KLINEARBITRAGE_H_
#define _IO_VIEW_KLINEARBITRAGE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "AxisYObject.h"
#include "IoViewKLine.h"
#include "ArbitrageManage.h"

class CMPIChildFrame;

enum E_ArbitrageChartShowType
{
	EACST_CloseDiff = 0,	// 收盘价价差图：用收盘价价差做线性图
	EACST_AvgDiff,			// 均价价差: 均价价差做线性图
	EACST_ArbKLine,			// 套利K线: 价差开，收，均画图 收>开=阳 均为中间圆点
	EACST_TowLegCmp,		// 两腿叠加图: 多-空腿同时显示的图

	EACST_Count
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewKLineArbitrage

// 实现套利接口类~~
class CIoViewKLineArbitrage : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify, public CArbitrageNotify
{
	// Construction
public:
	CIoViewKLineArbitrage();
	virtual ~CIoViewKLineArbitrage();

	DECLARE_DYNCREATE(CIoViewKLineArbitrage)

	// from CControlBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();

	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();

	virtual CString	ToXmlEleInChild();		// 子类保存xml元素

	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return  EIVT_MultiMerch; }

	virtual bool32	OnSpecialEsc();
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
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd);

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
	void	RequestViewData(bool32 bForceReq=false);
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
	virtual	void	KLineCycleChange(UINT nID);

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

	void		SetArbShowType(E_ArbitrageChartShowType eType);	// 变更显示类型
	void		AdjustArbCurveByShowType();						// 调整线的显示类型

	void		TestSetArb(bool32 bAsk=false);

	void		OnF5();	// 切换

	int32		GetSubMerchNeedDataCount();			// 商品所需的K线数量
	void		ResetNodeScreenCount();				// 重置屏幕点数
public:

	// 历史分时相关
	E_NodeTimeInterval GetTimeInterval();
	void			GetCrossKLine(OUT CKLine & KLineIn);

	//
	bool32			IsShowNewestKLine(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList);	// 判断当前是否显示最新一条K线, 如果不是， 则传出当前显示的时间范围
	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate = false);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate=true);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// ...fangz0729 十字光标的状态和位置应该K 线保存.不要放到ChartRegion 中.
	void			SetCrossKLineIndex(int32 iPos)	{ m_iCurCrossKLineIndex = iPos; }
	int32			GetCrossKLineIndex()			{ return m_iCurCrossKLineIndex; }

	void			SetRequestNodeFlag(bool32 bTrue){ m_bRequestNodesByRectView = bTrue;}

	void			OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue);

	// 除权复权相关
	void			RequestWeightData();

	void			OnKeyF8();

	void            UpdateKelineArbitrage(E_NodeTimeInterval NodeInterval);

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

	void			AdjustCtrlFloatContent();	// 调整详情窗口内容

private:

	bool32			LoadAllIndex(CNewMenu* pMenu);
	int32			LoadOftenIndex(CNewMenu* pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);

	void			UpdateKLineStyle();

	bool32			GetTimeIntervalInfo(IN E_NodeTimeInterval eNodeTimeInterval, OUT E_NodeTimeInterval &eNodeTimeIntervalCompare, OUT E_KLineTypeBase &eKLineTypeCompare, OUT int32 &iScale);
	void			SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval);
	void			ResetTimeInterval( T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare);

	bool32			OnZoomOut();
	bool32			OnZoomIn();

	int32			GetSpaceRightCount();		// 根据是否是显示最新返回4或者0

private:	

	bool32			m_bRequestNodesByRectView;	// 新建一个K 线的时候,根据当前显示区域的大小请求K 线.避免显示的不协调(过疏或过密)
	CKLine			m_KLineCrossNow;
	CStringArray	m_aAllFormulaNames;			// 所有指标: 每个ID 对应数组中一个名字
	int32			m_iCurCrossKLineIndex;		// 当前屏幕上十字光标显示位置
	int32			m_iNodeCountPerScreen;		// 当请屏幕上一屏显示K线条数(这是理论条数, 不是实际个数. 放缩到很小的时候, 没有那么多数据这个值会很实际显示的不符.)
	bool32			m_bForceUpdate;				// 是否强制更新
	//////////////////////////////////////////////////////////////////////////


	// 套利 有套利数据更新，套利公式变更，套利删除(!)，当前所关注的套利变更等等接口
	CArbitrage		m_Arbitrage;		// 当前所关注的套利模型，只能关注一个~~
	// 套利所有数据以cmp存放cmp商品的位置存放 T_MerchUserData只用来存放数据，MainUserData用来存放实际的合成数据，与商品无关

	E_ArbitrageChartShowType		m_eArbitrageShow;	// 套利类型显示

	CChartCurve		*m_pCuveOtherArbLeg;	// 另外一条腿的线

private:
	int32				m_iLastPickSubRegionIndex;	// 最后一个选定的副图

public:
	void				InitialImageResource();

	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// 判断两端K 线的异同. 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化

	//
	void				SetUpdateTime();												// 设置更新时间点	
	void				OnTimerUpdateLatestValuse(UINT uID);							// 更新实时指标值

	// 得到某时间点对应K 线的更新时间
	static bool32		GetKLineUpdateTime(IN CMerch* pMerch, IN CAbsCenterManager* pAbsCenterManager, IN const CGmtTime& TimeNow, IN E_NodeTimeInterval eTimeInterval, IN int32 iMiniuteUser, OUT bool32& bNeedTimer, OUT CGmtTime& TimeToUpdate, OUT bool32& bPassedUpdateTime, IN bool32 bGetKLineFinishTime = false);
	static CString		GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime = false);

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
	virtual bool32		AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void		RemoveCmpMerch(CMerch *pMerch){};
	virtual void		RemoveAllCmpMerch(){};
	virtual void		OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// 显示区间变更
	// 主图商品的显示区域变更，集中到此函数(UpdateMainMerch时机以外的)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd);
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// 获取指定region下的指标线

public:

	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewKLineArbitrage)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewKLineArbitrage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnTest();
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_KLINEARBITRAGE_H_

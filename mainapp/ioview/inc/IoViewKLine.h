#ifndef _IO_VIEW_KLINE_H_
#define _IO_VIEW_KLINE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "TrendIndex.h"
#include "SaneIndex.h"
#include "ExpertTrade.h"
#include "AxisYObject.h"
#include "LandMineStruct.h"
#include "DlgStrategyInfomation.h"

#include <map>
using std::multimap;

class CMPIChildFrame;
class CIoViewKLine;
class CDlgTrendIndexstatistics;
class CDlgUserCycleSet;

/////////////////////////////////////////////////////////////////////////////
// CIoViewKLine

extern const int32 KTimerIdUpdateSaneValues;

extern void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFile,CString StrMerchCode, int32 iMarketId);

// 此处的enum必须与CIndexDrawer中的k线绘制方法一致
enum E_MainKlineDrawStyle
{
	EMKDSNormal = 0,	// 空心线绘制
	EMKDSFill,			// 实心
	EMKDSAmerica,		// 美国线
	EMKDSClose,			// 收盘线
	EMKDSTower,			// 宝塔线
	EMKDSCount,			// 无效
};

int32	MainKlineDrawStyleToChartDrawType(E_MainKlineDrawStyle eStyle); // 主图K线类型切换到实际绘制类型

class CIoViewKLine : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewKLine();
	virtual ~CIoViewKLine();
	
	DECLARE_DYNCREATE(CIoViewKLine)

	// from CControlBase
public:
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();
	virtual BOOL	IsKindOfIoViewKLine(){return TRUE;}
	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();
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
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd);
	
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos);
	virtual bool32  IsAlreadyToShowAdv();
	//

	virtual void	DragMoveKLine(int32 icnt);	// 拖动K线平移

	void			OnDelAllPrompt();

	//
	void			OnAddClrKLine();
	void			OnDelClrKLine();
	
	void			DoAddClrKLine(CFormularContent *pFormularContent);
	void			AddClrKLine(const CString &StrKLineName);

	//
	void			OnAddTrade();
	void			OnDelTrade();
	void			DoAddTrade(CFormularContent *pFormulaTrade);

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
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch);	// 信息地雷更新
	virtual void	OnVDataStrategyData(const char *pszRecvData);	// 量化策略消息
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
	virtual bool32	GetViewRegionRect(OUT LPRECT pRect);
public:
	// 成本堆积
	void			DrawNcp(IN CMemDCEx* pDC);
	 
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

	// 虽然可以覆盖父类CIoViewChart的LoadAllIndex函数，避免以后混乱，重新定义函数LoadAllIndex函数
	bool32			KlineLoadAllIndex(CNewMenu* pMenu);
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
	void			OnMenuCnp();
	int32			LoadOftenIndex(CNewMenu* pMenu);
	int32			LoadJinDunSpecialIndex(CNewMenu *pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	

	void			UpdateKLineStyle();
	
	bool32			GetTimeIntervalInfo(IN E_NodeTimeInterval eNodeTimeInterval, OUT E_NodeTimeInterval &eNodeTimeIntervalCompare, OUT E_KLineTypeBase &eKLineTypeCompare, OUT int32 &iScale);
	void			SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval);
	void			ResetTimeInterval( T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare);
	
	// 视图优化 获取SaneIndex Tip
	bool32			GetSaneNodeTip(OUT CString &StrTip, IN int32 iSaneIndex);

	bool32			OnZoomOut();
	bool32			OnZoomIn();

	int32			GetSpaceRightCount();		// 根据是否是显示最新返回4或者0

	void			OnAQExtDoQueueItem();	//对接量化策略的gateway协议
	void			PackJson(string &strTransDataUtf8);//对接量化策略的json
	void			UnPackJson(string strMerchData);//对接量化策略的json

public:
	bool32			SpecialForZLMM(T_MerchNodeUserData* pData);
	bool32			BeNeedZLMM(T_MerchNodeUserData* pData);

public:
	bool32			GetShowClrKLineFlag();
	bool32			BeSysClrKLine();
	CString			GetClrKLineName();

    void            UpdateKeline(E_NodeTimeInterval NodeInterval);
	
		// 指标是否存在
	bool32          CheckIndexExist(const CString &StrIndexName);
	
private:
	void			UpdateShowClrKLine(T_MerchNodeUserData* pData, int32 iShowPosInFullList, int32 iShowCountInFullList);

private:	
	bool32			m_bNeedCnp;
	bool32			m_bRequestNodesByRectView;	// 新建一个K 线的时候,根据当前显示区域的大小请求K 线.避免显示的不协调(过疏或过密)
	CKLine			m_KLineCrossNow;
	
	int32			m_iCurCrossKLineIndex;		// 当前屏幕上十字光标显示位置
	int32			m_iNodeCountPerScreen;		// 当请屏幕上一屏显示K线条数(这是理论条数, 不是实际个数. 放缩到很小的时候, 没有那么多数据这个值会很实际显示的不符.)
	bool32			m_bForceUpdate;				// 是否强制更新
	//////////////////////////////////////////////////////////////////////////
	// TrendIndex
private:
	bool32				m_bShowTrendIndexNow;	// 当前是不是趋势指标
	bool32				m_bShowSaneIndexNow;	// 当前是不是稳健型指标
	bool32				m_bForceCalcSaneIndex;  // 与UpdateMainKLine中是否强制重算Sane

private:
	CFormularContent*	m_pFormularClrKLine;	// 五彩 K 线指标公式
	T_IndexOutArray*	m_pOutArrayClrKLine;	// 五彩 K 线计算结果

	CFormularContent*	m_pFormularTrade;		// 交易指标公式
	T_IndexOutArray*	m_pOutArrayTrade;		// 交易指标计算结果

	int32				m_iLastPickSubRegionIndex;	// 最后一个选定的副图

	CExpertTrade*		m_pExpertTrade;			// 专家交易提示
	// 筹码分布有关
	CRect				m_RectMainCurve;		// 主图curve的矩形框，发生变化时需要通知筹码分布
	CPriceToAxisYObject m_AxisYCalcObj;

	E_MainKlineDrawStyle m_eKlineDrawStyle;		// 主图K线绘制方法
	//广告dlg 变量
	//bool32              m_bMatchMerCode;        
	//bool32              m_bMatchTimer; 
//	bool32              m_bOwnAdvertise;          //拥有广告dlg的view,在changesize时要重画广告dlg
//	bool32              m_bViewIsvisible;

public:
	void				InitialImageResource();

	bool32				GetShowSaneIndexFlag()		{ return m_bShowSaneIndexNow;  }
	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// 判断两端K 线的异同. 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化
	
	//
	void				SetUpdateTime();												// 设置更新时间点	
	void				OnTimerUpdateLatestValuse(UINT uID);							// 更新实时指标值

	// 得到某时间点对应K 线的更新时间
	static bool32		GetKLineUpdateTime(IN CMerch* pMerch, IN CAbsCenterManager* pAbsCenterManager, IN const CGmtTime& TimeNow, IN E_NodeTimeInterval eTimeInterval, IN int32 iMiniuteUser, OUT bool32& bNeedTimer, OUT CGmtTime& TimeToUpdate, OUT bool32& bPassedUpdateTime, IN bool32 bGetKLineFinishTime = false);
	static CString		GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime = false);

	//
	bool32				GetSaneIndexNode(IN int32 iPos,OUT T_SaneIndexNode& Node);		// 取指标节点
	void				DrawSaneIndex(CMemDCEx* pDC);									// 画稳健型指标线	
	void				ReSetSaneIndex(bool32 bMerchChange);							// 将指标相关的数据清零	
	CString				GetSaneIndexShowName();											// 获取财富金盾指标的名称
	void                ShowJinDunIndex();

	// 除权绘制
	void				DrawWeightInfo(CMemDCEx *pDC);
	bool32				IsWeightDataCanShow(const CWeightData &wData) const;

	// 交易系统绘制
	void				DrawTrade(CMemDCEx* pDC);

	// 专家提示绘制
	void				DrawExpertTradePrompt(CMemDCEx* pDC);

	// 绘制策略信号
	void				DrawStrategySignal(CMemDCEx* pDC);
	bool				CompareTime(long long Time1, long long Time2);

	//
	bool32				AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);		// 增加该指标，依据主图、副图、五彩采取不同的行动
	// 用户投资日记点的绘制
	void				DrawUserNotes(CMemDCEx *pDC);
	void				DrawLandMines(CMemDCEx *pDC);	// 绘制地雷

	//void				AddShowIndex(const CString &StrIndexName);		// 增加该指标，依据主图、副图、五彩采取不同的行动
	void				RemoveShowIndex(const CString &StrIndexName);	// 删除指标，依据主图 副 五彩 采取不同
	void				DelCurrentIndex();
	void				AddCmpIndex();
	void				ChangeIndexToMainRegion(const CString &StrIndexName);				// 变更主图指标类型
	void				ReplaceIndex(const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex = false, bool32 bChangeStock = false, bool32 bDonotAddIfNotSameType=true);	// 使用新index替换指定index
	// 改变指定位置的副图指标, 如果该位置不存在，则添加一个新的副图，并将该指标加入,
	//  bAddSameIfExist 指定所有副图中即使存在该指标，仍按上述规则添加，否则取消此次添加
	//  iSubRegionIndex -1 添加新的副图，任何有效副图index为指定位置
	void				AddIndexToSubRegion(const CString &StrIndexName, bool32 bAddSameIfExist = true, int32 iSubRegionIndex = -1); 
	void				AddIndexToSubRegion(const CString &StrIndexName, CChartRegion *pSubRegion);
	void                AddIndexGroup(CString strIndexGroupName);
	void				ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false, bool32 bDelCmpIndex = false); // 清空指定region的指标
	void				GetCurrentIndexNameArray(OUT CStringArray &aIndexNames); // 获取当前所有主副指标名称, 指标名称都是唯一的，so

	// 得到某个region 允许的常用指标数组
	void				GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame = true);
	void				ChangeToNextIndex(bool32 bPre);

	// 简单叠加商品
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData, bool32 bDrawLine = true);
	virtual void				RemoveCmpMerch(CMerch *pMerch);
	virtual void				RemoveAllCmpMerch();
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// 显示区间变更
	void						AddExpMerch(IN CMerch *pMerch);							// 叠加大盘指数 

	// 主图商品的显示区域变更，集中到此函数(UpdateMainMerch时机以外的)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd);
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);
	void				RequestSingleCmpMerchViewData(CMerch *pMerch);	// 请求单个商品的数据

	// 筹码分布
	// iflag=0 激活通知 1=Y轴变化 2=mousemove wparame=price, lparam=timeid 3=updateMainMerchData
	//			4=坐标轴可能变化了
	void				FireIoViewChouMa(int32 iFlag, WPARAM w, LPARAM l);	
	void				FireIoViewChouMaAxisChange();	// 鼠标移动或者时间轴变化

	E_MainKlineDrawStyle GetMainKlineDrawStyle();		// 获取主图K线绘制类型
	void				SetMainKlineDrawStyle(E_MainKlineDrawStyle eStyle); // 设置主图K线绘制类型
	int32				GetCurChartKlineDrawType(){ return MainKlineDrawStyleToChartDrawType(GetMainKlineDrawStyle()); }
	void				NotifyIoViewKlineDrawStyleChange(E_MainKlineDrawStyle eNewStyle);	// 通知所有k线视图，主图类型变更了
	void				DoIoViewKlineDrawStyleChange(E_MainKlineDrawStyle eNewStyle);		// 响应主图类型的变更

	static	E_MainKlineDrawStyle GetStaticMainKlineDrawStyle(){ return sm_eMainKlineDrawStyle; };

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// 获取指定region下的指标线

public:
	virtual void		OnIntervalTimeChange(const CGmtTime& TimeBegin, const CGmtTime& TimeEnd);
	virtual	void		KLineCycleChange(UINT nID);

	// 获取五彩K线指标公式
	CFormularContent* GetClrFomular() { return m_pFormularClrKLine; }

public:
	friend	class CIoViewDuoGuTongLie;
	friend  class CDlgKLineQuant;
	friend  class CIoViewMultiCycleKline;
	CDlgTrendIndexstatistics*						m_pDlgTrendIndex;					// 统计对话框

	//////////////////////////////////////////////////////////////////////////	
	// SaneIndex
	bool32											m_bNeedCalcSaneLatestValues;		// 是否需要计算最新值
	CGmtTime										m_TimeToUpdateSaneIndex;			// 更新实时数据的时间点
	E_SaneIndexState								m_eSaneIndexState;					// 当前的稳健指标的状态
	T_IndexOutArray*								m_pIndexEMAForSane;					// GGTEMA 指标值序列
	
	CArray<T_SaneIndexNode,T_SaneIndexNode>			m_aSaneIndexValues;					// 稳健指标的值
	CArray<T_SaneNodeTip,T_SaneNodeTip&>			m_aSaneNodeTips;					// Tips		
	CArray<CMerch*, CMerch*>						m_aMerchsHaveCalcLatestValues;		// 已经算过最新值的商品,防止来回切换商品的时候重复多次计算最新值.
	CArray<CKLine, CKLine>							m_aKLineInterval;					// 区间统计的K线
	float	m_fPriceIntervalPreClose;

	mapLandMine										m_mapLandMine;						// 信息地雷数据
	typedef map<CMerch *, CGmtTime>	MerchReqTimeMap;
	MerchReqTimeMap		m_mapMerchLandMineHasReqTimes;				// 商品发出了信息地雷请求的时间, 不管有没有回来，若干时间段不再重发

	typedef multimap<long long, T_StrategyInfo>		StategyInfoMulmap;
	StategyInfoMulmap								m_mulmapStategyInfo;			// 用户策略信号信息
	// 用户策略信号
	typedef multimap<long long, CRect>			    StategyRectMulmap;
	StategyRectMulmap								m_mulmapStategyRects;			 // 用户用户策略信号区域
	bool											m_bReqStrategy;


	// 除权信息
	CArray<CRect, const CRect &>					m_aWeightRects;						// 除权信息显示区域，与商品中除权信息对应

	// 用户投资日记基本区域
	CMap<int32, int32, CRect, const CRect &>		m_mapUserNotesRect;				// 用户投资日记的基本区域
	typedef multimap<CString, CRect>	LandMineRectMap;
	LandMineRectMap									m_mapLandMinesRect;				// 信息地雷区域

	// 图标资源
	void											DrawZLMMIndex(CMemDCEx* pDC);		// 画ZLMM 指标提示
	CArray<uint32, uint32>							m_aZLMMCrossID;						// 主力买卖的金叉点
	bool32											m_bZLMMAlarm;						// 主力买卖指标是否要报警
	Image*											m_pImageZLMM;						// 主力买卖

	static	E_MainKlineDrawStyle		sm_eMainKlineDrawStyle;			// 静态表示当前k线视图中的主图k线类型
	

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewKLine)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewKLine)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnMenuExpertTrade(UINT nID);
	afx_msg void OnWeightMenu(UINT nID);
	afx_msg void OnMenuIntervalStatistic();
	afx_msg void OnTest();
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNotifyChouMa(WPARAM w, LPARAM l);
	afx_msg void OnTimer(UINT nIDEvent);

	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_KLINE_H_

#ifndef _IO_VIEW_TREND_H_
#define _IO_VIEW_TREND_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "IoViewShare.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "LandMineStruct.h"
#include "DlgEconomicTip.h"
#include "PolygonButton.h"
#include <map>
#include <vector>
// 财经日历合并数据时，连续的分钟数
#define CONTINUEATION_MINUTES 10
using std::map;
using std::vector;

class CMerch;
class CChartRegion;
class CIoViewTrend;
class CMPIChildFrame;
class CChartDrawer;

#ifndef _DLL_SAMPLE
#define _DLL_SAMPLE
#endif

/////////////////////////////////////////////////////////////////////////////
// CIoViewTrend
const int32 KTrendHeartBeatTimerId = 10007;

typedef struct _XAxisInfo
{
	CPoint pt;		// 时间刻度的显示位置
	int hour;		// 小时
	int mintues;	// 分钟
	_XAxisInfo()
	{
		hour = mintues = -1;
	}
}T_XAxisInfo;

typedef struct _CircleInfo
{
	CRect rt;		 // 圆点显示区域
	CString strTime; // 圆点对应x轴的时间刻度
}T_CircleInfo;

enum E_TrendBtnType
{
	ETBT_Volumn = 0,		// 成交量		- 股 期
	ETBT_Amount,			// 成交额		- 指
	ETBT_Index,				// 指标			- 指 股 期
	ETBT_VolRate,			// 量比			- 股
	ETBT_WindWarningLine,   // 风警线		- 股
	ETBT_BuySellForce,		// 买卖力道		- 股
	ETBT_CompetePrice,		// 竞价图		- 股
	ETBT_DuoKong,			// 多空指标		- 指
	ETBT_RiseFallRate,		// 涨跌率		- 指
	ETBT_PriceModel,		// 价格模型		- 期

	ETBT_Count
};

struct T_TrendBtn
{
	int32			m_iID;				// 按钮ID
	CPolygonButton  m_btnPolygon;		// 按钮
	E_TrendBtnType	m_eTrendBtnType;	// 按钮类型
	E_ReportType	m_eMerchType;		// 适合的商品类型
	
	T_TrendBtn(E_TrendBtnType eBtnType, E_ReportType eMerchType, CWnd *pParent=NULL);
	T_TrendBtn();
	void SetParent(CWnd *pParent);
};

class CIoViewTrend : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewTrend();
	virtual ~CIoViewTrend();
	
	DECLARE_DYNCREATE(CIoViewTrend)

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

	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch);	// 信息地雷更新
	virtual void	OnVDataClientTradeTimeUpdate();
	// 新增服务器数据类型
	virtual void	OnVDataGeneralNormalUpdate(CMerch *pMerch);
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchTrendIndexUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchAuctionUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchMinuteBSUpdate(CMerch* pMerch);
	virtual BOOL IsKindOfIoViewTrend() const {return TRUE;}

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
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	virtual void	OnKeySpace();

	virtual void	OnIoViewActive();
	virtual void    OnIoViewDeactive();
	virtual void	UpdateAxisSize(bool32 bPreDraw = true);

	// 获取副图的标题栏按钮:
	virtual int32	GetSubRegionTitleButtonNums();
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex);

	//
	virtual void	ClearLocalData(bool32 bClearAll = true);
	// 简单叠加商品
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData);
	virtual void				RemoveCmpMerch(CMerch *pMerch);
	virtual void				RemoveAllCmpMerch();
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);
	
	//
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd);
	
	virtual void	Draw();

	virtual	bool32	FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString	ToXmlInChild();
	virtual CString ToXmlEleInChild();

	virtual bool32	AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);
	// from CRegionViewParam

	virtual	void	GotoCompletePrice();
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

	virtual bool32  GetViewRegionRect(OUT LPRECT pRect);

	virtual bool32  OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix);

	virtual	void	OnChartDBClick(CPoint ptClick, int32 iNodePos);	// 双击事件
	virtual bool32	GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle);

	//
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos);
	virtual bool32  IsAlreadyToShowAdv();
//////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////// 设置是否显示均价和当前价
	virtual void   SetAvgPriceEvent();
// 
public:
	CGmtTime		GetRequestNodeTime(IN CMerch *pMerch);

	bool32			ServerTimeInOpenCloseTime ( CMerch* pMerch );
//	bool32			ServerTimeAfterCloseTime ( CMerch* pMerch );


	//
	void			DrawAlarmMerchLine();
	
	void			DrawRealtimePriceLine();
	void			DrawRealTimeSlider(CChartRegion* pRegion, CMemDCEx* pDC);

	void			GetDrawCrossInfoNoData(int32 &iOpenTime,int32 &iCloseTime);		// 无数据时候,画X 轴和详情窗口时用到

	// 最下面一排按钮相关
	void			DrawBottomBtns();
	void			InitMerchBtns();	// 初始化当前商品的按钮
	void			InitNoShowBtns();	// 初始化不显示按钮
	void			RecalcBtnRect(bool32 bNestRegion = false);	// 计算按钮的区域
	void			OnTrendBtnSelChanged(bool32 bReqData = true);	// 当前按钮类型变更了
	E_TrendBtnType	GetCurTrendBtnType();	// 当前的按钮类型
	bool32			InitRedGreenCurve();	// 初始化红绿柱状线
	void			CalcRedGreenValue();	// 根据当前商品计算红绿柱状线
	bool32			InitBtnIndex();			// 初始化按钮 指标 的指标
	bool32			IsRedGreenMerch(CMerch *pMerch);		// 指定商品是否是红绿线要求的商品

	void			DrawCompetePriceChart(CMemDCEx *pDC, CChartRegion &ChartRegion);	// 在所在的region绘制竞价图，主图&成交量副图绘制, 其它图形不管
	void			CalcCompetePriceValue();	// 计算竞价数据
	bool32			InitCompetePriceNodes();	// 初始化竞价图
	bool32			CalcCompetePriceMinMax(CChartCurve *pDepCurve, OUT float &fMin, OUT float &fMax);	// 计算竞价数据的最大最小值, 分为主图线与成交量副图线的计算
	bool32			CalcCompetePriceMinMaxVol(bool32 bVol, OUT float &fMin, OUT float &fMax);	// 计算竞价图的最大最小成交量值
	bool32			GetCompetePriceMinMax(bool32 bMainRegion, OUT CPriceToAxisYObject &AxisObj);	// 获取计算layout后竞价图的坐标轴计算(!!必须计算layout后用)，true主图, false成交量图

	bool32			RequestTrendPlusData();	// 请求分时的额外数据
	bool32			RequestTrendPlusPushData();	// 请求分时的额外的推送数据, 返回是否有推送数据请求了

	void			CalcTrendPlusSubRegionData();		// 计算一下分时副图额外的数据
	void			CalcDuoKongData();			// 多空
	void			CalcExpRiseFallRateData();	// 涨跌家数
	void			CalcVolRateData();			// 量比
	void			CalcBuySellForceData();		// 买卖力道
	void			CalcNoWeightExpLine();		// 不含权指数均线
	void			ShowProperAvgLine();		// 更具当前商品，显示合适的均线

	void			ShowProperTrendPlusCurve();	// 显示或者隐藏分时的额外的线
	bool32			IsShowTrendFirstOpenPrice();	// 是否显示前面的第一个开盘数据
	void			DrawTrendFirstOpenPrice(CMemDCEx *pDC);	// 绘制第一根开盘线

	bool32			GetCallAuctionTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 获取当前的竞价时间段
	bool32			IsTimeInCallAuction(const CGmtTime &TimeNow);	// 指定时间是否在竞价时间段内

	void			TrimPlusShowNodeData(INOUT CNodeSequence *pNodes);	// 截断显示线，以适合当前的显示区域

	void			DrawLandMines(CMemDCEx *pDC);	// 绘制地雷

	bool            IsCompetePriceShow();           // 竞价图是否显示

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
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& Rect);
	
	// 历史分时
public:
	void			SetHistoryTime(CGmtTime TimeHistory);
	void			SetHistoryFlag(bool32 bHistroy)	
	{
		if (bHistroy)
		{
			m_bShowTopToolBar = FALSE;
			m_bShowIndexToolBar = FALSE;
		}
		m_bHistoryTrend = bHistroy; 
	}
	void			OnCtrlLeftRight(bool32 bLeft) ;
	CString			GetHistroyTitle(CGmtTime Time);


public:
	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);			

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	
	void			ValueYAsNodata(float fYMin, float fYMax, const CRect& Rect,int32 iSkip, float fy,int32& iValue);

	// 新增画持仓线
	void			DrawHoldLine(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, IN CChartRegion* pRegion);

	void			RequestMinuteKLineData(T_MerchNodeUserData &MerchData);		// 请求几日的分钟k线数据
	void			TryReqMoreMultiDayMinuteKLineData();			// 尝试请求比先前发送的更多的要求的多日k线数据
	void			SetTrendMultiDay(int iDay, bool32 bClearData, bool32 bReqData);		// 设置多日分时
	void			UpdateTrendMultiDayChange();	// 变更了
	int32			GetTrendMultiDay(){return m_iTrendDayCount;}


	bool32			IsEnableSingleSection();			// 是否允许半场走势
	void			SetSingleSection(bool32 bSingle);	// 设置半场走势

	bool32			GetTrendStartEndTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// 获取分时图的开始&结束时间
	bool32			GetTrendSingleSectionOCTime(OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose); // 获取半场开收盘时间
	bool32			GetTrendFirstDayOpenCloseTime(OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose);	// 获取分时图的一天的第一个开盘，最后一个收盘时间

	// 商品叠加
	void				RequestSingleCmpMerchViewData(CMerch *pMerch);	// 请求单个商品的数据
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	bool32				CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax);	// 涨跌停

	bool32				IsShowRiseFallMax() const { return m_bShowRiseFallMax; }
	bool32				IsShowAvgPrice() const { return m_bShowAvgPrice; }

	float				GetTrendPrevClose();		// 更改昨收价取法

	void				AddSpecialCurveToSubRegion();	// 根据当前选择在副图中添加线
	CChartCurve			*GetSpecialCurve(int32 iSpecailId);	// 获取几根特殊线，用来填充或者清除数据
	void				SetSpecialCurvePickColor(int32 iSpecialId, uint32 uPickColor);		// 设置特殊线的匹配颜色
	bool32				GetSpecialCurvePickColor(int32 iSpecialId, OUT uint32 &uPickColor);	// 获取特殊线的匹配颜色

	bool32				AddIndexToSubRegion(const CString &StrIndexName, bool32 bShowRightDlg=false);	// 添加指标到副图，仅能添加到非成交量副图
	void				RemoveSubRegion();		// 删除非成交量副图
	CString				GetSubRegionTitle();	// 获取非成交量副图的名称
	bool32				LoadAllIndex(CNewMenu *pMenu);		// 加载所有菜单项
	int32				LoadOftenIndex(CNewMenu *pMenu);	// 加载常用菜单项
	void				ShowIndexMenu();					//显示指标菜单
	void				RequestVolBuySellData();		// 请求分时的每分钟内外盘信息

	bool32				IsSuspendedMerch();				// 是否为停牌商品，停牌商品填充虚假数据
	void				CreateSuspendedVirtualNoOpenKlineData();		// 在有实时报价数据，且有昨收无开盘，无实际分钟k下创建虚假的分钟k线数据
	void				CreateSuspendedVirtualNoOpenCmpKlineData();		// 创建停盘商品的虚拟cmpk线
	void				TrimSuspendedIndexData();		// 截断停盘数据的指标数据

	bool32				GetCurrentIOCTime(OUT CMarketIOCTimeInfo &IOCTime);	// 获取这个分时图的当天数据信息

private:
	
	//					根据商品类型判断是否需要成交量副图
	void				AddOrDelVolumeRegion(E_ReportType eBefore, E_ReportType eNow);

	//
	void				SetIndexToRegion(const CString& StrIndexName, CChartRegion* pRegion, bool32 bShowRightDlg=false);

	//
	int					TBottomBtnHitTest(CPoint point);
	int32				FindIndexByBtnID(int32 iID);		// 根据按钮Id找对应的arrary中的索引
	
public:
	friend	class CIoViewDuoGuTongLie;
	CChartDrawer*		m_pTrendDrawer;
	CArray<CGmtTime,CGmtTime>	m_TimesCombin;
	CArray<int32,int32>	m_PosCombin;
	
	CGmtTime			m_CornerTime;

	CNodeSequence		*m_pHoldNodes;		// 持仓线数据

	// 保存x轴的时间刻度信息
	std::vector<T_XAxisInfo> m_vecXAxisInfo;

	std::string m_strEconoData;

	// 小圆点的属性（即：半径、显示区域、弹出提示框）
	int m_radius;
	std::vector<T_CircleInfo> m_CircleInfo;
	CDlgEconomicTip m_dlgEconomicTip;

	// 
	BOOL m_bTracking;
	BOOL m_bOver;

public:
	std::vector<int> m_vecTimePos;

	virtual void	OnVDataEconoData(const char *pszRecvData);

	void StartReqEconoData();

	/*
	 * ParseEonomicData
	 * 解析财经日历数据
	 * 
	 */
	void ParseEonomicData(const char *pszData);

	void ParseLDorLK(IN std::string &str, OUT std::string &strLK, OUT std::string &strLD, OUT std::string &strPlaceholder, OUT bool &bBoth);

	/*
	 * SaveTimeDegreeScale
	 * 保存分时图x轴的时间刻度
	 * @param xPos 时间strTime的x坐标
	 * @param strTime 时间刻度
	 */
	void SaveTimeDegreeScale(int xPos, CString strTime);

	/*
	 * ComputeCircleInfo
	 * 计算财经日历数据在分时图中小圆点的位置
	 */
	void ComputeCircleInfo();// 计算小圆点坐标

	/*
	 * DrawCircleDot
	 * 绘制财经日历的小圆点
	 */
	void DrawCircleDot(CMemDCEx * pDC);	// 绘制财经日历的小圆点
	double myFabs(double dbNum);
	

	std::map<int, CString> m_mapIndexName;  //指标菜单项对应ID
private:
	bool32				m_bHistoryTrend;
	CGmtTime			m_TimeHistory;

private:
	int32				m_iOpenTimeForNoData;
	int32				m_iCloseTimeForNoData;

	bool m_bSizeChange;

	// 当超过1天时，需要先等日k线返回才能确定要请求多少天[时间段]的分钟k线数据
	int32				m_iTrendDayCount;		// 要查看几天的分时, 默认1
	// 在每次请求数据时，重新计算该数组
	CArray<CMarketIOCTimeInfo, const CMarketIOCTimeInfo &>	m_aTrendMultiDayIOCTimes; // 每天的开收盘，第一天为当天，往上为前n天

	// 当前时间所在的开收盘时间, init-第一个开场属于第一个开场
	//      中间的任意非开场时段属于上一个开场
	//		最后一个收场到End时间属于最后一个开场段
	// 仅给国内股票&指数提供
	bool32				m_bSingleSectionTrend;		// 半场走势

	float				m_fMinHold;
	float				m_fMaxHold;
	
	
	// t..fangz1010
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkMain;	// 主图 y轴分割线备份,用于切换时或者无商品数据时绘图
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkSub;	// 副图 y轴分割线备份,用于切换时或者无商品数据时绘图

	//
	CArray<CKLine, CKLine>							m_aKLineInterval;					// 区间统计的K线
	float m_fPriceIntervalPreClose;
	// 最下方按钮相关
	typedef CArray<T_TrendBtn, const T_TrendBtn &>	TrendBtnArray;
	static	TrendBtnArray	s_aInitTrendBtns;	// 初始化按钮
	TrendBtnArray	m_aTrendBtnCur;	// 当前按钮
	int32	m_iBtnCur;		// 当前按钮
	typedef CMap<int32, int32, CString , CString> TrendMerchTypeIndexNameMap;
	typedef CMap<int32, int32, int32, int32>	TrendMerchTypeBtnIndexMap;
	TrendMerchTypeIndexNameMap	m_mapMerchTypeIndexName;	// 商品类型与指标栏选择的指标名称
	TrendMerchTypeBtnIndexMap	m_mapMerchTypeBtnIndex;		// 每中商品类型对应选择的按钮索引
	CString			m_StrBtnIndexName;		// 切换到指标按钮时，当前的指标名称

	CStringArray	m_aAllFormulaNames;		// 所有指标名称数组

	CChartCurve		*m_pRedGreenCurve;	// 红绿线
	CNodeSequence	*m_pCompetePriceNodes;	// 竞价数据, 该数据不对应任何线，自己绘制

	CChartCurve		*m_pNoWeightExpCurve;	// 不含权指数线

	typedef map<int32, uint32>	SpecialCurvePickColorMap;
	SpecialCurvePickColorMap	m_mapSpecialCurves;		// 该分时图创建的特殊线所对应的唯一特征值, 由于线可能会被删除，所以保存特征值查找

	mapLandMine		m_mapLandMine;						// 信息地雷数据
	typedef multimap<CString, CRect>	LandMineRectMap;
	LandMineRectMap	m_mapLandMinesRect;					// 信息地雷区域

	CArray<double, double>	m_aSubRegionHeightRatios;	// 副图的高度比例

	bool32			m_bDrawYRigthPrice;					// 是否在右边画最新价的

	int32			m_iBtnHovering;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTrend)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTrend)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenu ( UINT nID );
	afx_msg void OnMenuIntervalStatistic();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMySettingChange();
	afx_msg void OnTrendIndexMenu(UINT nId);
	afx_msg void OnUpdateTrendIndexMenu(CCmdUI *pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TREND_H_

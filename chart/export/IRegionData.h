#pragma once
#include "chartstruct.h"
#include "NCButton.h"
#include "PolygonButton.h"
#include "SelfDraw.h"
#include "faceschemetype.h"
#include "facescheme.h"
#include "XTipWnd.h"
#include "Region.h"
#include "SaneIndex.h"
#include "AxisYObject.h"
#include "chartexport.h"
#include "ChartCurve.h"
class CChartRegionViewParam;
class CRegionDrawNotify;
typedef vector<CSelfDrawNode*> vecSelfDrawNode;
typedef vector<vecSelfDrawNode> mapSelfDrawNode;
typedef CArray<CMerch *, CMerch *> MerchArray;

// 指标区域上的按钮
#define INDEX_HELP		8888		// 帮助
#define INDEX_UPDATE	8889		// 修改参数

class CHART_EXPORT IChartRegionData
{
public:
	CArray<T_MerchNodeUserData*,T_MerchNodeUserData*>	m_MerchParamArray;		// 所有商品， 其中[0]一定是主商品

	// region列表
public:
	virtual void	OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd){};
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd){};
	virtual bool32 GetShowSaneIndexFlag(){return FALSE;};
	virtual void   GetNodeBeginEnd ( int32& iNodeBegin,int32& iNodeEnd ) = 0;
	virtual void   SetPickedSelfDrawCurve(CSelfDrawCurve* pCurve) = 0;
	virtual CSelfDrawCurve* GetPickedSelfDrawCurve() = 0;
	virtual T_IndexParam*	FindIndexParamByCurve ( CChartCurve* pCurve ) = 0;
	virtual void			FindCurvesByIndexParam ( T_IndexParam* pParam,CArray<CChartCurve*,CChartCurve*>& Curves ) = 0;
	virtual bool32	IsShowRiseFallMax() const{return FALSE;}
	virtual bool32	IsShowAvgPrice() const{return FALSE;}
	virtual bool32	CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax){return FALSE;};
	virtual CString GetSubRegionTitle() {return _T("");}
	virtual void CalcLayoutRegions(bool bOnlyUpdateMainRegion = true)= 0;
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode) = 0;
	virtual bool32	GetRisFallFlag()= 0;
	virtual bool32	GetSaneIndexNode(IN int32 iPos,OUT T_SaneIndexNode& Node){return FALSE;}
	virtual bool32			BeShowTopToolBar() = 0;
	virtual bool32			BeSysClrKLine(){return FALSE;}
	virtual CString GetClrKLineName() {return _T("");}
	virtual bool32	GetCrossLockInfo() const	= 0;
	virtual bool32			GetShowClrKLineFlag() {return FALSE;}
	virtual HCURSOR	SetNewCursor(HCURSOR hCursor){return FALSE;}
	virtual	void	DragMoveKLine(int32 icnt){};	// 拖动K线平移
	// 主图region
	CChartRegion*	m_pRegionMain;
	CChartRegion*	m_pRegionSeparator;				// Region分割线
	CChartRegion*	m_pRegionXBottom;				// x轴Region
	CChartRegion*	m_pRegionYLeft;					// 左y轴Region
	CChartRegion*	m_pRegionYRight;				// 右y轴Region
	CChartRegion*	m_pRegionLeftBottom;			// 左下角Region
	CChartRegion*	m_pRegionRightBottom;			// 右下角Region

	// 各个子region
	CArray<T_SubRegionParam, T_SubRegionParam&> m_SubRegions;//子Region

public:
	Image*			m_pImageShake;						// 震荡趋势
	Image*			m_pImageStrongStabilityUp;			// 强,稳定,上升
	Image*			m_pImageNormalStabilityUp;			// 一般,稳定,上升
	Image*			m_pImageWeekStabilityUp;			// 弱,稳定,上升

	Image*			m_pImageStrongUnStabilityUp;		// 强,不稳定,上升
	Image*			m_pImageNormalUnStabilityUp;		// 一般,不稳定,上升
	Image*			m_pImageWeekUnStabilityUp;			// 弱,不稳定,上升

	Image*			m_pImageStrongStabilityDown;		// 强,稳定,下降
	Image*			m_pImageNormalStabilityDown;		// 一般,稳定,下降
	Image*			m_pImageWeekStabilityDown;			// 弱,稳定,下降

	Image*			m_pImageStrongUnStabilityDown;		// 强,不稳定,下降
	Image*			m_pImageNormalUnStabilityDown;		// 一般,不稳定,下降
	Image*			m_pImageWeekUnStabilityDown;		// 弱,不稳定,下降
	Image*			m_pImageDTOC;						// 多头开仓
	Image*			m_pImageDTAC;						// 多头加仓
	Image*			m_pImageDTRC;						// 多头减仓
	Image*			m_pImageDTCC;						// 多头清仓

	Image*			m_pImageKTOC;						// 空头开仓
	Image*			m_pImageKTAC;						// 空头加仓
	Image*			m_pImageKTRC;						// 空头减仓
	Image*			m_pImageKTCC;						// 空头清仓

	Image*			m_pImageDCKO;						// 多清空开
	Image*			m_pImageKCDO;						// 空清多开

	Image*			m_pImageExpert;						// 空清多开

	Image*			m_pImageRiseFallFlag;				// 涨跌标记

	Image*			m_pImageCLBuy;						// 策略买信号
	Image*			m_pImageCLSell;						// 策略卖信号
	Image*			m_pImageCLMul;						// 策略多信号

	CImageList										m_ImageListRiseFallFlag;			// 涨跌标记
	CChartRegion*	m_pRegionPick;
	CChartCurve*	m_pCurvePick;
	CNodeSequence*	m_pNodesPick;
	CNodeData		m_NodePick;

	CNodeSequence*	m_pExportNodes;

	CPriceToAxisYObject::E_AxisYType m_eMainCurveAxisYType;		// 主图的Y坐标类型

	//CStringArray	m_aUserBlockNames;		// 由于都需要使用 加入到自选板块，转移IoViewBase中

public:
	uint32			m_uiFlag;
	int32			m_iChartType;//0:fs 1:kline 2:tick 3: expfs 4: klinearbitrage 5:trendarbitrage 6:多合约同列K线 7:多合约同列分时
	bool			m_bMoreView;// 是否是多合约同列界面
	CXTipWnd		m_TipWnd;
	CPoint			m_PtGridCtrl;
	int32			m_iDataServiceType;
	CString			m_StrText;
	CStringArray	m_FormulaNames;
	float			m_fPricePrevClose;
	int32			m_iPickSubRegionIndex;	// 选定的副图

protected:
	CChartRegionViewParam*	m_pRegionViewData;
	CRegionDrawNotify*	m_pRegionDrawNotify;


	CString			m_StrMerchCodeXml;
	int32			m_iMarketIdXml;

	int32			m_iSubRegionNumXml;
	CStringArray	m_IndexNameXml;
	CArray<int32,int32>	m_IndexRegionXml;
	CArray<int32,int32> m_aRegionHeightXml;
	CArray<double, double> m_aRegionHeightRadioXml;
	CArray<int32,int32> m_aRegionMainIndexXml;
	mapSelfDrawNode	m_mapRegionSelfDrawNodes;

	bool32			m_bShowTopToolBar;
	bool32			m_bShowIndexToolBar;
	bool32			m_bShowHelp;
	bool32			m_bShowExpBtn;			// K线分时等图表上是否显示指数按钮
	bool32			m_bShowTopMerchBar;		// 是否显示顶部商品名称

	int32			m_iIntervalXml;	//主图类型
	bool32			m_bTrendIndex;  // ...fangz 0815
	bool32			m_bSaneIndex;   // ...fangz 0815

	int32			m_iDrawTypeXml;//分析周期
	int32			m_iNodeNumXml;

	UINT			m_uiUserMinutesXml;
	UINT			m_uiUserDaysXml;

	CSelfDrawCurve*	m_SelfDrawCurvePick;
	bool32			m_bTodayRise;
	bool32			m_bTracking;	// 鼠标离开的标记
	bool32			m_bInitialFloat;

	// 简单叠加商品有关
	MerchArray	    m_aMerchsCmpNeedUpdateShowData;		// 显示区域变更导致的必须要更新的叠加商品
	bool32			m_bAutoAddCmpExpMerch;			// 自动叠加对应指数

	bool32			m_bEnableRightYRegion;			// 允许右Y轴
	bool32			m_bEnableLeftYRegion;			// 允许左Y轴

	// 放大缩小的三种模式
	// 1: 无十字光标的时候, 正常缩放
	// 2: 有十字光标, 而且是鼠标定位的(这时以定位的这跟K 线为中心缩放)
	// 3: 有十字光标, 而且是键盘左右键定位的(这时十字光标始终精确定位在这根K 线上)

	// bool32			m_bAccurateZoom;				// 对应第三种放缩的标记, 目前只K 线中用到, 以后闪电图也可能需要

public:
	bool32			m_bFromXml;
	T_IndexPostAdd	m_IndexPostAdd;

	CStringArray	m_aStrVolIndexNames;			// fangz0624.目前是写死的,指标系统好了以后再改.系统中所有的成交量型指标

	bool32			m_bForbidDrag;

	bool32			m_bLockCross;

protected:

	// 区间统计的起止时间
	CGmtTime		m_TimeIntevalBegin;
	CGmtTime		m_TimeIntevalEnd;

	// 外汇交易
	float			m_fPriceMouseClick;		// 弹出菜单处的价格
	HCURSOR         m_hNewCurosr;

	//k线和分时图 广告框
	bool32              m_bOwnAdvertise;          //拥有广告dlg的view,在changesize时要重画广告dlg
	bool32              m_bViewIsvisible;
	bool32              m_bIsHide;
	bool32              m_bIsKLineOrTend;    //K线或者分时图 

	std::map<int, CNCButton> m_mapTopBtn;			// 顶部工具栏左边公共按钮
	std::map<int, CNCButton> m_mapTopCenterBtn;		// 顶部工具栏中间公共按钮
	std::map<int, CNCButton> m_mapKRightBtn;		// K线顶部工具栏右边按钮
	std::map<int, CNCButton> m_mapTrendRightBtn;	// 分时图顶部工具栏右边按钮
	std::map<int, CNCButton> m_mapTopExpBtn;		// 顶部工具栏指数商品按钮

	std::map<int, CPolygonButton> m_mapBottomBtn;		// 底部指标工具栏
	int32				m_iTopBtnHovering;			// 标识鼠标进入按钮区域
	int32				m_iTopCenterHovering;
	int32				m_iKTopHovering;
	int32				m_iTrendTopHovering;
	int32				m_iBottomHovering;
	int32				m_iTopExpHovering;
	int32				m_iLeftWidth;				// top 左边按钮总宽度
	int32				m_iRightWidth;				// top 右边按钮总宽度

	CStringArray		m_ToolBarIndexName;			// 底部指标工具栏显示的指标名称
	int32				m_iDayIndex;				// 多日分时的索引
	CString				m_strMainIndexName;			// 主图指标名称

public:
	int32				m_iSaneTradeTimes;					// 交易次数
	int32				m_iSaneRealKlineNums;				// 真实统计数
	float				m_fSaneAccuracyRate;				// 准确率
	float				m_fSaneProfitability;				// 收益率
	float				m_fProfit;							// 收益		

	enum{
		// 分时按钮
		TOP_IOCMD_TREND,

		// 分时图顶部右边按钮
		TOP_TREND_SHOW,
		TOP_TREND_BID,
		TOP_TREND_TREND_ADD,
		TOP_TREND_MORE_TREND,
		TOP_TREND_TREND_SUB,
		TOP_TREND_SELFDRAW,
		TOP_TREND_TICK,
		TOP_TREND_MORE_MERCH,
		TOP_TREND_FLASH,	// 闪电交易

		// k线顶部右边按钮
		TOP_KINE_SHOW_MAININDEX,
		TOP_KINE_SHOW,
		TOP_KINE_SELFDRAW,
		TOP_KINE_TICK,
		TOP_KINE_ADD_CMP,
		TOP_KINE_FLASH,		// 闪电交易
		TOP_KINE_HELP,
		TOP_KINE_WEIGHT,	// 复权

		// 中部的按钮
		TOP_CENTER_PRE,			
		TOP_CENTER_MERCH,
		TOP_CENTER_NEXT,

		// 顶部指数商品按钮
		TOP_EXP_SZZS,	// 上证指数
		TOP_EXP_SZCZ,	// 深证成指
		TOP_EXP_CYBZ,	// 创业板指
		TOP_EXP_HSSB,	// 沪深300

		TOP_ADD_ZIXUAN,	// 添加到自选

		// 指标
		BOTTOM_INDEX_BEGIN,
	};
};

class CHART_EXPORT IIoViewBase
{
public:
	virtual COLORREF GetIoViewColor(E_SysColor eSysColor) = 0;
	virtual LOGFONT* GetIoViewFont(E_SysFont eSysFont) = 0;
	virtual CFont*			GetIoViewFontObject(E_SysFont eSysFont) = 0;
	virtual bool32			IsActive() = 0;
	virtual bool32			ParentIsIoView() = 0;
	virtual int32 GetChartType() = 0;
	virtual CMerch* GetMerchXml() = 0;	
	virtual BOOL IsKindOfIoViewKLine() const {return FALSE;}
	virtual BOOL IsKindOfIoViewTrend() const {return FALSE;}
	virtual BOOL IsKindOfIoViewChart() const {return FALSE;}
	// 每个业务视图的颜色和字体信息:
	CFontNode		m_aIoViewFont[ESFCount];					// 当前业务视图使用的字体
	COLORREF		m_aIoViewColor[ESCCount];					// 当前业务视图使用的颜色	
	
	int32				m_iActiveXpos;
	int32				m_iActiveYpos;
	COLORREF			m_ColorActive;
	CMerch			*m_pMerchXml;
	CMerchKey		m_MerchXml;			// 如果m_pMerchXml为NULL, 则取用该字段， 避免由于权限问题导致暂时去不到xml的问题
};

typedef struct CHART_EXPORT _IChartBrige
{
	CWnd *pWnd;
	IIoViewBase *pIoViewBase;
	IChartRegionData *pChartRegionData;

	_IChartBrige()
	{
		pWnd = NULL;
		pIoViewBase = NULL;
		pChartRegionData = NULL;
	}
}IChartBrige;
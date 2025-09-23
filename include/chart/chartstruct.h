#ifndef __CHART_STRUCT_XKDJKJS_20150831_HH__
#define __CHART_STRUCT_XKDJKJS_20150831_HH__
//#include "EngineCenterBase.h"
#include "TimeInterval.h"
#include "TrendIndex.h"
#include "chartexport.h"

class CNodeSequence;

#define  IMAGEID_NOUSEIMAGE				(UINT) 0   // 不用图标
#define  IOVIEW_TITLE_HEIGHT			(int32)18  // 标题栏高度

#define  IOVIEW_TITILE_LEFT_BUTTON_ID	(int32)-1	// 左箭头ID
#define  IOVIEW_TITILE_RIGHT_BUTTON_ID	(int32)-2	// 右箭头ID
#define  IOVIEW_TITLE_ADD_BUTTON_ID		(int32)-3	// 添加按钮ID
#define  IOVIEW_TITLE_DEL_BUTTON_ID		(int32)-4	// 删除按钮ID
#define  IOVIEW_TITLE_F7_BUTTON_ID		(int32)-5	// F7 按钮ID

class CNodeSequence;
class CChartRegion;
// 描述某一天走势显示相关信息, 仅在走势图中有用
CHART_EXPORT void KLine2NodeData ( const CKLine& KLine, CNodeData& NodeData);
CHART_EXPORT CString TimeInterval2String ( E_NodeTimeInterval eTimeInterval ,UINT uiUserMinutes = 0 , UINT uiUserDays = 0);
class CHART_EXPORT CTrendTradingDayInfo
{
public:
	CTrendTradingDayInfo();

public:
	void			Set(IN CMerch *pMerch, const CMarketIOCTimeInfo &MarketIOCTime);
	void			RecalcPrice(const CMerch &Merch);

	float			GetPrevReferPrice();		// 取上一交易日参考价格数值， 国内期货按昨结算价， 其他所有商品按昨收

	void			RecalcHold(const CMerch &Merch);
	float			GetPrevHold();

public:
	bool32			m_bInit;	
	bool32			m_bNeedRecalc;				// 有些字段值没有,可能不准,需要重算.

	// 
	CMerch			*m_pMerch;
	CMarketIOCTimeInfo	m_MarketIOCTime;		// 该日期交易时间

	float			m_fPricePrevClose;			// 昨收
	float			m_fPricePrevAvg;			// 昨结算
	float			m_fHoldPrev;				// 前持仓

	//
	CGmtTime		m_TimeLastUpdate;			// 上一次更新的时间
};

//同一种Merch的Nodes的UserData
struct CHART_EXPORT T_MerchNodeUserData
{
public:
	T_MerchNodeUserData(/*CViewData* p*/);
	~T_MerchNodeUserData();

public:
	bool32			UpdateShowData(int32 iShowPosInFullList, int32 iShowCountInFullList);

public:
	CMerch			*m_pMerchNode;

	//////////////////////////////////////////////////////////
	// 原始数据
	// 全部的数据
	CArray<CTick, CTick>	m_aTicksCompare;
	CArray<CKLine, CKLine>	m_aKLinesCompare;		// 当前屏幕显示的k线原始数据， 考虑到真实显示的数据， 有可能还要进行加工， 比如走势图是在1分钟K线的基础上增补而来
	E_NodeTimeInterval		m_eTimeIntervalCompare;	// 当前屏幕显示数据的原始数据类型， 考虑到15分钟线是由5分钟线拼出来的

	CArray<CKLine, CKLine>	m_aKLinesFull;			// 处理过后的全部数据， 比如：合并N分钟线， 再如：副图相对于主图可能存在的个别节点增删情况
	E_NodeTimeInterval		m_eTimeIntervalFull;	// 

	CArray<DWORD, DWORD>	m_aKLinesShowDataFlag;	// 与显示数据同长[PosInFullList=这边的0起点]的，标志各node节点标志的数组, 如同步，则使用该数组，否则抛弃且重置	xl 0106

	// 当前屏幕显示的内容在原始数据中位置， 通过该内容生成m_pKLinesShow
	int32			m_iShowPosInFullList;			// 当前屏幕显示的起始位置
	int32			m_iShowCountInFullList;	

	// 当前屏幕显示数据内容
	CNodeSequence	*m_pKLinesShow;					// 当前屏幕显示的K线

	///////////////////////////////////////////////////////////
	// 各个指标数据
	CArray<T_IndexParam*, T_IndexParam*>	aIndexs;	// 指标线数据

	///////////////////////////////////////////////////////////
	// 
	int32			m_iTimeUserMultipleMinutes;		// 自定义分钟线周期
	int32			m_iTimeUserMultipleDays;		// 自定义日线周期

	bool32			bMainMerch;
	bool32			bUpdateOnce;
	bool32			bNew;					// MainMerch是新创建的.

public:
	// 仅在走势图中有意思
	CTrendTradingDayInfo	m_TrendTradingDayInfo;

private:
	//CViewData*		m_pAbsCenterManager;
};


typedef struct T_FuctionButton 
{
public:
	int32		m_uID;
	UINT		m_uiBmpNormal;
	UINT		m_uiBmpFocus;
	UINT		m_uiBmpSelf;

	CString		m_StrText;
	CString		m_StrTips;

	T_FuctionButton(const CString& StrText, const CString& StrTips, int32 uID,  UINT uiBmpNormal, UINT uiBmpFocus, UINT uiBmpSelf)
	{
		m_uID	  = uID;		
		m_StrText = StrText;
		m_StrTips = StrTips;

		m_uiBmpNormal = uiBmpNormal;
		m_uiBmpFocus  = uiBmpFocus;
		m_uiBmpSelf   = uiBmpSelf;
	};

}T_FuctionButton;

// 每个子图的信息
typedef struct T_SubRegionParam
{
public:
	T_SubRegionParam()
	{
		m_pSubRegionMain	= NULL;
		m_pSubRegionYLeft	= NULL;
		m_pSubRegionYRight	= NULL;
	}

public:
	CChartRegion*	m_pSubRegionMain;
	CChartRegion*	m_pSubRegionYLeft;
	CChartRegion*	m_pSubRegionYRight;
} T_SubRegionParam;

typedef struct T_KLineID
{
	int32 iNum;
	CKLine KLine0;
	CKLine KLine1;
} T_KLineID;

typedef struct T_IndexPostAdd
{
	CString StrIndexName;
	CChartRegion* pRegion;
	int id;
} T_IndexPostAdd;
#endif
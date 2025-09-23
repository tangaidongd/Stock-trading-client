#ifndef _PLUG_IN_STRUCT_H_
#define _PLUG_IN_STRUCT_H_


#include "sharestruct.h"
#include "InfoExport.h"


// 插件模块的请求, 回应类型
typedef enum E_CommTypePlugIn
{
	ECTPIReqMerchIndex = 1000,		// 请求个股数据
	ECTPIRespMerchIndex,

	ECTPIReqBlock,					// 请求板块数据
	ECTPIRespBlock,

	ECTPIReqMerchSort,				// 请求商品排行
	ECTPIRespMerchSort,

	ECTPIReqBlockSort,				// 请求板块排行
	ECTPIRespBlockSort,

	ECTPIReqMainMonitor,			// 主力监控
	ECTPIRespMainMonitor,

	ECTPIReqTickEx,					// 分笔统计
	ECTPIRespTickEx,

	ECTPIReqHistoryTickEx,			// 历史分笔统计
	ECTPIRespHistoryTickEx,

	ECTPIReqChooseStock,			// 请求选股集合
	ECTPIRespChooseStock,

	ECTPIReqDapanState,				// 请求大盘状态
	ECTPIRespDapanState,

	ECTPIReqChooseStockStatus,		// 请求选股状态
	ECTPIRespChooseStockStatus,
	
	ECTPIReqAddPushMerchIndex,		// 增加个股推送
	ECTPIRespAddPushMerchIndex,

	ECTPIReqUpdatePushMerchIndex,	// 同步个股推送
	ECTPIRespUpdatePushMerchIndex,
	
	ECTPIReqAddPushBlock,			// 增加板块数据推送
	ECTPIRespAddPushBlock,

	ECTPIReqUpdatePushBlock,		// 同步板块数据推送
	ECTPIRespUpdatePushBlock,
	
	ECTPIReqAddPushMainMonitor,		// 增加主力监控推送
	ECTPIRespAddPushMainMonitor,

	ECTPIReqUpdatePushMainMonitor,	// 同步主力监控推送
	ECTPIRespUpdatePushMainMonitor,
	
	ECTPIReqAddPushTickEx,			// 增加分笔推送
	ECTPIRespAddPushTickEx,

	ECTPIReqUpdatePushTickEx,		// 同步分笔推送
	ECTPIRespUpdatePushTickEx,
	
	ECTPIReqAddPushChooseStock,		// 增加选股集合推送
	ECTPIRespAddPushChooseStock,

	ECTPIReqUpdatePushChooseStock,	// 同步选股集合推送
	ECTPIRespUpdatePushChooseStock,
	
	ECTPIReqDeletePushMerchData,	// 删除推送商品数据
	ECTPIRespDeletePushMerchData,

	ECTPIReqDeletePushBlockData,	// 删除推送板块数据
	ECTPIRespDeletePushBlockData,

	ECTPIReqDeletePushChooseStock,	// 删除推送选股数据
	ECTPIRespDeletePushChooseStock,
	
	ECTPIRespPushFlag,				// 请求,同步,删除推送的回包(回应上述操作是否成功, 不是具体数据)

	ECTPIReqPeriodTickEx,			// 请求带周期的分笔
	ECTPIRespPeriodTickEx,          // 带周期的分笔回复

	ECTPIReqPeriodMerchSort,        // 请求带周期的商品排行
	ECTPIRespPeriodMerchSort,        // 带周期的商品排行回复

	ECTPIReqAddPushMerchIndexEx3,        // 请求带周期的商品排行
	ECTPIReqAddPushMerchIndexEx5,        // 请求带周期的商品排行
	ECTPIReqAddPushMerchIndexEx10,        // 请求带周期的商品排行

	ECTPIRespAddPushMerchIndexEx3,        // 请求带周期的商品排行
	ECTPIRespAddPushMerchIndexEx5,        // 请求带周期的商品排行
	ECTPIRespAddPushMerchIndexEx10,        // 请求带周期的商品排行

	ECTPIReqShortMonitor,                 // 请求短线监控
	ECTPIRespShortMonitor,                // 短线监控回包

	ECTPIReqAddPushShortMonitor,          // 增加短线监控推送
	ECTPIRespAddPushShortMonitor,          // 增加短线监控推送

	ECTPIRespDelPushShortMonitor,          // 删除短线监控推送

	ECTPIReqDKMoney,						// 增加多空资金数据请求
	ECTPIRespDKMoney,						// 增加多空资金数据回复

	ECTPIReqCRTEStategyChooseStock,			// 选股集合扩展
	ECTPIRespCRTEStategyChooseStock,		// 选股集合扩展回包

	ECTPIReqAddPushCRTEStategyChooseStock,	 // 增加选股集合扩展推送
	ECTPIRespAddPushCRTEStategyChooseStock,

	ECTPIReqMainMasukura,					 // 主力增仓请求
	ECTPIRespMainMasukura,					 // 主力增仓回包
	
	ECTPIReqAddPushMainMasukura,			 // 增加主力增仓推送
	ECTPIRespAddPushMainMasukura,			

	//ECTPIReqAddPushCRTEStategyChooseStock,		// 增加选股集合扩展推送
	//ECTPIRespAddPushCRTEStategyChooseStock,


	ECTPIReqCount,

}E_CommTypePlugIn;

// 报价排行的字段
typedef enum E_ReportSortPlugIn
{
	ERSPIBegin   = 100,

	ERSPIExBigBuyVol,		// 超大单买入量
	ERSPIExBigSellVol,		// 超大单卖出量

	ERSPIBigBuyVol,			// 大单买入量
	ERSPIBigSellVol,		// 大单卖出量

	ERSPIMidBuyVol,			// 中单买入量
	ERSPIMidSellVol,		// 中单卖出量

	ERSPISmallBuyVol,		// 小单买入量
	ERSPISmallSellVol,		// 小单卖出量

	ERSPIMnSBuyVol,			// 中小单买入量
	ERSPIMnSSellVol,		// 中小单卖出量

	ERSPIExBigBuyAmount,	// 超大单买入额
	ERSPIExBigSellAmount,	// 超大单卖出额

	ERSPIBigBuyAmount,		// 大单买入额
	ERSPIBigSellAmount,		// 大单卖出额

	ERSPIMidBuyAmount,		// 中单买入额
	ERSPIMidSellAmount,		// 中单卖出额

	ERSPISmallBuyAmount,	// 小单买入额
	ERSPISmallSellAmount,	// 小单卖出额

	ERSPIExBigNetAmount,	// 超大单净额
	ERSPIBigNetAmount,		// 大单净额
	ERSPIMidNetAmount,		// 中单净额
	ERSPISmallNetAmount,	// 小单净额

	ERSPITotalBuyAmount,	// 买入总额
	ERSPITotalSellAmount,	// 卖出总额

	ERSPIExBigChange,		// 超大单换手率
	ERSPIBigChange,			// 大单换手率
	ERSPIBigBuyChange,		// 买入大单换手率

	ERSPITradeCounts,		// 成交笔数
	ERSPIAmountPerTrade,	// 每笔金额
	ERSPIStockPerTrade,		// 每笔股数
	ERSPIBuyRate,			// 买入比例

	ERSPIShort,				// 短线
	ERSPIMid,				// 中线

	ERSPIExBigNetBuyVolume,	// 超大单净买入量

	ERSPIEnd,
}E_ReportSortEx;

// 板块排序的字段
typedef enum E_BlockSort
{
	EBSBegin = 200,
	EBSAvgRise,			// 均涨幅
	EBSWeightRise,		// 权涨幅
	EBSChange,			// 换手率
	EBSPeRatio,			// 市盈率

	EBSEnd,
}E_BlockSort;

// 条件选股的类型 (普通请求和推送都用这个)
typedef enum E_ChooseStockType
{
	// 这些推送是推送某个商品的某些字段值
	
	ECSTChooseStock = 0x0001,	// 选股的字段[短线, 中线]
	ECSTMainMonitor = 0x0002,	// 主力监控
	ECSTTickEx		= 0x0004,	// 分笔统计

	// 这些推送是推送一个商品的集合, 请求的时候只要发类型, 跟具体商品无关

	ECSTShort		= 0x0008,	// 短线商品多集合
	ECSTMid			= 0x0010,	// 中线商品多集合
	ECSTMrjx		= 0x0020,	// 买入精选集合
	ECSTDpcl		= 0x0040,	// 大盘策略集合
	ECSTJglt		= 0x0080,	// 主力拉升集合
	ECSTXpjs		= 0x0100,	// 主力洗盘集合
	ECSTZjzt		= 0x0200,	// 游击涨停集合
	ECSTNsql		= 0x0400,	// 抗跌强庄集合
	ECSTCdft		= 0x0800,	// 超跌反弹集合
// 	ECSTZlcd		= 0x0020,	// 战略抄底 集合
// 	ECSTBdcd		= 0x0040,	// 波段抄底 集合
// 	ECSTCdft		= 0x0080,	// 超跌反弹 集合
// 	ECSTJdxgDuo		= 0x0100,	// 金盾选股多 集合
// 	ECSTShortKong		= 0x0200,	// 短线空 集合
// 	ECSTMidKong		= 0x0400,	// 中线空 集合
// 	ECSTZlzKong		= 0x0800,	// 战略做空 集合


	// 板块数据
	ECSTBlock		= 0x1000,	// 板块数据


	ECSTSlxs		=0x2000,	// 神龙吸水 集合
	ECSTHffl		=0x4000,	// 回风拂面 集合
	ECSTShjd		=0x8000,	// 三花聚顶 集合

	// 其它选股标记
// 	ECSTBdzKong		= 0x2000,	// 波段做空
// 	ECSTCmhtKong	= 0x4000,	// 超买回调空
// 	ECSTJdxgKong	= 0x8000,	// 金盾选股空

	ECSTShortMonitor	= 0x10000,	// 短线监控推送删除

	ECSTKtxj		= 0x00010000,	// 空头陷阱策略
	ECSTZdsd		= 0x00020000,	// 震荡市短买策略
	ECSTDxqs		= 0x00040000,	// 短线强势策略
	ECSTQzhq		= 0x00080000,	// 强者恒强策略
	ECSTBdcz		= 0x00100000,	// 波段操作策略
	
	ECSTMainMasukura = 0x00200000,	// 主力增仓
	
}E_PlugInPushType;

// 选股的状态 ( 按先后顺序对应三角, 圈圈叉叉什么的. )
enum E_StockState
{
	ESS0 = 0,
	ESS1,
	ESS2,
	ESS3,

	ESSCount,
};

// 大盘牛熊状态
enum E_DpnxState
{
	EDS0 = 0,
	EDS1,
	EDS2,
	EDS3,
	EDS4,

	EDSCount,
};

//选股状态
enum E_ChooseStockState
{
	ECSSNONE = 0x00,	// 都没选出股票
	ECSS1 = 0x01,		// 一剑飞仙-涨停先锋
	ECSS2 = 0x02,		// 青云直上-主力拉升
	ECSS3 = 0x04,		// 神龙吸水-洗盘结束
	ECSS4 = 0x08,		// 升龙逆天-逆势强龙
	ECSS5 = 0x10,		// 吸庄大法-游资强势
	ECSS6 = 0x20,		// 回风拂柳-超跌反弹
	ECSS7 = 0x40,		// 三花聚顶-战略选股

	ECSS8	= 0x80,		// 回风拂柳-空头陷阱
	ECSS9	= 0x100,	// 吸庄大法-震荡市短买
	ECSS10	= 0x200,	// 一剑飞仙-短线强势
	ECSS11	= 0x400,	// 青云直上-强者恒强
	ECSS12	= 0x800,	// 神龙吸水-波段操作
};

// 个股选股的状态回包结构
typedef struct T_RespMerchIndex
{ 
	T_RespMerchIndex()
	{
		m_uiMarket = INT_MIN;
		m_usShort = EDSCount;
		m_usMid = EDSCount;
		m_fCapticalFlow = 0.0f;
		//
		m_fAllNetAmount = 0.0f;
		m_fExBigBuyAmount = 0.0f;
		m_fExBigSellAmount = 0.0f;
		m_fExBigNetAmount = 0.0f;
		m_fExBigNetAmountProportion = 0.0f;
		m_fExBigNetTotalAmount = 0.0f;
		m_fExBigNetTotalAmountProportion = 0.0f;
		m_fBigBuyAmount = 0.0f;
		m_fBigSellAmount = 0.0f;
		m_fBigNetAmount = 0.0f;
		m_fBigNetAmountProportion = 0.0f;
		m_fBigNetTotalAmount = 0.0f;
		m_fBigNetTotalAmountProportion = 0.0f;
		m_fMidBuyAmount = 0.0f;
		m_fMidSellAmount = 0.0f;
		m_fMidNetAmount = 0.0f;
		m_fMidNetAmountProportion = 0.0f;
		m_fMidNetTotalAmount = 0.0f;
		m_fMidNetTotalAmountProportion = 0.0f;
		m_fSmallBuyAmount = 0.0f;
		m_fSmallSellAmount = 0.0f;
		m_fSmallNetAmount = 0.0f;
		m_fSmallNetAmountProportion = 0.0f;
		m_fSmallNetTotalAmount = 0.0f;
		m_fSmallNetTotalAmountProportion = 0.0f;
	}
	int32	m_uiMarket;					// 市场
	CString	m_StrCode;					// 代码
	int32	m_usShort;					// 短线状态	(E_StockState值)
	int32	m_usMid;					// 中线状态
	float	m_fCapticalFlow;			// 资金流向

	//
	float m_fAllNetAmount;			// 净流入(万元)

	//
	float m_fExBigBuyAmount;		// 超大单流入(万元)
	float m_fExBigSellAmount;		// 超大单流出(万元)
	float m_fExBigNetAmount;		// 超大单净额(万元)
	float m_fExBigNetAmountProportion;		// 超大单净额占比
	float m_fExBigNetTotalAmount;			// 超大单总额(万元)
	float m_fExBigNetTotalAmountProportion;	// 超大单总额占比

	//
	float m_fBigBuyAmount;		// 大单流入(万元)
	float m_fBigSellAmount;		// 大单流出(万元)
	float m_fBigNetAmount;		// 大单净额(万元)
	float m_fBigNetAmountProportion;		// 大单净额占比
	float m_fBigNetTotalAmount;				// 大单总额(万元)
	float m_fBigNetTotalAmountProportion;	// 大单总额占比

	//
	float m_fMidBuyAmount;		// 中单流入(万元)
	float m_fMidSellAmount;		// 中单流出(万元)
	float m_fMidNetAmount;		// 中单净额(万元)
	float m_fMidNetAmountProportion;		// 中单净额占比
	float m_fMidNetTotalAmount;				// 中单总额(万元)
	float m_fMidNetTotalAmountProportion;	// 中单总额占比

	//
	float m_fSmallBuyAmount;	// 小单流入(万元)
	float m_fSmallSellAmount;	// 小单流出(万元)
	float m_fSmallNetAmount;	// 小单净额(万元)
	float m_fSmallNetAmountProportion;		// 小单净额占比 
	float m_fSmallNetTotalAmount;			// 小单总额(万元)
	float m_fSmallNetTotalAmountProportion;	// 小单总额占比

}T_RespMerchIndex;

typedef struct T_RespMainMasukura
{
	T_RespMainMasukura()
	{					
		m_uiMarket = 0;;						
		m_StrCode = L"";		

		m_fMasukuraProportion = 0.0;			
		m_iRanked = 0;						
		m_fRise = 0.0;	
		m_f2DaysMasukuraProportion = 0.0;	
		m_i2DaysRanked = 0;					
		m_f2DaysRise = 0.0;	
		m_f3DaysMasukuraProportion = 0.0;	
		m_i3DaysRanked = 0;					
		m_f3DaysRise = 0.0;					
		m_f5DaysMasukuraProportion = 0.0;;		
		m_i5DaysRanked = 0;					
		m_f5DaysRise = 0.0;;					
		m_f10DaysMasukuraProportion = 0.0;;	
		m_i10DaysRanked = 0;				
		m_f10DaysRise = 0.0;;					
	}

	int32		m_uiMarket;						// 市场
	CString 	m_StrCode;						// 代码

	float		m_fMasukuraProportion;			// 今日增仓占比
	int32		m_iRanked;						// 今日排名
	float		m_fRise;						// 今日涨幅

	float		m_f2DaysMasukuraProportion;		// 2日增仓占比
	int32		m_i2DaysRanked;					// 2日排名
	float		m_f2DaysRise;					// 2日涨幅

	float		m_f3DaysMasukuraProportion;		// 3日增仓占比
	int32		m_i3DaysRanked;					// 3日排名
	float		m_f3DaysRise;					// 3日涨幅

	float		m_f5DaysMasukuraProportion;		// 5日增仓占比
	int32		m_i5DaysRanked;					// 5日排名
	float		m_f5DaysRise;					// 5日涨幅

	float		m_f10DaysMasukuraProportion;	// 10日增仓占比
	int32		m_i10DaysRanked;				// 10日排名
	float		m_f10DaysRise;					// 10日涨幅

}T_RespMainMasukura;

// 板块数据的回包结构
typedef struct T_LogicBlock
{
	T_LogicBlock()
	{
		m_iBlockId = INT_MIN;
		m_iRiseCounts = 0;
		m_iFallCounts = 0;
		m_iRiseDays   = 0;
		m_fAvgRise = m_fWeightRise = m_fTotalAmount = m_fChange = m_fPEratio = m_fMarketRate = m_fCapticalFlow = m_fMerchRise =
			m_fRiseRate = m_fMainNetVolume =m_fMainAmount = m_fVolumeRate = m_fAllVolume = m_fAllValue = m_fCircValue =0.0f;
	}
	int32	m_iBlockId;
	float	m_fAvgRise;			// 均涨幅
	float	m_fWeightRise;		// 权涨幅
	
	float	m_fTotalAmount;		// 总成交额
	float	m_fChange;			// 换手率
	float	m_fPEratio;			// 市盈率
	CString	m_StrMerchName;		// 领涨股票
	float   m_fMerchRise;		// 领涨股票涨幅
	int32   m_iRiseCounts;		// 涨股
	int32   m_iFallCounts;		// 跌股
	float	m_fMarketRate;		// 市场比
	float	m_fCapticalFlow;	// 资金流向
	int32   m_iRiseDays;		// 连涨天数
	float	m_fRiseRate;		// 涨速
	float	m_fMainNetVolume;	// 主力净量
	float	m_fMainAmount;		// 主力金额
	float	m_fVolumeRate;		// 量比
	float	m_fAllVolume;		// 总手
	float	m_fAllValue;		// 总市值
	float	m_fCircValue;		// 流通市值
}T_LogicBlock;

// 主力监控的请求结构(短线监控请求使用该结构)
typedef struct T_ReqMainMonitor
{
	int32			m_iCount;		// 从最新时间开始向前请求多少条数据
	
	int32			m_iBlockId;		// 板块ID
	CString			m_StrMerchCode;	// 商品代码 (如果传空值, 就返回这个板块所有商品的数据)
}T_ReqMainMonitor;

// 主力监控的回包结构
typedef struct T_RespMainMonitor
{
public:
	T_RespMainMonitor& operator=(const T_RespMainMonitor& Src)
	{
		if ( &Src == this )
		{
			return *this;
		}

		m_Time	  = Src.m_Time;
		m_fPrice  = Src.m_fPrice;
		m_fCounts = Src.m_fCounts;
		m_uBuy    = Src.m_uBuy;

		return *this;
	}

	long	m_Time;			// 时间
	float	m_fPrice;		// 价格
	float	m_fCounts;		// 手数
	u8		m_uBuy;			// 主动买盘 or 主动卖盘
	
}T_RespMainMonitor;

// 主力监控的数据类型
typedef std::vector<T_RespMainMonitor> MonitorArray;

typedef struct T_MainMonitorNode
{
	CMerchKey		m_MerchKey;
	MonitorArray	m_aMainMonitor;
}T_MainMonitorNode;

typedef std::vector<T_MainMonitorNode> mapMainMonitor;


// 短线监控的回包结构
typedef struct T_RespShortMonitor
{
public:
	T_RespShortMonitor& operator=(const T_RespShortMonitor& Src)
	{
		if ( &Src == this )
		{
			return *this;
		}

		m_Time	  = Src.m_Time;
		m_fPrice  = Src.m_fPrice;
		m_fCounts = Src.m_fCounts;
		m_uBuy    = Src.m_uBuy;
		m_uMonitorType = Src.m_uMonitorType;
		m_fIncRate = Src.m_fIncRate;

		return *this;
	}

	bool32 operator==(const T_RespShortMonitor& Src) const
	{
		return m_Time == Src.m_Time
		&& m_fPrice  == Src.m_fPrice
		&& m_fCounts == Src.m_fCounts
		&& m_uBuy == Src.m_uBuy
		&& m_uMonitorType == Src.m_uMonitorType
		&& m_fIncRate == Src.m_fIncRate;
	}

	long	m_Time;			// 时间
	float	m_fPrice;		// 价格
	float	m_fCounts;		// 手数
	u8		m_uBuy;			// 主动买盘 or 主动卖盘
	u8      m_uMonitorType; // 监控类型
	float   m_fIncRate;     // 涨跌幅

}T_RespShortMonitor;

// 主力监控的数据类型
typedef std::vector<T_RespShortMonitor> ShortMonitorArray;

typedef struct T_ShortMonitorNode
{
	CMerchKey		m_MerchKey;
	ShortMonitorArray	m_aShortMonitor;
}T_ShortMonitorNode;

typedef std::vector<T_ShortMonitorNode> mapShortMonitor;

// 分笔额外数据的结构
typedef struct T_TickEx
{
	int32	m_iMarketId;			// 市场
	CString	m_StrMerchCode;			// 代码
	
	float	m_fExBigBuyVol;			// 超大单买入量（万股）
	float	m_fExBigSellVol;		// 超大单卖出量（万股）
	
	float	m_fBigBuyVol;			// 大单买入量（万股）
	float	m_fBigSellVol;			// 大单卖出量（万股）
	
	float	m_fMidBuyVol;			// 中单买入量（万股）
	float	m_fMidSellVol;			// 中单卖出量（万股）
	
	float	m_fSmallBuyVol;			// 小单买入量（万股）
	float	m_fSmallSellVol;		// 小单卖出量（万股）
	
	float	m_fMnSBuyVol;			// 中小单买入量（万股）
	float	m_fMnSSellVol;			// 中小单卖出量（万股）
	
	float	m_fExBigBuyAmount;		// 超大单买入额（万元）
	float	m_fExBigSellAmount;		// 超大单卖出额（万元）
	
	float	m_fBigBuyAmount;		// 大单买入额（万元）				
	float	m_fBigSellAmount;		// 大单卖出额（万元）			

	float	m_fMidBuyAmount;		// 中单买入额（万元）
	float	m_fMidSellAmount;		// 中单卖出额（万元）
	
	float	m_fSmallBuyAmount;		// 小单买入额（万元）
	float	m_fSmallSellAmount;		// 小单卖出额（万元）
	
	float	m_fAllBuyAmount;		// 买入总额（万元）
	float	m_fAllSellAmount;		// 卖出总额（万元）
	
	float	m_fAmountPerTrans;		// 每笔金额（万元）
	float	m_fStocksPerTrans;		// 每笔股数（股）
	
	float	m_fPriceNew;			// 最新
	float	m_fExBigChange;			// 超大单换手率
	float	m_fBigChange;			// 大单换手率
	float	m_fBigBuyChange;		// 买入大单换手率
	uint32	m_uiTradeTimes;			// 成交笔数
	
	float	m_fBuyRate;				// 买入比例

	T_TickEx()
	{	
		m_iMarketId			= INT_MIN;
		m_StrMerchCode		= L"";

		m_fExBigBuyVol		= 0.;	
		m_fExBigSellVol		= 0.;	
		
		m_fBigBuyVol        = 0.;	
		m_fBigSellVol       = 0.;	
		
		m_fMidBuyVol        = 0.;	
		m_fMidSellVol       = 0.;	
		
		m_fSmallBuyVol      = 0.;	
		m_fSmallSellVol     = 0.;	
		
		m_fMnSBuyVol        = 0.;	
		m_fMnSSellVol		= 0.;	
		
		m_fExBigBuyAmount   = 0.;	
		m_fExBigSellAmount  = 0.;	
		
		m_fBigBuyAmount     = 0.;	
		m_fBigSellAmount    = 0.;	
		
		m_fMidBuyAmount     = 0.;	
		m_fMidSellAmount    = 0.;	
		
		m_fSmallBuyAmount   = 0.;	
		m_fSmallSellAmount  = 0.;	
		
		m_fAllBuyAmount     = 0.;	
		m_fAllSellAmount    = 0.;	
		
		m_fAmountPerTrans   = 0.;	
		m_fStocksPerTrans   = 0.;	
		
		m_fPriceNew         = 0.;	
		m_fExBigChange      = 0.;	
		m_fBigChange        = 0.;	
		m_fBigBuyChange     = 0.;	
		m_uiTradeTimes      = 0;	
		
		m_fBuyRate			= 0.;		
	}

}T_TickEx;

typedef struct T_ReqPushMerchData
{
	int32	m_iMarket;		// 市场
	CString m_StrCode;		// 代码
	u32		m_uType;		// 推送类型可以或的方式表示多个
	
}T_ReqPushMerchData;

typedef struct T_ReqPushBlockData
{
	int32	m_iBlockId;				// 市场	
	E_ChooseStockType m_eType;		// 推送类型可以或的方式表示多个

}T_ReqPushBlockData;

////////////////////////////////////////////////////////////////////////////////
class CMmiCommBasePlugIn;

// 复制一个对象出来
DATAINFO_DLL_EXPORT CMmiCommBasePlugIn*	 NewCopyCommPlugInReqObject(CMmiCommBasePlugIn *pCommBase);

// 判读两个请求是否同类型同对象
DATAINFO_DLL_EXPORT bool32				IsSamePlugInReqObject(const CMmiCommBasePlugIn *pCommSrc, const CMmiCommBasePlugIn *pCommDst);		


/************************************************************************************************************************************/
// 插件模块的请求基类
class DATAINFO_DLL_EXPORT CMmiCommBasePlugIn : public CMmiCommBase
{
public:
	CMmiCommBasePlugIn() { m_eCommType = ECTReqPlugIn; }
	virtual ~CMmiCommBasePlugIn() {}
	virtual CString GetSummary() { return L""; }

public:
	E_CommTypePlugIn m_eCommTypePlugIn;
};

/**********************************************************************************************************/
// 请求商品的选股字段 [短线, 中线] 值. 服务器每 5 分钟计算一次.
class DATAINFO_DLL_EXPORT CMmiReqMerchIndex : public CMmiCommBasePlugIn
{
public:
	CMmiReqMerchIndex() { m_eCommTypePlugIn = ECTPIReqMerchIndex; }
	virtual CString	GetSummary();
	
	//
	CMmiReqMerchIndex(const CMmiReqMerchIndex& Src);
	CMmiReqMerchIndex& operator= (const CMmiReqMerchIndex& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 商品的选股请求的回包
class DATAINFO_DLL_EXPORT CMmiRespMerchIndex : public CMmiCommBasePlugIn
{
public:
	CMmiRespMerchIndex() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMerchIndex; }
	virtual CString	GetSummary();

public:
	CArray<T_RespMerchIndex, T_RespMerchIndex> m_aMerchIndexList;	
};

// 请求主力增仓数据
class DATAINFO_DLL_EXPORT CMmiReqMainMasukura : public CMmiCommBasePlugIn
{
public:
	CMmiReqMainMasukura() { m_eCommTypePlugIn = ECTPIReqMainMasukura; }
	virtual CString	GetSummary();

	//
	CMmiReqMainMasukura(const CMmiReqMainMasukura& Src);
	CMmiReqMainMasukura& operator= (const CMmiReqMainMasukura& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 主力增仓请求的回包
class DATAINFO_DLL_EXPORT CMmiRespMainMasukura : public CMmiCommBasePlugIn
{
public:
	CMmiRespMainMasukura() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMainMasukura; }
	virtual CString	GetSummary();

public:
	CArray<T_RespMainMasukura, T_RespMainMasukura> m_aMainMasukuraList;	
};

// 请求板块数据 ( 均涨幅, 权涨幅, 领涨股票等等...)
class DATAINFO_DLL_EXPORT CMmiReqLogicBlock : public CMmiCommBasePlugIn
{
public:
	CMmiReqLogicBlock() { m_eCommTypePlugIn = ECTPIReqBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqLogicBlock(const CMmiReqLogicBlock& Src);
	CMmiReqLogicBlock& operator=(const CMmiReqLogicBlock& Src);

public:
	int32			m_iLogicBlockId;		// 市场编号

	//
	CArray<int32, int32>	m_aBlockMore;			
};

// 回包
class DATAINFO_DLL_EXPORT CMmiRespLogicBlock : public CMmiCommBasePlugIn
{
public:
	CMmiRespLogicBlock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespBlock; };
	virtual CString	GetSummary();
	
public:
	CArray<T_LogicBlock, T_LogicBlock&> m_aBlockData;
};

// 请求商品排行
class DATAINFO_DLL_EXPORT CMmiReqMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqMerchSort() { m_eCommTypePlugIn = ECTPIReqMerchSort; }
	virtual CString GetSummary() {return CString();}

	CMmiReqMerchSort(const CMmiReqMerchSort& Src);
	CMmiReqMerchSort& operator=(const CMmiReqMerchSort& Src);

public:
	int32			m_iMarketId;		// 市场编号
	bool32			m_bDescSort;		// 升序,降序
	int32			m_iStart;			// 起始位置
	int32			m_iCount;			// 商品个数
	
	E_ReportSortEx	m_eReportSortType;	// 排序类型
};

// 请求商品周期排行
class DATAINFO_DLL_EXPORT CMmiReqPeriodMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqPeriodMerchSort() { m_eCommTypePlugIn = ECTPIReqPeriodMerchSort; }
	~CMmiReqPeriodMerchSort() {}
	virtual CString GetSummary() {return CString();}

	CMmiReqPeriodMerchSort(const CMmiReqPeriodMerchSort& Src);
	CMmiReqPeriodMerchSort& operator=(const CMmiReqPeriodMerchSort& Src);

public:
	int32			m_iMarketId;		// 市场编号
	bool32			m_bDescSort;		// 升序,降序
	int32			m_iStart;			// 起始位置
	int32			m_iCount;			// 商品个数
    uint8           m_uiPeriod;         // 周期
	E_ReportSortEx	m_eReportSortType;	// 排序类型
};

// 应答商品排行榜数据
class DATAINFO_DLL_EXPORT CMmiRespMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespMerchSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMerchSort; }
	virtual ~CMmiRespMerchSort(){};
	
public:
	virtual CString GetSummary();
	
public:
	int32			m_iMarketId;				// 市场编号
	
	E_ReportSortEx	m_eReportSortType;
	bool32			m_bDescSort;				// 降序排行？
	int32			m_iStart;
	
	// 商品列表
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// 商品5档行情数据
};

// 应答带周期的商品排行榜数据
class DATAINFO_DLL_EXPORT CMmiRespPeriodMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespPeriodMerchSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPeriodMerchSort; }
	virtual ~CMmiRespPeriodMerchSort(){};

public:
	virtual CString GetSummary() { return CString(); }

public:
	int32			m_iMarketId;				// 市场编号

	E_ReportSortEx	m_eReportSortType;
	bool32			m_bDescSort;				// 降序排行？
	int32			m_iStart;
	uint8           m_iPeriod;

	// 商品列表
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// 商品5档行情数据
};

// 请求板块排行
class DATAINFO_DLL_EXPORT CMmiReqBlockSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqBlockSort() { m_eCommTypePlugIn = ECTPIReqBlockSort; }
	virtual CString GetSummary() {return CString();}

	CMmiReqBlockSort(const CMmiReqBlockSort& Src);
	CMmiReqBlockSort& operator=(const CMmiReqBlockSort& Src);

public:
	int32			m_iMarketId;		// 市场编号
	bool32			m_bDescSort;		// 升序,降序
	int32			m_iStart;			// 起始位置
	int32			m_iCount;			// 商品个数
	
	E_BlockSort		m_eBlockSortType;	// 排序类型
};	

class DATAINFO_DLL_EXPORT CMmiRespBlockSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespBlockSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespBlockSort; }
	virtual CString GetSummary();
	
public:
	int32			m_iBlockId;			// ID
	bool32			m_bDescSort;		// 升序,降序
	int32			m_iStart;			// 起始位置
	int32			m_iCount;			// 商品个数
	
	E_BlockSort		m_eBlockSortType;	// 排序类型

	CArray<int32, int32>	m_aBlockIDs;	// 排好的板块ID
};
	
// 请求主力监控数据 
class DATAINFO_DLL_EXPORT CMmiReqMainMonitor: public CMmiCommBasePlugIn
{
public:
	CMmiReqMainMonitor() { m_eCommTypePlugIn = ECTPIReqMainMonitor; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqMainMonitor(const CMmiReqMainMonitor& Src);
	CMmiReqMainMonitor& operator=(const CMmiReqMainMonitor& Src);

public:
	int32			m_iCount;		// 从最新时间开始向前请求多少条数据
	
	int32			m_iBlockId;		// 板块ID
	CString			m_StrMerchCode;	// 商品代码 (如果传空值, 就返回这个板块所有商品的数据)
		
	//
	CArray<T_ReqMainMonitor, T_ReqMainMonitor&>	m_aReqMore;	// 可以同时请求多个商品	
};

// 主力监控回包
class DATAINFO_DLL_EXPORT CMmiRespMainMonitor : public CMmiCommBasePlugIn
{
public:
	CMmiRespMainMonitor() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMainMonitor; };
	CMmiRespMainMonitor(const CMmiRespMainMonitor& Src);	
	CMmiRespMainMonitor& operator=(const CMmiRespMainMonitor& Src);

	virtual CString	GetSummary();

public:
	mapMainMonitor	m_mapMainMonitorData;
};

// 请求短线监控数据 
class DATAINFO_DLL_EXPORT CMmiReqShortMonitor: public CMmiCommBasePlugIn
{
public:
	CMmiReqShortMonitor() { m_eCommTypePlugIn = ECTPIReqShortMonitor; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqShortMonitor(const CMmiReqShortMonitor& Src);
	CMmiReqShortMonitor& operator=(const CMmiReqShortMonitor& Src);

public:
	int32			m_iCount;		// 从最新时间开始向前请求多少条数据

	int32			m_iBlockId;		// 板块ID
	CString			m_StrMerchCode;	// 商品代码 (如果传空值, 就返回这个板块所有商品的数据)

	//
	CArray<T_ReqMainMonitor, T_ReqMainMonitor&>	m_aReqMore;	// 可以同时请求多个商品	
};

// 短线监控回包
class DATAINFO_DLL_EXPORT CMmiRespShortMonitor : public CMmiCommBasePlugIn
{
public:
	CMmiRespShortMonitor() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespShortMonitor; }
	CMmiRespShortMonitor(const CMmiRespShortMonitor& Src);	
	CMmiRespShortMonitor& operator=(const CMmiRespShortMonitor& Src);

	virtual CString	GetSummary() {return CString();}

public:
	mapShortMonitor	m_mapShortMonitorData;
};

// 请求分笔统计
class DATAINFO_DLL_EXPORT CMmiReqTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqTickEx::CMmiReqTickEx() { m_eCommTypePlugIn = ECTPIReqTickEx; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqTickEx(const CMmiReqTickEx& Src);
	CMmiReqTickEx& operator=(const CMmiReqTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	
	CArray<CMerchKey, CMerchKey>	m_aMerchMore;
};

// 请求带周期的分笔统计
class DATAINFO_DLL_EXPORT CMmiReqPeriodTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqPeriodTickEx::CMmiReqPeriodTickEx() { m_eCommTypePlugIn = ECTPIReqPeriodTickEx; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPeriodTickEx(const CMmiReqPeriodTickEx& Src);
	CMmiReqPeriodTickEx& operator=(const CMmiReqPeriodTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	uint8       m_uiPeriod;                // 周期

	CArray<CPeriodMerchKey, CPeriodMerchKey>	m_aMerchMore;
};

// 分笔统计回包
class DATAINFO_DLL_EXPORT CMmiRespTickEx : public CMmiCommBasePlugIn	
{
public:
	CMmiRespTickEx::CMmiRespTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespTickEx; }
	virtual CString	GetSummary();
	
public:
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// 多日分笔统计回包
class DATAINFO_DLL_EXPORT CMmiRespPeriodTickEx : public CMmiCommBasePlugIn	
{
public:
	CMmiRespPeriodTickEx::CMmiRespPeriodTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPeriodTickEx; }
	virtual CString	GetSummary();

public:
	uint8 m_iPeriod; 
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// 请求历史分笔
class DATAINFO_DLL_EXPORT CMmiReqHistoryTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqHistoryTickEx() { m_eCommTypePlugIn = ECTPIReqHistoryTickEx; }
	virtual CString GetSummary() { return L""; }
	
	CMmiReqHistoryTickEx(const CMmiReqHistoryTickEx& Src);
	CMmiReqHistoryTickEx& operator= (const CMmiReqHistoryTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
};

// 历史分笔回包
class DATAINFO_DLL_EXPORT CMmiRespHistoryTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiRespHistoryTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespHistoryTickEx; }
	virtual CString GetSummary() { return L""; }

public:
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// 请求选股集合
class DATAINFO_DLL_EXPORT CMmiReqChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiReqChooseStock::CMmiReqChooseStock() { m_eCommTypePlugIn = ECTPIReqChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqChooseStock(const CMmiReqChooseStock& Src);
	CMmiReqChooseStock& operator=(const CMmiReqChooseStock& Src);

public:
	u16		m_uTypes;			// 选股类型 E_ChooseStockType交集 s
};

// 应答
class DATAINFO_DLL_EXPORT CMmiRespChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiRespChooseStock::CMmiRespChooseStock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespChooseStock; }
	virtual CString	GetSummary();
	
public:
	u16		m_uTypes;			// 选股类型 E_ChooseStockType交集 s

	CArray<CMerchKey, CMerchKey>	m_aMerchs;	// 商品集合
};



// 请求选股集合扩展
class DATAINFO_DLL_EXPORT CMmiReqCRTEStategyChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiReqCRTEStategyChooseStock::CMmiReqCRTEStategyChooseStock() { m_eCommTypePlugIn = ECTPIReqCRTEStategyChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqCRTEStategyChooseStock(const CMmiReqCRTEStategyChooseStock& Src);
	CMmiReqCRTEStategyChooseStock& operator=(const CMmiReqCRTEStategyChooseStock& Src);

public:
	u32		m_uTypes;			// 选股类型 E_ChooseStockType交集 s
};

// 请求选股集合扩展 应答
class DATAINFO_DLL_EXPORT CMmiRespCRTEStategyChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiRespCRTEStategyChooseStock::CMmiRespCRTEStategyChooseStock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespCRTEStategyChooseStock; }
	virtual CString	GetSummary();

public:
	u32		m_uTypes;			// 选股类型 E_ChooseStockType交集 s

	CArray<CMerchKey, CMerchKey>	m_aMerchs;	// 商品集合
};


// 请求大盘状态
class DATAINFO_DLL_EXPORT CMmiReqDapanState : public CMmiCommBasePlugIn
{
public:
	CMmiReqDapanState() { m_eCommTypePlugIn = ECTPIReqDapanState; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqDapanState(const CMmiReqDapanState& Src);
	CMmiReqDapanState& operator=(const CMmiReqDapanState& Src);
};

// 回包
class DATAINFO_DLL_EXPORT CMmiRespDapanState : public CMmiCommBasePlugIn
{
public:
	CMmiRespDapanState() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespDapanState; }
	virtual CString	GetSummary();

public:
	E_DpnxState	m_eDapanState;					// 大盘状态
}; 

// 请求选股状态
class DATAINFO_DLL_EXPORT CMmiReqChooseStockStatus : public CMmiCommBasePlugIn
{
public:
	CMmiReqChooseStockStatus() { m_eCommTypePlugIn = ECTPIReqChooseStockStatus; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqChooseStockStatus(const CMmiReqChooseStockStatus& Src);
	CMmiReqChooseStockStatus& operator=(const CMmiReqChooseStockStatus& Src);
};

// 回包
class DATAINFO_DLL_EXPORT CMmiRespChooseStockStatus : public CMmiCommBasePlugIn
{
public:
	CMmiRespChooseStockStatus() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespChooseStockStatus; }
	virtual CString	GetSummary();

public:
	u32	m_uStatus;					// 大盘状态
}; 



// 请求多空资金数据
class DATAINFO_DLL_EXPORT CMmiReqDKMoney : public CMmiCommBasePlugIn
{
public:
	CMmiReqDKMoney() { m_eCommTypePlugIn = ECTPIReqDKMoney; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqDKMoney(const CMmiReqDKMoney& Src);
	CMmiReqDKMoney& operator=(const CMmiReqDKMoney& Src);
public:
	int32		m_iMarketID;	
	CString		m_StrMerchCode;		// 商品
	u16			m_uType;			// 推送类型 E_ChooseStockType 的 "|" 操作
};

// 回包
class DATAINFO_DLL_EXPORT CMmiRespDKMoney : public CMmiCommBasePlugIn
{
public:
	CMmiRespDKMoney() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespDKMoney; }
	virtual CString	GetSummary() {return CString();};

public:
	float m_fITBF;//主动性买入资金
	float m_fITSF;//主动性卖出资金
	float m_fMultSide;//多方占比
	float m_fEmpSide;//空方占比
}; 



// 请求推送单个商品相关的推送数据, 包括 	ECSTChooseStock ECSTMainMonitor ECSTTickEx
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInMerchData(){ m_eCommTypePlugIn = ECTPIReqAddPushMerchIndex; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPushPlugInMerchData(const CMmiReqPushPlugInMerchData& Src);
	CMmiReqPushPlugInMerchData& operator=(const CMmiReqPushPlugInMerchData& Src);

public:
	int32		m_iMarketID;			
	CString		m_StrMerchCode;		// 商品
	u32			m_uType;			// 推送类型 E_ChooseStockType 的 "|" 操作

	CArray<T_ReqPushMerchData, T_ReqPushMerchData&>	m_aReqMore;
};

// 请求推送板块数据
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInBlockData(){ m_eCommTypePlugIn = ECTPIReqAddPushBlock;  m_eType = ECSTBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqPushPlugInBlockData(const CMmiReqPushPlugInBlockData& Src);
	CMmiReqPushPlugInBlockData& operator=(const CMmiReqPushPlugInBlockData& Src);

public:
	int32	m_iBlockID;
	E_ChooseStockType		m_eType;			// 推送类型 ECSTBlock

	CArray<T_ReqPushBlockData, T_ReqPushBlockData&> m_aReqMore;
};

// 请求推送条件选股的集合 ECSTShort ECSTMid ECSTMrjx ECSTDpcl ECSTJglt ECSTXpjs ECSTZjzt ECSTNsql ECSTCdft
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInChooseStock: public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInChooseStock() { m_eCommTypePlugIn = ECTPIReqAddPushChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPushPlugInChooseStock(const CMmiReqPushPlugInChooseStock& Src);
	CMmiReqPushPlugInChooseStock& operator=(const CMmiReqPushPlugInChooseStock& Src);

public:
	//
	u32			m_uType;			//  选股类型的 "|" 操作
};

// 同步推送商品相关数据, 用当前这些商品替换原来的推送商品
class DATAINFO_DLL_EXPORT CMmiReqUpdatePushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqUpdatePushPlugInMerchData(E_CommTypePlugIn eType)
	{
		if ( eType != ECTPIReqUpdatePushMerchIndex && eType != ECTPIReqUpdatePushTickEx && eType != ECTPIReqUpdatePushMainMonitor )
		{
			ASSERT(0);
		}

		m_eCommType = ECTReqPlugIn; 
		m_eCommTypePlugIn = eType;
	}

	virtual CString	GetSummary(){return CString();}
	
	CMmiReqUpdatePushPlugInMerchData(const CMmiReqUpdatePushPlugInMerchData& Src);
	CMmiReqUpdatePushPlugInMerchData& operator=(const CMmiReqUpdatePushPlugInMerchData& Src);
	
public:
	CArray<CMerchKey, CMerchKey> m_aMerchMore;
};

// 同步推送板块数据推送
class DATAINFO_DLL_EXPORT CMmiReqUpdatePushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqUpdatePushPlugInBlockData(){ m_eCommTypePlugIn = ECTPIReqUpdatePushBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqUpdatePushPlugInBlockData(const CMmiReqUpdatePushPlugInBlockData& Src);
	CMmiReqUpdatePushPlugInBlockData& operator=(const CMmiReqUpdatePushPlugInBlockData& Src);

public:	
	CArray<int32, int32>	m_aBlockIDs;		// 板块id
};

// 同步推送选股集合的请求跟添加推送一样
typedef CMmiReqPushPlugInChooseStock CMmiReqUpdatePushPlugInChooseStock;

// 删除商品数据推送
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInMerchData(E_CommTypePlugIn eType) { m_eCommTypePlugIn = ECTPIReqDeletePushMerchData; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqRemovePushPlugInMerchData(const CMmiReqRemovePushPlugInMerchData& Src);
	CMmiReqRemovePushPlugInMerchData& operator=(const CMmiReqRemovePushPlugInMerchData& Src);

public:	
	E_ChooseStockType				m_eType;
	CArray<CMerchKey, CMerchKey>	m_aMerchs;
};

// 删除板块数据推送
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInBlockData() { m_eCommTypePlugIn = ECTPIReqDeletePushBlockData; m_eType = ECSTBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqRemovePushPlugInBlockData(const CMmiReqRemovePushPlugInBlockData& Src);
	CMmiReqRemovePushPlugInBlockData& operator=(const CMmiReqRemovePushPlugInBlockData& Src);
	
public:		
	E_ChooseStockType		m_eType;
	CArray<int32, int32>	m_aBlockIDs;
};

// 删除选股推送
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInChooseStock: public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInChooseStock() { m_eCommTypePlugIn = ECTPIReqDeletePushChooseStock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqRemovePushPlugInChooseStock(const CMmiReqRemovePushPlugInChooseStock& Src);
	CMmiReqRemovePushPlugInChooseStock& operator=(const CMmiReqRemovePushPlugInChooseStock& Src);
	
public:		
	E_ChooseStockType	m_eType;
};

// 推送相关请求的回包
class DATAINFO_DLL_EXPORT CMmiRespPlugInPushFlag : public CMmiCommBasePlugIn
{
public:
	CMmiRespPlugInPushFlag() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPushFlag; }
	virtual CString	GetSummary() { return L""; }

	UINT m_uiTag;
};

#endif // _PLUG_IN_STRUCT_H_
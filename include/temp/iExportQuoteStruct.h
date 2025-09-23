#ifndef _IEXPORTQUOTESTRUCT_H_
#define _IEXPORTQUOTESTRUCT_H_

// 财富金盾客户端提供给外部合作dll的行情导出接口  - 结构体定义
#pragma pack(push, 1)

// 注意命名空间
// 所有出现char字串都是utf8编码，但是由于code一般不出现中文，所以可以当成ascii使用
namespace cfjdExport
{
	typedef unsigned long data32;

	enum{
		MAX_EXPORT_CODE_SIZE		= 32
	};
	enum{
		MAX_EXPORT_NAME_SIZE		= 32
	};
	enum{
		MAX_PRICEVP_COUNT = 20
	};

	// 商品数据类型
	enum E_ExportDataType
	{
		EEDTPrice = 1,	// 价格数据类型
		EEDTKLine = 2,	// K线数据类型
		EEDTTick  = 4,	// 分笔数据类型
	};

	// 请求行情数组型数据时，要求的范围类型
	enum E_ExportReqRangeType
	{
		EERRTRangeBE,	// 1值和2值为时间
		EERRTRangeBC,	// 1值为时间，2值为后祥的个数	
		EERRTRangeEC,	// 1值为时间，2值为前向的个数
	};

	// 请求行情数组型数据时，指定请求的数据范围
	typedef struct _ExportReqDataRange
	{
		int		iRangeType; // E_ExportReqRangeType 范围类型，影响开始和结束的值的定义

		struct BE 
		{
			data32	uBeginTime;		// 开始时间(含)
			data32	uEndTime;		// 结束时间(含)
		};
		struct BC
		{
			data32	uBeginTime;		// 开始时间(含)
			data32	uFollowCount;	// 指定时间后的若干K线
		};
		struct CE
		{
			data32	uEndTime;		// 结束时间(含)
			data32	uFrontCount;	// 指定时间前的若干K线
		};
		union
		{
			BE		stBE;
			BC		stBC;
			CE		stCE;
		} unRange;		// 实际范围
	}T_ExportReqDataRange;

	// 通知客户端包含某数组型行情数据时，指示当前存在该数据的范围
	typedef struct _ExportDataRange
	{
		data32		uBeginTime;		// 数据开始时间(含)
		data32		uEndTime;		// 数据结束时间(含)
		data32		uCount;			// 数据共有多少条
	}T_ExportDataRange;

	// 商品唯一标识
	typedef struct _ExportMerchKey
	{
		int		iMarket;							// 所在市场id
		char	szMerchCode[MAX_EXPORT_CODE_SIZE];	// 商品行情代码
	}T_ExportMerchKey;

	//////////////////////////////////////////////////////////////////////////
	// 市场部分
	// 市场类型
	enum E_ExportMarketType
	{
		EEMTFuturesCn = 0,		// 国内期货	
		EEMTStockCn,				// 国内证券
		EEMTMony,				// 外汇栏目
		EEMTExp,					// 指数栏目
		EEMTStockHk,				// 港股栏目
		EEMTFuturesForeign,		// 国外期货
			
		// 
		EEMTWarrantCn,			// 国内权证
		EEMTWarrantHk,			// 香港权证
			
		//
		EEMTFutureRelaMonth,		// 相对月期货
		EEMTFutureSpot,			// 现货
		EEMTExpForeign,			// 国外指数
		EEMTFutureGold,			// 黄金期货
	};

	enum E_ExportMarketStatus
	{
		EEMSUnknown = 0,		// 未知状态
		EEMSIniting,			// 正在初始化(竞价?)
		EEMSOpen,				// 已开盘
		EEMSClose,				// 已收盘
	};

	// 市场信息中的开收结构体
	typedef struct _ExportMarketOpenCloseTime 
	{
		int		iOpenMinute;		// 格式：以分钟为单位， 相对于00:00分的值
		int		iCloseMinute;
	}T_ExportMarketOpenCloseTime;

	// 市场信息
	typedef struct _ExportMarketInfo
	{
		int		iBigMarket;	// 大市场
		int		iMarket;	// 市场
		int		iTimeZone;	// 市场所在时区
		int		iMarketType;	// 市场类型 E_ExportMarketType 类型

		char	szBourseCode[MAX_EXPORT_CODE_SIZE];	// 在所在交易所的市场代码
		char	szEnName[MAX_EXPORT_NAME_SIZE];	// 英文名称
		char	szCnName[MAX_EXPORT_NAME_SIZE];	// 中文名称

		int		iTimeInitializePerDay;		// 每天初始化时间。格式：以分钟为单位， 相对于00:00分的值
											// 客户端每天都需要对系统的商品列表做初始化， 
											// 该时间描述每次初始化的时间（格林威治时间）， 对于国内股市， 该时间即“集合竞价”时间

		int		iOpenCloseTimeCount;		// 有效开收盘段个数
		T_ExportMarketOpenCloseTime	astOpenCloseTimes[12];	// 每天的开收盘时间段，有效个数 iOpenCloseTimeCount
		

		int		iRecentTradingDayCount;		// 有效最近交易日数据个数
		data32	aiRecentTradingDay[20];		// 单位: time_t 秒，后两天的交易日和前的交易日(如今天明天,然后往昨天10个交易日)

		int		iSubMerchCount;				// 下属的商品的个数
	}T_ExportMarketInfo, *LPT_ExportMarketInfo;

	// 商品信息
	typedef struct _ExportMerchInfo
	{
		T_ExportMerchKey	stMerchKey;		// 商品标识
		
		char	szBourseCode[MAX_EXPORT_CODE_SIZE];	// 在所在交易所的代码(交易代码)
		char	szEnName[MAX_EXPORT_NAME_SIZE];	// 英文名称
		char	szCnName[MAX_EXPORT_NAME_SIZE];	// 中文名称
		
		int		iSaveDec;		// 保留小数点位
	}T_ExportMerchInfo, *LPT_ExportMerchInfo;

	//////////////////////////////////////////////////////////////////////////
	// 价格数据部分
	// 行情中买卖档数据
	typedef struct _ExportPriceVolume
	{
		float			fPrice;
		float			fVolume;
	}T_ExportPriceVolume;


	// 行情
	typedef struct _ExportQuotePrice
	{
		T_ExportMerchKey	stMerchKey;		// 商品标识
		
		data32	lTime;			// 时间
		data32	lMilliSecond;	// 毫秒

		float	fLastClose;		// 昨收价
		float	fLastAvg;		// 昨结算

		float	fNow;			// 最新价
		float	fOpen;			// 开盘
		float	fHigh;			// 最高价
		float	fAvg;			// 平均价
		float	fLow;			// 最低价
		
		float	fCurVolume;		// 现手
		float	fVolume;		// 成交量	
		float	fAmount;		// 总成交额

		float	fHoldPrev;		// 前持仓
		float	fCurHold;		// 增仓
		float	fHold;			// 持仓量
		
		float	fVolumeRate;	// 量比
		float	fBuyVolume;		// 内盘(今日合计), 内盘(按买价成交), 在开盘时要清空
		float	fSellVolume;	// 外盘(今日合计), 外盘(按卖价成交), 在开盘时要清空

		T_ExportPriceVolume astBuyVP[MAX_PRICEVP_COUNT];		// 买盘 买卖档
		T_ExportPriceVolume astSellVP[MAX_PRICEVP_COUNT];		// 卖盘 买卖档
				
		float	fPE;					// 市盈率
		float	fTradeRate;				// 换手率
		float	fRiseRate;				// 涨速(最新价和最近5分钟收盘价的比率)
		float	fMoneyIn;				// 资金流向

		float	fSellAmount;			// 外盘成交额	
		int		iSeason;				// 季度	
	}T_ExportQuotePrice, *LPT_ExportQuotePrice;

	//////////////////////////////////////////////////////////////////////////
	// K线部分
	// K线类型
	enum E_ExportKLineType
	{
		EEKTUnknown = 0,		// 未知类型
		EEKTMin,	// 分钟K线
		EEKT5Min,	// 5分钟
		EEKT60Min,	// 60分钟
		EEKTDay,	// 日K线
		EEKTMonth,	// 月K线

		EEKT10Min = 10,		// 10分钟线
		EEKT15Min,			// 15分钟线
		EEKTWeek,			// 周线
	};

	// K线数据
	typedef struct _ExportKLineUnit
	{
		data32	lTime;		// 时间
		float	fOpen;		// 开盘
		float	fHigh;		// 最高
		float	fLow;		// 最低
		float	fClose;		// 收盘
		float	fVolume;	// 成交量
		float	fAmount;	// 成交额
		float	fHold;		// 持仓
		float	fAvg;		// 结算价
		unsigned short	usUpCount;		// 上涨家数
		unsigned short	usDownCount;	// 下跌家数
	}T_ExportKLineUnit;

	//////////////////////////////////////////////////////////////////////////
	// 分笔部分
	// 分笔类型
	enum E_ExportTickType
	{
		EETTUnknown  = 0,	// 不明盘
		EETTBuy		= 1,	// 买价
		EETTSell	= 2,	// 卖价
		EETTTrade	= 4,	// 成交价
	};
	enum E_ExportTickTradeKind
	{
		EETTKUnKnown = 0,		// 其它
		EETTKBuy,				// 以买价成交,内盘
		EETTKSell,				// 以卖价成交,外盘
	};

	// 分笔
	typedef struct _ExportTickUnit
	{
		data32	lTime;			// 时间
		data32	lMilliSecond;	// 毫秒(高10位)(低6位表示索引区分相同毫秒时的数据)
		float	fPrice;		// 价格
		float	fVolume;	// 单笔成交量
		float	fAmount;	// 单笔成交额
		float	fHold;		// 总持仓量
		int		iTickType;	// E_ExportTickType类型 
		int		iTradeKind;	// E_ExportTickTradeKind类型 内外盘标记
	}T_ExportTickUnit;
}

#pragma pack(pop)
#endif //!_IEXPORTQUOTESTRUCT_H_
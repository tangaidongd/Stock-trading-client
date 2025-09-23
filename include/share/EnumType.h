#ifndef _ENUM_TYPE_H_
#define _ENUM_TYPE_H_
enum E_ReportType
{
	ERTFuturesCn = 0,		// 国内期货	
	ERTStockCn,				// 国内证券
	ERTMony,				// 外汇栏目
	ERTExp,					// 指数栏目
	ERTStockHk,				// 港股栏目
	ERTFuturesForeign,		// 国外期货
		// 
	ERTWarrantCn,			// 国内权证
	ERTWarrantHk,			// 香港权证
	
	//
	ERTFutureRelaMonth,		// 相对月期货
	ERTFutureSpot,			// 现货
	ERTExpForeign,			// 国外指数
	ERTFutureGold,			// 黄金期货
	// 
	ERTCustom,				// 自定义
	
	ERTNone,				// 无意义(空报价表) 所有在此之后新加的必须小心栏目设置对话框里面的各tab item必须跳过该表头
		// 
	ERTSelectStock,			// 条件选股表头(北京项目临时增加)	// 由于现在这个位置取代了以前的None位置，所以如果保存了空报价表，则会出错
	
	ERTFinance,            // 基本财务表头
	
	ERTMerchSort,          // 热门强龙表头

	ERTCapitalFlow, 
	ERTMainMasukura,

	//
	ERTCount				
};

// 请求时间方式
typedef enum E_ReqTimeType
{
	ERTYSpecifyTime = 0,		// 指定时间
	ERTYFrontCount,				// 倒数个数, 往时间更早的方向
	ERTYCount					// 顺数个数, 往时间更新的方向
}E_ReqTimeType;

// 错误码
typedef enum E_RespErrorCode
{
	ERENone = 0,				// 无错误
	EREServer,					// 服务器错误
	ERERequestError,			// 请求错误
	EREInvalidArgu,				// 参数错误
	EREInvalidRight,			// 用户无此数据权限
	ERENoBigMarket = 20,		// 未找到大市场
	ERENoMarket,				// 未找到市场
	ERENoSymbol,				// 未找到商品
	ERENeedAuth = 30,			// 为认证
	EREAuthError,				// 认证错误

	ERENewVersion = 200,		// 协议版本不对
	//
	ERECount
}E_RespErrorCode;

// 排序依据
typedef enum E_MerchReportField		
{
	EMRFRisePercent = 0,		// 涨跌幅
	EMRFRiseValue,				// 涨跌
	EMRFRiseRate,				// 涨速
	EMRFPriceNew,				// 最新价
	EMRFPriceAverage,			// 均价
	EMRFVolumeCur,				// 现手
	EMRFPriceBuy,				// 买价
	EMRFPriceSell,				// 卖价
	EMRFVolumeTotal,			// 总手
	EMRFPriceOpen,				// 今开
	EMRFPricePrevClose,			// 昨收价
	EMRFPriceHigh,				// 最高价
	EMRFPriceLow,				// 最低价
	EMRFAmountTotal,			// 总金额
	EMRFAmplitude,				// 振幅
	EMRFVolRatio,				// 量比
	EMRFBidRatio,				// 委比
	EMRFBidDifference,			// 委差
	EMRFIn,						// 内盘
	EMRFOut,					// 外盘
	EMRFInOutRatio,				// 内外盘比	
	
	EMRFRisePercent5Min,		// 5分钟涨幅
	EMRFDropPercent5Min,		// 5分钟跌幅

	EMRFPricePreAvg,			// 昨结算和昨均价
	EMRFHold,					// 持仓
	EMRFCurAmount,				// 单笔成交额
	EMRFCurHold,				// 增仓

	EMRFCapticalFlow,			// 资金流向
	EMRFPeRate,					// 市盈率
	EMRFTradeRate,				// 换手率
	EMRFIntensity,				// 强弱度

	// 财务数据排行
	EMRFAllStock,				// 总股本
	EMRFCircStock,				// 流通股本
	EMRFAllAsset,				// 总资产
	EMRFFlowDebt,				// 流动负债
	EMRFPerFund,				// 每股公积金
	
	EMRFBusinessProfit,			// 营业利益
	EMRFPerNoDistribute,		// 每股未分配
	EMRFPerIncomeYear,			// 每股收益(年)
	EMRFPerPureAsset,			// 每股净资产
	EMRFChPerPureAsset,			// 调整每股净资产
	
	EMRFDorRightRate,			// 股东权益比
	EMRFCircMarketValue,		// 流通市值
	EMRFAllMarketValue,			// 总市值
	

	//
	EMRFCount
}E_MerchReportField;

typedef enum E_OffKLineType
{
	EOKTVipData = 0,
	EOKTIndexData,
	EOKTOfflineData,

	EOKTCount
}E_OffKLineType;

// 数据不来,重连的可能原因
typedef enum E_ReConnectType
{
	ERCTWaitingForOnePack = 0,		// commengine 中收一个包的时间超过 30s , 需要重连
	ERCTNoDataAtAll,				// commengine 中超过 30s 没有收到一点数据, 需要重连
	ERCTTooMuchTimeOutPack,			// dataengine 中有超过 3 个以上 30s 没有回的包, 需要重连
	ERCTPackTimeOut,				// dataengine 中有超过 5 秒没有回的包, 客户端提示网络不畅

	ERCTCount,
}E_ReConnectType;
#endif

#include "StdAfx.h"
#include "ReportScheme.h"
#include "coding.h"
#include <map>
using std::map;

CReportScheme * CReportScheme::s_Instance = NULL;
CString	m_strBuyPrice, m_strSellPrice, m_strNewPrice;

//////////////////////////////////////////////////////////////////////////
// XML
const char*	KStrElementNameHeadInfo				=	"HeadInfo";
const char*	KStrElementNameHeadItem				=	"HeadItem";
const char*	KStrElementAttriHeadType			=	"Type";
const char*	KStrElementAttriFixCol				=	"FixCol";
const char*	KStrElementAttriHeadName			=	"Name";
const char*	KStrElementAttriHeadWidth			=	"Width";
const char*	KStrElementAttriHeadColumnType		=	"HCType";	// 与名称对应的类型，如果名称找不到，则利用这个type加载

////////////////////////////////////////////////////////////////////////
// 常量列表, 该表与E_ReportHeader一一对应
T_ReportHeaderObject s_astReportHeaderObjects[] = 
{
	T_ReportHeaderObject(CReportScheme::ERHRowNo,				L"序",					L"RowNo",				L"序"),
	T_ReportHeaderObject(CReportScheme::ERHMerchCode,			L"代码",				L"Code",				L"代码"),
	T_ReportHeaderObject(CReportScheme::ERHMerchName,			L"名称",				L"Name",				L"名称"),
	T_ReportHeaderObject(CReportScheme::ERHPricePrevClose,		L"昨收",				L"PrevClose",			L"昨收"),	
	T_ReportHeaderObject(CReportScheme::ERHPricePrevBalance,	L"昨结",				L"PrevBalance",			L"昨结"),
	T_ReportHeaderObject(CReportScheme::ERHPriceOpen,			L"开盘",				L"PriceOpen",			L"开盘"),
	T_ReportHeaderObject(CReportScheme::ERHPriceNew,			L"最新", 				L"PriceNew",			L"最新"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBalance,		L"均价",				L"PriceBalance",		L"均价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceHigh,			L"最高",				L"PriceHigh",			L"最高"),
	T_ReportHeaderObject(CReportScheme::ERHPriceLow,			L"最低",				L"PriceLow",			L"最低"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeCur,			L"现手",				L"VolumeCur",			L"现手"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeTotal,			L"总手",				L"VolumeTotal",			L"总手"),
	T_ReportHeaderObject(CReportScheme::ERHAmount,				L"总金额",				L"Amount",				L"总金额"),
 	T_ReportHeaderObject(CReportScheme::ERHHold,				L"持仓量",				L"Hold",				L"持仓量"),
	T_ReportHeaderObject(CReportScheme::ERHRiseFall,			L"涨跌",				L"RiseFall",			L"涨跌"),
	T_ReportHeaderObject(CReportScheme::ERHRange ,				L"涨幅%",				L"Range ",				L"涨幅%"),
	T_ReportHeaderObject(CReportScheme::ERHSwing ,				L"振幅%",				L"Swing ",				L"振幅%"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeRate,			L"量比",				L"VolumeRate ",			L"量比"),
	T_ReportHeaderObject(CReportScheme::ERHPowerDegree,			L"强弱度%",				L"PowerDegree ",		L"强弱度%"),
	T_ReportHeaderObject(CReportScheme::ERHDKBallance,			L"多空平衡",			L"DKBallance ",			L"多空平衡"),
	T_ReportHeaderObject(CReportScheme::ERHDWin,				L"多头获利",			L"DWin ",				L"多头获利"),
	T_ReportHeaderObject(CReportScheme::ERHDLose,				L"多头停损",			L" DLose",				L"多头停损"),
	T_ReportHeaderObject(CReportScheme::ERHKWin,				L"空头回补",			L" KWin",				L"空头回补"),
	T_ReportHeaderObject(CReportScheme::ERHKLose,				L"空头停损",			L"KLose ",				L"空头停损"),
	T_ReportHeaderObject(CReportScheme::ERHRate,				L"委比%",				L"Rate ",				L"委比%"),
	T_ReportHeaderObject(CReportScheme::ERHSpeedRiseFall,		L"涨速%",				L"SpeedRiseFall ",		L"涨速%"),
	T_ReportHeaderObject(CReportScheme::ERHChangeRate,			L"换手率%",				L"ChangeRate ",			L"换手率%"),
	T_ReportHeaderObject(CReportScheme::ERHMarketWinRate,		L"市盈率",				L"MarketWinRate ",		L"市盈率"),
	T_ReportHeaderObject(CReportScheme::ERHBuyPrice,			L"买价",				L"BuyPrice ",			L"买入价"),
	T_ReportHeaderObject(CReportScheme::ERHSellPrice,			L"卖价",				L"SellPrice ",			L"卖出价"),
	T_ReportHeaderObject(CReportScheme::ERHBuyAmount,			L"买量",				L"BuyAmount ",			L"买入量"),
	T_ReportHeaderObject(CReportScheme::ERHSellAmount,			L"卖量",				L"SellAmount ",			L"卖出量"),
	T_ReportHeaderObject(CReportScheme::ERHDifferenceHold,		L"仓差",				L"DifferenceHold ",		L"持仓差"),
	T_ReportHeaderObject(CReportScheme::ERHTime,				L"时间",				L"Time ",				L"时间"),
	T_ReportHeaderObject(CReportScheme::ERHBuySellPrice ,		L"买价/卖价",			L" BuySellPrice",		L"买入价/卖出价 "),
	T_ReportHeaderObject(CReportScheme::ERHBuySellVolume ,		L"买/卖量",				L"BuySellVolume ",		L"买/卖量 "),
	T_ReportHeaderObject(CReportScheme::ERHPreDone1 ,			L"前成1",				L"PreDone1 ",			L"前成一"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone2 ,			L"前成2",				L"PreDone2 ",			L"前成二"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone3 ,			L"前成3",				L"PreDone3 ",			L"前成三"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone4 ,			L"前成4",				L"PreDone4 ",			L"前成四"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone5 ,			L"前成5",				L"PreDone5 ",			L"前成五"),
	//////////////////////////////////////////////////////////////////////////
	// fangz0609 Add for Warrants44

	T_ReportHeaderObject(CReportScheme::ERHTargetStockCode,		L"标的股",				L"TargetStockCode",		L"标的股"),
	T_ReportHeaderObject(CReportScheme::ERHTargetStockName ,	L"标的股名",			L"TargetStockName",		L"标的股名"),
	T_ReportHeaderObject(CReportScheme::ERHSellOrBuy ,			L"认购认沽",			L"SellOrBuy",			L"认购认沽"),
	T_ReportHeaderObject(CReportScheme::ERHXQRate ,				L"行权比",				L"XQRate",				L"行权比"),
	T_ReportHeaderObject(CReportScheme::ERHXQPrice ,			L"行权价",				L"XQPrice",				L"行权价"),
	T_ReportHeaderObject(CReportScheme::ERHMaturity ,			L"到期日",				L"Maturity",			L"到期日"),
	T_ReportHeaderObject(CReportScheme::ERHDuration ,			L"存续期",				L"Duration",			L"存续期"),
	T_ReportHeaderObject(CReportScheme::ERHTheoreticalPrice ,	L"理论价",				L"TheoreticalPrice",	L"理论价"),
	T_ReportHeaderObject(CReportScheme::ERHPremium ,			L"溢价",				L"Premium",				L"溢价"),
	T_ReportHeaderObject(CReportScheme::ERHUsaOrEuro ,			L"欧式美式",			L"UsaOrEuro",			L"欧式美式"),
	T_ReportHeaderObject(CReportScheme::ERHCirculateBallance ,	L"流通余额",			L"CirculateBallance",	L"流通余额"),
	T_ReportHeaderObject(CReportScheme::ERHDistributors ,		L"发行商",				L"Distributors",		L"发行商"),
	T_ReportHeaderObject(CReportScheme::ERHWarrantType ,		L"权证类型",			L"WarrantType",			L"权证类型"),
	T_ReportHeaderObject(CReportScheme::ERHPriceInOut ,			L"价内价外",			L"PriceInOut",			L"价内价外"),
	T_ReportHeaderObject(CReportScheme::ERHValueInside ,		L"内在价值",			L"ValueInside",			L"内在价值"),
	T_ReportHeaderObject(CReportScheme::ERHAmplitudeHistory ,	L"历史波幅",			L"AmplitudeHistory",	L"历史波幅"),
	T_ReportHeaderObject(CReportScheme::ERHAmplitudeExplicate ,	L"引伸波幅",			L"AmplitudeExplicate",	L"引伸波幅"),
	T_ReportHeaderObject(CReportScheme::ERHDelta ,				L"对冲值",				L"Delta",				L"对冲值"),
	T_ReportHeaderObject(CReportScheme::ERHGAMMA ,				L"GAMMA",				L"GAMMA",				L"GAMMA"),
	T_ReportHeaderObject(CReportScheme::ERHLeverRate ,			L"杠杆比率",			L"LeverRate",			L"杠杆比率"),
	T_ReportHeaderObject(CReportScheme::ERHLeverTrue ,			L"实际杠杆",			L"LeverTrue",			L"实际杠杆"),
	T_ReportHeaderObject(CReportScheme::ERHStockChangeRate ,	L"换股比率",			L"StockChangeRate",		L"换股比率"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeWildGoods ,	L"街货量",				L"VolumeWildGoods",		L"街货量"),
	T_ReportHeaderObject(CReportScheme::ERHRateWildGoods ,		L"街货比例",			L"RateWildGoods",		L"街货比例"),

	//////////////////////////////////////////////////////////////////////////
	// fangz0811 Add for Futures
	T_ReportHeaderObject(CReportScheme::ERHBuild ,				L"开仓",				L"Build",				L"开仓"),
	T_ReportHeaderObject(CReportScheme::ERHClear ,				L"平仓",				L"Clear",				L"平仓"),
	T_ReportHeaderObject(CReportScheme::ERHBuildPer ,			L"单开",				L"BuildPer",			L"单开"),
	T_ReportHeaderObject(CReportScheme::ERHClearPer ,			L"单平",				L"ClearPer",			L"单平"),
	T_ReportHeaderObject(CReportScheme::ERHAddPer ,				L"单增",				L"AddPer",				L"单增"),
	T_ReportHeaderObject(CReportScheme::ERHBuyVolumn ,			L"内盘",				L"BuyVolumn",			L"内盘"),
	T_ReportHeaderObject(CReportScheme::ERHSellVolumn ,			L"外盘",				L"SellVolumn",			L"外盘"),
	T_ReportHeaderObject(CReportScheme::ERHTrend ,				L"走势",				L"Trend",				L"走势"),

	//////////////////////////////////////////////////////////////////////////
	// xl 0813 短线中线选股字段
	T_ReportHeaderObject(CReportScheme::ERHShortLineSelect ,	L"短线",				L"ShortLineSelect",		L"短线"),
	T_ReportHeaderObject(CReportScheme::ERHMidLineSelect ,		L"中线",				L"MidLineSelect",		L"中线"),
	T_ReportHeaderObject(CReportScheme::ERHCapitalFlow ,		L"资金流向",			L"CapitalFlow",			L"资金流向"),

	//////////////////////////////////////////////////////////////////////////
	// xl 0820 财务数据
	T_ReportHeaderObject(CReportScheme::ERHAllCapital ,			L"总股本(万)",			L"AllCapital",			L"总股本(万)"),
	T_ReportHeaderObject(CReportScheme::ERHCircAsset ,			L"流通股本(万)",		L"CircAsset",			L"流通股本(万)"),
	T_ReportHeaderObject(CReportScheme::ERHAllAsset ,			L"总资产(万)",			L"AllAsset",			L"总资产(万)"),
	T_ReportHeaderObject(CReportScheme::ERHFlowDebt ,			L"流动负债(万)",		L"FlowDebt",			L"流动负债(万)"),
	T_ReportHeaderObject(CReportScheme::ERHPerFund ,			L"每股公积金",			L"PerFund",				L"每股公积金"),
	T_ReportHeaderObject(CReportScheme::ERHBusinessProfit ,		L"营业利益(万)",		L"BusinessProfit",		L"营业利益(万)"),
	T_ReportHeaderObject(CReportScheme::ERHPerNoDistribute ,	L"每股未分配",			L"PerNoDistribute",		L"每股未分配"),
	T_ReportHeaderObject(CReportScheme::ERHPerIncomeYear ,		L"每股收益",		L"PerIncomeYear",		L"每股收益"),
	T_ReportHeaderObject(CReportScheme::ERHPerPureAsset ,		L"每股净资产",			L"PerPureAsset",		L"每股净资产"),
	T_ReportHeaderObject(CReportScheme::ERHChPerPureAsset ,		L"调整每股净资产",		L"ChPerPureAsset",		L"调整每股净资产"),
	T_ReportHeaderObject(CReportScheme::ERHDorRightRate ,		L"股东权益比",			L"DorRightRate",		L"股东权益比"),
	T_ReportHeaderObject(CReportScheme::ERHCircMarketValue ,	L"流通市值(万)",		L"CircMarketValue",		L"流通市值(万)"),
	T_ReportHeaderObject(CReportScheme::ERHAllMarketValue ,		L"总市值(万)",			L"AllMarketValue",		L"总市值(万)"),

	T_ReportHeaderObject(CReportScheme::ERHBuySellRate ,		L"内外比",				L"BuySellRate",			L"内外比"),

	//////////////////////////////////////////////////////////////////////////
	// fangz 1015
	T_ReportHeaderObject(CReportScheme::ERHPriceSell5 ,			L"卖⑤价",				L"PriceSell5",			L"卖⑤价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell4 ,			L"卖④价",				L"PriceSell4",			L"卖④价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell3 ,			L"卖③价",				L"PriceSell3",			L"卖③价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell2 ,			L"卖②价",				L"PriceSell2",			L"卖②价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell1 ,			L"卖①价",				L"PriceSell1",			L"卖①价"),

	T_ReportHeaderObject(CReportScheme::ERHVolSell5 ,			L"卖⑤量",				L"VolSell5",			L"卖⑤量"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell4 ,			L"卖④量",				L"VolSell4",			L"卖④量"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell3 ,			L"卖③量",				L"VolSell3",			L"卖③量"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell2 ,			L"卖②量",				L"VolSell2",			L"卖②量"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell1 ,			L"卖①量",				L"VolSell1",			L"卖①量"),

	T_ReportHeaderObject(CReportScheme::ERHPriceBuy5 ,			L"买⑤价",				L"PriceBuy5",			L"买⑤价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy4 ,			L"买④价",				L"PriceBuy4",			L"买④价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy3 ,			L"买③价",				L"PriceBuy3",			L"买③价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy2 ,			L"买②价",				L"PriceBuy2",			L"买②价"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy1 ,			L"买①价",				L"PriceBuy1",			L"买①价"),

	T_ReportHeaderObject(CReportScheme::ERHVolBuy5 ,			L"买⑤量",				L"VolBuy5",				L"买⑤量"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy4 ,			L"买④量",				L"VolBuy4",				L"买④量"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy3 ,			L"买③量",				L"VolBuy3",				L"买③量"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy2 ,			L"买②量",				L"VolBuy2",				L"买②量"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy1 ,			L"买①量",				L"VolBuy1",				L"买①量"),
	T_ReportHeaderObject(CReportScheme::ERHIndustry,		    L"所属行业",			L"Industry",			L"所属行业"),
	T_ReportHeaderObject(CReportScheme::ERHZixuan,				L"自选",				L"ZiXuan",				L"自选"),
	T_ReportHeaderObject(CReportScheme::ERHAllNetAmount,		L"净流入",				L"AllNetAmount",		L"净流入"),
	T_ReportHeaderObject(CReportScheme::ERHExBigBuyAmount,		L"超大单流入",			L"ExBigBuyAmount",		L"超大单流入"),
	T_ReportHeaderObject(CReportScheme::ERHExBigSellAmount,		L"超大单流出",			L"ExBigSellAmount",		L"超大单流出"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetAmount,		L"超大单净额",			L"ExBigNetAmount",		L"超大单净额"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetAmountProportion,	 L"超大单净额占比(%)",	L"ExBigNetAmountProportion",		L"超大单净额占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetTotalAmount,			 L"超大单总额",			L"ExBigNetTotalAmount",				L"超大单总额"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetTotalAmountProportion,L"超大单总额占比(%)",	L"ExBigNetTotalAmountProportion",	L"超大单总额占比(%)"),
	
	T_ReportHeaderObject(CReportScheme::ERHBigBuyAmount,		L"大单流入",				L"BigBuyAmount",				L"大单流入"),
	T_ReportHeaderObject(CReportScheme::ERHBigSellAmount,		L"大单流出",				L"BigSellAmount",				L"大单流出"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetAmount,		L"大单净额",				L"BigNetAmount",				L"大单净额"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetAmountProportion,	L"大单净额占比(%)",		L"BigNetAmountProportion",		L"大单净额占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetTotalAmount,	L"大单总额",				L"BigNetTotalAmount",			L"大单总额"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetTotalAmountProportion,	L"大单总额占比(%)",	L"BigNetTotalAmountProportion",	L"大单总额占比(%)"),

	T_ReportHeaderObject(CReportScheme::ERHMidBuyAmount,		L"中单流入",				L"MidBuyAmount",					L"中单流入"),
	T_ReportHeaderObject(CReportScheme::ERHMidSellAmount,		L"中单流出",				L"MidSellAmount",					L"中单流出"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetAmount,		L"中单净额",				L"MidNetAmount",					L"中单净额"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetAmountProportion,	L"中单净额占比(%)",		L"MidNetAmountProportion",			L"中单净额占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetTotalAmount,	L"中单总额",				L"MidNetTotalAmount",				L"中单总额"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetTotalAmountProportion,	L"中单总额占比(%)",	L"MidNetTotalAmountProportion",		L"中单总额占比(%)"),

	T_ReportHeaderObject(CReportScheme::ERHSmallBuyAmount,					L"小单流入",			L"SmallBuyAmount",					L"小单流入"),
	T_ReportHeaderObject(CReportScheme::ERHSmallSellAmount,					L"小单流出",			L"SmallSellAmount",					L"小单流出"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetAmount,					L"小单净额",			L"SmallNetAmount",					L"小单净额"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetAmountProportion,		L"小单净额占比(%)",		L"SmallNetAmountProportion",		L"小单净额占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetTotalAmount,				L"小单总额",			L"SmallNetTotalAmount",				L"小单总额"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetTotalAmountProportion,	L"小单总额占比(%)",		L"SmallNetTotalAmountProportion",	L"小单总额占比(%)"),
	
	T_ReportHeaderObject(CReportScheme::ERHSelStockTime,	    L"入选时间",			L"SelStockTime",			L"入选时间"),
	T_ReportHeaderObject(CReportScheme::ERHSelPrice,	        L"入选价格",			L"SelectPrice",				L"入选价格"),
	T_ReportHeaderObject(CReportScheme::ERHSelRisePercent,	    L"入选后涨幅(%)",		L"SelectRisePercent",		L"入选后涨幅(%)"),

	T_ReportHeaderObject(CReportScheme::ERHTodayMasukuraProportion,	L"今日增仓占比(%)",	L"TodayMasukuraProportion",	L"今日增仓占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERHTodayRanked,				L"今日排名",		L"TodayRanked",				L"今日排名"),
	T_ReportHeaderObject(CReportScheme::ERHTodayRise,				L"今日涨幅(%)",		L"TodayRise",				L"今日涨幅(%)"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysMasukuraProportion,	L"2日增仓占比(%)",	L"2DaysMasukuraProportion",	L"2日增仓占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysRanked,				L"2日排名",			L"2DaysRanked",				L"2日排名"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysRise,				L"2日涨幅(%)",		L"2DaysRise",				L"2日涨幅(%)"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysMasukuraProportion,	L"3日增仓占比(%)",	L"3DaysMasukuraProportion",	L"3日增仓占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysRanked,				L"3日排名",			L"3DaysRanked",				L"3日排名"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysRise,				L"3日涨幅(%)",		L"3DaysRise",				L"3日涨幅(%)"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysMasukuraProportion,	L"5日增仓占比(%)",	L"5DaysMasukuraProportion",	L"5日增仓占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysRanked,				L"5日排名",			L"5DaysRanked",				L"5日排名"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysRise,				L"5日涨幅(%)",		L"5DaysRise",				L"5日涨幅(%)"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysMasukuraProportion,L"10日增仓占比(%)",	L"10DaysMasukuraProportion",L"10日增仓占比(%)"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysRanked,			L"10日排名",		L"10DaysRanked",			L"10日排名"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysRise,				L"10日涨幅(%)",		L"10DaysRise",				L"10日涨幅(%)"),
	T_ReportHeaderObject(CReportScheme::ERHMarketMargin,			L"市净率",			L"MarketMargin",			L"市净率"),
	T_ReportHeaderObject(CReportScheme::ERHNetProfit,				L"净利润",			L"NetProfit",				L"净利润"),
	T_ReportHeaderObject(CReportScheme::ERHTotalProfit,				L"利润总额",		L"TotalProfit",				L"利润总额"),
	T_ReportHeaderObject(CReportScheme::ERHassetsRate,				L"净资产收益率",	L"assetsRate",				L"净资产收益率"),
	T_ReportHeaderObject(CReportScheme::ERHNetProfitGrowthRate,		L"净利润增长率",	L"NetProfitGrowthRate",		L"净利润增长率"),
	T_ReportHeaderObject(CReportScheme::ERHNetAmountofthemain,		L"主力净量",		L"NetAmountofthemain",		L"主力净量"),
};


const int KReportHeaderObjectCount = sizeof(s_astReportHeaderObjects) / sizeof(T_ReportHeaderObject);

//////////////////////////////////////////////////////////////////////////
// 常量列表, 该表与E_ReportType一一对应
const T_ReportTypeObject s_astReportTypeObjects []=
{
	T_ReportTypeObject(ERTFuturesCn,				L"国内期货",			L"ChinaFutures"),				
	T_ReportTypeObject(ERTStockCn,				L"国内证券",			L"ChinaStock"),				
	T_ReportTypeObject(ERTMony,					L"做市商",			L"ForeigneExchange"),				
	T_ReportTypeObject(ERTExp,					L"指数栏目",			L"ShareIndex"),
	T_ReportTypeObject(ERTStockHk,				L"港股栏目",			L"HongKongStock"),
	T_ReportTypeObject(ERTFuturesForeign,		L"国外期货",			L"FuturesForeign"),	
	T_ReportTypeObject(ERTWarrantCn,				L"国内权证",			L"WarrantCn"),
	T_ReportTypeObject(ERTWarrantHk,				L"香港权证",			L"WarrantHk"),
	T_ReportTypeObject(ERTFutureRelaMonth,		L"相对月期货",			L"FutureRelaMonth"),
	T_ReportTypeObject(ERTFutureSpot,			L"现货",			    L"FutureSpot"),
	T_ReportTypeObject(ERTExpForeign,			L"国外指数",			L"ExpForeign"),
	T_ReportTypeObject(ERTFutureGold,			L"黄金期货",			L"FutureGold"),
	T_ReportTypeObject(ERTCustom,				L"自定义",				L"Custom"),
	T_ReportTypeObject(ERTFinance,				L"财务数据",			L"Finance"),
	T_ReportTypeObject(ERTNone,					L"空",					L"None"),
	T_ReportTypeObject(ERTSelectStock,			L"智能选股",			L"SelectStock"),
	T_ReportTypeObject(ERTMerchSort,			L"热门强龙",			L"HotStock"),
	T_ReportTypeObject(ERTCapitalFlow,			L"资金流向",			L"CapitalFlow"),			 
	T_ReportTypeObject(ERTMainMasukura,			L"主力增仓",			L"ERTMainMasukura"),          
};

const int KReportTypeObjectCount = sizeof(s_astReportTypeObjects) / sizeof(T_ReportTypeObject);  //43


typedef map<CReportScheme::E_ReportHeader, int>	ReportHeaderWidthMap;


static ReportHeaderWidthMap		s_mapReportHeaderWidthDefault;		// 默认表格宽度



//static const CString KStrPath1           = L"./config/";
//static const CString KStrPath2	       = L"/grid_head.ini";

static const CString KStrIniAttriFixCol		= L"fixcol";

//////////////////////////////////////////////////////////////////////////
// 初始化一些特殊的列的默认列宽
void	InitializeColDefaultWidth()
{
	if ( s_mapReportHeaderWidthDefault.empty() )
	{
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllCapital ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCircAsset ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllAsset ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHFlowDebt ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHBusinessProfit ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCircMarketValue ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllMarketValue ] = 130;

		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerFund ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerNoDistribute ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerIncomeYear ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerPureAsset ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHChPerPureAsset ] = 100;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHDorRightRate ] = 90;
		
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCapitalFlow ] = 100;
	}
}




////////////////////////////////////////////////////////////////////////
CReportScheme::CReportScheme()
{ 
	s_Instance = this;

	InitializeColDefaultWidth();		// 初始化默认列宽
}

CReportScheme::~CReportScheme()
{
	s_Instance = NULL;
}

CReportScheme* CReportScheme::Instance()
{
	return s_Instance;
}



int CReportScheme::GetReportColDefaultWidth(CReportScheme::E_ReportHeader eHeaderType)
{
	ReportHeaderWidthMap::const_iterator it = s_mapReportHeaderWidthDefault.find(eHeaderType);
	if ( it != s_mapReportHeaderWidthDefault.end() )
	{
		return it->second;
	}
	return DEFUALTWIDTH;
}



void CReportScheme::Construct(CString strPrivateFolderName)
{
	m_strPrivateName = strPrivateFolderName;

	// 清空
	int32 i;
	for ( i = 0; i < ERTCount; i++)
	{
		m_aReportHeadInfo[i].RemoveAll();
		m_aFixCol[i]=0;
	}
	
	// 赋值:
	ReadDataFromXml();

	// 以防万一,检查一下
	bool32 bInValid = false;

	for ( i = 0 ; i < ERTCount; i++ )
	{
		E_ReportType eType = (E_ReportType)i;
		
		if ( m_aReportHeadInfo[eType].GetSize() <= 0 )
		{
			bInValid = true;
			SetHeadDataToDefault(eType, m_aReportHeadInfo[eType], m_aFixCol[eType]);
		}
	}

	if ( bInValid )
	{
		// 有错误数据,重新保存一下.覆盖原来的.
		SaveDataToXml();
	}
}

void CReportScheme::GetReportHeadInfoList(E_ReportType eReportType,OUT CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	if (ERTCount == eReportType )
	{
		return;
	}

	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aFixCol)/sizeof(m_aFixCol[0])))
	{
		return;
	}

	aHeadInfo.RemoveAll();
	if (eReportType < ERTCount)
	{
		for (int32 i = 0; i < m_aReportHeadInfo[eReportType].GetSize(); i++)
		{
			T_HeadInfo HeadInfo = m_aReportHeadInfo[eReportType][i];
			aHeadInfo.Add(HeadInfo);
		}
	}

	// 避免数组越界
	if (eReportType < ERTCount)
	{
		iFixCol = m_aFixCol[eReportType];
	}
	
	
	if (aHeadInfo.GetSize()<=0)
	{
		aHeadInfo.RemoveAll();
		T_HeadInfo  HeadInfoDefault;
		for (int32 i =0;i<10;i++)
		{
			HeadInfoDefault.m_StrHeadNameCn = s_astReportHeaderObjects[i].m_StrCnName;
			//HeadInfoDefault.m_iHeadWidth	= DEFUALTWIDTH;
			HeadInfoDefault.m_iHeadWidth	= GetReportColDefaultWidth( s_astReportHeaderObjects[i].m_eReportHeader );
			HeadInfoDefault.m_eReportHeader = s_astReportHeaderObjects[i].m_eReportHeader;
			aHeadInfo.Add(HeadInfoDefault);			
		}	
		iFixCol = 0;
	}
}
CReportScheme::E_ReportHeader CReportScheme::GetReportHeaderEType(CString StrHeadCnName)
{
	StrHeadCnName = GetSpecialReportHeaderCnName(StrHeadCnName);

	bool32 bRightName = false;
	int32  iCount = 0;
	for (int32 i = 0;i<KReportHeaderObjectCount;i++)
	{
		if (StrHeadCnName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			bRightName = TRUE;
			iCount     = i;
			return s_astReportHeaderObjects[iCount].m_eReportHeader;
		}		
	}
	//ASSERT(bRightName && iCount>=0 && iCount<KReportHeaderObjectCount);
	return ERHCount;	// 返回非法值
}

CString CReportScheme::GetReportHeaderCnName(E_ReportHeader eReportHeader)
{
	ASSERT(eReportHeader < ERHCount);
	ASSERT(ERHCount == KReportHeaderObjectCount);
	if (eReportHeader >= ERHCount )
	{
		return _T("");
	}

	CString strName = s_astReportHeaderObjects[eReportHeader].m_StrCnName;
	strName = GetSpecialReportHeaderCnName(strName);
	return strName;
//	return s_astReportHeaderObjects[eReportHeader].m_StrCnName;
}

CString CReportScheme::GetReportTypeCnName(E_ReportType e_ReportType)
{
	ASSERT(e_ReportType < ERTCount );
	ASSERT(ERTCount == KReportTypeObjectCount);
	if (e_ReportType >= ERTCount )
	{
		return _T("");
	}
	return s_astReportTypeObjects[e_ReportType].m_StrCnName;
}

void CReportScheme::GetHead(E_ReportType eReportType,OUT CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead)
{
	// 根据Type ,获取存在的表头和他的补集
	ASSERT(eReportType<ERTCount);
	ASSERT(ERTCount == KReportTypeObjectCount);
	if (ERTCount == eReportType)
	{
		return;
	}
	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aReportHeadInfo)/sizeof(m_aReportHeadInfo[0])))
	{
		return;
	}

	int32 iSize =  m_aReportHeadInfo[eReportType].GetSize();
	aStrGridHead.RemoveAll();
	aStrAnotherGridHead.RemoveAll();

	if (ERTMony == eReportType)
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			if (ERHPriceNew == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strNewPrice);
			}
			else if (ERHBuyPrice == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strBuyPrice);
			}
			else if (ERHSellPrice == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strSellPrice);
			}
			else
			{
				aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
			}
		}
		for (int32 p=0;p<iSize;p++ )
		{
			for (int32 q=0;q<KReportHeaderObjectCount;q++)
			{
				CString strHeadName = s_astReportHeaderObjects[q].m_StrCnName;
				strHeadName = HeadSpecialCnName2HeadCnName(strHeadName);
				if ( m_aReportHeadInfo[eReportType][p].m_StrHeadNameCn == strHeadName)
				{
					if (ERHPriceNew == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strNewPrice);
					}
					else if (ERHBuyPrice == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strBuyPrice);
					}
					else if (ERHSellPrice == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strSellPrice);
					}
					else
					{
						aStrGridHead.Add(s_astReportHeaderObjects[q].m_StrSummary);
					}
					break;
				}
			}
		}
	}
	else
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
		}
		for (int32 p=0; p<iSize; p++ )
		{
			for (int32 q=0; q<KReportHeaderObjectCount; q++)
			{
				if ( m_aReportHeadInfo[eReportType][p].m_StrHeadNameCn == s_astReportHeaderObjects[q].m_StrCnName)
				{
					aStrGridHead.Add(s_astReportHeaderObjects[q].m_StrSummary);
					break;
				}
			}
		}
	}

	for ( int32 j=0;j < aStrGridHead.GetSize();j++)
	{
		for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
		{
			if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
			{	
				aStrAnotherGridHead.RemoveAt(i);
			}
		}		
	}	
}

void CReportScheme::GetHead(IN CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead,E_ReportType eReportType)
{
	// 已知一个表头集合,获取他的补集

	ASSERT(ERTCount == KReportTypeObjectCount);

	aStrAnotherGridHead.RemoveAll();

	if (ERTMony == eReportType)
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			if (ERHPriceNew==i)
			{
				aStrAnotherGridHead.Add(m_strNewPrice);
			}
			else if (ERHBuyPrice==i)
			{
				aStrAnotherGridHead.Add(m_strBuyPrice);
			}
			else if (ERHSellPrice==i)
			{
				aStrAnotherGridHead.Add(m_strSellPrice);
			}
			else
			{
				aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
			}
		}
		for ( int32 j=0;j < aStrGridHead.GetSize();j++)
		{
			for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
			{			
				if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
				{	
					aStrAnotherGridHead.RemoveAt(i);
				}
			}		
		}	
	}
	else
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
		}
		for ( int32 j=0;j < aStrGridHead.GetSize();j++)
		{
			for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
			{
				if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
				{	
					aStrAnotherGridHead.RemoveAt(i);
				}
			}		
		}	
	}
}

void CReportScheme::SaveColumnWidth(E_ReportType eReportType,int32 iColumn,int32 iWidth)
{

	ASSERT(eReportType<ERTCount);
	ASSERT(ERTCount == KReportTypeObjectCount);
	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aReportHeadInfo)/sizeof(m_aReportHeadInfo[0])))
	{
		return;
	}
	ASSERT(iColumn>=0 && iColumn<m_aReportHeadInfo[eReportType].GetSize());

	if (iColumn>=0 &&  iColumn<m_aReportHeadInfo[eReportType].GetSize())
	{
		return;
	}
	if (iWidth<=0||iWidth>=MAXWIDTH)
	{
		iWidth = MAXWIDTH;
	}
	

	// 重设全局数组的值
	int32& iWidthTest = m_aReportHeadInfo[eReportType][iColumn].m_iHeadWidth;
	iWidthTest = iWidth;	
}

void CReportScheme::DoHeadInfoString(CString StrHeadInfo,OUT CString& StrHeadName,OUT int32& iWidth)
{
	ASSERT(0 != StrHeadInfo.GetLength());
	StrHeadInfo.TrimLeft();
	StrHeadInfo.TrimRight();
	
	int32 iStart = StrHeadInfo.Find(_T("<"));
	StrHeadName  = StrHeadInfo.Left(iStart);

	int32 iEnd	 = StrHeadInfo.Find(_T(">"));
	CString StrWidth = StrHeadInfo.Mid(iStart+1,iEnd-iStart-1);
	iWidth = _ttoi(StrWidth);
	
	// 如果宽度不在正常范围,设置为默认
	if (iWidth<=0 || iWidth>=MAXWIDTH)
	{
		iWidth = MAXWIDTH;
	}
	// 如果名称不在正常范围,设置为默认
	bool32 bRightName = false;
	for (int32 i = 0;i<KReportHeaderObjectCount;i++)
	{
		if (StrHeadName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			bRightName = TRUE;
		}		
	}
	if (!bRightName)
	{
		StrHeadName = s_astReportHeaderObjects[0].m_StrCnName;
	}
}
CString CReportScheme::HeadCnName2SummaryName(CString StrCnName)
{
	if ((StrCnName==m_strBuyPrice)||(StrCnName==m_strSellPrice)||(StrCnName==m_strNewPrice))
	{
		return StrCnName;
	}

	for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
	{
		if (StrCnName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			return s_astReportHeaderObjects[i].m_StrSummary;
		}
	}
	return L"ERROR";
}
CString CReportScheme::HeadSummaryName2CnName(CString StrSummaryName)
{
	if ((StrSummaryName==m_strBuyPrice)||(StrSummaryName==m_strSellPrice)||(StrSummaryName==m_strNewPrice))
	{
		return StrSummaryName;
	}

	for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
	{
		if (StrSummaryName == s_astReportHeaderObjects[i].m_StrSummary)
		{
			return s_astReportHeaderObjects[i].m_StrCnName;
		}
	}
	return L"ERROR";
}

CString	CReportScheme::HeadSpecialCnName2HeadCnName(CString strName)
{
	CString strRet = strName;
	if (strName == m_strBuyPrice)
	{
		strRet = L"买价";
	}
	else if (strName == m_strSellPrice)
	{
		strRet = L"卖价";
	}
	else if (strName == m_strNewPrice)
	{
		strRet = L"最新";
	}
	return strRet;
}

CString	CReportScheme::GetSpecialReportHeaderCnName(CString strName)
{
	CString strRetName = strName;

	if (strRetName == m_strBuyPrice)
	{
		strRetName = L"买价";
	}
	else if (strRetName == m_strSellPrice)
	{
		strRetName = L"卖价";
	}
	else if (strRetName == m_strNewPrice)
	{
		strRetName = L"最新";
	}
	return strRetName;
}

CString CReportScheme::GetPath()
{
/*	
	CString StrPath,StrUserName;
				
	ASSERT(NULL != m_pDocument);
	StrUserName = m_pDocument->m_StrUserName;

	StrPath = KStrPath1 +  StrUserName + KStrPath2;	
	return StrPath;
*/

	CString StrPath;
	StrPath = CPathFactory::GetGridHeadFilePath(m_strPrivateName);
	return StrPath;
}

CString CReportScheme::GetFixColAttriName()
{
	return KStrIniAttriFixCol;
}

void CReportScheme::SetHeadDataToDefault(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	// 将某项表头的数据设置为默认数据:
	if (eReportType >= ERTCount )
	{
		return;
	}
	
	// 清空
	aHeadInfo.RemoveAll();
	iFixCol	= 0;
	
	// 填数据:
	int32 iSize = 0;
	
	if (eReportType == ERTFuturesCn || eReportType == ERTFutureRelaMonth) 
	{
		// 国内期货 
		iFixCol = 3;
		if ( eReportType == ERTFutureRelaMonth )			
		{
			iFixCol = 2;
		}
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"名称",		81),
			T_HeadInfo(L"最新",		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		70),
			T_HeadInfo(L"买价",		70),
			T_HeadInfo(L"卖价",		70),
			T_HeadInfo(L"买量",		70),
			T_HeadInfo(L"卖量",		70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"结算",		70),
			T_HeadInfo(L"持仓量",	70),
			T_HeadInfo(L"仓差",		70),
			T_HeadInfo(L"昨结",		70),
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"振幅%",	70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"总金额",		70),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}			
	}
	else if( eReportType == ERTStockCn || eReportType == ERTCustom)
	{
		// 国内证券
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		79),
			T_HeadInfo(L"涨幅%",	66),

			T_HeadInfo(L"短线",	70),
			T_HeadInfo(L"中线",	70),
			T_HeadInfo(L"资金流向",	100),

			T_HeadInfo(L"最新",		64),
			T_HeadInfo(L"涨跌",		63),
			T_HeadInfo(L"买价",		72),
			T_HeadInfo(L"卖价",		73),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		62),
			T_HeadInfo(L"开盘",		62),
			T_HeadInfo(L"昨收",		62),
			T_HeadInfo(L"最高",		62),
			T_HeadInfo(L"最低",		62),
			T_HeadInfo(L"总金额",		65),
			T_HeadInfo(L"振幅%",	64),
			T_HeadInfo(L"买量",		62),
			T_HeadInfo(L"卖量",		62),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTMony)
	{
		// 外汇
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"名称",		70),
			T_HeadInfo(m_strNewPrice,		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(m_strBuyPrice,		70),
			T_HeadInfo(m_strSellPrice,		70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"振幅%",	70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTExp)
	{
		// 指数
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"最新",		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"振幅%",	70),
			T_HeadInfo(L"总金额",		70),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}	
	}
	else if ( eReportType == ERTStockHk)
	{
		// 港股
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{			
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"最新",		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(L"买价",		70),
			T_HeadInfo(L"卖价",		70),
			T_HeadInfo(L"买量",		70),
			T_HeadInfo(L"卖量",		70),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"总金额",		70),
			T_HeadInfo(L"时间",		70),
			T_HeadInfo(L"序",		35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);						
		}
	}
	else if (eReportType == ERTFuturesForeign)
	{
		// 国外期货
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"名称",		70),
			T_HeadInfo(L"最新",		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"买价",		70),
			T_HeadInfo(L"卖价",		70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"持仓量",	70),
			T_HeadInfo(L"仓差",		70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"振幅%",	70),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if (eReportType == ERTWarrantCn)
	{
		// 权证
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		70),
			T_HeadInfo(L"涨幅%",	70),
			T_HeadInfo(L"最新",		70),
			T_HeadInfo(L"涨跌",		70),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		70),
			T_HeadInfo(L"买价",		70),
			T_HeadInfo(L"卖价",		70),
			T_HeadInfo(L"买量",		70),
			T_HeadInfo(L"卖量",		70),
			T_HeadInfo(L"开盘",		70),
			T_HeadInfo(L"昨收",		70),
			T_HeadInfo(L"最高",		70),
			T_HeadInfo(L"最低",		70),
			T_HeadInfo(L"总金额",		70),
			T_HeadInfo(L"时间",		70),
			T_HeadInfo(L"序",		35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}				
	}
	else if ( eReportType == ERTWarrantHk ) 
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"代码",			70),
			T_HeadInfo(L"名称",			70),
			T_HeadInfo(L"认购认沽",		70),
			T_HeadInfo(L"行权比",		70),
			T_HeadInfo(L"行权价",		70),
			T_HeadInfo(L"到期日",		70),
			T_HeadInfo(L"存续期",		70),
			T_HeadInfo(L"理论价",		70),
			T_HeadInfo(L"溢价",			70),
			T_HeadInfo(L"欧式美式",		70),
			T_HeadInfo(L"流通余额",		70),
			T_HeadInfo(L"发行商",		70),
			T_HeadInfo(L"权证类型",		70),
			T_HeadInfo(L"价内价外",		70),
			T_HeadInfo(L"内在价值",		70),
			T_HeadInfo(L"历史波幅",		70),
			T_HeadInfo(L"引伸波幅",		70),
			T_HeadInfo(L"对冲值",		70),
			T_HeadInfo(L"GAMMA",		70),
			T_HeadInfo(L"杠杆比率",		70),
			T_HeadInfo(L"实际杠杆",		70),
			T_HeadInfo(L"换股比率",		70),
			T_HeadInfo(L"街货量",		70),
			T_HeadInfo(L"街货比例",		70),
			T_HeadInfo(L"开仓",			70),
			T_HeadInfo(L"平仓",			70),
			T_HeadInfo(L"序",			35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTFutureSpot )
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"名称",			70),
			T_HeadInfo(L"代码",			70),
			T_HeadInfo(L"昨结",			70),
			T_HeadInfo(L"昨收",			70),
			T_HeadInfo(L"开盘",			70),
			T_HeadInfo(L"最高",			70),
			T_HeadInfo(L"最低",			70),
			T_HeadInfo(L"最新",			70),
			T_HeadInfo(L"现手",			70),	
			T_HeadInfo(L"持仓量",		70),
			T_HeadInfo(L"结算",			70),	
			T_HeadInfo(L"总金额",			70),
			T_HeadInfo(L"总手",		70),
			T_HeadInfo(L"涨跌",			70),
			T_HeadInfo(L"涨幅%",		70),
			
			T_HeadInfo(L"买①价",		70),
			T_HeadInfo(L"卖①价",		70),
			T_HeadInfo(L"买①量",		70),
			T_HeadInfo(L"卖①量",		70),

			T_HeadInfo(L"买②价",		70),
			T_HeadInfo(L"卖②价",		70),
			T_HeadInfo(L"买②量",		70),
			T_HeadInfo(L"卖②量",		70),

			T_HeadInfo(L"买③价",		70),
			T_HeadInfo(L"卖③价",		70),
			T_HeadInfo(L"买③量",		70),
			T_HeadInfo(L"卖③量",		70),

			T_HeadInfo(L"买④价",		70),
			T_HeadInfo(L"卖④价",		70),
			T_HeadInfo(L"买④量",		70),
			T_HeadInfo(L"卖④量",		70),

			T_HeadInfo(L"买⑤价",		70),
			T_HeadInfo(L"卖⑤价",		70),
			T_HeadInfo(L"买⑤量",		70),
			T_HeadInfo(L"卖⑤量",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTExpForeign )
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"名称",			70),
			T_HeadInfo(L"涨幅%",		70),
			T_HeadInfo(L"最新",			70),
			T_HeadInfo(L"涨跌",			70),
			T_HeadInfo(L"总手",		70),
			T_HeadInfo(L"开盘",			70),
			T_HeadInfo(L"最高",			70),
			T_HeadInfo(L"最低",			70),
			T_HeadInfo(L"昨收",			70),
			T_HeadInfo(L"代码",			70),
			T_HeadInfo(L"序",			35),
			T_HeadInfo(L"时间",			70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);						
		}
	}
	else if ( eReportType == ERTFutureGold )
	{
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"名称",			70),
			T_HeadInfo(L"最新",			70),
			T_HeadInfo(L"现手",			70),
			T_HeadInfo(L"买价",			70),
			T_HeadInfo(L"卖价",			70),
			T_HeadInfo(L"买量",			70),
			T_HeadInfo(L"卖量",			70),
			T_HeadInfo(L"总手",		70),
			T_HeadInfo(L"涨跌",			70),
			T_HeadInfo(L"涨幅%",		70),
			T_HeadInfo(L"开盘",			70),
			T_HeadInfo(L"最高",			70),
			T_HeadInfo(L"最低",			70),
			T_HeadInfo(L"昨收",			70),
			T_HeadInfo(L"结算",			70),
			T_HeadInfo(L"序",			70),
			T_HeadInfo(L"代码",			70),
			T_HeadInfo(L"时间",			70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTNone )		
	{
		//这些应该不显示！
		iFixCol = 1;
		
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"序",		70),
		};
		
		iSize = sizeof(aDefaul) / sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTSelectStock )
	{
		// 条件选股(国内证券)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		79),
			T_HeadInfo(L"涨幅%",	66),
			T_HeadInfo(L"短线",		66),
			T_HeadInfo(L"中线",		66),
			T_HeadInfo(L"资金流向",	70),
			T_HeadInfo(L"最新",		64),
			T_HeadInfo(L"涨跌",		63),
			T_HeadInfo(L"买价",		72),
			T_HeadInfo(L"卖价",		73),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		62),
			T_HeadInfo(L"开盘",		62),
			T_HeadInfo(L"昨收",		62),
			T_HeadInfo(L"最高",		62),
			T_HeadInfo(L"最低",		62),
			T_HeadInfo(L"总金额",		65),
			T_HeadInfo(L"振幅%",	64),
			T_HeadInfo(L"买量",		62),
			T_HeadInfo(L"卖量",		62),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTFinance )
	{
		// 财务数据(国内证券)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"代码",			  70),
			T_HeadInfo(L"名称",		      79),
			T_HeadInfo(L"涨幅%",	      66),
			T_HeadInfo(L"总股本(万)"    , 130),
			T_HeadInfo(L"总资产(万)"    , 130),
			T_HeadInfo(L"总市值(万)"    , 130),
			T_HeadInfo(L"流通市值(万)"  , 130),
			T_HeadInfo(L"流动负债(万)"  , 130),
			T_HeadInfo(L"每股公积金"    , 92),
			T_HeadInfo(L"营业利益(万)"  , 130),
			T_HeadInfo(L"每股未分配"    , 92),
			T_HeadInfo(L"每股收益"      , 90),
			T_HeadInfo(L"每股净资产"    , 92),
			T_HeadInfo(L"调整每股净资产", 130),
			T_HeadInfo(L"股东权益比"    , 92),
		};

		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);

		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTMerchSort )
	{
		// 热门强龙(国内证券)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"代码",		70),
			T_HeadInfo(L"名称",		79),
			T_HeadInfo(L"涨幅%",	66),
			T_HeadInfo(L"短线",		66),
			T_HeadInfo(L"中线",		66),
			T_HeadInfo(L"资金流向",	70),
			T_HeadInfo(L"最新",		64),
			T_HeadInfo(L"涨跌",		63),
			T_HeadInfo(L"买价",		72),
			T_HeadInfo(L"卖价",		73),
			T_HeadInfo(L"总手",	70),
			T_HeadInfo(L"现手",		62),
			T_HeadInfo(L"开盘",		62),
			T_HeadInfo(L"昨收",		62),
			T_HeadInfo(L"最高",		62),
			T_HeadInfo(L"最低",		62),
			T_HeadInfo(L"总金额",		65),
			T_HeadInfo(L"振幅%",	64),
			T_HeadInfo(L"买量",		62),
			T_HeadInfo(L"卖量",		62),
			T_HeadInfo(L"序",		35),
			T_HeadInfo(L"时间",		70),
		};

		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);

		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
}

void CReportScheme::ChangeHeadInfoData(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	if (eReportType >= ERTCount )
	{
		return;
	}

	m_aReportHeadInfo[eReportType].RemoveAll();
	m_aReportHeadInfo[eReportType].Copy(aHeadInfo);

	m_aFixCol[eReportType] = iFixCol;
}

void CReportScheme::SetBuySellPriceInfo(CString strBuyPrice, CString strSellPrice, CString strNewPrice)
{
	m_strBuyPrice = strBuyPrice;
	m_strSellPrice = strSellPrice;
	m_strNewPrice = strNewPrice;
}

bool32 CReportScheme::ReadDataFromXml()
{
	// 从私人目录下的 grid_head.xml 中读取用户自定义数据:


	CString StrPath = GetPath();

	std::string StrPathA ;
	Unicode2MultiChar(CP_ACP, StrPath, StrPathA);
	TiXmlDocument MyDoc = TiXmlDocument(StrPathA.c_str());
	if ( !MyDoc.LoadFile() )
	{	
		// xl 1103 新增公共目录下可以设置默认表头的xml加载
		if ( !CPathFactory::GetExistConfigFileName(StrPath, CPathFactory::GetGridHeadFileName()) )
		{
			return false;
		}
		Unicode2MultiChar(CP_ACP, StrPath, StrPathA);		
		if ( !MyDoc.LoadFile(StrPathA.c_str()) )
		{
			return false;
		}
		// 如果加载上了，则继续解析
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = MyDoc.FirstChildElement();
	
	if ( NULL == pRootElement )
	{
		return false;
	}

	TiXmlElement* pHeadInfoElment = pRootElement->FirstChildElement();

	while(pHeadInfoElment)
	{
		if ( NULL != pHeadInfoElment->Value() && 0 == strcmp(pHeadInfoElment->Value(), KStrElementNameHeadInfo) )
		{
			// 是 <HeadInfo Tpye = "1" FixCol = "1"> 这个节点

			// Type: 表头种类
			const char* StrType		= pHeadInfoElment->Attribute(KStrElementAttriHeadType);
			
			// FixCol: 表头固定项个数
			const char* StrFixCol	= pHeadInfoElment->Attribute(KStrElementAttriFixCol);
			
			if ( NULL == StrType )
			{
				pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
				continue;
			}

			int32 iType = atoi(StrType);

			if ( iType < 0 || iType >= (int32)ERTCount )
			{
				pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
				continue;
			}

			// 
			E_ReportType eReportType = (E_ReportType)iType;
			int32		 iFixCol	 = 0;	
			
			if ( NULL != StrFixCol )
			{
				iFixCol = atoi(StrFixCol);
				m_aFixCol[iType] = iFixCol;
			}
			
			// 读取这个种类下的表头数据:
			// 清空数据先:
			m_aReportHeadInfo[eReportType].RemoveAll();				

			TiXmlElement* pHeadItemElement = pHeadInfoElment->FirstChildElement();
			
			while(pHeadItemElement)
			{
				if ( NULL != pHeadItemElement->Value() && 0 == strcmp(pHeadItemElement->Value(), KStrElementNameHeadItem) )
				{
					// 是 <HeadItem Name = "序" Width = "70"> 这个节点
					const char* StrName  = pHeadItemElement->Attribute(KStrElementAttriHeadName);
					const char* StrWidth = pHeadItemElement->Attribute(KStrElementAttriHeadWidth);
					const char* StrHeaderColType = pHeadItemElement->Attribute(KStrElementAttriHeadColumnType);

					if ( NULL == StrName )
					{
						// 名字是错误数据,只有忽略这条了
						pHeadItemElement = pHeadItemElement->NextSiblingElement();
						continue;
					}
					
					T_HeadInfo HeadInfo;
					
					// 字符串处理,多字符转为宽字符:
					TCHAR wStrName[1024] = {'0'};
					
					MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), wStrName, sizeof(wStrName)/sizeof(TCHAR));
					
					HeadInfo.m_StrHeadNameCn = wStrName;
					//HeadInfo.m_iHeadWidth	 = 70;

					CReportScheme::E_ReportHeader eHeader = CReportScheme::Instance()->GetReportHeaderEType(wStrName);
					if (eHeader >= ERHCount )
					{
						// 没有这个字段, 尝试一次xml type的
						E_ReportHeader eHeaderXml = ERHCount;
						if ( NULL != StrHeaderColType )
						{
							eHeaderXml = (E_ReportHeader)atoi(StrHeaderColType);
						}
						if (eHeaderXml<ERHCount )
						{
							HeadInfo.m_StrHeadNameCn = CReportScheme::Instance()->GetReportHeaderCnName(eHeaderXml);
						}
						else
						{
							HeadInfo.m_StrHeadNameCn.Empty();
						}
						if ( !HeadInfo.m_StrHeadNameCn.IsEmpty() )
						{
							eHeader = eHeaderXml;
						}
						else
						{
							// 放弃了
							pHeadItemElement = pHeadItemElement->NextSiblingElement();
							continue;
						}
					}
					HeadInfo.m_eReportHeader = eHeader;
					HeadInfo.m_iHeadWidth    = CReportScheme::GetReportColDefaultWidth(eHeader);

					if ( NULL != StrWidth )
					{
						HeadInfo.m_iHeadWidth = atoi(StrWidth);
					}

					if (ERTMony==eReportType)
					{
						if (L"买价" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strBuyPrice;
						}
						else if (L"卖价" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strSellPrice;
						}
						else if (L"最新" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strNewPrice;
						}
					}

					m_aReportHeadInfo[eReportType].Add(HeadInfo);
				}

				pHeadItemElement = pHeadItemElement->NextSiblingElement();
			}

			if ( m_aReportHeadInfo[eReportType].GetSize() <= 0 )
			{
				// 有错误,设置为默认值:
				SetHeadDataToDefault(eReportType, m_aReportHeadInfo[eReportType], m_aFixCol[eReportType]);
			}
		}
		//
		pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
	}
	//
	return true;
}

bool32 CReportScheme::SaveDataToXml()
{
	
	CString StrPath = GetPath();

	std::string StrPathA ;
	Unicode2MultiChar(CP_ACP, StrPath, StrPathA);
	//
	TCHAR TStrPath[2048];
	lstrcpy(TStrPath, StrPath);
	_tcheck_if_mkdir(TStrPath);
	//

	TiXmlDocument MyDoc = TiXmlDocument(StrPathA.c_str());

	CString StrXml		= L"";
	CString StrHeadInfo = L"";

	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"GridHead\">\n";

	for(int32 i = 0; i < ERTCount; i++)
	{
		E_ReportType eReportType = (E_ReportType)i;
				
		StrHeadInfo.Format( L"<%s %s=\"%d\" %s=\"%d\" %s=\"%s\">\n",
							CString(KStrElementNameHeadInfo).GetBuffer(),
							CString(KStrElementAttriHeadType).GetBuffer(), i,
							CString(KStrElementAttriFixCol).GetBuffer(), m_aFixCol[i],
							CString(KStrElementAttriHeadName).GetBuffer(), GetReportTypeCnName(eReportType).GetBuffer() );
		
		for(int32 j = 0; j < m_aReportHeadInfo[eReportType].GetSize(); j++ )
		{
			CString StrHeadItem;
			
			StrHeadItem.Format( L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\"/>\n",
								CString(KStrElementNameHeadItem).GetBuffer(), 
								CString(KStrElementAttriHeadName).GetBuffer(),  m_aReportHeadInfo[eReportType].GetAt(j).m_StrHeadNameCn.GetBuffer(),
								CString(KStrElementAttriHeadWidth).GetBuffer(), m_aReportHeadInfo[eReportType].GetAt(j).m_iHeadWidth,
								CString(KStrElementAttriHeadColumnType).GetBuffer(),  m_aReportHeadInfo[eReportType].GetAt(j).m_eReportHeader
								);
			
			StrHeadInfo += StrHeadItem;
		}
		
		StrHeadInfo += L"</";
		StrHeadInfo += (CString)KStrElementNameHeadInfo;
		StrHeadInfo += L">\n";
		
		//
		StrXml += StrHeadInfo;
	}	
		
	StrXml += L"</XMLDATA>";
	//


	SaveXmlFile(StrPathA.c_str(), StrXml);

	return true;
}


void CReportScheme::SetPrivateFolderName(CString& strName)
{
	m_strPrivateName = strName;
}


bool32 T_HeadInfo::InitilizeHeadInfo( CReportScheme::E_ReportHeader eHeader )
{
	ASSERT( m_eReportHeader <= CReportScheme::ERHCount );
	m_eReportHeader = eHeader;
	m_StrHeadNameCn = CReportScheme::GetReportHeaderCnName(eHeader);
	m_iHeadWidth	= CReportScheme::GetReportColDefaultWidth(eHeader);
	return m_eReportHeader <= CReportScheme::ERHCount;
}

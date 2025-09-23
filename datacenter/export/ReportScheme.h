#ifndef _REPORT_SCHEME_H_
#define _REPORT_SCHEME_H_

#define GHCOUNTS	 67
#define MAXWIDTH	 500
#define DEFUALTWIDTH 70
#include "typedef.h"
#include "Enumtype.h"
#include "DataCenterExport.h"

struct T_ReportHeaderObject;
struct T_ReportTypeObject;
struct T_HeadInfo;

class CGGTongDoc;
// 系统报价表题头
class DATACENTER_DLL_EXPORT CReportScheme
{
public:	
	// 所有表头种类
	// 报价表题头列表
	enum E_ReportHeader 
	{
		ERHRowNo = 0,			// 行号
		ERHMerchCode,			// 代码
		ERHMerchName,			// 名称

		// 
		ERHPricePrevClose,		// 昨收价
		ERHPricePrevBalance,	// 昨结收（期货中名称， 等同于股票中昨收价）

		// 
		ERHPriceOpen,			// 开盘价

		// 
		ERHPriceNew,			// 最新价
	 // ERHPriceClose,			// 收盘价（=最新价）
		ERHPriceBalance,		// 结算价（期货中名称， =收盘价）

		// 
		ERHPriceHigh,			// 最高 
		ERHPriceLow,			// 最低

		//
		ERHVolumeCur,			// 现手
		//
		ERHVolumeTotal,			// 成交量
		// 
		ERHAmount,				// 金额
	 	ERHHold,				// 持仓 
		ERHRiseFall,			// 涨跌
		ERHRange,				// 涨幅%
		ERHSwing,				// 振幅%
		ERHVolumeRate,			// 量比
		ERHPowerDegree,			// 强弱度
		ERHDKBallance,			// 多空平衡
		ERHDWin,				// 多头获利
		ERHDLose,				// 多头停损
		ERHKWin,				// 空头回补
		ERHKLose,				// 空头停损
		ERHRate,				// 委比
		ERHSpeedRiseFall,		// 快速涨跌
		ERHChangeRate,			// 换手率
		ERHMarketWinRate,		// 市盈率		
		ERHBuyPrice,			// 买入价
		ERHSellPrice,			// 卖出价
		ERHBuyAmount,			// 买入量
		ERHSellAmount,			// 卖出量
		ERHDifferenceHold,		// 持仓差
		ERHTime,				// 时间
		ERHBuySellPrice,		// 买入价/卖出价
		ERHBuySellVolume,		// 买/卖量
		ERHPreDone1,			// 前成1
		ERHPreDone2,			// 前成2
		ERHPreDone3,			// 前成3
		ERHPreDone4,			// 前成4
		ERHPreDone5,			// 前成5
		
		// fangz0609 Add for Warrants
		ERHTargetStockCode,		// 标的股
		ERHTargetStockName,		// 标的股名
		ERHSellOrBuy,			// 认购认沽
		ERHXQRate,				// 行权比
		ERHXQPrice,				// 行权价
		ERHMaturity,			// 到期日
		ERHDuration,			// 存续期
		ERHTheoreticalPrice,	// 理论价
		ERHPremium,				// 溢价
		ERHUsaOrEuro,			// 欧式美式
		ERHCirculateBallance,	// 流通余额
		ERHDistributors,		// 发行商
		ERHWarrantType,			// 权证类型
		ERHPriceInOut,			// 价内价外
		ERHValueInside,			// 内在价值
		ERHAmplitudeHistory,	// 历史波幅
		ERHAmplitudeExplicate,  // 引伸波幅
		ERHDelta,				// 对冲值
		ERHGAMMA,				// GAMMA
		ERHLeverRate,			// 杠杆比率
		ERHLeverTrue,			// 实际杠杆
		ERHStockChangeRate,		// 换股比率
		ERHVolumeWildGoods,  	// 街货量
		ERHRateWildGoods,		// 街货比例

		// fangz0811 Add for Futures
		ERHBuild,				// 开仓
		ERHClear,				// 平仓
		ERHBuildPer,			// 单笔开仓
		ERHClearPer,			// 单笔平仓
		ERHAddPer,				// 单笔增仓
		ERHBuyVolumn,			// 内盘
		ERHSellVolumn,			// 外盘
		ERHTrend,				// 走势

		// xl 0813 短线中线条件选股
		ERHShortLineSelect,		// 短线选股
		ERHMidLineSelect,		// 中线选股
		ERHCapitalFlow,			// 资金流向

		// xl 0820 财务数据
		ERHAllCapital,			// 总股本
		ERHCircAsset,			// 流通股本
		ERHAllAsset,			// 总资产
		ERHFlowDebt,			// 流动负债
		ERHPerFund,				// 每股公积金

		ERHBusinessProfit,		// 营业利益
		ERHPerNoDistribute,		// 每股未分配
		ERHPerIncomeYear,		// 每股收益(年)
		ERHPerPureAsset,		// 每股净资产
		ERHChPerPureAsset,		// 调整每股净资产

		ERHDorRightRate,		// 股东权益比// 
		ERHCircMarketValue,		// 流通市值
		ERHAllMarketValue,		// 总市值

		// xl 0820 内外比？
		ERHBuySellRate,			// 内外比

		// fangz 1015
		ERHPriceSell5,			// 卖5价
		ERHPriceSell4,			// 卖4价
		ERHPriceSell3,			// 卖3价
		ERHPriceSell2,			// 卖2价
		ERHPriceSell1,			// 卖1价

		ERHVolSell5,			// 卖5量
		ERHVolSell4,			// 卖4量
		ERHVolSell3,			// 卖3量
		ERHVolSell2,			// 卖2量
		ERHVolSell1,			// 卖1量

		ERHPriceBuy5,			// 买5价
		ERHPriceBuy4,			// 买4价
		ERHPriceBuy3,			// 买3价
		ERHPriceBuy2,			// 买2价
		ERHPriceBuy1,			// 买1价

		ERHVolBuy5,				// 买5量
		ERHVolBuy4,				// 买4量
		ERHVolBuy3,				// 买3量
		ERHVolBuy2,				// 买2量
		ERHVolBuy1,				// 买1量

		ERHIndustry,			// 所属行业
		ERHZixuan,				// 自选

		// 
		ERHAllNetAmount,		// 净流入
		ERHExBigBuyAmount,		// 超大单流入
		ERHExBigSellAmount,		// 超大单流出
		ERHExBigNetAmount,		// 超大单净额
		ERHExBigNetAmountProportion,		// 超大单净额占比 
		ERHExBigNetTotalAmount,				// 超大单总额
		ERHExBigNetTotalAmountProportion,	// 超大单总额占比
		
		//
		ERHBigBuyAmount,		// 大单流入
		ERHBigSellAmount,		// 大单流出
		ERHBigNetAmount,		// 大单净额
		ERHBigNetAmountProportion,			// 大单净额占比 
		ERHBigNetTotalAmount,				// 大单总额
		ERHBigNetTotalAmountProportion,		// 大单总额占比
		
		//
		ERHMidBuyAmount,		// 中单流入
		ERHMidSellAmount,		// 中单流出
		ERHMidNetAmount,		// 中单净额
		ERHMidNetAmountProportion,			// 中单净额占比 
		ERHMidNetTotalAmount,				// 中单总额
		ERHMidNetTotalAmountProportion,		// 中单总额占比
		
		//
		ERHSmallBuyAmount,		// 小单流入
		ERHSmallSellAmount,		// 小单流出
		ERHSmallNetAmount,	    // 小单净额
		ERHSmallNetAmountProportion,		// 小单净额占比 
		ERHSmallNetTotalAmount,				// 小单总额
		ERHSmallNetTotalAmountProportion,	// 小单总额占比
		
		//
		ERHSelStockTime,	    // 选股时间
		ERHSelPrice,            // 入选价格
		ERHSelRisePercent,      // 入选后涨幅%

		ERHTodayMasukuraProportion,		// 今日增仓占比
		ERHTodayRanked,					// 今日排名
		ERHTodayRise,					// 今日涨幅

		ERH2DaysMasukuraProportion,		// 2日增仓占比
		ERH2DaysRanked,					// 2日排名
		ERH2DaysRise,					// 2日涨幅

		ERH3DaysMasukuraProportion,		// 3日增仓占比
		ERH3DaysRanked,					// 3日排名
		ERH3DaysRise,					// 3日涨幅

		ERH5DaysMasukuraProportion,		// 5日增仓占比
		ERH5DaysRanked,					// 5日排名
		ERH5DaysRise,					// 5日涨幅

		ERH10DaysMasukuraProportion,	// 10日增仓占比
		ERH10DaysRanked,				// 10日排名
		ERH10DaysRise,					// 10日涨幅

		// 无效值汇总
		ERHMarketMargin,				// 市净率
		ERHNetProfit,					// 净利润
		ERHTotalProfit,					// 利润总额
		ERHassetsRate,					// 净资产收益率	
		ERHNetProfitGrowthRate,			// 净利润增长率
		ERHNetAmountofthemain,			// 主力净量
		
		ERHCount
	};	

public:
	CReportScheme(); 
	~CReportScheme();
	void Construct(CString strPrivateFolderName);
public:
	static CReportScheme*	Instance();	// 该类仅允许一个对象
	
public:
	static bool32	IsFuture(E_ReportType eReportType) 
	{
		if (  ERTFuturesCn == eReportType
		   || ERTFuturesForeign == eReportType
		   || ERTFutureRelaMonth == eReportType
		   || ERTMony == eReportType
		   || ERTExpForeign == eReportType 
		   || ERTFutureGold == eReportType ) 	  
		{
			return true;
		}
		
		return false;
	}

public:
	void			GetReportHeadInfoList(E_ReportType eReportType,OUT CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	void			GetHead(E_ReportType eReportType,OUT CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead);
	void			GetHead(IN CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead,E_ReportType eReportType);
	
	void			ChangeHeadInfoData(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	bool32			ReadDataFromXml();
	bool32			SaveDataToXml();
	void			SetHeadDataToDefault(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	void			SetBuySellPriceInfo(CString strBuyPrice, CString strSellPrice, CString strNewPrice);
		
	void			SaveColumnWidth(E_ReportType eReportType,int32 iColumn,int32 iWidth);
	void			DoHeadInfoString(CString StrHeadInfo,OUT CString& StrHeadName,OUT int32& iWidth);
	CString			HeadCnName2SummaryName(CString StrCnName);
	CString			HeadSummaryName2CnName(CString StrSummaryName);
	static CString	GetReportHeaderCnName(E_ReportHeader eReportHeader);
	static CString	GetReportTypeCnName(E_ReportType e_ReportType);	
	CString			GetPath();	
	static CString  GetFixColAttriName();
	static			CReportScheme::E_ReportHeader  GetReportHeaderEType(CString StrHeadCnName);
	void			SetPrivateFolderName(CString& strName);
	static int		GetReportColDefaultWidth(CReportScheme::E_ReportHeader eHeaderType);
	CString			HeadSpecialCnName2HeadCnName(CString strName);
	static CString	GetSpecialReportHeaderCnName(CString strName);

private:
	CArray<T_HeadInfo,T_HeadInfo>		m_aReportHeadInfo[ERTCount];
	int32								m_aFixCol[ERTCount];		
private:
	static CReportScheme	*s_Instance;
	CString	                m_strPrivateName;	
};

typedef struct T_ReportHeaderObject
{
	CReportScheme::E_ReportHeader	m_eReportHeader;
	CString			m_StrCnName;
	CString			m_StrEnName;
	CString			m_StrSummary;
			
 	T_ReportHeaderObject(const CReportScheme::E_ReportHeader eReportHeader, const CString &StrCnName, const CString &StrEnName, const CString &StrSummary)
 	{
		m_eReportHeader = eReportHeader;

 		m_StrCnName		= StrCnName;
 		m_StrEnName		= StrEnName;
		m_StrSummary	= StrSummary;	
	};

}T_ReportHeaderObject;

typedef struct T_ReportTypeObject
{
	E_ReportType		m_eReportType;
	CString			m_StrCnName;
	CString			m_StrEnName;
	CString			m_StrSummary;
	T_ReportTypeObject(const E_ReportType eReportType,const CString & StrCnName,const CString & StrEnName)
	{
		m_eReportType = eReportType;
		
		m_StrCnName	  = StrCnName;
		m_StrEnName   = StrEnName;
	};

}T_ReportTypeObject;

typedef struct T_HeadInfo 
{
	CString m_StrHeadNameCn;
	int32	m_iHeadWidth;
	CReportScheme::E_ReportHeader m_eReportHeader;

public:	
	T_HeadInfo(){ m_eReportHeader = CReportScheme::ERHCount; };

	T_HeadInfo(const CString& StrHeadNameCn, const int32 iHeadWidth)
	{
		m_StrHeadNameCn = StrHeadNameCn;
		m_iHeadWidth	= iHeadWidth;
		m_eReportHeader = CReportScheme::GetReportHeaderEType(m_StrHeadNameCn);
		ASSERT( m_eReportHeader != CReportScheme::ERHCount );
	};
	bool32 InitilizeHeadInfo(CReportScheme::E_ReportHeader eHeader);
}T_HeadInfo;


#endif

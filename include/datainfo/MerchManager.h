#ifndef _MERCH_MANAGER_H_
#define _MERCH_MANAGER_H_

#pragma warning(disable: 4786)

#include "GmtTime.h"
#include "LandMineStruct.h"
#include "InfoExport.h"

//lint -sem(CArray::Add, custodial(1))

class CMerch;
class CBroker;
class CMarket;
class CBreed;
class CMmiBroker;
 
struct T_OpenCloseTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 市场（物理市场）相关
///////////////////////////////////////////////
typedef struct T_OpenCloseTime
{
public:
	T_OpenCloseTime()
	{
		m_iTimeOpen		= 0;
		m_iTimeClose	= 0;
	}

public:
	int32			m_iTimeOpen;			// 格式：以分钟为单位， 相对于00:00分的值
	int32			m_iTimeClose;
}T_OpenCloseTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct T_ClientTradeTime
{
public:
	T_ClientTradeTime()
	{
		m_iWeek = -1;
		m_iOpen = -1;
		m_iClose = -1;
		m_aOCTimes.RemoveAll();
	}

	T_ClientTradeTime(const T_ClientTradeTime& stSrc)
	{
		m_iWeek = stSrc.m_iWeek;
		m_iOpen = stSrc.m_iOpen;
		m_iClose= stSrc.m_iClose;

		m_aOCTimes.Copy(stSrc.m_aOCTimes);
	}

	const T_ClientTradeTime& T_ClientTradeTime::operator=(const T_ClientTradeTime& stSrc)
	{
		if (this != &stSrc)
		{
			m_iWeek = stSrc.m_iWeek;
			m_iOpen = stSrc.m_iOpen;
			m_iClose= stSrc.m_iClose;

			m_aOCTimes.Copy(stSrc.m_aOCTimes);
		}

		return *this;
	}

	//
	int32	m_iWeek;	// 星期几的时间，-6表示星期六到星期天，其它时间表示通用时间
	long	m_iOpen;	// 开盘时间
	long    m_iClose;	// 收盘时间

	//
	CArray<T_OpenCloseTime, T_OpenCloseTime&> m_aOCTimes;	// 交易时间节

}T_ClientTradeTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// 
typedef CArray<T_ClientTradeTime, T_ClientTradeTime&> arrClientTradeTime;

typedef struct T_TradeTimeInfo
{
public:
	T_TradeTimeInfo()
	{
		m_iTradeTimeID = -1;
		m_aClientTradeTime.RemoveAll();
	}

	//
	T_TradeTimeInfo(const T_TradeTimeInfo& stSrc)
	{
		m_iTradeTimeID = stSrc.m_iTradeTimeID;
		m_aClientTradeTime.Copy(stSrc.m_aClientTradeTime);
	}

	const T_TradeTimeInfo& T_TradeTimeInfo::operator=(const T_TradeTimeInfo& stSrc)
	{
		if (this != &stSrc)
		{
			m_iTradeTimeID = stSrc.m_iTradeTimeID;
			m_aClientTradeTime.Copy(stSrc.m_aClientTradeTime);
		}

		return *this;
	}

	//
	int32				m_iTradeTimeID;
	arrClientTradeTime	m_aClientTradeTime;

}T_TradeTimeInfo;

typedef CArray<T_TradeTimeInfo, T_TradeTimeInfo&> arrClientTradeTimeInfo;

///////////////////////////////////////
// 商品相关
///////////////////////////////////////

// 商品基本信息
class DATAINFO_DLL_EXPORT CMerchInfo
{
public:
	CMerchInfo()
	{
		m_iMarketId = -1;
		m_iSaveDec = 1;
		m_bRemoved = false;
	}

public:
	int32			m_iMarketId;		// 所属市场代码
	CString			m_StrMerchCode;		// 商品代码
	CString			m_StrMerchCodeInBourse;		// 商品交易所代码（该商品交易时使用的代码， 目前对期货而言， 我们目前看到的代码都是服务器整理过的， 比较统一的格式， 方便辨识， 但是实际的交易所是有别于该代码的！）
	CString			m_StrMerchEnName;	// 商品英文名
	CString			m_StrMerchCnName;	// 商品中文名
	int32			m_iSaveDec;			// 保留小数点位数
	int32			m_iTradeTimeID;		// 交易时间ID，如果为表示使用市场的时间
	int32			m_iTradeTimeType;	// 交易时间类型

	// 
	bool32			m_bRemoved;			// 该商品是否已不存在于该市场中
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

// 商品扩展信息
class DATAINFO_DLL_EXPORT CMerchExtendData
{
public:
	CMerchExtendData();
	~CMerchExtendData();
	
public:
	// zhangbo 0320 #有待补充
	//...
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// F10
class DATAINFO_DLL_EXPORT CMerchF10Node
{
public:
	CString			m_StrTitle;
	CString			m_StrContent;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CMerchF10
{
public:
	CMerchF10();
	~CMerchF10();
	
public:
	void			ParseF10String(CString StrF10);

public:
	const CMerchF10& operator=(const CMerchF10& MerchF10Src);

public:
	CArray<CMerchF10Node, CMerchF10Node> m_F10Nodes;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CSmartAttendMerch
{
public:
	CSmartAttendMerch();

public:
	const bool32 operator ==( const CSmartAttendMerch& SmartAttendMerch);
public:
	CMerch		*m_pMerch;
	UINT		m_iDataServiceTypes;		// 取值 E_DataServiceType的或值
	bool32		m_bNeedNews;				// ...fangz0124 暂时处理,  是否需要资讯通知
};
typedef CArray<CSmartAttendMerch, CSmartAttendMerch&> SmartAttendMerchArray;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct T_AttendMarketData
{
	int32				m_iMarketId;		// 关心的市场
	int					m_iEDSTypes;		// 关心数据类型集合 E_DataServiceType
}T_AttendMarketData;
typedef CArray<T_AttendMarketData, const T_AttendMarketData &> AttendMarketDataArray;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 分时指标数据
typedef struct T_MerchTrendIndex
{
public:
	T_MerchTrendIndex()
	{
		memset(this, 0, sizeof(T_MerchTrendIndex));
	}
	
	bool32  BeValid()
	{
		if ( m_lTime <= 0 )
		{
			return false;
		}

		if ( m_fVolumeRate <= 0 && m_fBuyVolumeSum <= 0 &&  m_fSellVolumeSum <= 0 )
		{
			return false;
		}

		//if ( m_fVolumeRate <= 0 && )
		//{
		//	return false;
		//}

		//if ( m_fBuyVolumeSum <= 0 )
		//{
		//	return false;
		//}

		//if ( m_fSellVolumeSum <= 0 )
		//{
		//	return false;
		//}

		return true;
	}

	long    m_lTime;				// 时间
	float	m_fVolumeRate;			// 量比
	float	m_fBuyVolumeSum;		// 五档买量之和
	float	m_fSellVolumeSum;		// 五档卖量之和
	
} T_MerchTrendIndex;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// 集合竟价数据
typedef struct T_MerchAuction
{
public:
	T_MerchAuction()
	{
		memset(this, 0, sizeof(T_MerchAuction));
	}
	
	long	m_lTime;				// 时间
	float	m_fBuyPrice;			// 买价
	float	m_fBuyVolume;			// 买量
	char	m_cFlag;				// 方向
	
} T_MerchAuction;


/////////////////////////////////////////////////////////////////////////////////////////////////////////

// 分钟内外盘数据
typedef struct T_MinuteBS
{
public:
	T_MinuteBS()
	{
		memset(this, 0, sizeof(T_MinuteBS));
	}
	
	bool32 BeValid()
	{
		if ( m_lTime <= 0 )
		{
			return false;
		}

		if ( m_fBuyVolume < 0 )
		{
			return false;
		}

		if ( m_fSellVolume < 0 )
		{
			return false;
		}

		return true;
	}

	long    m_lTime;				// 时间	
	float	m_fBuyVolume;			// 内盘
	float	m_fSellVolume;			// 外盘
	
} T_MinuteBS;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 服务器支持的各种文件类型
enum E_PublicFileType
{
	EPFTWeight = 0,		// 除权
	EPFTF10,			// F10数据
	EPFTExtend,			// 扩展数据(暂不支持)
	
	//
	EPFTCount
};

// 除权数据
struct LONDATE { public: DWORD Minute:6; DWORD Hour:5; DWORD Day:5; DWORD Month:4; DWORD Year:12; };

#pragma pack(1) 
struct T_WeightFileFormat
{
	LONDATE Date; 
	DWORD A,B,C,D,E;		// 送股数,配股数,配股价,红利,增股数 
	DWORD Base,FlowBase;	// 总股本,流通股本 
	DWORD Reserved;
};
#pragma pack()


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CWeightData
{
public:
	CWeightData()
	{
		B01 = B02 = B03 = B04 = B05 = 0.;
		Base = FlowBase = 0;
	}

public:
	void Init(const T_WeightFileFormat &WeightFileFormat)
	{
		m_TimeCurrent = CGmtTime(WeightFileFormat.Date.Year, WeightFileFormat.Date.Month, WeightFileFormat.Date.Day, WeightFileFormat.Date.Hour, WeightFileFormat.Date.Minute, 0);

		B01 = (float)WeightFileFormat.D;
		B02 = (float)WeightFileFormat.C;
		B03 = (float)WeightFileFormat.A;
		B04 = (float)WeightFileFormat.B;
		B05 = (float)WeightFileFormat.E;

		Base = WeightFileFormat.Base;
		FlowBase = WeightFileFormat.FlowBase;
	}

	void WeightPrice(INOUT float &fPrice, bool32 bFront) const 
	{
		if (bFront)	// 前复权： 复权价=(原价-红利+配股金额)/(原股本+送股+配股)
		{
			if (10.0 + B03 + B04 != 0.)
			{
				// 通达信算法
				//fPrice = (10.0 * fPrice - B01 + B04 * B02) / (10.0 + B03 + B04);	

				// 彭博算法
				fPrice *= 1000;
				fPrice = (float)((fPrice * 10.0 + B04 * 0.0001 * B02 - B01) / (10.0 + B03 * 0.0001 + B04 * 0.0001 + B05 * 0.0001));
				fPrice /= 1000;
			}
		}
		else		// 后复权： 
		{
			// 通达信算法
			//fPrice = (fPrice * (10.0 + B03 + B04) + B01 - B04 * B02) / 10.0;

			// 彭博算法
			fPrice *= 1000;
			fPrice = (float)(((10.0 + B03 * 0.0001 + B04 * 0.0001 + B05 * 0.0001) * fPrice - B04 * 0.0001 * B02 + B01) * 0.1);
			fPrice /= 1000;
		}
	}

	void WeightVolumeAndAmount(INOUT float &fVolume, INOUT float &fAmount, bool32 bFront) const
	{
		if (bFront)	// 前复权： 
		{
			//fAmount = fAmount + B02 * B04 / 10 * fVolume - fVolume * B01 / 10;
			//fVolume = fVolume + fVolume * (B03 + B04) / 100000.0;
			// 成交额不变，成交量与价格的变化成反比
			if ( 0.0f != fVolume )
			{
				float fPriceOld = fAmount/fVolume;
				WeightPrice(fPriceOld, bFront);
				fVolume = fAmount/fPriceOld;
			}
		}
		else		// 后复权： 
		{
			if (10 + B03 + B04 != 0)
			{
				//fVolume = fVolume * 100000.0 / (100000 + B03 + B04);
				//fAmount = fAmount - B02 * B04 / 10 * fVolume + fVolume * B01 / 10;
				// 成交额不变，成交量与价格的变化成反比
				if ( 0.0f != fVolume )
				{
					float fPriceOld = fAmount/fVolume;
					WeightPrice(fPriceOld, bFront);
					fVolume = fAmount/fPriceOld;
				}
			}		
		}
	}

public:
	CGmtTime		m_TimeCurrent;	// 时间（日）
	
	float			B03;			// 送股数
	float			B04;			// 配股数
	float			B02;			// 配股价
	float			B01;			// 红利
	float			B05;			// 增股数
	
	DWORD			Base;			// 总股本
	DWORD			FlowBase;		// 流通股本
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CFinanceData
{
public:
	CFinanceData()
	{		
		m_fAllCapical       = 0.;		
		m_fCircAsset		= 0.;		
		m_fAllAsset         = 0.;		
		m_fFlowDebt         = 0.;		
		m_fPerFund          = 0.;			
		
		m_fBusinessProfit   = 0.;	
		m_fPerNoDistribute  = 0.;	
		m_fPerIncomeYear    = 0.;	
		m_fPerPureAsset     = 0.;	
		m_fChPerPureAsset   = 0.;	
		
		m_fDorRightRate     = 0.;	
		m_iIncomeSeason		= 0;
	}

public:
	float	m_fAllCapical;			// 总股本
	float	m_fCircAsset;			// 流通股本
	float	m_fAllAsset;			// 总资产
	float	m_fFlowDebt;			// 流动负债
	float	m_fPerFund;				// 每股公积金
	
	float	m_fBusinessProfit;		// 营业利益
	float	m_fPerNoDistribute;		// 每股未分配
	float	m_fPerIncomeYear;		// 每股收益(年)
	float	m_fPerPureAsset;		// 每股净资产
	float	m_fChPerPureAsset;		// 调整每股净资产
	
	float	m_fDorRightRate;		// 股东权益比
	int32	m_iIncomeSeason;		// 收益季度

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 市场时间
class DATAINFO_DLL_EXPORT CMarketIOCTimeInfo
{
public:
	CMarketIOCTimeInfo(){}
	CMarketIOCTimeInfo(const CMarketIOCTimeInfo &Src)
	{
		m_TimeInit	= Src.m_TimeInit;
		m_TimeOpen	= Src.m_TimeOpen;
		m_TimeClose = Src.m_TimeClose;
		m_TimeEnd	= Src.m_TimeEnd;
		
		m_aOCTimes.Copy(Src.m_aOCTimes);
	}
	const CMarketIOCTimeInfo& operator=(const CMarketIOCTimeInfo &Src)
	{
		m_TimeInit	= Src.m_TimeInit;
		m_TimeOpen	= Src.m_TimeOpen;
		m_TimeClose = Src.m_TimeClose;
		m_TimeEnd	= Src.m_TimeEnd;
		
		m_aOCTimes.Copy(Src.m_aOCTimes);
		
		return *this;
	}
	
	// 计算当前商品一个交易日的分钟线条数
	int32 GetMaxTrendUnitCount() const
	{
		int32 iCount = 0;
		for (int32 i = 0; i < m_aOCTimes.GetSize(); i += 2)
		{
			iCount += (int32)((m_aOCTimes[i + 1].GetTime() - m_aOCTimes[i].GetTime()) / 60 + 1);
		}
		
		return iCount;
	}
	
public:
	CMsTime		m_TimeInit;		// 一天开始
	CMsTime		m_TimeOpen;		// 开盘
	CMsTime		m_TimeClose;	// 收盘			// 对于跨天交易， 交易日按Close时间所在天为准， 比如对于外汇， 星期一的交易时间为：星期天17:00 ~ 星期一17:00
	CMsTime		m_TimeEnd;		// 一天结束
	
	CArray<CGmtTime, const CGmtTime&>	m_aOCTimes;		// 交易 + 休息 + 交易 + 休息 + ..., 总是双数的
};

// 基本报价信息
#define PRICE_ARRAY_SIZE	20			// 20档

/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct T_PriceVolume
{
public:
	T_PriceVolume()
	{
		m_fPrice	= 0.;
		m_fVolume	= 0.;
	}

public:
	float			m_fPrice;
	float			m_fVolume;
}T_PriceVolume;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CRealtimePrice
{
public:
	enum E_UpdateKLineType
	{
		EUKTNone = 0,			// 不更新K线， 表明该笔数据不是一笔交易数据， 很可能是5档行情发生了变化
		EUKTUpdate,				// 更新所有类型(日，月， 分， 5分， 60分)的K线
		EUKTUpdateOnlyDayKLine,	// 仅更新日线

		// 
		EUKTCount
	};

public: 
	CRealtimePrice();
	~CRealtimePrice();

public:
	bool32			operator==(const CRealtimePrice &RealtimePrice) const;
	// CRealtimePrice& CRealtimePrice::operator= (const CRealtimePrice& RealtimePrice);

public:
	void			ReviseVolume(int32 iVolScale);
	void			InitMerchPrice();	// 集合竞价时间， 初始化市场

public:
	E_UpdateKLineType	m_eUpdateKLineType;

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	// 	
	CMsTime			m_TimeCurrent;

	// 
	float			m_fPricePrevClose;	// 前收价
	float			m_fPricePrevAvg;	// 前结算

	// 
	float			m_fPriceOpen;		// 开盘价
	float			m_fPriceHigh;		// 最高价
	float			m_fPriceLow;		// 最低价
	float			m_fPriceNew;		// 最新价
	float			m_fPriceAvg;		// 结算价

	// 
	float			m_fHoldPrev;		// 前持仓

	float			m_fVolumeCur;		// 当前成交量（股票中的“现手”）
	float			m_fVolumeTotal;		// 总成交量 （股票中的“总手”）
	float			m_fAmountTotal;		// 成交金额
	float			m_fHoldCur;			// 期货中的增仓，其他商品暂无意义 
	float			m_fHoldTotal;		// 期货中持仓量，其他商品暂无意义
	
	// 
	float			m_fVolumeRate;		// 量比
	float			m_fBuyVolume;		// 内盘, 以买价成交
	float			m_fSellVolume;		// 外盘, 以卖家成交

	// 买五、卖五
	T_PriceVolume	m_astBuyPrices[PRICE_ARRAY_SIZE];	// 买价列表， 买一（高）-> 买五（低）
	T_PriceVolume	m_astSellPrices[PRICE_ARRAY_SIZE];	// 卖价列表， 卖一（低）-> 卖五（高）
	
	//
	float			m_fPeRate;				// 市盈率
	float			m_fTradeRate;			// 换手率	
	float			m_fRiseRate;			// 涨速
	float			m_fCapticalFlow;		// 资金流向

	//
	float			m_fSellAmount;			// 外盘成交额	
	UINT			m_uiSeason;				// 季度
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

enum E_TickType
{
	ETTUnknown  = 0,	// 不明盘
	ETTBuy		= 1,	// 买价
	ETTSell		= 2,	// 卖价
	ETTTrade	= 4,	// 成交价

	// 
	ETTCount		
};

enum E_RealtimePushType
{
	ERPTUnknown			= 0,	// 
	ERPTPushPrice		= 1,	// 推送买卖盘
	ERPTPushTick		= 2,	// 推送Tick
	ERPTPushLevel2		= 4,	// 推送Level2
		
	// 
	ERPTCount		
};

// 成交明细数据
class DATAINFO_DLL_EXPORT CTick
{
public:
	enum E_TradeKind
	{
		ETKUnKnown = 0,		// 其它
		ETKBuy,				// 以买价成交,内盘
		ETKSell,			// 以卖价成交,外盘
		
		//
		ETKCount		
	};

public:
	CTick();

public:
	bool32			MaybeFault() const;
	bool32			operator==(const CTick &Tick) const;

	void			ReviseVolume(int32 iVolScale);

public:
	E_TickType		m_eTickType;		// 类型

public:
	CMsTime			m_TimeCurrent;
	
	float			m_fPrice;			// 价格
	float			m_fVolume;			// 现手
	float			m_fAmount;			// 该笔成交金额
	float			m_fHold;			// 总持仓量
	E_TradeKind		m_eTradeKind;		// 买卖盘信息
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CPrevPriceInfo
{
public:
	CPrevPriceInfo() 
	{
		m_uiDay = 0;
		m_fPrevClosePrice = 0.;
		m_fPrevAvgPrice = 0.;
	}
	
public:
	uint32			m_uiDay;				// 本记录描述的日期
	float			m_fPrevClosePrice;		// 昨收价
	float			m_fPrevAvgPrice;		// 昨结收
};

class DATAINFO_DLL_EXPORT CMerchTimeSales
{
public:
	CMerchTimeSales();
	~CMerchTimeSales();

public:
	
	static int32	QuickFindTickByTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer, bool32 bNearest);
	static int32	QuickFindTickByTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer, bool32 bNearest);

	static int32	QuickFindTickWithBigOrEqualReferTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer);
	static int32	QuickFindTickWithBigOrEqualReferTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer);

	static int32	QuickFindTickWithSmallOrEqualReferTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer);
	static int32	QuickFindTickWithSmallOrEqualReferTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer);

public:
	// 处理分笔成交历史数据
	bool32			ProcessTimesSaleData(const CMerchTimeSales	&MerchTimeSales);

	// 处理实时分笔数据
	bool32			ProcessRealtimeTick(const CTick &RealtimeTick);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CTick, CTick> m_Ticks;		// 分笔列表

	uint32			m_uiTailTime;		// 最早时间
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

// level2
class DATAINFO_DLL_EXPORT CLevel2Detail
{
public:
	CLevel2Detail()
	{
		m_bPos		= false;
		m_bBuy		= false;
		
		m_fPrice	= 0.;
		m_fVolume	= 0.;
	}

public:
	// 描述当前结构是不是描述经纪人下单的数据
	bool32			m_bPos;

	// 如果是经纪人
	CString			m_StrBrokerCode;	// 买卖经济席位代码
	CString			m_StrBrokerName;	// 买卖经济席位名称
	bool32			m_bBuy;				// 买/卖标志
	
	// 如果不是经纪人（档位）
	CString			m_StrPos;			// 档位名称（如果是档位）

	// 下面两个字段目前没有数据
	float			m_fPrice;			// 价 
	float			m_fVolume;			// 买（卖）量
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CLevel2Node
{
public:
	enum E_Level2Type { ELTBuy = 0, ELTSell = 1 };	// level2买/卖

public:
	CLevel2Node(); 
	CLevel2Node(const CLevel2Node& Level2NodeSrc);
	~CLevel2Node();

public:
	const CLevel2Node& operator=(const CLevel2Node& Level2NodeSrc);
	
public:
	float			m_fPrice;		// 买（卖）价
	E_Level2Type	m_eLevel2Type;	// 买/卖
	float			m_fVolume;		// 总成交量（各个经济席位在该价位买（卖）盘量总和）
	CArray<CLevel2Detail, CLevel2Detail> m_Level2Details;	// 该买（卖）盘价位上各个经济席位的信息
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CRealtimeLevel2
{
public:
	CRealtimeLevel2();
	~CRealtimeLevel2();

public:
	const CRealtimeLevel2& operator=(const CRealtimeLevel2& RealtimeLevel2Src);

public:
	CGmtTime		m_TimeCurrent;		// 该时间为商品所在时区的时间， 仅对相关字段赋值， 其它均为0值， 系统里面对该字段的处理均按该规则处理， 比如仅精确度到分钟

	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	float			m_fPriceAveBuy;		// 买入均价
	float			m_fPriceAveSell;	// 卖出均价
	
	float			m_fVolumeBuyTotal;	// 买量总和
	float			m_fVolumeSellTotal;	// 卖量总和

	CArray<CLevel2Node, CLevel2Node> m_Level2BuyNodes;	// 买价列表， 从高价到低价
	CArray<CLevel2Node, CLevel2Node> m_Level2SellNodes;	// 卖价列表， 从低价到高价


	CArray<CLevel2Detail, CLevel2Detail> m_xxxBuyLevel2List;	// 该买（卖）盘价位上各个经济席位的信息
	CArray<CLevel2Detail, CLevel2Detail> m_yyySellLevel2List;	// 该买（卖）盘价位上各个经济席位的信息
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// K线
class DATAINFO_DLL_EXPORT CKLine
{
public:
	CKLine()
	{
		m_fPriceOpen		= 0.;
		m_fPriceClose		= 0.;
		m_fPriceAvg			= 0.;
		m_fPriceHigh		= 0.;
		m_fPriceLow			= 0.;

		m_fVolume			= 0.;
		m_fAmount			= 0.;
		m_fHold				= 0.;

		m_usRiseMerchCount	= 0;
		m_usFallMerchCount	= 0;
	}

public:
	bool32			MaybeFault() const;
	bool32			operator==(const CKLine &KLine) const;
	CKLine&			operator+=(const CKLine &KLine);
	
	void			ReviseVolume(int32 iVolScale);

public:
	CGmtTime		m_TimeCurrent;		// 该时间针对不同的周期K线， 应该仅对相关字段赋值， 其它均为0值， 系统里面对该字段的处理均按该规则处理， 比如对日线， 时、分、秒均须设为0

	// 
	float			m_fPriceOpen;		// 开
	float			m_fPriceClose;		// 收
	float			m_fPriceAvg;		// 结算（均）
	float			m_fPriceHigh;		// 高
	float			m_fPriceLow;		// 低

	// 
	float			m_fVolume;			// 量
	float			m_fAmount;			// 金
	float			m_fHold;			// 仓	
	
	// 下面两个字段仅对大盘有意义
	uint16			m_usRiseMerchCount;		// 整个市场在该周期内的上涨家数
	uint16			m_usFallMerchCount;		// 整个市场在该周期内的下跌家数
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
enum E_KLineTypeBase	// 基本K线类型, 其他的K线类型或周期均由以下基础K线组合而来
{
	EKTBMinute = 0,		// 分钟线
	EKTBDay,			// 日线
	EKTB5Min,			// 5分钟线
	EKTBHour,			// 小时线
	EKTBMonth,			// 月线
	
	//
	EKTBCount			// 基本类型数量
};


class DATAINFO_DLL_EXPORT CMerchKLineNode
{
public:
	CMerchKLineNode();
	virtual ~CMerchKLineNode();

	CMerchKLineNode& CMerchKLineNode::operator= (const CMerchKLineNode& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}

		//
		m_iMarketId		= stSrc.m_iMarketId;			// 市场编号
		m_StrMerchCode	= stSrc.m_StrMerchCode;			// 商品代码		
		m_eKLineTypeBase= stSrc.m_eKLineTypeBase;		// K线类型
		m_KLines.Copy(stSrc.m_KLines);					// K线序列		
		m_uiTailTime = stSrc.m_uiTailTime;				// 最早时间

		return *this;
	}

public:
	// 二分查找快速定位
	static int32	QuickFindKLineByTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer, bool32 bNearest = false);
	static int32	QuickFindKLineWithBigOrEqualReferTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer);
	static int32	QuickFindKLineWithSmallOrEqualReferTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer);
	
	// K线合并
	static bool32	GetKLineTimeByMinute(const IN CMerch* pMerch, IN UINT uiTimeCurrent, IN int32 iMinutePeriod, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd);	
	static bool32	CombinMinuteN(const IN CMerch* pMerch, IN int32 iCombinPeriod, IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinDayN(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN int32 iCombinPeriod, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinMonthN(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN int32 iCombinPeriod, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinWeek(IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);

	// 复权
	// bFront			-- 前复权
	// bWeightVolume	-- 复权成交量
	// uiTimeSpecify	-- 定点复权时间， 默认0xffffffff表示忽略该时间
	static bool32	WeightKLine(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN const CArray<CWeightData, CWeightData&> &aWeightDatas, bool32 bFront, OUT CArray<CKLine, CKLine> &aKLinesDst, IN bool32 bWeightVolume = false, IN uint32 uiTimeSpecify = 0xffffffff);
	static void		DoWeight(INOUT CKLine &KLine, const CWeightData *pWeightDatas, int32 iWeightIndex, int32 iWeightCount, bool32 bFront, bool32 bWeightVolume);	// 单点复权

	//
	static bool32	CheckKLineList(const CArray<CKLine, CKLine> &aKLines);

private:	
	//
	static bool32	GetKLineTimeByMinute(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN UINT uiTimeCurrent, IN int32 iMinutePeriod, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd, bool32 bNeedAdjust);
	static bool32	CombinMinuteN(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN int32 iCombinPeriod, IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);

public:
	// 处理一段K线数据
	bool32			ProcessKLindData(const CMerchKLineNode &MerchKLineNode);

	// 增加一笔实时数据
	// bool32			ProcessRealtimePrice(IN int32 iTimeInitPerDay, const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, const CRealtimePrice &RealtimePrice);
	
	bool32			ProcessRealtimePrice(IN CMerch* pMerch, const CRealtimePrice &RealtimePrice);

private:
	// 增加一笔K线数据
	bool32			AddKLine(IN const CKLine &KLine, int32 iPosStart, OUT int32 &iPosAdd);
	
	
public:
	int32			m_iMarketId;			// 市场编号
	CString			m_StrMerchCode;			// 商品代码

	E_KLineTypeBase	m_eKLineTypeBase;		// K线类型
	CArray<CKLine, CKLine> m_KLines;		// K线序列
	
	uint32			m_uiTailTime;			// 最早时间
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 商品数据信息 
class DATAINFO_DLL_EXPORT CMerch
{
public:
	CMerch(CMarket &Market);
	~CMerch();

public:
	static bool32	ReadWeightDatasFromBuffer(const char *pcData, int32 iDataLen, OUT CArray<CWeightData, CWeightData&> &aWeightDatas);
	static bool32	ReadFinanceDataFromBuffer(const char *pcData, int32 iDataLen, OUT CFinanceData& FinanceData);

public:
	void			FreeAllHistoryData();
	void			FreeAllRealtimeData();
 
	bool32			DoesHaveHistoryData();

	bool32			DoesNeedPushPrice(IN const CGmtTime &TimeNow);
	bool32			DoesNeedPushTick(IN const CGmtTime &TimeNow);
	bool32			DoesNeedPushLevel2(IN const CGmtTime &TimeNow);

	bool32			DoesNeedReqWeightData(IN const CGmtTime &TimeNow);
	bool32			DoesNeedReqFinanceData(IN const CGmtTime &TimeNow);

public:
	// 查找指定类型（K线基本类型）的K线历史数据
	bool32			FindMerchKLineNode(E_KLineTypeBase eKLinetypeBase, OUT int32 &iPosFound, CMerchKLineNode *&pMerchKLineNodeFound) const;

	// 查找指定某天的昨收价， 为走势图显示用
	bool32			GetSpecialDayPrice(const CGmtTime &TimeSpecialDay, float &fPrevClosePrice, float &fPrevAvgPrice) const;

	// 查找指定某天的持仓
	bool32			GetSpecialDayHold(const CGmtTime &TimeSpecialDay, float &fPrevHold) const;

	// 获取相关时间
	bool32			GetOCTime(IN int32 uiTimeCurrent, OUT int32& iTimeInit, OUT CArray<T_OpenCloseTime, T_OpenCloseTime>& aOCTimes);

public:
	// 集合竟价数据更新
	void			OnMerchAucitonsUpdate(const CArray<T_MerchAuction, T_MerchAuction&>& aAuctions);

	// 分时走势数据更新
	bool32			OnMerchTrendIndexUpdate(const CArray<T_MerchTrendIndex, T_MerchTrendIndex&>& aMerchTrendIndex);

	// 分钟内外盘数据更新
	bool32			OnMerchMinuteBSUpdate(const CArray<T_MinuteBS, T_MinuteBS&>& aMerchMinuteBS);

	CString			GetMerchVarName();
public:
	CMerchInfo		m_MerchInfo;			// 商品基本信息
	
	// 以下数据非实时数据， 请求一遍， 一天内不需要更新， 隔天清理
	CMerchExtendData	*m_pMerchExtendData;// 商品扩展数据
	CMerchF10		*m_pMerchF10;			// 商品F10信息
	
	// 以下数据为实时推送的数据
	CRealtimePrice	*m_pRealtimePrice;		// 商品即时5档报价
	CTick			*m_pRealtimeTick;		// 商品即时分笔信息
	CRealtimeLevel2	*m_pRealtimeLevel2;		// 商品即时level2报价

	// 以下数据为商品的历史数据
	CArray<CMerchKLineNode*, CMerchKLineNode*> m_MerchKLineNodesPtr;// 商品的几个基本类型K线数据, 这里的每一个K线数据都是从当前时间起得数据， 不允许从中间抽出一段数据
	CMerchTimeSales	*m_pMerchTimeSales;		// 商品成交明细
	
	// 除权数据
	CArray<CWeightData, CWeightData&> m_aWeightDatas;
	uint32			m_uiWeightDataCRC;
	CGmtTime		m_TimeLastUpdateWeightDatas;
	bool32			m_bHaveReadLocalWeightFile;		// 是否从本地读取过除权数据

	// 财务数据 fangz 0818 暂时没有保存本地财务数据. 都是从服务器请求的
	CGmtTime		m_TimeLastUpdateFinanceData;	// 最后一次收到财务数据，若干时间后一次才允许更新
	CFinanceData*	m_pFinanceData;

	// 信息地雷
	mapLandMine		m_mapLandMine;

	// 集合竟价数据
	CArray<T_MerchAuction, T_MerchAuction&>	m_aAuctionDatas;

	// 分时走势指标数据
	CArray<T_MerchTrendIndex, T_MerchTrendIndex&> m_aMerchTrendIndexs; 

	// 分钟内外盘数据
	CArray<T_MinuteBS, T_MinuteBS&>	m_aMinuteBS;

	// 选股时间
	uint32 m_uiSelStockTime;
	// 入选价格
	float  m_fSelPrice;

public:
	CMarket			&m_Market;				// 商品所属市场指针， 快速定位市场
	CString			m_StrMerchFakeName;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 经济席位信息
///////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CBroker
{
public:
	CBroker(CBreed &Breed);
	~CBroker();

public:
	const CBroker& operator=(const CMmiBroker& MmiBroker);

public:
	CString			m_StrBrokerCode;	// 经济席位代码
	CString			m_StrBrokerCnName;	// 经济席位名称
	CString			m_StrBrokerEnName;

	bool32			m_bRemoved;

public:
	CBreed			&m_Breed;				// 商品所属交易品种指针， 快速定位交易品种
};


// 市场信息  
class DATAINFO_DLL_EXPORT CMarketInfo 
{
public:
	CMarketInfo();
	~CMarketInfo();

public:
	static bool32	GetDayTime(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN UINT uiTimeCurrent, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd, OUT UINT &uiTimeOpen, OUT UINT &uiTimeClose);

public:
	const CMarketInfo& operator=(const CMarketInfo& MarketInfoSrc);
	void GetTimeInfo(int32 &iTimeInitPerDay, int32 &iTimeOpenPerDay, int32 &iTimeClosePerDay);	// 取市场的初始化,开,收盘时间

public:
	int32			m_iMarketId;			// 市场编号
	int32			m_iShowId;				// 显示号
	int32			m_iTimeZone;			// 该市场所属时区

	//
	CString			m_StrBourseCode;		// 在所在交易所的市场代码
	CString			m_StrEnName;			// 英文名称
	CString			m_StrCnName;			// 中文名称
	
	// 
	int32			m_iTimeInitializePerDay;// 每天初始化时间。格式：以分钟为单位， 相对于00:00分的值
											// 客户端每天都需要对系统的商品列表做初始化， 
											// 该时间描述每次初始化的时间（格林威治时间）， 对于国内股市， 该时间即“集合竞价”时间
	// 
	CArray<T_OpenCloseTime, T_OpenCloseTime>	m_OpenCloseTimes;	// 每天几个开盘时间段（格林威治时间）

	// 后两天的交易日和前的交易日(如今天明天,然后往昨天10个交易日)
	CArray<uint32, uint32>	m_aRecentActiveDay;	

	//
	E_ReportType	m_eMarketReportType;	// 市场的报价种类

	//
	int32			m_iVolScale;			// 成交量显示时, 需要除的倍数
};

// 市场快照
class DATAINFO_DLL_EXPORT CMarketSnapshotInfo
{
public:
	CMarketSnapshotInfo() { m_iMarketId = -1; m_iRiseCount = 0; m_iFallCount = 0; }

public:
	bool32 operator==(const CMarketSnapshotInfo &MarketSnapshotInfo) const
	{
		if (m_iMarketId != MarketSnapshotInfo.m_iMarketId)
			return false;

		if (m_iRiseCount != MarketSnapshotInfo.m_iRiseCount)
			return false;

		if (m_iFallCount != MarketSnapshotInfo.m_iFallCount)
			return false;

		return true;
	}

public:
	int32			m_iMarketId;

	int32			m_iRiseCount;		// 上涨家数
	int32			m_iFallCount;		// 下跌家数
};

// 市场初始化状态
class DATAINFO_DLL_EXPORT CMarketReqState
{
public:
	CMarketReqState() { m_bRequesting = false; m_iCommunicationId = -1; }
	~CMarketReqState() { NULL; }

public:
	bool32			m_bRequesting;
	int32			m_iCommunicationId;
};

// 特殊商品的交易时间
class DATAINFO_DLL_EXPORT CMerchClientTradeTimeInfo
{
public:
	CMerchClientTradeTimeInfo() {m_aClientTradeTimeInfo.RemoveAll();}
	~CMerchClientTradeTimeInfo(){}
public:
	static arrClientTradeTimeInfo	m_aClientTradeTimeInfo;	// 特殊商品的交易时间
};


// 市场结构
class DATAINFO_DLL_EXPORT CMarket
{
public:
	CMarket(CBreed &m_Breed);
	~CMarket();
	
public:
	void			SetInitialized(bool32 bInitialized) { m_bInitialized = bInitialized; };

	// 该市场是否初始化过
	bool32			IsInitialized() { return m_bInitialized; };
	
	// 是否需要重新初始化
	bool32			DoesNeedInitialize(IN const CGmtTime &TimeNow);
	
	// 查找指定代码的商品
	bool32			FindMerch(IN const CString &StrMerchCode, OUT int32 &iPosFound, OUT CMerch *&pMerchFound);

	// 增加一个商品（不判断是否有重复）
	bool32			AddMerch(CMerchInfo &MerchInfo);

	// 查找指定时间是否开盘
	bool32			IsTradingDay(const CGmtTime &Time);
	
	// 获取某一天的市场时间相关信息(不判断是否有节假日类影响到指定天不开盘的情况)
	bool32			GetSpecialTradingDayTime(const CGmtTime &Time, CMarketIOCTimeInfo &TradingDayTime, const CMerchInfo& pMerchInfo);

	// 获取最近前后3个交易日
	bool32			GetRecent3TradingDay(const CGmtTime &Time, bool32 &bValidPrevTradingDay, CMarketIOCTimeInfo &PrevTradingDayTime, bool32 &bValidCurTradingDay, CMarketIOCTimeInfo &CurTradingDayTime, bool32 &bValidNextTradingDay, CMarketIOCTimeInfo &NextTradingDayTime, const CMerchInfo& MerchInfo);

	// 获取当前需要显示的交易日信息
	bool32			GetRecentTradingDay(const CGmtTime &Time, CMarketIOCTimeInfo &RecentTradingDayTime, const CMerchInfo& MerchInfo);

public:
	CMarketInfo		m_MarketInfo;			// 物理市场信息
	CMarketSnapshotInfo	m_MarketSnapshotInfo;	// 市场的快照信息

	// 
	CGmtTime		m_TimeLastInitialize;	// 最后一次初始化成功的时间（该市场所在时区的时间）

	// 
	CArray<CMerch*, CMerch*> m_MerchsPtr;	// 该市场下所有商品			// zhangbo 0326 #可优化，该处的数组可以是经过排序的， 所有关于该数据的相关操作可以对利用二分查找等算法大大加快查找过程
	std::map<CString, int32> m_aMapMerchs;	// 方便快速查找商品
	
	// 市场信息和商品列表请求状态
	CMarketReqState	m_MarketInfoReqState;
	CMarketReqState	m_MerchListReqState;
	
public:
	CBreed			&m_Breed;				// 所属交易品种信息

private: 
	bool32			m_bInitialized;			// 是否初始化过。至少初始化过一次， 哪怕是超过一个交易日前初始化的
};

///////////////////////////////////////////////
// 交易品种（如：大陆股票、香港股票）相关
///////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CBreed
{
public:
	CBreed();
	~CBreed();

public:
	// 查找指定代码的市场
	bool32			FindMarket(int iMarketId, OUT int32 &iPosFound, OUT CMarket *&pMarketFound);

	// 查找指定代码的商品
	bool32			FindMerch(IN const CString &StrMerchCode, OUT CMerch *&pMerchFound);

	// 查找指定代码的经济席位
	bool32			FindBroker(IN const CString &StrBrokerCode, OUT int32 &iPosFound, OUT CBroker *&pBrokerFound);

	// 增加一个市场（不判断是否有重复）
	bool32			AddMarket(CMarket *pMarket);

	// 增加一个经济席位（不判断是否有重复）
	bool32			AddBroker(CMmiBroker &BrokerSrc);

public:
	int32			m_iBreedId;				// 交易品种编号， 如：“上海证券市场”编号为0
	int32			m_iShowId;				// 显示的顺序
	CString			m_StrBreedCnName;		// 交易品种名称， 如：“上海证券市场”， “香港证券市场”， “全球外汇市场”。。。
	CString			m_StrBreedEnName;

	CArray<CMarket*, CMarket*> m_MarketListPtr;	// 市场列表
	std::map<int32, int32> m_aMapMarkets;	// 方便快速查找市场
	
	CArray<CBroker*, CBroker*> m_BrokersPtr;// 该市场下所有的经济席位	// zhangbo 0326 #可优化，该处的数组可以是经过排序的， 所有关于该数据的相关操作可以对利用二分查找等算法大大加快查找过程
	std::map<CString, int32> m_aMapBrokers;	// 方便快速查找经纪人
};

// 
// /////////////////////////////////////////////////////////////
// // 相关商品
class DATAINFO_DLL_EXPORT CSiblingMerch
{
public:
	CSiblingMerch() { m_pMerch = NULL; }

public:
	CMerch			*m_pMerch;
	
	//
	CString			m_StrSiblingName;
};

class DATAINFO_DLL_EXPORT CRelativeMerchNode
{
public: 
	const CRelativeMerchNode& operator=(const CRelativeMerchNode &Src);

public:
	CString			m_RelativeName;			// 关系名称
	CString			m_RelativeSummary;		// 关系描述
	
	CArray<CSiblingMerch, CSiblingMerch&>	m_SiblingMerchList;	// 商品列表
};


#endif



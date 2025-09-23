#ifndef _SHARE_STRUCT_H_
#define _SHARE_STRUCT_H_

#include "MerchManager.h"
#include "InfoExport.h"



// 所有业务请求、应答类型
typedef enum E_CommType
{
	//
	ECTReqAuth,					// 请求认证
	ECTRespAuth,				

	//
	ECTReqMarketInfo,			// 市场信息
	ECTRespMarketInfo,

	//
	ECTReqBroker,				// 经济席位信息
	ECTRespBroker,

	//
	ECTReqMerchInfo,			// 商品基本信息
	ECTRespMerchInfo,

	ECTReqPublicFile,			// 服务器文件信息
	ECTRespPublicFile,

	ECTReqLogicBlockFile,		// 服务器的板块文件
	ECTRespLogicBlockFile,

	ECTReqMerchExtendData,		// 商品扩展数据
	ECTRespMerchExtendData,

	ECTReqMerchF10,				// 商品F10信息
	ECTRespMerchF10,

	//
	ECTReqRealtimePrice,		// 实时商品5档行情
	ECTRespRealtimePrice,

	ECTReqRealtimeTick,			// 实时Tick
	ECTRespRealtimeTick,

	ECTReqRealtimeLevel2,		// 实时level2数据
	ECTRespRealtimeLevel2,

	//
	ECTReqMerchKLine,			// 商品历史K线数据
	ECTRespMerchKLine,

	ECTReqMerchTimeSales,		// 商品TimeSales数据
	ECTRespMerchTimeSales,

	// 
	ECTReqReport,				// 报价表
	ECTRespReport,

	//
	ECTReqBlockReport,			// 板块排行
	ECTRespBlockReport,		

	// 
	ECTReqMarketSnapshot,		// 市场快照, 目前有上涨家数，下跌家数信息
	ECTRespMarketSnapshot,	

	//
	ECTReqTradeTime,			// 商品的特殊交易时间
	ECTRespTradeTime,

	// 注册推送
	ECTRegisterPushTick,		// 注册推送Tick
	ECTAnsRegisterPushTick,	

	ECTRegisterPushPrice,		// 注册推送Price
	ECTAnsRegisterPushPrice,	

	ECTRegisterPushLevel2,		// 注册推送Level2
	ECTAnsRegisterPushLevel2,	

	ECTCancelAttendMerch,		// 取消商品实时数据推送
	ECTRespCancelAttendMerch,

	// 
	ECTHeartPing,				// 心跳包
	ECTRespHeart,

	ECTRespError,				// 服务器报错

	// 推送
	ECTPushPrice,				// 价格
	ECTPushTick,				// 分笔
	ECTPushLevel2,				// Level2

	//
	ECTUpdateSymbol,			// 更新商品列表
	ECTKickOut,					// 踢人

	// 
	ECTReqUploadData,			// 上传数据
	ECTRespUploadData,		

	// 
	ECTReqDownloadData,			// 下载数据
	ECTRespDownloadData,			

	//
	ECTReqNetTest,				// 网络测速
	ECTRespNetTest,

	ECTReqPlugIn,				// 插件模块的
	ECTRespPlugIn,				// 插件模块的	

	ECTAddRegisterPushPrice,		// 添加注册推送Price(非同步请求)
	ECTAnsAddRegisterPushPrice,	

	// 新增的服务器动态库类型
	ECTReqGeneralNormal = 2000,			// 请求指数普通数据(不含权涨跌幅, 买盘成交金额和, 买盘成交量和, 卖盘成交金额和, 卖盘成交量和, 瞬间上涨下跌家数)
	ECTRespGeneralNormal,

	ECTReqGeneralFinance,				// 请求指数的财务数据(总股本, 总流通盘, 总市值, 总流通市值, 平均市盈率, 平均市净率, 平均净益率)	
	ECTRespGeneralFinance,

	ECTReqMerchTrendIndex,				// 普通商品的数据(量比, 五档买盘量之和, 五档卖盘量之和)
	ECTRespMerchTrendIndex,

	ECTReqMerchAuction,	   				// 商品集合竞价请求(买价, 买量, 方向)
	ECTRespMerchAuction,

	ECTReqAddPushGeneralNormal, 		// 增加指数数据推送
	ECTRespAddPushGeneralNormal,

	ECTReqUpdatePushGeneralNormal, 		// 同步指数数据推送
	ECTRespUpdatePushGeneralNormal,

	ECTReqDelPushGeneralNormal,    		// 删除指数数据推送
	ECTRespDelPushGeneralNormal,

	ECTReqAddPushMerchTrendIndex,		// 增加商品分时指标数据推送
	ECTRespAddPushMerchTrendIndex,

	ECTReqUpdatePushMerchTrendIndex, 	// 同步商品分时指标数据推送
	ECTRespUpdatePushMerchTrendIndex,

	ECTReqDelPushMerchTrendIndex,    	// 删除商品分时指标数据推送
	ECTRespDelPushMerchTrendIndex,

	ECTReqFundHold,						// 请求基金持仓	
	ECTRespFundHold,					

	ECTReqMinuteBS,						// 请求商品分钟内外盘数据
	ECTRespMinuteBS,					

	ECTReqAddPushMinuteBS,				// 增加商品分钟内外盘数据
	ECTRespAddPushMinuteBS,

	ECTReqUpdatePushMinuteBS,			// 同步商品分钟内外盘数据
	ECTRespUpdatePushMinuteBS,

	ECTReqDelPushMinuteBS,    			// 删除商品分钟内外盘数据
	ECTRespDelPushMinuteBS,

	ECTReqAuthPlugInHeart,				// 认证插件心跳包
	ECTRespAuthPlugInHeart,				


	//
	ECTCount	
}E_CommType;


// 应答经济席位（列表）信息
class DATAINFO_DLL_EXPORT CMmiBroker
{
public:
	CString			m_StrBrokerCode;	// 经济席位代码
	CString			m_StrBrokerCnName;	// 经济席位名称
	CString			m_StrBrokerEnName;	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 业务级的通讯接口
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
// 通讯基类， 描述类型， 对应真实的通讯类
class DATAINFO_DLL_EXPORT CMmiCommBase
{
public:
	enum E_CompareResult
	{
		ECRLess = 0,			// 小于
		ECREqual,				// 等于
		ECRMore,				// 大于
		ECRIntersection			// 有交集
	};

public:
	virtual ~CMmiCommBase() { m_eCommType = ECTCount; }

public:
	virtual CString	GetSummary() = 0;

	// 比较两个通讯包， 看是否有包含、重叠关系
	// 返回：
	// false - 没有可比性(不是同类型同商品请求或两个区段无交集)， iResult无意义
	// true  - 看iResult的值， -1 被包含 0 相同 1 包含 2 有交集
	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public: 
	E_CommType		m_eCommType;
	uint32			m_uiTimeServer;		// 服务器时间， 仅在应答包中有效
}; 

//////////////////////////////////////////////////////////////////////////
// 请求认证信息
class DATAINFO_DLL_EXPORT CMmiReqAuth : public CMmiCommBase
{
public:
	CMmiReqAuth() { m_eCommType = ECTReqAuth; }

public:
	virtual CString GetSummary();

public:
	CString		m_StrReqJson;				// 请求信息封装成json后的数据
};

// 应答认证信息
class DATAINFO_DLL_EXPORT CMmiRespAuth : public CMmiCommBase
{
public:
	CMmiRespAuth() { m_eCommType = ECTRespAuth; }
	virtual ~CMmiRespAuth() {}

public:
	virtual CString GetSummary();

public:
	CString	m_StrRespJson;			// 返回的数据json串
};

///////////////////////////////////////////////////////////////////////////////
// 请求指定市场信息
class DATAINFO_DLL_EXPORT CMarketKey
{
public:
	int32	m_iBigMarketId;
	int32	m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqMarketInfo : public CMmiCommBase
{
public:
	CMmiReqMarketInfo() { m_eCommType = ECTReqMarketInfo; m_iBigMarketId = -1; m_iMarketId = -1; }

public:
	virtual CString	GetSummary();

public:
	CMmiReqMarketInfo(const CMmiReqMarketInfo& Src);
	CMmiReqMarketInfo& CMmiReqMarketInfo::operator=(const CMmiReqMarketInfo& Src);

public:
	int32			m_iBigMarketId;		// 大市场编号
	int32			m_iMarketId;		// 指定市场编号

	CArray<CMarketKey, CMarketKey&>	m_aMarketMore;	// 仅请求一个市场时， 无需理会该变量(为请求一个市场时书写简单)
};

// 应答指定市场信息
class DATAINFO_DLL_EXPORT CMmiRespMarketInfo : public CMmiCommBase
{
public:
	CMmiRespMarketInfo() { m_eCommType = ECTRespMarketInfo; }
	virtual ~CMmiRespMarketInfo();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMarketInfo& CMmiRespMarketInfo::operator=(const CMmiRespMarketInfo& Src);

public:
	CArray<CMarketInfo*, CMarketInfo*> m_MarketInfoListPtr;
};

///////////////////////////////////////////////////////////////////////////////
// 请求市场快照信息
class DATAINFO_DLL_EXPORT CMmiReqMarketSnapshot : public CMmiCommBase
{
public:
	CMmiReqMarketSnapshot() { m_eCommType = ECTReqMarketSnapshot; }

public:
	virtual CString	GetSummary();

//public:
//	const CMmiReqMarketSnapshot& CMmiReqMarketSnapshot::operator=(const CMmiReqMarketSnapshot& Src);

public:
	int32			m_iMarketId;		// 指定市场编号
};

// 应答市场快照信息
class DATAINFO_DLL_EXPORT CMmiRespMarketSnapshot : public CMmiCommBase
{
public:
	CMmiRespMarketSnapshot() { m_eCommType = ECTRespMarketSnapshot; }
	virtual ~CMmiRespMarketSnapshot();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMarketSnapshot& CMmiRespMarketSnapshot::operator=(const CMmiRespMarketSnapshot& Src);

public:
	CArray<CMarketSnapshotInfo*, CMarketSnapshotInfo*> m_MarketSnapshotListPtr;
};


///////////////////////////////////////////////////////////////////////////////
// 请求经济席位（列表）信息
class DATAINFO_DLL_EXPORT CMmiReqBroker  : public CMmiCommBase
{
public:
	CMmiReqBroker() { m_eCommType = ECTReqBroker; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqBroker& CMmiReqBroker::operator=(const CMmiReqBroker& Src);

public:
	int32			m_iBreedId;		// 大市场编号
};


class DATAINFO_DLL_EXPORT CMmiRespBroker : public CMmiCommBase
{
public:
	CMmiRespBroker() { m_eCommType = ECTRespBroker; }
	virtual ~CMmiRespBroker();
 
public:
	virtual CString	GetSummary();


private:
	const CMmiRespBroker& CMmiRespBroker::operator=(const CMmiRespBroker& Src);

public:
	int32			m_iBreedId;
	CArray<CMmiBroker*, CMmiBroker*> m_CommBrokerListPtr;
};


///////////////////////////////////////////////////////////////////////////////
// 请求商品（列表）信息
class DATAINFO_DLL_EXPORT CMmiReqMerchInfo : public CMmiCommBase
{
public:
	CMmiReqMerchInfo() { m_eCommType = ECTReqMerchInfo; m_iMarketId = -1; }

public:
	virtual CString	GetSummary();

public:
	CMmiReqMerchInfo::CMmiReqMerchInfo(const CMmiReqMerchInfo& Src);
 	CMmiReqMerchInfo& CMmiReqMerchInfo::operator=(const CMmiReqMerchInfo& Src);

public:
	int32			m_iMarketId;		// 市场编号
//	CString			m_StrMerchCode;		// 商品代码，空字符串表示请求该市场下所有的商品列表

	CArray<int32, int32> m_aMarketMore;	// 多个市场的同时请求
};

// 应答商品（列表）信息
class DATAINFO_DLL_EXPORT CMmiRespMerchInfo : public CMmiCommBase
{
public:
	CMmiRespMerchInfo() { m_eCommType = ECTRespMerchInfo; }
	virtual ~CMmiRespMerchInfo();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMerchInfo& CMmiRespMerchInfo::operator=(const CMmiRespMerchInfo& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CArray<CMerchInfo*, CMerchInfo*> m_MerchInfoListPtr;
};

///////////////////////////////////////////////////////////////////////////////
// 请求服务器文件信息
class DATAINFO_DLL_EXPORT CMmiReqPublicFile : public CMmiCommBase
{
public:
	CMmiReqPublicFile() { m_eCommType = ECTReqPublicFile; }
	
public:
	virtual CString	GetSummary();
	
	// public:
	// 	const CMmiReqPublicFile& CMmiReqPublicFile::operator=(const CMmiReqPublicFile& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码，空字符串表示请求该市场下所有的商品列表
	
	E_PublicFileType	m_ePublicFileType;
	uint32			m_uiCRC32;
};

// 应答服务器文件信息
class DATAINFO_DLL_EXPORT CMmiRespPublicFile : public CMmiCommBase
{
public:

public:
	CMmiRespPublicFile() : m_pcBuffer(NULL) { m_eCommType = ECTRespPublicFile; m_iValidDataLen = 0; }
	virtual ~CMmiRespPublicFile();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespPublicFile& CMmiRespPublicFile::operator=(const CMmiRespPublicFile& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;
	
	E_PublicFileType	m_ePublicFileType;
	uint32			m_uiCRC32;			// 数据CRC校验值	0 - 表示服务器无数据 
	
	char			*m_pcBuffer;
	int32			m_iValidDataLen;	// 数据长度， 不包含此结构		 仅当crc != 0时， 该值才有意义：0 - 表示本地是最新的数据 1 - 表示需要更新本地数据
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品扩展数据
class DATAINFO_DLL_EXPORT CMmiReqMerchExtendData : public CMmiCommBase
{
public:
	CMmiReqMerchExtendData() { m_eCommType = ECTReqMerchExtendData; }

public:
	virtual CString	GetSummary() { return L""; }

// public:
// 	const CMmiReqMerchExtendData& CMmiReqMerchExtendData::operator=(const CMmiReqMerchExtendData& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
};


// 应答商品扩展数据
class DATAINFO_DLL_EXPORT CMmiRespMerchExtendData : public CMmiCommBase
{
public:
	CMmiRespMerchExtendData() { m_eCommType = ECTRespMerchExtendData; }
	virtual ~CMmiRespMerchExtendData();

public:
	virtual CString	GetSummary() { return L""; }

private:
	const CMmiRespMerchExtendData& CMmiRespMerchExtendData::operator=(const CMmiRespMerchExtendData& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	CMerchExtendData m_MerchExtendData;	// 商品扩展数据
};


///////////////////////////////////////////////////////////////////////////////
// 请求商品F10信息
class DATAINFO_DLL_EXPORT CMmiReqMerchF10 : public CMmiCommBase
{
public:
	CMmiReqMerchF10() { m_eCommType = ECTReqMerchF10; }

public:
	virtual CString	GetSummary() { return L""; }

// public:
// 	const CMmiReqMerchF10& CMmiReqMerchF10::operator=(const CMmiReqMerchF10& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
};

// 应答商品F10信息
class DATAINFO_DLL_EXPORT CMmiRespMerchF10 : public CMmiCommBase
{
public:
	CMmiRespMerchF10() { m_eCommType = ECTRespMerchF10; }
	virtual ~CMmiRespMerchF10();
	
public:
	virtual CString	GetSummary() { return L""; }

private:
	const CMmiRespMerchF10& CMmiRespMerchF10::operator=(const CMmiRespMerchF10& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	CMerchF10		m_MerchF10;			// 商品F10信息
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品实时5档行情数据
class DATAINFO_DLL_EXPORT CMerchKey
{
public:
	CMerchKey() : m_iMarketId(0) {}
	CMerchKey(int32 iMarketId, LPCTSTR pwszMerchCode, uint32 uiSelTime = 0, float fSelPrice = 0.0 ) : m_iMarketId(iMarketId), 
		m_StrMerchCode(pwszMerchCode), m_uSelectTime(uiSelTime), m_fSelPrice(fSelPrice) {}
	
public:
	bool32 operator< (const CMerchKey& MerchKey) const
	{
		if ( m_iMarketId < MerchKey.m_iMarketId )
		{
			return true;
		}

		if ( m_iMarketId == MerchKey.m_iMarketId )
		{
			if ( m_StrMerchCode.CompareNoCase(MerchKey.m_StrMerchCode) < 0 )
			{
				return true;
			}
		}

		return false;
	}

	bool32	IsSameMerch(int32 iMarketId, const LPCTSTR pwszMerchCode)
	{
		if (m_iMarketId == iMarketId && m_StrMerchCode.CompareNoCase(pwszMerchCode) == 0)
			return true;

		return false;
	}

	bool32	IsSameMerch(const CMerchKey &MerchKey)
	{
		return IsSameMerch(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode); 
	}

public:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	u32             m_uSelectTime;
	float           m_fSelPrice;
};

class DATAINFO_DLL_EXPORT CPeriodMerchKey
{
public:
	CPeriodMerchKey() : m_iMarketId(0),m_StrMerchCode(L""), m_uiPeriod(0) {}
	CPeriodMerchKey(int32 iMarketId, LPCTSTR pwszMerchCode, uint8 uiPeriod) : m_iMarketId(iMarketId), m_StrMerchCode(pwszMerchCode), m_uiPeriod(uiPeriod) {}

public:
	bool32 operator< (const CPeriodMerchKey& MerchKey) const
	{
		if ( m_iMarketId < MerchKey.m_iMarketId )
		{
			return true;
		}

		if ( m_iMarketId == MerchKey.m_iMarketId )
		{
			if ( m_StrMerchCode.CompareNoCase(MerchKey.m_StrMerchCode) < 0 )
			{
				return true;
			}
		}

		return false;
	}

	bool32	IsSameMerch(int32 iMarketId, const LPCTSTR pwszMerchCode)
	{
		if (m_iMarketId == iMarketId && m_StrMerchCode.CompareNoCase(pwszMerchCode) == 0)
			return true;

		return false;
	}

	bool32	IsSameMerch(const CPeriodMerchKey &MerchKey)
	{
		return IsSameMerch(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode); 
	}

public:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	int8            m_uiPeriod;
};

class DATAINFO_DLL_EXPORT CMmiReqRealtimePrice : public CMmiCommBase
{
public:
	CMmiReqRealtimePrice() { m_eCommType = ECTReqRealtimePrice; }

public:
	virtual CString	GetSummary();

public:
 	const CMmiReqRealtimePrice& operator=(const CMmiReqRealtimePrice& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时5档行情数据
class DATAINFO_DLL_EXPORT CMmiRespRealtimePrice : public CMmiCommBase
{
public:
	CMmiRespRealtimePrice() { m_eCommType = ECTRespRealtimePrice; }
	virtual ~CMmiRespRealtimePrice();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespRealtimePrice& CMmiRespRealtimePrice::operator=(const CMmiRespRealtimePrice& Src);

public:
	CArray<CRealtimePrice*, CRealtimePrice*> m_RealtimePriceListPtr;	// 商品5档行情数据
};

///////////////////////////////////////////////////////////////////
// 请求商品实时Tick数据
class  DATAINFO_DLL_EXPORT CMmiReqRealtimeTick : public CMmiCommBase
{
public:
	CMmiReqRealtimeTick() { m_eCommType = ECTReqRealtimeTick; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiReqRealtimeTick& CMmiReqRealtimeTick::operator=(const CMmiReqRealtimeTick& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时Tick行情数据
class  DATAINFO_DLL_EXPORT CMmiRespRealtimeTick : public CMmiCommBase
{
public:
	CMmiRespRealtimeTick() { m_eCommType = ECTRespRealtimeTick; }
	virtual ~CMmiRespRealtimeTick();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespRealtimeTick& CMmiRespRealtimeTick::operator=(const CMmiRespRealtimeTick& Src);
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品实时level2数据
class  DATAINFO_DLL_EXPORT CMmiReqRealtimeLevel2 : public CMmiCommBase
{
public:
	CMmiReqRealtimeLevel2() { m_eCommType = ECTReqRealtimeLevel2; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqRealtimeLevel2& CMmiReqRealtimeLevel2::operator=(const CMmiReqRealtimeLevel2& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
};

// 应答商品实时level2数据
class DATAINFO_DLL_EXPORT  CMmiRespRealtimeLevel2 : public CMmiCommBase
{
public:
	CMmiRespRealtimeLevel2() { m_eCommType = ECTRespRealtimeLevel2; }
	virtual ~CMmiRespRealtimeLevel2();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespRealtimeLevel2& CMmiRespRealtimeLevel2::operator=(const CMmiRespRealtimeLevel2& Src);

public:
	CRealtimeLevel2	m_RealtimeLevel2;		// 商品level2数据
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品历史K线数据
class DATAINFO_DLL_EXPORT  CMmiReqMerchKLine : public CMmiCommBase
{
public:
	CMmiReqMerchKLine();

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqMerchKLine& CMmiReqMerchKLine::operator=(const CMmiReqMerchKLine& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);	
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	E_KLineTypeBase m_eKLineTypeBase;	// 请求的K线类型
	E_ReqTimeType	m_eReqTimeType;		// 请求时间类型
	union								// 请求段
	{
		struct	// 指定时间
		{
			CGmtTime	m_TimeStart;	
			CGmtTime	m_TimeEnd;
		};
		struct	// 倒数个数
		{
			CGmtTime	m_TimeSpecify;	// 倒数偏移
			int32		m_iFrontCount;	// 向前个数（向时间更早的方向）
		};
		struct	// 正数个数
		{
			CGmtTime	m_TimeSpecify;	// 正数偏移
			int32		m_iCount;		// 向后个数（向时间更新的方向）
		};
	};
};

// 应答商品历史K线数据
class DATAINFO_DLL_EXPORT  CMmiRespMerchKLine : public CMmiCommBase
{
public:
	CMmiRespMerchKLine() { m_eCommType = ECTRespMerchKLine; }
	virtual ~CMmiRespMerchKLine();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMerchKLine& CMmiRespMerchKLine::operator=(const CMmiRespMerchKLine& Src);

public:
	CMerchKLineNode	m_MerchKLineNode;	// 商品历史K线数据
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品TimeSales数据（每次请求总是固定返回某一个时间范围内的TimeSales内的数据）
class DATAINFO_DLL_EXPORT  CMmiReqMerchTimeSales : public CMmiCommBase
{
public:
	CMmiReqMerchTimeSales();

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqMerchTimeSales& CMmiReqMerchTimeSales::operator=(const CMmiReqMerchTimeSales& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码

	// 以下字段在请求实时数据时无意义
	E_ReqTimeType	m_eReqTimeType;		// 请求时间类型
	union								// 请求段
	{
		struct	// 指定时间
		{
			CGmtTime		m_TimeStart;
			CGmtTime		m_TimeEnd;
		};
		struct	// 倒数个数
		{
			CGmtTime		m_TimeSpecify;	// 指定时间
			int32			m_iFrontCount;	// 向前个数　
		};
		struct	// 正数个数
		{
			CGmtTime		m_TimeSpecify;	// 指定时间
			int32			m_iCount;		// 向后个数
		};
	};
};

// 应答商品历史TimeSales线数据
class DATAINFO_DLL_EXPORT  CMmiRespMerchTimeSales : public CMmiCommBase	// 
{
public:
	CMmiRespMerchTimeSales() { m_eCommType = ECTRespMerchTimeSales; }
	virtual ~CMmiRespMerchTimeSales();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespMerchTimeSales& CMmiRespMerchTimeSales::operator=(const CMmiRespMerchTimeSales& Src);
	
public:
	CMerchTimeSales	m_MerchTimeSales;	// 商品Tick线数据
};

///////////////////////////////////////////////////////////////////////////////
// 请求商品排行榜数据
class DATAINFO_DLL_EXPORT CMmiReqReport : public CMmiCommBase
{
public:
	CMmiReqReport() { m_eCommType = ECTReqReport; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqReport& CMmiReqReport::operator=(const CMmiReqReport& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public:
	int32			m_iMarketId;		// 市场编号
	E_MerchReportField	m_eMerchReportField;	// 商品报价表字段
	bool32			m_bDescSort;		// 降序？
	int32			m_iStart;
	int32			m_iCount;
};

// 应答商品排行榜数据
class DATAINFO_DLL_EXPORT CMmiRespReport : public CMmiCommBase
{
public:
	CMmiRespReport() { m_eCommType = ECTRespReport; }
	virtual ~CMmiRespReport();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespReport& CMmiRespReport::operator=(const CMmiRespReport& Src);

public:
	int32			m_iMarketId;		// 市场编号
	
	E_MerchReportField	m_eMerchReportField;
	bool32			m_bDescSort;		// 降序排行？
	int32			m_iStart;

	// 商品列表
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// 商品5档行情数据
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

// 请求商品实时Tick数据
class DATAINFO_DLL_EXPORT CMmiRegisterPushTick : public CMmiCommBase
{
public:
	CMmiRegisterPushTick() { m_eCommType = ECTRegisterPushTick; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushTick& CMmiRegisterPushTick::operator=(const CMmiRegisterPushTick& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时Tick行情数据
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushTick : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushTick() { m_eCommType = ECTAnsRegisterPushTick; }
	virtual ~CMmiAnsRegisterPushTick();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushTick& CMmiAnsRegisterPushTick::operator=(const CMmiAnsRegisterPushTick& Src);
};

// 请求商品实时Price数据
class DATAINFO_DLL_EXPORT CMmiRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiRegisterPushPrice() { m_eCommType = ECTRegisterPushPrice; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushPrice& CMmiRegisterPushPrice::operator=(const CMmiRegisterPushPrice& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时Price行情数据
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushPrice() { m_eCommType = ECTAnsRegisterPushPrice; }
	virtual ~CMmiAnsRegisterPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushPrice& CMmiAnsRegisterPushPrice::operator=(const CMmiAnsRegisterPushPrice& Src);
};

// 请求商品实时Levle2数据
class DATAINFO_DLL_EXPORT CMmiRegisterPushLevel2 : public CMmiCommBase
{
public:
	CMmiRegisterPushLevel2() { m_eCommType = ECTRegisterPushLevel2; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushLevel2& CMmiRegisterPushLevel2::operator=(const CMmiRegisterPushLevel2& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时Level2行情数据
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushLevel2 : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushLevel2() { m_eCommType = ECTAnsRegisterPushLevel2; }
	virtual ~CMmiAnsRegisterPushLevel2();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushLevel2& CMmiAnsRegisterPushLevel2::operator=(const CMmiAnsRegisterPushLevel2& Src);
};

// 向服务器注销某只商品的实时数据推送
class DATAINFO_DLL_EXPORT CPushMerchKey
{
public:
	int32			m_iMarketId;			// 市场编号
	CString			m_StrMerchCode;			// 商品代码
	
	int32			m_iRealtimePushTypes;	// E_RealtimePushType或值
};

class DATAINFO_DLL_EXPORT CMmiCancelAttendMerch : public CMmiCommBase
{
public:
	CMmiCancelAttendMerch() { m_eCommType = ECTCancelAttendMerch; }

public:
	virtual CString	GetSummary();

// public:
 	const CMmiCancelAttendMerch& CMmiCancelAttendMerch::operator=(const CMmiCancelAttendMerch& Src);

public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	int32			m_iRealtimePushTypes;		// E_RealtimePushType或值
	
	CArray<CPushMerchKey, CPushMerchKey&>	m_aPushMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答注销某只商品的实时数据推送
class DATAINFO_DLL_EXPORT CMmiRespCancelAttendMerch : public CMmiCommBase
{
public:
	CMmiRespCancelAttendMerch() { m_eCommType = ECTRespCancelAttendMerch; }
	virtual ~CMmiRespCancelAttendMerch();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespCancelAttendMerch& CMmiRespCancelAttendMerch::operator=(const CMmiRespCancelAttendMerch& Src);
};

///////////////////////////////////////////////////////////////////////////////
// 发送心跳包
class DATAINFO_DLL_EXPORT CMmiHeartPing : public CMmiCommBase
{
public:
	CMmiHeartPing() { m_eCommType = ECTHeartPing; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiHeartPing& CMmiHeartPing::operator=(const CMmiHeartPing& Src);

	CString			m_StrUserName;
	CString			m_StrPassword;
};

// 应答心跳包
class DATAINFO_DLL_EXPORT CMmiRespHeart : public CMmiCommBase
{
public:
	CMmiRespHeart() { m_eCommType = ECTRespHeart; }
	virtual ~CMmiRespHeart();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespHeart& CMmiRespHeart::operator=(const CMmiRespHeart& Src);
};

//////////////////////////////////////////////////////////////////////////
// 发送测速包
class DATAINFO_DLL_EXPORT CMmiReqNetTest : public CMmiCommBase
{
public:
	// 默认构造函数
	CMmiReqNetTest();	
	
	// 拷贝构造函数
	CMmiReqNetTest(const CMmiReqNetTest& MmiReqNetTest);

	// 赋值函数
	CMmiReqNetTest& CMmiReqNetTest::operator = (const CMmiReqNetTest& MmiReqNetTest);

	// 析构函数
	virtual ~CMmiReqNetTest();

public:
	virtual CString GetSummary();

	uint32			m_uiLen;
	char*			m_pData;
};

class DATAINFO_DLL_EXPORT CMmiRespNetTest : public CMmiCommBase
{
public:
	CMmiRespNetTest() { m_eCommType = ECTRespNetTest; m_uiTimeResp = -1; }
	virtual ~CMmiRespNetTest();
public:
	virtual CString GetSummary();
		
	uint32			m_uiTimeResp;
	uint32			m_uiLen;
	char*			m_pData;
};

////////////////////////////////////////////////////////////////////////////////
// 服务器报错
class DATAINFO_DLL_EXPORT CMmiRespError : public CMmiCommBase
{
public:
	CMmiRespError() { m_eCommType = ECTRespError; m_eReqCommType = ECTCount; m_eRespErrorCode = ERENone; m_iCommunicationId = -1;}
	virtual ~CMmiRespError();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespError& CMmiRespError::operator=(const CMmiRespError& Src);

public:
	CString			m_StrMerchCode;
	int32			m_iMarketId;

	E_CommType		m_eReqCommType;
	int32			m_iCommunicationId;

	CString			m_StrErrorMsg;
	E_RespErrorCode	m_eRespErrorCode;
};

////////////////////////////////////////////////////////////////////////////////
// 推送Price
class DATAINFO_DLL_EXPORT CMmiPushPrice : public CMmiCommBase
{
public:
	CMmiPushPrice() { m_eCommType = ECTPushPrice; }
	virtual ~CMmiPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushPrice& CMmiPushPrice::operator=(const CMmiPushPrice& Src);
	
public:
	CArray<CRealtimePrice*, CRealtimePrice*> m_RealtimePriceListPtr;	// 商品5档行情数据
};

// 推送Tick
class DATAINFO_DLL_EXPORT CMmiPushTick : public CMmiCommBase
{
public:
	CMmiPushTick() { m_eCommType = ECTPushTick; }
	virtual ~CMmiPushTick() {}
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushTick& CMmiPushTick::operator=(const CMmiPushTick& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
		
	CArray<CTick, CTick> m_Ticks;		// 
};

// 推送Level2
class DATAINFO_DLL_EXPORT CMmiPushLevel2 : public CMmiCommBase
{
public:
	CMmiPushLevel2() { m_eCommType = ECTPushLevel2; }
	virtual ~CMmiPushLevel2() {}
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushLevel2& CMmiPushLevel2::operator=(const CMmiPushLevel2& Src);
	
public:
	CRealtimeLevel2	m_RealtimeLevel2;		// 商品level2数据
};

class DATAINFO_DLL_EXPORT CMmiKickOut : public CMmiCommBase
{
public:
	CMmiKickOut() { m_eCommType = ECTKickOut; }

public:
	virtual CString GetSummary();

public:
	CString		m_strRespJson;		// 返回的json串
};

class DATAINFO_DLL_EXPORT CMmiReqUploadData : public CMmiCommBase
{
public:
	CMmiReqUploadData(); 
	virtual ~CMmiReqUploadData();

	// 拷贝构造
	CMmiReqUploadData(const CMmiReqUploadData& MmiReqUploadData);

	// 赋值运算符
	CMmiReqUploadData& CMmiReqUploadData::operator = (const CMmiReqUploadData& MmiReqUploadData);

public:
	virtual CString GetSummary();

public:
	uint32		m_uType;
	uint32		m_uLen;
	char*		m_pData;		
};

class DATAINFO_DLL_EXPORT CMmiRespUploadData : public CMmiCommBase
{
public:
	CMmiRespUploadData() { m_eCommType = ECTRespUploadData; }

public:
	virtual CString GetSummary();

public:
	uint32		m_uReserve;
};

class DATAINFO_DLL_EXPORT CMmiReqDownloadData : public CMmiCommBase
{
public:
	CMmiReqDownloadData() { m_eCommType = ECTReqDownloadData; }

public:
	virtual CString GetSummary();

public:
	uint32		m_uDataType;
	uint32		m_uReserve[4];		// 保留
};

class DATAINFO_DLL_EXPORT CMmiRespDownloadData : public CMmiCommBase
{
public:
	CMmiRespDownloadData() ; 
	virtual ~CMmiRespDownloadData();

	// 拷贝构造
	CMmiRespDownloadData(const CMmiRespDownloadData& MmiReqUploadData);

	// 赋值运算符
	CMmiRespDownloadData& CMmiRespDownloadData::operator = (const CMmiRespDownloadData& MmiReqUploadData);

public:
	virtual CString GetSummary();

public:
	uint32		m_uType;
	uint32		m_uLen;
	char*		m_pData;		
};

// 请求商品实时Price数据
class DATAINFO_DLL_EXPORT CMmiAddRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAddRegisterPushPrice() { m_eCommType = ECTAddRegisterPushPrice; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiAddRegisterPushPrice& operator=(const CMmiAddRegisterPushPrice& Src);
	
public:
	int32			m_iMarketId;		// 市场编号
	CString			m_StrMerchCode;		// 商品代码
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// 仅请求一个商品时， 无需理会该变量(为请求一个商品时书写简单)
};

// 应答商品实时Price行情数据
class DATAINFO_DLL_EXPORT CMmiAnsAddRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAnsAddRegisterPushPrice() { m_eCommType = ECTAnsAddRegisterPushPrice; }
	virtual ~CMmiAnsAddRegisterPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsAddRegisterPushPrice& operator=(const CMmiAnsAddRegisterPushPrice& Src);
};

////////////////////////////////////////////////////////////////////////////////
// 常用接口
DATAINFO_DLL_EXPORT CMmiCommBase*		NewCopyCommReqObject(CMmiCommBase *pCommBase);
DATAINFO_DLL_EXPORT bool32				IsSameReqObject(const CMmiCommBase *pCommSrc, const CMmiCommBase *pCommDst);		// 判读两个请求是否同类型同对象



/// 
typedef enum E_FormulaUpdateType
{
	// 系统指标
	EFUTParamTempUpdate = 0,// 临时参数修改,仅name影响当前指标线
	EFUTParamUpdate,		// 系统参数修改,影响所有name指标线
	EFUTIndexUpdate,		// name指标修改
	EFUTIndexDelete,		// name指标删除

	// 五彩 K 线
	EFUTClrKLineUpdate,		// 更新
	EFUTClrKLineDelete,		// 删除

	// 条件选股
	EFUTCdtChoseUpdate,		// 更新
	EFUTCdtChoseDelete,		// 删除

	// 交易指标
	EFUTTradeChoseUpdate,	// 更新
	EFUTTradeChoseDelete,    // 删除

} E_FormulaUpdateType;


//////////////////////////////////////////////////////////////////////////
// fangz 0811 测试新加
//////////////////////////////////////////////////////////////////////////

// 1: 行情服务器更新的

// 财务数据结构 (类似除权信息, 这个结构在解析服务器传过来的数据时用到)
typedef struct T_FinanceData
{
	float   m_fAllCapical;		// 总股本
	float	m_fCircAsset;		// 流通股本
	float	m_fAllAsset;		// 总资产
	float	m_fFlowDebt;		// 流动负债
	float	m_fPerFund;			// 每股公积金
	
	float	m_fBusinessProfit;	// 营业利益
	float	m_fPerNoDistribute;	// 每股未分配
	float	m_fPerIncomeYear;	// 每股收益(年)
	float	m_fPerPureAsset;	// 每股净资产
	float	m_fChPerPureAsset;	// 调整每股净资产
	
	float	m_fDorRightRate;		// 股东权益比
	float	m_fCircMarketValue;		// 流通市值
	float	m_fAllMarketValue;		// 总市值

}T_FinanceData;

// 请求逻辑板块文件
class DATAINFO_DLL_EXPORT CMmiReqLogicBlockFile : public CMmiCommBase
{
public:
	CMmiReqLogicBlockFile::CMmiReqLogicBlockFile() { m_eCommType = ECTReqLogicBlockFile; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqLogicBlockFile(const CMmiReqLogicBlockFile& Src);
	CMmiReqLogicBlockFile& operator=(const CMmiReqLogicBlockFile& Src);

public:
	unsigned long	m_uCRC32;		// 客户端已有数据的CRC32值
};

// 应答
class DATAINFO_DLL_EXPORT CMmiRespLogicBlockFile : public CMmiCommBase
{
public:
	CMmiRespLogicBlockFile::CMmiRespLogicBlockFile() { m_eCommType = ECTRespLogicBlockFile; m_uiDataLen = 0; m_pData = NULL; }
	~CMmiRespLogicBlockFile();

	virtual CString	GetSummary(){return CString();}

public:
	char*		m_pData;
	uint32		m_uiDataLen;  // 如果客户和服务端的CRC值一样，则szData为0,0，并且只有两个字节, 否则
							  // szData为uDataLen,  szData结尾会有两个两个'\0', 
							  // 注意: uDataLen也包括两个结尾字符	
};

// 请求板块排行
// class CMmiReqBlockReport : public CMmiCommBase
// {
// public:
// 	CMmiReqBlockReport() { m_eCommType = ECTReqReport; }
// 	
// public:
// 	virtual CString	GetSummary()  {return L""; }
// 	
// 	CMmiReqBlockReport(const CMmiReqBlockReport& Src);
// 	const CMmiReqBlockReport& operator=(const CMmiReqBlockReport& Src);
// 		
// public:
// 	int32			m_iBlockId;					// 板块编号
// 	E_MerchReportField	m_eMerchReportField;	// 商品报价表字段
// 	bool32			m_bDescSort;				// 降序？
// 	int32			m_iStart;
// 	int32			m_iCount;
// };

class DATAINFO_DLL_EXPORT CMmiReqBlockReport : public CMmiCommBase
{
public:
	CMmiReqBlockReport() { m_eCommType = ECTReqBlockReport; }
	virtual ~CMmiReqBlockReport(){}

public:
	CMmiReqBlockReport(const CMmiReqBlockReport& Src);
	const CMmiReqBlockReport& operator=(const CMmiReqBlockReport& Src);
	
	virtual CString GetSummary() { return L"123"; }

public:
	int32			m_iBlockId;					// 板块编号
	E_MerchReportField	m_eMerchReportField;	// 商品报价表字段
	bool32			m_bDescSort;				// 降序？
	int32			m_iStart;
	int32			m_iCount;
};

// 应答商品排行榜数据
class DATAINFO_DLL_EXPORT CMmiRespBlockReport : public CMmiCommBase
{
public:
	CMmiRespBlockReport() { m_eCommType = ECTRespBlockReport; }
	virtual ~CMmiRespBlockReport(){}
	
public:
	virtual CString	GetSummary() {return L""; }
	
private:
	const CMmiRespBlockReport& CMmiRespBlockReport::operator=(const CMmiRespBlockReport& Src);
	
public:
	int32			m_iBlockId;		// 市场编号
	
	E_MerchReportField	m_eMerchReportField;
	bool32			m_bDescSort;		// 降序排行？
	int32			m_iStart;
	
	// 商品列表
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// 商品5档行情数据
};

//////////////////////////////////////////////////////////////////////////
// 0225 新加的协议:

// 请求指数普通数据
class DATAINFO_DLL_EXPORT CMmiReqGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqGeneralNormal() { m_eCommType = ECTReqGeneralNormal; }
	virtual ~CMmiReqGeneralNormal(){}

public:
	virtual CString GetSummary();
	CMmiReqGeneralNormal(const CMmiReqGeneralNormal& stSrc);
	CMmiReqGeneralNormal& CMmiReqGeneralNormal::operator=(const CMmiReqGeneralNormal& stSrc);

public:
	int32	m_iMarketId;
};

// 具体数据格式
typedef struct T_GeneralNormal
{
public:
	T_GeneralNormal()
	{
		Clear();	
	}

	void Clear()
	{
		memset(this, 0, sizeof(T_GeneralNormal));
	}

	long	m_lTime;					// 时间
	float	m_fRiseRateNoWeighted;		// 不含权涨跌幅
	float	m_fAmountBuy;				// 买盘成交额总和
	float	m_fVolumeBuy;				// 买盘成交量总和
	float	m_fAmountSell;				// 卖盘成交额总和
	float	m_fVolumeSell;				// 卖盘成交量总和
	uint32	m_uiRiseMomentary;			// 瞬时上涨家数
	uint32	m_uiFallMomentary;			// 瞬时下跌家数	
	uint32	m_uiRiseCounts;				// 上涨家数
	uint32	m_uiFallCounts;				// 下跌家数

	float	m_fBuy1Amount;				// 买1 档金额总和
	float	m_fBuy5Amount;				// 买5 档金额总和
	float	m_fSell1Amount;				// 卖1 档金额总和
	float	m_fSell5Amount;				// 卖5 档金额总和

}T_GeneralNormal;

class DATAINFO_DLL_EXPORT CMmiRespGeneralNormal : public CMmiCommBase
{
public:
	CMmiRespGeneralNormal() { m_eCommType = ECTRespGeneralNormal; }
	virtual ~CMmiRespGeneralNormal(){}
	
public:
	virtual CString GetSummary();
	CMmiRespGeneralNormal(const CMmiRespGeneralNormal& stSrc);
	CMmiRespGeneralNormal& CMmiRespGeneralNormal::operator=(const CMmiRespGeneralNormal& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CArray<T_GeneralNormal, T_GeneralNormal&> m_aGeneralNormal;
};

//////////////////////////////////////////////////////////////////////////
// 请求指数财务数据(相关数值视图计算用到)
class DATAINFO_DLL_EXPORT CMmiReqGeneralFinance : public CMmiCommBase
{
public:
	CMmiReqGeneralFinance() { m_eCommType = ECTReqGeneralFinance; }
	virtual ~CMmiReqGeneralFinance(){}
	
public:
	virtual CString GetSummary();
	CMmiReqGeneralFinance(const CMmiReqGeneralFinance& stSrc);
	CMmiReqGeneralFinance& CMmiReqGeneralFinance::operator=(const CMmiReqGeneralFinance& stSrc);
	
public:
	int32	m_iMarketId;
};

//

// 具体数据格式
typedef struct T_GeneralFinance
{
public:
	T_GeneralFinance()
	{
		memset(this, 0, sizeof(T_GeneralFinance));
	}

	// 固定
	float	m_fAllStock;			// 总股本
	float	m_fCircStock;			// 总流通盘
	float	m_fAvgPureRate;			// 平均市净率
	float	m_fAvgPureGainRate;		// 平均净益率

	// 会变动(和实时价格有关)
	float	m_fAllValue;			// 总市值	
	float	m_fCircValue;			// 总流通市值
	float	m_fAvgPe;				// A股平均市盈率
	
}T_GeneralFinance;

class DATAINFO_DLL_EXPORT CMmiRespGeneralFinance : public CMmiCommBase
{
public:
	CMmiRespGeneralFinance() { m_eCommType = ECTRespGeneralFinance; }
	virtual ~CMmiRespGeneralFinance(){}
	
public:
	virtual CString GetSummary();
	CMmiRespGeneralFinance(const CMmiRespGeneralFinance& stSrc);
	CMmiRespGeneralFinance& CMmiRespGeneralFinance::operator=(const CMmiRespGeneralFinance& stSrc);
	
public:
	//
	int32	m_iMarketId;
	T_GeneralFinance m_stGeneralFinance;
};

//////////////////////////////////////////////////////////////////////////
// 请求普通商品的分时指标数据
class DATAINFO_DLL_EXPORT CMmiReqMerchTrendIndex : public CMmiCommBase
{
public:	
	CMmiReqMerchTrendIndex() { m_eCommType = ECTReqMerchTrendIndex; }
	virtual ~CMmiReqMerchTrendIndex(){}
	
public:
	virtual CString GetSummary();
	CMmiReqMerchTrendIndex(const CMmiReqMerchTrendIndex& stSrc);
	CMmiReqMerchTrendIndex& CMmiReqMerchTrendIndex::operator=(const CMmiReqMerchTrendIndex& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiRespMerchTrendIndex() { m_eCommType = ECTRespMerchTrendIndex; }
	virtual ~CMmiRespMerchTrendIndex(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMerchTrendIndex(const CMmiRespMerchTrendIndex& stSrc);
	CMmiRespMerchTrendIndex& CMmiRespMerchTrendIndex::operator=(const CMmiRespMerchTrendIndex& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MerchTrendIndex, T_MerchTrendIndex&> m_aMerchTrendIndex;
};

//////////////////////////////////////////////////////////////////////////
// 集合竟价数据
class DATAINFO_DLL_EXPORT CMmiReqMerchAution : public CMmiCommBase
{
public:	
	CMmiReqMerchAution() { m_eCommType = ECTReqMerchAuction; }
	virtual ~CMmiReqMerchAution(){}
	
public:
	virtual CString GetSummary();
	CMmiReqMerchAution(const CMmiReqMerchAution& stSrc);
	CMmiReqMerchAution& CMmiReqMerchAution::operator=(const CMmiReqMerchAution& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMerchAuction: public CMmiCommBase
{
public:
	CMmiRespMerchAuction() { m_eCommType = ECTRespMerchAuction; }
	virtual ~CMmiRespMerchAuction(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMerchAuction(const CMmiRespMerchAuction& stSrc);
	CMmiRespMerchAuction& CMmiRespMerchAuction::operator=(const CMmiRespMerchAuction& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MerchAuction, T_MerchAuction&> m_aMerchAuction;
}; 
//////////////////////////////////////////////////////////////////////////
// 指数推送
class DATAINFO_DLL_EXPORT CMmiReqAddPushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqAddPushGeneralNormal() { m_eCommType = ECTReqAddPushGeneralNormal; }
	virtual ~CMmiReqAddPushGeneralNormal(){}

public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求增加推送指数普通数据, 市场号%d", m_iMarketId); return StrSummary; }

	int32 m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqUpdatePushGeneralNormal() { m_eCommType = ECTReqUpdatePushGeneralNormal; }
	virtual ~CMmiReqUpdatePushGeneralNormal(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求同步推送指数普通数据, 市场号%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqDelPushGeneralNormal() { m_eCommType = ECTReqDelPushGeneralNormal; }
	virtual ~CMmiReqDelPushGeneralNormal(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求删除推送指数普通数据, 市场号%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
};

//////////////////////////////////////////////////////////////////////////
// 商品分时指标数据推送

class DATAINFO_DLL_EXPORT CMmiReqAddPushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqAddPushMerchTrendIndex() { m_eCommType = ECTReqAddPushMerchTrendIndex; }
	virtual ~CMmiReqAddPushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求增加推送商品分时指标数据, 市场号%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqUpdatePushMerchTrendIndex() { m_eCommType = ECTReqUpdatePushMerchTrendIndex; }
	virtual ~CMmiReqUpdatePushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求同步推送商品分时指标数据, 市场号%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqDelPushMerchTrendIndex() { m_eCommType = ECTReqDelPushMerchTrendIndex; }
	virtual ~CMmiReqDelPushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求删除推送商品分时指标数据, 市场号%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

// 请求基金持仓数据
// 基金持仓数据
typedef struct T_FundHold
{
public:
	T_FundHold()
	{
		m_StrName		= L"";
		m_fHold			= 0.;
		m_fAllPercent	= 0.;
		m_fFundPercent	= 0.;
		m_fPreHold		= 0.;
		m_fPerCent		= 0.;
	}
	
	bool32 T_FundHold::operator < (const T_FundHold& stOther) const
	{
		return m_fPerCent <= stOther.m_fPerCent;
	}
	
	//
	CString	m_StrName;			// 基金名字
	float	m_fHold;			// 持股数据
	float	m_fAllPercent;		// 占上市公司持股
	float	m_fFundPercent;		// 占基金净值%
	float	m_fPreHold;			// 上期持股
	float	m_fPerCent;			// 比例
	
}T_FundHold;

typedef std::set<T_FundHold> ListFundHold;
typedef std::map<CMerch*, ListFundHold> MapFundHold;

//
class DATAINFO_DLL_EXPORT CMmiReqFundHold : public CMmiCommBase
{
public:
	CMmiReqFundHold() { m_eCommType = ECTReqFundHold; m_uiCRC32 = 0; }
	virtual ~CMmiReqFundHold() {}

public:
	virtual CString GetSummary() { return L"请求基金持仓数据"; }

	uint32			m_uiCRC32;
};

//
#pragma pack(push)		// 保存对齐状态
#pragma pack(1)			// 设定为1字节对齐

// 总的包头
typedef struct T_ClientRespFundHoldHead
{
	uint16		uDataType;		// 数据类型, E_ExDataType
	uint32		ulCRC32;		// CRC校验, 如果和现在数据相同，则不返回数据
	uint32		uDataLen;		// 数据长度
	
}T_ClientRespFundHoldHead;

// 每个商品的数据头部
typedef struct T_ClientRespFundHoldMerchHead
{
	uint32	uSize;			// 此结构大小
	uint32	uiMarket;		// 市场
	char	szCode[32];		// 代码
	uint32	uCRC32;			// crc校验
	uint32	uiDataLen;		// 长度
	
}T_ClientRespFundHoldMerchHead;

// 每个商品的数据体(每个基金的信息)
typedef struct T_ClientRespFundHoldMerchBody
{
	uint32	uSize;				// 此结构大小
	char	szOrgName[32];		// 基金名字
	float	fHold;				// 持股数据
	float	fAllPercent;		// 占上市公司持股
	float	fFundPercent;		// 占基金净值%
	float	fPreHold;			// 上期持股
	float	fPerCent;			// 比例
	
}T_ClientRespFundHoldMerchBody;

#pragma pack(pop)		// 恢复默认的对齐方式

class DATAINFO_DLL_EXPORT CMmiRespFundHold : public CMmiCommBase
{
public:
	CMmiRespFundHold() { m_eCommType = ECTRespFundHold; m_uiDataLen = 0; m_pData = NULL; }
	virtual ~CMmiRespFundHold() { delete m_pData; m_pData = NULL; }

	virtual CString GetSummary() { return L"应答基金持仓数据"; }

private:
	CMmiRespFundHold& CMmiRespFundHold::operator=(const CMmiRespFundHold& Src);

public:
	uint32			m_uiCRC32;			// 数据CRC校验值	0 - 表示服务器无数据 
	uint32			m_uiDataLen;
	char*			m_pData;
};

// 请求商品的分钟内外盘数据
class DATAINFO_DLL_EXPORT CMmiReqMinuteBS : public CMmiCommBase
{
public:	
	CMmiReqMinuteBS() { m_eCommType = ECTReqMinuteBS; }
	virtual ~CMmiReqMinuteBS(){}
	
public:
	virtual CString GetSummary();
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMinuteBS : public CMmiCommBase
{
public:
	CMmiRespMinuteBS() { m_eCommType = ECTRespMinuteBS; }
	virtual ~CMmiRespMinuteBS(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMinuteBS(const CMmiRespMinuteBS& stSrc);
	CMmiRespMinuteBS& CMmiRespMinuteBS::operator=(const CMmiRespMinuteBS& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MinuteBS, T_MinuteBS&> m_aMinuteBS;
};

// 分钟内外盘数据的推送
class DATAINFO_DLL_EXPORT CMmiReqAddPushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqAddPushMinuteBS() { m_eCommType = ECTReqAddPushMinuteBS; }
	virtual ~CMmiReqAddPushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求增加推送商品分钟内外盘数据, 市场号%d 代码 %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqUpdatePushMinuteBS() { m_eCommType = ECTReqUpdatePushMinuteBS; }
	virtual ~CMmiReqUpdatePushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求同步推送商品分钟内外盘数据, 市场号%d 代码 %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqDelPushMinuteBS() { m_eCommType = ECTReqDelPushMinuteBS; }
	virtual ~CMmiReqDelPushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求删除推送商品分钟内外盘数据, 市场号%d 代码 %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

// 插件认证
class DATAINFO_DLL_EXPORT CMmiReqAuthPlugInHeart : public CMmiCommBase
{
public:
	CMmiReqAuthPlugInHeart() { m_eCommType = ECTReqAuthPlugInHeart; }
	virtual ~CMmiReqAuthPlugInHeart(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求插件认证心跳"); return StrSummary; }
	
	string m_StrUseName;
	string m_StrSessionID;
	string m_StrDNS;
	string m_StrPoxyDNS;
	string m_StrProxyUserName;
	string m_StrProxyUserPwd;
	int32  m_iProxyType;
};

class DATAINFO_DLL_EXPORT CMmiRespAuthPlugInHeart : public CMmiCommBase
{
public:
	CMmiRespAuthPlugInHeart() { m_eCommType = ECTRespAuthPlugInHeart; }
	virtual ~CMmiRespAuthPlugInHeart(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ANS 回包插件认证心跳"); return StrSummary; }
	
	int32 m_iRet;
	CString m_StrUserName;
};

// 特殊交易时间
class DATAINFO_DLL_EXPORT CMmiReqTradeTime : public CMmiCommBase
{
public:
	CMmiReqTradeTime() { m_eCommType = ECTReqTradeTime; }
	virtual ~CMmiReqTradeTime(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK 请求商品交易时间"); return StrSummary; }
	
	int32	m_iTradeTimeID;
};

class DATAINFO_DLL_EXPORT CMmiRespTradeTime : public CMmiCommBase
{
public:
	CMmiRespTradeTime() { m_eCommType = ECTRespTradeTime; }
	virtual ~CMmiRespTradeTime(){}
	
	const CMmiReqTradeTime& CMmiRespTradeTime::operator=(const CMmiReqTradeTime& stSrc);

public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ANS 请求商品交易时间"); return StrSummary; }
	
	arrClientTradeTimeInfo	m_aClientTradeTimeInfo;
};

#endif

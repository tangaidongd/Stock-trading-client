#ifndef _CENTER_MANAGER_STRUCT_H_
#define _CENTER_MANAGER_STRUCT_H_

#include "typedef.h"
#include "MerchManager.h"
#include "sharestruct.h"

enum E_MsgTypeResponse	  //消息通知函数
{
	EMSG_OnMsgRealtimeTickUpdate    = 10,	    // 实时分笔更新
	EMSG_OnMsgRealtimePriceUpdate,					// 实时行情
	EMSG_OnMsgRealtimeLevel2Update,
	EMSG_OnMsgPublicFileUpdate,
	EMSG_OnMsgOnMerchTrendIndexUpdate,
	EMSG_OnMsgOnMerchMinuteBSUpdate,
	EMSG_OnMsgOnMerchAuctionUpdate,
	EMSG_OnMsgOnGeneralNormalUpdate,
	EMSG_OnMsgOnGeneralFinanaceUpdate,
	EMSG_OnMsgNewsServerDisconnected,
	EMSG_OnMsgNewsServerConnected,
	EMSG_OnMsgNewsResponse,
	EMSG_OnMsgNewsListUpdate,
	EMSG_OnMsgLandMineUpdate,
	EMSG_OnMsgKLineNodeUpdate,
	EMSG_OnMsgF10Update,
	EMSG_OnMsgClientTradeTimeUpdate,
	EMSG_OnMsgAuthPlugInHeartResp,
	EMSG_OnMsgTimesaleUpdate,
	EMSG_EMSGOnMsgViewDataOnTimerFreeMemory,
	EMSG_OnMsgViewDataOnTimerInitializeMarket,
	EMSG_OnMsgViewDataOnTimerSyncPushing
};




// 数据下载的事件通知
class  COffLineDataNotify
{
public:
	// 请求都发完了
	virtual void OnAllRequestSended(E_CommType eType) = 0;

	// 正在下载
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType) = 0;

	// 下载成功
	virtual void OnDataDownLoadFinished(E_CommType eType) = 0;

	// 下载失败
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg) = 0;
};



// 下载的参数保存结构
typedef struct T_DownLoadOffLine
{
public:
	T_DownLoadOffLine()
	{
		Clear();
	}

	void Clear()
	{
		m_aMerchs.clear();
		m_eReqType  = ECTCount;
		m_TimeBeing = 0;
		m_TimeEnd	= 0;
		m_eKLineBase= EKTBCount;
	}

	//
	E_CommType		m_eReqType;		// 类型
	vector<CMerch*> m_aMerchs;		// 商品
	CGmtTime		m_TimeBeing;	// 起始时间
	CGmtTime		m_TimeEnd;		// 终止时间
	E_KLineTypeBase m_eKLineBase;	// K 线类型

}T_DownLoadOffLine;


// 请求回来的数据
typedef struct T_IndexChsStkRespData
{
public:
	int32					m_iRespID;			// 回来的ID 号
	int32					m_iMarketId;		// 市场编号
	CString					m_StrMerchCode;		// 商品代码
	E_KLineTypeBase			m_eKLineTypeBase;	// 请求的K线类型
	CArray<CKLine, CKLine>	m_aKLines;			// K 线

	//
	T_IndexChsStkRespData()
	{
		m_iRespID		= -1;
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_eKLineTypeBase= EKTBDay;
		m_aKLines.RemoveAll();
	}

	//
	T_IndexChsStkRespData(const T_IndexChsStkRespData& Data)
	{
		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);
	}

	T_IndexChsStkRespData& operator= (const T_IndexChsStkRespData& Data)
	{
		if ( &Data == this )
		{
			return *this;
		}

		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);

		return *this;
	}

}T_IndexChsStkRespData;

enum E_AttendMerchType	  //关注商品类型
{
	EA_Arbitrage = 0,	   // 套利
	EA_StatucBar ,		   // 状态栏
	EA_Alarm,				// 预警
	EA_RecordData,			// 数据库
	EA_Choose,				// 选股
	EA_SimulateTrade,		// 模拟交易商品
};


enum E_LoginState
{
	ELSNotLogin = 0,	// 未登录
	ELSAuthing,			// 认证中
	ELSAuthSuccess,		// 认证成功
	ELSAuthFail,		// 认证失败
	ELSDirectQuote,		// 直连行情服务器
	ELSLogining,		// 登录中
	ELSLoginSuccess,	// 登录成功
	ELSLoginFail		// 登录失败
};

enum E_MerchManagerType		// merchmanager中初始化的商品列表状态
{
	EMMT_None = 0,		// 无任何状态, 暂时就处理这两种~
	EMMT_StockCn = 1,	// 含国内股票
	EMMT_FutureCn = 2,	// 含国内期货
};


// 新闻内容
typedef struct T_NewsInfo
{
public:
	int32		m_iInfoIndex;		// 资讯类型ID
	int32		m_iIndexID;			// 资讯id
	CString		m_StrContent;		// 标题内容
	CString		m_StrTimeUpdate;	// 更新时间

}T_NewsInfo;

typedef CArray<T_NewsInfo, T_NewsInfo&> listNewsInfo;

#endif
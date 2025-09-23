#ifndef _MARKETRADAR_H_
#define _MARKETRADAR_H_

#include "GmtTime.h"
#include "MerchManager.h"
#include "viewdataListener.h"
#include "BlockConfig.h"
#include <afxmt.h>
#include <set>
#include "IRegionData.h"
using std::set;
#include <map>
using std::map;
//////////////////////////////////////////////////////////////////////////
// 市场雷达
enum E_MarketRadarCondition
{
	EMRC_None = 0,				// 无选

	EMRC_TodayNewHigh		= 0x0001,		// 创今日新高
	EMRC_TodayNewLow		= 0x0002,			// 创今日新低
	EMRC_BigBuy				= 0x0004,				// 大买挂盘	- 条件大单
	EMRC_BigSell			= 0x0008,				// 大卖挂盘	- 条件大单
	EMRC_LowOpenRise		= 0x0010,			// 低开长阳
	EMRC_HighOpenFall		= 0x0020,			// 高开长阴
	EMRC_RiseOver			= 0x0040,				// 涨幅突破临界比例 xx%
	EMRC_FallBelow			= 0x0080,				// 涨幅跌破临界比例 xx%
	EMRC_SingleTradeRiseFall = 0x0100,	// 异常单笔涨跌 xx%
	EMRC_HugeAmount			= 0x0200,			// 巨额成交门限 xxx万
	EMRC_BigVol				= 0x0400,				// 大单成交 	- 条件大单
	EMRC_RiseFallMax		= 0x0800,			// 涨跌停预警
	EMRC_BigVolInTime		= 0x1000,			// x 分钟内大单成交 > xx笔 	- 条件大单
};

enum E_MarketRadarAlarmVoiceType
{
	// 声音告警方式
	EMRAVT_None = 0,		// 无声音
	EMRAVT_System,			// 系统声音
	EMRAVT_User,			// 指定播放声音文件 - 文件
};

struct T_MarketRadarSubParameter
{
	DWORD m_dwConditions;			// 条件组合

	// 参数数据
	float m_fWhatIsBigVol;			// 什么是大量(大成交量) 量大于流通股本 xx% (无百分权)
	float m_fRiseOverValue;			// 涨幅临界值
	float m_fFallBelowValue;		// 跌幅临界值
	float m_fSingleTradeRiseFallValue;		// 异常单笔涨跌值
	float m_fHugeAmountValue;		// 巨额成交门限值
	
	int32	m_iBigVolTimeRange;		// x分钟内大单成交
	int32	m_iBigVolOverCount;		// > xx笔

	// 告警方式 声音 & 弹出列表 & 窗口状态栏闪烁
	E_MarketRadarAlarmVoiceType m_eVoiceType;
	CString	m_StrVoiceFilePath;		// 指定声音文件路径 (*.wav...)

	bool32	m_bShowDlgListAlarm;		// 显示列表

	T_MarketRadarSubParameter();		// 设定默认初始值
	bool32 operator==(const T_MarketRadarSubParameter &Param) const;
	bool32 CompareCalcParam(const T_MarketRadarSubParameter &Param) const;
};

enum E_MarketRadarMerchRange
{
	EMRMR_HSA = 0,		// 沪深A
	EMRMR_ZX,			// 我的自选
	EMRMR_MerchArray,	// 用户选择的商品数组
};


struct T_MarketRadarParameter
{
	T_MarketRadarSubParameter m_ParamSub;	// 子参数
	E_MarketRadarMerchRange	  m_eMerchRange;	// 用户选择的商品范围
	MerchArray				m_aMerchs;		// 商品列表

	T_MarketRadarParameter(){ m_eMerchRange = EMRMR_HSA; }
	T_MarketRadarParameter(const T_MarketRadarParameter &Param)
		: m_ParamSub(Param.m_ParamSub)
	{ 
		m_eMerchRange = EMRMR_HSA;
		m_aMerchs.Copy( Param.m_aMerchs );
	}
	const T_MarketRadarParameter &operator=(const T_MarketRadarParameter &Param){
		if ( this != &Param )
		{
			m_ParamSub = Param.m_ParamSub;
			m_eMerchRange = Param.m_eMerchRange;
			m_aMerchs.Copy( Param.m_aMerchs );
		}
		return *this;
	}

	bool32	CompareCalcParam(const T_MarketRadarParameter &Param) const;
};

struct T_MarketRadarResult
{
	CMerch *m_pMerch;						// 商品 & 实时报价数据
	CTick	m_TickLast;						// 最后一笔成交的tick记录, 有标志则买红卖绿，否则白色
	CGmtTime	m_TimeCalc;					// 计算时间
	CGmtTime	m_TimeInit;					// 对应的商品在计算时的初始化时间
	int32	m_iRiseFallFlag;				// 涨跌标志, <0 跌 0平 >0涨
	E_MarketRadarCondition m_eCondition;	// 满足的条件
};
CString	GetMarketRadarResultString(const T_MarketRadarResult &Result);
typedef CMap<CMerch *, CMerch *, T_MarketRadarResult, const T_MarketRadarResult&> MarketRadarResultMap;	// 计算结果集
typedef CArray<T_MarketRadarResult, const T_MarketRadarResult &> MarketRadarResultArray;


class CViewData;

class CMarketRadarListener
{
public:
	virtual	~CMarketRadarListener();

	enum E_AddCause
	{
		EAC_UpdateCondition = 0,	// 该商品的满足条件已经变更
		EAC_EqualCondition,			// 该商品满足的条件并没有变更
	};

	enum E_RemoveCause
	{
		EMC_OverInitTime = 0,		// 超过初始化时间的删除
		EMC_NotSatisfyCondition,	// 不在符合条件
	};

	// 所有通知都是异步的，而且通知发出来后数据可能已经再次变化了
	virtual void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause) = 0;	// 线程调用结果新的结果集, 异步
	virtual void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause) = 0;		// 结果被新的运算排除, 异步
	virtual  void OnClearMarketRadarResult() = 0;			// 结果集被清除
};

class CMarketRadarAlarm : public CMarketRadarListener
{
public:
	// 预警处理
	virtual	 void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause);	// 线程调用结果新的结果集, 异步
	virtual void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause){};		// 结果被新的运算排除, 异步
	virtual  void OnClearMarketRadarResult(){};			// 结果集被清除
};

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMRadarMap : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
	typedef CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> _MyBase;
	
	CMRadarMap(int nBlockSize = 10) : _MyBase(nBlockSize) {}
	
	VALUE *FindValue(ARG_KEY key)
	{
		// 查找key对应的value，有则返回指向value的指针，否则NULL
		// 该指针可能会在变更map数据结构的操作后失效
		ASSERT_VALID(this);
		
		UINT nHash;
		CAssoc* pAssoc = GetAssocAt(key, nHash);
		if (pAssoc == NULL)
			return NULL;  // not in map
		
		return &pAssoc->value;
	}
	
	const VALUE *FindValue(ARG_KEY key) const
	{
		return const_cast<CUserNoteMap *>(this)->FindValue(key);
	}
};

// 雷达计算
// 1.每隔t1时间，请求m条数据的数据，直至所有商品扫描完成
// 2.休息t2时间，再次开始1计算, 如果条件并未变更，且该商品不在交易时间段内，则跳过该商品的重复请求与计算
class CMarketRadarCalc : public CViewDataListner, public CBlockConfigListener
{
public:
	~CMarketRadarCalc();

	static CMarketRadarCalc &Instance();

	bool32	SetParameter(const T_MarketRadarParameter &Param);
	const T_MarketRadarParameter &GetParameter() const { return m_Param; };
	void	GetParameter(OUT T_MarketRadarParameter &Param) { Param = m_Param; };

	bool32	StartCalc();
	void	StopCalc();

	bool32	InitAndStartIf();	// 读取上次设置的结果，如果上次是运行的话，开始运行

	LONG		GetRunFlag(){ return (LONG)InterlockedCompareExchange(/*(PVOID *)&*/&m_lRunFlag, 0, 0); }

	void	RemoveResult(CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener = NULL);	// 删除商品的计算结果，商品不符合时或者商品到了初始化时间时，则清除该计算结果
	void	ClearResults(CMarketRadarListener *pIgnoreListener = NULL);		// 清除结果
	void	GetResults(OUT MarketRadarResultMap &mapResults, CMarketRadarListener *pIgnoreListener = NULL); // 获取计算的结果，此时将从结果中删除部分要初始化的商品
	void	GetResults(OUT MarketRadarResultArray &aResults, CMarketRadarListener *pIgnoreListener = NULL);	// 获得按时间早到晚的计算结果
	void	OnTimerFilterNeedInitMerch(CMarketRadarListener *pIgnoreListener = NULL);		// 调用此方法过滤部分要初始化的商品结果

	void		SaveStartFlag();		// 保存启动标志

	static	void	ResultMapToArray(const MarketRadarResultMap &mapResult, OUT MarketRadarResultArray &aResults);	// 按照计算时间排序结果

	static	void		ReadMerchArray(OUT MerchArray &aMerchs);	// 读取单独商品列表, 非xml
	static	void		SaveMerchArray(const MerchArray &aMerchs);	// 保存单独商品列表, 非xml

	void	AddListener(CMarketRadarListener *pListener);
	void	RemoveListener(CMarketRadarListener *pListener);

	// 行情, 获取换手率等数据, 无换手则应该是无成交量无交易，不考虑服务器没有流通股本数据情况	
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	
	// k 线, 不需要K线数据
	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine){};
	
	// 分笔
	virtual void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);

	virtual bool32 IsAttendCommType( E_CommType eType );

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType){};

private:
	CMarketRadarCalc();
	CMarketRadarCalc(const CMarketRadarCalc &);
	CMarketRadarCalc &operator=(const CMarketRadarCalc &);

	typedef CArray<CTick, const CTick &> TickArray;
	struct T_CalcMerchData
	{
		// 待计算商品数据
		CMerch			*m_pMerch;	// 标志该商品
		CRealtimePrice	 m_RealTimePrice;	// 实时报价
		TickArray		m_aTicks;	// 分笔数据

		float			m_fRiseFallStopRate;	// 涨停比例, 0.0f为没有涨跌停
		CGmtTime		m_TimeInit;		// 该商品对应的最近向前交易日时间(今天或以前)

		T_CalcMerchData();
		T_CalcMerchData(const T_CalcMerchData &MerchData);
		const T_CalcMerchData &operator=(const T_CalcMerchData &MerchData);
	};
	typedef CMRadarMap<CMerch *, CMerch *, T_CalcMerchData, const T_CalcMerchData &> CalcMerchDataMap;
	//typedef CMRadarMap<CMerch *, CMerch *, CGmtTime, const CGmtTime &> MerchTimeMap;
	typedef map<CMerch *, CGmtTime>		MerchTimeMap;

	static UINT __cdecl CalcThreadFunc( LPVOID pParam );		// 计算线程
	static UINT __cdecl ReqThreadFunc( LPVOID pParam );			// 请求线程

	void		ReadInitParam();		// 读取初始化配置, 商品列表怎么办~~, 数据量太大~
	void		SaveInitParam();		// 保存初始化配置
	

	LONG		SetRunFlag(LONG flag){ return InterlockedExchange((LPLONG)&m_lRunFlag, flag); }
	

	bool32		AddResult(const T_MarketRadarResult &Result);
	int32		AddResult(const MarketRadarResultMap &Results);

	void FireResultAdd(const MarketRadarResultMap &mapResults, CMarketRadarListener::E_AddCause eCause, CMarketRadarListener *pIgnoreListener = NULL);
	void		FireResultRemove(CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener = NULL);
	void		FireResultClear(CMarketRadarListener *pIgnoreListener = NULL);

	void		AddCalcMerchData(CMerch *pMerch, const CRealtimePrice &RealTimePrice);
	void		AddCalcMerchData(CMerch *pMerch, const TickArray &aTicks);
	void		DoAddNewCalcMerchData(INOUT T_CalcMerchData &MerchData);	// 实际添加到新数据队列，未同步

	bool32		RequestStartViewData();		// 开始请求商品数据，初始化等待数组等
	bool32		RequestNextViewData();		// 请求下一轮商品的数据, true有商品请求，false已经全部请求完毕
	bool32		TryRequestWaitEchoData();	// 尝试请求等待队列中的商品数据
	bool32		GetNextNewCalcMerchData(OUT T_CalcMerchData &MerchData, bool32 bPop=true);	// 获取下一个要计算的商品数据, bPop取完则删除
	void		ClearNewCalcMerchData();	// 清除要计算的商品数据

	int32		RequestMerchData(const MerchArray &aMerchs);

	CGmtTime	GetNow();

	bool32		IsMerchDataExpire(const T_CalcMerchData &MerchData);	// 商品数据是否已经不在有效时段了

	T_MarketRadarParameter m_Param;
	HANDLE				   m_hThreadCalc;	// 计算线程句柄
	HANDLE				   m_hThreadReq;	// 请求线程句柄
	volatile	LONG	   m_lRunFlag;	// 为非0则允许运行，0则不允许

	// 商品数据只需要被更新，不需要强制删除所有的数据
	CalcMerchDataMap		m_mapCalcMerchDataAll;	// 所有商品数据集合, 当有新数据回来时，在这里取其它数据
	CalcMerchDataMap		m_mapCalcMerchDataNew;	// 新返回的未处理的商品数据集合
	MarketRadarResultMap	m_mapResult;	// 结果集

	CAbsCenterManager				*m_pAbsCenterManager;

	MerchArray				m_aMerchsWaitReq;	// 等待发送请求的商品
	MerchArray				m_aMerchsWaitEcho;	// 当前已发出请求，正在等待数据回来的商品
	MerchTimeMap			m_mapMerchReqTimes;	// 每个商品发出去的请求时间

	CCriticalSection				m_LockResult;			// 结果集锁
	CCriticalSection				m_LockCalcMerchDataNew;	// 新接收到的商品数据锁
	CCriticalSection				m_LockReq;				// 请求锁
	CEvent							m_EventNewMerchData;		// 是否有商品数据可计算了

	//CSemaphore						m_SemaphoreMerchData;	// 有多少个数据

	CGmtTime	/*volatile*/			m_TimeCalcStart;		// 计算开始时的服务器时间
	DWORD								m_dwTimeCalcStart;	// 计算开始时的本地计时
	CArray<CMarketRadarListener *, CMarketRadarListener *>	m_aListeners;

	CMarketRadarAlarm			m_Alarm;
};


#endif //_MARKETRADAR_H_
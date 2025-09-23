#ifndef _RECORD_DATA_CENTER_H
#define _RECORD_DATA_CENTER_H

#include <afxtempl.h>
#include "IoViewShare.h"
#include "ViewDataListener.h"
#include "XTimer.h"
#include "synch.h"
#include "DataBase.h"
#include "Recordset.h"
#include <set>
#include <list>

#define MA_NUMS			3		// ma 线的条数固定为 3 条

using namespace std;

class CDataBaseProcess;
class CViewData;
class CShowTick;

typedef vector<float> ArrayFloat; 

// 定时器
extern const int32 KiTimerIdSaveData;
extern const int32 KiTimerPeriodSaveData;


//
typedef struct T_MerchNode
{
public:
	T_MerchNode()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
	}

	bool T_MerchNode::operator< (const T_MerchNode& stOther) const
	{
		if ( m_iMarketId < stOther.m_iMarketId )
		{
			return true;
		}
		else if ( m_iMarketId == stOther.m_iMarketId )
		{
			return (m_StrMerchCode.CompareNoCase(stOther.m_StrMerchCode) <= 0);
		}

		return false;
	}

	int32		m_iMarketId;
	CString		m_StrMerchCode;

}T_MerchNode;

// 文件中记录的周期结构
typedef struct T_Circles
{
	T_Circles()
	{
		m_iCircle		= 0;
		m_ReqTime		= 0;
		m_StrTableName	= L"";
		m_eIntervalOrignal = ENTICount;
		m_eKLineBase	= EKTBCount;
	}

	bool32 T_Circles::operator<(const T_Circles& stOther) const
	{
		if ( m_iCircle < stOther.m_iCircle )
		{
			return true;
		}
		else if ( m_iCircle == stOther.m_iCircle )
		{
			return (m_ReqTime.GetTime() < stOther.m_ReqTime.GetTime());
		}
		
		return false;
	}

	//
	int32		m_iCircle;
	CGmtTime	m_ReqTime;
	CString		m_StrTableName;

	E_NodeTimeInterval	m_eIntervalOrignal;	// 原始的周期
	E_KLineTypeBase		m_eKLineBase;		// 实际对应的 K 线基本周期
	
}T_Circles;

typedef list<CMmiReqMerchKLine> ListKLineReqs;
typedef map<CMerch*, ListKLineReqs> mapMerchReqs;

//
class CRecordDataCenter : public CViewDataListner
{
public:
	enum E_ErrorType					// 错误类型
	{
		EETNone = 0,					// 无错误
		EETFile,						// 文件不存在
		EETBlock,						// 板块非法
		EETTimeInterval,				// 周期错误
		EETDataBase,					// 数据库操作错误

		EETCount,
	};

	//
	enum E_WorkState
	{
		EWSNone = 0,					// 没有干活
		EWSSH,							// 在处理沪深板块的
		EWSHK,							// 在处理港股的
		EWSBoth,						// 同时处理以上两类
	};

	// From CViewDataListner
public:
	
	// 行情	
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	
	// Tick
	virtual void OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId);

	// K 线
	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine);

public:
	CRecordDataCenter(CAbsCenterManager* pAbsCenterManager);
	~CRecordDataCenter();

	bool32				InitialFromFile();										// 配置文件初始化

	// 开始工作
	bool32				StartWork();											
	
	// 结束工作	
	bool32				StopWork();											

	// 得到配置文件名
	static CString		GetSqlConfigName();										

	// 提示错误
	void				PromptDbErrMsg();										
	
	// 生成文件名
	std::string			GeneralFileName();
	
	// 记录日志
	void				DebugLog(LPCTSTR StrFormat, ...);

	// 取当前该请求的商品
	bool32				GetRequest(OUT CMerch*& pMerch, OUT CMmiReqMerchKLine& stReq);

	// 取得保存数据的时间
	CGmtTime			GetSaveTime(const CString& StrTime);

	// 设置待记录的数据
	void				SetSaveMerchData(const CMerchKLineNode& stMerchSave, bool32 bJustProcessSelfCircle);

	// 取记录数据
	bool32				GetMerchDataToSave(CMerchKLineNode& stMerchSave);

	// 记录完了, 清除本地的数据
	void				ClearLoacalSaveMerchData();

	// 得到K 线的时间字符串(数据库记录用)
	CString				GetKLineTimeString(const CKLine& KLine);
	CString				GetTimeString(const CGmtTime& Time, bool32 bOnlyDay = false);

	// 设置工作状态
	void				SetWorkState(E_WorkState eState);

	// 获取工作状态
	E_WorkState			GetWorkState();

	// 第一次启动时, 先清空数据库
	bool32				TruncateTableContent(const CString& StrTableName);

	// 查询K 线是否已经存在数据库
	
	
	// 记录数据库
	
	// 生成插入K 线的语句
	CString				GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults);
	CString				GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	CString				QueryKLineSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine);

	bool32				BeKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine);

	CString				GeneralKLineUpdateSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	void				UpdateKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	bool32				SaveKLineToDB(CMerch* pMerch, const CString& StrTableName, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults, bool32 bNeedJudgeSame = false);

	// 行情数据相关数据库操作
	bool32				BeRealtimePriceInDB(CMerch* pMerch);

	CString				QueryRealtimePriceSql(CMerch* pMerch);

	CString				GeneralRealtimePriceInsertSql(CMerch* pMerch);

	CString				GeneralRealtimePriceUpdateSql(CMerch* pMerch);

	// Tick 数据相关数据库操作
	CString				GeneralTickInsertSql(CMerch* pMerch, const CShowTick& Tick);

	// 定时器相关
public:
	void				SetRecordDataCenterTimer(int32 iTimerId, int32 iTimerPeriod);

	void				StopRecordDataCenterTimer(int32 iTimerId);

	void				OnRecordDataCenterTimer(int32 iTimerId);

	void				CheckTimeOut();

	// 线程相关
public:
	// 请求数据的线程函数
	bool32				BeginReqDataThread();
	static DWORD		WINAPI ThreadRequestData(LPVOID pParam);
	bool32				ThreadRequestData();
	
	// 处理数据的线程函数(第一次初始化和实时的分钟K数据记录)
	bool32				BeginSaveDataThread();
	static DWORD		WINAPI ThreadSaveData(LPVOID pParam);
	bool32				ThreadSaveData();

	// 定时记录其他周期的数据
	bool32				BeginTimerRecordThread();
	static DWORD		WINAPI ThreadTimerRecord(LPVOID pParam);
	bool32				ThreadTimerRecord();

private:
	
	// 周期是否合法
	bool32				BeValidCircle(int32 iCircle);
	
	// MA 参数是否合法
	void				ValidMaParms(int32& iMA);

	// 取得最大的 MA 周期
	int32				GetMaxMa();

	// 生成时间
	bool32				GetGmtTime(string StrTimeSrc, OUT CGmtTime& Time);

	// 生成各周期的请求
	bool32				GeneralRequests();

	// test
	void				TestRecordSqlToFile(const CString& StrSql, char* strFileName);

private:
	bool32				MA_Array(IN CKLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float*& pfResult);	

private:	
	bool32				m_bStart;									// 工作标志
	bool32				m_bInitialReqRealtime;						// 行情和 Tick  数据请求一开始就全部发出去

	int32				m_iReqID;									// 当前所发的请求ID 号
	bool32				m_bTimeToReqNext;							// 是否可以请求下一个商品

	E_ErrorType			m_eErrType;									// 错误类型
	
	E_WorkState			m_eWorkState;								// 工作状态
	LockSingle			m_LockWorkState;			

	CAbsCenterManager*			m_pAbsCenterManager;								// CAbsCenterManager

	FILE*				m_pFile;									// 日志	
	LockSingle			m_LockDebugLog;								// 写文件的锁

	set<CMerch*>		m_aMerchs;									// 所有的商品	
	LockSingle			m_LockaMerchs;
	
	DWORD				m_dwTimeRecord;								// 上一次定时记录数据的时间
	LockSingle			m_LockTimeRecord;									

	set<T_Circles>		m_aCircles;									// 周期
	bool32				m_b1Min;									// 是否有 1 分钟的周期

	int32				m_aMaParams[MA_NUMS];						// 字段
	LockSingle			m_LockaFields;

	bool32				m_bInitialSaveFinish;						// 初始化保存数据过程是否结束

	vector<CMmiReqMerchKLine>	m_aReqs;							// 每个商品需要发的几个请求(里面只保存了关键的字段值, 实际请求的时候需要补上商品信息等)
	
	map<E_KLineTypeBase, int32> m_mapMaxKLineCount;					// 每个周期需要的K 线最小数目

	//
	mapMerchReqs		m_mapMerchsForReq;							// 用来请求的商品集合(最开始等于m_aMerchs, 后来慢慢递减至0)
	LockSingle			m_LockMerchsForReq;								

	bool32				m_bstMerchSaveValid;						// 待记录的数据是否合法
	LockSingle			m_LockFlagstMerchSaveValid;				

	bool32				m_bJustProcessSelfCircle;					// 是否只处理本周期的(初始化的时候为 false. 分钟线实时数据的时候为 true. 定时器 1 分钟记录一次数据的时候为false)
	LockSingle			m_LockJustProcessSelfCircle;				

	CMerchKLineNode		m_stMerchSave;								// 待记录的数据
	LockSingle			m_LockstMerchSave;							// 锁

	DWORD				m_dwTimeSendReq;							// 发请求的时间

	// 数据库相关:
	CDataBase*			m_pDbProcess;								// 数据库处理中心
	CString				m_StrDbAddress;								// 地址
	CString				m_StrDbName;								// 名称
	DWORD				m_dwDbPort;									// 端口
	CString				m_StrDbUserName;							// 用户名		
	CString				m_StrDbPassword;							// 密码

	// 线程相关
	HANDLE				m_hThreadReqData;							// 请求线程
	HANDLE				m_hThreadSaveData;							// 取数据线程
	HANDLE				m_hThreadTimerRecord;						// 定时记录的线程

	bool32				m_bThreadReqExit;							// 请求线程退出标志
	bool32				m_bThreadSaveExit;							// 取数据线程退出标志
	bool32				m_bThreadTimerRecordExit;					// 定时记录线程退出标志
};

#endif // _RECORD_DATA_CENTER_H
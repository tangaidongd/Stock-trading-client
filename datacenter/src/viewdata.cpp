#pragma warning(disable: 4786)

#include "StdAfx.h"

#include "sharestruct.h"
#include "viewdata.h"
#include "LogFunctionTime.h"
#include "BlockConfig.h"
#include "ReportScheme.h"
#include "PluginFuncRight.h"
#include "coding.h"
#include "CCodeFile.h"
#include "LandMineStruct.h"
#include "OfflineDataManager.h"
#include "pinyin.h"
#include "../../JsonCpp/json.h"

//#include "ShortCut.h"
//#include "AlarmCenter.h"
//#include "UploadUserData.h"
//#include "ArbitrageManage.h"

static  T_ErrCode2StrInfo s_aPlusError[] = 
{
	//dll中针对不同错误的返回码
	T_ErrCode2StrInfo(-1, L"catch捕捉错误"),
	T_ErrCode2StrInfo(-2, L"初始化curl会话失败"),
	T_ErrCode2StrInfo(-3, L"返回与请求协议不对称"),
	T_ErrCode2StrInfo(-4, L"返回数据解析有误"),
	//以下错误码为服务端返回
	T_ErrCode2StrInfo(1001, L"未知异常"),
	T_ErrCode2StrInfo(1004, L"身份不合法，请重新登录!"),
	T_ErrCode2StrInfo(1011, L"报文格式错误"),
	T_ErrCode2StrInfo(1012, L"报文Session不正确!"),
	T_ErrCode2StrInfo(1013, L"报文读取错误!"	),
	T_ErrCode2StrInfo(1014, L"加密配置与服务端不一致!"),
	T_ErrCode2StrInfo(1015, L"验证码输入有误或已过期!"),
	T_ErrCode2StrInfo(1016, L"身份不正确，无法登录!"),
	T_ErrCode2StrInfo(1017, L"登录时间不正确！"	),
	T_ErrCode2StrInfo(1018, L"请求报文格式不正确！"),
	T_ErrCode2StrInfo(1020, L"日期格式错误!"	),
	T_ErrCode2StrInfo(1042, L"登录票据不正确！"),
	T_ErrCode2StrInfo(1044, L"协议版本不匹配！"),
	T_ErrCode2StrInfo(1045, L"未知名报文！"),
	T_ErrCode2StrInfo(10001, L"由于您长时间没有操作,所以您的用户状态已失效,请重新登录！"	),
	T_ErrCode2StrInfo(10016, L"交易代码不存在！"	),
	T_ErrCode2StrInfo(10017, L"登录密码输入有误！"	),
	T_ErrCode2StrInfo(10022, L"没有此用户,请重新登录！"),
	T_ErrCode2StrInfo(99999, L"未知错误")  //用于判断结尾
		
};

extern const TCHAR KStrLoginLogFileName[];		// 登录时间长Trace文件名

// 定时清理内存定时器
const int32 KTimerIdFreeMemory				= 1;
const int32 KTimerPeriodFreeMemory			= 3 * 60 * 1000;	// ms unit	// 每隔一段时间清理一次内存

const int32 KTimerIdSyncPushing				= 2;
const int32 KTimerPeriodSyncPushing			= 1000 * 20;	    // ms unit	// 每隔一段同步一次推送

// 定时检查市场初始化定时器
const int32 KTimerIdInitializeMarket		= 3;
const int32 KTimerPeriodInitializeMarket	= 30 * 1000;	    // ms unit

// 定时更新服务器时间
const int32 KTimerIdUpdateServerTime		= 4;
const int32 KTimerPeriodUpdateServerTime	= 50;			    // ms unit

// 登录等待行情服务器连接
const int32 KTimerIdWaitQuoteConnet			= 5;
const int32 KTimerPeriodWaitQuoteConnet		= 500;			    // ms unit

// 直连的时候等待行情服务器连接
const int32 KTimerIdWaitDirectQuoteConnet	= 6;

// 等 10 秒行情连接. 
const int32 KiCountsWaitQuoteConnet			= 10 * 1000 / KTimerPeriodWaitQuoteConnet;

// 等待 20 秒钟板块文件传送
const int32 KTimerIdWaitLogicBlockFileTraslateTime = 6;
const int32 KTimerPeriodLogicBlockFileTraslateTime = 20 * 1000;

// 一次请求的市场个数
static const int32 s_KiMarketRequestSegment	= 1;

// 一共要请求多少个配置信息
//static const int32 s_KiConfigCnt = 4;

//
int32			m_iTimeInitializePerDay;// 每天初始化时间。格式：以分钟为单位， 相对于00:00分的值
// 客户端每天都需要对系统的商品列表做初始化， 
// 该时间描述每次初始化的时间（格林威治时间）， 对于国内股市， 该时间即“集合竞价”时间
// 
CArray<T_OpenCloseTime, T_OpenCloseTime>	m_OpenCloseTimes;	// 每天几个开盘时间段（格林威治时间）

// 后两天的交易日和前的交易日(如今天明天,然后往昨天10个交易日)
CArray<uint32, uint32>	m_aRecentActiveDay;	

// Breed
static const char*	KStrElementNameBreed		= "breed";
static const char*	KStrAttriBreedID			= "id";
static const char*	KStrAttriBreedShowID		= "showid";
static const char*	KStrAttriBreedNameCn		= "namecn";
static const char*	KStrAttriBreedNameEn		= "nameen";

// Market
static const char*	KStrElementNameMarket		= "market";
static const char*	KStrAttriMarketID			= "id";
static const char*	KStrAttriMarketShowID		= "showid";
static const char*	KStrAttriMarketTimeZone		= "timezone";
static const char*	KStrAttriMarketNameCn		= "namecn";
static const char*	KStrAttriMarketNameEn		= "nameen";
static const char*	KStrAttriMarketReportType	= "reportype";
static const char*	KStrAttriMarketVolScale		= "volscale";
static const char*  KStrAttriMarketIniPerDay	= "iniPerDay";
static const char*  KStrAttriMarketOCTimes		= "octimes";
static const char*  KStrAttriMarketActiveDay	= "ActiveDay";

// CMerch
static const char*	KStrElementNameMerch		= "merch";
static const char*	KStrAttriMerchCode			= "code";
static const char*	KStrAttriMerchNameCn		= "namecn";
static const char*	KStrAttriMerchNameEn		= "nameen";
static const char*	KStrAttriMerchSaveDec		= "savedec";

/////////////////////////////////////////////
// 每个消息处理的应答包数
const int32 KProcessPackageCountPerMsg		= 100;

// t...fangz0828 
extern void TestKLine(const CArray<CKLine,CKLine>& aKLines,const char * pFile,CString StrMerchCode, int32 iMarketId);

// fangz0301
static CString GetTimeString()
{
	CTime Time = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(L"%02d:%02d:%02d", Time.GetHour(), Time.GetMinute(), Time.GetSecond());
	
	return StrTime;
}

// 为同步同步所用
struct T_MerchAndCommunication
{
public:
	T_MerchAndCommunication()
	{
		m_pMerch	= NULL;
		m_iCommunicationId = -1;
	}
	
public:
	CMerch			*m_pMerch;
	int32			m_iCommunicationId;
};

// 登录成功的备份信息

typedef struct T_LoginBackUp
{
public:
	T_LoginBackUp()
	{
		m_iSize = 0;
		m_iQuoterSvrPort = 0;
		m_uiUserID = 0;
		m_iServiceCount = 0;
		m_pServices = NULL;
	}
	
	//
	int32  m_iSize;

//	char   m_StrQuoterSvrAdd[100];		// 行情服务器地址
	int32  m_iQuoterSvrPort;		// 行情服务器端口
	CString m_StrQuoterSvrAdd;

	//
	int32  m_uiUserID;

	// 支持的数据服务ID 号
	int32  m_iServiceCount;
	int32* m_pServices;

}T_LoginBackUp;	
	   
CViewData::CViewData(CCommManager *pCommManager,CProxyAuthClientHelper* pAutherManager, CNewsManager* pNewsManager, CDataManager* pDataManager, CAbsOfflineDataManager &OfflineDataManager, HWND OwnerWnd)
:m_pCommManager(pCommManager),
m_pAutherManager(pAutherManager),
m_pNewsManager(pNewsManager),
m_pDataManager(pDataManager),
m_OfflineDataManager(OfflineDataManager),
m_OwnerWnd(OwnerWnd)
{
	m_pServiceDispose = pAutherManager->GetInterface()->GetServiceDispose();
	m_bOffLine = false;
	m_bManualReconnect = false;
    m_bDownLoading = false;

	// 
	m_eLoginState		= ELSNotLogin;

	// 
	m_iRecvedPackagerCount = 0;

	m_uiTimeLastUpdate = 0;

	//
	m_iNumsOfIndexChsStkReq	= -1;
	m_iNumsOfIndexChsStkHaveRecved = 0;
	m_pIndexChsStkMideCore = NULL;
	m_aRecvDataForIndexChsStk.RemoveAll();

	// 向DataManager注册自己
	m_pDataManager->AddDataManagerNotify(this);

	// 向 COptimizeServer 注册自己
	COptimizeServer::Instance()->AddNotify(this);
	COptimizeServer::Instance()->SetViewData(this);
	//
	m_aRecordDataCenterAttendMerchs.RemoveAll();
	m_aIndexChsDataAttendMerchs.RemoveAll();
	m_aSimulateTradeAttendMerchs.RemoveAll();

	m_bLogWaitQuoteConnect	= true;
	m_bLogWaitQuoteSortFinish = true;
	m_iWaitQuoteConnectCounts = 0;
	m_iWaitDirectQuoteConnectCounts = 0;
	m_aQuoteServerAll.RemoveAll();

	m_bKickOut	= false;

	m_dwMerchManagerType = EMMT_None;

	//
	m_aViewDataLinstner.RemoveAll();
	m_aViewNewsLinstner.RemoveAll();

	//
	m_aGeneralNormalH.RemoveAll();
	m_aGeneralNormalS.RemoveAll();

	//
//	m_pAlarmCenter = NULL;	

	//
	m_mapFundHold.clear();
	m_uiFundHoldCrc32 = 0;

	//
	m_aMarketToRequest.clear();
	m_aMarketRequesting.clear();

	m_bFastLogIn = true;

	//
	m_bAuthError = false;

	//
	m_bTodayFalg = false;

	//
	m_bMessageFlag = false;
	m_bDirectToQuote = false;
	m_iJinpingID = -1;

//	m_pTradeBid = NULL;
	m_bServerDisConnect	= false;
	m_fPriceClose = 0.0f;
	//m_bUseTradeQuote = false;

	m_LoginWnd = NULL;
	m_MainFrameWnd = NULL;

	m_strProduct = _T("");
	m_iRespConfigCnt = 0;
    m_iConfigCnt = 8;

	m_iInitializedMarketCount = 0;
	m_iAllMarketCount = 0;
	m_bReadyBreed = false;
	m_bBreedFromOfflineFile = false;
}

CViewData::~CViewData()
{
	// 向DataManager注销自己
	m_pDataManager->RemoveDataManagerNotify(this);

	// 向 COptimizeServer 注销自己
	COptimizeServer::Instance()->RemoveNotify(this);

	// 删除优选资源
	COptimizeServer::DelInstance();
	//
	m_IoViewList.RemoveAll();

//	DEL(m_pAlarmCenter);

	m_Timer.stopTimer();
}

bool32 CViewData::Construct()
{
	// 初始化拼音模块
//	InitPy();

	// 创建快捷键列表
//	BuildShortCutHotKeyList();

	// 定时器
	StartTimer(KTimerIdFreeMemory, KTimerPeriodFreeMemory);
	StartTimer(KTimerIdSyncPushing, KTimerPeriodSyncPushing);
	StartTimer(KTimerIdInitializeMarket, KTimerPeriodInitializeMarket);
	StartTimer(KTimerIdUpdateServerTime, KTimerPeriodUpdateServerTime);
	
	return true;
}

void CViewData::AddViewListener(IN CIoViewListner *pIoView)
{
	if (NULL == pIoView)
		return;

	bool32 bFind = FALSE;
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		if (m_IoViewList[i] == pIoView)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		m_IoViewList.Add(pIoView);
	}
}

void CViewData::RemoveViewListener(IN CIoViewListner *pIoView)
{
	if (NULL == pIoView)
		return;

	// 找到所有的, 剔除（可能有重复的情况）
	while (1)
	{
		int32 iFindPos = -1;
		for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
		{
			if (m_IoViewList[i] == pIoView)
			{
				iFindPos = i;
				break;
			}
		}

		if (iFindPos >= 0)
		{
			m_IoViewList.RemoveAt(iFindPos, 1);
		}
		else
		{
			break;
		}
	}
}


void CViewData::RequestViewData(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID /*= -1*/,IN int eDataSource  /* = EDSAuto*/)
{
	if (NULL == pMmiCommBase)
		return;

#ifdef TRACE_DLG
	//if (NULL != g_hwndTrace)
	//{
	//	CString *pStrTrace = new CString;
	//	*pStrTrace = pMmiCommBase->GetSummary();
	//	::PostMessage(g_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	//}
#endif
	
	switch(pMmiCommBase->m_eCommType)
	{
	case ECTReqAuth:  // 请求认证
		{
			OnViewReqAuth((const CMmiReqAuth *)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqMarketInfo:	// 请求市场信息
		{
			OnViewReqMarketInfo((const CMmiReqMarketInfo *)pMmiCommBase);
		}
		break;
	case ECTReqMarketSnapshot:	// 请求市场快照信息
		{
			OnViewReqMarketSnapshot((const CMmiReqMarketSnapshot *)pMmiCommBase);
		}
		break;
	case ECTReqBroker:	// 请求经济席位信息
		{
			OnViewReqBroker((const CMmiReqBroker *)pMmiCommBase);
		}
		break;
	case ECTReqMerchInfo:	// 请求商品信息
		{
			OnViewReqMerchInfo((const CMmiReqMerchInfo *)pMmiCommBase);
		}
		break;
	case ECTReqPublicFile:	// 请求服务器文件
		{
			OnViewReqPublicFile((const CMmiReqPublicFile *)pMmiCommBase);
		}
		break;

	case ECTReqMerchExtendData:	// 请求商品扩展数据
		{
			OnViewReqMerchExtendData((const CMmiReqMerchExtendData *)pMmiCommBase);
		}
		break;
	case ECTReqMerchF10:	// 请求商品扩展数据
		{
			OnViewReqMerchF10((const CMmiReqMerchF10 *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimePrice:	// 请求商品实时5档行情
		{
			OnViewReqRealtimePrice((const CMmiReqRealtimePrice *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimeTick:	// 请求商品Tick
		{
			OnViewReqRealtimeTick((const CMmiReqRealtimeTick *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimeLevel2:	// 请求商品实时level2数据
		{
			OnViewReqRealtimeLevel2((const CMmiReqRealtimeLevel2 *)pMmiCommBase);
		}
		break;
	case ECTReqMerchKLine:	// 请求商品历史K线数据
		{
			OnViewReqMerchKLine((const CMmiReqMerchKLine *)pMmiCommBase);
		}
		break;
	case ECTReqMerchTimeSales:	// 请求商品分笔成交历史
		{
			OnViewReqMerchTimeSales((const CMmiReqMerchTimeSales *)pMmiCommBase);
		}
		break;
	case ECTReqReport:	// 请求报价表
		{
			OnViewReqReport((const CMmiReqReport *)pMmiCommBase);
		}
		break;
	case ECTReqBlockReport: // 板块报价表商品数据排行请求
		{			
			OnViewReqBlockReport((const CMmiReqBlockReport *)pMmiCommBase);
		}
		break;
	case ECTReqTradeTime:	// 商品的特殊交易时间
		{
			OnReqClientTradeTime((const CMmiReqTradeTime*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTRegisterPushPrice:	// 注册买卖盘报价实时推送
		{
			OnViewRegisterPushPrice((const CMmiRegisterPushPrice *)pMmiCommBase);
		}
		break;
	case ECTRegisterPushTick:	// 注册分笔实时推送
		{
			OnViewRegisterPushTick((const CMmiRegisterPushTick *)pMmiCommBase);
		}
		break;
	case ECTRegisterPushLevel2:	// 注册Level2实时推送
		{
			OnViewRegisterPushLevel2((const CMmiRegisterPushLevel2 *)pMmiCommBase);
		}
		break;
	case ECTCancelAttendMerch:	// 请求取消商品实时数据推送
		{
			// zhangbo 20090626 #由于RegisterRealtime*一组函数包含有取消推送功能， 一般情况下不需要实现Cancel功能， 实现的目的是为了最大限度的减少服务器压力
//			OnViewCancelAttendMerch((const CMmiCancelAttendMerch *)pMmiCommBase);
		}
		break;

	case ECTReqNetTest:
		{
			OnViewReqNetTest((const CMmiReqNetTest *)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqGeneralNormal: // 请求大盘的普通数据
		{
			OnViewReqGeneralNormal((const CMmiReqGeneralNormal*)pMmiCommBase, iCommunicationID);	
		}
		break;
	case ECTReqGeneralFinance: // 请求大盘财务数据(相关数值视图计算用)
		{
			OnViewReqGeneralFiniance((const CMmiReqGeneralFinance*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqMerchTrendIndex:
		{
			OnViewReqMerchTrendIndex((const CMmiReqMerchTrendIndex*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqMerchAuction:
		{	
			OnViewReqMerchAuction((const CMmiReqMerchAution*)pMmiCommBase, iCommunicationID);
		}	
		break;
	case ECTReqAddPushGeneralNormal:
		{
			OnViewReqAddPushGeneralNormal((const CMmiReqAddPushGeneralNormal*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqUpdatePushGeneralNormal:
		{
			OnViewReqUpdatePushGeneralNormal((const CMmiReqUpdatePushGeneralNormal*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqDelPushGeneralNormal:
		{
			OnViewReqDelPushGeneralNormal((const CMmiReqDelPushGeneralNormal*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqAddPushMerchTrendIndex:
		{
			OnViewReqAddPushMerchTrendIndex((const CMmiReqAddPushMerchTrendIndex*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqUpdatePushMerchTrendIndex:
		{
			OnViewReqUpdatePushMerchTrendIndex((const CMmiReqUpdatePushMerchTrendIndex*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqDelPushMerchTrendIndex:
		{
			OnViewReqDelPushMerchTrendIndex((const CMmiReqDelPushMerchTrendIndex*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqPlugIn:
		{
			OnViewReqPlugInData(pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqFundHold:
		{
			OnViewReqFundHold((const CMmiReqFundHold*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqMinuteBS:
		{
			OnViewReqMinuteBS((const CMmiReqMinuteBS*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqAddPushMinuteBS:
		{
			OnViewReqAddPushMinuteBS((const CMmiReqAddPushMinuteBS*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqUpdatePushMinuteBS:
		{
			OnViewReqUpdatePushMinuteBS((const CMmiReqUpdatePushMinuteBS*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqDelPushMinuteBS:
		{
			OnViewReqDelPushMinuteBS((const CMmiReqDelPushMinuteBS*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqAuthPlugInHeart:
		{
			OnReqAuthPlugInHeart((const CMmiReqAuthPlugInHeart*)pMmiCommBase, iCommunicationID);
		}
		break;
	default:
		{
			OnViewReqBase(pMmiCommBase,iCommunicationID,eDataSource);
		}
		break;
	}

}

void CViewData::OnViewReqPlugInData(IN const CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID)
{
	if ( NULL == pMmiCommBase )
	{
		ASSERT(0);
		return;
	}
	
	CMmiCommBasePlugIn* pMmiCommBasePlugIn = (CMmiCommBasePlugIn*)pMmiCommBase;

	// xl 0523 财富金盾版本 阻止部分请求的发送
	switch(pMmiCommBasePlugIn->m_eCommTypePlugIn)
	{
	case ECTPIReqMerchIndex:
		{
			// 短线中线选股标志
			OnViewReqMerchIndex((const CMmiReqMerchIndex*)pMmiCommBase);
		}
		break;
	case ECTPIReqBlock:
		{			
			// 板块列表数据
			OnViewReqBlock((const CMmiReqLogicBlock*)pMmiCommBase);
		}
		break;
	case  ECTPIReqMerchSort:
		{
			// 大单排行排序
			OnViewReqMerchSort((const CMmiReqMerchSort*)pMmiCommBase);
		}
		break;
	case  ECTPIReqPeriodMerchSort:
		{
			// 大单排行周期排序
			OnViewReqPeriodMerchSort((const CMmiReqPeriodMerchSort*)pMmiCommBase);
		}
		break;
	case  ECTPIReqBlockSort:
		{	
			// 板块列表排行
			OnViewReqBlockSort((const CMmiReqBlockSort*)pMmiCommBase);
		}	
		break;
	case  ECTPIReqMainMonitor:
		{
			// 主力监控
			OnViewReqMainMonitor((const CMmiReqMainMonitor*)pMmiCommBase);
		}
		break;
	case  ECTPIReqShortMonitor:
		{
			// 短线监控
			OnViewReqShortMonitor((const CMmiReqShortMonitor*)pMmiCommBase);
		}
		break;
	case ECTPIReqTickEx:
		{
			// 资金流向
			OnViewReqTickEx((const CMmiReqTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqPeriodTickEx:
		{
			// 多日周期分笔请求
			OnViewReqPeriodTickEx((const CMmiReqPeriodTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqHistoryTickEx:
		{			
			// 历史资金流向数据
			OnViewReqHistoryTickEx((const CMmiReqHistoryTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqChooseStock:
		{
			// 短中战略选股请求
			OnViewReqChooseStock((const CMmiReqChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqCRTEStategyChooseStock:
		{
			// 短中战略选股扩展请求
			OnViewReqCRTEStategyChooseStock((const CMmiReqCRTEStategyChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqDapanState:
		{
			// 大盘状态请求
			OnViewReqDapanState((const CMmiReqDapanState*)pMmiCommBase);
		}
		break;
	case ECTPIReqChooseStockStatus:
		{
			// 选股状态请求
			OnViewReqChooseStockStatus((const CMmiReqChooseStockStatus*)pMmiCommBase);
		}
		break;
	case ECTPIReqDKMoney:
		{
			// 多空资金请求
			OnViewReqDKMoney((const CMmiReqDKMoney*)pMmiCommBase);
		}
		break;
	case ECTPIReqMainMasukura:
		{
			// 主力增仓
			OnViewReqMainMasukura((const CMmiReqMainMasukura*)pMmiCommBase);
		}
		break;
	case ECTPIReqAddPushMerchIndex:
	case ECTPIReqAddPushMainMonitor:
	case ECTPIReqAddPushTickEx:
	case ECTPIReqAddPushMerchIndexEx3:
	case ECTPIReqAddPushMerchIndexEx5:
	case ECTPIReqAddPushMerchIndexEx10:
	case ECTPIReqAddPushShortMonitor:
	case ECTPIReqAddPushMainMasukura:
		{
			OnViewReqAddPushPlugInMerchData((const CMmiReqPushPlugInMerchData*)pMmiCommBase);
		}
		break;
	case  ECTPIReqAddPushBlock:
		{
			OnViewReqAddPushPlugInBlockData((const CMmiReqPushPlugInBlockData*)pMmiCommBase);
		}
		break;
	case  ECTPIReqAddPushChooseStock:
		{
			OnViewReqAddPushPlugInChooseStock((const CMmiReqPushPlugInChooseStock*)pMmiCommBase);
		}
		break;
	case  ECTPIReqUpdatePushMerchIndex: // 短中数据
	case  ECTPIReqUpdatePushMainMonitor:
	case  ECTPIReqUpdatePushTickEx:
		{
			OnViewReqUpdatePushPlugInMerchData((const CMmiReqUpdatePushPlugInMerchData*)pMmiCommBase);	
		}
		break;
	case ECTPIReqUpdatePushBlock:
		{
			OnViewReqUpdatePushPlugInBlockData((const CMmiReqUpdatePushPlugInBlockData*)pMmiCommBase);
		}
		break;
	case ECTPIReqUpdatePushChooseStock:
		{
			// 选股推送
			OnViewReqUpdatePushPlugInChooseStock((const CMmiReqUpdatePushPlugInChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqDeletePushMerchData:
		{
			// 短中数据
			OnViewReqRemovePushPlugInMerchData((const CMmiReqRemovePushPlugInMerchData*)pMmiCommBase);
		}
		break;
	case ECTPIReqDeletePushBlockData:
		{
			OnViewReqRemovePushPlugInBlockData((const CMmiReqRemovePushPlugInBlockData*)pMmiCommBase);
		}
		break;
	case ECTPIReqDeletePushChooseStock:
		{
			// 短中选股
			OnViewReqRemovePushPlugInChooseStock((const CMmiReqRemovePushPlugInChooseStock*)pMmiCommBase);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
}

void CViewData::OnViewReqMerchIndex(IN const CMmiReqMerchIndex* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqBlock(IN const CMmiReqLogicBlock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqMerchSort(IN const CMmiReqMerchSort* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqPeriodMerchSort( IN const CMmiReqPeriodMerchSort* pMimiComBasePlugIn )
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqBlockSort(IN const CMmiReqBlockSort* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqMainMonitor(IN const CMmiReqMainMonitor* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqShortMonitor( IN const CMmiReqShortMonitor* pMimiComBasePlugIn )
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqTickEx(IN const CMmiReqTickEx* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqPeriodTickEx( IN const CMmiReqPeriodTickEx* pMimiComBasePlugIn )
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqHistoryTickEx(IN const CMmiReqHistoryTickEx* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqChooseStock(IN const CMmiReqChooseStock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqCRTEStategyChooseStock(IN const CMmiReqCRTEStategyChooseStock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}



void CViewData::OnViewReqDapanState(IN const CMmiReqDapanState* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqChooseStockStatus(IN const CMmiReqChooseStockStatus* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqDKMoney(IN const CMmiReqDKMoney* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqMainMasukura(IN const CMmiReqMainMasukura* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqAddPushPlugInMerchData(IN const CMmiReqPushPlugInMerchData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqAddPushPlugInBlockData(IN const CMmiReqPushPlugInBlockData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqAddPushPlugInChooseStock(IN const CMmiReqPushPlugInChooseStock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqUpdatePushPlugInMerchData(IN const CMmiReqUpdatePushPlugInMerchData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqUpdatePushPlugInBlockData(IN const CMmiReqUpdatePushPlugInBlockData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqUpdatePushPlugInChooseStock(IN const CMmiReqUpdatePushPlugInChooseStock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

//
void CViewData::OnViewReqRemovePushPlugInMerchData(IN const CMmiReqRemovePushPlugInMerchData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqRemovePushPlugInBlockData(IN const CMmiReqRemovePushPlugInBlockData* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnViewReqRemovePushPlugInChooseStock(IN const CMmiReqRemovePushPlugInChooseStock* pMimiComBasePlugIn)
{
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBasePlugIn, aMmiReqNodes);
}

void CViewData::OnXTimer(int32 iTimerId)
{
	// 多线程变单线程处理
	if (KTimerIdFreeMemory == iTimerId)
	{
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerFreeMemory, 0, 0);		// 清理内存
	}
	if (KTimerIdSyncPushing == iTimerId)
	{
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerSyncPushing, 0, 0);		// 同步推送
	}
	else if (KTimerIdInitializeMarket == iTimerId)
	{	
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerInitializeMarket, 0, 0);	// 初始化市场
	}
	else if (KTimerIdUpdateServerTime == iTimerId)							
	{
		OnTimerUpdateServerTime();											// 更新服务器时间
	}
	else if (KTimerIdWaitQuoteConnet == iTimerId)
	{
		OnTimerLogWaitQuoteConnect();										// 等待行情连接
	}	
	else if ( KTimerIdWaitLogicBlockFileTraslateTime == iTimerId )			// 板块文件传输超时
	{
		OnTimerLogicBlockTraslateTimeOut();
	}
	else if (KTimerIdWaitDirectQuoteConnet == iTimerId)						// 等待直连行情服务器连接
	{
		OnTimerWaitDirectQuoteConnect();
	}
}

void CViewData::OnAuthSuccessMain(bool bOfflineLogin)
{
	if (NULL == m_LoginWnd)
	{		
		return;
	}

	if ( NULL == m_pServiceDispose )
	{
		ASSERT(0);
		return;
	}	

	m_eLoginState = ELSLogining;
	// 通知登录对话框， 当前状态
	SendMessage(m_LoginWnd,UM_ViewData_OnAuthSuccess, m_iInitializedMarketCount, m_iAllMarketCount);

	// 创建私有目录
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();

	

}

void CViewData::InitialDataServers(CProxyInfo& proxyInfo,CString& strUserName,CString& strPwd)
{
	// 优选主站
	if ( NULL == m_pServiceDispose )
	{
		ASSERT(0);
		return;
	}

	// 得到行情服务器列表
	T_ServerInfo* pServerInfo;
	int iCountServers;
	m_pServiceDispose->GetServerList(&pServerInfo, iCountServers);

	CArray<T_ServerInfo, T_ServerInfo&> aNewsServerAll;

	// 设置值
	int32 i;
	for ( i = 0; i < iCountServers; i++ )
	{
		if ( CheckFlag (pServerInfo[i].enType, enSTQuote) )
		{	
			m_aQuoteServerAll.Add(pServerInfo[i]);
		}

		if ( CheckFlag(pServerInfo[i].enType, enSTInfo) )
		{
			aNewsServerAll.Add(pServerInfo[i]);
		}
	}

	_MYTRACE(L"认证服务器返回成功, 准备测速排序, 所有行情服务器如下:");

	// 设置排序参数
	COptimizeServer::Instance()->SetSortParams((T_ServerInfo*)m_aQuoteServerAll.GetData(), m_aQuoteServerAll.GetSize());

	// 建立各个服务器的连接
	m_pCommManager->SetStage(ECSTAGE_LoginInit);
	m_pCommManager->InitAllService(proxyInfo, strUserName, strPwd);
	m_pCommManager->StartAllService();

	CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
	m_pCommManager->GetCommnunicaionList(aActiveServer);
	for ( i = 0; i < aActiveServer.GetSize(); i++ )
	{
		CAbsCommunicationShow* p = aActiveServer.GetAt(i);
		if ( NULL == p )
		{
			continue;
		}

		//
		CServerState st;
		p->GetServerState(st);

		_MYTRACE(L"%d %s %s:%d",
			m_pCommManager->GetCommunicationID(st.m_StrServerAddr, st.m_uiServerPort),
			st.m_StrServerName.GetBuffer(),
			st.m_StrServerAddr.GetBuffer(),
			st.m_uiServerPort);

		st.m_StrServerName.ReleaseBuffer();
		st.m_StrServerAddr.ReleaseBuffer();

	}

	// 等行情服务器连接, 连上以后再排序
	//pDoc->m_pDlgLogIn->SetPromptInfo(L"连接行情服务器...");
	StartTimer(KTimerIdWaitQuoteConnet, KTimerPeriodWaitQuoteConnet);

	// 得到资讯服务器列表:
	m_pNewsManager->SetStage(ENSTAGE_LoginInit);
	m_pNewsManager->InitAllNewsCommunication(proxyInfo, aNewsServerAll, strUserName, strPwd);

	if ( !m_pNewsManager->StartAllNewsCommunication() )
	{
		ASSERT(0);
		
	}

	//
	m_pServiceDispose->ReleaseData(pServerInfo);
}

void CViewData::OnFinishSortServer()
{
	if ( ELSNotLogin == m_eLoginState )
	{
		// 登录过程中, 可能出现错误, 清空登录信息. 这个时候, 不要处理
		_MYTRACE(L"收到优选完成通知, 此时是未登录状态, 可能由于服务器断线造成, 不处理这个通知.");
		return;
	}

	if ( COptimizeServer::Instance()->GetForcePingFlag() )
	{
		// 重新ping 测速的
		return;
	}

	m_bManualReconnect = false;	
	//
	/*if ( NULL != pDoc->m_pDlgLogIn )
	{
		pDoc->m_pDlgLogIn->SetPromptInfo(L"优选主站完成!"); 
	}*/
	_MYTRACE(L"优选完成, 服务器顺序如下:");

	// 排序完成 1: 重设服务器顺序 2: 发认证请求
	arrServer aServerSorted;
	COptimizeServer::Instance()->GetSortedServers(aServerSorted);

	for ( int32 ii = 0 ; ii < aServerSorted.GetSize() ; ii++ )
	{
		CServerState ss = aServerSorted[ii];			
		_MYTRACE(L" => [%d] ID: %d 地址: %s 端口: %d", ss.m_iSortIndex, m_pCommManager->GetCommunicationID(ss.m_StrServerAddr, ss.m_uiServerPort), ss.m_StrServerAddr.GetBuffer(), ss.m_uiServerPort);			
		ss.m_StrServerAddr.ReleaseBuffer();
	}

	m_pCommManager->ReSetCommunicationIndex(aServerSorted);
	
	bool32 bAllError = true;
	bool32 bAutoSort = COptimizeServer::Instance()->BeAutoSortMode();

	for ( int32 i = 0; i < aServerSorted.GetSize(); i++ )
	{
		// 第一台行情服务器发认证请求

		CServerState stServerState = aServerSorted[i];
		bool32 bFirst = m_pCommManager->BeFirstCommunication(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
		
		if ( bFirst )
		{
			int32 iCommunicationID = m_pCommManager->GetCommunicationID(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
			bool32 bSevrConnected  = m_pCommManager->IsServerConneted(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
			
			//
			if ( !bSevrConnected )
			{			
				if ( bAutoSort )
				{
					_MYTRACE(L"第一台服务器连接异常, 取下一台服务器开始认证");
					continue;				
				}
				else
				{
					// 手选服务器，结束认证请求的发送
					break;
				}
			}
			
			//
			T_UserInfo stUserInfo;
			if ( NULL != m_pServiceDispose )
			{
				bAllError = false;

				m_pServiceDispose->GetUserInfo(stUserInfo);
				
				// 发送认证请求:
				CMmiReqAuth ReqAuth;
				PackQuoteAuthJson(ReqAuth.m_StrReqJson);
				
				RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);	

				_MYTRACE(L"排序结束, 取第一台服务器发送认证请求: UserID = %d Token = %s Product = %s UserName = %s",
					stUserInfo.iUserId, stUserInfo.wszToken, m_strProduct.GetBuffer(), stUserInfo.wszUserName);
				m_strProduct.ReleaseBuffer();
			}
			else
			{
				ASSERT(0);
				continue;
			}

			// 记录下登录信息, 下次连不上的时候用
			CArray<int32, int32> aServiceIDs;
			m_pCommManager->GetServerServiceList(stServerState.m_StrServerAddr, stServerState.m_uiServerPort, aServiceIDs);

			T_LoginBackUp stBackUp;
			stBackUp.m_StrQuoterSvrAdd = stServerState.m_StrServerAddr;
			stBackUp.m_iQuoterSvrPort = stServerState.m_uiServerPort;
			
			stBackUp.m_uiUserID		= stUserInfo.iUserId;
			stBackUp.m_iServiceCount= aServiceIDs.GetSize();

			stBackUp.m_iSize = sizeof(stBackUp) - sizeof(stBackUp.m_pServices) + stBackUp.m_iServiceCount * sizeof(int32);

			// 写文件记录
			CString strPath = CPathFactory::GetLoginBackUpPath(m_strUserName);
			if (!strPath.IsEmpty())
			{				
				std::string strPathA;
				Unicode2MultiChar(CP_ACP, strPath, strPathA);
				FILE* pFile = fopen(strPathA.c_str(), "wb+");
				if (NULL != pFile)
				{
					int32 iRet = fwrite(&stBackUp, sizeof(stBackUp) - sizeof(stBackUp.m_pServices), 1, pFile);
					if (iRet)
					{
						// ....
					}
					
					//
					for (int32 j = 0; j < aServiceIDs.GetSize(); j++)
					{
						int32 iID = aServiceIDs[j];
						fwrite(&iID, sizeof(int32), 1, pFile);
					}

					//
					fclose(pFile);
				}
			}
		}
	}

	// 如果判断模式的话，则面临手选连接不上，则后面的不再判断
	// 不判断模式，则可能不能连接到用户指定的服务器
	if ( bAllError && bAutoSort )
	{
		// 自动优选尝试进行剩下的连接的行情服务器的认证发送
		for ( int32 i = 0; i < aServerSorted.GetSize(); i++ )
		{
			CServerState stServerState = aServerSorted[i];
		
			bool32 bFirst = m_pCommManager->BeFirstCommunication(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
			if ( !bFirst )
			{
				int32 iCommunicationID = m_pCommManager->GetCommunicationID(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
				bool32 bSevrConnected  = m_pCommManager->IsServerConneted(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
				
				//
				if ( !bSevrConnected )
				{
					_MYTRACE(L"忽略不能连接的行情服务器: %d,%s", iCommunicationID, stServerState.m_StrServerAddr.GetBuffer());
					stServerState.m_StrServerAddr.ReleaseBuffer();
					continue;
				}
				
				//
				T_UserInfo stUserInfo;
				if ( NULL != m_pServiceDispose )
				{
					bAllError = false;
					
					m_pServiceDispose->GetUserInfo(stUserInfo);
					
					// 发送认证请求:
					CMmiReqAuth ReqAuth;
					PackQuoteAuthJson(ReqAuth.m_StrReqJson);
					
					RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);	
					
					_MYTRACE(L"排序结束, 取服务器(%d)发送认证请求: UserID = %d Token = %s Product = %s UserName = %s",
						iCommunicationID, stUserInfo.iUserId, stUserInfo.wszToken, m_strProduct.GetBuffer(), stUserInfo.wszUserName);
					m_strProduct.ReleaseBuffer();
					
					/*if ( NULL != pDoc->m_pDlgLogIn )
					{
						pDoc->m_pDlgLogIn->SetPromptInfo(L"最优行情主站开始认证..."); 					
					}*/
				}
				else
				{
					ASSERT(0);
					continue;
				}
			}
		}
	}

	//
	if ( bAllError )
	{	
		//		
		CString *pStrTip = NULL;
		if ( bAutoSort )
		{
			_MYTRACE(L"没有一台服务器连接正常, 无法发送认证请求, 提示稍后登录");
			pStrTip = new CString(L"没有一台服务器连接正常, 网络状况异常, 请稍候登录...");
		}
		else
		{
			_MYTRACE(L"首选服务器连接不正常, 无法发送认证请求, 提示更换首选");
			pStrTip = new CString(L"首选服务器连接错误, 请选择其它服务器!");
		}
		PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);
	}
	else
	{
		m_bLogWaitQuoteSortFinish = false;	// 终于完成优选的排序了
	}

}

void CViewData::OnFinishAllPing()
{
	if ( COptimizeServer::Instance()->GetForcePingFlag() )
	{
		// 重新ping 测速的
		return;
	}

	// ping 完成: 停掉其他不需要的行情服务器
	m_pCommManager->StopServiceAfterSortServer();	
}

bool32 CViewData::IsSomeIoViewAttendTheMerch(IN CMerch *pMerch, int32 iDataSeriveTypes)
{
	// 先看看商品是否为标题栏显示商品
	int i;
	for ( i = 0; i < m_StatucBarAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &AttendMerch = m_StatucBarAttendMerchs[i];
		if (AttendMerch.m_pMerch == pMerch && (AttendMerch.m_iDataServiceTypes & iDataSeriveTypes) != 0)
		{
			return TRUE;
		}
	}
	
	// 是不是条件选股的商品数据
	for ( i = 0; i < m_aIndexChsDataAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch &AttendMerch = m_aIndexChsDataAttendMerchs[i];
		if ( AttendMerch.m_pMerch == pMerch )
		{
			return TRUE;
		}
	}
	
	// 是不是要下载数据库的数据
	for ( i = 0; i < m_aRecordDataCenterAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch &AttendMerch = m_aRecordDataCenterAttendMerchs[i];
		if (AttendMerch.m_pMerch == pMerch )
		{
			return TRUE;
		}
	}

	// 再检查各个业务视图是否有关心的
	CIoViewListner *pIoView = NULL;
	for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
	{
		pIoView = m_IoViewList[iIndexIoView];
		if (NULL == pIoView)
		{
			continue;
		}

		SmartAttendMerchArray pArray;
		pIoView->GetSmartAttendArray(pArray);
		for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < pArray.GetSize(); iIndexAttendMerch++)
		{
			CSmartAttendMerch &SmartAttendMerch = pArray.GetAt(iIndexAttendMerch);
			if (SmartAttendMerch.m_pMerch == pMerch && (SmartAttendMerch.m_iDataServiceTypes & iDataSeriveTypes) != 0)
			{
				return TRUE;
			}
		}
	}
	

	return FALSE;
}

// 定时清理内存
// 挨个商品扫描， 看是否有相关联的视图
void CViewData::OnTimerFreeMemory(bool32 bForceFree)
{
	//
	CArray<CPushMerchKey, CPushMerchKey&> aCancelMerchs;

	// 
	
	// 所有的商品里面的K线和Tick线不需要再显示的， 就先清掉内存空间
	CBreed *pBreed = NULL;
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		if (NULL == pBreed)
		{
			continue;
		}

		CMarket *pMarket = NULL;
		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (NULL == pMarket)
			{
				continue;
			}

			CMerch *pMerch = NULL;
			for (int32 iIndexMerch = 0; iIndexMerch < pMarket->m_MerchsPtr.GetSize(); iIndexMerch++)
			{
				pMerch = pMarket->m_MerchsPtr[iIndexMerch];
				if (NULL == pMerch)
					continue;
				
				// 检查是否有K线数据
				bool32 bHasHistroyData = pMerch->DoesHaveHistoryData();
				if (bHasHistroyData)	// 检查是否有哪个视图关心该商品的历史数据
				{					
					if ( bForceFree || !IsSomeIoViewAttendTheMerch(pMerch, EDSTKLine | EDSTTimeSale))
					{
						// 没有任何视图表示自己与该商品的历史数据有关系， 删除该商品的历史数据
						/*
						TRACE(L"定时器清理 %d %s 的内存\n", 
							pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCnName);

						int32 iByteKLine = 0;
						for (int32 i = 0; i < pMerch->m_MerchKLineNodesPtr.GetSize(); i++)
						{
							int32 iByte = pMerch->m_MerchKLineNodesPtr[i]->m_KLines.GetSize() * sizeof(CKLine);
							TRACE(L"  %d K 线 %d 字节\n", pMerch->m_MerchKLineNodesPtr[i]->m_eKLineTypeBase, iByte);

							iByteKLine += iByte;
						}

						int32 iByteTick = 0;
						if ( NULL != pMerch->m_pMerchTimeSales) 
						{
							iBythneTick = pMerch->m_pMerchTimeSales->m_Ticks.GetSize() * sizeof(CTick);
							TRACE(L"  Tick %d 字节\n", iByteTick);
						}
						
						int32 iTotal = iByteKLine + iByteTick;
						float fK = iTotal / 1024.0f;
						float fM = fK / 1024.0f;
						TRACE(L"共清理内存: %d + %d = %d 字节(%.2f K %.2f M)\n", iByteKLine, iByteTick, iTotal, fK, fM);
						*/		
						pMerch->FreeAllHistoryData();
						m_OfflineDataManager.RemoveOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTVipData);
					}
				}
			}
		}
	}
}

void CViewData::OnTimerSyncPushing()
{
 	CArray<CMerch*, CMerch*> aPushingRealtimePrices;
	CArray<CMerch*, CMerch*> aPushingRealtimeTicks;
	CArray<CMerch*, CMerch*> aPushingRealtimeLevel2s;

	// 先看看商品是否为标题栏显示商品
	for (int i = 0; i < m_StatucBarAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &AttendMerch = m_StatucBarAttendMerchs[i];
		if ((AttendMerch.m_iDataServiceTypes & EDSTPrice) != 0)
		{
			aPushingRealtimePrices.Add(AttendMerch.m_pMerch);
		}

		if ((AttendMerch.m_iDataServiceTypes & EDSTTick) != 0)
		{
			aPushingRealtimeTicks.Add(AttendMerch.m_pMerch);
		}

		if ((AttendMerch.m_iDataServiceTypes & EDSTLevel2) != 0)
		{
			aPushingRealtimeLevel2s.Add(AttendMerch.m_pMerch);
		}
	}

	// 状态栏多空阵线数据 0, 1000
	{
		CMmiReqAddPushGeneralNormal reqGPush;
		reqGPush.m_iMarketId = 0;
		RequestViewData(&reqGPush);
		reqGPush.m_iMarketId = 1000;
		RequestViewData(&reqGPush);
	}

	// 是否有预警商品 && 打开了预警
	for ( int32 i = 0; i < m_aAlarmAttendMerchs.GetSize(); i++ )
	{

		CMerch* pMerch = m_aAlarmAttendMerchs[i].m_pMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
		aPushingRealtimePrices.Add(pMerch);
	}


	// 数据库
	for ( int32 i = 0; i < m_aRecordDataCenterAttendMerchs.GetSize(); i++ )
	{
		CMerch* pMerch = m_aRecordDataCenterAttendMerchs[i].m_pMerch;
		if ( NULL == pMerch )
		{
			continue;
		}

		//
		aPushingRealtimePrices.Add(pMerch);
		aPushingRealtimeTicks.Add(pMerch);
	}
	

	// 套利商品 - 转移至套利报价表
	for ( int32 i = 0; i < m_aArbitrageAttendMerchs.GetSize(); i++ )
	{

		CMerch* pMerch = m_aArbitrageAttendMerchs[i].m_pMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
		aPushingRealtimePrices.Add(pMerch);
	}
	
	
	// 再检查各个业务视图是否有关心的
	CIoViewListner *pIoView = NULL;
	for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
	{
		pIoView = m_IoViewList[iIndexIoView];
		if (NULL == pIoView) continue;

		const DWORD dwNeedPushType = pIoView->GetNeedPushDataType();
		
		if ( (dwNeedPushType&(EDSTPrice |EDSTTick |EDSTLevel2)) == 0 )
		{
			continue;	// 不需要这3中类型的任何一种推送
		}

		if ( !pIoView->IsNowCanReqData() )
		{
			continue;	// 此时该视图不方便请求数据
		}
		
		SmartAttendMerchArray pArray;
		pIoView->GetSmartAttendArray(pArray);
		for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < pArray.GetSize(); iIndexAttendMerch++)
		{
			CSmartAttendMerch &SmartAttendMerch = pArray.GetAt(iIndexAttendMerch);
			if ((SmartAttendMerch.m_iDataServiceTypes & EDSTPrice) != 0
				&& (dwNeedPushType & EDSTPrice) != 0 )				// xl 11/01/20  增加是否需要该推送的判断
			{
				aPushingRealtimePrices.Add(SmartAttendMerch.m_pMerch);
			}
			
			if ((SmartAttendMerch.m_iDataServiceTypes & EDSTTick) != 0
				&& (dwNeedPushType & EDSTTick) != 0 )
			{
				aPushingRealtimeTicks.Add(SmartAttendMerch.m_pMerch);
			}
			
			if ((SmartAttendMerch.m_iDataServiceTypes & EDSTLevel2) != 0
				&& (dwNeedPushType & EDSTLevel2) != 0 )
			{
				aPushingRealtimeLevel2s.Add(SmartAttendMerch.m_pMerch);
			}
		}
		
	}

	// 模拟交易商品注册推送
	for (int i = 0; i < m_aSimulateTradeAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &AttendMerch = m_aSimulateTradeAttendMerchs[i];
		aPushingRealtimePrices.Add(AttendMerch.m_pMerch);
	}
	
	// 向服务器同步推送买卖盘
	if (aPushingRealtimePrices.GetSize() > 0)
	{
		//
		CMmiRegisterPushPrice Req;
		Req.m_iMarketId		= aPushingRealtimePrices[0]->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode	= aPushingRealtimePrices[0]->m_MerchInfo.m_StrMerchCode;
		
		Req.m_aMerchMore.RemoveAll();
		{
			aPushingRealtimePrices.RemoveAt(0, 1);
			for (int32 i = 0; i < aPushingRealtimePrices.GetSize(); i++)
			{
				CMerch *pMerch = aPushingRealtimePrices[i];

				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}
		}
		
		// 向服务器发请求
		RequestViewData((CMmiCommBase *)&Req);
	}

	// 向服务器同步推送分笔
	if (aPushingRealtimeTicks.GetSize() > 0)
	{
		// 
		CMmiRegisterPushTick Req;
		Req.m_iMarketId		= aPushingRealtimeTicks[0]->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode	= aPushingRealtimeTicks[0]->m_MerchInfo.m_StrMerchCode;
		
		Req.m_aMerchMore.RemoveAll();
		{
			aPushingRealtimeTicks.RemoveAt(0, 1);
			for (int32 i = 0; i < aPushingRealtimeTicks.GetSize(); i++)
			{
				CMerch *pMerch = aPushingRealtimeTicks[i];
				
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}
		}
		
		// 向服务器发请求
		RequestViewData((CMmiCommBase *)&Req);
	}

	// 向服务器同步推送Level2
	if (aPushingRealtimeLevel2s.GetSize() > 0)
	{
		// 
		CMmiRegisterPushLevel2 Req;
		Req.m_iMarketId		= aPushingRealtimeLevel2s[0]->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode	= aPushingRealtimeLevel2s[0]->m_MerchInfo.m_StrMerchCode;
		
		Req.m_aMerchMore.RemoveAll();
		{
			aPushingRealtimeLevel2s.RemoveAt(0, 1);
			for (int32 i = 0; i < aPushingRealtimeLevel2s.GetSize(); i++)
			{
				CMerch *pMerch = aPushingRealtimeLevel2s[i];
				
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}
		}
		
		// 向服务器发请求
		RequestViewData((CMmiCommBase *)&Req);
	}
}

void CViewData::OnTimerInitializeMarket()
{
	if (!m_MerchManager.IsInitializedAll())	// 尚未做过一次完整的初始化的时候， 不需要处理
		return;


	// 所有的市场轮询， 是否需要初始化
	CBreed *pBreed = NULL;
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		if (NULL == pBreed)
			continue;

		CMarket *pMarket = NULL;
		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (NULL == pMarket)
				continue;

			CGmtTime TimeServer = GetServerTime();
			if (pMarket->DoesNeedInitialize(TimeServer))
			{
				// 初始化该市场
				PutIntoMarketReqList(pMarket);
				// InitializeMarket(*pMarket);
			}
		}
	}

	// 重新初始化
	ReqMarketInfo();
}

void CViewData::OnTimerUpdateServerTime()
{
	uint32 uiTimeNow = timeGetTime();
	
	uint32 uiAdd = (uiTimeNow - m_uiTimeLastUpdate) / 1000;
	if (uiAdd > 0)
	{
		CGmtTimeSpan TimeSpan(0, 0, 0, uiAdd);
		m_TimeServerNow += TimeSpan;

		// 
		m_uiTimeLastUpdate += uiAdd * 1000;
	}
}

void CViewData::OnTimerLogWaitQuoteConnect()
{
	++m_iWaitQuoteConnectCounts;
	
	//
	if ( (m_iWaitQuoteConnectCounts >= KiCountsWaitQuoteConnet) )
	{
		// 超过60 秒
		bool32 bAllNonConnect = CheckLogQuoteConnect(-1);

		OnFinishLogWaitQuoteConnect(bAllNonConnect);		
	}	
}


void CViewData::OnTimerWaitDirectQuoteConnect()
{
	++m_iWaitDirectQuoteConnectCounts;
	
	//
	if ( m_iWaitDirectQuoteConnectCounts >= KiCountsWaitQuoteConnet )
	{
		// 超过60 秒, 提示失败
		StopTimer(KTimerIdWaitDirectQuoteConnet);	
		m_iWaitDirectQuoteConnectCounts = 0;
		
		//
		// 在另外一处自己释放，在函数内屏蔽429错误
		//lint --e{593} 
		CString *pStrTip = new CString(L"直连行情服务器超时,请检查网络,稍候登录...");
		if (NULL != m_LoginWnd)
		{
			PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);			
		}		
	}
}

void CViewData::OnDataServiceConnected(int32 iServiceId)
{
	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServiceConnected, (WPARAM)iServiceId, 0);
}

void CViewData::OnMsgServiceConnected(int32 iServiceId)
{
	if (m_bDirectToQuote)
	{
		// 查找所有的尚未初始化的市场， 如果该服务器可以提供该市场的初始化数据
		for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
		{
			CBreed *pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
			for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
			{
				CMarket *pMarket = pBreed->m_MarketListPtr[iIndexMarket];
				// if (m_pServiceDispose->IsAttendService(pMarket->m_MarketInfo.m_iMarketId, EDSTGeneral, iServiceId))
				{
					if (!pMarket->IsInitialized())
					{
						if (!pMarket->m_MarketInfoReqState.m_bRequesting && !pMarket->m_MerchListReqState.m_bRequesting)
						{
							// 还没有任何初始化， 咱给他初始化一个先 :)
							// InitializeMarket(*pMarket);
							PutIntoMarketReqList(pMarket);
						}
						else
						{
							// 已经有其他的服务器在初始化了， 这里不需要处理
							// NULL;
						}
					}
					else
					{
						// 已经初始化成功了， 这里就不再处理了
						// NULL;
					}
				}				
			}
		}
		
		//
		ReqMarketInfo();
		
		return;
	}
	
	// 正常流程
	if ( m_bLogWaitQuoteSortFinish )
	{
		// 正在等待优选完成时，不需要处理service
		_MYTRACE(L"等待优选时, 服务%d 重连, 过滤", iServiceId);
		return;
	}

	// 查找所有的尚未初始化的市场， 如果该服务器可以提供该市场的初始化数据
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		CBreed *pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			CMarket *pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (!pMarket->IsInitialized())
			{
				if (!pMarket->m_MarketInfoReqState.m_bRequesting && !pMarket->m_MerchListReqState.m_bRequesting)
				{
					// 还没有任何初始化， 咱给他初始化一个先 :)
					// InitializeMarket(*pMarket);
					PutIntoMarketReqList(pMarket);
				}
				else
				{
					// 已经有其他的服务器在初始化了， 这里不需要处理
					// NULL;
				}
			}
			else
			{
				// 已经初始化成功了， 这里就不再处理了
				// NULL;
			}
		}
	}
	
	//
	ReqMarketInfo();

	// 只要任何一个视图下任何一个关注的商品与该服务器有关联， 重新请求该视图有的数据
	CIoViewListner *pIoView = NULL;
	for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
	{
		pIoView = m_IoViewList[iIndexIoView];
		if (NULL == pIoView)
			continue;

		// 查找该视图是否有什么业务与该服务器有关
		bool32 bAttendView = false;
		
		SmartAttendMerchArray pArray;
		pIoView->GetSmartAttendArray(pArray);
		for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < pArray.GetSize(); iIndexAttendMerch++)
		{
			CSmartAttendMerch &SmartAttendMerch = pArray.GetAt(iIndexAttendMerch);

			_AttendMerch AttendMerch;
			AttendMerch.iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			lstrcpy(AttendMerch.wszMerchCode, SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode);
			AttendMerch.iDataServiceTypes = SmartAttendMerch.m_iDataServiceTypes;
		//	if (m_pServiceDispose->IsAttendService(AttendMerch.iMarketId, (E_DataServiceType)AttendMerch.iDataServiceTypes, iServiceId))
			{
				// 该商品需要的数据与该服务器有关联
				bAttendView = true;
				break;
			}
		}

	
		if ( !bAttendView )
		{
			// 调用新的接口查看是否有关心该市场数据
			AttendMarketDataArray aAttendData;
			pIoView->GetAttendMarketDataArray(aAttendData);
			for ( int32 i = 0; i < aAttendData.GetSize() ; i++ )
			{
				//const T_AttendMarketData &data = aAttendData[i];
		//		if ( m_pServiceDispose->IsAttendService(data.m_iMarketId, (E_DataServiceType)data.m_iEDSTypes, iServiceId) )
				{
					bAttendView = true;
					break;
				}
			}
		}
		
		// 强制更新该页面数据
		if (bAttendView)
		{
			pIoView->ForceUpdateVisibleIoView();
		}
	}

	
	// 看状态栏需要显示的几个商品是否受影响
	CArray<CMerch*, CMerch*> aMerchs;
	for (int32 i = 0; i < m_StatucBarAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &SmartAttendMerch = m_StatucBarAttendMerchs[i];

		_AttendMerch AttendMerch;
		lstrcpy(AttendMerch.wszMerchCode, SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode);
		AttendMerch.iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
		AttendMerch.iDataServiceTypes = SmartAttendMerch.m_iDataServiceTypes;
		
	//	if (m_pServiceDispose->IsAttendService(AttendMerch.iMarketId, (E_DataServiceType)AttendMerch.iDataServiceTypes, iServiceId))
		{
			aMerchs.Add(SmartAttendMerch.m_pMerch);
		}
	}
	
	if (aMerchs.GetSize() > 0)
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId		= aMerchs[0]->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode	= aMerchs[0]->m_MerchInfo.m_StrMerchCode;
		
		Req.m_aMerchMore.RemoveAll();
		{
			aMerchs.RemoveAt(0, 1);
			for (int32 i = 0; i < aMerchs.GetSize(); i++)
			{
				CMerch *pMerch = aMerchs[i];
				
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}
		}

		RequestViewData((CMmiCommBase *)&Req);
	}

	// 重新注册推送
	if (m_bServerDisConnect)
	{
		OnTimerSyncPushing();
		m_bServerDisConnect = false;
	}
}

void CViewData::OnDataServiceDisconnected(int32 iServiceId)
{
	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServiceDisconnected, (WPARAM)iServiceId, 0);
}

void CViewData::OnMsgServiceDisconnected(int32 iServiceId)
{	
	if (!m_MerchManager.IsInitializedAll())	// 市场尚未初始化时， 不允许处理任何交互
		return;
}

void CViewData::OnDataServerConnected(int32 iCommunicationId)
{
	// 多线程变单线程处理
	_MYTRACE(L"ViewData 收到底层服务器连接上通知: OnDataServerConnected: %d, m_OwnerWnd = %d, 发消息给所属窗口", iCommunicationId, m_OwnerWnd);
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServerConnected, (WPARAM)iCommunicationId, 0);
}

void CViewData::OnMsgServerConnected(int32 iCommunicationId)
{
	
	if (m_bDirectToQuote)
	{
		//
		StopTimer(KTimerIdWaitDirectQuoteConnet);
		
		CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
		m_pCommManager->GetCommnunicaionList(aActiveServer);
		
		ASSERT(aActiveServer.GetSize() == 1);
		CAbsCommunicationShow* pComm = aActiveServer.GetAt(0);
		if (NULL == pComm)
		{
			ASSERT(0);;
			return;
		}
		
		//
		CServerState stServer;
		pComm->GetServerState(stServer);
		
		T_LoginBackUp stLoginBk;
		
		CString strPath = CPathFactory::GetLoginBackUpPath(m_strUserName);
		if (strPath.IsEmpty())
		{
			return;
		}
		
	
	
		std::string strPathA;
		Unicode2MultiChar(CP_ACP, strPath, strPathA);
		FILE* pFile = fopen(strPathA.c_str(), "rb+");
		if (NULL == pFile)
		{
			return;
		}
		
		int32 iSize = fread(&stLoginBk, sizeof(stLoginBk) - sizeof(stLoginBk.m_pServices), 1, pFile);
		if (iSize <= 0)
		{
			return;
		}
		
		
		int32 iCommunicationID = m_pCommManager->GetCommunicationID(stLoginBk.m_StrQuoterSvrAdd, stLoginBk.m_iQuoterSvrPort);
		bool32 bSevrConnected  = m_pCommManager->IsServerConneted(stLoginBk.m_StrQuoterSvrAdd, stLoginBk.m_iQuoterSvrPort);
		
		//
		if ( !bSevrConnected )
		{			
			ASSERT(0);;
			return;					
		}
		
		//
		T_UserInfo stUserInfo;
		
		// 发送认证请求:
		CMmiReqAuth ReqAuth;
		PackQuoteAuthJson(ReqAuth.m_StrReqJson);
		
		RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);		
		
		//************************************* by hx
		/*if (NULL != pDoc->m_pDlgLogIn && NULL != pDoc->m_pDlgLogIn->GetSafeHwnd())
		{
			CString StrPrompt;
			StrPrompt.Format(L"连接服务器成功, 开始请求市场信息...");
			pDoc->m_pDlgLogIn->SetPromptInfo(StrPrompt);		
			pDoc->m_pDlgLogIn->SetPromptInfo2(L"直连行情服务器成功!");		
		}*/
		//*************************************

		m_eLoginState = ELSLogining;
	}
	else
	{	
		if ( ELSAuthFail >= m_eLoginState )
		{
			// 非认证后的为不需要处理的通知
			ASSERT( 0 );
			return;
		}
		

		//
		if ( m_bLogWaitQuoteConnect || m_bLogWaitQuoteSortFinish )
		{
			// 登录时
			if ( m_bLogWaitQuoteConnect )
			{
				// 等待连接时
				_MYTRACE(L"ViewData 正式处理服务器 %d 连接成功消息!", iCommunicationId);
				CheckLogQuoteConnect(iCommunicationId);
			}
			
			// 等待优选完成过程中的重连不考虑
		}	
		else 
		{
			if ( m_bManualReconnect )
			{
				// 手动断线重连的时候, 不要重发认证:
				// NULL;
			}
			else
			{
				// 非登录时, 立刻发认证请求
				T_UserInfo stUserInfo;
				m_pServiceDispose->GetUserInfo(stUserInfo);
				
				// 发送认证请求:
				CMmiReqAuth ReqAuth;
				PackQuoteAuthJson(ReqAuth.m_StrReqJson);
				
				RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationId);
				
				CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
				m_pCommManager->GetCommnunicaionList(aActiveServer);
				
				//
				_MYTRACE(L"服务器 %d 重连上, 重发认证请求: UserID = %d Token = %s Product = %s UserName = %s",
					iCommunicationId,
					stUserInfo.iUserId, stUserInfo.wszToken, m_strProduct.GetBuffer(), stUserInfo.wszUserName);
				m_strProduct.ReleaseBuffer();
				
				OnTimerFreeMemory(true);		
				_tremove_dir(_T("vipdata"));

				CIoViewListner *pIoView = NULL;
				for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
				{
					pIoView = m_IoViewList[iIndexIoView];
					if (NULL == pIoView)
						continue;

					pIoView->ForceUpdateVisibleIoView();
				}

				// 立即注册推送
				OnTimerSyncPushing();
				return;
			}
		}
	}
	
	// 服务器重连, 为了保证数据完全匹配, 情况本地缓存和vipdata 目录
	{
		OnTimerFreeMemory(true);		
		_tremove_dir(_T("vipdata"));
	}
}

void CViewData::OnDataServerDisconnected(int32 iCommunicationId)
{
	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServerDisconnected, (WPARAM)iCommunicationId, 0);
}

void CViewData::OnMsgServerDisconnected(int32 iCommunicationId)
{
	CString StrLog;
	StrLog.Format(_T("服务器 %d 断开连接"), iCommunicationId);
	
	
	
	
	{
		CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
		m_pCommManager->GetCommnunicaionList(aActiveServer);
		
		for ( int i=0; i < aActiveServer.GetSize() ; i++ )
		{
			CServerState sState;
			aActiveServer[i]->GetServerState(sState);
			int iID = m_pCommManager->GetCommunicationID(sState.m_StrServerAddr, sState.m_uiServerPort);
			if ( iID == iCommunicationId )
			{
				StrLog.Format(_T("服务器 %s(%s:%d) 断开连接"), sState.m_StrServerName.GetBuffer()
					, sState.m_StrServerAddr.GetBuffer(), sState.m_uiServerPort);
			}
		}
	}

	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(StrLog, TRUE);
	if ( m_bLogWaitQuoteSortFinish )
	{
		// 等待优选完成的过程中，通知优选该服务器断线了
		COptimizeServer::Instance()->OnDataServerDisconnected(iCommunicationId);
	}

	if ( ELSLogining == m_eLoginState )
	{		
		ReLogInOnServerDisconnected(iCommunicationId);
		return;
	}

	//
	if (!m_MerchManager.IsInitializedAll())	// 市场尚未初始化时， 不允许处理任何交互
		return;

	// 掉线做个标记, 重连上的时候, 要重新同步一次推送
	m_bServerDisConnect = true;

	// 查看是否请求市场信息的服务器出了问题
	CBreed *pBreed = NULL;
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		if (NULL == pBreed)
		{
			continue;
		}

		CMarket *pMarket = NULL;
		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (NULL == pMarket)
			{
				continue;
			}

			//
			if (pMarket->m_MarketInfoReqState.m_bRequesting && iCommunicationId == pMarket->m_MarketInfoReqState.m_iCommunicationId)
			{
				pMarket->m_MarketInfoReqState.m_bRequesting			= false;
				pMarket->m_MarketInfoReqState.m_iCommunicationId	= -1;
			}

			//
			if (pMarket->m_MerchListReqState.m_bRequesting && iCommunicationId == pMarket->m_MerchListReqState.m_iCommunicationId)
			{
				pMarket->m_MerchListReqState.m_bRequesting			= false;
				pMarket->m_MerchListReqState.m_iCommunicationId		= -1;
			}
		}
	}

	// 检查看哪些实时数据推送受到影响
	{
		for (std::map<CMerch*, T_PushServerAndTime>::iterator it = m_aPushingRealtimePrices.begin(); it != m_aPushingRealtimePrices.end(); )
		{
			T_PushServerAndTime *pPushServerAndTime = &it->second;
			if (pPushServerAndTime->m_iCommunicationId == iCommunicationId)
			{
				m_aPushingRealtimePrices.erase(it++);
			}
			else
			{
				it++;
			}
		}
	}

	{
		for (std::map<CMerch*, T_PushServerAndTime>::iterator it = m_aPushingRealtimeTicks.begin(); it != m_aPushingRealtimeTicks.end(); )
		{
			T_PushServerAndTime *pPushServerAndTime = &it->second;
			if (pPushServerAndTime->m_iCommunicationId == iCommunicationId)
			{
				m_aPushingRealtimeTicks.erase(it++);
			}
			else
			{
				it++;
			}
		}
	}

	{
		for (std::map<CMerch*, T_PushServerAndTime>::iterator it = m_aPushingRealtimeLevel2s.begin(); it != m_aPushingRealtimeLevel2s.end(); )
		{
			T_PushServerAndTime *pPushServerAndTime = &it->second;
			if (pPushServerAndTime->m_iCommunicationId == iCommunicationId)
			{
				m_aPushingRealtimeLevel2s.erase(it++);
			}
			else
			{
				it++;
			}
		}
	}
}

void CViewData::ReLogInOnServerDisconnected(int32 iCommunicationId)
{
	_MYTRACE(L"登录过程中, 服务器%d 掉线", iCommunicationId);
	
	


	if ( ELSLogining != m_eLoginState )
	{
		ASSERT(0);
		return;
	}

	// 正在登录的时候, 提供市场信息的服务器断了
	CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
	m_pCommManager->GetCommnunicaionList(aActiveServer);
	
	CServerState sState;
	bool32 bFirstServer = false;
	for ( int32 i = 0; i < aActiveServer.GetSize() ; i++ )
	{		
		aActiveServer[i]->GetServerState(sState);
		int iID = m_pCommManager->GetCommunicationID(sState.m_StrServerAddr, sState.m_uiServerPort);
		if ( iID == iCommunicationId )
		{
			if ( m_pCommManager->BeFirstCommunication(sState.m_StrServerAddr, sState.m_uiServerPort) )
			{
				bFirstServer = true;
				break;
			}
		}
	}
	
	//
	if ( !bFirstServer )
	{
		_MYTRACE(L"不是正在请求市场信息的服务器. 不用管");
		return;
	}

	if ( m_bLogWaitQuoteSortFinish )
	{
		_MYTRACE(L"优选过程中首选连接服务器掉线, 等待后续处理");
		return;
	}

	//
	_MYTRACE(L"登录过程中, 服务器%d 掉线, 提示网络异常, 重新登录1", iCommunicationId);
	
	// 改成慢速登录的方式
	m_bFastLogIn = false;

	//
	ReSetLogData();
	
	// 记录这个错误的服务器
	COptimizeServer::Instance()->SaveErrServer(sState);
	
	//
	CString *pStrTip = new CString(L"网络状况异常, 请稍候登录...");
	PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);
}

void CViewData::OnMsgServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType)
{
	if ( m_bKickOut )
	{
		return;
	}

	if ( NULL != m_LoginWnd )
	{
		if ( eType != ERCTPackTimeOut )
		{
			// 登陆前的 - 直接重连
			_MYTRACE(L"+++V [%d] CViewData 处理通讯号(登录前): %d 上超时, 重连\n", eType, iCommunicationId);
			
			// 清除队列
			m_pDataManager->RemoveHistoryReqOfSpecifyServer(iCommunicationId);
			
			// 重连一下
			m_pCommManager->StartService(iCommunicationId);
			
			// 
			
			// 如果是登录的时候, 重发请求
			if ( ELSLogining == m_eLoginState )
			{
				ReLogInOnServerDisconnected(iCommunicationId);
			}
		}		
	}
	else
	{
		// 登录后的
		if ( m_bManualReconnect )
		{
			// 手动重连的时候, 就不要自动重连了
			return;
		}
		
		if ( eType == ERCTPackTimeOut )
		{

			PostMessage(m_MainFrameWnd,UM_Package_TimeOut,0,0);
		}
		else
		{
			PostMessage(m_MainFrameWnd,UM_Package_LongTime,0,0);

			_MYTRACE(L"+++V [%d] CViewData 处理通讯号: %d 上超时, 重连\n", eType, iCommunicationId);
			
			// 清除队列
			m_pDataManager->RemoveHistoryReqOfSpecifyServer(iCommunicationId);
			
			// 重连一下
			m_pCommManager->StartService(iCommunicationId);
					
			
			// 如果是登录的时候, 重发请求
			if ( ELSLogining == m_eLoginState )
			{
				ReLogInOnServerDisconnected(iCommunicationId);
			}
		}
	}

}

void CViewData::OnDataSyncServerTime(CGmtTime &TimeServerNow)
{
	m_TimeServerNow = TimeServerNow;
	m_uiTimeLastUpdate = timeGetTime();
}

void CViewData::OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType)
{
	// 线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServerLongTimeNoRecvData, (WPARAM)iCommunicationId, (LPARAM)eType);
}

void CViewData::OnDataRequestTimeOut(int32 iMmiReqId)
{
	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataRequestTimeOut, (WPARAM)iMmiReqId, 0);
}

void CViewData::OnMsgRequestTimeOut(int32 iMmiReqId)
{
	// NULL;
}

void CViewData::OnDataCommResponse()
{
#ifdef TRACE_DLG
	if (NULL != g_hwndTrace)
	{
		CString *pStrTrace = new CString;
		*pStrTrace = L"收到dataengine通知， 有新的包到达!";
		::PostMessage(g_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataCommResponse, 0, 0);
}

void CViewData::OnMsgCommResponse()
{	
	
	bool32 bPackageCountUpdate = false;

	int32 iProcessPackageCount = 1/*KProcessPackageCountPerMsg*/; // zhangbo 1030 #bug， 该处如果处理过多， 导致登录成功的消息处理可能晚于K线应答消息处理
	if (CViewData::ELSLoginSuccess == m_eLoginState)
		iProcessPackageCount = KProcessPackageCountPerMsg;

	while (iProcessPackageCount--)
	{
		CMmiRespNode *pMmiRespNode = m_pDataManager->PeekAndRemoveFirstResp();
		if (NULL == pMmiRespNode)
		{
			break;
		}
		else
		{
			int32 iMmiRespId			= pMmiRespNode->m_iMmiRespId;
			int32 iCommunicationId		= pMmiRespNode->m_iCommunicationId;
			CMmiCommBase *pMmiCommResp	= pMmiRespNode->m_pMmiCommResp;
			
			ASSERT(NULL != pMmiCommResp);
			if (NULL == pMmiCommResp)
			{
				break;
			}
			switch(pMmiCommResp->m_eCommType)
			{
			case ECTRespAuth:		// 应答认证信息
				{
					OnDataRespAuth(iMmiRespId, (const CMmiRespAuth *)pMmiCommResp);
				}
				break;
			case ECTRespMarketInfo:	// 应答市场信息
				{
					OnDataRespMarketInfo(iMmiRespId, (const CMmiRespMarketInfo *)pMmiCommResp);
				}
				break;			
			case ECTRespMarketSnapshot:	// 应答市场快照信息
				{
					OnDataRespMarketSnapshot(iMmiRespId, (const CMmiRespMarketSnapshot *)pMmiCommResp);
				}
				break;
			case ECTRespBroker:	// 应答经济席位信息
				{
					OnDataRespBroker(iMmiRespId, (const CMmiRespBroker *)pMmiCommResp);
				}
				break;
			case ECTRespMerchInfo:	// 应答商品信息
				{
					OnDataRespMerchInfo(iMmiRespId, (const CMmiRespMerchInfo *)pMmiCommResp);
				}
				break;
			case ECTRespPublicFile:	// 应答服务器文件
				{
					OnDataRespPublicFile(iMmiRespId, (const CMmiRespPublicFile *)pMmiCommResp);
				}
				break;
			case ECTRespLogicBlockFile: // 应答逻辑板块文件
				{
					OnDataRespLogicBlockFile(iMmiRespId, (const CMmiRespLogicBlockFile*)pMmiCommResp);
				}
				break;
			case ECTRespMerchExtendData:	// 应答商品扩展数据
				{
					OnDataRespMerchExtendData(iMmiRespId, (const CMmiRespMerchExtendData *)pMmiCommResp);
				}
				break;
			case ECTRespMerchF10:	// 应答商品扩展数据
				{
					OnDataRespMerchF10(iMmiRespId, (const CMmiRespMerchF10 *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimePrice:	// 应答商品实时5档行情
				{
					OnDataRespRealtimePrice(iMmiRespId, iCommunicationId, (const CMmiRespRealtimePrice *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimeLevel2:	// 应答商品实时level2数据
				{
					OnDataRespRealtimeLevel2(iMmiRespId, iCommunicationId, (const CMmiRespRealtimeLevel2 *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimeTick:	// 应该Tick
				{
					// 回来的包只是一个标志, 没有实际意义(类似注册推送)
					// NULL;
				}
				break;
			case ECTRespMerchKLine:	// 应答商品历史K线数据
				{
					OnDataRespMerchKLine(iMmiRespId, (CMmiRespMerchKLine *)pMmiCommResp);
				}
				break;
			case ECTRespMerchTimeSales:	// 应答商品分笔成交历史
				{
					OnDataRespMerchTimeSales(iMmiRespId, (const CMmiRespMerchTimeSales *)pMmiCommResp);
				}
				break;
			case ECTRespReport:	// 应答报价表
				{
					OnDataRespReport(iMmiRespId, iCommunicationId, (const CMmiRespReport *)pMmiCommResp);
				}
				break;
			case ECTRespBlockReport: // 应答板块内商品报价排序
				{
					// _T("应答板块报价商品排行...\r\n");
					OnDataRespBlockReport(iMmiRespId, iCommunicationId, (const CMmiRespBlockReport*)pMmiCommResp);
				}
				break;
			case ECTRespTradeTime:
				{
					OnDataRespClientTradeTime(iMmiRespId, (const CMmiRespTradeTime*)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushPrice:	// 注册推送买卖盘报价
				{
					OnDataAnsRegisterPushPrice(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushPrice *)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushTick:	// 注册推送分笔
				{
					OnDataAnsRegisterPushTick(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushTick *)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushLevel2:	// 注册推送Level2
				{
					OnDataAnsRegisterPushLevel2(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushLevel2 *)pMmiCommResp);
				}
				break;
			case ECTRespCancelAttendMerch:	// 应答取消商品实时数据推送
				{
					OnDataRespCancelAttendMerch(iMmiRespId, iCommunicationId, (const CMmiRespCancelAttendMerch *)pMmiCommResp);
				}
				break;
			case ECTRespError:		// 服务器报错
				{
					OnDataRespError(iMmiRespId, (const CMmiRespError *)pMmiCommResp);
				}
				break;

			// 以下是推送
			case ECTPushPrice:		// 推送买卖盘报价
				{
					OnDataPushPrice(iMmiRespId, iCommunicationId, (const CMmiPushPrice *)pMmiCommResp);
				}
				break;
			case ECTPushTick:		// 推送分笔
				{
					OnDataPushTick(iMmiRespId, iCommunicationId, (const CMmiPushTick *)pMmiCommResp);
				}
				break;
			case ECTPushLevel2:		// 推送Level2
				{
					OnDataPushLevel2(iMmiRespId, iCommunicationId, (const CMmiPushLevel2 *)pMmiCommResp);
				}
				break;
			case ECTUpdateSymbol:	// 更新商品列表
				{
					// ...fangz0331
				}
				break;
			case ECTKickOut:
				{
					OnDataKickOut(iMmiRespId, iCommunicationId, (const CMmiKickOut *)pMmiCommResp);
				}
				break;
			case ECTRespNetTest:
				{
					OnDataRespNetTest(iMmiRespId, (const CMmiRespNetTest *)pMmiCommResp);
				}
				break;
			case ECTRespGeneralNormal:
			case ECTRespAddPushGeneralNormal:
				{
					OnDataRespGeneralNormal(iMmiRespId, (const CMmiRespGeneralNormal*)pMmiCommResp);
				}
				break;
			case ECTRespGeneralFinance:
				{
					OnDataRespGeneralFiniance(iMmiRespId, (const CMmiRespGeneralFinance*)pMmiCommResp);
				}
				break;
			case ECTRespMerchTrendIndex:
			case ECTRespAddPushMerchTrendIndex:
				{
					OnDataRespMerchTrendIndex(iMmiRespId, (const CMmiRespMerchTrendIndex*)pMmiCommResp);
				}
				break;
			case ECTRespMerchAuction:
				{
					OnDataRespMerchAuction(iMmiRespId, (const CMmiRespMerchAuction*)pMmiCommResp);
				}
				break;
			case ECTRespFundHold:
				{
					OnDataRespFundHold(iMmiRespId, (const CMmiRespFundHold*)pMmiCommResp);
				}		
				break;
			case ECTRespMinuteBS:
			case ECTRespAddPushMinuteBS:
				{
					OnDataRespMinuteBS(iMmiRespId, (const CMmiRespMinuteBS*)pMmiCommResp);
				}
				break;
			case ECTRespPlugIn:
				{
					OnDataRespPlugInData(iMmiRespId, (const CMmiCommBasePlugIn*)pMmiCommResp);
				}
				break;
			case ECTRespAuthPlugInHeart:
				{
					OnDataRespAuthPlugInHeart(iMmiRespId, (const CMmiRespAuthPlugInHeart*)pMmiCommResp);
				}
				break;
			default:
				{
					ASSERT(0);
				}
				break;
			}

			// 严重声明：这里一定要记得释放该对象， 该对象由datamanager申请， 为了异步处理的需要， 而不自己释放!!!
			DEL(pMmiRespNode);
			
			// 收到的包数据增加
			m_iRecvedPackagerCount++;
			bPackageCountUpdate = true;
		}
	}

	if (bPackageCountUpdate)
	{
		int32 iii = 0;
		iii++;
	}

}

void CViewData::InitializeMarket(CMarket &Market)
{
	

	// 设置其尚未初始化
	Market.SetInitialized(false); 

	// 清空该市场下所有商品的实时数据
	int32 i;
	for ( i = 0; i < Market.m_MerchsPtr.GetSize(); i++)
	{
		CMerch *pMerch = Market.m_MerchsPtr[i];
		if (NULL == pMerch || NULL == pMerch->m_pRealtimePrice)
			continue;
		
		// 		
		pMerch->m_pRealtimePrice->InitMerchPrice();
		
		// 异步方式通知所有关心该数据的业务视图
		// PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);	

		// 如果是状态栏的商品数据
		for (int32 iIndexStatusMerch = 0; iIndexStatusMerch < m_StatucBarAttendMerchs.GetSize(); iIndexStatusMerch++)
		{
			if (pMerch == m_StatucBarAttendMerchs[iIndexStatusMerch].m_pMerch)
			{				
				PostMessage(m_MainFrameWnd,UM_MainFrame_OnStatusBarDataUpdate, 0, 0);
			}
		}
	
		// 更新:
		for ( int32 j = 0; j < m_IoViewList.GetSize(); j++)
		{
			CIoViewListner *pIoView = m_IoViewList[j];
			if ( pIoView->IsAttendData(pMerch, EDSTPrice) )
			{
				pIoView->OnVDataRealtimePriceUpdate(pMerch);
				pIoView->OnVDataForceUpdate();
			}
		}
	}

	// 清空上涨下跌家数
	if (0 != Market.m_MarketSnapshotInfo.m_iRiseCount || 0 != Market.m_MarketSnapshotInfo.m_iFallCount)
	{
		Market.m_MarketSnapshotInfo.m_iRiseCount = 0;
		Market.m_MarketSnapshotInfo.m_iFallCount = 0;

		// 通知所有业务视图
		for (i = 0; i < m_IoViewList.GetSize(); i++)
		{
			CIoViewListner *pIoView = m_IoViewList[i];
			pIoView->OnVDataMarketSnapshotUpdate(Market.m_MarketSnapshotInfo.m_iMarketId);
		}
	}

	/////////////////////////////////////////////////////////////
	// 发送指令获取该市场基本信息
	CMmiReqMarketInfo MmiReqMarketInfo;
	MmiReqMarketInfo.m_iMarketId = Market.m_MarketInfo.m_iMarketId;
	RequestViewData((CMmiCommBase *)&MmiReqMarketInfo);
	
	// 发送指令获取该市场下所有商品列表
	CMmiReqMerchInfo MmiReqMerchInfo;
	MmiReqMerchInfo.m_iMarketId = Market.m_MarketInfo.m_iMarketId;
	RequestViewData((CMmiCommBase *)&MmiReqMerchInfo);
}

void CViewData::OnDataRespAuth(int iMmiReqId, IN const CMmiRespAuth *pMmiRespAuth)
{
	if ( NULL == pMmiRespAuth )
	{
		ASSERT(0);;
		return;
	}

	// 什么都不干
	// NULL;
}

void CViewData::OnDataRespAuthPlugInHeart(int iMmiReqId, IN const CMmiRespAuthPlugInHeart* pMmiResp)
{
	if(NULL == pMmiResp)
	{
		ASSERT(0);;
		return;
	}

	if (pMmiResp->m_StrUserName != m_strUserName)
	{
		ASSERT(0);;
		return;
	}

	//
	PostMessage(m_OwnerWnd, UM_ViewData_OnAuthPlugInHeart, (WPARAM)pMmiResp->m_iRet, 0);
}

void CViewData::OnDataRespMarketInfo(int iMmiReqId, IN const CMmiRespMarketInfo *pMmiRespMarketInfo)
{
	if (NULL == pMmiRespMarketInfo)
	{
		ASSERT(0);;
		return;
	}
	
	// 逐个市场赋值, 找不到对应的市场的， 丢弃数据
	CMarketInfo *pMarketInfo = NULL;
	for (int32 iIndexMarket = 0; iIndexMarket < pMmiRespMarketInfo->m_MarketInfoListPtr.GetSize(); iIndexMarket++)
	{
		pMarketInfo = pMmiRespMarketInfo->m_MarketInfoListPtr[iIndexMarket];
		if (NULL == pMarketInfo)
			continue;
	
		CMarket *pMarketFound = NULL;
		if (m_MerchManager.FindMarket(pMarketInfo->m_iMarketId, pMarketFound))
		{
			// TRACE(L"市场号:%d \r\n", pMarketInfo->m_iMarketId);

			// 找到， 赋值
			pMarketFound->m_MarketInfo = *pMarketInfo;

			// 设置该市场已经成功获取市场信息
			pMarketFound->m_MarketInfoReqState.m_bRequesting = false;

			//			
			OnSomeInitializeMarketDataRet(*pMarketFound);
		} 
	}
}

void CViewData::OnDataRespMarketSnapshot(int iMmiReqId, IN const CMmiRespMarketSnapshot *pMmiRespMarketSnapshot)
{
	if (NULL == pMmiRespMarketSnapshot)
	{
		ASSERT(0);;
		return;
	}
		
	// 逐个市场赋值, 找不到对应的市场的， 丢弃数据
	CMarketSnapshotInfo *pMarketSnapshot = NULL;
	for (int32 iIndexMarket = 0; iIndexMarket < pMmiRespMarketSnapshot->m_MarketSnapshotListPtr.GetSize(); iIndexMarket++)
	{
		pMarketSnapshot = pMmiRespMarketSnapshot->m_MarketSnapshotListPtr[iIndexMarket];
		if (NULL == pMarketSnapshot)
			continue;
	
		CMarket *pMarketFound = NULL;
		if (m_MerchManager.FindMarket(pMarketSnapshot->m_iMarketId, pMarketFound))
		{
			if (pMarketFound->m_MarketSnapshotInfo == *pMarketSnapshot)	// 相同
				continue;

			// 找到， 赋值
			pMarketFound->m_MarketSnapshotInfo = *pMarketSnapshot;

			// 通知所有业务视图
			for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
			{
				CIoViewListner *pIoView = m_IoViewList[i];
				pIoView->OnVDataMarketSnapshotUpdate(pMarketFound->m_MarketSnapshotInfo.m_iMarketId);
			}
		}
	}
}

void CViewData::OnDataRespMerchInfo(int iMmiReqId, IN const CMmiRespMerchInfo *pMmiRespMerchInfo)
{
	if (NULL == pMmiRespMerchInfo)
	{
		ASSERT(0);;
		return;
	}
	
	CMarket *pMarket = NULL;
	if (m_MerchManager.FindMarket(pMmiRespMerchInfo->m_iMarketId, pMarket))
	{
		CMerchInfo *pMerchInfoSrc = NULL;
		for (int32 i = 0; i < pMmiRespMerchInfo->m_MerchInfoListPtr.GetSize(); i++)
		{
			pMerchInfoSrc = pMmiRespMerchInfo->m_MerchInfoListPtr[i];
			if (NULL == pMerchInfoSrc)
				continue;
		
			// 更新现有的商品信息,  分三种情况
			// 1) 有的保留
			// 2) 原来有现在没的置删除标志
			// 3) 新加的增加
			int32 iPosMerchFound = 0;
			CMerch *pMerchFound = NULL;
			if (pMarket->FindMerch(pMerchInfoSrc->m_StrMerchCode, iPosMerchFound, pMerchFound))
			{
				// 原来就有，保留该经济席位
				pMerchFound->m_MerchInfo = *pMerchInfoSrc;
				pMerchFound->m_MerchInfo.m_bRemoved = false;
			}
			else
			{
				// 找不到， 就追加一个
				pMarket->AddMerch(*pMerchInfoSrc);
			}
		}	
		
		// 设置该市场已经成功获取市场信息
		pMarket->m_MerchListReqState.m_bRequesting = false;

		// 
		OnSomeInitializeMarketDataRet(*pMarket);
	}
}

void CViewData::OnDataRespPublicFile(int iMmiReqId, IN const CMmiRespPublicFile *pMmiRespPublicFile)
{
	if (NULL == pMmiRespPublicFile)
	{
		ASSERT(0);;
		return;
	}

	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiRespPublicFile->m_StrMerchCode, pMmiRespPublicFile->m_iMarketId, pMerch))
		return;
	
	//
	if (0 == pMmiRespPublicFile->m_uiCRC32)	// 表示服务器没有该数据
	{
		;	// 暂时先不处理
	}
	else
	{
		// 无论如何，接收到数据更新财务数据更新时间
		if ( EPFTF10 == pMmiRespPublicFile->m_ePublicFileType )
		{
			pMerch->m_TimeLastUpdateFinanceData = GetServerTime();
		}

		if (pMmiRespPublicFile->m_iValidDataLen <= 0)	// 表示本地数据已经是最新的， 无需更新， 完美！！！
		{
			if ( EPFTWeight == pMmiRespPublicFile->m_ePublicFileType )
			{
				pMerch->m_TimeLastUpdateWeightDatas = GetServerTime();
			}
			
		}
		else
		{
			if (NULL == pMmiRespPublicFile->m_pcBuffer)
			{
				ASSERT(0);
				return;
			}
			else
			{
				if (EPFTWeight == pMmiRespPublicFile->m_ePublicFileType)	// 如果是除权数据， 保存该数据
				{
					if (CMerch::ReadWeightDatasFromBuffer(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen, pMerch->m_aWeightDatas))
					{
						// 更新相关数据
						pMerch->m_TimeLastUpdateWeightDatas = GetServerTime();
						pMerch->m_uiWeightDataCRC = CCodeFile::crc_32(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen);
						
						// 尝试保存到文件中
						CString StrFilePath;
						if (m_OfflineDataManager.GetPublishFilePath(EPFTWeight, pMmiRespPublicFile->m_iMarketId, pMmiRespPublicFile->m_StrMerchCode, StrFilePath))
						{
							_tcheck_if_mkdir(StrFilePath.LockBuffer());
							StrFilePath.UnlockBuffer();
							
							CFile File;
							if (File.Open(StrFilePath, CFile::modeWrite | CFile::modeCreate))
							{
								File.Write(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen);
								File.Close();
							}
						}
						
						// 异步方式通知所有关心该数据的业务视图
						PostMessage(m_OwnerWnd, UM_ViewData_OnPublicFileUpdate, (WPARAM)pMerch, EPFTWeight);
					}
				}
				else if ( EPFTF10 == pMmiRespPublicFile->m_ePublicFileType )	
				{
					//  F10 的财务数据
					if ( CMerch::ReadFinanceDataFromBuffer(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen, *pMerch->m_pFinanceData) )
					{
						pMerch->m_TimeLastUpdateFinanceData = GetServerTime();

						CString StrFilePath;
						if ( m_OfflineDataManager.GetPublishFilePath(EPFTF10, pMmiRespPublicFile->m_iMarketId, pMmiRespPublicFile->m_StrMerchCode, StrFilePath))
						{
							_tcheck_if_mkdir(StrFilePath.LockBuffer());
							StrFilePath.UnlockBuffer();

							CFile File;
							if ( File.Open(StrFilePath, CFile::modeWrite | CFile::modeCreate) )
							{
								File.Write(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen);
								File.Close();
							}
						}
						// 通知更新
						// ...
						PostMessage(m_OwnerWnd, UM_ViewData_OnPublicFileUpdate, (WPARAM)pMerch, EPFTF10);
					}
				}
			}
		}
	}
}

void CViewData::OnDataRespLogicBlockFile(int iMmiReqId, IN const CMmiRespLogicBlockFile *pMmiRespLogicBlockFile)
{

	if ( NULL == pMmiRespLogicBlockFile )
	{
		ASSERT(0);
		return;
	}

	if ( !CBlockConfig::m_bWaitForServerResp )
	{
		return;		// 不是等待状态不接受该包
	}

	// 如果存在登录框， 通知登录框状态变化
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString(L"接收板块信息成功，开始解析...");
		SendMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);
	}

	bool32 bParseBlock = CBlockConfig::RespLogicBlockFile(pMmiRespLogicBlockFile, true);
	if ( !bParseBlock )
	{
		if (NULL != m_LoginWnd)
		{
			CString StrBlockFileName;
			CBlockConfig::GetConfigFullPathName(StrBlockFileName);
			bParseBlock = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // 初始化板块文件
			if ( !bParseBlock )
			{
				CString *pStrTip = new CString(L"解析板块数据失败！！");
				SendMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);
			}
		}
	}



	// 如果存在登录框， 通知登录框状态变化
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString(L"提示：同步板块列表信息结束");
		SendMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);
	}
	

	OnAllMarketInitializeSuccess();
}

void CViewData::OnDataRespBroker(int iMmiReqId, IN const CMmiRespBroker *pMmiRespBroker)
{
	if (NULL == pMmiRespBroker)
	{
		return;
	}
	if (NULL != pMmiRespBroker)
	{
		ASSERT(0);
		return;
	}
	
	// 找到对应的大市场， 设置下面的经济席位
	CBreed *pBreed = NULL;
	int32 iPosBreed = 0;
	if (m_MerchManager.FindBreed(pMmiRespBroker->m_iBreedId, iPosBreed, pBreed))
	{
		CMmiBroker *pCommBroker = NULL;
		for (int32 i = 0; i < pMmiRespBroker->m_CommBrokerListPtr.GetSize(); i++)
		{
			pCommBroker = pMmiRespBroker->m_CommBrokerListPtr[i];
			if (NULL == pCommBroker)
				continue;
		
			// 更新现有的经济席位信息,  分三种情况
			// 1) 有的保留
			// 2) 原来有现在没的置删除标志
			// 3) 新加的增加
			int32 iPosBroker = 0;
			CBroker *pBroker = NULL;
			if (pBreed->FindBroker(pCommBroker->m_StrBrokerCode, iPosBroker, pBroker))
			{
				// 原来就有，保留该经济席位
				pBroker->m_bRemoved = false;
			}
			else
			{
				CMmiBroker BrokerNew;
				BrokerNew = *pCommBroker;
				
				// 找不到， 就追加一个
				pBreed->AddBroker(BrokerNew);
			}
		}

		// zhangbo 0507 #严重bug， 此处尚未处理， 如何初始化问题
// 		// 设置该市场已经成功获取市场信息
// 		pBreed->m_bRequestingMarketInfo = false;
// 
// 		// 
// 		OnSomeInitializeMarketDataRet(*pMarket);
	}
}


void CViewData::OnDataRespMerchExtendData(int iMmiReqId, IN const CMmiRespMerchExtendData *pMmiRespMerchExtendData)
{
	if (NULL == pMmiRespMerchExtendData)
	{
		ASSERT(0);
		return;
	}
	
	CMerch *pMerch = NULL;
	if (m_MerchManager.FindMerch(pMmiRespMerchExtendData->m_StrMerchCode, pMmiRespMerchExtendData->m_iMarketId, pMerch))
	{
		if (NULL == pMerch->m_pMerchExtendData)
		{
			pMerch->m_pMerchExtendData = new CMerchExtendData();
		}

		*pMerch->m_pMerchExtendData = pMmiRespMerchExtendData->m_MerchExtendData;

		// 通知所有视图扩展数据变化
		// bug, 暂时还未确定扩展数据格式
	}
}

void CViewData::OnDataRespMerchF10(int iMmiReqId, IN const CMmiRespMerchF10 *pMmiRespMerchF10)
{
	if (NULL == pMmiRespMerchF10)
	{
		ASSERT(0);
		return;
	}

	CMerch *pMerch = NULL;
	if (m_MerchManager.FindMerch(pMmiRespMerchF10->m_StrMerchCode, pMmiRespMerchF10->m_iMarketId, pMerch))
	{
		if (NULL == pMerch->m_pMerchF10)
		{
			pMerch->m_pMerchF10 = new CMerchF10();
		}

		*pMerch->m_pMerchF10 = pMmiRespMerchF10->m_MerchF10;

		// 通知到相应的视图刷新
		// bug, 暂时还未确定F10数据格式
	}
}

void CViewData::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{ 
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch))
		return;
	// 调试用， 检查服务器是否存在异常推送
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimePrices.find(pMerch);
	if (itFind == m_aPushingRealtimePrices.end())	// 找不到， 提示
	{
		CString StrTrace;
		StrTrace.Format(L"\n☆☆☆☆☆ 警告：已经Cancel掉的商品又有推送Price数据过来了, %d & %s\n", RealtimePrice.m_iMarketId, CString(RealtimePrice.m_StrMerchCode).GetBuffer());
		TRACE(StrTrace);
	}
#endif

	//RGUARD(LockSingle, pMerch->m_LockRealtimePrice, LockRealtimePrice);
	{
		////////////////////////////////////////////////////
		// 之前还未有收到过数据, 创建
		if (NULL == pMerch->m_pRealtimePrice)
		{
			pMerch->m_pRealtimePrice = new CRealtimePrice();
		}

		if (RealtimePrice.m_TimeCurrent < pMerch->m_pRealtimePrice->m_TimeCurrent)	// 该值时间太早， 忽略
			return;
			
		// 
		*pMerch->m_pRealtimePrice = RealtimePrice;
		pMerch->m_pRealtimePrice->ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);
		
// 		CString StrTrace;
// 		StrTrace.Format(L"====> %d & %s %g %g\n", RealtimePrice.m_iMarketId, RealtimePrice.m_StrMerchCode, RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevAvg);
// 		TRACE(StrTrace);

		// 如果是状态栏的商品数据
		/*if ( !IsWindow(pMainFrame->GetSafeHwnd()) || !pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
		{
			pMainFrame = NULL;
		}*/
		for (int32 i = 0; i < m_StatucBarAttendMerchs.GetSize() && NULL != m_MainFrameWnd; i++)
		{
			if (pMerch == m_StatucBarAttendMerchs[i].m_pMerch)
			{				
				PostMessage(m_MainFrameWnd,UM_MainFrame_OnStatusBarDataUpdate, 0, 0);
			}
		}
 
		// 调整各个基本K线
		bool32 bKLineProcessSuccess = false;
		for (int32 iIndexKLineNode = 0; iIndexKLineNode < pMerch->m_MerchKLineNodesPtr.GetSize(); iIndexKLineNode++)
		{
			CMerchKLineNode *pMerchKLineNode = pMerch->m_MerchKLineNodesPtr[iIndexKLineNode];
			if (NULL == pMerchKLineNode)
				continue;

			if (pMerchKLineNode->ProcessRealtimePrice(pMerch, *pMerch->m_pRealtimePrice))	// 调整K线成功
			{
				bKLineProcessSuccess = true;
			}
		}

		if (bKLineProcessSuccess)
		{		
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnKLineNodeUpdate, (WPARAM)pMerch, 0);	
		}

		// 通知所有关心实时推送price的业务视图
		{
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);	
		}

		// ...fangz 0104 状态栏, 条件预警那些也要改, 暂时留着, 没空改
		for (int i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
		{
			CViewDataListner* pListener = m_aViewDataLinstner[i];
			if ( NULL != pListener && pListener->IsAttendCommType(ECTReqRealtimePrice) )
			{
				pListener->OnRealtimePrice(RealtimePrice, iCommunicationId, iReqId);
			}
		}

		//TRACE(L"行情数据: %d %s %.2f %.2f %.2f\n", RealtimePrice.m_iMarketId, RealtimePrice.m_StrMerchCode, RealtimePrice.m_fPriceNew, RealtimePrice.m_fVolumeTotal, RealtimePrice.m_fAmountTotal);
 	}
}

void CViewData::OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId)
{
	if (MmiPushTick.m_Ticks.GetSize() <= 0)
		return;

	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(MmiPushTick.m_StrMerchCode, MmiPushTick.m_iMarketId, pMerch))
		return;

	// 调试用， 检查服务器是否存在异常推送
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeTicks.find(pMerch);
	if (itFind == m_aPushingRealtimeTicks.end())	// 找不到， 提示
	{
		CString StrTrace;
		StrTrace.Format(L"\n☆☆☆☆☆ 警告：已经Cancel掉的商品又有推送Tick数据过来了, %d & %s\n", MmiPushTick.m_iMarketId, CString(MmiPushTick.m_StrMerchCode).GetBuffer());
		//TRACE(StrTrace);
	}
#endif

//	RGUARD(LockSingle, pMerch->m_LockRealtimeTick, LockRealtimeTick);
	{
		////////////////////////////////////////////
		bool32 bUpdateRealtimeTick = false;

		CTick *pTicks = (CTick *)MmiPushTick.m_Ticks.GetData();
		for (int32 i = 0; i < MmiPushTick.m_Ticks.GetSize(); i++)
		{
			CTick &Tick = pTicks[i];

			// 之前还未有收到过数据, 创建
			if (NULL == pMerch->m_pRealtimeTick)
			{
				pMerch->m_pRealtimeTick = new CTick();
			}
		
			if (Tick.m_TimeCurrent < pMerch->m_pRealtimeTick->m_TimeCurrent)	// 该值时间太早， 忽略
				continue;
			
			// 
			*pMerch->m_pRealtimeTick = Tick;	
			pMerch->m_pRealtimeTick->ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);

			bUpdateRealtimeTick = true;
		}

		// 通知所有关心实时推送tick的业务视图
		if (bUpdateRealtimeTick)
		{
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeTickUpdate, (WPARAM)pMerch, 0);	
		}
	}

	// 更新timesale
	if ( NULL == pMerch->m_pMerchTimeSales )
	{
		// 没找到就建一个
		pMerch->m_pMerchTimeSales = new CMerchTimeSales();		
	}
	
	{
		bool32 bTimesaleProcessTickSuccess = false;

		CTick *pTicks = (CTick *)MmiPushTick.m_Ticks.GetData();
		for (int32 i = 0; i < MmiPushTick.m_Ticks.GetSize(); i++)
		{
			CTick &Tick = pTicks[i];
			if (1 != pMerch->m_Market.m_MarketInfo.m_iVolScale)
			{
				CTick TickCurrent = Tick;
				TickCurrent.ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);

				if (pMerch->m_pMerchTimeSales->ProcessRealtimeTick(TickCurrent))
					bTimesaleProcessTickSuccess = true;
			}
			else
			{
				if (pMerch->m_pMerchTimeSales->ProcessRealtimeTick(Tick))
					bTimesaleProcessTickSuccess = true;
			}
		}

		if (bTimesaleProcessTickSuccess)
		{
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnTimesaleUpdate, (WPARAM)pMerch, 0);
		}	
	}
	
	// ...fangz 0104 上面的选股也要这么处理, 暂时留着, 没空改
	for ( int32 i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
	{
		CViewDataListner* pListener = m_aViewDataLinstner[i];
		if ( NULL != pListener && pListener->IsAttendCommType(ECTReqRealtimeTick) )
		{				
			pListener->OnRealtimeTick(MmiPushTick, iCommunicationId);
		}
	}
}


void CViewData::OnRealtimeLevel2(const CRealtimeLevel2 &RealtimeLevel2, int32 iCommunicationId)
{
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(RealtimeLevel2.m_StrMerchCode, RealtimeLevel2.m_iMarketId, pMerch))
		return;

	// 调试用， 检查服务器是否存在异常推送
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeLevel2s.find(pMerch);
	if (itFind == m_aPushingRealtimeLevel2s.end())	// 找不到， 提示
	{
		CString StrTrace;
		StrTrace.Format(L"\n☆☆☆☆☆ 警告：已经Cancel掉的商品又有推送Level2数据过来了, %d & %s\n",RealtimeLevel2.m_iMarketId, CString(RealtimeLevel2.m_StrMerchCode).GetBuffer());
		//TRACE(StrTrace);
	}
#endif
	
//	RGUARD(LockSingle, pMerch->m_LockRealtimePrice, LockRealtimePrice);
	{
		/////////////////////////////////////////////////
		// 之前还未有收到过数据, 创建
		if (NULL == pMerch->m_pRealtimeLevel2)
		{
			pMerch->m_pRealtimeLevel2 = new CRealtimeLevel2();
		}
	
		// 
		*pMerch->m_pRealtimeLevel2 = RealtimeLevel2;
		
		// zhangbo 20080813 #bug， 该时间原本应该由服务器发送过来，这样比较准确， 此时赋值， 已经累积了通讯时间， 就不精确了
		pMerch->m_pRealtimeLevel2->m_TimeCurrent = GetServerTime();
		
		// 异步方式通知所有关心该数据的业务视图
		PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeLevel2Update, (WPARAM)pMerch, 0);
	}
}

void CViewData::OnDataRespRealtimePrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespRealtimePrice *pMmiRespRealtimePrice)
{
	if (NULL == pMmiRespRealtimePrice)
	{
		ASSERT(0);
		return;
	}

	const CRealtimePrice *pRealtimePrice = NULL;

	for (int32 i = 0; i < pMmiRespRealtimePrice->m_RealtimePriceListPtr.GetSize(); i++)
	{
		pRealtimePrice = pMmiRespRealtimePrice->m_RealtimePriceListPtr[i];
		if (NULL == pRealtimePrice)
		{
			continue;
		}

		OnRealtimePrice(*pRealtimePrice, iCommunicationId, iMmiReqId);
	}
}

void CViewData::OnDataRespRealtimeLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespRealtimeLevel2 *pMmiRespRealtimeLevel2)
{
	if (NULL == pMmiRespRealtimeLevel2)
	{
		ASSERT(0);;
		return;
	}
	const CRealtimeLevel2 &RealtimeLevel2 = pMmiRespRealtimeLevel2->m_RealtimeLevel2;
	OnRealtimeLevel2(RealtimeLevel2, iCommunicationId);
}

void CViewData::OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine)
{
	//_LogCheckTime("[==>Client] CViewData::收到历史 K 线回包", g_hwndTrace);

	if (NULL == pMmiRespMerchKLine)
	{
		ASSERT(0);
		return;
	}

	// 如果正在下载盘后数据
	if(m_bDownLoading)
	{
		for ( int32 i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
		{
			CViewDataListner* pListener = m_aViewDataLinstner[i];
			if ( NULL != pListener && pListener->IsAttendCommType(ECTReqMerchKLine) )
			{				
				pListener->OnDataRespMerchKLine(iMmiReqId, pMmiRespMerchKLine);
			}
		}

		return;
	}

	// 
	CMerch *pMerch = NULL;
	if (m_MerchManager.FindMerch(pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode, pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId, pMerch))
	{
		int32 iPosFound = 0;
		CMerchKLineNode *pMerchKLineNodeFound = NULL;

		if (!pMerch->FindMerchKLineNode(pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase, iPosFound, pMerchKLineNodeFound))
		{
			// 没找到就建一个
			pMerchKLineNodeFound = new CMerchKLineNode();
			pMerchKLineNodeFound->m_eKLineTypeBase = pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase;		
			pMerchKLineNodeFound->m_iMarketId	   = pMerch->m_MerchInfo.m_iMarketId;
			pMerchKLineNodeFound->m_StrMerchCode   = pMerch->m_MerchInfo.m_StrMerchCode;
			//

			pMerch->m_MerchKLineNodesPtr.Add(pMerchKLineNodeFound);
		}
		
		// 修正数据中的成交量
		if (pMerch->m_Market.m_MarketInfo.m_iVolScale != 1)
		{
			if (pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetSize() > 0)
			{
				CKLine *pKLines = (CKLine *)pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetData();
				for (int32 i = 0; i < pMmiRespMerchKLine->m_MerchKLineNode.m_KLines.GetSize(); i++)
				{
					pKLines[i].ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);
				}
			}
		}

		// 处理数据
		bool32 bProcessKLineSuccess = pMerchKLineNodeFound->ProcessKLindData(pMmiRespMerchKLine->m_MerchKLineNode);
	
 
 		if ( NULL != m_pIndexChsStkMideCore )
 		{			
 			// 脱机的不要发给业务视图,保存本地数据,等待线程来取:						
 			RGUARD(LockSingle, m_LockRecvDataForIndexChsStk, LockRecvDataForIndexChsStk);
 			RGUARD(LockSingle, m_LockNumsOfIndexChsStkReq, LockNumsOfIndexChsStkReq);
			// 消息阻塞的时候可能会有问题，暂时就这样处理吧
 			++m_iNumsOfIndexChsStkHaveRecved;
 			
 			// TRACE(L"共 %d 个请求, 收到 %d 个, 完成 = %d \n", m_iNumsOfIndexChsStkReq, m_iNumsOfIndexChsStkHaveRecved, bFinish);
 
 			// 通知收到收据
 			//m_pIndexChsStkMideCore->OnRecvKLineData(pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId, pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode, bFinish);
			PostMessage(m_MainFrameWnd, UM_IndexChsMideCore_KLineUpdate_Resp, (WPARAM)pMerch, (WPARAM)iMmiReqId);
 			// 保存数据
 			T_IndexChsStkRespData stIndexChsStkRespData;
 			
 			stIndexChsStkRespData.m_iRespID			= iMmiReqId;			
 			stIndexChsStkRespData.m_iMarketId		= pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId;
 			stIndexChsStkRespData.m_StrMerchCode	= pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode;
 			stIndexChsStkRespData.m_eKLineTypeBase	= pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase;
 			
 			if (bProcessKLineSuccess)
 			{
 				stIndexChsStkRespData.m_aKLines.Copy(pMmiRespMerchKLine->m_MerchKLineNode.m_KLines);
 			}
 			else
 			{
 				stIndexChsStkRespData.m_aKLines.RemoveAll();				
 			}	
 			
 			m_aRecvDataForIndexChsStk.Add(stIndexChsStkRespData);
 		
 		}
// 		else
		{
			if (bProcessKLineSuccess)
			{		
			//	_LogCheckTime("[==>Client] CViewData::收到历史 K 线回包, 处理完成通知视图更新", g_hwndTrace);

				// 异步方式通知所有关心该数据的业务视图
				PostMessage(m_OwnerWnd, UM_ViewData_OnKLineNodeUpdate, (WPARAM)pMerch, (LPARAM)iMmiReqId);
			}
		}

		// ...fangz 0104 上面的选股也要这么处理, 暂时留着, 没空改
		for ( int32 i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
		{
			CViewDataListner* pListener = m_aViewDataLinstner[i];
			if ( NULL != pListener && pListener->IsAttendCommType(ECTReqMerchKLine) )
			{				
				pListener->OnDataRespMerchKLine(iMmiReqId, pMmiRespMerchKLine);
			}
		}
	}
}

void CViewData::OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales)
{
	if (NULL == pMmiRespMerchTimeSales)
	{
		ASSERT(0);
		return;
	}

	CMerch *pMerch = NULL;
	if (m_MerchManager.FindMerch(pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode, pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId, pMerch))
	{
		if (NULL == pMerch->m_pMerchTimeSales)
		{
			// 没找到就建一个
			pMerch->m_pMerchTimeSales = new CMerchTimeSales();
		}

		// 修正数据中的成交量
		if (pMerch->m_Market.m_MarketInfo.m_iVolScale != 1)
		{
			if (pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetSize() > 0)
			{
				CTick *pTicks = (CTick *)pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetData();
				for (int32 i = 0; i < pMmiRespMerchTimeSales->m_MerchTimeSales.m_Ticks.GetSize(); i++)
				{
					pTicks[i].ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);
				}
			}
		}

		// 处理数据
		bool32 bRet = pMerch->m_pMerchTimeSales->ProcessTimesSaleData(pMmiRespMerchTimeSales->m_MerchTimeSales);
		if (bRet)
		{
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnTimesaleUpdate, (WPARAM)pMerch, 0);
		}

		//
		for ( int32 i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
		{
			CViewDataListner* pListener = m_aViewDataLinstner[i];
			if ( NULL != pListener && pListener->IsAttendCommType(ECTReqMerchTimeSales) )
			{
				pListener->OnDataRespMerchTimeSales(iMmiReqId, pMmiRespMerchTimeSales);
			}
		}
	}
}

void CViewData::OnDataRespReport(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespReport *pMmiRespReport)
{
	if (NULL == pMmiRespReport)
	{
		ASSERT(0);
		return;
	}

	// 通知所有的视图排行顺序改变
	int32				iMarketId			= pMmiRespReport->m_iMarketId;
	E_MerchReportField	eMerchReportField	= pMmiRespReport->m_eMerchReportField;
	bool32				bDescSort			= pMmiRespReport->m_bDescSort;
	int32				iPosStart			= pMmiRespReport->m_iStart;

	CArray<CMerch*, CMerch*> aMerchs;
	int32 i;
	for ( i = 0; i < pMmiRespReport->m_aMerchs.GetSize(); i++)
	{
		const CMerchKey &MerchKey = pMmiRespReport->m_aMerchs[i];

		CMerch *pMerch = NULL;
		m_MerchManager.FindMerch(MerchKey.m_StrMerchCode, MerchKey.m_iMarketId, pMerch);
		if (NULL == pMerch || pMerch->m_MerchInfo.m_iMarketId != iMarketId)
		{
			assert(0);
			CString strTempMerchCode = MerchKey.m_StrMerchCode;
			TRACE(_T("忽略报价表排序返回包商品: %d-%s\n"), MerchKey.m_iMarketId, strTempMerchCode.GetBuffer());
			strTempMerchCode.ReleaseBuffer();
	
			continue;
		}
				
		aMerchs.Add(pMerch);
	}

	// 通知所有的业务视图排行顺序改变, 这里不方便判断哪些视图感兴趣， 让具体视图自己去判断数据是否有意义
	for (i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->OnVDataReportUpdate(iMarketId, eMerchReportField, bDescSort, iPosStart, pMmiRespReport->m_aMerchs.GetSize(), aMerchs);
	}
}

void CViewData::OnDataRespBlockReport(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespBlockReport *pMmiRespBlockReport)
{
	if ( NULL == pMmiRespBlockReport )
	{
		ASSERT(0);
		return;
	}
	// 通知所有的视图排行顺序改变
	int32				iBlockId			= pMmiRespBlockReport->m_iBlockId;
	E_MerchReportField	eMerchReportField	= pMmiRespBlockReport->m_eMerchReportField;
	bool32				bDescSort			= pMmiRespBlockReport->m_bDescSort;
	int32				iPosStart			= pMmiRespBlockReport->m_iStart;
	
	CArray<CMerch*, CMerch*> aMerchs;
	int32 i;
	for ( i = 0; i < pMmiRespBlockReport->m_aMerchs.GetSize(); i++)
	{
		const CMerchKey &MerchKey = pMmiRespBlockReport->m_aMerchs[i];
		
		CMerch *pMerch = NULL;
		m_MerchManager.FindMerch(MerchKey.m_StrMerchCode, MerchKey.m_iMarketId, pMerch);
		if ( NULL == pMerch )
		{
			// 可能存在服务器端的商品客户端没有的
			CString strTempMerchCode = MerchKey.m_StrMerchCode;
			TRACE(_T("忽略板块排序返回包商品: %d-%s\n"), MerchKey.m_iMarketId, strTempMerchCode.GetBuffer());
			strTempMerchCode.ReleaseBuffer();
			continue;
		}
		
		aMerchs.Add(pMerch);
	}
	
	// 通知所有的业务视图排行顺序改变, 这里不方便判断哪些视图感兴趣， 让具体视图自己去判断数据是否有意义
	for (i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->OnVDataReportInBlockUpdate(iBlockId, eMerchReportField, bDescSort, iPosStart, pMmiRespBlockReport->m_aMerchs.GetSize(), aMerchs);
	}
}

void CViewData::OnDataAnsRegisterPushPrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushPrice *pMmiAnsRegisterPushPrice)
{
	// zhangbo 20090620 #待处理， 新协议
	//...
}

void CViewData::OnDataAnsRegisterPushTick(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushTick *pMmiAnsRegisterPushTick)
{
	// zhangbo 20090620 #待处理， 新协议
	//...
}

void CViewData::OnDataAnsRegisterPushLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushLevel2 *pMmiAnsRegisterPushLevel2)
{
	// zhangbo 20090620 #待处理， 新协议
	//...
}

void CViewData::OnDataRespCancelAttendMerch(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespCancelAttendMerch *pMmiRespCancelAttendMerch)
{
	if (NULL == pMmiRespCancelAttendMerch)
	{
		ASSERT(0);
		return;
	}

	// zhangbo 20090620 #取消推送的接口已经不再调用， 所以该处也不会进来
	//...
}

void CViewData::OnDataRespError(int32 iMmiReqId, IN const CMmiRespError *pMmiRespError)
{
	if (NULL == pMmiRespError)
	{
		ASSERT(0);
		return;
	}

	CString StrErrMsg = ((CMmiRespError *)pMmiRespError)->GetSummary();
	// TRACE(L"%s \n", StrErrMsg);

	if ( ECTReqAuth == pMmiRespError->m_eReqCommType 
	  && (ERENeedAuth == pMmiRespError->m_eRespErrorCode || EREAuthError == pMmiRespError->m_eRespErrorCode || ERERequestError == pMmiRespError->m_eRespErrorCode)
	   )
	{
		m_bAuthError = true;
		
		//
		CString StrMsg;
		StrMsg.Format(L"认证出现错误, 请稍候重新登录!");
		MessageBox(::GetActiveWindow(), StrMsg, AfxGetApp()->m_pszAppName, MB_ICONWARNING);	
	}
}

void CViewData::OnDataPushPrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushPrice *pMmiPushPrice)
{
	if (NULL == pMmiPushPrice)
	{
		ASSERT(0);
		return;
	}
	
	const CRealtimePrice *pRealtimePrice = NULL;
	for (int32 i = 0; i < pMmiPushPrice->m_RealtimePriceListPtr.GetSize(); i++)
	{
		pRealtimePrice = pMmiPushPrice->m_RealtimePriceListPtr[i];
		
		if (NULL == pRealtimePrice)
		{
			assert(0);
			continue;
		}
		
		// CString StrTime;
		// StrTime.Format(L"[%d] %04d-%02d-%02d %02d:%02d:%02d:", pRealtimePrice->m_TimeCurrent.m_Time.GetTime(), pRealtimePrice->m_TimeCurrent.m_Time.GetYear(), pRealtimePrice->m_TimeCurrent.m_Time.GetMonth(), pRealtimePrice->m_TimeCurrent.m_Time.GetDay(), pRealtimePrice->m_TimeCurrent.m_Time.GetHour(), pRealtimePrice->m_TimeCurrent.m_Time.GetMinute(), pRealtimePrice->m_TimeCurrent.m_Time.GetSecond());

		//
		// TRACE(L"===>推送:%s %s 买一: %.2f 卖一: %.2f \n", pRealtimePrice->m_StrMerchCode, StrTime, pRealtimePrice->m_astBuyPrices[0].m_fPrice, pRealtimePrice->m_astSellPrices[0].m_fPrice);
		OnRealtimePrice(*pRealtimePrice, iCommunicationId, iMmiReqId);
	}
}

void CViewData::OnDataPushTick(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushTick *pMmiPushTick)
{
	if (NULL == pMmiPushTick)
	{
		ASSERT(0);
		return;
	}
	
	OnRealtimeTick(*pMmiPushTick, iCommunicationId);
}

void CViewData::OnDataPushLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushLevel2 *pMmiPushLevel2)
{
	if (NULL == pMmiPushLevel2)
	{
		ASSERT(0);
		return;
	}
	
	const CRealtimeLevel2 &RealtimeLevel2 = pMmiPushLevel2->m_RealtimeLevel2;
	OnRealtimeLevel2(RealtimeLevel2, iCommunicationId);
}

void CViewData::OnDataKickOut(int iMmiReqId, int32 iCommunicationId, IN const CMmiKickOut *pMmiKickOut)
{
	if ( NULL == pMmiKickOut )
	{
		ASSERT(0);
		return;
	}

	//
	CString strTempRespJson = pMmiKickOut->m_strRespJson;
	_MYTRACE(L"被踢了:%s\n", strTempRespJson.GetBuffer());
	strTempRespJson.ReleaseBuffer();

	//
	m_bKickOut = true;
	
	// 连接都断掉
	m_pCommManager->StopAllService();
	m_pNewsManager->StopAllNewsCommunication();

	if (m_MainFrameWnd != NULL)
	{
		CString* pStr = new CString(_T("用户在别处登录!"));
		PostMessage(m_MainFrameWnd,UM_MainFrame_KickOut,(WPARAM)pStr,0);
	}
}

void CViewData::OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest)
{
	if ( NULL == pMmiRespNetTest )
	{
		ASSERT(0);
		return;
	}

	// 目前只有优选主站用到这个接口
	COptimizeServer::Instance()->OnDataRespNetTest(iMmiReqId, pMmiRespNetTest);
}

void CViewData::OnDataRespGeneralNormal(int iMmiReqId, IN const CMmiRespGeneralNormal *pMmiRespCMmiRespGeneralNormal)
{
	if ( NULL == pMmiRespCMmiRespGeneralNormal || pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetSize() <= 0 )
	{
		return;
	}

	// 只会有两种情况, 如果数组个数是 1 推送来的最新一笔数据, 多个的话是当天所有的数据
	// 可能当天所有正好是 1 笔数据, 不影响
	bool32 bUpdateLast = false;

	if ( pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetSize() == 1 )
	{
		bUpdateLast = true;
	}

	CMerch* pMerchNotify = NULL;
	
	//
	if ( 0 == pMmiRespCMmiRespGeneralNormal->m_iMarketId )
	{
		// 沪市
		m_MerchManager.FindMerch(L"000001", 0, pMerchNotify);

		int32 iSizeLocal = m_aGeneralNormalH.GetSize();

		//
		if ( iSizeLocal <= 0 || !bUpdateLast )
		{
			m_aGeneralNormalH.Copy(pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal);	
			
			// 上一笔的清空掉
			m_stGeneralNoramlHPre.Clear();
		}
		else
		{
			// 判断一下时间是否合法
			T_GeneralNormal stPushData   = pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetAt(0);
			T_GeneralNormal stLoacalLast = m_aGeneralNormalH.GetAt(iSizeLocal - 1);
		
			//
			long lTimePush	 = stPushData.m_lTime;
			long lTimeLoacal = stLoacalLast.m_lTime;

			//
			CGmtTime GmtPush(lTimePush);
			CGmtTime GmtLocal(lTimeLoacal);

			SaveMinute(GmtPush);
			SaveMinute(GmtLocal);

			lTimePush = (long)GmtPush.GetTime();
			lTimeLoacal = (long)GmtLocal.GetTime();

			//
			if ( lTimePush > lTimeLoacal )
			{
				// 追加
				m_aGeneralNormalH.Add(stPushData);

				// 保存上一笔
				m_stGeneralNoramlHPre = stLoacalLast;
			}
			else if ( lTimePush == lTimeLoacal )
			{
				// 替换
				m_aGeneralNormalH.RemoveAt(iSizeLocal - 1);
				m_aGeneralNormalH.Add(stPushData);

				// 保存上一笔
				m_stGeneralNoramlHPre = stLoacalLast;
			}
			else
			{
				// 不应该发生
				// ASSERT(0);;
				return;
			}
		}		
	}
	else if ( 1000 == pMmiRespCMmiRespGeneralNormal->m_iMarketId )
	{
		// 深市
		m_MerchManager.FindMerch(L"399001", 1000, pMerchNotify);

		int32 iSizeLocal = m_aGeneralNormalS.GetSize();

		//
		if ( iSizeLocal <= 0 || !bUpdateLast )
		{
			m_aGeneralNormalS.Copy(pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal);			

			// 清空上一笔
			m_stGeneralNoramlSPre.Clear();
		}
		else
		{
			// 判断一下时间是否合法
			T_GeneralNormal stPushData   = pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetAt(0);
			T_GeneralNormal stLoacalLast = m_aGeneralNormalS.GetAt(iSizeLocal - 1);

			//
			long lTimePush	 = stPushData.m_lTime;
			long lTimeLoacal = stLoacalLast.m_lTime;
			
			//
			//
			CGmtTime GmtPush(lTimePush);
			CGmtTime GmtLocal(lTimeLoacal);
			
			SaveMinute(GmtPush);
			SaveMinute(GmtLocal);
			
			lTimePush = (long)GmtPush.GetTime();
			lTimeLoacal = (long)GmtLocal.GetTime();
			
			//
			if ( lTimePush > lTimeLoacal )
			{
				// 追加
				m_aGeneralNormalS.Add(stPushData);

				//
				m_stGeneralNoramlSPre = stLoacalLast;
			}
			else if ( lTimePush == lTimeLoacal )
			{
				// 替换
				m_aGeneralNormalS.RemoveAt(iSizeLocal - 1);
				m_aGeneralNormalS.Add(stPushData);

				//
				m_stGeneralNoramlSPre = stLoacalLast;
			}
			else
			{
				// 不应该发生
				// ASSERT(0);;
				return;
			}
		}
	}

	if(m_MainFrameWnd)
	{
		PostMessage(m_MainFrameWnd, UM_ViewData_OnGeneralNormalUpdate, (WPARAM)pMerchNotify, 0);	
	}
	
}

void CViewData::OnDataRespGeneralFiniance(int iMmiReqId, IN const CMmiRespGeneralFinance *pMmiRespGeneralFinance)
{
	if ( NULL == pMmiRespGeneralFinance )
	{
		ASSERT(0);
		return;
	}
	
	//
	CMerch* pMerchNotify = NULL;
	if ( 0 == pMmiRespGeneralFinance->m_iMarketId )
	{
		m_stGeneralFinanceH = pMmiRespGeneralFinance->m_stGeneralFinance;
		m_MerchManager.FindMerch(L"000001", 0, pMerchNotify);
	}
	else if ( 1 == pMmiRespGeneralFinance->m_iMarketId )
	{
		m_stGeneralFinanceS = pMmiRespGeneralFinance->m_stGeneralFinance;
		m_MerchManager.FindMerch(L"399001", 1000, pMerchNotify);
	}

	// 
	PostMessage(m_OwnerWnd, UM_ViewData_OnGeneralFinanaceUpdate, (WPARAM)pMerchNotify, 0);	
}

void CViewData::OnDataRespMerchTrendIndex(int iMmiReqId, IN const CMmiRespMerchTrendIndex *pMmiRespMerchTrendIndex)
{
	if ( NULL == pMmiRespMerchTrendIndex )
	{
		ASSERT(0);
		return;
	}

	//
	CMerch* pMerch = NULL;
	m_MerchManager.FindMerch(pMmiRespMerchTrendIndex->m_StrMerchCode, pMmiRespMerchTrendIndex->m_iMarketId, pMerch);

	if ( NULL == pMerch )
	{
		ASSERT(0);
		return;
	}

	//
	if ( pMerch->OnMerchTrendIndexUpdate(pMmiRespMerchTrendIndex->m_aMerchTrendIndex) )
	{
		// 通知
		PostMessage(m_OwnerWnd, UM_ViewData_OnMerchTrendIndexUpdate, (WPARAM)pMerch, 0);	
	}	
}

void CViewData::OnDataRespMerchAuction(int iMmiReqId, IN const CMmiRespMerchAuction *pMmiRespMerchAuction)
{
	if ( NULL == pMmiRespMerchAuction )
	{
		ASSERT(0);
		return;
	}
	
	//
	CMerch* pMerch = NULL;
	m_MerchManager.FindMerch(pMmiRespMerchAuction->m_StrMerchCode, pMmiRespMerchAuction->m_iMarketId, pMerch);
	
	if ( NULL == pMerch )
	{
		ASSERT(0);
		return;
	}
	
	//
	pMerch->OnMerchAucitonsUpdate(pMmiRespMerchAuction->m_aMerchAuction);

	//
	PostMessage(m_OwnerWnd, UM_ViewData_OnMerchAuctionUpdate, (WPARAM)pMerch, 0);	
}

void CViewData::AbstractFundHoldInfo(uint32 uiDataLen, const char* pData)
{	
	// 为了 crc值和保存脱机数据与服务器传来的一致, 在上层处理这个协议解析
	if ( uiDataLen <= 0 || NULL == pData )
	{
		return;
	}

	//
	m_mapFundHold.clear();

	// 已读的数据长度
	uint32 uiReadLen = 0;

	_MYTRACE(L"解析基金持仓数据, 数据长度: %d", uiDataLen);

	//
	while (1)
	{
		//
		if ( uiReadLen >= uiDataLen )
		{
			break;
		}

		// 商品头部
		T_ClientRespFundHoldMerchHead stMerchHead;
		memcpyex(&stMerchHead, pData, sizeof(stMerchHead));

		uiReadLen += sizeof(stMerchHead);
		pData += sizeof(stMerchHead);

		// 商品数据		
		uint32 uiBodySize = 0;
		memcpyex(&uiBodySize, pData, sizeof(uiBodySize));
		if ( uiBodySize <= 0 )
		{
			ASSERT(0);
			return;
		}

		// 		
		int32	iMarketId = stMerchHead.uiMarket;
		wstring WStrCode;
		if ( !MultiChar2Unicode(1, stMerchHead.szCode, WStrCode) )
		{			
			_MYTRACE(L"解析基金持仓数据, 跳过不能认识代码的商品: %d XXX", iMarketId);
			
			uiReadLen += stMerchHead.uiDataLen;
			pData += stMerchHead.uiDataLen;

			continue;
		}

		//
		CMerch* pMerch = NULL;
		if ( !m_MerchManager.FindMerch(WStrCode.c_str(), iMarketId, pMerch) || NULL == pMerch )
		{			
			_MYTRACE(L"解析基金持仓数据, 跳过不认识的商品: %d %s", iMarketId, WStrCode.c_str());

			uiReadLen += stMerchHead.uiDataLen;
			pData += stMerchHead.uiDataLen;

			continue;		
		}

		//
		int32 iBodyCount = stMerchHead.uiDataLen / uiBodySize;
		T_ClientRespFundHoldMerchBody stMerchBody;

		// _MYTRACE(L"商品 %d %s 有 %d 数据如下:", iMarketId, WStrCode.c_str(), iBodyCount);

		//
		for ( int32 i = 0; i < iBodyCount; i++ )
		{
			//
			if ( uiReadLen >= uiDataLen )
			{
				break;
			}

			memcpyex(&stMerchBody, pData, uiBodySize);

			//
			uiReadLen += uiBodySize;
			pData += uiBodySize;	

			//
			T_FundHold stFundHold;
			
			wstring WStrOrgName;
			if ( !MultiChar2Unicode(1, stMerchBody.szOrgName, WStrOrgName) )
			{
				ASSERT(0);
				continue;
			}
			
			//
			stFundHold.m_StrName		= WStrOrgName.c_str();
			stFundHold.m_fAllPercent	= stMerchBody.fAllPercent;
			stFundHold.m_fFundPercent	= stMerchBody.fFundPercent;
			stFundHold.m_fHold			= stMerchBody.fHold;
			stFundHold.m_fPerCent		= stMerchBody.fPerCent;
			stFundHold.m_fPreHold		= stMerchBody.fPreHold;
			
			//			
			m_mapFundHold[pMerch].insert(stFundHold);

			//
			// _MYTRACE(L"   %s %.2f %.2f %.2f %.2f %.2f\n", stFundHold.m_StrName, 100.f * stFundHold.m_fPerCent, stFundHold.m_fAllPercent, stFundHold.m_fFundPercent, stFundHold.m_fHold, stFundHold.m_fPreHold);
		}
	}

//
 	for ( MapFundHold::const_iterator it = m_mapFundHold.begin(); it != m_mapFundHold.end(); ++it )
 	{
 		CMerch* pMerch = it->first;
 		if ( NULL == pMerch )
 		{
 			continue;
 		}
 
 		_MYTRACE(L"%d %s \n", pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		pMerch->m_MerchInfo.m_StrMerchCode.ReleaseBuffer();
 		
 		//int32 iSize = it->second.size();
 		
 		for ( ListFundHold::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
 		{
 			T_FundHold stFundHold = *it2;
 			_MYTRACE(L"   %s %.2f %.2f %.2f %.2f %.2f\n", stFundHold.m_StrName.GetBuffer(), 100.f * stFundHold.m_fPerCent, stFundHold.m_fAllPercent, stFundHold.m_fFundPercent, stFundHold.m_fHold, stFundHold.m_fPreHold);
 			stFundHold.m_StrName.ReleaseBuffer();
		}
 	}
}

void CViewData::OnDataRespMinuteBS(int iMmiReqId, IN const CMmiRespMinuteBS *pMmiRespMinuteBS)
{
	if ( NULL == pMmiRespMinuteBS )
	{
		ASSERT(0);
		return;
	}

	//
	CMerch* pMerch = NULL;
	m_MerchManager.FindMerch(pMmiRespMinuteBS->m_StrMerchCode, pMmiRespMinuteBS->m_iMarketId, pMerch);
	
	if ( NULL == pMerch )
	{
		ASSERT(0);
		return;
	}
	
	//
	if ( pMerch->OnMerchMinuteBSUpdate(pMmiRespMinuteBS->m_aMinuteBS) )
	{
		// 通知
		PostMessage(m_OwnerWnd, UM_ViewData_OnMerchMinuteBSUpdate, (WPARAM)pMerch, 0);	
	}
}

void CViewData::OnDataRespClientTradeTime(int iMmiReqID, IN const CMmiRespTradeTime* pMmiRespTradeTime)
{
	if (NULL == pMmiRespTradeTime)
	{
		ASSERT(0);
		return;
	}

	m_MerchManager.m_aClientTradeTimeInfo.Copy(pMmiRespTradeTime->m_aClientTradeTimeInfo);
	PostMessage(m_OwnerWnd, UM_ViewData_OnClientTradeTimeUpdate, 0, 0);

//
// 	for ( int32 i = 0; i < m_MerchManager.m_aClientTradeTimeInfo.GetSize(); i++ )
// 	{
// 		int32				iTradeTimeID = m_MerchManager.m_aClientTradeTimeInfo[i].m_iTradeTimeID;
// 		arrClientTradeTime&	aClientTradeTime = m_MerchManager.m_aClientTradeTimeInfo[i].m_aClientTradeTime;
// 
// 		TRACE(L"\niTradeID: %d aClientTradeTime数组(%d):\n", iTradeTimeID, aClientTradeTime.GetSize());
// 		//
// 		for (int32 j = 0; j < aClientTradeTime.GetSize(); j++)
// 		{
// 			int32	m_iWeek = aClientTradeTime[j].m_iWeek;	// 星期几的时间，-6表示星期六到星期天，其它时间表示通用时间
// 			long	m_iOpen = aClientTradeTime[j].m_iOpen;	// 开盘时间
// 			long    m_iClose= aClientTradeTime[j].m_iClose;	// 收盘时间
// 			
// 			TRACE(L"  ===========\n");
// 			TRACE(L"  开收盘时间: iweek: %d iopen: %d iclose: %d\n", m_iWeek, m_iOpen, m_iClose);
// 			TRACE(L"  交易时间节:");
// 
// 			//
// 			for (int32 k = 0; k < aClientTradeTime[j].m_aOCTimes.GetSize(); k++)
// 			{
// 				TRACE(L" open:%d close:%d ", aClientTradeTime[j].m_aOCTimes[k].m_iTimeOpen, aClientTradeTime[j].m_aOCTimes[k].m_iTimeClose);
// 			}
// 		}
// 	}
}

void CViewData::OnDataRespFundHold(int iMmiReqId, IN const CMmiRespFundHold *pMmiRespFundHold)
{
	if ( NULL == pMmiRespFundHold )
	{
		ASSERT(0);
		return;
	}

	//
	if ( pMmiRespFundHold->m_uiDataLen == 0 || NULL == pMmiRespFundHold->m_pData )
	{
		if ( m_uiFundHoldCrc32 != pMmiRespFundHold->m_uiCRC32 )
		{
			// 服务器上的crc值与本地不同，且无任何数据传过来
			// 服务器上除非是没有数据，否则是错误情况了
			// 对于本地文件，选择保留，此本地文件可能对应其它服务器的
			ASSERT( pMmiRespFundHold->m_uiCRC32 == 0 );
			m_mapFundHold.clear();	// 清除数据, 文件不处理
		}
		else if ( m_mapFundHold.empty() )
		{
			CString StrHoldFile = CPathFactory::GetFundHoldFileFullName();
			CStdioFile file;
			if ( file.Open(StrHoldFile, CFile::typeBinary |CFile::modeRead) )
			{
				ULONGLONG ullLength = file.GetLength();
				if (0 != ullLength && ullLength < UINT_MAX) //crude code for read file , modify by weng.cx
				{
					char *pBuf = new char[(uint32)ullLength];
					if ( NULL!=pBuf && file.Read(pBuf, (uint32)ullLength) == (uint32)ullLength )
					{
						AbstractFundHoldInfo((uint32)ullLength, pBuf);
					}
					delete []pBuf;
				}
				
				file.Close();
			}
		}
		m_uiFundHoldCrc32 = pMmiRespFundHold->m_uiCRC32;
		return;	// 服务器根本没有传任何数据，那就不处理任何部分了
	}

	m_uiFundHoldCrc32 = pMmiRespFundHold->m_uiCRC32;
	AbstractFundHoldInfo(pMmiRespFundHold->m_uiDataLen, pMmiRespFundHold->m_pData);

	// ...fangz 0421 保存脱机文件
	unsigned long uTest = CCodeFile::crc_32(pMmiRespFundHold->m_pData, pMmiRespFundHold->m_uiDataLen);
	if (uTest)
	{
		// ....
	}

	CString StrHoldFile = CPathFactory::GetFundHoldFileFullName();
	CStdioFile file;
	if ( file.Open(StrHoldFile, CFile::typeBinary |CFile::modeCreate |CFile::modeWrite) )
	{
		DWORD dwLength = pMmiRespFundHold->m_uiDataLen;	// 不管是0长度还是什么长度
		const int dwMaxLength = 10*1024*1024;
		if ( dwLength <= dwMaxLength ) // 不会超过10mb吧
		{
			file.Write(pMmiRespFundHold->m_pData, dwLength);
		}
		else
		{
			ASSERT( 0 );
		}
		file.Close();
	}
}

void CViewData::OnDataRespPlugInData(int iMmiReqId, IN const CMmiCommBasePlugIn *pMmiCommBase)
{	
	if ( NULL == pMmiCommBase )
	{
		ASSERT(0);
		return;
	}

	switch ( pMmiCommBase->m_eCommTypePlugIn )
	{
	case ECTPIRespMerchIndex:
	case ECTPIRespAddPushMerchIndex:
		{
			// 推送回包跟请求的回包一样的. 只是类型不同
			OnDataRespMerchIndex(iMmiReqId, (const CMmiRespMerchIndex*)pMmiCommBase);
		}
		break;
	case ECTPIRespMainMonitor:
	case ECTPIRespAddPushMainMonitor:
		{
			OnDataRespMainMonitor(iMmiReqId, (const CMmiRespMainMonitor *)pMmiCommBase);
		}
		break;

	case ECTPIRespPeriodTickEx:
	case ECTPIRespTickEx:
	case ECTPIRespAddPushTickEx:
	case ECTPIRespAddPushMerchIndexEx3:
	case ECTPIRespAddPushMerchIndexEx5:
	case ECTPIRespAddPushMerchIndexEx10:
		{
//			OnDataRespTickEx(iMmiReqId, (const CMmiRespTickEx *)pMmiCommBase);
		}
		break;
	case ECTPIRespHistoryTickEx:
		{			
			OnDataRespHistoryTickEx(iMmiReqId, (const CMmiRespHistoryTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIRespBlock:
	case ECTPIRespAddPushBlock:
		{			
			OnDataRespBlock(iMmiReqId, (const CMmiRespLogicBlock *)pMmiCommBase);
		}
		break;
	case ECTPIRespChooseStock:
	case ECTPIRespAddPushChooseStock:
		{
			OnDataRespChooseStock(iMmiReqId, (const CMmiRespChooseStock *)pMmiCommBase);
		}
		break;
	case ECTPIRespCRTEStategyChooseStock:
		{
			OnDataRespCRTEStategyChooseStock(iMmiReqId, (const CMmiRespCRTEStategyChooseStock *)pMmiCommBase);
		}
		break;
	case  ECTPIRespDapanState:
		{
			OnDataRespDapanState(iMmiReqId, (const CMmiRespDapanState *)pMmiCommBase);
		}
		break;
	case  ECTPIRespChooseStockStatus:
		{
			OnDataRespChooseStockStatus(iMmiReqId, (const CMmiRespChooseStockStatus *)pMmiCommBase);
		}
		break;
	case ECTPIRespMerchSort:
		{
			OnDataRespMerchSort(iMmiReqId, (const CMmiRespMerchSort *)pMmiCommBase);
		}
		break;
	case ECTPIRespPeriodMerchSort:
		{
			OnDataRespPeriodMerchSort(iMmiReqId, (const CMmiRespPeriodMerchSort *)pMmiCommBase);
		}
		break;
	case ECTPIRespBlockSort:
		{
			OnDataRespBlockSort(iMmiReqId, (const CMmiRespBlockSort *)pMmiCommBase);
		}
		break;
	case ECTPIRespPushFlag:
		{
			OnDataRespPushFlag(iMmiReqId, (const CMmiRespPlugInPushFlag*)pMmiCommBase);
			return;
		}
		break;
	case ECTPIRespDKMoney:
		{
			OnDataRespDKMoney(iMmiReqId, (const CMmiRespDKMoney*)pMmiCommBase);
		}
		break;
	case ECTPIRespShortMonitor:
	case ECTPIRespAddPushShortMonitor:
		{
			OnDataRespDKMoney(iMmiReqId, (const CMmiRespDKMoney*)pMmiCommBase);
		}
		break;
	case ECTPIRespMainMasukura:
	case ECTPIRespAddPushMainMasukura:
		{
			OnDataRespMainMasukura(iMmiReqId, (const CMmiRespMainMasukura*)pMmiCommBase);
		}	
		break;
	default:
		{
			ASSERT(0);			
		}
		break;
	}


	// plugin视图
	for ( int32 i=0; i < m_IoViewList.GetSize() ; i++ )
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if ( NULL != pIoView && pIoView->IsPluginDataView() && pIoView->IsNowCanReqData() )
		{
			pIoView->OnVDataPluginResp(pMmiCommBase);
		}
	}
}

void CViewData::OnDataRespMerchIndex(int iMmiReqId, IN const CMmiRespMerchIndex *pMmiRespMerchIndex)
{
	if ( NULL == pMmiRespMerchIndex )
	{
		ASSERT(0);
		return;
	}

	// TRACE(((CMmiRespMerchIndex *)pMmiRespMerchIndex)->GetSummary());
}

void CViewData::OnDataRespBlock(int iMmiReqId, IN const CMmiRespLogicBlock *pMmiRespLogicBlock)
{
	if ( NULL == pMmiRespLogicBlock )
	{
		ASSERT(0);
		return;
	}
 
	// TRACE(((CMmiRespLogicBlock *)pMmiRespLogicBlock)->GetSummary());
}

void CViewData::OnDataRespMerchSort(int iMmiReqId, IN const CMmiRespMerchSort *pMmiRespMerchSort)
{
	if ( NULL == pMmiRespMerchSort )
	{
		ASSERT(0);
		return;
	}

	for ( int32 i = 0; i < pMmiRespMerchSort->m_aMerchs.GetSize(); i++ )
	{
		CMerchKey MerchKey = pMmiRespMerchSort->m_aMerchs.GetAt(i);

		if ( pMmiRespMerchSort->m_eReportSortType == ERSPIShort )
		{
			TRACE(L"短线排序回包: %d %s \n", MerchKey.m_iMarketId, MerchKey.m_StrMerchCode.GetBuffer());
			MerchKey.m_StrMerchCode.ReleaseBuffer();
		}
	}

	// CMmiRespMerchSort *pMmiRespMerchSort2 = (CMmiRespMerchSort *)pMmiRespMerchSort;
	// CString StrSummary = pMmiRespMerchSort2->GetSummary();
}

void CViewData::OnDataRespPeriodMerchSort( int iMmiReqId, IN const CMmiRespPeriodMerchSort *pMmiRespMerchSort )
{
	if ( NULL == pMmiRespMerchSort )
	{
		ASSERT(0);
		return;
	}

	for ( int32 i = 0; i < pMmiRespMerchSort->m_aMerchs.GetSize(); i++ )
	{
		CMerchKey MerchKey = pMmiRespMerchSort->m_aMerchs.GetAt(i);

		if ( pMmiRespMerchSort->m_eReportSortType == ERSPIShort )
		{
			TRACE(L"排序回包: %d %s \n", MerchKey.m_iMarketId, MerchKey.m_StrMerchCode);
		}
	}
}

void CViewData::OnDataRespBlockSort(int iMmiReqId, IN const CMmiRespBlockSort *pMmiRespBlockSort)
{
	if ( NULL == pMmiRespBlockSort )
	{
		ASSERT(0);
		return;
	}

	// TRACE(((CMmiRespBlockSort *)pMmiRespBlockSort)->GetSummary());
}

void CViewData::OnDataRespMainMonitor(int iMmiReqId, IN const CMmiRespMainMonitor *pMmiRespMainMonitor)
{
	if ( NULL == pMmiRespMainMonitor )
	{
		ASSERT(0);
		return;
	}
	
	/*
	TRACE(_T("收到主力监控数据 %d 个\r\n"), pMmiRespMainMonitor->m_mapMainMonitorData.size());
	
	for ( mapMainMonitor::const_iterator it = pMmiRespMainMonitor->m_mapMainMonitorData.begin(); it != pMmiRespMainMonitor->m_mapMainMonitorData.end(); ++it )
	{
		T_MainMonitorNode stNode = *it;

		TRACE(L"商品 %d %s\n 的监控数据有%d 个\n", stNode.m_MerchKey.m_iMarketId, stNode.m_MerchKey.m_StrMerchCode, stNode.m_aMainMonitor.size());
		for ( MonitorArray::iterator it2 = stNode.m_aMainMonitor.begin(); it2 != stNode.m_aMainMonitor.end(); ++it2 )
		{
			T_RespMainMonitor stRespMainMonitor = *it2;
			TRACE(L"   时间: %d 价格:%.2f 手数:%.2f 标志:%d\n", stRespMainMonitor.m_Time, stRespMainMonitor.m_fPrice, stRespMainMonitor.m_fCounts, stRespMainMonitor.m_uBuy);
		}
	}
	*/
}

void CViewData::OnDataRespTickEx(int iMmiReqId, IN const CMmiRespTickEx *pMmiRespTickEx)
{
	if ( NULL == pMmiRespTickEx )
	{
		ASSERT(0);
		return;
	}

	for ( int32 i = 0; i < pMmiRespTickEx->m_aTickEx.GetSize(); i++ )
	{
		T_TickEx st = pMmiRespTickEx->m_aTickEx.GetAt(i);
		// TRACE(L"%s TICKEX--VIEWDATA: %.2f \n", st.m_StrMerchCode, st.m_fPriceNew);
	}

	// TRACE(((CMmiRespTickEx *)pMmiRespTickEx)->GetSummary());
}

void CViewData::OnDataRespHistoryTickEx(int iMmiReqId, IN const CMmiRespHistoryTickEx *pMmiRespHistoryTickEx)
{
	if ( NULL == pMmiRespHistoryTickEx )
	{
		ASSERT(0);
		return;
	}
	
	for ( int32 i = 0; i < pMmiRespHistoryTickEx->m_aTickEx.GetSize(); i++ )
	{
		T_TickEx st = pMmiRespHistoryTickEx->m_aTickEx.GetAt(i);
		// TRACE(L"%s TICKEX--VIEWDATA: %.2f \n", st.m_StrMerchCode, st.m_fPriceNew);
	}
	
	// TRACE(((CMmiRespTickEx *)pMmiRespTickEx)->GetSummary());
}

void CViewData::OnDataRespChooseStock(int iMmiReqId, IN const CMmiRespChooseStock *pMmiRespChooseStock)
{
	if ( NULL == pMmiRespChooseStock )
	{
		ASSERT(0);
		return;
	}
	
	if (m_MainFrameWnd != NULL)
	{
		CMmiRespChooseStock *pMmiResp = new CMmiRespChooseStock;
		pMmiResp->m_uTypes = ((CMmiRespChooseStock*)pMmiRespChooseStock)->m_uTypes;
		int32 iMerchSize = ((CMmiRespChooseStock*)pMmiRespChooseStock)->m_aMerchs.GetSize();
		pMmiResp->m_aMerchs.SetSize(0, iMerchSize + 1);
		for (int index =0; index < iMerchSize; index++)
		{
			pMmiResp->m_aMerchs.Add(pMmiRespChooseStock->m_aMerchs[index]);
		}

		PostMessage(m_MainFrameWnd,UM_MainFrame_ChooseStock_Resp,(WPARAM)pMmiResp,0);
	}
}


void CViewData::OnDataRespCRTEStategyChooseStock(int iMmiReqId, IN const CMmiRespCRTEStategyChooseStock *pMmiRespChooseStock)
{
	if ( NULL == pMmiRespChooseStock )
	{
		ASSERT(0);
		return;
	}

	if (m_MainFrameWnd != NULL)
	{
		CMmiRespCRTEStategyChooseStock *pMmiResp = new CMmiRespCRTEStategyChooseStock;
		pMmiResp->m_uTypes = ((CMmiRespCRTEStategyChooseStock*)pMmiRespChooseStock)->m_uTypes;
		int32 iMerchSize = ((CMmiRespCRTEStategyChooseStock*)pMmiRespChooseStock)->m_aMerchs.GetSize();
		pMmiResp->m_aMerchs.SetSize(0, iMerchSize + 1);
		for (int index =0; index < iMerchSize; index++)
		{
			pMmiResp->m_aMerchs.Add(pMmiRespChooseStock->m_aMerchs[index]);
		}

		PostMessage(m_MainFrameWnd,UM_MainFrame_ChooseStock_Resp,(WPARAM)pMmiResp,0);
	}
}

void CViewData::OnDataRespDapanState(int iMmiReqId, IN const CMmiRespDapanState *pMmiRespDapanState)
{
	if ( NULL == pMmiRespDapanState )
	{
		ASSERT(0);
		return;
	}

	if (m_MainFrameWnd != NULL)
	{
		CMmiRespDapanState *pMmiResp = new CMmiRespDapanState;
		pMmiResp->m_eDapanState = ((CMmiRespDapanState*)pMmiRespDapanState)->m_eDapanState;
		
		PostMessage(m_MainFrameWnd,UM_MainFrame_DapanState_Resp,(WPARAM)pMmiResp,0);
	}

//	TRACE(((CMmiRespDapanState *)pMmiRespDapanState)->GetSummary());
}

void CViewData::OnDataRespChooseStockStatus(int iMmiReqId, IN const CMmiRespChooseStockStatus *pMmiRespChooseStockStatus)
{
	if ( NULL == pMmiRespChooseStockStatus )
	{
		ASSERT(0);
		return;
	}

	if (m_MainFrameWnd != NULL)
	{
		CMmiRespChooseStockStatus *pMmiResp = new CMmiRespChooseStockStatus;
		pMmiResp->m_uStatus = ((CMmiRespChooseStockStatus*)pMmiRespChooseStockStatus)->m_uStatus;

		PostMessage(m_MainFrameWnd,UM_MainFrame_ChooseStockStatus_Resp,(WPARAM)pMmiResp,0);
	}
}
 

void CViewData::OnDataRespPushFlag(int iMmiReqId, IN const CMmiRespPlugInPushFlag *pMmiRespPlugInPushFlag)
{
	if ( NULL == pMmiRespPlugInPushFlag )
	{
		ASSERT(0);
		return;
	}
}

void CViewData::OnDataRespDKMoney(int iMmiReqId, IN const CMmiRespDKMoney *pMmiRespDKMoney)
{
	if ( NULL == pMmiRespDKMoney )
	{
		ASSERT(0);
		return;
	}
}

void CViewData::OnDataRespMainMasukura(int iMmiReqId, IN const CMmiRespMainMasukura *pMmiRespMainMasukura)
{
	if ( NULL == pMmiRespMainMasukura )
	{
		ASSERT(0);
		return;
	}
}

// 当某一个市场的某些初始化数据回来
void CViewData::OnSomeInitializeMarketDataRet(CMarket &Market)
{
	if (Market.m_MarketInfoReqState.m_bRequesting || Market.m_MerchListReqState.m_bRequesting)
		return;

	// 在请求队列里清除信息
	set<CMarket*, CmpMarket>::iterator itFind = m_aMarketRequesting.find(&Market);
	if ( itFind != m_aMarketRequesting.end() )
	{
		//
		m_aMarketRequesting.erase(itFind);
		m_iInitializedMarketCount++;		//	记录同步市场数
		//
		_MYTRACE(L"收到市场 %d 的回包\n", Market.m_MarketInfo.m_iMarketId);			
	}
	else
	{
		ASSERT(0);
	}

	// 该市场已经初始化成功
	Market.SetInitialized(true);
	Market.m_TimeLastInitialize = GetServerTime();;
	
	// 如果存在登录框， 通知登录框状态变化
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString;
		pStrTip->Format(L"提示：同步 %s 市场信息完成", Market.m_MarketInfo.m_StrCnName.GetBuffer());
		PostMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);

	}

	// 判断是否全部初始化完成， 是， 则登录过程完成
	if (m_iInitializedMarketCount != m_iAllMarketCount )
	{	
		//	记录市场是否包含股票，期货类型
		CBreed *pBreed = NULL;
		DWORD dwMerchManagerType = EMMT_None;
		for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
		{
			pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
			if (NULL == pBreed)
				continue;

			CMarket *pMarket = NULL;
			for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
			{
				pMarket = pBreed->m_MarketListPtr[iIndexMarket];
				if (NULL == pMarket)
					continue;

				if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTFuturesCn )
				{
					dwMerchManagerType |= EMMT_FutureCn;
				}
				else if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn )
				{
					dwMerchManagerType |= EMMT_StockCn;
				}				
			}
		}

		// 初始化MerchManagerType标志(记录股票，期货类型)
		m_dwMerchManagerType = dwMerchManagerType;		
		// 已经处理完了这批, 开始下一批请求:
		ReqMarketInfo();
	}
	else
	{		
		CSysBlockManager::Instance()->Initialize(&m_MerchManager);
		CUserBlockManager::Instance()->Initialize(m_strUserName,&m_MerchManager);
		CBlockConfig::Instance()->SetViewData(this);
		CBlockConfig::Instance()->RequestLogicBlockFile(this);		// 发出板块文件请求


		// 财富金盾 即使没有股票也发送
		if (NULL != m_LoginWnd /*&& (m_dwMerchManagerType&EMMT_StockCn) == EMMT_StockCn*/ )
		{
			_MYTRACE(L"提示：开始同步板块列表信息");

			CString *pStrTip = new CString(L"提示：开始同步板块列表信息...");
			PostMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);

			// 设置登录超时
			StartTimer(KTimerIdWaitLogicBlockFileTraslateTime, KTimerPeriodLogicBlockFileTraslateTime);
		}
		else
		{
			// 如果没有登录对话框，就用以前的吧
			// 通知所有市场初始化成功 - 等待板块文件初始化完成后，或者超时？
			_MYTRACE(L"跳过同步板块列表，直接初始化市场(%d)", m_dwMerchManagerType);
			OnAllMarketInitializeSuccess();
		}

		ASSERT(m_aMarketRequesting.empty());
		_MYTRACE(L"初始化市场完成!");
	}
	
	//{
	//
	//	// 这种情况发生在隔夜初始化
	//	// 所有与这个市场有关的业务视图统统需要通知ForceUpdate, 对于走势来说， 需要在这个时间点上判断该请求昨天的数据还是今天的数据
	//	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	//	{
	//		
	//		bool32 bAttendView = false;

	//		CIoViewListner *pIoView = m_IoViewList[i];
	//		SmartAttendMerchArray pArray;
	//		pIoView->GetSmartAttendArray(pArray);
	//		for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < pArray.GetSize(); iIndexAttendMerch++)
	//		{
	//			CSmartAttendMerch &SmartAttendMerch = pArray.GetAt(iIndexAttendMerch);
	//			if (Market.m_MarketInfo.m_iMarketId == SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId)
	//			{
	//				bAttendView = true;
	//				break;
	//			}
	//		}

	//		if (bAttendView)
	//		{
	//			// 强制更新业务视图				
	//			pIoView->ForceUpdateVisibleIoView();
	//		}
	//	}

	//	// 看还有没有需要初始化的, 继续请求:
	//	ReqMarketInfo();
	//
	//}
}

// 当所有市场初始化成功后， 仅在软件打开后第一次使用时会产生该事件
void CViewData::OnAllMarketInitializeSuccess()
{
	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(_T("市场初始化完成"), TRUE);
	if (ELSLogining != m_eLoginState)
	{
		ASSERT(0);
		return;
	}
	
	StopTimer(KTimerIdWaitLogicBlockFileTraslateTime);
	
	// 
	m_eLoginState = ELSLoginSuccess;

	// 设置全部初始化成功
	m_MerchManager.SetInitializedAll();

	// 这个需要商品信息, 所以放到这里初始化
	//m_pAlarmCenter = new CAlarmCenter(this);

	// 初始化完毕，场景改变
	m_pCommManager->SetStage(ECSTAGE_QuoteSupplier);
	m_pNewsManager->SetStage(ENSTAGE_NewsSupplier);
	
	if ( NULL != m_LoginWnd)
	{
		SendMessage(m_LoginWnd,UM_ViewData_OnAllMarketInitializeSuccess, 0, 0);
	}
	
	// 向所有业务视图发送强制刷新事件
	int32 i;
	for ( i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->ForceUpdateVisibleIoView();
	}

	// 向服务器发请求状态栏几个商品的报价
	CArray<CMerchKey, CMerchKey&> aMerchs;
	for (i = 0; i < m_StatucBarAttendMerchs.GetSize(); i++)
	{
		CMerchKey MerchKey;
		MerchKey.m_StrMerchCode = m_StatucBarAttendMerchs[i].m_pMerch->m_MerchInfo.m_StrMerchCode;
		MerchKey.m_iMarketId	= m_StatucBarAttendMerchs[i].m_pMerch->m_MerchInfo.m_iMarketId;

		aMerchs.Add(MerchKey);
	}

	if (aMerchs.GetSize() > 0)
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId		= aMerchs[0].m_iMarketId;
		Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
			
		aMerchs.RemoveAt(0, 1);
		Req.m_aMerchMore.Copy(aMerchs);

		RequestViewData((CMmiCommBase *)&Req);
	}
	
	// 装载相关商品
	CString StrRelativeMerchsFilePath	= CPathFactory::GetPublicConfigPath() + CPathFactory::GetRelativeMerchsFileName();
	m_RelativeMerchManager.m_RelativeMerchList.RemoveAll();
	CRelativeMerchManager::FromXml(StrRelativeMerchsFilePath, m_RelativeMerchManager.m_RelativeMerchList);

	CString StrHkWarrantFilePath		= CPathFactory::GetPublicConfigPath() + CPathFactory::GetHkWarrantFileName();
	m_RelativeMerchManager.m_HkWarrantList.RemoveAll();
	CRelativeMerchManager::FromXml(StrHkWarrantFilePath, m_RelativeMerchManager.m_HkWarrantList);

	
	// 更新一次基金数据	
// 	unsigned long uiCrc32 = 0;
// 	CMmiReqFundHold ReqFundHold;
// 	ReqFundHold.m_uiCRC32 = uiCrc32;
// 	RequestViewData(&ReqFundHold);

	//
	SaveMarketInfo();
}

void CViewData::OnViewReqAuth(IN const CMmiReqAuth *pMmiReqAuth, IN int32 iCommunicationID /*= -1*/)
{
	if ( m_bKickOut )
	{
		return;
	}

	if ( NULL == pMmiReqAuth || -1 == iCommunicationID )
	{
		ASSERT(0);
		return;
	}

	// 向服务器发认证请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqAuth, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqBase(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID /* = -1 */,IN int eDataSource /* = EDSAuto */)
{
	if (NULL == pMmiCommBase)
	{
		ASSERT(0);
		return;
	}
	
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData(pMmiCommBase, aMmiReqNodes, (E_DataSource)eDataSource,iCommunicationID);

}

void CViewData::OnViewReqMarketInfo(IN const CMmiReqMarketInfo *pMmiReqMarketInfo)
{
	if (NULL == pMmiReqMarketInfo)
	{
		ASSERT(0);
		return;
	}

	// 请求某一个市场的基本信息
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMarketInfo->m_iMarketId, pMarket))
	{
		return;		// 找不到对应的市场
	}
	
	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMarketInfo, aMmiReqNodes);

	// 根据通讯id， 设置状态
	int32 iCommunicationId = -1;
	if (iRet > 0)
	{
		if (aMmiReqNodes.GetSize() == 1)
		{
			CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
			if (NULL != pMmiReqNodeOut)
			{
				iCommunicationId = pMmiReqNodeOut->m_iCommunicationId;
			}
			else
			{
				ASSERT(0);		// 不应该发生这种情况
			}
		}
		else
		{
			ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
		}
	}

	if (iCommunicationId < 0)
	{
		pMarket->m_MarketInfoReqState.m_bRequesting			= false;
		pMarket->m_MarketInfoReqState.m_iCommunicationId	= -1;
	}
	else
	{
		pMarket->m_MarketInfoReqState.m_bRequesting			= true;
		pMarket->m_MarketInfoReqState.m_iCommunicationId	= iCommunicationId;
	}
}

void CViewData::OnViewReqMarketSnapshot(IN const CMmiReqMarketSnapshot *pMmiReqMarketSnapshot)
{
	if (NULL == pMmiReqMarketSnapshot)
	{
		ASSERT(0);
		return;
	}

	// 请求某一个市场的基本信息
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMarketSnapshot->m_iMarketId, pMarket))
	{
		// 找不到对应的市场
		return;
	}
	
	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMarketSnapshot, aMmiReqNodes);
}

void CViewData::OnViewReqBroker(IN const CMmiReqBroker *pMmiReqBroker)
{
	if (NULL == pMmiReqBroker)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的大市场
	CBreed *pBreed = NULL;
	int32 iPosBreed = 0;
	if (!m_MerchManager.FindBreed(pMmiReqBroker->m_iBreedId, iPosBreed, pBreed))
	{
		// 找不到对应的大市场
		return;
	}

	// 暂时设置所有的
	CBroker *pBroker = NULL;
	for (int32 iIndexBroker = 0; iIndexBroker < pBreed->m_BrokersPtr.GetSize(); iIndexBroker++)
	{
		pBroker = pBreed->m_BrokersPtr[iIndexBroker];
		if (NULL == pBroker)
			continue;

		pBroker->m_bRemoved = true;
	}
	
	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqBroker, aMmiReqNodes);
}

void CViewData::OnViewReqMerchInfo(IN const CMmiReqMerchInfo *pMmiReqMerchInfo)
{ 
	if (NULL == pMmiReqMerchInfo)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的市场
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMerchInfo->m_iMarketId, pMarket))
	{		
		return;	// 找不到对应的市场
	}

	// 清所有的标志
//	RGUARD(LockSingle, pMarket->m_LockMerchList, LockMerchList);
	{
		CMerch *pMerch = NULL;
		for (int32 iIndexMerch = 0; iIndexMerch < pMarket->m_MerchsPtr.GetSize(); iIndexMerch++)
		{
			pMerch = pMarket->m_MerchsPtr[iIndexMerch];
			if (NULL == pMerch)
				continue;

			pMerch->m_MerchInfo.m_bRemoved = true;
		}
	}
	
	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchInfo, aMmiReqNodes);

	// 根据通讯id， 设置状态
	int32 iCommunicationId = -1;
	if (iRet > 0)
	{
		if (aMmiReqNodes.GetSize() == 1)
		{
			CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
			if (NULL != pMmiReqNodeOut)
			{
				iCommunicationId = pMmiReqNodeOut->m_iCommunicationId;
			}
			else
			{
				ASSERT(0);		// 不应该发生这种情况
			}
		}
		else
		{
			ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
		}
	}

	if (iCommunicationId < 0)
	{
		pMarket->m_MerchListReqState.m_bRequesting			= false;
		pMarket->m_MerchListReqState.m_iCommunicationId	= -1;
	}
	else
	{
		pMarket->m_MerchListReqState.m_bRequesting			= true;
		pMarket->m_MerchListReqState.m_iCommunicationId	= iCommunicationId;
	}
}

void CViewData::OnViewReqPublicFile(IN const CMmiReqPublicFile *pMmiReqPublicFile)
{ 
	if (NULL == pMmiReqPublicFile)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqPublicFile->m_StrMerchCode, pMmiReqPublicFile->m_iMarketId, pMerch))
	{		
		return;	// 找不到对应的商品
	}
	
	if (EPFTWeight == pMmiReqPublicFile->m_ePublicFileType)
	{
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedReqWeightData(TimeServer))	// 是否需要更新除权数据
		{
			return;	// 不需要更新除权数据
		}
	}
	else if ( EPFTF10 == pMmiReqPublicFile->m_ePublicFileType )
	{
		// fangz 0818
		// xl 1104 财务数据2小时后发送请求
		// 获取本地财务数据，填充crc32值


		CString StrFilePath;
		const_cast<CMmiReqPublicFile *>(pMmiReqPublicFile)->m_uiCRC32 = 0;
		if ( m_OfflineDataManager.GetPublishFilePath(EPFTF10, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath) )
		{
			// 读数据:
			CFile file;
			ULONGLONG ullDataLen;
			char acBuffer[1024*5];
			memset(acBuffer, 0, sizeof(acBuffer));

			if ( file.Open(StrFilePath, CFile::modeRead | CFile::typeBinary) )				
			{
				if ( (ullDataLen = file.GetLength()) > 0 && (file.Read(acBuffer, (uint32)ullDataLen)) == (uint32)ullDataLen )
				{
					// 文件的CRC32 值
					const_cast<CMmiReqPublicFile *>(pMmiReqPublicFile)->m_uiCRC32 = CCodeFile::crc_32(acBuffer, (uint32)ullDataLen);
					
					// 处理数据
					if ( NULL == pMerch->m_pFinanceData )
					{
						pMerch->m_pFinanceData = new CFinanceData();
					}
					if ( CMerch::ReadFinanceDataFromBuffer(acBuffer, (int32)ullDataLen, *pMerch->m_pFinanceData) )    
					{
						// 给与一次虚假的更新
						PostMessage(m_OwnerWnd, UM_ViewData_OnPublicFileUpdate, (WPARAM)pMerch, EPFTF10);	
					}
				}

				file.Close();
			}			
		}
		
		CGmtTime TimeNow = GetServerTime();
		if ( !pMerch->DoesNeedReqFinanceData(TimeNow) )
		{
			return;	// 无需请求财务数据 - 静态数据更新是否应当采取为有请求必响应方式？？
		}
	}
	else
	{	
		// 不支持的类型
		ASSERT(0);
		return;
	}

	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqPublicFile, aMmiReqNodes);
}



void CViewData::OnViewReqMerchExtendData(IN const CMmiReqMerchExtendData *pMmiReqMerchExtendData)
{
	if (NULL == pMmiReqMerchExtendData)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchExtendData->m_StrMerchCode, pMmiReqMerchExtendData->m_iMarketId, pMerch))
	{
		// 找不到对应的商品
		return;
	}

	// 看是否已经存在扩展数据
	if (NULL != pMerch->m_pMerchExtendData)
	{
		// 已经存在数据
		return;
	}

	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchExtendData, aMmiReqNodes);
}

void CViewData::OnViewReqMerchF10(IN const CMmiReqMerchF10 *pMmiReqMerchF10)
{
	if (NULL == pMmiReqMerchF10)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchF10->m_StrMerchCode, pMmiReqMerchF10->m_iMarketId, pMerch))
	{
		// 找不到对应的商品
		return;
	}

	// 看是否已经存在F10数据
	if (NULL != pMerch->m_pMerchF10)
	{
		// 已经存在数据
		return;
	}

	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchF10, aMmiReqNodes);
}

void CViewData::OnViewReqRealtimePrice(IN const CMmiReqRealtimePrice *pMmiReqRealtimePrice)
{	
	if (NULL == pMmiReqRealtimePrice)
	{
		ASSERT(0);
		return;
	}

	// 逐个检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	CArray<CMerchKey, CMerchKey&> aMerchs;

	CString StrMerchCode;
	int32	iMarketId;
	for (int32 i = 0; i < pMmiReqRealtimePrice->m_aMerchMore.GetSize() + 1; i++)
	{
		CMerchKey MerchKey;

		if (0 == i)
		{
			StrMerchCode	= pMmiReqRealtimePrice->m_StrMerchCode;
			iMarketId		= pMmiReqRealtimePrice->m_iMarketId;
		}
		else
		{
			StrMerchCode	= pMmiReqRealtimePrice->m_aMerchMore[i - 1].m_StrMerchCode;
			iMarketId		= pMmiReqRealtimePrice->m_aMerchMore[i - 1].m_iMarketId;
		}

		MerchKey.m_iMarketId	= iMarketId;
		MerchKey.m_StrMerchCode	= StrMerchCode;

		// 
		bool32 bIgnoreRequest = false;

		// 查看是否有指定的商品
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// 先从推送队列中查找看是否有推送
		if ( NULL != pMerch->m_pRealtimePrice && pMerch->DoesNeedPushPrice(GetServerTime()) )
		{
			// 仅当开盘期间，且本地有实时报价数据的情况下在检查是否要过滤
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimePrices.find(pMerch);
			if (itFind != m_aPushingRealtimePrices.end())	// 找到， 表示正在推送
			{
				bIgnoreRequest = true;
			}	
		}
		
		// 
		if (bIgnoreRequest)
		{
			// 异步方式通知所有关心该数据的业务视图 - ！！此时有可能没有real time price数据
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);
		}
		else
		{
			// 增加一个商品
			aMerchs.Add(MerchKey);
		}
	}
	

	if ( m_bOffLine )
	{
		if ( aMerchs.GetSize() > 0 )
		{
			CMmiReqRealtimePrice Req;
			Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
			Req.m_iMarketId		= aMerchs[0].m_iMarketId;
			aMerchs.RemoveAt(0, 1);
			
			// 
			Req.m_aMerchMore.Copy(aMerchs);

			// 向服务器发请求
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSOfflineData);
			if (iRet)
			{
				// ...
			}
		}		
	}
	else
	{
		// 发请求，以市场区分多个包发送
		if (aMerchs.GetSize() > 0)
		{
			while (aMerchs.GetSize() > 0)
			{
				// 重新组织请求， 因请求的数据有可能会有改变
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(aMerchs[0].m_iMarketId, EDSTPrice);
				
				if (!m_bDirectToQuote && iServiceId < 0)	// 找不到对应的服务， 说明暂时没有服务器支持该类型数据
				{
					aMerchs.RemoveAt(0, 1);
					continue;
				}
				
				CMmiReqRealtimePrice Req;
				Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
				Req.m_iMarketId		= aMerchs[0].m_iMarketId;
				aMerchs.RemoveAt(0, 1);
				
				int32 i;
				for ( i = aMerchs.GetSize() -1; i >= 0; i--)
				{
					int32 iServiceIdCur = 0;//m_pServiceDispose->GetServiceId(aMerchs[i].m_iMarketId, EDSTPrice);
					if(!m_bDirectToQuote)
					{
						if (iServiceIdCur < 0)	// 找不到对应的服务， 说明暂时没有服务器支持该类型数据
						{
							aMerchs.RemoveAt(i, 1);
						}
						else if (iServiceIdCur == iServiceId)	// 同一个数据服务
						{
							Req.m_aMerchMore.Add(aMerchs[i]);
							aMerchs.RemoveAt(i, 1);
						}
						else
						{
							// NULL;
						}
					}					
				}
				
				// 向服务器发请求
				CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
				int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);
				
				// 根据通讯id， 设置状态
				int32 iCommunicationId = -1;
				if (iRet > 0)
				{
					if (aMmiReqNodes.GetSize() == 1)
					{
						CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
						if (NULL != pMmiReqNodeOut)
						{
							iCommunicationId = pMmiReqNodeOut->m_iCommunicationId;
						}
						else
						{
							ASSERT(0);;		// 不应该发生这种情况
						}
					}
					else
					{
						ASSERT(0);;	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
					}
				}
				
				// 
				if (iCommunicationId < 0)
				{
					// ASSERT(0);;	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
				}
				else
				{
					T_PushServerAndTime stPushServerAndTime;
					stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
					stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
					
					// 增加推送记录
					for (i = 0; i < Req.m_aMerchMore.GetSize() + 1; i++)
					{ 
						CMerch *pMerch = NULL;
						if (0 == i)
						{
							m_MerchManager.FindMerch(Req.m_StrMerchCode, Req.m_iMarketId, pMerch);
						}
						else
						{
							CMerchKey &MerchKey = Req.m_aMerchMore[i - 1];
							m_MerchManager.FindMerch(MerchKey.m_StrMerchCode, MerchKey.m_iMarketId, pMerch);
						}
						
						if (NULL != pMerch)
						{
							
							m_aPushingRealtimePrices[pMerch] = stPushServerAndTime;
						}
					}
				}
			}
		}
		else
		{
			// NULL;
		}
	}
	
} 

void CViewData::OnViewRegisterPushPrice(IN const CMmiRegisterPushPrice *pMmiRegisterPushPrice)
{
	if (NULL == pMmiRegisterPushPrice)
	{
		ASSERT(0);;
		return;
	}

	if (m_bDirectToQuote)
	{
		// 直连行情, 就这一个服务器, 不做乱七八糟的判断
		CString StrMerchCode;
		int32	iMarketId;

		CMmiRegisterPushPrice Req;
		Req.m_iMarketId = -1;
	
		for (int32 i = 0; i < pMmiRegisterPushPrice->m_aMerchMore.GetSize() + 1; i++)
		{
			if (0 == i)
			{
				StrMerchCode	= pMmiRegisterPushPrice->m_StrMerchCode;
				iMarketId		= pMmiRegisterPushPrice->m_iMarketId;
			}
			else
			{
				StrMerchCode	= pMmiRegisterPushPrice->m_aMerchMore[i - 1].m_StrMerchCode;
				iMarketId		= pMmiRegisterPushPrice->m_aMerchMore[i - 1].m_iMarketId;
			}
			
			// 
			//bool32 bIgnoreRequest = false;
			
			// 查看是否有指定的商品
			CMerch *pMerch = NULL;
			if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
				continue;
			
			// 过滤收盘后的不必要的请求
			CGmtTime TimeServer = GetServerTime();
			if (!pMerch->DoesNeedPushPrice(TimeServer))	// 是否是最新的数据， 如果是， 则不需要请求， 直接通知相关视图去刷新
			{
				continue;
			}

			//
			if (-1 == Req.m_iMarketId)
			{
				Req.m_iMarketId = iMarketId;
				Req.m_StrMerchCode = StrMerchCode;
			}
			else
			{
				CMerchKey Mk;
				Mk.m_iMarketId = iMarketId;
				Mk.m_StrMerchCode = StrMerchCode;

				Req.m_aMerchMore.Add(Mk);
			}
		}

		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSCommunication);
		if (iRet)
		{
			// ....
		}

		return;
	}
	
	// 如果已经有正在推送的数据， 则向原新的服务器注册该商品的推送， 避免出现这种情况：A服务器正在推送x商品, 此时又向B服务器注册x商品推送
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimePrices = m_aPushingRealtimePrices;

	// 清空买卖盘推送信息
	m_aPushingRealtimePrices.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// 逐个检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	CArray<T_MerchAndCommunication, T_MerchAndCommunication&> aMerchs;

	CString StrMerchCode;
	int32	iMarketId;
	for (int32 i = 0; i < pMmiRegisterPushPrice->m_aMerchMore.GetSize() + 1; i++)
	{
		if (0 == i)
		{
			StrMerchCode	= pMmiRegisterPushPrice->m_StrMerchCode;
			iMarketId		= pMmiRegisterPushPrice->m_iMarketId;
		}
		else
		{
			StrMerchCode	= pMmiRegisterPushPrice->m_aMerchMore[i - 1].m_StrMerchCode;
			iMarketId		= pMmiRegisterPushPrice->m_aMerchMore[i - 1].m_iMarketId;
		}

		// 
		bool32 bIgnoreRequest = false;

		// 查看是否有指定的商品
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// 过滤收盘后的不必要的请求
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushPrice(TimeServer))	// 是否是最新的数据， 如果是， 则不需要请求， 直接通知相关视图去刷新
		{
			bIgnoreRequest = true;
		}
		else
		{
			// NULL;
		}
		
		// 
		if (bIgnoreRequest)
		{
			// NULL;	// 不需要请求
		}
		else
		{
			// 增加一个商品推送
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// 从当前推送列表中查找， 看是否已经在推送列表中了， 如果已经在推送中了， 那就仍然使用当前推送的服务器注册， 继续推送， 避免出现两台服务器推送同一个商品的同一个类型的数据
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimePrices.find(pMerch);
			if (itFind == aPushingRealtimePrices.end())	// 没有找到该推送， 则重新查找一个能提供该商品实时行情推送的服务器
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTPrice);
				if (iServiceId >= 0)	// 可以找到对应的服务， 找到能提供该服务的所有服务器
				{
					if (!bHasInitServiceList)
					{
						m_pCommManager->GetActiveServiceList(aActiveServiceList);
						bHasInitServiceList = true;
					}

					//
					T_TinyServiceInfo *pTinyServices = (T_TinyServiceInfo *)aActiveServiceList.GetData();
					for (int32 iIndexService = 0; iIndexService < aActiveServiceList.GetSize(); iIndexService++)
					{
						if (pTinyServices[iIndexService].m_iServiceId == iServiceId)
						{
							if (pTinyServices[iIndexService].m_aCommunicationIds.GetSize() > 0)
							{
								// zhangbo 20090917 #待优化， 这里暂时就总是取提供该服务的第一台服务器， 后面应该取用更优解， 比如更快
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// 找到了， 就继续使用之前的服务器
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// 不要添加重复商品
				bool32 bExist = false;

				for ( int32 j = 0; j < aMerchs.GetSize(); j++ )
				{
					if ( MerchAndCommunication.m_pMerch == aMerchs.GetAt(j).m_pMerch )
					{
						bExist = true;
					}
				}

				if ( !bExist )
				{
					aMerchs.Add(MerchAndCommunication);
				}				
			}
		}
	}
	
	// 发请求，以市场区分多个包发送
	if (aMerchs.GetSize() > 0)
	{
		while (aMerchs.GetSize() > 0)
		{
			CArray<CMerch*, CMerch*> aMerchsSameServer;

			// 
			int32 iCommunicationId = aMerchs[aMerchs.GetSize() - 1].m_iCommunicationId;

			// 
			aMerchsSameServer.Add(aMerchs[aMerchs.GetSize() - 1].m_pMerch);
			aMerchs.RemoveAt(aMerchs.GetSize() - 1, 1);

			int32 i;
			for ( i = aMerchs.GetSize() -1; i >= 0; i--)
			{
				if (aMerchs[i].m_iCommunicationId == iCommunicationId)
				{
					aMerchsSameServer.Add(aMerchs[i].m_pMerch);
					aMerchs.RemoveAt(i, 1);
				}
			}

			// 向服务器发请求
			CMmiRegisterPushPrice Req;
			Req.m_iMarketId		= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_StrMerchCode;

			aMerchsSameServer.RemoveAt(aMerchsSameServer.GetSize() - 1, 1);
			for (i = 0; i < aMerchsSameServer.GetSize(); i++)
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= aMerchsSameServer[i]->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = aMerchsSameServer[i]->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}

			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSAuto, iCommunicationId);

			// 根据通讯id， 设置状态
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);// 不应该发生这种情况, 下层没有按指定的服务器发送请求
						}
					}
					else
					{
						ASSERT(0);	// 不应该发生这种情况
					}
				}
				else
				{
					ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// 增加推送记录
				for (int32 j = 0; j < aMerchsSameServer.GetSize(); j++)
				{ 
					CMerch *pMerch = aMerchsSameServer[j];
					m_aPushingRealtimePrices[pMerch] = stPushServerAndTime;
				}
			}
		}
	}
	else
	{			
		if ( aPushingRealtimePrices.empty() )
		{
			// 原来有推送, 现在不需要推送了, 这时候要取消推送, 不能什么事都不干
			CArray<int32, int32> aCommDealed;

			for ( std::map<CMerch*, T_PushServerAndTime>::iterator it = aPushingRealtimePrices.begin(); it != aPushingRealtimePrices.end(); ++it )
			{
				int32 iCommunicationId = it->second.m_iCommunicationId;

				bool32 bDealed = false;

				for ( int32 i = 0; i < aCommDealed.GetSize(); i++ )
				{
					if ( iCommunicationId == aCommDealed.GetAt(i) )
					{
						bDealed = true;
						break;
					}
				}

				//
				if ( !bDealed )
				{
					// 发一个空的推送, 取消掉现在的
					CMmiRegisterPushPrice Req;
					
					Req.m_iMarketId = -1;
					Req.m_StrMerchCode = L"";
					Req.m_aMerchMore.RemoveAll();

					//
					CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
					int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSAuto, iCommunicationId);
					if (iRet)
					{
						// ....
					}

					//
					aCommDealed.Add(iCommunicationId);
				}
			}
		}	
	}
}

void CViewData::OnViewReqRealtimeTick(IN const CMmiReqRealtimeTick *pMmiReqRealtimeTick)
{
	if (NULL == pMmiReqRealtimeTick)
	{
		ASSERT(0);
		return;
	}
	
	// 逐个检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	CArray<CMerchKey, CMerchKey&> aMerchs;

	CString StrMerchCode;
	int32	iMarketId;
	for (int32 i = 0; i < pMmiReqRealtimeTick->m_aMerchMore.GetSize() + 1; i++)
	{
		if (0 == i)
		{
			StrMerchCode	= pMmiReqRealtimeTick->m_StrMerchCode;
			iMarketId		= pMmiReqRealtimeTick->m_iMarketId;
		}
		else
		{
			StrMerchCode	= pMmiReqRealtimeTick->m_aMerchMore[i - 1].m_StrMerchCode;
			iMarketId		= pMmiReqRealtimeTick->m_aMerchMore[i - 1].m_iMarketId;
		}

		// 
		bool32 bIgnoreRequest = false;

		// 查看是否有指定的商品
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// 先从推送队列中查找看是否有推送
		if ( NULL != pMerch->m_pRealtimeTick && pMerch->DoesNeedPushTick(GetServerTime()) )
		{
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeTicks.find(pMerch);
			if (itFind != m_aPushingRealtimeTicks.end())	// 找到， 表示正在推送
			{
				bIgnoreRequest = true;
			}
		}

		// 
		if (bIgnoreRequest)
		{
			// 异步方式通知所有关心该数据的业务视图
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeTickUpdate, (WPARAM)pMerch, 0);
		}
		else
		{
			// 增加一个商品
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= iMarketId;
			MerchKey.m_StrMerchCode	= StrMerchCode;
			
			aMerchs.Add(MerchKey);
		}
	}
	
	// 发请求，以市场区分多个包发送
	if (aMerchs.GetSize() > 0)
	{
		while (aMerchs.GetSize() > 0)
		{
			// 重新组织请求， 因请求的数据有可能会有改变
			int32 iServiceId = 0;// m_pServiceDispose->JudgeServiceId(aMerchs[0].m_StrMerchCode, aMerchs[0].m_iMarketId, EDSTTick);
			if (iServiceId < 0)	// 找不到对应的服务， 说明暂时没有服务器支持该类型数据
				continue;
			
			CMmiReqRealtimeTick Req;
			Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
			Req.m_iMarketId		= aMerchs[0].m_iMarketId;
			aMerchs.RemoveAt(0, 1);

			int32 i;
			for ( i = aMerchs.GetSize() -1; i >= 0; i--)
			{
				int32 iServiceIdCur = 0;// m_pServiceDispose->JudgeServiceId(aMerchs[i].m_StrMerchCode, aMerchs[i].m_iMarketId, EDSTTick);
				if (iServiceIdCur < 0)	// 找不到对应的服务， 说明暂时没有服务器支持该类型数据
				{
					aMerchs.RemoveAt(i, 1);
				}
				else if (iServiceIdCur == iServiceId)	// 同一个数据服务
				{
					Req.m_aMerchMore.Add(aMerchs[i]);
					aMerchs.RemoveAt(i, 1);
				}
				else
				{
					// NULL;
				}
			}

			// 向服务器发请求
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);

			// 根据通讯id， 设置状态
			int32 iCommunicationId = -1;
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						iCommunicationId = pMmiReqNodeOut->m_iCommunicationId;
					}
					else
					{
						ASSERT(0);		// 不应该发生这种情况
					}
				}
				else
				{
					ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
				}
			}
			
			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
				
				// 增加推送记录
				for (i = 0; i < Req.m_aMerchMore.GetSize() + 1; i++)
				{ 
					CMerch *pMerch = NULL;
					if (0 == i)
					{
						m_MerchManager.FindMerch(Req.m_StrMerchCode, Req.m_iMarketId, pMerch);
					}
					else
					{
						CMerchKey &MerchKey = Req.m_aMerchMore[i - 1];
						m_MerchManager.FindMerch(MerchKey.m_StrMerchCode, MerchKey.m_iMarketId, pMerch);
					}
					
					if (NULL != pMerch)
					{
						
						m_aPushingRealtimeTicks[pMerch] = stPushServerAndTime;
					}
				}
			}
		}
	}
	else
	{
		// NULL;
	}
} 

void CViewData::OnViewRegisterPushTick(IN const CMmiRegisterPushTick *pMmiRegisterPushTick)
{
	if (NULL == pMmiRegisterPushTick)
	{
		ASSERT(0);
		return;
	}

	if (m_bDirectToQuote)
	{
		// 直连行情, 就这一个服务器, 不做乱七八糟的判断
		CString StrMerchCode;
		int32	iMarketId;
		
		CMmiRegisterPushTick Req;
		Req.m_iMarketId = -1;
		
		for (int32 i = 0; i < pMmiRegisterPushTick->m_aMerchMore.GetSize() + 1; i++)
		{
			if (0 == i)
			{
				StrMerchCode	= pMmiRegisterPushTick->m_StrMerchCode;
				iMarketId		= pMmiRegisterPushTick->m_iMarketId;
			}
			else
			{
				StrMerchCode	= pMmiRegisterPushTick->m_aMerchMore[i - 1].m_StrMerchCode;
				iMarketId		= pMmiRegisterPushTick->m_aMerchMore[i - 1].m_iMarketId;
			}
			
			// 
			
			// 查看是否有指定的商品
			CMerch *pMerch = NULL;
			if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
				continue;
			
			// 过滤收盘后的不必要的请求
			CGmtTime TimeServer = GetServerTime();
			if (!pMerch->DoesNeedPushPrice(TimeServer))	// 是否是最新的数据， 如果是， 则不需要请求， 直接通知相关视图去刷新
			{
				continue;
			}
			
			//
			if (-1 == Req.m_iMarketId)
			{
				Req.m_iMarketId = iMarketId;
				Req.m_StrMerchCode = StrMerchCode;
			}
			else
			{
				CMerchKey Mk;
				Mk.m_iMarketId = iMarketId;
				Mk.m_StrMerchCode = StrMerchCode;
				
				Req.m_aMerchMore.Add(Mk);
			}
		}
		
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSCommunication);
		
		return;
	}
	
	// 如果已经有正在推送的数据， 则向原新的服务器注册该商品的推送， 避免出现这种情况：A服务器正在推送x商品, 此时又向B服务器注册x商品推送
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimeTicks = m_aPushingRealtimeTicks;

	// 清空买卖盘推送信息
	m_aPushingRealtimeTicks.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// 逐个检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	CArray<T_MerchAndCommunication, T_MerchAndCommunication&> aMerchs;

	CString StrMerchCode;
	int32	iMarketId;
	for (int32 i = 0; i < pMmiRegisterPushTick->m_aMerchMore.GetSize() + 1; i++)
	{
		if (0 == i)
		{
			StrMerchCode	= pMmiRegisterPushTick->m_StrMerchCode;
			iMarketId		= pMmiRegisterPushTick->m_iMarketId;
		}
		else
		{
			StrMerchCode	= pMmiRegisterPushTick->m_aMerchMore[i - 1].m_StrMerchCode;
			iMarketId		= pMmiRegisterPushTick->m_aMerchMore[i - 1].m_iMarketId;
		}

		// 
		bool32 bIgnoreRequest = false;

		// 查看是否有指定的商品
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// 过滤收盘后的不必要的请求
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushTick(TimeServer))	// 是否是最新的数据， 如果是， 则不需要请求， 直接通知相关视图去刷新
		{
			bIgnoreRequest = true;
		}
		else
		{
			// NULL;
		}
		
		// 
		if (bIgnoreRequest)
		{
			// NULL;	// 不需要请求
		}
		else
		{
			// 增加一个商品推送
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// 从当前推送列表中查找， 看是否已经在推送列表中了， 如果已经在推送中了， 那就仍然使用当前推送的服务器注册， 继续推送， 避免出现两台服务器推送同一个商品的同一个类型的数据
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimeTicks.find(pMerch);
			if (itFind == aPushingRealtimeTicks.end())	// 没有找到该推送， 则重新查找一个能提供该商品实时行情推送的服务器
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTTick);
				if (iServiceId >= 0)	// 可以找到对应的服务， 找到能提供该服务的所有服务器
				{
					if (!bHasInitServiceList)
					{
						m_pCommManager->GetActiveServiceList(aActiveServiceList);
						bHasInitServiceList = true;
					}

					//
					T_TinyServiceInfo *pTinyServices = (T_TinyServiceInfo *)aActiveServiceList.GetData();
					for (int32 iIndexService = 0; iIndexService < aActiveServiceList.GetSize(); iIndexService++)
					{
						if (pTinyServices[iIndexService].m_iServiceId == iServiceId)
						{
							if (pTinyServices[iIndexService].m_aCommunicationIds.GetSize() > 0)
							{
								// zhangbo 20090917 #待优化， 这里暂时就总是取提供该服务的第一台服务器， 后面应该取用更优解， 比如更快
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// 找到了， 就继续使用之前的服务器
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// 不要添加重复商品
				bool32 bExist = false;
				
				for ( int32 j = 0; j < aMerchs.GetSize(); j++ )
				{
					if ( MerchAndCommunication.m_pMerch == aMerchs.GetAt(j).m_pMerch )
					{
						bExist = true;
					}
				}
				
				if ( !bExist )
				{
					aMerchs.Add(MerchAndCommunication);
				}	
			}
		}
	}
	
	// 发请求，以市场区分多个包发送
	if (aMerchs.GetSize() > 0)
	{
		while (aMerchs.GetSize() > 0)
		{
			CArray<CMerch*, CMerch*> aMerchsSameServer;

			// 
			int32 iCommunicationId = aMerchs[aMerchs.GetSize() - 1].m_iCommunicationId;

			// 
			aMerchsSameServer.Add(aMerchs[aMerchs.GetSize() - 1].m_pMerch);
			aMerchs.RemoveAt(aMerchs.GetSize() - 1, 1);

			int32 i;
			for ( i = aMerchs.GetSize() -1; i >= 0; i--)
			{
				if (aMerchs[i].m_iCommunicationId == iCommunicationId)
				{
					aMerchsSameServer.Add(aMerchs[i].m_pMerch);
					aMerchs.RemoveAt(i, 1);
				}
			}

			// 向服务器发请求
			CMmiRegisterPushTick Req;
			Req.m_iMarketId		= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_StrMerchCode;

			aMerchsSameServer.RemoveAt(aMerchsSameServer.GetSize() - 1, 1);
			for (i = 0; i < aMerchsSameServer.GetSize(); i++)
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= aMerchsSameServer[i]->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = aMerchsSameServer[i]->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}

			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSAuto, iCommunicationId);

			// 根据通讯id， 设置状态
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);	// 不应该发生这种情况, 下层没有按指定的服务器发送请求
						}
					}
					else
					{
						ASSERT(0);		// 不应该发生这种情况
					}
				}
				else
				{
					ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// 增加推送记录
				for (int32 j = 0; j < aMerchsSameServer.GetSize(); j++)
				{ 
					CMerch *pMerch = aMerchsSameServer[j];
					m_aPushingRealtimeTicks[pMerch] = stPushServerAndTime;
				}
			}
		}
	}
	else
	{
		// NULL;
	}
}


void CViewData::OnViewRegisterPushLevel2(IN const CMmiRegisterPushLevel2 *pMmiRegisterPushLevel2)
{
	if (NULL == pMmiRegisterPushLevel2)
	{
		ASSERT(0);
		return;
	}
	
	// 如果已经有正在推送的数据， 则向原新的服务器注册该商品的推送， 避免出现这种情况：A服务器正在推送x商品, 此时又向B服务器注册x商品推送
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimeLevel2s = m_aPushingRealtimeLevel2s;

	// 清空买卖盘推送信息
	m_aPushingRealtimeLevel2s.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// 逐个检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	CArray<T_MerchAndCommunication, T_MerchAndCommunication&> aMerchs;

	CString StrMerchCode;
	int32	iMarketId;
	for (int32 i = 0; i < pMmiRegisterPushLevel2->m_aMerchMore.GetSize() + 1; i++)
	{
		if (0 == i)
		{
			StrMerchCode	= pMmiRegisterPushLevel2->m_StrMerchCode;
			iMarketId		= pMmiRegisterPushLevel2->m_iMarketId;
		}
		else
		{
			StrMerchCode	= pMmiRegisterPushLevel2->m_aMerchMore[i - 1].m_StrMerchCode;
			iMarketId		= pMmiRegisterPushLevel2->m_aMerchMore[i - 1].m_iMarketId;
		}

		// 
		bool32 bIgnoreRequest = false;

		// 查看是否有指定的商品
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// 过滤收盘后的不必要的请求
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushLevel2(TimeServer))	// 是否是最新的数据， 如果是， 则不需要请求， 直接通知相关视图去刷新
		{
			bIgnoreRequest = true;
		}
		else
		{
			// NULL;
		}
		
		// 
		if (bIgnoreRequest)
		{
			// NULL;	// 不需要请求
		}
		else
		{
			// 增加一个商品推送
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// 从当前推送列表中查找， 看是否已经在推送列表中了， 如果已经在推送中了， 那就仍然使用当前推送的服务器注册， 继续推送， 避免出现两台服务器推送同一个商品的同一个类型的数据
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimeLevel2s.find(pMerch);
			if (itFind == aPushingRealtimeLevel2s.end())	// 没有找到该推送， 则重新查找一个能提供该商品实时行情推送的服务器
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTLevel2);
				if (iServiceId >= 0)	// 可以找到对应的服务， 找到能提供该服务的所有服务器
				{
					if (!bHasInitServiceList)
					{
						m_pCommManager->GetActiveServiceList(aActiveServiceList);
						bHasInitServiceList = true;
					}

					//
					T_TinyServiceInfo *pTinyServices = (T_TinyServiceInfo *)aActiveServiceList.GetData();
					for (int32 iIndexService = 0; iIndexService < aActiveServiceList.GetSize(); iIndexService++)
					{
						if (pTinyServices[iIndexService].m_iServiceId == iServiceId)
						{
							if (pTinyServices[iIndexService].m_aCommunicationIds.GetSize() > 0)
							{
								// zhangbo 20090917 #待优化， 这里暂时就总是取提供该服务的第一台服务器， 后面应该取用更优解， 比如更快
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// 找到了， 就继续使用之前的服务器
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// 不要添加重复商品
				bool32 bExist = false;
				
				for ( int32 j = 0; j < aMerchs.GetSize(); j++ )
				{
					if ( MerchAndCommunication.m_pMerch == aMerchs.GetAt(j).m_pMerch )
					{
						bExist = true;
					}
				}
				
				if ( !bExist )
				{
					aMerchs.Add(MerchAndCommunication);
				}	
			}
		}
	}
	
	// 发请求，以市场区分多个包发送
	if (aMerchs.GetSize() > 0)
	{
		while (aMerchs.GetSize() > 0)
		{
			CArray<CMerch*, CMerch*> aMerchsSameServer;

			// 
			int32 iCommunicationId = aMerchs[aMerchs.GetSize() - 1].m_iCommunicationId;

			// 
			aMerchsSameServer.Add(aMerchs[aMerchs.GetSize() - 1].m_pMerch);
			aMerchs.RemoveAt(aMerchs.GetSize() - 1, 1);

			int32 i;
			for ( i = aMerchs.GetSize() -1; i >= 0; i--)
			{
				if (aMerchs[i].m_iCommunicationId == iCommunicationId)
				{
					aMerchsSameServer.Add(aMerchs[i].m_pMerch);
					aMerchs.RemoveAt(i, 1);
				}
			}

			// 向服务器发请求
			CMmiRegisterPushLevel2 Req;
			Req.m_iMarketId		= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	= aMerchsSameServer[aMerchsSameServer.GetSize() - 1]->m_MerchInfo.m_StrMerchCode;

			aMerchsSameServer.RemoveAt(aMerchsSameServer.GetSize() - 1, 1);
			for (i = 0; i < aMerchsSameServer.GetSize(); i++)
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= aMerchsSameServer[i]->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = aMerchsSameServer[i]->m_MerchInfo.m_StrMerchCode;
				Req.m_aMerchMore.Add(MerchKey);
			}

			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes, EDSAuto, iCommunicationId);

			// 根据通讯id， 设置状态
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);	// 不应该发生这种情况, 下层没有按指定的服务器发送请求
						}
					}
					else
					{
						ASSERT(0);		// 不应该发生这种情况
					}
				}
				else
				{
					ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// 增加推送记录
				for (int32 j = 0; j < aMerchsSameServer.GetSize(); j++)
				{ 
					CMerch *pMerch = aMerchsSameServer[j];
					m_aPushingRealtimeLevel2s[pMerch] = stPushServerAndTime;
				}
			}
		}
	}
	else
	{
		// NULL;
	}
}



void CViewData::OnViewReqNetTest(IN const CMmiReqNetTest *pMmiReqNetTest, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqNetTest )
	{
		ASSERT(0);
		return;
	}

	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqNetTest, aMmiReqNodes, EDSCommunication, iCommunicationID);	
}

void CViewData::OnViewReqGeneralNormal(IN const CMmiReqGeneralNormal *pMmiReqGeneralNormal, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqGeneralNormal )
	{
		ASSERT(0);
		return;
	}

	if ( pMmiReqGeneralNormal->m_iMarketId != 0 && pMmiReqGeneralNormal->m_iMarketId != 1000 )
	{
		ASSERT(0);
		return;
	}

	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqGeneralNormal, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqGeneralFiniance(IN const CMmiReqGeneralFinance *pMmiReqGeneralFinance, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqGeneralFinance )
	{
		ASSERT(0);
		return;
	}
	
	if ( pMmiReqGeneralFinance->m_iMarketId != 0 && pMmiReqGeneralFinance->m_iMarketId != 1000 )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqGeneralFinance, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqMerchTrendIndex(IN const CMmiReqMerchTrendIndex *pMmiReqMerchTrendIndex, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqMerchTrendIndex )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchTrendIndex, aMmiReqNodes, EDSCommunication, iCommunicationID);	
}

void CViewData::OnViewReqMerchAuction(IN const CMmiReqMerchAution *pMmiReqMerchAution, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqMerchAution )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchAution, aMmiReqNodes, EDSCommunication, iCommunicationID);	
}

void CViewData::OnViewReqAddPushGeneralNormal(IN const CMmiReqAddPushGeneralNormal *pMmiReqAddPushGeneralNormal, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqAddPushGeneralNormal )
	{
		ASSERT(0);
		return;
	}
	
	if ( pMmiReqAddPushGeneralNormal->m_iMarketId != 0 && pMmiReqAddPushGeneralNormal->m_iMarketId != 1000 )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqAddPushGeneralNormal, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqUpdatePushGeneralNormal(IN const CMmiReqUpdatePushGeneralNormal* pMmiReqUpdatePushGeneralNormal, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqUpdatePushGeneralNormal )
	{
		ASSERT(0);
		return;
	}
	
	if ( pMmiReqUpdatePushGeneralNormal->m_iMarketId != 0 && pMmiReqUpdatePushGeneralNormal->m_iMarketId != 1000 )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqUpdatePushGeneralNormal, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqDelPushGeneralNormal(IN const CMmiReqDelPushGeneralNormal* pMmiReqDelPushGeneralNormal, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqDelPushGeneralNormal )
	{
		ASSERT(0);
		return;
	}
	
	if ( pMmiReqDelPushGeneralNormal->m_iMarketId != 0 && pMmiReqDelPushGeneralNormal->m_iMarketId != 1000 )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqDelPushGeneralNormal, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqAddPushMerchTrendIndex(IN const CMmiReqAddPushMerchTrendIndex *pMmiReqAddPushMerchTrendIndex, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqAddPushMerchTrendIndex )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqAddPushMerchTrendIndex, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqUpdatePushMerchTrendIndex(IN const CMmiReqUpdatePushMerchTrendIndex *pMmiReqUpdatePushMerchTrendIndex, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqUpdatePushMerchTrendIndex )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqUpdatePushMerchTrendIndex, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqDelPushMerchTrendIndex(IN const CMmiReqDelPushMerchTrendIndex *pMmiReqDelPushMerchTrendIndex, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqDelPushMerchTrendIndex )
	{
		ASSERT(0);
		return;
	}
	
	// 向服务器发测速请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqDelPushMerchTrendIndex, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqFundHold(IN const CMmiReqFundHold *pMmiReqFundHold, IN int32 iCommunicationID)
{	if ( NULL == pMmiReqFundHold )
	{
		ASSERT(0);
		return;
	}

	// 从本地读取保存的fundhold内容，计算crc
	// 如果外面设置了crc值，则必须考虑将这个自动改写crc的功能去掉
	ASSERT( pMmiReqFundHold->m_uiCRC32 == 0 );
	
	if ( m_mapFundHold.empty() )
	{
		CString StrHoldFile = CPathFactory::GetFundHoldFileFullName();
		CStdioFile file;
		if ( file.Open(StrHoldFile, CFile::typeBinary |CFile::modeRead) )
		{
			ULONGLONG ullLength = file.GetLength();
			if (ullLength != 0 && ullLength < UINT_MAX)
			{
				char *pBuf = new char[(uint32)ullLength];
				if ( NULL!=pBuf && file.Read(pBuf, (uint32)ullLength) == (uint32)ullLength )
				{
					// 选择在此处加载数据，则可能在数据不一致时导致先显示一阵错误的数据，然后在更新成正确的数据
					// AbstractFundHoldInfo(dwLength, pBuf);
					m_uiFundHoldCrc32 =  CCodeFile::crc_32(pBuf, (int32)ullLength);
				}
				delete []pBuf;
			}
			
			file.Close();
		}
	}

	const_cast<CMmiReqFundHold *>(pMmiReqFundHold)->m_uiCRC32 = m_uiFundHoldCrc32;	// 使用保存的crc32

	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqFundHold, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqMinuteBS(IN const CMmiReqMinuteBS *pMmiReqMinuteBS, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqMinuteBS )
	{
		ASSERT(0);
		return;
	}
	
	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMinuteBS, aMmiReqNodes, EDSAuto, iCommunicationID);
}


void CViewData::OnViewReqAddPushMinuteBS(IN const CMmiReqAddPushMinuteBS *pMmiReqAddPushMinuteBS, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqAddPushMinuteBS )
	{
		ASSERT(0);
		return;
	}
	
	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqAddPushMinuteBS, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqUpdatePushMinuteBS(IN const CMmiReqUpdatePushMinuteBS *pMmiReqUpdatePushMinuteBS, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqUpdatePushMinuteBS )
	{
		ASSERT(0);
		return;
	}
	
	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqUpdatePushMinuteBS, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqDelPushMinuteBS(IN const CMmiReqDelPushMinuteBS *pMmiReqDelPushMinuteBS, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqDelPushMinuteBS )
	{
		ASSERT(0);
		return;
	}
	
	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqDelPushMinuteBS, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnReqClientTradeTime(IN const CMmiReqTradeTime* pMmiReqTradeTime, IN int32 iCommunicationID)
{
	if ( NULL == pMmiReqTradeTime )
	{
		ASSERT(0);
		return;
	}

	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqTradeTime, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnReqAuthPlugInHeart(IN const CMmiReqAuthPlugInHeart* pMimiComBase, IN int32 iCommunicationID)
{
	if (NULL == pMimiComBase)
	{
		ASSERT(0);
		return;
	}

	//
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMimiComBase, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnMsgKLineNodeUpdate(IN CMerch *pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTKLine) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataMerchKLineUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgTimesaleUpdate(IN CMerch *pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTTimeSale) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataMerchTimeSalesUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTGeneral) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataPublicFileUpdate(pMerch, ePublicFileType);
		}
	}
}

void CViewData::OnMsgRealtimePriceUpdate(IN CMerch *pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataRealtimePriceUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgRealtimeTickUpdate(IN CMerch *pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTTick) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataRealtimeTickUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgClientTradeTimeUpdate()
{
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (NULL != pIoView && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataClientTradeTimeUpdate();
		}
	}
}

void CViewData::OnMsgRealtimeLevel2Update(IN CMerch *pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTLevel2) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataRealtimeLevel2Update(pMerch);
		}
	}
}

void CViewData::OnMsgNewsListUpdate()
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if ( NULL != pIoView  && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataNewsListUpdate();
		}
	}
}

void CViewData::OnMsgF10Update(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];	
		if ( NULL != pIoView )
		{
			pIoView->OnVDataF10Update();
		}
	}
}

void CViewData::OnMsgLandMineUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendNews(pMerch) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataLandMineUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgOnGeneralNormalUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataGeneralNormalUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgOnGeneralFinanaceUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataGeneralFinanaceUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgOnMerchTrendIndexUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataMerchTrendIndexUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgOnMerchAuctionUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataMerchAuctionUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgOnMerchMinuteBSUpdate(IN CMerch* pMerch)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (pIoView->IsAttendData(pMerch, EDSTPrice) && pIoView->IsNowCanReqData())
		{
			pIoView->OnVDataMerchMinuteBSUpdate(pMerch);
		}
	}
}

void CViewData::OnMsgAuthPlugInHeartResp(int iRet)
{
	if(0 == iRet)
	{
		//
		TRACE(L"插件认证心跳返回正常\n");
	}
	else
	{
		if (m_MainFrameWnd)
		{
			CString* pStr = new CString(GetPlusErrorStr(iRet));
			PostMessage(m_MainFrameWnd,UM_MainFrame_KickOut,(WPARAM)pStr,0);
		}
	}
}

void CViewData::OnWeightTypeChange()
{
	// 通只关心该数据的业务视图(其实只有K 线)
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner* pIoView = m_IoViewList[i];
		
		if ( NULL == pIoView )
		{			
			continue;
		}
		else
		{
			
			pIoView->OnWeightTypeChange();				
		}
	}
}

void CViewData::OnViewReqRealtimeLevel2(IN const CMmiReqRealtimeLevel2 *pMmiReqRealtimeLevel2)
{
	if (NULL == pMmiReqRealtimeLevel2)
	{
		ASSERT(0);
		return;
	}
	
	// 检查商品是否需要向服务器请求数据， 尽量减少服务器的通讯量
	// 
	bool32 bIgnoreRequest = false;

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqRealtimeLevel2->m_StrMerchCode, pMmiReqRealtimeLevel2->m_iMarketId, pMerch))
		return;

	// 先从推送队列中查找看是否有推送
	if ( NULL != pMerch->m_pRealtimeLevel2 && pMerch->DoesNeedPushLevel2(GetServerTime()) )
	{
		std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeLevel2s.find(pMerch);
		if (itFind != m_aPushingRealtimeLevel2s.end())	// 找到， 表示正在推送
		{
			bIgnoreRequest = true;
		}
	}

	// 
	if (bIgnoreRequest)
	{
		// 异步方式通知所有关心该数据的业务视图
		PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeLevel2Update, (WPARAM)pMerch, 0);
		return;
	}
		
	// 发请求
	int32 iServiceId = 0; 
	if (iServiceId < 0)	// 找不到对应的服务， 说明暂时没有服务器支持该类型数据
		return;

	// 重新组织请求， 因请求的数据有可能会有改变
	CMmiReqRealtimeLevel2 Req;
	Req.m_StrMerchCode	= pMmiReqRealtimeLevel2->m_StrMerchCode;
	Req.m_iMarketId		= pMmiReqRealtimeLevel2->m_iMarketId;
				
	// 向服务器发请求
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);
	
	// 根据通讯id， 设置状态
	int32 iCommunicationId = -1;
	if (iRet > 0)
	{
		if (aMmiReqNodes.GetSize() == 1)
		{
			CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
			if (NULL != pMmiReqNodeOut)
			{
				iCommunicationId = pMmiReqNodeOut->m_iCommunicationId;
			}
			else
			{
				ASSERT(0);		// 不应该发生这种情况
				return;
			}
		}
		else
		{
			ASSERT(0);	// 不应该发生这种情况，服务器应该立即发送该请求， 并且返回服务器信息
			return;
		}
	}
	
	// 
	if (iCommunicationId < 0)
	{
		ASSERT(0);	// 找不到可以提供指定数据服务的服务器， 可能是没有权限， 或者是服务器连不上
		return;
	}
	else
	{
		T_PushServerAndTime stPushServerAndTime;
		stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
		stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
		
		// 增加推送记录
		pMerch = NULL;
		m_MerchManager.FindMerch(Req.m_StrMerchCode, Req.m_iMarketId, pMerch);
		if (NULL != pMerch)
		{			
			m_aPushingRealtimeLevel2s[pMerch] = stPushServerAndTime;
		}
	}
}

void CViewData::OnViewReqMerchKLine(IN const CMmiReqMerchKLine *pMmiReqMerchKLine)
{
	if (NULL == pMmiReqMerchKLine)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_iMarketId, pMerch))
	{
		// 找不到对应的商品
		return;
	}

	if (pMmiReqMerchKLine->m_eKLineTypeBase != EKTBMinute && 
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTB5Min &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBHour &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBDay &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBMonth)
	{
		// k线类型不对
		return;
	}

	CString StrLog;
	if ( ERTYSpecifyTime == pMmiReqMerchKLine->m_eReqTimeType )
	{
		StrLog.Format(L"[Client==>] CViewData::请求历史K 线: %s", ((CMmiReqMerchKLine *)pMmiReqMerchKLine)->GetSummary().GetBuffer());
	}
	else if ( ERTYFrontCount == pMmiReqMerchKLine->m_eReqTimeType )
	{
		StrLog.Format(L"[Client==>] CViewData::向前请求历史K 线 %d 根", pMmiReqMerchKLine->m_iFrontCount);
	}
	else 
	{
		StrLog.Format(L"[Client==>] CViewData::请求历史K 线 %d 根", pMmiReqMerchKLine->m_iCount);
	}
	
	//_LogCheckTime(StrLog, g_hwndTrace);

	// 查找是否有指定类型的K线
	int32 iPosKLineFound = 0;
	CMerchKLineNode *pMerchKLineNodeFound = NULL;
	if (!pMerch->FindMerchKLineNode(pMmiReqMerchKLine->m_eKLineTypeBase, iPosKLineFound, pMerchKLineNodeFound) || pMerchKLineNodeFound->m_KLines.GetSize() <= 0)
	{
		// 向数据中心请求数据
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchKLine, aMmiReqNodes);
	}
	else
	{
		// zhangbo 0625 #待优化
		// 不对数据做判断, 这里做判断的话， 不能很好的判断数据是否连续的， 将会导致可能的数据不连续从而引发的不必要的错误，
		// 但同时也会带来性能上的问题, 因为对大部分情况来说， 这种连续性的考虑都是多余的， 给到datamanager后， 又会做很多查找之类的操作， 浪费了性能
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchKLine, aMmiReqNodes);
	}
	
	return;
}

void CViewData::OnViewReqMerchTimeSales(IN const CMmiReqMerchTimeSales *pMmiReqMerchTimeSales)
{
	if (NULL == pMmiReqMerchTimeSales)
	{
		ASSERT(0);
		return;
	}

	// 查看是否有指定的商品
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchTimeSales->m_StrMerchCode, pMmiReqMerchTimeSales->m_iMarketId, pMerch))
	{
		// 找不到对应的商品
		return;
	}

	// 
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchTimeSales, aMmiReqNodes);
}

void CViewData::OnViewReqReport(IN const CMmiReqReport *pMmiReqReport)
{
	if (NULL == pMmiReqReport)
	{
		ASSERT(0);
		return;
	}

	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqReport, aMmiReqNodes);
}

void CViewData::OnViewReqBlockReport(IN const CMmiReqBlockReport *pMmiReqBlockReport)
{
	if ( NULL == pMmiReqBlockReport )
	{
		ASSERT(0);
		return;
	}

	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqBlockReport, aMmiReqNodes);
}

void CViewData::BuildMerchHotKeyList (CArray<CHotKey, CHotKey&>&	arrHotKeys)
{

	//
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		CBreed *pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		if (NULL == pBreed)
			continue;

		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			CMarket *pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (NULL == pMarket)
				continue;

			for (int32 iIndexMerch = 0; iIndexMerch < pMarket->m_MerchsPtr.GetSize(); iIndexMerch++)
			{
				CMerch *pMerch = pMarket->m_MerchsPtr[iIndexMerch];
				if (NULL == pMerch)
					continue;

				CMerchInfo *pMerchInfo = &pMerch->m_MerchInfo;
				
				CArray<CString, CString&>	HotKeyList;
// 				if (pMerchInfo->m_StrMerchCodeInBourse.GetLength() > 0)		// 交易所代码做索引
// 					HotKeyList.Add(pMerchInfo->m_StrMerchCodeInBourse);
				if (pMerchInfo->m_StrMerchCode.GetLength() > 0)				// pobo代码做索引
					HotKeyList.Add(pMerchInfo->m_StrMerchCode);
// 				if (pMerchInfo->m_StrHotKey.GetLength() > 0)				// 热键做索引
// 					HotKeyList.Add(pMerchInfo->m_StrHotKey);

				// 将重复的过滤掉， 加快后面插入的速度
				int32 i;
				for ( i = 0; i < HotKeyList.GetSize() - 1; i++)
				{
					for (int32 j = HotKeyList.GetSize() - 1; j >= i + 1; j--)
					{
						if (HotKeyList[i] == HotKeyList[j])
						{
							HotKeyList.RemoveAt(j, 1);
						}
					}
				}

				// 
				for (i = 0; i < HotKeyList.GetSize(); i++)
				{
					CHotKey HotKey;
					
					HotKey.m_eHotKeyType	= EHKTMerch;
					HotKey.m_StrKey			= HotKeyList[i];
					HotKey.m_StrSummary		= pMerchInfo->m_StrMerchCnName;
					HotKey.m_iParam1		= pMerchInfo->m_iMarketId;
					HotKey.m_StrParam1		= pMerchInfo->m_StrMerchCode;
					HotKey.m_StrParam2		= ConvertHZToPY(pMerchInfo->m_StrMerchCnName);

					//AddHotKey(HotKey);
					arrHotKeys.Add(HotKey);
				}
			}	
		}		
	}	
}

void CViewData::FindRelativeMerchs(IN CMerch *pMerch, OUT CArray<CRelativeMerchNode, CRelativeMerchNode&> &AllRelativeMerchs)
{
	AllRelativeMerchs.RemoveAll();



	// 用该商品在相关商品列表中查找相关的商品
	CArray<CRelativeMerch, CRelativeMerch&> RelativeList;
	m_RelativeMerchManager.FindRelativeMerchs(pMerch, m_MerchManager, CRelativeMerch::ERTWarrant | CRelativeMerch::ERTAB | CRelativeMerch::ERTAH, RelativeList);
	if (RelativeList.GetSize() > 0)
	{
		CRelativeMerchNode RelativeMerchNode;
		RelativeMerchNode.m_RelativeSummary = L"相关商品";

		CSiblingMerch SiblingMerch;
		for (int32 i = 0; i < RelativeList.GetSize(); i++)
		{
			//
			CRelativeMerch &RelativeMerch = RelativeList[i];
			if (CRelativeMerch::EVSNormal != RelativeMerch.m_eVerifyState && CRelativeMerch::EVSExchange != RelativeMerch.m_eVerifyState)
			{
				ASSERT(0);
				continue;
			}

			if (NULL == RelativeMerch.m_pMerchSrc || NULL == RelativeMerch.m_pMerchDst)
			{
				ASSERT(0);
				continue;
			}

			//
			bool32 bSrc = false;
			CMerch *pMerchRelative = NULL;
			if (pMerch == RelativeMerch.m_pMerchSrc)
			{
				bSrc = false;
				pMerchRelative = RelativeMerch.m_pMerchDst;
			}
			else if (pMerch == RelativeMerch.m_pMerchDst)
			{
				bSrc = true;
				pMerchRelative = RelativeMerch.m_pMerchSrc;
			}
			else
			{
				ASSERT(0);
				continue;
			}

			if (CRelativeMerch::EVSExchange == RelativeMerch.m_eVerifyState)
				bSrc = !bSrc;
			
			// 
			SiblingMerch.m_pMerch		= pMerchRelative;
			
			if (CRelativeMerch::ERTWarrant == RelativeMerch.m_eRelativeType)
			{
				if (bSrc)
					SiblingMerch.m_StrSiblingName = L"标的股";
				else
					SiblingMerch.m_StrSiblingName = L"权证";
			}
			else if (CRelativeMerch::ERTAB == RelativeMerch.m_eRelativeType)
			{
				if (bSrc)
					SiblingMerch.m_StrSiblingName = L"A股";
				else
					SiblingMerch.m_StrSiblingName = L"B股";
			}
			else if (CRelativeMerch::ERTAH == RelativeMerch.m_eRelativeType)
			{
				if (bSrc)
					SiblingMerch.m_StrSiblingName = L"A股";
				else
					SiblingMerch.m_StrSiblingName = L"H股";
			}
			else
			{
				ASSERT(0);
				continue;
			}

			RelativeMerchNode.m_SiblingMerchList.Add(SiblingMerch);
		}

		// 
		if (RelativeMerchNode.m_SiblingMerchList.GetSize() > 0)
		{
			AllRelativeMerchs.Add(RelativeMerchNode);
		}
	}

}

CGmtTime CViewData::GetServerTime()
{

	if ( !m_bOffLine )
	{
		return m_TimeServerNow;
	}

	// 脱机的时候, 取本地时间
	SYSTEMTIME st = {0};
	GetLocalTime(&st);

	struct tm tm0 = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0 };
	time_t time = mktime(&tm0);

	//
	CGmtTime timeLocal(time);

	return timeLocal;
}

CGmtTime CViewData::GetMarketCurrentTime(const CMarket &Market)
{
	CGmtTime TimeServer = GetServerTime();

	CGmtTimeSpan TimeSpan(0, Market.m_MarketInfo.m_iTimeZone, 0, 0);
	
	return (TimeServer + TimeSpan);
}

void CViewData::OnRelativeMerchsFileUpdate()
{
	CString StrRelativeMerchsFile = CPathFactory::GetPublicConfigPath() + CPathFactory::GetRelativeMerchsFileName();

	// 清掉现在的港股权证以外的所有的关系
	m_RelativeMerchManager.m_RelativeMerchList.RemoveAll();

	// 从现在的文件中读取关系
	CRelativeMerchManager::FromXml(StrRelativeMerchsFile, m_RelativeMerchManager.m_RelativeMerchList);

	// 通知所有视图相关商品发生变化了
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		m_IoViewList[i]->OnVDataRelativeMerchsUpdate();
	}
}

void CViewData::OnHkWarrantsFileUpdate()
{
	CString StrHkWarrantFile = CPathFactory::GetPublicConfigPath() + CPathFactory::GetHkWarrantFileName();

	// 清掉现在的港股权证
	m_RelativeMerchManager.m_HkWarrantList.RemoveAll();

	// 从现在的文件中读取关系
	CRelativeMerchManager::FromXml(StrHkWarrantFile, m_RelativeMerchManager.m_HkWarrantList);

	// 通知所有视图相关商品发生变化了
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		m_IoViewList[i]->OnVDataRelativeMerchsUpdate();
	}
}

void CViewData::SetIndexChsStkMideCore(CIndexChsStkMideCore* pMidCore)
{	
	if ( NULL == pMidCore )
	{
		// 停止选股		
		m_pIndexChsStkMideCore = NULL;
		m_aRecvDataForIndexChsStk.RemoveAll();	
		
		m_iNumsOfIndexChsStkReq			= -1;
		m_iNumsOfIndexChsStkHaveRecved	= 0;
	}
	else
	{
		// 开始选股		
		m_pIndexChsStkMideCore = pMidCore;
	}
}

void CViewData::SetIndexChsStkReqNums(int32 iNumReqs)
{
	RGUARD(LockSingle, m_LockNumsOfIndexChsStkReq, LockNumsOfIndexChsStkReq);
	m_iNumsOfIndexChsStkReq = iNumReqs;
}

bool32 CViewData::GetRespMerchKLineForIndexChsStk(OUT CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>& RecvDataForIndexChsStk)
{	
	if ( NULL == m_pIndexChsStkMideCore )
	{
		return false;
	}
	
	RGUARD(LockSingle, m_LockRecvDataForIndexChsStk, LockRecvDataForIndexChsStk);
	RecvDataForIndexChsStk.RemoveAll();

	RecvDataForIndexChsStk.Copy(m_aRecvDataForIndexChsStk);
	m_aRecvDataForIndexChsStk.RemoveAll();

	return true;
}

bool32 CViewData::CheckLogQuoteConnect(int32 iCommunicationId)
{
	if ( !m_bLogWaitQuoteConnect )
	{
		ASSERT(0);
		return false;
	}

//	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == m_pServiceDispose)
	{
		return false;
	}

	if ( NULL != m_LoginWnd )
	{
		//pDoc->m_pDlgLogIn->SetPromptInfo(L"正在连接行情服务器...");
	}

	//
	T_ServerInfo* pServerInfo;
	int iCountServers;
	m_pServiceDispose->GetServerList(&pServerInfo, iCountServers);
	
	int32 iCountQuoteServers = 0;
	
	// 设置值
	for ( int32 i = 0; i < iCountServers; i++ )
	{
		if ( CheckFlag (pServerInfo[i].enType, enSTQuote) )
		{	
			iCountQuoteServers += 1;
		}				
	}

	// 登录时
	bool32 bAllConnect	  = false;
	bool32 bAllNonConnect = false;

	if ( -1 == iCommunicationId )
	{
		// 如果传入参数是 -1, 返回是否全部没连接上的bool32 值
		if ( iCountQuoteServers == m_aQuoteServerAll.GetSize() )
		{
			bAllNonConnect = true;
		}

		// 直接返回
		m_pServiceDispose->ReleaseData(pServerInfo);
		return bAllNonConnect;

		_MYTRACE(L"所有的服务器都没有连接上!");
	}
	else
	{
		//
		for ( int32 i = m_aQuoteServerAll.GetSize() -1; i >= 0; i-- )
		{
			T_ServerInfo stServerInfo = m_aQuoteServerAll.GetAt(i);
			
			// 看这个
			int32 iID = m_pCommManager->GetCommunicationID(stServerInfo.wszAddr, stServerInfo.iPort);
			if ( iID == iCommunicationId )
			{
				m_aQuoteServerAll.RemoveAt(i);
				_MYTRACE(L"未连接服务器数组剔除服务器: %d, 还有 %d 个未连接成功服务器", iID, m_aQuoteServerAll.GetSize());
				break;
			}
		}
	}

	//
	m_pServiceDispose->ReleaseData(pServerInfo);

	if ( 0 == m_aQuoteServerAll.GetSize() )
	{
		// 都连接上了
		bAllConnect = true;
		_MYTRACE(L"所有的服务器都连接上了!");
	}

	if ( iCountServers == m_aQuoteServerAll.GetSize() )
	{
		// 都没有连上
		bAllNonConnect = true;
		_MYTRACE(L"所有的服务器都没有连接上!");
	}

	// 全部连成功了, 结束等待
	if ( bAllConnect )
	{			
		OnFinishLogWaitQuoteConnect(false);		
	}

	return bAllNonConnect;
}

void CViewData::OnFinishLogWaitQuoteConnect(bool32 bAllNonConnect)
{
	if ( !m_bLogWaitQuoteConnect )
	{
		return;
	}
	
	StopTimer(KTimerIdWaitQuoteConnet);	
	m_bLogWaitQuoteConnect	  = false;
	m_iWaitQuoteConnectCounts = 0;
		
	//


	if ( bAllNonConnect )
	{
		if ( NULL != m_LoginWnd )
		{	
			ReSetLogData();

			//
			CString *pStrTip = new CString(L"连接行情服务器超时, 请检查网络,稍候登录...");
			PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);			
		}

		_MYTRACE(L"超过 60 秒, 所有行情服务器连接超时, 请检查网络,稍候登录...!");
	}
	else
	{
		// 排序		
		_MYTRACE(L"开始优选主站");

		if (m_aQuoteServerAll.GetSize() > 0)
		{
			for ( int32 i = 0; i < m_aQuoteServerAll.GetSize(); i++ )
			{
				CString StrAdd = m_aQuoteServerAll.GetAt(i).wszAddr;
				int32 iPort	   = m_aQuoteServerAll.GetAt(i).iPort;
				int32 iCommID  = m_pCommManager->GetCommunicationID(StrAdd, iPort);

				_MYTRACE(L"去掉连接超时的行情服务器: %d %s %s %d",
					iCommID, m_aQuoteServerAll.GetAt(i).wszName, StrAdd.GetBuffer(), iPort);
				StrAdd.ReleaseBuffer();

				// 停止这个行情, 防止过会儿连上了
				m_pCommManager->StopService(iCommID);
			}

			COptimizeServer::Instance()->DelNonConnectServer(m_aQuoteServerAll.GetData(), m_aQuoteServerAll.GetSize());
			m_aQuoteServerAll.RemoveAll();
		}
		
	//	pDoc->m_pDlgLogIn->SetPromptInfo(L"优选主站..."); 
		COptimizeServer::Instance()->SetViewData(this);
		bool32 bSortOK = COptimizeServer::Instance()->SortQuoteServers();
		if (bSortOK)
		{
			// ....
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
// 资讯相关
void CViewData::RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationID /* = -1 */)
{
	if ( NULL == pMmiNewsBase )
	{
		ASSERT(0);
		return;
	}

	m_pNewsManager->RequestNews(pMmiNewsBase, iCommunicationID);
}

void CViewData::OnNewsResponse()
{
	// 多线程变单线程处理
	PostMessage(m_OwnerWnd, UM_ViewData_OnNewsResponse, 0, 0);
}

void CViewData::OnNewsServerConnected(int32 iCommunicationId)
{
	PostMessage(m_OwnerWnd, UM_ViewData_OnNewsServerConnected, iCommunicationId, 0);
}	

void CViewData::OnNewsServerDisconnected(int32 iCommunicationId)
{
	PostMessage(m_OwnerWnd, UM_ViewData_OnNewsServerDisConnected, iCommunicationId, 0);
}

void CViewData::OnMsgNewsResponse()
{
	// 取出回包
	CMmiNewsRespNode* pRespNode = m_pNewsManager->PeekAndRemoveResp();
	
	while( NULL != pRespNode )
	{
		if ( NULL != pRespNode->m_pMmiNewsResp )
		{
			switch (pRespNode->m_pMmiNewsResp->m_eNewsType)
			{
			case ENTRespInfoList:
				{
					// 返回咨询列表
					OnNewsRespNewsList(pRespNode->m_iMmiRespId, (CMmiNewsRespNewsList*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTRespF10:
				{
					// 返回 F10
					OnNewsRespF10(pRespNode->m_iMmiRespId, (CMmiNewsRespF10*)pRespNode->m_pMmiNewsResp);
				}	
				break;
			
			case  ENTRespLandMine:
				{
					// 返回信息地雷
					OnNewsRespLandMine(pRespNode->m_iMmiRespId, (CMmiNewsRespLandMine*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTRespCodeIndex:
				{
					// 返回代码对应的资讯页面
					OnNewsRespCodeIndex(pRespNode->m_iMmiRespId, (CMmiNewsRespCodeIndex*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTPushTitle:
				{
					OnNewsPushNewsTitle(pRespNode->m_iMmiRespId, (CMmiNewsPushNewsTitle*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTRespTitleList:
				{
					OnNewsRespTitleList(pRespNode->m_iMmiRespId, (CMmiNewsRespTitleList*)pRespNode->m_pMmiNewsResp);
				}
				break;	
			case  ENTRespNewsContent:
				{
					OnNewsRespNewsContent(pRespNode->m_iMmiRespId, (CMmiNewsRespNewsContent*)pRespNode->m_pMmiNewsResp);
				}
				break;
			default:
				{
					ASSERT(0);
				}
				break;
			}
		}
		else
		{
			ASSERT(0);
		}

		// 释放资源
		DEL(pRespNode);

		// 取下一个应答包
		pRespNode = m_pNewsManager->PeekAndRemoveResp();
	}
}

void CViewData::OnMsgNewsServerConnected(int32 iCommunicationId)
{
	// by hx
	/*if ( NULL != m_pDlgF10 )
	{		
		m_pDlgF10->OnNewsServerConnected(iCommunicationId);
	}*/
}

void CViewData::OnMsgNewsServerDisconnected(int32 iCommunicationId)
{
	// by hx
	/*if ( NULL != m_pDlgF10 )
	{		
		m_pDlgF10->OnNewsServerDisConnected(iCommunicationId);
	}*/
}

void CViewData::OnNewsRespNewsList(int32 iMmiRespId, IN const CMmiNewsRespNewsList* pMmiNewsRespNewsList)
{
	if ( NULL == pMmiNewsRespNewsList )
	{
		ASSERT(0);
		return;
	}
 
/*
 	FILE* pFile = fopen("c:\\testlist.txt", "w");
 	if ( NULL != pFile )
 	{
 		USES_CONVERSION;
 		char* pStr = W2A(pMmiNewsRespNewsList->m_StrNews);
 		fprintf(pFile, pStr);
 		fclose(pFile);
 	}
*/
	// 异步方式通知所有关心该数据的业务视图
	//PostMessage(m_OwnerWnd, UM_ViewData_OnNewsListUpdate, 0, 0);	
}

void CViewData::OnNewsRespCodeIndex(int32 iMmiRespId, IN const CMmiNewsRespCodeIndex* pMmiNewsRespCodeIndex)
{
	if ( NULL == pMmiNewsRespCodeIndex )
	{
		ASSERT(0);
		return;
	}
	
	//
	CString StrCode, StrContent;
	
	CString StrTmp = pMmiNewsRespCodeIndex->m_StrCodeIndex;
	TCHAR* pStrSrc = StrTmp.LockBuffer();
	bool32 bOK = GetIndexCode(pStrSrc, StrCode, StrContent);
	if (bOK)
	{
		// ....
	}

	StrTmp.UnlockBuffer();
	
	m_mapCodeIndex[StrCode] = StrContent;
	
	//
	CString* pParam = new CString();
	*pParam = StrCode;
	
	//
	PostMessage(m_OwnerWnd, UM_ViewData_OnCodeIndexUpdate, (WPARAM)pParam, 0);	
}

void CViewData::OnNewsPushNewsTitle(int32 iMmiRespId, IN const CMmiNewsPushNewsTitle* pMmiNewsPush)
{
	/*
	if (m_bDelayQuote) 不知道有没有用先注掉
	{
		return;
	}
	*/
	//
	if ( NULL == pMmiNewsPush )
	{
		ASSERT(0);
		return;
	}
	
	
	CString StrTmp = pMmiNewsPush->m_StrTitle;
	TCHAR* pStrSrc = StrTmp.LockBuffer();	
	T_NewsInfo stNewsInfo;
	bool32 bOK = GetPushTitle(pStrSrc, stNewsInfo);
	StrTmp.UnlockBuffer();
	
	int32 iIndexID = stNewsInfo.m_iInfoIndex;

	if (bOK)
	{
		//
		if (GetJinpingID() == stNewsInfo.m_iInfoIndex)
		{
			m_listJinpingTitle.InsertAt(0, stNewsInfo);
		}
		else if (KiIndexNormal == stNewsInfo.m_iInfoIndex)
		{
			m_listNewsTitle.InsertAt(0, stNewsInfo);
		}

		TRACE(L"PUSH NEWS: %d %s %s \n", stNewsInfo.m_iIndexID, stNewsInfo.m_StrTimeUpdate.GetBuffer(), stNewsInfo.m_StrContent.GetBuffer());	
		stNewsInfo.m_StrTimeUpdate.ReleaseBuffer();
		stNewsInfo.m_StrContent.ReleaseBuffer();

		for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
		{
			CIoViewListner *pIoView = m_IoViewList[i];
			{					
				pIoView->OnVDataNewsPushTitle(stNewsInfo); 
			}
		}

		SendMessage(m_OwnerWnd, UM_ViewData_OnNewsPushTitle, iIndexID, (LPARAM)&stNewsInfo);	
		
	}
	else
	{
		ASSERT(0);
	}
}

void CViewData::OnNewsRespTitleList(int32 iMmiRespId, IN const CMmiNewsRespTitleList* pMmiNewsResp)
{
	
	if ( NULL == pMmiNewsResp )
	{
		ASSERT(0);
		return;
	}
	
	//
	CString StrTmp = pMmiNewsResp->m_StrTitles;
	TCHAR* pStrSrc = StrTmp.LockBuffer();
	
	listNewsInfo listTmp;
	GetTitleList(pStrSrc, listTmp);
	
	int32 iIndex = -1;

	//
	if (listTmp.GetSize() > 0)
	{
		iIndex = listTmp.GetAt(0).m_iInfoIndex;

		if (GetJinpingID() == iIndex) // 金评
		{
			m_listJinpingTitle.Copy(listTmp);
		}
		else if (KiIndexNormal == iIndex) // 普通资讯
		{
			m_listNewsTitle.Copy(listTmp);
		}
	}
	//
	StrTmp.UnlockBuffer();


	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		{					
			pIoView->OnVDataNewsTitleUpdate(listTmp); 
		}
	}

	SendMessage(m_OwnerWnd, UM_ViewData_OnNewsTitleUpdate, iIndex, (LPARAM)&listTmp);	
}

void CViewData::OnNewsRespNewsContent(int32 iMmiRespId, IN const CMmiNewsRespNewsContent* pMmiNewsResp)
{
	if ( NULL == pMmiNewsResp )
	{
		ASSERT(0);
		return;
	}
	
	//
	CString StrTmp = pMmiNewsResp->m_StrContent;
	TCHAR* pStrSrc = StrTmp.LockBuffer();	
	listNewsInfo aInfo;
	GetTitleList(pStrSrc, aInfo);
	StrTmp.UnlockBuffer();
	
	if (aInfo.GetSize() == 1)
	{
		for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
		{
			CIoViewListner *pIoView = m_IoViewList[i];
			{					
				pIoView->OnVDataNewsContentUpdate(aInfo[0]); 
			}
		}

		SendMessage(m_OwnerWnd, UM_ViewData_OnNewsContentUpdate, (WPARAM)&aInfo[0], 0);	
	}
	else
	{
		ASSERT(0);
	}
}

void CViewData::OnNewsRespF10(int32 iMmiRespId, IN const CMmiNewsRespF10* pMmiNewsRespF10)
{
	if ( NULL == pMmiNewsRespF10 )
	{
		ASSERT(0);
		return;
	}
	
	//
	PostMessage(m_OwnerWnd, UM_ViewData_OnF10Update, 0, 0);	

	for ( int32 i=0; i < m_aViewNewsLinstner.GetSize() ; i++ )
	{
		if ( m_aViewNewsLinstner[i] != NULL
			&& m_aViewNewsLinstner[i]->IsAttendNews(ENTReqF10) )
		{
			m_aViewNewsLinstner[i]->OnDataRespNewsData(iMmiRespId, pMmiNewsRespF10, NULL);
		}
	}
}

void CViewData::OnNewsRespLandMine(int32 iMmiRespId, IN const CMmiNewsRespLandMine* pMmiNewsRespLandMine)
{
	if ( NULL == pMmiNewsRespLandMine )
	{
		ASSERT(0);
		return;
	}

/*	
	FILE* pFile = fopen("c:\\LandMineResp.txt", "w");
	if ( NULL != pFile )
	{
		USES_CONVERSION;
		char* pStr = W2A(pMmiNewsRespLandMine->m_StrLandMine);
		fprintf(pFile, pStr);
		fclose(pFile);
	}
*/		

	CString StrErr;
	
	int32 iMarketId;
	CString StrMerchCode;

	vector<CLandMineTitle> aTitles;
	CLandMineContent stContent;

	CString StrLandMine = pMmiNewsRespLandMine->m_StrLandMine;
	TCHAR* pStrLandMine = StrLandMine.LockBuffer();
	bool32 bOK = ParaseLandMineString(pStrLandMine, iMarketId, StrMerchCode, aTitles, stContent, StrErr);
	StrLandMine.UnlockBuffer();
	
	CMerch* pMerch = NULL;

	if ( bOK && m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch) && NULL != pMerch )
	{		
		if ( !aTitles.empty() )
		{
			// 更新标题
			for ( vector<CLandMineTitle>::iterator it = aTitles.begin(); it != aTitles.end(); ++it )
			{
				CLandMineTitle stLandMineTitle = *it;
				
				// 找到跟这条标题时间相同的记录
				if ( pMerch->m_mapLandMine.count(stLandMineTitle.m_StrUpdateTime) > 0 )
				{
					//RangeLandMine range = pMerch->m_mapLandMine.equal_range(stLandMineTitle.m_StrUpdateTime);
					mapLandMine::_Pairii range = pMerch->m_mapLandMine.equal_range(stLandMineTitle.m_StrUpdateTime);
					
					bool32 bSame = false;
					for( mapLandMine::iterator itRange = range.first; itRange != range.second; ++itRange )
					{						
						CLandMineTitle stNow = itRange->second;
						
						// 判断列表title是否相同
						if ( stNow.m_iInfoIndex == stLandMineTitle.m_iInfoIndex
							&& stNow.m_StrUpdateTime == stLandMineTitle.m_StrUpdateTime
							&& stNow.m_StrCrc32 == stLandMineTitle.m_StrCrc32 )
						{
							// 就是这个标题了
							// 不相同的话就更新
							bSame = true;
							itRange->second = stLandMineTitle;		// 更新这个的内容
							if ( stLandMineTitle.m_stContent.m_iConLen == 0 )	
							{
								itRange->second.m_stContent = stNow.m_stContent;	// 新的无内容的话，内容保持原来的
							}
							//pMerch->m_mapLandMine.insert(make_pair(stLandMineTitle.m_StrUpdateTime, stLandMineTitle));
							break;
						}
					}
					
					if ( !bSame )
					{
						pMerch->m_mapLandMine.insert(make_pair(stLandMineTitle.m_StrUpdateTime, stLandMineTitle));
					}
				}
				else
				{
					pMerch->m_mapLandMine.insert(make_pair(stLandMineTitle.m_StrUpdateTime, stLandMineTitle));
				}
			}
		}
		else
		{
			// 更新内容
			for ( itLandMine it = pMerch->m_mapLandMine.begin(); it != pMerch->m_mapLandMine.end(); ++it )
			{
				//
				CLandMineTitle* pstTitle = &(it->second);
				
				if ( pstTitle->m_iInfoIndex == stContent.m_iInfoIndex )
				{
					pstTitle->m_stContent = stContent;
				}
			}
		}
		
		//
		PostMessage(m_OwnerWnd, UM_ViewData_OnLandMineUpdate, (WPARAM)pMerch, 0);	
		
		for ( int32 i=0; i < m_aViewNewsLinstner.GetSize() ; i++ )
		{
			if ( m_aViewNewsLinstner[i] != NULL
				&& m_aViewNewsLinstner[i]->IsAttendNews(ENTReqLandMine) )
			{
				m_aViewNewsLinstner[i]->OnDataRespNewsData(iMmiRespId, pMmiNewsRespLandMine, pMerch);
			}
		}		
	}

	
}

void CViewData::OnTimerLogicBlockTraslateTimeOut()
{
	PostMessage(m_OwnerWnd, UM_ViewData_OnBlockFileTraslateTimeout, NULL, NULL);
}

void CViewData::OnBlockFileTranslateTimeout()
{
	CBlockConfig::RespLogicBlockFile(NULL);

	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(_T("板块文件传输超时！"), TRUE);
	

	// 如果存在登录框， 通知登录框状态变化
	if (NULL != m_LoginWnd)
	{
		//pDoc->m_pDlgLogIn->StepProgress();
		// 使用本地的初始化
		CString StrPrompt;
		CString StrBlockFileName;
		bool32 bBlockOk = CBlockConfig::GetConfigFullPathName(StrBlockFileName);
		ASSERT( bBlockOk );
		StrPrompt.Format(L"提示：同步板块列表信息超时！使用本地板块文件初始化！");
		bBlockOk = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // 初始化板块文件 // 如果没有股票信息，那么就可以不用同步板块列表了
		ASSERT( bBlockOk );
	}

	OnAllMarketInitializeSuccess();
}



void CViewData::RequestStatusBarViewData()
{
	//ASSERT( 0 );	// 未实现 - 已经由push实现了 - 推送商品
	CMmiReqRealtimePrice Req;
	int32 i = 0;
	for ( i= 0; i < m_StatucBarAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = m_StatucBarAttendMerchs[i];
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}
		
		if ( 0 == i )
		{
			Req.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
		}
		else
		{
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
			
			Req.m_aMerchMore.Add(MerchKey);					
		}
	}
	
	if ( m_StatucBarAttendMerchs.GetSize() > 0 )
	{
		RequestViewData(&Req);
	}

	// 请求多空阵线数据 0, 1000
	CMmiReqGeneralNormal reqG;
	reqG.m_iMarketId = 0;
	RequestViewData(&reqG);
	reqG.m_iMarketId = 1000;
	RequestViewData(&reqG);
}

bool32 CViewData::GetMerchPowerValue( IN CMerch *pMerchStock, OUT float &fPowerValue )
{
	fPowerValue = 0.0;  // 要做这个处理么？？

	// 只处理沪 深 两个大市场下的商品
	if ( pMerchStock == NULL || NULL == pMerchStock->m_pRealtimePrice )
	{
		return false;
	}
	
	m_fPriceClose = pMerchStock->m_pRealtimePrice->m_fPricePrevClose;

	CMerch *pMerchIndexCmp = NULL;	// 指数参考
	const int32 iBreedId = pMerchStock->m_Market.m_Breed.m_iBreedId;
	if ( iBreedId == 0 )	// 沪 - 上证指数
	{
		m_MerchManager.FindMerch(_T("000001"), 0, pMerchIndexCmp);
	}
	else if ( iBreedId == 1 )	// 深 - 深证综指
	{
		m_MerchManager.FindMerch(_T("399106"), 1000, pMerchIndexCmp);
	}

	if ( pMerchIndexCmp == NULL || pMerchIndexCmp->m_pRealtimePrice == NULL )
	{
		return false;
	}

	// 强弱度: 涨跌幅-股指涨跌幅（深证的取深证综指、上证的取上证综指）
	float fRisePecentStock = 0.0;
	if ( pMerchStock->m_pRealtimePrice->m_fPricePrevClose != 0.0 )
	{
		fRisePecentStock = (pMerchStock->m_pRealtimePrice->m_fPriceNew - pMerchStock->m_pRealtimePrice->m_fPricePrevClose) / pMerchStock->m_pRealtimePrice->m_fPricePrevClose;
	}

	float fRisePecentCmp = 0.0;
	if ( pMerchIndexCmp->m_pRealtimePrice->m_fPricePrevClose != 0.0 )
	{
		fRisePecentCmp = (pMerchIndexCmp->m_pRealtimePrice->m_fPriceNew - pMerchIndexCmp->m_pRealtimePrice->m_fPricePrevClose) / pMerchIndexCmp->m_pRealtimePrice->m_fPricePrevClose;
	}

	fPowerValue = fRisePecentStock - fRisePecentCmp;

	return true;
}

void CViewData::ReSetLogData()
{
	//	登录失败, 就返回到最原始的状态, 所有信息清空. 重新认证, 重新登录
	//	需要还原登录情况为:
	
	//	AuthSuccessMain 中启动定时器前先关掉定时器
	//	m_bLogWaitQuoteConnect, m_iWaitQuoteConnectCounts, m_aQuoteServerAll 这些登录信息
	//	删除MerchManager中所有初始化好了的Breed信息
	//	修改m_eLoginState状态
	//	清0 DlgLogin的Progress
	//	Doc中的账户信息清空
	//	优选排除
	//	停止所有连接的服务
	//	(将OnAuthSuccessMain中的初始化用户权限，版本信息移到初始化市场完成后)

	//
	m_eLoginState = ELSNotLogin;

	InitPush(false);
	//
	// 
	StopTimer(KTimerIdWaitQuoteConnet);

	// 
	m_bLogWaitQuoteConnect	= true;
	m_bLogWaitQuoteSortFinish = true;
	m_iWaitQuoteConnectCounts = 0;
	m_aQuoteServerAll.RemoveAll();

	//
	//m_MerchManager.Clear();
	m_aMarketToRequest.clear();
	m_aMarketRequesting.clear();


	m_strUserName = L"";
	m_strPassword = L"";

	
	m_pCommManager->StopAllService();
	m_pNewsManager->StopAllNewsCommunication();
	

	
	COptimizeServer::DelInstance();
	
	// 向 COptimizeServer 注册自己
	COptimizeServer::Instance()->AddNotify(this);
	COptimizeServer::Instance()->SetViewData(this);

	// 清空市场请求信息
	CBreed *pBreed = NULL;
	m_iAllMarketCount = m_iInitializedMarketCount = 0;
	for (int32 iIndexBreed = 0; iIndexBreed < m_MerchManager.m_BreedListPtr.GetSize(); iIndexBreed++)
	{
		pBreed = m_MerchManager.m_BreedListPtr[iIndexBreed];
		if (NULL == pBreed)
		{
			continue;
		}
		
		CMarket *pMarket = NULL;
		for (int32 iIndexMarket = 0; iIndexMarket < pBreed->m_MarketListPtr.GetSize(); iIndexMarket++)
		{
			pMarket = pBreed->m_MarketListPtr[iIndexMarket];
			if (NULL == pMarket)
			{
				continue;
			}
			
			//
			pMarket->m_MerchListReqState.m_bRequesting			= false;
			pMarket->m_MerchListReqState.m_iCommunicationId		= -1;
			pMarket->SetInitialized(false);
			++m_iAllMarketCount;
		}
	}
	m_iInitializedMarketCount = 0;	
}



void CViewData::AddViewDataListner(CViewDataListner* pListener)
{
	if ( NULL == pListener )
	{
		return;
	}

	for ( int32 i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
	{
		if ( m_aViewDataLinstner[i] == pListener )
		{
			return;
		}
	}

	//
	m_aViewDataLinstner.Add(pListener);

}

void CViewData::DelViewNewsListner(CViewNewsListner* pListener)
{
	if ( NULL == pListener )
	{
		return;
	}
	
	for ( int32 i = m_aViewNewsLinstner.GetSize() - 1; i >= 0; i-- )
	{
		if ( m_aViewNewsLinstner[i] == pListener )
		{
			m_aViewNewsLinstner.RemoveAt(i);			
		}
	}
}


void CViewData::AddViewNewsListner(CViewNewsListner* pListener)
{
	if ( NULL == pListener )
	{
		return;
	}

	for ( int32 i = 0; i < m_aViewNewsLinstner.GetSize(); i++ )
	{
		if ( m_aViewNewsLinstner[i] == pListener )
		{
			return;
		}
	}

	//
	m_aViewNewsLinstner.Add(pListener);

}

void CViewData::DelViewDataListner(CViewDataListner* pListener)
{
	if ( NULL == pListener )
	{
		return;
	}

	for ( int32 i = m_aViewDataLinstner.GetSize() - 1; i >= 0; i-- )
	{
		if ( m_aViewDataLinstner[i] == pListener )
		{
			m_aViewDataLinstner.RemoveAt(i);
			return;
		}
	}

	/*for ( int32 i = m_aViewDataLinstner.GetSize() - 1; i >= 0; i-- )
	{
		delete m_aViewDataLinstner[i];
	}
	m_aViewDataLinstner.RemoveAll();*/
}



void CViewData::SaveMarketInfo()
{
	CString strPath = CPathFactory::GetMarketInfoFilePath();

 	if ( strPath.IsEmpty() )
 	{
 		ASSERT(0);
 		return;
 	}

	//
	CString StrXml;
	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"MarketInfo\">\n";

	for ( int32 i = 0; i < m_MerchManager.m_BreedListPtr.GetSize(); i++ )
	{
		// 大市场
		CBreed* pBreed = m_MerchManager.m_BreedListPtr[i];
		if ( NULL == pBreed )
		{
			continue;
		}

		//
		CString StrBreed;
		StrBreed.Format(L"<%s %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%s\">",
						CString(KStrElementNameBreed).GetBuffer(),
						CString(KStrAttriBreedID).GetBuffer(), pBreed->m_iBreedId,
						CString(KStrAttriBreedShowID).GetBuffer(), pBreed->m_iShowId,
						CString(KStrAttriBreedNameCn).GetBuffer(), pBreed->m_StrBreedCnName.GetBuffer(),
						CString(KStrAttriBreedNameEn).GetBuffer(), pBreed->m_StrBreedEnName.GetBuffer());

		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr[j];
			if ( NULL == pMarket )
			{
				continue;
			}

			// 保存开收盘时间
			CString StrOCTimes;
			for ( int32 iIndexOC = 0; iIndexOC < pMarket->m_MarketInfo.m_OpenCloseTimes.GetSize(); iIndexOC++ )
			{
				CString StrOCTime;
				if ( iIndexOC == pMarket->m_MarketInfo.m_OpenCloseTimes.GetSize() - 1 )
				{
					StrOCTime.Format(L"%d,%d", pMarket->m_MarketInfo.m_OpenCloseTimes.GetAt(iIndexOC).m_iTimeOpen, pMarket->m_MarketInfo.m_OpenCloseTimes.GetAt(iIndexOC).m_iTimeClose);
				}
				else
				{
					StrOCTime.Format(L"%d,%d,", pMarket->m_MarketInfo.m_OpenCloseTimes.GetAt(iIndexOC).m_iTimeOpen, pMarket->m_MarketInfo.m_OpenCloseTimes.GetAt(iIndexOC).m_iTimeClose);
				}	
				
				StrOCTimes += StrOCTime;
			}

			// 最近交易日
			CString StrActiveDay;
			for ( int32 iIndexAct = 0; iIndexAct < pMarket->m_MarketInfo.m_aRecentActiveDay.GetSize(); iIndexAct ++)
			{
				CString StrActNow;

				if ( iIndexAct == pMarket->m_MarketInfo.m_aRecentActiveDay.GetSize() - 1)
				{
					StrActNow.Format(L"%d", pMarket->m_MarketInfo.m_aRecentActiveDay.GetAt(iIndexAct));
				}
				else
				{
					StrActNow.Format(L"%d,", pMarket->m_MarketInfo.m_aRecentActiveDay.GetAt(iIndexAct));
				}
				
				StrActiveDay += StrActNow;
			}

			//
			CString StrMarket;
			StrMarket.Format(L"<%s %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%s\">",
							CString(KStrElementNameMarket).GetBuffer(),
							CString(KStrAttriMarketID).GetBuffer(), pMarket->m_MarketInfo.m_iMarketId,
							CString(KStrAttriMarketShowID).GetBuffer(), pMarket->m_MarketInfo.m_iShowId,
							CString(KStrAttriMarketTimeZone).GetBuffer(), pMarket->m_MarketInfo.m_iTimeZone,
							CString(KStrAttriMarketNameCn).GetBuffer(), pMarket->m_MarketInfo.m_StrCnName.GetBuffer(),
							CString(KStrAttriMarketNameEn).GetBuffer(), pMarket->m_MarketInfo.m_StrEnName.GetBuffer(),
							CString(KStrAttriMarketReportType).GetBuffer(), pMarket->m_MarketInfo.m_eMarketReportType,
							CString(KStrAttriMarketVolScale).GetBuffer(), pMarket->m_MarketInfo.m_iVolScale,				
							CString(KStrAttriMarketIniPerDay).GetBuffer(), pMarket->m_MarketInfo.m_iTimeInitializePerDay,
 							CString(KStrAttriMarketOCTimes).GetBuffer(), StrOCTimes.GetBuffer(),		
 							CString(KStrAttriMarketActiveDay).GetBuffer(), StrActiveDay.GetBuffer());	


			for ( int32 k = 0; k < pMarket->m_MerchsPtr.GetSize(); k++ )
			{
				CMerch* pMerch = pMarket->m_MerchsPtr[k];
				if ( NULL == pMerch || -1 != pMerch->m_MerchInfo.m_StrMerchCnName.Find('&') )
				{
					continue;
				}

				//
				CString StrMerch;
				StrMerch.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\"/>",
								CString(KStrElementNameMerch).GetBuffer(),
								CString(KStrAttriMerchCode).GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
								CString(KStrAttriMerchNameCn).GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(),
								CString(KStrAttriMerchNameEn).GetBuffer(), pMerch->m_MerchInfo.m_StrMerchEnName.GetBuffer(),
								CString(KStrAttriMerchSaveDec).GetBuffer(), pMerch->m_MerchInfo.m_iSaveDec);

				//
				StrMarket += StrMerch;							
			}

			//
			StrMarket += L"</";
			StrMarket += KStrElementNameMarket;
			StrMarket += L">";

			//
			StrBreed += StrMarket;			
		}

		//
		StrBreed += L"</";
		StrBreed += KStrElementNameBreed;
		StrBreed += L">";

		//
		StrXml	 += StrBreed;
	}

	StrXml += L"</XMLDATA>";

	//
	int iLen = WideCharToMultiByte(CP_UTF8, 0, StrXml, -1, NULL, 0, NULL, NULL);
	if (0 != iLen)
	{
		char* strXml = new char[iLen];
		memset(strXml, 0, iLen);

		//
		WideCharToMultiByte(CP_UTF8, 0, StrXml, -1, strXml, iLen, NULL, NULL);
			
	
		std::string strPathA;
		Unicode2MultiChar(CP_ACP, strPath.GetBuffer(), strPathA);
		FILE* pFile = fopen(strPathA.c_str(), "w+");
		if ( NULL != pFile )
		{
			fprintf(pFile, "%s", strXml);
			fclose(pFile);
		}

		DEL_ARRAY(strXml);
	}
}

void SplitString(IN const CString& StrSrc, TCHAR ch, IN OUT CStringArray& aString)
{
	aString.RemoveAll();

	if ( StrSrc.IsEmpty() )
	{
		return;
	}

	//
	int32 iLen	 = StrSrc.GetLength();
	int32 iStart = 0;

	while (1)
	{
		CString StrSub;
	
		//
		int32 iPos = StrSrc.Find(ch, iStart);

		if ( -1 == iPos )
		{
			StrSub = StrSrc.Mid(iStart, iLen - iStart);
		}
		else
		{
			StrSub = StrSrc.Mid(iStart, iPos - iStart);
		}
		
		//
		if ( !StrSub.IsEmpty() )
		{
			aString.Add(StrSub);
		}

		//
		iStart = iPos + 1;

		//
		if ( -1 == iPos )
		{
			return;
		}
	}
}

bool32 CViewData::SplitMarketOCTimeString(IN const CString& StrOCTime, OUT CArray<T_OpenCloseTime, T_OpenCloseTime>& aOCTimes)
{
	aOCTimes.RemoveAll();

	//
	if ( StrOCTime.IsEmpty() )
	{	
		return false;
	}

	//
	CStringArray aStrOCTime;

	//
	SplitString(StrOCTime, ',', aStrOCTime);
	int32 iSizeOCTimes = aStrOCTime.GetSize();

	if ( iSizeOCTimes <= 0 || ((iSizeOCTimes % 2) != 0) )
	{
		ASSERT(0);
		return false;
	}
	
	//
	for ( int32 i = 0; i < aStrOCTime.GetSize(); i += 2 )
	{
		CString StrOpen = aStrOCTime.GetAt(i);
		CString StrClose = aStrOCTime.GetAt(i + 1);

		//
		string stropen, strclose;
		
		Unicode2MultiChar(CP_ACP, StrOpen, stropen);
		Unicode2MultiChar(CP_ACP, StrClose, strclose);

		//
		T_OpenCloseTime stOpenCloseTime;

		stOpenCloseTime.m_iTimeOpen  = atoi(stropen.c_str());
		stOpenCloseTime.m_iTimeClose = atoi(strclose.c_str());

		//
		aOCTimes.Add(stOpenCloseTime);

		// TRACE(L" 开收盘时间: %d %d \n", stOpenCloseTime.m_iTimeOpen, stOpenCloseTime.m_iTimeClose);
	}

	return true;
}

bool32 CViewData::SplitMarketActiveDays(IN const CString& StrActiveDay, OUT CArray<uint32, uint32>& aActiveDays)
{
	aActiveDays.RemoveAll();

	//
	if ( StrActiveDay.IsEmpty() )
	{
		return false;
	}

	CStringArray aStrActiveDays;
	
	//
	SplitString(StrActiveDay, ',', aStrActiveDays);
	
	//
	int32 iSize = aStrActiveDays.GetSize();

	//
	if ( iSize <= 0  )
	{
		ASSERT(0);
		return false;
	}

	if ( iSize != 12 )
	{
		ASSERT(0);	
	}
	
	//
	for ( int32 i = 0; i < iSize; i ++ )
	{
		CString StrNow = aStrActiveDays.GetAt(i);
		
		//
		string strnow;
		
		Unicode2MultiChar(CP_ACP, StrNow, strnow);
			
		//
		UINT uiTime = atoi(strnow.c_str());

		//
		aActiveDays.Add(uiTime);

		// TRACE(L"交易日--> %d \n", uiTime);
	}

	return true;
}

bool32 CViewData::InitialMarketInfoFromFile(CMerchManager& merchManager, CString& StrErr)
{
	StrErr = L"";

	//
	CString strPath = CPathFactory::GetMarketInfoFilePath();
	
	if ( strPath.IsEmpty() )
	{
		ASSERT(0);
		StrErr = L"获取配置文件路径失败";
		return false;
 	}
	
	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		ASSERT(0);
		StrErr = L"装载配置文件失败";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);
		StrErr = L"配置文件读取失败";
		return false;
	}

	// 市场类型
	DWORD dwMerchManagerType = EMMT_None;
	
	//
	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// 读取 breed 的属性				
		const char* pStrBreedID	   = pBreedElement->Attribute(KStrAttriBreedID);
		const char* pStrBreedShowID = pBreedElement->Attribute(KStrAttriBreedShowID);
		const char* pStrBreedNameCn = pBreedElement->Attribute(KStrAttriBreedNameCn);
		const char* pStrBreedNameEn = pBreedElement->Attribute(KStrAttriBreedNameEn);

		if ( NULL == pStrBreedID || NULL == pStrBreedShowID || NULL == pStrBreedNameCn )
		{			
			ASSERT(0);
			continue;
		}

		int32 iID			= atoi(pStrBreedID);
		int32 iShowID		= atoi(pStrBreedShowID);

		wstring StrNameCn, StrNameEn;
			
		MultiChar2Unicode(CP_UTF8, pStrBreedNameCn, StrNameCn);
		MultiChar2Unicode(CP_UTF8, pStrBreedNameEn, StrNameEn);
		
		if ( iID < 0 || iShowID < 0 || StrNameCn.length() <= 0 )
		{
			ASSERT(0);
			continue;
		}

		//
		CBreed* pBreed = new CBreed();
		pBreed->m_iBreedId = iID;
		pBreed->m_iShowId  = iShowID;
		pBreed->m_StrBreedCnName = StrNameCn.c_str();
		pBreed->m_StrBreedEnName = StrNameEn.c_str();

		merchManager.AddBreed(pBreed);

		// 再遍历这个Breed 下面的市场:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// 市场属性
			const char* pStrMarketID	   = pMarketElement->Attribute(KStrAttriMarketID);
			const char* pStrMarketShowID = pMarketElement->Attribute(KStrAttriMarketShowID);
			const char* pStrZone   = pMarketElement->Attribute(KStrAttriMarketTimeZone);
			const char* pStrMarketNameCn = pMarketElement->Attribute(KStrAttriMarketNameCn);
			const char* pStrMarketNameEn = pMarketElement->Attribute(KStrAttriMarketNameEn);
			const char* pStrRType  = pMarketElement->Attribute(KStrAttriMarketReportType);
			const char* pStrScale  = pMarketElement->Attribute(KStrAttriMarketVolScale);
			const char* pStrIniDay = pMarketElement->Attribute(KStrAttriMarketIniPerDay);
			const char* pStrOCTime = pMarketElement->Attribute(KStrAttriMarketOCTimes);
			const char* pStrActDay = pMarketElement->Attribute(KStrAttriMarketActiveDay);

			if ( NULL == pStrMarketID || NULL == pStrMarketShowID || NULL == pStrMarketNameCn || NULL == pStrRType || NULL == pStrScale || NULL == pStrIniDay || NULL == pStrOCTime || NULL == pStrActDay )
			{	
				ASSERT(0);
				continue;
			}
			
			int32 iMarketID			= atoi(pStrMarketID);
			int32 iMarketShowID		= atoi(pStrMarketShowID);
			int32 iTimeZone		= atoi(pStrZone);
			int32 iReportType	= atoi(pStrRType);
			int32 iVolScale		= atoi(pStrScale);
			//int32 iInitialPerDay= atoi(pStrIniDay);

			wstring  StrOctime, StrActDay;
			
			MultiChar2Unicode(CP_UTF8, pStrMarketNameCn, StrNameCn);
			MultiChar2Unicode(CP_UTF8, pStrMarketNameEn, StrNameEn);
			MultiChar2Unicode(CP_UTF8, pStrOCTime, StrOctime);
			MultiChar2Unicode(CP_UTF8, pStrActDay, StrActDay);
			
			// showid判断暂时去掉， 运维配置后再判断 cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0 */|| iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			// 
			// 全局释放的时候会释放，在函数内屏蔽429错误
			//lint --e{429} 

			CMarket* pMarket = new CMarket(*pBreed);
			pMarket->m_MarketInfo.m_iMarketId	= iMarketID;
			pMarket->m_MarketInfo.m_iShowId		= iMarketShowID;
			pMarket->m_MarketInfo.m_iTimeZone	= iTimeZone;			
			pMarket->m_MarketInfo.m_StrCnName	= StrNameCn.c_str();
			pMarket->m_MarketInfo.m_StrEnName	= StrNameEn.c_str();
			
			// 取开收盘时间
			if ( !SplitMarketOCTimeString(StrOctime.c_str(), pMarket->m_MarketInfo.m_OpenCloseTimes) )
			{
				ASSERT(0);
				continue;
			}

			// 取最近交易日
			if ( !SplitMarketActiveDays(StrActDay.c_str(), pMarket->m_MarketInfo.m_aRecentActiveDay) )
			{
				ASSERT(0);
				continue;
			}

			//
			pMarket->m_MarketInfo.m_eMarketReportType = (E_ReportType)iReportType;

			//
			if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTFuturesCn )
			{
				dwMerchManagerType |= EMMT_FutureCn;
			}
			else if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn )
			{
				dwMerchManagerType |= EMMT_StockCn;
			}
		
			//
			pBreed->AddMarket(pMarket);

			// 再遍历这个市场下面的商品:
			for ( TiXmlElement* pMerchElement = pMarketElement->FirstChildElement(); NULL != pMerchElement; pMerchElement = pMerchElement->NextSiblingElement() )
			{
				// 商品属性
				const char* pStrMerchCode   = pMerchElement->Attribute(KStrAttriMerchCode);
				const char* pStrMerchNameCn = pMerchElement->Attribute(KStrAttriMerchNameCn);
				const char* pStrMerchNameEn = pMerchElement->Attribute(KStrAttriMerchNameEn);
				const char* pStrDec    = pMerchElement->Attribute(KStrAttriMerchSaveDec);

				//
				if ( NULL == pStrMerchCode || NULL == pStrMerchNameCn || NULL == pStrDec )
				{
					ASSERT(0);
					continue;
				}

				//
				int32 iDec = atoi(pStrDec);
				wstring StrMerchCode, StrMerchNameCn, StrMerchNameEn;
				
				MultiChar2Unicode(CP_UTF8, pStrMerchCode, StrMerchCode);
				MultiChar2Unicode(CP_UTF8, pStrMerchNameCn, StrMerchNameCn);
				MultiChar2Unicode(CP_UTF8, pStrMerchNameEn, StrMerchNameEn);				

				if ( iDec < 0 || StrMerchCode.length() <= 0 || StrNameCn.length() <= 0 )
				{
					ASSERT(0);
					continue;
				}

				//
				CMerchInfo MerchInfo;
				MerchInfo.m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
				MerchInfo.m_StrMerchCode	= StrMerchCode.c_str();
				MerchInfo.m_StrMerchCnName= StrMerchNameCn.c_str();
				MerchInfo.m_StrMerchEnName= StrMerchNameEn.c_str();
				MerchInfo.m_iSaveDec		= iDec;

				pMarket->AddMerch(MerchInfo);
			}

			//
			pMarket->SetInitialized(true);
		}
	}
	//
	m_dwMerchManagerType = dwMerchManagerType;

	// 脱机版的 OnAllMarketInitializeSuccess()
	CString StrBlockFileName;
	bool32 bBlockOk = CBlockConfig::GetConfigFullPathName(StrBlockFileName);
	ASSERT( bBlockOk );
	bBlockOk = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // 初始化板块文件 // 如果没有股票信息，那么就可以不用同步板块列表了

	//
	m_eLoginState = ELSLoginSuccess;

	// 设置全部初始化成功
	merchManager.SetInitializedAll();

	// 初始化快捷键
//	BuildMerchHotKeyList();
	
	// 向所有业务视图发送强制刷新事件
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->ForceUpdateVisibleIoView();
	}

	return true;
}

bool32 CViewData::SaveUserRightInfo()
{
	//
	//// 保存当前的用户权限
	//CString pStrPath = CPathFactory::GetUserInfoPath();
	//
	//if (!CCodeFile::DeCodeFile(pStrPath))
	//{
	//	return false;
	//}

	////
	//CString StrUserName = m_strUserName;
	//if ( StrUserName.IsEmpty() )
	//{
	//	return false;
	//}

	////
	//USES_CONVERSION;
	//char* cPath = W2A(pStrPath);
	//TiXmlDocument Doc(cPath);
	//if ( !Doc.LoadFile() )
	//{
	//	return false;
	//}

	////
	//TiXmlElement* pRootElement = Doc.RootElement();
	//if ( NULL == pRootElement )
	//{
	//	return false;
	//}

	//TiXmlElement* pElementUserInfo = pRootElement->FirstChildElement();
	//
	//if ( NULL == pElementUserInfo )
	//{
	//	return false;
	//}
	//
	//// 找到对应节点
	//TiXmlElement* pUserElement = NULL;

	////
	//for ( TiXmlElement* pUserElementNow = pElementUserInfo->FirstChildElement(); NULL != pUserElementNow; pUserElementNow = pUserElementNow->NextSiblingElement() )
	//{
	//	const char* pStrUserName = pUserElementNow->Attribute(KStrElementAttriUserName);
	//	
	//	wstring strName;
	//	Utf8ToUnicode(pStrUserName, strName);

	//	if ( 0 == StrUserName.CompareNoCase(strName.c_str()) )
	//	{
	//		pUserElement = pUserElementNow;
	//		break;
	//	}
	//}

	//if ( NULL == pUserElement )
	//{
	//	return false;
	//}

	//// 先把现有的都删掉吧
	//pUserElement->Clear();

	////
	//const CPluginFuncRight::FuncCodeNameMap userRight = CPluginFuncRight::Instance().GetUserFuncRight();

	//for ( CPluginFuncRight::FuncCodeNameMap::const_iterator it = userRight.begin(); it != userRight.end(); ++it )
	//{
	//	int iCode  = it->first;
	//	CString StrName = it->second;
	//	
	//	//
	//	string strName;
	//	UnicodeToUtf8(StrName, strName);

	//	//
	//	TiXmlElement* pNewChild = new TiXmlElement(KStrElementNameUserRight);
	//	pNewChild->SetAttribute(KStrElementAttriRightCode, iCode);
	//	pNewChild->SetAttribute(KStrElementAttriRightName, strName.c_str());

	//	//
	//	pUserElement->LinkEndChild(pNewChild);
	//}

	////
	//Doc.SaveFile();

	//CCodeFile::EnCodeFile(pStrPath);

	return true;
}


bool32 CViewData::BeLocalOfflineDataEnough(E_KLineTypeBase eType)
{
	// 以上证指数为标准
	CGmtTime TimeBegin,TimeEnd; 
	int32 iCount;
	
	CMerch* pMerch = NULL;
	if ( m_MerchManager.FindMerch(L"000001", 0, pMerch) && NULL != pMerch )
	{
		// 找到 000001 的时间作为标准
		if ( m_OfflineDataManager.GetOfflineKLinesSnapshot(0, L"000001", eType, EOKTOfflineData, TimeBegin, TimeEnd, iCount) )
		{			
			//
			CGmtTime TimeNow = GetServerTime();
		
			//
			CMarketIOCTimeInfo stMarketIOCTimeInfo;
			pMerch->m_Market.GetRecentTradingDay(TimeNow, stMarketIOCTimeInfo, pMerch->m_MerchInfo);
			CGmtTime TimeTradingDay = stMarketIOCTimeInfo.m_TimeClose.m_Time;
			SaveDay(TimeTradingDay);

			//
			SaveDay(TimeNow);
			SaveDay(TimeEnd);

			if ( TimeNow.GetTime() > TimeTradingDay.GetTime() )
			{
				TimeNow = TimeTradingDay;
			}

			//
			if ( TimeEnd.GetTime() == TimeNow.GetTime() )
			{
				return true;
			}
		}
	}

	return false;
}

void CViewData::SetRecordDataCenterAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch&>& aMerchs)
{
	m_aRecordDataCenterAttendMerchs.Copy(aMerchs);
}



const CViewData::GeneralNormalArray * CViewData::GetGeneralNormalArray( CMerch *pMerch )
{
	// 0-000001 1000-399001才能获取对应的
	if ( NULL == pMerch )
	{
		return NULL;
	}

	int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
	if ( 0==iMarketId
		&& StrMerchCode == _T("000001") )
	{
		return &m_aGeneralNormalH;
	}
	else if ( 1000 == iMarketId
		&& StrMerchCode == _T("399001") )
	{
		return &m_aGeneralNormalS;
	}
	return NULL;
}

bool32 CViewData::GetGeneralNormalHFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell)
{
	iFlagBuy  = 0;
	iFlagSell = 0;

	if ( m_aGeneralNormalH.GetSize() <= 0 )
	{
		return false;
	}
	
	if ( m_stGeneralNoramlHPre.m_lTime <= 0 )
	{
		return false;
	}

	//
	T_GeneralNormal stNewest = m_aGeneralNormalH.GetAt(m_aGeneralNormalH.GetSize() - 1);

	// 买标志
	if ( stNewest.m_fBuy5Amount > m_stGeneralNoramlHPre.m_fBuy5Amount )
	{
		iFlagBuy = 1;
	}
	else if ( stNewest.m_fBuy5Amount == m_stGeneralNoramlHPre.m_fBuy5Amount )
	{
		iFlagBuy = 0;
	}
	else
	{
		iFlagBuy = -1;
	}

	// 卖标志
	if ( stNewest.m_fSell5Amount > m_stGeneralNoramlHPre.m_fSell5Amount )
	{
		iFlagSell = 1;
	}
	else if ( stNewest.m_fSell5Amount == m_stGeneralNoramlHPre.m_fSell5Amount )
	{
		iFlagSell = 0;
	}
	else
	{
		iFlagSell = -1;
	}

	return true;
}

bool32 CViewData::GetGeneralNormalSFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell)
{
	iFlagBuy  = 0;
	iFlagSell = 0;
	
	if ( m_aGeneralNormalS.GetSize() <= 0 )
	{
		return false;
	}
	
	if ( m_stGeneralNoramlSPre.m_lTime <= 0 )
	{
		return false;
	}
	
	//
	T_GeneralNormal stNewest = m_aGeneralNormalS.GetAt(m_aGeneralNormalS.GetSize() - 1);
	
	// 买标志
	if ( stNewest.m_fBuy5Amount > m_stGeneralNoramlSPre.m_fBuy5Amount )
	{
		iFlagBuy = 1;
	}
	else if ( stNewest.m_fBuy5Amount == m_stGeneralNoramlSPre.m_fBuy5Amount )
	{
		iFlagBuy = 0;
	}
	else
	{
		iFlagBuy = -1;
	}
	
	// 卖标志
	if ( stNewest.m_fSell5Amount > m_stGeneralNoramlSPre.m_fSell5Amount )
	{
		iFlagSell = 1;
	}
	else if ( stNewest.m_fSell5Amount == m_stGeneralNoramlSPre.m_fSell5Amount )
	{
		iFlagSell = 0;
	}
	else
	{
		iFlagSell = -1;
	}
	
	return true;
}

void CViewData::PutIntoMarketReqList(CMarket* pMarket)
{
	if ( NULL == pMarket )
	{
		ASSERT(0);;
		return;
	}

	//
	m_aMarketToRequest.insert(pMarket);
	_MYTRACE(L"市场 %d 加入请求队列", pMarket->m_MarketInfo.m_iMarketId);
}

void CViewData::ReqMarketInfo()
{
	if ( !m_aMarketRequesting.empty() )
	{
		return;
	}

	//
	m_aMarketRequesting.clear();

	//
	int32 iCounts = s_KiMarketRequestSegment; 

	if ( m_bFastLogIn )
	{
		iCounts = m_aMarketToRequest.size();
	}

	while(iCounts)
	{
		if ( m_aMarketToRequest.empty() )
		{
			break;
		}

		//
		CMarket* pMarket = *m_aMarketToRequest.begin();		
		m_aMarketRequesting.insert(pMarket);

		//
		m_aMarketToRequest.erase(m_aMarketToRequest.begin());
		iCounts -= 1;
	}
	
	//
	for ( set<CMarket*, CmpMarket>::iterator it = m_aMarketRequesting.begin(); it != m_aMarketRequesting.end(); ++it )
	{
		CMarket* pMarket = *it;
		if ( NULL == pMarket )
		{
			continue;
		}

		//
		_MYTRACE(L"发送市场: %d 的请求", pMarket->m_MarketInfo.m_iMarketId);	
		InitializeMarket(*pMarket);
	}
}

void CViewData::DisconnectAllQuoteServer()
{
	m_pCommManager->StopAllService();

	m_aPushingRealtimePrices.clear();
	m_aPushingRealtimeTicks.clear(); 
	m_aPushingRealtimeLevel2s.clear();

	// 其它数据清空
}


//获取今评标志  2013-7-23
bool CViewData::GetTodayFlag()
{
	return m_bTodayFalg;
}

void CViewData::SetTodayFlag(bool bFlag)
{
	m_bTodayFalg = bFlag;
}

//Im消息标志 2013-10-31 add by cym
bool CViewData::GetMessageFlag()
{
	return m_bMessageFlag;
}  

void CViewData::SetMessageFlag(bool bFlag)
{
	m_bMessageFlag= bFlag;
}




void CViewData::OnMsgCodeIndexUpdate(IN const CString& StrCode)
{
	// 通知所有关心该数据的业务视图
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		{
			pIoView->OnVDataCodeIndexUpdate(StrCode);
		}
	}
}


bool32 CViewData::DirectConnectQuote()
{
	// 直连服务器
	if (!LoadMarketOnDirectQuote())
	{
		return false;
	}
	
	// 创建私有目录
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	
	// 获取行情服务器
	CString strPath = CPathFactory::GetLoginBackUpPath(m_strUserName);
	if (strPath.IsEmpty())
	{
		return false;
	}

	T_LoginBackUp stLoginBk;


	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	FILE* pFile = fopen(strPathA.c_str(), "rb+");
	if (NULL == pFile)
	{
		return false;
	}

	int32 iSize = fread(&stLoginBk, sizeof(stLoginBk) - sizeof(stLoginBk.m_pServices), 1, pFile);
	if (iSize < 0 || stLoginBk.m_iSize <= 0)
	{
		fclose(pFile);
		return false;
	}

	int32 iCount = stLoginBk.m_iServiceCount;
	CArray<int32, int32> aServiceIDs;

	for (int32 i = 0; i < iCount; i++)
	{
		int32 iID = -1;
		fread(&iID, sizeof(iID), 1, pFile);
		if (iID >= 0)
		{
			aServiceIDs.Add(iID);
		}
	}

	fclose(pFile);

	//
	if (stLoginBk.m_StrQuoterSvrAdd.IsEmpty() || stLoginBk.m_iQuoterSvrPort <= 0)
	{
		return false;
	}

	// 得到行情服务器列表
	T_ServerInfo stServer;
	wcscpy(stServer.wszAddr, stLoginBk.m_StrQuoterSvrAdd);
	stServer.iPort = stLoginBk.m_iQuoterSvrPort;

	m_aQuoteServerAll.Add(stServer);
	
	// 建立各个服务器的连接
	m_pCommManager->SetStage(ECSTAGE_LoginInit);
	m_pCommManager->InitDirectService(m_proxyInfo, m_strUserName, m_strPassword, stLoginBk.m_StrQuoterSvrAdd, stLoginBk.m_iQuoterSvrPort, aServiceIDs);
	m_pCommManager->StartAllService();

	// 设置标志, 启动定时器
	SetDirectQuote(true);
	StartTimer(KTimerIdWaitDirectQuoteConnet, KTimerPeriodWaitQuoteConnet);

	//
	return true;
}

bool32 CViewData::LoadMarketOnDirectQuote()
{

	
	if ( NULL == m_LoginWnd)
	{		
		return false;
	}

	//
	CString StrErr = L"";
	
	//
	CString strPath = CPathFactory::GetMarketInfoFilePath();
	
	if ( strPath.IsEmpty() )
	{
		ASSERT(0);
		StrErr = L"获取配置文件路径失败";
		return false;
 	}

	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		ASSERT(0);;
		StrErr = L"装载配置文件失败";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);;
		StrErr = L"配置文件读取失败";
		return false;
	}

	// 市场类型
	//DWORD dwMerchManagerType = EMMT_None;
	
	// 初始化所有的交易品种和下面的物理市场
	int32 iAllMarketCount = 0;
	int32 iInitializedMarketCount = 0;

	//
	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// 读取 breed 的属性				
		const char* pStrBreedID	   = pBreedElement->Attribute(KStrAttriBreedID);
		const char* pStrBreedShowID = pBreedElement->Attribute(KStrAttriBreedShowID);
		const char* pStrBreedNameCn = pBreedElement->Attribute(KStrAttriBreedNameCn);
		const char* pStrBreedNameEn = pBreedElement->Attribute(KStrAttriBreedNameEn);

		if ( NULL == pStrBreedID || NULL == pStrBreedShowID || NULL == pStrBreedNameCn )
		{			
			ASSERT(0);;
			continue;
		}

		int32 iBreedID			= atoi(pStrBreedID);
		int32 iBreedShowID		= atoi(pStrBreedShowID);

		wstring StrNameCn, StrNameEn;
			
		MultiChar2Unicode(CP_UTF8, pStrBreedNameCn, StrNameCn);
		MultiChar2Unicode(CP_UTF8, pStrBreedNameEn, StrNameEn);
		
		if ( iBreedID < 0 || iBreedShowID < 0 || StrNameCn.length() <= 0 )
		{
			ASSERT(0);
			continue;
		}

		//
		CBreed* pBreed = new CBreed();
		pBreed->m_iBreedId = iBreedID;
		pBreed->m_iShowId  = iBreedShowID;
		pBreed->m_StrBreedCnName = StrNameCn.c_str();
		pBreed->m_StrBreedEnName = StrNameEn.c_str();

		m_MerchManager.AddBreed(pBreed);

		// 再遍历这个Breed 下面的市场:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// 市场属性
			const char* pStrMarketID	   = pMarketElement->Attribute(KStrAttriMarketID);
			const char* pStrMarketShowID = pMarketElement->Attribute(KStrAttriMarketShowID);
			const char* pStrZone   = pMarketElement->Attribute(KStrAttriMarketTimeZone);
			const char* pStrMarketNameCn = pMarketElement->Attribute(KStrAttriMarketNameCn);
			const char* pStrMarketNameEn = pMarketElement->Attribute(KStrAttriMarketNameEn);
			const char* pStrRType  = pMarketElement->Attribute(KStrAttriMarketReportType);
			const char* pStrScale  = pMarketElement->Attribute(KStrAttriMarketVolScale);
			const char* pStrIniDay = pMarketElement->Attribute(KStrAttriMarketIniPerDay);
			const char* pStrOCTime = pMarketElement->Attribute(KStrAttriMarketOCTimes);
			const char* pStrActDay = pMarketElement->Attribute(KStrAttriMarketActiveDay);

			if ( NULL == pStrMarketID || NULL == pStrMarketShowID || NULL == pStrMarketNameCn || NULL == pStrRType || NULL == pStrScale || NULL == pStrIniDay || NULL == pStrOCTime || NULL == pStrActDay )
			{	
				ASSERT(0);
				continue;
			}
			
			int32 iMarketID			= atoi(pStrMarketID);
			int32 iMarketShowID		= atoi(pStrMarketShowID);
			int32 iTimeZone		= atoi(pStrZone);
			int32 iReportType	= atoi(pStrRType);
			int32 iVolScale		= atoi(pStrScale);
			//int32 iInitialPerDay= atoi(pStrIniDay);

			wstring  StrOctime, StrActDay;
			
			MultiChar2Unicode(CP_UTF8, pStrMarketNameCn, StrNameCn);
			MultiChar2Unicode(CP_UTF8, pStrMarketNameEn, StrNameEn);
			MultiChar2Unicode(CP_UTF8, pStrOCTime, StrOctime);
			MultiChar2Unicode(CP_UTF8, pStrActDay, StrActDay);
			
			// showid判断暂时去掉， 运维配置后再判断 cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0*/ || iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			int32 iPosMarket;
			CMarket* pTmp;
			if ( !pBreed->FindMarket(iMarketID, iPosMarket, pTmp) )
			{
				// 全局释放的时候会释放，在函数内屏蔽429错误
				//lint --e{429} 
				CMarket* pMarket = new CMarket(*pBreed);
				pMarket->m_MarketInfo.m_iMarketId	= iMarketID;
				pMarket->m_MarketInfo.m_iShowId		= iMarketShowID;
				pMarket->m_MarketInfo.m_iTimeZone	= iTimeZone;			
				pMarket->m_MarketInfo.m_StrCnName	= StrNameCn.c_str();
				pMarket->m_MarketInfo.m_StrEnName	= StrNameEn.c_str();
				
				pBreed->AddMarket(pMarket);

				//
				if ( pMarket->IsInitialized() )
				{
					iInitializedMarketCount++;
				}
			}

			iAllMarketCount++;			
		}
	}

	return true;
}

CString CViewData::GetPlusErrorStr(int errCode)
{
	if ( 0 < errCode  && errCode < 90) //1-89 开源库curl定义返回错误码
	{
		return L"开源库curl错误!";
	}
	
	if ( 99 < errCode && errCode < 506) //100-505是http错误
	{
		CString strTmp;
		strTmp.Format(L"HTTP 错误: %d", errCode);
		return strTmp;
	}

	int i;
	for (i = 0; s_aPlusError[i].errCode != 99999; i++ )
	{
		if ( errCode == s_aPlusError[i].errCode)
			break;
	}
	
	return s_aPlusError[i].errStr;
}


void CViewData::SetUserInfo(CString& strUserName,CString& strPwd,CString& strProduct)
{
	m_strUserName = strUserName;
	m_strPassword = strPwd;
	m_strProduct = strProduct;
}

void CViewData::SetOffLineData(bool bOffLine)
{
	m_bOffLine = bOffLine;
}

//***************************************** by hx
//void CViewData::SetAuthResult(const T_PlugInAuth& stPlugInAuth)
//{
//	m_stAuthPlugin = stPlugInAuth;
//}	
//
//T_PlugInAuth CViewData::GetAuthResult()
//{
//	return m_stAuthPlugin;
//}

//*******************************************


void CViewData::SetLoginHWnd(HWND hWnd)
{
//	SetFrameHwnd(NULL);
	m_LoginWnd = hWnd;
	m_OwnerWnd = hWnd;
}
void CViewData::SetFrameHwnd(HWND hWnd)
{
//	SetLoginHWnd(NULL);
	m_MainFrameWnd = hWnd;
	m_OwnerWnd = hWnd;
}

CString CViewData::GetUserName()
{
	return m_strUserName;
}

CString CViewData::GetUserPwd()
{
	return m_strPassword;
}

CString CViewData::GetProduct()
{
	return m_strProduct;
}

bool CViewData::GetManualReConnectFlag()
{
	return m_bManualReconnect == 0 ? false : true;
}

void CViewData::SetManualReConnectFlag(bool bFlag)
{
	m_bManualReconnect = bFlag;
}

void CViewData::AddAttendMerch(CSmartAttendMerch& attendMerch,E_AttendMerchType type )
{
	if (type == EA_Alarm)
	{
		m_aAlarmAttendMerchs.Add(attendMerch);
	}
	else if (type == EA_RecordData)
	{
		m_aRecordDataCenterAttendMerchs.Add(attendMerch);
	}
	else if (type == EA_StatucBar)
	{
		m_StatucBarAttendMerchs.Add(attendMerch);
	}
	else if (type == EA_Arbitrage)
	{
		m_aArbitrageAttendMerchs.Add(attendMerch);
	}
	else if (type == EA_Choose)
	{
		m_aIndexChsDataAttendMerchs.Add(attendMerch);
	}
	else if (type == EA_SimulateTrade)
	{
		m_aSimulateTradeAttendMerchs.Add(attendMerch);
	}
}

void CViewData::RemoveAttendMerch(E_AttendMerchType type)
{
	if (type == EA_Alarm)
	{
		m_aAlarmAttendMerchs.RemoveAll();
	}
	else if (type == EA_RecordData)
	{
		m_aRecordDataCenterAttendMerchs.RemoveAll();
	}
	else if (type == EA_StatucBar)
	{
		m_StatucBarAttendMerchs.RemoveAll();
	}
	else if (type == EA_Arbitrage)
	{
		m_aArbitrageAttendMerchs.RemoveAll();
	}
	else if (type == EA_Choose)
	{
		m_aIndexChsDataAttendMerchs.RemoveAll();
	}
	else if (type == EA_SimulateTrade)
	{
		m_aSimulateTradeAttendMerchs.RemoveAll();
	}
}

// 配置信息是否都返回了
void CViewData::IsRespConfigComplete()
{
	m_iRespConfigCnt++;
	if ( (m_iConfigCnt==m_iRespConfigCnt ) && (m_LoginWnd!=NULL))
	{
		PostMessage(m_LoginWnd, UM_ViewData_OnConfigSuccessTransToMain, 0, 0);	
	}
}

void CViewData::OnRespAuth(bool bSucc, IN const wchar_t *pszMsg)
{
	m_iRespConfigCnt = 0;

	// 认证成功
	if (bSucc)
	{
		if ( NULL == m_LoginWnd)
		{		
			return;
		}

		if (ELSAuthing != m_eLoginState)
		{
			ASSERT(0);
			return;
		}

		//
		m_eLoginState = ELSAuthSuccess;
		PostMessage(m_LoginWnd,UM_ViewData_OnAuthSuccessTransToMain, 0, 0);	
	}
	else
	{
		if (ELSAuthing != m_eLoginState)
		{
			ASSERT(0);
			return;
		}
		m_eLoginState = ELSLogining;

		// 通知登录对话框， 当前状态
		CString *pStrMsg = new CString(pszMsg);
		PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrMsg, (LPARAM)enETOther );
	}
}

void CViewData::OnRespUserRight(bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();

	// 获取用户权限列表
	if (bSucc)
	{
		T_RightInfo *pstRightList = NULL;
		int iRightListItemCount = 0;
		CPluginFuncRight::Instance().ClearRight();
		m_pServiceDispose->GetFuncRightList(&pstRightList, iRightListItemCount);
		if ( NULL != pstRightList )
		{
		
			for ( int i=0; i < iRightListItemCount ; i++ )
			{
				T_UserRightInfo stUserRight;
				stUserRight.iRightCode = pstRightList[i].iFunID;
				stUserRight.StrRightName = pstRightList[i].wszName;
				stUserRight.bRight = pstRightList[i].iHasFun;
				CPluginFuncRight::Instance().AddUserRight(stUserRight);
			}

			m_pServiceDispose->ReleaseData(pstRightList);
		}
	}
}

void CViewData::OnRespRightTip(bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();

	// 获取权限提示内容
	if (bSucc)
	{
		T_RightTip *pstRightTip = NULL;
		int iTipItemCount = 0;
		CPluginFuncRight::Instance().ClearRightTip();
		m_pServiceDispose->GetRightTip(&pstRightTip, iTipItemCount);
		if ( NULL != pstRightTip )
		{
		
			for ( int i=0; i < iTipItemCount; i++ )
			{
				T_RightPromptCode stRightTip;
				stRightTip.iRightCode = pstRightTip[i].iFunID;
				stRightTip.StrImgUrl = pstRightTip[i].wszImgUrl;
				stRightTip.StrLinkUrl = pstRightTip[i].wszLinkUrl;
				stRightTip.StrTitle = pstRightTip[i].wszTitle;

				CPluginFuncRight::Instance().AddRightTips(stRightTip);
			}

			m_pServiceDispose->ReleaseData(pstRightTip);
		}
	}
}

void CViewData::OnRespMsgTip(bool bSucc, IN const wchar_t *pszMsg)
{
	::SendMessage(m_MainFrameWnd, UM_InfoCenter, (WPARAM)pszMsg, 0);
}

void CViewData::OnRespQueryServerInfo(bool bSucc, IN const wchar_t *pszMsg)
{
	ASSERT(m_hBridgeWnd);
	if (!m_bReadyBreed)
	{
		if (!bSucc)
		{
			SendMessage(m_hBridgeWnd, UM_BridgeGetNetBreedFailed, 0, 0);
			return;
		}
	}
	
	if (!m_bReadyBreed)
	{						
		//	告诉前端，我正在处理
		SendMessage(m_hBridgeWnd, UM_BridgeBegin, 0, 0);
		//	优先网络的大市场数据，如果大市场数据没有，则直接用本地脱机市场文件，进行脱机登录
		{				
			//	
			GetNetBreedInfo(m_MerchManager);		
			if(0 != m_MerchManager.m_BreedListPtr.GetSize())
			{
				//	如果请求到最新的大市场数据，则直接拿来用
				m_iInitializedMarketCount = 0;
				m_iAllMarketCount = 0;
				for (int i =0; i < m_MerchManager.m_BreedListPtr.GetSize(); ++i)
				{
					const CBreed& bread = *m_MerchManager.m_BreedListPtr[i];
					m_iAllMarketCount += bread.m_MarketListPtr.GetSize();			;
				}	
				m_bBreedFromOfflineFile = false;
			}
			else
			{
				//	没有大市场数据，直接用脱机的
				CString tError;
				GetOfflineFileData(m_MerchManager, tError);		//	本地脱机大市场数据	
				m_bBreedFromOfflineFile = true;
			}		
		}		
		//	告诉前端，我已经处理完毕了
		SendMessage(m_hBridgeWnd, UM_BridgeEnd, 0, 0);

		//	大市场数已经准备好了
		m_bReadyBreed = true;	
	}
	else
	{
		IsRespConfigComplete();
	}
}

void CViewData::OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();
}

void CViewData::OnRespTextBannerConfig(IN bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();
}

void CViewData::OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();
}

void CViewData::OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();
}

void CViewData::OnRespQueryCusoptional(IN bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();
}

void CViewData::OnRespSaveCusoptional(IN bool bSucc, IN const wchar_t *pszMsg)
{

}

void CViewData::OnEconoData(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		//if (pIoView->IsAttendData(pMerch, EDSTPrice))
		{
			pIoView->OnVDataEconoData(pszRecvData);
		}
	}
}

void CViewData::OnRespStrategyData(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->OnVDataStrategyData(pszRecvData);
	}
}


void CViewData::OnRespPickModelTypeInfo(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	if (m_MainFrameWnd)
	{
		string strMsg;
		strMsg.empty();
		strMsg = pszRecvData;
		CString* pStr = new CString(strMsg.c_str());
		PostMessage(m_MainFrameWnd,UM_PickModelTypeInfo,(WPARAM)pStr,0);
	}
}

void CViewData::OnRespPickModelTypeStatus(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	if (m_MainFrameWnd)
	{
		string strMsg;
		strMsg.empty();
		strMsg = pszRecvData;
		CString* pStr = new CString(strMsg.c_str());
		PostMessage(m_MainFrameWnd,UM_PickModelTypeStatus,(WPARAM)pStr,0);
	}
}


void CViewData::OnRespQueryNewStockInfo(const char *pszRecvData, const bool bListedStock)
{
	if (!pszRecvData)
	{
		return;
	}

	if (m_MainFrameWnd != NULL)
	{
		string *pstrRecvData = new string(pszRecvData);
		PostMessage(m_MainFrameWnd,UM_MainFrame_NewStock_Resp,(WPARAM)pstrRecvData, (LPARAM)bListedStock);
	}
}


void CViewData::OnRespCheckToken(const char *pszRecvData)
{
	Json::Reader jsonReader;
	Json::Value	 jsonValue;
	if (jsonReader.parse(pszRecvData, jsonValue))
	{
		int32 iStatus = 0;

		iStatus = jsonValue["status"].asInt();
		if (0 != iStatus) // 成功
		{
			if (m_MainFrameWnd && !m_bKickOut )
			{
				m_bKickOut = true;
				string strMsg = jsonValue["msg"].asString().c_str();
				
				CString* pStr = new CString(strMsg.c_str());
				PostMessage(m_MainFrameWnd,UM_MainFrame_KickOut,(WPARAM)pStr,0);
			}
		}
	}
}

void CViewData::PackQuoteAuthJson(CString& strReq)
{
	T_UserInfo stUserInfo;
	if ( NULL == m_pServiceDispose )
	{
		return;
	}

	m_pServiceDispose->GetUserInfo(stUserInfo);
	string sUser, sToken, sProduct;
	UnicodeToGbk32(m_strUserName, sUser);
	UnicodeToGbk32(m_strProduct, sProduct);
	UnicodeToGbk32(stUserInfo.wszToken, sToken);

	Json::Value vData;
	vData["user"] = sUser;
	vData["userid"] = stUserInfo.iUserId;
	vData["token"] = sToken;
	vData["product"] = sProduct;
	vData["org_code"] = m_pServiceDispose->GetOrgCode();

	Json::FastWriter jsonWriter;
	string sOut = jsonWriter.write(vData);
	strReq = sOut.c_str();
}
bool32			CViewData::OfflineLogin(IN CString& StrUserName, IN const CString& tServerIP, IN const CString& tKey, IN const CString& orgCode)
{	
	// 定时器不要了
	InitPush(false);

	// 保存用户名
	m_strUserName = StrUserName;	

	// 创建私有目录
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(StrUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();

	// 设置请求类型
	m_pDataManager->ForceRequestType(EDSCommunication);

	vector<unsigned int> vtPort;
	//	连接认证服务器，为请求网络市场数据做准备
	if(!m_pAutherManager->GetInterface()->ConnectServer(tServerIP,vtPort))
	{
		ASSERT(0);
		return false;
	}

	// 请求服务器市场相关信息
	m_pAutherManager->GetInterface()->ReqQueryServerInfo(tKey, orgCode, 48, true, 1);


	CBlockConfig::Instance()->SetViewData(this);
	
	return true;
}
bool32 CViewData::OffLineInitial( IN CString& StrUserName, OUT CString& StrErr )
{
     StrErr.Empty();
 
      //设置标志
   /*  if ( !bOffLine )
     {
         return false;
     }
 */
     // 默认的用户名:
     //if ( !bLastUser || /*stUserInfo.m_aUserRights.GetSize() <= 0 ||*/ StrUserName.IsEmpty() )
     //{
     //    // ...fangz0107 待处理
     //    ASSERT(0);
     //    return false;
     //}
 
     // 禁用推送
    InitPush(false);
 
     // 保存用户名
     m_strUserName = StrUserName;
 
     // 初始化用户权限	
     T_RightInfo *pRightList = NULL;
     int iRightListItemCount = 0;
     m_pServiceDispose->GetFuncRightList(&pRightList, iRightListItemCount);
     if ( NULL != pRightList )
     {
         for ( int i=0; i < iRightListItemCount ; i++ )
         {
             TRACE(_T("UserRight: %s-%s\r\n"), pRightList[i].iFunID, pRightList[i].wszName);
 
             T_UserRightInfo userRight;
             userRight.iRightCode = pRightList[i].iFunID;
             userRight.StrRightName = pRightList[i].wszName;
             CPluginFuncRight::Instance().AddUserRight(userRight);
 
         }
         m_pServiceDispose->ReleaseData(pRightList);
     }
 
     // 创建私有目录
     CString StrPrivatePath;
     StrPrivatePath = CPathFactory::GetPrivateConfigPath(StrUserName);
     _tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
     StrPrivatePath.ReleaseBuffer();
     StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
     _tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
     StrPrivatePath.ReleaseBuffer();
 
     // 设置请求类型
     m_pDataManager->ForceRequestType(EDSOfflineData);

 	 CBlockConfig::Instance()->SetViewData(this);


	
      //市场列表那些信息的初始化
     if ( !InitialMarketInfoFromFile(m_MerchManager, StrErr) )
     {
		 ASSERT(0);
         return false;
     }

	 m_iInitializedMarketCount = 0;
	 m_iAllMarketCount = 0;
	 for (int i =0; i < m_MerchManager.m_BreedListPtr.GetSize(); ++i)
	 {
		 const CBreed& bread = *m_MerchManager.m_BreedListPtr[i];
		 m_iAllMarketCount += bread.m_MarketListPtr.GetSize();			;
	 }	

	 //	标志使用本地市场文件
	 m_bBreedFromOfflineFile = true;
	 //	已经准备好了大市场数据
	 m_bReadyBreed = true;	
    return true;
}

// 自选股同步
void CViewData::UploadUserBlock()
{
	// 获取服务器相关的板块 将服务器相关板块数据 组成对应格式Json格式后上传
	T_Block *pServerBlock = CUserBlockManager::Instance()->GetServerBlock();

	if ( NULL == pServerBlock )
	{
		return ;
	}

	// 编制Json格式数据
	const int32 iMerchCount = pServerBlock->m_aMerchs.GetSize();
	string strMerchCode = "", strMarketId = "";
	CString StrToken = L"";
	// 获取token
	StrToken = m_pServiceDispose->GetToken();
	Json::Value jsonData;
	CString StrMarketId = L"";
	int32 iDataType = 1;
	CString StrUserData = L"";

	if ( iMerchCount > 0 )
	{
		for ( int32 i=0; i < iMerchCount ; i++ )
		{
			Json::Value jsonMerch;
			CMerch *pMerch = pServerBlock->m_aMerchs[i];
			if ( NULL == pMerch )
			{
				continue;
			}
			StrMarketId.Format(L"%d", pMerch->m_MerchInfo.m_iMarketId);

			strMerchCode = _Unicode2MultiChar(pMerch->m_MerchInfo.m_StrMerchCode);
			strMarketId  = _Unicode2MultiChar(StrMarketId);

			jsonMerch["market"] = Json::Value(strMarketId);
			jsonMerch["code"]   = Json::Value(strMerchCode);
			jsonData.append(jsonMerch);
		}
		StrUserData = jsonData.toStyledString().c_str();
	}
	// 请求上传用户自选股数据
	 m_pAutherManager->GetInterface()->ReqSaveCusoptional(StrUserData, iDataType);
}

void CViewData::DownloadUserBlock()
{
	int32 iDataType = 1;		// 自选股类型为1
	
	// 请求下载用户自选股数据
	m_pAutherManager->GetInterface()->ReqQueryCusoptional(iDataType);

}

void CViewData::UpdateUserBlock()
{
	CViewData::MerchArray aMerchs;

	string strUserBlock = m_pAutherManager->GetInterface()->GetServiceDispose()->GetCusoptional();
	Json::Reader jsonReader;
	Json::Value  jsonValue;
	
	if (jsonReader.parse(strUserBlock, jsonValue))
	{
		int32 iSize = jsonValue.size();

		for(int i=0; i<iSize; ++i)
		{
			CMerch *pMerch = NULL;
			int32 iMarketId = atoi(jsonValue[i]["market"].asCString());
			CString StrCode = _A2W(jsonValue[i]["code"].asCString());
			if ( m_MerchManager.FindMerch(StrCode, iMarketId, pMerch) )
			{
				aMerchs.Add(pMerch);
			}
			else
			{
				TRACE(_T("服务器自选股忽略商品: %d - %s\r\n"), iMarketId, StrCode);
			}
		}

		// 如果没有默认的服务器自选板块，则新建，有则修改
		T_Block block;
		T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
		if ( NULL == pBlockServer )
		{
			block.m_eHeadType = ERTCustom;
			block.m_bServerBlock = true;		// 服务器有关的板块
			block.m_StrName	  = CUserBlockManager::GetDefaultServerBlockName();
			block.m_StrHotKey = ConvertHZToPY(block.m_StrName);
			bool32 b = CUserBlockManager::Instance()->AddUserBlock(block);
			ASSERT( b );
			bool32 bAdded = false;
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
				bAdded = bAdded || bAdd2;
			}
			if ( bAdded )
			{
				// 商品多的时候, 提高性能, 只通知和保存文件一次.
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
			}
		}
		else
		{
			block = *pBlockServer;
			// 商品列表合并还是取缔，暂时取合并
			bool32 bAdded = false;
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
				bAdded = bAdded || bAdd2;
			}
			if ( bAdded )
			{
				// 商品多的时候, 提高性能, 只通知和保存文件一次.
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
			}
		}
	}
}

void			CViewData::SetBridge(HWND hWnd)
{
	m_hBridgeWnd = hWnd;
	ASSERT(m_hBridgeWnd);
}

void CViewData::GetNetBreedInfo(CMerchManager& merchManager)
{
	m_iAllMarketCount = m_iInitializedMarketCount = 0;
	// 所有大市场
	T_BigMarketInfo* pBreedList = NULL;
	int iCountBreedList = 0;
	m_pServiceDispose->GetBigMarketList(&pBreedList, iCountBreedList);

	if ( 0 >= iCountBreedList )
	{
		//	繁殖所为0，异常啊
		ASSERT(0);				
		return;
	}

	// 所有小市场
	T_MarketInfo* pMarketInfo = NULL;
	int iCountMarket;
	m_pServiceDispose->GetMarketList(&pMarketInfo, iCountMarket);

	if ( 0 >= iCountMarket )
	{
		//	没有市场？？，异常啊
		ASSERT(0);
		return;
	}

	DWORD dwMerchManagerType = 0;
	for ( int32 iIndexBreed = 0; iIndexBreed < iCountBreedList; ++iIndexBreed )
	{
		// 某个具体的大市场
		T_BigMarketInfo stBreed = pBreedList[iIndexBreed];

		CBreed* pBreed = NULL;
		int32 iPosBreed = 0;

		if ( !merchManager.FindBreed(stBreed.iBigMarket, iPosBreed, pBreed) )
		{
			pBreed = new CBreed;
			ASSERT(NULL != pBreed);

			pBreed->m_iBreedId			= stBreed.iCode;
			pBreed->m_iShowId			= stBreed.iShowID;
			pBreed->m_StrBreedCnName	= stBreed.wszCnName;
			//			pBreed->m_StrBreedEnName	= stBreed.szEnName;

			// 添加大市场
			merchManager.AddBreed(pBreed);
		}

		for ( int32 iIndexMarket = 0; iIndexMarket < iCountMarket; ++iIndexMarket )
		{
			if ( pMarketInfo[iIndexMarket].iBigMarket == stBreed.iBigMarket )
			{
				// 设置该交易品种下的所有物理市场

				//lint --e{429}
				CMarket *pMarket = NULL;
				int32 iPosMarket = 0;

				if ( !pBreed->FindMarket(pMarketInfo[iIndexMarket].iMarket, iPosMarket, pMarket) )
				{
					pMarket = new CMarket(*pBreed);
					pMarket->m_MarketInfo.m_iMarketId = pMarketInfo[iIndexMarket].iMarket;
					pMarket->m_MarketInfo.m_iShowId   = pMarketInfo[iIndexMarket].iShowID;
					pMarket->m_MarketInfo.m_StrCnName = pMarketInfo[iIndexMarket].wszName;
					pMarket->m_MarketInfo.m_eMarketReportType =ERTStockCn;
					pBreed->AddMarket(pMarket);
				}

				if (ERTCustom == pMarket->m_MarketInfo.m_eMarketReportType)
				{
					pMarket->m_MarketInfo.m_eMarketReportType =ERTFuturesCn;
				}
				if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTFuturesCn )
				{
					dwMerchManagerType |= EMMT_FutureCn;
				}
				else if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn )
				{
					dwMerchManagerType |= EMMT_StockCn;
				}

				m_iAllMarketCount++;

				if ( pMarket->IsInitialized() )
				{
					m_iInitializedMarketCount++;
				}
			}
		}
	}

	// 释放资源
	m_pServiceDispose->ReleaseData(pBreedList);

	// 释放资源
	m_pServiceDispose->ReleaseData(pMarketInfo);

	m_dwMerchManagerType = dwMerchManagerType;
}

void CViewData::ForceUpdateVisibleAttentIoView()
{
	if (0 == m_IoViewList.GetSize())
	{
		return;
	}
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		if (NULL != pIoView)
		{
			pIoView->ForceUpdateVisibleIoView();
		}		
	}
}

//	从脱机市场文件中加载数据到内存中
bool32	CViewData::GetOfflineFileData(CMerchManager& merchManager, CString& StrErr)
{

	StrErr = L"";
	//
	CString strPath = CPathFactory::GetMarketInfoFilePath();

	if ( strPath.IsEmpty() )
	{
		ASSERT(0);
		StrErr = L"获取配置文件路径失败";
		return false;
	}

	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		//ASSERT(0);
		StrErr = L"装载配置文件失败";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);
		StrErr = L"配置文件读取失败";
		return false;
	}

	// 市场类型
	DWORD dwMerchManagerType = EMMT_None;

	merchManager.Clear();

	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// 读取 breed 的属性				
		const char* pStrBreedID	   = pBreedElement->Attribute(KStrAttriBreedID);
		const char* pStrBreedShowID = pBreedElement->Attribute(KStrAttriBreedShowID);
		const char* pStrBreedNameCn = pBreedElement->Attribute(KStrAttriBreedNameCn);
		const char* pStrBreedNameEn = pBreedElement->Attribute(KStrAttriBreedNameEn);

		if ( NULL == pStrBreedID || NULL == pStrBreedShowID || NULL == pStrBreedNameCn )
		{			
			ASSERT(0);
			continue;
		}

		int32 iID			= atoi(pStrBreedID);
		int32 iShowID		= atoi(pStrBreedShowID);

		wstring StrNameCn, StrNameEn;

		MultiChar2Unicode(CP_UTF8, pStrBreedNameCn, StrNameCn);
		MultiChar2Unicode(CP_UTF8, pStrBreedNameEn, StrNameEn);

		if ( iID < 0 || iShowID < 0 || StrNameCn.length() <= 0 )
		{
			ASSERT(0);
			continue;
		}

		//
		CBreed* pBreed = new CBreed();
		pBreed->m_iBreedId = iID;
		pBreed->m_iShowId  = iShowID;
		pBreed->m_StrBreedCnName = StrNameCn.c_str();
		pBreed->m_StrBreedEnName = StrNameEn.c_str();

		merchManager.AddBreed(pBreed);

		// 再遍历这个Breed 下面的市场:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// 市场属性
			const char* pStrMarketID	   = pMarketElement->Attribute(KStrAttriMarketID);
			const char* pStrMarketShowID = pMarketElement->Attribute(KStrAttriMarketShowID);
			const char* pStrZone   = pMarketElement->Attribute(KStrAttriMarketTimeZone);
			const char* pStrMarketNameCn = pMarketElement->Attribute(KStrAttriMarketNameCn);
			const char* pStrMarketNameEn = pMarketElement->Attribute(KStrAttriMarketNameEn);
			const char* pStrRType  = pMarketElement->Attribute(KStrAttriMarketReportType);
			const char* pStrScale  = pMarketElement->Attribute(KStrAttriMarketVolScale);
			const char* pStrIniDay = pMarketElement->Attribute(KStrAttriMarketIniPerDay);
			const char* pStrOCTime = pMarketElement->Attribute(KStrAttriMarketOCTimes);
			const char* pStrActDay = pMarketElement->Attribute(KStrAttriMarketActiveDay);

			if ( NULL == pStrMarketID || NULL == pStrMarketShowID || NULL == pStrMarketNameCn || NULL == pStrRType || NULL == pStrScale || NULL == pStrIniDay || NULL == pStrOCTime || NULL == pStrActDay )
			{	
				ASSERT(0);
				continue;
			}

			int32 iMarketID			= atoi(pStrMarketID);
			int32 iMarketShowID		= atoi(pStrMarketShowID);
			int32 iTimeZone		= atoi(pStrZone);
			int32 iReportType	= atoi(pStrRType);
			int32 iVolScale		= atoi(pStrScale);
			//int32 iInitialPerDay= atoi(pStrIniDay);

			wstring  StrOctime, StrActDay;

			MultiChar2Unicode(CP_UTF8, pStrMarketNameCn, StrNameCn);
			MultiChar2Unicode(CP_UTF8, pStrMarketNameEn, StrNameEn);
			MultiChar2Unicode(CP_UTF8, pStrOCTime, StrOctime);
			MultiChar2Unicode(CP_UTF8, pStrActDay, StrActDay);

			// showid判断暂时去掉， 运维配置后再判断 cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0 */|| iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			// 
			// 全局释放的时候会释放，在函数内屏蔽429错误
			//lint --e{429} 

			CMarket* pMarket = new CMarket(*pBreed);
			pMarket->m_MarketInfo.m_iMarketId	= iMarketID;
			pMarket->m_MarketInfo.m_iShowId		= iMarketShowID;
			pMarket->m_MarketInfo.m_iTimeZone	= iTimeZone;			
			pMarket->m_MarketInfo.m_StrCnName	= StrNameCn.c_str();
			pMarket->m_MarketInfo.m_StrEnName	= StrNameEn.c_str();

			// 取开收盘时间
			if ( !SplitMarketOCTimeString(StrOctime.c_str(), pMarket->m_MarketInfo.m_OpenCloseTimes) )
			{
				ASSERT(0);
				continue;
			}

			// 取最近交易日
			if ( !SplitMarketActiveDays(StrActDay.c_str(), pMarket->m_MarketInfo.m_aRecentActiveDay) )
			{
				ASSERT(0);
				continue;
			}

			//
			pMarket->m_MarketInfo.m_eMarketReportType = (E_ReportType)iReportType;

			//
			if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTFuturesCn )
			{
				dwMerchManagerType |= EMMT_FutureCn;
			}
			else if ( pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn )
			{
				dwMerchManagerType |= EMMT_StockCn;
			}

			//
			pBreed->AddMarket(pMarket);

			continue;
			// 再遍历这个市场下面的商品:
			for ( TiXmlElement* pMerchElement = pMarketElement->FirstChildElement(); NULL != pMerchElement; pMerchElement = pMerchElement->NextSiblingElement() )
			{
				// 商品属性
				const char* pStrMerchCode   = pMerchElement->Attribute(KStrAttriMerchCode);
				const char* pStrMerchNameCn = pMerchElement->Attribute(KStrAttriMerchNameCn);
				const char* pStrMerchNameEn = pMerchElement->Attribute(KStrAttriMerchNameEn);
				const char* pStrDec    = pMerchElement->Attribute(KStrAttriMerchSaveDec);

				//
				if ( NULL == pStrMerchCode || NULL == pStrMerchNameCn || NULL == pStrDec )
				{
					ASSERT(0);
					continue;
				}

				//
				int32 iDec = atoi(pStrDec);
				wstring StrMerchCode, StrMerchNameCn, StrMerchNameEn;

				MultiChar2Unicode(CP_UTF8, pStrMerchCode, StrMerchCode);
				MultiChar2Unicode(CP_UTF8, pStrMerchNameCn, StrMerchNameCn);
				MultiChar2Unicode(CP_UTF8, pStrMerchNameEn, StrMerchNameEn);				

				if ( iDec < 0 || StrMerchCode.length() <= 0 || StrNameCn.length() <= 0 )
				{
					ASSERT(0);
					continue;
				}

				//
				CMerchInfo MerchInfo;
				MerchInfo.m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
				MerchInfo.m_StrMerchCode	= StrMerchCode.c_str();
				MerchInfo.m_StrMerchCnName= StrMerchNameCn.c_str();
				MerchInfo.m_StrMerchEnName= StrMerchNameEn.c_str();
				MerchInfo.m_iSaveDec		= iDec;

				pMarket->AddMerch(MerchInfo);
			}

			//pMarket->SetInitialized(true); 不需要设置true，登录可以重新校对一次
		}
	}
	return true;
}

void	CViewData::InitPush(bool32 bEnable)
{
	// 定时器
	if (bEnable)
	{
		StartTimer(KTimerIdFreeMemory, KTimerPeriodFreeMemory);
		StartTimer(KTimerIdSyncPushing, KTimerPeriodSyncPushing);
		StartTimer(KTimerIdInitializeMarket, KTimerPeriodInitializeMarket);
		StartTimer(KTimerIdUpdateServerTime, KTimerPeriodUpdateServerTime);
	}
	else
	{
		StopTimer(KTimerIdFreeMemory);
		StopTimer(KTimerIdSyncPushing);
		StopTimer(KTimerIdInitializeMarket);
		StopTimer(KTimerIdUpdateServerTime);
	}

}

bool32	CViewData::IsOffLineLogin()
{
	return m_bBreedFromOfflineFile || !m_bReadyBreed;
}
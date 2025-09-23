#ifndef _VIEW_DATA_H_
#define _VIEW_DATA_H_

#pragma warning(disable: 4786)

#include <map>
#include "typedef.h"
#include "XTimer.h"

#include "PlugInStruct.h"
#include "commmanager.h"
#include "NewsManager.h"
#include "datamanager.h"
#include "hotkey.h"
#include "RelativeMerchManager.h"
#include "iAuthNotify.h"
#include "OptimizeServer.h"
#include "ViewDataListener.h"
#include "SocketClient.h"
#include "ProxyAuthClientHelper.h"

struct T_PushServerAndTime
{
public:
	T_PushServerAndTime()
	{
		m_iCommunicationId	= 0;
		m_uiUpdateTime		= 0;
	}

public:
	int32			m_iCommunicationId;
	uint32			m_uiUpdateTime;
};

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
typedef struct T_ErrCode2StrInfo
{
public:
	int errCode;
	CString errStr;
public:
	T_ErrCode2StrInfo(int iCode, CString strErr)
	{
		errCode = iCode;
		errStr  = strErr;
	}
}T_ErrCode2StrInfo;

class CIndexChsStkMideCore;

/////////////////////////////////////////////////////////////
//									   	
class DATACENTER_DLL_EXPORT CViewData : public CXTimerAgent, public CAbsCNewsManagerNotify, public CDataManagerNotify, public iAuthNotify, public COptimizeServerNotify
{
public:
	typedef CArray<CMerch *, CMerch *>		MerchArray;

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

	enum E_AttendMerchType	  //关注商品类型
	{
		EA_Arbitrage = 0,	  //套利
		EA_StatucBar ,		   //状态栏
		EA_Alarm,				//预警
		EA_RecordData,			//数据库
		EA_Choose,				//选股
		EA_SimulateTrade,		// 模拟交易商品
	};

public:
	CViewData(CCommManager *pCommManager, CProxyAuthClientHelper* pAutherManager, CNewsManager* pNewsManager, CDataManager* pDataManager, CAbsOfflineDataManager &OfflineDataManager, HWND OwnerWnd);
	~CViewData();

public: 
	bool32			Construct();
	
public:
	void			SetOwnerWnd(HWND hWndOwner) { m_OwnerWnd = hWndOwner; }
	void			SetServiceDispose(iServiceDispose *pServiceDispose) { if (NULL == pServiceDispose) return; m_pServiceDispose = pServiceDispose;}
	iServiceDispose *GetServiceDispose(){ return m_pServiceDispose; }
	CAbsOfflineDataManager& GetOfflineDataManager() { return m_OfflineDataManager; }
public:
	// 业务视图队列相关
	void			AddViewListener(IN CIoViewListner *pIoView);
	void			RemoveViewListener(IN CIoViewListner *pIoView);
	

	
	// 请求数据
	void			RequestViewData(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID = -1,IN int eDataSource = EDSAuto);


	
	// 请求资讯
	void			RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationID = -1);


	// 返回当前服务器时间（格林威治时间）
	CGmtTime		GetServerTime(); 

	// 返回某个市场所在时区的的当前时间（相对于服务器的格林威治时间加上该市场的时差）
	CGmtTime		GetMarketCurrentTime(const CMarket &Market);

	void			OnAuthSuccessMain(bool bOfflineLogin);
	
	void			OnMsgCodeIndexUpdate(IN const CString& StrCode);
	
	//根据错误码，返回错误字符串
	CString         GetPlusErrorStr(int errCode);
public:
	void			InitializeMarket(CMarket &Market);

public:
	// from CDataManagerNotify
	virtual	void	OnDataServiceConnected(int32 iServiceId);
	virtual	void	OnDataServiceDisconnected(int32 iServiceId);
	virtual	void	OnDataRequestTimeOut(int32 iMmiReqId);
	virtual	void	OnDataCommResponse();
	virtual void	OnDataServerConnected(int32 iCommunicationId);
	virtual void	OnDataServerDisconnected(int32 iCommunicationId);
	virtual void	OnDataSyncServerTime(CGmtTime &TimeServerNow);
	virtual void	OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);

	// from CAbsCNewsManagerNotify
	virtual void	OnNewsResponse();
	virtual void	OnNewsServerConnected(int32 iCommunicationId);
	virtual void	OnNewsServerDisconnected(int32 iCommunicationId);

	// from IAuthNotify
public:
	virtual void OnRespAuth(bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespUserRight(bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespRightTip(bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespMsgTip(bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespUserDealRecord(bool bSucc, IN const wchar_t *pszMsg){}

	virtual void OnRespQueryServerInfo(bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespTextBannerConfig(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespSaveCusoptional(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnRespQueryCusoptional(IN bool bSucc, IN const wchar_t *pszMsg);

	virtual void OnEconoData(const char *pszRecvData);

	virtual void OnRespStrategyData(const char *pszRecvData);

	virtual void OnRespPickModelTypeInfo(const char *pszRecvData);

	virtual void OnRespPickModelTypeStatus(const char *pszRecvData);

	virtual void OnRespQueryNewStockInfo(const char *pszRecvData, const bool bListedStock);

	virtual void OnRespCheckToken(const char *pszRecvData);


	virtual void OnRespPushMsgList(const char *pszRecvData) {};
	virtual void OnRespPushMsgDetail(const char *pszRecvData) {};
	

	// from COptimizeServerNotify	
public:
	virtual void	OnFinishSortServer();
	virtual void	OnFinishAllPing();			
	
	// from CXTimerAgent
public:
	virtual	void	OnXTimer(int32 iTimerId);


	// DataManager事件的本线程处理接口， 与CDataManagerNotify定义的接口一一对应
public:
	void			OnMsgServiceConnected(int32 iServiceId);
	void			OnMsgServiceDisconnected(int32 iServiceId);
	void			OnMsgRequestTimeOut(int32 iMmiReqId);
	void			OnMsgCommResponse();
	void			OnMsgServerConnected(int32 iCommunicationId);
	void			OnMsgServerDisconnected(int32 iCommunicationId);
	void			OnMsgServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);

	// 其他异步通知事件
	void			OnMsgKLineNodeUpdate(IN CMerch *pMerch);
	void			OnMsgTimesaleUpdate(IN CMerch *pMerch);
	void			OnMsgPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	void			OnMsgRealtimePriceUpdate(IN CMerch *pMerch);
	void			OnMsgRealtimeLevel2Update(IN CMerch *pMerch);
	void			OnMsgRealtimeTickUpdate(IN CMerch *pMerch);
	void			OnMsgClientTradeTimeUpdate();

	//
	void			OnMsgNewsListUpdate();
	void			OnMsgF10Update(IN CMerch* pMerch);
	void			OnMsgLandMineUpdate(IN CMerch* pMerch);

	//
	void			OnMsgOnGeneralNormalUpdate(IN CMerch* pMerch);
	void			OnMsgOnGeneralFinanaceUpdate(IN CMerch* pMerch);
	void			OnMsgOnMerchTrendIndexUpdate(IN CMerch* pMerch);
	void			OnMsgOnMerchAuctionUpdate(IN CMerch* pMerch);
	void			OnMsgOnMerchMinuteBSUpdate(IN CMerch* pMerch);

	// 除权类型变化
	void			OnWeightTypeChange();

	// 板块文件超时
	void			OnBlockFileTranslateTimeout();

public:
	// datamanager应答数据
	void			OnDataRespAuth(int iMmiReqId, IN const CMmiRespAuth *pMmiRespAuth);
	void			OnDataRespMarketInfo(int iMmiReqId, IN const CMmiRespMarketInfo *pMmiRespMarketInfo);
	void			OnDataRespMarketSnapshot(int iMmiReqId, IN const CMmiRespMarketSnapshot *pMmiRespMarketSnapshot);
	void			OnDataRespBroker(int iMmiReqId, IN const CMmiRespBroker *pMmiRespBroker);
	void			OnDataRespMerchInfo(int iMmiReqId, IN const CMmiRespMerchInfo *pMmiRespMerchInfo);
	void			OnDataRespPublicFile(int iMmiReqId, IN const CMmiRespPublicFile *pMmiRespPublicFile);
	void			OnDataRespLogicBlockFile(int iMmiReqId, IN const CMmiRespLogicBlockFile *pMmiRespLogicBlockFile);
	void			OnDataRespMerchExtendData(int iMmiReqId, IN const CMmiRespMerchExtendData *pMmiRespMerchExtendData);
	void			OnDataRespMerchF10(int iMmiReqId, IN const CMmiRespMerchF10 *pMmiRespMerchF10);
	void			OnDataRespRealtimePrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespRealtimePrice *pMmiRespRealtimePrice);
	void			OnDataRespRealtimeLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespRealtimeLevel2 *pMmiRespRealtimeLevel2);
	void			OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine);
	void			OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);
	void			OnDataRespReport(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespReport *pMmiRespReport);
	void			OnDataRespBlockReport(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespBlockReport *pMmiRespBlockReport);
	void			OnDataAnsRegisterPushPrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushPrice *pMmiAnsRegisterPushPrice);
	void			OnDataAnsRegisterPushTick(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushTick *pMmiAnsRegisterPushTick);
	void			OnDataAnsRegisterPushLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushLevel2 *pMmiAnsRegisterPushLevel2);
	void			OnDataRespCancelAttendMerch(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespCancelAttendMerch *pMmiRespCancelAttendMerch);
	void			OnDataRespError(int32 iMmiReqId, IN const CMmiRespError *pMmiRespError);

	void			OnDataPushPrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushPrice *pMmiPushPrice);
	void			OnDataPushTick(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushTick *pMmiPushTick);
	void			OnDataPushLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiPushLevel2 *pMmiPushLevel2);

	void			OnDataKickOut(int iMmiReqId, int32 iCommunicationId, IN const CMmiKickOut *pMmiKickOut);
	void			OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest);

	//
	void			OnDataRespGeneralNormal(int iMmiReqId, IN const CMmiRespGeneralNormal *pMmiRespCMmiRespGeneralNormal);
	void			OnDataRespGeneralFiniance(int iMmiReqId, IN const CMmiRespGeneralFinance *pMmiRespGeneralFinance);
	void			OnDataRespMerchTrendIndex(int iMmiReqId, IN const CMmiRespMerchTrendIndex *pMmiRespMerchTrendIndex);
	void			OnDataRespMerchAuction(int iMmiReqId, IN const CMmiRespMerchAuction *pMmiRespMerchAuction);

	void			OnDataRespFundHold(int iMmiReqId, IN const CMmiRespFundHold *pMmiRespFundHold);
	void			AbstractFundHoldInfo(uint32 iDataLen, const char* pData);

	void			OnDataPushGeneralNormal(int iMmiReqId, int32 iCoummunicationId, IN const CMmiRespGeneralNormal *pMmiRespCMmiRespGeneralNormal);
	void			OnDataPushMerchTrendIndex(int iMmiReqId, int32 iCoummunicationId, IN const CMmiRespMerchTrendIndex *pMmiRespMerchTrendIndex);

	void			OnDataRespMinuteBS(int iMmiReqId, IN const CMmiRespMinuteBS *pMmiRespMinuteBS);
	void			OnDataRespClientTradeTime(int iMmiReqID, IN const CMmiRespTradeTime* pMmiRespTradeTime);

	// ioview请求数据
	void			OnViewReqAuth(IN const CMmiReqAuth *pMmiReqAuth, IN int32 iCommunicationID = -1);
	void			OnViewReqMarketInfo(IN const CMmiReqMarketInfo *pMmiReqMarketInfo);
	void			OnViewReqMarketSnapshot(IN const CMmiReqMarketSnapshot *pMmiReqMarketSnapshot);
	void			OnViewReqBroker(IN const CMmiReqBroker *pMmiReqBroker);
	void			OnViewReqMerchInfo(IN const CMmiReqMerchInfo *pMmiReqMerchInfo);
	void			OnViewReqPublicFile(IN const CMmiReqPublicFile *pMmiReqPublicFile);

	void			OnViewReqMerchExtendData(IN const CMmiReqMerchExtendData *pMmiReqMerchExtendData);
	void			OnViewReqMerchF10(IN const CMmiReqMerchF10 *pMmiReqMerchF10);
	void			OnViewReqRealtimePrice(IN const CMmiReqRealtimePrice *pMmiReqRealtimePrice);
	void			OnViewReqRealtimeTick(IN const CMmiReqRealtimeTick *pMmiReqRealtimeTick);
	void			OnViewReqRealtimeLevel2(IN const CMmiReqRealtimeLevel2 *pMmiReqRealtimeLevel2);
	void			OnViewReqMerchKLine(IN const CMmiReqMerchKLine *pMmiReqMerchKLine);
	void			OnViewReqMerchTimeSales(IN const CMmiReqMerchTimeSales *pMmiReqMerchTimeSales);
	void			OnViewReqReport(IN const CMmiReqReport *pMmiReqReport);
	void			OnViewReqBlockReport(IN const CMmiReqBlockReport *pMmiReqBlockReport);
	void			OnViewRegisterPushPrice(IN const CMmiRegisterPushPrice *pMmiRegisterPushPrice);
	void			OnViewRegisterPushTick(IN const CMmiRegisterPushTick *pMmiRegisterPushPrice);
	void			OnViewRegisterPushLevel2(IN const CMmiRegisterPushLevel2 *pMmiRegisterPushPrice);

	void			OnViewReqNetTest(IN const CMmiReqNetTest *pMmiReqNetTest, IN int32 iCommunicationID);

	//
	void			OnViewReqGeneralNormal(IN const CMmiReqGeneralNormal *pMmiReqGeneralNormal, IN int32 iCommunicationID);
	void			OnViewReqGeneralFiniance(IN const CMmiReqGeneralFinance *pMmiReqGeneralFinance, IN int32 iCommunicationID);
	void			OnViewReqMerchTrendIndex(IN const CMmiReqMerchTrendIndex *pMmiReqMerchTrendIndex, IN int32 iCommunicationID);
	void			OnViewReqMerchAuction(IN const CMmiReqMerchAution *pMmiReqMerchAution, IN int32 iCommunicationID);
	
	void			OnViewReqAddPushGeneralNormal(IN const CMmiReqAddPushGeneralNormal *pMmiReqAddPushGeneralNormal, IN int32 iCommunicationID);
	void			OnViewReqUpdatePushGeneralNormal(IN const CMmiReqUpdatePushGeneralNormal* pMmiReqUpdatePushGeneralNormal, IN int32 iCommunicationID);
	void			OnViewReqDelPushGeneralNormal(IN const CMmiReqDelPushGeneralNormal* pMmiReqDelPushGeneralNormal, IN int32 iCommunicationID);

	void			OnViewReqAddPushMerchTrendIndex(IN const CMmiReqAddPushMerchTrendIndex *pMmiReqAddPushMerchTrendIndex, IN int32 iCommunicationID);
	void			OnViewReqUpdatePushMerchTrendIndex(IN const CMmiReqUpdatePushMerchTrendIndex *pMmiReqUpdatePushMerchTrendIndex, IN int32 iCommunicationID);
	void			OnViewReqDelPushMerchTrendIndex(IN const CMmiReqDelPushMerchTrendIndex *pMmiReqDelPushMerchTrendIndex, IN int32 iCommunicationID);
	
	void			OnViewReqFundHold(IN const CMmiReqFundHold *pMmiReqFundHold, IN int32 iCommunicationID);
	
	void			OnViewReqMinuteBS(IN const CMmiReqMinuteBS *pMmiReqMinuteBS, IN int32 iCommunicationID);

	void			OnViewReqAddPushMinuteBS(IN const CMmiReqAddPushMinuteBS *pMmiReqAddPushMinuteBS, IN int32 iCommunicationID);
	void			OnViewReqUpdatePushMinuteBS(IN const CMmiReqUpdatePushMinuteBS *pMmiReqUpdatePushMinuteBS, IN int32 iCommunicationID);
	void			OnViewReqDelPushMinuteBS(IN const CMmiReqDelPushMinuteBS *pMmiReqDelPushMinuteBS, IN int32 iCommunicationID);
	void			OnReqClientTradeTime(IN const CMmiReqTradeTime* pMmiReqTradeTime, IN int32 iCommunicationID);
	// NewsEngine 事件接口
public:
	virtual void	OnMsgNewsResponse();
	virtual void	OnMsgNewsServerConnected(int32 iCommunicationId);
	virtual void	OnMsgNewsServerDisconnected(int32 iCommunicationId);


	// 处理应答
	void			OnNewsRespNewsList(int32 iMmiRespId, IN const CMmiNewsRespNewsList* pMmiNewsRespNewsList);
	void			OnNewsRespF10(int32 iMmiRespId, IN const CMmiNewsRespF10* pMmiNewsRespF10);
	void			OnNewsRespLandMine(int32 iMmiRespId, IN const CMmiNewsRespLandMine* pMmiNewsRespLandMine);
	void            OnNewsRespTitleList(int32 iMmiRespId, IN const CMmiNewsRespTitleList* pMmiNewsResp);//test2013-5-19
	void	        OnNewsRespNewsContent(int32 iMmiRespId, IN const CMmiNewsRespNewsContent* pMmiNewsResp);
	void			OnNewsRespCodeIndex(int32 iMmiRespId, IN const CMmiNewsRespCodeIndex* pMmiNewsRespCodeIndex);
	void			OnNewsPushNewsTitle(int32 iMmiRespId, IN const CMmiNewsPushNewsTitle* pMmiNewsPush);
	// 新增插件的处理
public:  
	// 请求

	void			OnViewReqBase(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID = -1,IN int eDataSource = EDSAuto);

	void			OnViewReqPlugInData(IN const CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID);

	void			OnViewReqMerchIndex(IN const CMmiReqMerchIndex* pMimiComBasePlugIn);

	void			OnViewReqBlock(IN const CMmiReqLogicBlock* pMimiComBasePlugIn);

	void			OnViewReqMerchSort(IN const CMmiReqMerchSort* pMimiComBasePlugIn);

	void            OnViewReqPeriodMerchSort(IN const CMmiReqPeriodMerchSort* pMimiComBasePlugIn);

	void			OnViewReqBlockSort(IN const CMmiReqBlockSort* pMimiComBasePlugIn);

	void			OnViewReqMainMonitor(IN const CMmiReqMainMonitor* pMimiComBasePlugIn);

	void            OnViewReqShortMonitor(IN const CMmiReqShortMonitor* pMimiComBasePlugIn);

	void			OnViewReqTickEx(IN const CMmiReqTickEx* pMimiComBasePlugIn);

	void			OnViewReqPeriodTickEx(IN const CMmiReqPeriodTickEx* pMimiComBasePlugIn);

	void			OnViewReqHistoryTickEx(IN const CMmiReqHistoryTickEx* pMimiComBasePlugIn);

	void			OnViewReqChooseStock(IN const CMmiReqChooseStock* pMimiComBasePlugIn);

	void			OnViewReqCRTEStategyChooseStock(IN const CMmiReqCRTEStategyChooseStock* pMimiComBasePlugIn);

	void			OnViewReqDapanState(IN const CMmiReqDapanState* pMimiComBasePlugIn);

	void			OnViewReqChooseStockStatus(IN const CMmiReqChooseStockStatus* pMimiComBasePlugIn);

	void			OnViewReqDKMoney(IN const CMmiReqDKMoney* pMimiComBasePlugIn);

	void			OnViewReqMainMasukura(IN const CMmiReqMainMasukura* pMimiComBasePlugIn);

	//
	void			OnViewReqAddPushPlugInMerchData(IN const CMmiReqPushPlugInMerchData* pMimiComBasePlugIn);

	void			OnViewReqAddPushPlugInBlockData(IN const CMmiReqPushPlugInBlockData* pMimiComBasePlugIn);

	void			OnViewReqAddPushPlugInChooseStock(IN const CMmiReqPushPlugInChooseStock* pMimiComBasePlugIn);

	//
	void			OnViewReqUpdatePushPlugInMerchData(IN const CMmiReqUpdatePushPlugInMerchData* pMimiComBasePlugIn);
	
	void			OnViewReqUpdatePushPlugInBlockData(IN const CMmiReqUpdatePushPlugInBlockData* pMimiComBasePlugIn);
	
	void			OnViewReqUpdatePushPlugInChooseStock(IN const CMmiReqUpdatePushPlugInChooseStock* pMimiComBasePlugIn);

	//
	void			OnViewReqRemovePushPlugInMerchData(IN const CMmiReqRemovePushPlugInMerchData* pMimiComBasePlugIn);
	
	void			OnViewReqRemovePushPlugInBlockData(IN const CMmiReqRemovePushPlugInBlockData* pMimiComBasePlugIn);
	
	void			OnViewReqRemovePushPlugInChooseStock(IN const CMmiReqRemovePushPlugInChooseStock* pMimiComBasePlugIn);

	// 应答
	void			OnDataRespPlugInData(int iMmiReqId, IN const CMmiCommBasePlugIn *pMmiCommBase);

	void			OnDataRespMerchIndex(int iMmiReqId, IN const CMmiRespMerchIndex *pMmiRespMerchIndex);

	void			OnDataRespBlock(int iMmiReqId, IN const CMmiRespLogicBlock *pMmiRespLogicBlock);

	void			OnDataRespMerchSort(int iMmiReqId, IN const CMmiRespMerchSort *pMmiRespMerchSort);

	void			OnDataRespPeriodMerchSort(int iMmiReqId, IN const CMmiRespPeriodMerchSort *pMmiRespMerchSort);

	void			OnDataRespBlockSort(int iMmiReqId, IN const CMmiRespBlockSort *pMmiRespBlockSort);

	void			OnDataRespMainMonitor(int iMmiReqId, IN const CMmiRespMainMonitor *pMmiRespMainMonitor);

	void			OnDataRespTickEx(int iMmiReqId, IN const CMmiRespTickEx *pMmiRespTickEx);

	void			OnDataRespHistoryTickEx(int iMmiReqId, IN const CMmiRespHistoryTickEx *pMmiRespHistoryTickEx);

	void			OnDataRespChooseStock(int iMmiReqId, IN const CMmiRespChooseStock *pMmiRespChooseStock);

	void			OnDataRespCRTEStategyChooseStock(int iMmiReqId, IN const CMmiRespCRTEStategyChooseStock *pMmiRespChooseStock);

	void			OnDataRespDapanState(int iMmiReqId, IN const CMmiRespDapanState *pMmiRespDapanState);

	void			OnDataRespChooseStockStatus(int iMmiReqId, IN const CMmiRespChooseStockStatus *pMmiRespChooseStockStatus);

	void			OnDataRespPushFlag(int iMmiReqId, IN const CMmiRespPlugInPushFlag *pMmiRespPlugInPushFlag);

	void			OnDataRespDKMoney(int iMmiReqId, IN const CMmiRespDKMoney *pMmiRespDKMoney);

	void			OnDataRespMainMasukura(int iMmiReqId, IN const CMmiRespMainMasukura *pMmiRespMainMasukura);

// 新增的插件认证心跳包请求
	void			OnReqAuthPlugInHeart(IN const CMmiReqAuthPlugInHeart* pMimiComBase, IN int32 iCommunicationID);

	void			OnDataRespAuthPlugInHeart(int iMmiReqId, IN const CMmiRespAuthPlugInHeart* pMmiResp);

	void			OnMsgAuthPlugInHeartResp(int iRet);

public:
	// 
	void			FindRelativeMerchs(IN CMerch *pMerch, OUT CArray<CRelativeMerchNode, CRelativeMerchNode&> &AllRelativeMerchs);
	void			OnRelativeMerchsFileUpdate();
	void			OnHkWarrantsFileUpdate();

private:
	// 当某一个市场的某些初始化数据回来
	void			OnSomeInitializeMarketDataRet(CMarket &Market);
	
	// 当全部的市场初始化成功
	void			OnAllMarketInitializeSuccess();	

	// 正在登录请求的时候, 服务器掉线(网络原因或者长时间未收到数据自动重连)
	void			ReLogInOnServerDisconnected(int32 iCommunicationId);

public:
	// 定时清理内存
	void			OnTimerFreeMemory(bool32 bForceFree = false);

	// 定时同步推送
	void			OnTimerSyncPushing();

	// 定时检查市场初始化 
	void			OnTimerInitializeMarket();

	// 定时更新服务器时间
	void			OnTimerUpdateServerTime();

	// 登录检查行情连接
	void			OnTimerLogWaitQuoteConnect();

	// 等待直连行情连接
	void			OnTimerWaitDirectQuoteConnect();

	// 板块传输文件超时处理
	void			OnTimerLogicBlockTraslateTimeOut();

	// 认证插件心跳包
	void			OnTimerPlugInAuthHeart();

	// 停止所有行情连接
	void			DisconnectAllQuoteServer();

	// 停止所有资讯连接
	void			DisconnectAllNewsServer();

private:
	// 检查是否有业务视图关注某个商品的某种数据业务
	bool32			IsSomeIoViewAttendTheMerch(IN CMerch *pMerch, int32 iDataSeriveTypes);

public:
	void			OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	void			OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId);
	void			OnRealtimeLevel2(const CRealtimeLevel2 &RealtimeLevel2, int32 iCommunicationId);

private:
	void			ForceUpdateVisibleIoView(CIoViewListner *pIoView);

	// 快捷键相关
public:
	//void			RemoveHotKey(E_HotKeyType eHotKeyType);
	//void			AddHotKey(CHotKey &HotKey);
	void			BuildMerchHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);
	//void			BuildShortCutHotKeyList();
	//void			BuildUserBlockHotKeyList();
	
	// 条件选股相关
public:
	void			SetIndexChsStkMideCore(CIndexChsStkMideCore* pMidCore);
	void			SetIndexChsStkReqNums(int32 iNumReqs);
	bool32			GetRespMerchKLineForIndexChsStk(OUT CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>& RecvDataForIndexChsStk);

	// 条件预警
	//CAlarmCenter*	GetAlarmCenter() const { return m_pAlarmCenter; }

 	bool            GetTodayFlag(); //是否有今评
 	void            SetTodayFlag(bool bFlag);

	bool            GetMessageFlag(); //是否有IM消息
	void            SetMessageFlag(bool bFlag);


	void			SetLoginHWnd(HWND hWnd);
	void			SetFrameHwnd(HWND hWnd);

	void			AddAttendMerch(CSmartAttendMerch& attendMerch,E_AttendMerchType type );
	void			RemoveAttendMerch(E_AttendMerchType type);

	void			ForceUpdateVisibleAttentIoView();	//	强制更新已经监控的可见视图
	// 处理强弱度相关的几个指数
public:	
	
	// 处理左下角几个商品xx
	void			RequestStatusBarViewData();		// 请求statusbar需要请求的数据 几个指数数据等
	
	// 处理个股强弱度等xx
	bool32			GetMerchPowerValue(IN CMerch *pMerchStock, OUT float &fPowerValue);	// 根据商品判断个股的强弱度

	DWORD			GetMerchManagerType()			// 返回商品列表中包含商品类型标志，避免每次搜索
	{ 
		//ASSERT( m_MerchManager.IsInitializedAll() );
		return m_dwMerchManagerType; }	

public:
	typedef CArray<T_GeneralNormal, T_GeneralNormal&> GeneralNormalArray;
	const GeneralNormalArray *GetGeneralNormalArray(CMerch *pMerch);
	
	const GeneralNormalArray &GetGeneralNormalArrayH() { return m_aGeneralNormalH; }
	const GeneralNormalArray &GetGeneralNormalArrayS() { return m_aGeneralNormalS; }
	
	bool32			GetGeneralNormalHFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell);
	bool32			GetGeneralNormalSFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell);

	const T_GeneralFinance& GetGeneralFinanceH() { return m_stGeneralFinanceH; }
	const T_GeneralFinance& GetGeneralFinanceS() { return m_stGeneralFinanceS; }

	const MapFundHold	&GetFundHold() const  { return m_mapFundHold; }

private:
	bool32			CheckLogQuoteConnect(int32 iCommunicationId);
	void			OnFinishLogWaitQuoteConnect(bool32 bAllNonConnect);

	void			IsRespConfigComplete();

public:
	void			ReSetLogData();

	// 
public:
	// 添加数据收听者
	void			AddViewDataListner(CViewDataListner* pListener);
	void			DelViewDataListner(CViewDataListner* pListener);	

	void		    AddViewNewsListner(CViewNewsListner* pListener);
	void			DelViewNewsListner(CViewNewsListner* pListener);

public:
	//	离线登录
	bool32			OfflineLogin(IN CString& StrUserName, IN const CString& tServerIP , IN const CString& tKey, IN const CString& orgCode);

    // 脱机登录的初始化过程
    bool32			OffLineInitial(IN CString& StrUserName, OUT CString& StrErr);
	// 保存市场列表(脱机登录时用到)
	void			SaveMarketInfo();
	
	// 分离字符串取开收盘时间
	bool32			SplitMarketOCTimeString(IN const CString& StrOCTime, OUT CArray<T_OpenCloseTime, T_OpenCloseTime>& aOCTimes);
	
	// 分离字符串取最近交易日
	bool32			SplitMarketActiveDays(IN const CString& StrActiveDay, OUT CArray<uint32, uint32>& aActiveDays);

	//	市场推送，内存清理，服务时间，商品行情
	void			InitPush(bool32 bEnable);
	//	大市场数据是否来源于脱机市场
	bool32			IsBreedFromOfflineFile() { return m_bBreedFromOfflineFile;}
	// 读取市场列表(脱机登录时从配置文件中获取市场列表)
	bool32			InitialMarketInfoFromFile(CMerchManager& merchManager, CString& StrErr);
	//	读取市场列表(脱机登录时从配置文件中获取市场列表,不包含业务逻辑)
	bool32			GetOfflineFileData(CMerchManager& merchManager, CString& StrErr);
	// 保存用户权限信息(脱机登录时用到)
	bool32			SaveUserRightInfo();
	//	重新登录情况下，登录成功处理（取消了市场信息处理部分、关注市场请求部分）
	void			ReLoginSucc();
	//	是否是脱机登录
	bool32			IsOffLineLogin();
public:
	// 判断本地数据是否足够
	bool32			BeLocalOfflineDataEnough(E_KLineTypeBase eType);

public:
	
	void			SetRecordDataCenterAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch&>& aMerchs);

private:

	// 加入请求队列
	void			PutIntoMarketReqList(CMarket* pMarket);

	// 开始请求
	void			ReqMarketInfo();

public:
	bool32			BeAuthError() const { return m_bAuthError; }

	// 直连行情服务器相关
public:
	void			SetDirectQuote(bool32 bDirect) { m_bDirectToQuote = bDirect; }
	bool32			BeDirectQuote() { return m_bDirectToQuote; } 
	
	bool32			DirectConnectQuote();
	bool32			LoadMarketOnDirectQuote();

public:
	void			SetUserInfo(CString& strUserName,CString& strPwd,CString& strProduct);
	void			SetOffLineData(bool bOffLine);	
	void			InitialDataServers(CProxyInfo& proxyInfo,CString& strUserName,CString& strPwd);
	CString			GetUserName();
	CString			GetUserPwd();
	CString			GetProduct();

	bool			GetManualReConnectFlag();
	void			SetManualReConnectFlag(bool bFlag);

	void			PackQuoteAuthJson(CString& strReq);		// 行情认证请求封装为json串
	 //typedef CArray<CIoViewListner*, CIoViewListner*> IoViewList;
    const CArray<CIoViewListner*, CIoViewListner*> &GetIoViewList() {return m_IoViewList; }// 业务视图监听列表
	

public:
	int32			GetJinpingID() { return m_iJinpingID; }
    void            SetConfigRequstCount(int32 iCount) { m_iConfigCnt = iCount; }
	// 自选股同步
	void            UploadUserBlock();	
	void            DownloadUserBlock();
	void            UpdateUserBlock();

	//////////////////////////////////////////////////////////////////////////	离线登录api
	void			SetBridge(HWND hWnd);
	void			GetNetBreedInfo(CMerchManager& merchManager);
	
/////////////////////////////////////////////////////////////////// 
// 
public:
	E_LoginState	m_eLoginState;					// 登录状态
	bool32			m_bKickOut;						// 是否被踢了
	
	// 非常重要的数据都在这里了
public:	
	//lint -sem(CMerchManager::AddBreed, custodial(1))
	CMerchManager	m_MerchManager;					// 商品管理
	CRelativeMerchManager	m_RelativeMerchManager;	// 股票关系列表

	DWORD			m_dwMerchManagerType;

	
	float	m_fPriceClose;	// 这里保存前收价，给导出分笔数据到EXCEL的对话框使用
private:
	CGmtTime		m_TimeServerNow;				// 当前时间（格林威治时间）, 该时间参照服务器时间，因为各个客户端的时间有可能不准确， 对于初始化市场这样的依赖时间的操作来说， 一定要大家都能统一一个时间
	uint32			m_uiTimeLastUpdate;				// timeGetTime()
	
	listNewsInfo	m_listNewsTitle;				// 普通新闻
	listNewsInfo	m_listJinpingTitle;				// 金评
	
	T_NewsInfo		m_stNewsContentNow;	
	map<CString, CString> m_mapCodeIndex;
	bool32			m_bDelayQuote;			// 是否是延时行情

    int32           m_iConfigCnt;

public:
	CAbsOfflineDataManager &m_OfflineDataManager;
	
		
private:
	iServiceDispose	*m_pServiceDispose;
public:
	HWND			m_OwnerWnd;
	HWND			m_LoginWnd;
	HWND			m_MainFrameWnd;

	// 是否正在下载盘后数据, 下载盘后数据时不把历史K线数据存在内存
	bool32			m_bDownLoading;
public:
	CCommManager	    *m_pCommManager;
	CProxyAuthClientHelper* m_pAutherManager;
	CNewsManager	    *m_pNewsManager;
	CDataManager	    *m_pDataManager;

private:
	int32			m_iRecvedPackagerCount;													// 收到的包的数量

private:
	SmartAttendMerchArray		m_StatucBarAttendMerchs;	// 状态栏显示的几个商品的信息
	SmartAttendMerchArray		m_aAlarmAttendMerchs;		//预警关注商品
	SmartAttendMerchArray		m_aRecordDataCenterAttendMerchs;	// 记录数据库的商品的信息
	SmartAttendMerchArray		m_aIndexChsDataAttendMerchs;		// 选股的商品信息
	SmartAttendMerchArray		m_aArbitrageAttendMerchs;			// 预警的商品信息
	SmartAttendMerchArray		m_aSimulateTradeAttendMerchs;		// 模拟交易的商品信息

private:
	LockSingle											m_LockRecvDataForIndexChsStk;		// 回来的请求的锁
	LockSingle											m_LockNumsOfIndexChsStkReq;			// 请求个数的锁
public:
	CIndexChsStkMideCore*								m_pIndexChsStkMideCore;				// 指标选股
	int32												m_iNumsOfIndexChsStkReq;			// 指标选股的请求个数
	int32												m_iNumsOfIndexChsStkHaveRecved;		// 已经收到的请求个数
	CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>	m_aRecvDataForIndexChsStk;		// 回给指标选股的请求

private:
	bool32									m_bLogWaitQuoteConnect;							// 登录时等待行情服务器连接 (测速ping 需要服务器连接上, 发auth 也需要)
	int32									m_iWaitQuoteConnectCounts;					    // 等待的次数
	CArray<T_ServerInfo, T_ServerInfo&>		m_aQuoteServerAll;								// 所有的行情服务器
	bool32									m_bLogWaitQuoteSortFinish;						// 等待行情优选完成中

	//
	int32									m_iWaitDirectQuoteConnectCounts;				// 等待直连行情的次数

	// 实时推送队列
private:
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimePrices;						// 字符串格式：“市场-商品代码”， 后面是上次同步服务器和时间
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimeTicks;
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimeLevel2s;

						
	
private:
	//// 条件预警中心
	//CAlarmCenter*	m_pAlarmCenter;

private:
	
	CArray<CViewDataListner*, CViewDataListner*> m_aViewDataLinstner;	// 数据监听者
	CArray<CViewNewsListner*, CViewNewsListner*> m_aViewNewsLinstner;	// 资讯监听者
	CArray<CIoViewListner*, CIoViewListner*>	 m_IoViewList;			// 业务视图监听列表

private:
	CArray<T_GeneralNormal, T_GeneralNormal&>	 m_aGeneralNormalH;	// 沪市的大盘普通数据
	CArray<T_GeneralNormal, T_GeneralNormal&>	 m_aGeneralNormalS;	// 深市的大盘普通数据

	T_GeneralNormal		m_stGeneralNoramlHPre;	// 沪市的前一笔大盘普通数据	
	T_GeneralNormal		m_stGeneralNoramlSPre;  // 深市的前一笔大盘普通数据

	T_GeneralFinance	m_stGeneralFinanceH;	// 沪市的大盘财务数据
	T_GeneralFinance	m_stGeneralFinanceS;	// 深市的大盘财务数据

private:
	MapFundHold			m_mapFundHold;			// 商品的基金持仓信息
	uint32				m_uiFundHoldCrc32;		// 校验值	
	
private:
	bool32				m_bFastLogIn;			// 是否快速登录
	
private:
	struct CmpMarket
	{
		bool operator()(const CMarket* p1, const CMarket* p2) const
		{
			if ( NULL == p1 || NULL == p2 )
			{
				ASSERT(0);
				return false;
			}

			return (p1->m_MarketInfo.m_iMarketId - p2->m_MarketInfo.m_iMarketId < 0);
		}
	};

	set<CMarket*, CmpMarket>			m_aMarketToRequest;		// 待请求的市场号序列(登录时避免同时发大量的市场信息请求)
	set<CMarket*, CmpMarket>			m_aMarketRequesting;	// 正在请求的市场

private:
	bool32								m_bAuthError;			// 是否认证出现错误了

private:
	bool32								m_bDirectToQuote;		// 是否直连行情服务器


private:
	bool								m_bTodayFalg;			// 是否有今评
	int32								m_iJinpingID;			// 今评的ID

private:
	
	bool                                m_bMessageFlag;//是否有消息
	
	bool32								m_bServerDisConnect;	// 是否掉线了

private:
	CString								m_strUserName;
	CString								m_strPassword;
	CString								m_strProduct;
	bool								m_bOffLine;         //是否离线数据
	CProxyInfo							m_proxyInfo;
	bool32								m_bManualReconnect;	// 是否在手动重连服务器, 这个时候, 行情服务器连接上了不要发认证请求, 待测速完成再发

	int32								m_iRespConfigCnt;	// 请求配置返回的个数（一共4个配置请求, 全部返回才开始连接行情）

public:
	bool32								m_bBreedFromOfflineFile;		//	大市场数据来源于脱机市场文件
	bool32								m_bReadyBreed;			//	是否已经准备好了大市场数据（第一次从许可证服务器获取大市场）
	HWND								m_hBridgeWnd;					//	桥窗体，负责主界面未创建情况下的消息接收窗体	
	int32								m_iInitializedMarketCount;		//	未初始化市场总数
	int32								m_iAllMarketCount;				//	市场总数
	
};


#endif


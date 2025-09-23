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

// �������������
typedef struct T_IndexChsStkRespData
{
public:
	int32					m_iRespID;			// ������ID ��
	int32					m_iMarketId;		// �г����
	CString					m_StrMerchCode;		// ��Ʒ����
	E_KLineTypeBase			m_eKLineTypeBase;	// �����K������
	CArray<CKLine, CKLine>	m_aKLines;			// K ��

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
		ELSNotLogin = 0,	// δ��¼
		ELSAuthing,			// ��֤��
		ELSAuthSuccess,		// ��֤�ɹ�
		ELSAuthFail,		// ��֤ʧ��
		ELSDirectQuote,		// ֱ�����������
		ELSLogining,		// ��¼��
		ELSLoginSuccess,	// ��¼�ɹ�
		ELSLoginFail		// ��¼ʧ��
	};

	enum E_MerchManagerType		// merchmanager�г�ʼ������Ʒ�б�״̬
	{
		EMMT_None = 0,		// ���κ�״̬, ��ʱ�ʹ���������~
		EMMT_StockCn = 1,	// �����ڹ�Ʊ
		EMMT_FutureCn = 2,	// �������ڻ�
	};

	enum E_AttendMerchType	  //��ע��Ʒ����
	{
		EA_Arbitrage = 0,	  //����
		EA_StatucBar ,		   //״̬��
		EA_Alarm,				//Ԥ��
		EA_RecordData,			//���ݿ�
		EA_Choose,				//ѡ��
		EA_SimulateTrade,		// ģ�⽻����Ʒ
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
	// ҵ����ͼ�������
	void			AddViewListener(IN CIoViewListner *pIoView);
	void			RemoveViewListener(IN CIoViewListner *pIoView);
	

	
	// ��������
	void			RequestViewData(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID = -1,IN int eDataSource = EDSAuto);


	
	// ������Ѷ
	void			RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationID = -1);


	// ���ص�ǰ������ʱ�䣨��������ʱ�䣩
	CGmtTime		GetServerTime(); 

	// ����ĳ���г�����ʱ���ĵĵ�ǰʱ�䣨����ڷ������ĸ�������ʱ����ϸ��г���ʱ�
	CGmtTime		GetMarketCurrentTime(const CMarket &Market);

	void			OnAuthSuccessMain(bool bOfflineLogin);
	
	void			OnMsgCodeIndexUpdate(IN const CString& StrCode);
	
	//���ݴ����룬���ش����ַ���
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


	// DataManager�¼��ı��̴߳���ӿڣ� ��CDataManagerNotify����Ľӿ�һһ��Ӧ
public:
	void			OnMsgServiceConnected(int32 iServiceId);
	void			OnMsgServiceDisconnected(int32 iServiceId);
	void			OnMsgRequestTimeOut(int32 iMmiReqId);
	void			OnMsgCommResponse();
	void			OnMsgServerConnected(int32 iCommunicationId);
	void			OnMsgServerDisconnected(int32 iCommunicationId);
	void			OnMsgServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);

	// �����첽֪ͨ�¼�
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

	// ��Ȩ���ͱ仯
	void			OnWeightTypeChange();

	// ����ļ���ʱ
	void			OnBlockFileTranslateTimeout();

public:
	// datamanagerӦ������
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

	// ioview��������
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
	// NewsEngine �¼��ӿ�
public:
	virtual void	OnMsgNewsResponse();
	virtual void	OnMsgNewsServerConnected(int32 iCommunicationId);
	virtual void	OnMsgNewsServerDisconnected(int32 iCommunicationId);


	// ����Ӧ��
	void			OnNewsRespNewsList(int32 iMmiRespId, IN const CMmiNewsRespNewsList* pMmiNewsRespNewsList);
	void			OnNewsRespF10(int32 iMmiRespId, IN const CMmiNewsRespF10* pMmiNewsRespF10);
	void			OnNewsRespLandMine(int32 iMmiRespId, IN const CMmiNewsRespLandMine* pMmiNewsRespLandMine);
	void            OnNewsRespTitleList(int32 iMmiRespId, IN const CMmiNewsRespTitleList* pMmiNewsResp);//test2013-5-19
	void	        OnNewsRespNewsContent(int32 iMmiRespId, IN const CMmiNewsRespNewsContent* pMmiNewsResp);
	void			OnNewsRespCodeIndex(int32 iMmiRespId, IN const CMmiNewsRespCodeIndex* pMmiNewsRespCodeIndex);
	void			OnNewsPushNewsTitle(int32 iMmiRespId, IN const CMmiNewsPushNewsTitle* pMmiNewsPush);
	// ��������Ĵ���
public:  
	// ����

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

	// Ӧ��
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

// �����Ĳ����֤����������
	void			OnReqAuthPlugInHeart(IN const CMmiReqAuthPlugInHeart* pMimiComBase, IN int32 iCommunicationID);

	void			OnDataRespAuthPlugInHeart(int iMmiReqId, IN const CMmiRespAuthPlugInHeart* pMmiResp);

	void			OnMsgAuthPlugInHeartResp(int iRet);

public:
	// 
	void			FindRelativeMerchs(IN CMerch *pMerch, OUT CArray<CRelativeMerchNode, CRelativeMerchNode&> &AllRelativeMerchs);
	void			OnRelativeMerchsFileUpdate();
	void			OnHkWarrantsFileUpdate();

private:
	// ��ĳһ���г���ĳЩ��ʼ�����ݻ���
	void			OnSomeInitializeMarketDataRet(CMarket &Market);
	
	// ��ȫ�����г���ʼ���ɹ�
	void			OnAllMarketInitializeSuccess();	

	// ���ڵ�¼�����ʱ��, ����������(����ԭ����߳�ʱ��δ�յ������Զ�����)
	void			ReLogInOnServerDisconnected(int32 iCommunicationId);

public:
	// ��ʱ�����ڴ�
	void			OnTimerFreeMemory(bool32 bForceFree = false);

	// ��ʱͬ������
	void			OnTimerSyncPushing();

	// ��ʱ����г���ʼ�� 
	void			OnTimerInitializeMarket();

	// ��ʱ���·�����ʱ��
	void			OnTimerUpdateServerTime();

	// ��¼�����������
	void			OnTimerLogWaitQuoteConnect();

	// �ȴ�ֱ����������
	void			OnTimerWaitDirectQuoteConnect();

	// ��鴫���ļ���ʱ����
	void			OnTimerLogicBlockTraslateTimeOut();

	// ��֤���������
	void			OnTimerPlugInAuthHeart();

	// ֹͣ������������
	void			DisconnectAllQuoteServer();

	// ֹͣ������Ѷ����
	void			DisconnectAllNewsServer();

private:
	// ����Ƿ���ҵ����ͼ��עĳ����Ʒ��ĳ������ҵ��
	bool32			IsSomeIoViewAttendTheMerch(IN CMerch *pMerch, int32 iDataSeriveTypes);

public:
	void			OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	void			OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId);
	void			OnRealtimeLevel2(const CRealtimeLevel2 &RealtimeLevel2, int32 iCommunicationId);

private:
	void			ForceUpdateVisibleIoView(CIoViewListner *pIoView);

	// ��ݼ����
public:
	//void			RemoveHotKey(E_HotKeyType eHotKeyType);
	//void			AddHotKey(CHotKey &HotKey);
	void			BuildMerchHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);
	//void			BuildShortCutHotKeyList();
	//void			BuildUserBlockHotKeyList();
	
	// ����ѡ�����
public:
	void			SetIndexChsStkMideCore(CIndexChsStkMideCore* pMidCore);
	void			SetIndexChsStkReqNums(int32 iNumReqs);
	bool32			GetRespMerchKLineForIndexChsStk(OUT CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>& RecvDataForIndexChsStk);

	// ����Ԥ��
	//CAlarmCenter*	GetAlarmCenter() const { return m_pAlarmCenter; }

 	bool            GetTodayFlag(); //�Ƿ��н���
 	void            SetTodayFlag(bool bFlag);

	bool            GetMessageFlag(); //�Ƿ���IM��Ϣ
	void            SetMessageFlag(bool bFlag);


	void			SetLoginHWnd(HWND hWnd);
	void			SetFrameHwnd(HWND hWnd);

	void			AddAttendMerch(CSmartAttendMerch& attendMerch,E_AttendMerchType type );
	void			RemoveAttendMerch(E_AttendMerchType type);

	void			ForceUpdateVisibleAttentIoView();	//	ǿ�Ƹ����Ѿ���صĿɼ���ͼ
	// ����ǿ������صļ���ָ��
public:	
	
	// �������½Ǽ�����Ʒxx
	void			RequestStatusBarViewData();		// ����statusbar��Ҫ��������� ����ָ�����ݵ�
	
	// �������ǿ���ȵ�xx
	bool32			GetMerchPowerValue(IN CMerch *pMerchStock, OUT float &fPowerValue);	// ������Ʒ�жϸ��ɵ�ǿ����

	DWORD			GetMerchManagerType()			// ������Ʒ�б��а�����Ʒ���ͱ�־������ÿ������
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
	// �������������
	void			AddViewDataListner(CViewDataListner* pListener);
	void			DelViewDataListner(CViewDataListner* pListener);	

	void		    AddViewNewsListner(CViewNewsListner* pListener);
	void			DelViewNewsListner(CViewNewsListner* pListener);

public:
	//	���ߵ�¼
	bool32			OfflineLogin(IN CString& StrUserName, IN const CString& tServerIP , IN const CString& tKey, IN const CString& orgCode);

    // �ѻ���¼�ĳ�ʼ������
    bool32			OffLineInitial(IN CString& StrUserName, OUT CString& StrErr);
	// �����г��б�(�ѻ���¼ʱ�õ�)
	void			SaveMarketInfo();
	
	// �����ַ���ȡ������ʱ��
	bool32			SplitMarketOCTimeString(IN const CString& StrOCTime, OUT CArray<T_OpenCloseTime, T_OpenCloseTime>& aOCTimes);
	
	// �����ַ���ȡ���������
	bool32			SplitMarketActiveDays(IN const CString& StrActiveDay, OUT CArray<uint32, uint32>& aActiveDays);

	//	�г����ͣ��ڴ���������ʱ�䣬��Ʒ����
	void			InitPush(bool32 bEnable);
	//	���г������Ƿ���Դ���ѻ��г�
	bool32			IsBreedFromOfflineFile() { return m_bBreedFromOfflineFile;}
	// ��ȡ�г��б�(�ѻ���¼ʱ�������ļ��л�ȡ�г��б�)
	bool32			InitialMarketInfoFromFile(CMerchManager& merchManager, CString& StrErr);
	//	��ȡ�г��б�(�ѻ���¼ʱ�������ļ��л�ȡ�г��б�,������ҵ���߼�)
	bool32			GetOfflineFileData(CMerchManager& merchManager, CString& StrErr);
	// �����û�Ȩ����Ϣ(�ѻ���¼ʱ�õ�)
	bool32			SaveUserRightInfo();
	//	���µ�¼����£���¼�ɹ�����ȡ�����г���Ϣ�����֡���ע�г����󲿷֣�
	void			ReLoginSucc();
	//	�Ƿ����ѻ���¼
	bool32			IsOffLineLogin();
public:
	// �жϱ��������Ƿ��㹻
	bool32			BeLocalOfflineDataEnough(E_KLineTypeBase eType);

public:
	
	void			SetRecordDataCenterAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch&>& aMerchs);

private:

	// �����������
	void			PutIntoMarketReqList(CMarket* pMarket);

	// ��ʼ����
	void			ReqMarketInfo();

public:
	bool32			BeAuthError() const { return m_bAuthError; }

	// ֱ��������������
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

	void			PackQuoteAuthJson(CString& strReq);		// ������֤�����װΪjson��
	 //typedef CArray<CIoViewListner*, CIoViewListner*> IoViewList;
    const CArray<CIoViewListner*, CIoViewListner*> &GetIoViewList() {return m_IoViewList; }// ҵ����ͼ�����б�
	

public:
	int32			GetJinpingID() { return m_iJinpingID; }
    void            SetConfigRequstCount(int32 iCount) { m_iConfigCnt = iCount; }
	// ��ѡ��ͬ��
	void            UploadUserBlock();	
	void            DownloadUserBlock();
	void            UpdateUserBlock();

	//////////////////////////////////////////////////////////////////////////	���ߵ�¼api
	void			SetBridge(HWND hWnd);
	void			GetNetBreedInfo(CMerchManager& merchManager);
	
/////////////////////////////////////////////////////////////////// 
// 
public:
	E_LoginState	m_eLoginState;					// ��¼״̬
	bool32			m_bKickOut;						// �Ƿ�����
	
	// �ǳ���Ҫ�����ݶ���������
public:	
	//lint -sem(CMerchManager::AddBreed, custodial(1))
	CMerchManager	m_MerchManager;					// ��Ʒ����
	CRelativeMerchManager	m_RelativeMerchManager;	// ��Ʊ��ϵ�б�

	DWORD			m_dwMerchManagerType;

	
	float	m_fPriceClose;	// ���ﱣ��ǰ�ռۣ��������ֱ����ݵ�EXCEL�ĶԻ���ʹ��
private:
	CGmtTime		m_TimeServerNow;				// ��ǰʱ�䣨��������ʱ�䣩, ��ʱ����շ�����ʱ�䣬��Ϊ�����ͻ��˵�ʱ���п��ܲ�׼ȷ�� ���ڳ�ʼ���г�����������ʱ��Ĳ�����˵�� һ��Ҫ��Ҷ���ͳһһ��ʱ��
	uint32			m_uiTimeLastUpdate;				// timeGetTime()
	
	listNewsInfo	m_listNewsTitle;				// ��ͨ����
	listNewsInfo	m_listJinpingTitle;				// ����
	
	T_NewsInfo		m_stNewsContentNow;	
	map<CString, CString> m_mapCodeIndex;
	bool32			m_bDelayQuote;			// �Ƿ�����ʱ����

    int32           m_iConfigCnt;

public:
	CAbsOfflineDataManager &m_OfflineDataManager;
	
		
private:
	iServiceDispose	*m_pServiceDispose;
public:
	HWND			m_OwnerWnd;
	HWND			m_LoginWnd;
	HWND			m_MainFrameWnd;

	// �Ƿ����������̺�����, �����̺�����ʱ������ʷK�����ݴ����ڴ�
	bool32			m_bDownLoading;
public:
	CCommManager	    *m_pCommManager;
	CProxyAuthClientHelper* m_pAutherManager;
	CNewsManager	    *m_pNewsManager;
	CDataManager	    *m_pDataManager;

private:
	int32			m_iRecvedPackagerCount;													// �յ��İ�������

private:
	SmartAttendMerchArray		m_StatucBarAttendMerchs;	// ״̬����ʾ�ļ�����Ʒ����Ϣ
	SmartAttendMerchArray		m_aAlarmAttendMerchs;		//Ԥ����ע��Ʒ
	SmartAttendMerchArray		m_aRecordDataCenterAttendMerchs;	// ��¼���ݿ����Ʒ����Ϣ
	SmartAttendMerchArray		m_aIndexChsDataAttendMerchs;		// ѡ�ɵ���Ʒ��Ϣ
	SmartAttendMerchArray		m_aArbitrageAttendMerchs;			// Ԥ������Ʒ��Ϣ
	SmartAttendMerchArray		m_aSimulateTradeAttendMerchs;		// ģ�⽻�׵���Ʒ��Ϣ

private:
	LockSingle											m_LockRecvDataForIndexChsStk;		// �������������
	LockSingle											m_LockNumsOfIndexChsStkReq;			// �����������
public:
	CIndexChsStkMideCore*								m_pIndexChsStkMideCore;				// ָ��ѡ��
	int32												m_iNumsOfIndexChsStkReq;			// ָ��ѡ�ɵ��������
	int32												m_iNumsOfIndexChsStkHaveRecved;		// �Ѿ��յ����������
	CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>	m_aRecvDataForIndexChsStk;		// �ظ�ָ��ѡ�ɵ�����

private:
	bool32									m_bLogWaitQuoteConnect;							// ��¼ʱ�ȴ�������������� (����ping ��Ҫ������������, ��auth Ҳ��Ҫ)
	int32									m_iWaitQuoteConnectCounts;					    // �ȴ��Ĵ���
	CArray<T_ServerInfo, T_ServerInfo&>		m_aQuoteServerAll;								// ���е����������
	bool32									m_bLogWaitQuoteSortFinish;						// �ȴ�������ѡ�����

	//
	int32									m_iWaitDirectQuoteConnectCounts;				// �ȴ�ֱ������Ĵ���

	// ʵʱ���Ͷ���
private:
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimePrices;						// �ַ�����ʽ�����г�-��Ʒ���롱�� �������ϴ�ͬ����������ʱ��
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimeTicks;
	std::map<CMerch*, T_PushServerAndTime>	m_aPushingRealtimeLevel2s;

						
	
private:
	//// ����Ԥ������
	//CAlarmCenter*	m_pAlarmCenter;

private:
	
	CArray<CViewDataListner*, CViewDataListner*> m_aViewDataLinstner;	// ���ݼ�����
	CArray<CViewNewsListner*, CViewNewsListner*> m_aViewNewsLinstner;	// ��Ѷ������
	CArray<CIoViewListner*, CIoViewListner*>	 m_IoViewList;			// ҵ����ͼ�����б�

private:
	CArray<T_GeneralNormal, T_GeneralNormal&>	 m_aGeneralNormalH;	// ���еĴ�����ͨ����
	CArray<T_GeneralNormal, T_GeneralNormal&>	 m_aGeneralNormalS;	// ���еĴ�����ͨ����

	T_GeneralNormal		m_stGeneralNoramlHPre;	// ���е�ǰһ�ʴ�����ͨ����	
	T_GeneralNormal		m_stGeneralNoramlSPre;  // ���е�ǰһ�ʴ�����ͨ����

	T_GeneralFinance	m_stGeneralFinanceH;	// ���еĴ��̲�������
	T_GeneralFinance	m_stGeneralFinanceS;	// ���еĴ��̲�������

private:
	MapFundHold			m_mapFundHold;			// ��Ʒ�Ļ���ֲ���Ϣ
	uint32				m_uiFundHoldCrc32;		// У��ֵ	
	
private:
	bool32				m_bFastLogIn;			// �Ƿ���ٵ�¼
	
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

	set<CMarket*, CmpMarket>			m_aMarketToRequest;		// ��������г�������(��¼ʱ����ͬʱ���������г���Ϣ����)
	set<CMarket*, CmpMarket>			m_aMarketRequesting;	// ����������г�

private:
	bool32								m_bAuthError;			// �Ƿ���֤���ִ�����

private:
	bool32								m_bDirectToQuote;		// �Ƿ�ֱ�����������


private:
	bool								m_bTodayFalg;			// �Ƿ��н���
	int32								m_iJinpingID;			// ������ID

private:
	
	bool                                m_bMessageFlag;//�Ƿ�����Ϣ
	
	bool32								m_bServerDisConnect;	// �Ƿ������

private:
	CString								m_strUserName;
	CString								m_strPassword;
	CString								m_strProduct;
	bool								m_bOffLine;         //�Ƿ���������
	CProxyInfo							m_proxyInfo;
	bool32								m_bManualReconnect;	// �Ƿ����ֶ�����������, ���ʱ��, ����������������˲�Ҫ����֤����, ����������ٷ�

	int32								m_iRespConfigCnt;	// �������÷��صĸ�����һ��4����������, ȫ�����زſ�ʼ�������飩

public:
	bool32								m_bBreedFromOfflineFile;		//	���г�������Դ���ѻ��г��ļ�
	bool32								m_bReadyBreed;			//	�Ƿ��Ѿ�׼�����˴��г����ݣ���һ�δ����֤��������ȡ���г���
	HWND								m_hBridgeWnd;					//	�Ŵ��壬����������δ��������µ���Ϣ���մ���	
	int32								m_iInitializedMarketCount;		//	δ��ʼ���г�����
	int32								m_iAllMarketCount;				//	�г�����
	
};


#endif


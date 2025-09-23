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
	//dll����Բ�ͬ����ķ�����
	T_ErrCode2StrInfo(-1, L"catch��׽����"),
	T_ErrCode2StrInfo(-2, L"��ʼ��curl�Ựʧ��"),
	T_ErrCode2StrInfo(-3, L"����������Э�鲻�Գ�"),
	T_ErrCode2StrInfo(-4, L"�������ݽ�������"),
	//���´�����Ϊ����˷���
	T_ErrCode2StrInfo(1001, L"δ֪�쳣"),
	T_ErrCode2StrInfo(1004, L"��ݲ��Ϸ��������µ�¼!"),
	T_ErrCode2StrInfo(1011, L"���ĸ�ʽ����"),
	T_ErrCode2StrInfo(1012, L"����Session����ȷ!"),
	T_ErrCode2StrInfo(1013, L"���Ķ�ȡ����!"	),
	T_ErrCode2StrInfo(1014, L"�������������˲�һ��!"),
	T_ErrCode2StrInfo(1015, L"��֤������������ѹ���!"),
	T_ErrCode2StrInfo(1016, L"��ݲ���ȷ���޷���¼!"),
	T_ErrCode2StrInfo(1017, L"��¼ʱ�䲻��ȷ��"	),
	T_ErrCode2StrInfo(1018, L"�����ĸ�ʽ����ȷ��"),
	T_ErrCode2StrInfo(1020, L"���ڸ�ʽ����!"	),
	T_ErrCode2StrInfo(1042, L"��¼Ʊ�ݲ���ȷ��"),
	T_ErrCode2StrInfo(1044, L"Э��汾��ƥ�䣡"),
	T_ErrCode2StrInfo(1045, L"δ֪�����ģ�"),
	T_ErrCode2StrInfo(10001, L"��������ʱ��û�в���,���������û�״̬��ʧЧ,�����µ�¼��"	),
	T_ErrCode2StrInfo(10016, L"���״��벻���ڣ�"	),
	T_ErrCode2StrInfo(10017, L"��¼������������"	),
	T_ErrCode2StrInfo(10022, L"û�д��û�,�����µ�¼��"),
	T_ErrCode2StrInfo(99999, L"δ֪����")  //�����жϽ�β
		
};

extern const TCHAR KStrLoginLogFileName[];		// ��¼ʱ�䳤Trace�ļ���

// ��ʱ�����ڴ涨ʱ��
const int32 KTimerIdFreeMemory				= 1;
const int32 KTimerPeriodFreeMemory			= 3 * 60 * 1000;	// ms unit	// ÿ��һ��ʱ������һ���ڴ�

const int32 KTimerIdSyncPushing				= 2;
const int32 KTimerPeriodSyncPushing			= 1000 * 20;	    // ms unit	// ÿ��һ��ͬ��һ������

// ��ʱ����г���ʼ����ʱ��
const int32 KTimerIdInitializeMarket		= 3;
const int32 KTimerPeriodInitializeMarket	= 30 * 1000;	    // ms unit

// ��ʱ���·�����ʱ��
const int32 KTimerIdUpdateServerTime		= 4;
const int32 KTimerPeriodUpdateServerTime	= 50;			    // ms unit

// ��¼�ȴ��������������
const int32 KTimerIdWaitQuoteConnet			= 5;
const int32 KTimerPeriodWaitQuoteConnet		= 500;			    // ms unit

// ֱ����ʱ��ȴ��������������
const int32 KTimerIdWaitDirectQuoteConnet	= 6;

// �� 10 ����������. 
const int32 KiCountsWaitQuoteConnet			= 10 * 1000 / KTimerPeriodWaitQuoteConnet;

// �ȴ� 20 ���Ӱ���ļ�����
const int32 KTimerIdWaitLogicBlockFileTraslateTime = 6;
const int32 KTimerPeriodLogicBlockFileTraslateTime = 20 * 1000;

// һ��������г�����
static const int32 s_KiMarketRequestSegment	= 1;

// һ��Ҫ������ٸ�������Ϣ
//static const int32 s_KiConfigCnt = 4;

//
int32			m_iTimeInitializePerDay;// ÿ���ʼ��ʱ�䡣��ʽ���Է���Ϊ��λ�� �����00:00�ֵ�ֵ
// �ͻ���ÿ�춼��Ҫ��ϵͳ����Ʒ�б�����ʼ���� 
// ��ʱ������ÿ�γ�ʼ����ʱ�䣨��������ʱ�䣩�� ���ڹ��ڹ��У� ��ʱ�伴�����Ͼ��ۡ�ʱ��
// 
CArray<T_OpenCloseTime, T_OpenCloseTime>	m_OpenCloseTimes;	// ÿ�켸������ʱ��Σ���������ʱ�䣩

// ������Ľ����պ�ǰ�Ľ�����(���������,Ȼ��������10��������)
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
// ÿ����Ϣ�����Ӧ�����
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

// Ϊͬ��ͬ������
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

// ��¼�ɹ��ı�����Ϣ

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

//	char   m_StrQuoterSvrAdd[100];		// �����������ַ
	int32  m_iQuoterSvrPort;		// ����������˿�
	CString m_StrQuoterSvrAdd;

	//
	int32  m_uiUserID;

	// ֧�ֵ����ݷ���ID ��
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

	// ��DataManagerע���Լ�
	m_pDataManager->AddDataManagerNotify(this);

	// �� COptimizeServer ע���Լ�
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
	// ��DataManagerע���Լ�
	m_pDataManager->RemoveDataManagerNotify(this);

	// �� COptimizeServer ע���Լ�
	COptimizeServer::Instance()->RemoveNotify(this);

	// ɾ����ѡ��Դ
	COptimizeServer::DelInstance();
	//
	m_IoViewList.RemoveAll();

//	DEL(m_pAlarmCenter);

	m_Timer.stopTimer();
}

bool32 CViewData::Construct()
{
	// ��ʼ��ƴ��ģ��
//	InitPy();

	// ������ݼ��б�
//	BuildShortCutHotKeyList();

	// ��ʱ��
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

	// �ҵ����е�, �޳����������ظ��������
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
	case ECTReqAuth:  // ������֤
		{
			OnViewReqAuth((const CMmiReqAuth *)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqMarketInfo:	// �����г���Ϣ
		{
			OnViewReqMarketInfo((const CMmiReqMarketInfo *)pMmiCommBase);
		}
		break;
	case ECTReqMarketSnapshot:	// �����г�������Ϣ
		{
			OnViewReqMarketSnapshot((const CMmiReqMarketSnapshot *)pMmiCommBase);
		}
		break;
	case ECTReqBroker:	// ���󾭼�ϯλ��Ϣ
		{
			OnViewReqBroker((const CMmiReqBroker *)pMmiCommBase);
		}
		break;
	case ECTReqMerchInfo:	// ������Ʒ��Ϣ
		{
			OnViewReqMerchInfo((const CMmiReqMerchInfo *)pMmiCommBase);
		}
		break;
	case ECTReqPublicFile:	// ����������ļ�
		{
			OnViewReqPublicFile((const CMmiReqPublicFile *)pMmiCommBase);
		}
		break;

	case ECTReqMerchExtendData:	// ������Ʒ��չ����
		{
			OnViewReqMerchExtendData((const CMmiReqMerchExtendData *)pMmiCommBase);
		}
		break;
	case ECTReqMerchF10:	// ������Ʒ��չ����
		{
			OnViewReqMerchF10((const CMmiReqMerchF10 *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimePrice:	// ������Ʒʵʱ5������
		{
			OnViewReqRealtimePrice((const CMmiReqRealtimePrice *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimeTick:	// ������ƷTick
		{
			OnViewReqRealtimeTick((const CMmiReqRealtimeTick *)pMmiCommBase);
		}
		break;
	case ECTReqRealtimeLevel2:	// ������Ʒʵʱlevel2����
		{
			OnViewReqRealtimeLevel2((const CMmiReqRealtimeLevel2 *)pMmiCommBase);
		}
		break;
	case ECTReqMerchKLine:	// ������Ʒ��ʷK������
		{
			OnViewReqMerchKLine((const CMmiReqMerchKLine *)pMmiCommBase);
		}
		break;
	case ECTReqMerchTimeSales:	// ������Ʒ�ֱʳɽ���ʷ
		{
			OnViewReqMerchTimeSales((const CMmiReqMerchTimeSales *)pMmiCommBase);
		}
		break;
	case ECTReqReport:	// ���󱨼۱�
		{
			OnViewReqReport((const CMmiReqReport *)pMmiCommBase);
		}
		break;
	case ECTReqBlockReport: // ��鱨�۱���Ʒ������������
		{			
			OnViewReqBlockReport((const CMmiReqBlockReport *)pMmiCommBase);
		}
		break;
	case ECTReqTradeTime:	// ��Ʒ�����⽻��ʱ��
		{
			OnReqClientTradeTime((const CMmiReqTradeTime*)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTRegisterPushPrice:	// ע�������̱���ʵʱ����
		{
			OnViewRegisterPushPrice((const CMmiRegisterPushPrice *)pMmiCommBase);
		}
		break;
	case ECTRegisterPushTick:	// ע��ֱ�ʵʱ����
		{
			OnViewRegisterPushTick((const CMmiRegisterPushTick *)pMmiCommBase);
		}
		break;
	case ECTRegisterPushLevel2:	// ע��Level2ʵʱ����
		{
			OnViewRegisterPushLevel2((const CMmiRegisterPushLevel2 *)pMmiCommBase);
		}
		break;
	case ECTCancelAttendMerch:	// ����ȡ����Ʒʵʱ��������
		{
			// zhangbo 20090626 #����RegisterRealtime*һ�麯��������ȡ�����͹��ܣ� һ������²���Ҫʵ��Cancel���ܣ� ʵ�ֵ�Ŀ����Ϊ������޶ȵļ��ٷ�����ѹ��
//			OnViewCancelAttendMerch((const CMmiCancelAttendMerch *)pMmiCommBase);
		}
		break;

	case ECTReqNetTest:
		{
			OnViewReqNetTest((const CMmiReqNetTest *)pMmiCommBase, iCommunicationID);
		}
		break;
	case ECTReqGeneralNormal: // ������̵���ͨ����
		{
			OnViewReqGeneralNormal((const CMmiReqGeneralNormal*)pMmiCommBase, iCommunicationID);	
		}
		break;
	case ECTReqGeneralFinance: // ������̲�������(�����ֵ��ͼ������)
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

	// xl 0523 �Ƹ���ܰ汾 ��ֹ��������ķ���
	switch(pMmiCommBasePlugIn->m_eCommTypePlugIn)
	{
	case ECTPIReqMerchIndex:
		{
			// ��������ѡ�ɱ�־
			OnViewReqMerchIndex((const CMmiReqMerchIndex*)pMmiCommBase);
		}
		break;
	case ECTPIReqBlock:
		{			
			// ����б�����
			OnViewReqBlock((const CMmiReqLogicBlock*)pMmiCommBase);
		}
		break;
	case  ECTPIReqMerchSort:
		{
			// ����������
			OnViewReqMerchSort((const CMmiReqMerchSort*)pMmiCommBase);
		}
		break;
	case  ECTPIReqPeriodMerchSort:
		{
			// ��������������
			OnViewReqPeriodMerchSort((const CMmiReqPeriodMerchSort*)pMmiCommBase);
		}
		break;
	case  ECTPIReqBlockSort:
		{	
			// ����б�����
			OnViewReqBlockSort((const CMmiReqBlockSort*)pMmiCommBase);
		}	
		break;
	case  ECTPIReqMainMonitor:
		{
			// �������
			OnViewReqMainMonitor((const CMmiReqMainMonitor*)pMmiCommBase);
		}
		break;
	case  ECTPIReqShortMonitor:
		{
			// ���߼��
			OnViewReqShortMonitor((const CMmiReqShortMonitor*)pMmiCommBase);
		}
		break;
	case ECTPIReqTickEx:
		{
			// �ʽ�����
			OnViewReqTickEx((const CMmiReqTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqPeriodTickEx:
		{
			// �������ڷֱ�����
			OnViewReqPeriodTickEx((const CMmiReqPeriodTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqHistoryTickEx:
		{			
			// ��ʷ�ʽ���������
			OnViewReqHistoryTickEx((const CMmiReqHistoryTickEx*)pMmiCommBase);
		}
		break;
	case ECTPIReqChooseStock:
		{
			// ����ս��ѡ������
			OnViewReqChooseStock((const CMmiReqChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqCRTEStategyChooseStock:
		{
			// ����ս��ѡ����չ����
			OnViewReqCRTEStategyChooseStock((const CMmiReqCRTEStategyChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqDapanState:
		{
			// ����״̬����
			OnViewReqDapanState((const CMmiReqDapanState*)pMmiCommBase);
		}
		break;
	case ECTPIReqChooseStockStatus:
		{
			// ѡ��״̬����
			OnViewReqChooseStockStatus((const CMmiReqChooseStockStatus*)pMmiCommBase);
		}
		break;
	case ECTPIReqDKMoney:
		{
			// ����ʽ�����
			OnViewReqDKMoney((const CMmiReqDKMoney*)pMmiCommBase);
		}
		break;
	case ECTPIReqMainMasukura:
		{
			// ��������
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
	case  ECTPIReqUpdatePushMerchIndex: // ��������
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
			// ѡ������
			OnViewReqUpdatePushPlugInChooseStock((const CMmiReqUpdatePushPlugInChooseStock*)pMmiCommBase);
		}
		break;
	case ECTPIReqDeletePushMerchData:
		{
			// ��������
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
			// ����ѡ��
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
	// ���̱߳䵥�̴߳���
	if (KTimerIdFreeMemory == iTimerId)
	{
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerFreeMemory, 0, 0);		// �����ڴ�
	}
	if (KTimerIdSyncPushing == iTimerId)
	{
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerSyncPushing, 0, 0);		// ͬ������
	}
	else if (KTimerIdInitializeMarket == iTimerId)
	{	
		PostMessage(m_OwnerWnd, UM_ViewData_OnTimerInitializeMarket, 0, 0);	// ��ʼ���г�
	}
	else if (KTimerIdUpdateServerTime == iTimerId)							
	{
		OnTimerUpdateServerTime();											// ���·�����ʱ��
	}
	else if (KTimerIdWaitQuoteConnet == iTimerId)
	{
		OnTimerLogWaitQuoteConnect();										// �ȴ���������
	}	
	else if ( KTimerIdWaitLogicBlockFileTraslateTime == iTimerId )			// ����ļ����䳬ʱ
	{
		OnTimerLogicBlockTraslateTimeOut();
	}
	else if (KTimerIdWaitDirectQuoteConnet == iTimerId)						// �ȴ�ֱ���������������
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
	// ֪ͨ��¼�Ի��� ��ǰ״̬
	SendMessage(m_LoginWnd,UM_ViewData_OnAuthSuccess, m_iInitializedMarketCount, m_iAllMarketCount);

	// ����˽��Ŀ¼
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
	// ��ѡ��վ
	if ( NULL == m_pServiceDispose )
	{
		ASSERT(0);
		return;
	}

	// �õ�����������б�
	T_ServerInfo* pServerInfo;
	int iCountServers;
	m_pServiceDispose->GetServerList(&pServerInfo, iCountServers);

	CArray<T_ServerInfo, T_ServerInfo&> aNewsServerAll;

	// ����ֵ
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

	_MYTRACE(L"��֤���������سɹ�, ׼����������, �����������������:");

	// �����������
	COptimizeServer::Instance()->SetSortParams((T_ServerInfo*)m_aQuoteServerAll.GetData(), m_aQuoteServerAll.GetSize());

	// ��������������������
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

	// ���������������, �����Ժ�������
	//pDoc->m_pDlgLogIn->SetPromptInfo(L"�������������...");
	StartTimer(KTimerIdWaitQuoteConnet, KTimerPeriodWaitQuoteConnet);

	// �õ���Ѷ�������б�:
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
		// ��¼������, ���ܳ��ִ���, ��յ�¼��Ϣ. ���ʱ��, ��Ҫ����
		_MYTRACE(L"�յ���ѡ���֪ͨ, ��ʱ��δ��¼״̬, �������ڷ������������, ���������֪ͨ.");
		return;
	}

	if ( COptimizeServer::Instance()->GetForcePingFlag() )
	{
		// ����ping ���ٵ�
		return;
	}

	m_bManualReconnect = false;	
	//
	/*if ( NULL != pDoc->m_pDlgLogIn )
	{
		pDoc->m_pDlgLogIn->SetPromptInfo(L"��ѡ��վ���!"); 
	}*/
	_MYTRACE(L"��ѡ���, ������˳������:");

	// ������� 1: ���������˳�� 2: ����֤����
	arrServer aServerSorted;
	COptimizeServer::Instance()->GetSortedServers(aServerSorted);

	for ( int32 ii = 0 ; ii < aServerSorted.GetSize() ; ii++ )
	{
		CServerState ss = aServerSorted[ii];			
		_MYTRACE(L" => [%d] ID: %d ��ַ: %s �˿�: %d", ss.m_iSortIndex, m_pCommManager->GetCommunicationID(ss.m_StrServerAddr, ss.m_uiServerPort), ss.m_StrServerAddr.GetBuffer(), ss.m_uiServerPort);			
		ss.m_StrServerAddr.ReleaseBuffer();
	}

	m_pCommManager->ReSetCommunicationIndex(aServerSorted);
	
	bool32 bAllError = true;
	bool32 bAutoSort = COptimizeServer::Instance()->BeAutoSortMode();

	for ( int32 i = 0; i < aServerSorted.GetSize(); i++ )
	{
		// ��һ̨�������������֤����

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
					_MYTRACE(L"��һ̨�����������쳣, ȡ��һ̨��������ʼ��֤");
					continue;				
				}
				else
				{
					// ��ѡ��������������֤����ķ���
					break;
				}
			}
			
			//
			T_UserInfo stUserInfo;
			if ( NULL != m_pServiceDispose )
			{
				bAllError = false;

				m_pServiceDispose->GetUserInfo(stUserInfo);
				
				// ������֤����:
				CMmiReqAuth ReqAuth;
				PackQuoteAuthJson(ReqAuth.m_StrReqJson);
				
				RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);	

				_MYTRACE(L"�������, ȡ��һ̨������������֤����: UserID = %d Token = %s Product = %s UserName = %s",
					stUserInfo.iUserId, stUserInfo.wszToken, m_strProduct.GetBuffer(), stUserInfo.wszUserName);
				m_strProduct.ReleaseBuffer();
			}
			else
			{
				ASSERT(0);
				continue;
			}

			// ��¼�µ�¼��Ϣ, �´������ϵ�ʱ����
			CArray<int32, int32> aServiceIDs;
			m_pCommManager->GetServerServiceList(stServerState.m_StrServerAddr, stServerState.m_uiServerPort, aServiceIDs);

			T_LoginBackUp stBackUp;
			stBackUp.m_StrQuoterSvrAdd = stServerState.m_StrServerAddr;
			stBackUp.m_iQuoterSvrPort = stServerState.m_uiServerPort;
			
			stBackUp.m_uiUserID		= stUserInfo.iUserId;
			stBackUp.m_iServiceCount= aServiceIDs.GetSize();

			stBackUp.m_iSize = sizeof(stBackUp) - sizeof(stBackUp.m_pServices) + stBackUp.m_iServiceCount * sizeof(int32);

			// д�ļ���¼
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

	// ����ж�ģʽ�Ļ�����������ѡ���Ӳ��ϣ������Ĳ����ж�
	// ���ж�ģʽ������ܲ������ӵ��û�ָ���ķ�����
	if ( bAllError && bAutoSort )
	{
		// �Զ���ѡ���Խ���ʣ�µ����ӵ��������������֤����
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
					_MYTRACE(L"���Բ������ӵ����������: %d,%s", iCommunicationID, stServerState.m_StrServerAddr.GetBuffer());
					stServerState.m_StrServerAddr.ReleaseBuffer();
					continue;
				}
				
				//
				T_UserInfo stUserInfo;
				if ( NULL != m_pServiceDispose )
				{
					bAllError = false;
					
					m_pServiceDispose->GetUserInfo(stUserInfo);
					
					// ������֤����:
					CMmiReqAuth ReqAuth;
					PackQuoteAuthJson(ReqAuth.m_StrReqJson);
					
					RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);	
					
					_MYTRACE(L"�������, ȡ������(%d)������֤����: UserID = %d Token = %s Product = %s UserName = %s",
						iCommunicationID, stUserInfo.iUserId, stUserInfo.wszToken, m_strProduct.GetBuffer(), stUserInfo.wszUserName);
					m_strProduct.ReleaseBuffer();
					
					/*if ( NULL != pDoc->m_pDlgLogIn )
					{
						pDoc->m_pDlgLogIn->SetPromptInfo(L"����������վ��ʼ��֤..."); 					
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
			_MYTRACE(L"û��һ̨��������������, �޷�������֤����, ��ʾ�Ժ��¼");
			pStrTip = new CString(L"û��һ̨��������������, ����״���쳣, ���Ժ��¼...");
		}
		else
		{
			_MYTRACE(L"��ѡ���������Ӳ�����, �޷�������֤����, ��ʾ������ѡ");
			pStrTip = new CString(L"��ѡ���������Ӵ���, ��ѡ������������!");
		}
		PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);
	}
	else
	{
		m_bLogWaitQuoteSortFinish = false;	// ���������ѡ��������
	}

}

void CViewData::OnFinishAllPing()
{
	if ( COptimizeServer::Instance()->GetForcePingFlag() )
	{
		// ����ping ���ٵ�
		return;
	}

	// ping ���: ͣ����������Ҫ�����������
	m_pCommManager->StopServiceAfterSortServer();	
}

bool32 CViewData::IsSomeIoViewAttendTheMerch(IN CMerch *pMerch, int32 iDataSeriveTypes)
{
	// �ȿ�����Ʒ�Ƿ�Ϊ��������ʾ��Ʒ
	int i;
	for ( i = 0; i < m_StatucBarAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &AttendMerch = m_StatucBarAttendMerchs[i];
		if (AttendMerch.m_pMerch == pMerch && (AttendMerch.m_iDataServiceTypes & iDataSeriveTypes) != 0)
		{
			return TRUE;
		}
	}
	
	// �ǲ�������ѡ�ɵ���Ʒ����
	for ( i = 0; i < m_aIndexChsDataAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch &AttendMerch = m_aIndexChsDataAttendMerchs[i];
		if ( AttendMerch.m_pMerch == pMerch )
		{
			return TRUE;
		}
	}
	
	// �ǲ���Ҫ�������ݿ������
	for ( i = 0; i < m_aRecordDataCenterAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch &AttendMerch = m_aRecordDataCenterAttendMerchs[i];
		if (AttendMerch.m_pMerch == pMerch )
		{
			return TRUE;
		}
	}

	// �ټ�����ҵ����ͼ�Ƿ��й��ĵ�
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

// ��ʱ�����ڴ�
// ������Ʒɨ�裬 ���Ƿ������������ͼ
void CViewData::OnTimerFreeMemory(bool32 bForceFree)
{
	//
	CArray<CPushMerchKey, CPushMerchKey&> aCancelMerchs;

	// 
	
	// ���е���Ʒ�����K�ߺ�Tick�߲���Ҫ����ʾ�ģ� ��������ڴ�ռ�
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
				
				// ����Ƿ���K������
				bool32 bHasHistroyData = pMerch->DoesHaveHistoryData();
				if (bHasHistroyData)	// ����Ƿ����ĸ���ͼ���ĸ���Ʒ����ʷ����
				{					
					if ( bForceFree || !IsSomeIoViewAttendTheMerch(pMerch, EDSTKLine | EDSTTimeSale))
					{
						// û���κ���ͼ��ʾ�Լ������Ʒ����ʷ�����й�ϵ�� ɾ������Ʒ����ʷ����
						/*
						TRACE(L"��ʱ������ %d %s ���ڴ�\n", 
							pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCnName);

						int32 iByteKLine = 0;
						for (int32 i = 0; i < pMerch->m_MerchKLineNodesPtr.GetSize(); i++)
						{
							int32 iByte = pMerch->m_MerchKLineNodesPtr[i]->m_KLines.GetSize() * sizeof(CKLine);
							TRACE(L"  %d K �� %d �ֽ�\n", pMerch->m_MerchKLineNodesPtr[i]->m_eKLineTypeBase, iByte);

							iByteKLine += iByte;
						}

						int32 iByteTick = 0;
						if ( NULL != pMerch->m_pMerchTimeSales) 
						{
							iBythneTick = pMerch->m_pMerchTimeSales->m_Ticks.GetSize() * sizeof(CTick);
							TRACE(L"  Tick %d �ֽ�\n", iByteTick);
						}
						
						int32 iTotal = iByteKLine + iByteTick;
						float fK = iTotal / 1024.0f;
						float fM = fK / 1024.0f;
						TRACE(L"�������ڴ�: %d + %d = %d �ֽ�(%.2f K %.2f M)\n", iByteKLine, iByteTick, iTotal, fK, fM);
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

	// �ȿ�����Ʒ�Ƿ�Ϊ��������ʾ��Ʒ
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

	// ״̬������������� 0, 1000
	{
		CMmiReqAddPushGeneralNormal reqGPush;
		reqGPush.m_iMarketId = 0;
		RequestViewData(&reqGPush);
		reqGPush.m_iMarketId = 1000;
		RequestViewData(&reqGPush);
	}

	// �Ƿ���Ԥ����Ʒ && ����Ԥ��
	for ( int32 i = 0; i < m_aAlarmAttendMerchs.GetSize(); i++ )
	{

		CMerch* pMerch = m_aAlarmAttendMerchs[i].m_pMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
		aPushingRealtimePrices.Add(pMerch);
	}


	// ���ݿ�
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
	

	// ������Ʒ - ת�����������۱�
	for ( int32 i = 0; i < m_aArbitrageAttendMerchs.GetSize(); i++ )
	{

		CMerch* pMerch = m_aArbitrageAttendMerchs[i].m_pMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
		aPushingRealtimePrices.Add(pMerch);
	}
	
	
	// �ټ�����ҵ����ͼ�Ƿ��й��ĵ�
	CIoViewListner *pIoView = NULL;
	for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
	{
		pIoView = m_IoViewList[iIndexIoView];
		if (NULL == pIoView) continue;

		const DWORD dwNeedPushType = pIoView->GetNeedPushDataType();
		
		if ( (dwNeedPushType&(EDSTPrice |EDSTTick |EDSTLevel2)) == 0 )
		{
			continue;	// ����Ҫ��3�����͵��κ�һ������
		}

		if ( !pIoView->IsNowCanReqData() )
		{
			continue;	// ��ʱ����ͼ��������������
		}
		
		SmartAttendMerchArray pArray;
		pIoView->GetSmartAttendArray(pArray);
		for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < pArray.GetSize(); iIndexAttendMerch++)
		{
			CSmartAttendMerch &SmartAttendMerch = pArray.GetAt(iIndexAttendMerch);
			if ((SmartAttendMerch.m_iDataServiceTypes & EDSTPrice) != 0
				&& (dwNeedPushType & EDSTPrice) != 0 )				// xl 11/01/20  �����Ƿ���Ҫ�����͵��ж�
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

	// ģ�⽻����Ʒע������
	for (int i = 0; i < m_aSimulateTradeAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &AttendMerch = m_aSimulateTradeAttendMerchs[i];
		aPushingRealtimePrices.Add(AttendMerch.m_pMerch);
	}
	
	// �������ͬ������������
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
		
		// �������������
		RequestViewData((CMmiCommBase *)&Req);
	}

	// �������ͬ�����ͷֱ�
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
		
		// �������������
		RequestViewData((CMmiCommBase *)&Req);
	}

	// �������ͬ������Level2
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
		
		// �������������
		RequestViewData((CMmiCommBase *)&Req);
	}
}

void CViewData::OnTimerInitializeMarket()
{
	if (!m_MerchManager.IsInitializedAll())	// ��δ����һ�������ĳ�ʼ����ʱ�� ����Ҫ����
		return;


	// ���е��г���ѯ�� �Ƿ���Ҫ��ʼ��
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
				// ��ʼ�����г�
				PutIntoMarketReqList(pMarket);
				// InitializeMarket(*pMarket);
			}
		}
	}

	// ���³�ʼ��
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
		// ����60 ��
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
		// ����60 ��, ��ʾʧ��
		StopTimer(KTimerIdWaitDirectQuoteConnet);	
		m_iWaitDirectQuoteConnectCounts = 0;
		
		//
		// ������һ���Լ��ͷţ��ں���������429����
		//lint --e{593} 
		CString *pStrTip = new CString(L"ֱ�������������ʱ,��������,�Ժ��¼...");
		if (NULL != m_LoginWnd)
		{
			PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);			
		}		
	}
}

void CViewData::OnDataServiceConnected(int32 iServiceId)
{
	// ���̱߳䵥�̴߳���
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServiceConnected, (WPARAM)iServiceId, 0);
}

void CViewData::OnMsgServiceConnected(int32 iServiceId)
{
	if (m_bDirectToQuote)
	{
		// �������е���δ��ʼ�����г��� ����÷����������ṩ���г��ĳ�ʼ������
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
							// ��û���κγ�ʼ���� �۸�����ʼ��һ���� :)
							// InitializeMarket(*pMarket);
							PutIntoMarketReqList(pMarket);
						}
						else
						{
							// �Ѿ��������ķ������ڳ�ʼ���ˣ� ���ﲻ��Ҫ����
							// NULL;
						}
					}
					else
					{
						// �Ѿ���ʼ���ɹ��ˣ� ����Ͳ��ٴ�����
						// NULL;
					}
				}				
			}
		}
		
		//
		ReqMarketInfo();
		
		return;
	}
	
	// ��������
	if ( m_bLogWaitQuoteSortFinish )
	{
		// ���ڵȴ���ѡ���ʱ������Ҫ����service
		_MYTRACE(L"�ȴ���ѡʱ, ����%d ����, ����", iServiceId);
		return;
	}

	// �������е���δ��ʼ�����г��� ����÷����������ṩ���г��ĳ�ʼ������
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
					// ��û���κγ�ʼ���� �۸�����ʼ��һ���� :)
					// InitializeMarket(*pMarket);
					PutIntoMarketReqList(pMarket);
				}
				else
				{
					// �Ѿ��������ķ������ڳ�ʼ���ˣ� ���ﲻ��Ҫ����
					// NULL;
				}
			}
			else
			{
				// �Ѿ���ʼ���ɹ��ˣ� ����Ͳ��ٴ�����
				// NULL;
			}
		}
	}
	
	//
	ReqMarketInfo();

	// ֻҪ�κ�һ����ͼ���κ�һ����ע����Ʒ��÷������й����� �����������ͼ�е�����
	CIoViewListner *pIoView = NULL;
	for (int32 iIndexIoView = 0; iIndexIoView < m_IoViewList.GetSize(); iIndexIoView++)
	{
		pIoView = m_IoViewList[iIndexIoView];
		if (NULL == pIoView)
			continue;

		// ���Ҹ���ͼ�Ƿ���ʲôҵ����÷������й�
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
				// ����Ʒ��Ҫ��������÷������й���
				bAttendView = true;
				break;
			}
		}

	
		if ( !bAttendView )
		{
			// �����µĽӿڲ鿴�Ƿ��й��ĸ��г�����
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
		
		// ǿ�Ƹ��¸�ҳ������
		if (bAttendView)
		{
			pIoView->ForceUpdateVisibleIoView();
		}
	}

	
	// ��״̬����Ҫ��ʾ�ļ�����Ʒ�Ƿ���Ӱ��
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

	// ����ע������
	if (m_bServerDisConnect)
	{
		OnTimerSyncPushing();
		m_bServerDisConnect = false;
	}
}

void CViewData::OnDataServiceDisconnected(int32 iServiceId)
{
	// ���̱߳䵥�̴߳���
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServiceDisconnected, (WPARAM)iServiceId, 0);
}

void CViewData::OnMsgServiceDisconnected(int32 iServiceId)
{	
	if (!m_MerchManager.IsInitializedAll())	// �г���δ��ʼ��ʱ�� ���������κν���
		return;
}

void CViewData::OnDataServerConnected(int32 iCommunicationId)
{
	// ���̱߳䵥�̴߳���
	_MYTRACE(L"ViewData �յ��ײ������������֪ͨ: OnDataServerConnected: %d, m_OwnerWnd = %d, ����Ϣ����������", iCommunicationId, m_OwnerWnd);
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
		
		// ������֤����:
		CMmiReqAuth ReqAuth;
		PackQuoteAuthJson(ReqAuth.m_StrReqJson);
		
		RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);		
		
		//************************************* by hx
		/*if (NULL != pDoc->m_pDlgLogIn && NULL != pDoc->m_pDlgLogIn->GetSafeHwnd())
		{
			CString StrPrompt;
			StrPrompt.Format(L"���ӷ������ɹ�, ��ʼ�����г���Ϣ...");
			pDoc->m_pDlgLogIn->SetPromptInfo(StrPrompt);		
			pDoc->m_pDlgLogIn->SetPromptInfo2(L"ֱ������������ɹ�!");		
		}*/
		//*************************************

		m_eLoginState = ELSLogining;
	}
	else
	{	
		if ( ELSAuthFail >= m_eLoginState )
		{
			// ����֤���Ϊ����Ҫ�����֪ͨ
			ASSERT( 0 );
			return;
		}
		

		//
		if ( m_bLogWaitQuoteConnect || m_bLogWaitQuoteSortFinish )
		{
			// ��¼ʱ
			if ( m_bLogWaitQuoteConnect )
			{
				// �ȴ�����ʱ
				_MYTRACE(L"ViewData ��ʽ��������� %d ���ӳɹ���Ϣ!", iCommunicationId);
				CheckLogQuoteConnect(iCommunicationId);
			}
			
			// �ȴ���ѡ��ɹ����е�����������
		}	
		else 
		{
			if ( m_bManualReconnect )
			{
				// �ֶ�����������ʱ��, ��Ҫ�ط���֤:
				// NULL;
			}
			else
			{
				// �ǵ�¼ʱ, ���̷���֤����
				T_UserInfo stUserInfo;
				m_pServiceDispose->GetUserInfo(stUserInfo);
				
				// ������֤����:
				CMmiReqAuth ReqAuth;
				PackQuoteAuthJson(ReqAuth.m_StrReqJson);
				
				RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationId);
				
				CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aActiveServer;
				m_pCommManager->GetCommnunicaionList(aActiveServer);
				
				//
				_MYTRACE(L"������ %d ������, �ط���֤����: UserID = %d Token = %s Product = %s UserName = %s",
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

				// ����ע������
				OnTimerSyncPushing();
				return;
			}
		}
	}
	
	// ����������, Ϊ�˱�֤������ȫƥ��, ������ػ����vipdata Ŀ¼
	{
		OnTimerFreeMemory(true);		
		_tremove_dir(_T("vipdata"));
	}
}

void CViewData::OnDataServerDisconnected(int32 iCommunicationId)
{
	// ���̱߳䵥�̴߳���
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServerDisconnected, (WPARAM)iCommunicationId, 0);
}

void CViewData::OnMsgServerDisconnected(int32 iCommunicationId)
{
	CString StrLog;
	StrLog.Format(_T("������ %d �Ͽ�����"), iCommunicationId);
	
	
	
	
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
				StrLog.Format(_T("������ %s(%s:%d) �Ͽ�����"), sState.m_StrServerName.GetBuffer()
					, sState.m_StrServerAddr.GetBuffer(), sState.m_uiServerPort);
			}
		}
	}

	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(StrLog, TRUE);
	if ( m_bLogWaitQuoteSortFinish )
	{
		// �ȴ���ѡ��ɵĹ����У�֪ͨ��ѡ�÷�����������
		COptimizeServer::Instance()->OnDataServerDisconnected(iCommunicationId);
	}

	if ( ELSLogining == m_eLoginState )
	{		
		ReLogInOnServerDisconnected(iCommunicationId);
		return;
	}

	//
	if (!m_MerchManager.IsInitializedAll())	// �г���δ��ʼ��ʱ�� ���������κν���
		return;

	// �����������, �����ϵ�ʱ��, Ҫ����ͬ��һ������
	m_bServerDisConnect = true;

	// �鿴�Ƿ������г���Ϣ�ķ�������������
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

	// ��鿴��Щʵʱ���������ܵ�Ӱ��
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
	_MYTRACE(L"��¼������, ������%d ����", iCommunicationId);
	
	


	if ( ELSLogining != m_eLoginState )
	{
		ASSERT(0);
		return;
	}

	// ���ڵ�¼��ʱ��, �ṩ�г���Ϣ�ķ���������
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
		_MYTRACE(L"�������������г���Ϣ�ķ�����. ���ù�");
		return;
	}

	if ( m_bLogWaitQuoteSortFinish )
	{
		_MYTRACE(L"��ѡ��������ѡ���ӷ���������, �ȴ���������");
		return;
	}

	//
	_MYTRACE(L"��¼������, ������%d ����, ��ʾ�����쳣, ���µ�¼1", iCommunicationId);
	
	// �ĳ����ٵ�¼�ķ�ʽ
	m_bFastLogIn = false;

	//
	ReSetLogData();
	
	// ��¼�������ķ�����
	COptimizeServer::Instance()->SaveErrServer(sState);
	
	//
	CString *pStrTip = new CString(L"����״���쳣, ���Ժ��¼...");
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
			// ��½ǰ�� - ֱ������
			_MYTRACE(L"+++V [%d] CViewData ����ͨѶ��(��¼ǰ): %d �ϳ�ʱ, ����\n", eType, iCommunicationId);
			
			// �������
			m_pDataManager->RemoveHistoryReqOfSpecifyServer(iCommunicationId);
			
			// ����һ��
			m_pCommManager->StartService(iCommunicationId);
			
			// 
			
			// ����ǵ�¼��ʱ��, �ط�����
			if ( ELSLogining == m_eLoginState )
			{
				ReLogInOnServerDisconnected(iCommunicationId);
			}
		}		
	}
	else
	{
		// ��¼���
		if ( m_bManualReconnect )
		{
			// �ֶ�������ʱ��, �Ͳ�Ҫ�Զ�������
			return;
		}
		
		if ( eType == ERCTPackTimeOut )
		{

			PostMessage(m_MainFrameWnd,UM_Package_TimeOut,0,0);
		}
		else
		{
			PostMessage(m_MainFrameWnd,UM_Package_LongTime,0,0);

			_MYTRACE(L"+++V [%d] CViewData ����ͨѶ��: %d �ϳ�ʱ, ����\n", eType, iCommunicationId);
			
			// �������
			m_pDataManager->RemoveHistoryReqOfSpecifyServer(iCommunicationId);
			
			// ����һ��
			m_pCommManager->StartService(iCommunicationId);
					
			
			// ����ǵ�¼��ʱ��, �ط�����
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
	// �̴߳���
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataServerLongTimeNoRecvData, (WPARAM)iCommunicationId, (LPARAM)eType);
}

void CViewData::OnDataRequestTimeOut(int32 iMmiReqId)
{
	// ���̱߳䵥�̴߳���
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
		*pStrTrace = L"�յ�dataengine֪ͨ�� ���µİ�����!";
		::PostMessage(g_hwndTrace, 0x456, (WPARAM)timeGetTime(), (LPARAM)pStrTrace);
	}
#endif

	// ���̱߳䵥�̴߳���
	PostMessage(m_OwnerWnd, UM_ViewData_OnDataCommResponse, 0, 0);
}

void CViewData::OnMsgCommResponse()
{	
	
	bool32 bPackageCountUpdate = false;

	int32 iProcessPackageCount = 1/*KProcessPackageCountPerMsg*/; // zhangbo 1030 #bug�� �ô����������࣬ ���µ�¼�ɹ�����Ϣ�����������K��Ӧ����Ϣ����
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
			case ECTRespAuth:		// Ӧ����֤��Ϣ
				{
					OnDataRespAuth(iMmiRespId, (const CMmiRespAuth *)pMmiCommResp);
				}
				break;
			case ECTRespMarketInfo:	// Ӧ���г���Ϣ
				{
					OnDataRespMarketInfo(iMmiRespId, (const CMmiRespMarketInfo *)pMmiCommResp);
				}
				break;			
			case ECTRespMarketSnapshot:	// Ӧ���г�������Ϣ
				{
					OnDataRespMarketSnapshot(iMmiRespId, (const CMmiRespMarketSnapshot *)pMmiCommResp);
				}
				break;
			case ECTRespBroker:	// Ӧ�𾭼�ϯλ��Ϣ
				{
					OnDataRespBroker(iMmiRespId, (const CMmiRespBroker *)pMmiCommResp);
				}
				break;
			case ECTRespMerchInfo:	// Ӧ����Ʒ��Ϣ
				{
					OnDataRespMerchInfo(iMmiRespId, (const CMmiRespMerchInfo *)pMmiCommResp);
				}
				break;
			case ECTRespPublicFile:	// Ӧ��������ļ�
				{
					OnDataRespPublicFile(iMmiRespId, (const CMmiRespPublicFile *)pMmiCommResp);
				}
				break;
			case ECTRespLogicBlockFile: // Ӧ���߼�����ļ�
				{
					OnDataRespLogicBlockFile(iMmiRespId, (const CMmiRespLogicBlockFile*)pMmiCommResp);
				}
				break;
			case ECTRespMerchExtendData:	// Ӧ����Ʒ��չ����
				{
					OnDataRespMerchExtendData(iMmiRespId, (const CMmiRespMerchExtendData *)pMmiCommResp);
				}
				break;
			case ECTRespMerchF10:	// Ӧ����Ʒ��չ����
				{
					OnDataRespMerchF10(iMmiRespId, (const CMmiRespMerchF10 *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimePrice:	// Ӧ����Ʒʵʱ5������
				{
					OnDataRespRealtimePrice(iMmiRespId, iCommunicationId, (const CMmiRespRealtimePrice *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimeLevel2:	// Ӧ����Ʒʵʱlevel2����
				{
					OnDataRespRealtimeLevel2(iMmiRespId, iCommunicationId, (const CMmiRespRealtimeLevel2 *)pMmiCommResp);
				}
				break;
			case ECTRespRealtimeTick:	// Ӧ��Tick
				{
					// �����İ�ֻ��һ����־, û��ʵ������(����ע������)
					// NULL;
				}
				break;
			case ECTRespMerchKLine:	// Ӧ����Ʒ��ʷK������
				{
					OnDataRespMerchKLine(iMmiRespId, (CMmiRespMerchKLine *)pMmiCommResp);
				}
				break;
			case ECTRespMerchTimeSales:	// Ӧ����Ʒ�ֱʳɽ���ʷ
				{
					OnDataRespMerchTimeSales(iMmiRespId, (const CMmiRespMerchTimeSales *)pMmiCommResp);
				}
				break;
			case ECTRespReport:	// Ӧ�𱨼۱�
				{
					OnDataRespReport(iMmiRespId, iCommunicationId, (const CMmiRespReport *)pMmiCommResp);
				}
				break;
			case ECTRespBlockReport: // Ӧ��������Ʒ��������
				{
					// _T("Ӧ���鱨����Ʒ����...\r\n");
					OnDataRespBlockReport(iMmiRespId, iCommunicationId, (const CMmiRespBlockReport*)pMmiCommResp);
				}
				break;
			case ECTRespTradeTime:
				{
					OnDataRespClientTradeTime(iMmiRespId, (const CMmiRespTradeTime*)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushPrice:	// ע�����������̱���
				{
					OnDataAnsRegisterPushPrice(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushPrice *)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushTick:	// ע�����ͷֱ�
				{
					OnDataAnsRegisterPushTick(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushTick *)pMmiCommResp);
				}
				break;
			case ECTAnsRegisterPushLevel2:	// ע������Level2
				{
					OnDataAnsRegisterPushLevel2(iMmiRespId, iCommunicationId, (const CMmiAnsRegisterPushLevel2 *)pMmiCommResp);
				}
				break;
			case ECTRespCancelAttendMerch:	// Ӧ��ȡ����Ʒʵʱ��������
				{
					OnDataRespCancelAttendMerch(iMmiRespId, iCommunicationId, (const CMmiRespCancelAttendMerch *)pMmiCommResp);
				}
				break;
			case ECTRespError:		// ����������
				{
					OnDataRespError(iMmiRespId, (const CMmiRespError *)pMmiCommResp);
				}
				break;

			// ����������
			case ECTPushPrice:		// ���������̱���
				{
					OnDataPushPrice(iMmiRespId, iCommunicationId, (const CMmiPushPrice *)pMmiCommResp);
				}
				break;
			case ECTPushTick:		// ���ͷֱ�
				{
					OnDataPushTick(iMmiRespId, iCommunicationId, (const CMmiPushTick *)pMmiCommResp);
				}
				break;
			case ECTPushLevel2:		// ����Level2
				{
					OnDataPushLevel2(iMmiRespId, iCommunicationId, (const CMmiPushLevel2 *)pMmiCommResp);
				}
				break;
			case ECTUpdateSymbol:	// ������Ʒ�б�
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

			// ��������������һ��Ҫ�ǵ��ͷŸö��� �ö�����datamanager���룬 Ϊ���첽�������Ҫ�� �����Լ��ͷ�!!!
			DEL(pMmiRespNode);
			
			// �յ��İ���������
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
	

	// ��������δ��ʼ��
	Market.SetInitialized(false); 

	// ��ո��г���������Ʒ��ʵʱ����
	int32 i;
	for ( i = 0; i < Market.m_MerchsPtr.GetSize(); i++)
	{
		CMerch *pMerch = Market.m_MerchsPtr[i];
		if (NULL == pMerch || NULL == pMerch->m_pRealtimePrice)
			continue;
		
		// 		
		pMerch->m_pRealtimePrice->InitMerchPrice();
		
		// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
		// PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);	

		// �����״̬������Ʒ����
		for (int32 iIndexStatusMerch = 0; iIndexStatusMerch < m_StatucBarAttendMerchs.GetSize(); iIndexStatusMerch++)
		{
			if (pMerch == m_StatucBarAttendMerchs[iIndexStatusMerch].m_pMerch)
			{				
				PostMessage(m_MainFrameWnd,UM_MainFrame_OnStatusBarDataUpdate, 0, 0);
			}
		}
	
		// ����:
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

	// ��������µ�����
	if (0 != Market.m_MarketSnapshotInfo.m_iRiseCount || 0 != Market.m_MarketSnapshotInfo.m_iFallCount)
	{
		Market.m_MarketSnapshotInfo.m_iRiseCount = 0;
		Market.m_MarketSnapshotInfo.m_iFallCount = 0;

		// ֪ͨ����ҵ����ͼ
		for (i = 0; i < m_IoViewList.GetSize(); i++)
		{
			CIoViewListner *pIoView = m_IoViewList[i];
			pIoView->OnVDataMarketSnapshotUpdate(Market.m_MarketSnapshotInfo.m_iMarketId);
		}
	}

	/////////////////////////////////////////////////////////////
	// ����ָ���ȡ���г�������Ϣ
	CMmiReqMarketInfo MmiReqMarketInfo;
	MmiReqMarketInfo.m_iMarketId = Market.m_MarketInfo.m_iMarketId;
	RequestViewData((CMmiCommBase *)&MmiReqMarketInfo);
	
	// ����ָ���ȡ���г���������Ʒ�б�
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

	// ʲô������
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
	
	// ����г���ֵ, �Ҳ�����Ӧ���г��ģ� ��������
	CMarketInfo *pMarketInfo = NULL;
	for (int32 iIndexMarket = 0; iIndexMarket < pMmiRespMarketInfo->m_MarketInfoListPtr.GetSize(); iIndexMarket++)
	{
		pMarketInfo = pMmiRespMarketInfo->m_MarketInfoListPtr[iIndexMarket];
		if (NULL == pMarketInfo)
			continue;
	
		CMarket *pMarketFound = NULL;
		if (m_MerchManager.FindMarket(pMarketInfo->m_iMarketId, pMarketFound))
		{
			// TRACE(L"�г���:%d \r\n", pMarketInfo->m_iMarketId);

			// �ҵ��� ��ֵ
			pMarketFound->m_MarketInfo = *pMarketInfo;

			// ���ø��г��Ѿ��ɹ���ȡ�г���Ϣ
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
		
	// ����г���ֵ, �Ҳ�����Ӧ���г��ģ� ��������
	CMarketSnapshotInfo *pMarketSnapshot = NULL;
	for (int32 iIndexMarket = 0; iIndexMarket < pMmiRespMarketSnapshot->m_MarketSnapshotListPtr.GetSize(); iIndexMarket++)
	{
		pMarketSnapshot = pMmiRespMarketSnapshot->m_MarketSnapshotListPtr[iIndexMarket];
		if (NULL == pMarketSnapshot)
			continue;
	
		CMarket *pMarketFound = NULL;
		if (m_MerchManager.FindMarket(pMarketSnapshot->m_iMarketId, pMarketFound))
		{
			if (pMarketFound->m_MarketSnapshotInfo == *pMarketSnapshot)	// ��ͬ
				continue;

			// �ҵ��� ��ֵ
			pMarketFound->m_MarketSnapshotInfo = *pMarketSnapshot;

			// ֪ͨ����ҵ����ͼ
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
		
			// �������е���Ʒ��Ϣ,  ���������
			// 1) �еı���
			// 2) ԭ��������û����ɾ����־
			// 3) �¼ӵ�����
			int32 iPosMerchFound = 0;
			CMerch *pMerchFound = NULL;
			if (pMarket->FindMerch(pMerchInfoSrc->m_StrMerchCode, iPosMerchFound, pMerchFound))
			{
				// ԭ�����У������þ���ϯλ
				pMerchFound->m_MerchInfo = *pMerchInfoSrc;
				pMerchFound->m_MerchInfo.m_bRemoved = false;
			}
			else
			{
				// �Ҳ����� ��׷��һ��
				pMarket->AddMerch(*pMerchInfoSrc);
			}
		}	
		
		// ���ø��г��Ѿ��ɹ���ȡ�г���Ϣ
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
	if (0 == pMmiRespPublicFile->m_uiCRC32)	// ��ʾ������û�и�����
	{
		;	// ��ʱ�Ȳ�����
	}
	else
	{
		// ������Σ����յ����ݸ��²������ݸ���ʱ��
		if ( EPFTF10 == pMmiRespPublicFile->m_ePublicFileType )
		{
			pMerch->m_TimeLastUpdateFinanceData = GetServerTime();
		}

		if (pMmiRespPublicFile->m_iValidDataLen <= 0)	// ��ʾ���������Ѿ������µģ� ������£� ����������
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
				if (EPFTWeight == pMmiRespPublicFile->m_ePublicFileType)	// ����ǳ�Ȩ���ݣ� ���������
				{
					if (CMerch::ReadWeightDatasFromBuffer(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen, pMerch->m_aWeightDatas))
					{
						// �����������
						pMerch->m_TimeLastUpdateWeightDatas = GetServerTime();
						pMerch->m_uiWeightDataCRC = CCodeFile::crc_32(pMmiRespPublicFile->m_pcBuffer, pMmiRespPublicFile->m_iValidDataLen);
						
						// ���Ա��浽�ļ���
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
						
						// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
						PostMessage(m_OwnerWnd, UM_ViewData_OnPublicFileUpdate, (WPARAM)pMerch, EPFTWeight);
					}
				}
				else if ( EPFTF10 == pMmiRespPublicFile->m_ePublicFileType )	
				{
					//  F10 �Ĳ�������
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
						// ֪ͨ����
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
		return;		// ���ǵȴ�״̬�����ܸð�
	}

	// ������ڵ�¼�� ֪ͨ��¼��״̬�仯
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString(L"���հ����Ϣ�ɹ�����ʼ����...");
		SendMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);
	}

	bool32 bParseBlock = CBlockConfig::RespLogicBlockFile(pMmiRespLogicBlockFile, true);
	if ( !bParseBlock )
	{
		if (NULL != m_LoginWnd)
		{
			CString StrBlockFileName;
			CBlockConfig::GetConfigFullPathName(StrBlockFileName);
			bParseBlock = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // ��ʼ������ļ�
			if ( !bParseBlock )
			{
				CString *pStrTip = new CString(L"�����������ʧ�ܣ���");
				SendMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);
			}
		}
	}



	// ������ڵ�¼�� ֪ͨ��¼��״̬�仯
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString(L"��ʾ��ͬ������б���Ϣ����");
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
	
	// �ҵ���Ӧ�Ĵ��г��� ��������ľ���ϯλ
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
		
			// �������еľ���ϯλ��Ϣ,  ���������
			// 1) �еı���
			// 2) ԭ��������û����ɾ����־
			// 3) �¼ӵ�����
			int32 iPosBroker = 0;
			CBroker *pBroker = NULL;
			if (pBreed->FindBroker(pCommBroker->m_StrBrokerCode, iPosBroker, pBroker))
			{
				// ԭ�����У������þ���ϯλ
				pBroker->m_bRemoved = false;
			}
			else
			{
				CMmiBroker BrokerNew;
				BrokerNew = *pCommBroker;
				
				// �Ҳ����� ��׷��һ��
				pBreed->AddBroker(BrokerNew);
			}
		}

		// zhangbo 0507 #����bug�� �˴���δ���� ��γ�ʼ������
// 		// ���ø��г��Ѿ��ɹ���ȡ�г���Ϣ
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

		// ֪ͨ������ͼ��չ���ݱ仯
		// bug, ��ʱ��δȷ����չ���ݸ�ʽ
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

		// ֪ͨ����Ӧ����ͼˢ��
		// bug, ��ʱ��δȷ��F10���ݸ�ʽ
	}
}

void CViewData::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{ 
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch))
		return;
	// �����ã� ���������Ƿ�����쳣����
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimePrices.find(pMerch);
	if (itFind == m_aPushingRealtimePrices.end())	// �Ҳ����� ��ʾ
	{
		CString StrTrace;
		StrTrace.Format(L"\n������ ���棺�Ѿ�Cancel������Ʒ��������Price���ݹ�����, %d & %s\n", RealtimePrice.m_iMarketId, CString(RealtimePrice.m_StrMerchCode).GetBuffer());
		TRACE(StrTrace);
	}
#endif

	//RGUARD(LockSingle, pMerch->m_LockRealtimePrice, LockRealtimePrice);
	{
		////////////////////////////////////////////////////
		// ֮ǰ��δ���յ�������, ����
		if (NULL == pMerch->m_pRealtimePrice)
		{
			pMerch->m_pRealtimePrice = new CRealtimePrice();
		}

		if (RealtimePrice.m_TimeCurrent < pMerch->m_pRealtimePrice->m_TimeCurrent)	// ��ֵʱ��̫�磬 ����
			return;
			
		// 
		*pMerch->m_pRealtimePrice = RealtimePrice;
		pMerch->m_pRealtimePrice->ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);
		
// 		CString StrTrace;
// 		StrTrace.Format(L"====> %d & %s %g %g\n", RealtimePrice.m_iMarketId, RealtimePrice.m_StrMerchCode, RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevAvg);
// 		TRACE(StrTrace);

		// �����״̬������Ʒ����
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
 
		// ������������K��
		bool32 bKLineProcessSuccess = false;
		for (int32 iIndexKLineNode = 0; iIndexKLineNode < pMerch->m_MerchKLineNodesPtr.GetSize(); iIndexKLineNode++)
		{
			CMerchKLineNode *pMerchKLineNode = pMerch->m_MerchKLineNodesPtr[iIndexKLineNode];
			if (NULL == pMerchKLineNode)
				continue;

			if (pMerchKLineNode->ProcessRealtimePrice(pMerch, *pMerch->m_pRealtimePrice))	// ����K�߳ɹ�
			{
				bKLineProcessSuccess = true;
			}
		}

		if (bKLineProcessSuccess)
		{		
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
			PostMessage(m_OwnerWnd, UM_ViewData_OnKLineNodeUpdate, (WPARAM)pMerch, 0);	
		}

		// ֪ͨ���й���ʵʱ����price��ҵ����ͼ
		{
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);	
		}

		// ...fangz 0104 ״̬��, ����Ԥ����ЩҲҪ��, ��ʱ����, û�ո�
		for (int i = 0; i < m_aViewDataLinstner.GetSize(); i++ )
		{
			CViewDataListner* pListener = m_aViewDataLinstner[i];
			if ( NULL != pListener && pListener->IsAttendCommType(ECTReqRealtimePrice) )
			{
				pListener->OnRealtimePrice(RealtimePrice, iCommunicationId, iReqId);
			}
		}

		//TRACE(L"��������: %d %s %.2f %.2f %.2f\n", RealtimePrice.m_iMarketId, RealtimePrice.m_StrMerchCode, RealtimePrice.m_fPriceNew, RealtimePrice.m_fVolumeTotal, RealtimePrice.m_fAmountTotal);
 	}
}

void CViewData::OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId)
{
	if (MmiPushTick.m_Ticks.GetSize() <= 0)
		return;

	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(MmiPushTick.m_StrMerchCode, MmiPushTick.m_iMarketId, pMerch))
		return;

	// �����ã� ���������Ƿ�����쳣����
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeTicks.find(pMerch);
	if (itFind == m_aPushingRealtimeTicks.end())	// �Ҳ����� ��ʾ
	{
		CString StrTrace;
		StrTrace.Format(L"\n������ ���棺�Ѿ�Cancel������Ʒ��������Tick���ݹ�����, %d & %s\n", MmiPushTick.m_iMarketId, CString(MmiPushTick.m_StrMerchCode).GetBuffer());
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

			// ֮ǰ��δ���յ�������, ����
			if (NULL == pMerch->m_pRealtimeTick)
			{
				pMerch->m_pRealtimeTick = new CTick();
			}
		
			if (Tick.m_TimeCurrent < pMerch->m_pRealtimeTick->m_TimeCurrent)	// ��ֵʱ��̫�磬 ����
				continue;
			
			// 
			*pMerch->m_pRealtimeTick = Tick;	
			pMerch->m_pRealtimeTick->ReviseVolume(pMerch->m_Market.m_MarketInfo.m_iVolScale);

			bUpdateRealtimeTick = true;
		}

		// ֪ͨ���й���ʵʱ����tick��ҵ����ͼ
		if (bUpdateRealtimeTick)
		{
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeTickUpdate, (WPARAM)pMerch, 0);	
		}
	}

	// ����timesale
	if ( NULL == pMerch->m_pMerchTimeSales )
	{
		// û�ҵ��ͽ�һ��
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
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
			PostMessage(m_OwnerWnd, UM_ViewData_OnTimesaleUpdate, (WPARAM)pMerch, 0);
		}	
	}
	
	// ...fangz 0104 �����ѡ��ҲҪ��ô����, ��ʱ����, û�ո�
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

	// �����ã� ���������Ƿ�����쳣����
#ifdef _DEBUG
	std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeLevel2s.find(pMerch);
	if (itFind == m_aPushingRealtimeLevel2s.end())	// �Ҳ����� ��ʾ
	{
		CString StrTrace;
		StrTrace.Format(L"\n������ ���棺�Ѿ�Cancel������Ʒ��������Level2���ݹ�����, %d & %s\n",RealtimeLevel2.m_iMarketId, CString(RealtimeLevel2.m_StrMerchCode).GetBuffer());
		//TRACE(StrTrace);
	}
#endif
	
//	RGUARD(LockSingle, pMerch->m_LockRealtimePrice, LockRealtimePrice);
	{
		/////////////////////////////////////////////////
		// ֮ǰ��δ���յ�������, ����
		if (NULL == pMerch->m_pRealtimeLevel2)
		{
			pMerch->m_pRealtimeLevel2 = new CRealtimeLevel2();
		}
	
		// 
		*pMerch->m_pRealtimeLevel2 = RealtimeLevel2;
		
		// zhangbo 20080813 #bug�� ��ʱ��ԭ��Ӧ���ɷ��������͹����������Ƚ�׼ȷ�� ��ʱ��ֵ�� �Ѿ��ۻ���ͨѶʱ�䣬 �Ͳ���ȷ��
		pMerch->m_pRealtimeLevel2->m_TimeCurrent = GetServerTime();
		
		// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	//_LogCheckTime("[==>Client] CViewData::�յ���ʷ K �߻ذ�", g_hwndTrace);

	if (NULL == pMmiRespMerchKLine)
	{
		ASSERT(0);
		return;
	}

	// ������������̺�����
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
			// û�ҵ��ͽ�һ��
			pMerchKLineNodeFound = new CMerchKLineNode();
			pMerchKLineNodeFound->m_eKLineTypeBase = pMmiRespMerchKLine->m_MerchKLineNode.m_eKLineTypeBase;		
			pMerchKLineNodeFound->m_iMarketId	   = pMerch->m_MerchInfo.m_iMarketId;
			pMerchKLineNodeFound->m_StrMerchCode   = pMerch->m_MerchInfo.m_StrMerchCode;
			//

			pMerch->m_MerchKLineNodesPtr.Add(pMerchKLineNodeFound);
		}
		
		// ���������еĳɽ���
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

		// ��������
		bool32 bProcessKLineSuccess = pMerchKLineNodeFound->ProcessKLindData(pMmiRespMerchKLine->m_MerchKLineNode);
	
 
 		if ( NULL != m_pIndexChsStkMideCore )
 		{			
 			// �ѻ��Ĳ�Ҫ����ҵ����ͼ,���汾������,�ȴ��߳���ȡ:						
 			RGUARD(LockSingle, m_LockRecvDataForIndexChsStk, LockRecvDataForIndexChsStk);
 			RGUARD(LockSingle, m_LockNumsOfIndexChsStkReq, LockNumsOfIndexChsStkReq);
			// ��Ϣ������ʱ����ܻ������⣬��ʱ�����������
 			++m_iNumsOfIndexChsStkHaveRecved;
 			
 			// TRACE(L"�� %d ������, �յ� %d ��, ��� = %d \n", m_iNumsOfIndexChsStkReq, m_iNumsOfIndexChsStkHaveRecved, bFinish);
 
 			// ֪ͨ�յ��վ�
 			//m_pIndexChsStkMideCore->OnRecvKLineData(pMmiRespMerchKLine->m_MerchKLineNode.m_iMarketId, pMmiRespMerchKLine->m_MerchKLineNode.m_StrMerchCode, bFinish);
			PostMessage(m_MainFrameWnd, UM_IndexChsMideCore_KLineUpdate_Resp, (WPARAM)pMerch, (WPARAM)iMmiReqId);
 			// ��������
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
			//	_LogCheckTime("[==>Client] CViewData::�յ���ʷ K �߻ذ�, �������֪ͨ��ͼ����", g_hwndTrace);

				// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
				PostMessage(m_OwnerWnd, UM_ViewData_OnKLineNodeUpdate, (WPARAM)pMerch, (LPARAM)iMmiReqId);
			}
		}

		// ...fangz 0104 �����ѡ��ҲҪ��ô����, ��ʱ����, û�ո�
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
			// û�ҵ��ͽ�һ��
			pMerch->m_pMerchTimeSales = new CMerchTimeSales();
		}

		// ���������еĳɽ���
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

		// ��������
		bool32 bRet = pMerch->m_pMerchTimeSales->ProcessTimesSaleData(pMmiRespMerchTimeSales->m_MerchTimeSales);
		if (bRet)
		{
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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

	// ֪ͨ���е���ͼ����˳��ı�
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
			TRACE(_T("���Ա��۱����򷵻ذ���Ʒ: %d-%s\n"), MerchKey.m_iMarketId, strTempMerchCode.GetBuffer());
			strTempMerchCode.ReleaseBuffer();
	
			continue;
		}
				
		aMerchs.Add(pMerch);
	}

	// ֪ͨ���е�ҵ����ͼ����˳��ı�, ���ﲻ�����ж���Щ��ͼ����Ȥ�� �þ�����ͼ�Լ�ȥ�ж������Ƿ�������
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
	// ֪ͨ���е���ͼ����˳��ı�
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
			// ���ܴ��ڷ������˵���Ʒ�ͻ���û�е�
			CString strTempMerchCode = MerchKey.m_StrMerchCode;
			TRACE(_T("���԰�����򷵻ذ���Ʒ: %d-%s\n"), MerchKey.m_iMarketId, strTempMerchCode.GetBuffer());
			strTempMerchCode.ReleaseBuffer();
			continue;
		}
		
		aMerchs.Add(pMerch);
	}
	
	// ֪ͨ���е�ҵ����ͼ����˳��ı�, ���ﲻ�����ж���Щ��ͼ����Ȥ�� �þ�����ͼ�Լ�ȥ�ж������Ƿ�������
	for (i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->OnVDataReportInBlockUpdate(iBlockId, eMerchReportField, bDescSort, iPosStart, pMmiRespBlockReport->m_aMerchs.GetSize(), aMerchs);
	}
}

void CViewData::OnDataAnsRegisterPushPrice(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushPrice *pMmiAnsRegisterPushPrice)
{
	// zhangbo 20090620 #������ ��Э��
	//...
}

void CViewData::OnDataAnsRegisterPushTick(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushTick *pMmiAnsRegisterPushTick)
{
	// zhangbo 20090620 #������ ��Э��
	//...
}

void CViewData::OnDataAnsRegisterPushLevel2(int iMmiReqId, int32 iCommunicationId, IN const CMmiAnsRegisterPushLevel2 *pMmiAnsRegisterPushLevel2)
{
	// zhangbo 20090620 #������ ��Э��
	//...
}

void CViewData::OnDataRespCancelAttendMerch(int iMmiReqId, int32 iCommunicationId, IN const CMmiRespCancelAttendMerch *pMmiRespCancelAttendMerch)
{
	if (NULL == pMmiRespCancelAttendMerch)
	{
		ASSERT(0);
		return;
	}

	// zhangbo 20090620 #ȡ�����͵Ľӿ��Ѿ����ٵ��ã� ���Ըô�Ҳ�������
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
		StrMsg.Format(L"��֤���ִ���, ���Ժ����µ�¼!");
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
		// TRACE(L"===>����:%s %s ��һ: %.2f ��һ: %.2f \n", pRealtimePrice->m_StrMerchCode, StrTime, pRealtimePrice->m_astBuyPrices[0].m_fPrice, pRealtimePrice->m_astSellPrices[0].m_fPrice);
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
	_MYTRACE(L"������:%s\n", strTempRespJson.GetBuffer());
	strTempRespJson.ReleaseBuffer();

	//
	m_bKickOut = true;
	
	// ���Ӷ��ϵ�
	m_pCommManager->StopAllService();
	m_pNewsManager->StopAllNewsCommunication();

	if (m_MainFrameWnd != NULL)
	{
		CString* pStr = new CString(_T("�û��ڱ𴦵�¼!"));
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

	// Ŀǰֻ����ѡ��վ�õ�����ӿ�
	COptimizeServer::Instance()->OnDataRespNetTest(iMmiReqId, pMmiRespNetTest);
}

void CViewData::OnDataRespGeneralNormal(int iMmiReqId, IN const CMmiRespGeneralNormal *pMmiRespCMmiRespGeneralNormal)
{
	if ( NULL == pMmiRespCMmiRespGeneralNormal || pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetSize() <= 0 )
	{
		return;
	}

	// ֻ�����������, ������������ 1 ������������һ������, ����Ļ��ǵ������е�����
	// ���ܵ������������� 1 ������, ��Ӱ��
	bool32 bUpdateLast = false;

	if ( pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal.GetSize() == 1 )
	{
		bUpdateLast = true;
	}

	CMerch* pMerchNotify = NULL;
	
	//
	if ( 0 == pMmiRespCMmiRespGeneralNormal->m_iMarketId )
	{
		// ����
		m_MerchManager.FindMerch(L"000001", 0, pMerchNotify);

		int32 iSizeLocal = m_aGeneralNormalH.GetSize();

		//
		if ( iSizeLocal <= 0 || !bUpdateLast )
		{
			m_aGeneralNormalH.Copy(pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal);	
			
			// ��һ�ʵ���յ�
			m_stGeneralNoramlHPre.Clear();
		}
		else
		{
			// �ж�һ��ʱ���Ƿ�Ϸ�
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
				// ׷��
				m_aGeneralNormalH.Add(stPushData);

				// ������һ��
				m_stGeneralNoramlHPre = stLoacalLast;
			}
			else if ( lTimePush == lTimeLoacal )
			{
				// �滻
				m_aGeneralNormalH.RemoveAt(iSizeLocal - 1);
				m_aGeneralNormalH.Add(stPushData);

				// ������һ��
				m_stGeneralNoramlHPre = stLoacalLast;
			}
			else
			{
				// ��Ӧ�÷���
				// ASSERT(0);;
				return;
			}
		}		
	}
	else if ( 1000 == pMmiRespCMmiRespGeneralNormal->m_iMarketId )
	{
		// ����
		m_MerchManager.FindMerch(L"399001", 1000, pMerchNotify);

		int32 iSizeLocal = m_aGeneralNormalS.GetSize();

		//
		if ( iSizeLocal <= 0 || !bUpdateLast )
		{
			m_aGeneralNormalS.Copy(pMmiRespCMmiRespGeneralNormal->m_aGeneralNormal);			

			// �����һ��
			m_stGeneralNoramlSPre.Clear();
		}
		else
		{
			// �ж�һ��ʱ���Ƿ�Ϸ�
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
				// ׷��
				m_aGeneralNormalS.Add(stPushData);

				//
				m_stGeneralNoramlSPre = stLoacalLast;
			}
			else if ( lTimePush == lTimeLoacal )
			{
				// �滻
				m_aGeneralNormalS.RemoveAt(iSizeLocal - 1);
				m_aGeneralNormalS.Add(stPushData);

				//
				m_stGeneralNoramlSPre = stLoacalLast;
			}
			else
			{
				// ��Ӧ�÷���
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
		// ֪ͨ
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
	// Ϊ�� crcֵ�ͱ����ѻ������������������һ��, ���ϲ㴦�����Э�����
	if ( uiDataLen <= 0 || NULL == pData )
	{
		return;
	}

	//
	m_mapFundHold.clear();

	// �Ѷ������ݳ���
	uint32 uiReadLen = 0;

	_MYTRACE(L"��������ֲ�����, ���ݳ���: %d", uiDataLen);

	//
	while (1)
	{
		//
		if ( uiReadLen >= uiDataLen )
		{
			break;
		}

		// ��Ʒͷ��
		T_ClientRespFundHoldMerchHead stMerchHead;
		memcpyex(&stMerchHead, pData, sizeof(stMerchHead));

		uiReadLen += sizeof(stMerchHead);
		pData += sizeof(stMerchHead);

		// ��Ʒ����		
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
			_MYTRACE(L"��������ֲ�����, ����������ʶ�������Ʒ: %d XXX", iMarketId);
			
			uiReadLen += stMerchHead.uiDataLen;
			pData += stMerchHead.uiDataLen;

			continue;
		}

		//
		CMerch* pMerch = NULL;
		if ( !m_MerchManager.FindMerch(WStrCode.c_str(), iMarketId, pMerch) || NULL == pMerch )
		{			
			_MYTRACE(L"��������ֲ�����, ��������ʶ����Ʒ: %d %s", iMarketId, WStrCode.c_str());

			uiReadLen += stMerchHead.uiDataLen;
			pData += stMerchHead.uiDataLen;

			continue;		
		}

		//
		int32 iBodyCount = stMerchHead.uiDataLen / uiBodySize;
		T_ClientRespFundHoldMerchBody stMerchBody;

		// _MYTRACE(L"��Ʒ %d %s �� %d ��������:", iMarketId, WStrCode.c_str(), iBodyCount);

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
		// ֪ͨ
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
// 		TRACE(L"\niTradeID: %d aClientTradeTime����(%d):\n", iTradeTimeID, aClientTradeTime.GetSize());
// 		//
// 		for (int32 j = 0; j < aClientTradeTime.GetSize(); j++)
// 		{
// 			int32	m_iWeek = aClientTradeTime[j].m_iWeek;	// ���ڼ���ʱ�䣬-6��ʾ�������������죬����ʱ���ʾͨ��ʱ��
// 			long	m_iOpen = aClientTradeTime[j].m_iOpen;	// ����ʱ��
// 			long    m_iClose= aClientTradeTime[j].m_iClose;	// ����ʱ��
// 			
// 			TRACE(L"  ===========\n");
// 			TRACE(L"  ������ʱ��: iweek: %d iopen: %d iclose: %d\n", m_iWeek, m_iOpen, m_iClose);
// 			TRACE(L"  ����ʱ���:");
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
			// �������ϵ�crcֵ�뱾�ز�ͬ�������κ����ݴ�����
			// �������ϳ�����û�����ݣ������Ǵ��������
			// ���ڱ����ļ���ѡ�������˱����ļ����ܶ�Ӧ������������
			ASSERT( pMmiRespFundHold->m_uiCRC32 == 0 );
			m_mapFundHold.clear();	// �������, �ļ�������
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
		return;	// ����������û�д��κ����ݣ��ǾͲ������κβ�����
	}

	m_uiFundHoldCrc32 = pMmiRespFundHold->m_uiCRC32;
	AbstractFundHoldInfo(pMmiRespFundHold->m_uiDataLen, pMmiRespFundHold->m_pData);

	// ...fangz 0421 �����ѻ��ļ�
	unsigned long uTest = CCodeFile::crc_32(pMmiRespFundHold->m_pData, pMmiRespFundHold->m_uiDataLen);
	if (uTest)
	{
		// ....
	}

	CString StrHoldFile = CPathFactory::GetFundHoldFileFullName();
	CStdioFile file;
	if ( file.Open(StrHoldFile, CFile::typeBinary |CFile::modeCreate |CFile::modeWrite) )
	{
		DWORD dwLength = pMmiRespFundHold->m_uiDataLen;	// ������0���Ȼ���ʲô����
		const int dwMaxLength = 10*1024*1024;
		if ( dwLength <= dwMaxLength ) // ���ᳬ��10mb��
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
			// ���ͻذ�������Ļذ�һ����. ֻ�����Ͳ�ͬ
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


	// plugin��ͼ
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
			TRACE(L"��������ذ�: %d %s \n", MerchKey.m_iMarketId, MerchKey.m_StrMerchCode.GetBuffer());
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
			TRACE(L"����ذ�: %d %s \n", MerchKey.m_iMarketId, MerchKey.m_StrMerchCode);
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
	TRACE(_T("�յ������������ %d ��\r\n"), pMmiRespMainMonitor->m_mapMainMonitorData.size());
	
	for ( mapMainMonitor::const_iterator it = pMmiRespMainMonitor->m_mapMainMonitorData.begin(); it != pMmiRespMainMonitor->m_mapMainMonitorData.end(); ++it )
	{
		T_MainMonitorNode stNode = *it;

		TRACE(L"��Ʒ %d %s\n �ļ��������%d ��\n", stNode.m_MerchKey.m_iMarketId, stNode.m_MerchKey.m_StrMerchCode, stNode.m_aMainMonitor.size());
		for ( MonitorArray::iterator it2 = stNode.m_aMainMonitor.begin(); it2 != stNode.m_aMainMonitor.end(); ++it2 )
		{
			T_RespMainMonitor stRespMainMonitor = *it2;
			TRACE(L"   ʱ��: %d �۸�:%.2f ����:%.2f ��־:%d\n", stRespMainMonitor.m_Time, stRespMainMonitor.m_fPrice, stRespMainMonitor.m_fCounts, stRespMainMonitor.m_uBuy);
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

// ��ĳһ���г���ĳЩ��ʼ�����ݻ���
void CViewData::OnSomeInitializeMarketDataRet(CMarket &Market)
{
	if (Market.m_MarketInfoReqState.m_bRequesting || Market.m_MerchListReqState.m_bRequesting)
		return;

	// ����������������Ϣ
	set<CMarket*, CmpMarket>::iterator itFind = m_aMarketRequesting.find(&Market);
	if ( itFind != m_aMarketRequesting.end() )
	{
		//
		m_aMarketRequesting.erase(itFind);
		m_iInitializedMarketCount++;		//	��¼ͬ���г���
		//
		_MYTRACE(L"�յ��г� %d �Ļذ�\n", Market.m_MarketInfo.m_iMarketId);			
	}
	else
	{
		ASSERT(0);
	}

	// ���г��Ѿ���ʼ���ɹ�
	Market.SetInitialized(true);
	Market.m_TimeLastInitialize = GetServerTime();;
	
	// ������ڵ�¼�� ֪ͨ��¼��״̬�仯
	if (NULL != m_LoginWnd)
	{
		CString *pStrTip = new CString;
		pStrTip->Format(L"��ʾ��ͬ�� %s �г���Ϣ���", Market.m_MarketInfo.m_StrCnName.GetBuffer());
		PostMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);

	}

	// �ж��Ƿ�ȫ����ʼ����ɣ� �ǣ� ���¼�������
	if (m_iInitializedMarketCount != m_iAllMarketCount )
	{	
		//	��¼�г��Ƿ������Ʊ���ڻ�����
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

		// ��ʼ��MerchManagerType��־(��¼��Ʊ���ڻ�����)
		m_dwMerchManagerType = dwMerchManagerType;		
		// �Ѿ�������������, ��ʼ��һ������:
		ReqMarketInfo();
	}
	else
	{		
		CSysBlockManager::Instance()->Initialize(&m_MerchManager);
		CUserBlockManager::Instance()->Initialize(m_strUserName,&m_MerchManager);
		CBlockConfig::Instance()->SetViewData(this);
		CBlockConfig::Instance()->RequestLogicBlockFile(this);		// ��������ļ�����


		// �Ƹ���� ��ʹû�й�ƱҲ����
		if (NULL != m_LoginWnd /*&& (m_dwMerchManagerType&EMMT_StockCn) == EMMT_StockCn*/ )
		{
			_MYTRACE(L"��ʾ����ʼͬ������б���Ϣ");

			CString *pStrTip = new CString(L"��ʾ����ʼͬ������б���Ϣ...");
			PostMessage(m_LoginWnd,UM_ViewData_OnMessage, (WPARAM)pStrTip,0);

			// ���õ�¼��ʱ
			StartTimer(KTimerIdWaitLogicBlockFileTraslateTime, KTimerPeriodLogicBlockFileTraslateTime);
		}
		else
		{
			// ���û�е�¼�Ի��򣬾�����ǰ�İ�
			// ֪ͨ�����г���ʼ���ɹ� - �ȴ�����ļ���ʼ����ɺ󣬻��߳�ʱ��
			_MYTRACE(L"����ͬ������б�ֱ�ӳ�ʼ���г�(%d)", m_dwMerchManagerType);
			OnAllMarketInitializeSuccess();
		}

		ASSERT(m_aMarketRequesting.empty());
		_MYTRACE(L"��ʼ���г����!");
	}
	
	//{
	//
	//	// ������������ڸ�ҹ��ʼ��
	//	// ����������г��йص�ҵ����ͼͳͳ��Ҫ֪ͨForceUpdate, ����������˵�� ��Ҫ�����ʱ������жϸ�������������ݻ��ǽ��������
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
	//			// ǿ�Ƹ���ҵ����ͼ				
	//			pIoView->ForceUpdateVisibleIoView();
	//		}
	//	}

	//	// ������û����Ҫ��ʼ����, ��������:
	//	ReqMarketInfo();
	//
	//}
}

// �������г���ʼ���ɹ��� ��������򿪺��һ��ʹ��ʱ��������¼�
void CViewData::OnAllMarketInitializeSuccess()
{
	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(_T("�г���ʼ�����"), TRUE);
	if (ELSLogining != m_eLoginState)
	{
		ASSERT(0);
		return;
	}
	
	StopTimer(KTimerIdWaitLogicBlockFileTraslateTime);
	
	// 
	m_eLoginState = ELSLoginSuccess;

	// ����ȫ����ʼ���ɹ�
	m_MerchManager.SetInitializedAll();

	// �����Ҫ��Ʒ��Ϣ, ���Էŵ������ʼ��
	//m_pAlarmCenter = new CAlarmCenter(this);

	// ��ʼ����ϣ������ı�
	m_pCommManager->SetStage(ECSTAGE_QuoteSupplier);
	m_pNewsManager->SetStage(ENSTAGE_NewsSupplier);
	
	if ( NULL != m_LoginWnd)
	{
		SendMessage(m_LoginWnd,UM_ViewData_OnAllMarketInitializeSuccess, 0, 0);
	}
	
	// ������ҵ����ͼ����ǿ��ˢ���¼�
	int32 i;
	for ( i = 0; i < m_IoViewList.GetSize(); i++)
	{
		CIoViewListner *pIoView = m_IoViewList[i];
		pIoView->ForceUpdateVisibleIoView();
	}

	// �������������״̬��������Ʒ�ı���
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
	
	// װ�������Ʒ
	CString StrRelativeMerchsFilePath	= CPathFactory::GetPublicConfigPath() + CPathFactory::GetRelativeMerchsFileName();
	m_RelativeMerchManager.m_RelativeMerchList.RemoveAll();
	CRelativeMerchManager::FromXml(StrRelativeMerchsFilePath, m_RelativeMerchManager.m_RelativeMerchList);

	CString StrHkWarrantFilePath		= CPathFactory::GetPublicConfigPath() + CPathFactory::GetHkWarrantFileName();
	m_RelativeMerchManager.m_HkWarrantList.RemoveAll();
	CRelativeMerchManager::FromXml(StrHkWarrantFilePath, m_RelativeMerchManager.m_HkWarrantList);

	
	// ����һ�λ�������	
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

	// �����������֤����
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

	// ����ĳһ���г��Ļ�����Ϣ
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMarketInfo->m_iMarketId, pMarket))
	{
		return;		// �Ҳ�����Ӧ���г�
	}
	
	// �������������
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMarketInfo, aMmiReqNodes);

	// ����ͨѶid�� ����״̬
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
				ASSERT(0);		// ��Ӧ�÷����������
			}
		}
		else
		{
			ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
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

	// ����ĳһ���г��Ļ�����Ϣ
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMarketSnapshot->m_iMarketId, pMarket))
	{
		// �Ҳ�����Ӧ���г�
		return;
	}
	
	// �������������
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

	// �鿴�Ƿ���ָ���Ĵ��г�
	CBreed *pBreed = NULL;
	int32 iPosBreed = 0;
	if (!m_MerchManager.FindBreed(pMmiReqBroker->m_iBreedId, iPosBreed, pBreed))
	{
		// �Ҳ�����Ӧ�Ĵ��г�
		return;
	}

	// ��ʱ�������е�
	CBroker *pBroker = NULL;
	for (int32 iIndexBroker = 0; iIndexBroker < pBreed->m_BrokersPtr.GetSize(); iIndexBroker++)
	{
		pBroker = pBreed->m_BrokersPtr[iIndexBroker];
		if (NULL == pBroker)
			continue;

		pBroker->m_bRemoved = true;
	}
	
	// �������������
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

	// �鿴�Ƿ���ָ�����г�
	CMarket *pMarket = NULL;
	if (!m_MerchManager.FindMarket(pMmiReqMerchInfo->m_iMarketId, pMarket))
	{		
		return;	// �Ҳ�����Ӧ���г�
	}

	// �����еı�־
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
	
	// �������������
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchInfo, aMmiReqNodes);

	// ����ͨѶid�� ����״̬
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
				ASSERT(0);		// ��Ӧ�÷����������
			}
		}
		else
		{
			ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
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

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqPublicFile->m_StrMerchCode, pMmiReqPublicFile->m_iMarketId, pMerch))
	{		
		return;	// �Ҳ�����Ӧ����Ʒ
	}
	
	if (EPFTWeight == pMmiReqPublicFile->m_ePublicFileType)
	{
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedReqWeightData(TimeServer))	// �Ƿ���Ҫ���³�Ȩ����
		{
			return;	// ����Ҫ���³�Ȩ����
		}
	}
	else if ( EPFTF10 == pMmiReqPublicFile->m_ePublicFileType )
	{
		// fangz 0818
		// xl 1104 ��������2Сʱ��������
		// ��ȡ���ز������ݣ����crc32ֵ


		CString StrFilePath;
		const_cast<CMmiReqPublicFile *>(pMmiReqPublicFile)->m_uiCRC32 = 0;
		if ( m_OfflineDataManager.GetPublishFilePath(EPFTF10, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath) )
		{
			// ������:
			CFile file;
			ULONGLONG ullDataLen;
			char acBuffer[1024*5];
			memset(acBuffer, 0, sizeof(acBuffer));

			if ( file.Open(StrFilePath, CFile::modeRead | CFile::typeBinary) )				
			{
				if ( (ullDataLen = file.GetLength()) > 0 && (file.Read(acBuffer, (uint32)ullDataLen)) == (uint32)ullDataLen )
				{
					// �ļ���CRC32 ֵ
					const_cast<CMmiReqPublicFile *>(pMmiReqPublicFile)->m_uiCRC32 = CCodeFile::crc_32(acBuffer, (uint32)ullDataLen);
					
					// ��������
					if ( NULL == pMerch->m_pFinanceData )
					{
						pMerch->m_pFinanceData = new CFinanceData();
					}
					if ( CMerch::ReadFinanceDataFromBuffer(acBuffer, (int32)ullDataLen, *pMerch->m_pFinanceData) )    
					{
						// ����һ����ٵĸ���
						PostMessage(m_OwnerWnd, UM_ViewData_OnPublicFileUpdate, (WPARAM)pMerch, EPFTF10);	
					}
				}

				file.Close();
			}			
		}
		
		CGmtTime TimeNow = GetServerTime();
		if ( !pMerch->DoesNeedReqFinanceData(TimeNow) )
		{
			return;	// ��������������� - ��̬���ݸ����Ƿ�Ӧ����ȡΪ���������Ӧ��ʽ����
		}
	}
	else
	{	
		// ��֧�ֵ�����
		ASSERT(0);
		return;
	}

	// �������������
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

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchExtendData->m_StrMerchCode, pMmiReqMerchExtendData->m_iMarketId, pMerch))
	{
		// �Ҳ�����Ӧ����Ʒ
		return;
	}

	// ���Ƿ��Ѿ�������չ����
	if (NULL != pMerch->m_pMerchExtendData)
	{
		// �Ѿ���������
		return;
	}

	// �������������
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

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchF10->m_StrMerchCode, pMmiReqMerchF10->m_iMarketId, pMerch))
	{
		// �Ҳ�����Ӧ����Ʒ
		return;
	}

	// ���Ƿ��Ѿ�����F10����
	if (NULL != pMerch->m_pMerchF10)
	{
		// �Ѿ���������
		return;
	}

	// �������������
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

	// ��������Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
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

		// �鿴�Ƿ���ָ������Ʒ
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// �ȴ����Ͷ����в��ҿ��Ƿ�������
		if ( NULL != pMerch->m_pRealtimePrice && pMerch->DoesNeedPushPrice(GetServerTime()) )
		{
			// ���������ڼ䣬�ұ�����ʵʱ�������ݵ�������ڼ���Ƿ�Ҫ����
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimePrices.find(pMerch);
			if (itFind != m_aPushingRealtimePrices.end())	// �ҵ��� ��ʾ��������
			{
				bIgnoreRequest = true;
			}	
		}
		
		// 
		if (bIgnoreRequest)
		{
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ - ������ʱ�п���û��real time price����
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimePriceUpdate, (WPARAM)pMerch, 0);
		}
		else
		{
			// ����һ����Ʒ
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

			// �������������
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
		// ���������г����ֶ��������
		if (aMerchs.GetSize() > 0)
		{
			while (aMerchs.GetSize() > 0)
			{
				// ������֯���� ������������п��ܻ��иı�
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(aMerchs[0].m_iMarketId, EDSTPrice);
				
				if (!m_bDirectToQuote && iServiceId < 0)	// �Ҳ�����Ӧ�ķ��� ˵����ʱû�з�����֧�ָ���������
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
						if (iServiceIdCur < 0)	// �Ҳ�����Ӧ�ķ��� ˵����ʱû�з�����֧�ָ���������
						{
							aMerchs.RemoveAt(i, 1);
						}
						else if (iServiceIdCur == iServiceId)	// ͬһ�����ݷ���
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
				
				// �������������
				CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
				int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);
				
				// ����ͨѶid�� ����״̬
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
							ASSERT(0);;		// ��Ӧ�÷����������
						}
					}
					else
					{
						ASSERT(0);;	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
					}
				}
				
				// 
				if (iCommunicationId < 0)
				{
					// ASSERT(0);;	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
				}
				else
				{
					T_PushServerAndTime stPushServerAndTime;
					stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
					stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
					
					// �������ͼ�¼
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
		// ֱ������, ����һ��������, �������߰�����ж�
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
			
			// �鿴�Ƿ���ָ������Ʒ
			CMerch *pMerch = NULL;
			if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
				continue;
			
			// �������̺�Ĳ���Ҫ������
			CGmtTime TimeServer = GetServerTime();
			if (!pMerch->DoesNeedPushPrice(TimeServer))	// �Ƿ������µ����ݣ� ����ǣ� ����Ҫ���� ֱ��֪ͨ�����ͼȥˢ��
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
	
	// ����Ѿ����������͵����ݣ� ����ԭ�µķ�����ע�����Ʒ�����ͣ� ����������������A��������������x��Ʒ, ��ʱ����B������ע��x��Ʒ����
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimePrices = m_aPushingRealtimePrices;

	// ���������������Ϣ
	m_aPushingRealtimePrices.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// ��������Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
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

		// �鿴�Ƿ���ָ������Ʒ
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// �������̺�Ĳ���Ҫ������
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushPrice(TimeServer))	// �Ƿ������µ����ݣ� ����ǣ� ����Ҫ���� ֱ��֪ͨ�����ͼȥˢ��
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
			// NULL;	// ����Ҫ����
		}
		else
		{
			// ����һ����Ʒ����
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// �ӵ�ǰ�����б��в��ң� ���Ƿ��Ѿ��������б����ˣ� ����Ѿ����������ˣ� �Ǿ���Ȼʹ�õ�ǰ���͵ķ�����ע�ᣬ �������ͣ� ���������̨����������ͬһ����Ʒ��ͬһ�����͵�����
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimePrices.find(pMerch);
			if (itFind == aPushingRealtimePrices.end())	// û���ҵ������ͣ� �����²���һ�����ṩ����Ʒʵʱ�������͵ķ�����
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTPrice);
				if (iServiceId >= 0)	// �����ҵ���Ӧ�ķ��� �ҵ����ṩ�÷�������з�����
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
								// zhangbo 20090917 #���Ż��� ������ʱ������ȡ�ṩ�÷���ĵ�һ̨�������� ����Ӧ��ȡ�ø��Ž⣬ �������
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// �ҵ��ˣ� �ͼ���ʹ��֮ǰ�ķ�����
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// ��Ҫ����ظ���Ʒ
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
	
	// ���������г����ֶ��������
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

			// �������������
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

			// ����ͨѶid�� ����״̬
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);// ��Ӧ�÷����������, �²�û�а�ָ���ķ�������������
						}
					}
					else
					{
						ASSERT(0);	// ��Ӧ�÷����������
					}
				}
				else
				{
					ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// �������ͼ�¼
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
			// ԭ��������, ���ڲ���Ҫ������, ��ʱ��Ҫȡ������, ����ʲô�¶�����
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
					// ��һ���յ�����, ȡ�������ڵ�
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
	
	// ��������Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
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

		// �鿴�Ƿ���ָ������Ʒ
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// �ȴ����Ͷ����в��ҿ��Ƿ�������
		if ( NULL != pMerch->m_pRealtimeTick && pMerch->DoesNeedPushTick(GetServerTime()) )
		{
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeTicks.find(pMerch);
			if (itFind != m_aPushingRealtimeTicks.end())	// �ҵ��� ��ʾ��������
			{
				bIgnoreRequest = true;
			}
		}

		// 
		if (bIgnoreRequest)
		{
			// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
			PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeTickUpdate, (WPARAM)pMerch, 0);
		}
		else
		{
			// ����һ����Ʒ
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= iMarketId;
			MerchKey.m_StrMerchCode	= StrMerchCode;
			
			aMerchs.Add(MerchKey);
		}
	}
	
	// ���������г����ֶ��������
	if (aMerchs.GetSize() > 0)
	{
		while (aMerchs.GetSize() > 0)
		{
			// ������֯���� ������������п��ܻ��иı�
			int32 iServiceId = 0;// m_pServiceDispose->JudgeServiceId(aMerchs[0].m_StrMerchCode, aMerchs[0].m_iMarketId, EDSTTick);
			if (iServiceId < 0)	// �Ҳ�����Ӧ�ķ��� ˵����ʱû�з�����֧�ָ���������
				continue;
			
			CMmiReqRealtimeTick Req;
			Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
			Req.m_iMarketId		= aMerchs[0].m_iMarketId;
			aMerchs.RemoveAt(0, 1);

			int32 i;
			for ( i = aMerchs.GetSize() -1; i >= 0; i--)
			{
				int32 iServiceIdCur = 0;// m_pServiceDispose->JudgeServiceId(aMerchs[i].m_StrMerchCode, aMerchs[i].m_iMarketId, EDSTTick);
				if (iServiceIdCur < 0)	// �Ҳ�����Ӧ�ķ��� ˵����ʱû�з�����֧�ָ���������
				{
					aMerchs.RemoveAt(i, 1);
				}
				else if (iServiceIdCur == iServiceId)	// ͬһ�����ݷ���
				{
					Req.m_aMerchMore.Add(aMerchs[i]);
					aMerchs.RemoveAt(i, 1);
				}
				else
				{
					// NULL;
				}
			}

			// �������������
			CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
			int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);

			// ����ͨѶid�� ����״̬
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
						ASSERT(0);		// ��Ӧ�÷����������
					}
				}
				else
				{
					ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
				}
			}
			
			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
				
				// �������ͼ�¼
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
		// ֱ������, ����һ��������, �������߰�����ж�
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
			
			// �鿴�Ƿ���ָ������Ʒ
			CMerch *pMerch = NULL;
			if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
				continue;
			
			// �������̺�Ĳ���Ҫ������
			CGmtTime TimeServer = GetServerTime();
			if (!pMerch->DoesNeedPushPrice(TimeServer))	// �Ƿ������µ����ݣ� ����ǣ� ����Ҫ���� ֱ��֪ͨ�����ͼȥˢ��
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
	
	// ����Ѿ����������͵����ݣ� ����ԭ�µķ�����ע�����Ʒ�����ͣ� ����������������A��������������x��Ʒ, ��ʱ����B������ע��x��Ʒ����
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimeTicks = m_aPushingRealtimeTicks;

	// ���������������Ϣ
	m_aPushingRealtimeTicks.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// ��������Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
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

		// �鿴�Ƿ���ָ������Ʒ
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// �������̺�Ĳ���Ҫ������
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushTick(TimeServer))	// �Ƿ������µ����ݣ� ����ǣ� ����Ҫ���� ֱ��֪ͨ�����ͼȥˢ��
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
			// NULL;	// ����Ҫ����
		}
		else
		{
			// ����һ����Ʒ����
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// �ӵ�ǰ�����б��в��ң� ���Ƿ��Ѿ��������б����ˣ� ����Ѿ����������ˣ� �Ǿ���Ȼʹ�õ�ǰ���͵ķ�����ע�ᣬ �������ͣ� ���������̨����������ͬһ����Ʒ��ͬһ�����͵�����
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimeTicks.find(pMerch);
			if (itFind == aPushingRealtimeTicks.end())	// û���ҵ������ͣ� �����²���һ�����ṩ����Ʒʵʱ�������͵ķ�����
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTTick);
				if (iServiceId >= 0)	// �����ҵ���Ӧ�ķ��� �ҵ����ṩ�÷�������з�����
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
								// zhangbo 20090917 #���Ż��� ������ʱ������ȡ�ṩ�÷���ĵ�һ̨�������� ����Ӧ��ȡ�ø��Ž⣬ �������
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// �ҵ��ˣ� �ͼ���ʹ��֮ǰ�ķ�����
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// ��Ҫ����ظ���Ʒ
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
	
	// ���������г����ֶ��������
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

			// �������������
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

			// ����ͨѶid�� ����״̬
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);	// ��Ӧ�÷����������, �²�û�а�ָ���ķ�������������
						}
					}
					else
					{
						ASSERT(0);		// ��Ӧ�÷����������
					}
				}
				else
				{
					ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// �������ͼ�¼
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
	
	// ����Ѿ����������͵����ݣ� ����ԭ�µķ�����ע�����Ʒ�����ͣ� ����������������A��������������x��Ʒ, ��ʱ����B������ע��x��Ʒ����
	std::map<CMerch*, T_PushServerAndTime> aPushingRealtimeLevel2s = m_aPushingRealtimeLevel2s;

	// ���������������Ϣ
	m_aPushingRealtimeLevel2s.clear();

	// 
	bool32 bHasInitServiceList = false;
	CArray<T_TinyServiceInfo, T_TinyServiceInfo&> aActiveServiceList;
	
	// ��������Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
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

		// �鿴�Ƿ���ָ������Ʒ
		CMerch *pMerch = NULL;
		if (!m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch))
			continue;

		// �������̺�Ĳ���Ҫ������
		CGmtTime TimeServer = GetServerTime();
		if (!pMerch->DoesNeedPushLevel2(TimeServer))	// �Ƿ������µ����ݣ� ����ǣ� ����Ҫ���� ֱ��֪ͨ�����ͼȥˢ��
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
			// NULL;	// ����Ҫ����
		}
		else
		{
			// ����һ����Ʒ����
			T_MerchAndCommunication MerchAndCommunication;
			MerchAndCommunication.m_pMerch = pMerch;

			// �ӵ�ǰ�����б��в��ң� ���Ƿ��Ѿ��������б����ˣ� ����Ѿ����������ˣ� �Ǿ���Ȼʹ�õ�ǰ���͵ķ�����ע�ᣬ �������ͣ� ���������̨����������ͬһ����Ʒ��ͬһ�����͵�����
			std::map<CMerch*, T_PushServerAndTime>::iterator itFind = aPushingRealtimeLevel2s.find(pMerch);
			if (itFind == aPushingRealtimeLevel2s.end())	// û���ҵ������ͣ� �����²���һ�����ṩ����Ʒʵʱ�������͵ķ�����
			{
				int32 iServiceId = 0;//m_pServiceDispose->GetServiceId(iMarketId, EDSTLevel2);
				if (iServiceId >= 0)	// �����ҵ���Ӧ�ķ��� �ҵ����ṩ�÷�������з�����
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
								// zhangbo 20090917 #���Ż��� ������ʱ������ȡ�ṩ�÷���ĵ�һ̨�������� ����Ӧ��ȡ�ø��Ž⣬ �������
								MerchAndCommunication.m_iCommunicationId = pTinyServices[iIndexService].m_aCommunicationIds[0];
							}

							break;
						}
					}
				}
			}
			else	// �ҵ��ˣ� �ͼ���ʹ��֮ǰ�ķ�����
			{
				T_PushServerAndTime *pPushServerAndTime = (T_PushServerAndTime *)&itFind->second;
				MerchAndCommunication.m_iCommunicationId = pPushServerAndTime->m_iCommunicationId;
			}
			
			// 
			if (MerchAndCommunication.m_iCommunicationId >= 0)
			{
				// ��Ҫ����ظ���Ʒ
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
	
	// ���������г����ֶ��������
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

			// �������������
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

			// ����ͨѶid�� ����״̬
			if (iRet > 0)
			{
				if (aMmiReqNodes.GetSize() == 1)
				{
					CMmiReqNode *pMmiReqNodeOut = aMmiReqNodes[0];
					if (NULL != pMmiReqNodeOut)
					{
						if (iCommunicationId != pMmiReqNodeOut->m_iCommunicationId)
						{
							ASSERT(0);	// ��Ӧ�÷����������, �²�û�а�ָ���ķ�������������
						}
					}
					else
					{
						ASSERT(0);		// ��Ӧ�÷����������
					}
				}
				else
				{
					ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
				}
			}

			// 
			if (iCommunicationId < 0)
			{
				// ASSERT(0);;	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
			}
			else
			{
				T_PushServerAndTime stPushServerAndTime;
				stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
				stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();

				// �������ͼ�¼
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

	// �����������������
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

	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
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
	
	// �����������������
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	m_pDataManager->RequestData((CMmiCommBase *)pMmiReqDelPushMerchTrendIndex, aMmiReqNodes, EDSCommunication, iCommunicationID);
}

void CViewData::OnViewReqFundHold(IN const CMmiReqFundHold *pMmiReqFundHold, IN int32 iCommunicationID)
{	if ( NULL == pMmiReqFundHold )
	{
		ASSERT(0);
		return;
	}

	// �ӱ��ض�ȡ�����fundhold���ݣ�����crc
	// �������������crcֵ������뿼�ǽ�����Զ���дcrc�Ĺ���ȥ��
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
					// ѡ���ڴ˴��������ݣ�����������ݲ�һ��ʱ��������ʾһ���������ݣ�Ȼ���ڸ��³���ȷ������
					// AbstractFundHoldInfo(dwLength, pBuf);
					m_uiFundHoldCrc32 =  CCodeFile::crc_32(pBuf, (int32)ullLength);
				}
				delete []pBuf;
			}
			
			file.Close();
		}
	}

	const_cast<CMmiReqFundHold *>(pMmiReqFundHold)->m_uiCRC32 = m_uiFundHoldCrc32;	// ʹ�ñ����crc32

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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
		TRACE(L"�����֤������������\n");
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
	// ֻͨ���ĸ����ݵ�ҵ����ͼ(��ʵֻ��K ��)
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
	
	// �����Ʒ�Ƿ���Ҫ��������������ݣ� �������ٷ�������ͨѶ��
	// 
	bool32 bIgnoreRequest = false;

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqRealtimeLevel2->m_StrMerchCode, pMmiReqRealtimeLevel2->m_iMarketId, pMerch))
		return;

	// �ȴ����Ͷ����в��ҿ��Ƿ�������
	if ( NULL != pMerch->m_pRealtimeLevel2 && pMerch->DoesNeedPushLevel2(GetServerTime()) )
	{
		std::map<CMerch*, T_PushServerAndTime>::iterator itFind = m_aPushingRealtimeLevel2s.find(pMerch);
		if (itFind != m_aPushingRealtimeLevel2s.end())	// �ҵ��� ��ʾ��������
		{
			bIgnoreRequest = true;
		}
	}

	// 
	if (bIgnoreRequest)
	{
		// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
		PostMessage(m_OwnerWnd, UM_ViewData_OnRealtimeLevel2Update, (WPARAM)pMerch, 0);
		return;
	}
		
	// ������
	int32 iServiceId = 0; 
	if (iServiceId < 0)	// �Ҳ�����Ӧ�ķ��� ˵����ʱû�з�����֧�ָ���������
		return;

	// ������֯���� ������������п��ܻ��иı�
	CMmiReqRealtimeLevel2 Req;
	Req.m_StrMerchCode	= pMmiReqRealtimeLevel2->m_StrMerchCode;
	Req.m_iMarketId		= pMmiReqRealtimeLevel2->m_iMarketId;
				
	// �������������
	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iRet = m_pDataManager->RequestData((CMmiCommBase *)&Req, aMmiReqNodes);
	
	// ����ͨѶid�� ����״̬
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
				ASSERT(0);		// ��Ӧ�÷����������
				return;
			}
		}
		else
		{
			ASSERT(0);	// ��Ӧ�÷������������������Ӧ���������͸����� ���ҷ��ط�������Ϣ
			return;
		}
	}
	
	// 
	if (iCommunicationId < 0)
	{
		ASSERT(0);	// �Ҳ��������ṩָ�����ݷ���ķ������� ������û��Ȩ�ޣ� �����Ƿ�����������
		return;
	}
	else
	{
		T_PushServerAndTime stPushServerAndTime;
		stPushServerAndTime.m_iCommunicationId	= iCommunicationId;
		stPushServerAndTime.m_uiUpdateTime		= (uint32)GetServerTime().GetTime();
		
		// �������ͼ�¼
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

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchKLine->m_StrMerchCode, pMmiReqMerchKLine->m_iMarketId, pMerch))
	{
		// �Ҳ�����Ӧ����Ʒ
		return;
	}

	if (pMmiReqMerchKLine->m_eKLineTypeBase != EKTBMinute && 
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTB5Min &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBHour &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBDay &&
		pMmiReqMerchKLine->m_eKLineTypeBase != EKTBMonth)
	{
		// k�����Ͳ���
		return;
	}

	CString StrLog;
	if ( ERTYSpecifyTime == pMmiReqMerchKLine->m_eReqTimeType )
	{
		StrLog.Format(L"[Client==>] CViewData::������ʷK ��: %s", ((CMmiReqMerchKLine *)pMmiReqMerchKLine)->GetSummary().GetBuffer());
	}
	else if ( ERTYFrontCount == pMmiReqMerchKLine->m_eReqTimeType )
	{
		StrLog.Format(L"[Client==>] CViewData::��ǰ������ʷK �� %d ��", pMmiReqMerchKLine->m_iFrontCount);
	}
	else 
	{
		StrLog.Format(L"[Client==>] CViewData::������ʷK �� %d ��", pMmiReqMerchKLine->m_iCount);
	}
	
	//_LogCheckTime(StrLog, g_hwndTrace);

	// �����Ƿ���ָ�����͵�K��
	int32 iPosKLineFound = 0;
	CMerchKLineNode *pMerchKLineNodeFound = NULL;
	if (!pMerch->FindMerchKLineNode(pMmiReqMerchKLine->m_eKLineTypeBase, iPosKLineFound, pMerchKLineNodeFound) || pMerchKLineNodeFound->m_KLines.GetSize() <= 0)
	{
		// ������������������
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		m_pDataManager->RequestData((CMmiCommBase *)pMmiReqMerchKLine, aMmiReqNodes);
	}
	else
	{
		// zhangbo 0625 #���Ż�
		// �����������ж�, �������жϵĻ��� ���ܺܺõ��ж������Ƿ������ģ� ���ᵼ�¿��ܵ����ݲ������Ӷ������Ĳ���Ҫ�Ĵ���
		// ��ͬʱҲ����������ϵ�����, ��Ϊ�Դ󲿷������˵�� ���������ԵĿ��Ƕ��Ƕ���ģ� ����datamanager�� �ֻ����ܶ����֮��Ĳ����� �˷�������
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

	// �鿴�Ƿ���ָ������Ʒ
	CMerch *pMerch = NULL;
	if (!m_MerchManager.FindMerch(pMmiReqMerchTimeSales->m_StrMerchCode, pMmiReqMerchTimeSales->m_iMarketId, pMerch))
	{
		// �Ҳ�����Ӧ����Ʒ
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
// 				if (pMerchInfo->m_StrMerchCodeInBourse.GetLength() > 0)		// ����������������
// 					HotKeyList.Add(pMerchInfo->m_StrMerchCodeInBourse);
				if (pMerchInfo->m_StrMerchCode.GetLength() > 0)				// pobo����������
					HotKeyList.Add(pMerchInfo->m_StrMerchCode);
// 				if (pMerchInfo->m_StrHotKey.GetLength() > 0)				// �ȼ�������
// 					HotKeyList.Add(pMerchInfo->m_StrHotKey);

				// ���ظ��Ĺ��˵��� �ӿ���������ٶ�
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



	// �ø���Ʒ�������Ʒ�б��в�����ص���Ʒ
	CArray<CRelativeMerch, CRelativeMerch&> RelativeList;
	m_RelativeMerchManager.FindRelativeMerchs(pMerch, m_MerchManager, CRelativeMerch::ERTWarrant | CRelativeMerch::ERTAB | CRelativeMerch::ERTAH, RelativeList);
	if (RelativeList.GetSize() > 0)
	{
		CRelativeMerchNode RelativeMerchNode;
		RelativeMerchNode.m_RelativeSummary = L"�����Ʒ";

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
					SiblingMerch.m_StrSiblingName = L"��Ĺ�";
				else
					SiblingMerch.m_StrSiblingName = L"Ȩ֤";
			}
			else if (CRelativeMerch::ERTAB == RelativeMerch.m_eRelativeType)
			{
				if (bSrc)
					SiblingMerch.m_StrSiblingName = L"A��";
				else
					SiblingMerch.m_StrSiblingName = L"B��";
			}
			else if (CRelativeMerch::ERTAH == RelativeMerch.m_eRelativeType)
			{
				if (bSrc)
					SiblingMerch.m_StrSiblingName = L"A��";
				else
					SiblingMerch.m_StrSiblingName = L"H��";
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

	// �ѻ���ʱ��, ȡ����ʱ��
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

	// ������ڵĸ۹�Ȩ֤��������еĹ�ϵ
	m_RelativeMerchManager.m_RelativeMerchList.RemoveAll();

	// �����ڵ��ļ��ж�ȡ��ϵ
	CRelativeMerchManager::FromXml(StrRelativeMerchsFile, m_RelativeMerchManager.m_RelativeMerchList);

	// ֪ͨ������ͼ�����Ʒ�����仯��
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		m_IoViewList[i]->OnVDataRelativeMerchsUpdate();
	}
}

void CViewData::OnHkWarrantsFileUpdate()
{
	CString StrHkWarrantFile = CPathFactory::GetPublicConfigPath() + CPathFactory::GetHkWarrantFileName();

	// ������ڵĸ۹�Ȩ֤
	m_RelativeMerchManager.m_HkWarrantList.RemoveAll();

	// �����ڵ��ļ��ж�ȡ��ϵ
	CRelativeMerchManager::FromXml(StrHkWarrantFile, m_RelativeMerchManager.m_HkWarrantList);

	// ֪ͨ������ͼ�����Ʒ�����仯��
	for (int32 i = 0; i < m_IoViewList.GetSize(); i++)
	{
		m_IoViewList[i]->OnVDataRelativeMerchsUpdate();
	}
}

void CViewData::SetIndexChsStkMideCore(CIndexChsStkMideCore* pMidCore)
{	
	if ( NULL == pMidCore )
	{
		// ֹͣѡ��		
		m_pIndexChsStkMideCore = NULL;
		m_aRecvDataForIndexChsStk.RemoveAll();	
		
		m_iNumsOfIndexChsStkReq			= -1;
		m_iNumsOfIndexChsStkHaveRecved	= 0;
	}
	else
	{
		// ��ʼѡ��		
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
		//pDoc->m_pDlgLogIn->SetPromptInfo(L"�����������������...");
	}

	//
	T_ServerInfo* pServerInfo;
	int iCountServers;
	m_pServiceDispose->GetServerList(&pServerInfo, iCountServers);
	
	int32 iCountQuoteServers = 0;
	
	// ����ֵ
	for ( int32 i = 0; i < iCountServers; i++ )
	{
		if ( CheckFlag (pServerInfo[i].enType, enSTQuote) )
		{	
			iCountQuoteServers += 1;
		}				
	}

	// ��¼ʱ
	bool32 bAllConnect	  = false;
	bool32 bAllNonConnect = false;

	if ( -1 == iCommunicationId )
	{
		// ������������ -1, �����Ƿ�ȫ��û�����ϵ�bool32 ֵ
		if ( iCountQuoteServers == m_aQuoteServerAll.GetSize() )
		{
			bAllNonConnect = true;
		}

		// ֱ�ӷ���
		m_pServiceDispose->ReleaseData(pServerInfo);
		return bAllNonConnect;

		_MYTRACE(L"���еķ�������û��������!");
	}
	else
	{
		//
		for ( int32 i = m_aQuoteServerAll.GetSize() -1; i >= 0; i-- )
		{
			T_ServerInfo stServerInfo = m_aQuoteServerAll.GetAt(i);
			
			// �����
			int32 iID = m_pCommManager->GetCommunicationID(stServerInfo.wszAddr, stServerInfo.iPort);
			if ( iID == iCommunicationId )
			{
				m_aQuoteServerAll.RemoveAt(i);
				_MYTRACE(L"δ���ӷ����������޳�������: %d, ���� %d ��δ���ӳɹ�������", iID, m_aQuoteServerAll.GetSize());
				break;
			}
		}
	}

	//
	m_pServiceDispose->ReleaseData(pServerInfo);

	if ( 0 == m_aQuoteServerAll.GetSize() )
	{
		// ����������
		bAllConnect = true;
		_MYTRACE(L"���еķ���������������!");
	}

	if ( iCountServers == m_aQuoteServerAll.GetSize() )
	{
		// ��û������
		bAllNonConnect = true;
		_MYTRACE(L"���еķ�������û��������!");
	}

	// ȫ�����ɹ���, �����ȴ�
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
			CString *pStrTip = new CString(L"���������������ʱ, ��������,�Ժ��¼...");
			PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrTip, (LPARAM)enETOther);			
		}

		_MYTRACE(L"���� 60 ��, ����������������ӳ�ʱ, ��������,�Ժ��¼...!");
	}
	else
	{
		// ����		
		_MYTRACE(L"��ʼ��ѡ��վ");

		if (m_aQuoteServerAll.GetSize() > 0)
		{
			for ( int32 i = 0; i < m_aQuoteServerAll.GetSize(); i++ )
			{
				CString StrAdd = m_aQuoteServerAll.GetAt(i).wszAddr;
				int32 iPort	   = m_aQuoteServerAll.GetAt(i).iPort;
				int32 iCommID  = m_pCommManager->GetCommunicationID(StrAdd, iPort);

				_MYTRACE(L"ȥ�����ӳ�ʱ�����������: %d %s %s %d",
					iCommID, m_aQuoteServerAll.GetAt(i).wszName, StrAdd.GetBuffer(), iPort);
				StrAdd.ReleaseBuffer();

				// ֹͣ�������, ��ֹ�����������
				m_pCommManager->StopService(iCommID);
			}

			COptimizeServer::Instance()->DelNonConnectServer(m_aQuoteServerAll.GetData(), m_aQuoteServerAll.GetSize());
			m_aQuoteServerAll.RemoveAll();
		}
		
	//	pDoc->m_pDlgLogIn->SetPromptInfo(L"��ѡ��վ..."); 
		COptimizeServer::Instance()->SetViewData(this);
		bool32 bSortOK = COptimizeServer::Instance()->SortQuoteServers();
		if (bSortOK)
		{
			// ....
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
// ��Ѷ���
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
	// ���̱߳䵥�̴߳���
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
	// ȡ���ذ�
	CMmiNewsRespNode* pRespNode = m_pNewsManager->PeekAndRemoveResp();
	
	while( NULL != pRespNode )
	{
		if ( NULL != pRespNode->m_pMmiNewsResp )
		{
			switch (pRespNode->m_pMmiNewsResp->m_eNewsType)
			{
			case ENTRespInfoList:
				{
					// ������ѯ�б�
					OnNewsRespNewsList(pRespNode->m_iMmiRespId, (CMmiNewsRespNewsList*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTRespF10:
				{
					// ���� F10
					OnNewsRespF10(pRespNode->m_iMmiRespId, (CMmiNewsRespF10*)pRespNode->m_pMmiNewsResp);
				}	
				break;
			
			case  ENTRespLandMine:
				{
					// ������Ϣ����
					OnNewsRespLandMine(pRespNode->m_iMmiRespId, (CMmiNewsRespLandMine*)pRespNode->m_pMmiNewsResp);
				}
				break;
			case ENTRespCodeIndex:
				{
					// ���ش����Ӧ����Ѷҳ��
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

		// �ͷ���Դ
		DEL(pRespNode);

		// ȡ��һ��Ӧ���
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
	// �첽��ʽ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	if (m_bDelayQuote) ��֪����û������ע��
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

		if (GetJinpingID() == iIndex) // ����
		{
			m_listJinpingTitle.Copy(listTmp);
		}
		else if (KiIndexNormal == iIndex) // ��ͨ��Ѷ
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
			// ���±���
			for ( vector<CLandMineTitle>::iterator it = aTitles.begin(); it != aTitles.end(); ++it )
			{
				CLandMineTitle stLandMineTitle = *it;
				
				// �ҵ�����������ʱ����ͬ�ļ�¼
				if ( pMerch->m_mapLandMine.count(stLandMineTitle.m_StrUpdateTime) > 0 )
				{
					//RangeLandMine range = pMerch->m_mapLandMine.equal_range(stLandMineTitle.m_StrUpdateTime);
					mapLandMine::_Pairii range = pMerch->m_mapLandMine.equal_range(stLandMineTitle.m_StrUpdateTime);
					
					bool32 bSame = false;
					for( mapLandMine::iterator itRange = range.first; itRange != range.second; ++itRange )
					{						
						CLandMineTitle stNow = itRange->second;
						
						// �ж��б�title�Ƿ���ͬ
						if ( stNow.m_iInfoIndex == stLandMineTitle.m_iInfoIndex
							&& stNow.m_StrUpdateTime == stLandMineTitle.m_StrUpdateTime
							&& stNow.m_StrCrc32 == stLandMineTitle.m_StrCrc32 )
						{
							// �������������
							// ����ͬ�Ļ��͸���
							bSame = true;
							itRange->second = stLandMineTitle;		// �������������
							if ( stLandMineTitle.m_stContent.m_iConLen == 0 )	
							{
								itRange->second.m_stContent = stNow.m_stContent;	// �µ������ݵĻ������ݱ���ԭ����
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
			// ��������
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

	// XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(_T("����ļ����䳬ʱ��"), TRUE);
	

	// ������ڵ�¼�� ֪ͨ��¼��״̬�仯
	if (NULL != m_LoginWnd)
	{
		//pDoc->m_pDlgLogIn->StepProgress();
		// ʹ�ñ��صĳ�ʼ��
		CString StrPrompt;
		CString StrBlockFileName;
		bool32 bBlockOk = CBlockConfig::GetConfigFullPathName(StrBlockFileName);
		ASSERT( bBlockOk );
		StrPrompt.Format(L"��ʾ��ͬ������б���Ϣ��ʱ��ʹ�ñ��ذ���ļ���ʼ����");
		bBlockOk = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // ��ʼ������ļ� // ���û�й�Ʊ��Ϣ����ô�Ϳ��Բ���ͬ������б���
		ASSERT( bBlockOk );
	}

	OnAllMarketInitializeSuccess();
}



void CViewData::RequestStatusBarViewData()
{
	//ASSERT( 0 );	// δʵ�� - �Ѿ���pushʵ���� - ������Ʒ
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

	// �������������� 0, 1000
	CMmiReqGeneralNormal reqG;
	reqG.m_iMarketId = 0;
	RequestViewData(&reqG);
	reqG.m_iMarketId = 1000;
	RequestViewData(&reqG);
}

bool32 CViewData::GetMerchPowerValue( IN CMerch *pMerchStock, OUT float &fPowerValue )
{
	fPowerValue = 0.0;  // Ҫ���������ô����

	// ֻ���� �� �������г��µ���Ʒ
	if ( pMerchStock == NULL || NULL == pMerchStock->m_pRealtimePrice )
	{
		return false;
	}
	
	m_fPriceClose = pMerchStock->m_pRealtimePrice->m_fPricePrevClose;

	CMerch *pMerchIndexCmp = NULL;	// ָ���ο�
	const int32 iBreedId = pMerchStock->m_Market.m_Breed.m_iBreedId;
	if ( iBreedId == 0 )	// �� - ��ָ֤��
	{
		m_MerchManager.FindMerch(_T("000001"), 0, pMerchIndexCmp);
	}
	else if ( iBreedId == 1 )	// �� - ��֤��ָ
	{
		m_MerchManager.FindMerch(_T("399106"), 1000, pMerchIndexCmp);
	}

	if ( pMerchIndexCmp == NULL || pMerchIndexCmp->m_pRealtimePrice == NULL )
	{
		return false;
	}

	// ǿ����: �ǵ���-��ָ�ǵ�������֤��ȡ��֤��ָ����֤��ȡ��֤��ָ��
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
	//	��¼ʧ��, �ͷ��ص���ԭʼ��״̬, ������Ϣ���. ������֤, ���µ�¼
	//	��Ҫ��ԭ��¼���Ϊ:
	
	//	AuthSuccessMain ��������ʱ��ǰ�ȹص���ʱ��
	//	m_bLogWaitQuoteConnect, m_iWaitQuoteConnectCounts, m_aQuoteServerAll ��Щ��¼��Ϣ
	//	ɾ��MerchManager�����г�ʼ�����˵�Breed��Ϣ
	//	�޸�m_eLoginState״̬
	//	��0 DlgLogin��Progress
	//	Doc�е��˻���Ϣ���
	//	��ѡ�ų�
	//	ֹͣ�������ӵķ���
	//	(��OnAuthSuccessMain�еĳ�ʼ���û�Ȩ�ޣ��汾��Ϣ�Ƶ���ʼ���г���ɺ�)

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
	
	// �� COptimizeServer ע���Լ�
	COptimizeServer::Instance()->AddNotify(this);
	COptimizeServer::Instance()->SetViewData(this);

	// ����г�������Ϣ
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
		// ���г�
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

			// ���濪����ʱ��
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

			// ���������
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

		// TRACE(L" ������ʱ��: %d %d \n", stOpenCloseTime.m_iTimeOpen, stOpenCloseTime.m_iTimeClose);
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

		// TRACE(L"������--> %d \n", uiTime);
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
		StrErr = L"��ȡ�����ļ�·��ʧ��";
		return false;
 	}
	
	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		ASSERT(0);
		StrErr = L"װ�������ļ�ʧ��";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);
		StrErr = L"�����ļ���ȡʧ��";
		return false;
	}

	// �г�����
	DWORD dwMerchManagerType = EMMT_None;
	
	//
	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// ��ȡ breed ������				
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

		// �ٱ������Breed ������г�:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// �г�����
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
			
			// showid�ж���ʱȥ���� ��ά���ú����ж� cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0 */|| iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			// 
			// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
			//lint --e{429} 

			CMarket* pMarket = new CMarket(*pBreed);
			pMarket->m_MarketInfo.m_iMarketId	= iMarketID;
			pMarket->m_MarketInfo.m_iShowId		= iMarketShowID;
			pMarket->m_MarketInfo.m_iTimeZone	= iTimeZone;			
			pMarket->m_MarketInfo.m_StrCnName	= StrNameCn.c_str();
			pMarket->m_MarketInfo.m_StrEnName	= StrNameEn.c_str();
			
			// ȡ������ʱ��
			if ( !SplitMarketOCTimeString(StrOctime.c_str(), pMarket->m_MarketInfo.m_OpenCloseTimes) )
			{
				ASSERT(0);
				continue;
			}

			// ȡ���������
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

			// �ٱ�������г��������Ʒ:
			for ( TiXmlElement* pMerchElement = pMarketElement->FirstChildElement(); NULL != pMerchElement; pMerchElement = pMerchElement->NextSiblingElement() )
			{
				// ��Ʒ����
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

	// �ѻ���� OnAllMarketInitializeSuccess()
	CString StrBlockFileName;
	bool32 bBlockOk = CBlockConfig::GetConfigFullPathName(StrBlockFileName);
	ASSERT( bBlockOk );
	bBlockOk = CBlockConfig::Instance()->Initialize(StrBlockFileName);  // ��ʼ������ļ� // ���û�й�Ʊ��Ϣ����ô�Ϳ��Բ���ͬ������б���

	//
	m_eLoginState = ELSLoginSuccess;

	// ����ȫ����ʼ���ɹ�
	merchManager.SetInitializedAll();

	// ��ʼ����ݼ�
//	BuildMerchHotKeyList();
	
	// ������ҵ����ͼ����ǿ��ˢ���¼�
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
	//// ���浱ǰ���û�Ȩ��
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
	//// �ҵ���Ӧ�ڵ�
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

	//// �Ȱ����еĶ�ɾ����
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
	// ����ָ֤��Ϊ��׼
	CGmtTime TimeBegin,TimeEnd; 
	int32 iCount;
	
	CMerch* pMerch = NULL;
	if ( m_MerchManager.FindMerch(L"000001", 0, pMerch) && NULL != pMerch )
	{
		// �ҵ� 000001 ��ʱ����Ϊ��׼
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
	// 0-000001 1000-399001���ܻ�ȡ��Ӧ��
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

	// ���־
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

	// ����־
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
	
	// ���־
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
	
	// ����־
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
	_MYTRACE(L"�г� %d �����������", pMarket->m_MarketInfo.m_iMarketId);
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
		_MYTRACE(L"�����г�: %d ������", pMarket->m_MarketInfo.m_iMarketId);	
		InitializeMarket(*pMarket);
	}
}

void CViewData::DisconnectAllQuoteServer()
{
	m_pCommManager->StopAllService();

	m_aPushingRealtimePrices.clear();
	m_aPushingRealtimeTicks.clear(); 
	m_aPushingRealtimeLevel2s.clear();

	// �����������
}


//��ȡ������־  2013-7-23
bool CViewData::GetTodayFlag()
{
	return m_bTodayFalg;
}

void CViewData::SetTodayFlag(bool bFlag)
{
	m_bTodayFalg = bFlag;
}

//Im��Ϣ��־ 2013-10-31 add by cym
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
	// ֪ͨ���й��ĸ����ݵ�ҵ����ͼ
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
	// ֱ��������
	if (!LoadMarketOnDirectQuote())
	{
		return false;
	}
	
	// ����˽��Ŀ¼
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(m_strUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	
	// ��ȡ���������
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

	// �õ�����������б�
	T_ServerInfo stServer;
	wcscpy(stServer.wszAddr, stLoginBk.m_StrQuoterSvrAdd);
	stServer.iPort = stLoginBk.m_iQuoterSvrPort;

	m_aQuoteServerAll.Add(stServer);
	
	// ��������������������
	m_pCommManager->SetStage(ECSTAGE_LoginInit);
	m_pCommManager->InitDirectService(m_proxyInfo, m_strUserName, m_strPassword, stLoginBk.m_StrQuoterSvrAdd, stLoginBk.m_iQuoterSvrPort, aServiceIDs);
	m_pCommManager->StartAllService();

	// ���ñ�־, ������ʱ��
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
		StrErr = L"��ȡ�����ļ�·��ʧ��";
		return false;
 	}

	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		ASSERT(0);;
		StrErr = L"װ�������ļ�ʧ��";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);;
		StrErr = L"�����ļ���ȡʧ��";
		return false;
	}

	// �г�����
	//DWORD dwMerchManagerType = EMMT_None;
	
	// ��ʼ�����еĽ���Ʒ�ֺ�����������г�
	int32 iAllMarketCount = 0;
	int32 iInitializedMarketCount = 0;

	//
	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// ��ȡ breed ������				
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

		// �ٱ������Breed ������г�:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// �г�����
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
			
			// showid�ж���ʱȥ���� ��ά���ú����ж� cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0*/ || iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			int32 iPosMarket;
			CMarket* pTmp;
			if ( !pBreed->FindMarket(iMarketID, iPosMarket, pTmp) )
			{
				// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
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
	if ( 0 < errCode  && errCode < 90) //1-89 ��Դ��curl���巵�ش�����
	{
		return L"��Դ��curl����!";
	}
	
	if ( 99 < errCode && errCode < 506) //100-505��http����
	{
		CString strTmp;
		strTmp.Format(L"HTTP ����: %d", errCode);
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

// ������Ϣ�Ƿ񶼷�����
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

	// ��֤�ɹ�
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

		// ֪ͨ��¼�Ի��� ��ǰ״̬
		CString *pStrMsg = new CString(pszMsg);
		PostMessage(m_LoginWnd,UM_ViewData_OnAuthFail, (WPARAM)pStrMsg, (LPARAM)enETOther );
	}
}

void CViewData::OnRespUserRight(bool bSucc, IN const wchar_t *pszMsg)
{
	IsRespConfigComplete();

	// ��ȡ�û�Ȩ���б�
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

	// ��ȡȨ����ʾ����
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
		//	����ǰ�ˣ������ڴ���
		SendMessage(m_hBridgeWnd, UM_BridgeBegin, 0, 0);
		//	��������Ĵ��г����ݣ�������г�����û�У���ֱ���ñ����ѻ��г��ļ��������ѻ���¼
		{				
			//	
			GetNetBreedInfo(m_MerchManager);		
			if(0 != m_MerchManager.m_BreedListPtr.GetSize())
			{
				//	����������µĴ��г����ݣ���ֱ��������
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
				//	û�д��г����ݣ�ֱ�����ѻ���
				CString tError;
				GetOfflineFileData(m_MerchManager, tError);		//	�����ѻ����г�����	
				m_bBreedFromOfflineFile = true;
			}		
		}		
		//	����ǰ�ˣ����Ѿ����������
		SendMessage(m_hBridgeWnd, UM_BridgeEnd, 0, 0);

		//	���г����Ѿ�׼������
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
		if (0 != iStatus) // �ɹ�
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
	// ��ʱ����Ҫ��
	InitPush(false);

	// �����û���
	m_strUserName = StrUserName;	

	// ����˽��Ŀ¼
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(StrUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();

	// ������������
	m_pDataManager->ForceRequestType(EDSCommunication);

	vector<unsigned int> vtPort;
	//	������֤��������Ϊ���������г�������׼��
	if(!m_pAutherManager->GetInterface()->ConnectServer(tServerIP,vtPort))
	{
		ASSERT(0);
		return false;
	}

	// ����������г������Ϣ
	m_pAutherManager->GetInterface()->ReqQueryServerInfo(tKey, orgCode, 48, true, 1);


	CBlockConfig::Instance()->SetViewData(this);
	
	return true;
}
bool32 CViewData::OffLineInitial( IN CString& StrUserName, OUT CString& StrErr )
{
     StrErr.Empty();
 
      //���ñ�־
   /*  if ( !bOffLine )
     {
         return false;
     }
 */
     // Ĭ�ϵ��û���:
     //if ( !bLastUser || /*stUserInfo.m_aUserRights.GetSize() <= 0 ||*/ StrUserName.IsEmpty() )
     //{
     //    // ...fangz0107 ������
     //    ASSERT(0);
     //    return false;
     //}
 
     // ��������
    InitPush(false);
 
     // �����û���
     m_strUserName = StrUserName;
 
     // ��ʼ���û�Ȩ��	
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
 
     // ����˽��Ŀ¼
     CString StrPrivatePath;
     StrPrivatePath = CPathFactory::GetPrivateConfigPath(StrUserName);
     _tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
     StrPrivatePath.ReleaseBuffer();
     StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
     _tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
     StrPrivatePath.ReleaseBuffer();
 
     // ������������
     m_pDataManager->ForceRequestType(EDSOfflineData);

 	 CBlockConfig::Instance()->SetViewData(this);


	
      //�г��б���Щ��Ϣ�ĳ�ʼ��
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

	 //	��־ʹ�ñ����г��ļ�
	 m_bBreedFromOfflineFile = true;
	 //	�Ѿ�׼�����˴��г�����
	 m_bReadyBreed = true;	
    return true;
}

// ��ѡ��ͬ��
void CViewData::UploadUserBlock()
{
	// ��ȡ��������صİ�� ����������ذ������ ��ɶ�Ӧ��ʽJson��ʽ���ϴ�
	T_Block *pServerBlock = CUserBlockManager::Instance()->GetServerBlock();

	if ( NULL == pServerBlock )
	{
		return ;
	}

	// ����Json��ʽ����
	const int32 iMerchCount = pServerBlock->m_aMerchs.GetSize();
	string strMerchCode = "", strMarketId = "";
	CString StrToken = L"";
	// ��ȡtoken
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
	// �����ϴ��û���ѡ������
	 m_pAutherManager->GetInterface()->ReqSaveCusoptional(StrUserData, iDataType);
}

void CViewData::DownloadUserBlock()
{
	int32 iDataType = 1;		// ��ѡ������Ϊ1
	
	// ���������û���ѡ������
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
				TRACE(_T("��������ѡ�ɺ�����Ʒ: %d - %s\r\n"), iMarketId, StrCode);
			}
		}

		// ���û��Ĭ�ϵķ�������ѡ��飬���½��������޸�
		T_Block block;
		T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
		if ( NULL == pBlockServer )
		{
			block.m_eHeadType = ERTCustom;
			block.m_bServerBlock = true;		// �������йصİ��
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
				// ��Ʒ���ʱ��, �������, ֻ֪ͨ�ͱ����ļ�һ��.
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
			}
		}
		else
		{
			block = *pBlockServer;
			// ��Ʒ�б�ϲ�����ȡ�ޣ���ʱȡ�ϲ�
			bool32 bAdded = false;
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
				bAdded = bAdded || bAdd2;
			}
			if ( bAdded )
			{
				// ��Ʒ���ʱ��, �������, ֻ֪ͨ�ͱ����ļ�һ��.
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
	// ���д��г�
	T_BigMarketInfo* pBreedList = NULL;
	int iCountBreedList = 0;
	m_pServiceDispose->GetBigMarketList(&pBreedList, iCountBreedList);

	if ( 0 >= iCountBreedList )
	{
		//	��ֳ��Ϊ0���쳣��
		ASSERT(0);				
		return;
	}

	// ����С�г�
	T_MarketInfo* pMarketInfo = NULL;
	int iCountMarket;
	m_pServiceDispose->GetMarketList(&pMarketInfo, iCountMarket);

	if ( 0 >= iCountMarket )
	{
		//	û���г��������쳣��
		ASSERT(0);
		return;
	}

	DWORD dwMerchManagerType = 0;
	for ( int32 iIndexBreed = 0; iIndexBreed < iCountBreedList; ++iIndexBreed )
	{
		// ĳ������Ĵ��г�
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

			// ��Ӵ��г�
			merchManager.AddBreed(pBreed);
		}

		for ( int32 iIndexMarket = 0; iIndexMarket < iCountMarket; ++iIndexMarket )
		{
			if ( pMarketInfo[iIndexMarket].iBigMarket == stBreed.iBigMarket )
			{
				// ���øý���Ʒ���µ����������г�

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

	// �ͷ���Դ
	m_pServiceDispose->ReleaseData(pBreedList);

	// �ͷ���Դ
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

//	���ѻ��г��ļ��м������ݵ��ڴ���
bool32	CViewData::GetOfflineFileData(CMerchManager& merchManager, CString& StrErr)
{

	StrErr = L"";
	//
	CString strPath = CPathFactory::GetMarketInfoFilePath();

	if ( strPath.IsEmpty() )
	{
		ASSERT(0);
		StrErr = L"��ȡ�����ļ�·��ʧ��";
		return false;
	}

	std::string strPathA;
	Unicode2MultiChar(CP_ACP, strPath, strPathA);
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		//ASSERT(0);
		StrErr = L"װ�������ļ�ʧ��";
		return false;
	}

	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		ASSERT(0);
		StrErr = L"�����ļ���ȡʧ��";
		return false;
	}

	// �г�����
	DWORD dwMerchManagerType = EMMT_None;

	merchManager.Clear();

	for ( TiXmlElement* pBreedElement = pRoot->FirstChildElement(); NULL != pBreedElement; pBreedElement = pBreedElement->NextSiblingElement() )
	{
		// ��ȡ breed ������				
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

		// �ٱ������Breed ������г�:
		for ( TiXmlElement* pMarketElement = pBreedElement->FirstChildElement(); NULL != pMarketElement; pMarketElement = pMarketElement->NextSiblingElement() )
		{
			// �г�����
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

			// showid�ж���ʱȥ���� ��ά���ú����ж� cfj
			if ( iMarketID < 0 /*|| iMarketShowID < 0 */|| iTimeZone < 0 || iReportType < 0 || iVolScale < 0 || StrNameCn.length() <= 0 || StrOctime.length() <= 0 || StrActDay.length() <= 0 )
			{
				ASSERT(0);
				continue;
			}

			// 
			// ȫ���ͷŵ�ʱ����ͷţ��ں���������429����
			//lint --e{429} 

			CMarket* pMarket = new CMarket(*pBreed);
			pMarket->m_MarketInfo.m_iMarketId	= iMarketID;
			pMarket->m_MarketInfo.m_iShowId		= iMarketShowID;
			pMarket->m_MarketInfo.m_iTimeZone	= iTimeZone;			
			pMarket->m_MarketInfo.m_StrCnName	= StrNameCn.c_str();
			pMarket->m_MarketInfo.m_StrEnName	= StrNameEn.c_str();

			// ȡ������ʱ��
			if ( !SplitMarketOCTimeString(StrOctime.c_str(), pMarket->m_MarketInfo.m_OpenCloseTimes) )
			{
				ASSERT(0);
				continue;
			}

			// ȡ���������
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
			// �ٱ�������г��������Ʒ:
			for ( TiXmlElement* pMerchElement = pMarketElement->FirstChildElement(); NULL != pMerchElement; pMerchElement = pMerchElement->NextSiblingElement() )
			{
				// ��Ʒ����
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

			//pMarket->SetInitialized(true); ����Ҫ����true����¼��������У��һ��
		}
	}
	return true;
}

void	CViewData::InitPush(bool32 bEnable)
{
	// ��ʱ��
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
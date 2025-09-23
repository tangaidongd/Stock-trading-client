#include "StdAfx.h"
#include "CRecordDataCenter.h"
#include "sharestruct.h"
#include "tinyxml.h"
#include <io.h>
#include "coding.h"
#include "IoViewShare.h"
#include "IoViewTimeSale.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// һ������ĵȴ�ʱ��
static const int32 KiTimeOut					= 1000 * 60 * 2;		    // 2 ���ӻ�û�ذ�, ����һ��

static const int32 KiSaveDataGap				= 1000 * 60;				// 1 ���Ӽ�¼һ���������ڵ�

// һ�β������������
static const int32 KiRecordNumsPerTime			= 50;						

// ��־�ļ�
static const char* strFileLogPath				= "DbLog";

// ��ʱ��:
const int32 KiTimerIdSaveData					= 1000;						// �ж��Ƿ�������ʱ��
const int32 KiTimerPeriodSaveData				= 1000 * 1;					// 

// xml �ļ��ֶ�
static const CString KStrFileName				= L"./sqlconfig.xml";		// �����ļ���

static const char* KE_DataBase					= "DataBase";
static const char* KA_dbaddress					= "address";
static const char* KA_dbname					= "dbname";
static const char* KA_dbport					= "dbport";
static const char* KA_username					= "username";
static const char* KA_password					= "password";

static const char* KE_Merchs					= "Merchs";
static const char* KA_Code						= "code";
static const char* KA_Id						= "id";

static const char* KE_Circles					= "Circles";
static const char* KA_CirValue					= "value";
static const char* KA_ReqTime					= "reqtime";

static const char* KE_Mas						= "MA";
static const char* KA_MaValue					= "value";

//
static const int32 KiCircleMaxUserMin			= 500;
static const int32 KiCircleDay					= 1000;
static const int32 KiCircleWeek					= 2000;
static const int32 KiCircleMonth				= 3000;
static const int32 KiCircleYear					= 4000;

//
static const int32 KiMaMin						= 1;
static const int32 KiMaMax						= 200;

// ���ݿ����
CString KStrTableTick			= L"Ticks";
CString KStrTableReport			= L"Report";
CString KstrTableKLine			= L"KLine";

// ���ݿ��ֶ���
CString KStrCode				= L"code";
CString KStrName				= L"name";
CString KStrTime				= L"time";
CString KStrOpen				= L"priceopen";
CString KStrHigh				= L"pricehigh";
CString KStrLow					= L"pricelow";
CString KStrClose				= L"priceclose";
CString KStrPreClose			= L"pricepreclose";
CString KStrAvg					= L"priceavg";
CString KStrPreAvg				= L"pricepreavg";
CString KStrVolume				= L"volume";
CString KStrVolumeCur			= L"volumecur";
CString KStrHold				= L"hold";
CString KStrHoldDif				= L"holddif";
CString KStrBuyPrice1			= L"buyprice1";
CString KStrBuyVolume1			= L"buyvolume1";
CString KStrSellPrice1			= L"sellprice1";
CString KStrSellVolume1			= L"sellvolume1";
CString KStrProperty			= L"property";

CString KStrMA1					= L"ma1";
CString KStrMA2					= L"ma2";
CString KStrMA3					= L"ma3";

//
CRecordDataCenter::CRecordDataCenter(CAbsCenterManager* pAbsCenterManager)
{
 	ASSERT(NULL != pAbsCenterManager);
 	m_pAbsCenterManager = pAbsCenterManager;
 
	m_iReqID			= -100;
 	m_bStart			= false;						// �Ƿ�ʼ����
	
	m_bTimeToReqNext	= false;							
 	m_pDbProcess		= NULL;							// ���ݿ�
 	
	m_eErrType			= EETNone;						// ��������
	m_eWorkState		= EWSNone;

	m_dwDbPort			= 1433;

 	m_aMerchs.clear();
	m_mapMerchsForReq.clear();
	ZeroMemory(m_aMaParams, sizeof(m_aMaParams));
	m_aCircles.clear();

	m_b1Min				= false;
	m_bstMerchSaveValid	= false;	
 	m_hThreadReqData	= NULL;							// �����߳�
 	m_hThreadSaveData	= NULL;							// ȡ�����߳�
	m_hThreadTimerRecord= NULL;

	m_bJustProcessSelfCircle = false;
	m_bInitialSaveFinish	 = false;
	m_bInitialReqRealtime	 = true;

 	m_bThreadReqExit	= true;							// �����߳��˳���־
 	m_bThreadSaveExit	= true;							// ȡ�����߳��˳���־
	m_bThreadTimerRecordExit = true;	

	m_dwTimeSendReq		= (DWORD)-1;
	m_dwTimeRecord		= (DWORD)-1;

 	// ������ʱ����Ϊ�ļ���
	std::string sFileLogPath = GeneralFileName();
 	m_pFile	= fopen(sFileLogPath.c_str(), "wa+");
 	ASSERT(NULL != m_pFile);

	// ȫ��������
	m_eListenType = ECTCount;
	m_pAbsCenterManager->AddViewDataListner(this);
}

// ��Աָ�������stopWork�������Ѿ��ͷ�
//lint --e{1579}
CRecordDataCenter::~CRecordDataCenter()
{
	StopWork();
}

std::string CRecordDataCenter::GeneralFileName()
{
	// ������ʱ����Ϊ�ļ���
	time_t TimeNow = time(NULL);
	struct tm* pTimeNow = localtime(&TimeNow);
	
	//
	char strPath[MAX_PATH];
	memset(strPath, 0, MAX_PATH*sizeof(char));
	
	//
	sprintf(strPath, "%s_%04d%02d%02d%02d%02d%02d%s", strFileLogPath, pTimeNow->tm_year + 1900, pTimeNow->tm_mon, pTimeNow->tm_mday, pTimeNow->tm_hour, pTimeNow->tm_min, pTimeNow->tm_sec, ".txt");
	
	std::string StrPath = strPath;
	return StrPath;
}

CString GetNowLogTime()
{
	CString StrTime;
	CTime Time = CTime::GetCurrentTime();
	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", Time.GetHour(), Time.GetMinute(), Time.GetSecond());
	return StrTime;
}

bool32 CRecordDataCenter::BeValidCircle(int32 iCircle)
{
	if ( iCircle > 0 && iCircle <= KiCircleMaxUserMin )
	{
		return true;
	}

	if ( KiCircleDay == iCircle || KiCircleWeek == iCircle || KiCircleMonth == iCircle || KiCircleYear == iCircle )
	{
		return true;
	}

	return false;
}

void CRecordDataCenter::ValidMaParms(int32& iMA)
{
	if ( iMA <= 0 )
	{
		iMA = 0;
	}

	if ( iMA > KiMaMax )
	{
		iMA = KiMaMax;
	}
}

int32 CRecordDataCenter::GetMaxMa()
{
	RGUARD(LockSingle, m_LockaFields, LockaFields);

	int32 iReturn = -1;

	//
	for ( int32 i = 0; i < MA_NUMS; i++ )
	{		
		int32 iField = m_aMaParams[i];

		//		
		iReturn = iField > iReturn ? iField : iReturn;		
	}

	//
	return iReturn;
}

bool32 CRecordDataCenter::GetGmtTime(string StrTimeSrc, OUT CGmtTime& Time)
{
	Time = 0;

	// L"20110318112630" ���ָ�ʽ
	const char* StrExample = "20110318112630";
	const char* StrFix0	   = "0";

	const char* StrTime = StrTimeSrc.c_str();
	
	if ( 0 == strcmp(StrFix0, StrTime) )
	{
		CGmtTime time1(0);
		Time = time1;
		return true;
	}
	else if ( 1 == strcmp(StrFix0, StrTime) )
	{
		CGmtTime time1(-1);
		Time = time1;
		return true;
	}
	else 
	{
		if ( strlen(StrTime) != strlen(StrExample) )
		{
			DebugLog(L"ʱ�� %s ��ʽ�Ƿ�", StrTime);
			//ASSERT(0);
			return false;
		}
	}
	
	// ��
	char strYear[5] = {0};
	strncpy(strYear, StrTime, 4);
	int32 iYear = atoi(strYear);	
	
	if ( iYear <= 0 || iYear >= 3000 )
	{
		//ASSERT(0);
		return false;
	}
		
	// ��
	char strMonth[3] = {0};
	strncpy(strMonth, StrTime + 4, 2);
	int32 iMonth = atoi(strMonth);

	if ( iMonth <= 0 || iMonth >= 13 )
	{
		//ASSERT(0);
		return false;
	}

	// ��
	char strDay[3] = {0};
	strncpy(strDay, StrTime + 6, 2);
	int32 iDay = atoi(strDay);

	if ( 2 == iMonth )
	{
		if ( iDay <= 0 || iDay > 29 )
		{
			//ASSERT(0);
			return false;
		}
	}
	else
	{
		if ( iDay <= 0 || iDay > 31 )
		{
			//ASSERT(0);
			return false;
		}
	}

	// ʱ
	char strHour[3] = {0};
	strncpy(strHour, StrTime + 8, 2);
	int32 iHour = atoi(strHour);
	
	if ( iHour < 0 || iHour >= 24 )
	{
		//ASSERT(0);
		return false;
	}

	// ��
	char strMinute[3] = {0};
	strncpy(strMinute, StrTime + 10, 2);
	int32 iMinute = atoi(strMinute);
	
	if ( iMinute < 0 || iMinute >= 60 )
	{
		//ASSERT(0);
		return false; 
	}

	// ��
	char strSecond[3] = {0};
	strncpy(strSecond, StrTime + 12, 2);
	int32 iSecond = atoi(strSecond);
	
	if ( iSecond < 0 || iSecond >= 60 )
	{
		//ASSERT(0);
		return false;
	}

	//
	CGmtTime TimeXml(iYear, iMonth, iDay, iHour - 8, iMinute, iSecond);
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	
	if ( TimeNow < TimeXml )
	{
		// �����ڵ�ʱ�仹��
		return false;
	}

	//
	Time = TimeXml;

	return true;
}

bool32 CRecordDataCenter::GeneralRequests()
{
	m_aReqs.clear();	
	m_mapMerchsForReq.clear();
	m_mapMaxKLineCount.clear();

	// ����MA ����
	int32 iMaxMa = GetMaxMa();

	// ����ÿ�����ڵ�, ����ÿ������������Ҫ�����������
	map<E_KLineTypeBase, CMmiReqMerchKLine> aReqs;
	
	//
	for ( set<T_Circles>::iterator it = m_aCircles.begin(); it != m_aCircles.end(); ++it )
	{
		T_Circles stCircles = *it;
		
		//
		E_NodeTimeInterval eNodeTimeInterval = ENTICount;
		int32 iMinuteUser = 0;

		if ( 1 == stCircles.m_iCircle )
		{
			eNodeTimeInterval = ENTIMinute;
		}
		else if ( KiCircleDay == stCircles.m_iCircle )
		{
			eNodeTimeInterval = ENTIDay;
		}
		else if ( KiCircleWeek == stCircles.m_iCircle )
		{
			eNodeTimeInterval = ENTIWeek;
		}
		else if ( KiCircleMonth == stCircles.m_iCircle )
		{
			eNodeTimeInterval = ENTIMonth;
		}
		else if ( KiCircleYear == stCircles.m_iCircle )
		{
			eNodeTimeInterval = ENTIYear;
		}
		else if ( stCircles.m_iCircle > 0 && stCircles.m_iCircle <= KiCircleMaxUserMin )
		{
			eNodeTimeInterval = ENTIMinuteUser;
			iMinuteUser = stCircles.m_iCircle;
		}

		//
		E_NodeTimeInterval eCompare;
		E_KLineTypeBase eKLineBase;
		int32 iSacle;

		//
		if ( GetTimeIntervalInfo(iMinuteUser, 0, eNodeTimeInterval, eCompare, eKLineBase, iSacle) )
		{
			// ��¼ÿ��������Ҫ����� K ���� (���ڶ�ʱ��¼��ʱ��, ȡK ��)
			if ( iMaxMa > 0  )
			{
				m_mapMaxKLineCount[eKLineBase] = iSacle * iMaxMa;
			}
			else
			{
				m_mapMaxKLineCount[eKLineBase] = 1;
			}

			// ��������ṹ���ֵ
			it->m_eIntervalOrignal = eNodeTimeInterval;
			it->m_eKLineBase = eKLineBase;

			// ��ǰ���������:
			CMmiReqMerchKLine ReqNow;
	
			//
			DWORD TimeNow = stCircles.m_ReqTime.GetTime();
			DebugLog(L"��ǰ����Ļ��������� %d ��ǰ�������ʱ��: %d", eKLineBase, TimeNow);

			if ( (UINT)-1 == TimeNow )
			{
				ReqNow.m_eReqTimeType	= ERTYFrontCount;	
			}
			else if ( 0 == TimeNow )
			{
				ReqNow.m_eReqTimeType	= ERTYFrontCount;
			}
			else
			{
				ReqNow.m_eReqTimeType	= ERTYCount;
			}
			
			//
			ReqNow.m_eKLineTypeBase = eKLineBase;
			
			// �����ID ֵ��ʱ���������ʱ��ֵ
			ReqNow.m_TimeSpecify	= TimeNow;

			// �������ʱ�������ֵ
			ReqNow.m_iCount			= iSacle;

			
			// �ȿ��Ѿ����ڵ���������ʲô���				
			map<E_KLineTypeBase, CMmiReqMerchKLine>::iterator itFind = aReqs.find(eKLineBase);
			
			bool32 bExist = false;
			CMmiReqMerchKLine ReqExist;
			
			if ( aReqs.end() != itFind )
			{
				bExist = true;
				ReqExist = itFind->second;
			}

			//
			if ( (UINT)-1 == TimeNow || !bExist )
			{
				// ��ǰ�� -1 ��ʾҪ��������K �� 
				// ���� ����û�е�ʱ��������
				aReqs[eKLineBase] = ReqNow;

				if (!bExist)
				{
					DebugLog(L"����������޼�¼, ʹ�õ�ǰ��");
				}
				else
				{
					DebugLog(L"��ǰ�ķ�Χ���, ʹ�õ�ǰ��");
				}				
			}
			else if ( 0 == TimeNow )
			{
				// ��ǰ�� 0. ��С�ķ�Χ, ��ȡԭ������, ʲô�����ô���
				DebugLog(L"��ǰ���� 0 , ʹ��ԭ����");
			}
			else
			{
				DWORD TimeExist = ReqExist.m_TimeSpecify.GetTime();
				DebugLog(L"ԭ����ʱ����%d");

				// ��ǰ������һ�������ʱ��ֵ
				
				if ( (UINT)-1 == TimeExist )
				{
					// ԭ���ķ�Χ��, ���ô���
					DebugLog(L"ԭ���ķ�Χ���, ʹ��ԭ����");
				}
				else if ( 0 == TimeExist )
				{
					// ԭ����0, �����ڵ�
					aReqs[eKLineBase] = ReqNow;
					DebugLog(L"ԭ������0, ʹ�����ڵ�");
				}
				else
				{
					// ԭ��Ҳ��һ��ʱ��ֵ, �ǾͿ�˭��ʱ�����
					if ( TimeNow < TimeExist )
					{
						DebugLog(L"���ڵķ�Χ��ԭ���Ĵ�, ʹ�����ڵ�");
						aReqs[eKLineBase] = ReqNow;
					}
				}
			}
		}
		else
		{
			//ASSERT(0);
			DebugLog(L"��ȡ������Ϣʧ��!!!");
		}
	}

	if ( aReqs.empty() )
	{
		DebugLog(L"��������ʧ��");
		return false;
	}

	// �����������:
	DebugLog(L"�����������:");

	for ( map<E_KLineTypeBase, CMmiReqMerchKLine>::iterator itReq = aReqs.begin(); itReq != aReqs.end(); ++itReq )
	{
		CMmiReqMerchKLine st = itReq->second;
		m_aReqs.push_back(st);
		DebugLog(L"��������:%d ��������: %d ʱ���:%d ����: %d", st.m_eKLineTypeBase, st.m_eReqTimeType, st.m_TimeSpecify.GetTime(), st.m_iCount);
	}

	// ��־�����
	for ( map<E_KLineTypeBase, int32>::iterator itMaxCount = m_mapMaxKLineCount.begin(); itMaxCount != m_mapMaxKLineCount.end(); ++itMaxCount )
	{
		DebugLog(L"����: %d ��Ҫ�����K ����Ŀ: %d", itMaxCount->first, itMaxCount->second);
	}

	// ����Ĵ���Ʒ��Ϣ������:
	for ( set<CMerch*>::iterator itMerch = m_aMerchs.begin(); itMerch != m_aMerchs.end(); ++itMerch )
	{
		//
		CMerch* pMerch = *itMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
			
		ListKLineReqs ListReqs;

		// �����������:
		for ( vector<CMmiReqMerchKLine>::iterator itTmp = m_aReqs.begin(); itTmp != m_aReqs.end(); ++itTmp )
		{
			//
			CMmiReqMerchKLine Req = *itTmp;
			int32 iTime		= Req.m_TimeSpecify.GetTime();
			int32 iScale	= Req.m_iCount;

			//
			Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
			Req.m_iCount		= 0;

			if ( -1 == iTime )
			{
				Req.m_eReqTimeType = ERTYFrontCount;
				Req.m_TimeSpecify  = m_pAbsCenterManager->GetServerTime();
				Req.m_iFrontCount  = 10000;
			}
			else if ( 0 == iTime )
			{
				// ֻ�������������Ҫʹ�ñ���
				Req.m_eReqTimeType = ERTYFrontCount;
				Req.m_TimeSpecify  = m_pAbsCenterManager->GetServerTime();
				Req.m_iFrontCount  = 1;

				if ( iMaxMa > 0 )
				{
					Req.m_iFrontCount = iMaxMa * iScale;
				}
			}
			else
			{
				Req.m_eReqTimeType = ERTYSpecifyTime;
				Req.m_TimeStart	   = iTime;
				Req.m_TimeEnd	   = m_pAbsCenterManager->GetServerTime();
			}

			ListReqs.push_back(Req);
		}

		//
		m_mapMerchsForReq[pMerch] = ListReqs;
	}

	return true;
}

bool32 CRecordDataCenter::InitialFromFile()									
{
	DebugLog(L"��ʼ��ȡ�����ļ�");

	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	//
	CString StrFilePath = GetSqlConfigName();
	string sFilePath = _Unicode2MultiChar(StrFilePath);
	const char* strFilePath = sFilePath.c_str();
	TiXmlDocument Doc(strFilePath);
	if ( !Doc.LoadFile() )
	{		
		return false;		 
	}

	//
	TiXmlElement* pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		//ASSERT(0);
		return false;
	}

	// DataBase �ڵ�
	TiXmlElement* pElementDataBase = pRootElement->FirstChildElement();
	if ( NULL == pElementDataBase )
	{
		//ASSERT(0);
		return false;
	}

	// ��ַ
	const char* strValue = pElementDataBase->Attribute(KA_dbaddress);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrAddress;
	Utf8ToUnicode(strValue, WStrAddress);
	m_StrDbAddress = WStrAddress.c_str();
	
	// ���ݿ�����
	strValue = pElementDataBase->Attribute(KA_dbname);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrDbName;
	Utf8ToUnicode(strValue, WStrDbName);
	m_StrDbName = WStrDbName.c_str();

	// ���ݿ�˿�
	strValue = pElementDataBase->Attribute(KA_dbport);
	if ( NULL != strValue && strlen(strValue) > 0 )
	{		
		m_dwDbPort = atoi(strValue);
	}

	// �û���
	strValue = pElementDataBase->Attribute(KA_username);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrUserName;
	Utf8ToUnicode(strValue, WStrUserName);
	m_StrDbUserName = WStrUserName.c_str();
	
	// ����
	strValue = pElementDataBase->Attribute(KA_password);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrPassWord;
	Utf8ToUnicode(strValue, WStrPassWord);
	m_StrDbPassword = WStrPassWord.c_str();
	
	// ��Ʒ�ڵ�
	TiXmlElement* pElementMerch = pElementDataBase->NextSiblingElement();
	if ( NULL == pElementMerch )
	{
		//ASSERT(0);
		return false;
	}

	// �������Ʒ
	
	CArray<CSmartAttendMerch, CSmartAttendMerch&> aSmartAttendMerch;
	for ( TiXmlElement* pElementMerchNow = pElementMerch->FirstChildElement(); NULL != pElementMerchNow; pElementMerchNow = pElementMerchNow->NextSiblingElement() )
	{
		const char* strCode = pElementMerchNow->Attribute(KA_Code);
		if ( NULL == strCode || strlen(strCode) <= 0 )
		{
			continue;
		}

		//
		wstring WStrCode;
		Utf8ToUnicode(strCode, WStrCode);

		CString StrCode = WStrCode.c_str();

		const char* strId = pElementMerchNow->Attribute(KA_Id);
		if ( NULL == strId || strlen(strId) <= 0 )
		{
			continue;
		}

		int32 iID = atoi(strId);

		// �ҵ������Ʒ:
		CMerch* pMerch = NULL;
		m_pAbsCenterManager->GetMerchManager().FindMerch(StrCode, iID, pMerch);
		if ( NULL == pMerch )
		{
			DebugLog(L"%s ��Ʒû���ҵ�, ����...", StrCode.GetBuffer());
			continue;
		}

		// ����
		if ( 0 == m_aMerchs.count(pMerch) )
		{
			m_aMerchs.insert(pMerch);
			
			// ��ע��Ʒ
			CSmartAttendMerch stSmartAttendMerch;
			stSmartAttendMerch.m_bNeedNews = false;
			stSmartAttendMerch.m_iDataServiceTypes = 0;
			stSmartAttendMerch.m_pMerch = pMerch;
			
			//
			aSmartAttendMerch.Add(stSmartAttendMerch);
		}
		else
		{
			DebugLog(L"�ظ���Ʒ %s, ����", StrCode.GetBuffer());
		}
	}

	// ���ù�ע��Ʒ
	m_pAbsCenterManager->SetRecordDataCenterAttendMerchs(aSmartAttendMerch);
	

	// ����
	TiXmlElement* pElementCircles = pElementMerch->NextSiblingElement();
	if ( NULL == pElementCircles )
	{
		//ASSERT(0);
		return false;
	}

	for ( TiXmlElement* pElementCircleNow = pElementCircles->FirstChildElement(); NULL != pElementCircleNow; pElementCircleNow = pElementCircleNow->NextSiblingElement() )
	{
		const char* strCircle = pElementCircleNow->Attribute(KA_CirValue);
		if ( NULL == strCircle || strlen(strCircle) <= 0 )
		{
			continue;
		}

		//
		int32 iCircle = atoi(strCircle);
	
		// �ж��Ƿ�Ϸ�
		if ( !BeValidCircle(iCircle) )
		{
			DebugLog(L"���ڷǷ�: %d", iCircle);
			continue;
		}
		
		const char* strTime = pElementCircleNow->Attribute(KA_ReqTime);
		if ( NULL == strTime || strlen(strTime) <= 0 )
		{
			continue;
		}

		//
		T_Circles stSave;
		stSave.m_iCircle	= iCircle;
		if ( !GetGmtTime(string(strTime), stSave.m_ReqTime) )
		{
			continue;
		}
		
		//
		stSave.m_StrTableName.Format(L"%s%d", KstrTableKLine.GetBuffer(), stSave.m_iCircle);
		m_aCircles.insert(stSave);
		
		// �Ƿ��� 1 ���ӵ�����
		if ( 1 == iCircle )
		{
			m_b1Min = true;
		}

		// ������ʱ������ֶλ�ԭ�� 0 
		// ...fangz 0318 ���Ե�ʱ��ȥ���������
		pElementCircleNow->SetAttribute(KA_ReqTime, 0);	
	}

	// MA
	TiXmlElement* pElementMa = pElementCircles->NextSiblingElement();
	if ( NULL == pElementMa )
	{
		//ASSERT(0);
		return false;
	}

	int32 iMaIndex = 0;

	for ( TiXmlElement* pElementMaNow = pElementMa->FirstChildElement(); NULL != pElementMaNow; pElementMaNow = pElementMaNow->NextSiblingElement() )
	{
		// �������
		if ( iMaIndex >= MA_NUMS )
		{
			break;
		}

		const char* strMa = pElementMaNow->Attribute(KA_MaValue);
		if ( NULL == strMa || strlen(strMa) <= 0 )
		{		
			iMaIndex += 1;
			continue;
		}

		//
		int32 iMa = atoi(strMa);

		// �ж��Ƿ�Ϸ�, ����MA ������
		ValidMaParms(iMa);
				
		//
		m_aMaParams[iMaIndex] = iMa;
		iMaIndex += 1;
	}

	//
	Doc.SaveFile(strFilePath);

	return true;
}

bool32 CRecordDataCenter::StartWork()											
{
	CString StrDebugLog;
	DebugLog(L"��ʼ����");

	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	//
	if ( !InitialFromFile() )
	{
		DebugLog(L"��ȡ�����ļ�ʧ��");

		//ASSERT(0);
		return false;		
	}

	//
	if ( !GeneralRequests() )
	{
		DebugLog(L"������������ʧ��");
		//ASSERT(0);
		return false;
	}

	// ��ʱ��
	SetRecordDataCenterTimer(KiTimerIdSaveData, KiTimerPeriodSaveData);

	//
	if ( !CDataBase::InitComEnv() )
	{
		DebugLog(L"���ݿ��ʼ��ʧ��");
		PromptDbErrMsg();		
		return false;
	}

	DEL(m_pDbProcess);
	m_pDbProcess = new CDataBase();
	if ( NULL == m_pDbProcess )
	{
		//ASSERT(0);
		return false;
	}

	//
	m_pDbProcess->SetConnectionInfo(m_StrDbAddress, NULL, m_dwDbPort, m_StrDbName, m_StrDbUserName, m_StrDbPassword);
	
	if ( !m_pDbProcess->OpenConnection() )
	{
		DebugLog(L"���ݿ��ʼ��ʧ��");
		PromptDbErrMsg();
		return false;
	}

	//
	for ( set<T_Circles>::iterator it = m_aCircles.begin(); it != m_aCircles.end(); ++it )
	{
		T_Circles stCircle = *it;
		if ( -1 == stCircle.m_ReqTime.GetTime() )
		{
			TruncateTableContent(stCircle.m_StrTableName);
			DebugLog(L"����ʱ��Ϊ -1, ��ɾ��������������. ȫ����д.");
		}
	}
	
	//
	m_bStart = true;
	m_bTimeToReqNext = true;
	
	//
	if ( !BeginReqDataThread() )
	{
		DebugLog(L"���������߳�ʧ��!");
		//ASSERT(0);		
		return false;			
	}
	
	//
	if ( !BeginSaveDataThread() )
	{
		DebugLog(L"�������ݿ��¼�߳�ʧ��!");
		//ASSERT(0);
		return false;
	}
	
	if ( !BeginTimerRecordThread() )
	{
		DebugLog(L"������ʱ��¼�����߳�ʧ��!");
		//ASSERT(0);
		return false;
	}

	//
	DebugLog(L"��ʼ����!");
	return true;
}

bool32 CRecordDataCenter::StopWork()													
{
	//
	m_bStart = false;
	
	//
	m_bThreadReqExit  = true;
	m_bThreadSaveExit  = true;
	m_bThreadTimerRecordExit = true;

	// �����߳�
	if ( NULL != m_hThreadReqData )	
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadReqData, INFINITE) )
		{
			// �˳���
		}	
	}
	
	// �����߳�
	if ( NULL != m_hThreadSaveData )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadSaveData, INFINITE) )
		{
			// �˳���
		}
	}

	// ��ʱ��¼�߳�
	if ( NULL != m_hThreadTimerRecord )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadTimerRecord, INFINITE) )
		{
			// �˳���
		}
	}

	//
	StopRecordDataCenterTimer(KiTimerIdSaveData);

	// ���ݿ�
	DEL(m_pDbProcess);
	CDataBase::UnitComEnv();

	// ���	
	m_eErrType			= EETNone;				
	m_bstMerchSaveValid	= false;	
	m_pAbsCenterManager->RemoveAttendMerch(EA_RecordData);
	m_pAbsCenterManager->DelViewDataListner(this);

	m_bJustProcessSelfCircle = false;
	m_bInitialSaveFinish	 = false;
	m_bInitialReqRealtime	 = true;

	m_aMerchs.clear();
	m_mapMerchsForReq.clear();
	ZeroMemory(m_aMaParams, sizeof(m_aMaParams));
	m_aCircles.clear();	
	m_b1Min				= false;

	//
	DEL_HANDLE(m_hThreadReqData);							
	DEL_HANDLE(m_hThreadSaveData);							
	DEL_HANDLE(m_hThreadTimerRecord);

	m_dwTimeSendReq		= (DWORD)-1;
	m_dwTimeRecord		= (DWORD)-1;

	DebugLog(L"ֹͣ����");

	return true;
}

void CRecordDataCenter::PromptDbErrMsg()
{
 	CString StrMsg = m_pDbProcess->GetComErrorDescribe();
 	::MessageBox(GetActiveWindow(), StrMsg, L"�Ƹ����", MB_ICONWARNING);	
}

CString CRecordDataCenter::GetSqlConfigName()
{
	return KStrFileName;
}

bool32 CRecordDataCenter::BeginReqDataThread()
{
	m_bThreadReqExit = false;

	m_hThreadReqData = CreateThread(NULL, 0, ThreadRequestData, this, 0, NULL);
	
	if ( NULL == m_hThreadReqData )
	{
		return false;
	}
	
	DebugLog(L"�������������߳�");

	return true;
}

DWORD WINAPI CRecordDataCenter::ThreadRequestData(LPVOID pParam)
{
	CRecordDataCenter* pThis = (CRecordDataCenter*)pParam;
	
	if ( NULL != pThis )
	{
		while ( !pThis->m_bThreadReqExit )
		{
			if ( !pThis->m_bStart )
			{
				break;
			}

			//
			bool32 bFinish = pThis->ThreadRequestData();
			
			if ( bFinish )
			{
				if ( !pThis->m_mapMerchsForReq.empty() )
				{
					// ������
					pThis->DebugLog(L"�������. �˳�!!!");				
				}
				
				// �����, �������
				{
					RGUARD(LockSingle, pThis->m_LockMerchsForReq, LockMerchsForReq);
					pThis->m_mapMerchsForReq.clear();

					pThis->SetWorkState(EWSNone);
				}
			}

			Sleep(100);
		}		
	}

	return 0;
}

bool32 CRecordDataCenter::ThreadRequestData()
{
	if ( !m_bTimeToReqNext )
	{
		// û��ʱ��
		return false;
	}

	// ���Ͽ���, ��д��������Ǳߴ�
	m_bTimeToReqNext = false;

	//
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return true;
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsDataManager )
	{
		//ASSERT(0);
		return true;
	}

	// �����Tick ���ݿ���һ��ʼ����������
	if ( m_bInitialReqRealtime )
	{
		// �����յ���������, ȡ���ռ�
		CMmiReqRealtimePrice ReqRealtimePrice;
	
		// ����ʵʱTick
		CMmiReqRealtimeTick ReqTick;

		//
		for ( set<CMerch*>::iterator itMerch = m_aMerchs.begin(); itMerch != m_aMerchs.end(); ++itMerch )
		{
			CMerch* pMerchNow = *itMerch;
			if ( NULL == pMerchNow )
			{
				continue;
			}

			//
			if ( itMerch == m_aMerchs.begin() )
			{
				ReqRealtimePrice.m_iMarketId	= pMerchNow->m_MerchInfo.m_iMarketId;
				ReqRealtimePrice.m_StrMerchCode	= pMerchNow->m_MerchInfo.m_StrMerchCode;
				
				//
				ReqTick.m_iMarketId				= pMerchNow->m_MerchInfo.m_iMarketId;
				ReqTick.m_StrMerchCode			= pMerchNow->m_MerchInfo.m_StrMerchCode;
			}
			else
			{
				CMerchKey MerchKey;

				MerchKey.m_iMarketId	= pMerchNow->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode	= pMerchNow->m_MerchInfo.m_StrMerchCode;

				//
				ReqRealtimePrice.m_aMerchMore.Add(MerchKey);
				ReqTick.m_aMerchMore.Add(MerchKey);
			}
		}
	
		//
		m_pAbsCenterManager->RequestViewData(&ReqRealtimePrice);
		m_pAbsCenterManager->RequestViewData(&ReqTick);		

		m_bInitialReqRealtime = false;
	}

	//
	CMerch* pMerchReq = NULL;
	CMmiReqMerchKLine stReq;

	if ( GetRequest(pMerchReq, stReq) )
	{
		// ȡ����, û��Ʒ��
		return true;
	}
	else
	{
		if ( NULL == pMerchReq )
		{
			//ASSERT(0);
			m_bTimeToReqNext = true;
			return false;
		}

		// ���� K ��
		CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
		if ( pDoc->m_pAbsDataManager->RequestData(&stReq, aMmiReqNodes, EDSCommunication) )
		{
			if ( 1 == aMmiReqNodes.GetSize() )
			{
				m_iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;
			}
			else
			{
				//ASSERT(0);
				return true;
			}
		}
		else
		{
			// 
			DebugLog(L"����K ��ʧ��!");
			//ASSERT(0);
			return true;
		}
		
		//
		m_dwTimeSendReq = timeGetTime();
		DebugLog(L"���� %s �ĵ���K ������, ʱ�� %d", pMerchReq->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_dwTimeSendReq);
	}

	return false;
}

bool32 CRecordDataCenter::BeginSaveDataThread()
{
	m_bThreadSaveExit = false;
	
	m_hThreadSaveData = CreateThread(NULL, 0, ThreadSaveData, this, 0, NULL);
	
	if ( NULL == m_hThreadSaveData )
	{
		return false;
	}
	
	DebugLog(L"����д���ݿ��߳�");

	return true;
}

DWORD WINAPI CRecordDataCenter::ThreadSaveData(LPVOID pParam)
{
	CRecordDataCenter* pThis = (CRecordDataCenter*)pParam;
	
	if ( NULL != pThis )
	{
		while ( !pThis->m_bThreadSaveExit )
		{
			if ( !pThis->m_bStart ) 
			{
				break;
			}

			pThis->ThreadSaveData();
			Sleep(100);	
		}		
	}

	return 0;
}

bool32 CRecordDataCenter::ThreadSaveData()
{
	/*
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	CMerchKLineNode stMerchSave;
	if ( !GetMerchDataToSave(stMerchSave) )
	{
		return false;
	}
	
	int32 iKLineSize = stMerchSave.m_KLines.GetSize();

	CMerch* pMerch = NULL;
	m_pAbsCenterManager->GetMerchManager().FindMerch(stMerchSave.m_StrMerchCode, stMerchSave.m_iMarketId, pMerch);
	
	if ( NULL == pMerch || iKLineSize <= 0 )
	{
		if ( NULL != pMerch )
		{
			DebugLog(L"%s ��ƷK ������Ϊ��, ����", stMerchSave.m_StrMerchCode);
		}
		else
		{
			DebugLog(L"%d %s ��Ʒ������, ����", stMerchSave.m_iMarketId, stMerchSave.m_StrMerchCode);
		}
		
		//
		ClearLoacalSaveMerchData();
	
		// ��ʼ������һ����Ʒ
		m_bTimeToReqNext = true;

		// //ASSERT(0);		
		return false;
	}
		 
	// ��¼����
	CKLine* pKLine = stMerchSave.m_KLines.GetData();
	E_KLineTypeBase eKLineBase = stMerchSave.m_eKLineTypeBase;

	//
	for ( set<T_Circles>::iterator it = m_aCircles.begin(); it != m_aCircles.end(); ++ it )
	{
		T_Circles st = *it;

		// ������������ڵ�, ��K ��, ��ָ��, д���ݿ�
		if ( eKLineBase == st.m_eKLineBase )
		{
			if ( m_bJustProcessSelfCircle )
			{
				// ���ֻ�������ָ�����ڵ�, ��ô����Ҫ�����ж�, ���Ƿ����������������Ϊ�������Զ�������, �ǵĻ���pass , ������
				bool32 bSelfCircle = false;
				
				E_NodeTimeInterval eCompareNow;
				E_KLineTypeBase eKLineBaseNow;
				int32 iSacleNow;
				
				//
				if ( GetTimeIntervalInfo(st.m_iCircle, 0, st.m_eIntervalOrignal, eCompareNow, eKLineBaseNow, iSacleNow) )
				{
					if ( st.m_eIntervalOrignal == eCompareNow && 1 == iSacleNow )
					{
						bSelfCircle = true;	
					}
				}

				//
				if ( !bSelfCircle )
				{
					//
					DebugLog(L"%d %d %d��ָ������, Pass... ", st.m_iCircle, (int32)st.m_eIntervalOrignal, (int32)st.m_eKLineBase);
					continue;
				}
			}
				 
			// 
			CArray<CKLine, CKLine> aKLinesSave;
			CKLine* pKLineSave = NULL;
			int32 iSizeSave = 0;

			//
			switch (st.m_eIntervalOrignal)
			{
			case ENTIMinute:
			case ENTIMinute5:
			case ENTIMinute60:
			case ENTIDay:
			case ENTIMonth:
				{			
					aKLinesSave.SetSize(iKLineSize);
					pKLineSave = (CKLine*)aKLinesSave.GetData();
					memcpyex(pKLineSave, pKLine, sizeof(CKLine) * iKLineSize);					
					
					iSizeSave  = iKLineSize;
				}
				break;					
			case ENTIMinute15:
				{
					if (!CMerchKLineNode::CombinMinuteN(pMerch->m_Market.m_MarketInfo.m_iTimeInitializePerDay, 
						pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes, 15,
						stMerchSave.m_KLines, aKLinesSave))
					{
						//ASSERT(0);
					}
					
					pKLineSave = (CKLine*)aKLinesSave.GetData();
					iSizeSave  = aKLinesSave.GetSize();
				}
				break;
			case ENTIMinute30:
				{
					if (!CMerchKLineNode::CombinMinuteN(pMerch->m_Market.m_MarketInfo.m_iTimeInitializePerDay, 
						pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes, 30, 
						stMerchSave.m_KLines, aKLinesSave))
					{
						//ASSERT(0);
					}		

					pKLineSave = (CKLine*)aKLinesSave.GetData();
					iSizeSave  = aKLinesSave.GetSize();
				}
				break;
			case ENTIMinuteUser:
				{					
					if (!CMerchKLineNode::CombinMinuteN(pMerch->m_Market.m_MarketInfo.m_iTimeInitializePerDay, 
						pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes, st.m_iCircle, 
						stMerchSave.m_KLines, aKLinesSave))
					{
						//ASSERT(0);
					}

					pKLineSave = (CKLine*)aKLinesSave.GetData();
					iSizeSave  = aKLinesSave.GetSize();
				}	
				break;
			case ENTIWeek:
				{
					if (!CMerchKLineNode::CombinWeek(stMerchSave.m_KLines, aKLinesSave))
					{
						//ASSERT(0);
					}

					pKLineSave = (CKLine*)aKLinesSave.GetData();
					iSizeSave  = aKLinesSave.GetSize();
				}
				break;
			case ENTIYear:
				{
					if (!CMerchKLineNode::CombinMonthN(stMerchSave.m_KLines, 12, aKLinesSave))
					{
						//ASSERT(0);
					}
				
					pKLineSave = (CKLine*)aKLinesSave.GetData();
					iSizeSave  = aKLinesSave.GetSize();
				}
				break;
			default:
				//ASSERT(0);
				break;
			}	

			// K ��׼������, ���費��Ҫ�� MA
			vector<ArrayFloat> aMaResults;

			//
			for ( int32 iIndexMa = 0; iIndexMa < MA_NUMS; ++iIndexMa )
			{
				int32 iMaParam = m_aMaParams[iIndexMa];

				//
				ArrayFloat aResults(iSizeSave, 0.);				
				float* pResults = (float*)&(*aResults.begin());

				//
				if ( 0 != iMaParam )
				{
					//
					if ( !MA_Array(pKLineSave, iSizeSave, iMaParam, pResults) )
					{
						DebugLog(L"����%s ����%d MA%d ʧ��", pMerch->m_MerchInfo.m_StrMerchCode, st.m_iCircle, iMaParam);
						continue;
					}
					
					// ���������
					DebugLog(L"����%s ����%d MA%d �ɹ�", pMerch->m_MerchInfo.m_StrMerchCode, st.m_iCircle, iMaParam);
				}
				
				if ( m_bInitialSaveFinish )
				{
					// ʵʱ��¼�������ݵ�ʱ��, ֻҪ����һ���Ϳ�����					
					float fMaLast = *aResults.rbegin();
					aResults.clear();
					aResults.push_back(fMaLast);
				}

				aMaResults.push_back(aResults);
			}

			// ��ʼ��¼���ݿ�:
			bool32 bNeedJudgeSame = true;
			
			if ( m_bInitialSaveFinish )
			{
				bNeedJudgeSame = true;
			}
			else
			{
				if ( -1 == st.m_ReqTime.GetTime() )
				{
					bNeedJudgeSame = false;
				}
			}

			//
			if ( m_bInitialSaveFinish )
			{
				// ֻȡ����һ��������
				CKLine KLineLast = aKLinesSave.GetAt(iSizeSave - 1);
				aKLinesSave.RemoveAll();
				aKLinesSave.Add(KLineLast);

				iSizeSave = aKLinesSave.GetSize();
			}
			
			//
			DebugLog(L"��¼K �� %d ��, �ж��ظ� = %d", iSizeSave, bNeedJudgeSame);
			
			CString StrTableName = st.m_StrTableName;			
			SaveKLineToDB(pMerch, StrTableName, aKLinesSave, aMaResults, bNeedJudgeSame);			
		}		
	}
	
	// ��ձ���
	ClearLoacalSaveMerchData();

	// ������һ��
	m_bTimeToReqNext = true;

	//
	if ( m_mapMerchsForReq.empty() && !m_bInitialSaveFinish )
	{
		// 
		DebugLog(L"��ʼ����¼�������!!!");
		m_bInitialSaveFinish = true;
	}
	*/
	return true;
}

bool32 CRecordDataCenter::BeginTimerRecordThread()
{
	m_bThreadTimerRecordExit = false;
	
	m_hThreadTimerRecord = CreateThread(NULL, 0, ThreadTimerRecord, this, 0, NULL);
	
	if ( NULL == m_hThreadTimerRecord )
	{
		return false;
	}
	
	DebugLog(L"������ʱ��¼���ݿ��߳�");

	return true;
}

DWORD WINAPI CRecordDataCenter::ThreadTimerRecord(LPVOID pParam)
{
	CRecordDataCenter* pThis = (CRecordDataCenter*)pParam;
	
	if ( NULL != pThis )
	{
		while ( !pThis->m_bThreadSaveExit )
		{
			if ( !pThis->m_bStart ) 
			{
				break;
			}

			pThis->ThreadTimerRecord();
			Sleep(100);	
		}		
	}

	return 0;
}

bool32 CRecordDataCenter::ThreadTimerRecord()
{
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	if ( !m_bInitialSaveFinish )
	{
		// ��ʼ������û���
		return false;
	}

	// �ж��Ƿ��˼�¼ʱ��
	DWORD dwTimeNow = timeGetTime();
	if ( dwTimeNow - m_dwTimeRecord < KiSaveDataGap )
	{
		return false;
	}

	// ��ʼ�����¼����
	for ( set<CMerch*>::iterator itMerch = m_aMerchs.begin(); itMerch != m_aMerchs.end(); ++itMerch )
	{
		CMerch* pMerch = *itMerch;

		if ( NULL == pMerch )
		{
			continue;
		}

		//
		DebugLog(L"��ʼ��ʱ��¼%s ������", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		
		for ( set<T_Circles>::iterator itCircle = m_aCircles.begin(); itCircle != m_aCircles.end(); ++itCircle )
		{
			T_Circles stCircle = *itCircle;
			
			DebugLog(L"����Ϊ%d %d", stCircle.m_iCircle, stCircle.m_eKLineBase);

			// ȡ��������
			E_KLineTypeBase eKLineBase = stCircle.m_eKLineBase;
			
			// ����������������Ҫ�� K ����
			map<E_KLineTypeBase, int32>::iterator itFind = m_mapMaxKLineCount.find(eKLineBase);
			if ( m_mapMaxKLineCount.end() == itFind )
			{
				// 
				DebugLog(L"û���ҵ�������ڵ�����0!!!");				
				continue;			
			}
			
			//
			int32 iNeedCount = itFind->second;
			
			//
			int32 iPosFind;
			CMerchKLineNode* pMerchKLineNode = NULL;
			pMerch->FindMerchKLineNode(eKLineBase, iPosFind, pMerchKLineNode);
			
			//
			if ( NULL == pMerchKLineNode || iPosFind < 0 )
			{
				DebugLog(L"û���ҵ�������ڵ�����1!!!");
				continue;
			}

			//
			int32 iSrcCount = pMerchKLineNode->m_KLines.GetSize();			
			CKLine* pKLineSrc = (CKLine*)pMerchKLineNode->m_KLines.GetData();
			
			if ( iSrcCount <= 0 || NULL == pKLineSrc )
			{
				DebugLog(L"û���ҵ�������ڵ�����2!!!");
				continue;
			}
			
			// �������Ҫ���µ����������� K ��
			if ( iNeedCount > iSrcCount )
			{
				iNeedCount = iSrcCount;
			}
			
			if ( iNeedCount <= 0 )
			{
				DebugLog(L"û���ҵ�������ڵ�����3!!!");
				//ASSERT(0);
				continue;
			}
		
			//
			CArray<CKLine, CKLine> aKLineNeed;
			aKLineNeed.SetSize(iNeedCount);
			CKLine* pKLineNeed = (CKLine*)aKLineNeed.GetData();
			
			// ��������
			int32 iIndexBegin = iSrcCount - iNeedCount;
			memcpyex(pKLineNeed, pKLineSrc + iIndexBegin, sizeof(CKLine) * iNeedCount);
			
			// ���� MA
			vector<ArrayFloat> aMaResults;
			
			//
			for ( int32 iIndexMa = 0; iIndexMa < MA_NUMS; ++iIndexMa )
			{
				int32 iMaParam = m_aMaParams[iIndexMa];
				
				//
				ArrayFloat aResults(iNeedCount, 0.);				
				float* pResults = (float*)&(*aResults.begin());
				
				//
				if ( 0 != iMaParam )
				{
					//
					if ( !MA_Array(pKLineNeed, iNeedCount, iMaParam, pResults) )
					{
						DebugLog(L"����%s ����%d MA%d ʧ��", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stCircle.m_iCircle, iMaParam);
						continue;
					}
					
					// ���������
					DebugLog(L"����%s ����%d MA%d �ɹ�", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stCircle.m_iCircle, iMaParam);
				}

				// ֻȡ���һ��ֵ:
				float fMaLast = *aResults.rbegin();
				aResults.clear();
				aResults.push_back(fMaLast);

				//
				aMaResults.push_back(aResults);
			}
			
			// ��ʼ��¼���ݿ�:
			CString StrTableName = stCircle.m_StrTableName;			

			// ֻȡ����һ��������
			CKLine KLineLast = aKLineNeed.GetAt(iNeedCount - 1);
			aKLineNeed.RemoveAll();
			aKLineNeed.Add(KLineLast);

			//
			DebugLog(L"����ɹ�, д�����ݿ�");

			// ��¼
			SaveKLineToDB(pMerch, StrTableName, aKLineNeed, aMaResults, true);
		}
	}
	
	//
	m_dwTimeRecord = timeGetTime();

	//
	return true;
}

void CRecordDataCenter::SetRecordDataCenterTimer(int32 iTimerId, int32 iTimerPeriod)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	
	if ( NULL != pMainFrame )
	{
		pMainFrame->SetTimer(iTimerId, iTimerPeriod, NULL);
	}
}

void CRecordDataCenter::StopRecordDataCenterTimer(int32 iTimerId)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	
	if ( NULL != pMainFrame )
	{
		pMainFrame->KillTimer(iTimerId);
	}
}

void CRecordDataCenter::OnRecordDataCenterTimer(int32 iTimerId)
{
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
	}

	//
	if ( KiTimerIdSaveData == iTimerId )
	{
		// �ж�һ����û�г�ʱ��
		CheckTimeOut();
	}
}

void CRecordDataCenter::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}

	//
	CMerch* pMerch = NULL;
	m_pAbsCenterManager->GetMerchManager().FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch);

	if ( NULL == pMerch )
	{
		return;
	}

	//
	set<CMerch*>::iterator itFind = m_aMerchs.find(pMerch);
	if ( m_aMerchs.end() == itFind )
	{
		return;
	}

	// ��¼�����ݿ���
	bool32 bRerocdInDb = BeRealtimePriceInDB(pMerch);

	if ( !bRerocdInDb )
	{
		// ����һ��
		CString StrSql = GeneralRealtimePriceInsertSql(pMerch);

		if ( m_pDbProcess->Execute(StrSql, NULL) )
		{
			DebugLog(L"����һ�������¼: %s %s �ɹ�", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
		else
		{
			DebugLog(L"����һ�������¼: %s %s ʧ��", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
	}
	else
	{
		// ����
		CString StrSql = GeneralRealtimePriceUpdateSql(pMerch);
		
		bool32 bTest = false;
		if ( bTest )
		{
			TestRecordSqlToFile(StrSql, "c:\\realtimeupdate.txt");
		}
		
		if ( m_pDbProcess->Execute(StrSql, NULL) )
		{
			DebugLog(L"����һ�������¼: %s %s �ɹ�", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
		else
		{
			DebugLog(L"����һ�������¼: %s %s ʧ��", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
	}
}

void CRecordDataCenter::OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}
	
	//
	CMerch* pMerch = NULL;
	m_pAbsCenterManager->GetMerchManager().FindMerch(MmiPushTick.m_StrMerchCode, MmiPushTick.m_iMarketId, pMerch);
	
	if ( NULL == pMerch || NULL == pMerch->m_pMerchTimeSales )
	{
		//ASSERT(0);
		return;
	}
	
	//
	set<CMerch*>::iterator itFind = m_aMerchs.find(pMerch);
	if ( m_aMerchs.end() == itFind )
	{
		return;
	}

	//
	for ( int32 i = 0; i < MmiPushTick.m_Ticks.GetSize(); i++ )
	{
		int32 iPos = pMerch->m_pMerchTimeSales->QuickFindTickByTime(pMerch->m_pMerchTimeSales->m_Ticks, MmiPushTick.m_Ticks[i].m_TimeCurrent, false);

		if ( iPos < 0 )
		{
			DebugLog(L"δ�ҵ��˷ֱʼ�¼:%s %s", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), 
				GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer());
			continue;
		}

		if ( 0 == iPos )
		{
			// ��һ��, �޷�����
			continue;
		}

		//
		CTick TickPre = pMerch->m_pMerchTimeSales->m_Ticks.GetAt(iPos - 1);

		CShowTick Tick;
		Tick = pMerch->m_pMerchTimeSales->m_Ticks.GetAt(iPos);

		Tick.m_TimePrev			= TickPre.m_TimeCurrent;
		Tick.m_fHoldTotalPrev	= TickPre.m_fHold;
		Tick.Calculate();

		//
		CString StrSQL = GeneralTickInsertSql(pMerch, Tick);

		if ( m_pDbProcess->Execute(StrSQL, NULL) )
		{
			DebugLog(L"�ɹ���¼�ֱ�����: %s %s %.2f %.2f %s", 
				     pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
					 GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer(), 
					 Tick.m_fPrice, Tick.m_fHold - Tick.m_fHoldTotalPrev, Tick.m_StrProperty.GetBuffer());
		}
		else
		{
			DebugLog(L"��¼�ֱ�����ʧ��: %s %s", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer());
		}
	}

	// ��ʼ���Ĺ����Ѿ����, ������ʵʱ��������K �߸���
	if ( m_bInitialSaveFinish && m_b1Min ) 
	{
		// 1 ���� K ��, ʵʱ�������ݿ�
		{
			// Դ K ������
			int32 iPos;
			CMerchKLineNode* pKLineNode;
			if ( !pMerch->FindMerchKLineNode(EKTBMinute, iPos, pKLineNode) || iPos < 0 )
			{
				DebugLog(L"û���ҵ�ԴK ������, ���������Ӧ�÷���!");
				//ASSERT(0);
				return;
			}
			
			int32 iSrcCount = pKLineNode->m_KLines.GetSize();
			CKLine* pKLineSrc = (CKLine*)pKLineNode->m_KLines.GetData();
			
			if ( iSrcCount <= 0 || NULL == pKLineSrc )
			{
				return;
			}
			
			// �������Ҫ���µ����������� K ��
			int32 iNeedCount = 1;
			
			int32 iMaxMa = GetMaxMa();
			if ( -1 != iMaxMa )
			{
				iNeedCount = iMaxMa;
			}
			
			//
			if ( iNeedCount > iSrcCount )
			{
				iNeedCount = iSrcCount;
			}
			
			if ( iNeedCount <= 0 )
			{
				//ASSERT(0);
				return;
			}
			
			//
			DebugLog(L"ʵʱ����1 ���� K ��: ���MA �Ĳ���Ϊ %d. ����K �������� %d ��, ȡ���µ� %d ��", iMaxMa, iSrcCount, iNeedCount);
			
			//
			CMerchKLineNode MerchKLineNode;
			MerchKLineNode = *pKLineNode;
			MerchKLineNode.m_KLines.SetSize(iNeedCount);
			CKLine* pKLineNeed = (CKLine*)MerchKLineNode.m_KLines.GetData();
			
			// ��������
			int32 iIndexBegin = iSrcCount - iNeedCount;
			memcpyex(pKLineNeed, pKLineSrc + iIndexBegin, sizeof(CKLine) * iNeedCount);
			
			// ���ò���, �����̼߳�¼���ݿ�
			SetSaveMerchData(MerchKLineNode, true);
		}
	}
}

void CRecordDataCenter::OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine)
{
	if ( NULL == pMmiRespMerchKLine || NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return;
	}

	// ��һ�ε�ʱ��������K ������, ����� K �߻�����:
	if ( iMmiReqId == m_iReqID )
	{
		// ��¼���ݿ�
		SetSaveMerchData(pMmiRespMerchKLine->m_MerchKLineNode, false);

		// �������ʱ���־��λ
		m_dwTimeSendReq = (DWORD)-1;
	}
}

void CRecordDataCenter::SetSaveMerchData(const CMerchKLineNode& stMerchSave, bool32 bJustProcessSelfCircle)
{
	RGUARD(LockSingle, m_LockstMerchSave, LockstMerchSave);
	RGUARD(LockSingle, m_LockFlagstMerchSaveValid, LockFlagstMerchSaveValid);
	RGUARD(LockSingle, m_LockJustProcessSelfCircle, LockJustProcessSelfCircle);
	
	//
	m_bstMerchSaveValid = true;
	m_bJustProcessSelfCircle = bJustProcessSelfCircle;
	m_stMerchSave = stMerchSave;

	//
	// DebugLog(L"�յ� %s �� K ������: %d ��", stMerchSave.m_StrMerchCode, stMerchSave.m_KLines.GetSize());
}

bool32 CRecordDataCenter::GetMerchDataToSave(OUT CMerchKLineNode& stMerchSave)
{
	RGUARD(LockSingle, m_LockstMerchSave, LockstMerchSave);
	RGUARD(LockSingle, m_LockFlagstMerchSaveValid, LockFlagstMerchSaveValid);

	if ( !m_bstMerchSaveValid )
	{
		return false;
	}

	stMerchSave = m_stMerchSave;
	return true;
}

void CRecordDataCenter::ClearLoacalSaveMerchData()
{
	RGUARD(LockSingle, m_LockstMerchSave, LockstMerchSave);
	RGUARD(LockSingle, m_LockFlagstMerchSaveValid, LockFlagstMerchSaveValid);

	m_bstMerchSaveValid				= false;

	//
	m_stMerchSave.m_iMarketId		= -1;
	m_stMerchSave.m_eKLineTypeBase	= EKTBCount;
	m_stMerchSave.m_uiTailTime		= 0;
	
	m_stMerchSave.m_KLines.SetSize(0, 100);
}

CGmtTime CRecordDataCenter::GetSaveTime(const CString& StrTime)
{
	if ( StrTime.GetLength() <= 0 || NULL == m_pAbsCenterManager)
	{
		return 0;
	}

	//
	CString StrTmp = StrTime;
	StrTmp.TrimLeft();
	StrTmp.TrimRight();

	//
	CString StrHour = StrTime.Left(2);
	CString StrMin	= StrTime.Right(2);

	//
	string sHour = _Unicode2MultiChar(StrHour);
	const char* strHour = sHour.c_str();
	int32 iHour	  = 0;
	if (strHour != NULL)
	{
		iHour = atoi(strHour);
	}

	string sMin = _Unicode2MultiChar(StrMin);
	const char* strMin  = sMin.c_str();
	int32 iMin	  = 0;
	if (strMin != NULL)
	{
		iMin = atoi(strMin);
	}

	//
	if ( iHour < 0 || iHour >= 24 || iMin < 0 || iMin >= 60 )
	{
		return 0;
	}

	//
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	SaveDay(TimeNow);

	//
	iHour -= 8;

	//
	CGmtTimeSpan TimeSpan(0, iHour, iMin, 0);
	CGmtTime TimeReturn = TimeNow + TimeSpan;

	//
	return TimeReturn;
}

bool32 CRecordDataCenter::GetRequest(OUT CMerch*& pMerch, OUT CMmiReqMerchKLine& stReq)
{
	RGUARD(LockSingle, m_LockMerchsForReq, LockMerchsForReq);

	if ( !m_mapMerchsForReq.empty() )
	{
		for ( mapMerchReqs::iterator it = m_mapMerchsForReq.begin(); it != m_mapMerchsForReq.end(); ++it )
		{
			CMerch* pMerchNow = it->first;
			if ( NULL == pMerchNow )
			{
				//ASSERT(0);
				continue;
			}

			//
			ListKLineReqs ListReqs = it->second;
		
			//
			if ( ListReqs.empty() )
			{
				//ASSERT(0);				
				continue;
			}
			
			// ��Ʒ
			pMerch = pMerchNow;

			// ����, ȡ�б��һ��			
			stReq = *ListReqs.begin();

			// ���±�������
			ListReqs.pop_front();
			m_mapMerchsForReq[pMerch] = ListReqs;

			//
			if ( ListReqs.empty() )
			{
				m_mapMerchsForReq.erase(it);
			}

			DebugLog(L"ȡ������, ��Ʒ: %s ����: %s", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stReq.GetSummary().GetBuffer());

			return false;
		}
	}

	return true;
}

CString CRecordDataCenter::GetKLineTimeString(const CKLine& KLine)
{
	CString StrTime;
	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", 
					KLine.m_TimeCurrent.GetYear(), KLine.m_TimeCurrent.GetMonth(), KLine.m_TimeCurrent.GetDay(),
					KLine.m_TimeCurrent.GetHour() + 8, KLine.m_TimeCurrent.GetMinute(), KLine.m_TimeCurrent.GetSecond());

	return StrTime;
}

CString CRecordDataCenter::GetTimeString(const CGmtTime& Time, bool32 bOnlyDay /*= false*/)
{
	CString StrTime;

	if ( bOnlyDay )
	{
		StrTime.Format(L"%04d-%02d-%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay());				   
	}
	else
	{
		StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d",
				   Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour() + 8, Time.GetMinute(), Time.GetSecond());
	}
	
	return StrTime;
}

CString CRecordDataCenter::GeneralKLineUpdateSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3)
{
	if ( NULL == pMerch )
	{
		//ASSERT(0);
		return L"";
	}
	
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

	CString StrPriceOpen	= Float2String(KLine.m_fPriceOpen,  iSaveDec, false, false);		
	CString StrPriceHigh	= Float2String(KLine.m_fPriceHigh,  iSaveDec, false, false);
	CString StrPriceLow		= Float2String(KLine.m_fPriceLow,   iSaveDec, false, false);
	CString StrPriceClose	= Float2String(KLine.m_fPriceClose, iSaveDec, false, false);
	CString StrPriceAvg		= Float2String(KLine.m_fPriceAvg,	iSaveDec, false, false);

	CString StrVolumn		= Float2String(KLine.m_fVolume,		iSaveDec, false, false);
	CString StrHold			= Float2String(KLine.m_fHold,		iSaveDec, false, false);
		
	CString StrTime			= GetKLineTimeString(KLine);
	CString StrMA1			= Float2String(fMa1, iSaveDec, false, false);
	CString StrMA2			= Float2String(fMa2, iSaveDec, false, false);
	CString StrMA3			= Float2String(fMa3, iSaveDec, false, false);

	CString	StrTable = StrTableName;
	CString StrSQL;
	StrSQL.Format(L"update %s set %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s', %s = '%s', %s = '%s', %s = '%s' where %s = '%s' and %s = '%s' and %s = '%s'",
				  StrTable.GetBuffer(), 
				  KStrOpen.GetBuffer(), StrPriceOpen.GetBuffer(),
				  KStrHigh.GetBuffer(), StrPriceHigh.GetBuffer(),
				  KStrLow.GetBuffer(), StrPriceLow.GetBuffer(),
				  KStrClose.GetBuffer(), StrPriceClose.GetBuffer(),
				  KStrAvg.GetBuffer(), StrPriceAvg.GetBuffer(),
				  KStrVolume.GetBuffer(), StrVolumn.GetBuffer(),
				  KStrHold.GetBuffer(), StrHold.GetBuffer(),
				  KStrMA1.GetBuffer(), StrMA1.GetBuffer(),
				  KStrMA2.GetBuffer(), StrMA2.GetBuffer(),
				  KStrMA3.GetBuffer(), StrMA3.GetBuffer(),
				  KStrCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
				  KStrName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(),
				  KStrTime.GetBuffer(), StrTime.GetBuffer());

	//
	return StrSQL;	
}

bool32 CRecordDataCenter::BeKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine)
{
	if ( NULL == pMerch || NULL == m_pDbProcess )
	{
		return false;
	}

	//
	CString StrSQL = QueryKLineSql(StrTableName, pMerch, KLine);
	CRecordSet QuerySet;
	if ( !m_pDbProcess->Execute(StrSQL, &QuerySet) )
	{
		DebugLog(L"��ѯ %s �� K ������ %s ʧ��!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
		return false;
	}

	if ( QuerySet.GetRecordCount() > 0 )
	{
		DebugLog(L"��ѯ���� %s �� K ������ %s �� %d ����¼", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer(), QuerySet.GetRecordCount());
		return true;
	}

	//
	return false;
}

void CRecordDataCenter::UpdateKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3)
{
	if ( NULL == pMerch || NULL == m_pDbProcess )
	{
		return;
	}

	//
	CString StrSQL = GeneralKLineUpdateSql(StrTableName, pMerch, KLine, fMa1, fMa2, fMa3);

	if ( !m_pDbProcess->Execute(StrSQL, NULL) )
	{
		DebugLog(L"���� %s �� K ������ %s ʧ��!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
	}

	DebugLog(L"���� %s �� K ������ %s �ɹ�!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
}

void CRecordDataCenter::SetWorkState(E_WorkState eState)
{
	RGUARD(LockSingle, m_LockWorkState, LockWorkState);
	m_eWorkState = eState;
}

CRecordDataCenter::E_WorkState CRecordDataCenter::GetWorkState()
{
	RGUARD(LockSingle, m_LockWorkState, LockWorkState);
	return m_eWorkState;
}

bool32 CRecordDataCenter::TruncateTableContent(const CString& StrTableName)
{
	CString StrSql;
	CString StrName = StrTableName;
	StrSql.Format(L"TRUNCATE TABLE %s", StrName.GetBuffer());
	
	return (m_pDbProcess->Execute(StrSql, NULL));	
}

void CRecordDataCenter::CheckTimeOut()
{
	DWORD dwTimeNow = timeGetTime();

	if ( ((UINT)-1 != m_dwTimeSendReq) && (dwTimeNow - m_dwTimeSendReq >= KiTimeOut) )
	{
		//
		DebugLog(L"��ǰʱ��:%d ������ʱ��: %d, �Ѿ���ʱ... �����������, ������һ����Ʒ");
	
		m_dwTimeSendReq = dwTimeNow;
		m_bTimeToReqNext = true;
	}
}

bool32 CRecordDataCenter::MA_Array(IN CKLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float*& pfResult)
{
	//
	if ( NULL == pDataSrc || iCountSrc <= 0 || iParam <= 0 )
	{	
		return false;
	}
	
	//
	for ( int i = 0; i < iCountSrc; i++ )
	{
		int iParamNow = iParam;
		int iCountNow = i + 1;
		
		if ( iParamNow > iCountNow )
		{
			iParamNow = iCountNow;
		}
		
		//
		int   iIndex = i;
		int	  iLoop  = iParamNow;
		float fSum   = 0.;
		
		while(iLoop > 0)
		{
			fSum += pDataSrc[iIndex].m_fPriceClose;
			
			iIndex--;
			iLoop--;
		}
		
		pfResult[i] = fSum / iParamNow;
	}
	
	return true;
}

void CRecordDataCenter::DebugLog(LPCTSTR format, ...)
{
#ifndef _DEBUG
	return;
#endif
	
	//
	RGUARD(LockSingle, m_LockDebugLog, LockDebugLog);
	
	//
	if ( NULL == m_pFile )
	{
		return;
	}
	
	va_list args;
	TCHAR buffer[1024];
	memset(buffer, 0, sizeof(char) * 1024);
	
	va_start( args, format );
	vwprintf(format, args); 
	vswprintf(buffer, format, args);

	//
	USES_CONVERSION;	
	CString StrLog;
	StrLog.Format(L"%s: %s\r\n", GetNowLogTime().GetBuffer(), buffer);
	
	TRACE(StrLog);

	fprintf(m_pFile, "%s", _Unicode2MultiChar(StrLog).c_str());
	fflush(m_pFile);
}

CString	CRecordDataCenter::GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults)
{
	if ( NULL == pMerch || aKLines.GetSize() <= 0 || aMaResults.size() != MA_NUMS )
	{
		//ASSERT(0);
		return L"";
	}

	int32 i;
	for ( i = 0; i < MA_NUMS; i++ )
	{
		if ( aMaResults[i].size() != aKLines.GetSize() )
		{
			//ASSERT(0);
			return L"";
		}
	}

	//
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	
	//
	CString StrTab = StrTableName;
	CString StrSQL;
	StrSQL.Format(L"insert into %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values",
				  StrTab.GetBuffer(), KStrCode.GetBuffer(), KStrName.GetBuffer(), KStrTime.GetBuffer(), 
				  KStrOpen.GetBuffer(), KStrHigh.GetBuffer(), KStrLow.GetBuffer(), KStrClose.GetBuffer(), KStrAvg.GetBuffer(), 
				  KStrVolume.GetBuffer(), KStrHold.GetBuffer(), KStrMA1.GetBuffer(), KStrMA2.GetBuffer(), KStrMA3.GetBuffer());

	// K ������
	CKLine* pKLine = (CKLine*)aKLines.GetData();

	// Ma ����
	float*  pMA1   = (float*)&(*aMaResults[0].begin());
	float*  pMA2   = (float*)&(*aMaResults[1].begin());
	float*  pMA3   = (float*)&(*aMaResults[2].begin());

	//
	for ( i = 0; i < aKLines.GetSize(); i++ )
	{
		CString StrKLine;

		CString StrPriceOpen	= Float2String(pKLine[i].m_fPriceOpen,  iSaveDec, false, false);		
		CString StrPriceHigh	= Float2String(pKLine[i].m_fPriceHigh,  iSaveDec, false, false);
		CString StrPriceLow		= Float2String(pKLine[i].m_fPriceLow,   iSaveDec, false, false);
		CString StrPriceClose	= Float2String(pKLine[i].m_fPriceClose, iSaveDec, false, false);
		CString StrPriceAvg		= Float2String(pKLine[i].m_fPriceAvg,	iSaveDec, false, false);

		CString StrVolumn		= Float2String(pKLine[i].m_fVolume,		iSaveDec, false, false);
		CString StrHold			= Float2String(pKLine[i].m_fHold,		iSaveDec, false, false);

		CString StrTime			= GetKLineTimeString(pKLine[i]);
		CString StrMA1			= Float2String(pMA1[i], iSaveDec, false, false);
		CString StrMA2			= Float2String(pMA2[i], iSaveDec, false, false);
		CString StrMA3			= Float2String(pMA3[i], iSaveDec, false, false);
		
		StrKLine.Format(L"('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",	
						pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
						pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(),
						StrTime.GetBuffer(), StrPriceOpen.GetBuffer(), StrPriceHigh.GetBuffer(), StrPriceLow.GetBuffer(), 
						StrPriceClose.GetBuffer(), StrPriceAvg.GetBuffer(), StrVolumn.GetBuffer(), StrHold.GetBuffer(),
						StrMA1.GetBuffer(), StrMA2.GetBuffer(), StrMA3.GetBuffer());
		
		StrSQL += StrKLine;					
		if ( i != aKLines.GetSize() - 1 )
		{
			StrSQL += L",";
		}
	}
	
	return StrSQL;
}

CString CRecordDataCenter::GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3)
{
	if ( NULL == pMerch )
	{
		//ASSERT(0);
		return L"";
	}
		
	//
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	
	//
	CString StrTable = StrTableName;
	CString StrSQL;
	StrSQL.Format(L"insert into %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values",
		StrTable.GetBuffer(), KStrCode.GetBuffer(), KStrName.GetBuffer(), KStrTime.GetBuffer(), 
		KStrOpen.GetBuffer(), KStrHigh.GetBuffer(), KStrLow.GetBuffer(), KStrClose.GetBuffer(), KStrAvg.GetBuffer(), 
		KStrVolume.GetBuffer(), KStrHold.GetBuffer(), KStrMA1.GetBuffer(), KStrMA2.GetBuffer(), KStrMA3.GetBuffer());

	CString StrKLine;
	CString StrPriceOpen	= Float2String(KLine.m_fPriceOpen,  iSaveDec, false, false);		
	CString StrPriceHigh	= Float2String(KLine.m_fPriceHigh,  iSaveDec, false, false);
	CString StrPriceLow		= Float2String(KLine.m_fPriceLow,   iSaveDec, false, false);
	CString StrPriceClose	= Float2String(KLine.m_fPriceClose, iSaveDec, false, false);
	CString StrPriceAvg		= Float2String(KLine.m_fPriceAvg,	iSaveDec, false, false);
	
	CString StrVolumn		= Float2String(KLine.m_fVolume,		iSaveDec, false, false);
	CString StrHold			= Float2String(KLine.m_fHold,		iSaveDec, false, false);
	
	CString StrTime			= GetKLineTimeString(KLine);
	CString StrMA1			= Float2String(fMa1, iSaveDec, false, false);
	CString StrMA2			= Float2String(fMa2, iSaveDec, false, false);
	CString StrMA3			= Float2String(fMa3, iSaveDec, false, false);
	
	StrKLine.Format(L"('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",	
		pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
		pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(),
		StrTime.GetBuffer(), StrPriceOpen.GetBuffer(), StrPriceHigh.GetBuffer(), StrPriceLow.GetBuffer(), StrPriceClose.GetBuffer(), 
		StrPriceAvg.GetBuffer(), StrVolumn.GetBuffer(), StrHold.GetBuffer(),StrMA1.GetBuffer(), StrMA2.GetBuffer(), StrMA3.GetBuffer());
	
	StrSQL += StrKLine;					

	return StrSQL;
}

CString CRecordDataCenter::QueryKLineSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine)
{
	if ( NULL == pMerch )
	{
		//ASSERT(0);
		return L"";	
	}
	
	CString StrTime = GetKLineTimeString(KLine);
	
	//
	CString StrTable = StrTableName;
	CString StrSQL;
	StrSQL.Format(L"select * from %s where %s = '%s' and %s = '%s'",
		StrTable.GetBuffer(), 
		KStrCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
		KStrTime.GetBuffer(), StrTime.GetBuffer());
	
	return StrSQL;
}

bool32 CRecordDataCenter::SaveKLineToDB(CMerch* pMerch, const CString& StrTableName, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults, bool32 bNeedJudgeSame /*= false*/)
{
	if ( NULL == pMerch || StrTableName.GetLength() <= 0 || aKLines.GetSize() <= 0 || aMaResults.empty() )
	{
		DebugLog(L"����¼�����ݷǷ�, ��¼ʧ��...");
		//ASSERT(0);
		return false;
	}

	//
	int32 iKLineSize = aKLines.GetSize();
	CKLine* pKLine = (CKLine*)aKLines.GetData();
	int32 iIndex = 0;
	
	if ( !bNeedJudgeSame )
	{
		// ����Ҫ�ж��ظ�����
		while( iIndex < iKLineSize )
		{
			int32 iBegin = iIndex;
			int32 iEnd	 = iBegin + KiRecordNumsPerTime - 1;
			
			if ( iEnd >= iKLineSize )
			{
				iEnd = iKLineSize - 1;
			}
			
			int32 iCountsNow = iEnd - iBegin + 1;
			iIndex = iEnd + 1;
			
			//
			CArray<CKLine, CKLine> aKLineNow;
			aKLineNow.SetSize(iCountsNow);
			
			CKLine* pKLineNow = aKLineNow.GetData();		
			memcpyex(pKLineNow, pKLine + iBegin, iCountsNow * sizeof(CKLine));
			
			vector<ArrayFloat> aMaResultsNow;
			for ( int32 i = 0; i < MA_NUMS; i++ )
			{
				//
				ArrayFloat aTmp(iCountsNow, 0.);
				float* pMaNow = &(*(aTmp.begin()));
				const float* pMaSrc = &(*(aMaResults[i].begin()));
				memcpyex(pMaNow, pMaSrc, iCountsNow * sizeof(float));

				aMaResultsNow.push_back(aTmp);
			}
			
			//
		//	DWORD time1 = timeGetTime();
			CString StrSql = GeneralKLineInsertSql(StrTableName, pMerch, aKLineNow, aMaResultsNow);
		//	DWORD time2 = timeGetTime();
			
			bool32 btest = false;
			if ( btest )
			{
				TestRecordSqlToFile(StrSql, "c:\\insertkline.txt");
			}

			// TRACE(L"         ===����SQL ����ʱ: %d ms=== \n", time2 - time1);
			
			//DWORD time3 = timeGetTime();
			
			if ( !m_pDbProcess->Execute(StrSql, NULL) )
			{								
				DebugLog(L"%s\n ��¼���ݿ�ʧ��!!!", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
				PromptDbErrMsg();
				continue;
			}	
			
			//DWORD time4 = timeGetTime();
			// TRACE(L"         ===��¼���ݿ��ʱ: %d ms=== \n", time4 - time3);	
		} 
	}
	else
	{
		// ��Ҫ�ж��ظ�, ֻ��һ������д��
		for ( int32 i = 0; i < iKLineSize; i++ )
		{
			//
			bool32 bKLineInDB = BeKlineInDB(StrTableName, pMerch, pKLine[i]);

			if ( bKLineInDB )
			{
				// ����
				DebugLog(L"%s %s %s K �ߴ���, ׼������...", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), GetKLineTimeString(pKLine[i]).GetBuffer());
				UpdateKlineInDB(StrTableName, pMerch, pKLine[i], aMaResults[0][i], aMaResults[1][i], aMaResults[2][i]);
			}
			else
			{
				// ����
				DebugLog(L"%s %s %s K �߲�����, ׼������...", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), GetKLineTimeString(pKLine[i]).GetBuffer());
				CString StrSQL = GeneralKLineInsertSql(StrTableName, pMerch, pKLine[i], aMaResults[0][i], aMaResults[1][i], aMaResults[2][i]);

				if ( m_pDbProcess->Execute(StrSQL, NULL) )
				{
					DebugLog(L"����ɹ�!");
				}
				else
				{
					DebugLog(L"����ʧ��!");
				}
			}
		}
	}

	return true;
}

CString CRecordDataCenter::QueryRealtimePriceSql(CMerch* pMerch)
{
	// ���ơ����롢�����ߡ��͡����¡��ɽ������ֲ����������̡�����ۡ�����㡢���֡��ֲ��һ����һ������һ������һ��ʱ�� 

	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return L"";
	}

	//
	CGmtTime TimePrice = pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time;
	CString StrTime = GetTimeString(TimePrice);
	
	//
	CString StrSQL;
	StrSQL.Format(L"select * from %s where %s = '%s'",
		KStrTableReport.GetBuffer(), 
		KStrCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());		
	
	return StrSQL;
}

bool32 CRecordDataCenter::BeRealtimePriceInDB(CMerch* pMerch)
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return false;
	}

	//
	CString StrQuery = QueryRealtimePriceSql(pMerch);
	if ( StrQuery.IsEmpty() )
	{
		DebugLog(L"���������ѯ���ʧ��!");
		return false;
	}

	//
	CRecordSet QuerySet;
	if ( !m_pDbProcess->Execute(StrQuery, &QuerySet) )
	{
		DebugLog(L"��ѯ %s �� ��������ʧ��!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		return false;
	}
	
	if ( QuerySet.GetRecordCount() > 0 )
	{
		DebugLog(L"��ѯ���� %s ���������ݹ� %d ����¼", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), QuerySet.GetRecordCount());
		return true;
	}

	//
	return false;
}

CString CRecordDataCenter::GeneralRealtimePriceInsertSql(CMerch* pMerch)
{
	// ���ơ����롢�����ߡ��͡����¡��ɽ������ֲ����������̡�����ۡ�����㡢���֡��ֲ��һ����һ������һ������һ��ʱ�� 
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return L"";
	}

	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	
	CString StrPriceOpen	= Float2String(pMerch->m_pRealtimePrice->m_fPriceOpen, iSaveDec, false, false);
	CString StrPriceHigh	= Float2String(pMerch->m_pRealtimePrice->m_fPriceHigh, iSaveDec, false, false);
	CString StrPriceLow		= Float2String(pMerch->m_pRealtimePrice->m_fPriceLow, iSaveDec, false, false);	
	CString StrPriceClose	= Float2String(pMerch->m_pRealtimePrice->m_fPriceNew, iSaveDec, false, false);
	CString StrVolumn		= Float2String(pMerch->m_pRealtimePrice->m_fVolumeTotal, iSaveDec, false, false);
	CString StrHold			= Float2String(pMerch->m_pRealtimePrice->m_fHoldTotal, iSaveDec, false, false);
	CString StrPreClose		= Float2String(pMerch->m_pRealtimePrice->m_fPricePrevClose, iSaveDec, false, false);
	CString StrAvg			= Float2String(pMerch->m_pRealtimePrice->m_fPriceAvg, iSaveDec, false, false);
	CString StrPreAvg		= Float2String(pMerch->m_pRealtimePrice->m_fPricePrevAvg, iSaveDec, false, false);
	CString StrVolumnCur	= Float2String(pMerch->m_pRealtimePrice->m_fVolumeCur, iSaveDec, false, false);
	CString StrHoldDif		= Float2String(pMerch->m_pRealtimePrice->m_fHoldTotal - pMerch->m_pRealtimePrice->m_fHoldPrev, iSaveDec, false, false);
	CString StrBuyPrice1	= Float2String(pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, iSaveDec, false, false);
	CString StrBuyVolume1	= Float2String(pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fVolume, iSaveDec, false, false);
	CString StrSellPrice1	= Float2String(pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, iSaveDec, false, false);
	CString StrSellVolume1	= Float2String(pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fVolume, iSaveDec, false, false);

	CGmtTime TimePrice = pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time;
	CString StrTime = GetTimeString(TimePrice);		

	//
	CString StrSQL;
	StrSQL.Format(L"insert into %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",		
				  KStrTableReport.GetBuffer(), KStrName.GetBuffer(), KStrCode.GetBuffer(), KStrOpen.GetBuffer(), 
				  KStrHigh.GetBuffer(), KStrLow.GetBuffer(), KStrClose.GetBuffer(), KStrVolume.GetBuffer(), KStrHold.GetBuffer(), 
				  KStrPreClose.GetBuffer(), KStrAvg.GetBuffer(), KStrPreAvg.GetBuffer(), KStrVolumeCur.GetBuffer(), KStrHoldDif.GetBuffer(), 
				  KStrBuyPrice1.GetBuffer(), KStrSellPrice1.GetBuffer(), KStrBuyVolume1.GetBuffer(), KStrSellVolume1.GetBuffer(),
				  KStrTime.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				  StrPriceOpen.GetBuffer(), StrPriceHigh.GetBuffer(), StrPriceLow.GetBuffer(), StrPriceClose.GetBuffer(), 
				  StrVolumn.GetBuffer(), StrHold.GetBuffer(), StrPreClose.GetBuffer(), StrAvg.GetBuffer(), StrPreAvg.GetBuffer(), 
				  StrVolumnCur.GetBuffer(), StrHoldDif.GetBuffer(), StrBuyPrice1.GetBuffer(), StrSellPrice1.GetBuffer(), 
				  StrBuyVolume1.GetBuffer(), StrSellVolume1.GetBuffer(), StrTime.GetBuffer());

	return StrSQL;
}

CString CRecordDataCenter::GeneralRealtimePriceUpdateSql(CMerch* pMerch)
{
	// ���ơ����롢�����ߡ��͡����¡��ɽ������ֲ����������̡�����ۡ�����㡢���֡��ֲ��һ����һ������һ������һ��ʱ�� 
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		return L"";
	}
	
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	
	CString StrPriceOpen	= Float2String(pMerch->m_pRealtimePrice->m_fPriceOpen, iSaveDec, false, false);
	CString StrPriceHigh	= Float2String(pMerch->m_pRealtimePrice->m_fPriceHigh, iSaveDec, false, false);
	CString StrPriceLow		= Float2String(pMerch->m_pRealtimePrice->m_fPriceLow, iSaveDec, false, false);	
	CString StrPriceClose	= Float2String(pMerch->m_pRealtimePrice->m_fPriceNew, iSaveDec, false, false);
	CString StrVolumn		= Float2String(pMerch->m_pRealtimePrice->m_fVolumeTotal, iSaveDec, false, false);
	CString StrHold			= Float2String(pMerch->m_pRealtimePrice->m_fHoldTotal, iSaveDec, false, false);
	CString StrPreClose		= Float2String(pMerch->m_pRealtimePrice->m_fPricePrevClose, iSaveDec, false, false);
	CString StrAvg			= Float2String(pMerch->m_pRealtimePrice->m_fPriceAvg, iSaveDec, false, false);
	CString StrPreAvg		= Float2String(pMerch->m_pRealtimePrice->m_fPricePrevAvg, iSaveDec, false, false);
	CString StrVolumnCur	= Float2String(pMerch->m_pRealtimePrice->m_fVolumeCur, iSaveDec, false, false);
	CString StrHoldDif		= Float2String(pMerch->m_pRealtimePrice->m_fHoldTotal - pMerch->m_pRealtimePrice->m_fHoldPrev, iSaveDec, false, false);
	CString StrBuyPrice1	= Float2String(pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, iSaveDec, false, false);
	CString StrBuyVolume1	= Float2String(pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fVolume, iSaveDec, false, false);
	CString StrSellPrice1	= Float2String(pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, iSaveDec, false, false);
	CString StrSellVolume1	= Float2String(pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fVolume, iSaveDec, false, false);
	
	CGmtTime TimePrice = pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time;	
	CString StrTime = GetTimeString(TimePrice);		
	
	//
	CString StrSQL;
	StrSQL.Format(L"update %s set %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' , %s = '%s' where %s = '%s' and %s = '%s' ",					
				  KStrTableReport.GetBuffer(),
				  KStrOpen.GetBuffer(), StrPriceOpen.GetBuffer(), 
				  KStrHigh.GetBuffer(), StrPriceHigh.GetBuffer(),
				  KStrLow.GetBuffer(), StrPriceLow.GetBuffer(),
				  KStrClose.GetBuffer(), StrPriceClose.GetBuffer(),
				  KStrVolume.GetBuffer(), StrVolumn.GetBuffer(),
				  KStrHold.GetBuffer(), StrHold.GetBuffer(),
				  KStrPreClose.GetBuffer(), StrPreClose.GetBuffer(),
				  KStrAvg.GetBuffer(), StrAvg.GetBuffer(),
				  KStrPreAvg.GetBuffer(), StrPreAvg.GetBuffer(),
				  KStrVolumeCur.GetBuffer(), StrVolumnCur.GetBuffer(),
				  KStrHoldDif.GetBuffer(), StrHoldDif.GetBuffer(),
				  KStrBuyPrice1.GetBuffer(), StrBuyPrice1.GetBuffer(),
				  KStrBuyVolume1.GetBuffer(), StrBuyVolume1.GetBuffer(),
				  KStrSellPrice1.GetBuffer(), StrSellPrice1.GetBuffer(),
				  KStrSellVolume1.GetBuffer(), StrSellVolume1.GetBuffer(),
				  KStrTime.GetBuffer(),	StrTime.GetBuffer(),

				  KStrCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
				  KStrName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
				  

	return StrSQL;
}

CString CRecordDataCenter::GeneralTickInsertSql(CMerch* pMerch, const CShowTick& Tick)
{
	// ���ơ����롢���¼ۡ����֡�ÿ�ʲֲ������ǡ�ʱ��	
	if ( NULL == pMerch )
	{
		return L"";
	}
	
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	
	CString StrPriceClose	= Float2String(Tick.m_fPrice, iSaveDec, false, false);	
	CString StrPriceAvg		= L"";
	
	if ( NULL != pMerch->m_pRealtimePrice )
	{
		StrPriceAvg = Float2String(pMerch->m_pRealtimePrice->m_fPriceAvg, iSaveDec, false, false);
	}		
	
	CString StrVolumnCur	= Float2String(Tick.m_fVolume, iSaveDec, false, false);
	CString StrHoldDif		= Float2String(Tick.m_fHold - Tick.m_fHoldTotalPrev, false, false);
	CString	StrProperty		= Tick.m_StrProperty;
	CString StrTime			= GetTimeString(Tick.m_TimeCurrent.m_Time);		
	
	//
	CString StrSQL;
	StrSQL.Format(L"insert into %s (%s,%s,%s,%s,%s,%s,%s,%s) values ('%s','%s','%s','%s','%s','%s','%s','%s')",		
				  KStrTableTick.GetBuffer(), KStrName.GetBuffer(), KStrCode.GetBuffer(), KStrClose.GetBuffer(), 
				  KStrAvg.GetBuffer(), KStrVolumeCur.GetBuffer(), KStrHoldDif.GetBuffer(), KStrProperty.GetBuffer(), 
				  KStrTime.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				  StrPriceClose.GetBuffer(), StrPriceAvg.GetBuffer(), StrVolumnCur.GetBuffer(), StrHoldDif.GetBuffer(), StrProperty.GetBuffer(), StrTime.GetBuffer());
	
	return StrSQL;
}

void CRecordDataCenter::TestRecordSqlToFile(const CString& StrSql, char* strFileName)
{
	FILE* pFile = fopen(strFileName, "w+");
	if ( NULL != pFile )
	{
		string strtest;
		UnicodeToUtf8(StrSql, strtest);
		fprintf(pFile, "%s", strtest.c_str());
		fclose(pFile);
	}
}

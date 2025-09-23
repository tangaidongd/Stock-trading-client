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
// 一个请求的等待时间
static const int32 KiTimeOut					= 1000 * 60 * 2;		    // 2 分钟还没回包, 就下一条

static const int32 KiSaveDataGap				= 1000 * 60;				// 1 分钟记录一次所有周期的

// 一次插入多少条数据
static const int32 KiRecordNumsPerTime			= 50;						

// 日志文件
static const char* strFileLogPath				= "DbLog";

// 定时器:
const int32 KiTimerIdSaveData					= 1000;						// 判断是否到了收盘时间
const int32 KiTimerPeriodSaveData				= 1000 * 1;					// 

// xml 文件字段
static const CString KStrFileName				= L"./sqlconfig.xml";		// 配置文件名

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

// 数据库表名
CString KStrTableTick			= L"Ticks";
CString KStrTableReport			= L"Report";
CString KstrTableKLine			= L"KLine";

// 数据库字段名
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
 	m_bStart			= false;						// 是否开始工作
	
	m_bTimeToReqNext	= false;							
 	m_pDbProcess		= NULL;							// 数据库
 	
	m_eErrType			= EETNone;						// 错误类型
	m_eWorkState		= EWSNone;

	m_dwDbPort			= 1433;

 	m_aMerchs.clear();
	m_mapMerchsForReq.clear();
	ZeroMemory(m_aMaParams, sizeof(m_aMaParams));
	m_aCircles.clear();

	m_b1Min				= false;
	m_bstMerchSaveValid	= false;	
 	m_hThreadReqData	= NULL;							// 请求线程
 	m_hThreadSaveData	= NULL;							// 取数据线程
	m_hThreadTimerRecord= NULL;

	m_bJustProcessSelfCircle = false;
	m_bInitialSaveFinish	 = false;
	m_bInitialReqRealtime	 = true;

 	m_bThreadReqExit	= true;							// 请求线程退出标志
 	m_bThreadSaveExit	= true;							// 取数据线程退出标志
	m_bThreadTimerRecordExit = true;	

	m_dwTimeSendReq		= (DWORD)-1;
	m_dwTimeRecord		= (DWORD)-1;

 	// 以启动时间作为文件名
	std::string sFileLogPath = GeneralFileName();
 	m_pFile	= fopen(sFileLogPath.c_str(), "wa+");
 	ASSERT(NULL != m_pFile);

	// 全部都收听
	m_eListenType = ECTCount;
	m_pAbsCenterManager->AddViewDataListner(this);
}

// 成员指针变量在stopWork（）里已经释放
//lint --e{1579}
CRecordDataCenter::~CRecordDataCenter()
{
	StopWork();
}

std::string CRecordDataCenter::GeneralFileName()
{
	// 以启动时间作为文件名
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

	// L"20110318112630" 这种格式
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
			DebugLog(L"时间 %s 格式非法", StrTime);
			//ASSERT(0);
			return false;
		}
	}
	
	// 年
	char strYear[5] = {0};
	strncpy(strYear, StrTime, 4);
	int32 iYear = atoi(strYear);	
	
	if ( iYear <= 0 || iYear >= 3000 )
	{
		//ASSERT(0);
		return false;
	}
		
	// 月
	char strMonth[3] = {0};
	strncpy(strMonth, StrTime + 4, 2);
	int32 iMonth = atoi(strMonth);

	if ( iMonth <= 0 || iMonth >= 13 )
	{
		//ASSERT(0);
		return false;
	}

	// 日
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

	// 时
	char strHour[3] = {0};
	strncpy(strHour, StrTime + 8, 2);
	int32 iHour = atoi(strHour);
	
	if ( iHour < 0 || iHour >= 24 )
	{
		//ASSERT(0);
		return false;
	}

	// 分
	char strMinute[3] = {0};
	strncpy(strMinute, StrTime + 10, 2);
	int32 iMinute = atoi(strMinute);
	
	if ( iMinute < 0 || iMinute >= 60 )
	{
		//ASSERT(0);
		return false; 
	}

	// 秒
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
		// 比现在的时间还大
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

	// 最大的MA 参数
	int32 iMaxMa = GetMaxMa();

	// 遍历每个周期的, 生产每个基本周期需要发的最大请求
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
			// 记录每个周期需要的最大 K 线数 (用于定时记录的时候, 取K 线)
			if ( iMaxMa > 0  )
			{
				m_mapMaxKLineCount[eKLineBase] = iSacle * iMaxMa;
			}
			else
			{
				m_mapMaxKLineCount[eKLineBase] = 1;
			}

			// 修正这个结构体的值
			it->m_eIntervalOrignal = eNodeTimeInterval;
			it->m_eKLineBase = eKLineBase;

			// 当前的这个请求:
			CMmiReqMerchKLine ReqNow;
	
			//
			DWORD TimeNow = stCircles.m_ReqTime.GetTime();
			DebugLog(L"当前请求的基本周期是 %d 当前这个请求时间: %d", eKLineBase, TimeNow);

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
			
			// 用这个ID 值暂时保存请求的时间值
			ReqNow.m_TimeSpecify	= TimeNow;

			// 用这个暂时保存比例值
			ReqNow.m_iCount			= iSacle;

			
			// 先看已经存在的请求里是什么情况				
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
				// 当前是 -1 表示要请求所有K 线 
				// 或者 本地没有的时候就用这个
				aReqs[eKLineBase] = ReqNow;

				if (!bExist)
				{
					DebugLog(L"这个周期暂无记录, 使用当前的");
				}
				else
				{
					DebugLog(L"当前的范围最大, 使用当前的");
				}				
			}
			else if ( 0 == TimeNow )
			{
				// 当前是 0. 最小的范围, 就取原来就行, 什么都不用处理
				DebugLog(L"当前的是 0 , 使用原来的");
			}
			else
			{
				DWORD TimeExist = ReqExist.m_TimeSpecify.GetTime();
				DebugLog(L"原来的时间是%d");

				// 当前定义了一个请求的时间值
				
				if ( (UINT)-1 == TimeExist )
				{
					// 原来的范围大, 不用处理
					DebugLog(L"原来的范围最大, 使用原来的");
				}
				else if ( 0 == TimeExist )
				{
					// 原来是0, 用现在的
					aReqs[eKLineBase] = ReqNow;
					DebugLog(L"原来的是0, 使用现在的");
				}
				else
				{
					// 原来也是一个时间值, 那就看谁的时间更早
					if ( TimeNow < TimeExist )
					{
						DebugLog(L"现在的范围比原来的大, 使用现在的");
						aReqs[eKLineBase] = ReqNow;
					}
				}
			}
		}
		else
		{
			//ASSERT(0);
			DebugLog(L"获取周期信息失败!!!");
		}
	}

	if ( aReqs.empty() )
	{
		DebugLog(L"生成请求失败");
		return false;
	}

	// 生成请求完毕:
	DebugLog(L"生成请求完毕:");

	for ( map<E_KLineTypeBase, CMmiReqMerchKLine>::iterator itReq = aReqs.begin(); itReq != aReqs.end(); ++itReq )
	{
		CMmiReqMerchKLine st = itReq->second;
		m_aReqs.push_back(st);
		DebugLog(L"基本周期:%d 请求类型: %d 时间点:%d 倍数: %d", st.m_eKLineTypeBase, st.m_eReqTimeType, st.m_TimeSpecify.GetTime(), st.m_iCount);
	}

	// 日志打出来
	for ( map<E_KLineTypeBase, int32>::iterator itMaxCount = m_mapMaxKLineCount.begin(); itMaxCount != m_mapMaxKLineCount.end(); ++itMaxCount )
	{
		DebugLog(L"周期: %d 需要的最大K 线数目: %d", itMaxCount->first, itMaxCount->second);
	}

	// 具体的带商品信息的请求:
	for ( set<CMerch*>::iterator itMerch = m_aMerchs.begin(); itMerch != m_aMerchs.end(); ++itMerch )
	{
		//
		CMerch* pMerch = *itMerch;
		if ( NULL == pMerch )
		{
			continue;
		}
			
		ListKLineReqs ListReqs;

		// 填充具体的请求:
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
				// 只有这种情况才需要使用倍数
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
	DebugLog(L"开始读取配置文件");

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

	// DataBase 节点
	TiXmlElement* pElementDataBase = pRootElement->FirstChildElement();
	if ( NULL == pElementDataBase )
	{
		//ASSERT(0);
		return false;
	}

	// 地址
	const char* strValue = pElementDataBase->Attribute(KA_dbaddress);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrAddress;
	Utf8ToUnicode(strValue, WStrAddress);
	m_StrDbAddress = WStrAddress.c_str();
	
	// 数据库名称
	strValue = pElementDataBase->Attribute(KA_dbname);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrDbName;
	Utf8ToUnicode(strValue, WStrDbName);
	m_StrDbName = WStrDbName.c_str();

	// 数据库端口
	strValue = pElementDataBase->Attribute(KA_dbport);
	if ( NULL != strValue && strlen(strValue) > 0 )
	{		
		m_dwDbPort = atoi(strValue);
	}

	// 用户名
	strValue = pElementDataBase->Attribute(KA_username);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrUserName;
	Utf8ToUnicode(strValue, WStrUserName);
	m_StrDbUserName = WStrUserName.c_str();
	
	// 密码
	strValue = pElementDataBase->Attribute(KA_password);
	if ( NULL == strValue || strlen(strValue) <= 0 )
	{
		return false;
	}
	
	//
	wstring WStrPassWord;
	Utf8ToUnicode(strValue, WStrPassWord);
	m_StrDbPassword = WStrPassWord.c_str();
	
	// 商品节点
	TiXmlElement* pElementMerch = pElementDataBase->NextSiblingElement();
	if ( NULL == pElementMerch )
	{
		//ASSERT(0);
		return false;
	}

	// 具体的商品
	
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

		// 找到这个商品:
		CMerch* pMerch = NULL;
		m_pAbsCenterManager->GetMerchManager().FindMerch(StrCode, iID, pMerch);
		if ( NULL == pMerch )
		{
			DebugLog(L"%s 商品没有找到, 忽略...", StrCode.GetBuffer());
			continue;
		}

		// 保存
		if ( 0 == m_aMerchs.count(pMerch) )
		{
			m_aMerchs.insert(pMerch);
			
			// 关注商品
			CSmartAttendMerch stSmartAttendMerch;
			stSmartAttendMerch.m_bNeedNews = false;
			stSmartAttendMerch.m_iDataServiceTypes = 0;
			stSmartAttendMerch.m_pMerch = pMerch;
			
			//
			aSmartAttendMerch.Add(stSmartAttendMerch);
		}
		else
		{
			DebugLog(L"重复商品 %s, 过滤", StrCode.GetBuffer());
		}
	}

	// 设置关注商品
	m_pAbsCenterManager->SetRecordDataCenterAttendMerchs(aSmartAttendMerch);
	

	// 周期
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
	
		// 判断是否合法
		if ( !BeValidCircle(iCircle) )
		{
			DebugLog(L"周期非法: %d", iCircle);
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
		
		// 是否含有 1 分钟的周期
		if ( 1 == iCircle )
		{
			m_b1Min = true;
		}

		// 把请求时间这个字段还原成 0 
		// ...fangz 0318 测试的时候去掉这个机制
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
		// 最多三个
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

		// 判断是否合法, 解析MA 的周期
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
	DebugLog(L"开始工作");

	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	//
	if ( !InitialFromFile() )
	{
		DebugLog(L"读取配置文件失败");

		//ASSERT(0);
		return false;		
	}

	//
	if ( !GeneralRequests() )
	{
		DebugLog(L"生产请求序列失败");
		//ASSERT(0);
		return false;
	}

	// 定时器
	SetRecordDataCenterTimer(KiTimerIdSaveData, KiTimerPeriodSaveData);

	//
	if ( !CDataBase::InitComEnv() )
	{
		DebugLog(L"数据库初始化失败");
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
		DebugLog(L"数据库初始化失败");
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
			DebugLog(L"请求时间为 -1, 先删除表中所有内容. 全部重写.");
		}
	}
	
	//
	m_bStart = true;
	m_bTimeToReqNext = true;
	
	//
	if ( !BeginReqDataThread() )
	{
		DebugLog(L"启动请求线程失败!");
		//ASSERT(0);		
		return false;			
	}
	
	//
	if ( !BeginSaveDataThread() )
	{
		DebugLog(L"启动数据库记录线程失败!");
		//ASSERT(0);
		return false;
	}
	
	if ( !BeginTimerRecordThread() )
	{
		DebugLog(L"启动定时记录数据线程失败!");
		//ASSERT(0);
		return false;
	}

	//
	DebugLog(L"开始工作!");
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

	// 请求线程
	if ( NULL != m_hThreadReqData )	
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadReqData, INFINITE) )
		{
			// 退出了
		}	
	}
	
	// 接收线程
	if ( NULL != m_hThreadSaveData )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadSaveData, INFINITE) )
		{
			// 退出了
		}
	}

	// 定时记录线程
	if ( NULL != m_hThreadTimerRecord )
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadTimerRecord, INFINITE) )
		{
			// 退出了
		}
	}

	//
	StopRecordDataCenterTimer(KiTimerIdSaveData);

	// 数据库
	DEL(m_pDbProcess);
	CDataBase::UnitComEnv();

	// 清空	
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

	DebugLog(L"停止工作");

	return true;
}

void CRecordDataCenter::PromptDbErrMsg()
{
 	CString StrMsg = m_pDbProcess->GetComErrorDescribe();
 	::MessageBox(GetActiveWindow(), StrMsg, L"财富金盾", MB_ICONWARNING);	
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
	
	DebugLog(L"启动数据请求线程");

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
					// 出错了
					pThis->DebugLog(L"请求出错. 退出!!!");				
				}
				
				// 完成了, 清空数据
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
		// 没到时间
		return false;
	}

	// 关上开关, 等写数据完成那边打开
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

	// 行情和Tick 数据可以一开始就请求下来
	if ( m_bInitialReqRealtime )
	{
		// 请求当日的行情数据, 取昨收价
		CMmiReqRealtimePrice ReqRealtimePrice;
	
		// 请求实时Tick
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
		// 取完了, 没商品了
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

		// 请求 K 线
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
			DebugLog(L"请求K 线失败!");
			//ASSERT(0);
			return true;
		}
		
		//
		m_dwTimeSendReq = timeGetTime();
		DebugLog(L"请求 %s 的当日K 线数据, 时间 %d", pMerchReq->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_dwTimeSendReq);
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
	
	DebugLog(L"启动写数据库线程");

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
			DebugLog(L"%s 商品K 线数据为空, 跳过", stMerchSave.m_StrMerchCode);
		}
		else
		{
			DebugLog(L"%d %s 商品不存在, 跳过", stMerchSave.m_iMarketId, stMerchSave.m_StrMerchCode);
		}
		
		//
		ClearLoacalSaveMerchData();
	
		// 开始请求下一个商品
		m_bTimeToReqNext = true;

		// //ASSERT(0);		
		return false;
	}
		 
	// 记录数据
	CKLine* pKLine = stMerchSave.m_KLines.GetData();
	E_KLineTypeBase eKLineBase = stMerchSave.m_eKLineTypeBase;

	//
	for ( set<T_Circles>::iterator it = m_aCircles.begin(); it != m_aCircles.end(); ++ it )
	{
		T_Circles st = *it;

		// 是这个基本周期的, 合K 线, 算指标, 写数据库
		if ( eKLineBase == st.m_eKLineBase )
		{
			if ( m_bJustProcessSelfCircle )
			{
				// 如果只处理这个指定周期的, 那么还需要增加判断, 看是否是以这个基本周期为基础的自定义周期, 是的话就pass , 不处理
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
					DebugLog(L"%d %d %d非指定周期, Pass... ", st.m_iCircle, (int32)st.m_eIntervalOrignal, (int32)st.m_eKLineBase);
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

			// K 线准备好了, 看需不需要算 MA
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
						DebugLog(L"计算%s 周期%d MA%d 失败", pMerch->m_MerchInfo.m_StrMerchCode, st.m_iCircle, iMaParam);
						continue;
					}
					
					// 保存计算结果
					DebugLog(L"计算%s 周期%d MA%d 成功", pMerch->m_MerchInfo.m_StrMerchCode, st.m_iCircle, iMaParam);
				}
				
				if ( m_bInitialSaveFinish )
				{
					// 实时记录分钟数据的时候, 只要最新一根就可以了					
					float fMaLast = *aResults.rbegin();
					aResults.clear();
					aResults.push_back(fMaLast);
				}

				aMaResults.push_back(aResults);
			}

			// 开始记录数据库:
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
				// 只取最新一根就行了
				CKLine KLineLast = aKLinesSave.GetAt(iSizeSave - 1);
				aKLinesSave.RemoveAll();
				aKLinesSave.Add(KLineLast);

				iSizeSave = aKLinesSave.GetSize();
			}
			
			//
			DebugLog(L"记录K 线 %d 条, 判断重复 = %d", iSizeSave, bNeedJudgeSame);
			
			CString StrTableName = st.m_StrTableName;			
			SaveKLineToDB(pMerch, StrTableName, aKLinesSave, aMaResults, bNeedJudgeSame);			
		}		
	}
	
	// 清空本地
	ClearLoacalSaveMerchData();

	// 请求下一个
	m_bTimeToReqNext = true;

	//
	if ( m_mapMerchsForReq.empty() && !m_bInitialSaveFinish )
	{
		// 
		DebugLog(L"初始化记录过程完成!!!");
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
	
	DebugLog(L"启动定时记录数据库线程");

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
		// 初始化处理还没完成
		return false;
	}

	// 判断是否到了记录时间
	DWORD dwTimeNow = timeGetTime();
	if ( dwTimeNow - m_dwTimeRecord < KiSaveDataGap )
	{
		return false;
	}

	// 开始处理记录数据
	for ( set<CMerch*>::iterator itMerch = m_aMerchs.begin(); itMerch != m_aMerchs.end(); ++itMerch )
	{
		CMerch* pMerch = *itMerch;

		if ( NULL == pMerch )
		{
			continue;
		}

		//
		DebugLog(L"开始定时记录%s 的数据", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		
		for ( set<T_Circles>::iterator itCircle = m_aCircles.begin(); itCircle != m_aCircles.end(); ++itCircle )
		{
			T_Circles stCircle = *itCircle;
			
			DebugLog(L"周期为%d %d", stCircle.m_iCircle, stCircle.m_eKLineBase);

			// 取基本周期
			E_KLineTypeBase eKLineBase = stCircle.m_eKLineBase;
			
			// 看看这个周期最多需要的 K 线数
			map<E_KLineTypeBase, int32>::iterator itFind = m_mapMaxKLineCount.find(eKLineBase);
			if ( m_mapMaxKLineCount.end() == itFind )
			{
				// 
				DebugLog(L"没有找到这个周期的数据0!!!");				
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
				DebugLog(L"没有找到这个周期的数据1!!!");
				continue;
			}

			//
			int32 iSrcCount = pMerchKLineNode->m_KLines.GetSize();			
			CKLine* pKLineSrc = (CKLine*)pMerchKLineNode->m_KLines.GetData();
			
			if ( iSrcCount <= 0 || NULL == pKLineSrc )
			{
				DebugLog(L"没有找到这个周期的数据2!!!");
				continue;
			}
			
			// 整理出需要更新的数据依赖的 K 线
			if ( iNeedCount > iSrcCount )
			{
				iNeedCount = iSrcCount;
			}
			
			if ( iNeedCount <= 0 )
			{
				DebugLog(L"没有找到这个周期的数据3!!!");
				//ASSERT(0);
				continue;
			}
		
			//
			CArray<CKLine, CKLine> aKLineNeed;
			aKLineNeed.SetSize(iNeedCount);
			CKLine* pKLineNeed = (CKLine*)aKLineNeed.GetData();
			
			// 拷贝数据
			int32 iIndexBegin = iSrcCount - iNeedCount;
			memcpyex(pKLineNeed, pKLineSrc + iIndexBegin, sizeof(CKLine) * iNeedCount);
			
			// 计算 MA
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
						DebugLog(L"计算%s 周期%d MA%d 失败", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stCircle.m_iCircle, iMaParam);
						continue;
					}
					
					// 保存计算结果
					DebugLog(L"计算%s 周期%d MA%d 成功", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stCircle.m_iCircle, iMaParam);
				}

				// 只取最后一个值:
				float fMaLast = *aResults.rbegin();
				aResults.clear();
				aResults.push_back(fMaLast);

				//
				aMaResults.push_back(aResults);
			}
			
			// 开始记录数据库:
			CString StrTableName = stCircle.m_StrTableName;			

			// 只取最新一根就行了
			CKLine KLineLast = aKLineNeed.GetAt(iNeedCount - 1);
			aKLineNeed.RemoveAll();
			aKLineNeed.Add(KLineLast);

			//
			DebugLog(L"计算成功, 写入数据库");

			// 记录
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
		// 判断一下有没有超时的
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

	// 记录到数据库中
	bool32 bRerocdInDb = BeRealtimePriceInDB(pMerch);

	if ( !bRerocdInDb )
	{
		// 插入一条
		CString StrSql = GeneralRealtimePriceInsertSql(pMerch);

		if ( m_pDbProcess->Execute(StrSql, NULL) )
		{
			DebugLog(L"新增一条行情记录: %s %s 成功", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
		else
		{
			DebugLog(L"新增一条行情记录: %s %s 失败", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
	}
	else
	{
		// 更新
		CString StrSql = GeneralRealtimePriceUpdateSql(pMerch);
		
		bool32 bTest = false;
		if ( bTest )
		{
			TestRecordSqlToFile(StrSql, "c:\\realtimeupdate.txt");
		}
		
		if ( m_pDbProcess->Execute(StrSql, NULL) )
		{
			DebugLog(L"更新一条行情记录: %s %s 成功", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
				GetTimeString(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time).GetBuffer());
		}
		else
		{
			DebugLog(L"更新一条行情记录: %s %s 失败", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
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
			DebugLog(L"未找到此分笔记录:%s %s", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), 
				GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer());
			continue;
		}

		if ( 0 == iPos )
		{
			// 第一笔, 无法计算
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
			DebugLog(L"成功记录分笔数据: %s %s %.2f %.2f %s", 
				     pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), 
					 GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer(), 
					 Tick.m_fPrice, Tick.m_fHold - Tick.m_fHoldTotalPrev, Tick.m_StrProperty.GetBuffer());
		}
		else
		{
			DebugLog(L"记录分笔数据失败: %s %s", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetTimeString(MmiPushTick.m_Ticks[i].m_TimeCurrent.m_Time).GetBuffer());
		}
	}

	// 初始化的过程已经完成, 正常的实时数据引起K 线更新
	if ( m_bInitialSaveFinish && m_b1Min ) 
	{
		// 1 分钟 K 线, 实时更新数据库
		{
			// 源 K 线数据
			int32 iPos;
			CMerchKLineNode* pKLineNode;
			if ( !pMerch->FindMerchKLineNode(EKTBMinute, iPos, pKLineNode) || iPos < 0 )
			{
				DebugLog(L"没有找到源K 线数据, 这种情况不应该发生!");
				//ASSERT(0);
				return;
			}
			
			int32 iSrcCount = pKLineNode->m_KLines.GetSize();
			CKLine* pKLineSrc = (CKLine*)pKLineNode->m_KLines.GetData();
			
			if ( iSrcCount <= 0 || NULL == pKLineSrc )
			{
				return;
			}
			
			// 整理出需要更新的数据依赖的 K 线
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
			DebugLog(L"实时更新1 分钟 K 线: 最大MA 的参数为 %d. 本地K 线数据有 %d 根, 取最新的 %d 根", iMaxMa, iSrcCount, iNeedCount);
			
			//
			CMerchKLineNode MerchKLineNode;
			MerchKLineNode = *pKLineNode;
			MerchKLineNode.m_KLines.SetSize(iNeedCount);
			CKLine* pKLineNeed = (CKLine*)MerchKLineNode.m_KLines.GetData();
			
			// 拷贝数据
			int32 iIndexBegin = iSrcCount - iNeedCount;
			memcpyex(pKLineNeed, pKLineSrc + iIndexBegin, sizeof(CKLine) * iNeedCount);
			
			// 设置参数, 丢给线程记录数据库
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

	// 第一次的时候发了所有K 线请求, 请求的 K 线回来了:
	if ( iMmiReqId == m_iReqID )
	{
		// 记录数据库
		SetSaveMerchData(pMmiRespMerchKLine->m_MerchKLineNode, false);

		// 将请求的时间标志复位
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
	// DebugLog(L"收到 %s 的 K 线数据: %d 根", stMerchSave.m_StrMerchCode, stMerchSave.m_KLines.GetSize());
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
			
			// 商品
			pMerch = pMerchNow;

			// 请求, 取列表第一个			
			stReq = *ListReqs.begin();

			// 更新本地数据
			ListReqs.pop_front();
			m_mapMerchsForReq[pMerch] = ListReqs;

			//
			if ( ListReqs.empty() )
			{
				m_mapMerchsForReq.erase(it);
			}

			DebugLog(L"取到请求, 商品: %s 请求: %s", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), stReq.GetSummary().GetBuffer());

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
		DebugLog(L"查询 %s 的 K 线数据 %s 失败!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
		return false;
	}

	if ( QuerySet.GetRecordCount() > 0 )
	{
		DebugLog(L"查询到有 %s 的 K 线数据 %s 共 %d 条记录", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer(), QuerySet.GetRecordCount());
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
		DebugLog(L"更新 %s 的 K 线数据 %s 失败!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
	}

	DebugLog(L"更新 %s 的 K 线数据 %s 成功!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), GetKLineTimeString(KLine).GetBuffer());
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
		DebugLog(L"当前时间:%d 请求发送时间: %d, 已经超时... 忽略这个请求, 继续下一个商品");
	
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

	// K 线数据
	CKLine* pKLine = (CKLine*)aKLines.GetData();

	// Ma 数据
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
		DebugLog(L"待记录的数据非法, 记录失败...");
		//ASSERT(0);
		return false;
	}

	//
	int32 iKLineSize = aKLines.GetSize();
	CKLine* pKLine = (CKLine*)aKLines.GetData();
	int32 iIndex = 0;
	
	if ( !bNeedJudgeSame )
	{
		// 不需要判断重复数据
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

			// TRACE(L"         ===生成SQL 语句耗时: %d ms=== \n", time2 - time1);
			
			//DWORD time3 = timeGetTime();
			
			if ( !m_pDbProcess->Execute(StrSql, NULL) )
			{								
				DebugLog(L"%s\n 记录数据库失败!!!", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
				PromptDbErrMsg();
				continue;
			}	
			
			//DWORD time4 = timeGetTime();
			// TRACE(L"         ===记录数据库耗时: %d ms=== \n", time4 - time3);	
		} 
	}
	else
	{
		// 需要判断重复, 只能一条条的写了
		for ( int32 i = 0; i < iKLineSize; i++ )
		{
			//
			bool32 bKLineInDB = BeKlineInDB(StrTableName, pMerch, pKLine[i]);

			if ( bKLineInDB )
			{
				// 更新
				DebugLog(L"%s %s %s K 线存在, 准备更新...", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), GetKLineTimeString(pKLine[i]).GetBuffer());
				UpdateKlineInDB(StrTableName, pMerch, pKLine[i], aMaResults[0][i], aMaResults[1][i], aMaResults[2][i]);
			}
			else
			{
				// 插入
				DebugLog(L"%s %s %s K 线不存在, 准备插入...", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), GetKLineTimeString(pKLine[i]).GetBuffer());
				CString StrSQL = GeneralKLineInsertSql(StrTableName, pMerch, pKLine[i], aMaResults[0][i], aMaResults[1][i], aMaResults[2][i]);

				if ( m_pDbProcess->Execute(StrSQL, NULL) )
				{
					DebugLog(L"插入成功!");
				}
				else
				{
					DebugLog(L"插入失败!");
				}
			}
		}
	}

	return true;
}

CString CRecordDataCenter::QueryRealtimePriceSql(CMerch* pMerch)
{
	// 名称、代码、开、高、低、最新、成交量、持仓量、昨收盘、结算价、昨结算、现手、仓差、买一、卖一、买量一、卖量一、时间 

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
		DebugLog(L"生成行情查询语句失败!");
		return false;
	}

	//
	CRecordSet QuerySet;
	if ( !m_pDbProcess->Execute(StrQuery, &QuerySet) )
	{
		DebugLog(L"查询 %s 的 行情数据失败!", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		return false;
	}
	
	if ( QuerySet.GetRecordCount() > 0 )
	{
		DebugLog(L"查询到有 %s 的行情数据共 %d 条记录", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), QuerySet.GetRecordCount());
		return true;
	}

	//
	return false;
}

CString CRecordDataCenter::GeneralRealtimePriceInsertSql(CMerch* pMerch)
{
	// 名称、代码、开、高、低、最新、成交量、持仓量、昨收盘、结算价、昨结算、现手、仓差、买一、卖一、买量一、卖量一、时间 
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
	// 名称、代码、开、高、低、最新、成交量、持仓量、昨收盘、结算价、昨结算、现手、仓差、买一、卖一、买量一、卖量一、时间 
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
	// 名称、代码、最新价、现手、每笔仓差、买卖标记、时间	
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

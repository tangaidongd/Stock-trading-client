// AlarmCenter.cpp: implementation of the CAlarmCenter class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "AlarmCenter.h"
#include <mmsystem.h>
#include "MerchManager.h"
#include "PathFactory.h"
#include "tinyxml.h"
#include "CFormularContent.h"
#include "ArbitrageManage.h"
#include "coding.h"
#include "formulaengine.h"
//#include "IoViewShare.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern CString g_strUserName;

// xml
static const char* KE_Flags		   = "flags";
static const char* KA_BeAlarm	   = "alarm";
static const char* KA_BePrompt	   = "prompt";
static const char* KA_Promptype	   = "promptype";
static const char* KA_PrompDlg	   = "prompdlg";
static const char* KA_PrompStatus  = "prompstatus";

static const char* KE_FixConditions = "fixconditions";
static const char* KE_Merch			= "merch";
static const char* KA_Code			= "code";
static const char* KA_Id			= "id";
static const char* KA_Up			= "up";
static const char* KA_Down			= "down";
static const char* KA_Trend			= "trend";
static const char* KA_Rise          = "rise";
static const char* KA_Change		= "change";
static const char* KA_ETF			= "etf";

static const char* KE_ChangeConditions = "changeconditions";
static const char* KE_Formulars	   = "formulars";
static const char* KA_Name		   = "name";



//////////////////////////////////////////////////////////////////////
CAlarmConditions::CAlarmConditions()
{
	m_bInitialCalc = false;

	for ( int32 i = 0; i < FIX_ALM_CDS; i++ )
	{
		m_aFixedParams[i] = 0.;
	}

	m_aKLines.RemoveAll();
}

// 拷贝构造
CAlarmConditions::CAlarmConditions(const CAlarmConditions& stSrc)
{
	//
	m_bInitialCalc = stSrc.m_bInitialCalc;

	//
	for ( int32 i = 0; i < FIX_ALM_CDS; i++ )
	{
		m_aFixedParams[i] = stSrc.m_aFixedParams[i];
	}

	//
	m_aKLines.Copy(stSrc.m_aKLines);
}

// 赋值
CAlarmConditions& CAlarmConditions::operator=(const CAlarmConditions& stSrc)
{
	if ( this == &stSrc )
	{
		return *this;
	}

	//
	m_bInitialCalc = stSrc.m_bInitialCalc;

	//
	for ( int32 i = 0; i < FIX_ALM_CDS; i++ )
	{
		m_aFixedParams[i] = stSrc.m_aFixedParams[i];
	}

	//
	m_aKLines.Copy(stSrc.m_aKLines);

	return *this;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlarmCenter::CAlarmCenter(CAbsCenterManager* pCenterManager)
{
	m_mapAlarams.clear();
	m_aAlarmFormulars.RemoveAll();
	m_mapArbitrageAlarams.clear();

	m_aNotifys.RemoveAll();

	m_pCenterManager = pCenterManager;
	m_bNeedAlarm = false;
	m_bPrompt	 = false;
	m_eAlarmType = EATNoSound;
	m_bShowStatus = false;
	m_bShowDlg	 = true;
	m_eListenType = ECTCount;
	if ( NULL == m_pCenterManager )
	{		
		return;
	}
	
	
}
//lint --e{1579}
CAlarmCenter::~CAlarmCenter()
{	
	// 指针已经释放，防止崩溃需要先清空
	m_aAlarmFormulars.RemoveAll();
	
	ToXml();
	
	if (m_pCenterManager)
	{
		m_pCenterManager->DelViewDataListner(this);
	}		
	
}

bool32 CAlarmCenter::FromXml()
{
	// 先清空所有
	m_bPrompt = false;
	m_bNeedAlarm = false;
	m_mapAlarams.clear();
	m_aAlarmFormulars.RemoveAll();

	CString strUserName;
	if (m_pCenterManager)
	{
		strUserName = m_pCenterManager->GetUserName();
	}
	// 路径
	CString StrPathW = CPathFactory::GetAlarmMerchPath(strUserName);
	if ( StrPathW.IsEmpty() )
	{
		//////ASSERT(0);
		return false;
	}

	
	std::string strPathA;
	Unicode2MultiChar(CP_ACP, StrPathW, strPathA);

	// 装载
	TiXmlDocument Doc(strPathA.c_str());
	if ( !Doc.LoadFile() )
	{
		return false;
	}
		 
	// 解析
	TiXmlElement *pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}
	
	// 遍历
	for( TiXmlElement* pElement = pRootElement->FirstChildElement(); NULL != pElement; pElement = pElement->NextSiblingElement() )
	{
		if ( 0 == strcmp(pElement->Value(), KE_Flags) )
		{
			// 标志

			// 是否报警
			const char* strAlarm = pElement->Attribute(KA_BeAlarm);
			if ( NULL != strAlarm )
			{
				m_bNeedAlarm = atoi(strAlarm);
			}

			// 是否提示
			const char* strPrompt = pElement->Attribute(KA_BePrompt);
			if ( NULL != strPrompt )
			{
				m_bPrompt = atoi(strPrompt);
			}

			// 提示类型
			const char* strPromptype = pElement->Attribute(KA_Promptype);
			if ( NULL != strPromptype )
			{
				m_eAlarmType = (E_AlarmType)atoi(strPromptype);
			}

			// 对话框提示
			const char* strPrompDlg = pElement->Attribute(KA_PrompDlg);
			if ( NULL != strPrompDlg )
			{
				m_bShowDlg = atoi(strPrompDlg);
			}

			// 状态栏提示
			const char* strPrompStatus = pElement->Attribute(KA_PrompStatus);
			if ( NULL != strPrompStatus )
			{
				m_bShowStatus = atoi(strPrompStatus);
			}
		}
		else if ( 0 == strcmp(pElement->Value(), KE_FixConditions) )
		{
			// 固定预警条件
			for ( TiXmlElement* pMerchElement = pElement->FirstChildElement(); NULL != pMerchElement; pMerchElement = pMerchElement->NextSiblingElement() )
			{
				CMerch* pMerchNow = NULL;
				CAlarmConditions stAlarmConditions;

				// id
				int32 iMarketID = 0;
				const char* strID = pMerchElement->Attribute(KA_Id);
				if ( NULL != strID )
				{
					iMarketID = atoi(strID);
				}

				// code
				CString StrCode;
				const char* strCode = pMerchElement->Attribute(KA_Code);
				if ( NULL != strCode )
				{
					wstring WStrCode;
					Utf8ToUnicode(strCode, WStrCode);
					StrCode = WStrCode.c_str();
				}

				if ( NULL != m_pCenterManager )
				{
					m_pCenterManager->GetMerchManager().FindMerch(StrCode, iMarketID, pMerchNow);
					if ( NULL == pMerchNow )
					{
						continue;
					}
				}
				
				// 上破价				
				const char* strUpPrice = pMerchElement->Attribute(KA_Up);
				if ( NULL != strUpPrice )
				{
					 stAlarmConditions.m_aFixedParams[0] = atof(strUpPrice);
				}

				// 下破价				
				const char* strDownPrice = pMerchElement->Attribute(KA_Down);
				if ( NULL != strDownPrice )
				{
					stAlarmConditions.m_aFixedParams[1] = atof(strDownPrice);
				}

				// 涨幅
				const char* strRiseRate = pMerchElement->Attribute(KA_Rise);
				if (NULL != strRiseRate)
				{
					stAlarmConditions.m_aFixedParams[2] = atof(strRiseRate);
				}

				// 换手率
				const char* strChangeRate = pElement->Attribute(KA_Change);
				if (NULL != strChangeRate)
				{
					stAlarmConditions.m_aFixedParams[3] = atof(strChangeRate);
				}

				// 分时				
				const char* strTrend = pMerchElement->Attribute(KA_Trend);
				if ( NULL != strTrend )
				{
					stAlarmConditions.m_aFixedParams[4] = atof(strTrend);
				}
 
// 				// ETF				
// 				const char* strEtf = pMerchElement->Attribute(KA_ETF);
// 				if ( NULL != strEtf )
// 				{
// 					stAlarmConditions.m_aFixedParams[4] = atof(strEtf);
// 				}	
				
				// 保存数据
				m_mapAlarams[pMerchNow] = stAlarmConditions;
			}
		}
		else if ( 0 == strcmp(pElement->Value(), KE_ChangeConditions) )
		{
			// 可变预警条件(公式)
			for ( TiXmlElement* pFormularElement = pElement->FirstChildElement(); NULL != pFormularElement; pFormularElement = pFormularElement->NextSiblingElement() )
			{
				// name
				const char* strName = pFormularElement->Attribute(KA_Name);
				if ( NULL != strName )
				{
					//
					wstring WStrName;
					Utf8ToUnicode(strName, WStrName);
					CFormularContent* pFormular = CFormulaLib::instance()->GetFomular(WStrName.c_str());
					if ( NULL != pFormular )
					{
						m_aAlarmFormulars.Add(pFormular);
					}
				}
			}
		}
	}

	//
	if ( m_bNeedAlarm )	
	{
		m_eListenType = ECTReqRealtimePrice;
		if (m_pCenterManager)
		{
			m_pCenterManager->AddViewDataListner(this);
		}
		
	}

	// 请求数据
	RequestData(NULL);

	// 套利的初始化
	ArbitrageFromXml();

	return true;	
}

bool32 CAlarmCenter::ArbitrageFromXml()
{

	//
	m_mapArbitrageAlarams.clear();

	// 从套利的数据里初始化	
	const CArray<CArbitrage, CArbitrage&>& aArbitrages = CArbitrageManage::Instance()->GetArbitrageDataRef();
	CArbitrage* pData = (CArbitrage*)aArbitrages.GetData();

	//
	for ( int32 i = 0; i < aArbitrages.GetSize(); i++ )
	{
		CArbitrage* pNow = pData + i;
		if ( NULL == pNow )
		{
			continue;
		}

		//
		if ( pData[i].m_bAlarm )
		{
			CAlarmConditions stAlarmConditions;
			stAlarmConditions.m_aFixedParams[0] = pNow->m_fUp;
			stAlarmConditions.m_aFixedParams[1] = pNow->m_fDown;
		
			//
			m_mapArbitrageAlarams[pNow] = stAlarmConditions;
		}
	}	

	//
	RequestAllArbitrageData();

	//
	return true;
}

bool32 CAlarmCenter::ToXml()
{
	CString strUserName;
	if (m_pCenterManager)
	{
		strUserName = m_pCenterManager->GetUserName();
	}
	CString StrPathW = CPathFactory::GetAlarmMerchPath(strUserName);
	if ( StrPathW.IsEmpty() )
	{
		assert(0);
		return false;
	}

	//
	CString StrXml = L"";
	CString StrHead = L"";
	
	// XML 的版本号
	StrHead = L"<?xml version =\"1.0\" encoding=\"utf-8\" ?> \n";
	StrXml += StrHead;
	
	StrHead = L"<XMLDATA version=\"1.0.1.0\" app = \"ggtong\" data = \"alarm\" >\n",
	StrXml += StrHead;

	// Flag
	CString StrFlag;
	StrFlag.Format(L"<%s %s = \"%d\" %s = \"%d\" %s = \"%d\" %s = \"%d\" %s = \"%d\"/>\n", CString(KE_Flags).GetBuffer(), 
		CString(KA_BeAlarm).GetBuffer(), m_bNeedAlarm, CString(KA_BePrompt).GetBuffer(), m_bPrompt, 
		CString(KA_Promptype).GetBuffer(), (int32)m_eAlarmType, CString(KA_PrompDlg).GetBuffer(), (int32)m_bShowDlg, CString(KA_PrompStatus).GetBuffer(), (int32)m_bShowStatus);
	StrXml += StrFlag;

	// FixCondition
	CString StrFixConditionElement;
	StrFixConditionElement.Format(L"<%s>\n", CString(KE_FixConditions).GetBuffer());
	StrXml += StrFixConditionElement;

	for ( mapMerchAlarms::iterator it = m_mapAlarams.begin(); it != m_mapAlarams.end(); ++it )
	{
		CMerch* pMerch = it->first;
		CAlarmConditions stAlarmConditions = it->second;

		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		CString StrMerch;
		StrMerch.Format(L"<%s %s = \"%s\" %s = \"%d\" %s = \"%.2f\" %s = \"%.2f\" %s = \"%.2f\" %s = \"%.2f\" %s = \"%.2f\"/>\n",
						CString(KE_Merch).GetBuffer(),
						CString(KA_Code).GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
						CString(KA_Id).GetBuffer(), pMerch->m_MerchInfo.m_iMarketId,
						CString(KA_Up).GetBuffer(), stAlarmConditions.m_aFixedParams[0],
						CString(KA_Down).GetBuffer(), stAlarmConditions.m_aFixedParams[1],
						CString(KA_Rise).GetBuffer(), stAlarmConditions.m_aFixedParams[2],
						CString(KA_Change).GetBuffer(), stAlarmConditions.m_aFixedParams[3],
						CString(KA_Trend).GetBuffer(), stAlarmConditions.m_aFixedParams[4]);
						
		//
		StrXml += StrMerch;
	}

	// FixCondition 结束符
	StrXml += L"</";
	StrXml += CString(KE_FixConditions);
	StrXml += L">\n";

	// ChangeConditions
	CString StrChangeConditionElement;
	StrChangeConditionElement.Format(L"<%s>\n", CString(KE_ChangeConditions).GetBuffer());
	StrXml += StrChangeConditionElement;

	// Formulars
	for ( int32 i = 0; i < m_aAlarmFormulars.GetSize(); i++ )
	{
		CFormularContent* pFormular = m_aAlarmFormulars[i];
		if ( NULL == pFormular )
		{
			continue;
		}

		//
		CString StrFormular;
		StrFormular.Format(L"<%s %s = \"%s\"/>\n", CString(KE_Formulars).GetBuffer(), CString(KA_Name).GetBuffer(), pFormular->name.GetBuffer());
		StrXml += StrFormular;
	}

	// ChangeConditions 结束符
	StrXml += L"</";
	StrXml += CString(KE_ChangeConditions);
	StrXml += L">\n";

	// 整个XML 的结束符
	StrXml += L"</XMLDATA>";
	
	std::string strPathA;
	Unicode2MultiChar(CP_ACP, StrPathW, strPathA);
	// 保存
	SaveXmlFile(strPathA.c_str(), StrXml);

	return true;	
}

void CAlarmCenter::RequestData(CMerch* pMerch /*= NULL*/)
{
	//
	if ( NULL == m_pCenterManager )
	{
		return;
	}

	//
	CArray<CMerch*, CMerch*> aMerchs;
	
	//
	if ( NULL == pMerch )
	{
		GetAttendMerchs(1, aMerchs);
	}
	else
	{
		aMerchs.Add(pMerch);
	}

	if ( aMerchs.GetSize() <= 0 )
	{
		return;
	}

	//
	CMmiReqRealtimePrice Req;
	
	Req.m_iMarketId		= aMerchs[0]->m_MerchInfo.m_iMarketId;
	Req.m_StrMerchCode	= aMerchs[0]->m_MerchInfo.m_StrMerchCode;

	int32 i = 0;
	for ( i = 1; i < aMerchs.GetSize(); i++ )
	{		
		CMerchKey MerchKey;
		MerchKey.m_iMarketId	= aMerchs[i]->m_MerchInfo.m_iMarketId;
		MerchKey.m_StrMerchCode	= aMerchs[i]->m_MerchInfo.m_StrMerchCode;
		Req.m_aMerchMore.Add(MerchKey);		
	}

	m_pCenterManager->RequestViewData(&Req);

	// 取本地的脱机K 线数据
	for ( i = 0; i < aMerchs.GetSize(); i++)
	{
		CMerch* pMerchNow = aMerchs[i];
		if ( NULL == pMerchNow )
		{
			continue;
		}

		//
		mapMerchAlarms::iterator itFind = m_mapAlarams.find(pMerchNow);
		if ( m_mapAlarams.end() == itFind )
		{
			continue;
		}

		//
		CAlarmConditions stToAdd = itFind->second;
		
		// 取脱机数据
		m_pCenterManager->GetDataManager()->ReadOfflineKLines(pMerchNow->m_MerchInfo.m_iMarketId, pMerchNow->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, stToAdd.m_aKLines);
		
		// 更新集合
		m_mapAlarams[pMerchNow] = stToAdd;
	}		
}

void CAlarmCenter::RequestAllArbitrageData()
{
	//
	if ( NULL == m_pCenterManager )
	{
		return;
	}
	
	//
	CArray<CMerch*, CMerch*> aMerchs;
	GetAttendMerchs(2, aMerchs);
	
	if ( aMerchs.GetSize() <= 0 )
	{
		return;
	}
	
	//
	CMmiReqRealtimePrice Req;
	
	Req.m_iMarketId		= aMerchs[0]->m_MerchInfo.m_iMarketId;
	Req.m_StrMerchCode	= aMerchs[0]->m_MerchInfo.m_StrMerchCode;
	
	for ( int32 i = 1; i < aMerchs.GetSize(); i++ )
	{		
		CMerchKey MerchKey;
		MerchKey.m_iMarketId	= aMerchs[i]->m_MerchInfo.m_iMarketId;
		MerchKey.m_StrMerchCode	= aMerchs[i]->m_MerchInfo.m_StrMerchCode;
		Req.m_aMerchMore.Add(MerchKey);		
	}
	
	m_pCenterManager->RequestViewData(&Req);
	
	// 取本地的脱机K 线数据
	for ( mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.begin(); it != m_mapArbitrageAlarams.end(); ++it )
	{
		CArbitrage* pstArbitrage = it->first;
		if ( NULL == pstArbitrage )
		{
			continue;
		}
		//
		CAlarmConditions stAlarmCondition = it->second;

		//
		CArray<CKLine, CKLine> aKLineA;
		CArray<CKLine, CKLine> aKLineB;
		CArray<CKLine, CKLine> aKLineC;
		CArbitrageManage::Merch2KLineMap mapKLines;

		//
		if ( NULL != pstArbitrage->m_MerchA.m_pMerch )
		{
			// 
			m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchA.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchA.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineA);
			mapKLines[pstArbitrage->m_MerchA.m_pMerch] = &aKLineA;
		}

		if ( NULL != pstArbitrage->m_MerchB.m_pMerch )
		{
			//
			m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchB.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchB.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineB);
			mapKLines[pstArbitrage->m_MerchB.m_pMerch] = &aKLineB;
		}

		if ( EABThreeMerch == pstArbitrage->m_eArbitrage )
		{
			if ( NULL != pstArbitrage->m_MerchC.m_pMerch )
			{
				//
				m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchC.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchC.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineA);
				mapKLines[pstArbitrage->m_MerchC.m_pMerch] = &aKLineC;
			}
		}

		// 合成K 线
		CArray<CKLine, CKLine> aKLineCombinA;
		CArray<CKLine, CKLine> aKLineCombinB;		

		
		CArbitrageManage::Instance()->CombineArbitrageKLine(*pstArbitrage, mapKLines, aKLineCombinA, aKLineCombinB, stAlarmCondition.m_aKLines);
		
		// 更新集合
		m_mapArbitrageAlarams[pstArbitrage] = stAlarmCondition;
	}
}

void CAlarmCenter::RequestArbitrageData(CArbitrage* pstArbitrage)
{
	//
	if ( NULL == m_pCenterManager->GetDataManager() || NULL == pstArbitrage )
	{
		return;
	}
	
	//
	CArray<CMerch*, CMerch*> aMerchs;
	if ( NULL != pstArbitrage->m_MerchA.m_pMerch )
	{
		aMerchs.Add(pstArbitrage->m_MerchA.m_pMerch);
	}

	if ( NULL != pstArbitrage->m_MerchB.m_pMerch )
	{
		aMerchs.Add(pstArbitrage->m_MerchB.m_pMerch);
	}

	if ( NULL != pstArbitrage->m_MerchC.m_pMerch )
	{
		aMerchs.Add(pstArbitrage->m_MerchC.m_pMerch);
	}
	
	//
	if ( aMerchs.GetSize() <= 0 )
	{
		return;
	}
	
	//
	CMmiReqRealtimePrice Req;
	
	Req.m_iMarketId		= aMerchs[0]->m_MerchInfo.m_iMarketId;
	Req.m_StrMerchCode	= aMerchs[0]->m_MerchInfo.m_StrMerchCode;
	
	for ( int32 i = 1; i < aMerchs.GetSize(); i++ )
	{		
		CMerchKey MerchKey;
		MerchKey.m_iMarketId	= aMerchs[i]->m_MerchInfo.m_iMarketId;
		MerchKey.m_StrMerchCode	= aMerchs[i]->m_MerchInfo.m_StrMerchCode;
		Req.m_aMerchMore.Add(MerchKey);		
	}
	
	m_pCenterManager->RequestViewData(&Req);
	
	// 取本地的脱机K 线数据
	mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.find(pstArbitrage);
	if ( m_mapArbitrageAlarams.end() == it )
	{
		////ASSERT(0);
		return;
	}

	//
	CAlarmConditions stAlarmCondition = it->second;
	
	//
	CArray<CKLine, CKLine> aKLineA;
	CArray<CKLine, CKLine> aKLineB;
	CArray<CKLine, CKLine> aKLineC;
	CArbitrageManage::Merch2KLineMap mapKLines;
	
	if ( NULL != pstArbitrage->m_MerchA.m_pMerch )
	{
		// 
		m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchA.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchA.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineA);
		mapKLines[pstArbitrage->m_MerchA.m_pMerch] = &aKLineA;
	}
	
	if ( NULL != pstArbitrage->m_MerchB.m_pMerch )
	{
		//
		m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchB.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchB.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineB);
		mapKLines[pstArbitrage->m_MerchB.m_pMerch] = &aKLineB;
	}
	
	if ( EABThreeMerch == pstArbitrage->m_eArbitrage )
	{
		if ( NULL != pstArbitrage->m_MerchC.m_pMerch )
		{
			//
			m_pCenterManager->GetDataManager()->ReadOfflineKLines(pstArbitrage->m_MerchC.m_pMerch->m_MerchInfo.m_iMarketId, pstArbitrage->m_MerchC.m_pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, aKLineA);
			mapKLines[pstArbitrage->m_MerchC.m_pMerch] = &aKLineC;
		}
	}
	
	// 合成K 线
	CArray<CKLine, CKLine> aKLineCombinA;
	CArray<CKLine, CKLine> aKLineCombinB;		
	CArbitrageManage::Instance()->CombineArbitrageKLine(*pstArbitrage, mapKLines, aKLineCombinA, aKLineCombinB, stAlarmCondition.m_aKLines);
	
	// 更新集合
	m_mapArbitrageAlarams[pstArbitrage] = stAlarmCondition;
}

void CAlarmCenter::CombinKLine(CMerch* pMerch)
{
	if ( NULL == pMerch )
	{
		return;
	}

	// 更新本地 K 线
	int32 iPosFound;
	CMerchKLineNode *pKLineRequest = NULL;
	
	if ( pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineRequest) )
	{
		if ( NULL == pKLineRequest || pKLineRequest->m_KLines.GetSize() <= 0 )
		{
			;
		}
		else
		{
			int32 iSizeSrc = pKLineRequest->m_KLines.GetSize();
			
			CAlarmConditions stLocal;
			GetMerchFixAlarms(pMerch, stLocal);
			int32 iSizeLocal = stLocal.m_aKLines.GetSize();
			
			//
			if ( iSizeLocal <= 0 )
			{
				stLocal.m_aKLines.Copy(pKLineRequest->m_KLines);
			}
			else
			{
				CKLine KLineLastSrc  = pKLineRequest->m_KLines.GetAt(iSizeSrc - 1);
				CGmtTime TimeLastSrc = KLineLastSrc.m_TimeCurrent;
				
				CKLine KLineLastLocal  = stLocal.m_aKLines.GetAt(iSizeLocal - 1);
				CGmtTime TimeLastLocal = KLineLastLocal.m_TimeCurrent;
				
				SaveDay(TimeLastSrc);
				SaveDay(TimeLastLocal);
				
				//
				if ( TimeLastLocal.GetTime() < TimeLastSrc.GetTime() )
				{
					// 当前有的数据最后一根时间小于最新的, 把最新一根加到最后
					stLocal.m_aKLines.Add(KLineLastSrc);
				}
				else if ( TimeLastLocal.GetTime() == TimeLastSrc.GetTime() )
				{
					// 两个时间相同, 用新的替换旧的
					stLocal.m_aKLines.SetAt(iSizeLocal - 1, KLineLastSrc);
				}
				else
				{
					// 不应该发生
					assert(0);
				}
				
				// 更新
				m_mapAlarams[pMerch] = stLocal;
			}
		}		
	}
}

void CAlarmCenter::NotifyAlarm(void* pData, CAlarmNotify::E_AlarmNotify eType)
{
	// 通知
	for ( int32 i = 0; i < m_aNotifys.GetSize(); i++ )
	{
		CAlarmNotify* pNotify = m_aNotifys[i];
		if ( NULL == pNotify )
		{
			continue;
		}
	
		pNotify->OnAlarmsChanged(pData, eType);
	}
}

void CAlarmCenter::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{
	if ( NULL == m_pCenterManager )
	{
		////ASSERT(0);
		return;
	}

	//
	CMerch* pMerch = NULL;
	m_pCenterManager->GetMerchManager().FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch);
	
	if ( NULL == pMerch )
	{
		return;
	}
	
	// 商品的报警
	{
		mapMerchAlarms::iterator itFind = m_mapAlarams.find(pMerch);
		if ( m_mapAlarams.end() != itFind )
		{
			CAlarmConditions stAlarmConditions = itFind->second;
			
			if ( iReqId > 0 && stAlarmConditions.m_bInitialCalc )
			{
				// 只在推送数据的时候计算报警
				return;
			}
			
			stAlarmConditions.m_bInitialCalc = true;
			m_mapAlarams[pMerch] = stAlarmConditions;
			
			// 更新本地 K 线
			CombinKLine(pMerch);
			
			// 看需要不需要报警
			CalcAlarmCdts(pMerch);
		}
	}
	
	// 套利的报警
	{
		CArray<CMerch*, CMerch*> aArbitrageMerchs;
		GetAttendMerchs(2, aArbitrageMerchs);
		
		bool32 bAttendMerch = false;
		for ( int32 i = 0; i < aArbitrageMerchs.GetSize(); i++ )
		{
			if ( pMerch == aArbitrageMerchs.GetAt(i) )
			{
				bAttendMerch = true;
				break;
			}
		}

		//
		if ( bAttendMerch )
		{
			CalcArbitrageAlarmCdts(pMerch);
		}		
	}	
}

void CAlarmCenter::OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew)
{
	for ( mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.begin(); it != m_mapArbitrageAlarams.end(); ++it )
	{
		CArbitrage* p = it->first;
		if ( NULL == p )
		{
			continue;
		}

		//
		if ( *p == stArbitrageNew )
		{
			ModifyArbitrageAlarms(p, it->second);
		}
	}
}

void CAlarmCenter::OnArbitrageDel(const CArbitrage& stArbitrage)
{
	CArbitrage* pDel = NULL;
	for ( mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.begin(); it != m_mapArbitrageAlarams.end(); ++it )
	{
		CArbitrage* p = it->first;
		if ( NULL == p )
		{
			continue;
		}
		
		//
		if ( *p == stArbitrage )
		{
			// 删掉
			pDel = p;
			break;
		}
	}
	
	//
	DelArbitrageAlarms(pDel);
}

void CAlarmCenter::AddMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions)
{
	if ( NULL == pMerch )
	{ 
		////ASSERT(0);
		return;
	}

	//
	m_mapAlarams[pMerch] = stAlarmCoditions;

	// 请求数据
	RequestData(pMerch);

	// 通知
	NotifyAlarm(pMerch, CAlarmNotify::EANAddMerch);
}
	
void CAlarmCenter::ModifyMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions)
{
	if ( NULL == pMerch )
	{ 
		////ASSERT(0);
		return;
	}
	
	//
	m_mapAlarams[pMerch] = stAlarmCoditions;

	// 通知
	NotifyAlarm(pMerch, CAlarmNotify::EANModifyMerch);
}

void CAlarmCenter::DelMerchAlarms(CMerch* pMerch)
{
	if ( NULL == pMerch )
	{ 
		////ASSERT(0);
		return;
	}
	
	//
	m_mapAlarams.erase(pMerch);

	// 通知
	NotifyAlarm(pMerch, CAlarmNotify::EANDelMerch);
}

void CAlarmCenter::AddArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions)
{
	//
	m_mapArbitrageAlarams[pstArbitrage] = stAlarmCoditions;
	
	// 请求数据
	RequestArbitrageData(pstArbitrage);
	
	// 通知
	NotifyAlarm(pstArbitrage, CAlarmNotify::EANAddArbitrage);
}

void CAlarmCenter::ModifyArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions)
{
	//
	m_mapArbitrageAlarams[pstArbitrage] = stAlarmCoditions;
	
	// 通知
	NotifyAlarm(pstArbitrage, CAlarmNotify::EANModifyArbitrage);
}

void CAlarmCenter::DelArbitrageAlarms(CArbitrage* pstArbitrage)
{
	m_mapArbitrageAlarams.erase(pstArbitrage);

	//
	// 通知
	NotifyAlarm(pstArbitrage, CAlarmNotify::EANDelArbitrage);
}

void CAlarmCenter::DelAllMerchAlarms()
{
	m_mapAlarams.clear();

	// 通知
	NotifyAlarm(NULL, CAlarmNotify::EANDelAllMerch);
}

void CAlarmCenter::DelAllArbitrageAlarms()
{
	m_mapArbitrageAlarams.clear();

	// 通知
	NotifyAlarm(NULL, CAlarmNotify::EANDelAllMerch);
}
	
void CAlarmCenter::AddFormularAlarms(CFormularContent* pFormular)
{
	if ( NULL == pFormular )
	{
		return;
	}

	//
	for ( int32 i = 0; i < m_aAlarmFormulars.GetSize(); i++ )
	{
		if ( m_aAlarmFormulars[i] == pFormular )
		{
			return;
		}
	}

	//
	m_aAlarmFormulars.Add(pFormular);

	// 通知
	NotifyAlarm(pFormular, CAlarmNotify::EANAddFormular);

	// 重算
	{
		for ( mapMerchAlarms::iterator it = m_mapAlarams.begin(); it != m_mapAlarams.end(); ++it )
		{
			CMerch* pMerch = it->first;
			if ( NULL != pMerch )
			{
				CalcAlarmCdts(pMerch);
			}
		}
	}
}

void CAlarmCenter::ModifyFormularAlarms(CFormularContent* pFormular)
{
	if ( NULL == pFormular )
	{
		return;
	}
	
	//
	for ( int32 i = 0; i < m_aAlarmFormulars.GetSize(); i++ )
	{
		if ( m_aAlarmFormulars[i] == pFormular )
		{
			m_aAlarmFormulars.RemoveAt(i);
			m_aAlarmFormulars.Add(pFormular);
			break;
		}
	}
		
	// 通知
	NotifyAlarm(pFormular, CAlarmNotify::EANModifyFormular);

	// 重算
	{
		for ( mapMerchAlarms::iterator it = m_mapAlarams.begin(); it != m_mapAlarams.end(); ++it )
		{
			CMerch* pMerch = it->first;
			if ( NULL != pMerch )
			{
				CalcAlarmCdts(pMerch);
			}
		}
	}
}

void CAlarmCenter::DelFormularAlarms(CFormularContent* pFormular)
{
	if ( NULL == pFormular )
	{
		return;
	}
	
	//
	for ( int32 i = 0; i < m_aAlarmFormulars.GetSize(); i++ )
	{
		if ( m_aAlarmFormulars[i] == pFormular )
		{
			m_aAlarmFormulars.RemoveAt(i);
			break;
		}
	}
	
	// 通知
	NotifyAlarm(pFormular, CAlarmNotify::EANDelFormular);
}

void CAlarmCenter::DelAllFormularAlarms()
{
	m_aAlarmFormulars.RemoveAll();

	// 通知
	NotifyAlarm(NULL, CAlarmNotify::EANDelAllFormular);
}

void CAlarmCenter::AddNotify(CAlarmNotify* pNotify)
{
	if ( NULL == pNotify )
	{
		return;
	}

	//
	for ( int32 i = 0; i < m_aNotifys.GetSize(); i++ )
	{
		if ( m_aNotifys[i] == pNotify )
		{
			return;
		}
	}

	//
	m_aNotifys.Add(pNotify);
}
	
void CAlarmCenter::DelNotify(CAlarmNotify* pNotify)
{
	if ( NULL == pNotify )
	{
		return;
	}
	
	//
	for ( int32 i = 0; i < m_aNotifys.GetSize(); i++ )
	{
		if ( m_aNotifys[i] == pNotify )
		{
			m_aNotifys.RemoveAt(i);
			return;
		}
	}	
}

void CAlarmCenter::GetAttendMerchs(int32 iFlag, OUT CArray<CMerch*, CMerch*>& aAttendMerchs)
{
	//
	aAttendMerchs.RemoveAll();

	//
	set<CMerch*> aMerchs;

	if ( 0 == iFlag || 1 == iFlag )
	{
		for ( mapMerchAlarms::iterator it = m_mapAlarams.begin(); it != m_mapAlarams.end(); ++it )
		{
			CMerch* pMerch = it->first;
			if ( NULL != pMerch )
			{
				aMerchs.insert(pMerch);
			}
		}
	}

	//
	if ( 0 == iFlag || 2 == iFlag )
	{
		for ( mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.begin(); it != m_mapArbitrageAlarams.end(); ++it )
		{
			CArbitrage* pst = it->first;
			if ( NULL == pst )
			{
				continue;
			}

			CArbitrage::ArbMerchArray aMerchArb;
			pst->GetOtherMerchs(aMerchArb, true);
			for ( int i=0; i < aMerchArb.size() ; ++i )
			{
				aMerchs.insert( aMerchArb[i] );
			}
		}
	}

	//
	for ( set<CMerch*>::iterator itMerch = aMerchs.begin(); itMerch != aMerchs.end(); ++itMerch )
	{
		CMerch* pMerch = *itMerch;
		if ( NULL == pMerch )
		{
			continue;
		}

		//
		aAttendMerchs.Add(pMerch);
	}
}

void CAlarmCenter::GetAlarmMap(OUT mapMerchAlarms& mapAlarms)
{
	//
	mapAlarms = m_mapAlarams;
}

void CAlarmCenter::GetArbitrageMap(OUT mapArbitrageAlarms& mapArbitrage)
{
	ArbitrageFromXml();
	mapArbitrage = m_mapArbitrageAlarams;
}

void CAlarmCenter::GetAlarmFormulars(OUT CArray<CFormularContent*, CFormularContent*>& aFormulars)
{
	aFormulars.Copy(m_aAlarmFormulars);
}

bool32 CAlarmCenter::GetMerchFixAlarms(IN CMerch* pMerch, OUT CAlarmConditions& stAlarmConditions)
{
	if ( NULL == pMerch )
	{
		return false;
	}

	//
	mapMerchAlarms::iterator itFind = m_mapAlarams.find(pMerch);
	if ( m_mapAlarams.end() == itFind )
	{
		return false;
	}

	//
	stAlarmConditions = itFind->second;

	//
	return true;
}

bool32	CAlarmCenter::GetArbitrageFixAlarms(IN CArbitrage* pArbitrage, OUT CAlarmConditions& stAlarmConditions)
{
	if ( NULL == pArbitrage )
	{
		return false;
	}

	//
	mapArbitrageAlarms::iterator itFind = m_mapArbitrageAlarams.find(pArbitrage);
	if ( m_mapArbitrageAlarams.end() == itFind )
	{
		return false;
	}

	//
	stAlarmConditions = itFind->second;

	//
	return true;
}

void CAlarmCenter::SetAlarmFlag(bool32 bAlarm)
{
	if ( m_bNeedAlarm == bAlarm )
	{
		return;
	}

	//
	m_bNeedAlarm = bAlarm;

	// 通知
	CAlarmNotify::E_AlarmNotify eType = CAlarmNotify::EANAlarmOn;
	if ( !m_bNeedAlarm )
	{
		eType = CAlarmNotify::EANAlarmOff;
	}

	NotifyAlarm(NULL, eType);

	// 如果关掉报警, 就取消ViewData
	if ( NULL != m_pCenterManager )
	{
		m_pCenterManager->RemoveAttendMerch(EA_Alarm);
		if ( m_bNeedAlarm )
		{
			m_eListenType = ECTReqRealtimePrice;
			m_pCenterManager->AddViewDataListner(this);	

			CArray<CMerch*,CMerch*> merchAarry;
			GetAttendMerchs(0,merchAarry);

			for (int i = 0; i < merchAarry.GetSize(); i++)
			{
				CMerch* pMerch = merchAarry[i];
				if ( NULL == pMerch )
				{
					continue;
				}

				CSmartAttendMerch SmartAttendMerch;
				SmartAttendMerch.m_pMerch = pMerch;
				SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;

				m_pCenterManager->AddAttendMerch(SmartAttendMerch,EA_Alarm);
			}

		}
		else
		{
			m_pCenterManager->DelViewDataListner(this);
		}
	}

	ToXml();
}

void CAlarmCenter::SetPromptFlag(bool32 bPrompt)
{
	m_bPrompt = bPrompt;

	// 通知
	CAlarmNotify::E_AlarmNotify eType = CAlarmNotify::EANPromptOn;
	if ( !m_bPrompt )
	{
		eType = CAlarmNotify::EANPromptOff;
	}
	
	NotifyAlarm(NULL, eType);

	ToXml();
}

void CAlarmCenter::SetAlarmType(E_AlarmType eType)
{
	 m_eAlarmType = eType; 
	 ToXml();
}

void CAlarmCenter::SetAlarmDlgType(bool32 bDlg)
{
	m_bShowDlg = bDlg;
	ToXml();
}

void CAlarmCenter::SetAlarmStatusType(bool32 bStatus)
{
	m_bShowStatus = bStatus;
	ToXml();
}

void CAlarmCenter::CalcAlarmCdts(CMerch* pMerch)
{
	if ( !m_bNeedAlarm )
	{
		return;
	}

	mapMerchAlarms::iterator itFind = m_mapAlarams.find(pMerch);
		
	if ( m_mapAlarams.end() == itFind )
	{
		return;
	}
		
	if ( NULL == pMerch->m_pRealtimePrice )
	{
		return;
	}

	// 
	CAlarmConditions* pstAlarmConditions = &itFind->second;
	CString StrPrompt = L"";
	
	// 具体的判断如下:
	{		
		if ( 0. != pMerch->m_pRealtimePrice->m_fPriceNew )
		{
			// 最新价不能等于 0 
			// 固定的几个条件
			float fPriceUp = pstAlarmConditions->m_aFixedParams[0];
			if ( 0. != fPriceUp && pMerch->m_pRealtimePrice->m_fPriceNew >= fPriceUp )
			{
				// 上破 xx 元
				StrPrompt = L"价位上破";
				goto ALARM_NOW;
			}
			
			//
			float fPriceLow = pstAlarmConditions->m_aFixedParams[1];
			if ( (0. != fPriceLow) && (pMerch->m_pRealtimePrice->m_fPriceNew <= fPriceLow) )
			{
				// 下破 xx 元
				StrPrompt = L"价位下破";
				goto ALARM_NOW;
			}

			float fRiseRate = pstAlarmConditions->m_aFixedParams[2];

			// 获取昨收价
			float fPricePrevClose = pMerch->m_pRealtimePrice->m_fPricePrevClose;
			float fPricePrevAvg = pMerch->m_pRealtimePrice->m_fPricePrevAvg;
			float fPrevReferPrice = fPricePrevClose;

			//
			if ( ERTFuturesCn == pMerch->m_Market.m_MarketInfo.m_eMarketReportType )
			{
				// 仅国内期货需要昨结
				fPrevReferPrice = fPricePrevAvg;
			}

			float fNewRiseRate = (pMerch->m_pRealtimePrice->m_fPriceNew - pMerch->m_pRealtimePrice->m_fPricePrevClose) / fPrevReferPrice * 100;
			
			if ( (0. != fRiseRate) && (fNewRiseRate >= fRiseRate) )
			{
				// 涨幅
				StrPrompt = L"涨幅";
				goto ALARM_NOW;
			}

			float fChangeRate = pstAlarmConditions->m_aFixedParams[3];
			if ( (0. != fChangeRate) && (pMerch->m_pRealtimePrice->m_fTradeRate >= fChangeRate) )
			{
				// 换手率
				StrPrompt = L"换手率";
				goto ALARM_NOW;
			}
			
			//
			bool32 bJudgeTrend = pstAlarmConditions->m_aFixedParams[4] == 1.0 ? true : false;
			if ( bJudgeTrend && fabsf(pMerch->m_pRealtimePrice->m_fPriceNew - pMerch->m_pRealtimePrice->m_fPriceAvg) <= 0.003 )
			{
				// 分时线与均线交叉
				StrPrompt = L"分时交叉";
				goto ALARM_NOW;
			}
			
// 			//
// 			float fYijiaRate = pstAlarmConditions->m_aFixedParams[4];
// 			if ( 0/*(0. != fYijiaRate) && fangz0303*/)
// 			{
// 				// 溢价率
// 				StrPrompt = L"溢价率";
// 				goto ALARM_NOW;
// 			}
		}
		
		// 公式条件
		for ( int32 i = 0; i < m_aAlarmFormulars.GetSize(); i++ )
		{
			CFormularContent* pFormular = m_aAlarmFormulars.GetAt(i);
			if ( NULL == pFormular )
			{
				continue;
			}
			
			// 计算指标
			T_IndexOutArray* pResult = formula_index(pFormular, pstAlarmConditions->m_aKLines);
			
			//
			if ( NULL == pResult )
			{
				continue;
			}
			
			//
			if ( 1 != pResult->iIndexNum )
			{
				////ASSERT(0);
				continue;
			}
			
			//
			int32 iPointNum = pResult->index[0].iPointNum;
			if ( pResult->index[0].pPoint[iPointNum - 1] != 1.0 )
			{
				//
				StrPrompt = pFormular->name;
				goto ALARM_NOW;
			}
			
		}	
	}

	return;

ALARM_NOW:
	{
		// 报警		
		if ( GetAlarmFlag() && GetPromptFlag() )
		{
			if ( CAlarmCenter::EATNoSound == m_eAlarmType )
			{
				;
			}
			else if ( CAlarmCenter::EATSysSound == m_eAlarmType )
			{
				MessageBeep(MB_ICONEXCLAMATION);
			}
			else if ( CAlarmCenter::EATCustomSound == m_eAlarmType )
			{
				CString strUserName;
				if (m_pCenterManager)
				{
					strUserName = m_pCenterManager->GetUserName();
				}
				CString StrPath = CPathFactory::GetCustomAlarmPath(strUserName);
				if ( StrPath.IsEmpty() )
				{
					return;
				}

				//
				PlaySound(StrPath, NULL, SND_ASYNC|SND_FILENAME|SND_NOSTOP);			
			}
		}
		
		for ( int j = m_aNotifys.GetSize()-1; j >=0 ; j-- )
		{
			CAlarmNotify* pNotify = m_aNotifys[j];
			if ( NULL == pNotify )
			{
				continue;
			}
			
			//
			pNotify->Alarm(pMerch, StrPrompt);
		}
	}	
}

void CAlarmCenter::CalcArbitrageAlarmCdts(CMerch* pMerch)
{
	if ( !m_bNeedAlarm || NULL == pMerch )
	{
		return;
	}

	//
	CArray<CArbitrage, CArbitrage&> aArbitrages;
	CArbitrageManage::Instance()->GetArbitrageDatasByMerch(pMerch, aArbitrages);
	
	//
	for ( int32 i = 0; i < aArbitrages.GetSize(); i++ )
	{		
		for ( mapArbitrageAlarms::iterator it = m_mapArbitrageAlarams.begin(); it != m_mapArbitrageAlarams.end(); ++it )
		{
			CArbitrage* pst = it->first;
			if ( NULL == pst )
			{
				continue;
			}

			//
			CArbitrage stArbitrage = aArbitrages.GetAt(i);

			//
			if ( stArbitrage == (*pst) )
			{
				//
				CAlarmConditions* pstAlarmConditions = &it->second;
				
				// 
				CString StrPrompt = L"";

				// 最新价差作为报警条件
				float fPriceNew = stArbitrage.GetPrice(EAHPriceDifNew);
				
				// 具体的判断如下:
				{		
					if ( 0. != fPriceNew )
					{
						// 最新价不能等于 0 
						// 固定的几个条件
						float fPriceUp = pstAlarmConditions->m_aFixedParams[0];
						if ( 0. != fPriceUp && fPriceNew >= fPriceUp )
						{
							// 上破 xx 元
							StrPrompt = L"价位上破";
							goto ALARM_NOW;
						}
						
						//
						float fPriceLow = pstAlarmConditions->m_aFixedParams[1];
						if ( (0. != fPriceLow) && (fPriceNew <= fPriceLow) )
						{
							// 下破 xx 元
							StrPrompt = L"价位下破";
							goto ALARM_NOW;
						}
					}
					
					continue;
				}
ALARM_NOW:
				{
					// 报警		
					if ( GetAlarmFlag() && GetPromptFlag() )
					{
						if ( CAlarmCenter::EATNoSound == m_eAlarmType )
						{
							;
						}
						else if ( CAlarmCenter::EATSysSound == m_eAlarmType )
						{
							MessageBeep(MB_ICONEXCLAMATION);
						}
						else if ( CAlarmCenter::EATCustomSound == m_eAlarmType )
						{
							CString strUserName;
							if (m_pCenterManager)
							{
								strUserName = m_pCenterManager->GetUserName();
							}
							CString StrPath = CPathFactory::GetCustomAlarmPath(strUserName);
							if ( StrPath.IsEmpty() )
							{
								return;
							}
							
							//
							PlaySound(StrPath, NULL, SND_ASYNC|SND_FILENAME|SND_NOSTOP);	
						}
					}
					
					for ( int j = m_aNotifys.GetSize()-1; j >=0 ; j-- )
					{
						CAlarmNotify* pNotify = m_aNotifys[j];
						if ( NULL == pNotify )
						{
							continue;
						}
						
						//
						pNotify->AlarmArbitrage(pst, StrPrompt);
					}
				}				 
			}
		}
	}
}

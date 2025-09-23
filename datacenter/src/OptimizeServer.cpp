// OptimizeServer.cpp: implementation of the COptimizeServer class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "viewdata.h"
#include "OptimizeServer.h"
#include "coding.h"
#include "synch.h"
#include <process.h>
#include "TraceLog.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const char*	KStrElementNameAuto			= "ServerInfoAuto";
static const char*	KStrElementNameManual		= "ServerInfoManual";
static const char*	KStrElementNameErr			= "ServerErr";

static const char*  KStrElementNameServer		= "Server";
static const char*	KStrElementAttriName		= "Name";
static const char*	KStrElementAttriIp			= "Ip";
static const char*	KStrElementAttriPort		= "Port";
static const char*	KStrElementAttriMaxOline	= "MaxOline";
static const char*	KStrElementAttriGroup		= "Group";
static const char*	KStrElementAttriType		= "Type";
static const char*	KStrElementAttriPing		= "Ping";

static const float	KfWeightPing				= (float)0.5;
static const float	KfWeightType				= (float)0.5;

// static const float	KfWeightGroup				= (float)3 / 40;
// static const float	KfWeightOline				= (float)1 / 40;
// static const float	KfWeightMaxOline			= (float)1 / 40;

// �ݶ���һЩ����ֵ:
static const int32 KiPingMax					= 1000; 
static const int32 KiGroupMax					= 3;
static const int32 KiOlineUserMax				= 1000;
static const int32 KiMaxOlineUserMax			= 5000;

static const float KfFreeServerRate				= 0.1f;

		

// ��������������
static const float KfMaxServerLoad				= 0.9f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptimizeServer* COptimizeServer::m_pThis = NULL;

//
COptimizeServer::COptimizeServer()
{
	m_pThis = NULL;	
	m_pViewData = NULL;
}

COptimizeServer::~COptimizeServer()
{
	// �ͷ��ڴ�

	// Xml �ĵ���ָ��
	DEL(m_pXmlDoc);

	//
	m_apNotify.RemoveAll();

	//
	m_hOwnWnd = NULL;

	//
	m_aServersFromAuth.RemoveAll();
	m_aServersFromXmlManual.RemoveAll();
	m_aServersFromXmlAuto.RemoveAll();
	m_aErrServer.RemoveAll();
}

void COptimizeServer::Construct()
{
	m_pXmlDoc = NULL;
	m_bForcePing	 = false;
	m_bPingCompleted = false;
	m_bSortCompleted = false;
	m_bLocalAndAuthIdentical = false;
	
	m_aServersFromXmlAuto.RemoveAll();
	m_aServersFromXmlManual.RemoveAll();
	m_aServersFromAuth.RemoveAll();
	m_aServersFromAuth.RemoveAll();
	m_aServerPingInfo.RemoveAll();

	//
	if ( !GetServerInfoFromXml() )
	{
		ASSERT(0);;
	}

	//
	GetErrServer();
}

COptimizeServer* COptimizeServer::Instance()
{
	if ( NULL == m_pThis )
	{
		m_pThis = new COptimizeServer();
		if ( NULL == m_pThis )
		{
			ASSERT(0);
			return NULL;
		}

		m_pThis->Construct();
	}

	return m_pThis;
}


void COptimizeServer::DelInstance()
{
	delete m_pThis;
	m_pThis = NULL;
}

void COptimizeServer::AddNotify(COptimizeServerNotify* pNotify)
{
	if ( NULL == pNotify )
	{
		return;
	}

	for ( int32 i = 0; i < m_apNotify.GetSize(); i++ )
	{
		if ( pNotify == m_apNotify[i] )
		{
			return;
		}
	}

	//
	m_apNotify.Add(pNotify);
}

void COptimizeServer::RemoveNotify(COptimizeServerNotify* pNotify)
{
	if ( NULL == pNotify )
	{
		return;
	}

	for ( int32 i = 0; i < m_apNotify.GetSize(); i++ )
	{
		if ( pNotify == m_apNotify[i] )
		{
			m_apNotify.RemoveAt(i);
			return;
		}
	}
}

void COptimizeServer::SetNetType(E_NetType eNetType)
{
	m_eNetType = eNetType;
}

void COptimizeServer::SetSortMode(bool32 bAuto)
{
	m_bAutoOptimize = bAuto;
}

bool32 COptimizeServer::BeAutoSortMode()
{
	return m_bAutoOptimize;
}

void COptimizeServer::T_ServerInfo2ServerState(IN const T_ServerInfo& serIn, OUT CServerState& serOut)
{
	serOut.m_StrServerName		= serIn.wszName;
	serOut.m_StrServerAddr		= serIn.wszAddr;
	
	serOut.m_iServerID			= serIn.iID;
	serOut.m_uiServerPort		= serIn.iPort;
	serOut.m_uiServerHttpPort	= serIn.iHttpPort;
	
	serOut.m_iOnLineCount		= serIn.iOnLineCount;
	serOut.m_iMaxCount			= serIn.iMaxCount;
	serOut.m_iGroup				= serIn.iGroup;
	serOut.m_iNetType			= serIn.iSerNetID;
}

void COptimizeServer::SetSortParams(const T_ServerInfo* pServerFromAuth, int32 iServerCounts)
{
	_TESTTRACE(L"�������������: \n");

	//
	if ( NULL == pServerFromAuth || 0 >= iServerCounts )
	{
		return;
	}

	m_aServersFromAuth.RemoveAll();

	//
	for ( int32 i = 0; i < iServerCounts; i++ )
	{
		T_ServerInfo stServer = pServerFromAuth[i];
		CServerState stLocalServer;

		//
		T_ServerInfo2ServerState(stServer, stLocalServer);
		m_aServersFromAuth.Add(stLocalServer);

		//
		SetLatestOnlineData(stLocalServer);

		_TESTTRACE(L"������%d : %s:%d \n", i+1, stServer.wszAddr, stServer.iPort);
	}

	// �ж��Ƿ�һ��
	m_bLocalAndAuthIdentical = JudgeLocalAndAuthServers();
	_TESTTRACE(L"�Զ�ѡ�� = %d ���غͷ������Ƿ�һ��: %d \n", m_bAutoOptimize, m_bLocalAndAuthIdentical);	
}

void COptimizeServer::DelNonConnectServer(const T_ServerInfo* pServerNonConnect, int32 iServerCounts)
{	
	// ɾ�����Ӳ��ϵķ�����, ��Ҫ������ѡ
	if ( NULL == pServerNonConnect || iServerCounts <= 0 )
	{
		return;
	}

	// 
	for ( int32 i = 0; i < iServerCounts; i++ )
	{
		const T_ServerInfo* pServerDel = &pServerNonConnect[i];
		if ( NULL == pServerDel )
		{
			continue;
		}
		
		//
		for ( int32 j = m_aServersFromAuth.GetSize() - 1; j >= 0; j-- )
		{
			//
			CServerState ServerStateLocal = m_aServersFromAuth[j];

			//
			if ( 0 == lstrcmp(pServerDel->wszAddr, ServerStateLocal.m_StrServerAddr) && (pServerDel->iPort == ServerStateLocal.m_uiServerPort) )
			{
				// ɾ�����������:
				m_aServersFromAuth.RemoveAt(j);
				//
				_TESTTRACE(L"ȥ�����Ӳ��ϵ����������: %s:%d \n", ServerStateLocal.m_StrServerAddr.GetBuffer(), ServerStateLocal.m_uiServerPort);
				ServerStateLocal.m_StrServerAddr.ReleaseBuffer();
			}
		}
	}
}

int32 COptimizeServer::CalcServerScore(const CServerState& stServerState)
{
	// ����������÷�
	float fScore = 0.;
	
	// Ping
	float fScorePing = 100 * ((float)stServerState.m_uiPingValue / KiPingMax);
	
	// Ping Խ�ͷ�Խ��
	fScorePing = 100.f - fScorePing;
	
	fScorePing = fScorePing < 0 ? 0 : fScorePing;
	fScorePing = fScorePing > 100 ? 100 : fScorePing;
	
	// Type	
	float fScoreType = 0.;
	
	if ( m_eNetType == (E_NetType)stServerState.m_iNetType )
	{
		fScoreType = 100.;
	}
	
	//
	fScore = ((float)fScorePing * KfWeightPing)	+ ((float)fScoreType * KfWeightType);
	
	int32 iScoreRev = (int32)fScore;
	
	CString strTempServerAddr = stServerState.m_StrServerAddr;
	_TESTTRACE(L"====> ������ %s:%d �÷�: ping: %.2f ��ping = %u��type: %.2f TOTAL: %d\n", 
		strTempServerAddr.GetBuffer(), stServerState.m_uiServerPort,
		fScorePing, stServerState.m_uiPingValue, fScoreType, iScoreRev);
	//
	strTempServerAddr.ReleaseBuffer();

	return iScoreRev;
}

// int32 COptimizeServer::CalcServerScore(const CCommunication* pServer)
// {
// 	// ����������÷�
// 	if ( NULL == pServer )
// 	{
// 		ASSERT(0);
// 		return -1;
// 	}
// 	
// 	//
// 	CServerState stServerState;
// 	((CCommunication*)pServer)->GetServerState(stServerState);
// 
// 	//
// 	if ( stServerState.m_uiPingValue >= KiPingMax )
// 	{
// 		// ��ʱ�����ϵ�ֱ�ӷ���
// 		// return 0;
// 	}
// 
// 	//
// 	float fScore = 0.;
// 
// 	// Ping
// 	float fScorePing = 100 * ((float)pServer->m_uiPingValue / KiPingMax);
// 		
// 	// Ping Խ�ͷ�Խ��
// 	fScorePing = 100.f - fScorePing;
// 
// 	fScorePing = fScorePing < 0 ? 0 : fScorePing;
// 	fScorePing = fScorePing > 100 ? 100 : fScorePing;
// 	
// 	// Type	
// 	float fScoreType = 0.;
// 
// 	if ( m_eNetType == (E_NetType)stServerState.m_iNetType )
// 	{
// 		fScoreType = 100.;
// 	}
// 
// 	// Group	
// 	float fScoreGroup = 100 * ((float)stServerState.m_iGroup / KiGroupMax);
// 
// 	fScoreGroup = fScoreGroup < 0 ? 0 : fScoreGroup;
// 	fScoreGroup = fScoreGroup > 100 ? 100 : fScoreGroup;
// 
// 	// OlineUser
// 	float fScoreOlineUser = 100 * ((float)stServerState.m_iOnLineCount / KiOlineUserMax);
// 
// 	fScoreOlineUser = fScoreOlineUser < 0 ? 0 : fScoreOlineUser;
// 	fScoreOlineUser = fScoreOlineUser > 100 ? 100 : fScoreOlineUser;
// 
// 	// ������Խ�ٷ�Խ��
// 	fScoreOlineUser = 100.0f - fScoreOlineUser;
// 
// 	// MaxOline
// 	float fScoreMaxOlineUser = 100 * ((float)stServerState.m_iMaxCount/ KiMaxOlineUserMax);
// 	
// 	fScoreMaxOlineUser = fScoreMaxOlineUser < 0 ? 0 : fScoreMaxOlineUser;
// 	fScoreMaxOlineUser = fScoreMaxOlineUser > 100 ? 100 : fScoreMaxOlineUser;
// 
// 	if ( (float)stServerState.m_iOnLineCount / (float)stServerState.m_iMaxCount < KfFreeServerRate )
// 	{
// 		// ���������ɺ�С��ʱ��, ������������������, ��������
// 		fScoreOlineUser		= 100.0f;
// 		fScoreMaxOlineUser	= 100.0f;
// 	}
// 
// 	//
// 	fScore = ((float)fScorePing * KfWeightPing)	  + ((float)fScoreType * KfWeightType)
// 		   + ((float)fScoreGroup * KfWeightGroup) + ((float)fScoreOlineUser * KfWeightOline)   
// 		   + ((float)fScoreMaxOlineUser * KfWeightMaxOline);
// 	
// 	int32 iScoreRev = (int32)fScore;
// 
// 	_TESTTRACE(L"====> ������ %s:%d �÷�: ping: %.2f ��ping = %u��type: %.2f group: %.2f olineuser: %.2f maxoline: %.2f total: %d \n", stServerState.m_StrServerAddr, stServerState.m_uiServerPort,
// 		  fScorePing, stServerState.m_uiPingValue, fScoreType, fScoreGroup, fScoreOlineUser, fScoreMaxOlineUser, iScoreRev);
// 	//
// 	return iScoreRev;
// }

bool32 COptimizeServer::JudgeLocalAndAuthServers()
{		
	if ( m_bAutoOptimize )
	{
		// �Զ���
		if ( m_aServersFromAuth.GetSize() != m_aServersFromXmlAuto.GetSize() )
		{
			// ���ߵķ�������������ͬ, ֱ���жϲ�һ��
			return false;
		}

		//
		for ( int32 i = 0; i < m_aServersFromXmlAuto.GetSize(); i++ )
		{
			bool32 bFind = false;

			//
			CServerState stServerLocal = m_aServersFromXmlAuto[i];
			
			//
			for ( int32 j = 0; j < m_aServersFromAuth.GetSize(); j++ )
			{
				CServerState stServerAuth = m_aServersFromAuth[j];

				if ( BeServerIdentical(stServerLocal, stServerAuth) )
				{
					bFind = true;
				}
			}

			//
			if ( !bFind )
			{
				// ��һ̨������û���Ϻ�, ���ж���һ��
				return false;
			}
		}
	}
	else
	{
		// �ֶ���
		if ( m_aServersFromAuth.GetSize() != m_aServersFromXmlManual.GetSize() )
		{
			return false;
		}

		//
		for ( int32 i = 0; i < m_aServersFromXmlManual.GetSize(); i++ )
		{
			bool32 bFind = false;
			
			//
			CServerState stServerLocal = m_aServersFromXmlManual[i];
						
			//
			for ( int32 j = 0; j < m_aServersFromAuth.GetSize(); j++ )
			{
				CServerState stServerAuth = m_aServersFromAuth[j];
				
				//
				if ( BeServerIdentical(stServerLocal, stServerAuth) )
				{
					bFind = true;
				}
			}
			
			//
			if ( !bFind )
			{
				// ��һ̨������û���Ϻ�, ���ж���һ��
				return false;
			}
		}
	}

	return true;
} 

CString COptimizeServer::GetNetTypeString(E_NetType eNetType, bool32 bDetail /*= false*/)
{
	CString StrRev;

	if ( ENTDianxin == eNetType )
	{
		StrRev = L"����";

		if ( bDetail )
		{
			StrRev += L"(���, ADSL������)";
		}
	}
	else if ( ENTWangtong == eNetType )
	{
		StrRev = L"��ͨ";
		
		if ( bDetail )
		{
			StrRev += L"(���, ADSL������)";
		}
	}
	else if ( ENTYidong == eNetType )
	{
		StrRev = L"�ƶ�";

		if ( bDetail )
		{
			StrRev += L"(���, ADSL������)";
		}
	}
	else if ( ENTLiantong == eNetType )
	{
		StrRev = L"��ͨ";

		if ( bDetail )
		{
			StrRev += L"(���, ADSL������)";
		}
	}
	else
	{
		StrRev = L"����";

		if ( bDetail )
		{
			StrRev += L"(����ͨ, ��ͨ��)";
		}
	}

	return StrRev;
}

bool32 COptimizeServer::BeLocalAndAuthServersIdentical()
{
	return m_bLocalAndAuthIdentical;
}

bool32 COptimizeServer::BeSortCompleted()
{
	return m_bSortCompleted;
}

void COptimizeServer::GetSortedServers(OUT arrServer& aServers)
{
	aServers.Copy(m_aServersAftSort);
}

bool32 COptimizeServer::SortQuoteServersByXmlFile(IN arrServer& aServerIN, OUT arrServer& aServerOUT)
{
	//
	if ( aServerIN.GetSize() <= 0 )
	{
		ASSERT(0);
		return false;
	}
	
	//
	aServerOUT.RemoveAll();
	bool32 bOK = SortServers(aServerIN, aServerOUT);
	if (bOK)
	{
		// .....
	}

	// �������
	for (int32 i = 0; i < aServerOUT.GetSize(); i++ )
	{
		CServerState& stServerAftSort = aServerOUT[i];
		stServerAftSort.m_iSortIndex = i;
		
		_TESTTRACE(L"XML���� ������: %s:%d ���: %d �÷�: %d \n", stServerAftSort.m_StrServerAddr.GetBuffer(), stServerAftSort.m_uiServerPort, stServerAftSort.m_iSortIndex, stServerAftSort.m_iSortScore);
		stServerAftSort.m_StrServerAddr.ReleaseBuffer();
	}

	return true;
}

bool32 COptimizeServer::SortQuoteServersByPing()
{
	//
	m_bPingCompleted = false;
	
	// ���ó�ʱ�Ķ�ʱ��	
	StartMyTimer();

	// ����ping			
	PingAllServers();

	return true;
}

bool32 COptimizeServer::SortQuoteServers()
{
	m_bSortCompleted = false;

	//
	m_aServersAftSort.RemoveAll();
	
	// 
	if ( m_bLocalAndAuthIdentical && !m_bForcePing )
	{
		// һ���Ļ�, ȡ��ͬ�� xml �ڵ��������Ϣ���������
		if ( m_bAutoOptimize )
		{
			SortQuoteServersByXmlFile(m_aServersFromXmlAuto, m_aServersAftSort);
			
			// ֪ͨ�������
			OnSortCompleted();
		}
		else 
		{
			// �ֶ���
			SortQuoteServersByXmlFile(m_aServersFromXmlManual, m_aServersAftSort);
		
			SendMessage(m_pViewData->m_OwnerWnd, UM_MainFrame_ManualOptimize, 0, (LPARAM)&m_aServersAftSort);
		}
	}
	else
	{
		// ��һ������Ҫ�� auth �����ķ���������ping �����
		SortQuoteServersByPing();
	}

	//
	return true;
}

void COptimizeServer::SetManualSortResult(IN const arrServer& aServers)
{
	m_aServersAftSort.Copy(aServers);
	
	// �������
	OnSortCompleted();

	// ֪ͨping ����
	for ( int32 i = 0; i < m_apNotify.GetSize(); i++ )
	{
		COptimizeServerNotify* pNotify = m_apNotify[i];
		if ( NULL != pNotify )
		{		
			pNotify->OnFinishAllPing(); 
		}
	}
}

bool32 COptimizeServer::GetForcePingFlag()
{
	return m_bForcePing;
}

void COptimizeServer::SetForcePingFlag(bool32 bForcePing)
{
	m_bForcePing = bForcePing;
}

bool32 COptimizeServer::RequestNetTest(IN const CMmiReqNetTest *pMmiReqNetTest, IN int32 iCommunicationID, OUT int32& iReqID)
{
	if ( NULL == pMmiReqNetTest || 0 >= iCommunicationID )
	{
		ASSERT(0);
		return false;
	}

	if (m_pViewData == NULL)
	{
		return false;
	}


	CArray<CMmiReqNode*, CMmiReqNode*> aMmiReqNodes;
	int32 iReVal = m_pViewData->m_pDataManager->RequestData((CMmiCommBase *)pMmiReqNetTest, aMmiReqNodes, EDSCommunication, iCommunicationID);	
	
	if ( (iReVal < 0) || (1 != aMmiReqNodes.GetSize()) )
	{
		// �������
		ASSERT(0);
		return false;
	}
	
	// ID ��
	iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;

	//
	return true;
}

void COptimizeServer::OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest)
{
	if ( m_bPingCompleted )
	{
		// �Ѿ���ʱ����, ����Ҳû��
		return;
	}

	// ping ������
	int32 iCountPingComeleted = 0;

	for ( int32 i = 0; i < m_aServerPingInfo.GetSize(); i++ )
	{
		T_ServerSortPing& stServerSortPing = m_aServerPingInfo[i];
		
		if ( stServerSortPing.m_iReqID == iMmiReqId )
		{
			stServerSortPing.m_bCompeleted	= true;
			stServerSortPing.m_bTimeOut		= false;
			stServerSortPing.m_TimeResp		= pMmiRespNetTest->m_uiTimeResp;
			
			stServerSortPing.m_stServer.m_uiPingValue = stServerSortPing.m_TimeResp - stServerSortPing.m_TimeReq;
			
			if ( stServerSortPing.m_stServer.m_uiPingValue <= 0 )
			{
				// ����Ͳ�Ӧ����
				ASSERT(0);
				stServerSortPing.m_stServer.m_uiPingValue = 50;
			}

			// ֪ͨ
			for ( int32 j = 0; j < m_apNotify.GetSize(); j++ )
			{
				if ( NULL != m_apNotify[j] )
				{
					m_apNotify[j]->OnFinishPingServer(stServerSortPing.m_stServer);
				}
			}

			_TESTTRACE(L"ping �ذ�, ID = %d, ʱ�� = %d , ping = %d \n", iMmiReqId, stServerSortPing.m_TimeResp, stServerSortPing.m_stServer.m_uiPingValue);
		}

		if ( stServerSortPing.m_bCompeleted )
		{
			++iCountPingComeleted;
		}
	}

	if ( iCountPingComeleted == m_aServerPingInfo.GetSize() && !m_bPingCompleted )
	{		
		_TESTTRACE(L"ping ȫ�������� \n");
		OnPingCompleted();		
	}
}

void COptimizeServer::ForceRePingServer()
{
	if ( m_bAutoOptimize )
	{
		// �϶����ֶ���ʱ��
		ASSERT(0);
		return;
	}

	//
	m_aServersAftSort.RemoveAll();

	// ���·� ping ����
	SortQuoteServersByPing();
}

int32 COptimizeServer::GetMaxPingValue()
{
	return KiPingMax;
}

bool32 COptimizeServer::GetServerInfo(const TiXmlElement* pElement, arrServer& aServers)
{
	if ( NULL == pElement )
	{
		ASSERT(0);
		return false;
	}

	aServers.RemoveAll();

	//
	TiXmlElement* pChild = (TiXmlElement*)pElement->FirstChildElement();

	while (pChild)
	{
		const char* StrName		= pChild->Attribute(KStrElementAttriName);
		const char* StrIp		= pChild->Attribute(KStrElementAttriIp);
		const char* StrPort		= pChild->Attribute(KStrElementAttriPort);
		const char* StrMaxOline = pChild->Attribute(KStrElementAttriMaxOline);
		const char* StrGroup	= pChild->Attribute(KStrElementAttriGroup);
		const char* StrType		= pChild->Attribute(KStrElementAttriType);
		const char* StrPing		= pChild->Attribute(KStrElementAttriPing);

		// ����
		TCHAR TStrName[1024] = {0};		
		if (NULL != StrName)
		{
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrName, sizeof(TStrName) / sizeof(TCHAR));
		}
		
		// Ip
		TCHAR TStrIp[1024] = {0};		
		if (NULL != StrIp)
		{
			MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrIp, sizeof(TStrIp) / sizeof(TCHAR));
		}
		
		// �˿�
		int32 iPort = -1;
		if (NULL != StrPort)
		{
			iPort = atoi(StrPort);
		}
		
		// ���������
		int32 iMaxOline = -1;
		if (NULL != StrMaxOline)
		{
			iMaxOline = atoi(StrMaxOline);
		}
		
		// ��
		int32 iGroup = -1;
		if (NULL != StrGroup)
		{
			iGroup = atoi(StrGroup);
		}
		
		// ����
		//	modify by weng.cx
		int32 iType = -1;
		if (NULL != StrType)
		{
			iType = atoi(StrType);
		}
		
		// ping
		int32 iPing = 0;
		
		if (NULL != StrPing)
		{
			iPing = atoi(StrPing);
		}
		
		if ( iPing <= 0 )
		{
			iPing = KiPingMax;
		}

		CServerState stServerState;
	
		stServerState.m_StrServerName = TStrName;
		stServerState.m_StrServerAddr = TStrIp;
		stServerState.m_uiServerPort = iPort;
		stServerState.m_iMaxCount = iMaxOline;
		stServerState.m_iGroup = iGroup;
		stServerState.m_iNetType = iType;
		stServerState.m_uiPingValue = iPing;
		
		//		
		aServers.Add(stServerState);
		pChild = pChild->NextSiblingElement();
	}

	return true;
}

bool32 COptimizeServer::BeServerIdentical(const CServerState& ServerState1, const CServerState& ServerState2)
{
	// ����, ��ַ, �˿�, ���������, ��, �������� ֻҪ��һ����һ�����жϲ�һ��

	if ( ServerState1.m_StrServerName != ServerState2.m_StrServerName
	  || ServerState1.m_StrServerAddr != ServerState2.m_StrServerAddr
	  || ServerState1.m_uiServerPort  != ServerState2.m_uiServerPort
	  || ServerState1.m_iMaxCount	  != ServerState2.m_iMaxCount
	  || ServerState1.m_iGroup		  != ServerState2.m_iGroup
	  || ServerState1.m_iNetType	  != ServerState2.m_iNetType)
	{
		return false;
	}

	return true;
}

bool32 COptimizeServer::GetServerInfoFromXml()
{
	CString StrConfigFile = CPathFactory::GetSortServerPath();
	
	if ( StrConfigFile.GetLength() <= 0 )
	{
		return false;
	}
	
	//

	std::string StrConfigFileA ;
	Unicode2MultiChar(CP_ACP, StrConfigFile, StrConfigFileA);

	DEL(m_pXmlDoc);

	m_pXmlDoc = new TiXmlDocument(StrConfigFileA.c_str());
	if ( NULL == m_pXmlDoc )
	{		
		return false;
	}
	
	if ( !m_pXmlDoc->LoadFile(StrConfigFileA.c_str()) )
	{		
		return false;
	}
	
	// �����
	m_aServersFromXmlAuto.RemoveAll();
	m_aServersFromXmlManual.RemoveAll();
	
	// ���ڵ�
	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	TiXmlElement* pElementChild = pRootElement->FirstChildElement();
	
	while (pElementChild)
	{
		if ( 0 == strcmp(pElementChild->Value(), KStrElementNameAuto) )
		{
			// ȡ�Զ�����ķ�������Ϣ
			m_aServersFromXmlAuto.RemoveAll();
			if ( !GetServerInfo(pElementChild, m_aServersFromXmlAuto) )
			{
				ASSERT(0);
			}
		}
		else if ( 0 == strcmp(pElementChild->Value(), KStrElementNameManual) )
		{
			// ȡ�ֶ�����ķ�������Ϣ
			m_aServersFromXmlAuto.RemoveAll();
			if( !GetServerInfo(pElementChild, m_aServersFromXmlManual) )
			{
				ASSERT(0);
			}			
		}	
		
		pElementChild = pElementChild->NextSiblingElement();
	}

	return true;
}

CString COptimizeServer::GetCommunicationXmlString(const CServerState& stServerState)
{
	CString StrXml = L"";

	//	
	StrXml.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" />\n", 
		CString(KStrElementNameServer).GetBuffer(),
		CString(KStrElementAttriName).GetBuffer(), CString(stServerState.m_StrServerName).GetBuffer(),
		CString(KStrElementAttriIp).GetBuffer(), CString(stServerState.m_StrServerAddr).GetBuffer(),
		CString(KStrElementAttriPort).GetBuffer(), stServerState.m_uiServerPort,
		CString(KStrElementAttriMaxOline).GetBuffer(), stServerState.m_iMaxCount,
		CString(KStrElementAttriGroup).GetBuffer(), stServerState.m_iGroup,
		CString(KStrElementAttriType).GetBuffer(), stServerState.m_iNetType,
		CString(KStrElementAttriPing).GetBuffer(), stServerState.m_uiPingValue);

	return StrXml;
}

bool32 COptimizeServer::SaveServerInfoToXml()
{
	// 1: ÿ��ping ��֮�󱣴������ļ�
	// 2: �ֶ������Ժ󱣴������ļ�

	CString strConfigFile = CPathFactory::GetSortServerPath();
	
	if (  strConfigFile.IsEmpty())
	{
		return false;
	}

	// ���� XML
	CString StrXml;
	
	// ���ڵ�
	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"SortServer\">\n";
	
	char* StrXmlNodeNameFirst	= NULL;
	char* StrXmlNodeNameSecond	= NULL;

	if ( m_bAutoOptimize )
	{
		//
		StrXmlNodeNameFirst  = (char*)KStrElementNameManual;
		StrXmlNodeNameSecond = (char*)KStrElementNameAuto;

		//
		StrXml += L"<";
		StrXml += StrXmlNodeNameFirst;
		StrXml += L">\n";

		// �Զ�����, ����������Զ���, �ֶ���ԭ��		
		for ( int32 i = 0; i < m_aServersFromXmlManual.GetSize(); i++ )
		{
			CString StrServerNode;
			
			CServerState stServer = m_aServersFromXmlManual[i];
			
			//
			StrServerNode = GetCommunicationXmlString(stServer);
			StrXml += StrServerNode;
		}

		//
		StrXml += L"</";
		StrXml += StrXmlNodeNameFirst;
		StrXml += L">\n";
	}
	else
	{
		//
		StrXmlNodeNameFirst  = (char*)KStrElementNameAuto;
		StrXmlNodeNameSecond = (char*)KStrElementNameManual;

		//
		StrXml += L"<";
		StrXml += StrXmlNodeNameFirst;
		StrXml += L">\n";

		// �ֶ�����, ����������ֶ���, �Զ���ԭ��		
		for ( int32 i = 0; i < m_aServersFromXmlAuto.GetSize(); i++ )
		{
			CString StrServerNode;
			
			CServerState stServer = m_aServersFromXmlAuto[i];
			
			//
			StrServerNode = GetCommunicationXmlString(stServer);			
			StrXml += StrServerNode;
		}

		//
		StrXml += L"</";
		StrXml += StrXmlNodeNameFirst;
		StrXml += L">\n";
	}

	// �����Ľڵ�
	StrXml += L"<";
	StrXml += StrXmlNodeNameSecond;
	StrXml += L">\n";


	if ( m_aServerPingInfo.GetSize() > 0 )
	{
		for ( int32 i = 0; i < m_aServerPingInfo.GetSize(); i++ )
		{		
			CString StrServerNode;
			
			CServerState stServer = m_aServerPingInfo[i].m_stServer;
					
			StrServerNode = GetCommunicationXmlString(stServer);
			StrXml += StrServerNode;
		}
	}
	else
	{
		for ( int32 i = 0; i < m_aServersAftSort.GetSize(); i++ )
		{		
			CString StrServerNode;
			
			CServerState stServer = m_aServersAftSort[i];

			//
			StrServerNode = GetCommunicationXmlString(stServer);
			StrXml += StrServerNode;
		}
	}

	//
	StrXml += L"</";
	StrXml += StrXmlNodeNameSecond;
	StrXml += L">\n";

	//
	StrXml += L"</XMLDATA>";
	
	USES_CONVERSION;
	SaveXmlFile(_W2A(strConfigFile), StrXml);

	//
	return true;
}

void COptimizeServer::PingAllServers()
{
	if ( m_aServersFromAuth.GetSize() <= 0 )
	{
		ASSERT(0);
		return;
	}
	
	if (m_pViewData == NULL)
	{
		return;
	}

	//
	m_bPingCompleted = false;
	m_aServerPingInfo.RemoveAll();

	//
	for ( int32 i = 0; i < m_aServersFromAuth.GetSize(); i++ )
	{
		CServerState stServerState = m_aServersFromAuth[i];
		T_ServerSortPing stServerSortPing;
		
		stServerSortPing.m_stServer	   = stServerState;
		stServerSortPing.m_bCompeleted = false;
		stServerSortPing.m_bTimeOut	   = false;

		// �õ�ָ�������������:		
		bool32 bSevrConnected  = m_pViewData->m_pCommManager->IsServerConneted(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
		int32 iCommunicationID = m_pViewData->m_pCommManager->GetCommunicationID(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
		
		if ( !bSevrConnected )
		{
			// ������Ч����Ϣ, ���µ�����ʱ�Ĵ���
			stServerSortPing.m_TimeReq = timeGetTime();
			stServerSortPing.m_bTimeOut= true;

			m_aServerPingInfo.Add(stServerSortPing);			
			continue;
		}

		if ( -1 == iCommunicationID )
		{
			ASSERT(0);
		
			// ������Ч����Ϣ, ���µ�����ʱ�Ĵ��� 
			stServerSortPing.m_TimeReq = timeGetTime();
			stServerSortPing.m_bTimeOut= true; 

			m_aServerPingInfo.Add(stServerSortPing);			
			continue;
		}

		// ������:
		CMmiReqNetTest Req;
		
		int aiData[5] = {1,2,3,4,5};
		
		Req.m_uiLen = sizeof(aiData);
		Req.m_pData = new char[Req.m_uiLen];
		memset(Req.m_pData, 0, Req.m_uiLen);
		memcpyex(Req.m_pData, aiData, Req.m_uiLen);
			
		//
		int32 iReqID = -1;
		if ( !RequestNetTest(&Req, iCommunicationID, iReqID) )
		{
			ASSERT(0);
			continue;			
		}
		
		if ( -1 == iReqID )
		{
			ASSERT(0);
			continue;
		}

		stServerSortPing.m_TimeReq = timeGetTime();
		stServerSortPing.m_iReqID  = iReqID;

		//
		m_aServerPingInfo.Add(stServerSortPing);

		//
		_TESTTRACE(L"��ping ����: %s:%d ����ID: %d ����ʱ��: %d \n", stServerState.m_StrServerAddr.GetBuffer(), stServerState.m_uiServerPort, iReqID, stServerSortPing.m_TimeReq);
		stServerState.m_StrServerAddr.ReleaseBuffer();
	}	
}

void COptimizeServer::OnPingCompleted()
{
	_TESTTRACE(L"ping ���� \n");

	m_bPingCompleted = true;	
	StopMyTimer();

	// �ȸ����������
	arrServer aServerBeforeSort;

	int32 i = 0;
	//
	for ( i = 0; i < m_aServerPingInfo.GetSize(); i++ )
	{
		//
		bool32 bFind = false;
		if (bFind)
		{
			// .....
		}
		
		// Ҫ���������Ա������ֵ, ����ȡָ�������
		CServerState& stServer = m_aServerPingInfo[i].m_stServer;
				
		int32 iSocre = CalcServerScore(stServer);
		
		if ( iSocre < 0 || iSocre > 100 )
		{
			ASSERT(0);
		}
		
		stServer.m_iSortScore = iSocre;												
		aServerBeforeSort.Add(stServer);
	}
		
	if ( m_bLocalAndAuthIdentical && !m_bForcePing)
	{
		// ����ÿ�ε�SortQuoteServers�Ὣm_aServersAftSort���, ���Ի����ʵ���Ϻ������������m_aServersAftSort���¸�ֵ xl
		// һ����. �ǵ�¼��Ĳ���
		// NULL;
	}
	else
	{
		// �ǵ�¼�����еĲ���
		bool32 bOK = SortServers(aServerBeforeSort, m_aServersAftSort);
		if (bOK)
		{
			// .....
		}

		if (!m_bAutoOptimize && !m_bForcePing)
		{
			// ������ֶ���, ping ��󵯳��������
			SendMessage(m_pViewData->m_OwnerWnd, UM_MainFrame_ManualOptimize, 0, (LPARAM)&m_aServersAftSort);
		}

		if ( !m_bForcePing )
		{
			//
			OnSortCompleted();
		}		
	}

	//
	SaveServerInfoToXml();	
	m_aServerPingInfo.RemoveAll();

	// ֪ͨping ����
	for ( i = 0; i < m_apNotify.GetSize(); i++ )
	{
		COptimizeServerNotify* pNotify = m_apNotify[i];
		if ( NULL != pNotify )
		{		
			pNotify->OnFinishAllPing(); 
		}
	}

	if ( m_bForcePing )
	{
		// ������� ping ��, ֪ͨ�������, ���½���
		OnSortCompleted();
	}
}

void COptimizeServer::OnSortCompleted()
{
	m_bSortCompleted = true;
	
	if( !m_bAutoOptimize )
	{
		SaveServerInfoToXml();
	}
	
	for ( int32 i = 0; i < m_apNotify.GetSize(); i++ )
	{
		COptimizeServerNotify* pNotify = m_apNotify[i];
		if ( NULL != pNotify )
		{
			pNotify->OnFinishSortServer();				
		}
	}
}

void COptimizeServer::StartMyTimer()
{
	// ���ó�ʱ�Ķ�ʱ��
	if ( NULL == m_hOwnWnd )
	{
		return;
	}

	//
	::KillTimer(m_hOwnWnd, KiTimerIdPing);
	::SetTimer(m_hOwnWnd, KiTimerIdPing, KiTimerPeriodPing, NULL);	
}

void COptimizeServer::StopMyTimer()
{
	if ( NULL == m_hOwnWnd )
	{
		return;
	}
	
	//
	::KillTimer(m_hOwnWnd, KiTimerIdPing);
}

void COptimizeServer::OnMyTimer(int32 iTimerId)
{
	if ( KiTimerIdPing == iTimerId )
	{
		for ( int32 i = 0; i < m_aServerPingInfo.GetSize(); i++ )
		{
			T_ServerSortPing& stServerSortPing = m_aServerPingInfo[i];
			
			if ( !stServerSortPing.m_bCompeleted )
			{							
				// ����ʱ��, ��û��ping ������, ���ó�ʱ
				stServerSortPing.m_bTimeOut = true;
				stServerSortPing.m_TimeResp = stServerSortPing.m_TimeReq + KiPingMax;
				stServerSortPing.m_stServer.m_uiPingValue = KiPingMax;
				
				_TESTTRACE(L"��ʱ��  ping û��ȫ������. %d ����������Ϊ��ʱ \n", stServerSortPing.m_iReqID);
				
				// ֪ͨ
				for ( int32 j = 0; j < m_apNotify.GetSize(); j++ )
				{
					if ( NULL != m_apNotify[j] )
					{
						m_apNotify[j]->OnFinishPingServer(stServerSortPing.m_stServer);
					}
				}
			}
		}
		
		//
		OnPingCompleted();
	}
}

void COptimizeServer::OnDataServerDisconnected( int32 iCommunicationId )
{
	// ���������ߣ����൱�ڸ÷������Ѿ�ʧȥ�ʸ��ˣ������ж��Ƿ�Ӧ�������Ƚ���
	if ( m_bPingCompleted || m_pViewData == NULL )
	{
		// �Ѿ���ʱ����, ����Ҳû��
		return;
	}

	// ping ������
	int32 iCountPingComeleted = 0;
	
	for ( int32 i = m_aServerPingInfo.GetSize()-1; i >= 0; --i )
	{
		T_ServerSortPing& stServerSortPing = m_aServerPingInfo[i];

		//
		int32 iComm = m_pViewData->m_pCommManager->GetCommunicationID(stServerSortPing.m_stServer.m_StrServerAddr, stServerSortPing.m_stServer.m_uiServerPort);

		if ( iComm == iCommunicationId )
		{
			// �������������������
			_TESTTRACE(L"Pingѡ������������, ������%d������, ��pingֵ��ʱ����", iCommunicationId);
			stServerSortPing.m_bCompeleted = true;
			stServerSortPing.m_bTimeOut = true;
			stServerSortPing.m_TimeResp = stServerSortPing.m_TimeReq + KiPingMax;
			stServerSortPing.m_stServer.m_uiPingValue = KiPingMax;
		}

		if ( stServerSortPing.m_bCompeleted )
		{
			++iCountPingComeleted;
		}
	}
	
	if ( iCountPingComeleted == m_aServerPingInfo.GetSize() && !m_bPingCompleted )
	{		
		_TESTTRACE(L"ping ȫ��������(dis) \n");
		OnPingCompleted();		
	}
}

void COptimizeServer::SetLatestOnlineData(CServerState& stServer)
{
	//
	
	for (int32 i = 0; i < m_aServersFromXmlAuto.GetSize(); i++)
	{
		CServerState &stLocalXml = m_aServersFromXmlAuto[i];

		if (stLocalXml.m_StrServerAddr == stServer.m_StrServerAddr && stLocalXml.m_uiServerPort == stServer.m_uiServerPort)
		{
			stLocalXml.m_iOnLineCount = stServer.m_iOnLineCount;
			break;
		}
	}		

	for (int32 i = 0; i < m_aServersFromXmlManual.GetSize(); i++)
	{
		CServerState &stLocalXml = m_aServersFromXmlManual[i];
		
		if (stLocalXml.m_StrServerAddr == stServer.m_StrServerAddr && stLocalXml.m_uiServerPort == stServer.m_uiServerPort)
		{
			stLocalXml.m_iOnLineCount = stServer.m_iOnLineCount;
			break;
		}
	}	
}

bool compareOnlineUser(const CServerState& s1, const CServerState& s2)
{
	if (s1.m_iMaxCount == 0 || s2.m_iMaxCount == 0)
	{
		ASSERT(0);
		return false;
	}

	//
	float f1 = (float)s1.m_iOnLineCount / s1.m_iMaxCount;
	float f2 = (float)s2.m_iOnLineCount / s2.m_iMaxCount;

	if(f1 == f2)
	{
		return s1.m_iMaxCount > s2.m_iMaxCount;
	}

	return f1 < f2;
}

bool compareScore(const CServerState& s1, const CServerState& s2)
{
	// �����ߵ���ǰ��
	if (s1.m_iSortScore == s2.m_iSortScore)
	{
		return compareOnlineUser(s1, s2);
	}
	
	return s1.m_iSortScore > s2.m_iSortScore;
}

typedef vector<CServerState> vecServer;
typedef vector<vecServer>	 vecvecSrv;

void SplitServers(int32 iScoreSplit, IN vector<CServerState>& aSrc, OUT vecvecSrv& aServers)
{
	if (aSrc.empty() || iScoreSplit <= 0)
	{
		ASSERT(0);
		return;
	}

	aServers.clear();

	//
	int32 iScoreMax = aSrc[0].m_iSortScore;
	int32 iScoreMin = iScoreMax - iScoreSplit;

	//	
	vector<CServerState> vecNow;
	for (uint32 i = 0; i < aSrc.size(); i++)
	{
		CServerState stNow = aSrc[i];
		int32 iScore = stNow.m_iSortScore;

		if (iScore >= iScoreMin)
		{
			vecNow.push_back(stNow);
		}		
		else
		{
			aServers.push_back(vecNow);
			vecNow.clear();

			//
			iScoreMax = stNow.m_iSortScore;
			iScoreMin = iScoreMax - iScoreSplit;

			//
			vecNow.push_back(stNow);
		}

		if (i == aSrc.size() - 1)
		{
			aServers.push_back(vecNow);
		}
	}
}

//
bool32 COptimizeServer::FinalCheckServer(INOUT arrServer& aServers)
{
	int32 iSize = aServers.GetSize();
	if (iSize <= 0)
	{
		ASSERT(0);
		return false;
	}

	if (1 == iSize)
	{
		// ֻ��һ��, û��ѡ��
		return true;
	}

	//
	_TESTTRACE(L"�����һ��ѡ���ķ������Ƿ񳬸���, �Ƿ����\n");
	bool32 bEnd = false;
	int32 iTimes=0;
	bool32 bAdjust = false;

	//
	while(!bEnd)
	{
		CServerState st = aServers[0];
		float fLoad = (float)st.m_iOnLineCount / st.m_iMaxCount;

		if (fLoad >= KfMaxServerLoad || BeErrServer(st))
		{
			if (fLoad >= KfMaxServerLoad)
			{
				_TESTTRACE(L"������%s %d ��ǰ����:%d �������: %d ���ɱ�:%.2f �ѳ�������, �ŵ�����ȥ\n", st.m_StrServerAddr.GetBuffer(), st.m_uiServerPort, st.m_iOnLineCount, st.m_iMaxCount, (float)st.m_iOnLineCount/st.m_iMaxCount);
				st.m_StrServerAddr.ReleaseBuffer();
			}
			else
			{
				_TESTTRACE(L"������%s %d �ϴε�¼��������, �ŵ�����ȥ\n", st.m_StrServerAddr.GetBuffer(), st.m_uiServerPort);
				st.m_StrServerAddr.ReleaseBuffer();
			}
			
			//
			iTimes += 1;
			aServers.RemoveAt(0);
			aServers.Add(st);
			bAdjust = true;
		}	
		else
		{
			bEnd = true;
		}

		//
		if (iTimes >= iSize)
		{
			bEnd = true;
		}
	}	

	for (int32 i = 0; i < iSize; i++)
	{
		CServerState& st = aServers[i];
		st.m_iSortIndex = i;
	}

	return true;
}

//
bool32 COptimizeServer::SortServers(IN arrServer& aServerIN, OUT arrServer& aServerOUT)
{
	if ( aServerIN.GetSize() <= 0 )
	{
		ASSERT(0);
		return false;
	}

#ifndef  _DEBUG
	if (1 == aServerIN.GetSize())
	{
		aServerOUT.Copy(aServerIN);
		return true;
	}
#endif	

	//
	aServerOUT.RemoveAll();
	vector<CServerState> aServer;

	
	//
	for( int32 i = 0; i < aServerIN.GetSize(); i++ )
	{
		// Ҫ���������Ա������ֵ, ����ȡָ�������
		CServerState& stServer = aServerIN[i];
				
		int32 iSocre = CalcServerScore(stServer);
		
		if ( iSocre < 0 || iSocre > 100 )
		{
			ASSERT(0);
		}
		
		stServer.m_iSortScore = iSocre;
		aServer.push_back(stServer);
	}

	// �Ȱ���ֵ����
	std::sort(aServer.begin(), aServer.end(), compareScore);

	// �ٰ�����������һ��
	const int32 iSplitScore = 5;

	vecvecSrv aSplitedSrvs;
	SplitServers(iSplitScore, aServer, aSplitedSrvs);

	//
	for (uint32 i = 0; i < aSplitedSrvs.size(); i++)
	{
		vecServer vecNow = aSplitedSrvs[i];
		std::sort(vecNow.begin(), vecNow.end(), compareOnlineUser);

		for (uint32 j = 0; j < vecNow.size(); j++)
		{
			CServerState stTmp = vecNow[j];
			aServerOUT.Add(stTmp);
			_TESTTRACE(L"����: %s %d ����: %d ��ǰ����: %d �������: %d ���߱�: %.2f\n", stTmp.m_StrServerAddr.GetBuffer(), stTmp.m_uiServerPort, stTmp.m_iSortScore, stTmp.m_iOnLineCount, stTmp.m_iMaxCount, (float)stTmp.m_iOnLineCount/stTmp.m_iMaxCount);
			stTmp.m_StrServerAddr.ReleaseBuffer();
		}		
	}

	if (aServer.size() != aServerOUT.GetSize())
	{
		_TESTTRACE(L"BUG!!! ����ǰ���������������\n");
		ASSERT(0);
		std::sort(aServer.begin(), aServer.end(), compareScore);
		SplitServers(iSplitScore, aServer, aSplitedSrvs);
	}

	// ����֤һ��, �������ķ������Ͷ�������ȥ
	FinalCheckServer(aServerOUT);

	_TESTTRACE(L"���ս��:\n");
	for (int32 j = 0; j < aServerOUT.GetSize(); j++)
	{
		CServerState stTmp = aServerOUT[j];
		_TESTTRACE(L"����%d: %s %d ����: %d ��ǰ����: %d �������: %d ���߱�: %.2f\n", stTmp.m_iSortIndex, stTmp.m_StrServerAddr.GetBuffer(), stTmp.m_uiServerPort, stTmp.m_iSortScore, stTmp.m_iOnLineCount, stTmp.m_iMaxCount, (float)stTmp.m_iOnLineCount/stTmp.m_iMaxCount);
		stTmp.m_StrServerAddr.ReleaseBuffer();
	}		

	return true;
}

bool32 COptimizeServer::BeErrServer(const CServerState& st)
{
	int32 iSize = m_aErrServer.GetSize();

	if (iSize <= 0)
	{
		return false;
	}

	//
	for (int32 i = 0; i < iSize; i++)
	{
		CServerState stErr = m_aErrServer[i];

		if (stErr.m_uiServerPort == st.m_uiServerPort && 0 == stErr.m_StrServerAddr.CompareNoCase(st.m_StrServerAddr))
		{
			return true;
		}
	}

	return false;
}

void COptimizeServer::ClearErrServer()
{
	m_aErrServer.RemoveAll();
	CString StrPath = CPathFactory::GetLoginErrServerPath();
	BOOL bOK = ::DeleteFile(StrPath);

	_TESTTRACE(L"��¼�ɹ�, ɾ���������������ļ�%d", bOK);
}

bool32 COptimizeServer::GetErrServer()
{
	CString StrPath = CPathFactory::GetLoginErrServerPath();
	
	if ( StrPath.GetLength() <= 0 )
	{
		return false;
	}
	
	//
	std::string StrPathA ;
	Unicode2MultiChar(CP_ACP, StrPath, StrPathA);
	
	
	TiXmlDocument doc(StrPathA.c_str());
	if ( !doc.LoadFile(StrPathA.c_str()) )
	{		
		return false;
	}
	
	// �����
	m_aErrServer.RemoveAll();

	TiXmlElement* pRootElement = doc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}
	
	TiXmlElement* pElementChild = pRootElement->FirstChildElement();
	if (NULL == pElementChild)
	{
		return false;
	}

	GetServerInfo(pElementChild, m_aErrServer);
	return true;
}

bool32 COptimizeServer::SaveErrServer(CServerState& stServer)
{
	_TESTTRACE(L"������������: %s %d\n", stServer.m_StrServerAddr.GetBuffer(), stServer.m_uiServerPort);
	stServer.m_StrServerAddr.ReleaseBuffer();
	
	// ���жϴ��ڷ�
	if (BeErrServer(stServer))
	{
		_TESTTRACE(L"����������Ѿ������б���!");
		return false;
	}
	
	// �����ھ����
	m_aErrServer.Add(stServer);

	//
	if (m_aErrServer.GetSize() <= 0)
	{
		return false;
	}

	//
	CString strConfigFile = CPathFactory::GetSortServerPath();
	
	if ( strConfigFile.IsEmpty() )
	{
		return false;
	}
	
	// ���� XML
	CString StrXml;
	
	// ���ڵ�
	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"SortServer\">\n";	

	StrXml += L"<";
	StrXml += KStrElementNameErr;
	StrXml += L">\n";

	for ( int32 i = 0; i < m_aErrServer.GetSize(); i++ )
	{
		CString StrServerNode;
		
		CServerState stTmpServer = m_aErrServer[i];
		
		//
		StrServerNode = GetCommunicationXmlString(stTmpServer);
		StrXml += StrServerNode;
	}

	StrXml += L"<";
	StrXml += KStrElementNameErr;
	StrXml += L">\n";

	StrXml += L"</XMLDATA>";	

	
	
	SaveXmlFile(_W2A(strConfigFile), StrXml);
	//
	return true;
}

void COptimizeServer::SetViewData(CViewData* pViewData)
{
	m_pViewData = pViewData;
}
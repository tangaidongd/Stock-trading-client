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

// 暂定的一些常量值:
static const int32 KiPingMax					= 1000; 
static const int32 KiGroupMax					= 3;
static const int32 KiOlineUserMax				= 1000;
static const int32 KiMaxOlineUserMax			= 5000;

static const float KfFreeServerRate				= 0.1f;

		

// 服务器负荷上限
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
	// 释放内存

	// Xml 文档类指针
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
	_TESTTRACE(L"设置排序服务器: \n");

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

		_TESTTRACE(L"服务器%d : %s:%d \n", i+1, stServer.wszAddr, stServer.iPort);
	}

	// 判断是否一致
	m_bLocalAndAuthIdentical = JudgeLocalAndAuthServers();
	_TESTTRACE(L"自动选股 = %d 本地和服务器是否一致: %d \n", m_bAutoOptimize, m_bLocalAndAuthIdentical);	
}

void COptimizeServer::DelNonConnectServer(const T_ServerInfo* pServerNonConnect, int32 iServerCounts)
{	
	// 删除连接不上的服务器, 不要进行优选
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
				// 删掉这个服务器:
				m_aServersFromAuth.RemoveAt(j);
				//
				_TESTTRACE(L"去掉连接不上的行情服务器: %s:%d \n", ServerStateLocal.m_StrServerAddr.GetBuffer(), ServerStateLocal.m_uiServerPort);
				ServerStateLocal.m_StrServerAddr.ReleaseBuffer();
			}
		}
	}
}

int32 COptimizeServer::CalcServerScore(const CServerState& stServerState)
{
	// 计算服务器得分
	float fScore = 0.;
	
	// Ping
	float fScorePing = 100 * ((float)stServerState.m_uiPingValue / KiPingMax);
	
	// Ping 越低分越高
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
	_TESTTRACE(L"====> 服务器 %s:%d 得分: ping: %.2f 【ping = %u】type: %.2f TOTAL: %d\n", 
		strTempServerAddr.GetBuffer(), stServerState.m_uiServerPort,
		fScorePing, stServerState.m_uiPingValue, fScoreType, iScoreRev);
	//
	strTempServerAddr.ReleaseBuffer();

	return iScoreRev;
}

// int32 COptimizeServer::CalcServerScore(const CCommunication* pServer)
// {
// 	// 计算服务器得分
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
// 		// 超时连不上的直接放弃
// 		// return 0;
// 	}
// 
// 	//
// 	float fScore = 0.;
// 
// 	// Ping
// 	float fScorePing = 100 * ((float)pServer->m_uiPingValue / KiPingMax);
// 		
// 	// Ping 越低分越高
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
// 	// 这是人越少分越高
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
// 		// 服务器负荷很小的时候, 不考虑这两个的因素, 都给满分
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
// 	_TESTTRACE(L"====> 服务器 %s:%d 得分: ping: %.2f 【ping = %u】type: %.2f group: %.2f olineuser: %.2f maxoline: %.2f total: %d \n", stServerState.m_StrServerAddr, stServerState.m_uiServerPort,
// 		  fScorePing, stServerState.m_uiPingValue, fScoreType, fScoreGroup, fScoreOlineUser, fScoreMaxOlineUser, iScoreRev);
// 	//
// 	return iScoreRev;
// }

bool32 COptimizeServer::JudgeLocalAndAuthServers()
{		
	if ( m_bAutoOptimize )
	{
		// 自动的
		if ( m_aServersFromAuth.GetSize() != m_aServersFromXmlAuto.GetSize() )
		{
			// 两边的服务器数量都不同, 直接判断不一样
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
				// 有一台服务器没对上号, 就判定不一样
				return false;
			}
		}
	}
	else
	{
		// 手动的
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
				// 有一台服务器没对上号, 就判定不一样
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
		StrRev = L"电信";

		if ( bDetail )
		{
			StrRev += L"(宽带, ADSL及拨号)";
		}
	}
	else if ( ENTWangtong == eNetType )
	{
		StrRev = L"网通";
		
		if ( bDetail )
		{
			StrRev += L"(宽带, ADSL及拨号)";
		}
	}
	else if ( ENTYidong == eNetType )
	{
		StrRev = L"移动";

		if ( bDetail )
		{
			StrRev += L"(宽带, ADSL及拨号)";
		}
	}
	else if ( ENTLiantong == eNetType )
	{
		StrRev = L"联通";

		if ( bDetail )
		{
			StrRev += L"(宽带, ADSL及拨号)";
		}
	}
	else
	{
		StrRev = L"其他";

		if ( bDetail )
		{
			StrRev += L"(有线通, 铁通等)";
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

	// 设置序号
	for (int32 i = 0; i < aServerOUT.GetSize(); i++ )
	{
		CServerState& stServerAftSort = aServerOUT[i];
		stServerAftSort.m_iSortIndex = i;
		
		_TESTTRACE(L"XML排序 服务器: %s:%d 序号: %d 得分: %d \n", stServerAftSort.m_StrServerAddr.GetBuffer(), stServerAftSort.m_uiServerPort, stServerAftSort.m_iSortIndex, stServerAftSort.m_iSortScore);
		stServerAftSort.m_StrServerAddr.ReleaseBuffer();
	}

	return true;
}

bool32 COptimizeServer::SortQuoteServersByPing()
{
	//
	m_bPingCompleted = false;
	
	// 设置超时的定时器	
	StartMyTimer();

	// 重新ping			
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
		// 一样的话, 取不同的 xml 节点服务器信息排序就是了
		if ( m_bAutoOptimize )
		{
			SortQuoteServersByXmlFile(m_aServersFromXmlAuto, m_aServersAftSort);
			
			// 通知完成排序
			OnSortCompleted();
		}
		else 
		{
			// 手动的
			SortQuoteServersByXmlFile(m_aServersFromXmlManual, m_aServersAftSort);
		
			SendMessage(m_pViewData->m_OwnerWnd, UM_MainFrame_ManualOptimize, 0, (LPARAM)&m_aServersAftSort);
		}
	}
	else
	{
		// 不一样都是要用 auth 传来的服务器重新ping 排序的
		SortQuoteServersByPing();
	}

	//
	return true;
}

void COptimizeServer::SetManualSortResult(IN const arrServer& aServers)
{
	m_aServersAftSort.Copy(aServers);
	
	// 排序完成
	OnSortCompleted();

	// 通知ping 完了
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
		// 请求错误
		ASSERT(0);
		return false;
	}
	
	// ID 号
	iReqID = aMmiReqNodes.GetAt(0)->m_iMmiReqId;

	//
	return true;
}

void COptimizeServer::OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest)
{
	if ( m_bPingCompleted )
	{
		// 已经到时间了, 回来也没用
		return;
	}

	// ping 返回了
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
				// 这个就不应该了
				ASSERT(0);
				stServerSortPing.m_stServer.m_uiPingValue = 50;
			}

			// 通知
			for ( int32 j = 0; j < m_apNotify.GetSize(); j++ )
			{
				if ( NULL != m_apNotify[j] )
				{
					m_apNotify[j]->OnFinishPingServer(stServerSortPing.m_stServer);
				}
			}

			_TESTTRACE(L"ping 回包, ID = %d, 时间 = %d , ping = %d \n", iMmiReqId, stServerSortPing.m_TimeResp, stServerSortPing.m_stServer.m_uiPingValue);
		}

		if ( stServerSortPing.m_bCompeleted )
		{
			++iCountPingComeleted;
		}
	}

	if ( iCountPingComeleted == m_aServerPingInfo.GetSize() && !m_bPingCompleted )
	{		
		_TESTTRACE(L"ping 全都回来了 \n");
		OnPingCompleted();		
	}
}

void COptimizeServer::ForceRePingServer()
{
	if ( m_bAutoOptimize )
	{
		// 肯定是手动的时候
		ASSERT(0);
		return;
	}

	//
	m_aServersAftSort.RemoveAll();

	// 重新发 ping 请求
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

		// 名称
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
		
		// 端口
		int32 iPort = -1;
		if (NULL != StrPort)
		{
			iPort = atoi(StrPort);
		}
		
		// 最大在线数
		int32 iMaxOline = -1;
		if (NULL != StrMaxOline)
		{
			iMaxOline = atoi(StrMaxOline);
		}
		
		// 组
		int32 iGroup = -1;
		if (NULL != StrGroup)
		{
			iGroup = atoi(StrGroup);
		}
		
		// 类型
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
	// 名称, 地址, 端口, 最大在线数, 组, 网络类型 只要有一个不一样就判断不一样

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
	
	// 清空先
	m_aServersFromXmlAuto.RemoveAll();
	m_aServersFromXmlManual.RemoveAll();
	
	// 根节点
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
			// 取自动排序的服务器信息
			m_aServersFromXmlAuto.RemoveAll();
			if ( !GetServerInfo(pElementChild, m_aServersFromXmlAuto) )
			{
				ASSERT(0);
			}
		}
		else if ( 0 == strcmp(pElementChild->Value(), KStrElementNameManual) )
		{
			// 取手动排序的服务器信息
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
	// 1: 每次ping 了之后保存配置文件
	// 2: 手动排序以后保存配置文件

	CString strConfigFile = CPathFactory::GetSortServerPath();
	
	if (  strConfigFile.IsEmpty())
	{
		return false;
	}

	// 保存 XML
	CString StrXml;
	
	// 根节点
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

		// 自动排序, 保存排序后自动的, 手动的原样		
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

		// 手动排序, 保存排序后手动的, 自动的原样		
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

	// 排序后的节点
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

		// 得到指定的行情服务器:		
		bool32 bSevrConnected  = m_pViewData->m_pCommManager->IsServerConneted(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
		int32 iCommunicationID = m_pViewData->m_pCommManager->GetCommunicationID(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
		
		if ( !bSevrConnected )
		{
			// 加上无效的信息, 等下当作超时的处理
			stServerSortPing.m_TimeReq = timeGetTime();
			stServerSortPing.m_bTimeOut= true;

			m_aServerPingInfo.Add(stServerSortPing);			
			continue;
		}

		if ( -1 == iCommunicationID )
		{
			ASSERT(0);
		
			// 加上无效的信息, 等下当作超时的处理 
			stServerSortPing.m_TimeReq = timeGetTime();
			stServerSortPing.m_bTimeOut= true; 

			m_aServerPingInfo.Add(stServerSortPing);			
			continue;
		}

		// 发请求:
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
		_TESTTRACE(L"发ping 请求: %s:%d 请求ID: %d 请求时间: %d \n", stServerState.m_StrServerAddr.GetBuffer(), stServerState.m_uiServerPort, iReqID, stServerSortPing.m_TimeReq);
		stServerState.m_StrServerAddr.ReleaseBuffer();
	}	
}

void COptimizeServer::OnPingCompleted()
{
	_TESTTRACE(L"ping 结束 \n");

	m_bPingCompleted = true;	
	StopMyTimer();

	// 先给服务器打分
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
		
		// 要设置这个成员变量的值, 所以取指针的引用
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
		// 由于每次的SortQuoteServers会将m_aServersAftSort清空, 所以会造成实际上后续不会继续将m_aServersAftSort重新赋值 xl
		// 一样的. 是登录后的测速
		// NULL;
	}
	else
	{
		// 是登录过程中的测速
		bool32 bOK = SortServers(aServerBeforeSort, m_aServersAftSort);
		if (bOK)
		{
			// .....
		}

		if (!m_bAutoOptimize && !m_bForcePing)
		{
			// 如果是手动的, ping 完后弹出排序界面
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

	// 通知ping 完了
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
		// 如果是重 ping 的, 通知排序完成, 更新界面
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
	// 设置超时的定时器
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
				// 到了时间, 还没有ping 回来的, 设置超时
				stServerSortPing.m_bTimeOut = true;
				stServerSortPing.m_TimeResp = stServerSortPing.m_TimeReq + KiPingMax;
				stServerSortPing.m_stServer.m_uiPingValue = KiPingMax;
				
				_TESTTRACE(L"定时器  ping 没有全部回来. %d 号请求设置为超时 \n", stServerSortPing.m_iReqID);
				
				// 通知
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
	// 服务器断线，则相当于该服务器已经失去资格了，重新判断是否应当结束比较了
	if ( m_bPingCompleted || m_pViewData == NULL )
	{
		// 已经到时间了, 回来也没用
		return;
	}

	// ping 返回了
	int32 iCountPingComeleted = 0;
	
	for ( int32 i = m_aServerPingInfo.GetSize()-1; i >= 0; --i )
	{
		T_ServerSortPing& stServerSortPing = m_aServerPingInfo[i];

		//
		int32 iComm = m_pViewData->m_pCommManager->GetCommunicationID(stServerSortPing.m_stServer.m_StrServerAddr, stServerSortPing.m_stServer.m_uiServerPort);

		if ( iComm == iCommunicationId )
		{
			// 就是这个服务器悲剧了
			_TESTTRACE(L"Ping选服务器过程中, 服务器%d断线了, 做ping值超时处理", iCommunicationId);
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
		_TESTTRACE(L"ping 全都回来了(dis) \n");
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
	// 分数高的排前面
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
		// 只有一个, 没的选了
		return true;
	}

	//
	_TESTTRACE(L"最后检测一次选出的服务器是否超负荷, 是否错误\n");
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
				_TESTTRACE(L"服务器%s %d 当前人数:%d 最大在线: %d 负荷比:%.2f 已超过警戒, 排到后面去\n", st.m_StrServerAddr.GetBuffer(), st.m_uiServerPort, st.m_iOnLineCount, st.m_iMaxCount, (float)st.m_iOnLineCount/st.m_iMaxCount);
				st.m_StrServerAddr.ReleaseBuffer();
			}
			else
			{
				_TESTTRACE(L"服务器%s %d 上次登录发生错误, 排到后面去\n", st.m_StrServerAddr.GetBuffer(), st.m_uiServerPort);
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
		// 要设置这个成员变量的值, 所以取指针的引用
		CServerState& stServer = aServerIN[i];
				
		int32 iSocre = CalcServerScore(stServer);
		
		if ( iSocre < 0 || iSocre > 100 )
		{
			ASSERT(0);
		}
		
		stServer.m_iSortScore = iSocre;
		aServer.push_back(stServer);
	}

	// 先按分值排序
	std::sort(aServer.begin(), aServer.end(), compareScore);

	// 再按分数区间排一次
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
			_TESTTRACE(L"排序: %s %d 分数: %d 当前在线: %d 最大在线: %d 在线比: %.2f\n", stTmp.m_StrServerAddr.GetBuffer(), stTmp.m_uiServerPort, stTmp.m_iSortScore, stTmp.m_iOnLineCount, stTmp.m_iMaxCount, (float)stTmp.m_iOnLineCount/stTmp.m_iMaxCount);
			stTmp.m_StrServerAddr.ReleaseBuffer();
		}		
	}

	if (aServer.size() != aServerOUT.GetSize())
	{
		_TESTTRACE(L"BUG!!! 排序前后服务器数量不对\n");
		ASSERT(0);
		std::sort(aServer.begin(), aServer.end(), compareScore);
		SplitServers(iSplitScore, aServer, aSplitedSrvs);
	}

	// 再验证一下, 不正常的服务器就丢到后面去
	FinalCheckServer(aServerOUT);

	_TESTTRACE(L"最终结果:\n");
	for (int32 j = 0; j < aServerOUT.GetSize(); j++)
	{
		CServerState stTmp = aServerOUT[j];
		_TESTTRACE(L"排序%d: %s %d 分数: %d 当前在线: %d 最大在线: %d 在线比: %.2f\n", stTmp.m_iSortIndex, stTmp.m_StrServerAddr.GetBuffer(), stTmp.m_uiServerPort, stTmp.m_iSortScore, stTmp.m_iOnLineCount, stTmp.m_iMaxCount, (float)stTmp.m_iOnLineCount/stTmp.m_iMaxCount);
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

	_TESTTRACE(L"登录成功, 删除错误服务器标记文件%d", bOK);
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
	
	// 清空先
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
	_TESTTRACE(L"保存错误服务器: %s %d\n", stServer.m_StrServerAddr.GetBuffer(), stServer.m_uiServerPort);
	stServer.m_StrServerAddr.ReleaseBuffer();
	
	// 先判断存在否
	if (BeErrServer(stServer))
	{
		_TESTTRACE(L"错误服务器已经存在列表中!");
		return false;
	}
	
	// 不存在就添加
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
	
	// 保存 XML
	CString StrXml;
	
	// 根节点
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
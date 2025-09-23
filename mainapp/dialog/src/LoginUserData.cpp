#include "StdAfx.h"
#include "LoginUserData.h"
#include "PathFactory.h"
#include "tinyxml.h"
#include "IoViewBase.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
const char*	  KStrElementNameServerInfoQuote  = "QuoteInfo";		//����
const char*	  KStrElementAttriServerIndex	  = "ServerIndex";
const char*	  KStrElementNameServer			  = "Server";
const char*	  KStrElementAttriServerName	  = "ServerName";
const char*	  KStrElementAttriServerIp		  = "ServerIp";
const char*	  KStrElementAttriServerPort	  = "ServerPort";
const char*	  KStrElementAttriServerNetType	  = "ServerNetType";

const char*	  KStrElementNameBackupServerPort  = "BackupServerPort";		//��������˿�

// ����ѡ��
const char*   KStrElementAttriPickModelServer	= "PickModelServer";	//ѡ�ɵ�ַ��IP
const char*   KStrElementAttriPickModelType		= "PickModelType";		//ѡ������


//const char*	  KStrElementAttriServerDefault	= "ServerDefault";
//const char*   KStrElementNameServerInfoNews = "NewsInfo";			//��Ѷ����
//const char*   KStrElementNameServerInfoOrgCode = "OrgCode";			//��Ѷ���Ļ�������
//const char*   KStrElementNameServerInfoNewsOrgCode = "newsOrgCode";	// ������Ѷ��������
//const char*   KStrElementNameServerInfoPlatCode = "PlatCode";			//��Ѷ����ƽ̨����
const char*	  KStrElementNameServerInfoTrades	= "ServerInfoTrades";
const char*	  KStrElementAttriServerTradeIndex	= "ServerTradeIndex";
const char*   KStrElementAttriServerInfoCenter  = "MessageInfo";	//��Ϣ����
const char*   KStrElementAttriGateWayInfo		= "GateWayInfo";	//gatewy��ַ
const char*   KStrElementAttriQueryStockInfo	= "QueryStockInfo";	//�����¹�IP����
const char*   KStrElementnNameServerEcono	 = "CalendarTipInfo";	//��������
const char*   KStrElementAttriServerEconoURL = "IP";

const char*	  KStrElementNameUserInfo		= "UserInfo";
const char*	  KStrElementNameUser			= "User";
const char*	  KStrElementAttriUserName		= "UserName";
const char*	  KStrElementAttriUserPwd		= "UserPwd";
const char*	  KStrElementAttriBeSave		= "bSave";
const char*   KStrElementAttriBeLastUser	= "bLastUser";
const char*	  KStrElementAttriEnvIndex	    = "EnvIndex"; //2013-7-4
const char*	  KStrElementAttriAutoLogin		= "bAutoLogin";
const char*	  KStrElementAttriSCToolBar		= "bShowScToolBar";

const char*	  KStrElementNameProxyInfo		= "ProxyInfo";
const char*	  KStrElementNameProxy			= "Proxy";
const char*	  KStrElementAttriBeUseProxy	= "bUseProxy";
const char*	  KStrElementAttriProxyType		= "ProxyType";
const char*	  KStrElementAttriProxyAddress	= "ProxyAddress";
const char*	  KStrElementAttriProxyPort		= "ProxyPort";
const char*	  KStrElementAttriProxyUserName	= "ProxyUserName";
const char*	  KStrElementAttriProxyUserPwd	= "ProxyUserPwd";
const char*	  KStrElementAttriGreenChannel	= "GreenChannel";


const char*	  KStrElementNameUserRight		= "Right";
const char*	  KStrElementAttriRightCode		= "Code";
const char*	  KStrElementAttriRightName		= "Name";

const char*   KStrElementAttriNetWorkSet    = "TradeInfo";		//ʵ�̽���
const char*   KStrElementAttriNetWork       = "Server";
const char*   KStrElementAttriNetWorkName   = "Name";
const char*   KStrElementAttriNetWorkURL    = "IP";
const char*   KStrElementAttriNetWorkPort   = "Port";
const char*   KStrElementAttriNetWorkSelected = "selected";
const char*   KStrElementAttriNetWorkRunMode = "RunMode";
const char*   KStrElementAttriSimulateTrade  = "STradeInfo";	//ģ�⽻��
//////////////////////////////////////////////////////////////////////////
//
CServerConfigManager & CServerConfigManager::Instance()
{
	static CServerConfigManager manager;
	return manager;
}

CServerConfigManager::CServerConfigManager()
{
	FromXml();
}

bool32 CServerConfigManager::FromXml()
{
	// �ӹ�����config Ŀ¼�� Xml �ļ��л�÷�������Ϣ:

	// 1: ��ձ�������:
	m_aQuotes.RemoveAll();
	m_aTrades.RemoveAll();
	m_aNews.RemoveAll();

	// 2: ���ļ�:
	CString StrPath  = CPathFactory::GetServerInfoPath();
	
	if ( StrPath.IsEmpty() )
	{
		return FALSE;
	}

	TiXmlDocument myDocument = TiXmlDocument(_Unicode2MultiChar(StrPath).c_str());

	if ( !myDocument.LoadFile())
	{
		return FALSE;
	}

	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return FALSE;
	}
	
	// ServerInfoQuote, �����������Ϣ
	TiXmlElement* pQuoteServersElement = pRootElement->FirstChildElement(KStrElementNameServerInfoQuote);
	if( NULL != pQuoteServersElement && NULL != pQuoteServersElement->Value()  )
	{
		ASSERT( 0 == strcmp(pQuoteServersElement->Value(), KStrElementNameServerInfoQuote) );
		TiXmlElement* pServerElement = pQuoteServersElement->FirstChildElement(KStrElementNameServer);

		for ( ; NULL!=pServerElement ; pServerElement=pServerElement->NextSiblingElement(KStrElementNameServer) )
		{
			T_HostInfo ServerInfo;
			
			// ����:
			const char* StrName = pServerElement->Attribute(KStrElementAttriServerName);
			
			if ( NULL == StrName )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// IP:
			const char* StrIp = pServerElement->Attribute(KStrElementAttriServerIp);
			
			if ( NULL == StrIp )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// �˿�:
			const char* StrPort = pServerElement->Attribute(KStrElementAttriServerPort);
			
			if ( NULL == StrPort )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			} 
			
			// ��������:
			UINT uiNetType = 1;
			
			const char* StrNetType = pServerElement->Attribute(KStrElementAttriServerNetType);
			if ( NULL != StrNetType )
			{
				uiNetType = atoi(StrNetType);				
			}
			
			// ת������:
			TCHAR TStrHostName[1024];
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			
			//
			TCHAR TStrHostAdd[1024];
			memset(TStrHostAdd, 0, sizeof(TStrHostAdd));
			MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrHostAdd, sizeof(TStrHostAdd) / sizeof(TCHAR));
			
			//
			ServerInfo.StrHostName		= TStrHostName;
			ServerInfo.StrHostAddress	= TStrHostAdd;
			ServerInfo.uiHostPort		= (UINT)atoi(StrPort);
			ServerInfo.uiNetType		= uiNetType;
			
			m_aQuotes.Add(ServerInfo);
		}
	}


	// ServerInfoTrades, ���׷�������Ϣ
	TiXmlElement* pTradesServersElement = pRootElement->FirstChildElement(KStrElementNameServerInfoTrades);
	if( NULL != pTradesServersElement && NULL != pTradesServersElement->Value()  )
	{
		ASSERT( 0 == strcmp(pTradesServersElement->Value(), KStrElementNameServerInfoTrades) );
		TiXmlElement* pServerElement = pTradesServersElement->FirstChildElement(KStrElementNameServer);

		for ( ; NULL!=pServerElement ; pServerElement=pServerElement->NextSiblingElement(KStrElementNameServer) )
		{
			T_HostInfo ServerInfo;
			
			// ����:
			const char* StrName = pServerElement->Attribute(KStrElementAttriServerName);
			
			if ( NULL == StrName )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// IP:
			const char* StrIp = pServerElement->Attribute(KStrElementAttriServerIp);
			
			if ( NULL == StrIp )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// �˿�:
			const char* StrPort = pServerElement->Attribute(KStrElementAttriServerPort);
			
			if ( NULL == StrPort )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			} 
			
			// ��������:
			UINT uiNetType = 1;
			
			const char* StrNetType = pServerElement->Attribute(KStrElementAttriServerNetType);
			if ( NULL != StrNetType )
			{
				uiNetType = atoi(StrNetType);				
			}
			
			// ת������:
			TCHAR TStrHostName[1024];
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			
			//
			TCHAR TStrHostAdd[1024];
			memset(TStrHostAdd, 0, sizeof(TStrHostAdd));
			MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrHostAdd, sizeof(TStrHostAdd) / sizeof(TCHAR));
			
			//
			ServerInfo.StrHostName		= TStrHostName;
			ServerInfo.StrHostAddress	= TStrHostAdd;
			ServerInfo.uiHostPort		= (UINT)atoi(StrPort);
			ServerInfo.uiNetType		= uiNetType;
			
			m_aTrades.Add(ServerInfo);
		}
	}

	return TRUE;
}

bool32 CServerConfigManager::GetTradeServers( OUT HostInfoArray &aServers )
{
	aServers.Copy(m_aTrades);
	return aServers.GetSize() > 0;
}

bool32 CServerConfigManager::GetQuoteServers( OUT HostInfoArray &aServers )
{
	aServers.Copy(m_aQuotes);
	return aServers.GetSize() > 0;
}

bool32 CServerConfigManager::GetNewsServers( OUT HostInfoArray &aServers )
{
	aServers.Copy(m_aNews);
	return aServers.GetSize() > 0;
}

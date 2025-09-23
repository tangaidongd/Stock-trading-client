#include "StdAfx.h"
#include "UrlConfig.h"
#include "PathFactory.h"

const char* KStrElementAlias			= "UrlAlias";
const char* KStrElementAttriName		= "Name";
const char* KStrElementAttriUrl			= "Url";

const CString KStrFileUrlConfig			= L"urlconfig.xml";

CUrlConfig::CUrlConfig():m_bInit(false)
{

}


CUrlConfig & CUrlConfig::Instance()
{
	static CUrlConfig config;
	config.Initialize();		// 仅默认的一次初始化	
	return config;
}

bool CUrlConfig::Initialize()
{
	if (!m_bInit)
	{
		CString filePath = CPathFactory::GetPublicConfigPath() + KStrFileUrlConfig;

		TiXmlDocument Doc(_W2A(filePath));
		if (!Doc.LoadFile())
		{
			return false;
		}

		TiXmlElement* pRootElement = Doc.RootElement();
		if ( NULL == pRootElement )
		{
			return false;
		}

		TiXmlElement* pElement = pRootElement->FirstChildElement();
		const char* pElementValue = pElement->Value();
		if (NULL == pElementValue)
		{
			return false;
		}
		if (0 == strcmp(pElementValue, KStrElementAlias))
		{
			for(TiXmlElement* pElemChild= pElement->FirstChildElement(); NULL != pElemChild; pElemChild = pElemChild->NextSiblingElement())
			{
				const char* pNameValue = pElemChild->Attribute(KStrElementAttriName);
				const char* pUrlValue = pElemChild->Attribute(KStrElementAttriUrl);
				
				if (NULL == pNameValue || NULL == pUrlValue)
				{
					continue;
				}

				m_mapUrlAlias[_A2W(pNameValue)] = _A2W(pUrlValue);
			}
		}

		
		m_bInit = true;
	}
	return m_bInit;
}



CString CUrlConfig::GetUrl(const LPCTSTR &AliasName)
{
	CString strUrl;
	map<CString,CString>::const_iterator itName = m_mapUrlAlias.find(AliasName);
	if ( itName != m_mapUrlAlias.end() )
	{
		strUrl = itName->second;
	}
	
	return strUrl;
}
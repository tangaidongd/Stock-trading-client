#include "stdafx.h"

#include "tinyxml.h"

#include "MerchBlocks.h"
#include "UserBlockManager.h"
#include "ShareFun.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CMBXmlNode::SetUserData(void* p )
{
	m_pUserData = p;
}
void* CMBXmlNode::GetUserData()
{
	return m_pUserData;
}

CMBXmlBlock::CMBXmlBlock()
{
	m_eType = XNBlock;
	m_pUserData = NULL;
}

CMBXmlUserBlock::CMBXmlUserBlock()
{
	m_eType		= XNBlock;
	m_pUserData	= NULL;

	m_ClrUserBlock	  = CFaceScheme::Instance()->GetSysColor(ESCAmount);
	m_eReportHeadTpye = ERTCustom;	
}

CMerchBlocks::CMerchBlocks()
{
	m_pXmlDocument		= NULL;
	m_bContinueBrowser	= true;
	m_bVersion2			= false;
}

CMerchBlocks::~CMerchBlocks()
{
	Free();
}
bool32 CMerchBlocks::Load ( const char* strFile )
{
	//Free First!
	Free();

	//load xml
	m_pXmlDocument = new TiXmlDocument(strFile);
	if ( !m_pXmlDocument->LoadFile() )
	{
		Free();
		return false;
	}
	return true;
}
void CMerchBlocks::Free ( )
{
	if ( NULL != m_pXmlDocument )
	{
		delete m_pXmlDocument;
		m_pXmlDocument = NULL;
	}
}
void CMerchBlocks::Save()
{
	if ( NULL != m_pXmlDocument )
	{
		chmod(m_pXmlDocument->Value(),_S_IWRITE);
		m_pXmlDocument->SaveFile();
	}
}
void CMerchBlocks::Browser ( TiXmlNode* pStartNode,bool32 bVersion2)
{
	m_bContinueBrowser = true;
	m_bVersion2 = bVersion2;
	DoBrowser ( pStartNode);
}
void CMerchBlocks::DoBrowser ( TiXmlNode* pStartNode)
{
	if ( NULL == m_pXmlDocument )
	{
		return;
	}
	if ( !m_bContinueBrowser )
	{
		return;
	}
	if ( NULL == pStartNode )
	{
		pStartNode = m_pXmlDocument->RootElement();
	}

	for(TiXmlNode *pNode = pStartNode->FirstChild(); pNode; pNode = pNode->NextSibling())
	{
		if ( pNode->Type() == TiXmlNode::ELEMENT )
		{
			DoNode((TiXmlElement*)pNode);
		}
		if ( !m_bContinueBrowser )
		{
			break;
		}
		DoBrowser(pNode);
	}
}
void CMerchBlocks::DoNode( TiXmlElement* pNode )
{
	const char* strValue = pNode->Value();
	if ( NULL == strValue || '\0' == strValue[0] ) return;

	//block
	if ( 0 == strcmp( KStrElementBlockName, strValue ) )
	{
		// һ������û�������Ϣ�ǲ�һ����:
		
		const char* strName   = pNode->Attribute(KStrElementBlockAttriName);			// ��pNode�л�ȡName����
		const char* strHotkey = pNode->Attribute(KStrElementBlockAttriHotKey);			// ��pNode�л�ȡhotkey����
		const char* strColor  = pNode->Attribute(KStrElementBlockAttriColor);			// ��pNode�л�ȡcolor����
		const char* strHead   = pNode->Attribute(KStrElementBlockAttriHead);			// ��pNode�л�ȡhead����
		
		if ( NULL != strColor && NULL != strHead)
		{
			// �����Ĵ���
			CMBXmlUserBlock BlockNode;

			BlockNode.m_StrName			= _A2W(strName);
			BlockNode.m_StrHotkey		= _A2W(strHotkey );
			BlockNode.m_ClrUserBlock	= atol(strColor);
			BlockNode.m_eReportHeadTpye = (E_ReportType)atol(strHead);

			if ( m_bVersion2 )
			{
				m_bContinueBrowser = OnBrowserSub2 ( pNode,&BlockNode,pNode->Parent()->GetUserData());
			}
			else
			{
				m_bContinueBrowser = OnBrowserSub ( &BlockNode,pNode->Parent()->GetUserData());
			}

			pNode->SetUserData(BlockNode.GetUserData());

		}
		else
		{
			// ԭ���Ĵ���
			CMBXmlBlock BlockNode;
			BlockNode.m_StrName = _A2W(strName);
			BlockNode.m_StrHotkey = _A2W(strHotkey );
			
			if ( m_bVersion2 )
			{
				m_bContinueBrowser = OnBrowserSub2 ( pNode,&BlockNode,pNode->Parent()->GetUserData());
			}
			else
			{
				m_bContinueBrowser = OnBrowserSub ( &BlockNode,pNode->Parent()->GetUserData());
			}
			pNode->SetUserData(BlockNode.GetUserData());
		}
		
	}
	//merch
	if ( 0 == strcmp( KStrElementMerchName, strValue ) )
	{
		CMBXmlMerch MerchNode;
		const char* strMerchCode = pNode->Attribute(KStrElementMerchAttriCode);//��pNode�л�ȡcode����
		const char* strMarketId = pNode->Attribute(KStrElementMerchAttriMarket);//��pNode�л�ȡmarket����
		int32 iMarketId = atoi ( strMarketId);
		CString StrMerchCode = _A2W (strMerchCode);

		CGGTongDoc *pDoc = AfxGetDocument();
		CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if (!pAbsCenterManager)
		{
			return;
		}

		if (!pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, MerchNode.m_pMerch))
			return;

		if ( m_bVersion2 )
		{
			m_bContinueBrowser = OnBrowserSub2 ( pNode, &MerchNode,pNode->Parent()->GetUserData());
		}
		else
		{
			m_bContinueBrowser = OnBrowserSub ( &MerchNode,pNode->Parent()->GetUserData());
		}
		pNode->SetUserData(MerchNode.GetUserData());
	}
}
bool32 CMerchBlocks::OnBrowserSub2 ( TiXmlNode* pRawNode,CMBXmlNode* pNode,void* pParentUserData )
{
	return false;
}
bool32 CMerchBlocks::OnBrowserSub ( CMBXmlNode* pNode,void* pParentUserData )
{
	return false;
}
bool32 CMerchBlocks::GetBlockMerchs ( CString StrBlock, CStringArray& MerchsCode,CArray<int,int>& MerchsMakretId)
{
	return true;
}

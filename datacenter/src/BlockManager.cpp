#include "StdAfx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <algorithm>
#include "BlockManager.h"
#include "MerchManager.h"
#include "ShareFun.h"
#include "PathFactory.h"

//#include "UserBlockManager.h"
#include "viewdata.h"
#include "coding.h"

using namespace std; 

CSysBlockManager*	CSysBlockManager::m_pThis	= NULL;
CUserBlockManager*	CUserBlockManager::m_pThis	= NULL;

//
static const char* KStrElementRootName			= "blocks";
static const char* KStrElementMerchName			= "merch";
static const char* KStrElementMerchAttriMarket	= "market";
static const char* KStrElementMerchAttriCode	= "code";
static const char* KStrElementMerchAttriFake	= "fakename";

static const char* KStrElementBlockName			= "block";
static const char* KStrElementBlockAttriName	= "name";
static const char* KStrElementBlockAttriHotKey	= "hotkey";

static const char* KStrElementBlockAttriColor	= "color";
static const char* KStrElementBlockAttriHead	= "head";
const char* KStrElementBlockAttriServer = "server";

//////////////////////////////////////////////////////////////////////////
// ����


CSubjectUserBlock::CSubjectUserBlock()
{
	m_aObservers.RemoveAll();
}

CSubjectUserBlock::~CSubjectUserBlock()
{
	m_aObservers.RemoveAll();
}

void CSubjectUserBlock::AddObserver(CObserverUserBlock* pObserver)
{
	m_aObservers.Add(pObserver);
}

void CSubjectUserBlock::DelObserver(CObserverUserBlock* pObserver)
{
	for ( int32 i = 0; i < m_aObservers.GetSize(); i++ )
	{
		if ( m_aObservers[i] == pObserver )
		{
			m_aObservers.RemoveAt(i);
			return;
		}
	}
}

void CSubjectUserBlock::Notify(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	for ( int32 i = 0; i < m_aObservers.GetSize(); i++ )
	{
		if ( NULL != m_aObservers[i] )
		{
			m_aObservers[i]->OnUserBlockUpdate(eUpdateType);
		}		
	}
}

//////////////////////////////////////////////////////////////////////////
// �۲���
CObserverUserBlock::CObserverUserBlock()
{
	m_pSubject = CUserBlockManager::Instance();
	m_pSubject->AddObserver(this);
}

CObserverUserBlock::~CObserverUserBlock()
{
	if ( NULL != m_pSubject )
	{
		m_pSubject->DelObserver(this);
	}
}

//////////////////////////////////////////////////////////////////////////
//
CBlockManager::CBlockManager()
{

}
CBlockManager::~CBlockManager()
{
	DEL(m_pXmlDoc);

	///> �ͷ���Դ
	m_aMerchsAll.RemoveAll();
	m_aBlocksAll.RemoveAll();
}
void CBlockManager::Construct(CString StrPath)
{
	m_pXmlDoc = NULL;
	m_StrPath = StrPath;
	
	m_aMerchsAll.RemoveAll();
	m_aBlocksAll.RemoveAll();
}

bool32 CBlockManager::LoadXmlFile(CMerchManager* pManager)
{
	if ( m_StrPath.GetLength() <= 0 )
	{
		return false;
	}

	if ( NULL != m_pXmlDoc )
	{
		TRACE(_T("�û�����ļ������أ�\r\n"));
		DEL(m_pXmlDoc);
	}

	//
	std::string StrPathA;
	Unicode2MultiChar(CP_ACP, m_StrPath, StrPathA);	
	m_pXmlDoc = new TiXmlDocument(StrPathA.c_str());
	if ( NULL == m_pXmlDoc )
	{		
		return false;
	}
	
	if ( !m_pXmlDoc->LoadFile(StrPathA.c_str()) )
	{
		if ( !AutoCreateXmlFile() )
		{
			DEL(m_pXmlDoc);
			return false;
		}
	}

	// �����
	m_aBlocksAll.RemoveAll();
	m_aMerchsAll.RemoveAll();

	// ���ڵ�
	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	// ���ڵ�
	TiXmlElement* pBlockElement = pRootElement->FirstChildElement();

	while( pBlockElement )
	{
		const char* pStrName	= pBlockElement->Attribute(KStrElementBlockAttriName);
		const char* pStrHotKey	= pBlockElement->Attribute(KStrElementBlockAttriHotKey);
		const char* pStrColor	= pBlockElement->Attribute(KStrElementBlockAttriColor);
		const char* pStrHead	= pBlockElement->Attribute(KStrElementBlockAttriHead);
		const char* pStrServer	= pBlockElement->Attribute(KStrElementBlockAttriServer);
		
		if ( NULL == pStrName )
		{
			pBlockElement = pBlockElement->NextSiblingElement();
			continue;
		}

		// ����
		TCHAR TStrName[1024];
		memset(TStrName, 0, sizeof(TStrName));
		MultiCharCoding2Unicode(EMCCUtf8, pStrName, strlen(pStrName), TStrName, sizeof(TStrName) / sizeof(TCHAR));

		T_Block	stBlockToAdd;
		stBlockToAdd.m_StrName = (CString)TStrName;

		// ��ݼ�
		if ( NULL == pStrHotKey )
		{
			stBlockToAdd.m_StrHotKey = ConvertHZToPY(stBlockToAdd.m_StrName);
		}
		else
		{
			TCHAR TStrHotKey[1024];
			memset(TStrHotKey, 0, sizeof(TStrHotKey));
			MultiCharCoding2Unicode(EMCCUtf8, pStrHotKey, strlen(pStrHotKey), TStrHotKey, sizeof(TStrHotKey) / sizeof(TCHAR));
			
			stBlockToAdd.m_StrHotKey = (CString)TStrHotKey;
		}

		// ��ɫ
		if ( NULL != pStrColor )
		{
			stBlockToAdd.m_clrBlock = atoi(pStrColor);
		}

		// ��ͷ
		if ( NULL != pStrHead )
		{
			stBlockToAdd.m_eHeadType = (E_ReportType)atoi(pStrHead);
		}

		// server
		if ( NULL != pStrServer )
		{
			stBlockToAdd.m_bServerBlock = atoi(pStrServer) != 0;
		}

		// XML �ڵ�
		stBlockToAdd.m_pXmlElement = pBlockElement;

		// �������µ���Ʒ
		TiXmlElement* pMerchElement = pBlockElement->FirstChildElement();
		CArray<TiXmlElement*, TiXmlElement*> aDelNodes;
		while ( pMerchElement )
		{
			// ����
			const char* pStrMerchCode	= pMerchElement->Attribute(KStrElementMerchAttriCode);
			if ( NULL == pStrMerchCode )
			{
				pMerchElement = pMerchElement->FirstChildElement();
				continue;
			}

			// �г���
			const char* pStrMarketId	= pMerchElement->Attribute(KStrElementMerchAttriMarket);
			if ( NULL == pStrMarketId )
			{
				pMerchElement = pMerchElement->FirstChildElement();
				continue;
			}
			int32 iMarketId = atoi(pStrMarketId);

			// ����
			const char* pStrMerchFake	= pMerchElement->Attribute(KStrElementMerchAttriFake);
			if ( NULL == pStrMerchFake )
			{
				pStrMerchFake = "";
			}
			

			if ( NULL != pManager )
			{
				CMerch* pMerch = NULL;
				pManager->FindMerch(_A2W(pStrMerchCode), iMarketId, pMerch);

				if ( NULL != pMerch )
				{
					pMerch->m_StrMerchFakeName = _A2W(pStrMerchFake);

					// �ӵ����������Ʒ�б���:
					stBlockToAdd.m_aMerchs.Add(pMerch);

					// Map ����
					stBlockToAdd.m_aMapMerchToXml[pMerch] = pMerchElement;

					// �ӵ���������Ʒ�б���:
					m_aMerchsAll.Add(pMerch);
				}
				else
				{
					aDelNodes.Add(pMerchElement);
				}
			}
			
			pMerchElement = pMerchElement->NextSiblingElement();
		}

		// ��������
		m_aBlocksAll.Add(stBlockToAdd);

		int iSize = aDelNodes.GetSize();
		for (int index=0; index<iSize; index++)
		{
			pBlockElement->RemoveChild(aDelNodes[index]);
		}

		if (0 < iSize)
		{
			SaveXmlFile();
		}

		//
		pBlockElement = pBlockElement->NextSiblingElement();
	}

	return true;
}

bool32 CBlockManager::SaveXmlFile()
{
	if ( m_StrPath.GetLength() <= 0 )
	{
		return false;
	}

	if (0 == chmod(m_pXmlDoc->Value(), _S_IWRITE))
	{
		m_pXmlDoc->SaveFile();
		return true;
	}

	return false;
}

T_Block* CBlockManager::GetBlock(IN const CString& StrBlockName)
{
	if ( StrBlockName.GetLength() <= 0 )
	{
		return NULL;
	}

	for ( int32 i = 0; i < m_aBlocksAll.GetSize(); i++ )
	{
		if ( StrBlockName == m_aBlocksAll[i].m_StrName )
		{
			return &m_aBlocksAll[i];
		}
	}

	return NULL;
}

T_Block* CBlockManager::GetBlock(IN CMerch* pMerch)
{	
	// һ����Ʒ�������ڶ����ѡ��, ֻ�����ҵ���һ��ƥ��İ��
	if ( NULL == pMerch )
	{
		return NULL;
	}

	//
	CArray<T_Block, T_Block&> aBlocks;
	GetBlocks(aBlocks);

	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	{
		if ( BeMerchInBlock(pMerch, aBlocks[i].m_StrName) )
		{
			T_Block* pBlock = GetBlock(aBlocks[i].m_StrName);
			return pBlock;
		}
	}

	return NULL;
}

void CBlockManager::GetBlocks(OUT CArray<T_Block, T_Block&>& aBlocks)
{
	aBlocks.RemoveAll();
	aBlocks.Copy(m_aBlocksAll);
}

void CBlockManager::GetMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs)
{
	aMerchs.RemoveAll();
	aMerchs.Copy(m_aMerchsAll);
}

bool32 CBlockManager::GetMerchsInBlock(IN const CString& StrBlockName, OUT CArray<CMerch*, CMerch*>& aMerchs)
{
	aMerchs.RemoveAll();

	if ( StrBlockName.GetLength() <= 0 )
	{
		return false;
	}

	//
	T_Block* pBlocks = GetBlock(StrBlockName);
	
	if ( NULL == pBlocks )
	{
		return false;
	}

	aMerchs.Copy(pBlocks->m_aMerchs);
	
	return true;
}

bool32 CBlockManager::BeMerchInBlock(IN const CMerch* pMerch, IN const CString& StrBlockName)
{
	if ( NULL == pMerch || StrBlockName.GetLength() <= 0 )
	{
		return false;
	}

	//
	CArray<CMerch*, CMerch*> aMerchs;

	if ( !GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return false;
	}

	for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
	{
		if ( aMerchs[i] == pMerch )
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳ���

CSysBlockManager* CSysBlockManager::Instance()
{
	if ( NULL == m_pThis )
	{
		// ��һ��:
		m_pThis = new CSysBlockManager();		
	}

	return m_pThis;
}

void CSysBlockManager::DelInstance()
{
	DEL(m_pThis);
}
bool32 CSysBlockManager::AutoCreateXmlFile()
{
	return true;
}

bool CSysBlockManager::Initialize(CMerchManager* pManager)
{
	CString StrPath = CPathFactory::GetSysBlocksFileFullName();

	Construct(StrPath);		
	if ( !LoadXmlFile(pManager) )
	{
		//ASSERT(0);  // �Ѿ��Ƴ���xml
		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �û����

CUserBlockManager* CUserBlockManager::Instance()
{
	if ( NULL == m_pThis )
	{
		// ��һ��:
		m_pThis = new CUserBlockManager();
	}
	
	return m_pThis;
}

void CUserBlockManager::DelInstance()
{
	DEL(m_pThis);
}

bool CUserBlockManager::Initialize(CString strUserName,CMerchManager* pManager)
{
	m_pMerchManager = pManager;
	if (strUserName.IsEmpty())
	{
		ASSERT(0);
		return false;
	}

	CString StrPath = CPathFactory::GetUserBlocksFileFullName(strUserName);
	
	CString StrPublicPath  = CPathFactory::GetUserBlocksPulbicFilePullName();
	std::string StrPublicPathA;
	Unicode2MultiChar(CP_ACP, StrPublicPath, StrPublicPathA);	
	// ����û�����ļ������ڣ��ҹ���Ŀ¼���кϷ��Ĺ����û�����ļ�����copy����������ԭ������
	if ( _taccess(StrPath, 0) < 0 )	// ������˽��
	{
		// ���public���Ƿ�Ϸ�
		TiXmlDocument myDocument = TiXmlDocument(StrPublicPathA.c_str());
		if ( myDocument.LoadFile() )
		{	
			TiXmlElement *pRootElement = myDocument.RootElement();
			if ( NULL != pRootElement )
			{
				pRootElement = pRootElement->FirstChildElement();
				if (NULL != pRootElement)
				{
					const char * pcValue = pRootElement->Value();
					if (NULL != pcValue && 0 == strcmp( KStrElementBlockName, pcValue ))
					{
						// ��block���ʣ�����Ϊ�ǺϷ����˰� copy�ļ�
						::CopyFile(StrPublicPath, StrPath, TRUE);
					}
				}
			}
		}
	}

	Construct(StrPath);
	if ( !LoadXmlFile(pManager) )
	{
		ASSERT(0);
		return false;
	}

	return true;
}

bool32 CUserBlockManager::AutoCreateXmlFile()
{
	if ( m_StrPath.GetLength() <= 0 )
	{
		ASSERT(0);
		return false;
	}
	
	
	
	std::string StrPathA;
	Unicode2MultiChar(CP_ACP, m_StrPath, StrPathA);	
	if ( 0 != _access ( StrPathA.c_str(), 0 ))
	{
		//CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='workspace'>\n<block name='��ѡ��1' hotkey='zxg1' color='16776960' head='12' >\n</block>\n</XMLDATA>");
		CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='workspace'>\n<block name='�ҵ���ѡ' hotkey='wdzx' color='16776960' head='12' server='1'>\n</block>\n</XMLDATA>");	// ���ֻ�������ͬ, Ĭ�ϵĵ�һ�������serverͬ����
		
		if ( NULL == m_pXmlDoc )
		{
			m_pXmlDoc = new TiXmlDocument(StrPathA.c_str());
		}
		
		ASSERT(NULL != m_pXmlDoc);
		
		//
		int32 iOutBufferSize = StrDefaultXml.GetLength();
		iOutBufferSize *= 2;
		iOutBufferSize += 100;		// ��ֹStrContent����Ϊ0
		char *pcOutBuffer = new char[iOutBufferSize];
		
		if (NULL != pcOutBuffer)
		{
			// ��ת��д���ļ�������
			memset(pcOutBuffer, 0, iOutBufferSize);
			Unicode2MultiCharCoding(EMCCUtf8, StrDefaultXml, StrDefaultXml.GetLength(), pcOutBuffer, iOutBufferSize);
			
			// ת���ļ���
			wchar_t awcFileName[MAX_PATH];
			memset(awcFileName, 0, sizeof(awcFileName));
			MultiCharCoding2Unicode(EMCCSystem, StrPathA.c_str(), StrPathA.length(), awcFileName, sizeof(awcFileName) / sizeof(wchar_t));
			
			CFile File;
			if (File.Open(awcFileName, CFile::modeCreate | CFile::modeWrite))
			{
				File.Write(pcOutBuffer, strlen(pcOutBuffer));
				File.Close();
			}
			
			//
			m_pXmlDoc->Parse(pcOutBuffer);
			
			// ��Ҫ�����ͷ�
			DEL_ARRAY(pcOutBuffer);
			
			return true;
		}		
	}
	
	return false;
}
bool32 CUserBlockManager::AddMerchToUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify /*= true*/)
{
	if ( NULL == pMerch || StrBlockName.GetLength() <= 0 )
	{
		return false;
	}

	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	// 0: �ж��Ƿ��ظ����:
	if ( BeMerchInBlock(pMerch, StrBlockName) )
	{
		return false;
	}

	// ���������������:

	// 1: XML ���ݸ���
	char strMerchCode[256];
	memset(strMerchCode, 0, sizeof(strMerchCode));
	Unicode2MultiCharCoding(EMCCUtf8, pMerch->m_MerchInfo.m_StrMerchCode, pMerch->m_MerchInfo.m_StrMerchCode.GetLength(), strMerchCode, 256);

	char strMerchFake[256];
	memset(strMerchFake, 0, sizeof(strMerchFake));
	Unicode2MultiCharCoding(EMCCUtf8, pMerch->m_StrMerchFakeName, pMerch->m_StrMerchFakeName.GetLength(), strMerchFake, 256);
	
	char  strMarketId[256];
	sprintf(strMarketId, "%d", pMerch->m_MerchInfo.m_iMarketId);
	
	TiXmlElement* pMerchElement = new TiXmlElement(KStrElementMerchName);
	pMerchElement->SetAttribute(KStrElementMerchAttriCode, strMerchCode);
	pMerchElement->SetAttribute(KStrElementMerchAttriFake, strMerchFake);
	pMerchElement->SetAttribute(KStrElementMerchAttriMarket, strMarketId);

	pBlock->m_pXmlElement->LinkEndChild(pMerchElement);			

	// 2: ���ݳ�Ա��������
	pBlock->m_aMapMerchToXml[pMerch] = pMerchElement;
	pBlock->m_aMerchs.Add(pMerch);

	// ���������ļ��е�����
	m_aMerchsAll.Add(pMerch);

	if ( bAutoSaveAndNotify )
	{
		//
		SaveXmlFile();
	
		// ֪ͨ�۲���,���ݸ�����:
		Notify(CSubjectUserBlock::EUBUMerch);
	}

	return true;
}

bool32 CUserBlockManager::DelMerchFromUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify /*= true*/)
{
	if ( NULL == pMerch || StrBlockName.GetLength() <= 0 )
	{
		return false;
	}
	
	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	if ( !BeMerchInBlock(pMerch, StrBlockName) )
	{
		return false;
	}
	
	// ���������������:
	
	// 1: XML ���ݸ���
	std::map<CMerch*, TiXmlElement*>::iterator itFind = pBlock->m_aMapMerchToXml.find(pMerch);	
	
	if ( itFind == pBlock->m_aMapMerchToXml.end() )
	{
		ASSERT(0);
		return false;		
	}

	TiXmlElement* pMerchElement = itFind->second;
	if ( NULL == pMerchElement )
	{
		return false;
	}

	pBlock->m_pXmlElement->RemoveChild(pMerchElement);
	
	// 2: ���ݳ�Ա��������
	int32 i;
	for ( i = 0; i < pBlock->m_aMerchs.GetSize(); i++ )
	{
		if ( pBlock->m_aMerchs[i] == pMerch )
		{
			pBlock->m_aMerchs.RemoveAt(i);
			break;
		}
	}

	//
	pBlock->m_aMapMerchToXml.erase(itFind);
 
	// ���������ļ�����
	for ( i = 0; i < m_aMerchsAll.GetSize(); i++ )
	{
		if ( m_aMerchsAll[i] == pMerch )
		{
			m_aMerchsAll.RemoveAt(i);
			break;
		}
	}

	if ( bAutoSaveAndNotify )
	{
		//
		SaveXmlFile();
		
		// ֪ͨ�۲���,���ݸ�����:
		Notify(CSubjectUserBlock::EUBUMerch);
	}

	return true;
}

bool32 CUserBlockManager::ReplaceUserBlockMerchs(IN const CArray<CMerch*, CMerch*>& aMerchsNew, IN const CString& StrBlockName, bool32 bSaveBefore /*= false*/)
{
	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	//
	if ( !bSaveBefore )
	{
		// ȫ���滻				
		pBlock->m_aMerchs.RemoveAll();
		pBlock->m_pXmlElement->Clear();
		pBlock->m_aMapMerchToXml.clear();

		for ( int32 i = 0; i < aMerchsNew.GetSize(); i++ )
		{
			CMerch* pMerch = aMerchsNew.GetAt(i);
			if ( NULL == pMerch )
			{
				continue;
			}

			// ������Ʒ
			pBlock->m_aMerchs.Add(pMerch);
			
			// ���� XML �ڵ�
			char strMerchCode[256];
			memset(strMerchCode, 0, sizeof(strMerchCode));
			Unicode2MultiCharCoding(EMCCUtf8, pMerch->m_MerchInfo.m_StrMerchCode, pMerch->m_MerchInfo.m_StrMerchCode.GetLength(), strMerchCode, 256);
			
			char strMerchFake[256];
			memset(strMerchFake, 0, sizeof(strMerchFake));
			Unicode2MultiCharCoding(EMCCUtf8, pMerch->m_StrMerchFakeName, pMerch->m_StrMerchFakeName.GetLength(), strMerchFake, 256);

			char  strMarketId[256];
			sprintf(strMarketId, "%d", pMerch->m_MerchInfo.m_iMarketId);

			TiXmlElement* pMerchElement = new TiXmlElement(KStrElementMerchName);
			pMerchElement->SetAttribute(KStrElementMerchAttriCode, strMerchCode);
			pMerchElement->SetAttribute(KStrElementMerchAttriFake, strMerchFake);
			pMerchElement->SetAttribute(KStrElementMerchAttriMarket, strMarketId);
			
			pBlock->m_pXmlElement->LinkEndChild(pMerchElement);			

			// ���� map
			pBlock->m_aMapMerchToXml[pMerch] = pMerchElement;
		}
		
	
	}
	else
	{
		for ( int32 i = 0; i < aMerchsNew.GetSize(); i++ )
		{
			CMerch* pMerch = aMerchsNew.GetAt(i);
			if ( NULL == pMerch )
			{
				continue;
			}
			
			// ֱ��������Ӿ�����, �����ڲ����ж��Ƿ��ظ�
			AddMerchToUserBlock(pMerch, StrBlockName, false);
		}
	}
	
	// �����ļ�
	SaveXmlFile();
	
	// ֪ͨ
	Notify(CSubjectUserBlock::EUBUMerch);

	return true;
}

bool32 CUserBlockManager::AddUserBlock(IN T_Block& BlockInfoNew)
{
	if ( BlockInfoNew.m_StrName.GetLength() <= 0 )
	{
		return false;
	}

	// ����XML ����
	if ( NULL == m_pXmlDoc )
	{
		return false;
	}

	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	char strName[512];
	char strHotkey[512];
	char strColor[512];
	char strHead[512];
	char strServer[20];

	memset(strName, 0, sizeof(strName));
	Unicode2MultiCharCoding(EMCCUtf8, BlockInfoNew.m_StrName, BlockInfoNew.m_StrName.GetLength(), strName, 512);
	
	memset(strHotkey, 0, sizeof(strHotkey));
	Unicode2MultiCharCoding(EMCCUtf8, BlockInfoNew.m_StrHotKey, BlockInfoNew.m_StrHotKey.GetLength(), strHotkey, 512);

	memset(strColor, 0, sizeof(strColor));
	sprintf(strColor, "%lu", (unsigned long)BlockInfoNew.m_clrBlock);
	
	memset(strHead, 0, sizeof(strHead));
	sprintf(strHead, "%d", (int32)BlockInfoNew.m_eHeadType);

	memset(strServer, 0, sizeof(strServer));
	sprintf(strServer, "%s", BlockInfoNew.m_bServerBlock ? "1" : "0");

	TiXmlElement* pBlockElement = new TiXmlElement(KStrElementBlockName);
	pBlockElement->SetAttribute(KStrElementBlockAttriName, strName);
	pBlockElement->SetAttribute(KStrElementBlockAttriHotKey, strHotkey);
	pBlockElement->SetAttribute(KStrElementBlockAttriColor, strColor);
	pBlockElement->SetAttribute(KStrElementBlockAttriHead, strHead);
	pBlockElement->SetAttribute(KStrElementBlockAttriServer, strServer);
	
	pRootElement->LinkEndChild(pBlockElement);

	// �������ݳ�Ա������
	BlockInfoNew.m_pXmlElement = pBlockElement;
	m_aBlocksAll.Add(BlockInfoNew);

	//
	SaveXmlFile();

	// ֪ͨ�۲���,���ݸ�����:
	Notify(CSubjectUserBlock::EUBUBlock);

	return true;
}

bool32 CUserBlockManager::ModifyUserBlock(IN const CString& StrBlockNameOld, IN const T_Block& BlockInfoNew)
{
	// �޸�ֻ���޸�����,��ݼ�,��ɫ,��ͷ �⼸���ֶ�
	T_Block* pBlock = GetBlock(StrBlockNameOld);
	
	if ( NULL == pBlock )
	{
		return false;
	}


	// �޸ĳ�Ա��������
	if ( pBlock->m_StrName != BlockInfoNew.m_StrName )
	{
		pBlock->m_bChangeName	= true;
		pBlock->m_StrNameOld	= pBlock->m_StrName;
	}

	pBlock->m_StrName		= BlockInfoNew.m_StrName;
	pBlock->m_StrHotKey		= BlockInfoNew.m_StrHotKey;
	pBlock->m_clrBlock		= BlockInfoNew.m_clrBlock;
	pBlock->m_eHeadType		= BlockInfoNew.m_eHeadType;
	//pBlock->m_bServerBlock ���Բ���

	// �޸� XML ����:
	if ( NULL == pBlock->m_pXmlElement )
	{
		ASSERT(0);
		return false;
	}

	char strName[512];
	char strHotkey[512];
	char strColor[512];
	char strHead[512];
	char strServer[20];
	
	memset(strName, 0, sizeof(strName));
	Unicode2MultiCharCoding(EMCCUtf8, pBlock->m_StrName, pBlock->m_StrName.GetLength(), strName, 512);
	
	memset(strHotkey, 0, sizeof(strHotkey));
	Unicode2MultiCharCoding(EMCCUtf8, pBlock->m_StrName, pBlock->m_StrHotKey.GetLength(), strHotkey, 512);
	
	memset(strColor, 0, sizeof(strColor));
	sprintf(strColor, "%lu", (unsigned long)pBlock->m_clrBlock);
	
	memset(strHead, 0, sizeof(strHead));
	sprintf(strHead, "%d", (int32)pBlock->m_eHeadType);

	memset(strServer, 0, sizeof(strServer));
	sprintf(strServer, "%s", pBlock->m_bServerBlock ? "1" : "0");
	
	//
	pBlock->m_pXmlElement->SetAttribute(KStrElementBlockAttriName, strName);
	pBlock->m_pXmlElement->SetAttribute(KStrElementBlockAttriHotKey, strHotkey);
	pBlock->m_pXmlElement->SetAttribute(KStrElementBlockAttriColor, strColor);
	pBlock->m_pXmlElement->SetAttribute(KStrElementBlockAttriHead, strHead);
	pBlock->m_pXmlElement->SetAttribute(KStrElementBlockAttriServer, strServer);

	//
	SaveXmlFile();

	// ֪ͨ�۲���,���ݸ�����:
	Notify(CSubjectUserBlock::EUBUBlock);
	
	//
	if ( pBlock->m_bChangeName )
	{
		pBlock->m_bChangeName = false;
	}

	return true;
}

bool32 CUserBlockManager::DelUserBlock(IN const CString& StrBlockName)
{
	T_Block* pBlock = GetBlock(StrBlockName);

	// ���û�ҵ�������, ֱ�ӷ���ʧ��
	if ( NULL == pBlock )
	{
		return false;
	}

	// ���� XML ����
	if ( NULL == m_pXmlDoc )
	{
		return false;
	}

	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	ASSERT( !pBlock->m_bServerBlock );	// ��������ذ�鲻��ɾ��
	
	pRootElement->RemoveChild(pBlock->m_pXmlElement);

	// ���³�Ա���ݱ���

	// ɾ����Ʒ
	for ( int32 j = 0; j < pBlock->m_aMerchs.GetSize(); j++ )
	{
		CMerch* pMerchToDel = pBlock->m_aMerchs[j];

		for ( int32 i = 0; i < m_aMerchsAll.GetSize(); i++ )
		{
			CMerch* pMerchExist = m_aMerchsAll[i];
			
			if ( pMerchExist == pMerchToDel )
			{
				m_aMerchsAll.RemoveAt(i);
				break;
			}
		}
	}

	// ɾ�����
	for ( int32 i = 0; i < m_aBlocksAll.GetSize(); i++ )
	{
		if ( StrBlockName == m_aBlocksAll[i].m_StrName )
		{
			m_aBlocksAll.RemoveAt(i);
			break;
		}
	}
	
	SaveXmlFile();

	// ֪ͨ�۲���,���ݸ�����:
	Notify(CSubjectUserBlock::EUBUBlock);

	return true;
}


bool32	CUserBlockManager::DelAllUserBlock(IN const CString& StrBlockName, bool32 bAutoSaveAndNotify)
{
	T_Block* pBlock = GetBlock(StrBlockName);

	// ���û�ҵ�������, ֱ�ӷ���ʧ��
	if ( NULL == pBlock )
	{
		return false;
	}

	// ���� XML ����
	if ( NULL == m_pXmlDoc )
	{
		return false;
	}

	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	//ɾ���ð�����нڵ�
	pBlock->m_pXmlElement->Clear();

	// ���³�Ա���ݱ���

	// ɾ����Ʒ
	for ( int32 j = 0; j < pBlock->m_aMerchs.GetSize(); j++ )
	{
		CMerch* pMerchToDel = pBlock->m_aMerchs[j];

		for ( int32 i = 0; i < m_aMerchsAll.GetSize(); i++ )
		{
			CMerch* pMerchExist = m_aMerchsAll[i];

			if ( pMerchExist == pMerchToDel )
			{
				m_aMerchsAll.RemoveAt(i);
				break;
			}
		}
	}

	// ɾ������µ�������Ʒ
	for ( int32 i = 0; i < m_aBlocksAll.GetSize(); i++ )
	{
		if ( StrBlockName == m_aBlocksAll[i].m_StrName )
		{
			m_aBlocksAll.GetAt(i).m_aMerchs.RemoveAll();
			break;
		}
	}

	if ( bAutoSaveAndNotify )
	{
		//
		SaveXmlFile();

		// ֪ͨ�۲���,���ݸ�����:
		Notify(CSubjectUserBlock::EUBUMerch);
	}

}

bool32 CUserBlockManager::ChangeOwnMerchPosition(bool32 bPre, IN const CString& StrBlockName, IN CMerch* pMerch)
{
	// // ȫ���ͷŵ�ʱ���ͷţ��ں���������429����
	//lint --e{429}

	// ��֤�Ϸ���
	if ( NULL == pMerch )
	{
		return false;
	}

	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	std::map<CMerch*, TiXmlElement*>::iterator itFind = pBlock->m_aMapMerchToXml.find(pMerch);	
	
	TiXmlElement* pElementBlock = pBlock->m_pXmlElement;
	TiXmlElement* pElementMerch = itFind->second;

	if ( NULL == pElementMerch  || pElementMerch->Parent() != pElementBlock )
	{
		ASSERT(0);
		return false;
	}

	// �ƶ�λ��:
	if ( 1 == pBlock->m_aMerchs.GetSize() )
	{
		// ֻ��һ����Ʒ,�����ƶ�ûʲô�仯
		return true;
	}

	TiXmlNode* pNodeToInsert = NULL;
	if ( bPre )
	{
		// ǰ��һλ
		pNodeToInsert = pElementMerch->PreviousSibling();

		if( NULL == pNodeToInsert )
		{
			ASSERT(0);
			return false;
		}
		
		//
		const char* strMerchCode = _W2A(pMerch->m_MerchInfo.m_StrMerchCode);
		
		char  strMarket[256];
		sprintf(strMarket, "%d", pMerch->m_MerchInfo.m_iMarketId);

		
		TiXmlElement* pNodeInsert = new TiXmlElement(KStrElementMerchName);
		pNodeInsert->SetAttribute(KStrElementMerchAttriCode,   strMerchCode);
		pNodeInsert->SetAttribute(KStrElementMerchAttriMarket, strMarket);
		
		//	
		TiXmlElement* pNodeInsert2 = pElementMerch->Parent()->InsertBeforeChild(pNodeToInsert, *pNodeInsert)->ToElement();
		pElementMerch->Parent()->RemoveChild(pElementMerch);
				
		// ����map 
		pBlock->m_aMapMerchToXml[pMerch] = pNodeInsert2;
	}
	else
	{
		// ����һλ
		pNodeToInsert = pElementMerch->NextSibling();

		if( NULL == pNodeToInsert )
		{
			ASSERT(0);
			return false;
		}
		
		//
		const char* strMerchCode = _W2A(pMerch->m_MerchInfo.m_StrMerchCode);
		
		char  strMarket[256];
		sprintf(strMarket, "%d", pMerch->m_MerchInfo.m_iMarketId);

		TiXmlElement* pNodeInsert = new TiXmlElement(KStrElementMerchName);
		pNodeInsert->SetAttribute(KStrElementMerchAttriCode,   strMerchCode);
		pNodeInsert->SetAttribute(KStrElementMerchAttriMarket, strMarket);
		
		//	
		TiXmlElement* pNodeInsert2 = pElementMerch->Parent()->InsertAfterChild(pNodeToInsert, *pNodeInsert)->ToElement();;
		pElementMerch->Parent()->RemoveChild(pElementMerch);

		// ����map 
		pBlock->m_aMapMerchToXml[pMerch] = pNodeInsert2;
	}

	//
	SaveXmlFile();
	LoadXmlFile(m_pMerchManager);

	//
	Notify(CSubjectUserBlock::EUBUMerch);

	return true;
}

bool32 CUserBlockManager::ChangeOwnMerchPosition(IN const CString& StrBlockName, bool32 bPre, IN CMerch* pPreMerch,  IN CMerch* pNextMerch)
{
	// // ȫ���ͷŵ�ʱ���ͷţ��ں���������429����
	//lint --e{429}

	// ��֤�Ϸ���
	if ( (NULL==pPreMerch) || (NULL==pNextMerch) )
	{
		return false;
	}

	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	// �ƶ�λ��:
	if ( pBlock->m_aMerchs.GetSize() <= 1)
	{
		// ���ڵ���һ����Ʒ,�ƶ�ûʲô�仯
		return true;
	}

	std::map<CMerch*, TiXmlElement*>::iterator itFindPre = pBlock->m_aMapMerchToXml.find(pPreMerch);	
	std::map<CMerch*, TiXmlElement*>::iterator itFindNext = pBlock->m_aMapMerchToXml.find(pNextMerch);

	ASSERT(itFindPre != pBlock->m_aMapMerchToXml.end() && itFindNext != pBlock->m_aMapMerchToXml.end());
	if (!(itFindPre != pBlock->m_aMapMerchToXml.end() && itFindNext != pBlock->m_aMapMerchToXml.end()))
	{
		return false;
	}
	TiXmlElement* pElementBlock = pBlock->m_pXmlElement;
	TiXmlElement* pElementMerchPre = itFindPre->second;
	TiXmlElement* pElementMerchNext = itFindNext->second;

	if ( NULL == pElementMerchPre || pElementMerchPre->Parent() != pElementBlock )
	{
		ASSERT(0);
		return false;
	}

	if ( NULL == pElementMerchNext || pElementMerchNext->Parent() != pElementBlock )
	{
		ASSERT(0);
		return false;
	}

	{
		const char* strPreMerchCode = _W2A(pPreMerch->m_MerchInfo.m_StrMerchCode);
		char  strPreMarket[256];
		sprintf(strPreMarket, "%d", pPreMerch->m_MerchInfo.m_iMarketId);

		TiXmlElement* pPreNode = new TiXmlElement(KStrElementMerchName);
		pPreNode->SetAttribute(KStrElementMerchAttriCode,   strPreMerchCode);
		pPreNode->SetAttribute(KStrElementMerchAttriMarket, strPreMarket);

		TiXmlElement* pNextNode = pElementMerchNext;

		TiXmlElement* pNodeInsert = NULL;
		if (bPre)
		{
			pNodeInsert = pElementMerchPre->Parent()->InsertBeforeChild(pNextNode, *pPreNode)->ToElement();
		}
		else
		{
			pNodeInsert = pElementMerchPre->Parent()->InsertAfterChild(pNextNode, *pPreNode)->ToElement();
		}
		
		pElementMerchPre->Parent()->RemoveChild(pElementMerchPre);

		// ����map 
		pBlock->m_aMapMerchToXml[pPreMerch] = pNodeInsert;
	}

	//
	SaveXmlFile();
	LoadXmlFile(m_pMerchManager);

	//
	Notify(CSubjectUserBlock::EUBUMerch);

	return true;
}

CString CUserBlockManager::GetServerBlockName()
{
	T_Block *pBlock = GetServerBlock();
	if ( NULL != pBlock )
	{
		return pBlock->m_StrName;
	}
	return _T("");
}

bool32 CUserBlockManager::IsServerBlock( const T_Block &Block )
{
	return Block.m_bServerBlock;
}

T_Block		* CUserBlockManager::GetServerBlock()
{
	int32 i;
	for ( i=0; i < m_aBlocksAll.GetSize() ; i++ )
	{
		if ( IsServerBlock(m_aBlocksAll[i]) )
		{
			return &m_aBlocksAll[i];
		}
	}
	// ���û���趨��־�Ļ�����Ѱ�Ƿ��е���Ĭ�����ֵ���ѡ��
	CString StrDefaultName = GetDefaultServerBlockName();
	for ( i=0; i < m_aBlocksAll.GetSize() ; i++ )
	{
		if ( m_aBlocksAll[i].m_StrName == StrDefaultName )
		{
			return &m_aBlocksAll[i];
		}
	}
	//ASSERT( 0 );	// ʵ��û�оͱ�Ǹ��
	return NULL;
}

CString CUserBlockManager::GetDefaultServerBlockName()
{
	return _T("�ҵ���ѡ");
}

void CUserBlockManager::BuildUserBlockHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys)
{
	//---wangyongxue 
	RemoveHotKey(EHKTBlock, arrHotKeys);

	for ( int32 i = 0; i < m_aBlocksAll.GetSize(); i++ )
	{
		T_Block stBlock = m_aBlocksAll.GetAt(i);
		
		CHotKey HotKey;
		
		HotKey.m_eHotKeyType	= EHKTBlock;
		HotKey.m_StrKey			= stBlock.m_StrHotKey;
		HotKey.m_StrSummary		= stBlock.m_StrName;
		HotKey.m_StrParam1		= stBlock.m_StrName;
		HotKey.m_iParam1		= 1;
		
		arrHotKeys.Add(HotKey);
	}
}

void CUserBlockManager::RemoveHotKey(E_HotKeyType eHotKeyType, CArray<CHotKey, CHotKey&>&	arrHotKeys)
{
	for (int32 i = arrHotKeys.GetSize() - 1; i >= 0; i--)
	{
		if (arrHotKeys[i].m_eHotKeyType == eHotKeyType)
			arrHotKeys.RemoveAt(i, 1);
	}
}

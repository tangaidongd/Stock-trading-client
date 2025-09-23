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
// 主题


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
// 观察者
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

	///> 释放资源
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
		TRACE(_T("用户板块文件被重载！\r\n"));
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

	// 清空先
	m_aBlocksAll.RemoveAll();
	m_aMerchsAll.RemoveAll();

	// 根节点
	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	// 板块节点
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

		// 名称
		TCHAR TStrName[1024];
		memset(TStrName, 0, sizeof(TStrName));
		MultiCharCoding2Unicode(EMCCUtf8, pStrName, strlen(pStrName), TStrName, sizeof(TStrName) / sizeof(TCHAR));

		T_Block	stBlockToAdd;
		stBlockToAdd.m_StrName = (CString)TStrName;

		// 快捷键
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

		// 颜色
		if ( NULL != pStrColor )
		{
			stBlockToAdd.m_clrBlock = atoi(pStrColor);
		}

		// 表头
		if ( NULL != pStrHead )
		{
			stBlockToAdd.m_eHeadType = (E_ReportType)atoi(pStrHead);
		}

		// server
		if ( NULL != pStrServer )
		{
			stBlockToAdd.m_bServerBlock = atoi(pStrServer) != 0;
		}

		// XML 节点
		stBlockToAdd.m_pXmlElement = pBlockElement;

		// 这个板块下的商品
		TiXmlElement* pMerchElement = pBlockElement->FirstChildElement();
		CArray<TiXmlElement*, TiXmlElement*> aDelNodes;
		while ( pMerchElement )
		{
			// 代码
			const char* pStrMerchCode	= pMerchElement->Attribute(KStrElementMerchAttriCode);
			if ( NULL == pStrMerchCode )
			{
				pMerchElement = pMerchElement->FirstChildElement();
				continue;
			}

			// 市场号
			const char* pStrMarketId	= pMerchElement->Attribute(KStrElementMerchAttriMarket);
			if ( NULL == pStrMarketId )
			{
				pMerchElement = pMerchElement->FirstChildElement();
				continue;
			}
			int32 iMarketId = atoi(pStrMarketId);

			// 别名
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

					// 加到这个板块的商品列表中:
					stBlockToAdd.m_aMerchs.Add(pMerch);

					// Map 保存
					stBlockToAdd.m_aMapMerchToXml[pMerch] = pMerchElement;

					// 加到整个的商品列表中:
					m_aMerchsAll.Add(pMerch);
				}
				else
				{
					aDelNodes.Add(pMerchElement);
				}
			}
			
			pMerchElement = pMerchElement->NextSiblingElement();
		}

		// 加入板块中
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
	// 一个商品可能属于多个自选股, 只返回找到第一个匹配的板块
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
// 系统板块

CSysBlockManager* CSysBlockManager::Instance()
{
	if ( NULL == m_pThis )
	{
		// 第一次:
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
		//ASSERT(0);  // 已经移除其xml
		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 用户板块

CUserBlockManager* CUserBlockManager::Instance()
{
	if ( NULL == m_pThis )
	{
		// 第一次:
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
	// 如果用户板块文件不存在，且公共目录下有合法的公共用户板块文件，则copy过来，否则按原来处理
	if ( _taccess(StrPath, 0) < 0 )	// 不存在私有
	{
		// 检查public下是否合法
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
						// 有block性质，就认为是合法的了吧 copy文件
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
		//CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='workspace'>\n<block name='自选股1' hotkey='zxg1' color='16776960' head='12' >\n</block>\n</XMLDATA>");
		CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='workspace'>\n<block name='我的自选' hotkey='wdzx' color='16776960' head='12' server='1'>\n</block>\n</XMLDATA>");	// 与手机名称相同, 默认的第一个板块是server同步的
		
		if ( NULL == m_pXmlDoc )
		{
			m_pXmlDoc = new TiXmlDocument(StrPathA.c_str());
		}
		
		ASSERT(NULL != m_pXmlDoc);
		
		//
		int32 iOutBufferSize = StrDefaultXml.GetLength();
		iOutBufferSize *= 2;
		iOutBufferSize += 100;		// 防止StrContent长度为0
		char *pcOutBuffer = new char[iOutBufferSize];
		
		if (NULL != pcOutBuffer)
		{
			// 先转换写入文件的内容
			memset(pcOutBuffer, 0, iOutBufferSize);
			Unicode2MultiCharCoding(EMCCUtf8, StrDefaultXml, StrDefaultXml.GetLength(), pcOutBuffer, iOutBufferSize);
			
			// 转换文件名
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
			
			// 不要忘记释放
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

	// 0: 判断是否重复添加:
	if ( BeMerchInBlock(pMerch, StrBlockName) )
	{
		return false;
	}

	// 更新这个板块的内容:

	// 1: XML 内容更新
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

	// 2: 数据成员变量更新
	pBlock->m_aMapMerchToXml[pMerch] = pMerchElement;
	pBlock->m_aMerchs.Add(pMerch);

	// 更新整个文件中的内容
	m_aMerchsAll.Add(pMerch);

	if ( bAutoSaveAndNotify )
	{
		//
		SaveXmlFile();
	
		// 通知观察者,数据更新了:
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
	
	// 更新这个板块的内容:
	
	// 1: XML 内容更新
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
	
	// 2: 数据成员变量更新
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
 
	// 更新整个文件内容
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
		
		// 通知观察者,数据更新了:
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
		// 全部替换				
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

			// 增加商品
			pBlock->m_aMerchs.Add(pMerch);
			
			// 增加 XML 节点
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

			// 保存 map
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
			
			// 直接往里添加就行了, 函数内部会判断是否重复
			AddMerchToUserBlock(pMerch, StrBlockName, false);
		}
	}
	
	// 保存文件
	SaveXmlFile();
	
	// 通知
	Notify(CSubjectUserBlock::EUBUMerch);

	return true;
}

bool32 CUserBlockManager::AddUserBlock(IN T_Block& BlockInfoNew)
{
	if ( BlockInfoNew.m_StrName.GetLength() <= 0 )
	{
		return false;
	}

	// 更新XML 内容
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

	// 更新数据成员的内容
	BlockInfoNew.m_pXmlElement = pBlockElement;
	m_aBlocksAll.Add(BlockInfoNew);

	//
	SaveXmlFile();

	// 通知观察者,数据更新了:
	Notify(CSubjectUserBlock::EUBUBlock);

	return true;
}

bool32 CUserBlockManager::ModifyUserBlock(IN const CString& StrBlockNameOld, IN const T_Block& BlockInfoNew)
{
	// 修改只有修改名称,快捷键,颜色,表头 这几个字段
	T_Block* pBlock = GetBlock(StrBlockNameOld);
	
	if ( NULL == pBlock )
	{
		return false;
	}


	// 修改成员变量内容
	if ( pBlock->m_StrName != BlockInfoNew.m_StrName )
	{
		pBlock->m_bChangeName	= true;
		pBlock->m_StrNameOld	= pBlock->m_StrName;
	}

	pBlock->m_StrName		= BlockInfoNew.m_StrName;
	pBlock->m_StrHotKey		= BlockInfoNew.m_StrHotKey;
	pBlock->m_clrBlock		= BlockInfoNew.m_clrBlock;
	pBlock->m_eHeadType		= BlockInfoNew.m_eHeadType;
	//pBlock->m_bServerBlock 属性不变

	// 修改 XML 内容:
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

	// 通知观察者,数据更新了:
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

	// 如果没找到这个板块, 直接返回失败
	if ( NULL == pBlock )
	{
		return false;
	}

	// 更新 XML 内容
	if ( NULL == m_pXmlDoc )
	{
		return false;
	}

	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	ASSERT( !pBlock->m_bServerBlock );	// 服务器相关板块不能删除
	
	pRootElement->RemoveChild(pBlock->m_pXmlElement);

	// 更新成员数据变量

	// 删除商品
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

	// 删除板块
	for ( int32 i = 0; i < m_aBlocksAll.GetSize(); i++ )
	{
		if ( StrBlockName == m_aBlocksAll[i].m_StrName )
		{
			m_aBlocksAll.RemoveAt(i);
			break;
		}
	}
	
	SaveXmlFile();

	// 通知观察者,数据更新了:
	Notify(CSubjectUserBlock::EUBUBlock);

	return true;
}


bool32	CUserBlockManager::DelAllUserBlock(IN const CString& StrBlockName, bool32 bAutoSaveAndNotify)
{
	T_Block* pBlock = GetBlock(StrBlockName);

	// 如果没找到这个板块, 直接返回失败
	if ( NULL == pBlock )
	{
		return false;
	}

	// 更新 XML 内容
	if ( NULL == m_pXmlDoc )
	{
		return false;
	}

	TiXmlElement* pRootElement = m_pXmlDoc->RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	//删除该板块所有节点
	pBlock->m_pXmlElement->Clear();

	// 更新成员数据变量

	// 删除商品
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

	// 删除板块下的所有商品
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

		// 通知观察者,数据更新了:
		Notify(CSubjectUserBlock::EUBUMerch);
	}

}

bool32 CUserBlockManager::ChangeOwnMerchPosition(bool32 bPre, IN const CString& StrBlockName, IN CMerch* pMerch)
{
	// // 全局释放的时候释放，在函数内屏蔽429错误
	//lint --e{429}

	// 验证合法性
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

	// 移动位置:
	if ( 1 == pBlock->m_aMerchs.GetSize() )
	{
		// 只有一个商品,上下移动没什么变化
		return true;
	}

	TiXmlNode* pNodeToInsert = NULL;
	if ( bPre )
	{
		// 前移一位
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
				
		// 更新map 
		pBlock->m_aMapMerchToXml[pMerch] = pNodeInsert2;
	}
	else
	{
		// 后移一位
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

		// 更新map 
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
	// // 全局释放的时候释放，在函数内屏蔽429错误
	//lint --e{429}

	// 验证合法性
	if ( (NULL==pPreMerch) || (NULL==pNextMerch) )
	{
		return false;
	}

	T_Block* pBlock = GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return false;
	}

	// 移动位置:
	if ( pBlock->m_aMerchs.GetSize() <= 1)
	{
		// 少于等于一个商品,移动没什么变化
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

		// 更新map 
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
	// 如果没有设定标志的话，找寻是否有等于默认名字的自选股
	CString StrDefaultName = GetDefaultServerBlockName();
	for ( i=0; i < m_aBlocksAll.GetSize() ; i++ )
	{
		if ( m_aBlocksAll[i].m_StrName == StrDefaultName )
		{
			return &m_aBlocksAll[i];
		}
	}
	//ASSERT( 0 );	// 实在没有就抱歉了
	return NULL;
}

CString CUserBlockManager::GetDefaultServerBlockName()
{
	return _T("我的自选");
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

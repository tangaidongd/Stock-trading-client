#pragma warning(disable:4786)
#include "StdAfx.h"

#include "BlockConfig.h"
#include "MerchManager.h"
#include "viewdata.h"
#include "hotkey.h"
#include "CCodeFile.h"
#include <string.h>



// 123
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int32 KIInvalidId	 = -2; // ���а��������ؽṹ��Ҫid�ı�ʾ��Чid��ֵ
static const int32 KIDefaultMarketBlockId = -1;		// ����a�ɼٶ�id


static const  char KXmlStrRootKey[]					=		"XMLDATA";
static const  char KXmlStrRootVersionAttri[]			=		"version";
static const  char KXmlStrRootDataAttri[]			=		"data";
static const  char KXmlStrRootDataAttriValue[]		=		"sys_b0s";		// ��Ϊ��xml��ʶ���־


static const char KXmlStrIdAttri[]		=	"id";
static const char KXmlStrOrderAttri[]	=	"s";


static const char KXmlStrCollectionKey[]		=	"b0";
static const char KXmlStrCollectionInfoNameAttri[]	=	"name";
static const char KXmlStrBlockKey[]			=	"b1";
static const char KXmlStrBlockInfoNameAttri[]		=	"n";
static const char KXmlStrBlockTypeAttri[]			=	"t";

static const char KXmlStrMerchKey[]				= "m";
static const char KXmlStrMerchCodeAttri[]		= "c";
static const char KXmlStrMerchMarketAttri[]		= "m";

#define INVALID_VALUE (-1)
//////////////////////////////////////////////////////////////////////////
CBlockConfig *CBlockConfig::m_spThis = NULL; 
bool32		  CBlockConfig::m_bWaitForServerResp = false;
const	int32			CBlockConfig::KIUserBlockCollectionId = -200;
const	int32			CBlockConfig::KIUserBlockCollectionOrderId = INT_MAX;	// Ĭ����ѡ��λ��
const	int32			CBlockConfig::KIMaxUserBlockId = -2000;
const	int32			CBlockConfig::KIMinUserBlockId = -4000;
		int32			CBlockConfig::KIMaxSpecialSHSZAMarketBlockId = -5000;
const	int32			CBlockConfig::KISpecialSHSZAMarketBlockCollectionId = -201; // ����a�����������г�

const CString			CBlockConfig::KStrUserBlockCollection = _T("�Զ���");

CBlockConfig::CBlockConfig()
{

	m_pViewData = NULL;
	m_bInitialized  = false;

	m_iNewUserBlockId = KIMaxUserBlockId;
	
	m_collectionSHSZA.m_iBlockCollectionId = KISpecialSHSZAMarketBlockCollectionId;
	m_collectionSHSZA.m_StrName = _T("����A�����Ӱ�鼯��");	// �ð�鼯��Ӧ������
}

CBlockConfig::~CBlockConfig()
{
	RemoveAllBlockCollections();	
	if ( m_spThis == this )
	{
		m_spThis = NULL;
	}
}


CBlockConfig * CBlockConfig::Instance()
{
	if ( NULL == m_spThis )
	{
		m_spThis = new CBlockConfig();
	}
	return m_spThis;
}

void CBlockConfig::DeleteInstance()
{
	DEL(m_spThis);
	ASSERT( m_spThis == NULL );
}

bool32 CBlockConfig::Initialize( const CString &StrXmlFileName )
{
	DWORD dwTime = timeGetTime();

	FireConfigListener(CBlockConfigListener::BeforeInitialize);	// ֪ͨ��ʼ

	RemoveAllBlockCollections();		// �����������

	TiXmlDocument	tiDoc;
	char szFile[1024];
	::WideCharToMultiByte(CP_OEMCP, 0, StrXmlFileName, -1, szFile, sizeof(szFile), NULL, NULL);
	m_bInitialized = tiDoc.LoadFile(szFile, TIXML_ENCODING_LEGACY);		// gb2312�ı��룬��ǿ�Ʊ����ʽ��ֻ��֤�ָ��ַ���ȷ�Ϳ�����
	if ( m_bInitialized )
	{
		m_bInitialized = LoadFromXml(tiDoc);
	}
	_MYTRACE(_T("��xml�ļ�����: %d"), m_bInitialized);

	m_bInitialized = LoadFromUserBlock() && m_bInitialized;		// ����Ҫ���ر��ص��Զ���
	//m_bInitialized = m_bInitialized && LoadSelectBlock();
	ASSERT( m_bInitialized );

//	BuildHotKeyList();

	FireConfigListener(CBlockConfigListener::Initialized);		// ֪ͨ����

	TRACE(_T("����б��ʼ��: %dms\r\n"), timeGetTime()-dwTime);

	return m_bInitialized;
}

bool32 CBlockConfig::IsInitialized() const
{
	return m_bInitialized;
}

bool32 CBlockConfig::GetConfigFullPathName( OUT CString &StrPath )
{
	StrPath.Empty();

	// ����ȡд������·��
	CString StrPublicPath = CPathFactory::GetPublicConfigPath();
	StrPath = StrPublicPath + _T("BlockConfig.xml");
	return true;
}

bool32 CBlockConfig::LoadFromXml( TiXmlDocument &tiDoc )
{

	if ( NULL == m_pViewData )
	{
		ASSERT( 0 );
		return false;
	}
	TiXmlElement *pTiEle = tiDoc.FirstChildElement(KXmlStrRootKey);
	if ( NULL == pTiEle )
	{
		ASSERT( 0 );
		_MYTRACE(_T("����б�: �޷��������ڵ�"));
		return false;
	}
	// ��֤data��־, version�� - TODO
	CString StrVersion, StrDataFlag;
	
	const char *pAttriValue = pTiEle->Attribute(KXmlStrRootVersionAttri);
	GB2312ToUnicode(pAttriValue, StrVersion);
	ASSERT( StrVersion.GetLength() > 0 );
	pAttriValue = pTiEle->Attribute(KXmlStrRootDataAttri);
	ASSERT( pAttriValue != NULL && _stricmp(pAttriValue, KXmlStrRootDataAttriValue) == 0 );
	GB2312ToUnicode(pAttriValue, StrDataFlag);
	
	// ����������� - ��Щ�����Ǽٶ�����insertʱ�жϳ����ģ��Ƿ���Ҫ��¼����
	TiXmlElement *pTiEleChild = pTiEle->FirstChildElement(KXmlStrCollectionKey);
	while (NULL != pTiEleChild)
	{
		CBlockCollection	collection;

		//DWORD dwTime = timeGetTime();
		
		pAttriValue = pTiEleChild->Attribute(KXmlStrCollectionInfoNameAttri);
		GB2312ToUnicode(pAttriValue, collection.m_StrName);
		pAttriValue = pTiEleChild->Attribute(KXmlStrIdAttri);	// ����id
		if ( NULL != pAttriValue )
		{
			collection.m_iBlockCollectionId = atoi(pAttriValue);
		}
		pAttriValue = pTiEleChild->Attribute(KXmlStrOrderAttri);	// ˳���
		if ( NULL != pAttriValue )
		{
			collection.m_iOrderId = atoi(pAttriValue);
		}

		CBlockCollection *pNewCollection = InsertBlockCollection(collection);
		ASSERT( pNewCollection != NULL );
		if ( pNewCollection != NULL )
		{
			ASSERT( pNewCollection->m_iBlockCollectionId >= 0 && pNewCollection->m_iOrderId >= 0 );

			TiXmlElement *pTiEleBlock = pTiEleChild->FirstChildElement(KXmlStrBlockKey);
			while ( NULL != pTiEleBlock )
			{
				CBlockInfo	blockInfo;
				pAttriValue = pTiEleBlock->Attribute(KXmlStrBlockInfoNameAttri);
				GB2312ToUnicode(pAttriValue, blockInfo.m_StrBlockName);
				pAttriValue = pTiEleBlock->Attribute(KXmlStrIdAttri);
				if ( NULL != pAttriValue )
				{
					blockInfo.m_iBlockId = atoi(pAttriValue);
				}
				pAttriValue = pTiEleBlock->Attribute(KXmlStrOrderAttri);
				if ( NULL != pAttriValue )
				{
					blockInfo.m_iOrderId = atoi(pAttriValue);
				}
				// ����������� - ���� 1Ϊ���� 0Ϊ��ͨ
				pAttriValue = pTiEleBlock->Attribute(KXmlStrBlockTypeAttri);
				if ( NULL != pAttriValue )
				{
					int32 iType = atoi(pAttriValue);
					if ( iType == 1 )
					{
						blockInfo.m_iType = CBlockInfo::typeMarketClassBlock;
					}
					else if ( iType == 0 )
					{
						blockInfo.m_iType = CBlockInfo::typeNormalBlock;
					}
					else if ( iType == 2 )
					{
						blockInfo.m_iType = CBlockInfo::typeFutureClassBlock;
					}
					else
					{
						blockInfo.m_iType = iType;	// ���Ǹ���Ҫ���ֵ
						ASSERT( 0 );		// �������岻֧��
					}
				}
				CString StrMerchCode;
				CMerch *pMerch = NULL;
				int32   iMarketId = -1;

				pAttriValue = pTiEleBlock->Attribute(KXmlStrMerchCodeAttri);		// ���������봦��
				if ( NULL != pAttriValue )
				{
					GB2312ToUnicode(pAttriValue, StrMerchCode);
				}
				pAttriValue = pTiEleBlock->Attribute(KXmlStrMerchMarketAttri);
				if ( NULL != pAttriValue )
				{
					iMarketId = atoi(pAttriValue);
					// ���봦��
					if ( StrMerchCode.Find(_T('*'), 0) == -1 )
					{
						if ( m_pViewData->m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch) )	// û�е���Ʒ������
						{
							blockInfo.m_pMerch = pMerch;
						}
						else
						{
							TRACE(_T("BlockConfig Ignore Merch: %d-%s\r\n"), iMarketId, StrMerchCode);
						}
					}
					else
					{
						//// ��������
						//bHasMaskMerch = true;		// ����Ҫ������������Ʒ�ܷ񹲴��ж�
						//MerchArray aMerchsMask;
						//FillMerchByMaskString(StrMerchCode, iMarketId, aMerchsMask);
						//pNewBlock->m_blockInfo.Append(aMerchsMask);

						// �趨�������Ӱ��
						// bool32 b = LoadSepecialSHSZAMarket(*pNewBlock, iMarketId);
						// //ASSERT( b );
						// if ( !b )
						// {
						// 	TRACE(_T("BlockConfig Ignore MarketBlock: %d\r\n"), iMarketId);
						// }
					}
				}
				// ������ݼ�
				blockInfo.m_StrBlockShortCut = ConvertHZToPY(blockInfo.m_StrBlockName);

				CBlockLikeMarket *pNewBlock = InsertBlockIntoCollection(pNewCollection, blockInfo, true);
				ASSERT( NULL != pNewBlock );
				if ( NULL != pNewBlock )
				{
					//DWORD dwTime = timeGetTime();

					bool32 bHasMaskMerch = false;
					TiXmlElement *pTiEleMerch = pTiEleBlock->FirstChildElement(KXmlStrMerchKey);
					while ( NULL != pTiEleMerch )
					{
						CString StrMerchCode;
						CMerch *pMerch = NULL;
						int32   iMarketId = -1;

						pAttriValue = pTiEleMerch->Attribute(KXmlStrMerchCodeAttri);		// ���������봦��
						if ( NULL != pAttriValue )
						{
							GB2312ToUnicode(pAttriValue, StrMerchCode);
						}
						pAttriValue = pTiEleMerch->Attribute(KXmlStrMerchMarketAttri);
						if ( NULL != pAttriValue )
						{
							iMarketId = atoi(pAttriValue);
							// ���봦��
							if ( StrMerchCode.Find(_T('*'), 0) == -1 )
							{
								if ( m_pViewData->m_MerchManager.FindMerch(StrMerchCode, iMarketId, pMerch) )	// û�е���Ʒ������
								{
									pNewBlock->m_blockInfo.Add(pMerch);
								}
								else
								{
									TRACE(_T("BlockConfig Ignore Merch: %d-%s\r\n"), iMarketId, StrMerchCode.GetBuffer());
									StrMerchCode.ReleaseBuffer();
								}
							}
							else
							{
								// ��������
								bHasMaskMerch = true;		// ����Ҫ������������Ʒ�ܷ񹲴��ж�
								MerchArray aMerchsMask;
								FillMerchByMaskString(StrMerchCode, iMarketId, aMerchsMask);
								pNewBlock->m_blockInfo.Append(aMerchsMask);

								// �趨�������Ӱ��
// 								bool32 b = LoadSepecialSHSZAMarket(*pNewBlock, iMarketId);
// 								//ASSERT( b );
// 								if ( !b )
// 								{
// 									TRACE(_T("BlockConfig Ignore MarketBlock: %d\r\n"), iMarketId);
// 								}
							}
						}
						//ASSERT( pMerch != NULL );

						pTiEleMerch = pTiEleMerch->NextSiblingElement(KXmlStrMerchKey);
					}

					//TRACE(_T("    ������ %s %dms\r\n"), blockInfo.m_StrBlockName, timeGetTime()-dwTime);
					if ( pNewBlock->m_blockInfo.m_aSubMerchs.GetSize() == 0 
						&& pNewBlock->m_blockInfo.m_iType != CBlockInfo::typeNormalBlock
						&& pNewBlock->m_blockInfo.m_iType != CBlockInfo::typeMarketClassBlock )
					{
						// û����Ʒ�Ŀհ�飬ɾ����
						m_mapBlocks.erase(pNewBlock->m_blockInfo.m_iBlockId);
						for ( int32 i=0; i < pNewCollection->m_aBlocks.GetSize() ; ++i )
						{
							if ( pNewCollection->m_aBlocks[i] == pNewBlock )
							{
								pNewCollection->m_aBlocks.RemoveAt(i);
								delete pNewBlock;
								pNewBlock = NULL;
								TRACE(_T("BlockConfig del Block: %d\r\n"), blockInfo.m_iBlockId);
								break;
							}
						}
					}
				}

				pTiEleBlock = pTiEleBlock->NextSiblingElement(KXmlStrBlockKey);
			}
		}

		if ( NULL != pNewCollection && pNewCollection->m_aBlocks.GetSize() == 0 )
		{
			// �ռ���ɾ��
			for ( int32 i=0; i < m_aBlockCollections.GetSize() ; ++i )
			{
				if ( m_aBlockCollections[i] == pNewCollection )
				{
					m_aBlockCollections.RemoveAt(i);
					delete pNewCollection;
					pNewCollection = NULL;
					_MYTRACE(_T("ɾ��col: %s \n"), collection.m_StrName.GetBuffer());
					collection.m_StrName.ReleaseBuffer();
					break;
				}
			}
		}
		else
		{
			//_MYTRACE(_T("����col: %s %d ms\r\n"), collection.m_StrName, timeGetTime()-dwTime);
			_MYTRACE(_T("����col: %s \n"), collection.m_StrName.GetBuffer());
			collection.m_StrName.ReleaseBuffer();
		}
		pTiEleChild = pTiEleChild->NextSiblingElement(KXmlStrCollectionKey);
	}

	m_StrXmlVersion = StrVersion;

	// ��������A�ɵĿ������� - ������findblock�п��ٲ���
	CBlockLikeMarket *pBlockHSA = GetDefaultMarketClassBlock();
	if ( pBlockHSA != NULL )
	{
  		m_mapBlocks[ GetDefaultMarketlClassBlockPseudoId() ] = pBlockHSA;
	}

	return true;
}

bool32 CBlockConfig::LoadFromUserBlock()
{
	CBlockCollection collection;
	collection.m_iBlockCollectionId = KIUserBlockCollectionId;
	collection.m_iOrderId = KIUserBlockCollectionOrderId;	// ��ѡ��
	collection.m_StrName = KStrUserBlockCollection;
	CBlockCollection *pNewCollection = InsertBlockCollection(collection);
	ASSERT( NULL != pNewCollection );
	if ( NULL != pNewCollection )
	{
		CArray<T_Block, T_Block &> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);
		int nBlocksSize = aBlocks.GetSize();
		const int32 iMinBlockSize = min(nBlocksSize, KIMaxUserBlockId-KIMinUserBlockId +1);
		ASSERT( iMinBlockSize == aBlocks.GetSize() );		// ������֤����ض��û���ѡ��
		for ( int i=0; i < iMinBlockSize ; i++ )
		{
			CBlockInfo blockInfo;
			blockInfo.m_iOrderId = i;
			blockInfo.m_iBlockId = m_iNewUserBlockId;		// ʹ�����µ��û����id
			blockInfo.m_iType	 = CBlockInfo::typeUserBlock;		// �û��������
			m_iNewUserBlockId--;
			blockInfo.m_StrBlockName = aBlocks[i].m_StrName;
			blockInfo.m_StrBlockShortCut = aBlocks[i].m_StrHotKey;
			CBlockLikeMarket *pNewBlock = InsertBlockIntoCollection(pNewCollection, blockInfo);
			ASSERT( pNewBlock != NULL );
			if ( NULL != pNewBlock )
			{
				// ��������Ʒ����Ҫ���˵���ͼҪע�����
				pNewBlock->m_blockInfo.Copy(aBlocks[i].m_aMerchs);
			}
		}
	}
	return pNewCollection != NULL;
}

void CBlockConfig::GB2312ToUnicode( const char *pcsGB2312, OUT CString &StrWide )
{
	if ( pcsGB2312 == NULL )
	{
		StrWide.Empty();
		return;
	}

	int iCodePage = 936;
	int iNeedLen = ::MultiByteToWideChar(iCodePage, 0, pcsGB2312, -1, NULL, 0); // gb2312
	ASSERT( iNeedLen > 0 );
	int iRealLen = ::MultiByteToWideChar(iCodePage, 0, pcsGB2312, -1, StrWide.GetBuffer(iNeedLen), iNeedLen);
	StrWide.ReleaseBuffer();
	ASSERT( iRealLen == iNeedLen && StrWide.GetLength() == iNeedLen-1 );
}

CBlockCollection	* CBlockConfig::InsertBlockCollection( int32 iCollectionId, int32 iOrderId, const CString &StrCollectionName )
{
	ASSERT( iCollectionId >=0 || iCollectionId == KIUserBlockCollectionId );	// ������� �ļ��еĴ���=0�� �Զ�����ǹ̶�ֵ
	ASSERT( !StrCollectionName.IsEmpty() );
	if ( StrCollectionName.IsEmpty() )
	{
		return NULL;
	}

	// OrderId�����
	
	// ��������Ƿ��ظ�
	int i = 0;
	for ( i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->m_iBlockCollectionId == iCollectionId )		// �������ظ�
		{
			ASSERT( 0 );
			return NULL;
		}
		if ( m_aBlockCollections[i]->m_StrName == StrCollectionName )
		{
			ASSERT( 0 );		// �����棬����������
		}
	}

	CBlockCollection *pNewCollection = new CBlockCollection();	// ������ڴ棺)
	pNewCollection->m_iBlockCollectionId = iCollectionId;
	pNewCollection->m_iOrderId = iOrderId;
	pNewCollection->m_StrName = StrCollectionName;
	bool32 bInserted = false;
	for ( i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->m_iOrderId > iOrderId )	// ��С˳��id����id���У����ں���
		{
			m_aBlockCollections.InsertAt(i, pNewCollection);
			bInserted = true;
			break;
		}
	}
	if ( !bInserted )
	{
		m_aBlockCollections.Add(pNewCollection);
	}

	return pNewCollection;
}

CBlockCollection	* CBlockConfig::InsertBlockCollection( const CBlockCollection &collection )
{
	return InsertBlockCollection(collection.m_iBlockCollectionId, collection.m_iOrderId, collection.m_StrName);
}

void CBlockConfig::AddListener( CBlockConfigListener *pListener )
{
	if ( NULL != pListener )
	{
		for ( int i=0; i < m_aListeners.GetSize() ; i++ )
		{
			if ( pListener == m_aListeners[i] )
			{
				return;
			}
		}
		m_aListeners.Add(pListener);
	}
}

void CBlockConfig::RemoveListener( CBlockConfigListener *pListener )
{
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] == pListener )
		{
			m_aListeners.RemoveAt(i);
			return;
		}
	}
}

void CBlockConfig::FireBlockListener( int32 iBlockId, CBlockConfigListener::E_BlockNotifyType eType )
{
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != NULL )
		{
			m_aListeners[i]->OnBlockConfigChange(iBlockId, eType);
		}
	}
}

void CBlockConfig::FireConfigListener( CBlockConfigListener::E_InitializeNotifyType eType )
{
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != NULL )
		{
			m_aListeners[i]->OnConfigInitialize(eType);
		}
	}
}

void CBlockConfig::RemoveAllBlockCollections()
{
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		DEL(m_aBlockCollections[i]);
	}
	m_aBlockCollections.RemoveAll();
	m_mapBlocks.clear();
}

void CBlockConfig::GetBlockCollectionNames( OUT CStringArray &aNames ) const
{
	aNames.RemoveAll();
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsValid() )
		{
			aNames.Add(m_aBlockCollections[i]->m_StrName);
		}
	}
}

CBlockCollection	* CBlockConfig::GetBlockCollectionByName( const CString &StrCollectionName ) const
{
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsValid() && m_aBlockCollections[i]->m_StrName == StrCollectionName )
		{
			return m_aBlockCollections[i];
		}
	}
	return NULL;
}

CBlockCollection	* CBlockConfig::GetBlockCollectionById( int32 iBlockCollectionId ) const
{
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsValid() && m_aBlockCollections[i]->m_iBlockCollectionId == iBlockCollectionId )
		{
			return m_aBlockCollections[i];
		}
	}
	return NULL;
}

void CBlockConfig::GetBlocksByName( const CString &StrBlockName, OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	for ( BlockMap::const_iterator it=m_mapBlocks.begin(); it != m_mapBlocks.end() ; it++ )
	{
		if ( it->second->IsValidBlock() && it->second->m_blockInfo.m_StrBlockName == StrBlockName )
		{
			aBlocks.Add(it->second);
		}
	}
}

CBlockLikeMarket    * CBlockConfig::FindBlock( int32 iBlockId ) const
{
// 	if ( iBlockId == KIDefaultPhysicalBlockId )		// ����A�ɱ���id - �Ѿ���loadʱ����������
// 	{
// 		return GetDefaultPhysicalClassBlock();		
// 	}

	BlockMap::const_iterator it = m_mapBlocks.find(iBlockId);
	if ( it != m_mapBlocks.end() && it->second->IsValidBlock() )
	{
		return it->second;
	}
	return NULL;
}

CBlockLikeMarket    * CBlockConfig::FindBlock( const CString &StrBlockName ) const
{
	BlockArray aBlocks;
	GetBlocksByName(StrBlockName, aBlocks);
	if ( aBlocks.GetSize() > 0 )
	{
		return aBlocks[0];
	}
	return NULL;
}

void CBlockConfig::GetBlocksByMerch( CMerch *pMerch, OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	if ( NULL != pMerch )
	{
		GetBlocksByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
	}
}

void CBlockConfig::GetBlocksByMerch( int32 iMarket, const CString &StrMerchCode, OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	for ( BlockMap::const_iterator it=m_mapBlocks.begin(); it != m_mapBlocks.end() ; it++ )
	{
		if ( !it->second->IsValidBlock() )
		{
			continue;
		}
		
		if ( it->second->FindMerch(iMarket, StrMerchCode) != NULL )
		{
			aBlocks.Add(it->second);
		}
	}
}

int32 CBlockConfig::GetBlockIdByMerch(int32 iMarket, const CString &StrMerchCode) const
{
	int32 iBlockId = INVALID_VALUE;
	for ( BlockMap::const_iterator it=m_mapBlocks.begin(); it != m_mapBlocks.end() ; it++ )
	{
		if ( !it->second->IsValidBlock() )
		{
			continue;
		}

		if (NULL != it->second->m_blockInfo.m_pMerch &&
			iMarket == it->second->m_blockInfo.m_pMerch->m_MerchInfo.m_iMarketId
			&& StrMerchCode == it->second->m_blockInfo.m_pMerch->m_MerchInfo.m_StrMerchCode)
		{
			iBlockId =  it->second->m_blockInfo.m_iBlockId;
		}
	}
	return iBlockId;
}
CBlockCollection	* CBlockConfig::GetUserBlockCollection() const
{
	return GetBlockCollectionById(KIUserBlockCollectionId);
}

void CBlockConfig::GetUserBlocks( OUT BlockArray &aUserBlocks ) const
{
	aUserBlocks.GetSize();
	CBlockCollection *pCollection = GetUserBlockCollection();
	if ( NULL != pCollection )
	{
		pCollection->GetValidBlocks(aUserBlocks);
	}
	ASSERT( NULL != pCollection && aUserBlocks.GetSize() > 0 );	// �����������һ����ѡ��
}


// ��ѡ�ɸ���
void CBlockConfig::OnUserBlockUpdate( CSubjectUserBlock::E_UserBlockUpdate eUpdateType )
{
	// ��ѡ�ɸ��µ�֪ͨ:		
	
	if ( CSubjectUserBlock::EUBUMerch == eUpdateType )
	{
		// ֻ��������Ʒ - ˢ���û������Ϣ
		CArray<T_Block, T_Block &> aOrgBlocks;
		CUserBlockManager::Instance()->GetBlocks(aOrgBlocks);
		BlockArray aBlockAsMarket;
		GetUserBlocks(aBlockAsMarket);
		for ( int i=0; i < aOrgBlocks.GetSize() ; i++ )	// �����������Ҷ�Ӧ�����������
		{
			for ( int j=0; j < aBlockAsMarket.GetSize() ; j++ )
			{
				if ( aBlockAsMarket[j]->m_blockInfo.m_StrBlockName == aOrgBlocks[i].m_StrName )
				{
					// ���ж����
					bool32 bSame = aBlockAsMarket[j]->m_blockInfo.m_aSubMerchs.GetSize() == aOrgBlocks[i].m_aMerchs.GetSize();
					if ( bSame  )
					{
						bSame = ( 0 == memcmp(aBlockAsMarket[j]->m_blockInfo.m_aSubMerchs.GetData(),
											 aOrgBlocks[i].m_aMerchs.GetData(), aOrgBlocks[i].m_aMerchs.GetSize() * sizeof(CMerch *)) );
					}
					if ( !bSame )
					{
						aBlockAsMarket[j]->m_blockInfo.Copy(aOrgBlocks[i].m_aMerchs);
						FireBlockListener(aBlockAsMarket[j]->m_blockInfo.m_iBlockId, CBlockConfigListener::BlockModified);
					}
					break;	// ֻ�Ƚ���ͬ��һ��
				}
			}
		}
	}
	else if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
	{
		// ��鱾�����Ϣ������ - ����������°�� �༭�˰����Ϣ ɾ���˰��
		// Ϊ�˱�����Initialize֪ͨ����ȥ���û����id��Ψһ��������ӵİ��id�����濿
		// �������ˢ�£� ɾ��������id��֪ͨɾ���������Ʒ��Ϣ
		int i = 0;
		CArray<bool32, bool32> aOrgChecked;
		CArray<T_Block, T_Block &> aOrgBlocks;
		CUserBlockManager::Instance()->GetBlocks(aOrgBlocks);
		aOrgChecked.SetSize(aOrgBlocks.GetSize());
		for ( i=0; i < aOrgBlocks.GetSize() ; i++ )
		{
			aOrgChecked[i] = false;
		}

		BlockArray aBlockAsMarket;
		GetUserBlocks(aBlockAsMarket);
		
		for ( i=0; i < aBlockAsMarket.GetSize() ; i++ )	// �����������Ҷ�Ӧ�����������
		{
			int j = 0;
			for ( j=0; j < aOrgBlocks.GetSize() ; j++ )
			{
				CString StrBlockName = aOrgBlocks[j].m_bChangeName ? aOrgBlocks[j].m_StrNameOld : aOrgBlocks[j].m_StrName;
				if ( aBlockAsMarket[i]->m_blockInfo.m_StrBlockName == StrBlockName )
				{
					// ���ж����
					bool32 bSame = aBlockAsMarket[i]->m_blockInfo.m_aSubMerchs.GetSize() == aOrgBlocks[j].m_aMerchs.GetSize();
					if ( bSame  )
					{
						bSame = 0 == memcmp(aBlockAsMarket[i]->m_blockInfo.m_aSubMerchs.GetData(),
							aOrgBlocks[j].m_aMerchs.GetData(), aOrgBlocks[j].m_aMerchs.GetSize() * sizeof(CMerch *));
					}
					if ( !bSame )
					{
						aBlockAsMarket[i]->m_blockInfo.Copy(aOrgBlocks[j].m_aMerchs);
					}
					bSame = bSame && StrBlockName == aBlockAsMarket[i]->m_blockInfo.m_StrBlockName;
					aBlockAsMarket[i]->m_blockInfo.m_StrBlockShortCut = aOrgBlocks[j].m_StrHotKey;	// ��ݼ�����������
					aBlockAsMarket[i]->m_blockInfo.m_StrBlockName = aOrgBlocks[j].m_StrName;	// ��������
					if ( !bSame )
					{
						FireBlockListener(aBlockAsMarket[i]->m_blockInfo.m_iBlockId, CBlockConfigListener::BlockModified);
					}
					aOrgChecked[j] = true;
					break;	// ֻ�Ƚ���ͬ��һ��
				}
			}

			if ( j >= aOrgBlocks.GetSize() )	// ��ɾ���İ��
			{
				if ( aBlockAsMarket[i]->IsValidBlock() )
				{
					// ֻ�Ǳ��ɾ��, �����Ʒ��Ϣ����ʵ��ɾ�����󣬷�ֹ�����ط������ָ�����	- ʵ��Ҫ����Ļ����Ժ�������ָ��
					aBlockAsMarket[i]->m_bDeleted = true;		// ��ɾ�����
					aBlockAsMarket[i]->m_blockInfo.RemoveAll();
					aBlockAsMarket[i]->m_blockInfo.m_StrBlockName.Empty();		// ��հ������
					FireBlockListener(aBlockAsMarket[i]->m_blockInfo.m_iBlockId, CBlockConfigListener::BlockDeleted);	
					m_mapBlocks.erase(aBlockAsMarket[i]->m_blockInfo.m_iBlockId);	// ������������
				}
			}
		}

		// ����Ƿ�����ӵİ��
		ASSERT( aOrgBlocks.GetSize() == aOrgChecked.GetSize() );
		for ( i=0; i < aOrgChecked.GetSize() ; i++ )
		{
			if ( !aOrgChecked[i] )	// ����ӵ�
			{
				CBlockCollection *pUserCollection = GetUserBlockCollection();
				ASSERT( NULL != pUserCollection );
				if ( pUserCollection != NULL )
				{
					CBlockInfo blockInfo;
					blockInfo.m_iOrderId = -m_iNewUserBlockId;		// ��������
					blockInfo.m_iBlockId = m_iNewUserBlockId;		// ʹ�����µ��û����id
					blockInfo.m_iType    = CBlockInfo::typeUserBlock;
					m_iNewUserBlockId--;
					blockInfo.m_StrBlockName = aOrgBlocks[i].m_StrName;
					blockInfo.m_StrBlockShortCut = aOrgBlocks[i].m_StrHotKey;
					CBlockLikeMarket *pNewBlock = InsertBlockIntoCollection(pUserCollection, blockInfo, true);
					ASSERT( pNewBlock != NULL );
					if ( NULL != pNewBlock )
					{
						pNewBlock->m_blockInfo.Copy(aOrgBlocks[i].m_aMerchs);
						FireBlockListener(blockInfo.m_iBlockId, CBlockConfigListener::BlockAdded);
					}
				}
			}
		}
	}
}


CBlockLikeMarket    * CBlockConfig::InsertBlockIntoCollection( CBlockCollection *pCollection, const CBlockInfo &blockInfo, bool32 bAddToCache /*= true*/ )
{
	if ( NULL == pCollection || !blockInfo.IsValid() )
	{
		return NULL;
	}

	if ( bAddToCache )
	{
		// ����Ƿ��Ѿ������ˣ��������ظ�����
		if ( m_mapBlocks.count(blockInfo.m_iBlockId) > 0 )
		{
			ASSERT( 0 );
			return NULL;
		}
	}

	CBlockLikeMarket *pNewBlock = pCollection->InsertBlock(blockInfo);
	if ( NULL == pNewBlock )
	{
		return NULL;
	}

	{
		// ���һЩ����� debug
		// �û������insertǰ��Ӧ�����ú��û�����־
		ASSERT( pCollection->m_iBlockCollectionId != KIUserBlockCollectionId || blockInfo.m_iType == CBlockInfo::typeUserBlock );
		// ����a�Ӱ��ҲӦ�����úñ�־
		ASSERT( pCollection->m_iBlockCollectionId != KISpecialSHSZAMarketBlockCollectionId || blockInfo.m_iType == CBlockInfo::typeHSASubBlock );
	}

	if ( bAddToCache )	// ��������
	{
		if ( pCollection == GetBlockCollectionById(pCollection->m_iBlockCollectionId)
			|| pCollection == &m_collectionSHSZA )	// Ҫ�����ü����˵Ĳ���
		{
			m_mapBlocks[blockInfo.m_iBlockId] = pNewBlock;
		}
		else
		{
			ASSERT( 0 );
		}
	}
	return pNewBlock;
}

void CBlockConfig::GetCollectionIdArray( OUT IdArray &aIds ) const
{
	aIds.RemoveAll();
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsValid() )
		{
			aIds.Add(m_aBlockCollections[i]->m_iBlockCollectionId);
		}
	}
}

bool32 CBlockConfig::GetCollectionName( int32 iCollectionId, OUT CString &StrCollectionName ) const
{
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsValid() && m_aBlockCollections[i]->m_iBlockCollectionId == iCollectionId )
		{
			StrCollectionName = m_aBlockCollections[i]->m_StrName;
			return true;
		}
	}
	return false;
}

bool32 CBlockConfig::IsUserBlock( int32 iBlockId ) const
{
	CBlockLikeMarket *pBlock = FindBlock(iBlockId);
	if ( pBlock != NULL )
	{
		if ( pBlock->m_blockCollection.m_iBlockCollectionId == KIUserBlockCollectionId )
		{
			return true;
		}
	}
	return false;
}


bool32 CBlockConfig::IsUserCollection( int32 iCollectionId ) const
{
	return iCollectionId == KIUserBlockCollectionId;	
}


int32 CBlockConfig::GetCollectionCount() const
{
	return m_aBlockCollections.GetSize();
}

int32 CBlockConfig::GetBlockCount( DWORD dwFlag /*= 0*/ ) const
{
	if ( dwFlag != 0 )
	{
		ASSERT( 0 );		// δʵ�ֱ�־����
	}
	return m_mapBlocks.size();
}

void CBlockConfig::GetBlockIdArray( OUT IdArray &aIds, DWORD dwFlag /*= 0*/ ) const
{
	ASSERT( dwFlag == 0 );

	aIds.RemoveAll();
	for ( BlockMap::const_iterator it = m_mapBlocks.begin(); it != m_mapBlocks.end() ; it++ )
	{
		if ( it->second->IsValidBlock() )
		{
			aIds.Add(it->first);
		}
	}
}

void CBlockConfig::GetBlockNameArray( OUT CStringArray &aNames, DWORD dwFlag /*= 0*/ ) const
{
	ASSERT( dwFlag == 0 );
	
	aNames.RemoveAll();
	for ( BlockMap::const_iterator it = m_mapBlocks.begin(); it != m_mapBlocks.end() ; it++ )
	{
		if ( it->second->IsValidBlock() )
		{
			aNames.Add(it->second->m_blockInfo.m_StrBlockName);
		}
	}
}

bool32 CBlockConfig::RequestLogicBlockFile( CViewData *pViewData )
{
	CMmiReqLogicBlockFile req;
	if ( NULL != pViewData )
	{
		bool32 bCrcOk = false;
		CFile file;
		CString StrFileName;
		GetConfigFullPathName(StrFileName);
		if ( file.Open(StrFileName, CFile::modeRead) )
		{
			ULONGLONG ullFileLength = file.GetLength();
			if ( ullFileLength != 0 && ullFileLength < UINT_MAX)
			{
				char *pBuf = new char[(uint32)ullFileLength];		 //	crude code for read file, modify by weng.cx
				if (NULL != pBuf)
				{
					if (file.Read(pBuf, (uint32)ullFileLength) == (uint32)ullFileLength)
					{
						req.m_uCRC32 = CCodeFile::crc_32(pBuf, (uint32)ullFileLength);
						bCrcOk = true;
					}
					delete []pBuf;
					pBuf = NULL;
				}						
			}
			file.Close();
		}
		if ( !bCrcOk )
		{
			req.m_uCRC32 = 0;
		}
		pViewData->RequestViewData(&req);
		m_bWaitForServerResp = true;
		return true;
	}
	return false;
}

bool32 CBlockConfig::RespLogicBlockFile( const CMmiRespLogicBlockFile *pResp, bool32 bNeedInit/* = false*/ )
{
	CFile file;
	CString StrFileName;
	GetConfigFullPathName(StrFileName);

	m_bWaitForServerResp = false;
	if ( pResp != NULL && pResp->m_pData != NULL && pResp->m_uiDataLen >= 2 )
	{
		if ( file.Open(StrFileName, CFile::modeCreate |CFile::modeWrite) )
		{
			file.Write(pResp->m_pData, pResp->m_uiDataLen-2);
			file.Close();

			return Instance()->Initialize(StrFileName);
		}
	}
	if ( bNeedInit )
	{
		return Instance()->Initialize(StrFileName);	// ��Ҫ��ʼ����ʹ�ñ����ļ���ʼ��
	}
	return pResp != NULL && pResp->m_pData == NULL;
}

bool32 CBlockConfig::IsUserMerch( CMerch *pMerch ) const
{
	return CUserBlockManager::Instance()->GetBlock(pMerch) != NULL;
}


int32 CBlockConfig::GetDefaultMarketlClassBlockPseudoId()
{
	return KIDefaultMarketBlockId;	
}


bool32 CBlockConfig::GetDefaultMarketClassBlockId( OUT int32 &iBlockId ) const
{
	CBlockLikeMarket *pBlock = GetDefaultMarketClassBlock();
	if ( NULL != pBlock )
	{
		iBlockId = pBlock->m_blockInfo.m_iBlockId;
		return true;
	}
	return false;
}


CBlockLikeMarket	* CBlockConfig::GetDefaultMarketClassBlock() const
{
	CBlockLikeMarket *pBlockDef = FindBlock(KIDefaultMarketBlockId);
	if ( pBlockDef == NULL )
	{
		CStringArray	aDefaultNames;
		aDefaultNames.Add(_T("����A��"));
		aDefaultNames.Add(_T("�A��"));
		aDefaultNames.Add(_T("�������"));
		aDefaultNames.Add(_T("�����"));
		BlockArray	aBlocks;
		GetMarketClassBlocks(aBlocks);
		for ( int i=0; i < aBlocks.GetSize() ; i++ )
		{
			for ( int j=0; j < aDefaultNames.GetSize() ; j++ )
			{
				if ( aBlocks[i]->m_blockInfo.m_StrBlockName.CompareNoCase(aDefaultNames[j]) == 0 )
				{
					return	aBlocks[i];
				}
			}
		}
		return NULL;
	}
	return pBlockDef;
}


bool32 CBlockConfig::GetMarketClassCollectionId( OUT int32 &iCollectionId ) const
{
	for ( int i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsMarketClassBlockCollection() )
		{
			iCollectionId = m_aBlockCollections[i]->m_iBlockCollectionId;
			return true;
		}
	}
	return false;
}

void CBlockConfig::GetMarketClassBlocks( OUT BlockArray &aClassBlocks ) const
{
	int i =0 ;
	aClassBlocks.RemoveAll();
	for ( i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		if ( m_aBlockCollections[i]->IsMarketClassBlockCollection() )
		{
			BlockArray	aBSub;
			m_aBlockCollections[i]->GetValidBlocks(aBSub);
			aClassBlocks.Append(aBSub);
		}
	}
}

void CBlockConfig::GetNormalBlocks( OUT BlockArray &aBlocks ) const
{
	int i =0 ;
	aBlocks.RemoveAll();
	for ( i=0; i < m_aBlockCollections.GetSize() ; i++ )
	{
		// Ŀǰֻ�з��� �û� ��ͨ�ļ�������
		if ( m_aBlockCollections[i]->m_aBlocks.GetSize() > 0
			&& m_aBlockCollections[i]->m_aBlocks[0]->m_blockInfo.m_iType == CBlockInfo::typeNormalBlock )
		{
			BlockArray	aBSub;
			m_aBlockCollections[i]->GetValidBlocks(aBSub);
			aBlocks.Append(aBSub);
		}
	}
}

void CBlockConfig::FillMerchByMaskString( const CString &StrMask, int32 iMarketId, OUT MerchArray &aMerchsFind )
{
	//DWORD dwTime = timeGetTime();

	aMerchsFind.RemoveAll();


	if ( NULL == m_pViewData )
	{
		return;
	}

	CMarket *pMarket = NULL;
	if ( !m_pViewData->m_MerchManager.FindMarket(iMarketId, pMarket) || NULL == pMarket )
	{
		return;
	}

	MerchArray	aMerchsTmp;
	aMerchsTmp.Copy(pMarket->m_MerchsPtr);
	CStringArray	aMerchsName;
	aMerchsName.SetSize(aMerchsTmp.GetSize());
	int i=0;
	for ( i=0; i < aMerchsTmp.GetSize() ; i++ )
	{
		aMerchsName[i] = aMerchsTmp[i]->m_MerchInfo.m_StrMerchCode;
	}


	if ( StrMask.Find(_T('*')) == -1 )
	{
		for ( i=aMerchsName.GetSize()-1; i >= 0 ; i-- )	//	modify by weng.cx
		{
			if ( aMerchsName[i].Find(StrMask) == -1 )
			{
				aMerchsName.RemoveAt(i);
				aMerchsTmp.RemoveAt(i);
			}
		}
	}
	else
	{
		bool32	bPreCharIsStar = false;
		for ( i=0; i < StrMask.GetLength() ; i++ )		// ����ƥ���ǲ���Ҫ�ã�
		{
			TCHAR ch = StrMask.GetAt(i);
			if ( ch != _T('*') )	// ����*�ţ��Ϳ�
			{
				if ( bPreCharIsStar )	// ǰ��һ����*�ţ��Ϳ������Ƿ��и��ַ���û�еĻ�����Ϊ��ƥ��
				{
					for ( int j=aMerchsName.GetSize()-1; j >= 0 ; j-- )
					{
						int iFind = aMerchsName[j].Find(ch, 0);
						if ( iFind < 0 )	// �޸��ַ� ��ƥ��
						{
							aMerchsName.RemoveAt(j);
							aMerchsTmp.RemoveAt(j);
						}	
						else	// �и��ַ������Ѿ�ƥ����ַ�sɾ��
						{
							aMerchsName[j].Delete(0, iFind+1);	// ɾ�����ַ�
						}
					}
				}
				else					// ǰ��һ������*����ǰ�ַ��ǲ��Ǹ��ַ���û�еĻ�����ƥ��
				{
					for ( int j=aMerchsName.GetSize()-1; j >= 0 ; j-- )
					{
						if ( aMerchsName[j].GetAt(0) != ch )
						{
							aMerchsName.RemoveAt(j);
							aMerchsTmp.RemoveAt(j);
						}
						else	// ɾ����ǰƥ���˵��ַ�
						{
							aMerchsName[j].Delete(0, 1);
						}
					}
				}
				bPreCharIsStar = false;
			}
			else
			{
				bPreCharIsStar = true;	// * ��ȫƥ�䣬������
			}
		}
	}
	
	aMerchsFind.Copy(aMerchsTmp);

	//TRACE(_T("������Ʒ: %d-%s %dms\r\n"), iMarketId, StrMask, timeGetTime()-dwTime);
}


bool32 CBlockConfig::LoadSepecialSHSZAMarket( CBlockLikeMarket &blockParent, int32 iMarketId )
{

	if ( NULL == m_pViewData )
	{
		return false;
	}
	
	CMarket *pMarket = NULL;
	if ( !m_pViewData->m_MerchManager.FindMarket(iMarketId, pMarket) || NULL == pMarket )
	{
		return false;
	}

	CBlockInfo marketBlockInfo;
	marketBlockInfo.m_iBlockId = KIMaxSpecialSHSZAMarketBlockId--;
	marketBlockInfo.m_iType = CBlockInfo::typeHSASubBlock;		// ����a�Ӱ����������;		
	marketBlockInfo.m_StrBlockName = pMarket->m_MarketInfo.m_StrCnName;
	CBlockLikeMarket *pNewBlock = InsertBlockIntoCollection(&m_collectionSHSZA, marketBlockInfo, true);
	ASSERT( pNewBlock != NULL );
	ASSERT( FindBlock(marketBlockInfo.m_iBlockId) != NULL );
	if ( NULL != pNewBlock )
	{
		pNewBlock->m_blockInfo.Copy(pMarket->m_MerchsPtr);
		blockParent.m_blockInfo.m_aSubBlockIds.Add(pNewBlock->m_blockInfo.m_iBlockId);
		return true;
	}
	return false;
}

CString CBlockConfig::GetBlockName( int32 iBlockId ) const
{
	CBlockLikeMarket *pBlock = FindBlock(iBlockId);
	if ( pBlock != NULL )
	{
		return pBlock->m_blockInfo.m_StrBlockName;
	}
	return CString();
}

void CBlockConfig::BuildHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys)
{
	for ( int32 i=0; i < m_aBlockCollections.GetSize() ; i++ )	// �����������飬��ѡ��飬�����Ӽ�, ������һ���߼����
	{
		for ( int32 j=0; j < m_aBlockCollections[i]->m_aBlocks.GetSize() ; j++ )
		{
			const CBlockLikeMarket &block = *m_aBlockCollections[i]->m_aBlocks[j];
			if ( !block.IsValidBlock() 
				|| CBlockInfo::typeNormalBlock != block.m_blockInfo.GetBlockType() )
			{
				continue;
			}

			CHotKey	hotKey;
			hotKey.m_eHotKeyType = EHKTLogicBlock;
			if ( block.m_blockInfo.m_StrBlockShortCut.IsEmpty() )
			{
				hotKey.m_StrKey		 = ConvertHZToPY(block.m_blockInfo.m_StrBlockName);
			}
			else
			{
				hotKey.m_StrKey		 = block.m_blockInfo.m_StrBlockShortCut;
			}
			hotKey.m_StrSummary	 = block.m_blockInfo.m_StrBlockName + _T("[���]");
			hotKey.m_StrParam1   = block.m_blockInfo.m_StrBlockName;
			hotKey.m_iParam1	 = 4;	// =T_BlockDesc::EBTBlockLogical;
			hotKey.m_iParam2	 = block.m_blockInfo.m_iBlockId;	// Ψһ��blockid

			arrHotKeys.Add(hotKey);
		}
	}
}

void CBlockConfig::SetViewData(CViewData* pData)
{
	m_pViewData = pData;
}



//////////////////////////////////////////////////////////////////////////
// �����ṹ - ��Щ�ṹ�����ݲ������ᷢ��֪ͨ

// ��鼯��
CBlockCollection::CBlockCollection()
{
	m_iBlockCollectionId = KIInvalidId;
	m_iOrderId = 0;
	m_aBlocks.SetSize(0, 300);
}

CBlockCollection::~CBlockCollection()
{
	RemoveAllBlock();
}

CBlockLikeMarket	* CBlockCollection::InsertBlock( const CBlockInfo &blockInfo )
{
	if ( IsValid() && blockInfo.IsValid() )
	{
		int i = 0;
		for ( i=0; i < m_aBlocks.GetSize() ; i++ )
		{
			if ( m_aBlocks[i]->m_blockInfo.m_iBlockId == blockInfo.m_iBlockId )	// ����ظ�
			{
				ASSERT( 0 );
				return NULL;
			}

			if ( m_aBlocks[i]->m_blockInfo.m_StrBlockName == blockInfo.m_StrBlockName && m_iBlockCollectionId != CBlockConfig::KISpecialSHSZAMarketBlockCollectionId )
			{				
				/*ASSERT( 0 );*/ //	��֪���˶��Ե����壬��ʱ���а�
			}
		}

		CBlockLikeMarket	*pNewBlock = new CBlockLikeMarket(*this);
		pNewBlock->m_blockInfo = blockInfo;
		pNewBlock->m_logicBlockData.m_iBlockId = blockInfo.m_iBlockId;
		bool32 bInserted = false;
		for ( i=0; i < m_aBlocks.GetSize() ; i++ )
		{
			if ( m_aBlocks[i]->m_blockInfo.m_iOrderId > blockInfo.m_iOrderId ) // small -> big
			{
				m_aBlocks.InsertAt(i, pNewBlock);
				bInserted = true;
				break;
			}
		}
		if ( !bInserted )
		{
			m_aBlocks.Add(pNewBlock);
		}

		return pNewBlock;
	}
	else
	{
		ASSERT( 0 );
	}
	return NULL;
}

void CBlockCollection::RemoveAllBlock()
{
	for ( int i=0; i < m_aBlocks.GetSize() ; i++ )
	{
		DEL(m_aBlocks[i]);
	}
	m_aBlocks.RemoveAll();
}

CBlockLikeMarket  * CBlockCollection::FindBlock( int32 iBlockId ) const
{
	for ( int i=0; i < m_aBlocks.GetSize() && IsValid() ; i++ )
	{
		if ( m_aBlocks[i]->IsValidBlock() && m_aBlocks[i]->m_blockInfo.m_iBlockId == iBlockId )
		{
			return m_aBlocks[i];
		}
	}
	return NULL;
}

void CBlockCollection::GetBlockByName( const CString &StrBlockName, OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	for ( int i=0; i < m_aBlocks.GetSize() && IsValid() ; i++ )
	{
		if ( m_aBlocks[i]->IsValidBlock() && m_aBlocks[i]->m_blockInfo.m_StrBlockName == StrBlockName )
		{
			aBlocks.Add(m_aBlocks[i]);
		}
	}
}

void CBlockCollection::GetBlockByMerch( CMerch *pMerch, OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	if ( NULL == pMerch )
	{
		return;
	}
	GetBlockByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
}

void CBlockCollection::GetBlockByMerch( int32 iMarketid, const CString &StrMerchCode, OUT BlockArray &aBlocks ) const
{
	for ( int i=0; i < m_aBlocks.GetSize() && IsValid() ; i++ )
	{
		if ( m_aBlocks[i]->IsValidBlock() )
		{
			if ( m_aBlocks[i]->FindMerch(iMarketid, StrMerchCode) != NULL )
			{
				aBlocks.Add(m_aBlocks[i]);
			}	
		}
	}
}

bool32 CBlockCollection::IsValid() const
{
	if ( NULL != this && m_iBlockCollectionId != KIInvalidId )
	{
		return true;
	}
	return false;
}

void CBlockCollection::GetValidBlocks( OUT BlockArray &aBlocks ) const
{
	aBlocks.RemoveAll();
	if ( IsValid() )
	{
		for ( int i=0; i < m_aBlocks.GetSize() ; i++ )
		{
			if ( m_aBlocks[i]->IsValidBlock() )
			{
				aBlocks.Add(m_aBlocks[i]);
			}
		}
	}
}

int32 CBlockCollection::GetValidBlockCount() const
{
	int iCount = 0;
	if ( IsValid() )
	{
		for ( int i=0; i < m_aBlocks.GetSize() ; i++ )
		{
			if ( m_aBlocks[i]->IsValidBlock() )
			{
				iCount++;
			}
		}
	}
	return iCount;
}

void CBlockCollection::GetValidBlockIdArray( OUT IdArray &aIds ) const
{
	aIds.RemoveAll();
	if ( IsValid() )
	{
		for ( int i=0; i < m_aBlocks.GetSize() ; i++ )
		{
			if ( m_aBlocks[i]->IsValidBlock() )
			{
				aIds.Add(m_aBlocks[i]->m_blockInfo.m_iBlockId);
			}
		}
	}
}

bool32 CBlockCollection::IsMarketClassBlockCollection() const
{
	BlockArray	aBlocks;
	GetValidBlocks(aBlocks);
	for ( int i=0; i < aBlocks.GetSize() ; i++ )
	{
		if ( aBlocks[i]->m_blockInfo.IsMarketClassBlock() )
		{
			return true;
		}
	}
	return false;
}

bool32 CBlockCollection::IsUserBlockCollection() const
{
	return m_iBlockCollectionId == CBlockConfig::KIUserBlockCollectionId;
}



// Block

CBlockLikeMarket::CBlockLikeMarket( const CBlockCollection &collection )
:m_blockCollection(collection)
{
	m_bDeleted = false;

	m_logicBlockData.m_iBlockId = KIInvalidId;
}

CMerch			* CBlockLikeMarket::FindMerch( int32 iMarketId, const CString &StrMerchCode ) const
{
	return IsValidBlock() ? m_blockInfo.FindMerchInBlock(iMarketId, StrMerchCode) : NULL;
}

bool32 CBlockLikeMarket::IsMerchInBlock( CMerch *pMerch ) const
{
	return 	IsValidBlock() ? m_blockInfo.IsMerchInBlock(pMerch) : NULL;
}

bool32 CBlockLikeMarket::IsValidBlock() const
{
	if ( NULL != this && !m_bDeleted && m_blockInfo.IsValid() )
	{
		return true;
	}
	return false;
}

bool32 CBlockLikeMarket::IsValidLogicBlockData() const
{
	return /*!m_logicBlockData.m_StrMerchBestCode.IsEmpty() &&*/ m_logicBlockData.m_iBlockId != KIInvalidId;
}

bool32 CBlockLikeMarket::IsUserBlock() const
{
	return m_blockCollection.IsUserBlockCollection();
}


// block info

CBlockInfo::CBlockInfo()
{
	m_iBlockId = KIInvalidId;
	m_iOrderId = 0;
	m_pMerch   = NULL;
	m_iType = typeNormalBlock;
}

CBlockInfo::CBlockInfo( const CBlockInfo &blockInfo )
{
	m_iBlockId = blockInfo.m_iBlockId;
	m_iOrderId = blockInfo.m_iOrderId;
	m_StrBlockName = blockInfo.m_StrBlockName;
	m_StrBlockShortCut = blockInfo.m_StrBlockShortCut;
	m_iType	=	blockInfo.m_iType;
	m_aSubMerchs.Copy(blockInfo.m_aSubMerchs);
	m_aSubBlockIds.Copy(blockInfo.m_aSubBlockIds);

	m_mapCodeMerchs = blockInfo.m_mapCodeMerchs;
	m_mapNameMerchs = blockInfo.m_mapNameMerchs;
}

const CBlockInfo & CBlockInfo::operator=( const CBlockInfo &blockInfo )
{
	if ( this == &blockInfo )
	{
		return *this;
	}
	m_iBlockId = blockInfo.m_iBlockId;
	m_iOrderId = blockInfo.m_iOrderId;
	m_StrBlockName = blockInfo.m_StrBlockName;
	m_StrBlockShortCut = blockInfo.m_StrBlockShortCut;
	m_iType		=	blockInfo.m_iType;
	m_aSubMerchs.Copy(blockInfo.m_aSubMerchs);
	m_aSubBlockIds.Copy(blockInfo.m_aSubBlockIds);

	m_pMerch        = blockInfo.m_pMerch;
	m_mapCodeMerchs = blockInfo.m_mapCodeMerchs;
	m_mapNameMerchs = blockInfo.m_mapNameMerchs;

	return *this;
}

bool32 CBlockInfo::IsValid() const
{
	return this != NULL && m_iBlockId != KIInvalidId;		// ���Ʋ���
}

CMerch	* CBlockInfo::FindMerchInBlock( int32 iMarketId, const CString &StrMerchCode ) const
{
	for ( int i=0; IsValid() && i < m_aSubMerchs.GetSize() ; i++ )
	{
		if ( m_aSubMerchs[i]->m_MerchInfo.m_iMarketId == iMarketId
			 && m_aSubMerchs[i]->m_MerchInfo.m_StrMerchCode == StrMerchCode )
		{
			return m_aSubMerchs[i];
		}
	}
	return NULL;
}

bool32 CBlockInfo::IsMerchInBlock( CMerch *pMerch ) const
{
	return NULL != pMerch && NULL != FindMerchInBlock(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode);
}

bool32 CBlockInfo::FillBlock( OUT T_Block &block )
{
	if ( !IsValid() )
	{
		return false;
	}

	block = T_Block();
	block.m_StrName = m_StrBlockName;
	block.m_StrHotKey = m_StrBlockShortCut;
	block.m_aMerchs.Copy(m_aSubMerchs);
	block.m_eHeadType = ERTStockCn;		// Ĭ�Ϲ���֤ȯ��ͷ

	return true;
}

int32 CBlockInfo::GetInvalidId()
{
	return KIInvalidId;
}

bool32 CBlockInfo::IsMarketClassBlock() const
{
	return m_iType == typeMarketClassBlock;
}


void CBlockInfo::Copy( const MerchArray &aMerchs )
{
	m_aSubMerchs.Copy(aMerchs);
	
	RecreateMerchIndexs();
}

void CBlockInfo::Append( const MerchArray &aMerchs )
{
	m_aSubMerchs.Append(aMerchs);

	RecreateMerchIndexs();
}

void CBlockInfo::Add( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}
	m_aSubMerchs.Add(pMerch);
	
	AddMerchIndex(pMerch);
}

void CBlockInfo::SetAt( int iIndex, CMerch *pMerch )
{
	if ( iIndex >= 0 && iIndex < m_aSubMerchs.GetSize() && NULL != pMerch )
	{
		CMerch *pMerchOld = m_aSubMerchs[iIndex];
		m_aSubMerchs[iIndex] = pMerch;
		RemoveMerchIndex(pMerchOld);
		AddMerchIndex(pMerch);
	}
	else
	{
		ASSERT( 0 );
	}
}

CMerch *CBlockInfo::RemoveAt( int iIndex)
{
	if ( iIndex >=0 && iIndex < m_aSubMerchs.GetSize() )
	{
		CMerch *pMerchRemove = m_aSubMerchs[iIndex];
		RemoveMerchIndex(pMerchRemove);
		m_aSubMerchs.RemoveAt(iIndex);
	}
	else
	{
		ASSERT( 0 );
	}
	return NULL;
}

void CBlockInfo::RecreateMerchIndexs(bool32 bForce /*= false*/)
{
	m_mapCodeMerchs.clear();
	m_mapNameMerchs.clear();
	m_mapSubMerchMarkets.RemoveAll();

	// �˹��̺�ʱ����
	if ( bForce )
	{
		for ( int32 i=0; i < m_aSubMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_aSubMerchs[i];
			if ( NULL == pMerch )
			{
				ASSERT( 0 );
				continue;
			}
			
			m_mapCodeMerchs[ pMerch->m_MerchInfo.m_StrMerchCode ] = pMerch;
			m_mapNameMerchs[ pMerch->m_MerchInfo.m_StrMerchCnName ] = pMerch;
			
			int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
			int32 iValue;
			if ( m_mapSubMerchMarkets.Lookup(iMarketId, iValue) )
			{
				m_mapSubMerchMarkets[iMarketId]++;
			}
			else
			{
				m_mapSubMerchMarkets[ iMarketId ] = 1;
			}
		}
	}
	else
	{
		for ( int32 i=0; i < m_aSubMerchs.GetSize() ; i++ )
		{
			CMerch *pMerch = m_aSubMerchs[i];
			if ( NULL == pMerch )
			{
				ASSERT( 0 );
				continue;
			}
			
			//m_mapCodeMerchs[ pMerch->m_MerchInfo.m_StrMerchCode ] = pMerch;
			//m_mapNameMerchs[ pMerch->m_MerchInfo.m_StrMerchCnName ] = pMerch;
			
			int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
			int32 iValue;
			if ( m_mapSubMerchMarkets.Lookup(iMarketId, iValue) )
			{
				m_mapSubMerchMarkets[iMarketId]++;
			}
			else
			{
				m_mapSubMerchMarkets[ iMarketId ] = 1;
			}
		}
	}
	
}

void CBlockInfo::AddMerchIndex( CMerch *pMerch )
{
	if ( NULL != pMerch )
	{
		m_mapCodeMerchs[ pMerch->m_MerchInfo.m_StrMerchCode ] = pMerch;
		m_mapNameMerchs[ pMerch->m_MerchInfo.m_StrMerchCnName ] = pMerch;

		int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
		int32 iValue;
		if ( m_mapSubMerchMarkets.Lookup(iMarketId, iValue) )
		{
			m_mapSubMerchMarkets[iMarketId]++;
		}
		else
		{
			m_mapSubMerchMarkets[ iMarketId ] = 1;
		}
	}
	else
	{
		ASSERT( 0 );
	}
}

void CBlockInfo::RemoveMerchIndex( CMerch *pMerch )
{
	if ( NULL != pMerch )
	{
		m_mapCodeMerchs.erase( pMerch->m_MerchInfo.m_StrMerchCode );
		m_mapNameMerchs.erase( pMerch->m_MerchInfo.m_StrMerchCnName );

		int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
		int32 iValue;
		if ( m_mapSubMerchMarkets.Lookup(iMarketId, iValue) )
		{
			iValue--;
			if ( iValue <= 0 )
			{
				m_mapSubMerchMarkets.RemoveKey(iMarketId);
			}
			else
			{
				m_mapSubMerchMarkets[iMarketId]--;
			}
		}
	}
	else
	{
		ASSERT( 0 );
	}
}

CMerch  * CBlockInfo::FindMerchByMerchCode( const CString &StrMerchCode ) const
{
	MerchStringMap::const_iterator it = m_mapCodeMerchs.find(StrMerchCode);
	if ( it != m_mapCodeMerchs.end() )
	{
		return it->second;
	}
	else if ( m_aSubMerchs.GetSize() != m_mapNameMerchs.size() )
	{
		const_cast<CBlockInfo &>(*this).RecreateMerchIndexs(true);	// ���´�������
		it = m_mapCodeMerchs.find(StrMerchCode);
		if ( it != m_mapCodeMerchs.end() )
		{
			return it->second;
		}
	}
	return NULL;
}

CMerch  * CBlockInfo::FindMerchByCnName( const CString &StrMerchCnName ) const
{
	MerchStringMap::const_iterator it = m_mapNameMerchs.find(StrMerchCnName);
	if ( it != m_mapNameMerchs.end() )
	{
		return it->second;
	}
	else if ( m_aSubMerchs.GetSize() != m_mapNameMerchs.size() )
	{
		const_cast<CBlockInfo &>(*this).RecreateMerchIndexs(true);	// ���´�������
		it = m_mapNameMerchs.find(StrMerchCnName);
		if ( it != m_mapNameMerchs.end() )
		{
			return it->second;
		}
	}
	return NULL;
}

void CBlockInfo::RemoveAll()
{
	m_aSubMerchs.RemoveAll();

	m_mapCodeMerchs.clear();
	m_mapNameMerchs.clear();
	m_mapSubMerchMarkets.RemoveAll();
}

CBlockInfo::E_BlockType CBlockInfo::GetBlockType() const
{
	return (E_BlockType)m_iType;
}




// Block�����ֶ�


CBlockDataPseudo::HeaderNameMap CBlockDataPseudo::m_smapHeaderNames;

void CBlockDataPseudo::GetDefaultColNames( OUT CStringArray &aStrNames )
{
	HeaderArray aHeaders;
	GetDefaultColArray(aHeaders);
	aStrNames.RemoveAll();
	GetColNames(aHeaders, aStrNames);
}
 
int32 CBlockDataPseudo::GetColNames( IN const HeaderArray &aHeaders, OUT CStringArray &aStrNames )
{
	InitializeHeaderNameMap();

	int32 iRet = 0;
	aStrNames.SetSize(m_smapHeaderNames.size());
	for ( int32 i=0; i < aHeaders.GetSize() ; i++ )
	{
		if ( GetColName(aHeaders[i], aStrNames[iRet]) )
		{
			iRet++;
		}
	}

	aStrNames.SetSize(iRet);

	return iRet;
}

void CBlockDataPseudo::GetDefaultColArray( OUT HeaderArray &aHeaders )
{
	aHeaders.RemoveAll();
	for ( int i=0; i < HeaderCount ; i++ )
	{
		aHeaders.Add((E_Header)i);
	}
}

bool32 CBlockDataPseudo::GetColName( E_Header eHeader, OUT CString &StrName )
{
	InitializeHeaderNameMap();
	HeaderNameMap::const_iterator it = m_smapHeaderNames.find(eHeader);
	if (  it != m_smapHeaderNames.end() )
	{
		StrName = it->second;
		return true;
	}
	return false;
}


void CBlockDataPseudo::InitializeHeaderNameMap()
{
	if ( m_smapHeaderNames.empty() )
	{
		m_smapHeaderNames.insert(HeaderNameMap::value_type(ShowRowNo,		    _T("��  ")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(BlockId,		        _T("������")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(BlockName,	        _T("�������")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(RiseRate,		    _T("�Ƿ�%")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(RiseSpeed,	        _T("����%")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(PrincipalPureNum,    _T("��������")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(PrincipalAmount,		_T("�������")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(AmountRate,		    _T("����")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(RiseMerchCount,	 	_T("�Ǽ���")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(FallMerchCount,      _T("������")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(RiseTopestMerch,	    _T("���ǹ�")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(TotalHand,           _T("����")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(TotalAmount,         _T("�ܽ��")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(TotalMarketValue,    _T("����ֵ")));
		m_smapHeaderNames.insert(HeaderNameMap::value_type(CircleMarketValue,   _T("��ͨ��ֵ")));
	}
	ASSERT( m_smapHeaderNames.size() == HeaderCount );
}

bool32 CBlockDataPseudo::GetColValue( E_Header eHeader,  IN CViewData *pViewData, OUT void *pValue ) const
{
	// �����mapҪ�� - �Ƚ�����
	if ( NULL == pValue )
	{
		return false;
	}

	float *pF = (float *)pValue;

	bool32 bRet = false;
	switch (eHeader)
	{
	case RiseRate:
		*pF = m_blockData.m_fAvgRise * 100;
		bRet = true;
		break;
	case WeightRiseRate:
		*pF = m_blockData.m_fWeightRise;
		bRet = true;
		break;
	case TotalAmount:
		*pF = m_blockData.m_fTotalAmount;
		bRet = true;
		break;
	case MarketRate:
		*pF = m_blockData.m_fMarketRate;
		bRet = true;
		break;
	case ChangeRate:
		*pF = m_blockData.m_fChange;
		bRet = true;
		break;
	case MarketWinRateDync:
		*pF = m_blockData.m_fPEratio;
		bRet = true;
		break;
	case RiseMerchRate:
		//*pF = m_blockData.m_fRiseRate;
		{
			*pF = (float)m_blockData.m_iRiseCounts;
			if ( 0 != m_blockData.m_iFallCounts )
			{
				*pF = m_blockData.m_iRiseCounts/(float)m_blockData.m_iFallCounts;
			}
		}
		bRet = true;
		break;
	case CapitalFlow:
		*pF = m_blockData.m_fCapticalFlow;
		bRet = true;
		break;
	case RiseMerchCount:
		*pF =(float) m_blockData.m_iRiseCounts;
		bRet = true;
		break;
	case FallMerchCount:
		*pF = (float)m_blockData.m_iFallCounts;
		bRet = true;
		break;
	case MerchRise:
		{
			*pF = (float)m_blockData.m_fMerchRise;
			bRet = true;
			break;
		}
		break;
	case RiseDays:
		{
			*pF = m_blockData.m_iRiseDays;
			bRet = true;
			break;
		}
		break;
	case RiseSpeed:
		{
			*pF = m_blockData.m_fRiseRate * 100;
			bRet = true;
			break;
		}
		break;
	case PrincipalPureNum:
		{
			*pF = m_blockData.m_fMainNetVolume * 100;
			bRet = true;
			break;
		}
		break;
	case PrincipalAmount:
		{
			*pF = m_blockData.m_fMainAmount;
			bRet = true;
			break;
		}
		break;
	case AmountRate: 
		{
			*pF = m_blockData.m_fVolumeRate;
			bRet = true;
			break;
		}
		break;
	case TotalHand:  
		{
			*pF = m_blockData.m_fAllVolume;
			bRet = true;
			break;
		}
		break;
	case TotalMarketValue:
		{
			*pF = m_blockData.m_fAllValue;
			bRet = true;
			break;
		}
		break;
	case CircleMarketValue:
		{
			*pF = m_blockData.m_fCircValue;
			bRet = true;
			break;
		}
		break;
	default:
		ASSERT( 0 );
	}

	return bRet;
}


// 
CBlockDataPseudo::CBlockDataPseudo()
{
	m_blockData.m_iBlockId = KIInvalidId;
}

CBlockDataPseudo::CBlockDataPseudo( const T_LogicBlock &logicData )
{
	m_blockData = logicData;
}

bool32 CBlockDataPseudo::ConvertNativeHeaderToSortHeader( IN E_Header eNativeHeader, OUT E_BlockSort &eSortHeader )
{
	typedef map<CBlockDataPseudo::E_Header, E_BlockSort>	HeaderMap;
	HeaderMap	mapHeader;
	mapHeader[ CBlockDataPseudo::RiseRate ] = EBSAvgRise;
	mapHeader[ CBlockDataPseudo::WeightRiseRate ] = EBSWeightRise;
	mapHeader[ CBlockDataPseudo::ChangeRate ] = EBSChange;
	mapHeader[ CBlockDataPseudo::MarketWinRateDync ] = EBSPeRatio;

	if ( mapHeader.count(eNativeHeader) > 0 )
	{
		eSortHeader = mapHeader[eNativeHeader];
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
CBlockConfigListener::~CBlockConfigListener()
{
	if ( CBlockConfig::PureInstance() != NULL )
	{
		CBlockConfig::PureInstance()->RemoveListener(this);		// ���Զ�ע�ᣬ������Ϊ��ֹע����ûע��������������ע��
	}
}




#include "StdAfx.h"
#include "coding.h"
#include "UploadUserData.h"

#include "viewdata.h"
#include "BlockManager.h"
#include "proxy_auth_client_base.h"
#include "CCodeFile.h"



static const int MAX_CODE = 8;	// ��Ʒ������󳤶�
static const int MAX_NAME = 16;	// ��Ʒ������󳤶�

static const char* KE_SelData       = "SELDATA";
static const char* KE_Sel           = "Sel";
static const char* KA_MarketId      = "mk";
static const char* KA_StockCode     = "cd";
static const char* KA_StockName     = "nm";
static const char* KA_TradeCode     = "tc";
static const char* KA_Dec           = "dc";
static const char* KA_TradeTimeId   = "ti";
static const char* KA_TradeTimeType = "tt";
#pragma pack(push)
#pragma pack(1)		// ���ܱ���������
typedef struct _PhoneSymbol
{
	u16	uMarket;				// �г�
	u8	aCode[MAX_CODE];		// ����
}T_PhoneSymbol;


typedef struct _PhoneSymbolEx : public _PhoneSymbol
{
	u8	aName[MAX_NAME];		// ����, ��Э�����Ϊ����
	u8	aTradeCode[MAX_CODE];	// ��Լ�� (�ͻ��˲��ù���)
	u8	iDec;					// С����λ��
}T_PhoneSymbolEx;
#pragma pack(pop)


CUploadUserDataListener::~CUploadUserDataListener()
{
	CUploadUserData::instance()->RemoveRespListener(this);		// ���շ�ֹû�м�ʱɾ��
}

//////////////////////////////////////////////////////////////////////////
//
//IMPLEMENT_DYNAMIC(CUploadUserData, )
CUploadUserData::CUploadUserData()
{
	m_pViewData = NULL;
}

CUploadUserData * CUploadUserData::instance()
{
	static CUploadUserData sUpload;
	return &sUpload;
}

void CUploadUserData::AddRespListener( CUploadUserDataListener *pListener )
{
	if ( NULL != pListener )
	{
		for ( int i=0; i < m_aListeners.GetSize() ; i++ )
		{
			if ( m_aListeners[i] == pListener )
			{
				return;
			}
		}
		m_aListeners.Add(pListener);
	}
}

void CUploadUserData::RemoveRespListener( CUploadUserDataListener *pListener )
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

void CUploadUserData::FireUploadDataListener( const CMmiRespUploadData *pResp )
{
	ASSERT( NULL != pResp );
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		ASSERT( NULL != m_aListeners[i] );
		if ( m_aListeners[i] != NULL )
		{
			m_aListeners[i]->OnUserSelUploadResp(pResp);
		}
	}
}

void CUploadUserData::FireDownloadDataListener( const CMmiRespDownloadData *pResp )
{
	ASSERT( NULL != pResp );
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		ASSERT( NULL != m_aListeners[i] );
		if ( m_aListeners[i] != NULL )
		{
			m_aListeners[i]->OnUserSelDownloadResp(pResp);
		}
	}
}

bool32 CUploadUserData::Initialize( CViewData *pViewData )
{
	ASSERT( NULL == m_pViewData || m_pViewData == pViewData );	// ����ʼ��һ�� ���� ��ͬһ��������ʼ��(��һ����)
	if ( NULL == pViewData || NULL == pViewData->GetServiceDispose() )
	{
		ASSERT( 0 );
		return false;
	}
	// ����:
	//		���û��Զ�ʹ�÷����������滻���صĶ�Ӧ���
	//		�������й������ṩ ���ط�������ѡ���� �� �ϴ�x����������� ѡ��
	//		���湤����ʱ����ѡ�����ϴ�
	//const bool32 bNewUser = AfxGetDocument()->m_bNewUser;

	// ��ʼ��
	m_pViewData = pViewData;		// Ĭ���������ǳɹ���

	//if ( bNewUser )	// �����û�����Ĭ�ϵĳ�ʼ���滻����
	{
		// ��������ʹ�õ��Ǻϲ��������������û������Ի�ȡ���µ�
		return LoadDataFromViewData();
	}
	return true;
}


bool32 CUploadUserData::LoadDataFromViewData()
{
	if ( NULL == m_pViewData )
	{
		ASSERT( 0 );
		return false;
	}
	// ���²�����Ҫ�޸ģ��˽�����upload����
	const char *pContent = m_pViewData->GetServiceDispose()->GetCusoptional();
	//pContent = pViewData->GetServiceDispose()->GetPrivateData(EUDT_MobileUserSel);
	
	if ( NULL == pContent )
	{
		TRACE(_T("NULL�������ֻ���ѡ�ɣ�ʹ���ϴεı�����ѡ�����ã�\r\n"));
		return true;
	}
	
	const int32 iLen = strlen(pContent) + 1;
	char *pOrg = new char[iLen];
	if ( NULL == pOrg )
	{
		return false;
	}
	// base64 ����
	int32 iDecodeLen = iLen;
	bool32 bRet = CCodeFile::DeCodeString((char *)pContent, iLen-1, pOrg, &iDecodeLen, CCodeFile::ECFTBase64);	// �ִ�β��������
	ASSERT( bRet );

// 	CStdioFile file;
// 	file.Open(_T("d:\\user_mobile.dat"), CFile::modeCreate |CFile::modeWrite |CFile::typeBinary);
// 	file.Write(pOrg, iDecodeLen);
// 	file.Close();
// 	file.Open(_T("d:\\user_mobile_base64.txt"), CFile::modeCreate |CFile::modeWrite |CFile::typeBinary);
// 	file.Write(pContent, iLen-1);
// 	file.Close();
// 
// 	int32 iSize1 = sizeof(T_PhoneSymbol);
// 	TRACE(_T("T_PhoneSymbol: %d "), iSize1);
// 	iSize1 = sizeof(T_PhoneSymbolEx);
// 	TRACE(_T("T_PhoneSymbolEx: %d\n"), iSize1);
	
	if ( ReadUserBlockFromServer(pOrg, iDecodeLen) )
	{
		TRACE(_T("ʹ�÷������ֻ���ѡ�������ļ���������ѡ���ļ�������������\r\n"));
	}
	else
	{
		TRACE(_T("�������ֻ���ѡ�������ļ����ܽ�����ʹ���ϴεı�����ѡ�����ã�\r\n"));
	}
	
	delete[] pOrg;
	pOrg = NULL;
	
	return true;
}

void	PhoneSymbolExToMerchData(IN const T_PhoneSymbolEx &symbolEx, OUT int32 &iMarketId, OUT CString &StrCode)
{
	// java�ڲ�ʹ�õ��������ֽ��� bigendian
	iMarketId = ntohs(symbolEx.uMarket);
	char szCode[MAX_CODE+1];
	memcpyex(szCode, symbolEx.aCode, MAX_CODE);
	szCode[MAX_CODE] = '\0';
	StrCode = szCode;
}

void	MerchDataToPhoneSymbolEx(IN CMerch *pMerch, OUT T_PhoneSymbolEx &symbolEx)
{
	
	if ( NULL == pMerch )
	{
		return;
	}
	symbolEx.uMarket = (u16)pMerch->m_MerchInfo.m_iMarketId;
	symbolEx.uMarket = htons(symbolEx.uMarket);		// �����ֽ���
	std::string strCodeA;
	Unicode2MultiChar(CP_ACP, pMerch->m_MerchInfo.m_StrMerchCode, strCodeA);
	strncpy((char *)symbolEx.aCode, strCodeA.c_str(), MAX_CODE);
	
	// nameΪUnicode����
	_tcsncpy((TCHAR *)symbolEx.aName, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), MAX_NAME/sizeof(TCHAR));
	
	// tradecodeΪcode
	strncpy((char *)symbolEx.aTradeCode, strCodeA.c_str(), MAX_CODE);
	
	symbolEx.iDec = (u8)pMerch->m_MerchInfo.m_iSaveDec;
}

bool32 CUploadUserData::ReadUserBlockFromServer( const char *pContent, int nContentLen )
{
	if ( NULL == pContent || nContentLen < 1 || NULL == m_pViewData )
	{
		ASSERT( 0 );
		return false;
	}

// 	CXmlParser xml;
// 	bool bSuc = xml.ParserData(pContent);
// 
// 	if (!bSuc)
// 	{
// 		TRACE(_T("����XMLʧ�ܣ�\r\n"));
// 		return false;
// 	}
// 
// 	const TiXmlElement *pRoot = xml.GetRootNode();
// 
// 	const TiXmlElement *pSelData = xml.FindSubNode(pRoot, KE_SelData);
// 	
// 	ParseUserData(pSelData);
	
	return true;
}

int CUploadUserData::ParseUserData(const TiXmlElement * pEle)
{
// 	CViewData::MerchArray aMerchs;
// 
// 	if (NULL == pEle)
// 	{
// 		return 0;
// 	}
// 	CXmlParser xml;	
// 	CNodeList List;
// 	
// 	USES_CONVERSION;
// 
// 	xml.FindSubNodeList(pEle, List, KE_Sel);
// 	
// 	for(int i=0; i<List.size(); ++i)
// 	{
// 		const TiXmlElement * pUserData = List[i];
// 		
// 		CMerch *pMerch = NULL;
// 		int32 iMarketId = atoi(xml.GetNodeAttribute(pUserData, KA_MarketId));
// 		CString StrCode = _A2W(xml.GetNodeAttribute(pUserData, KA_StockCode));
// 		if ( m_pViewData->m_MerchManager.FindMerch(StrCode, iMarketId, pMerch) )
// 		{
// 			aMerchs.Add(pMerch);
// 		}
// 		else
// 		{
// 			TRACE(_T("��������ѡ�ɺ�����Ʒ: %d - %s\r\n"), iMarketId, StrCode);
// 		}
// 	}
// 
// 	// ���û��Ĭ�ϵķ�������ѡ��飬���½��������޸�
// 	T_Block block;
// 	T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
// 	if ( NULL == pBlockServer )
// 	{
// 		block.m_eHeadType = ERTCustom;
// 		block.m_bServerBlock = true;		// �������йصİ��
// 		block.m_StrName	  = CUserBlockManager::GetDefaultServerBlockName();
// 		block.m_StrHotKey = ConvertHZToPY(block.m_StrName);
// 		bool32 b = CUserBlockManager::Instance()->AddUserBlock(block);
// 		ASSERT( b );
// 		bool32 bAdded = false;
// 		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
// 		{
// 			bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
// 			bAdded = bAdded || bAdd2;
// 		}
// 		if ( bAdded )
// 		{
// 			// ��Ʒ���ʱ��, �������, ֻ֪ͨ�ͱ����ļ�һ��.
// 			CUserBlockManager::Instance()->SaveXmlFile();
// 			CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
// 		}
// 	}
// 	else
// 	{
// 		block = *pBlockServer;
// 		// ��Ʒ�б�ϲ�����ȡ�ޣ���ʱȡ�ϲ�
// 		bool32 bAdded = false;
// 		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
// 		{
// 			bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
// 			bAdded = bAdded || bAdd2;
// 		}
// 		if ( bAdded )
// 		{
// 			// ��Ʒ���ʱ��, �������, ֻ֪ͨ�ͱ����ļ�һ��.
// 			CUserBlockManager::Instance()->SaveXmlFile();
// 			CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
// 		}
// 	}

	return true;
}

bool32 CUploadUserData::ReqUploadData( const CMmiReqUploadData &uploadData )
{
	if ( NULL != m_pViewData )
	{
		m_pViewData->RequestViewData((CMmiReqUploadData *)&uploadData);
		return true;
	}
	ASSERT( 0 );
	return false;
}

void CUploadUserData::OnRespUploadData( const CMmiRespUploadData *pResp )
{
	TRACE(_T("Upload data ok\r\n"));
	FireUploadDataListener(pResp);	// ֪ͨ
}

bool32 CUploadUserData::ReqDownloadData( const CMmiReqDownloadData &downloadData )
{
	if ( NULL != m_pViewData )
	{
		m_pViewData->RequestViewData((CMmiReqDownloadData *)&downloadData);
		return true;
	}
	ASSERT( 0 );
	return false;
}

void CUploadUserData::OnRespDownloadData( const CMmiRespDownloadData *pResp )
{
	if ( NULL == pResp )
	{
		ASSERT(0);
		return;
	}
	//
	if (0  == pResp->m_uLen || NULL == pResp->m_pData )
	{
		return;
	}

	const char *pContent = pResp->m_pData;
	
	if ( NULL == pContent )
	{
		TRACE(_T("NULL�������ֻ���ѡ�ɣ�ʹ���ϴεı�����ѡ�����ã�\r\n"));
		return ;
	}

	const int32 iLen = strlen(pContent) + 1;
	char *pOrgData = new char[iLen];
	if ( NULL == pOrgData )
	{
		return ;
	}
	// base64 ����
	int32 iDecodeLen = iLen;
	bool32 bRet = CCodeFile::DeCodeString((char *)pContent, iLen-1, pOrgData, &iDecodeLen, CCodeFile::ECFTBase64);	// �ִ�β��������
	ASSERT( bRet );

	if ( ReadUserBlockFromServer(pOrgData, iDecodeLen) )
	{
		TRACE(_T("ʹ�÷������ֻ���ѡ�������ļ���������ѡ���ļ�������������\r\n"));
	}
	else
	{
		TRACE(_T("�������ֻ���ѡ�������ļ����ܽ�����ʹ���ϴεı�����ѡ�����ã�\r\n"));
	}
	
	FireDownloadDataListener(pResp);

	DEL_ARRAY(pOrgData) ;
}

bool32 CUploadUserData::UploadUserBlock()
{
	//USES_CONVERSION;
	// ��ȡ��������صİ�� ����������ذ������ ��ɶ�Ӧ��ʽ base64���ϴ�
	T_Block *pServerBlock = CUserBlockManager::Instance()->GetServerBlock();
	if ( NULL == pServerBlock )
	{
		return false;
	}

	CMmiReqUploadData req;
	req.m_uType = EUDT_MobileUserSel;

	// ���Ƹ�ʽ����
	const int32 iMerchCount = pServerBlock->m_aMerchs.GetSize();
	
	if ( iMerchCount < 1 )
	{
		if (IDCANCEL == AfxMessageBox(L"��ǰ��ѡ���б�Ϊ�գ�����շ��������������ѡ�ɣ�\r\n��ȷ���Ƿ��ϴ�?" ,MB_OKCANCEL | MB_ICONEXCLAMATION))
		{
			return true;
		}
		// ����Ҫ�����ˣ�ֱ�Ӵ�0��������
		req.m_uLen = 1;
		req.m_pData = new char[1];	// Ҫ��Ҫ�����أ�
		req.m_pData[0] = '\0';
	}
	else
	{
		
		TiXmlDocument *pXmlDoc = new TiXmlDocument();
		if (NULL == pXmlDoc)
		{
			return false;
		}
		
		// XML �İ汾��
		TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "utf-8","yes");
		if (NULL == pDeclaration)
		{
			return false;
		}
		pXmlDoc->LinkEndChild(pDeclaration);

		TiXmlElement *pRootEle = new TiXmlElement("XMLDATA");
		if (NULL == pRootEle)
		{
			return false;
		}
		pXmlDoc->LinkEndChild(pRootEle);
		
		// SELDATA
		TiXmlElement *pSelDataEle = new TiXmlElement(KE_SelData);
		if (NULL == pSelDataEle)
		{
			return false;
		}
		pRootEle->LinkEndChild(pSelDataEle);
		for ( int32 i=0; i < iMerchCount ; i++ )
		{
			CMerch *pMerch = pServerBlock->m_aMerchs[i];
			if ( NULL == pMerch )
			{
				continue;
			}
			TiXmlElement *pSelEle = new TiXmlElement(KE_Sel);
			ASSERT(NULL != pSelEle);
			pSelDataEle->LinkEndChild(pSelEle);
			
			// mk
			pSelEle->SetAttribute(KA_MarketId, pMerch->m_MerchInfo.m_iMarketId);
			string strMerchCode ,strMerchCnName;
			UnicodeToUtf8(pMerch->m_MerchInfo.m_StrMerchCode, strMerchCode);
			// cd
			pSelEle->SetAttribute(KA_StockCode, strMerchCode.c_str());
			// nm
			UnicodeToUtf8(pMerch->m_MerchInfo.m_StrMerchCnName, strMerchCnName);
			pSelEle->SetAttribute(KA_StockName, strMerchCnName.c_str());
			// tc
			pSelEle->SetAttribute(KA_TradeCode, strMerchCode.c_str());
			// dc
			pSelEle->SetAttribute(KA_Dec, pMerch->m_MerchInfo.m_iSaveDec);
			// ti
			pSelEle->SetAttribute(KA_TradeTimeId, pMerch->m_MerchInfo.m_iTradeTimeID);
			// tt
			pSelEle->SetAttribute(KA_TradeTimeType, pMerch->m_MerchInfo.m_iTradeTimeType);
		}
		
		TiXmlPrinter printer;  
		printer.SetIndent("\t");  
		pXmlDoc->Accept( &printer );  
		 
		int iDataSize = printer.Size() + 1;  
		  
		char *pData = new char[iDataSize];
		memset(pData, 0, iDataSize);

		memcpyex( pData, printer.CStr(), printer.Size() ); 
	
		DEL(pXmlDoc);
// 		if (NULL != pData)
// 		{
// 			// ��ת��д���ļ�������
// 			memset(pData, 0, iDataSize);
// 			Unicode2MultiCharCoding(EMCCUtf8, StrXml, StrXml.GetLength(), pData, iDataSize);
// 		}
		
		int32 iLenEncode = (iDataSize/3 + 2)*4;
		char *pDataEncode = new char[iLenEncode+2];		// req�����ͷ�
		bool32 b = CCodeFile::EnCodeString((char *)pData, iDataSize, pDataEncode, &iLenEncode, CCodeFile::ECFTBase64);
		ASSERT( b );
		pDataEncode[iLenEncode] = '\0';		// �ִ�������

		DEL_ARRAY(pData);
	
		req.m_pData = pDataEncode;
		req.m_uLen = iLenEncode+1;

		int32 iLen = strlen(req.m_pData);
		ASSERT( iLen == iLenEncode );
	}
	
	return ReqUploadData(req);
}

bool32 CUploadUserData::DownloadUserBlock()
{
	CMmiReqDownloadData req;
	req.m_uDataType = EUDT_MobileUserSel;
	req.m_uReserve[0] = 0;
	
	return ReqDownloadData(req);
}


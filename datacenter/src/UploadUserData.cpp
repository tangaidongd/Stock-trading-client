#include "StdAfx.h"
#include "coding.h"
#include "UploadUserData.h"

#include "viewdata.h"
#include "BlockManager.h"
#include "proxy_auth_client_base.h"
#include "CCodeFile.h"



static const int MAX_CODE = 8;	// 商品代码最大长度
static const int MAX_NAME = 16;	// 商品名字最大长度

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
#pragma pack(1)		// 不能编译器对齐
typedef struct _PhoneSymbol
{
	u16	uMarket;				// 市场
	u8	aCode[MAX_CODE];		// 代码
}T_PhoneSymbol;


typedef struct _PhoneSymbolEx : public _PhoneSymbol
{
	u8	aName[MAX_NAME];		// 名字, 以协议编码为编码
	u8	aTradeCode[MAX_CODE];	// 合约号 (客户端不用关心)
	u8	iDec;					// 小数点位数
}T_PhoneSymbolEx;
#pragma pack(pop)


CUploadUserDataListener::~CUploadUserDataListener()
{
	CUploadUserData::instance()->RemoveRespListener(this);		// 保险防止没有即时删除
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
	ASSERT( NULL == m_pViewData || m_pViewData == pViewData );	// 仅初始化一次 或者 用同一个反复初始化(就一个哈)
	if ( NULL == pViewData || NULL == pViewData->GetServiceDispose() )
	{
		ASSERT( 0 );
		return false;
	}
	// 规则:
	//		新用户自动使用服务器数据替换本地的对应板块
	//		程序运行过程中提供 下载服务器自选数据 与 上传x板块至服务器 选项
	//		保存工作区时将自选数据上传
	//const bool32 bNewUser = AfxGetDocument()->m_bNewUser;

	// 初始化
	m_pViewData = pViewData;		// 默认以下总是成功的

	//if ( bNewUser )	// 仅新用户进行默认的初始化替换数据
	{
		// 由于现在使用的是合并法则，所以所有用户都可以获取最新的
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
	// 以下策略需要修改，此仅测试upload功能
	const char *pContent = m_pViewData->GetServiceDispose()->GetCusoptional();
	//pContent = pViewData->GetServiceDispose()->GetPrivateData(EUDT_MobileUserSel);
	
	if ( NULL == pContent )
	{
		TRACE(_T("NULL服务器手机自选股，使用上次的本地自选股配置！\r\n"));
		return true;
	}
	
	const int32 iLen = strlen(pContent) + 1;
	char *pOrg = new char[iLen];
	if ( NULL == pOrg )
	{
		return false;
	}
	// base64 解码
	int32 iDecodeLen = iLen;
	bool32 bRet = CCodeFile::DeCodeString((char *)pContent, iLen-1, pOrg, &iDecodeLen, CCodeFile::ECFTBase64);	// 字串尾部不计算
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
		TRACE(_T("使用服务器手机自选股配置文件，本地自选股文件被覆盖抛弃！\r\n"));
	}
	else
	{
		TRACE(_T("服务器手机自选股配置文件不能解析，使用上次的本地自选股配置！\r\n"));
	}
	
	delete[] pOrg;
	pOrg = NULL;
	
	return true;
}

void	PhoneSymbolExToMerchData(IN const T_PhoneSymbolEx &symbolEx, OUT int32 &iMarketId, OUT CString &StrCode)
{
	// java内部使用的是网络字节序 bigendian
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
	symbolEx.uMarket = htons(symbolEx.uMarket);		// 网络字节序
	std::string strCodeA;
	Unicode2MultiChar(CP_ACP, pMerch->m_MerchInfo.m_StrMerchCode, strCodeA);
	strncpy((char *)symbolEx.aCode, strCodeA.c_str(), MAX_CODE);
	
	// name为Unicode编码
	_tcsncpy((TCHAR *)symbolEx.aName, pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), MAX_NAME/sizeof(TCHAR));
	
	// tradecode为code
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
// 		TRACE(_T("解析XML失败！\r\n"));
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
// 			TRACE(_T("服务器自选股忽略商品: %d - %s\r\n"), iMarketId, StrCode);
// 		}
// 	}
// 
// 	// 如果没有默认的服务器自选板块，则新建，有则修改
// 	T_Block block;
// 	T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
// 	if ( NULL == pBlockServer )
// 	{
// 		block.m_eHeadType = ERTCustom;
// 		block.m_bServerBlock = true;		// 服务器有关的板块
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
// 			// 商品多的时候, 提高性能, 只通知和保存文件一次.
// 			CUserBlockManager::Instance()->SaveXmlFile();
// 			CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
// 		}
// 	}
// 	else
// 	{
// 		block = *pBlockServer;
// 		// 商品列表合并还是取缔，暂时取合并
// 		bool32 bAdded = false;
// 		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
// 		{
// 			bool32 bAdd2 = CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
// 			bAdded = bAdded || bAdd2;
// 		}
// 		if ( bAdded )
// 		{
// 			// 商品多的时候, 提高性能, 只通知和保存文件一次.
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
	FireUploadDataListener(pResp);	// 通知
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
		TRACE(_T("NULL服务器手机自选股，使用上次的本地自选股配置！\r\n"));
		return ;
	}

	const int32 iLen = strlen(pContent) + 1;
	char *pOrgData = new char[iLen];
	if ( NULL == pOrgData )
	{
		return ;
	}
	// base64 解码
	int32 iDecodeLen = iLen;
	bool32 bRet = CCodeFile::DeCodeString((char *)pContent, iLen-1, pOrgData, &iDecodeLen, CCodeFile::ECFTBase64);	// 字串尾部不计算
	ASSERT( bRet );

	if ( ReadUserBlockFromServer(pOrgData, iDecodeLen) )
	{
		TRACE(_T("使用服务器手机自选股配置文件，本地自选股文件被覆盖抛弃！\r\n"));
	}
	else
	{
		TRACE(_T("服务器手机自选股配置文件不能解析，使用上次的本地自选股配置！\r\n"));
	}
	
	FireDownloadDataListener(pResp);

	DEL_ARRAY(pOrgData) ;
}

bool32 CUploadUserData::UploadUserBlock()
{
	//USES_CONVERSION;
	// 获取服务器相关的板块 将服务器相关板块数据 组成对应格式 base64后上传
	T_Block *pServerBlock = CUserBlockManager::Instance()->GetServerBlock();
	if ( NULL == pServerBlock )
	{
		return false;
	}

	CMmiReqUploadData req;
	req.m_uType = EUDT_MobileUserSel;

	// 编制格式数据
	const int32 iMerchCount = pServerBlock->m_aMerchs.GetSize();
	
	if ( iMerchCount < 1 )
	{
		if (IDCANCEL == AfxMessageBox(L"当前自选股列表为空，将清空服务器所保存的自选股，\r\n请确认是否上传?" ,MB_OKCANCEL | MB_ICONEXCLAMATION))
		{
			return true;
		}
		// 不需要编码了，直接传0长度数据
		req.m_uLen = 1;
		req.m_pData = new char[1];	// 要不要申请呢？
		req.m_pData[0] = '\0';
	}
	else
	{
		
		TiXmlDocument *pXmlDoc = new TiXmlDocument();
		if (NULL == pXmlDoc)
		{
			return false;
		}
		
		// XML 的版本号
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
// 			// 先转换写入文件的内容
// 			memset(pData, 0, iDataSize);
// 			Unicode2MultiCharCoding(EMCCUtf8, StrXml, StrXml.GetLength(), pData, iDataSize);
// 		}
		
		int32 iLenEncode = (iDataSize/3 + 2)*4;
		char *pDataEncode = new char[iLenEncode+2];		// req析构释放
		bool32 b = CCodeFile::EnCodeString((char *)pData, iDataSize, pDataEncode, &iLenEncode, CCodeFile::ECFTBase64);
		ASSERT( b );
		pDataEncode[iLenEncode] = '\0';		// 字串话结束

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


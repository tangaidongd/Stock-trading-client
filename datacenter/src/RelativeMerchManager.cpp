#include "StdAfx.h"



#include "MerchManager.h"

#include "RelativeMerchManager.h"

///////////////////////////////////////////////
const char * KStrXmlFileDataValue						= "relative list";

const char * KStrRelativeElement						= "Relative";
const char * KStrRelativeElementAttrType				= "type";
const char * KStrRelativeElementAttrSrcCode				= "src_code";
const char * KStrRelativeElementAttrSrcBreed			= "src_market";
const char * KStrRelativeElementAttrDstCode				= "dst_code";
const char * KStrRelativeElementAttrDstBreed			= "dst_market";

// 相关种类关键字
const char * KStrRelativeElementAttrTypeValueWarrant	= "warrant";
const char * KStrRelativeElementAttrTypeValueAB			= "A/B";
const char * KStrRelativeElementAttrTypeValueAH			= "A/H";

////////////////////////////////////////////////////////////////////////
//
CRelativeMerch::CRelativeMerch()
{
	m_eVerifyState	= EVSNotVeriry;

	m_eRelativeType = ERTUnknown;
	
	m_pMerchSrc		= NULL;
	m_pMerchDst		= NULL;
}

CRelativeMerch::~CRelativeMerch()
{

}

bool32 CRelativeMerch::FromXml(const TiXmlElement *pElementRelativeMerch)
{
	if (NULL == pElementRelativeMerch)
		return false;

	// 先确定是不是relative项
	const char *pcElementValue = pElementRelativeMerch->Value();
	if (NULL == pcElementValue || strcmp(pcElementValue, KStrRelativeElement) != 0)
		return false;

	// 逐个字段取值
	const char *pcAttrValue = pElementRelativeMerch->Attribute(KStrRelativeElementAttrType);
	if (NULL == pcAttrValue)
		return false;

	if (strcmp(pcAttrValue, KStrRelativeElementAttrTypeValueWarrant) == 0)
	{
		m_eRelativeType = ERTWarrant;
	}
	else if (strcmp(pcAttrValue, KStrRelativeElementAttrTypeValueAB) == 0)
	{
		m_eRelativeType = ERTAB;
	}
	else if (strcmp(pcAttrValue, KStrRelativeElementAttrTypeValueAH) == 0)
	{
		m_eRelativeType = ERTAH;
	}
	else
	{
		// 不可识别的类型
		ASSERT(0);;	// zhangbo 0708 #测试时， 留意这种情况的发生
		return false;
	}

	//
	pcAttrValue = pElementRelativeMerch->Attribute(KStrRelativeElementAttrSrcCode);
	if (NULL == pcAttrValue)
		return false;

	m_StrSrcMerchCode = pcAttrValue;

	//
	pcAttrValue = pElementRelativeMerch->Attribute(KStrRelativeElementAttrSrcBreed);
	if (NULL == pcAttrValue)
		return false;

	m_iSrcBreedId = atoi(pcAttrValue);	

	//
	pcAttrValue = pElementRelativeMerch->Attribute(KStrRelativeElementAttrDstCode);
	if (NULL == pcAttrValue)
		return false;

	m_StrDstMerchCode = pcAttrValue;

	//
	pcAttrValue = pElementRelativeMerch->Attribute(KStrRelativeElementAttrDstBreed);
	if (NULL == pcAttrValue)
		return false;

	m_iDstBreedId = atoi(pcAttrValue);

	// 对商品代码做一些处理
	// zhangbo 0708 #bug, 这里对香港的商品做一下特别处理， 4位的升到5位
	// 这里指定2为香港市场， 虽然没错， 但是理论上应该有一个地方统一理解大市场编号才好
	if (2 == m_iSrcBreedId)
	{
		if (m_StrSrcMerchCode.GetLength() == 4)
		{
			m_StrSrcMerchCode = L"0" + m_StrSrcMerchCode;
		}
	}

	if (2 == m_iDstBreedId)
	{
		if (m_StrDstMerchCode.GetLength() == 4)
		{
			m_StrDstMerchCode = L"0" + m_StrDstMerchCode;
		}
	}

	return true;
}

void CRelativeMerch::Verfiy(CMerchManager &MerchManager)
{
	//
	m_eVerifyState = EVSNotVeriry;

	// 查找源商品对应的商品信息
	int32 iPosBreedSrc = -1;
	CBreed *pBreedSrc = NULL;
	if (!MerchManager.FindBreed(m_iSrcBreedId, iPosBreedSrc, pBreedSrc))
	{
		m_eVerifyState = EVSFail;
		return;
	}
	
	if (!pBreedSrc->FindMerch(m_StrSrcMerchCode, m_pMerchSrc))
	{
		m_eVerifyState = EVSFail;
		return;
	}

	// 查找目标商品对应的商品信息
	int32 iPosBreedDst = -1;
	CBreed *pBreedDst = NULL;
	if (!MerchManager.FindBreed(m_iDstBreedId, iPosBreedDst, pBreedDst))
	{
		m_eVerifyState = EVSFail;
		return;
	}

	if (!pBreedDst->FindMerch(m_StrDstMerchCode, m_pMerchDst))
	{
		m_eVerifyState = EVSFail;
		return;
	}
	
	// 必要时， 调整src/dst位置
	if (ERTWarrant == m_eRelativeType)
	{
		if ((ERTStockCn == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTWarrantCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType) || 
			(ERTStockHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTWarrantHk == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// 内地股票 + 内地权证 or 香港股票 + 香港权证
			// perfect!
			m_eVerifyState = EVSNormal;
			return;
		}
		else if ((ERTWarrantCn == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType) ||
			(ERTWarrantHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockHk == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// 内地权证 + 内地股票 or 香港权证 + 香港股票 
			m_eVerifyState = EVSExchange;
			return;
		}
		else
		{
			// 不合理的对应
			m_eVerifyState = EVSFail;
			return;
		}
	}
	else if (ERTAB == m_eRelativeType)
	{
		if (ERTStockCn != m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType ||
			ERTStockCn != m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType)
		{
			m_eVerifyState = EVSFail;
			return;
		}

		// 
		if (m_pMerchSrc->m_Market.m_Breed.m_iBreedId != m_pMerchDst->m_Market.m_Breed.m_iBreedId)
		{
			m_eVerifyState = EVSFail;
			return;
		}

		if (m_pMerchSrc->m_Market.m_MarketInfo.m_iMarketId == m_pMerchDst->m_Market.m_MarketInfo.m_iMarketId)
		{
			m_eVerifyState = EVSFail;
			return;
		}
		else if (m_pMerchSrc->m_Market.m_MarketInfo.m_iMarketId > m_pMerchDst->m_Market.m_MarketInfo.m_iMarketId)
		{
			// zhangbo 0708 #bug, 这里假定A股的市场代码一定比B股小， 虽然这个假定目前是没问题的， 但是理论上感觉不爽。。。
			m_eVerifyState = EVSExchange;
			return;
		}
		else
		{
			m_eVerifyState = EVSNormal;
			return;
		}
	}
	else if (ERTAH == m_eRelativeType)
	{
		if ((ERTStockCn == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockHk == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// 内地股票 + 香港股票
			// perfect!
			m_eVerifyState = EVSNormal;
			return;
		}
		else if ((ERTStockHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// 香港股票 + 内地股票
			// 交换
			m_eVerifyState = EVSExchange;
			return;
		}
		else
		{
			// 不合理的对应
			m_eVerifyState = EVSFail;
			return;
		}
	}
	else
	{
		ASSERT(0);;
	}

	m_eVerifyState = EVSFail;
}


bool32 CRelativeMerch::IsRelative(IN CMerch *pMerch, CMerchManager &MerchManager, int32 iRelativeTypes)
{
	if ((m_eRelativeType & iRelativeTypes) == 0)
		return false;

	if (EVSFail == m_eVerifyState)
		return false;
	
	// 还没验证过的， 看着有可能的， 现在验证
	if (EVSNotVeriry == m_eVerifyState)
	{
		if (m_StrSrcMerchCode.CompareNoCase(pMerch->m_MerchInfo.m_StrMerchCode) == 0 ||
			m_StrDstMerchCode.CompareNoCase(pMerch->m_MerchInfo.m_StrMerchCode) == 0)
		{
			Verfiy(MerchManager);
		}
	}

	// 验证完了， 再看是否满足条件
	if (EVSNormal == m_eVerifyState ||
		EVSExchange == m_eVerifyState)
	{
		if (NULL == m_pMerchSrc || NULL == m_pMerchDst)
		{
			ASSERT(0);;
			return false;
		}

		if (m_pMerchSrc == pMerch ||
			m_pMerchDst == pMerch)
		{
			return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////////////////
//
CRelativeMerchManager::CRelativeMerchManager()
{
}

CRelativeMerchManager::~CRelativeMerchManager()
{
	m_RelativeMerchList.RemoveAll();
}

bool32 CRelativeMerchManager::FromXml(const CString &StrFile, CArray<CRelativeMerch, CRelativeMerch&> &RelativeList)
{
	// 
	RelativeList.RemoveAll();

	// 先转码为单字节路径
	char acFilePath[MAX_PATH] = {0};
	memset(acFilePath, 0, sizeof(acFilePath));
	Unicode2MultiCharCoding(EMCCSystem, StrFile, StrFile.GetLength(), acFilePath, sizeof(acFilePath));

	// 使用tinyxml解析该文件
	TiXmlDocument XmlDocument(acFilePath);
	if (!XmlDocument.LoadFile())
		return false;
		
	TiXmlElement *pRootElement = XmlDocument.RootElement();
	if (NULL == pRootElement)
		return false;
	
	if (strcmp(GetXmlRootElement(), pRootElement->Value()) != 0)
		return false;
	
	// 判断是否有 app = "ggtong" 属性
	const char *pcAttrValue = pRootElement->Attribute(GetXmlRootElementAttrApp());
	if (NULL == pcAttrValue || strcmp(pcAttrValue, GetXmlRootElementAttrAppValue()) != 0)
		return false;

	// 判断是否有data = "relative list"属性
	pcAttrValue = pRootElement->Attribute(GetXmlRootElementAttrData());
	if (NULL == pcAttrValue || strcmp(pcAttrValue, KStrXmlFileDataValue) != 0)
		return false;

	// 总算确认了这个文件就是我想要的文件了 :)
	// 挨个读取相关商品信息
	TiXmlElement *pElementRelativeMerch = pRootElement->FirstChildElement();
	while (NULL != pElementRelativeMerch)
	{
		CRelativeMerch RelativeMerch;
		if (RelativeMerch.FromXml(pElementRelativeMerch))
		{
			RelativeList.Add(RelativeMerch);
		}

		pElementRelativeMerch = pElementRelativeMerch->NextSiblingElement();
	}
	
	return true;
}

void CRelativeMerchManager::FindRelativeMerchs(IN CMerch *pMerch, CMerchManager &MerchManager, int32 iRelativeTypes, CArray<CRelativeMerch, CRelativeMerch&> &RelativeList)
{
	// 
	RelativeList.RemoveAll();

	int32 i;
	for ( i = 0; i < m_RelativeMerchList.GetSize(); i++)
	{
		CRelativeMerch &RelativeMerch = m_RelativeMerchList[i];

		if (RelativeMerch.IsRelative(pMerch, MerchManager, iRelativeTypes))
		{
			// 添加一个
			RelativeList.Add(RelativeMerch);
		}
	}

	// 
	for (i = 0; i < m_HkWarrantList.GetSize(); i++)
	{
		CRelativeMerch &RelativeMerch = m_HkWarrantList[i];

		if (RelativeMerch.IsRelative(pMerch, MerchManager, iRelativeTypes))
		{
			// 添加一个
			RelativeList.Add(RelativeMerch);
		}
	}
}

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

// �������ؼ���
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

	// ��ȷ���ǲ���relative��
	const char *pcElementValue = pElementRelativeMerch->Value();
	if (NULL == pcElementValue || strcmp(pcElementValue, KStrRelativeElement) != 0)
		return false;

	// ����ֶ�ȡֵ
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
		// ����ʶ�������
		ASSERT(0);;	// zhangbo 0708 #����ʱ�� ������������ķ���
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

	// ����Ʒ������һЩ����
	// zhangbo 0708 #bug, �������۵���Ʒ��һ���ر��� 4λ������5λ
	// ����ָ��2Ϊ����г��� ��Ȼû�� ����������Ӧ����һ���ط�ͳһ�����г���Ųź�
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

	// ����Դ��Ʒ��Ӧ����Ʒ��Ϣ
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

	// ����Ŀ����Ʒ��Ӧ����Ʒ��Ϣ
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
	
	// ��Ҫʱ�� ����src/dstλ��
	if (ERTWarrant == m_eRelativeType)
	{
		if ((ERTStockCn == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTWarrantCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType) || 
			(ERTStockHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTWarrantHk == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// �ڵع�Ʊ + �ڵ�Ȩ֤ or ��۹�Ʊ + ���Ȩ֤
			// perfect!
			m_eVerifyState = EVSNormal;
			return;
		}
		else if ((ERTWarrantCn == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType) ||
			(ERTWarrantHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockHk == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// �ڵ�Ȩ֤ + �ڵع�Ʊ or ���Ȩ֤ + ��۹�Ʊ 
			m_eVerifyState = EVSExchange;
			return;
		}
		else
		{
			// ������Ķ�Ӧ
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
			// zhangbo 0708 #bug, ����ٶ�A�ɵ��г�����һ����B��С�� ��Ȼ����ٶ�Ŀǰ��û����ģ� ���������ϸо���ˬ������
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
			// �ڵع�Ʊ + ��۹�Ʊ
			// perfect!
			m_eVerifyState = EVSNormal;
			return;
		}
		else if ((ERTStockHk == m_pMerchSrc->m_Market.m_MarketInfo.m_eMarketReportType) && (ERTStockCn == m_pMerchDst->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			// ��۹�Ʊ + �ڵع�Ʊ
			// ����
			m_eVerifyState = EVSExchange;
			return;
		}
		else
		{
			// ������Ķ�Ӧ
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
	
	// ��û��֤���ģ� �����п��ܵģ� ������֤
	if (EVSNotVeriry == m_eVerifyState)
	{
		if (m_StrSrcMerchCode.CompareNoCase(pMerch->m_MerchInfo.m_StrMerchCode) == 0 ||
			m_StrDstMerchCode.CompareNoCase(pMerch->m_MerchInfo.m_StrMerchCode) == 0)
		{
			Verfiy(MerchManager);
		}
	}

	// ��֤���ˣ� �ٿ��Ƿ���������
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

	// ��ת��Ϊ���ֽ�·��
	char acFilePath[MAX_PATH] = {0};
	memset(acFilePath, 0, sizeof(acFilePath));
	Unicode2MultiCharCoding(EMCCSystem, StrFile, StrFile.GetLength(), acFilePath, sizeof(acFilePath));

	// ʹ��tinyxml�������ļ�
	TiXmlDocument XmlDocument(acFilePath);
	if (!XmlDocument.LoadFile())
		return false;
		
	TiXmlElement *pRootElement = XmlDocument.RootElement();
	if (NULL == pRootElement)
		return false;
	
	if (strcmp(GetXmlRootElement(), pRootElement->Value()) != 0)
		return false;
	
	// �ж��Ƿ��� app = "ggtong" ����
	const char *pcAttrValue = pRootElement->Attribute(GetXmlRootElementAttrApp());
	if (NULL == pcAttrValue || strcmp(pcAttrValue, GetXmlRootElementAttrAppValue()) != 0)
		return false;

	// �ж��Ƿ���data = "relative list"����
	pcAttrValue = pRootElement->Attribute(GetXmlRootElementAttrData());
	if (NULL == pcAttrValue || strcmp(pcAttrValue, KStrXmlFileDataValue) != 0)
		return false;

	// ����ȷ��������ļ���������Ҫ���ļ��� :)
	// ������ȡ�����Ʒ��Ϣ
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
			// ���һ��
			RelativeList.Add(RelativeMerch);
		}
	}

	// 
	for (i = 0; i < m_HkWarrantList.GetSize(); i++)
	{
		CRelativeMerch &RelativeMerch = m_HkWarrantList[i];

		if (RelativeMerch.IsRelative(pMerch, MerchManager, iRelativeTypes))
		{
			// ���һ��
			RelativeList.Add(RelativeMerch);
		}
	}
}

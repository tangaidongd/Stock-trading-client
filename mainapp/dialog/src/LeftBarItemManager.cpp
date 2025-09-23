#include "StdAfx.h"
#include "LeftBarItemManager.h"
#include "IoViewChooseStock.h"

#define INVALID_VALUE  (-1)

CLeftBarItemManager::CLeftBarItemManager()
{
	m_dwChooseStockStatus = INVALID_VALUE;
	
	InitMarketMap();
	InitItemMap();
}

CLeftBarItemManager & CLeftBarItemManager::Instance()
{
	static CLeftBarItemManager leftBarManager;
	
	return leftBarManager;
}
CLeftBarItemManager::~CLeftBarItemManager()
{

}

bool32 CLeftBarItemManager::InitItemMap()
{
	DWORD uNewStatus = CChooseStockState::Instance().GetCurrentChooseStockState();
	
	if (uNewStatus == m_dwChooseStockStatus && INVALID_VALUE != m_dwChooseStockStatus)
	{
		return false;
	}

	m_dwChooseStockStatus = uNewStatus;

	m_mapItemList.clear();
	vector<T_ButtonItem> vecItems;
	vecItems.clear();
	vecItems.push_back(T_ButtonItem(EBID_StockCN_HQBJ, L"���鱨��" ,L"����A��"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_JBCW, L"��������" ,L""));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_ZHPM, L"�ۺ�����" ,L"�ۺ�����"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_DGTL, L"���ͬ��" ,L"���ͬ��"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_ZJJM, L"�ʽ����" ,L"�ʽ����"));

	m_mapItemList.insert(std::make_pair(ELBT_StockCN, vecItems));

 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_StockHK_HQBJ, L"���鱨��" ,L"�۹�"));
 	vecItems.push_back(T_ButtonItem(EBID_StockHK_DGTL, L"���ͬ��" ,L"���ͬ��"));
 	m_mapItemList.insert(std::make_pair(ELBT_StockHK, vecItems));
 
 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_Future_HQBJ, L"���鱨��" ,L"�ڻ�"));
 	vecItems.push_back(T_ButtonItem(EBID_Future_DGTL, L"���ͬ��" ,L"���ͬ��"));
 	m_mapItemList.insert(std::make_pair(ELBT_Futures, vecItems));
 
 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_GlobalIndex_HQBJ, L"���鱨��" ,L"ȫ��ָ��"));
 	vecItems.push_back(T_ButtonItem(EBID_GlobalIndex_DGTL, L"���ͬ��" ,L"���ͬ��"));
 	m_mapItemList.insert(std::make_pair(ELBT_GlobalIndex, vecItems));
 
	vecItems.clear();
	vecItems.push_back(T_ButtonItem(EBID_QuoteForeign_HQBJ, L"���鱨��", L"����г�"));
	vecItems.push_back(T_ButtonItem(EBID_QuoteForeign_DGTL, L"���ͬ��", L"���ͬ��"));
	m_mapItemList.insert(std::make_pair(ELBT_QuoteForeign, vecItems));
 
	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_YJFX, L"һ������", L"", true,  L"׷����ͣ�����ٳɶ࣬�������������",uNewStatus & ECSS10));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_QYZS, L"����ֱ��", L"", true,L"������������׽�������������˵�Ʒ��",uNewStatus & ECSS11));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLXS, L"������ˮ", L"", true,L"����ϴ�̽��������ǵ�����ʱ��",uNewStatus & ECSS12));
	//	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLNT, L"��������", L"", true,L"���̴���һ��죬���������ʾ�����ռ�",uNewStatus & ECSS4));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_XZDF, L"��ׯ��", L"", true,L"������������춯����׼��������ʱ��",uNewStatus & ECSS9));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_HFFL, L"�ط����", L"", true,L"����Ϊ����������ͨ����Ѱ�ҳ��׻���",uNewStatus & ECSS8));
	//vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SHJD, L"�����۶�", L"", true,L"�����桢�����桢�ʽ��棬��λһ�徫ѡ��ʤ���Ƚ�����Ʒ��",uNewStatus & ECSS7));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock, vecItems));

 	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_YJFX, L"�ȵ�׷��", L"", true,  L"�����г����Ÿ�������ȵ��ֻ��е���ͷ����",uNewStatus & ECSS10));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_XZDF, L"������", L"", true,L"�ھ򱬷���ǿ��Ǳ������Ʒ�֣���ǰ����",uNewStatus & ECSS9));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLXS, L"��ͣԤ��", L"", true,L"������ۡ��������������أ�ֱ��ÿ����ͣƷ��",uNewStatus & ECSS12));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock1, vecItems));
	
	vecItems.clear(); 
//	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLNT, L"��������", L"", true,L"���̴���һ��죬���������ʾ�����ռ�",uNewStatus & ECSS4));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_QYZS, L"������ţ", L"", true,L"�����ݷ����ɼ����ֵ��ϵ���ڲ����л�ȡ����",uNewStatus & ECSS11));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLQF, L"˫�����", L"", true,L"�۸��ʽ�����л��������ֱ��������ص���������",uNewStatus & ECSS6));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_HFFL, L"���׷���ʦ", L"", true,L"����ר����Ƚ���г�����ѡ��ʤ��ţ��",uNewStatus & ECSS8));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock2, vecItems));

	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SHJD, L"CCTV���", L"", true,L"CCTV֤ȯ��Ѷ���߽������ľ�ѡ��Ʊ�أ�Ͷ�ʽ���������",uNewStatus & ECSS7));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock3, vecItems));

	vecItems.clear();
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_XLFD, L"���ƾѻ�", L"���ƾѻ�", true, L"רע���߽��ף������ƾѻ������鷢", false));				
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_CHGR, L"��ʤ���", L"��ʤ���", true, L"���մ󲨶λ��ᣬ�������ʹ��Ч������", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_XRZL, L"��������", L"��������", true, L"����ָ���ϣ�����ʤ�ʸ���", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_YYLJ, L"Ǯ��Ų��", L"Ǯ��Ų��", true, L"��ض�������仯����ʱ������ʾ", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_MSKK, L"������ţ", L"������ţ", true, L"�����Ӷ�һ�ۿɹۣ�����ƽ�־����ƿ�", false));				 
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_DXCP, L"���߲���", L"���߲���", true, L"ȫ�׶��߲�����ϵ,����ץ����", false));				 
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_ZXCP, L"���߲���", L"���߲���", true, L"�״��������ܱ�Ƶ��������,��Ȳ�׽����ţ��", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_QRFJ, L"ǿ���ֽ�", L"ǿ���ֽ�", true, L"�ƿ�ţ��,��Ϥ�г�ǿ��״��", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_JGCB, L"�����ɱ�", L"�����ɱ�", true, L"��ػ����ֲֳɱ�,���ٲ���ǿׯţ��", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_JDZB, L"���ָ��", L"���ָ��", true, L"�����ɫָ�꣬��ʱ���������źţ��ɹ��ʼ���", false));
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_SJHY, L"��ɱ����", L"��ɱ����", true, L"��Ϥ�����ʽ������ƾѻ�һ����ɱ", false)); 
	m_mapItemList.insert(std::make_pair(ELBT_StockDecision, vecItems));

	return true;
}

void CLeftBarItemManager::InitMarketMap()
{
// 	����A��
// 		m_mapMarketList.insert(std::make_pair( 1, ELBT_HUSHENSTOCK));		// ��֤A��
// 		m_mapMarketList.insert(std::make_pair( 2, ELBT_HUSHENSTOCK));		// ��֤B��
// 		m_mapMarketList.insert(std::make_pair( 1001, ELBT_HUSHENSTOCK));	// ��֤A��
// 		m_mapMarketList.insert(std::make_pair( 1002, ELBT_HUSHENSTOCK));	// ����B��
// 		m_mapMarketList.insert(std::make_pair( 1004, ELBT_HUSHENSTOCK));	// ��С��ҵ
// 		m_mapMarketList.insert(std::make_pair( 1008, ELBT_HUSHENSTOCK));	// ��ҵ��
// 		m_mapMarketList.insert(std::make_pair( 2000, ELBT_HUSHENSTOCK));	// ����A��
// 		m_mapMarketList.insert(std::make_pair( 2002, ELBT_HUSHENSTOCK));
// 		m_mapMarketList.insert(std::make_pair( 2003, ELBT_HUSHENSTOCK));	// ��֤A��
// 		m_mapMarketList.insert(std::make_pair( 2004, ELBT_HUSHENSTOCK));	// ��֤A��
// 		m_mapMarketList.insert(std::make_pair( 2005, ELBT_HUSHENSTOCK));	// ��С��ҵ
// 		m_mapMarketList.insert(std::make_pair( 2006, ELBT_HUSHENSTOCK));	// ��ҵ��
// 		m_mapMarketList.insert(std::make_pair( 2007, ELBT_HUSHENSTOCK));	// ����B��
// 		m_mapMarketList.insert(std::make_pair( 2001, ELBT_HUSHENSTOCK));	// ��֤B��
// 		m_mapMarketList.insert(std::make_pair( 2012, ELBT_HUSHENSTOCK));	// ��֤B��

}

void CLeftBarItemManager::GetItemsByType( E_LeftBarType eLeftBarType,OUT std::vector<T_ButtonItem>& vecItemInfo )
{
	vecItemInfo.clear();
	if(m_mapItemList.count(eLeftBarType) > 0)
	{
		vecItemInfo.assign(m_mapItemList[eLeftBarType].begin(), m_mapItemList[eLeftBarType].end());
	}
}

void CLeftBarItemManager::ChangeCheckStatus(const E_LeftBarType &eType, const int32 &iID, const CString &StrWspName)
{
	std::vector<T_ButtonItem>::iterator iter = m_mapItemList[eType].begin();

	for ( ; iter !=  m_mapItemList[eType].end(); ++iter)
	{
		if (iter->m_StrWspName == StrWspName )
		{
			if (iter->m_iBtnID == iID)
			{
				iter->m_bChecked = true;
			}
			else
			{
				iter->m_bChecked = false;
			}
		}
	}
}

void CLeftBarItemManager::ResetCheckStatus(const CString &StrWspName)
{
	map<E_LeftBarType, vector<T_ButtonItem>>::iterator iterMap = m_mapItemList.begin();

	for ( ; iterMap != m_mapItemList.end(); ++iterMap)
	{
		std::vector<T_ButtonItem>::iterator iterVect = iterMap->second.begin();

		for ( ; iterVect != iterMap->second.end(); ++iterVect)
		{
			if (iterVect->m_StrWspName == StrWspName)
			{
				iterVect->m_bChecked = true;
			}
		}
	}
}
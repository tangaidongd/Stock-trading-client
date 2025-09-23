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
	vecItems.push_back(T_ButtonItem(EBID_StockCN_HQBJ, L"行情报价" ,L"沪深A股"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_JBCW, L"基本财务" ,L""));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_ZHPM, L"综合排名" ,L"综合排名"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_DGTL, L"多股同列" ,L"多股同列"));
	vecItems.push_back(T_ButtonItem(EBID_StockCN_ZJJM, L"资金解密" ,L"资金解密"));

	m_mapItemList.insert(std::make_pair(ELBT_StockCN, vecItems));

 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_StockHK_HQBJ, L"行情报价" ,L"港股"));
 	vecItems.push_back(T_ButtonItem(EBID_StockHK_DGTL, L"多股同列" ,L"多股同列"));
 	m_mapItemList.insert(std::make_pair(ELBT_StockHK, vecItems));
 
 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_Future_HQBJ, L"行情报价" ,L"期货"));
 	vecItems.push_back(T_ButtonItem(EBID_Future_DGTL, L"多股同列" ,L"多股同列"));
 	m_mapItemList.insert(std::make_pair(ELBT_Futures, vecItems));
 
 	vecItems.clear();
 	vecItems.push_back(T_ButtonItem(EBID_GlobalIndex_HQBJ, L"行情报价" ,L"全球指数"));
 	vecItems.push_back(T_ButtonItem(EBID_GlobalIndex_DGTL, L"多股同列" ,L"多股同列"));
 	m_mapItemList.insert(std::make_pair(ELBT_GlobalIndex, vecItems));
 
	vecItems.clear();
	vecItems.push_back(T_ButtonItem(EBID_QuoteForeign_HQBJ, L"行情报价", L"外汇市场"));
	vecItems.push_back(T_ButtonItem(EBID_QuoteForeign_DGTL, L"多股同列", L"多股同列"));
	m_mapItemList.insert(std::make_pair(ELBT_QuoteForeign, vecItems));
 
	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_YJFX, L"一剑飞仙", L"", true,  L"追击涨停，积少成多，分享敢死队收益",uNewStatus & ECSS10));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_QYZS, L"青云直上", L"", true,L"暴力拉升，捕捉即将进入主升浪的品种",uNewStatus & ECSS11));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLXS, L"神龙吸水", L"", true,L"主力洗盘结束，正是低吸好时机",uNewStatus & ECSS12));
	//	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLNT, L"升龙逆天", L"", true,L"万绿丛中一点红，逆势飙升揭示上升空间",uNewStatus & ECSS4));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_XZDF, L"吸庄大法", L"", true,L"监控盘中主力异动，精准把握坐轿时机",uNewStatus & ECSS9));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_HFFL, L"回风拂柳", L"", true,L"波段为王，在上升通道中寻找抄底机会",uNewStatus & ECSS8));
	//vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SHJD, L"三花聚顶", L"", true,L"基本面、技术面、资金面，三位一体精选高胜率稳健收益品种",uNewStatus & ECSS7));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock, vecItems));

 	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_YJFX, L"热点追踪", L"", true,  L"跟踪市场热门概念，把握热点轮换中的龙头机会",uNewStatus & ECSS10));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_XZDF, L"主题掘金", L"", true,L"挖掘爆发力强的潜在主题品种，提前布局",uNewStatus & ECSS9));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLXS, L"涨停预测", L"", true,L"分析宏观、基本面利好因素，直击每日涨停品种",uNewStatus & ECSS12));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock1, vecItems));
	
	vecItems.clear(); 
//	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLNT, L"升龙逆天", L"", true,L"万绿丛中一点红，逆势飙升揭示上升空间",uNewStatus & ECSS4));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_QYZS, L"波段擒牛", L"", true,L"大数据分析股价与估值关系，在波动中获取收益",uNewStatus & ECSS11));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SLQF, L"双龙齐飞", L"", true,L"价格资金两面夹击，剖析分笔数据隐藏的主力机密",uNewStatus & ECSS6));
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_HFFL, L"靠谱分析师", L"", true,L"资深专家深度解读市场，优选高胜率牛股",uNewStatus & ECSS8));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock2, vecItems));

	vecItems.clear(); 
	vecItems.push_back(T_ButtonItem(EBID_SmartSelStock_SHJD, L"CCTV金股", L"", true,L"CCTV证券资讯在线教育中心精选股票池，投资教育两不误",uNewStatus & ECSS7));
	m_mapItemList.insert(std::make_pair(ELBT_SmartSelStock3, vecItems));

	vecItems.clear();
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_XLFD, L"王牌狙击", L"王牌狙击", true, L"专注短线交易，如王牌狙击例不虚发", false));				
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_CHGR, L"决胜金叉", L"决胜金叉", true, L"把握大波段机会，配合周线使用效果极佳", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_XRZL, L"海底捞月", L"海底捞月", true, L"两大指标结合，交易胜率更高", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_YYLJ, L"钱坤挪移", L"钱坤挪移", true, L"监控多空力量变化，即时操作提示", false));				 
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_MSKK, L"波段擒牛", L"波段擒牛", true, L"抄底逃顶一眼可观，开仓平仓尽在掌控", false));				 
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_DXCP, L"短线操盘", L"短线操盘", true, L"全套短线操盘体系,轻松抓波段", false));				 
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_ZXCP, L"中线操盘", L"中线操盘", true, L"首创中线智能变频分析技术,深度捕捉中线牛股", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_QRFJ, L"强弱分界", L"强弱分界", true, L"掌控牛熊,洞悉市场强弱状况", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_JGCB, L"机构成本", L"机构成本", true, L"监控机构持仓成本,快速布局强庄牛股", false));
//	vecItems.push_back(T_ButtonItem(EBID_StockDecision_JDZB, L"金盾指标", L"金盾指标", true, L"金盾特色指标，及时发出买卖信号，成功率极高", false));
	vecItems.push_back(T_ButtonItem(EBID_StockDecision_SJHY, L"猎杀主力", L"猎杀主力", true, L"洞悉主力资金动向，王牌狙击一击必杀", false)); 
	m_mapItemList.insert(std::make_pair(ELBT_StockDecision, vecItems));

	return true;
}

void CLeftBarItemManager::InitMarketMap()
{
// 	沪深A股
// 		m_mapMarketList.insert(std::make_pair( 1, ELBT_HUSHENSTOCK));		// 上证A股
// 		m_mapMarketList.insert(std::make_pair( 2, ELBT_HUSHENSTOCK));		// 上证B股
// 		m_mapMarketList.insert(std::make_pair( 1001, ELBT_HUSHENSTOCK));	// 深证A股
// 		m_mapMarketList.insert(std::make_pair( 1002, ELBT_HUSHENSTOCK));	// 深圳B股
// 		m_mapMarketList.insert(std::make_pair( 1004, ELBT_HUSHENSTOCK));	// 中小企业
// 		m_mapMarketList.insert(std::make_pair( 1008, ELBT_HUSHENSTOCK));	// 创业板
// 		m_mapMarketList.insert(std::make_pair( 2000, ELBT_HUSHENSTOCK));	// 沪深A股
// 		m_mapMarketList.insert(std::make_pair( 2002, ELBT_HUSHENSTOCK));
// 		m_mapMarketList.insert(std::make_pair( 2003, ELBT_HUSHENSTOCK));	// 上证A股
// 		m_mapMarketList.insert(std::make_pair( 2004, ELBT_HUSHENSTOCK));	// 深证A股
// 		m_mapMarketList.insert(std::make_pair( 2005, ELBT_HUSHENSTOCK));	// 中小企业
// 		m_mapMarketList.insert(std::make_pair( 2006, ELBT_HUSHENSTOCK));	// 创业板
// 		m_mapMarketList.insert(std::make_pair( 2007, ELBT_HUSHENSTOCK));	// 沪深B股
// 		m_mapMarketList.insert(std::make_pair( 2001, ELBT_HUSHENSTOCK));	// 上证B股
// 		m_mapMarketList.insert(std::make_pair( 2012, ELBT_HUSHENSTOCK));	// 深证B股

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
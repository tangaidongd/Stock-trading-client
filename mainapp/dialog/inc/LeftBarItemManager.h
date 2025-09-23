#ifndef _LEFT_BAR_MANAGER_H_
#define _LEFT_BAR_MANAGER_H_

#include <map>
#include <vector>

using std::map;
using std::vector;

// 侧边栏类型
enum E_LeftBarType
{
	ELBT_NONE,
	ELBT_StockCN,								 // 沪深A股   
	ELBT_StockHK,                                // 港股
	ELBT_Futures,                                // 期货
	ELBT_GlobalIndex,                            // 全球指数
	ELBT_QuoteForeign,                           // 外汇
	ELBT_SmartSelStock,                          //智能选股
	ELBT_SmartSelStock1,						 // 智能选股
	ELBT_SmartSelStock2,						 // 智能选股
	ELBT_SmartSelStock3,						 // 智能选股
	ELBT_StockDecision,							 // 个股决策
};

// 按钮图片类型
enum E_ImageType
{

	EIT_HQBJ,				    // 行情报价				
	EIT_JBCW,					// 基本财务
	EIT_ZHPM,				    // 综合排名
	EIT_DGTL,					// 多股同列
	EIT_ZJPM,					// 资金排名
	EIT_JGCC,					// 机构持仓
	EIT_LHBD,					// 龙虎榜单
	EIT_JYXW,					// 交易行为
	EIT_YJTS,					// 业绩透视
	EIT_XGIPO,					// 新股IPO
	EIT_RMBK,					// 热门板块
	EIT_CPTX,					// 操盘提醒
	EIT_RDWJ,					// 热点挖掘
	EIT_ZLJK,					// 主力监控
	EIT_TREND,					// 分时图
	EIT_KLINE,					// K线图
	EIT_CJMX,					// 成交明细
	EIT_FJB,					// 分价表
	EIT_ZJLX,					// 资金流向
	EIT_DZQT,					// 多周期图
	EIT_F10,					// F10
	EIT_GGZX,					// 个股资讯
	EIT_WXLH,					// 五行量化
	EIT_GGYJ,					// 个股预警
	EIT_GGCD,					// 个股抄底
	EIT_GGTD,					// 个股逃顶
	EIT_YJSD,					// 一箭三雕
};

enum E_BtnID
{
	// 沪深A股
	EBID_StockCN_HQBJ = 10000,                      // 行情报价
	EBID_StockCN_JBCW,							    // 基本财务
	EBID_StockCN_ZHPM,							    // 综合排名					
	EBID_StockCN_DGTL,								// 多股同列
	EBID_StockCN_ZJJM,								// 资金解密

	// 港股
	EBID_StockHK_HQBJ = 10100,	                    // 行情报价                			
	EBID_StockHK_DGTL,								// 多股同列

	// 期货
	EBID_Future_HQBJ = 10200,						// 行情报价
	EBID_Future_DGTL,								// 多股同列 

	// 全球指数
	EBID_GlobalIndex_HQBJ = 10400,					// 行情报价
	EBID_GlobalIndex_DGTL,							// 多股同列

	// 外汇
	EBID_QuoteForeign_HQBJ = 10500,					// 行情报价
	EBID_QuoteForeign_DGTL,							// 多股同列

	// 智能选股
	EBID_SmartSelStock_YJFX = 10600,				// 120-短线强势-一剑飞仙-热点追踪
	EBID_SmartSelStock_QYZS,						// 121-强者恒强-青云直上-波段擒牛
	EBID_SmartSelStock_SLXS,						// 122 波段操作-神龙吸水-涨停预测
	EBID_SmartSelStock_SLNT,		                // 升龙逆天
	EBID_SmartSelStock_XZDF,						// 119 震荡短买-吸庄大法-主题掘金
	EBID_SmartSelStock_HFFL,						// 118 空头陷阱-回风拂柳-靠谱分析师
	EBID_SmartSelStock_SHJD,						// 117 三花聚顶-CCTV金股
	EBID_SmartSelStock_SLQF,						// 116 回风拂柳-双龙齐飞

	// 个股决策
	EBID_StockDecision_XLFD = 10700,				// 小李飞刀 -王牌狙击
	EBID_StockDecision_CHGR,						// 长虹贯日 -决胜金叉
	EBID_StockDecision_XRZL,						// 仙人指路
	EBID_StockDecision_YYLJ,						// 阴阳两仪
	EBID_StockDecision_MSKK,						// 妙手空空
	EBID_StockDecision_SJHY,						// 三剑合一
	EBID_StockDecision_DXCP,						// 短线操盘
	EBID_StockDecision_ZXCP,						// 中线操盘
	EBID_StockDecision_QRFJ,						// 强弱分界
	EBID_StockDecision_JGCB,						// 机构成本
	EBID_StockDecision_JDZB,						// 金盾指标
};

struct T_ButtonItem
{
	CString m_StrName;
	int32   m_iBtnID;
	CString m_StrWspName;
	CString m_StrDesc;
	bool32  m_bHasResult;
	bool32  m_bChecked;

	T_ButtonItem(int32 iID, CString StrName, CString StrWspName, bool32 bChecked=true, CString StrDesc=L"", bool32 hasResult = false)
	{
		m_StrName = StrName;
		m_iBtnID  = iID;
		m_StrWspName = StrWspName;
		m_bHasResult = hasResult;
		m_StrDesc    = StrDesc;
		m_bChecked   = bChecked;
	}
};

class CLeftBarItemManager
{
public:
	static CLeftBarItemManager &Instance();
protected:
	CLeftBarItemManager();
	~CLeftBarItemManager();

public:
	bool32 InitItemMap();
	void   InitMarketMap();
	
public:
	void GetItemsByType(E_LeftBarType eLeftBarType,OUT vector<T_ButtonItem>& vecItemInfo);
	void ChangeCheckStatus(const E_LeftBarType &eType, const int32 &iID, const CString &StrWspName);
	// 重置状态
	void ResetCheckStatus(const CString &StrWspName);
    
private:
	map<E_LeftBarType, vector<T_ButtonItem>> m_mapItemList;   // 侧边栏按钮集合
	map<E_ImageType, Image*> m_mapImageList;                  // 侧边栏按钮图片集合

public:
	map<int32, E_LeftBarType> m_mapMarketList;				  // 市场集合
private:
	DWORD                     m_dwChooseStockStatus;          // 选股状态
};

#endif
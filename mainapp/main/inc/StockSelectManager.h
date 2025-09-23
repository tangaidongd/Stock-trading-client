#ifndef _STOCK_SELECT_MANAGER_H_
#define _STOCK_SELECT_MANAGER_H_

#include "MerchManager.h"

class CStockSelectTypeChanged
{
public:
	CStockSelectTypeChanged();		// 自动注册与解除
	virtual ~CStockSelectTypeChanged();

	virtual	void	OnStockSelectTypeChanged() = 0;	// 程序改变了选股类型，视图需要重新根据现在选择的类型发送请求
};

// 选股数据管理 处理选股的标志和权限
class CStockSelectManager
{
public:
	enum E_IndexSelectStock		// 各选股条件只能取check按钮模式并选，实际结果进行筛选 所有选项都有的商品才算选中
	{
		ShortLine		= 0x0001,	// 短线选股
		MidLine			= 0x0002,	// 中线选股
		BuySelect = 4,				// 买入精选
 		
		StrategySelectBegin,
		StrategyJGLT = 0x8,		    // 机构拉抬
		StrategyXPJS = 0x10,		// 主力洗盘
		StrategyYJZT = 0x20,		// 游击涨停
		StrategyNSQL = 0x40,		// 抗跌强庄
		StrategyCDFT = 0x80,		// 超跌反弹
		StrategySLXS = 0x100,	    // 神龙吸水
		StrategyHFFL = 0x200,	    // 回风拂柳
		StrategySHJD = 0x400,	    // 三花聚顶
		StrategyKTXJ = 0x800,		// 空头陷阱策略
		StrategyZDSD = 0x1000,		// 震荡市短买策略
		StrategyDXQS = 0x2000,		// 短线强势策略
		StrategyQZHQ = 0x4000,	    // 强者恒强策略
		StrategyBDCZ = 0x8000,	    // 波段操作策略
		StrategySelectEnd,

		IndexStrategySelectStart,
 		IndexStrategy  = 0x10000,  // 战略选股
 		RedStrategy    = 0x20000  | IndexStrategy,	// 大盘红/绿/蓝/黄/暴跌策略
 		GreenStrategy  = 0x40000  | IndexStrategy,
 		BlueStrategy   = 0x80000  | IndexStrategy,
 		YellowStrategy = 0x100000 | IndexStrategy,
 		FallStrategy   = 0x200000 | IndexStrategy,
		IndexStrategySelectEnd,

		EISS_End,
 	};
	
	enum E_HotSelectStock
	{
		HotTimeSale = 1,		// 大单强势 热门强势 资金强势 并关系 - 结果筛选模式
		HotForce = 2,
		HotCapital = 4,
	};
	
	enum{
		OpenStockPool = 0xFFFF,			// 注意保持打开股票池
	};
	
	enum E_CurrentSelect
	{
		IndexShortMidSelect,			// 短线 中线 ... 请求
		HotSelect,						// 热门强势请求
	};
	
	static	CStockSelectManager &Instance();
	virtual ~CStockSelectManager();
	
	
	DWORD	GetIndexSelectStockFlag() const;
	// 	DWORD	GetStrategySelectStockFlag() const  { return m_dwStrategySelectStockFlag; }
	// 	DWORD	GetIndexStrategySelectFlag() const { return m_dwIndexStrategySelectFlag; }
	DWORD	GetHotSelectStockFlag() const { return m_dwHotSelectStockFlag; }
	
	bool32	SetIndexSelectStockFlag(DWORD dwIndexFlag, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);
	// 	void	SetStrategySelectStockFlag(DWORD dwStrategyFlag) { m_dwStrategySelectStockFlag = dwStrategyFlag; }
	// 	void	SetIndexStrategySelectFlag(DWORD dwIndexFlag) { m_dwIndexStrategySelectFlag = dwIndexFlag; }
	bool32	SetHotSelectStockFlag(DWORD dwHotFlag, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);
	
	void	SetCurrentReqType(E_CurrentSelect eSel, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);		// 设置当前选股请求类型, 通知发出对应请求
	// 应当在调用此函数前先设置好请求参数
	E_CurrentSelect	GetCurrentReqType() const { return m_eCurrentSelect; }
	
	bool32	IsCurrentReqUnderUserRight(int *pRightDeny = NULL);		// 当前选股条件是否在用户权限允许下
	bool32  IsCurrentReqUnderUserRight(E_CurrentSelect eSelectType, int *pRightDeny = NULL);
	
	void	PromptLackRight(int iRightDeny);

	void	AddListener(CStockSelectTypeChanged *pListener);
	void	RemoveListener(CStockSelectTypeChanged *pListener);
	
	static bool32 IsDefaultReqUnderUserRight(int *pRightDeny = NULL);	// 默认选项是否在用户权限下
	static bool32 IsReqUnderUserRight(E_CurrentSelect eSelectType, DWORD dwFlag, int *pRightDeny = NULL);		// 指定选项
	
	static bool32 GetIndexSelectStockEnumByCmdId(UINT nId, OUT E_IndexSelectStock &eJGXG);

private:
	CStockSelectManager();

	// 不实现赋值
	CStockSelectManager(const CStockSelectManager &);
	CStockSelectManager &operator=(const CStockSelectManager &);

	void	FireChanged();

	DWORD	m_dwIndexSelectStockFlag;		// 短中  // 战略 大盘一起
	// 	DWORD	m_dwStrategySelectStockFlag;	// 战略
	// 	DWORD	m_dwIndexStrategySelectFlag;	// 大盘 - 与牛熊有关
	DWORD	m_dwHotSelectStockFlag;			// 热门强势
	
	
	E_CurrentSelect			m_eCurrentSelect;

	CArray<CStockSelectTypeChanged *, CStockSelectTypeChanged *> m_aListeners;
};


#endif //_STOCK_SELECT_MANAGER_H_
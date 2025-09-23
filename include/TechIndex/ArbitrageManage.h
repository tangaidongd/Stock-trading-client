// ArbitrageManage.h: interface for the CArbitrageManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ARBITRAGEMANAGE_H_)
#define _ARBITRAGEMANAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <set>
#include <map>
#include <vector>
#include "AlarmNotify.h"
#include "ShareFun.h"
#include "TechExport.h"
#include "EngineCenterBase.h"


using namespace std;

class CAlarmCenter;
//class CMerch;
//class CViewData;
class TiXmlElement;
class CArbitrageFormula;
class CArbitrageFormulaVariableVar;

// 套利类型
typedef enum E_Arbitrage
{
	EABTwoMerch = 0,		// 两腿, 两个商品
	EABThreeMerch,			// 蝶式, 三个商品

	EABCount,
		
}E_Arbitrage;

// 价差类型
typedef enum E_ArbPrice
{
	EAPDiv = 0,				// 除法
	EAPDiff,				// 减法

	EAPCount,				
		
}E_ArbPrice;

// 套利报价表头类型
typedef enum E_ArbitrageHead
{
	EAHName = 0,		// 名称
	EAHPriceDifAccord,	// 对价价差
	EAHPriceDifQuote,	// 报价价差
	EAHPriceDifNew,		// 最新价差
	EAHRisePriceDif,	// 价差涨跌
	EAHPriceMerchA,		// 商品 A 最新价
	EAHPriceMerchB,		// 商品 B 最新价
	EAHPriceMerchC,		// 商品 C 最新价
	EAHPriceDifOpen,	// 开盘价差
	EAHPriceDifPreClose,// 昨收价差
	EAHPriceDifPreAvg,	// 昨结价差
	EAHPriceDifAvg,		// 结算价差

	EAHCount,		
		
}E_ArbitrageHead;

class CArbitrageManage;

// 套利结构
class EXPORT_CLASS CArbitrage
{
public:
	typedef vector<CArbitrageFormulaVariableVar> ArbVarArray;
	typedef vector<CMerch *>	ArbMerchArray;

	typedef struct T_ArbMerch
	{
		CMerch				*m_pMerch;
		ArbMerchArray		m_aOtherMerch;				// 其它插入商品，可能是任何商品
		CArbitrageFormula	*m_pFormular;				// 初始化NULL，深层拷贝，析构delete

		T_ArbMerch()
		{
			m_pMerch = NULL;
			m_pFormular = NULL;
		}
		~T_ArbMerch();
		T_ArbMerch(const T_ArbMerch &other);
		const T_ArbMerch &operator=(const T_ArbMerch &other);

		bool operator==(const T_ArbMerch &other) const;
		bool operator!=(const T_ArbMerch &other) const { return !(*this==other); }
		bool operator<(const T_ArbMerch &other) const;

		void Copy(const T_ArbMerch &other);
	}T_ArbMerch;

public:
	CArbitrage();
	~CArbitrage();

	bool operator==(const CArbitrage& stOther) const;
	bool operator<(const CArbitrage& stOther) const;
	CArbitrage& operator=(const CArbitrage& stOther);
	CArbitrage(const CArbitrage& stOther);

	// 读数据
	bool32	FromXml(TiXmlElement* pElement,CAbsCenterManager* pData);
	bool32	LoadMerchFromXml(TiXmlElement *pElement, OUT T_ArbMerch &arbMerch,CAbsCenterManager* pData);
	
	// 保存文件
	CString ToXml();
	CString SaveMerchToXml(const T_ArbMerch &arbMerch) const;

	// 取这个结构界面显示的名称
	CString	GetShowName() const;

	// 取各种价差
	float	GetPrice(E_ArbitrageHead eHeadType) const;

	// 算价格
	float	FormularPrice(float fPriceOriginal, CArbitrageFormula* pFormular, CMerch *pMerchToCalc, E_ArbitrageHead eHead) const;
	// 计算K线
	//  varResult - OUT，计算的结果集
	//  varIn - IN, 主商品的K线参数
	//  pFormula - IN, 公式
	//  pMerchToCalc - IN, 主商品
	//  aOtherVars - IN, 插入商品的K线参数, 这些变量数组中的每一个变量必须设定名称，其名称为对应的插入商品名
	bool32	FormularPrice(OUT CArbitrageFormulaVariableVar &varResult
						, INOUT CArbitrageFormulaVariableVar &varIn
						, CArbitrageFormula *pFormula
						, CMerch *pMerchToCalc
						, const ArbVarArray &aOtherVars) const;

	bool32	IsNeedCMerch() const;

	// 合并的插入商品
	int32	GetOtherMerchs(OUT ArbMerchArray &aMerchs, bool32 bContainABC=false) const;
	int32	GetOtherMerchsCount(bool32 bContainABC=false) const;

	float	GetOtherMerchPrice(CMerch *pOtherMerch, E_ArbitrageHead eHead) const;

public:

// 	CMerch*		m_pMerchA;
// 	CMerch*		m_pMerchB;
// 	CMerch*		m_pMerchC;

	T_ArbMerch	m_MerchA;
	T_ArbMerch	m_MerchB;
	T_ArbMerch	m_MerchC;
	
	E_Arbitrage	m_eArbitrage;
	E_ArbPrice	m_eArbPrice;

	int32		m_iSaveDec;
	// 预警相关的 xml 字段, 放在套利里保存处理简单些
public:
	bool32		m_bAlarm;				// 是否预警	
	float		m_fUp;					// 上破价
	float		m_fDown;				// 下破价

};

// 通知类
class CArbitrageNotify
{
public:
	// 增加套利
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage)	= 0;

	// 删除套利
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage)	= 0;

	// 修改套利
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew) = 0;
};

// 套利分析管理类
class EXPORT_CLASS CArbitrageManage : public CAlarmNotify
{
public:
	typedef CArray<CKLine, CKLine >	KLineInMerchArray;
	typedef vector<KLineInMerchArray *> KLineInMerchPtrArray;
	typedef map<CMerch *, KLineInMerchArray *> Merch2KLineMap;
	typedef set<CArbitrage> ArbitrageSet;

private:
	typedef CArray<CArbitrage, CArbitrage&> ArbitrageArray;
	typedef map<CMerch*, ArbitrageSet> MerchArbitrageMap;
	typedef CArray<CKLine, CKLine&> KLineArray;

public:	
	virtual ~CArbitrageManage();

public:
	// 条件有改变
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify);
	
	// 达到预警条件, 报警了
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt){}
	
	// 发警报
	virtual void AlarmArbitrage( CArbitrage* pArbitrage, const CString& StrPrompt){}

public:
	// 单例
	static CArbitrageManage*	Instance();

	static void		DelInstance();

	bool			Initialize(CAbsCenterManager* pData,CAlarmCenter* pAlarm);

	//
	bool32		BeLoadXmlSuccessed() const	{ return m_bFromXml; }
	void		ReLoadXml() { FromXml(); }

	// 通知
	void		AddNotify(CArbitrageNotify* p);

	void		DelNotify(CArbitrageNotify* p);

	// 套利数据
	void		AddArbitrage(const CArbitrage& stArbitrage);

	void		DelArbitrage(const CArbitrage& stArbitrage);

	void		ModifyArbitrage(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);

	// 获取套利的所有商品
	void		GetArbitrageMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs);

	// 获取所有的套利数据
	void		GetArbitrageDatas(OUT CArray<CArbitrage, CArbitrage&>& aArbitrages);
	const CArray<CArbitrage, CArbitrage&>& GetArbitrageDataRef() { return m_aArbitrages; }

	// 获取商品对应的套利模型
	void		GetArbitrageDatasByMerch(IN CMerch* pMerch, OUT CArray<CArbitrage, CArbitrage&>& aArbitrages);
	void		GetArbitrageDatasByMerch(IN CMerch* pMerch, OUT ArbitrageSet	&sArbitrages);

	// 合成K 线, 
// 	static bool32 CombineArbitrageKLine(IN const CArbitrage& st,								// 套利模型, 用于取套利指标算法
// 										IN const CArray<CKLine, CKLine>& aKLineA,				// 商品 A 的原始 K 线
// 										IN const CArray<CKLine, CKLine>& aKLineB,				// 商品 B 的原始 K 线
// 										IN const CArray<CKLine, CKLine>& aKLineC,				// 商品 C 的原始 K 线
// 										IN const KLineInMerchPtrArray	&aOtherKLinePtrs,		// 其它插入商品的原始K线
// 										OUT CArray<CKLine, CKLine>& aKLineCombineA,				// 合成好的 A 腿
// 										OUT CArray<CKLine, CKLine>& aKLineCombineB,				// 合成好的 B 腿
// 										OUT CArray<CKLine, CKLine>& aKLineCombine);				// 合成好的整条 K 线
	static bool32 CombineArbitrageKLine(IN const CArbitrage& st,								// 套利模型, 用于取套利指标算法
										IN const Merch2KLineMap &mapMerchKLines,				// 所有套利有关商品的K线数据
										OUT CArray<CKLine, CKLine>& aKLineCombineA,				// 合成好的 A 腿
										OUT CArray<CKLine, CKLine>& aKLineCombineB,				// 合成好的 B 腿
										OUT CArray<CKLine, CKLine>& aKLineCombine);				// 合成好的整条 K 线

	// 合并共同时间
	static bool32 CombineArbitrageKLineTime(OUT vector<long> &aCommTimes,						// 共同时间
											IN const KLineInMerchArray *pKLineA,				// A线，不能为NULL
											IN const KLineInMerchArray *pKLineB,				// B线，不能为NULL
											IN const KLineInMerchArray *pKLineC);				// C线，能为NULL

private:
	CArbitrageManage();

	// Xml 操作
	bool32		FromXml();

	bool32		ToXml();

	// 更新所有商品集合和对应关系
	void		RebuildMerchsAllRelationShips();

	// 更新对应关系
	void		UpdateMap(CMerch* pMerch, const CArbitrage& stArbitrage);


private:
	// 
	bool32			m_bFromXml;

	// 数据中心
	CAbsCenterManager*		m_pCenterManager;

	//预警中心
	CAlarmCenter*   m_pAlarmCenter; 

	// 通知队列
	CArray<CArbitrageNotify*, CArbitrageNotify*>	m_apNotifys;

	// 套利模型数组
	ArbitrageArray	m_aArbitrages;

	// 自己的指针
	static CArbitrageManage*	m_pThis;

	// 所有的商品集合
	set<CMerch*>	m_apMerchsAll;

	// 每个商品对应的套利模型集合
	MerchArbitrageMap	m_mapMerchArbitrage;

};

#endif // !defined(_ARBITRAGEMANAGE_H_)

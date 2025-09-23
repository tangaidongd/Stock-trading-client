#ifndef _SELECT_STOCK_STRUCT_H
#define _SELECT_STOCK_STRUCT_H

#include <set>
#include "MerchManager.h"
#include "ReportScheme.h"
#include "CFormularContent.h"
//#include "IoViewShare.h"
#include "TimeInterval.h"
#include "define.h"
#include "TechExport.h"
using namespace std;



// 选股类型
enum E_SelectStockType
{
	ESSTCondition = 0,	// 条件选股
	ESSTCustom,			// 定制选股

	ESSTCounts
};

struct T_CustomCdtCell;

typedef struct T_IndexOut
{
	T_IndexOut();
	~T_IndexOut();
	int32	iPointNum;
	float*	pPoint;
	CString StrName;
	int32	iType;
	int32	iTypeEx;
	int32	iThick;
	int32	iColor;
	int32	iKlineDrawType;		// drawkline2时绘制K线的方式 0-普通空心阳 1-实心阳 2-美国线

	// fangz zhibiao 移植增加字段. 在 DrawStickLine 时, 解析这个字符串得到 宽度和是否空心 两个参数的值
	//								  StringData	时  保存DrawText 的 Text
	CString			StrExtraData;	

	// DrawNum 需要保存一系列的字符串
	CStringArray	aStrExtraData;	

	// fangz zhibiao 移植增加字段. 在 DrawStickLine 时, 画矩形需要上下两个坐标值.
	float*  pPointExtra1;

	// fangz zhibiao 移植增加字段. 在 DrawKline 的时候, 需要4 个参数. 以前一个指标一根线只需要 float *pPoint 就搞定.
	// 这个指标需要高开低收, 每个节点 4 个参数才能搞定.
	float*  pPointExtra2;
	float*  pPointExtra3;

	
}	T_IndexOut;

typedef struct T_IndexOutArray
{
	int32			iIndexNum;
	T_IndexOut		index[256];		

	// 保存一个名字的map ,加快以指标名字频繁访问时速度	
	CMap<CString, LPCTSTR, int32, int32&> m_aMapNameToIndex;

}T_IndexOutArray;

// 商品排序函数
struct MerchCmp
{
	bool operator()(const CMerch* pMerch1, const CMerch* pMerch2) const
	{
		if ( NULL == pMerch1 || NULL == pMerch2 )
		{
			return false;
		}
		
		//if ( pMerch1->m_MerchInfo.m_iMarketId < pMerch1->m_MerchInfo.m_iMarketId )
		//{
			//return false;
		//}
		
		//return (pMerch1->m_MerchInfo.m_StrMerchCode.CompareNoCase(pMerch2->m_MerchInfo.m_StrMerchCode) < 0);	
		return true;
	}
};

// 公式排序函数
struct FormularCmp
{
	bool operator()(const CFormularContent* p1, const CFormularContent* p2) const
	{
		if ( NULL == p1 || NULL == p2 )
		{
			return false;
		}
		
		return (p1->name.CompareNoCase(p2->name) < 0);
	}
};

// 条件选股的参数结构
typedef struct T_ConditionSelectParam
{
public:
	T_ConditionSelectParam()
	{
		m_aMerchs.clear();
		m_aFormulars.clear();
		m_TimeBegin = 0;
		m_TimeEnd = 0;
		m_bAndCondition = false;
		m_bHistory = false;
		m_eInterval = ENTICount;
	}
	
	bool32 BeValid()
	{
		if ( m_aMerchs.empty() || m_aFormulars.empty() )
		{
			return false;
		}
		
		if ( m_TimeBegin.GetTime() <= 0 || m_TimeEnd.GetTime() <= 0 || m_TimeEnd < m_TimeBegin )
		{
			return false;
		}
		
		if ( (int32)m_eInterval < 0 || m_eInterval >= ENTICount )
		{
			return false;
		}
		
		return true;
	}
	// 待选商品
	set<CMerch*, MerchCmp> m_aMerchs;
	
	// 计算计算条件
	set<CFormularContent*, FormularCmp> m_aFormulars;
	
	// 选股时间段
	CTime	m_TimeBegin;
	CTime	m_TimeEnd;
	
	// 是与条件还是或条件
	bool32  m_bAndCondition;
	
	// 是否历史阶段选股
	bool32  m_bHistory;

	// 选股周期
	E_NodeTimeInterval m_eInterval;
	
}T_ConditionSelectParam;

// 定制选股的参数结构
typedef struct T_CustomSelectParam
{
public:
	T_CustomSelectParam()
	{
		m_aMerchs.clear();
		m_aCondtionCells.RemoveAll();
		m_bAndCondition = false;
	}
	
	T_CustomSelectParam& T_CustomSelectParam::operator=(const T_CustomSelectParam& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}

		m_aMerchs = stSrc.m_aMerchs;
		m_aCondtionCells.Copy(stSrc.m_aCondtionCells);
		m_bAndCondition = stSrc.m_bAndCondition;

		return *this;
	}

	bool32 BeValid()
	{
		if ( m_aMerchs.empty() || m_aCondtionCells.GetSize() <= 0 )
		{
			return false;
		}
	
		return true;
	}

	// 待选商品
	set<CMerch*, MerchCmp> m_aMerchs;
	
	// 计算计算条件
	CArray<T_CustomCdtCell, T_CustomCdtCell&> m_aCondtionCells;
		
	// 是与条件还是或条件
	bool32  m_bAndCondition;
	
}T_CustomSelectParam;

//////////////////////////////////////////////////////////////////////////
// 定制选股的每一个小条件的结构 (类似条件选股中一个个的公式)
typedef struct EXPORT_CLASS T_CustomCdtCell
{
public:	
	T_CustomCdtCell()
	{
		m_iIndex = 0;
		m_iCmp	 = -1;
		m_fValue = 0.;
	}
	
	CString		GetString();
	CString		GetName();
	CString		GetPostName();
	
	int32		m_iIndex;			// (取 E_ReportHeader 的值, 其他条件里几个取特殊值)
	int32		m_iCmp;				// 比较符号, 大于 等于 小于
	float		m_fValue;			// 比较的值
	
}T_CustomCdtCell;

// 定制选股的条件结构
typedef struct T_CustomStockCdt
{
public:
	
	T_CustomStockCdt()
	{
		m_eHeadType = CReportScheme::ERHCount;
		m_StrPostName.Empty();
	}
	
	T_CustomStockCdt(CReportScheme::E_ReportHeader eHeadType, const CString& StrPostName)
	{
		m_eHeadType	  = eHeadType;
		m_StrPostName = StrPostName;
	}
	
	//
	CReportScheme::E_ReportHeader m_eHeadType;	// 类型
	CString	m_StrPostName;						// 后缀名
	
}T_CustomStockCdt;

//
static const T_CustomStockCdt s_KaValues0[] =
{
	T_CustomStockCdt(CReportScheme::ERHAllCapital,		L"万股"),	// 总股本
	T_CustomStockCdt(CReportScheme::ERHCircAsset,		L"万股"),	// 流通股本
	T_CustomStockCdt(CReportScheme::ERHAllAsset,		L"万元"),	// 总资产			
	T_CustomStockCdt(CReportScheme::ERHFlowDebt,		L"万元"),	// 流动负债
	T_CustomStockCdt(CReportScheme::ERHPerFund,			L"元"),		// 每股公积金
	T_CustomStockCdt(CReportScheme::ERHBusinessProfit,	L"万元"),	// 营业利益
	T_CustomStockCdt(CReportScheme::ERHPerNoDistribute,	L"元"),		// 每股未分配
	T_CustomStockCdt(CReportScheme::ERHPerIncomeYear,	L"元"),		// 每股收益(年)
	T_CustomStockCdt(CReportScheme::ERHPerPureAsset,	L"元"),		// 每股净资产		
	T_CustomStockCdt(CReportScheme::ERHChPerPureAsset,	L"元"),		// 调整每股净资产
	T_CustomStockCdt(CReportScheme::ERHDorRightRate,	L""),		// 股东权益比		
};

static const int32 s_KiCountsValues0 = sizeof(s_KaValues0) / sizeof(T_CustomStockCdt);

//
static const T_CustomStockCdt s_KaValues1[] =
{
	T_CustomStockCdt(CReportScheme::ERHPriceNew,		L"元"),		// 最新
	T_CustomStockCdt(CReportScheme::ERHPriceOpen,		L"元"),		// 开
	T_CustomStockCdt(CReportScheme::ERHPriceHigh,		L"元"),		// 高
	T_CustomStockCdt(CReportScheme::ERHPriceLow,		L"元"),		// 低
	T_CustomStockCdt(CReportScheme::ERHPricePrevClose,	L"元"),		// 昨收
	T_CustomStockCdt(CReportScheme::ERHVolumeTotal,		L"手"),		// 量
	T_CustomStockCdt(CReportScheme::ERHAmount,			L"元"),		// 额
	T_CustomStockCdt(CReportScheme::ERHRange,			L"%"),		// 涨幅
	T_CustomStockCdt(CReportScheme::ERHSwing,			L"%"),		// 振幅
	T_CustomStockCdt(CReportScheme::ERHMarketWinRate,	L""),		// 市盈率	
	T_CustomStockCdt(CReportScheme::ERHChangeRate,		L"%"),		// 换手率
	T_CustomStockCdt(CReportScheme::ERHVolumeRate,		L""),		// 量比
};

static const int32 s_KiCountsValues1 = sizeof(s_KaValues1) / sizeof(T_CustomStockCdt);

//
static const CString s_KaValues2[] = 
{
	L"KDJ中的J值",							// 对应 ID 1000
	L"RSI指标值",							// 对应 ID 1001
	L"DMI中的ADX值",						// 对应 ID 1002
};

static const int32 s_KiCountsValues2 = sizeof(s_KaValues2) / sizeof(CString);

// 取基本周期
AFX_EXT_API bool32 GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale);


#endif // _SELECT_STOCK_STRUCT_H

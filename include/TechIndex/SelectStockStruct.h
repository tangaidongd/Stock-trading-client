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



// ѡ������
enum E_SelectStockType
{
	ESSTCondition = 0,	// ����ѡ��
	ESSTCustom,			// ����ѡ��

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
	int32	iKlineDrawType;		// drawkline2ʱ����K�ߵķ�ʽ 0-��ͨ������ 1-ʵ���� 2-������

	// fangz zhibiao ��ֲ�����ֶ�. �� DrawStickLine ʱ, ��������ַ����õ� ��Ⱥ��Ƿ���� ����������ֵ
	//								  StringData	ʱ  ����DrawText �� Text
	CString			StrExtraData;	

	// DrawNum ��Ҫ����һϵ�е��ַ���
	CStringArray	aStrExtraData;	

	// fangz zhibiao ��ֲ�����ֶ�. �� DrawStickLine ʱ, ��������Ҫ������������ֵ.
	float*  pPointExtra1;

	// fangz zhibiao ��ֲ�����ֶ�. �� DrawKline ��ʱ��, ��Ҫ4 ������. ��ǰһ��ָ��һ����ֻ��Ҫ float *pPoint �͸㶨.
	// ���ָ����Ҫ�߿�����, ÿ���ڵ� 4 ���������ܸ㶨.
	float*  pPointExtra2;
	float*  pPointExtra3;

	
}	T_IndexOut;

typedef struct T_IndexOutArray
{
	int32			iIndexNum;
	T_IndexOut		index[256];		

	// ����һ�����ֵ�map ,�ӿ���ָ������Ƶ������ʱ�ٶ�	
	CMap<CString, LPCTSTR, int32, int32&> m_aMapNameToIndex;

}T_IndexOutArray;

// ��Ʒ������
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

// ��ʽ������
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

// ����ѡ�ɵĲ����ṹ
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
	// ��ѡ��Ʒ
	set<CMerch*, MerchCmp> m_aMerchs;
	
	// �����������
	set<CFormularContent*, FormularCmp> m_aFormulars;
	
	// ѡ��ʱ���
	CTime	m_TimeBegin;
	CTime	m_TimeEnd;
	
	// �����������ǻ�����
	bool32  m_bAndCondition;
	
	// �Ƿ���ʷ�׶�ѡ��
	bool32  m_bHistory;

	// ѡ������
	E_NodeTimeInterval m_eInterval;
	
}T_ConditionSelectParam;

// ����ѡ�ɵĲ����ṹ
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

	// ��ѡ��Ʒ
	set<CMerch*, MerchCmp> m_aMerchs;
	
	// �����������
	CArray<T_CustomCdtCell, T_CustomCdtCell&> m_aCondtionCells;
		
	// �����������ǻ�����
	bool32  m_bAndCondition;
	
}T_CustomSelectParam;

//////////////////////////////////////////////////////////////////////////
// ����ѡ�ɵ�ÿһ��С�����Ľṹ (��������ѡ����һ�����Ĺ�ʽ)
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
	
	int32		m_iIndex;			// (ȡ E_ReportHeader ��ֵ, ���������Ｘ��ȡ����ֵ)
	int32		m_iCmp;				// �ȽϷ���, ���� ���� С��
	float		m_fValue;			// �Ƚϵ�ֵ
	
}T_CustomCdtCell;

// ����ѡ�ɵ������ṹ
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
	CReportScheme::E_ReportHeader m_eHeadType;	// ����
	CString	m_StrPostName;						// ��׺��
	
}T_CustomStockCdt;

//
static const T_CustomStockCdt s_KaValues0[] =
{
	T_CustomStockCdt(CReportScheme::ERHAllCapital,		L"���"),	// �ܹɱ�
	T_CustomStockCdt(CReportScheme::ERHCircAsset,		L"���"),	// ��ͨ�ɱ�
	T_CustomStockCdt(CReportScheme::ERHAllAsset,		L"��Ԫ"),	// ���ʲ�			
	T_CustomStockCdt(CReportScheme::ERHFlowDebt,		L"��Ԫ"),	// ������ծ
	T_CustomStockCdt(CReportScheme::ERHPerFund,			L"Ԫ"),		// ÿ�ɹ�����
	T_CustomStockCdt(CReportScheme::ERHBusinessProfit,	L"��Ԫ"),	// Ӫҵ����
	T_CustomStockCdt(CReportScheme::ERHPerNoDistribute,	L"Ԫ"),		// ÿ��δ����
	T_CustomStockCdt(CReportScheme::ERHPerIncomeYear,	L"Ԫ"),		// ÿ������(��)
	T_CustomStockCdt(CReportScheme::ERHPerPureAsset,	L"Ԫ"),		// ÿ�ɾ��ʲ�		
	T_CustomStockCdt(CReportScheme::ERHChPerPureAsset,	L"Ԫ"),		// ����ÿ�ɾ��ʲ�
	T_CustomStockCdt(CReportScheme::ERHDorRightRate,	L""),		// �ɶ�Ȩ���		
};

static const int32 s_KiCountsValues0 = sizeof(s_KaValues0) / sizeof(T_CustomStockCdt);

//
static const T_CustomStockCdt s_KaValues1[] =
{
	T_CustomStockCdt(CReportScheme::ERHPriceNew,		L"Ԫ"),		// ����
	T_CustomStockCdt(CReportScheme::ERHPriceOpen,		L"Ԫ"),		// ��
	T_CustomStockCdt(CReportScheme::ERHPriceHigh,		L"Ԫ"),		// ��
	T_CustomStockCdt(CReportScheme::ERHPriceLow,		L"Ԫ"),		// ��
	T_CustomStockCdt(CReportScheme::ERHPricePrevClose,	L"Ԫ"),		// ����
	T_CustomStockCdt(CReportScheme::ERHVolumeTotal,		L"��"),		// ��
	T_CustomStockCdt(CReportScheme::ERHAmount,			L"Ԫ"),		// ��
	T_CustomStockCdt(CReportScheme::ERHRange,			L"%"),		// �Ƿ�
	T_CustomStockCdt(CReportScheme::ERHSwing,			L"%"),		// ���
	T_CustomStockCdt(CReportScheme::ERHMarketWinRate,	L""),		// ��ӯ��	
	T_CustomStockCdt(CReportScheme::ERHChangeRate,		L"%"),		// ������
	T_CustomStockCdt(CReportScheme::ERHVolumeRate,		L""),		// ����
};

static const int32 s_KiCountsValues1 = sizeof(s_KaValues1) / sizeof(T_CustomStockCdt);

//
static const CString s_KaValues2[] = 
{
	L"KDJ�е�Jֵ",							// ��Ӧ ID 1000
	L"RSIָ��ֵ",							// ��Ӧ ID 1001
	L"DMI�е�ADXֵ",						// ��Ӧ ID 1002
};

static const int32 s_KiCountsValues2 = sizeof(s_KaValues2) / sizeof(CString);

// ȡ��������
AFX_EXT_API bool32 GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale);


#endif // _SELECT_STOCK_STRUCT_H

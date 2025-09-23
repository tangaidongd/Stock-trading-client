// FormulaCalc.h: interface for the CFormulaCalc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FORMULACALC_H_
#define _FORMULACALC_H_

#pragma once

#include <vector>

#include "ShareFun.h"
#include "MerchManager.h"

using namespace std;

struct T_CapticalFlow;
typedef CKLine KLine;
class CExpertTrade;

// 优化计算需要用到的数据结构

// 公共的参数数据源
typedef struct T_CycValues
{
public:
	void Clear()
	{
		m_aZJ.clear();
		m_aYZ.clear();
		m_aCYC12.clear();
		m_aCYC25.clear();
		m_aCYC50.clear();
		m_aCYC0.clear();
	}
	
	bool BeValid()
	{
		int iSize1 = m_aZJ.size();
		int iSize2 = m_aYZ.size();
		int iSize3 = m_aCYC0.size();
		int iSize4 = m_aCYC12.size();
		int iSize5 = m_aCYC25.size();
		int iSize6 = m_aCYC50.size();

		if ( (iSize1 == iSize2) && (iSize1 == iSize3) && (iSize1 == iSize4) && (iSize1 == iSize5) && (iSize1 == iSize6) && (iSize1 > 0)  )
		{
			return true;
		}

		return false;
	}

	T_CycValues()
	{
		Clear();	
	}

	T_CycValues(const T_CycValues& Src)
	{
		m_aZJ	 = Src.m_aZJ;
		m_aYZ	 = Src.m_aYZ;
		m_aCYC12 = Src.m_aCYC12;
		m_aCYC25 = Src.m_aCYC25;
		m_aCYC50 = Src.m_aCYC50;
		m_aCYC0  = Src.m_aCYC0;
	}

	T_CycValues& operator= (const T_CycValues& Src)
	{
		if ( this == &Src )
		{
			return *this;
		}

		m_aZJ	 = Src.m_aZJ;
		m_aYZ	 = Src.m_aYZ;
		m_aCYC12 = Src.m_aCYC12;
		m_aCYC25 = Src.m_aCYC25;
		m_aCYC50 = Src.m_aCYC50;
		m_aCYC0  = Src.m_aCYC0;

		return *this;
	}

	//
	vector<float>	m_aZJ;
	vector<float>	m_aYZ;
	vector<float>	m_aCYC12;
	vector<float>	m_aCYC25;
	vector<float>	m_aCYC50;
	vector<float>	m_aCYC0;

}T_CycValues;

// 主力拉升的指标字段
typedef struct T_CalcInfoJglt
{
public:
	void Clear()
	{
		m_iIndexLastSatisfy = -1;
	}

	T_CalcInfoJglt()
	{
		Clear();
	}

	T_CalcInfoJglt(const T_CalcInfoJglt& Src)
	{
		m_iIndexLastSatisfy = Src.m_iIndexLastSatisfy;		
	}

	T_CalcInfoJglt& operator= (const T_CalcInfoJglt& Src)
	{
		if ( this == &Src )
		{
			return *this;
		}

		m_iIndexLastSatisfy = Src.m_iIndexLastSatisfy;		
		return *this;
	}

	//
	int		m_iIndexLastSatisfy;	// 最后一个满足条件的索引

}T_CalcInfoJglt;

// 主力洗盘的指标字段
typedef struct T_CalcInfoXpjs
{
public:
	void Clear()
	{
		m_iIndexLastSatisfy = -1;
	}

	T_CalcInfoXpjs()
	{
		Clear();
	}

	T_CalcInfoXpjs(const T_CalcInfoXpjs& Src)
	{
		m_iIndexLastSatisfy = Src.m_iIndexLastSatisfy;		
	}

	T_CalcInfoXpjs& operator= (const T_CalcInfoXpjs& Src)
	{
		if ( this == &Src )
		{
			return *this;
		}

		m_iIndexLastSatisfy = Src.m_iIndexLastSatisfy;		
		return *this;
	}

	//
	int		m_iIndexLastSatisfy;	// 最后一个满足条件的索引

}T_CalcInfoXpjs;


//

// 指标计算类
class CFormulaCalc  
{
public:
	CFormulaCalc();
	virtual ~CFormulaCalc();

public:
	// 盘中选股公式计算
	// 判断是否需要更新最新一根针对 Filter 公式的
	static bool BeNeedUpdateLast(int iParam, int iIndexNow, int iIndexLast);

	// 战略选股公式
	static bool CalcCycValues(IN KLine* pKLine, IN int iCountKLine, OUT float*& pZJ, OUT float*& pYZ, OUT float*& pCYC0, OUT float*& pCYC12, OUT float*& pCYC25, OUT float*& pCYC50);
	static bool UpdateLastCalcCycValue(IN CExpertTrade* pData);

	// 2、个股选股公式（仅判断日K线）：
	// 1) 主力拉升公式：
	static bool CalcJgtlIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult);
	static bool UpdateLastJgtlIndex(IN CExpertTrade* pData, OUT bool& bSatisfy);

	// 2) 主力洗盘公式：
	static bool CalcXpjsIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult);
	static bool UpdateLastXpjsIndex(IN CExpertTrade* pData, OUT bool& bSatisfy);

	// 3) 游击涨停公式：
	static bool CalcZjztIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, OUT vector<UINT>& aResult);

	// 4) 抗跌强庄公式：
	static bool CalcNsqlIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCloseDapan, IN float fCloseDapanPre, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult);

	// 5) 超跌反弹公式：	
	static bool CalcMaVar4ForCdft(IN KLine* pKLine, IN int iCountKLine, OUT float& fMaVar4);
	static bool CalcCdftIndex(IN KLine* pKLine, IN int iCountKLine, OUT bool& bSatisfy);
	static bool CalcCdftIndex(IN KLine* pKLine, IN int iCountKLine, OUT vector<UINT>& aResult);

private:
	// 因为都是不需要历史数据的, 所以传出参数, 只要算最新一根K 线的就行了
	// MA 单个值
	static bool	MA(IN KLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult);	
	static bool	MA(IN float* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult);	

	// MA 整个序列
	static bool	MA_ARRAY(IN float* pDataSrc, IN int iCountSrc, IN int iParam, OUT float*& pfResult);

	//  
	// DMA (参数是一个固定值)
	static bool	DMA(IN float* pDataSrc, IN int iCountSrc, IN float fParam, OUT float*& pResult);	

	// DMA(参数是一个变动的值, 数组pParam 提供. fParamEx 这个是参数的倍数关系值.)
	static bool	DMA(IN float* pDataSrc, IN int iCountSrc, IN float* pParam, IN float fParamEx, OUT float*& pResult);	
	
	// Cross 
	static void	Cross(IN float fParam1, IN float fParam1Pre, IN float fParam2, IN float fParam2Pre, OUT bool& bResult);	
	
	// SUM
	static bool	Sum(IN KLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult);

	// SUM
	static void	If(IN bool bParam1, IN float fParam2, IN float fParam3, OUT float& fResult);
};

#endif // _FORMULACALC_H_

// ExpertTrade.h: interface for the CExpertTrade class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_EXPERTTRADE_H_)
#define _EXPERTTRADE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FormulaCalc.h"
#include "TechExport.h"

class CKLine;

class EXPORT_CLASS CExpertTrade  
{
friend class CFormulaCalc;

public:
	// 专家交易提示的类型
	typedef enum E_ExpertTradeType
	{
		EETTJglt = 1,
		EETTXpjs,
		EETTZjzt,
		EETTNsql,
		EETTCdft,

		EETTCount,
	}E_ExpertTradeType;

public:
	CExpertTrade(E_ExpertTradeType eType, float fCircStock);
	virtual ~CExpertTrade();

	// 更新全部

	// 数据变化
	bool32		UpdateAll(const CArray<CKLine, CKLine>& aKLines);

	// 类型变化
	bool32		UpdateAll(E_ExpertTradeType eType);

	// 更新最新一个点
	bool32		UpdataLatest();

	// 设置流通股本值
	void		SetCircStockValue(float fCircStockValue) { m_fCircStock = fCircStockValue / 100.f; }

	// 取类型值
	E_ExpertTradeType GetTradeType() const { return m_eTradeType; }

	// 取结果
	void		GetTradePoint(OUT CArray<UINT, UINT>& aTradeTime);
	
	// 取菜单名
	static CString	GetTradeString(E_ExpertTradeType eType);

private:

	// 专家交易提示的类型
	E_ExpertTradeType		m_eTradeType;

	// 发出提示的点
	vector<UINT>			m_aTradeTime;	
	
	// K 线数据
	CArray<CKLine, CKLine&>	m_aKLines;

	// 流通股本
	float					m_fCircStock;

	// 公共的参数数据源
	T_CycValues				m_stCycValues;			
};

#endif // !defined(_EXPERTTRADE_H_)

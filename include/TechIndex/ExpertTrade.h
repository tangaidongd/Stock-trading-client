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
	// ר�ҽ�����ʾ������
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

	// ����ȫ��

	// ���ݱ仯
	bool32		UpdateAll(const CArray<CKLine, CKLine>& aKLines);

	// ���ͱ仯
	bool32		UpdateAll(E_ExpertTradeType eType);

	// ��������һ����
	bool32		UpdataLatest();

	// ������ͨ�ɱ�ֵ
	void		SetCircStockValue(float fCircStockValue) { m_fCircStock = fCircStockValue / 100.f; }

	// ȡ����ֵ
	E_ExpertTradeType GetTradeType() const { return m_eTradeType; }

	// ȡ���
	void		GetTradePoint(OUT CArray<UINT, UINT>& aTradeTime);
	
	// ȡ�˵���
	static CString	GetTradeString(E_ExpertTradeType eType);

private:

	// ר�ҽ�����ʾ������
	E_ExpertTradeType		m_eTradeType;

	// ������ʾ�ĵ�
	vector<UINT>			m_aTradeTime;	
	
	// K ������
	CArray<CKLine, CKLine&>	m_aKLines;

	// ��ͨ�ɱ�
	float					m_fCircStock;

	// �����Ĳ�������Դ
	T_CycValues				m_stCycValues;			
};

#endif // !defined(_EXPERTTRADE_H_)

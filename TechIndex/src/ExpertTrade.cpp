// ExpertTrade.cpp: implementation of the CExpertTrade class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ExpertTrade.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpertTrade::CExpertTrade(E_ExpertTradeType eType, float fCircStock)
{
	m_eTradeType = eType;
	m_fCircStock = fCircStock;
}

CExpertTrade::~CExpertTrade()
{

}

CString CExpertTrade::GetTradeString(E_ExpertTradeType eType)
{
	if ( EETTJglt == eType )
	{
		return L"主力拉升";
	}
	else if ( EETTXpjs == eType )
	{
		return L"主力洗盘";
	}
	else if ( EETTZjzt == eType )
	{
		return L"游击涨停";
	}
	else if ( EETTNsql == eType )
	{
		return L"抗跌强庄";
	}
	else if ( EETTCdft == eType )
	{
		return L"超跌反弹";
	}
	
	return L"";
}

void CExpertTrade::GetTradePoint(OUT CArray<UINT, UINT>& aTradeTime)
{
	aTradeTime.RemoveAll();

	//
	for ( int32 i = 0; i < m_aTradeTime.size(); i++ )
	{
		aTradeTime.Add(m_aTradeTime[i]);
	}
}

bool32 CExpertTrade::UpdateAll(const CArray<CKLine, CKLine>& aKLines)
{
	// 先看一下数据跟本地是不是一样的
	//lint --e{438}
	bool32 bSame = false;

	const CKLine* pSrc = (const CKLine*)aKLines.GetData();
	CKLine* pLocal = (CKLine*)m_aKLines.GetData();

	if ( NULL == pSrc )
	{
		return false;
	}

	if ( aKLines.GetSize() != m_aKLines.GetSize() )
	{	
		//
		m_aKLines.RemoveAll();
		m_aKLines.SetSize(aKLines.GetSize());
		pLocal = (CKLine*)m_aKLines.GetData();

		//
		memcpyex(pLocal, pSrc, aKLines.GetSize() * sizeof(CKLine));
	}
	else
	{
		int32 iResult = memcmp((void*)pSrc, (void*)pLocal, sizeof(CKLine)*aKLines.GetSize());
		
		//
		if ( 0 == iResult )
		{
			bSame = true;
			return true;
		}
	}
	
	//
	bool32 bOK = false;

	if ( !bSame )
	{
		int32 iCountKLine = m_aKLines.GetSize();

		m_stCycValues.m_aZJ.resize(iCountKLine, 0);
		m_stCycValues.m_aYZ.resize(iCountKLine, 0);
		m_stCycValues.m_aCYC0.resize(iCountKLine, 0);
		m_stCycValues.m_aCYC12.resize(iCountKLine, 0);
		m_stCycValues.m_aCYC25.resize(iCountKLine, 0);
		m_stCycValues.m_aCYC50.resize(iCountKLine, 0);

		//
		float* pZJ    = &(*m_stCycValues.m_aZJ.begin());
		float* pYZ    = &(*m_stCycValues.m_aYZ.begin());
		float* pCYC0  = &(*m_stCycValues.m_aCYC0.begin());
		float* pCYC12 = &(*m_stCycValues.m_aCYC12.begin());
		float* pCYC25 = &(*m_stCycValues.m_aCYC25.begin());
		float* pCYC50 = &(*m_stCycValues.m_aCYC50.begin());
		
		// 计算参数
		if ( !CFormulaCalc::CalcCycValues(pLocal, iCountKLine, pZJ, pYZ, pCYC0, pCYC12, pCYC25, pCYC50) )
		{
			m_aTradeTime.clear();
			return false;
		}

		//		
		if ( EETTJglt == m_eTradeType )
		{
			bOK = CFormulaCalc::CalcJgtlIndex(pLocal, iCountKLine, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
		}		
		else if ( EETTXpjs == m_eTradeType )
		{
			bOK = CFormulaCalc::CalcXpjsIndex(pLocal, iCountKLine, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
		}
		else if ( EETTZjzt == m_eTradeType )
		{
			bOK = CFormulaCalc::CalcZjztIndex(pLocal, iCountKLine, m_fCircStock, m_aTradeTime);
		}
		else if ( EETTNsql == m_eTradeType )
		{
			// ...fangz1210 大盘数据暂未获取
			bOK = CFormulaCalc::CalcNsqlIndex(pLocal, iCountKLine, 0, 0, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
		}
		else if ( EETTCdft == m_eTradeType )
		{
			bOK = CFormulaCalc::CalcCdftIndex(pLocal, iCountKLine, m_aTradeTime);
		}
		else 
		{
			////ASSERT(0);
			return false;
		}
	}

	if ( !bOK )
	{
		m_aTradeTime.clear();
		return false;
	}

	return true;		
}

bool32 CExpertTrade::UpdateAll(E_ExpertTradeType eType)
{
	if ( m_eTradeType == eType )
	{
		return false;
	}

	//
	m_eTradeType =eType;

	//
	bool32 bOK = false;
	int32 iCountKLine = m_aKLines.GetSize();
	CKLine* pLocal = (CKLine*)m_aKLines.GetData();

	if ( iCountKLine <= 0 || NULL == pLocal )
	{
		return false;
	}

	//
	m_stCycValues.m_aZJ.resize(iCountKLine, 0);
	m_stCycValues.m_aYZ.resize(iCountKLine, 0);
	m_stCycValues.m_aCYC0.resize(iCountKLine, 0);
	m_stCycValues.m_aCYC12.resize(iCountKLine, 0);
	m_stCycValues.m_aCYC25.resize(iCountKLine, 0);
	m_stCycValues.m_aCYC50.resize(iCountKLine, 0);
	
	//
	float* pZJ    = &(*m_stCycValues.m_aZJ.begin());
	float* pYZ    = &(*m_stCycValues.m_aYZ.begin());
	float* pCYC0  = &(*m_stCycValues.m_aCYC0.begin());
	float* pCYC12 = &(*m_stCycValues.m_aCYC12.begin());
	float* pCYC25 = &(*m_stCycValues.m_aCYC25.begin());
	float* pCYC50 = &(*m_stCycValues.m_aCYC50.begin());
	
	// 计算参数
	if ( !CFormulaCalc::CalcCycValues(pLocal, iCountKLine, pZJ, pYZ, pCYC0, pCYC12, pCYC25, pCYC50) )
	{
		//
		m_aTradeTime.clear();
		return false;
	}
	
	//		
	if ( EETTJglt == m_eTradeType )
	{
		bOK = CFormulaCalc::CalcJgtlIndex(pLocal, iCountKLine, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
	}		
	else if ( EETTXpjs == m_eTradeType )
	{
		bOK = CFormulaCalc::CalcXpjsIndex(pLocal, iCountKLine, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
	}
	else if ( EETTZjzt == m_eTradeType )
	{
		bOK = CFormulaCalc::CalcZjztIndex(pLocal, iCountKLine, m_fCircStock, m_aTradeTime);
	}
	else if ( EETTNsql == m_eTradeType )
	{
		// ...fangz1210 大盘数据暂未获取
		bOK = CFormulaCalc::CalcNsqlIndex(pLocal, iCountKLine, 0, 0, m_fCircStock, pCYC0, pCYC12, pCYC25, pCYC50, m_aTradeTime);
	}
	else if ( EETTCdft == m_eTradeType )
	{
		bOK = CFormulaCalc::CalcCdftIndex(pLocal, iCountKLine, m_aTradeTime);
	}
	else 
	{
		////ASSERT(0);
		return false;
	}

	if ( !bOK )
	{
		m_aTradeTime.clear();
		return false;
	}

	return true;
}

bool32 CExpertTrade::UpdataLatest()
{	
	return true;
}

#include "StdAfx.h"
#include "ExportTest.h"

#include "ExportIndexCalc.h"
#include "ExportQuoteRequest.h"

void CExportTest::OnQuoteMarketInfoUpdate( const T_ExportMarketInfo &stMarketInfo )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::OnQuoteDataPriceUpdate( const T_ExportQuotePrice &stItem )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::OnQuoteDataKLineUpdate( const T_ExportMerchKey &stMerchKey, int iKLineType, const T_ExportDataRange &stDataRange )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::OnQuoteDataTickUpdate( const T_ExportMerchKey &stMerchKey, const T_ExportDataRange &stDataRange )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::OnQuoteMarketStatusChanged( int iMarketId, int eNewStatus, int eOldStatus )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::OnIndexChanged( const wchar_t *pwszIndexName, int eChangeEvent )
{
	int iii = 0;
	iii = 1;
}

void CExportTest::Test1()
{
	T_ExportMerchKey stKey = {0};
	stKey.iMarket = 3000;
	strcpy(stKey.szMerchCode, "010301");
	CExportQuoteRequest::Instance().AttentionMerchPushData(this, stKey, 0xfff, 0);

	T_ExportReqDataRange stRange = {0};
	stRange.iRangeType = EERRTRangeEC;
	stRange.unRange.stCE.uEndTime = time(NULL) + 60;
	stRange.unRange.stCE.uFrontCount = 200;

	bool b2 = CExportQuoteRequest::Instance().RequestKLine(stKey, EEKTDay, stRange);
	ASSERT( b2 );

	int iMarketCount = CExportQuoteRequest::Instance().GetMarketInfoCount();
	for ( int iMar=0; iMar<iMarketCount; ++iMar )
	{
		T_ExportMarketInfo stMar = {0};
		bool b = CExportQuoteRequest::Instance().GetMarketInfo(iMar, stMar);
		ASSERT( b );

		T_ExportMarketInfo stMar2 = {0};
		b = CExportQuoteRequest::Instance().GetMarketInfoById(stMar.iMarket, stMar2);
		ASSERT( b );

		for ( int iMerch=0; iMerch<stMar.iSubMerchCount; ++iMerch )
		{
			T_ExportMerchInfo stMerch = {0};
			b = CExportQuoteRequest::Instance().GetMerchInfo(stMar.iMarket, iMerch, stMerch);
			ASSERT( b );

			T_ExportMerchInfo stMerch2 = {0};
			b = CExportQuoteRequest::Instance().GetMerchInfo(stMerch.stMerchKey, stMerch2);
			ASSERT( b );
		}
	}
}

void CExportTest::Test2()
{
	T_ExportMerchKey stKey = {0};
	stKey.iMarket = 3000;
	strcpy(stKey.szMerchCode, "010301");

	T_ExportKLineUnit *pUnit = NULL;
	int iUnitCount = 0;
	T_ExportReqDataRange stRange = {0};
	stRange.iRangeType = EERRTRangeEC;
	stRange.unRange.stCE.uEndTime = time(NULL) + 60;
	stRange.unRange.stCE.uFrontCount = 200;
	bool b1 = CExportQuoteRequest::Instance().GetKLine(stKey, EEKTDay, stRange, &pUnit, iUnitCount);
	if ( b1 )
	{
		T_ExportIndexCalcResult *pResult = NULL;
		int iResultCount = 0;
		bool bIndex = CExportIndexCalc::Instance().CalcIndex(_T("MA"), pUnit, iUnitCount, &pResult, iResultCount);
		if ( bIndex )
		{

			CExportIndexCalc::Instance().FreeCalcResultData(pResult, iResultCount);
			pResult = NULL;
			iResultCount = 0;
		}

		CExportQuoteRequest::Instance().FreeMyAllocMem(pUnit);
		pUnit = NULL;
	}

	T_ExportDataRange stFullRange = {0};
	bool b2 = CExportQuoteRequest::Instance().GetKLineRange(stKey, EEKTDay, stFullRange);
	if ( b2 )
	{
		int iii = 0;
		iii = 1;
	}

	T_ExportQuotePrice stPrice = {0};
	bool b3 = CExportQuoteRequest::Instance().GetPrice(stKey, stPrice);
	if ( b3 )
	{
		int iii = 0;
		iii = 1;
	}

	T_ExportMerchInfo stMerch;
	bool b4 = CExportQuoteRequest::Instance().GetMerchInfoByTradeCode("000001", stMerch);
	if ( b4 )
	{
		int iii = 0;
		iii = 1;
	}

	bool b5 = CExportQuoteRequest::Instance().IsInTradingTime(stKey, (data32)time(NULL), false);
	if ( b5 )
	{
		int iii = 0;
		iii = 1;
	}
}

#ifndef _BUYSELLPRICEEXCHANGE_H_
#define _BUYSELLPRICEEXCHANGE_H_


#include "XmlShare.h"

#define BUYSELLPRICEEXCHANGE_SECTION		_T("BuySellPriceExchange")
#define BUYSELLPRICEEXCHANGE_MARKET_KEY		_T("Market")

// 是否是指定的需要交换买卖价的市场 - 市商市场
inline bool32	IsBuySellPriceExchangeMarket(int32 iMarketId)
{
	CString StrValue;
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(BUYSELLPRICEEXCHANGE_SECTION, BUYSELLPRICEEXCHANGE_MARKET_KEY, NULL, StrValue) )
	{
		return _ttoi(StrValue)==iMarketId;
	}
	// 没有指定
	return false;
}

inline bool32 IsBuySellPriceExchangeMerch(CMerch *pMerch)
{
	if ( NULL!=pMerch )
	{
		return IsBuySellPriceExchangeMarket(pMerch->m_MerchInfo.m_iMarketId);
	}
	return false;
}

#endif //!_BUYSELLPRICEEXCHANGE_H_
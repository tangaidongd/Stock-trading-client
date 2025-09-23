#ifndef _EXPORTTEST_H_
#define _EXPORTTEST_H_

#include "iExportQuoteRequestNotify.h"
#include "iExportIndexCalc.h"

using namespace cfjdExport;

class CExportTest : public iExportQuoteRequestNotify, public iExportIndexCalcNotify
{
public:

	void	Test1();
	void	Test2();
	
	// iExportQuoteRequestNotify
public:
	virtual void	OnQuoteMarketInfoUpdate(const T_ExportMarketInfo &stMarketInfo);
	virtual void	OnQuoteDataPriceUpdate(const T_ExportQuotePrice &stItem);
	virtual void	OnQuoteDataKLineUpdate(const T_ExportMerchKey &stMerchKey, int iKLineType, const T_ExportDataRange &stDataRange);
	virtual void	OnQuoteDataTickUpdate(const T_ExportMerchKey &stMerchKey, const T_ExportDataRange &stDataRange);
	virtual void	OnQuoteMarketStatusChanged(int iMarketId, int eNewStatus, int eOldStatus);

	// iExportIndexCalcNotify
public:
	virtual void	OnIndexChanged(const wchar_t *pwszIndexName, int eChangeEvent);
};

#endif //!_EXPORTTEST_H_
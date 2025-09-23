#ifndef _EXPORTQUOTESTRUCTHELPER_H_
#define _EXPORTQUOTESTRUCTHELPER_H_


struct T_ExportMerchKeyCompareFunc 
{
	bool operator()(const T_ExportMerchKey &st1, const T_ExportMerchKey &st2) const
	{
		if ( st1.iMarket<st2.iMarket )
		{
			return true;
		}
		else if ( st2.iMarket < st1.iMarket )
		{
			return false;
		}
		
		return stricmp(st1.szMerchCode, st2.szMerchCode) < 0;
	}
};

struct T_TradeCodeCompareFunc
{
	bool operator()(const string &s1, const string &s2) const
	{
		if ( s1.empty() )
		{
			return !s2.empty();
		}
		else if ( s2.empty() )
		{
			return false;
		}
		
		return stricmp(s1.c_str(), s2.c_str()) < 0;
	}
};


#endif //!_EXPORTQUOTESTRUCTHELPER_H_
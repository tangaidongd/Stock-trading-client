#include "StdAfx.h"
#include "MerchDataManager.h"

////////////////////////////////////////////////
// 商品管理
////////////////////////////////////////////////
CMerchManager::CMerchManager()
{
	m_bInitializedAll = false;
	//
	m_BreedListPtr.SetSize(0, 32);
	//m_aClientTradeTimeInfo.RemoveAll();
}

CMerchManager::~CMerchManager()
{
	Clear();
}

void CMerchManager::Clear()
{
	m_bInitializedAll = false;

	for (int32 i = 0; i < m_BreedListPtr.GetSize(); i++)
	{
		DEL(m_BreedListPtr[i]);
	}
	m_BreedListPtr.RemoveAll();

	//
	m_aMapBreeds.clear();
}

void CMerchManager::AddBreed(CBreed *pBreed)
{
	ASSERT(NULL != pBreed);
	if ( NULL == pBreed )
	{
		return;
	}

	// 按照showid排序加入，id小的在前
	int32 i = 0;
	for ( i=0; i < m_BreedListPtr.GetSize() ; i++ )
	{
		if ( m_BreedListPtr[i]!=NULL )
		{
			if ( m_BreedListPtr[i]->m_iShowId > pBreed->m_iShowId )
			{
				m_BreedListPtr.InsertAt(i, pBreed);
				break;
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	if ( i == m_BreedListPtr.GetSize() )
	{
		m_BreedListPtr.Add( pBreed );
	}

	// 重新对应map中的快捷映射
	for ( i=0; i < m_BreedListPtr.GetSize() ; i++ )
	{
		m_aMapBreeds[m_BreedListPtr[i]->m_iBreedId] = i;
	}
}

bool32 CMerchManager::FindBreed(int32 iBreedId, OUT int32 &iPosFound, OUT CBreed *&pBreedFound)
{
	iPosFound = -1;
	pBreedFound = NULL;

	std::map<int32, int32>::iterator itFound = m_aMapBreeds.find(iBreedId);
	if (itFound == m_aMapBreeds.end())	// 找不到
	{
		return false;
	}

	// 找到
	int32 iIndex = itFound->second;
	if (iIndex < 0 || iIndex >= m_BreedListPtr.GetSize())
	{
		return false;
	}

	CBreed *pBreed = m_BreedListPtr.GetAt(iIndex);
	if (NULL == pBreed || pBreed->m_iBreedId != iBreedId)
	{
		return false;
	}

	iPosFound	= iIndex;
	pBreedFound = pBreed;
	return true;
}

bool32 CMerchManager::FindBreed(IN const CString& StrBreedName, OUT CBreed *&pBreedFound)
{
	pBreedFound = NULL;

	for ( int32 i = 0; i < m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = m_BreedListPtr.GetAt(i);

		if ( NULL != pBreed && 0 == pBreed->m_StrBreedCnName.CompareNoCase(StrBreedName) )
		{
			pBreedFound = pBreed;
			return true;
		}
	}

	return false;
}

bool32 CMerchManager::FindMarket(int32 iMarketId, OUT CMarket *&pMarketFound)
{
	CBreed *pBreed = NULL;
	for (int32 i = 0; i < m_BreedListPtr.GetSize(); i++)
	{
		pBreed = m_BreedListPtr[i];
		if (NULL == pBreed)
			continue;

		int32 iPosMarketFound = 0;
		if (pBreed->FindMarket(iMarketId, iPosMarketFound, pMarketFound))
			return true;
	}

	return false;
}

bool32 CMerchManager::FindMarket(CString StrMarketName, OUT CMarket *& pMarketFound)
{
	CBreed * pBreed = NULL;
	for ( int32 i = 0; i < m_BreedListPtr.GetSize(); i++)
	{
		pBreed = m_BreedListPtr[i];
		if (NULL == pBreed)
		{
			continue;
		}

		for ( int32 j = 0 ; j < pBreed->m_MarketListPtr.GetSize(); j++)
		{
			CMarket * pMarket = pBreed->m_MarketListPtr[j];
			if (NULL == pMarket)
			{
				continue;
			}

			if (pMarket->m_MarketInfo.m_StrCnName.CompareNoCase(StrMarketName) == 0)
			{
				pMarketFound = pMarket;
				return true;
			}
		}
	}

	return false;
}

bool32 CMerchManager::FindMerch(IN const CString &StrMerchCode, int32 iMarketId, OUT CMerch *&pMerchFound)
{
	//
	CMarket *pMarketFound = NULL;
	if (FindMarket(iMarketId, pMarketFound))
	{
		int32 iPosMerchFound = 0;
		if (pMarketFound->FindMerch(StrMerchCode, iPosMerchFound, pMerchFound))
			return true;
	}

	return false;
}

bool32 CMerchManager::FindTradeMerch(IN const CString &StrTradeCode, int32 iMarketId, OUT CMerch *&pMerchFound)
{
	CMarket *pMarketFound = NULL;
	if (FindMarket(iMarketId, pMarketFound))
	{
		for (int32 i = 0; i < pMarketFound->m_MerchsPtr.GetSize(); i++)
		{
			CMerch* pMerch = pMarketFound->m_MerchsPtr[i];
			if (NULL == pMerch)
			{
				continue;
			}

			//
			if (0 == pMerch->m_MerchInfo.m_StrMerchCodeInBourse.CompareNoCase(StrTradeCode))
			{
				pMerchFound = pMarketFound->m_MerchsPtr[i];
				return true;
			}
		}
	}

	return false;
}

bool32 CMerchManager::FindMerch(IN const CString &StrMerchCode, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	for ( int32 i = 0; i < m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = m_BreedListPtr[i];
		if ( NULL == pBreed )
		{
			continue;
		}

		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr[j];
			if ( NULL == pMarket )
			{
				continue;
			}

			//
			if (FindMerch(StrMerchCode, pMarket->m_MarketInfo.m_iMarketId, pMerchFound))
			{
				return true;
			}
		}
	}

	//
	return false;
}

bool32 CMerchManager::FindTradeMerch(IN const CString &StrTradeCode, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	for ( int32 i = 0; i < m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = m_BreedListPtr[i];
		if ( NULL == pBreed )
		{
			continue;
		}

		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr[j];
			if ( NULL == pMarket )
			{
				continue;
			}

			//
			if (FindTradeMerch(StrTradeCode, pMarket->m_MarketInfo.m_iMarketId, pMerchFound))
			{
				return true;
			}
		}
	}

	//
	return false;
}

bool32 CMerchManager::FindNextMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	if (NULL == pMerch)
		return false;

	CMarket &Market = pMerch->m_Market;

	//	RGUARD(LockSingle, pMarketFound->m_LockMerchList, LockMerchList);
	{
		int32 iPosMerch = -1;
		CMerch *pMerchCur = NULL;
		if (!Market.FindMerch(pMerch->m_MerchInfo.m_StrMerchCode, iPosMerch, pMerchCur))
		{
			return false;
		}

		if (pMerch != pMerchCur)
		{
			return false;
		}

		if (iPosMerch < 0 || iPosMerch >= Market.m_MerchsPtr.GetSize())
		{
			return false;
		}

		//
		if (iPosMerch == Market.m_MerchsPtr.GetSize() - 1)
		{
			iPosMerch = 0;
		}
		else
		{
			iPosMerch++;
		}

		pMerchFound = Market.m_MerchsPtr[iPosMerch];
		return true;
	}
}

bool32 CMerchManager::FindPrevMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	if (NULL == pMerch)
		return false;

	CMarket &Market = pMerch->m_Market;

	//	RGUARD(LockSingle, pMarketFound->m_LockMerchList, LockMerchList);
	{
		int32 iPosMerch = -1;
		CMerch *pMerchCur = NULL;
		if (!Market.FindMerch(pMerch->m_MerchInfo.m_StrMerchCode, iPosMerch, pMerchCur))
		{
			return false;
		}

		if (pMerch != pMerchCur)
		{
			return false;
		}

		if (iPosMerch < 0 || iPosMerch >= Market.m_MerchsPtr.GetSize())
		{
			return false;
		}

		//
		if (iPosMerch == 0)
		{
			iPosMerch = Market.m_MerchsPtr.GetSize() - 1;
		}
		else
		{
			iPosMerch--;
		}

		pMerchFound = Market.m_MerchsPtr[iPosMerch];
		return true;
	}
}

bool32 CMerchManager::FindBroker(IN const CString &StrBrokerCode, int32 iBreedId, OUT CBroker *&pBrokerFound)
{
	CBreed *pBreedFound = NULL;
	int32 iFoundPos = 0;

	if (FindBreed(iBreedId, iFoundPos, pBreedFound))
	{
		int32 iPosBrokerFound = 0;
		if (pBreedFound->FindBroker(StrBrokerCode, iPosBrokerFound, pBrokerFound))
			return true;
	}

	return false;
}

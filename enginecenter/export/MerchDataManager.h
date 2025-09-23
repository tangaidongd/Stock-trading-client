#ifndef _MERCH_DATA_MANAGER_H_
#define _MERCH_DATA_MANAGER_H_

#pragma warning(disable: 4786)

#if _MSC_VER > 1000
#pragma once
#endif 

#include "EngineCenterExport.h"
#include "MerchManager.h"

//////////////////////////////////////////////
// 商品管理
//////////////////////////////////////////////


class ENGINECENTER_DLL_EXPORT CMerchManager
{
public:
	CMerchManager();
	~CMerchManager();

public:
	void			Clear();

	void			SetInitializedAll() { m_bInitializedAll = true; };
	bool32			IsInitializedAll() { return m_bInitializedAll; };

	// 查找指定代码的大市场
	bool32			FindBreed(int32 iBreedId, OUT int32 &iPosFound, OUT CBreed *&pBreedFound);

	// 查找指定代码的大市场
	bool32			FindBreed(IN const CString& StrBreedName, OUT CBreed *&pBreedFound);

	// 查找指定市场（品种 -> 市场）
	bool32			FindMarket(int32 iMarketId, OUT CMarket *&pMarketFound);

	//... fangz0407Add 精确吗? (市场名称 -> 市场)
	bool32			FindMarket(CString StrMarketName,OUT CMarket *& pMarketFound);

	// 查找指定商品（品种 -> 市场 -> 商品）
	bool32			FindMerch(IN const CString &StrMerchCode, int32 iMarketId, OUT CMerch *&pMerchFound);

	bool32			FindMerch(IN const CString &StrMerchCode, OUT CMerch *&pMerchFound);
	bool32			FindTradeMerch(IN const CString &StrTradeCode, OUT CMerch *&pMerchFound);
	bool32			FindTradeMerch(IN const CString &StrTradeCode, int32 iMarketId, OUT CMerch *&pMerchFound);

	// 查找指定商品所在物理市场下的下一个存放位置的商品（品种 -> 市场 -> 商品）
	bool32			FindNextMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound);
	bool32			FindPrevMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound);

	// 查找指定经纪人席位（品种 -> 市场 -> 经纪人席位）
	bool32			FindBroker(IN const CString &StrBrokerCode, int32 iBreedId, OUT CBroker *&pBrokerFound);

public:
	void			AddBreed(CBreed *pBreed);

public:
	CArray<CBreed*, CBreed*> m_BreedListPtr;	// 交易品种列表
	std::map<int32, int32> m_aMapBreeds;		// 方便快速查找大市场

public:
	//static arrClientTradeTimeInfo	m_aClientTradeTimeInfo;	// 特殊商品的交易时间

private:
	bool32			m_bInitializedAll;
};


#endif // _MERCH_DATA_MANAGER_H_

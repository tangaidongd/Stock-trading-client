#ifndef _MERCH_DATA_MANAGER_H_
#define _MERCH_DATA_MANAGER_H_

#pragma warning(disable: 4786)

#if _MSC_VER > 1000
#pragma once
#endif 

#include "EngineCenterExport.h"
#include "MerchManager.h"

//////////////////////////////////////////////
// ��Ʒ����
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

	// ����ָ������Ĵ��г�
	bool32			FindBreed(int32 iBreedId, OUT int32 &iPosFound, OUT CBreed *&pBreedFound);

	// ����ָ������Ĵ��г�
	bool32			FindBreed(IN const CString& StrBreedName, OUT CBreed *&pBreedFound);

	// ����ָ���г���Ʒ�� -> �г���
	bool32			FindMarket(int32 iMarketId, OUT CMarket *&pMarketFound);

	//... fangz0407Add ��ȷ��? (�г����� -> �г�)
	bool32			FindMarket(CString StrMarketName,OUT CMarket *& pMarketFound);

	// ����ָ����Ʒ��Ʒ�� -> �г� -> ��Ʒ��
	bool32			FindMerch(IN const CString &StrMerchCode, int32 iMarketId, OUT CMerch *&pMerchFound);

	bool32			FindMerch(IN const CString &StrMerchCode, OUT CMerch *&pMerchFound);
	bool32			FindTradeMerch(IN const CString &StrTradeCode, OUT CMerch *&pMerchFound);
	bool32			FindTradeMerch(IN const CString &StrTradeCode, int32 iMarketId, OUT CMerch *&pMerchFound);

	// ����ָ����Ʒ���������г��µ���һ�����λ�õ���Ʒ��Ʒ�� -> �г� -> ��Ʒ��
	bool32			FindNextMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound);
	bool32			FindPrevMerch(IN CMerch *pMerch, OUT CMerch *&pMerchFound);

	// ����ָ��������ϯλ��Ʒ�� -> �г� -> ������ϯλ��
	bool32			FindBroker(IN const CString &StrBrokerCode, int32 iBreedId, OUT CBroker *&pBrokerFound);

public:
	void			AddBreed(CBreed *pBreed);

public:
	CArray<CBreed*, CBreed*> m_BreedListPtr;	// ����Ʒ���б�
	std::map<int32, int32> m_aMapBreeds;		// ������ٲ��Ҵ��г�

public:
	//static arrClientTradeTimeInfo	m_aClientTradeTimeInfo;	// ������Ʒ�Ľ���ʱ��

private:
	bool32			m_bInitializedAll;
};


#endif // _MERCH_DATA_MANAGER_H_

#ifndef _RELATIVE_MERCH_MANAGER_H_
#define _RELATIVE_MERCH_MANAGER_H_

#include "typedef.h"
#include "tinyxml.h"

class CMerchManager;
class CMerch;

////////////////////////////////////////////////////////////
// 
class CRelativeMerch
{
public:
	enum E_RelativeType
	{
		ERTUnknown = 0,		
			ERTWarrant = 1,			// 权证
			ERTAB	   = 2,			// A/B
			ERTAH	   = 4			// AH
	};
	
	enum E_VerifyState
	{
		EVSNotVeriry = 0,	// 未校验过
			EVSFail,			// 校验失败
			EVSNormal,			// 正常
			EVSExchange,		// 正常， src/dst理解交换
	};
	
public:
	CRelativeMerch();
	~CRelativeMerch();
	
public:
	bool32			FromXml(const TiXmlElement *pElementRelativeMerch);
	void			Verfiy(CMerchManager &MerchManager);
	
	bool32			IsRelative(IN CMerch *pMerch, CMerchManager &MerchManager, int32 iRelativeTypes);
	
public:
	// 
	E_VerifyState	m_eVerifyState;		// 校验状态， 校验过得就不用再次校验了
	
	//
	E_RelativeType	m_eRelativeType;	// 关联类别
	
	// 
	CString			m_StrSrcMerchCode;
	int32			m_iSrcBreedId;
	CMerch			*m_pMerchSrc;		// 标的股(权证)/A(H)/A(B)
	
	// 
	CString			m_StrDstMerchCode;
	int32			m_iDstBreedId;
	CMerch			*m_pMerchDst;		// (标的股)权证/(A)H/(A)B
};

////////////////////////////////////////////////////////////
// 
class CRelativeMerchManager
{
public:
	CRelativeMerchManager();
	~CRelativeMerchManager();
	
public:
	static bool32	FromXml(const CString &StrFile, CArray<CRelativeMerch, CRelativeMerch&> &RelativeList);
	
public:
	void			FindRelativeMerchs(IN CMerch *pMerch, CMerchManager &MerchManager, int32 iRelativeTypes, CArray<CRelativeMerch, CRelativeMerch&> &RelativeList);
	
	//////////////////////////////////////////
public:
	CArray<CRelativeMerch, CRelativeMerch&> m_HkWarrantList;		// 香港权证对照表
	CArray<CRelativeMerch, CRelativeMerch&> m_RelativeMerchList;	// 除香港权证以外的其他相关属性
	
};

#endif




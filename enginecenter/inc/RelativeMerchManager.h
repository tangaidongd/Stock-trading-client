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
			ERTWarrant = 1,			// Ȩ֤
			ERTAB	   = 2,			// A/B
			ERTAH	   = 4			// AH
	};
	
	enum E_VerifyState
	{
		EVSNotVeriry = 0,	// δУ���
			EVSFail,			// У��ʧ��
			EVSNormal,			// ����
			EVSExchange,		// ������ src/dst��⽻��
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
	E_VerifyState	m_eVerifyState;		// У��״̬�� У����þͲ����ٴ�У����
	
	//
	E_RelativeType	m_eRelativeType;	// �������
	
	// 
	CString			m_StrSrcMerchCode;
	int32			m_iSrcBreedId;
	CMerch			*m_pMerchSrc;		// ��Ĺ�(Ȩ֤)/A(H)/A(B)
	
	// 
	CString			m_StrDstMerchCode;
	int32			m_iDstBreedId;
	CMerch			*m_pMerchDst;		// (��Ĺ�)Ȩ֤/(A)H/(A)B
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
	CArray<CRelativeMerch, CRelativeMerch&> m_HkWarrantList;		// ���Ȩ֤���ձ�
	CArray<CRelativeMerch, CRelativeMerch&> m_RelativeMerchList;	// �����Ȩ֤����������������
	
};

#endif




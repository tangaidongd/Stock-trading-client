#ifndef _SHARE_STUCT_NEWS_H
#define _SHARE_STUCT_NEWS_H

#include "InfoExport.h"




// Э���е��ֶ�ֵ
const TCHAR KcSeprator			= 0x01;
const TCHAR	KcEnd				= 0x00;

const CString   KStrKeyInfoIndex		= L"InfoIndex";
const CString  KStrKeyParentIndex	= L"ParentIndex";
const CString  KStrKeyBeginPos		= L"BeginPos";
const CString  KStrKeyTotalCount		= L"TotalCount";
const CString  KStrKeyMarket			= L"Market";
const CString  KStrKeyCode			= L"Code";
const CString  KStrKeyDataType		= L"DataType";
const CString  KStrKeyInfoType		= L"InfoType";
const CString  KStrKeyChildDataType	= L"ChildDataType";
const CString  KStrKeyTitle			= L"Title";

const CString  KStrKeyContent		= L"Content";
const CString  KStrKeyUpdateTime		= L"UpdateTime";
const CString  KStrKeyCrc32			= L"CRC32";
const CString  KStrKeyAuthor			= L"Author";
const CString  KStrKeyCategory		= L"Category";
const CString  KStrKeyConLen			= L"ConLen";

// �������͵���Ѷindex ID
// AFX_EXT_DATA const int32	  KiIndexJinping		= 100;
const int32 KiIndexNormal			= 3;

// Э����ǰ��������ֶθ���
const int32	KiProFiledCounts		= 3;

// ��Ѷ����
enum E_InfoType
{
	EITDataSource = 1,			// ����Դ
	EITF10,						// F10
	EITWeight,					// ȨϢ��Ϣ	
	EITLandmine,				// ��Ϣ����
	EITCodeIndex,				// �����Ӧ��Ѷ
	EITTitleList,				// �����б�
	EITBulletin,				// ����������
	EITNewsTree,				// ��Ѷ��

	EITCount,
};

// ��������
enum E_DataType
{
	EDTTitle = 1,				// ���ű���
	EDTContent,					// ��������

	EDTCount,
};

// �����������Ӧ����е�����
enum E_NewsType
{
	ENTNetTest,					// ���Բ�������, ����Ҫ��¼�ȿɷ���, ���ݳ�����0-1K֮��	
	
	ENTReqHeart,				// ����
	ENTRespHeart,				// Ӧ�����

	ENTReqAuth,					// ��֤��, ��ʱ����
	ENTRespAuth,				// Ӧ����֤��

	ENTReqInfoList,				// ������Ѷ�б�
	ENTRespInfoList,			// Ӧ����ѯ�б�

	ENTReqF10,					// ���� F10
	ENTRespF10,					// Ӧ�� F10		
	
	ENTReqLandMine,				// ������Ϣ����
	ENTRespLandMine,			// Ӧ����Ϣ����

	ENTReqCodeIndex,			// ��������Ӧ����Ѷ
	ENTRespCodeIndex,			// Ӧ������Ӧ����Ѷ

	ENTReqTitleList,			// �������ű����б�
	ENTRespTitleList,			// Ӧ�����ű����б�

	ENTReqNewsContent,			// ������������
	ENTRespNewsContent,			// Ӧ����������

	ENTReqAddPush,				// �������
	ENTReqDeletePush,			// ɾ������
	ENTReqUpdatePush,			// ͬ������
		
	ENTPushInfo,				// ������Ѷ

	//
	ENTPushTitle,				// �������ű���

	ENTErrMsg,					// ������Ϣ, ֻ���Ƿ���˷���ͻ���
};
 
//////////////////////////////////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CMmiNewsBase
{
public:
	CMmiNewsBase::CMmiNewsBase(){}
	virtual CMmiNewsBase::~CMmiNewsBase(){}

	virtual CString GetSummary() = 0;

public:
	E_NewsType	m_eNewsType;
};

// ������Ѷ�б�
class DATAINFO_DLL_EXPORT CMmiNewsReqNewsList : public CMmiNewsBase
{
public:
	CMmiNewsReqNewsList::CMmiNewsReqNewsList() {m_eNewsType = ENTReqInfoList;}
	virtual CMmiNewsReqNewsList::~CMmiNewsReqNewsList() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;
		StrSummary.Format(L"������ѯ�б� Type = %d", m_eNewsType);

		return StrSummary;
	}
};

class DATAINFO_DLL_EXPORT CMmiNewsRespNewsList : public CMmiNewsBase
{
public:
	CMmiNewsRespNewsList::CMmiNewsRespNewsList() {m_eNewsType = ENTRespInfoList;}
	virtual CMmiNewsRespNewsList::~CMmiNewsRespNewsList() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;
		StrSummary.Format(L"Ӧ����ѯ�б� Type = %d, ����: %s", m_eNewsType, m_StrNews);

		return StrSummary;
	}

public:
	CString	m_StrNews;
};

// ���� F10
class DATAINFO_DLL_EXPORT CMmiNewsReqF10 : public CMmiNewsBase
{
public:
	CMmiNewsReqF10::CMmiNewsReqF10() {m_eNewsType = ENTReqF10;}
	virtual CMmiNewsReqF10::~CMmiNewsReqF10() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"������Ѷ:F10 Type = %d [%d, %s] InfoIndex: %d BeginPos: %d m_iCount: %d",
			m_eNewsType, m_iMarketId, m_StrMerchCode, m_iIndexInfo, m_iBeginPos, m_iCount);	
		return StrSummary;
	}

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;

	int32		m_iIndexInfo;
	int32		m_iBeginPos;
	int32		m_iCount;
};

class DATAINFO_DLL_EXPORT CMmiNewsRespF10 : public CMmiNewsBase
{
public:
	CMmiNewsRespF10::CMmiNewsRespF10() {m_eNewsType = ENTRespF10;}
	virtual CMmiNewsRespF10::~CMmiNewsRespF10() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"Ӧ����Ѷ:F10 Type = %d F10: %s",
			m_eNewsType, m_StrF10);	
		return StrSummary;
	}

public:
	CString	m_StrF10;
};

// ������Ϣ����
class DATAINFO_DLL_EXPORT CMmiNewsReqLandMine : public CMmiNewsBase
{
public:
	CMmiNewsReqLandMine::CMmiNewsReqLandMine() {m_eNewsType = ENTReqLandMine;}
	virtual CMmiNewsReqLandMine::~CMmiNewsReqLandMine() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	
	int32		m_iIndexInfo;	
	int32		m_iCount;
	
	int32		m_iTimeType;		
	CString		m_StrTime0;
	CString		m_StrTime1;

	/*
		TimeType: ʱ������, ��ѡ(��Ϣ������)
			  0 Time0��ʼʱ�� Time1����ʱ��
			  1 Time0 ��ʼʱ��͸���(m_iCount, ��ʱ time1 ��Ч)
			  2 Time1 ����ʱ��͸���(m_iCount, ��ʱ time0 ��Ч)	


		// ����Ĺ���:
		1: ȡ����: iIndexInfo Ϊ EITLandmine. ���ص��Ǳ�������
		2: ȡ����: ȡĳ����Ϣ���ױ����Ӧ������, iIndexInfo Ϊ�������� iIndexInfo. timetype �� time0 , time1 �ֶζ� 0. �������
	*/
};

// �ذ���Ϣ����
class DATAINFO_DLL_EXPORT CMmiNewsRespLandMine : public CMmiNewsBase
{
public:
	CMmiNewsRespLandMine::CMmiNewsRespLandMine() {m_eNewsType = ENTRespLandMine;}
	virtual CMmiNewsRespLandMine::~CMmiNewsRespLandMine() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	CString	m_StrLandMine;
};

// ��������Ӧ��Ѷ
class DATAINFO_DLL_EXPORT CMmiNewsReqCodeIndex : public CMmiNewsBase
{
public:
	CMmiNewsReqCodeIndex() { m_eNewsType = ENTReqCodeIndex; }
	~CMmiNewsReqCodeIndex(){}
	
	virtual CString GetSummary(){return L"";}

public:
	CString m_StrCode;
};

class DATAINFO_DLL_EXPORT CMmiNewsRespCodeIndex : public CMmiNewsBase
{
public:
	CMmiNewsRespCodeIndex::CMmiNewsRespCodeIndex() {m_eNewsType = ENTRespCodeIndex;}
	virtual CMmiNewsRespCodeIndex::~CMmiNewsRespCodeIndex() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	CString	m_StrCodeIndex;
};

// �������ű����б�
class DATAINFO_DLL_EXPORT CMmiNewsReqTitleList : public CMmiNewsBase
{
public: 
	CMmiNewsReqTitleList() { m_eNewsType = ENTReqTitleList; m_iInfoIndex = KiIndexNormal; m_iBeginPos = 0; m_iNeedCounts = 20; }
	~CMmiNewsReqTitleList(){}

	virtual CString GetSummary() { return L"�������ű����б�"; }

public:
	int32		m_iInfoIndex;	// �ڶ������������� , 3 ����ͨ����, 100 �ǽ���
	int32		m_iBeginPos;
	int32		m_iNeedCounts;	// ��Ҫ�������������
};

//
class DATAINFO_DLL_EXPORT CMmiNewsRespTitleList : public CMmiNewsBase
{
public:
	CMmiNewsRespTitleList() { m_eNewsType = ENTRespTitleList; }
	virtual ~CMmiNewsRespTitleList(){}

	virtual CString GetSummary() { return L"Ӧ�����ű����б�"; }

public:
	// mapNewsTitles	m_mapNewsTitles;
	CString			m_StrTitles;
};

//
class DATAINFO_DLL_EXPORT CMmiNewsPushNewsTitle : public CMmiNewsBase
{
public:
	CMmiNewsPushNewsTitle() { m_eNewsType = ENTPushTitle; }
	virtual ~CMmiNewsPushNewsTitle(){}
	
	virtual CString GetSummary() { return L"�������ű���"; }
	
public:
	CString			m_StrTitle;
};

// 
class DATAINFO_DLL_EXPORT CMmiNewsReqNewsContent : public CMmiNewsBase
{
public:
	CMmiNewsReqNewsContent() { m_eNewsType = ENTReqNewsContent; }
	virtual CString GetSummary() { return L"������������"; }

public:
	int32	m_iIndexID;
};

// 
class DATAINFO_DLL_EXPORT CMmiNewsRespNewsContent : public CMmiNewsBase
{
public:
	CMmiNewsRespNewsContent() { m_eNewsType = ENTRespNewsContent; }
	virtual CString GetSummary() { return L"Ӧ����������"; }
	
public:
	CString	m_StrContent;
};

// ��������
class DATAINFO_DLL_EXPORT CMmmiNewsReqAddPush : public CMmiNewsBase
{
public:
	CMmmiNewsReqAddPush() { m_eNewsType = ENTReqAddPush; m_iIndexID = KiIndexNormal; }

	virtual CString GetSummary() { return L"����������Ѷ"; }

public:

	int32	m_iIndexID; // Ŀǰ��Ѷ���ĸ�ID ��2, û��������, ��ʱ����Ҫ�������ֵ
};

// ͬ������
class DATAINFO_DLL_EXPORT CMmmiNewsReqUpdatePush : public CMmiNewsBase
{
public:
	CMmmiNewsReqUpdatePush() { m_eNewsType = ENTReqUpdatePush; m_iIndexID = KiIndexNormal; }
	
	virtual CString GetSummary() { return L"����������Ѷ"; }
	
public:
	
	int32	m_iIndexID; // Ŀǰ��Ѷ���ĸ�ID ��2, û��������, ��ʱ����Ҫ�������ֵ
};

// ɾ������
class DATAINFO_DLL_EXPORT CMmmiNewsReqDelPush : public CMmiNewsBase
{
public:
	CMmmiNewsReqDelPush() { m_eNewsType = ENTReqDeletePush; m_iIndexID = KiIndexNormal; }
	
	virtual CString GetSummary() { return L"����������Ѷ"; }
	
public:
	
	int32	m_iIndexID; // Ŀǰ��Ѷ���ĸ�ID ��2, û��������, ��ʱ����Ҫ�������ֵ
};

// ������
class DATAINFO_DLL_EXPORT CMmiNewsReqHeart : public CMmiNewsBase
{
public:
	CMmiNewsReqHeart::CMmiNewsReqHeart() {m_eNewsType = ENTReqHeart;}
	virtual CMmiNewsReqHeart::~CMmiNewsReqHeart() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"����������: Type = %d ", m_eNewsType);	
		return StrSummary;
	}
};

class DATAINFO_DLL_EXPORT CMmiNewsRespHeart : public CMmiNewsBase
{
public:
	CMmiNewsRespHeart::CMmiNewsRespHeart() {m_eNewsType = ENTRespHeart;}
	virtual CMmiNewsRespHeart::~CMmiNewsRespHeart() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"Ӧ��������: Type = %d ", m_eNewsType);	
		return StrSummary;
	}
};

// �����
class DATAINFO_DLL_EXPORT CMmiNewsRespErrMsg : public CMmiNewsBase
{
public:
	CMmiNewsRespErrMsg::CMmiNewsRespErrMsg() {m_eNewsType = ENTRespHeart;}
	virtual CMmiNewsRespErrMsg::~CMmiNewsRespErrMsg() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"�յ������: Type = %d ������Ϣ: %s", m_eNewsType, m_StrErrMsg);	
		return StrSummary;
	}

public:
	CString		m_StrErrMsg;
};


#endif _SHARE_STUCT_NEWS_H
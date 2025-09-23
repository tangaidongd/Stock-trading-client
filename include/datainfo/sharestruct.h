#ifndef _SHARE_STRUCT_H_
#define _SHARE_STRUCT_H_

#include "MerchManager.h"
#include "InfoExport.h"



// ����ҵ������Ӧ������
typedef enum E_CommType
{
	//
	ECTReqAuth,					// ������֤
	ECTRespAuth,				

	//
	ECTReqMarketInfo,			// �г���Ϣ
	ECTRespMarketInfo,

	//
	ECTReqBroker,				// ����ϯλ��Ϣ
	ECTRespBroker,

	//
	ECTReqMerchInfo,			// ��Ʒ������Ϣ
	ECTRespMerchInfo,

	ECTReqPublicFile,			// �������ļ���Ϣ
	ECTRespPublicFile,

	ECTReqLogicBlockFile,		// �������İ���ļ�
	ECTRespLogicBlockFile,

	ECTReqMerchExtendData,		// ��Ʒ��չ����
	ECTRespMerchExtendData,

	ECTReqMerchF10,				// ��ƷF10��Ϣ
	ECTRespMerchF10,

	//
	ECTReqRealtimePrice,		// ʵʱ��Ʒ5������
	ECTRespRealtimePrice,

	ECTReqRealtimeTick,			// ʵʱTick
	ECTRespRealtimeTick,

	ECTReqRealtimeLevel2,		// ʵʱlevel2����
	ECTRespRealtimeLevel2,

	//
	ECTReqMerchKLine,			// ��Ʒ��ʷK������
	ECTRespMerchKLine,

	ECTReqMerchTimeSales,		// ��ƷTimeSales����
	ECTRespMerchTimeSales,

	// 
	ECTReqReport,				// ���۱�
	ECTRespReport,

	//
	ECTReqBlockReport,			// �������
	ECTRespBlockReport,		

	// 
	ECTReqMarketSnapshot,		// �г�����, Ŀǰ�����Ǽ������µ�������Ϣ
	ECTRespMarketSnapshot,	

	//
	ECTReqTradeTime,			// ��Ʒ�����⽻��ʱ��
	ECTRespTradeTime,

	// ע������
	ECTRegisterPushTick,		// ע������Tick
	ECTAnsRegisterPushTick,	

	ECTRegisterPushPrice,		// ע������Price
	ECTAnsRegisterPushPrice,	

	ECTRegisterPushLevel2,		// ע������Level2
	ECTAnsRegisterPushLevel2,	

	ECTCancelAttendMerch,		// ȡ����Ʒʵʱ��������
	ECTRespCancelAttendMerch,

	// 
	ECTHeartPing,				// ������
	ECTRespHeart,

	ECTRespError,				// ����������

	// ����
	ECTPushPrice,				// �۸�
	ECTPushTick,				// �ֱ�
	ECTPushLevel2,				// Level2

	//
	ECTUpdateSymbol,			// ������Ʒ�б�
	ECTKickOut,					// ����

	// 
	ECTReqUploadData,			// �ϴ�����
	ECTRespUploadData,		

	// 
	ECTReqDownloadData,			// ��������
	ECTRespDownloadData,			

	//
	ECTReqNetTest,				// �������
	ECTRespNetTest,

	ECTReqPlugIn,				// ���ģ���
	ECTRespPlugIn,				// ���ģ���	

	ECTAddRegisterPushPrice,		// ���ע������Price(��ͬ������)
	ECTAnsAddRegisterPushPrice,	

	// �����ķ�������̬������
	ECTReqGeneralNormal = 2000,			// ����ָ����ͨ����(����Ȩ�ǵ���, ���̳ɽ�����, ���̳ɽ�����, ���̳ɽ�����, ���̳ɽ�����, ˲�������µ�����)
	ECTRespGeneralNormal,

	ECTReqGeneralFinance,				// ����ָ���Ĳ�������(�ܹɱ�, ����ͨ��, ����ֵ, ����ͨ��ֵ, ƽ����ӯ��, ƽ���о���, ƽ��������)	
	ECTRespGeneralFinance,

	ECTReqMerchTrendIndex,				// ��ͨ��Ʒ������(����, �嵵������֮��, �嵵������֮��)
	ECTRespMerchTrendIndex,

	ECTReqMerchAuction,	   				// ��Ʒ���Ͼ�������(���, ����, ����)
	ECTRespMerchAuction,

	ECTReqAddPushGeneralNormal, 		// ����ָ����������
	ECTRespAddPushGeneralNormal,

	ECTReqUpdatePushGeneralNormal, 		// ͬ��ָ����������
	ECTRespUpdatePushGeneralNormal,

	ECTReqDelPushGeneralNormal,    		// ɾ��ָ����������
	ECTRespDelPushGeneralNormal,

	ECTReqAddPushMerchTrendIndex,		// ������Ʒ��ʱָ����������
	ECTRespAddPushMerchTrendIndex,

	ECTReqUpdatePushMerchTrendIndex, 	// ͬ����Ʒ��ʱָ����������
	ECTRespUpdatePushMerchTrendIndex,

	ECTReqDelPushMerchTrendIndex,    	// ɾ����Ʒ��ʱָ����������
	ECTRespDelPushMerchTrendIndex,

	ECTReqFundHold,						// �������ֲ�	
	ECTRespFundHold,					

	ECTReqMinuteBS,						// ������Ʒ��������������
	ECTRespMinuteBS,					

	ECTReqAddPushMinuteBS,				// ������Ʒ��������������
	ECTRespAddPushMinuteBS,

	ECTReqUpdatePushMinuteBS,			// ͬ����Ʒ��������������
	ECTRespUpdatePushMinuteBS,

	ECTReqDelPushMinuteBS,    			// ɾ����Ʒ��������������
	ECTRespDelPushMinuteBS,

	ECTReqAuthPlugInHeart,				// ��֤���������
	ECTRespAuthPlugInHeart,				


	//
	ECTCount	
}E_CommType;


// Ӧ�𾭼�ϯλ���б���Ϣ
class DATAINFO_DLL_EXPORT CMmiBroker
{
public:
	CString			m_StrBrokerCode;	// ����ϯλ����
	CString			m_StrBrokerCnName;	// ����ϯλ����
	CString			m_StrBrokerEnName;	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ҵ�񼶵�ͨѶ�ӿ�
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
// ͨѶ���࣬ �������ͣ� ��Ӧ��ʵ��ͨѶ��
class DATAINFO_DLL_EXPORT CMmiCommBase
{
public:
	enum E_CompareResult
	{
		ECRLess = 0,			// С��
		ECREqual,				// ����
		ECRMore,				// ����
		ECRIntersection			// �н���
	};

public:
	virtual ~CMmiCommBase() { m_eCommType = ECTCount; }

public:
	virtual CString	GetSummary() = 0;

	// �Ƚ�����ͨѶ���� ���Ƿ��а������ص���ϵ
	// ���أ�
	// false - û�пɱ���(����ͬ����ͬ��Ʒ��������������޽���)�� iResult������
	// true  - ��iResult��ֵ�� -1 ������ 0 ��ͬ 1 ���� 2 �н���
	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public: 
	E_CommType		m_eCommType;
	uint32			m_uiTimeServer;		// ������ʱ�䣬 ����Ӧ�������Ч
}; 

//////////////////////////////////////////////////////////////////////////
// ������֤��Ϣ
class DATAINFO_DLL_EXPORT CMmiReqAuth : public CMmiCommBase
{
public:
	CMmiReqAuth() { m_eCommType = ECTReqAuth; }

public:
	virtual CString GetSummary();

public:
	CString		m_StrReqJson;				// ������Ϣ��װ��json�������
};

// Ӧ����֤��Ϣ
class DATAINFO_DLL_EXPORT CMmiRespAuth : public CMmiCommBase
{
public:
	CMmiRespAuth() { m_eCommType = ECTRespAuth; }
	virtual ~CMmiRespAuth() {}

public:
	virtual CString GetSummary();

public:
	CString	m_StrRespJson;			// ���ص�����json��
};

///////////////////////////////////////////////////////////////////////////////
// ����ָ���г���Ϣ
class DATAINFO_DLL_EXPORT CMarketKey
{
public:
	int32	m_iBigMarketId;
	int32	m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqMarketInfo : public CMmiCommBase
{
public:
	CMmiReqMarketInfo() { m_eCommType = ECTReqMarketInfo; m_iBigMarketId = -1; m_iMarketId = -1; }

public:
	virtual CString	GetSummary();

public:
	CMmiReqMarketInfo(const CMmiReqMarketInfo& Src);
	CMmiReqMarketInfo& CMmiReqMarketInfo::operator=(const CMmiReqMarketInfo& Src);

public:
	int32			m_iBigMarketId;		// ���г����
	int32			m_iMarketId;		// ָ���г����

	CArray<CMarketKey, CMarketKey&>	m_aMarketMore;	// ������һ���г�ʱ�� �������ñ���(Ϊ����һ���г�ʱ��д��)
};

// Ӧ��ָ���г���Ϣ
class DATAINFO_DLL_EXPORT CMmiRespMarketInfo : public CMmiCommBase
{
public:
	CMmiRespMarketInfo() { m_eCommType = ECTRespMarketInfo; }
	virtual ~CMmiRespMarketInfo();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMarketInfo& CMmiRespMarketInfo::operator=(const CMmiRespMarketInfo& Src);

public:
	CArray<CMarketInfo*, CMarketInfo*> m_MarketInfoListPtr;
};

///////////////////////////////////////////////////////////////////////////////
// �����г�������Ϣ
class DATAINFO_DLL_EXPORT CMmiReqMarketSnapshot : public CMmiCommBase
{
public:
	CMmiReqMarketSnapshot() { m_eCommType = ECTReqMarketSnapshot; }

public:
	virtual CString	GetSummary();

//public:
//	const CMmiReqMarketSnapshot& CMmiReqMarketSnapshot::operator=(const CMmiReqMarketSnapshot& Src);

public:
	int32			m_iMarketId;		// ָ���г����
};

// Ӧ���г�������Ϣ
class DATAINFO_DLL_EXPORT CMmiRespMarketSnapshot : public CMmiCommBase
{
public:
	CMmiRespMarketSnapshot() { m_eCommType = ECTRespMarketSnapshot; }
	virtual ~CMmiRespMarketSnapshot();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMarketSnapshot& CMmiRespMarketSnapshot::operator=(const CMmiRespMarketSnapshot& Src);

public:
	CArray<CMarketSnapshotInfo*, CMarketSnapshotInfo*> m_MarketSnapshotListPtr;
};


///////////////////////////////////////////////////////////////////////////////
// ���󾭼�ϯλ���б���Ϣ
class DATAINFO_DLL_EXPORT CMmiReqBroker  : public CMmiCommBase
{
public:
	CMmiReqBroker() { m_eCommType = ECTReqBroker; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqBroker& CMmiReqBroker::operator=(const CMmiReqBroker& Src);

public:
	int32			m_iBreedId;		// ���г����
};


class DATAINFO_DLL_EXPORT CMmiRespBroker : public CMmiCommBase
{
public:
	CMmiRespBroker() { m_eCommType = ECTRespBroker; }
	virtual ~CMmiRespBroker();
 
public:
	virtual CString	GetSummary();


private:
	const CMmiRespBroker& CMmiRespBroker::operator=(const CMmiRespBroker& Src);

public:
	int32			m_iBreedId;
	CArray<CMmiBroker*, CMmiBroker*> m_CommBrokerListPtr;
};


///////////////////////////////////////////////////////////////////////////////
// ������Ʒ���б���Ϣ
class DATAINFO_DLL_EXPORT CMmiReqMerchInfo : public CMmiCommBase
{
public:
	CMmiReqMerchInfo() { m_eCommType = ECTReqMerchInfo; m_iMarketId = -1; }

public:
	virtual CString	GetSummary();

public:
	CMmiReqMerchInfo::CMmiReqMerchInfo(const CMmiReqMerchInfo& Src);
 	CMmiReqMerchInfo& CMmiReqMerchInfo::operator=(const CMmiReqMerchInfo& Src);

public:
	int32			m_iMarketId;		// �г����
//	CString			m_StrMerchCode;		// ��Ʒ���룬���ַ�����ʾ������г������е���Ʒ�б�

	CArray<int32, int32> m_aMarketMore;	// ����г���ͬʱ����
};

// Ӧ����Ʒ���б���Ϣ
class DATAINFO_DLL_EXPORT CMmiRespMerchInfo : public CMmiCommBase
{
public:
	CMmiRespMerchInfo() { m_eCommType = ECTRespMerchInfo; }
	virtual ~CMmiRespMerchInfo();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMerchInfo& CMmiRespMerchInfo::operator=(const CMmiRespMerchInfo& Src);

public:
	int32			m_iMarketId;		// �г����
	CArray<CMerchInfo*, CMerchInfo*> m_MerchInfoListPtr;
};

///////////////////////////////////////////////////////////////////////////////
// ����������ļ���Ϣ
class DATAINFO_DLL_EXPORT CMmiReqPublicFile : public CMmiCommBase
{
public:
	CMmiReqPublicFile() { m_eCommType = ECTReqPublicFile; }
	
public:
	virtual CString	GetSummary();
	
	// public:
	// 	const CMmiReqPublicFile& CMmiReqPublicFile::operator=(const CMmiReqPublicFile& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ���룬���ַ�����ʾ������г������е���Ʒ�б�
	
	E_PublicFileType	m_ePublicFileType;
	uint32			m_uiCRC32;
};

// Ӧ��������ļ���Ϣ
class DATAINFO_DLL_EXPORT CMmiRespPublicFile : public CMmiCommBase
{
public:

public:
	CMmiRespPublicFile() : m_pcBuffer(NULL) { m_eCommType = ECTRespPublicFile; m_iValidDataLen = 0; }
	virtual ~CMmiRespPublicFile();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespPublicFile& CMmiRespPublicFile::operator=(const CMmiRespPublicFile& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;
	
	E_PublicFileType	m_ePublicFileType;
	uint32			m_uiCRC32;			// ����CRCУ��ֵ	0 - ��ʾ������������ 
	
	char			*m_pcBuffer;
	int32			m_iValidDataLen;	// ���ݳ��ȣ� �������˽ṹ		 ����crc != 0ʱ�� ��ֵ�������壺0 - ��ʾ���������µ����� 1 - ��ʾ��Ҫ���±�������
};

///////////////////////////////////////////////////////////////////////////////
// ������Ʒ��չ����
class DATAINFO_DLL_EXPORT CMmiReqMerchExtendData : public CMmiCommBase
{
public:
	CMmiReqMerchExtendData() { m_eCommType = ECTReqMerchExtendData; }

public:
	virtual CString	GetSummary() { return L""; }

// public:
// 	const CMmiReqMerchExtendData& CMmiReqMerchExtendData::operator=(const CMmiReqMerchExtendData& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
};


// Ӧ����Ʒ��չ����
class DATAINFO_DLL_EXPORT CMmiRespMerchExtendData : public CMmiCommBase
{
public:
	CMmiRespMerchExtendData() { m_eCommType = ECTRespMerchExtendData; }
	virtual ~CMmiRespMerchExtendData();

public:
	virtual CString	GetSummary() { return L""; }

private:
	const CMmiRespMerchExtendData& CMmiRespMerchExtendData::operator=(const CMmiRespMerchExtendData& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	CMerchExtendData m_MerchExtendData;	// ��Ʒ��չ����
};


///////////////////////////////////////////////////////////////////////////////
// ������ƷF10��Ϣ
class DATAINFO_DLL_EXPORT CMmiReqMerchF10 : public CMmiCommBase
{
public:
	CMmiReqMerchF10() { m_eCommType = ECTReqMerchF10; }

public:
	virtual CString	GetSummary() { return L""; }

// public:
// 	const CMmiReqMerchF10& CMmiReqMerchF10::operator=(const CMmiReqMerchF10& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
};

// Ӧ����ƷF10��Ϣ
class DATAINFO_DLL_EXPORT CMmiRespMerchF10 : public CMmiCommBase
{
public:
	CMmiRespMerchF10() { m_eCommType = ECTRespMerchF10; }
	virtual ~CMmiRespMerchF10();
	
public:
	virtual CString	GetSummary() { return L""; }

private:
	const CMmiRespMerchF10& CMmiRespMerchF10::operator=(const CMmiRespMerchF10& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	CMerchF10		m_MerchF10;			// ��ƷF10��Ϣ
};

///////////////////////////////////////////////////////////////////////////////
// ������Ʒʵʱ5����������
class DATAINFO_DLL_EXPORT CMerchKey
{
public:
	CMerchKey() : m_iMarketId(0) {}
	CMerchKey(int32 iMarketId, LPCTSTR pwszMerchCode, uint32 uiSelTime = 0, float fSelPrice = 0.0 ) : m_iMarketId(iMarketId), 
		m_StrMerchCode(pwszMerchCode), m_uSelectTime(uiSelTime), m_fSelPrice(fSelPrice) {}
	
public:
	bool32 operator< (const CMerchKey& MerchKey) const
	{
		if ( m_iMarketId < MerchKey.m_iMarketId )
		{
			return true;
		}

		if ( m_iMarketId == MerchKey.m_iMarketId )
		{
			if ( m_StrMerchCode.CompareNoCase(MerchKey.m_StrMerchCode) < 0 )
			{
				return true;
			}
		}

		return false;
	}

	bool32	IsSameMerch(int32 iMarketId, const LPCTSTR pwszMerchCode)
	{
		if (m_iMarketId == iMarketId && m_StrMerchCode.CompareNoCase(pwszMerchCode) == 0)
			return true;

		return false;
	}

	bool32	IsSameMerch(const CMerchKey &MerchKey)
	{
		return IsSameMerch(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode); 
	}

public:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	u32             m_uSelectTime;
	float           m_fSelPrice;
};

class DATAINFO_DLL_EXPORT CPeriodMerchKey
{
public:
	CPeriodMerchKey() : m_iMarketId(0),m_StrMerchCode(L""), m_uiPeriod(0) {}
	CPeriodMerchKey(int32 iMarketId, LPCTSTR pwszMerchCode, uint8 uiPeriod) : m_iMarketId(iMarketId), m_StrMerchCode(pwszMerchCode), m_uiPeriod(uiPeriod) {}

public:
	bool32 operator< (const CPeriodMerchKey& MerchKey) const
	{
		if ( m_iMarketId < MerchKey.m_iMarketId )
		{
			return true;
		}

		if ( m_iMarketId == MerchKey.m_iMarketId )
		{
			if ( m_StrMerchCode.CompareNoCase(MerchKey.m_StrMerchCode) < 0 )
			{
				return true;
			}
		}

		return false;
	}

	bool32	IsSameMerch(int32 iMarketId, const LPCTSTR pwszMerchCode)
	{
		if (m_iMarketId == iMarketId && m_StrMerchCode.CompareNoCase(pwszMerchCode) == 0)
			return true;

		return false;
	}

	bool32	IsSameMerch(const CPeriodMerchKey &MerchKey)
	{
		return IsSameMerch(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode); 
	}

public:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	int8            m_uiPeriod;
};

class DATAINFO_DLL_EXPORT CMmiReqRealtimePrice : public CMmiCommBase
{
public:
	CMmiReqRealtimePrice() { m_eCommType = ECTReqRealtimePrice; }

public:
	virtual CString	GetSummary();

public:
 	const CMmiReqRealtimePrice& operator=(const CMmiReqRealtimePrice& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����Ʒʵʱ5����������
class DATAINFO_DLL_EXPORT CMmiRespRealtimePrice : public CMmiCommBase
{
public:
	CMmiRespRealtimePrice() { m_eCommType = ECTRespRealtimePrice; }
	virtual ~CMmiRespRealtimePrice();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespRealtimePrice& CMmiRespRealtimePrice::operator=(const CMmiRespRealtimePrice& Src);

public:
	CArray<CRealtimePrice*, CRealtimePrice*> m_RealtimePriceListPtr;	// ��Ʒ5����������
};

///////////////////////////////////////////////////////////////////
// ������ƷʵʱTick����
class  DATAINFO_DLL_EXPORT CMmiReqRealtimeTick : public CMmiCommBase
{
public:
	CMmiReqRealtimeTick() { m_eCommType = ECTReqRealtimeTick; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiReqRealtimeTick& CMmiReqRealtimeTick::operator=(const CMmiReqRealtimeTick& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����ƷʵʱTick��������
class  DATAINFO_DLL_EXPORT CMmiRespRealtimeTick : public CMmiCommBase
{
public:
	CMmiRespRealtimeTick() { m_eCommType = ECTRespRealtimeTick; }
	virtual ~CMmiRespRealtimeTick();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespRealtimeTick& CMmiRespRealtimeTick::operator=(const CMmiRespRealtimeTick& Src);
};

///////////////////////////////////////////////////////////////////////////////
// ������Ʒʵʱlevel2����
class  DATAINFO_DLL_EXPORT CMmiReqRealtimeLevel2 : public CMmiCommBase
{
public:
	CMmiReqRealtimeLevel2() { m_eCommType = ECTReqRealtimeLevel2; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqRealtimeLevel2& CMmiReqRealtimeLevel2::operator=(const CMmiReqRealtimeLevel2& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
};

// Ӧ����Ʒʵʱlevel2����
class DATAINFO_DLL_EXPORT  CMmiRespRealtimeLevel2 : public CMmiCommBase
{
public:
	CMmiRespRealtimeLevel2() { m_eCommType = ECTRespRealtimeLevel2; }
	virtual ~CMmiRespRealtimeLevel2();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespRealtimeLevel2& CMmiRespRealtimeLevel2::operator=(const CMmiRespRealtimeLevel2& Src);

public:
	CRealtimeLevel2	m_RealtimeLevel2;		// ��Ʒlevel2����
};

///////////////////////////////////////////////////////////////////////////////
// ������Ʒ��ʷK������
class DATAINFO_DLL_EXPORT  CMmiReqMerchKLine : public CMmiCommBase
{
public:
	CMmiReqMerchKLine();

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqMerchKLine& CMmiReqMerchKLine::operator=(const CMmiReqMerchKLine& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);	
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	E_KLineTypeBase m_eKLineTypeBase;	// �����K������
	E_ReqTimeType	m_eReqTimeType;		// ����ʱ������
	union								// �����
	{
		struct	// ָ��ʱ��
		{
			CGmtTime	m_TimeStart;	
			CGmtTime	m_TimeEnd;
		};
		struct	// ��������
		{
			CGmtTime	m_TimeSpecify;	// ����ƫ��
			int32		m_iFrontCount;	// ��ǰ��������ʱ�����ķ���
		};
		struct	// ��������
		{
			CGmtTime	m_TimeSpecify;	// ����ƫ��
			int32		m_iCount;		// ����������ʱ����µķ���
		};
	};
};

// Ӧ����Ʒ��ʷK������
class DATAINFO_DLL_EXPORT  CMmiRespMerchKLine : public CMmiCommBase
{
public:
	CMmiRespMerchKLine() { m_eCommType = ECTRespMerchKLine; }
	virtual ~CMmiRespMerchKLine();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespMerchKLine& CMmiRespMerchKLine::operator=(const CMmiRespMerchKLine& Src);

public:
	CMerchKLineNode	m_MerchKLineNode;	// ��Ʒ��ʷK������
};

///////////////////////////////////////////////////////////////////////////////
// ������ƷTimeSales���ݣ�ÿ���������ǹ̶�����ĳһ��ʱ�䷶Χ�ڵ�TimeSales�ڵ����ݣ�
class DATAINFO_DLL_EXPORT  CMmiReqMerchTimeSales : public CMmiCommBase
{
public:
	CMmiReqMerchTimeSales();

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqMerchTimeSales& CMmiReqMerchTimeSales::operator=(const CMmiReqMerchTimeSales& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	// �����ֶ�������ʵʱ����ʱ������
	E_ReqTimeType	m_eReqTimeType;		// ����ʱ������
	union								// �����
	{
		struct	// ָ��ʱ��
		{
			CGmtTime		m_TimeStart;
			CGmtTime		m_TimeEnd;
		};
		struct	// ��������
		{
			CGmtTime		m_TimeSpecify;	// ָ��ʱ��
			int32			m_iFrontCount;	// ��ǰ������
		};
		struct	// ��������
		{
			CGmtTime		m_TimeSpecify;	// ָ��ʱ��
			int32			m_iCount;		// ������
		};
	};
};

// Ӧ����Ʒ��ʷTimeSales������
class DATAINFO_DLL_EXPORT  CMmiRespMerchTimeSales : public CMmiCommBase	// 
{
public:
	CMmiRespMerchTimeSales() { m_eCommType = ECTRespMerchTimeSales; }
	virtual ~CMmiRespMerchTimeSales();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiRespMerchTimeSales& CMmiRespMerchTimeSales::operator=(const CMmiRespMerchTimeSales& Src);
	
public:
	CMerchTimeSales	m_MerchTimeSales;	// ��ƷTick������
};

///////////////////////////////////////////////////////////////////////////////
// ������Ʒ���а�����
class DATAINFO_DLL_EXPORT CMmiReqReport : public CMmiCommBase
{
public:
	CMmiReqReport() { m_eCommType = ECTReqReport; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiReqReport& CMmiReqReport::operator=(const CMmiReqReport& Src);

	virtual bool32	CompareReq(const CMmiCommBase *pSrc, OUT E_CompareResult &eCompareResult);

public:
	int32			m_iMarketId;		// �г����
	E_MerchReportField	m_eMerchReportField;	// ��Ʒ���۱��ֶ�
	bool32			m_bDescSort;		// ����
	int32			m_iStart;
	int32			m_iCount;
};

// Ӧ����Ʒ���а�����
class DATAINFO_DLL_EXPORT CMmiRespReport : public CMmiCommBase
{
public:
	CMmiRespReport() { m_eCommType = ECTRespReport; }
	virtual ~CMmiRespReport();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespReport& CMmiRespReport::operator=(const CMmiRespReport& Src);

public:
	int32			m_iMarketId;		// �г����
	
	E_MerchReportField	m_eMerchReportField;
	bool32			m_bDescSort;		// �������У�
	int32			m_iStart;

	// ��Ʒ�б�
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// ��Ʒ5����������
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

// ������ƷʵʱTick����
class DATAINFO_DLL_EXPORT CMmiRegisterPushTick : public CMmiCommBase
{
public:
	CMmiRegisterPushTick() { m_eCommType = ECTRegisterPushTick; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushTick& CMmiRegisterPushTick::operator=(const CMmiRegisterPushTick& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����ƷʵʱTick��������
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushTick : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushTick() { m_eCommType = ECTAnsRegisterPushTick; }
	virtual ~CMmiAnsRegisterPushTick();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushTick& CMmiAnsRegisterPushTick::operator=(const CMmiAnsRegisterPushTick& Src);
};

// ������ƷʵʱPrice����
class DATAINFO_DLL_EXPORT CMmiRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiRegisterPushPrice() { m_eCommType = ECTRegisterPushPrice; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushPrice& CMmiRegisterPushPrice::operator=(const CMmiRegisterPushPrice& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����ƷʵʱPrice��������
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushPrice() { m_eCommType = ECTAnsRegisterPushPrice; }
	virtual ~CMmiAnsRegisterPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushPrice& CMmiAnsRegisterPushPrice::operator=(const CMmiAnsRegisterPushPrice& Src);
};

// ������ƷʵʱLevle2����
class DATAINFO_DLL_EXPORT CMmiRegisterPushLevel2 : public CMmiCommBase
{
public:
	CMmiRegisterPushLevel2() { m_eCommType = ECTRegisterPushLevel2; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiRegisterPushLevel2& CMmiRegisterPushLevel2::operator=(const CMmiRegisterPushLevel2& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����ƷʵʱLevel2��������
class DATAINFO_DLL_EXPORT CMmiAnsRegisterPushLevel2 : public CMmiCommBase
{
public:
	CMmiAnsRegisterPushLevel2() { m_eCommType = ECTAnsRegisterPushLevel2; }
	virtual ~CMmiAnsRegisterPushLevel2();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsRegisterPushLevel2& CMmiAnsRegisterPushLevel2::operator=(const CMmiAnsRegisterPushLevel2& Src);
};

// �������ע��ĳֻ��Ʒ��ʵʱ��������
class DATAINFO_DLL_EXPORT CPushMerchKey
{
public:
	int32			m_iMarketId;			// �г����
	CString			m_StrMerchCode;			// ��Ʒ����
	
	int32			m_iRealtimePushTypes;	// E_RealtimePushType��ֵ
};

class DATAINFO_DLL_EXPORT CMmiCancelAttendMerch : public CMmiCommBase
{
public:
	CMmiCancelAttendMerch() { m_eCommType = ECTCancelAttendMerch; }

public:
	virtual CString	GetSummary();

// public:
 	const CMmiCancelAttendMerch& CMmiCancelAttendMerch::operator=(const CMmiCancelAttendMerch& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	int32			m_iRealtimePushTypes;		// E_RealtimePushType��ֵ
	
	CArray<CPushMerchKey, CPushMerchKey&>	m_aPushMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ��ע��ĳֻ��Ʒ��ʵʱ��������
class DATAINFO_DLL_EXPORT CMmiRespCancelAttendMerch : public CMmiCommBase
{
public:
	CMmiRespCancelAttendMerch() { m_eCommType = ECTRespCancelAttendMerch; }
	virtual ~CMmiRespCancelAttendMerch();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespCancelAttendMerch& CMmiRespCancelAttendMerch::operator=(const CMmiRespCancelAttendMerch& Src);
};

///////////////////////////////////////////////////////////////////////////////
// ����������
class DATAINFO_DLL_EXPORT CMmiHeartPing : public CMmiCommBase
{
public:
	CMmiHeartPing() { m_eCommType = ECTHeartPing; }

public:
	virtual CString	GetSummary();

// public:
// 	const CMmiHeartPing& CMmiHeartPing::operator=(const CMmiHeartPing& Src);

	CString			m_StrUserName;
	CString			m_StrPassword;
};

// Ӧ��������
class DATAINFO_DLL_EXPORT CMmiRespHeart : public CMmiCommBase
{
public:
	CMmiRespHeart() { m_eCommType = ECTRespHeart; }
	virtual ~CMmiRespHeart();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespHeart& CMmiRespHeart::operator=(const CMmiRespHeart& Src);
};

//////////////////////////////////////////////////////////////////////////
// ���Ͳ��ٰ�
class DATAINFO_DLL_EXPORT CMmiReqNetTest : public CMmiCommBase
{
public:
	// Ĭ�Ϲ��캯��
	CMmiReqNetTest();	
	
	// �������캯��
	CMmiReqNetTest(const CMmiReqNetTest& MmiReqNetTest);

	// ��ֵ����
	CMmiReqNetTest& CMmiReqNetTest::operator = (const CMmiReqNetTest& MmiReqNetTest);

	// ��������
	virtual ~CMmiReqNetTest();

public:
	virtual CString GetSummary();

	uint32			m_uiLen;
	char*			m_pData;
};

class DATAINFO_DLL_EXPORT CMmiRespNetTest : public CMmiCommBase
{
public:
	CMmiRespNetTest() { m_eCommType = ECTRespNetTest; m_uiTimeResp = -1; }
	virtual ~CMmiRespNetTest();
public:
	virtual CString GetSummary();
		
	uint32			m_uiTimeResp;
	uint32			m_uiLen;
	char*			m_pData;
};

////////////////////////////////////////////////////////////////////////////////
// ����������
class DATAINFO_DLL_EXPORT CMmiRespError : public CMmiCommBase
{
public:
	CMmiRespError() { m_eCommType = ECTRespError; m_eReqCommType = ECTCount; m_eRespErrorCode = ERENone; m_iCommunicationId = -1;}
	virtual ~CMmiRespError();

public:
	virtual CString	GetSummary();

private:
	const CMmiRespError& CMmiRespError::operator=(const CMmiRespError& Src);

public:
	CString			m_StrMerchCode;
	int32			m_iMarketId;

	E_CommType		m_eReqCommType;
	int32			m_iCommunicationId;

	CString			m_StrErrorMsg;
	E_RespErrorCode	m_eRespErrorCode;
};

////////////////////////////////////////////////////////////////////////////////
// ����Price
class DATAINFO_DLL_EXPORT CMmiPushPrice : public CMmiCommBase
{
public:
	CMmiPushPrice() { m_eCommType = ECTPushPrice; }
	virtual ~CMmiPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushPrice& CMmiPushPrice::operator=(const CMmiPushPrice& Src);
	
public:
	CArray<CRealtimePrice*, CRealtimePrice*> m_RealtimePriceListPtr;	// ��Ʒ5����������
};

// ����Tick
class DATAINFO_DLL_EXPORT CMmiPushTick : public CMmiCommBase
{
public:
	CMmiPushTick() { m_eCommType = ECTPushTick; }
	virtual ~CMmiPushTick() {}
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushTick& CMmiPushTick::operator=(const CMmiPushTick& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
		
	CArray<CTick, CTick> m_Ticks;		// 
};

// ����Level2
class DATAINFO_DLL_EXPORT CMmiPushLevel2 : public CMmiCommBase
{
public:
	CMmiPushLevel2() { m_eCommType = ECTPushLevel2; }
	virtual ~CMmiPushLevel2() {}
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiPushLevel2& CMmiPushLevel2::operator=(const CMmiPushLevel2& Src);
	
public:
	CRealtimeLevel2	m_RealtimeLevel2;		// ��Ʒlevel2����
};

class DATAINFO_DLL_EXPORT CMmiKickOut : public CMmiCommBase
{
public:
	CMmiKickOut() { m_eCommType = ECTKickOut; }

public:
	virtual CString GetSummary();

public:
	CString		m_strRespJson;		// ���ص�json��
};

class DATAINFO_DLL_EXPORT CMmiReqUploadData : public CMmiCommBase
{
public:
	CMmiReqUploadData(); 
	virtual ~CMmiReqUploadData();

	// ��������
	CMmiReqUploadData(const CMmiReqUploadData& MmiReqUploadData);

	// ��ֵ�����
	CMmiReqUploadData& CMmiReqUploadData::operator = (const CMmiReqUploadData& MmiReqUploadData);

public:
	virtual CString GetSummary();

public:
	uint32		m_uType;
	uint32		m_uLen;
	char*		m_pData;		
};

class DATAINFO_DLL_EXPORT CMmiRespUploadData : public CMmiCommBase
{
public:
	CMmiRespUploadData() { m_eCommType = ECTRespUploadData; }

public:
	virtual CString GetSummary();

public:
	uint32		m_uReserve;
};

class DATAINFO_DLL_EXPORT CMmiReqDownloadData : public CMmiCommBase
{
public:
	CMmiReqDownloadData() { m_eCommType = ECTReqDownloadData; }

public:
	virtual CString GetSummary();

public:
	uint32		m_uDataType;
	uint32		m_uReserve[4];		// ����
};

class DATAINFO_DLL_EXPORT CMmiRespDownloadData : public CMmiCommBase
{
public:
	CMmiRespDownloadData() ; 
	virtual ~CMmiRespDownloadData();

	// ��������
	CMmiRespDownloadData(const CMmiRespDownloadData& MmiReqUploadData);

	// ��ֵ�����
	CMmiRespDownloadData& CMmiRespDownloadData::operator = (const CMmiRespDownloadData& MmiReqUploadData);

public:
	virtual CString GetSummary();

public:
	uint32		m_uType;
	uint32		m_uLen;
	char*		m_pData;		
};

// ������ƷʵʱPrice����
class DATAINFO_DLL_EXPORT CMmiAddRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAddRegisterPushPrice() { m_eCommType = ECTAddRegisterPushPrice; }
	
public:
	virtual CString	GetSummary();
	
public:
	const CMmiAddRegisterPushPrice& operator=(const CMmiAddRegisterPushPrice& Src);
	
public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// Ӧ����ƷʵʱPrice��������
class DATAINFO_DLL_EXPORT CMmiAnsAddRegisterPushPrice : public CMmiCommBase
{
public:
	CMmiAnsAddRegisterPushPrice() { m_eCommType = ECTAnsAddRegisterPushPrice; }
	virtual ~CMmiAnsAddRegisterPushPrice();
	
public:
	virtual CString	GetSummary();
	
private:
	const CMmiAnsAddRegisterPushPrice& operator=(const CMmiAnsAddRegisterPushPrice& Src);
};

////////////////////////////////////////////////////////////////////////////////
// ���ýӿ�
DATAINFO_DLL_EXPORT CMmiCommBase*		NewCopyCommReqObject(CMmiCommBase *pCommBase);
DATAINFO_DLL_EXPORT bool32				IsSameReqObject(const CMmiCommBase *pCommSrc, const CMmiCommBase *pCommDst);		// �ж����������Ƿ�ͬ����ͬ����



/// 
typedef enum E_FormulaUpdateType
{
	// ϵͳָ��
	EFUTParamTempUpdate = 0,// ��ʱ�����޸�,��nameӰ�쵱ǰָ����
	EFUTParamUpdate,		// ϵͳ�����޸�,Ӱ������nameָ����
	EFUTIndexUpdate,		// nameָ���޸�
	EFUTIndexDelete,		// nameָ��ɾ��

	// ��� K ��
	EFUTClrKLineUpdate,		// ����
	EFUTClrKLineDelete,		// ɾ��

	// ����ѡ��
	EFUTCdtChoseUpdate,		// ����
	EFUTCdtChoseDelete,		// ɾ��

	// ����ָ��
	EFUTTradeChoseUpdate,	// ����
	EFUTTradeChoseDelete,    // ɾ��

} E_FormulaUpdateType;


//////////////////////////////////////////////////////////////////////////
// fangz 0811 �����¼�
//////////////////////////////////////////////////////////////////////////

// 1: ������������µ�

// �������ݽṹ (���Ƴ�Ȩ��Ϣ, ����ṹ�ڽ���������������������ʱ�õ�)
typedef struct T_FinanceData
{
	float   m_fAllCapical;		// �ܹɱ�
	float	m_fCircAsset;		// ��ͨ�ɱ�
	float	m_fAllAsset;		// ���ʲ�
	float	m_fFlowDebt;		// ������ծ
	float	m_fPerFund;			// ÿ�ɹ�����
	
	float	m_fBusinessProfit;	// Ӫҵ����
	float	m_fPerNoDistribute;	// ÿ��δ����
	float	m_fPerIncomeYear;	// ÿ������(��)
	float	m_fPerPureAsset;	// ÿ�ɾ��ʲ�
	float	m_fChPerPureAsset;	// ����ÿ�ɾ��ʲ�
	
	float	m_fDorRightRate;		// �ɶ�Ȩ���
	float	m_fCircMarketValue;		// ��ͨ��ֵ
	float	m_fAllMarketValue;		// ����ֵ

}T_FinanceData;

// �����߼�����ļ�
class DATAINFO_DLL_EXPORT CMmiReqLogicBlockFile : public CMmiCommBase
{
public:
	CMmiReqLogicBlockFile::CMmiReqLogicBlockFile() { m_eCommType = ECTReqLogicBlockFile; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqLogicBlockFile(const CMmiReqLogicBlockFile& Src);
	CMmiReqLogicBlockFile& operator=(const CMmiReqLogicBlockFile& Src);

public:
	unsigned long	m_uCRC32;		// �ͻ����������ݵ�CRC32ֵ
};

// Ӧ��
class DATAINFO_DLL_EXPORT CMmiRespLogicBlockFile : public CMmiCommBase
{
public:
	CMmiRespLogicBlockFile::CMmiRespLogicBlockFile() { m_eCommType = ECTRespLogicBlockFile; m_uiDataLen = 0; m_pData = NULL; }
	~CMmiRespLogicBlockFile();

	virtual CString	GetSummary(){return CString();}

public:
	char*		m_pData;
	uint32		m_uiDataLen;  // ����ͻ��ͷ���˵�CRCֵһ������szDataΪ0,0������ֻ�������ֽ�, ����
							  // szDataΪuDataLen,  szData��β������������'\0', 
							  // ע��: uDataLenҲ����������β�ַ�	
};

// ����������
// class CMmiReqBlockReport : public CMmiCommBase
// {
// public:
// 	CMmiReqBlockReport() { m_eCommType = ECTReqReport; }
// 	
// public:
// 	virtual CString	GetSummary()  {return L""; }
// 	
// 	CMmiReqBlockReport(const CMmiReqBlockReport& Src);
// 	const CMmiReqBlockReport& operator=(const CMmiReqBlockReport& Src);
// 		
// public:
// 	int32			m_iBlockId;					// �����
// 	E_MerchReportField	m_eMerchReportField;	// ��Ʒ���۱��ֶ�
// 	bool32			m_bDescSort;				// ����
// 	int32			m_iStart;
// 	int32			m_iCount;
// };

class DATAINFO_DLL_EXPORT CMmiReqBlockReport : public CMmiCommBase
{
public:
	CMmiReqBlockReport() { m_eCommType = ECTReqBlockReport; }
	virtual ~CMmiReqBlockReport(){}

public:
	CMmiReqBlockReport(const CMmiReqBlockReport& Src);
	const CMmiReqBlockReport& operator=(const CMmiReqBlockReport& Src);
	
	virtual CString GetSummary() { return L"123"; }

public:
	int32			m_iBlockId;					// �����
	E_MerchReportField	m_eMerchReportField;	// ��Ʒ���۱��ֶ�
	bool32			m_bDescSort;				// ����
	int32			m_iStart;
	int32			m_iCount;
};

// Ӧ����Ʒ���а�����
class DATAINFO_DLL_EXPORT CMmiRespBlockReport : public CMmiCommBase
{
public:
	CMmiRespBlockReport() { m_eCommType = ECTRespBlockReport; }
	virtual ~CMmiRespBlockReport(){}
	
public:
	virtual CString	GetSummary() {return L""; }
	
private:
	const CMmiRespBlockReport& CMmiRespBlockReport::operator=(const CMmiRespBlockReport& Src);
	
public:
	int32			m_iBlockId;		// �г����
	
	E_MerchReportField	m_eMerchReportField;
	bool32			m_bDescSort;		// �������У�
	int32			m_iStart;
	
	// ��Ʒ�б�
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// ��Ʒ5����������
};

//////////////////////////////////////////////////////////////////////////
// 0225 �¼ӵ�Э��:

// ����ָ����ͨ����
class DATAINFO_DLL_EXPORT CMmiReqGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqGeneralNormal() { m_eCommType = ECTReqGeneralNormal; }
	virtual ~CMmiReqGeneralNormal(){}

public:
	virtual CString GetSummary();
	CMmiReqGeneralNormal(const CMmiReqGeneralNormal& stSrc);
	CMmiReqGeneralNormal& CMmiReqGeneralNormal::operator=(const CMmiReqGeneralNormal& stSrc);

public:
	int32	m_iMarketId;
};

// �������ݸ�ʽ
typedef struct T_GeneralNormal
{
public:
	T_GeneralNormal()
	{
		Clear();	
	}

	void Clear()
	{
		memset(this, 0, sizeof(T_GeneralNormal));
	}

	long	m_lTime;					// ʱ��
	float	m_fRiseRateNoWeighted;		// ����Ȩ�ǵ���
	float	m_fAmountBuy;				// ���̳ɽ����ܺ�
	float	m_fVolumeBuy;				// ���̳ɽ����ܺ�
	float	m_fAmountSell;				// ���̳ɽ����ܺ�
	float	m_fVolumeSell;				// ���̳ɽ����ܺ�
	uint32	m_uiRiseMomentary;			// ˲ʱ���Ǽ���
	uint32	m_uiFallMomentary;			// ˲ʱ�µ�����	
	uint32	m_uiRiseCounts;				// ���Ǽ���
	uint32	m_uiFallCounts;				// �µ�����

	float	m_fBuy1Amount;				// ��1 ������ܺ�
	float	m_fBuy5Amount;				// ��5 ������ܺ�
	float	m_fSell1Amount;				// ��1 ������ܺ�
	float	m_fSell5Amount;				// ��5 ������ܺ�

}T_GeneralNormal;

class DATAINFO_DLL_EXPORT CMmiRespGeneralNormal : public CMmiCommBase
{
public:
	CMmiRespGeneralNormal() { m_eCommType = ECTRespGeneralNormal; }
	virtual ~CMmiRespGeneralNormal(){}
	
public:
	virtual CString GetSummary();
	CMmiRespGeneralNormal(const CMmiRespGeneralNormal& stSrc);
	CMmiRespGeneralNormal& CMmiRespGeneralNormal::operator=(const CMmiRespGeneralNormal& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CArray<T_GeneralNormal, T_GeneralNormal&> m_aGeneralNormal;
};

//////////////////////////////////////////////////////////////////////////
// ����ָ����������(�����ֵ��ͼ�����õ�)
class DATAINFO_DLL_EXPORT CMmiReqGeneralFinance : public CMmiCommBase
{
public:
	CMmiReqGeneralFinance() { m_eCommType = ECTReqGeneralFinance; }
	virtual ~CMmiReqGeneralFinance(){}
	
public:
	virtual CString GetSummary();
	CMmiReqGeneralFinance(const CMmiReqGeneralFinance& stSrc);
	CMmiReqGeneralFinance& CMmiReqGeneralFinance::operator=(const CMmiReqGeneralFinance& stSrc);
	
public:
	int32	m_iMarketId;
};

//

// �������ݸ�ʽ
typedef struct T_GeneralFinance
{
public:
	T_GeneralFinance()
	{
		memset(this, 0, sizeof(T_GeneralFinance));
	}

	// �̶�
	float	m_fAllStock;			// �ܹɱ�
	float	m_fCircStock;			// ����ͨ��
	float	m_fAvgPureRate;			// ƽ���о���
	float	m_fAvgPureGainRate;		// ƽ��������

	// ��䶯(��ʵʱ�۸��й�)
	float	m_fAllValue;			// ����ֵ	
	float	m_fCircValue;			// ����ͨ��ֵ
	float	m_fAvgPe;				// A��ƽ����ӯ��
	
}T_GeneralFinance;

class DATAINFO_DLL_EXPORT CMmiRespGeneralFinance : public CMmiCommBase
{
public:
	CMmiRespGeneralFinance() { m_eCommType = ECTRespGeneralFinance; }
	virtual ~CMmiRespGeneralFinance(){}
	
public:
	virtual CString GetSummary();
	CMmiRespGeneralFinance(const CMmiRespGeneralFinance& stSrc);
	CMmiRespGeneralFinance& CMmiRespGeneralFinance::operator=(const CMmiRespGeneralFinance& stSrc);
	
public:
	//
	int32	m_iMarketId;
	T_GeneralFinance m_stGeneralFinance;
};

//////////////////////////////////////////////////////////////////////////
// ������ͨ��Ʒ�ķ�ʱָ������
class DATAINFO_DLL_EXPORT CMmiReqMerchTrendIndex : public CMmiCommBase
{
public:	
	CMmiReqMerchTrendIndex() { m_eCommType = ECTReqMerchTrendIndex; }
	virtual ~CMmiReqMerchTrendIndex(){}
	
public:
	virtual CString GetSummary();
	CMmiReqMerchTrendIndex(const CMmiReqMerchTrendIndex& stSrc);
	CMmiReqMerchTrendIndex& CMmiReqMerchTrendIndex::operator=(const CMmiReqMerchTrendIndex& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiRespMerchTrendIndex() { m_eCommType = ECTRespMerchTrendIndex; }
	virtual ~CMmiRespMerchTrendIndex(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMerchTrendIndex(const CMmiRespMerchTrendIndex& stSrc);
	CMmiRespMerchTrendIndex& CMmiRespMerchTrendIndex::operator=(const CMmiRespMerchTrendIndex& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MerchTrendIndex, T_MerchTrendIndex&> m_aMerchTrendIndex;
};

//////////////////////////////////////////////////////////////////////////
// ���Ͼ�������
class DATAINFO_DLL_EXPORT CMmiReqMerchAution : public CMmiCommBase
{
public:	
	CMmiReqMerchAution() { m_eCommType = ECTReqMerchAuction; }
	virtual ~CMmiReqMerchAution(){}
	
public:
	virtual CString GetSummary();
	CMmiReqMerchAution(const CMmiReqMerchAution& stSrc);
	CMmiReqMerchAution& CMmiReqMerchAution::operator=(const CMmiReqMerchAution& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMerchAuction: public CMmiCommBase
{
public:
	CMmiRespMerchAuction() { m_eCommType = ECTRespMerchAuction; }
	virtual ~CMmiRespMerchAuction(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMerchAuction(const CMmiRespMerchAuction& stSrc);
	CMmiRespMerchAuction& CMmiRespMerchAuction::operator=(const CMmiRespMerchAuction& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MerchAuction, T_MerchAuction&> m_aMerchAuction;
}; 
//////////////////////////////////////////////////////////////////////////
// ָ������
class DATAINFO_DLL_EXPORT CMmiReqAddPushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqAddPushGeneralNormal() { m_eCommType = ECTReqAddPushGeneralNormal; }
	virtual ~CMmiReqAddPushGeneralNormal(){}

public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ������������ָ����ͨ����, �г���%d", m_iMarketId); return StrSummary; }

	int32 m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqUpdatePushGeneralNormal() { m_eCommType = ECTReqUpdatePushGeneralNormal; }
	virtual ~CMmiReqUpdatePushGeneralNormal(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ͬ������ָ����ͨ����, �г���%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushGeneralNormal : public CMmiCommBase
{
public:
	CMmiReqDelPushGeneralNormal() { m_eCommType = ECTReqDelPushGeneralNormal; }
	virtual ~CMmiReqDelPushGeneralNormal(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ɾ������ָ����ͨ����, �г���%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
};

//////////////////////////////////////////////////////////////////////////
// ��Ʒ��ʱָ����������

class DATAINFO_DLL_EXPORT CMmiReqAddPushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqAddPushMerchTrendIndex() { m_eCommType = ECTReqAddPushMerchTrendIndex; }
	virtual ~CMmiReqAddPushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ��������������Ʒ��ʱָ������, �г���%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqUpdatePushMerchTrendIndex() { m_eCommType = ECTReqUpdatePushMerchTrendIndex; }
	virtual ~CMmiReqUpdatePushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ͬ��������Ʒ��ʱָ������, �г���%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushMerchTrendIndex : public CMmiCommBase
{
public:
	CMmiReqDelPushMerchTrendIndex() { m_eCommType = ECTReqDelPushMerchTrendIndex; }
	virtual ~CMmiReqDelPushMerchTrendIndex(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ɾ��������Ʒ��ʱָ������, �г���%d", m_iMarketId); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

// �������ֲ�����
// ����ֲ�����
typedef struct T_FundHold
{
public:
	T_FundHold()
	{
		m_StrName		= L"";
		m_fHold			= 0.;
		m_fAllPercent	= 0.;
		m_fFundPercent	= 0.;
		m_fPreHold		= 0.;
		m_fPerCent		= 0.;
	}
	
	bool32 T_FundHold::operator < (const T_FundHold& stOther) const
	{
		return m_fPerCent <= stOther.m_fPerCent;
	}
	
	//
	CString	m_StrName;			// ��������
	float	m_fHold;			// �ֹ�����
	float	m_fAllPercent;		// ռ���й�˾�ֹ�
	float	m_fFundPercent;		// ռ����ֵ%
	float	m_fPreHold;			// ���ڳֹ�
	float	m_fPerCent;			// ����
	
}T_FundHold;

typedef std::set<T_FundHold> ListFundHold;
typedef std::map<CMerch*, ListFundHold> MapFundHold;

//
class DATAINFO_DLL_EXPORT CMmiReqFundHold : public CMmiCommBase
{
public:
	CMmiReqFundHold() { m_eCommType = ECTReqFundHold; m_uiCRC32 = 0; }
	virtual ~CMmiReqFundHold() {}

public:
	virtual CString GetSummary() { return L"�������ֲ�����"; }

	uint32			m_uiCRC32;
};

//
#pragma pack(push)		// �������״̬
#pragma pack(1)			// �趨Ϊ1�ֽڶ���

// �ܵİ�ͷ
typedef struct T_ClientRespFundHoldHead
{
	uint16		uDataType;		// ��������, E_ExDataType
	uint32		ulCRC32;		// CRCУ��, ���������������ͬ���򲻷�������
	uint32		uDataLen;		// ���ݳ���
	
}T_ClientRespFundHoldHead;

// ÿ����Ʒ������ͷ��
typedef struct T_ClientRespFundHoldMerchHead
{
	uint32	uSize;			// �˽ṹ��С
	uint32	uiMarket;		// �г�
	char	szCode[32];		// ����
	uint32	uCRC32;			// crcУ��
	uint32	uiDataLen;		// ����
	
}T_ClientRespFundHoldMerchHead;

// ÿ����Ʒ��������(ÿ���������Ϣ)
typedef struct T_ClientRespFundHoldMerchBody
{
	uint32	uSize;				// �˽ṹ��С
	char	szOrgName[32];		// ��������
	float	fHold;				// �ֹ�����
	float	fAllPercent;		// ռ���й�˾�ֹ�
	float	fFundPercent;		// ռ����ֵ%
	float	fPreHold;			// ���ڳֹ�
	float	fPerCent;			// ����
	
}T_ClientRespFundHoldMerchBody;

#pragma pack(pop)		// �ָ�Ĭ�ϵĶ��뷽ʽ

class DATAINFO_DLL_EXPORT CMmiRespFundHold : public CMmiCommBase
{
public:
	CMmiRespFundHold() { m_eCommType = ECTRespFundHold; m_uiDataLen = 0; m_pData = NULL; }
	virtual ~CMmiRespFundHold() { delete m_pData; m_pData = NULL; }

	virtual CString GetSummary() { return L"Ӧ�����ֲ�����"; }

private:
	CMmiRespFundHold& CMmiRespFundHold::operator=(const CMmiRespFundHold& Src);

public:
	uint32			m_uiCRC32;			// ����CRCУ��ֵ	0 - ��ʾ������������ 
	uint32			m_uiDataLen;
	char*			m_pData;
};

// ������Ʒ�ķ�������������
class DATAINFO_DLL_EXPORT CMmiReqMinuteBS : public CMmiCommBase
{
public:	
	CMmiReqMinuteBS() { m_eCommType = ECTReqMinuteBS; }
	virtual ~CMmiReqMinuteBS(){}
	
public:
	virtual CString GetSummary();
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiRespMinuteBS : public CMmiCommBase
{
public:
	CMmiRespMinuteBS() { m_eCommType = ECTRespMinuteBS; }
	virtual ~CMmiRespMinuteBS(){}
	
public:
	virtual CString GetSummary();
	CMmiRespMinuteBS(const CMmiRespMinuteBS& stSrc);
	CMmiRespMinuteBS& CMmiRespMinuteBS::operator=(const CMmiRespMinuteBS& stSrc);
	
public:
	//
	int32	m_iMarketId;
	CString m_StrMerchCode;
	CArray<T_MinuteBS, T_MinuteBS&> m_aMinuteBS;
};

// �������������ݵ�����
class DATAINFO_DLL_EXPORT CMmiReqAddPushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqAddPushMinuteBS() { m_eCommType = ECTReqAddPushMinuteBS; }
	virtual ~CMmiReqAddPushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ��������������Ʒ��������������, �г���%d ���� %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqUpdatePushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqUpdatePushMinuteBS() { m_eCommType = ECTReqUpdatePushMinuteBS; }
	virtual ~CMmiReqUpdatePushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ͬ��������Ʒ��������������, �г���%d ���� %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

class DATAINFO_DLL_EXPORT CMmiReqDelPushMinuteBS : public CMmiCommBase
{
public:
	CMmiReqDelPushMinuteBS() { m_eCommType = ECTReqDelPushMinuteBS; }
	virtual ~CMmiReqDelPushMinuteBS(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ����ɾ��������Ʒ��������������, �г���%d ���� %s", m_iMarketId, m_StrMerchCode); return StrSummary; }
	
	int32 m_iMarketId;
	CString m_StrMerchCode;
};

// �����֤
class DATAINFO_DLL_EXPORT CMmiReqAuthPlugInHeart : public CMmiCommBase
{
public:
	CMmiReqAuthPlugInHeart() { m_eCommType = ECTReqAuthPlugInHeart; }
	virtual ~CMmiReqAuthPlugInHeart(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ��������֤����"); return StrSummary; }
	
	string m_StrUseName;
	string m_StrSessionID;
	string m_StrDNS;
	string m_StrPoxyDNS;
	string m_StrProxyUserName;
	string m_StrProxyUserPwd;
	int32  m_iProxyType;
};

class DATAINFO_DLL_EXPORT CMmiRespAuthPlugInHeart : public CMmiCommBase
{
public:
	CMmiRespAuthPlugInHeart() { m_eCommType = ECTRespAuthPlugInHeart; }
	virtual ~CMmiRespAuthPlugInHeart(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ANS �ذ������֤����"); return StrSummary; }
	
	int32 m_iRet;
	CString m_StrUserName;
};

// ���⽻��ʱ��
class DATAINFO_DLL_EXPORT CMmiReqTradeTime : public CMmiCommBase
{
public:
	CMmiReqTradeTime() { m_eCommType = ECTReqTradeTime; }
	virtual ~CMmiReqTradeTime(){}
	
public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ASK ������Ʒ����ʱ��"); return StrSummary; }
	
	int32	m_iTradeTimeID;
};

class DATAINFO_DLL_EXPORT CMmiRespTradeTime : public CMmiCommBase
{
public:
	CMmiRespTradeTime() { m_eCommType = ECTRespTradeTime; }
	virtual ~CMmiRespTradeTime(){}
	
	const CMmiReqTradeTime& CMmiRespTradeTime::operator=(const CMmiReqTradeTime& stSrc);

public:
	virtual CString GetSummary() { CString StrSummary; StrSummary.Format(L"ANS ������Ʒ����ʱ��"); return StrSummary; }
	
	arrClientTradeTimeInfo	m_aClientTradeTimeInfo;
};

#endif

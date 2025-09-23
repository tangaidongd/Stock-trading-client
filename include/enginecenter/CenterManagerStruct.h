#ifndef _CENTER_MANAGER_STRUCT_H_
#define _CENTER_MANAGER_STRUCT_H_

#include "typedef.h"
#include "MerchManager.h"
#include "sharestruct.h"

enum E_MsgTypeResponse	  //��Ϣ֪ͨ����
{
	EMSG_OnMsgRealtimeTickUpdate    = 10,	    // ʵʱ�ֱʸ���
	EMSG_OnMsgRealtimePriceUpdate,					// ʵʱ����
	EMSG_OnMsgRealtimeLevel2Update,
	EMSG_OnMsgPublicFileUpdate,
	EMSG_OnMsgOnMerchTrendIndexUpdate,
	EMSG_OnMsgOnMerchMinuteBSUpdate,
	EMSG_OnMsgOnMerchAuctionUpdate,
	EMSG_OnMsgOnGeneralNormalUpdate,
	EMSG_OnMsgOnGeneralFinanaceUpdate,
	EMSG_OnMsgNewsServerDisconnected,
	EMSG_OnMsgNewsServerConnected,
	EMSG_OnMsgNewsResponse,
	EMSG_OnMsgNewsListUpdate,
	EMSG_OnMsgLandMineUpdate,
	EMSG_OnMsgKLineNodeUpdate,
	EMSG_OnMsgF10Update,
	EMSG_OnMsgClientTradeTimeUpdate,
	EMSG_OnMsgAuthPlugInHeartResp,
	EMSG_OnMsgTimesaleUpdate,
	EMSG_EMSGOnMsgViewDataOnTimerFreeMemory,
	EMSG_OnMsgViewDataOnTimerInitializeMarket,
	EMSG_OnMsgViewDataOnTimerSyncPushing
};




// �������ص��¼�֪ͨ
class  COffLineDataNotify
{
public:
	// ���󶼷�����
	virtual void OnAllRequestSended(E_CommType eType) = 0;

	// ��������
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType) = 0;

	// ���سɹ�
	virtual void OnDataDownLoadFinished(E_CommType eType) = 0;

	// ����ʧ��
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg) = 0;
};



// ���صĲ�������ṹ
typedef struct T_DownLoadOffLine
{
public:
	T_DownLoadOffLine()
	{
		Clear();
	}

	void Clear()
	{
		m_aMerchs.clear();
		m_eReqType  = ECTCount;
		m_TimeBeing = 0;
		m_TimeEnd	= 0;
		m_eKLineBase= EKTBCount;
	}

	//
	E_CommType		m_eReqType;		// ����
	vector<CMerch*> m_aMerchs;		// ��Ʒ
	CGmtTime		m_TimeBeing;	// ��ʼʱ��
	CGmtTime		m_TimeEnd;		// ��ֹʱ��
	E_KLineTypeBase m_eKLineBase;	// K ������

}T_DownLoadOffLine;


// �������������
typedef struct T_IndexChsStkRespData
{
public:
	int32					m_iRespID;			// ������ID ��
	int32					m_iMarketId;		// �г����
	CString					m_StrMerchCode;		// ��Ʒ����
	E_KLineTypeBase			m_eKLineTypeBase;	// �����K������
	CArray<CKLine, CKLine>	m_aKLines;			// K ��

	//
	T_IndexChsStkRespData()
	{
		m_iRespID		= -1;
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_eKLineTypeBase= EKTBDay;
		m_aKLines.RemoveAll();
	}

	//
	T_IndexChsStkRespData(const T_IndexChsStkRespData& Data)
	{
		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);
	}

	T_IndexChsStkRespData& operator= (const T_IndexChsStkRespData& Data)
	{
		if ( &Data == this )
		{
			return *this;
		}

		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);

		return *this;
	}

}T_IndexChsStkRespData;

enum E_AttendMerchType	  //��ע��Ʒ����
{
	EA_Arbitrage = 0,	   // ����
	EA_StatucBar ,		   // ״̬��
	EA_Alarm,				// Ԥ��
	EA_RecordData,			// ���ݿ�
	EA_Choose,				// ѡ��
	EA_SimulateTrade,		// ģ�⽻����Ʒ
};


enum E_LoginState
{
	ELSNotLogin = 0,	// δ��¼
	ELSAuthing,			// ��֤��
	ELSAuthSuccess,		// ��֤�ɹ�
	ELSAuthFail,		// ��֤ʧ��
	ELSDirectQuote,		// ֱ�����������
	ELSLogining,		// ��¼��
	ELSLoginSuccess,	// ��¼�ɹ�
	ELSLoginFail		// ��¼ʧ��
};

enum E_MerchManagerType		// merchmanager�г�ʼ������Ʒ�б�״̬
{
	EMMT_None = 0,		// ���κ�״̬, ��ʱ�ʹ���������~
	EMMT_StockCn = 1,	// �����ڹ�Ʊ
	EMMT_FutureCn = 2,	// �������ڻ�
};


// ��������
typedef struct T_NewsInfo
{
public:
	int32		m_iInfoIndex;		// ��Ѷ����ID
	int32		m_iIndexID;			// ��Ѷid
	CString		m_StrContent;		// ��������
	CString		m_StrTimeUpdate;	// ����ʱ��

}T_NewsInfo;

typedef CArray<T_NewsInfo, T_NewsInfo&> listNewsInfo;

#endif
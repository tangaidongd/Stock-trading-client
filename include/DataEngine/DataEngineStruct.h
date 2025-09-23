#ifndef _DATA_ENGINE_STRUCT_H_
#define _DATA_ENGINE_STRUCT_H_

#include "typedef.h"
#include "sharestruct.h"
#include "SocketClient.h"
#include "SharePubStruct.h"

//#define  MAX_NAME_LEN	64

enum E_ServerTypeByEngine
{
	enEngineSTNone		= 0,
	enEngineSTQuote		= 1, 	// ���������
	enEngineSTTrade		= 2, 	// ���׷�����
	enEngineSTNews		= 4,	// ��Ϣ������
	enEngineSTWarn		= 8,	// Ԥ��
	enEngineSTInfo		= 32,   // ��Ѷ������
};

// ����Դ
enum E_DataSource
{
	EDSAuto = 0,							// Ĭ�Ϸ�ʽ�� �����п��ܴ����ѻ����ݵ����� ���Ȳ����ѻ�����
	EDSVipData,								// ������ʱ������ȡ
	EDSCommunication,						// ��������ȡ����
	EDSOfflineData,							// �����ѻ��ļ���ȡ (EDSVipData EDSOfflineData Ҫ�ϲ�, ��ʷԭ��, �Ƚϻ���)	

	EDSCounts,
};


// ��֤״̬
enum E_AuthState
{
	EASAuthSuccessed = 0,		// �ɹ�
	EASAuthing,					// ������֤
	EASAuthFailed,				// 

	EASAuthCount
};

// ����״̬
enum E_Connect_Status
{
	EG_CNT_STATUS_CONNECT  = 1,
	EG_CNT_STATUS_DIS_CONNECT,
	EG_CNT_STATUS_COUNT
};


enum E_COMMSTAGE
{
	ECSTAGE_LoginInit = 0,		// ���ڵ�¼��ʼ��������
	ECSTAGE_QuoteSupplier,		// �����ṩ��
	ECSTAGE_COUNT
};

//
//// �����������Ϣ
//class  CProxyInfo
//{
//public:
//	// ��������
//	enum E_ProxyType
//	{
//		EPTNone = 0,	// ֱ��
//		EPTHttp,		// http����
//		EPTSock4,		// SOCK4����
//		EPTSock5,		// SOCK5����
//		EPTCount
//	};
//public:
//	CProxyInfo() { m_eProxyType = EPTNone; }
//	~CProxyInfo() { }
//public:
//	bool32 IsUseProxy()
//	{
//		if (m_eProxyType <= EPTNone || m_eProxyType >= EPTCount)
//			return false;
//		return true;
//	}
//public:
//	E_ProxyType		m_eProxyType;
//	CString			m_StrProxyAddr;
//	int32			m_iProxyPort;
//	CString			m_StrProxyUser;
//	CString			m_StrProxyPassword;
//};



// ÿ��ҵ�������¼
class  AFX_API_EXPORT CMmiReqNode
{
public:
	CMmiReqNode();
	~CMmiReqNode();
public:
	// ��¼��ԭʼ����
	CMmiCommBase	*m_pMmiCommReq;			// ����ԭʼ����������
	E_DataSource	m_eDataSource;			// ������������Դ
	int32			m_iMmiReqId;			// ����Ӧ���
	int32			m_iCommSerialNo;		// ��������ˮ�ţ� ����EDSCommunication��Ч
	int32			m_iCommunicationId;		// ͨѶ��
public:
	uint32			m_uiTimeRequest;		// ���������ʱ�䣬 ���ڳ�ʱ���� ���ڳ���һ��ʱ��û��Ӧ������� ���ǽ��ò��ٵȴ���Ӧ��
};

// ÿ��ҵ��Ӧ���¼
class  AFX_API_EXPORT CMmiRespNode
{
public:
	CMmiRespNode();
	~CMmiRespNode();
public:
	// ��¼��Ӧ������
	CMmiCommBase	*m_pMmiCommResp;
	int32			m_iMmiRespId;			// Ӧ���
public:
	int32			m_iCommunicationId;		// ������id
	uint32			m_uiTimeResponse;		// �÷�����ʱ��
};





// ������״̬�ṹ��
class  CServerState
{
public:
	bool32		m_bConnected;			// �Ƿ���������
	E_AuthState	m_eAuthState;			// ��֤״̬

	uint32		m_uiKeepSecond;			// �������Ͽ�ʼ������ά��ʱ�䣨�룩
	uint32		m_uiSendBytes;			// ��������
	uint32		m_uiRecvBytes;			// ��������
	uint32		m_uiPingValue;			// pingֵ�� ��Ӧ��̨���������ٶ�

	CString		m_StrServerName;		// ����
	CString		m_StrServerAddr;		// Ip ��ַ

	int32		m_iCommunicationID;		// m_iCommunicationID
	int32		m_iServerID;			// ID
	uint32		m_uiServerPort;			// �˿�
	uint32		m_uiServerHttpPort;		// HTTP�˿�

	int32		m_iOnLineCount;			// ������
	int32		m_iMaxCount;			// ���������
	int32		m_iGroup;				// ������
	int32		m_iNetType;				// ����������������

	uint32		m_iSortIndex;			// ��������
	int32		m_iSortScore;			// ����÷�

};


// ͨѶ����࣬ �����˶��ⲿģ����ṩ�Ĺ��ܽӿ�
class  CAbsCommunicationShow
{
public:
	virtual void	GetServerSummary(OUT CString &StrAddr, OUT uint32 &uiPort, OUT CProxyInfo &ProxyInfo) = 0;
	virtual void	GetServerState(OUT CServerState &ServerState) = 0;
};

//
//// ��������Ϣ
//typedef struct _ServerInfo
//{
//	int				iID;						// ������ID
//	E_ServerTypeByEngine	enType;				// ����������
//	wchar_t			wszName[MAX_NAME_LEN];			// ����
//	wchar_t			wszAddr[32];				// IP
//	int				iPort;						// �˿�
//	int				iHttpPort;					// HTTP�˿�
//	int				iOnLineCount;				// ������
//	int				iMaxCount;					// ���������
//	int				iSerNetID;					// ����������������ID
//	wchar_t			wszSerKey[MAX_NAME_LEN];
//	int				iGroup;						// ������
//
//	_ServerInfo()
//	{
//		iID					= 0;
//		enType				= enEngineSTNone;
//		wszName[0]			= 0;
//		wszAddr[0]			= 0;
//		wszSerKey[0]		= 0;
//		iPort				= 0;
//		iHttpPort			= 0;
//		iOnLineCount		= 0;
//		iMaxCount			= 0;
//		iSerNetID			= 0;
//		iGroup				= 1;
//	};
//
//	_ServerInfo(const _ServerInfo& serverinfo)
//	{
//		iID			= serverinfo.iID;
//		enType		= serverinfo.enType;
//		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
//		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
//		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
//		iPort		= serverinfo.iPort;
//		iHttpPort	= serverinfo.iHttpPort;
//		iOnLineCount = serverinfo.iOnLineCount;
//		iMaxCount	= serverinfo.iMaxCount;
//		iSerNetID	= serverinfo.iSerNetID;
//		iGroup		= serverinfo.iGroup;	
//	}
//
//	_ServerInfo& operator=(const _ServerInfo& serverinfo)//���������  
//	{  
//		if ( this == &serverinfo )
//		{
//			return *this;
//		}
//
//		iID			= serverinfo.iID;
//		enType		= serverinfo.enType;
//		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
//		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
//		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
//		iPort		= serverinfo.iPort;
//		iHttpPort	= serverinfo.iHttpPort;
//		iOnLineCount = serverinfo.iOnLineCount;
//		iMaxCount	= serverinfo.iMaxCount;
//		iSerNetID	= serverinfo.iSerNetID;
//		iGroup		= serverinfo.iGroup;
//
//		return *this;
//	} 
//
//}T_ServerInfo;


#endif
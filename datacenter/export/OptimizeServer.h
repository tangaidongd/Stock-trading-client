// OptimizeServer.h: interface for the COptimizeServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPTIMIZESERVER_H_
#define _OPTIMIZESERVER_H_
#include "DataCenterExport.h"

#include "communication.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TiXmlDocument;
class TiXmlElement;
class CViewData;


// ping �Ķ�ʱ��  ��� 5 s
const int32	KiTimerIdPing				= 61215891;				
const int32	KiTimerPeriodPing			= 1500;	


typedef CArray<CServerState, CServerState&> arrServer;

// ��������� ping ����Ϣ�ṹ
typedef struct T_ServerSortPing
{
	int32			m_iReqID;				// ���� ID
	bool32			m_bCompeleted;			// �Ƿ����
	bool32			m_bTimeOut;				// �Ƿ�ʱ

	CServerState	m_stServer;
	DWORD			m_TimeReq;				// �������ʱ��
	DWORD			m_TimeResp;				// ���������ʱ��

	T_ServerSortPing()
	{
		m_iReqID		= -1;
		m_bCompeleted	= false;
		m_bTimeOut		= false;

		m_TimeReq		= -1;
		m_TimeResp		= -1;
	}

}T_ServerSortPing;

class DATACENTER_DLL_EXPORT COptimizeServerNotify
{
public: 
	virtual void  OnFinishSortServer(){}								// ������ɵ�ʱ��, 
	virtual void  OnFinishAllPing(){}									// ping ��ɵ�ʱ��. 	
	virtual void  OnFinishPingServer(CServerState stServer){}			// ping ��һ̨������
};

// ��ѡ��վ
class DATACENTER_DLL_EXPORT COptimizeServer
{
public:
	enum E_NetType
	{
		ENTDianxin = 1,
		ENTWangtong,
		ENTYidong,
		ENTLiantong,
		ENTOther,

		ENTCount,
	};

private:
	COptimizeServer();

public:
	void			Construct();
	virtual ~COptimizeServer();

public:
	// ����ģʽ
	static COptimizeServer* Instance();
	
	static void				DelInstance();

	// ����������
	void			SetOwner(HWND hWnd) { if (NULL == hWnd)return; m_hOwnWnd = hWnd; }

	// ���ӹ۲���
	void			AddNotify(COptimizeServerNotify* pNotify);
	
	// ɾ���۲���
	void			RemoveNotify(COptimizeServerNotify* pNotify);
	
	// ������������
	void			SetNetType(E_NetType eNetType);

	// �����Ƿ��ֶ�����
	void			SetSortMode(bool32 bAuto);

	// �õ�����ģʽ
	bool32			BeAutoSortMode();

	// ��������Ĳ���
	void			SetSortParams(const T_ServerInfo* pServerFromAuth, int32 iServerCounts);

	// �޳����Ӳ��ϵ����������
	void			DelNonConnectServer(const T_ServerInfo* pServerNonConnect, int32 iServerCounts);

	// ��������������
	int32			CalcServerScore(const CServerState& stServer);			

	// ���������������
	bool32			SortQuoteServers();								

	// ��ping �����������
	bool32			SortQuoteServersByPing();	

	// �Ի����ֶ�����Ľ��
	void			SetManualSortResult(IN const arrServer& aServers);

	// ���� ping �ı�־ (�Ƿ��ֶ���ѡ�Ľ���ǿ����ping)
	void			SetForcePingFlag(bool32 bForcePing);

	// �õ� ping �ı�־
	bool32			GetForcePingFlag();

	// ���� ping ����
	bool32			RequestNetTest(IN const CMmiReqNetTest *pMmiReqNetTest, IN int32 iCommunicationID, OUT int32& iReqID);

	// �յ� ping �Ļذ�
	void			OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest);

	// �������з�����������- -��֤���÷����������ӿ��ܲ��ȶ�
	void			OnDataServerDisconnected(int32 iCommunicationId);
	
	// �ֶ������ ��ping
	void			ForceRePingServer();

	// �õ�ping �����ֵ
	int32			GetMaxPingValue();
	
	// �õ������ķ�����
	void			GetSortedServers(OUT arrServer& aServers);

	// �õ��������͵��ַ���
	CString			GetNetTypeString(E_NetType eNetType, bool32 bDetail = false);
	
	// ���رȽϽ��
	bool32			BeLocalAndAuthServersIdentical();				

	// �Ƚ������������Ƿ�һ��
	bool32			BeServerIdentical(const CServerState& ServerState1, const CServerState& ServerState2);

	// �Ƿ��������
	bool32			BeSortCompleted();

	// ������ʱ��
	void			StartMyTimer();

	// �رն�ʱ��
	void			StopMyTimer();

	// ��ʱ������
	void			OnMyTimer(int32 iTimerId);

	// ��������������Ϣ
	void			ClearErrServer();

	// �����������Ϣ
	bool32			SaveErrServer(CServerState& stServer);

	void			SetViewData(CViewData* pViewData);

private:
	
	// Ping ������
	void			PingAllServers();

	// Ping ����
	void			OnPingCompleted();

	// �����ź���
	void			OnSortCompleted();

	// ���ļ��ж�ȡ��������Ϣ
	bool32			GetServerInfoFromXml();							
	
	// ����XML �ڵ��ȡ��������Ϣ
	bool32			GetServerInfo(const TiXmlElement* pElement, arrServer& aServers);

	// �Ƚϱ��صĺ���֤���صķ�������Ϣ
	bool32			JudgeLocalAndAuthServers();		

	// �������ļ�����Ϣ���������
	bool32			SortQuoteServersByXmlFile(IN arrServer& aServerIN, OUT arrServer& aServerOUT);	

	// �õ���������xml �ַ���
	CString			GetCommunicationXmlString(const CServerState& stServer);
	
	// ����ε���Ϣ���浽�ļ� 
	bool32			SaveServerInfoToXml();	

	// ������������ת��
	void			T_ServerInfo2ServerState(IN const T_ServerInfo& serIn, OUT CServerState& serOut);
	
	// ÿ�μ����ʱ��, ��ǰ������Ҫȡ���µ�, ����֤���ص���Ϣ
	void			SetLatestOnlineData(CServerState& stServer);			

	// ���յ������㷨, ����ping ��type ���, ���жϷ����������ʵ�����
	bool32			SortServers(IN arrServer& aServerIN, OUT arrServer& aServerOUT);

	// ��֤�������������, ���̫����, ��������, ��֤����������ǲ��Ǵ���ķ�����
	bool32			FinalCheckServer(INOUT arrServer& aServers);

	// �Ƿ����ķ�����
	bool32			BeErrServer(const CServerState& st);

	// ��ȡ�����������Ϣ
	bool32			GetErrServer();



private:
	static	COptimizeServer* m_pThis;									// ָ���Լ���ָ��
	TiXmlDocument*			 m_pXmlDoc;									// Xml ���ĵ���ָ��
	E_NetType				 m_eNetType;								// ��������
	
	bool32					 m_bAutoOptimize;							// �Ƿ��Զ���ѡ
	bool32					 m_bLocalAndAuthIdentical;					// ���صķ�������Ϣ����֤����Ϣ�Ƿ�һ��
	bool32					 m_bPingCompleted;							// �Ƿ�ping ����
	bool32					 m_bSortCompleted;							// �Ƿ��������
	bool32					 m_bForcePing;								// �Ƿ����ֶ����²���

	CViewData*				 m_pViewData;

	//
	CArray<COptimizeServerNotify*,COptimizeServerNotify*> m_apNotify;	// ��֪ͨ��ģ��

	arrServer m_aServersFromXmlManual;	// �ļ��ж�ȡ�ķ�������Ϣ(�ֶ�)	
	arrServer m_aServersFromXmlAuto;	// �ļ��ж�ȡ�ķ�������Ϣ(�Զ�)

	arrServer m_aServersFromAuth;		// ��֤���صķ�������Ϣ	
	arrServer m_aServersAftSort;		// �����ķ�������Ϣ

	arrServer m_aErrServer;				// ��ѡ��, ��¼��������ķ�����

	CArray<T_ServerSortPing, T_ServerSortPing&> m_aServerPingInfo;		// ������ ping �����	

	HWND					m_hOwnWnd;									// ���ھ��, �û���ʱ��
};

#endif // _OPTIMIZESERVER_H_

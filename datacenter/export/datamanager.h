#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_

#include "typedef.h"
#include "sharestruct.h"
#include "synch.h"
//#include "CommonInterface.h"
#include "ShareFun.h"
#include "commmanager.h"
//#include "AbsOfflineDataManager.h"

#include "XTimer.h"
#include "DataCenterExport.h"

class CAbsCommManager;
class CCommManager;
class CAbsOfflineDataManager;
////////////////////////////////////////////////////////////////////////
// ����Դ
enum E_DataSource
{
	EDSAuto = 0,							// Ĭ�Ϸ�ʽ�� �����п��ܴ����ѻ����ݵ����� ���Ȳ����ѻ�����
	EDSVipData,								// ������ʱ������ȡ
	EDSCommunication,						// ��������ȡ����
	EDSOfflineData,							// �����ѻ��ļ���ȡ (EDSVipData EDSOfflineData Ҫ�ϲ�, ��ʷԭ��, �Ƚϻ���)	

	EDSCounts,
};

// ÿ��ҵ�������¼
class DATACENTER_DLL_EXPORT CMmiReqWaitingNode
{
public:
	CMmiReqWaitingNode();
	~CMmiReqWaitingNode();

public:
	// ��¼��ԭʼ����
	CMmiCommBase	*m_pMmiCommReq;			// ����ԭʼ����������
	E_DataSource	m_eDataSource;			// ������������Դ
	int32			m_iCommunicationId;		// ͨѶ������id

public:
	uint32			m_uiTimeRequest;		// ���������ʱ��,  ÿ�����󶼱��뾭��һ����ʱ����ܴ���
};

// ÿ��ҵ�������¼
class DATACENTER_DLL_EXPORT CMmiReqNode
{
public:
	CMmiReqNode();
	~CMmiReqNode();

public:
	// ��¼��ԭʼ����
	CMmiCommBase	*m_pMmiCommReq;			// ����ԭʼ����������
	E_DataSource	m_eDataSource;			// ������������Դ

	// 
	int32			m_iMmiReqId;			// ����Ӧ���
	int32			m_iCommSerialNo;		// ��������ˮ�ţ� ����EDSCommunication��Ч
	int32			m_iCommunicationId;		// ͨѶ��
	
public:
	uint32			m_uiTimeRequest;		// ���������ʱ�䣬 ���ڳ�ʱ���� ���ڳ���һ��ʱ��û��Ӧ������� ���ǽ��ò��ٵȴ���Ӧ��
};

// ÿ��ҵ��Ӧ���¼
class DATACENTER_DLL_EXPORT CMmiRespNode
{
public:
	CMmiRespNode();
	~CMmiRespNode();

public:
	// ��¼��Ӧ������
	CMmiCommBase	*m_pMmiCommResp;

	//
	int32			m_iMmiRespId;			// Ӧ���

public:
	int32			m_iCommunicationId;		// ������id
	uint32			m_uiTimeResponse;		// �÷�����ʱ��
};


// д�ѻ�K �ߵĲ����ṹ
typedef struct T_OffLineKLineParam
{
	friend class CDataManager;

public:
	T_OffLineKLineParam()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_eKLineTypeBase= EKTBDay;
		m_aKLines.RemoveAll();
	}

	T_OffLineKLineParam(int32 iMarketId, const CString& StrMerchCode, E_KLineTypeBase eKLineTypeBase, const CArray<CKLine, CKLine>& aKLines)
	{
		m_iMarketId		= iMarketId;
		m_StrMerchCode	= StrMerchCode;
		m_eKLineTypeBase= eKLineTypeBase;
		
		m_aKLines.SetSize(aKLines.GetSize());
		
		if ( aKLines.GetSize() > 0 )
		{
			memcpyex((CKLine*)m_aKLines.GetData(), (const CKLine*)aKLines.GetData(), aKLines.GetSize() * sizeof(CKLine));
		}		
	}

	T_OffLineKLineParam& T_OffLineKLineParam::operator = (const T_OffLineKLineParam& stOffLineKLineParam)
	{
		if ( &stOffLineKLineParam == this )
		{
			return *this;
		}

		m_iMarketId		= stOffLineKLineParam.m_iMarketId;
		m_StrMerchCode	= stOffLineKLineParam.m_StrMerchCode;
		m_eKLineTypeBase= stOffLineKLineParam.m_eKLineTypeBase;
		
		m_aKLines.SetSize(stOffLineKLineParam.m_aKLines.GetSize());
		
		if ( stOffLineKLineParam.m_aKLines.GetSize() > 0 )
		{
			memcpyex((CKLine*)m_aKLines.GetData(), (const CKLine*)stOffLineKLineParam.m_aKLines.GetData(), stOffLineKLineParam.m_aKLines.GetSize() * sizeof(CKLine));
		}

		return *this;
	}

private:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	E_KLineTypeBase m_eKLineTypeBase;
	CArray<CKLine, CKLine> m_aKLines;

}T_OffLineKLineParam;


// д�ѻ� Tick �Ĳ����ṹ
typedef struct T_OffLineTickParam
{
	friend class CDataManager;

public:
	T_OffLineTickParam()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_aTicks.RemoveAll();
	}
	
	T_OffLineTickParam(int32 iMarketId, const CString& StrMerchCode, const CArray<CTick, CTick>& aTicks)
	{
		m_iMarketId		= iMarketId;
		m_StrMerchCode	= StrMerchCode;
		
		m_aTicks.SetSize(aTicks.GetSize());
		
		if ( aTicks.GetSize() > 0 )
		{
			memcpyex((CTick*)m_aTicks.GetData(), (const CTick*)aTicks.GetData(), aTicks.GetSize() * sizeof(CTick));
		}		
	}

	T_OffLineTickParam& T_OffLineTickParam::operator= (const T_OffLineTickParam& stOffLineTickParam)
	{
		if ( this == &stOffLineTickParam )
		{
			return *this;
		}

		m_iMarketId		= stOffLineTickParam.m_iMarketId;
		m_StrMerchCode	= stOffLineTickParam.m_StrMerchCode;
		m_aTicks.Copy(stOffLineTickParam.m_aTicks);

		m_aTicks.SetSize(stOffLineTickParam.m_aTicks.GetSize());
		
		if ( stOffLineTickParam.m_aTicks.GetSize() > 0 )
		{
			memcpyex((CTick*)m_aTicks.GetData(), (const CTick*)stOffLineTickParam.m_aTicks.GetData(), stOffLineTickParam.m_aTicks.GetSize() * sizeof(CTick));
		}

		return *this;
	}
private:
	int32		m_iMarketId;
	CString		m_StrMerchCode;	
	CArray<CTick, CTick> m_aTicks;

}T_OffLineTickParam;

// �ѻ��������ݽṹ
typedef struct T_OffLineRealtimePriceParam
{
	friend class CDataManager;

public:
	T_OffLineRealtimePriceParam()
	{
	
	}

	~T_OffLineRealtimePriceParam()
	{		
	}

	//
	T_OffLineRealtimePriceParam(const CRealtimePrice& RealtimePrice)
	{
		m_RealtimePrice = RealtimePrice;
	}

	//
	T_OffLineRealtimePriceParam& T_OffLineRealtimePriceParam::operator=(T_OffLineRealtimePriceParam& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}

		//
		m_RealtimePrice = stSrc.m_RealtimePrice;
		return *this;
	}

private:
	CRealtimePrice m_RealtimePrice;

}T_OffLineRealtimePriceParam;
////////////////////////////////////////////////////////////////////////////////////
// ���ݹ���֪ͨ
class DATACENTER_DLL_EXPORT CDataManagerNotify
{
public:
	virtual void	OnDataServiceConnected(int32 iServiceId) = 0;
	virtual void	OnDataServiceDisconnected(int32 iServiceId) = 0;
	virtual void	OnDataRequestTimeOut(int32 iMmiReqId) = 0;
	virtual void	OnDataCommResponse() = 0;
	virtual void	OnDataServerConnected(int32 iCommunicationId) = 0;
	virtual void	OnDataServerDisconnected(int32 iCommunicationId) = 0;
	virtual void	OnDataSyncServerTime(CGmtTime &TimeServerNow) = 0;
	virtual void	OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType) = 0;
};


// ���ݹ�����࣬ �����˶����ṩ�Ĺ��ܽӿ�
class DATACENTER_DLL_EXPORT CAbsDataManager
{
public:
	// �������ݣ� �����ݿ������ѻ������У� Ҳ���ܴ����ڷ�����
	// ���ᴦ������ <0 ��ʾ����ʧ�� =0 ��������� >0 �Ѿ����뷢�Ͷ���
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1) = 0;
	
	//
	virtual int32	GetRespCount() = 0;
	virtual CMmiRespNode*	PeekAndRemoveFirstResp() = 0;

	//
	virtual void	AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;
	virtual void	RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	//
	virtual void	RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId) = 0;

	//
	virtual void	ForceRequestType(E_DataSource eDataSource) = 0;
};

//
class DATACENTER_DLL_EXPORT CDataManager : public CCommManagerNotify, public CAbsDataManager, public CXTimerAgent
{
public:
	CDataManager(CAbsCommManager *pCommManager, CAbsOfflineDataManager &OfflineDataManager);
	~CDataManager();

public:
	void GetLocalTime(CString &strTime);
	bool32			Construct();
	
	// from CAbsDataManager
public:
	// �������ݣ� �����ݿ������ѻ������У� Ҳ���ܴ����ڷ�����
	// ���ᴦ������ <0 ��ʾ����ʧ�� =0 ��������� >0 �Ѿ����뷢�Ͷ���
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1);
	
	virtual int32	GetRespCount();
	virtual CMmiRespNode*	PeekAndRemoveFirstResp();

	//
	void			AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify);
	void			RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify);

	// �����ѷ��Ͷ���, ɾ����Ӧ���������ѷ�����
	void			RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId);

	//
	void			ForceRequestType(E_DataSource eDataSource);

	// from CCommManagerNotify
public:
	virtual void	OnCommDataServiceConnected(int32 iServiceId);
	virtual void	OnCommDataServiceDisconnected(int32 iServiceId);
	virtual void	OnCommResponse(int32 iCommSerialNo, int32 iCommunicationId, IN CMmiCommBase *pMmiCommResp);
	virtual void	OnCommServerConnected(int32 iCommunicationId);
	virtual void	OnCommServerDisconnected(int32 iCommunicationId);
	virtual void	OnCommServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);	

	// from CXTimerAgent
public:
	virtual	void	OnXTimer(int32 iTimerId);

public:
	void			SetCommManager(CCommManager *pCommManager) { m_pCommManager = (CAbsCommManager *)pCommManager; }

	int32			GetMmiReqId()	{ return m_iLastMmiReqId++; }


private:
	// ��ʱ����ʱ��û�л�Ӧ�������
	void			OnTimerClearMmiReqList();
	
	// ��ʱ�����ѻ����������, ʵ���첽֪ͨ��Ŀ��
	void			OnTimerProcessOfflineReq();

	// ͳ��ÿ��communication �ж��ٸ���ʱ������
	void			DealTimeOutInfomations(CArray<CPoint, CPoint>& aTimeOutReqPairs, CMmiReqNode* pReqNode);

	// ��ʱ�����������
	void			OnTimerProcessMmiReqWaitingList();	// �����ܵ��Ż�
	
	// ����֪ͨ�¼�����
	void			NotifyDataServiceConnected(int32 iServiceId);
	void			NotifyDataServiceDisconnected(int32 iServiceId);
	void			NotifyDataRequestTimeOut(int32 iMmiReqId);
	void			NotifyDataCommResponse();
	void			NotifyServerConnected(int32 iCommunicationId);
	void			NotifyServerDisconnected(int32 iCommunicationId);
	void			NotifyDataSyncServerTimer(CGmtTime &TimeServer);
	void			NotifyServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);		

	// ���������
	// �������ݣ� �����ݿ������ѻ������У� Ҳ���ܴ����ڷ�����
	// ����ҵ��ͨѶ�ţ� <0 ��ʾ����ʧ��
	void			RequestDataImmediately(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1);

	void			PushReqIntoWaitingQueue(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId);
	void			AppendWaitingReq(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId);
	bool32			RequestDataOnlyFromCommunication(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut, int32 iCommunicationId = -1);
	bool32			RequestDataOnlyFromVipData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut);
	bool32			RequestDataOnlyFromOfflineData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut);
	
	// �Լ�������ķ��������������ӹ�(��Ҫ������������ܲ�������Ӧ���)
	void			RepairResponseData(CMmiCommBase *pMmiCommReq, CMmiCommBase *pMmiCommResp);
	
	// ͬ��������ʱ��
	void			PreTranslateSycnTime(uint32 uiTimeRequest, uint32 uiTimeResponse, uint32 uiTimeServer);

	// �����ѽ��ܶ��У� ������ڸ÷��������͵�ʵʱ���ݣ� ��ɾ���� ������ServerDisconnect��Ϣ��CommResponse��Ϣǰ��˳���������ϲ��ʵʱ���ݴ�����߼�����
	void			RemoveRealtimeDataRespOfSpecifyServer(int32 iCommunicationId);

	// ���������в���
private:
	void			RemoveAllWaitingReq();

	// ��������в���
private:
	void			RemoveAllReq();

	// Ӧ����в���
private:
	void			AddResp(CMmiRespNode *pMmiRespNode, bool32 bOfflineData = false);
	void			RemoveAllResp();

private:
	// ���������ѻ����ݵ��߳�
	bool32			BeginThreadProcessOffLineData();
	
	// �ѻ� K ��
	static void	    ThreadProcessOffLineKLine(LPVOID lParam);			
	void			ThreadProcessOffLineKLine();			
	
	void			PushOffLineKLineList(T_OffLineKLineParam& stOffLineKLineParam);
	
	// �ѻ� Tick
	static void		ThreadProcessOffLineTick(LPVOID lParam);
	void			ThreadProcessOffLineTick();

	void			PushOffLineTickList(T_OffLineTickParam& stOffLineTickParam);

	// �ѻ� RealtimePrice
	static void		ThreadProcessOffLineRealtimePrice(LPVOID lParam);
	void			ThreadProcessOffLineRealtimePrice();
	
	void			PushOffLineRealtimePriceList(T_OffLineRealtimePriceParam& stOffLineRealtimePriceParam);

private:
	bool32			Lock(HANDLE hMutex);
	void			UnLock(HANDLE hMutex);

	
//////////////////////////////////////////////////////////
// ����
private:
	uint32			m_iLastMmiReqId;

private:
	CArray<CMmiReqWaitingNode*, CMmiReqWaitingNode*>	m_aMmiReqWaitingListPtr;	// �������У� �����������ʷ���������ִ���
	CArray<CMmiReqNode*, CMmiReqNode*>					m_MmiReqListPtr;			// �ѷ��������
	//lint -sem(m_MmiRespListPtr::Add, custodial(1))
	CArray<CMmiRespNode*, CMmiRespNode*>				m_MmiRespListPtr;			// Ӧ�����

private:
	CAbsCommManager	*m_pCommManager;				// CommManager��������
	CAbsOfflineDataManager	&m_OfflineDataManager;	// �ѻ����ݲ�������

private:
	CArray<CDataManagerNotify*, CDataManagerNotify*> m_pDataManagerNotifyPtrList;

private:
	E_DataSource	m_eForceDataSource;				// ǿ����ĳ�ַ�ʽ��������

private:
	LockSingle		m_LockReqWaitingList;
	LockSingle		m_LockReqList;
	LockSingle		m_LockAnsList;

	HANDLE			m_hMutexReqWaitingList;
	HANDLE			m_hMutexReqList;
	HANDLE			m_hMutexRespList;

private:
	LockSingle		m_LockOffLineKLine;
	LockSingle		m_LockOffLineTick;
	LockSingle		m_LockOffLineRealtimePrice;

	HANDLE			m_hThreadOffLineKLine;
	HANDLE			m_hThreadOffLineTick;
	HANDLE			m_hThreadOffLineRealtimePrice;

	bool32			m_bThreadExit;
	// �ȴ�ThreadProcessOffLineTick�߳��˳�
	HANDLE			m_hEventThreadOffLineTick;

	// �ȴ�ThreadProcessOffLineKLine�߳��˳�
	HANDLE			m_hEventThreadOffLineKLine;

	// �ȴ�ThreadProcessOffLineRealtimePrice�߳��˳�
	HANDLE m_hThreadProcessOffLineRealtimePrice;

	CArray<T_OffLineKLineParam, T_OffLineKLineParam&>	m_aOffLineKLines;
	CArray<T_OffLineTickParam, T_OffLineTickParam&>		m_aOffLineTicks;
	CArray<T_OffLineRealtimePriceParam, T_OffLineRealtimePriceParam&> m_aOffLineRealtimePrice;

private:
	bool32			m_bHasSyncServerTime;
	vector<CString> m_vecLogInfo;

private:
	// 1022 #for trace
	HWND			m_hwndTrace;
};


#endif



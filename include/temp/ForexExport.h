#ifndef _FOREX_EXPORT_H_
#define _FOREX_EXPORT_H_

#include "ForexStruct.h"
#include "synch.h"

//
#ifndef _FOREX_DLL
#define FOREXDLL _declspec(dllimport)
#else
#define FOREXDLL _declspec(dllexport)
#endif

class CForexMerch;
class CForexDataCenter;
class COrderInfo;

// ����֪ͨ
class FOREXDLL CAbsForexNotify
{
public:
	// �������

	// ������Ϣ
	virtual void	OnErrorMsg(E_ErrorCode eTypeErr) = 0;

	// ������
	virtual void	OnHeart(IN const CString& StrTime) = 0;

	// ��¼�ɹ�
	virtual void	OnLoginSuccess() = 0;

	// �ض��������
	virtual void	OnReLogServer(IN const CServer& Server) = 0;
	
	// �յ��ʻ���Ϣ
	virtual void	OnAccountInfo(IN const CAccountInfo& AccountInfo) = 0;
	
	// �յ�������Ϣ
	virtual void	OnCurrencyInfo(IN const CArray<CCurrency, CCurrency&>& aCurrency) = 0;
	
	// �յ�������Ʒ��Ϣ
	virtual void	OnMerchInfo(IN const CArray<CForexMerch, CForexMerch&>& aMerchs) = 0;
	
	// �յ��ɽ���Ϣ
	virtual void	OnBargainInfo(IN const CArray<CBargain, CBargain&>& aBargains) = 0;
	
	// �յ�ί�е���Ϣ
	virtual void	OnOrderInfo(IN const CArray<COrderInfo, COrderInfo&>& aOrderInfos) = 0;
	
	// �յ��ѽ���ƽ�ֵ�
	virtual void	OnHistoryOrderInfo(IN const CArray<CHistoryOrderInfo, CHistoryOrderInfo&>& aHistoryOrderInfos) = 0;
	
	// �յ�����Ϣ
	virtual void	OnGroupInfo(IN const CGroup& Group) = 0;
	
	// �յ�������Ϣ
	virtual void	OnBaseInfo(IN const CBaseInfo& BaseInfo) = 0;
	
	// �յ�������Ϣ
	virtual void	OnBroadCast(IN const CBroadCast& BroadCast) = 0;	

	// ��������
	virtual void	OnPushRealtimePrice(const CForexMerch& Merch) = 0;

	// ����������������ӳ�����
	virtual void	OnNetError(int iError) = 0;

	/* ------------------------------ ������� ------------------------------
	/*
	/* �յ����֪ͨ��ʱ��, ���������ڵ����е�����Ϣ�����Ѿ����¹���
	/* ����ƽ�ֻ��µ��ɹ�, ����ʹ�ýӿڷ��ص����ݽṹ�������Ѻ���Ϣ���ֶ�
	/*
	/* ---------------------------------------------------------------------*/

	// �¼�ʱ���ɹ�
	virtual void	OnCommitImmiOK(const CBargain& Bargain)	= 0;

	// �¼�ʱ��ʧ��
	virtual void	OnCommitImmiFail(const COrderInfo& OrderInfo) = 0;

	// ƽ�ּ�ʱ���ɹ�
	virtual void	OnCommitCloseOK(const CBargain& Bargain) = 0;

	// ƽ�ּ�ʱ��ʧ��
	virtual void	OnCommitCloseFail(const COrderInfo& OrderInfo) = 0;
	
	// ��ί�е��ɹ�
	virtual void	OnCommitEntryOK(const COrderInfo& OrderInfo) = 0;

	// ��ί�е�ʧ��
	virtual void	OnCommitEntryFail(const COrderInfo& OrderInfo) = 0;

	// �޸�ί�е��ɹ�
	virtual void	OnCommitModifyEntryOK(const COrderInfo& OrderInfo) = 0;
	
	// �޸�ί�е�ʧ��
	virtual void	OnCommitModifyEntryFail(const COrderInfo& OrderInfo) = 0;

	// ɾ��ί�е��ɹ�
	virtual void	OnCommitDelEntryOK(const COrderInfo& OrderInfo) = 0;
	
	// ɾ��ί�е�ʧ��
	virtual void	OnCommitDelEntryFail(const COrderInfo& OrderInfo) = 0;

	// ��¼������
	virtual	void	OnForexLogInfo(const CArray<CForexLogInfo, const CForexLogInfo &> &logInfo) = 0;
};

// ��㽻��Э�鴦��
class FOREXDLL CAbsForexManager
{
public:
	// ɾ������
	virtual void	ReleaseObj() = 0;

	// ����֪ͨ
	virtual void	SetNotify(CAbsForexNotify* pNotify) = 0;

	// ȡ��֪ͨ
	virtual void	UnSetNotify() = 0;

	// ����	
	virtual bool	ConnectServer(const CServer& Server) = 0;
	
	// �Ͽ�����	
	virtual void	DisConnect() = 0;

	// �����¼
	virtual	bool	ReqLogin(const CString& StrUserID, const CString& StrPwd) = 0;

	// �����˳�
	virtual	bool	ReqLogOut(const CString& StrUserID, const CString& StrPwd) = 0;

	// �����µ�
	virtual	bool	CommitBill(const COrderInfo &order) = 0;

	// ������ʷ��¼
	virtual bool	ReqHistoryOrderInfo(time_t tStart, time_t tEnd) = 0;

	// ��ȡ��������
	virtual CForexDataCenter* GetDataCenter() = 0;

	// ��ȡ��������ַ
	virtual void	GetServer(CServer& Server) = 0;
};

// ��������
class FOREXDLL CForexDataCenter
{
public:
	CForexDataCenter();
	virtual ~CForexDataCenter();
	
public:
	void		SetNotify(CAbsForexNotify* pNotify);
	
	void		UnSetNotify();
	
	void		OnRealtimePriceUpdate(const CForexRealtimePrice& ForexRealtimePrice);

	// ȡ��ǰ���ڲ����:
	int			GetInternalNo();

public:
	
	// ������Ϣ
	void		GetBaseInfo(CBaseInfo& BaseInfo);

	// ������Ϣ
	void		GetCurrencysInfo(CArray<CCurrency, CCurrency&>& aCurrency);

	// ȡ����Ϣ
	void		GetGroupInfo(CGroup& Group);

	// ������Ϣ
	void		GetBroadCast(CBroadCast& BroadCast);

	// �ʺ���Ϣ
	void		GetAccountInfo(CAccountInfo& AccountInfo);

	// ��Ʒ��Ϣ
	void		GetForexMerchs(CArray<CForexMerch, CForexMerch&>& aMerchs);

	// �ɽ���Ϣ
	void		GetBargainsInfo(CArray<CBargain, CBargain&>& aBargain);

	// ί�е���Ϣ
	void		GetOrdersInfo(CArray<COrderInfo, COrderInfo&>& aOrdersInfo);

	// �ѽ���ƽ�ֵ���Ϣ
	void		GetHistoryOrderInfo(CArray<CHistoryOrderInfo, CHistoryOrderInfo&>& aHistoryOrderInfo);

	// ���������صĴ���
	E_ErrorCode	GetErrorCode();

	//
public:
	//
	CAbsForexNotify* m_pNotify;

private:

	// ������Ϣ
	CBaseInfo		m_BaseInfo;
	LockSingle		m_LockBaseInfo;

	// ������Ϣ
	CArray<CCurrency, CCurrency&> m_aCurrency;
	LockSingle		m_LockCurrency;

	// ����Ϣ
	CGroup			m_Group;
	LockSingle		m_LockGroup;

	// ������Ϣ
	CBroadCast		m_BroadCast;
	LockSingle		m_LockBroadCast;

	// �ʺ���Ϣ
	CAccountInfo	m_AccountInfo;
	LockSingle		m_LockAccountInfo;

	// ��Ʒ��Ϣ
	CArray<CForexMerch, CForexMerch&>	m_aMerchsInfo;
	LockSingle		m_LockMerchsInfo;

	// �ɽ���Ϣ
	CArray<CBargain, CBargain&>	m_aBargains;
	LockSingle		m_LockBargains;

	// ί�е���Ϣ
	CArray<COrderInfo, COrderInfo&> m_aOrdersInfo; 
	LockSingle		m_LockOrdersInfo;

	// �ѽ���ƽ�ֵ���Ϣ
	CArray<CHistoryOrderInfo, CHistoryOrderInfo&> m_aHistoryOrdersInfo; 
	LockSingle		m_LockHistoryOrdersInfo;

	// ���������صĴ���
	E_ErrorCode		m_eErrServer;
	LockSingle		m_LockErrServer;

// 
friend class CPackagePara;
};

// ��ȡ��̬�����ָ��
extern "C" FOREXDLL CAbsForexManager* GetAbsForexSocket();

#endif	// _FOREX_EXPORT_H_
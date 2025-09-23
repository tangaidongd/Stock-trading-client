#if !defined(_TRADE_EXPORT_H_)
#define _TRADE_EXPORT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �����Ľӿ���
#include "tradenginestruct.h"

class AFX_EXT_CLASS CTradeNotify
{
public:
	
	// ���ӳɹ�
	virtual void	OnConnected() = 0;

	// ���ӶϿ�
	virtual void	OnDisConnected() = 0;

	// ����֤��ذ�
	virtual void	OnRespQueryCode(CClientRespCode* pResp) = 0;

	// ��¼�ɹ�
	virtual void	OnLogInOK(CClientRespLogIn* pResp) = 0;

	// �ǳ��ɹ�
	virtual void	OnLogOutOK(CClientRespLogOut* pResp) = 0;

	// �ͻ���Ϣ�ذ�
	virtual void	OnRespUserInfo(CClientRespUserInfo* pResp) = 0;

	// ��Ʒ��Ϣ�ذ�
	virtual void	OnRespMerchInfo(CClientRespMerchInfo* pResp) = 0;

	// ί�������ذ�
	virtual void	OnRespEntrust(CClientRespEntrust* pResp) = 0;

	// ����ί�е��ذ�
	virtual void	OnRespCancelEntrust(CClientRespCancelEntrust* pResp) = 0;

	// ��ѯί�е��ذ�
	virtual void	OnRespQueryEntrust(CClientRespQueryEntrust* pResp) = 0;

	// ��ѯ�ɳ����ذ�
	virtual void	OnRespQueryCancelEntrust(CClientRespQueryCancelEntrust* pResp) = 0;

	// ��ѯ�ɽ��ذ�
	virtual void	OnRespQueryDeal(CClientRespQueryDeal* pResp) = 0;

	// ��ѯ�ֲֻذ�
	virtual void	OnRespQueryHold(CClientRespQueryHold* pResp) = 0;

	// ��ѯ�ֲֻ��ܻذ�
	virtual void	OnRespQueryHoldTotal(CClientRespQueryHoldTotal* pResp) = 0;

	// ��Է�����ԱID�ذ�
	virtual void	OnRespQueryTraderID(CClientRespTraderID* pResp) = 0;	

	// ����ذ�
	virtual void	OnRespQuote(CClientRespQuote* pResp) = 0;

	// ����ֹӯֹ��ذ�
	virtual void	OnRespSetStopLP(CClientRespSetStopLP* pResp) = 0;

	// ȡ��ֹӯֹ��ذ�
	virtual void	OnRespCancelStopLP(CClientRespCancelStopLP* pResp) = 0;

	// �޸�����ذ�
	virtual void	OnRespModifyPwd(CClientRespModifyPwd* pResp) = 0;

	// ������
	virtual void	OnError(CClientRespError* pResp) = 0;
};

class AFX_EXT_CLASS CTradeClient
{
public:
	CTradeClient()
	{
		m_pNotify = NULL;
	}

	//
	void		  SetNotifyer(CTradeNotify* pNotify) { if (NULL == pNotify)return;	m_pNotify = pNotify; }

	// ���ӷ�����
	virtual bool32 Connect(const CString & StrServerAdd, const int32 iPort, bool32 bUseProxy, const CProxyInfo& stProxy) = 0;

	// �Ͽ�����
	virtual	void   DisConnect() = 0;

	// ����֤��
	virtual bool32 ReqQueryCode(const CClientQueryCode& stReq) = 0;	

	// ��Է�����ԱID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq) = 0;	

	// ��¼
	virtual bool32 ReqLogIn(const CClientReqLogIn& stReq) = 0;	

	// �ǳ�
	virtual bool32 ReqLogOut(const CString& StrAccount, const CString& StrSessionID, const CString& StrSessionInfo) = 0;	

	// ����ͻ���Ϣ
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq) = 0;

	// ������Ʒ��Ϣ
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq) = 0;

	// ����ί������
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq) = 0;

	// ������ί�е�
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq) = 0;

	// �����ѯί�е�
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq) = 0;

	// �����ѯ�ɳ���
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq)=0;

	// �����ѯ�ɽ�
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq) = 0;

	// �����ѯ�ֲ�
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq) = 0;

	// �����ѯ�ֲֻ���
	virtual bool32 ReqQueryHoldTotal(const CClientReqQueryHoldTotal& stReq) = 0;

	// ��������
	virtual bool32 ReqQute(const CClientReqQuote& stReq) = 0;

	// ����ֹӯֹ��
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq) = 0;

	// ȡ��ֹӯֹ��
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq) = 0;

	// �����޸�����
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq) = 0;

	//��CreateTradeClient �����Ŀռ䣬������Destroy�ͷ�
	virtual void Destroy() = 0;

	//
	CTradeNotify*	m_pNotify;
};

AFX_EXT_CLASS CTradeClient* CreateTradeClient();


#endif // _TRADE_EXPORT_H_
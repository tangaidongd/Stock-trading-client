#ifndef _TRADEQUERYINTERFACE_H_
#define _TRADEQUERYINTERFACE_H_


#include "SocketClient.h"
#include "tradenginexport.h"

enum E_TradeLoginStatus
{
	ETLS_NotLogin = 0,		// δ��¼
	ETLS_Logining,			// ����¼
	ETLS_Logined,			// �ѵ�¼
	ETLS_Locked,
};

enum E_TipType
{
	ETT_NONE = 0,
	ETT_SUC,		// �ɹ�
	ETT_ERR,		// ����
	ETT_TIP,		// ��ʾ
	ETT_WAR,		// ����
};

enum E_MsgType
{
	EMT_NONE = 0,
	EMT_CONNECTED,		// ���ӳɹ�
	EMT_DISCONNECTED,	// ���ӶϿ�
	EMT_QUERYCODE,		// ����֤��
	EMT_LOGINOK,		// ��¼�ɹ�
	EMT_LOGOUTOK,		// �ǳ��ɹ�
	EMT_USERINFO,		// �ͻ���Ϣ
	EMT_MERCHINFO,		// ��Ʒ��Ϣ
	EMT_ENTRUST,		// ί������
	EMT_CANCELENTRUST,	// ����ί�е�
	EMT_QUERYENTRUST,	// ��ѯί�е�
	EMT_QUERYDEAL,		// ��ѯ�ɽ�
	EMT_QUERYHOLD,		// ��ѯ�ֲ�
	EMT_QUERYHOLDSUMMARY,// ��ֲֻ���
	EMT_QUOTE,			// ����
	EMT_QUERYTRADERID,	// ��Է�����ԱID
	EMT_SETSTOPLP,		// ����ֹӯֹ��
	EMT_CANCELSTOPLP,	// ȡ��ֹӯֹ��
	EMT_MODIFY_PWD,		// �޸�����
	EMT_ERROR,			// ������
};

#define WM_ENTRUSTTYPE_CLOSE	WM_USER+1000	//ƽ��
#define WM_HOLDDETAIL_CLICK		WM_USER+1001	//ѡ�е����ֲ�
#define WM_CLOSETRADE			WM_USER+1002	//�رս���֪ͨ
#define WM_NETWORKSTATUS_CHANGE	WM_USER+1003	//����״̬�ı�֪ͨ
#define WM_HIDETRADEWND			WM_USER+1004	//���ؽ��״���

#define WM_RESP_TRADE_NOTIFY	WM_USER+1005	//�ذ�֪ͨ��Ϣ

enum E_TradeType
{
	ECT_Market = 0,		// �м�
	ECT_Limit,			// ָ��
};

enum E_CloseBtnStatus
{
	ECBS_Enable = 0,
	ECBS_UnEnable,
};

#include <vector>
using std::vector;

 //
 typedef vector<T_RespQueryHold>	 QueryHoldDetailResultVector;  // ��ѯ�ֲ�
 typedef vector<T_RespQueryHoldTotal> QueryHoldSummaryResultVector;// ��ֲֻ���
 typedef vector<T_RespQueryEntrust>  QueryEntrustResultVector;     //��ѯί�е�
 typedef vector<T_RespQueryDeal>	 QueryDealResultVector;		 // ��ѯ�ɽ�
 typedef vector<T_TradeMerchInfo>	 QueryCommInfoResultVector;    // ��Ʒ��Ϣ
 typedef vector<T_RespQuote>		 QueryQuotationResultVector;	 // ����
 typedef vector<T_RespTraderID>		 QueryTraderIDResultVector;	 // ��Է�����ID

class iTradeBidNotify;
class iTradeBid
{
public:
	virtual	void	AddTradeBidNotify(iTradeBidNotify *pNotify) = 0;
	virtual void	RemoveTradeBidNotify(iTradeBidNotify *pNotify) = 0;
	
	// query����
	// ����ͻ���Ϣ
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg) = 0;
	// ������Ʒ��Ϣ
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq, CString &strErrMsg) = 0;
	// ��ѯ�Է�����ID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq, CString &strErrMsg) = 0;
	// ����ί������
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg) = 0;
	// ������ί�е�
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg) = 0;
	// �����ѯί�е�
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq, CString &strErrMsg) = 0;
	// �����ѯ�ɳ���
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq, CString &strErrMsg) = 0;
	// �����ѯ�ɽ�
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq, CString &strErrMsg) = 0;
	// �����ѯ�ֲ�
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq, CString &strErrMsg) = 0;
	//�����ֲֻ���
	virtual bool32 ReqQueryHoldSummary(const CClientReqQueryHoldTotal& stReq, CString &strErrMsg) = 0;
	// ��������
	virtual bool32 ReqQute(const CClientReqQuote& stReq, CString &strErrMsg) = 0;
	// ����ֹӯֹ��
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg) = 0;
	// ȡ��ֹӯֹ��
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg) = 0;
	// �����޸�����
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg) = 0;

	// Get����
	virtual QueryHoldDetailResultVector				&GetCacheHoldDetail() = 0;
	virtual const QueryHoldSummaryResultVector		&GetCacheHoldSummary() = 0;
	virtual QueryEntrustResultVector				&GetCacheEntrust() = 0;
	virtual const QueryDealResultVector				&GetCacheDeal() = 0 ;
	virtual const QueryCommInfoResultVector			&GetCacheCommInfo() = 0 ;
	virtual const QueryTraderIDResultVector			&GetCacheTraderID() = 0 ;
	virtual const CClientRespUserInfo			    &GetCacheUserInfo() = 0;
	virtual QueryQuotationResultVector				&GetCacheQuotation() = 0 ;
	virtual const CClientRespError					&GetCacheError() = 0;
	virtual const CClientRespLogIn					&GetCacheLogIn() = 0;

	virtual const CClientRespEntrust				&GetCacheReqEntrust() = 0;
	virtual const CClientRespCancelEntrust			&GetCacheReqCancelEntrust() = 0;
	virtual const CClientRespSetStopLP				&GetCacheReqSetStopLP() = 0;
	virtual const CClientRespCancelStopLP			&GetCacheReqCancelStopLP() = 0;
	virtual const CClientRespModifyPwd				&GetCacheReqModifyPwd() = 0;

	// ����
	virtual int	HeartBeating(CString &strTipMsg) = 0;

	virtual void	WndMaxMin() = 0;
	virtual void	WndClose(int iMode) = 0;
	virtual BOOL	GetWndStatus() = 0;

	//��¼״̬��ѯ
	virtual E_TradeLoginStatus GetTradeLoginStatus() = 0;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryEntrustResultVector 	m_aCacheEntrust;
	QueryDealResultVector 		m_aCacheDeal;
	QueryCommInfoResultVector	m_aCacheCommInfo;
	QueryQuotationResultVector	m_aCacheQuotation;
	QueryTraderIDResultVector	m_aCacheTraderID;
	CClientRespError			m_stCacheError;	// �����
	CClientRespLogIn			m_stCacheLogIn;	// ��½
	CClientRespUserInfo			m_stCacheUserInfo; // �ͻ���Ϣ
	CClientRespCode				m_stCacheCode; // ��֤��

	CClientRespEntrust			m_stCacheReqEntrust; // ί�������ذ�
	CClientRespCancelEntrust	m_stCacheReqCancelEntrust; 
	CClientRespSetStopLP		m_stCacheReqSetStopLP; 
	CClientRespCancelStopLP		m_stCacheReqCancelStopLP; 
	CClientRespModifyPwd		m_stCacheReqModifyPwd; 
	bool						m_bReqQuick;	// �����µ�
};

class iTradeBidNotify
{
public:
	
	// ��¼����״̬��� status = E_TradeLoginStatus
	virtual void	OnLoginStatusChanged(int iCurStatus, int iOldStatus) = 0;
	virtual bool32	OnInitQuery(CString &strTipMsg) = 0;
	virtual bool32	OnQueryUserInfo(CString &strTipMsg) = 0;
	virtual void	OnInitCommInfo() = 0;
	
	virtual void	OnQueryHoldDetailResponse() = 0;
	virtual void	OnQueryHoldSummaryResponse() = 0;
//	virtual void	OnQueryEntrustResponse() = 0;
	virtual void	OnQueryLimitEntrustResponse() = 0;
	virtual void	OnQueryDealResponse() = 0;
	virtual void	OnQueryCommInfoResponse() = 0;
	virtual void	OnQueryTraderIDResponse() = 0;
	virtual void	OnQueryUserInfoResponse() = 0;
	virtual void	OnQueryQuotationResponse() = 0;
	virtual void	OnDisConnectResponse() = 0;

	virtual void	OnReqEntrustResponse(bool32 bShowTip=true) = 0;
	virtual void	OnReqCancelEntrustResponse() = 0;
	virtual void	OnReqSetStopLPResponse() = 0;
	virtual void	OnReqCancelStopLPResponse() = 0;
	virtual void	OnReqModifyPwdResponse() = 0;
};

class CRespTradeNotify : public CTradeNotify
{
public:
	CRespTradeNotify(HWND m_hwnd)
	{
		m_notifyHwnd = m_hwnd;
	}

	~CRespTradeNotify();
	
	// ���ӳɹ�
	virtual void	OnConnected();
	
	// ���ӶϿ�
	virtual void	OnDisConnected();

	// ����֤��ذ�
	virtual void	OnRespQueryCode(CClientRespCode* pResp);
	
	// ��¼�ɹ�
	virtual void	OnLogInOK(CClientRespLogIn* pResp);
	
	// �ǳ��ɹ�
	virtual void	OnLogOutOK(CClientRespLogOut* pResp);
	
	// �ͻ���Ϣ�ذ�
	virtual void	OnRespUserInfo(CClientRespUserInfo* pResp);
	
	// ��Ʒ��Ϣ�ذ�
	virtual void	OnRespMerchInfo(CClientRespMerchInfo* pResp);

	// �Է�����ԱID�ذ�
	virtual void	OnRespQueryTraderID(CClientRespTraderID* pResp);
	
	// ί�������ذ�
	virtual void	OnRespEntrust(CClientRespEntrust* pResp);
	
	// ����ί�е��ذ�
	virtual void	OnRespCancelEntrust(CClientRespCancelEntrust* pResp);
	
	// ��ѯ��ѯ�ɳ����ذ�
	virtual void	OnRespQueryCancelEntrust(CClientRespQueryCancelEntrust* pResp);

	// ��ѯί�е��ذ�
	virtual void	OnRespQueryEntrust(CClientRespQueryEntrust* pResp);
	
	// ��ѯ�ɽ��ذ�
	virtual void	OnRespQueryDeal(CClientRespQueryDeal* pResp);
	
	// ��ѯ�ֲֻذ�
	virtual void	OnRespQueryHold(CClientRespQueryHold* pResp);

	// ��ѯ�ֲֻ��ܻذ�
	virtual void	OnRespQueryHoldTotal(CClientRespQueryHoldTotal* pResp);
	
	// ����ذ�
	virtual void	OnRespQuote(CClientRespQuote* pResp);
	
	// ����ֹӯֹ��ذ�
	virtual void	OnRespSetStopLP(CClientRespSetStopLP* pResp);
	
	// ȡ��ֹӯֹ��ذ�
	virtual void	OnRespCancelStopLP(CClientRespCancelStopLP* pResp);

	// �޸�����ذ�
	virtual void	OnRespModifyPwd(CClientRespModifyPwd* pResp);
	
	// ������
	virtual void	OnError(CClientRespError* pResp);

	// Get����
	const QueryHoldDetailResultVector		&GetCacheHoldDetail(){ return m_aCacheHoldDetail;} ;
	const QueryHoldSummaryResultVector		&GetCacheHoldSummary() { return m_aCacheHoldSummary; }
	const QueryEntrustResultVector			&GetCacheEntrust(){ return m_aCacheEntrust;}
	const QueryDealResultVector				&GetCacheDeal(){ return m_aCacheDeal;}
	const QueryCommInfoResultVector			&GetCacheCommInfo(){ return m_aCacheCommInfo;}
	const CClientRespUserInfo				&GetCacheUserInfo(){ return m_stCacheUserInfo;}
	const QueryQuotationResultVector		&GetCacheQuotation(){ return m_aCacheQuotation;}
	const QueryTraderIDResultVector			&GetCacheTraderID(){ return m_aCacheTraderID;}
	const CClientRespError					&GetCacheError(){ return m_stCacheError;}
	const CClientRespLogIn					&GetCacheLogIn(){ return m_stCacheLogIn;}
	const CClientRespCode					&GetCacheCode(){ return m_stCacheCode;}

	const CClientRespEntrust				&GetCacheReqEntrust(){ return m_stCacheReqEntrust;}
	const CClientRespCancelEntrust			&GetCacheReqCancelEntrust(){ return m_stCacheReqCancelEntrust;}
	const CClientRespSetStopLP				&GetCacheReqSetStopLP(){ return m_stCacheReqSetStopLP;}
	const CClientRespCancelStopLP			&GetCacheReqCancelStopLP(){ return m_stCacheReqCancelStopLP;}
	const CClientRespModifyPwd				&GetCacheReqModifyPwd(){ return m_stCacheReqModifyPwd;}

private:
	HWND m_notifyHwnd;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryHoldSummaryResultVector m_aCacheHoldSummary;
	QueryEntrustResultVector 	m_aCacheEntrust;
	QueryDealResultVector 		m_aCacheDeal;
	QueryCommInfoResultVector	m_aCacheCommInfo;
	QueryQuotationResultVector	m_aCacheQuotation;
	QueryTraderIDResultVector	m_aCacheTraderID;
	CClientRespError			m_stCacheError;	// �����
	CClientRespLogIn			m_stCacheLogIn;	// ��½
	CClientRespUserInfo			m_stCacheUserInfo;// �ͻ���Ϣ
	CClientRespCode				m_stCacheCode;	// ��֤��

	CClientRespEntrust			m_stCacheReqEntrust; // ί�������ذ�
	CClientRespCancelEntrust	m_stCacheReqCancelEntrust; 
	CClientRespSetStopLP		m_stCacheReqSetStopLP; 
	CClientRespCancelStopLP		m_stCacheReqCancelStopLP; 
	CClientRespModifyPwd		m_stCacheReqModifyPwd; 
};
#endif
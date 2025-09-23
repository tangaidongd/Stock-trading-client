#ifndef _TRADECONTAINERWND_H_
#define _TRADECONTAINERWND_H_

#include "WndRectButton.h"
#include "TradeLoginInterface.h"
#include "DlgTradeBidContainer.h"
#include "DlgTradeLogin.h"
#include "TradeQueryInterface.h"
#include "ExportMT2DispatchThread.h"
#include "AuthoPlugin.h"
#include "DlgQuickOrder.h"
#include "synch.h"

#include <vector>
using std::vector;
#include <list>
using std::list;
#include <map>
using std::map;
#include <set>
using std::set;

class CMyContainerBar;
class CDlgTradeLogin;
class CDlgQuickOrder;
class CViewData;

class CTradeContainerWnd 
: public CRectButtonWndSimpleImpl
 , public iTradeLogin
 , public iTradeBid
{
	DECLARE_DYNCREATE(CTradeContainerWnd);

public:
	CTradeContainerWnd(E_TradeLoginType eTradeType);
	CTradeContainerWnd(){}
	~CTradeContainerWnd();

	int	 Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext  = NULL );

	bool Login();
	void LoginOK();
	// CRectButtonWndSimpleImpl
public:
	virtual	void	GetAllBtns(WndRectButtonVector &aBtns);
	virtual void	OnDraw(CDC &dc);			// ����ʱ���̳ж�����ô˽ӿڻ��ư�ť
	virtual	void	RecalcLayout(bool32 bNeedPaint = true);			// ���������ť���ӿؼ�size

// 	// iTradeLogin
 public:
	virtual bool    Connect(const T_TradeLoginInfo &stLoginInfo, iTradeLoginNotify *pNotify);
	virtual void	DisConnect(int iMode=0);
 	virtual void	StopLogin(int iMode=0);
 	virtual void	OnLoginDlgCancel();		// ��¼����Ҫ�ر���
// 
	// iTradeBid
 public:
	 // ��ѯ
	virtual	void	AddTradeBidNotify(iTradeBidNotify *pNotify);
	virtual void	RemoveTradeBidNotify(iTradeBidNotify *pNotify);

	// ����ͻ���Ϣ
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg);
	// ������Ʒ��Ϣ
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq, CString &strErrMsg);
	// �����ѯ����ԱID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq, CString &strErrMsg);
	// ����ί������
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg);
	// ������ί�е�
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg);
	// �����ѯί�е�
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq, CString &strErrMsg);
	// �����ѯ�ɳ���
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq, CString &strErrMsg);
	// �����ѯ�ɽ�
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq, CString &strErrMsg);
	// �����ѯ�ֲ�
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq, CString &strErrMsg);
	// �����ѯ�ֲֻ���
	virtual bool32 ReqQueryHoldSummary(const CClientReqQueryHoldTotal& stReq, CString &strErrMsg);
	// ��������
	virtual bool32 ReqQute(const CClientReqQuote& stReq, CString &strErrMsg);
	// ����ֹӯֹ��
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg);
	// ȡ��ֹӯֹ��
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg);
	// �����޸�����
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg);

	// Get
	QueryHoldDetailResultVector				&GetCacheHoldDetail(){ return m_aCacheHoldDetail;}
	const QueryHoldSummaryResultVector		&GetCacheHoldSummary() { return m_aCacheHoldSummary; }
	QueryEntrustResultVector				&GetCacheEntrust(){ return m_aCacheEntrust;}
	const QueryDealResultVector				&GetCacheDeal(){ return m_aCacheDeal;}
	const QueryCommInfoResultVector			&GetCacheCommInfo(){ return m_aCacheCommInfo;}
	const CClientRespUserInfo				&GetCacheUserInfo(){ return m_stCacheUserInfo;}
	QueryQuotationResultVector				&GetCacheQuotation(){ return m_aCacheQuotation;}
	const QueryTraderIDResultVector			&GetCacheTraderID(){ return m_aCacheTraderID;}
	const CClientRespError					&GetCacheError(){ return m_stCacheError;}
	const CClientRespLogIn					&GetCacheLogIn(){ return m_stCacheLogIn;}
	const CClientRespCode					&GetCacheCode(){ return m_stCacheCode;}

	const CClientRespEntrust				&GetCacheReqEntrust(){ return m_stCacheReqEntrust;}
	const CClientRespCancelEntrust			&GetCacheReqCancelEntrust(){ return m_stCacheReqCancelEntrust;}
	const CClientRespSetStopLP				&GetCacheReqSetStopLP(){ return m_stCacheReqSetStopLP;}
	const CClientRespCancelStopLP			&GetCacheReqCancelStopLP(){ return m_stCacheReqCancelStopLP;}
	const CClientRespModifyPwd				&GetCacheReqModifyPwd(){ return m_stCacheReqModifyPwd;}

	// ����
	virtual int	HeartBeating(CString &strTipMsg);

	//�����С��
	virtual void	WndMaxMin();
	virtual BOOL	GetWndStatus() {return m_bWndMaxMinStatus;};
	virtual void	WndClose(int iMode);	//0-���µ�¼1-����ע��2-����ע��3-�ر���������
	virtual E_TradeLoginStatus GetTradeLoginStatus();

public:

// 	// ����״̬
 	bool			IsLogin();
 	void			TryLogin();		// ���û��¼���򵯳���¼���¼���Ѿ���¼����ʾ���׽���
	void			LoginErr();
public:
	// Bar����
	CMyContainerBar *GetParentBar() const;
	
	// �Ŵ���С
	BOOL			Maximize();
	BOOL			IsMaximized() const;
	BOOL			RestoreSize();
	BOOL			Minimize();
	BOOL			IsMinimized();
	void			DoMyToggleDock();

	// ��ɫ
	void			SetBtnColors();

	// 
	void			DoOnLoginSuc();
	void			DoStopTrade();
	void			CheckLoginSuc(int eRecvPackType);

	void			ShowProperChild();
	void			SetLoginStatus( int eStatus );


	void			OnTradeLogIn(int32 iFlag);  // ʵ�̽���
	void			OnTradeQuotationPush(const CClientRespQuote *pAns, const int iCount);
	void			OnSimulateTradeLogIn(int32 iFlag, CString strQuoteFlag=_T("0")); // ģ�⽻��
	void			OnMsgSimulateTradeQuotePush(IN CMerch *pMerch); // ģ�⽻����Ʒ���������

	CMerch*			TradeCode2ClientMerch(const CString& StrTradeCode);
	CString			ClientMerch2TradeCode(const CMerch* pMerch);
public:

	CTradeClient *m_tradeClient;
	CRespTradeNotify *m_respTradeNotify;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryHoldSummaryResultVector m_aCacheHoldSummary;
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
	CClientRespModifyPwd	    m_stCacheReqModifyPwd;
	
	BOOL m_bWndMaxMinStatus;	//���ڵ�ǰ״̬ 0-��С 1-���
	CSize m_MaxSize;			// ��С֮ǰ�Ĵ�С

public:
	void DoOnQueryHoldDetailResponse();
	void DoOnQueryHoldSummaryResponse();
	void DoOnQueryEntrustResponse();
	void DoOnQueryDealResponse();
	void DoOnQueryCommInfoResponse();
	void DoOnQueryTraderIDResponse();
	void DoOnQueryUserInfoResponse();
	void DoOnQueryQuotationResponse();
	void DoOnErrorResponse();
	void DoOnDisConnectResponse();
	void DoOnQueryCodeResponse();

	void DoOnReqEntrustResponse();
	void DoOnReqCancelEntrustResponse();
	void DoOnReqSetStopLPResponse();
	void DoOnReqCancelStopLPResponse();
	void DoOnReqModifyPwdResponse();
	void SetQuoteValue();

public:
	volatile int			m_eLoginStatus;
	CDlgTradeLogin			*m_pDlgLogin;
	CDlgTradeBidContainer	*m_pDlgTradeBidParent;

	int32 iRespCnt;
	int32 m_MaxRespCnt;

private:
	CWndRectButton		*m_pBtnClose;
	CWndRectButton		*m_pBtnMax;
	CWndRectButton		*m_pBtnMin;
	CButton				m_BtnLogin;

 	iTradeLoginNotify	*m_pLoginNotify;
 	T_TradeLoginInfo	m_stTradeLoginInfo;
 	char				m_szAccountCur[100];

	volatile bool		m_bLoginSuc;
	volatile long		m_lWaitTradeDTPProcess;
	typedef vector<iTradeBidNotify *> iTradeBidNotifyVector;
	iTradeBidNotifyVector	m_aTradeBidNotify;
	iTradeBidNotify		*m_pTradeBidNotify;

	bool32				m_bRecvMsg;

	// ���� END

	DECLARE_MESSAGE_MAP();
	// Generated message map functions
	//{{AFX_MSG(CTradeContainerWnd)
	afx_msg	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnDestroy();
	afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void		OnTradeClose();
	afx_msg void		OnTradeMax();
	afx_msg void		OnTradeMin();
	afx_msg void		OnBtnClick(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void		OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void		OnTimer(UINT nIDEvent);
	afx_msg	LRESULT		OnMsgRespTradeNotify(WPARAM w, LPARAM l);
	LRESULT				OnMsgHideTradeWnd( WPARAM w, LPARAM l );

	afx_msg	void		OnBtnTradeLogin();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
};

#endif //!_TRADECONTAINERWND_H_
// GGTongDoc.h : interface of the CGGTongDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GGTONGDOC_H__194B9636_9140_4489_8C49_3EFAC40A30F3__INCLUDED_)
#define AFX_GGTONGDOC_H__194B9636_9140_4489_8C49_3EFAC40A30F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "facescheme.h"
#include "ReportScheme.h"
#include "SocketClient.h"
#include "module.h"
#include "OptimizeServer.h"
#include "ProxyAuthClientHelper.h"
#include "httpClient.h"
#include "EngineCenterBase.h"

// class CAuthManager;
class CDataManager;
class CViewData;
class CCommManager;
class CNewsManager;
class COfflineDataManager;
class CProxyAuthClientHelper;
class CGGTongDoc;
class CDlgbuding;
class CDlgNewLogin;
class CAlarmCenter;
class CHttpClient;

class CAbsLoginDlgObserver
{
public:
	virtual bool32	OnLoginDlgDoModalSuccess() = 0;
}; 

CGGTongDoc* AfxGetDocument();
/////////////////////////////////////////////////
class CGGTongDoc : public CDocument, public CAbsLoginDlgObserver
{
protected: // create from serialization only
	CGGTongDoc();
	DECLARE_DYNCREATE(CGGTongDoc)

public:
	virtual ~CGGTongDoc();

	// from CAbsLoginDlgObserver
public:
	virtual bool32	OnLoginDlgDoModalSuccess();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	bool32			Login();
	void			PostInit();

	bool32			CreateAllGGTongObject(void);
	void			DestroyAllGGTongObject(void);

    void SetInfoCenterIP(const CString& StrAddress);
    CString GetInfoCenterIP();

	void SetInfoCenterPort(int nPort);
	int GetInfoCenterPort();

	void SetGateWayIP(const CString& StrAddress);
	CString GetGateWayIP();
	void SetGateWayPort(int nPort);
	int GetGateWayPort();

	// pickmodelѡ��
	void SetPickModelIP(const CString& StrAddress);
	CString GetPickModelIP();
	void SetPickModelPort(int nPort);
	int GetPickModelPort();


	// �����¹ɵ�IP��Ϣ
	void SetQueryStockIP(const CString& StrAddress);
	CString GetQueryStockIP();
	void SetQueryStockPort(int nPort);
	int GetQueryStockPort();

	
	// ��ȡ�ƾ�������������URL��ַ
	void SetEconoServerURL(std::string strURL);
	void GetEconoServerURL(std::string &strURL);

	bool32 IsReqEcono();
	bool32 IsReqMessageInfo();

	bool   GetReqMessageInfoStatus() {return m_bReqMessageInfo;};
private:
    CString         m_StrInfoCenterIP;				// ��Ѷ���ĵ�IP
	int				m_nInfoCenterPort;				// ��Ѷ���ĵĶ˿�

	CString         m_StrGateWayIP;					// GateWayIP
	int				m_nGateWayPort;					// GateWay�˿�

	CString         m_StrPickModelIP;					// PickModelIP
	int				m_nPickModelPort;					// PickModel�˿�

	CString         m_StrQueryStockIP;					// QueryStockIP
	int				m_nQueryStockPort;					// QueryStock�˿�

	std::string		m_strEconoServerURL;			// �ƾ�������������URL��ַ

	bool32			m_bReqEcono;					// �Ƿ�����ƾ�������û�����÷�����������
	bool32			m_bReqMessageInfo;				// �Ƿ�������Ϣ������Ϣ��û�����÷�����������

/////////////////////////////////////////////////////////////////
// 
public:
	CAlarmCenter    *m_pAarmCneter;
	http_client::CHttpClient		*m_pHttpClient;

	CAbsCenterManager		*m_pAbsCenterManager;
	CAbsDataManager			*m_pAbsDataManager;
	CProxyAuthClientHelper	*m_pAutherManager;
	CAbsNewsManager			*m_pNewsManager;


	// 
	CDlgNewLogin	*m_pDlgLogIn;
	CDlgbuding		*m_pDlgbuding;

	// ������¼�������Ϣ�� ����Ҫ�õ�
	CProxyInfo		m_ProxyInfo;					// ��¼ʱ���õĴ����������Ϣ
	bool32			m_bNewUser;						// �Ƿ�Ϊ��һ�ε�¼���û�
	E_NetType m_eNetType;							// �û�����������
	// ����������
	CFaceScheme		m_FaceScheme;
	CReportScheme	m_ReportScheme;

	//�Ƿ��Զ���¼
	BOOL			m_bAutoLogin;
    BOOL            m_isDefaultUser;

	// �Ƿ���ʾ������
	BOOL			m_bShowSCToolBar;
	
	//��������¼�û���ʶ��
	CString         m_StrOpenId;

	//��������¼���� (1 ΢�� 2 QQ  3΢��)
	CString         m_StrLoginType;
	
	//�Ƿ񿪻�����  
	BOOL            m_bAutoRun;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGGTongDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument( );
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CGGTongDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GGTONGDOC_H__194B9636_9140_4489_8C49_3EFAC40A30F3__INCLUDED_)

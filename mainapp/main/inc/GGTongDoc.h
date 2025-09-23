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

	// pickmodel选股
	void SetPickModelIP(const CString& StrAddress);
	CString GetPickModelIP();
	void SetPickModelPort(int nPort);
	int GetPickModelPort();


	// 请求新股的IP信息
	void SetQueryStockIP(const CString& StrAddress);
	CString GetQueryStockIP();
	void SetQueryStockPort(int nPort);
	int GetQueryStockPort();

	
	// 获取财经日历服务器的URL地址
	void SetEconoServerURL(std::string strURL);
	void GetEconoServerURL(std::string &strURL);

	bool32 IsReqEcono();
	bool32 IsReqMessageInfo();

	bool   GetReqMessageInfoStatus() {return m_bReqMessageInfo;};
private:
    CString         m_StrInfoCenterIP;				// 资讯中心的IP
	int				m_nInfoCenterPort;				// 资讯中心的端口

	CString         m_StrGateWayIP;					// GateWayIP
	int				m_nGateWayPort;					// GateWay端口

	CString         m_StrPickModelIP;					// PickModelIP
	int				m_nPickModelPort;					// PickModel端口

	CString         m_StrQueryStockIP;					// QueryStockIP
	int				m_nQueryStockPort;					// QueryStock端口

	std::string		m_strEconoServerURL;			// 财经日历服务器的URL地址

	bool32			m_bReqEcono;					// 是否请求财经日历，没有配置服务器不请求
	bool32			m_bReqMessageInfo;				// 是否请求消息中心信息，没有配置服务器不请求

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

	// 保留登录的相关信息， 后面要用到
	CProxyInfo		m_ProxyInfo;					// 登录时设置的代理服务器信息
	bool32			m_bNewUser;						// 是否为第一次登录的用户
	E_NetType m_eNetType;							// 用户的网络类型
	// 两个公共类
	CFaceScheme		m_FaceScheme;
	CReportScheme	m_ReportScheme;

	//是否自动登录
	BOOL			m_bAutoLogin;
    BOOL            m_isDefaultUser;

	// 是否显示工具栏
	BOOL			m_bShowSCToolBar;
	
	//第三方登录用户标识符
	CString         m_StrOpenId;

	//第三方登录类型 (1 微信 2 QQ  3微博)
	CString         m_StrLoginType;
	
	//是否开机启动  
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

#if !defined(AFX_DLGCONNECTSET_H__46288C5C_04C5_4DC2_AFC1_CCE69FC94AC3__INCLUDED_)
#define AFX_DLGCONNECTSET_H__46288C5C_04C5_4DC2_AFC1_CCE69FC94AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgconnectset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectSet dialog
#include "SocketClient.h"
#include "DlgNewLogin.h"
#include "DialogEx.h"
#include "afxwin.h"
class CDlgConnectSet : public CDialogEx, public CSocketNotify
{
// Construction
public:
	CDlgConnectSet(CWnd* pParent = NULL);   // standard constructor
	~CDlgConnectSet();
	virtual void	OnSocketConnected();
	virtual void	OnSocketRecvOneOrMore();
	virtual void	OnSocketError();
//	virtual void	OnSocketLongTimeNoRecvData(){};
// Dialog Data
	//{{AFX_DATA(CDlgConnectSet)
	enum { IDD = IDD_DIALOG_CONNECTSET };
	CButton	m_CtrlCheckProxy;
	CEdit	m_CtrlEditProxyPwd;
	CEdit	m_CtrlEditProxyName;
	CEdit	m_CtrlEditProxyPort;
	CEdit	m_CtrlEditProxyAdress;
	CComboBox m_ComboHost;
	CButton m_CtrlCheckGreen;
	BOOL	m_bCheckGreen;


	BOOL	m_bCheckProxy;
	BOOL	m_bCheckManual;
	int		m_iRadio;
	UINT	m_uiProxyPort;
	CString	m_StrProxyAdress;
	CString	m_StrProxyName;
	CString	m_StrProxyPwd;
	//}}AFX_DATA
public:
	void	SetInitialProxyInfo(const T_ProxyInfo& ProxyInfo);
	void	SetInitialInfo(const CString& StrUserName, bool32 bUserProxy, int32 iHostIndex, bool32 bManual, bool32 bGreenChannel = FALSE);

	bool32	SetProxyInfoToXml();
	bool32	SetProxyCtrlStates(bool32 bCheck);
	bool32	SetUserInfoToXml();
	void	GetHostString(IN CString StrHostInfo,OUT CString& StrHostName,OUT CString& StrHostAddress,OUT CString& StrHostPort);
	bool32  ReadProxyInfoFromXml(const CString &StrUserName);
	void	GetServerInfoFromXml();

public:
	CSocketClient *						m_pSocketClient;
	T_ProxyInfo							m_ProxyInfo;
	int32								m_iProxyType;
	CString								m_StrUserName;

	//
	int32								m_iHostIndex;
	CArray<T_HostInfo, T_HostInfo>		m_aHostInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConnectSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConnectSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckProxy();
	afx_msg void OnButtonTest();
	afx_msg void OnRadioSock4();
	afx_msg void OnRadioSock5();
	afx_msg void OnRadioHttp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECTSET_H__46288C5C_04C5_4DC2_AFC1_CCE69FC94AC3__INCLUDED_)

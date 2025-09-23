#if !defined(AFX_DLGNEWLOGIN_H__DEB7F96B_BA0C_4D8B_894D_49900A1E0D90__INCLUDED_)
#define AFX_DLGNEWLOGIN_H__DEB7F96B_BA0C_4D8B_894D_49900A1E0D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewLogin.h : header file

#include "TextProgressCtrl.h"
#include "OptimizeServer.h"
#include "LoginUserData.h"
#include "NCButton.h"
#include "ConfigInfo.h"
#include "EditFlat.h"
#include "HButton.h"
#include "DlgNewComment.h"

#include <map>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgNewLogin dialog
class CDlgStatic;
class CAbsLoginDlgObserver;

typedef	struct	T_UsersInfo
{	
	T_UsersInfo()
	{
		BeSavePassword = BeLastUser = false; 
		IndexServer = 0;
		EnvIndex = 0;
		IsShowSCToolBar = false;
		m_aUserRights.RemoveAll();
	}
	
	T_UsersInfo& T_UsersInfo::operator=(const T_UsersInfo& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}
		
		//
		StrUserName		= stSrc.StrUserName;
		StrPassword		= stSrc.StrPassword;
		BeSavePassword	= stSrc.BeSavePassword;
		BeLastUser		= stSrc.BeLastUser;
		IndexServer		= stSrc.IndexServer;
		EnvIndex        = stSrc.EnvIndex; //2013-7-4
		IsAtutoLogin	= stSrc.IsAtutoLogin;
		IsShowSCToolBar = stSrc.IsShowSCToolBar;
		m_aUserRights.Copy(stSrc.m_aUserRights);
		
		return *this;
	}
	
	CString		StrUserName;
	CString		StrPassword;		//������ڰ�ȫ����
	bool32		BeSavePassword;
	bool32		BeLastUser;
	int32		IndexServer;
	int32       EnvIndex;//2013-7-4
	bool32      IsAtutoLogin;
	bool32      IsShowSCToolBar;
	
	CArray<T_RightInfo, T_RightInfo&> m_aUserRights; // רΪ�ѻ�ʱʹ�õ�
	
}T_UsersInfo;


class CDlgNewLogin : public CDialog
{
// Construction
public:
	CDlgNewLogin(CAbsLoginDlgObserver &LoginDlgObserver, CWnd* pParent = NULL);   // standard constructor
	~CDlgNewLogin();

public:
	void			GetServerInfoFromXml(bool32& bValidServerQuotes, bool32& bValidServerNews);
	
	bool32		    GetUserInfoFromXml();
	bool32		    SetUserInfoToXml();
	bool32		    SetDefaultUserInfoToXml();
	void			DeleteUserInfoFromXml(const CString &StrUserName);
	
	bool32		    GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo);	
	T_HostInfo &    GetHostInfoDefault();
	void			ReqCommConfigInfo();	// ����������Ϣ
	bool32			ReqRight();		// ����Ȩ��
	bool32			ReqAuth();		// ������Ȩ
public:		
	void			DrawPromptInfo(CDC &dc);
	void			SetPromptInfo(const CString &StrPrompt);
	void			SetPromptInfo2(const CString &StrPrompt);
	void			SetProgress(int32 iPos, int32 iRange = -1);
	void			StepProgress();
	void			SetControlsState(E_LoginState eLoginState);
	static bool32	SetUserInfoStatus(CString strUserName,BOOL bAutoLogin = FALSE,BOOL bManualOptimaze =FALSE);
	static bool32	SetUserToolBarStatus(CString strUserName,BOOL bShowSCToolBar = FALSE,BOOL bManualOptimaze =FALSE);

    BOOL            IsDefaultUser() { return m_bDefaultUser; }

public:
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//// ѡ��ģ������
	//void			GetPickModelInfoFromXml();

private:
	bool32			ConnectAuth(int32 iIndexServer = -1);
	void			ReLoadAuthModule();
	void			ReSetLogData();

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// ����Ƿ��ڰ�ť��

private:
	void			SetNet();
	void			Connect(bool32 bCustom);	
	void            ShowThirdPartyLogin(int32 iID);//������������¼��

	void			DrawTitleBar();
	void			DrawLogo(Gdiplus::Graphics &graphics, const RectF &rcF);	// ���Ʊ�������LOGO
public:
	static void	    MD5(std::string strSr, std::string &strResult);		// md5����

public:
	T_HostInfo		m_HostInfoDefault;	
	int32			m_iHostIndex;
	
protected:	
	CFont			m_fontStaticText;
	CFont			m_fontCheckText;
	
	bool32			m_bSameUserLogin;				//	�Ƿ���ͬ�˻��ظ���¼
	bool32			m_bNewUser;
	bool32			m_bUseProxy;					// �Ƿ�ʹ�ô���
	bool32			m_bGreenChannel;				// �Ƿ�ʹ����ɫͨ��
	
	bool32			m_bLogInError;					// �Ƿ��ȡ�����ļ�ʧ��

	CArray<T_HostInfo, T_HostInfo>	m_aHostInfo;	// ��������Ϣ
	CArray<T_UsersInfo, T_UsersInfo&>	m_aUserInfo;	// �û���Ϣ
	CArray<T_PlusInfo,T_PlusInfo>       m_aPlusInfo;
	vector<UINT>	m_vUiServerBackup;				// ���ö˿���Ϣ

	
private:
	CAbsLoginDlgObserver	&m_LoginDlgObserver;
	int32					m_iIndexCurAuth;
	bool32					m_bCancelAuth;
	bool32					m_bFirstTime;	
	DWORD					m_nAutoLoginTimer;

private:
	BOOL			m_bSavePwd;
	BOOL            m_bSaveUser;
	BOOL			m_bManualOptimaze;
	int32			m_indexServerSel;		// ѡ��ķ���������
	CString			m_strSelService;		// ѡ��ķ�����
	
private:
	BOOL		m_bDefaultUser;
	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// ��ʶ�����밴ť����
	bool32		m_bShowWeb;					// ����ַ��ʱ��, ����ͼƬ
	CRect		m_rcCaption;				// ������������
	CRect		m_rcLeft;					// ������������
	CRect		m_rcRight;					// ��¼�������

	Image		*m_pImgCaption;
	Image		*m_pImgLogo;
	Image		*m_pImgTop;
	Image		*m_pImgBottom;
	Image		*m_pImgCheck;
	Image		*m_pImgUnCheck;

	Image       *m_pImgLoginClose;
	Image       *m_pImgLoginConnect;
	Image       *m_pImgRegister;
	Image		*m_pImgLoginSetting;
	Image       *m_pImgLoginQQ;
	Image       *m_pImgLoginWechat;

	//���Linkbutton
	CHButton m_CtrBtnReg;
	CHButton m_CtrBtnVisitor;
	CHButton m_CtrBtnOffline;
    CHButton m_CtrBtnPasswordRetake;

	CString		m_strAppName;
	CDlgNewCommentIE  *m_pDlgComment;		// ����

public:
// Dialog Data
	//{{AFX_DATA(CDlgNewLogin)
	enum { IDD = IDD_DIALOG_NEW_LOGIN };
		// NOTE: the ClassWizard will add data members here
	CTextProgressCtrl	m_CtrlProgressLogin;
	CDlgStatic			&m_CtrlStaticPrompt;
	CDlgStatic			&m_CtrlStaticPrompt2;
	CEditFlat      m_CtrlEditPassword;
	CEditFlat       m_CtrlEditName;

	CString			m_StrUserName;
	CString			m_StrPassword;
	int				m_envIndex;
	BOOL            m_bAutoLogin;
	BOOL            m_bShowSCToolBar;	
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewLogin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewLogin)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	// �����¼
	afx_msg LRESULT OnMsgViewDataOnAuthSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataAuthSuccessTransToMain(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataConfigSuccessTransToMain(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAuthFail(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAllMarketInitializeSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataRequestTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataCommResponse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerLongTimeNoRecvData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgViewDataOnBlockFileTranslateTimeout(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMsgMainFrameManualOptimize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebThirdLoginRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCloseDlg(WPARAM wParam, LPARAM lParam);
	

	//
	afx_msg LRESULT	OnMsgDllTraceLog(WPARAM w, LPARAM l);	
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnButtonReg();
	afx_msg void OnButtonVisitor();
	afx_msg void OnButtonOffLine();
    afx_msg void OnBnClickedBtnPasswordRetake();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnEnChangeEditUser();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWLOGIN_H__DEB7F96B_BA0C_4D8B_894D_49900A1E0D90__INCLUDED_)

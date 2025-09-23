#ifndef _DLGTRADELOGIN_H_
#define _DLGTRADELOGIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTradeLogin.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgTradeLogin dialog
#include "DialogEx.h"
#include "FontClrLable.h"
#include "DlgStatic.h"
#include "TradeLoginInterface.h"
#include "WndRectButton.h"
#include "AuthoPlugin.h"
#include "DlgNetWorkSet.h"
#include "NCButton.h"
#include "EditFlat.h"
#include "HButton.h"

#include <vector>
using std::vector;
  
#define  BaseDialog	CDialog
class CDlgTradeLogin : public CDialog, public iTradeLoginNotify
{
private:
	struct T_TradeServerInfo 
	{
		CString		StrName;
		CString		StrIP;
		UINT		nPort;

		T_TradeServerInfo()
		{
			nPort = 0;
		}
	};
	typedef vector<T_TradeServerInfo>	TradeServerInfoVector;

	// 交易登录需要的认证信息
	struct T_TradeAuthInfo 
	{
		CString		StrCompanyName;		// 公司名称
	};

	enum E_LoginStatus
	{
		ELSNotLogin = 0,		// 未登录
		ELSLogining,			// 登录ing
		ELSLogined,				// 已登录
	};

	// Construction
public:
	CDlgTradeLogin(E_TradeLoginType eTradeType, CWnd* pParent = NULL);   // standard constructor
	~CDlgTradeLogin();
	
	// iTradeLoginNotify
public:
	virtual void	OnLoginOK();
	virtual void	OnLoginUserStop();			// 登录终止了
	virtual void	OnLoginError(const CString &StrErr);
	virtual void	OnLoginStep(const CString &Str);		// 登陆时的步骤显示
	virtual void	OnUpdateValidCode();

public:
	// 初始化服务器信息，用户信息等，必须在认证登录完成后
	void			InitLogin(iTradeLogin *pLogin);

	int				Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }

private:
	// 初始化服务器信息
	void			InitServerInfo();

	// 初始化登录信息，在服务器初始化后
	void			InitLoginInfo();
	void			LoadLoginInfo();
	void			SaveLoginInfo();
	void			GetRegisterAddress();

	void			ShowPrompt(const CString &Str);

	void			SetCtrlByLoginStatus(int iLoginStatus);

	bool			GetUserInputLoginInfo();

//	void			LoadTradeAuthInfo(T_TradeAuthInfo &stAuth);

public:
	// Dialog Data
	//{{AFX_DATA(CDlgTradeLogin)
	enum { IDD = IDD_DIALOG_TRADELOGIN };
	CComboBox		m_ComboServer;
	CEditFlat		m_EditUser;
	CEditFlat		m_EditPassword;
	CDlgStatic		&m_StaticPrompt;
	CEditFlat		m_CtrlEditValidCode;
	CString			m_StrValidCode;
	CString			m_StrUser;
	char m_chRegisterAddress[128];
	//}}AFX_DATA

private:
	CPoint			m_ptLastCenter;

	iTradeLogin		*m_pLogin;

	TradeServerInfoVector	m_aServer;
	T_TradeServerInfo		m_stLastServerSel;

	T_TradeLoginInfo	m_stLoginInfo;
	T_TradeAuthInfo		m_stAuthInfo;

	E_LoginStatus		m_eCurLoginStatus;
public:
	void GetValidCode();
	void SetProtect(BOOL bProtect);
	bool FromXml();
	bool32 GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo);

private:
	void	AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int		TButtonHitTest(CPoint point);	// 鼠标是否在按钮上
	void	DrawTitleBar();
	void	DrawLogo(Gdiplus::Graphics &graphics, const RectF &rcF);	// 绘制标题栏的LOGO
	void	OnBtnProtect();
	void	OnOK();
	void	OnButtonSetPro();
	void	SimulateTradeLogin();
	void	HideControl(bool isSimulate);	// 如果是模拟效果，则的许多控件不需要显示，因为它是自动登录的。

private:
	char*		m_pDataValidCode;
	int			m_iLenImgValidCode;
	Image*		m_pImageValidCode;
	CBitmap		m_bmpBackgroud;
	Image *m_pImageLogin;
	int32			m_iCloseBtnState;	// 0 - 正常情况, 1 - enter, 2 - press
	CPoint			m_ptLastPress;
	// 	CWndRectButton		*m_pBtnClose;
	// 	CWndRectButton		
	int m_iLinkID;
	CDlgNetWorkSet n_dlgNetWork;

	bool32			m_bUseProxy;					// 是否使用代理
	CString			m_sValidCode;
	int m_x1[3], m_x2[3], m_y1[3], m_y2[3];
	CRect		m_rcCaption;			// 标题栏的区域
	CRect		m_rcBg;					// 背景的区域
	CRect		m_rcYzm;				// 验证码的区域
	Image		*m_pImgCaption;
	Image		*m_pImgBg;
	Image		*m_pImgLogo;			// 标题栏的LOGO图片
	Image		*m_pImgCheck;
	Image		*m_pImgUnCheck;
	Image       *m_pImgLoginClose;
	Image       *m_pImgLoginSetting;
	Image       *m_pImgLoginConnect;

	CString		m_strAppName;			// 应用程序名字public:
	
	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// 标识鼠标进入按钮区域

	CHButton	m_CtrBtnReg;
	CFont		m_fontStaticText;

	BOOL		m_bSaveUser;
	BOOL		m_bProtect;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTradeLogin)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgTradeLogin)
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnChangeValidCode();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnEnChangeEditTradeUser();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnButtonApply();
	LRESULT OnMsgGetValidcode( WPARAM w, LPARAM l );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!_DLGTRADELOGIN_H_
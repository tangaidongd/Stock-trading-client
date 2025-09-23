#if !defined(AFX_STATUSBAREX_H__)
#define AFX_STATUSBAREX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusBarEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx window
#include "facescheme.h"
#include "XTipWnd.h"


#include "AlarmCenter.h"
#include "iAuthNotify.h"
#include "keyboarddlg.h"
#include "EditFlat.h"
#include "SpecialCEdit.h"
#include "XTimer.h"


class auth::iServiceDispose;
class auth::iProxyAuthClient;

const int32 KMaxNumStatus = 4;

class CStatusBarEx;
class CRectData : public CRect
{
public:
	CRectData::CRectData() 
	{
		// AfxMessageBox(L"NULL");
		m_pParent = NULL;
		SetRectEmpty();
	}

public:
	CStatusBarEx* m_pParent;
	int32	m_iMinWidth;
	CString m_StrName;
	CString m_StrNow;
	CString m_StrCompare;
	CString m_StrAmount;
	
	const CRectData& operator=(const CRect& rect)
	{
		this->left = rect.left;
		this->right = rect.right;
		this->top = rect.top;
		this->bottom = rect.bottom;
		return *this;
	}
	void DrawRect( CDC* pDC,CRectData& rectData );
	void DrawRectRow2(CDC* pDC,CRectData& rectData );
	void DrawRectBBS(CDC* pDC,CRectData& rectData, CRect &RectShow);
	void DrawRectAlarmMerch(CDC * pDC,CRectData & rectData);
	void DrawRectNetWork(CDC * pDC,CRectData& rectData);
	void DrawRectRecentMerch(CDC * pDC,CRectData& rectData);
	void DrawShowTips(CDC * pDC,CRectData& rectData);

	void DrawRectLeftBar(CDC * pDC,CRectData& rectData);
	// ����������ť
	void DrawRectSearch(CDC *pDC,CRectData& rectData, int icltHeight);

	void DrawRectDuoKongZhenXian(CDC *pDC, CRectData &rectData);	// ���ƶ������

	void DrawRectTodayComment(CDC * pDC,CRectData& rectData); //����
	void DrawRectMessage(CDC * pDC,CRectData& rectData);//IM��Ϣ
	bool HitTest ( CPoint pt, int32 iMouseAction );
};

struct T_OrgAdvertisement		// �������
{
	CString m_StrTitle;			// ����
	CString m_StrUrl;			// url
	CString m_StrContent;		// ����
	int32	m_iIndex;			// ���
};

class CStatusBarEx : public CStatusBar, public CXTimerAgent, public CAlarmNotify
{
friend class CRectData;

public:

typedef CArray<CMerch *, CMerch *> MerchArray;

struct T_BarMerch
{
	T_BarMerch();
	T_BarMerch(const CString &StrName, int32 iMarket, const CString &StrCode);

	//
	CString m_StrName;
	int32	m_iMarket;
	CString m_StrCode;
	CMerch  *m_pMerch;		// ��ֵ�п���ΪNULL
};

// Construction
public:
	CStatusBarEx();
	
public:
	void			UpdateTime();
	void			UpdateNetworkPic();
	void			UpdateNetworkDetail();
	
	//
	void			PromptLongTimeNoRecvData(bool bErr = true);
	void			PromptSomePackOutTime(bool bErr = true);

	void			GetStatusBarMerchs(OUT MerchArray &aMerchs);		// !!�����������п�����NULL��Ʒָ��
	void			SendStatusBarMerchs(IN MerchArray &aMerchs);

	void			UpdateStatusBarMerch(int32 iIndex, const T_BarMerch &barMerch);		// ����barmerch����Ϣ���������ݣ�������ʾ

	void			InitStatusBBSLogFont(OUT LOGFONT &lf);	// ��ʼ�����ʹ�õ�����
	CSize			GetProperBBSSizeShow();		// ���ݵ�ǰ�Ĺ�����ݣ���ȡ���ʵĹ�����С
	int32			GetAdvIndexByPoint(CPoint pt);	// ��ȡpt���Ĺ��������û����-1

	auth::iServiceDispose	*GetServiceDispose();
	auth::iProxyAuthClient	*GetAuthManager();


	
////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarEx)
	//}}AFX_VIRTUAL
	// Implementation
public:
	virtual ~CStatusBarEx();
	void   GetClientRect(LPRECT pRect);
	void   UpdateRect ( int32 iWidth, int32 iHeight );
	void   HitTestRectData ( CPoint pt,int32 iType );
	void   SetMKNumber ( int32 iNumber );
	int32  GetMKNumber();
	void   SetRectData(int32 iCurSel,CRectData RectData);
	void   GetRectData(int32 iCurSel,OUT CRectData& RectData);
	int32  GetCurSel();
	int32  GetStatusBarHeight();
	void   ShowMerchData(int32 iMerchIndex);

	void   CreateSpiriteDialog();
	void   CreateAlarmDialog(bool32 bShow = true);
	void   CreateNetWorkDialog();
	void   CreateRecentMerchDialog();
	void   CreateIndexPromptDialog();
	void   CreateNewsView(); //��������Ƹ���Ѷ�����оʹ�
	void   ShowImMessage();
	void   HideTips();
	CString  GetTime(UINT uiSeconds);

	void	LoadFromXml();
	void	SaveToXml();

	void	OnRealTimePriceUpdate();		// ʵʱ���۸���

	void	OnDuoKongGeneralNormalUpdate();		// ����������ݸ���


	void   GetKeyBoarEditRect(CRect &rc);
	void   SetKeyBoarEditText(CString szDisData);

	virtual BOOL  PreTranslateMessage( MSG* pMsg );
	
	//	��ʼ��״̬����
	void	InitStatusEx();
	void	UnInit();

// from CXTimerAgent
public:
	virtual	void	OnXTimer(int32 iTimerId);

// from AlarmNotify
public:
	// �����иı�
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify);
	
	// �ﵽԤ������, ������
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt);
	
	// ������
	virtual void AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt);

	

// from iAuthUserDataNotify
public:
	virtual void OnOrgDataUpdate();
	
private:
	CAlarmCenter*	GetAlarmCenter();



	void    CreateAndDisPlayKeyborad();				//��������ʾ���̾���
	void	RefreshSpecifiedArea();												//ˢ�¾ֲ�����
		
	LRESULT SearchEditClick( WPARAM w, LPARAM l );


public:
	bool32		m_bShowNetworDetailNotity;	    //	�Ƿ���ʾ��������ʧ������֪ͨ
	bool32		m_bConnect;				// �Ƿ��Ѿ�����������
	bool32		m_bConnectNews;			// �Ƿ���������Ѷ
	int32		m_iShowPic1;
	int32		m_iUpdateTimeCounter;
	int32		m_iUpdateNetworkPicCounter;
	int32		m_iUpdateNetworkDetailCounter;

	CMap<DWORD, DWORD, int32, int32> m_mapNetworkNotifyCount;		// ÿ���������Ͷ�Ӧ�ı�������
	//int32		m_iNetworkNotifyCount;		// ���������������޾�����ʾ

	// ���
	CArray<T_OrgAdvertisement, const T_OrgAdvertisement &> m_aOrgAdvs;
	struct T_OrgAdvRect : public CRect
	{
		int32 iOrgIndex;		// ��Ӧ�Ĺ��index
	};
	CArray<T_OrgAdvRect, const T_OrgAdvRect &> m_aOrgAdvRects;		// ÿ���������

private:
	CKeyBoardDlg *m_pDlgKeyBoard;
	int32		m_iHeight;
	CRectData	m_RectMerchKind[KMaxNumStatus];
	T_BarMerch	m_BarMerchKind[KMaxNumStatus];		// ��ǰ ״̬����Ʒ�б�
	int32		m_iBarMerchCur;					// ��ǰ����hotkey�ı��Ŀ��
	const int32 m_ciBarMerchCount;

	//CBindHotkey	m_bindHotKey;					// ��־Ҫ��ȡhotkey��		// ֱ�ӵ���mainFrame��showdlg

	int32		m_iMerchKindNumber;				// �������������mRectMerchKind��������, ӦΪ�������䶯
	CRectData	m_RectBBS;
	CRect		m_RectShow;

	CRectData	m_RectRecentMerch;
	CRectData	m_RectTitle;	
	CRectData	m_RectAlarm;
	CRectData	m_RectNetwork;
	CRectData	m_RectTime;
	CRectData   m_RectTodayComment;//����
	CRectData   m_RectMessage;//IM��Ϣ
	CRectData	m_RectSearch;
	CRectData	m_RectScToolBar;	// ��������ťλ��
	CRectData	m_RectShowTips;		// ��ʾ����

	CSpecialCEdit	m_KeyBoarEdit;
	bool			m_bKeyBoardEditClick;		//�Ƿ��û������������
	CRect			m_KeyBoarEditRect;
	CFont			m_EditFont;
	
	

	

	CRectData	m_RectDuoKong;	// �������
	T_GeneralNormal	m_LastOldDuoKongH;		// �������ϵ�һ�ʶ�� - �Ϻ�
	T_GeneralNormal	m_LastOldDuoKongS;		// �������ϵ�һ�ʶ�� - ����
	CArray<int32, int32>	m_aDuoKongImageH;	// ���ͼƬ - �Ϻ�
	CArray<int32, int32>	m_aDuoKongImageS;	// ���ͼƬ - ����
	
	int32		m_iCurSel;
	CRectData	m_RectNow;
	CString		m_StrCurTime;
	CXTipWnd	m_TipWnd;
	CString		m_StrTips;

	BOOL		m_bMouseTrack;
	CPoint		m_ptLastMouseMove;
	
	Image*		m_pImageBkGround;
	Image*		m_pImageAlarmOn;	
	Image*		m_pImageAlarmOff;
	Image*		m_pImageNetOn1;
	Image*		m_pImageNetOn2;
	Image*		m_pImageNetOn3;
	Image*		m_pImageNetOn4;
	Image*		m_pImageNetOff;
	Image*		m_pImageRecentMerch;
	Image*		m_pImageBkBBS;
	Image*		m_pImageBkBBSLeft;
	Image*		m_pImageBkBBSRight;
	Image*      m_pImageTodayCommentOn;
	Image*      m_pImageTodayCommentOff;
	Image*      m_pImageTodayCommentRed;
	Image*      m_pImageTodayCommentBlue;
	Image*      m_pImageMessage;
	Image*      m_pImageMessageShow;
	Image*      m_pIMageMessageHide;
	Bitmap*		m_pImageDuoKongSmallBmp;
	Image*		m_pImageSearch;
	Image*		m_pImageSearch_BK;
//	Image*		m_pImageSiderBarHidden;
//	Image*		m_pImageSiderShow;	
	Image*		m_pImageSCToolBar;	


	bool32		m_bMember;
	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBarEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point32);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point32);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point32);
	afx_msg void OnUpdateShow2(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenu ( UINT nID );
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseHover(WPARAM nFlags, LPARAM point);
	afx_msg LRESULT OnMouseLeave(WPARAM , LPARAM );
	afx_msg LRESULT OnMsgHotKey(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMsgDlgHotKeyClose(WPARAM w, LPARAM l);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void			DoOnOrgDataUpdate();
	afx_msg void OnChangeEdit();
	afx_msg void OnSetEditFocus();
	afx_msg void OnKillEditFocus();
	
	

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSBAREX_H__)

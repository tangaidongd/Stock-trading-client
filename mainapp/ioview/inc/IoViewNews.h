#ifndef _IO_VIEW_NEWS_H_
#define _IO_VIEW_NEWS_H_

#include "IoViewBase.h"
#include "WndCef.h"
#include "UrlParser.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewNews
class CIoViewNews : public CIoViewBase
{
	// Construction
public:
	CIoViewNews();
	virtual ~CIoViewNews();
	void SetLobbyUrl(const CString& strUrl){m_strLobbyUrl = strUrl;}	// ����ֱ��������ҳ��ַ
	DECLARE_DYNCREATE(CIoViewNews)
	// from CWnd
public:
	virtual	BOOL	DestroyWindow();
	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	// from CIoViewBase
public:

	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);

	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	
	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

    void			RefreshWeb(CString strUrl);
   

	void			ChangeWebStatus(bool32 bActive);

	UrlParser			SetCustomUrlQueryValue(CString strKey, CString strValue);
	UrlParser			GetCustomUrl(); //	��ȡ��ǰ��ҳ���Զ�����ҳ


public:
	static CString GetOrgCode(LPCTSTR urlParam = NULL,bool BSha1 = true);
	CString GetWebNewsUrl(LPCTSTR urlAddParam = NULL);
	CString GetWebNewsUrlByTreeId(CString StrTreeId);
	CString	GetWebNewsListUrl();
	CString	GetStockNewsUrl();
	CString	GetQuantifyTraderUrl();
	CString GetLiveVideoUrl(CString StrOldUrl);

	// ִ�к���
	void FrameExecuteJavaScript(char* pstrFun);

	// web�˷��أ���ʵ����ע���
	void JavaScriptResultValue(char* pstrFun);


	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch){}
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch){return false;}
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerch){}
	virtual void	OnVDataClearCompareMerchs(){}
	virtual void	OnVDataMarketSnapshotUpdate(int32 iMarketId){}
	virtual void	OnVDataMerchInfoUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType){}
	virtual void	OnVDataRealtimeLevel2Update(IN CMerch *pMerch){}
	virtual void	OnVDataRealtimeTickUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataBrokerInfoUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataBlockInfoUpdate(){}
	virtual void	OnVDataMyselUpdate(){}
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs){}
	virtual void	OnVDataRelativeMerchsUpdate(){}
	
private:
	void			OpenWebNewsCenter();	// ����ҳ������Ѷ����
    
	void			InitButtons();
	int32			TButtonHitTest(CPoint point);	// ��ȡ��ǰ������ڵİ�ť
	////////////////////////////////////////////////////////////

	void			ShowMoreInWsp();		// �ڰ�������ʾ����url
	void			ShowMoreInDlg();		// �ڶԻ�����ʾ����url
	void			ShowMoreInExternal();	// ���ⲿ�������ʾ����url
	
	
//	void			ShowInformationCentreNews(CString strParser, bool32 bRefresh, bool32 bIsSha1 = true); // ҳ����ת���ػ�������֮��ĸ�����Ŀ
	//
public:
	bool32		m_bFromXml;				// �²������ͼ���ǰ���򿪵�

protected:

	// ��ťID
	enum
	{
		BTN_NAME=1,
		BTN_MORE,
	};

	// ���ఴť����ҳ�ķ�ʽ
	enum E_MoreShowType
	{
		EMST_WSP = 0,	// �ڰ����
		EMST_DLG,		// �����Ի����
		EMST_WEB		// ���ⲿ�������
	};

	CWndCef m_wndCef;
	UrlParser		m_CustomUrl;

	bool			m_bInitialized;
	CString         m_StrMoreUrl;
	CString			m_StrShowName;	// ��ʾ��ť������, Ҳ��"����"��ť��Ӧ�İ���
	E_MoreShowType	m_eMoreShowType;	// ���ఴť����ҳ�ķ�ʽ
	int32			m_iTimerPeriod;
	int32			m_isWebNews;	// 0.�������ȵĶ��� 1.������ҳ��Ѷ���� 2.������Ƕ��ֱ������ 3.������ 4.������Ƕ������ֱ�� 5����ֱ��(����������ʾ����) 6.������Ѷ 7.��ѡ����Ѷ 8.�������� 9.�¹���Ѷ 10.�۹���Ѷ
	CString			m_strLobbyUrl;	// ֱ�����������ӵ�ַ
	CString         m_StrTreeId;

	int32		m_iMoreBtnHovering;		// ��ʶ�����밴ť����
	CNCButton	m_BtnName;				// ����
	CNCButton	m_BtnMore;				// ���ఴť
	int32		m_iWebYPos;				// ��ҳ�ؼ� Y����ʼ����

	COLORREF	m_clrNameBk;			// ���ֵı�����ɫ
	COLORREF	m_clrNameText;			// ���ֵ��ı���ɫ

	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewNews)
	//}}AFX_VIRTUAL
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewNews)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_

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
	void SetLobbyUrl(const CString& strUrl){m_strLobbyUrl = strUrl;}	// 设置直播大厅网页地址
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
	UrlParser			GetCustomUrl(); //	获取当前网页的自定义网页


public:
	static CString GetOrgCode(LPCTSTR urlParam = NULL,bool BSha1 = true);
	CString GetWebNewsUrl(LPCTSTR urlAddParam = NULL);
	CString GetWebNewsUrlByTreeId(CString StrTreeId);
	CString	GetWebNewsListUrl();
	CString	GetStockNewsUrl();
	CString	GetQuantifyTraderUrl();
	CString GetLiveVideoUrl(CString StrOldUrl);

	// 执行函数
	void FrameExecuteJavaScript(char* pstrFun);

	// web端返回，其实不关注这个
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
	void			OpenWebNewsCenter();	// 打开网页新闻资讯中心
    
	void			InitButtons();
	int32			TButtonHitTest(CPoint point);	// 获取当前鼠标所在的按钮
	////////////////////////////////////////////////////////////

	void			ShowMoreInWsp();		// 在版面里显示更多url
	void			ShowMoreInDlg();		// 在对话框显示更多url
	void			ShowMoreInExternal();	// 在外部浏览器显示更多url
	
	
//	void			ShowInformationCentreNews(CString strParser, bool32 bRefresh, bool32 bIsSha1 = true); // 页面跳转加载机构代码之类的附加项目
	//
public:
	bool32		m_bFromXml;				// 新插入的视图还是版面打开的

protected:

	// 按钮ID
	enum
	{
		BTN_NAME=1,
		BTN_MORE,
	};

	// 更多按钮打开网页的方式
	enum E_MoreShowType
	{
		EMST_WSP = 0,	// 在版面打开
		EMST_DLG,		// 弹出对话框打开
		EMST_WEB		// 在外部浏览器打开
	};

	CWndCef m_wndCef;
	UrlParser		m_CustomUrl;

	bool			m_bInitialized;
	CString         m_StrMoreUrl;
	CString			m_StrShowName;	// 显示按钮的名称, 也是"更多"按钮对应的版面
	E_MoreShowType	m_eMoreShowType;	// 更多按钮打开网页的方式
	int32			m_iTimerPeriod;
	int32			m_isWebNews;	// 0.代表最先的定义 1.代表网页资讯中心 2.代表内嵌的直播大厅 3.代表广告 4.代表内嵌的文字直播 5文字直播(带参数，显示更多) 6.个股资讯 7.自选股资讯 8.量化操盘 9.新股资讯 10.港股资讯
	CString			m_strLobbyUrl;	// 直播大厅的链接地址
	CString         m_StrTreeId;

	int32		m_iMoreBtnHovering;		// 标识鼠标进入按钮区域
	CNCButton	m_BtnName;				// 名字
	CNCButton	m_BtnMore;				// 更多按钮
	int32		m_iWebYPos;				// 网页控件 Y的起始坐标

	COLORREF	m_clrNameBk;			// 名字的背景颜色
	COLORREF	m_clrNameText;			// 名字的文本颜色

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

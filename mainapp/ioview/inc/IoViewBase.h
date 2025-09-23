#ifndef _IO_VIEW_BASE_H_
#define _IO_VIEW_BASE_H_


#include "MerchManager.h"
#include "ControlBase.h"
#include "proxy_auth_client_base.h"
#include "tinyxml.h"
#include "sharestruct.h"
#include "facescheme.h"
#include "ioviewtitle.h"
#include "NewMenu.h"
#include "ReportScheme.h"
#include "ViewDataListener.h"
#include "IRegionData.h"
#include "memdc.h"


using namespace auth;

class CAbsCenterManager;
class CIoViewManager;
class CGGTongView;
class CDelayKeyDown;



/////////////////////////////////////////////////////////////////////////////
// CIoViewBase

extern const char* KIoViewKLineWeightType;
typedef struct _GV_DRAWMARK_ITEM GV_DRAWMARK_ITEM;
struct T_MarkData;

class CIoViewBase : public CControlBase, public CIoViewListner,public IIoViewBase
{
	DECLARE_DYNAMIC(CIoViewBase)
public:
	typedef			CArray<CGmtTime, const CGmtTime &>  GmtTimeArray;		// 与市场ioc一致
	typedef			CArray<CMerch *, CMerch *>			MerchArray;

	static enum E_WeightType
	{
		EWTPre = 0,
		EWTAft,
		EWTNone,

		EWTCount,
	};

	enum E_IoViewType	// 业务视图类型
	{
		EIVT_SingleMerch = 0,	// 单商品视图 k 分 闪等
		EIVT_MultiMerch,		// 多商品视图 报价 排行 星空等
		EIVT_MultiBlock,		// 多板块视图 板块列表
	};


public:
	friend CIoViewManager;

	// Construction
public:
	CIoViewBase();
	virtual ~CIoViewBase();
	virtual int32 GetChartType(){return -1;}
public:
	int32			GetIoViewGroupId();	// 业务视图分组以IoViewManager为基本分配单位， <= 0 表示独立视图， 不需要同其他视图分享数据
	int32			GetGuid();
	void			SetCenterManager(CAbsCenterManager *pAbsCenterManager);
	CAbsCenterManager*		GetCenterManager()	{ return m_pAbsCenterManager; }
	void			SetIoViewManager(CIoViewManager *pIoViewManager) { m_pIoViewManager = pIoViewManager; }
	void			SetBiSplitTrackDelFlag(bool32 bDel);
	CIoViewManager* GetIoViewManager()								 { return m_pIoViewManager; }
	CGGTongView*	GetParentGGtongView();

	bool32			IsNeedPushData(CMerch *pMerch, E_DataServiceType eDataServiceType);		// 是否需要注册该服务类型的推送，默认全部都有

	static void		AppendIoViewsMenu(CNewMenu* pMenu, bool32 bLockSplit=false);
	void			AppendStdMenu(CMenu *pMenu, DWORD dwFlag = 0);
	void			AppendStdMenu(CNewMenu *pNewMenu, DWORD dwFlag = 0);
	void			BringToTop(CString StrBlockName = L"");
	void			DrawCorner(CMemDC *pMemDc, CRect rcWindows);
private:
	void			GetPolygon(CRect rect, CPoint ptsLeft[7] ,CPoint ptsRight[7], int iMaxLength = 12, int iMinLength = 8, int iCornerBorderH = 1);
	void			DrawPolygonBorder(CMemDC *pMemDc, CRect rect, COLORREF clrFill = RGB(234, 23, 23), COLORREF clrBorder = RGB(234, 23, 23));
public:
	//from CIoViewListner
	virtual void	OnVDataMarketSnapshotUpdate(int32 iMarketId){}
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType) {}
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch) {}
	virtual void	OnVDataRealtimeLevel2Update(IN CMerch *pMerch) {}
	virtual void	OnVDataRealtimeTickUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch){}
	virtual void	OnVDataRelativeMerchsUpdate() {}	
	virtual void	OnVDataGeneralNormalUpdate(CMerch* pMerch) {}
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch* pMerch){}
	virtual void	OnVDataMerchTrendIndexUpdate(CMerch* pMerch) {} //@
	virtual void	OnVDataMerchAuctionUpdate(CMerch* pMerch){} //@
	virtual void	OnVDataMerchMinuteBSUpdate(CMerch* pMerch) {}
	virtual void	OnVDataClientTradeTimeUpdate(){}
	virtual void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort,int32 iPosStart, 
		int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs) {}
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, 
		int32 iOrgMerchCount, const CArray<CMerch*, CMerch*> &aMerchs) {}

	virtual void	OnVDataEconoData(const char *pszRecvData){}

	virtual void	OnVDataStrategyData(const char *pszRecvData){}

	virtual void	OnVDataQueryNewStockInfo(const char *pszRecvData){}

	virtual void	OnVDataNewsListUpdate() {} //@
	virtual void	OnVDataF10Update() {} //@
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch) {}
	virtual void	OnVDataNewsPushTitle(const T_NewsInfo& stNewsInfo) {}
	virtual void	OnVDataNewsTitleUpdate(IN listNewsInfo& listTitles) {}
	virtual void	OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo) {}
	virtual void	OnVDataCodeIndexUpdate(IN const CString& StrCode) {} 

	virtual void	OnVDataForceUpdate() {}
	virtual void	OnWeightTypeChange() {}

	virtual bool32	IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType);
	virtual bool32	IsAttendNews(IN CMerch* pMerch);
	virtual bool32	IsNowCanReqData();
	virtual	DWORD	GetNeedPushDataType();
	virtual void    ForceUpdateVisibleIoView();
	virtual void	GetSmartAttendArray(OUT SmartAttendMerchArray &aAttends);
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);	// 获取该视图感兴趣的市场和市场数据

	//from myself
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) = 0;     // 通知视图改变关注的商品

	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType) {}										  // 表头发生变化										 
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName ){} // 指标公式发生变化
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch){ return false;}	     // 叠加视图相关
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerch) {}
	virtual void	OnVDataClearCompareMerchs(){}
	virtual void    OnIoViewGroupIdChange(int32 iNewGroupId){}	

	virtual bool32	IsNowCanRefreshUI();	// 视图不可见的时候不刷新界面

public:
	static const char*	GetXmlElementValue();
	static const char*	GetXmlElementAttrIoViewType();
	static const char*	GetXmlElementAttrMarketId();
	static const char*	GetXmlElementAttrMerchCode();
	static const char*	GetXmlElementAttrShowTabName();

public: // virtual

	virtual bool32	FromXml(TiXmlElement * pElement) = 0;
	virtual CString	ToXml() = 0;
	virtual CString	GetDefaultXML() = 0;
	virtual void	SetChildFrameTitle() = 0;

	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	// --chenfj
	virtual void	OnIoViewTabShow(){};		// 点击tab，该视图显示
	virtual void	OnIoViewTabHide(){};		// 点击tab，该视图隐藏

	// 切换商品时候, 取下一个商品
	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);/*{return NULL;}*/

	virtual CMerch* GetMerchXml();		// 获取视图有兴趣的一个xml
	virtual void	GetMerchXmls(OUT MerchArray &aMerchs);


	virtual E_IoViewType GetIoViewType() { return EIVT_SingleMerch; }	// 返回视图类型，默认单商品
	virtual	void	OnEscBackFrameMerch(CMerch *pMerch){}			// 当使用ESC返回视图页面时，调用新激活的页面中所有视图的该接口

	virtual void    OnUpdateIoViewMenu(CCmdUI *pCmdUI);			// mainframe询问cmd的状态

	virtual	bool32	IsIoViewContainer() { return false; }		// 是否是IoView容器视图，下面是否还存在子视图
	virtual	CIoViewBase* GetActiveIoView();					// 获取该视图下当前的激活子视图，默认就是自己

	virtual	bool32	OnSpecialEsc(){ return true; };							// 子视图在处理特殊的返回时，返回false表示不要继续了，true继续处理

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return true;}
	virtual void	LockRedraw(){}
	virtual void	UnLockRedraw(){}
	//判断窗口是隐藏或者显示
	virtual void    OnShowViews(BOOL bShow){};

	virtual	void	KLineCycleChange(UINT nID){};
public: // virtual #viewdata notify ioview
	// 
	virtual void	RequestViewData(){}

	// 字体发生变化
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();

	// 北京项目数据统一接口 - 交给具体视图去分析
	virtual bool32	IsPluginDataView() { return false; }	// 返回一个标志还是一个bool？
	virtual	void	OnVDataPluginResp(const  CMmiCommBase *pResp) {};
	virtual bool32  IsKindOfReportSelect() { return false; }

public:
	bool32			GetParentGGTongViewDragFlag();
	void			InitialIoViewFace(CIoViewBase * pIoView);

	void			SetColorsFromXml(TiXmlElement * pElement);				// 解析XML 得到颜色信息
	void			SetFontsFromXml(TiXmlElement * pElement);				// 解析XML 得到字体信息

	CString			SaveColorsToXml();
	CString			SaveFontsToXml();

	COLORREF		GetIoViewColor(E_SysColor eSysColor);
	LOGFONT*		GetIoViewFont(E_SysFont eSysFont);	
	CFont*			GetIoViewFontObject(E_SysFont eSysFont);

	//	根据列表内容获取指定样式的字体
	//	列表头
	LOGFONT*		GetColumnExLF(bool bBold = false);
	//	中文行字体
	LOGFONT*		GetChineseRowExLF(bool bBold= false);
	//	序号字体
	LOGFONT*		GetIndexRowExLF(bool bBold= false);
	//	数字行字体
	LOGFONT*		GetDigitRowExLF(bool bBold= false);
	//	商品代码行字体
	LOGFONT*		GetCodeRowExLF(bool bBold= false);
	

	CFont*			SetIoViewFontObject(E_SysFont eSysFont, LOGFONT& lf);
	void			SetIoViewFontArray(CFontNode (&aSysFont)[ESFCount]); 
	void			SetIoViewColorArray(COLORREF (&aSysColor)[ESCCount]);

	void			SetIoViewFontArraySave(CArray<T_SysFontSave,T_SysFontSave>& aSysFontSave);
	void			SetIoViewColorArraySave(CArray<T_SysColorSave,T_SysColorSave>& aSysColorSave);

	void			ChangeFontByUpDownKey(bool32 bBigger);
	BOOL			DealEscKey(bool32 bDealCross = true);										// 处理ESC 跳转
	BOOL			DealEscKeyReturnToReport();

	virtual void	DoShowStdPopupMenu();

	E_ReportType	GetMerchKind(IN CMerch *pMerch);

	CMerch*         GetMerchAccordExpMerch(CMerch* pMerch);	// 获取商品对应的指数

	// 由merch key转化为商品，不能转化的根据要求填NULL或者不填
	int32			ConvertMerchKeysToMerchs(const CArray<CMerchKey, CMerchKey &> &aKeys, OUT CArray<CMerch *, CMerch *> &aMerchs, bool32 bIgnoreMissedKey); // 换回实际转化的非NULL商品的数量
	CMerch*         ConvertMerchKeyToMerch(int32 iMarketId, const CString &StrMerchCode);

	// 标记相关
	bool32			InitDrawMarkItem(const T_MarkData &MarkData, OUT GV_DRAWMARK_ITEM &DrawMarkItem);	// 初始化绘制标记参数
	bool32			InitDrawMarkItem(CMerch *pMerch, OUT GV_DRAWMARK_ITEM &DrawMarkItem);
	// 使用是否自选股，标记绘制商品名称, dc中要设置好字体
	// 按照居中单行依次绘制类似  浦发银行(600000)T   字样, 如果不要绘制code则少去中间code部分
	void			DrawMerchName(CDC &dc, CMerch *pMerch, const CRect &rcText, bool32 bDrawCode = true, OUT CRect *pRectMark=NULL);	

	bool32			IsLockedSplit();



public:						

	static E_WeightType GetWeightType();
	static void			SetWeightType(E_WeightType eWeightType);
	static bool32		ReadWeightDataFromLocalFile(IN CMerch* pMerch, OUT CArray<CWeightData,CWeightData&>& aWeightData, OUT UINT& uiCrc32);

	// 前向与后向商品切换相关
	static CMerch		*PeekLastBackMerch(bool32 bRemove=false);	// 获取最后一个后向商品
	static void			AddLastCurrentMerch(CMerch *pMerch);		// 添加最后的当前商品
	static CMerch		*PeekLastPrevMerch(bool32 bRemove=false);	// 获取最后一个前向商品

public:
	static bool32	BePassedCloseTime(CMerch* pMerch);

public:
	bool32			IsActive() { return m_bActive; }
	void			SetActiveFlag(bool32 bActive) { m_bActive = bActive; }
	bool32			IsActiveInFrame();		// 是否在frame中active

	bool32			ParentIsIoView();		// 父亲窗口为ioview窗口，这种情况各种响应要作出调整

	void			SetHasIgnoreLastViewDataReqFlag(bool32 bIgnored);

	// 所有交易时间都是基于时间0
	bool32			GetNowServerTime(OUT CGmtTime &TimeNow);
	void			GetTradeTimes(IN const CGmtTime &TimeSpecify, IN const MerchArray &aMerchs, OUT GmtTimeArray &aTradeTimes );	// 指定日期获取合并后的交易时间段, 不计算休息日等无开盘日时间
	bool32			IsTimeInTradeTimes(IN const GmtTimeArray &aTradeTimes, IN const CGmtTime &TimeCheck); // 指定时间是否在交易时间段内
	bool32			IsNowInTradeTimes(IN const GmtTimeArray &aTradeTimes);		// 按照服务器时间，现在是否在交易时间段内
	bool32			IsNowInTradeTimes(IN const MerchArray &aMerchs, bool32 bCheckTradeDay = false); // checkTradeDay是否应当检查指定日期是否为交易日
	bool32			IsNowInTradeTimes(CMerch *pMerch, bool32 bCheckTradeDay = false);		// 检查指定单个商品是否在交易时间段内，！Null Merch是错误参数
	bool32			IsTimeInTradeDay(IN const MerchArray &aMerchs, const CGmtTime &TimeSpecify); // 指定时间是否在商品集合的交易日内

	static CMerch  *GetMerchAccordExpMerchStatic(CMerch* pMerch);
	static void		GetSpecialAccordExpMerch(OUT MerchArray &aExpMerchs);	// 获取重要的几个指数
protected:
	// !建议视图调用此方法请求viewdata数据，而不直接使用viewdata请求
	// bForceReq - 强制发送请求，不论窗口是否在隐藏状态
	// bLogIfCancel - 该请求如果由于窗口隐藏而忽略，是否做最后一个请求未发出标志
	// 返回值 - 该请求是否通过viewdata发送了
	bool32			DoRequestViewData(CMmiCommBase &req, bool32 bForceReq = false, bool32 bLogIfCancel = true);
	bool32			RequestLastIgnoredReqData();		// 发送最后一次过滤掉的请求，建议在OnIoViewActive中调用，替换每次强制的forceupdate

	void			RegisterHotKeyTarget(bool32 bReg);	// 注册键盘精灵

public:
	// 被拖动的时候, 当前视图被拖出去的商品(类似报价表等视图要重载这个函数)
	virtual			CMerch*	GetDragMerch()	{ return m_pMerchXml; }

	// 判断这个地方能不能脱掉 :)
	virtual			bool32 BeValidDragArea(const CPoint& pt) { return true; }

	// 显示预警条件
	void            ShowSetupAlarms();

	// 开启关闭定时器
	void			SetKillerTimer(bool32 bSet);

public:	
	SmartAttendMerchArray     m_aSmartAttendMerchs;				// 关注的业务列表

	CArray<T_SysFontSave,T_SysFontSave>		 m_aIoViewFontSave;				// 需要保存XML 的字体信息(用户自己修改的字体)
	CArray<T_SysColorSave,T_SysColorSave>	 m_aIoViewColorSave;			// 需要保存XML 的颜色信息(用户自己修改的颜色)

	CString					m_StrTabShowName;	// 每个视图标签的名字
	
	bool32					m_bShowNow;	// 当前视图是否显示

protected:
	CAbsCenterManager		*m_pAbsCenterManager;
	CIoViewManager	*m_pIoViewManager;
	int32			m_iGuid;
	bool32			m_bDelByBiSplitTrack;
	bool32			m_bLockRedraw;
	static E_WeightType m_eWeightType;			// 权重类型

	CStringArray	m_aUserBlockNames;			// 所有自选股板块名

	bool32			m_bActive;

	bool32			m_bHasIgnoreLastViewDataReq;

	bool32          m_bFirstRequestData;

private:
	CDelayKeyDown*	m_pDelayPageUpDown;

protected:
	static	CIoViewBase*	m_pIoViewMerchChangeSrc;

	static	bool32			sm_bInBackMerchChanging;		// 正在后向切换商品
	static  MerchArray		sm_aBackMerchs;					// 后向商品列表
	static	MerchArray		sm_aPrevMerchs;					// 前向商品列表

public:
	////////////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewBase)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewBase)
	afx_msg void OnMainFrameMenu ( UINT nID );
	afx_msg void OnMenu(UINT nID);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBlockMenu(UINT nID);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_BASE_H_

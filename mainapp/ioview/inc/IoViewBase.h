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
	typedef			CArray<CGmtTime, const CGmtTime &>  GmtTimeArray;		// ���г�iocһ��
	typedef			CArray<CMerch *, CMerch *>			MerchArray;

	static enum E_WeightType
	{
		EWTPre = 0,
		EWTAft,
		EWTNone,

		EWTCount,
	};

	enum E_IoViewType	// ҵ����ͼ����
	{
		EIVT_SingleMerch = 0,	// ����Ʒ��ͼ k �� ����
		EIVT_MultiMerch,		// ����Ʒ��ͼ ���� ���� �ǿյ�
		EIVT_MultiBlock,		// ������ͼ ����б�
	};


public:
	friend CIoViewManager;

	// Construction
public:
	CIoViewBase();
	virtual ~CIoViewBase();
	virtual int32 GetChartType(){return -1;}
public:
	int32			GetIoViewGroupId();	// ҵ����ͼ������IoViewManagerΪ�������䵥λ�� <= 0 ��ʾ������ͼ�� ����Ҫͬ������ͼ��������
	int32			GetGuid();
	void			SetCenterManager(CAbsCenterManager *pAbsCenterManager);
	CAbsCenterManager*		GetCenterManager()	{ return m_pAbsCenterManager; }
	void			SetIoViewManager(CIoViewManager *pIoViewManager) { m_pIoViewManager = pIoViewManager; }
	void			SetBiSplitTrackDelFlag(bool32 bDel);
	CIoViewManager* GetIoViewManager()								 { return m_pIoViewManager; }
	CGGTongView*	GetParentGGtongView();

	bool32			IsNeedPushData(CMerch *pMerch, E_DataServiceType eDataServiceType);		// �Ƿ���Ҫע��÷������͵����ͣ�Ĭ��ȫ������

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
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);	// ��ȡ����ͼ����Ȥ���г����г�����

	//from myself
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) = 0;     // ֪ͨ��ͼ�ı��ע����Ʒ

	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType) {}										  // ��ͷ�����仯										 
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName ){} // ָ�깫ʽ�����仯
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch){ return false;}	     // ������ͼ���
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerch) {}
	virtual void	OnVDataClearCompareMerchs(){}
	virtual void    OnIoViewGroupIdChange(int32 iNewGroupId){}	

	virtual bool32	IsNowCanRefreshUI();	// ��ͼ���ɼ���ʱ��ˢ�½���

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
	virtual void	OnIoViewTabShow(){};		// ���tab������ͼ��ʾ
	virtual void	OnIoViewTabHide(){};		// ���tab������ͼ����

	// �л���Ʒʱ��, ȡ��һ����Ʒ
	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);/*{return NULL;}*/

	virtual CMerch* GetMerchXml();		// ��ȡ��ͼ����Ȥ��һ��xml
	virtual void	GetMerchXmls(OUT MerchArray &aMerchs);


	virtual E_IoViewType GetIoViewType() { return EIVT_SingleMerch; }	// ������ͼ���ͣ�Ĭ�ϵ���Ʒ
	virtual	void	OnEscBackFrameMerch(CMerch *pMerch){}			// ��ʹ��ESC������ͼҳ��ʱ�������¼����ҳ����������ͼ�ĸýӿ�

	virtual void    OnUpdateIoViewMenu(CCmdUI *pCmdUI);			// mainframeѯ��cmd��״̬

	virtual	bool32	IsIoViewContainer() { return false; }		// �Ƿ���IoView������ͼ�������Ƿ񻹴�������ͼ
	virtual	CIoViewBase* GetActiveIoView();					// ��ȡ����ͼ�µ�ǰ�ļ�������ͼ��Ĭ�Ͼ����Լ�

	virtual	bool32	OnSpecialEsc(){ return true; };							// ����ͼ�ڴ�������ķ���ʱ������false��ʾ��Ҫ�����ˣ�true��������

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return true;}
	virtual void	LockRedraw(){}
	virtual void	UnLockRedraw(){}
	//�жϴ��������ػ�����ʾ
	virtual void    OnShowViews(BOOL bShow){};

	virtual	void	KLineCycleChange(UINT nID){};
public: // virtual #viewdata notify ioview
	// 
	virtual void	RequestViewData(){}

	// ���巢���仯
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();

	// ������Ŀ����ͳһ�ӿ� - ����������ͼȥ����
	virtual bool32	IsPluginDataView() { return false; }	// ����һ����־����һ��bool��
	virtual	void	OnVDataPluginResp(const  CMmiCommBase *pResp) {};
	virtual bool32  IsKindOfReportSelect() { return false; }

public:
	bool32			GetParentGGTongViewDragFlag();
	void			InitialIoViewFace(CIoViewBase * pIoView);

	void			SetColorsFromXml(TiXmlElement * pElement);				// ����XML �õ���ɫ��Ϣ
	void			SetFontsFromXml(TiXmlElement * pElement);				// ����XML �õ�������Ϣ

	CString			SaveColorsToXml();
	CString			SaveFontsToXml();

	COLORREF		GetIoViewColor(E_SysColor eSysColor);
	LOGFONT*		GetIoViewFont(E_SysFont eSysFont);	
	CFont*			GetIoViewFontObject(E_SysFont eSysFont);

	//	�����б����ݻ�ȡָ����ʽ������
	//	�б�ͷ
	LOGFONT*		GetColumnExLF(bool bBold = false);
	//	����������
	LOGFONT*		GetChineseRowExLF(bool bBold= false);
	//	�������
	LOGFONT*		GetIndexRowExLF(bool bBold= false);
	//	����������
	LOGFONT*		GetDigitRowExLF(bool bBold= false);
	//	��Ʒ����������
	LOGFONT*		GetCodeRowExLF(bool bBold= false);
	

	CFont*			SetIoViewFontObject(E_SysFont eSysFont, LOGFONT& lf);
	void			SetIoViewFontArray(CFontNode (&aSysFont)[ESFCount]); 
	void			SetIoViewColorArray(COLORREF (&aSysColor)[ESCCount]);

	void			SetIoViewFontArraySave(CArray<T_SysFontSave,T_SysFontSave>& aSysFontSave);
	void			SetIoViewColorArraySave(CArray<T_SysColorSave,T_SysColorSave>& aSysColorSave);

	void			ChangeFontByUpDownKey(bool32 bBigger);
	BOOL			DealEscKey(bool32 bDealCross = true);										// ����ESC ��ת
	BOOL			DealEscKeyReturnToReport();

	virtual void	DoShowStdPopupMenu();

	E_ReportType	GetMerchKind(IN CMerch *pMerch);

	CMerch*         GetMerchAccordExpMerch(CMerch* pMerch);	// ��ȡ��Ʒ��Ӧ��ָ��

	// ��merch keyת��Ϊ��Ʒ������ת���ĸ���Ҫ����NULL���߲���
	int32			ConvertMerchKeysToMerchs(const CArray<CMerchKey, CMerchKey &> &aKeys, OUT CArray<CMerch *, CMerch *> &aMerchs, bool32 bIgnoreMissedKey); // ����ʵ��ת���ķ�NULL��Ʒ������
	CMerch*         ConvertMerchKeyToMerch(int32 iMarketId, const CString &StrMerchCode);

	// ������
	bool32			InitDrawMarkItem(const T_MarkData &MarkData, OUT GV_DRAWMARK_ITEM &DrawMarkItem);	// ��ʼ�����Ʊ�ǲ���
	bool32			InitDrawMarkItem(CMerch *pMerch, OUT GV_DRAWMARK_ITEM &DrawMarkItem);
	// ʹ���Ƿ���ѡ�ɣ���ǻ�����Ʒ����, dc��Ҫ���ú�����
	// ���վ��е������λ�������  �ַ�����(600000)T   ����, �����Ҫ����code����ȥ�м�code����
	void			DrawMerchName(CDC &dc, CMerch *pMerch, const CRect &rcText, bool32 bDrawCode = true, OUT CRect *pRectMark=NULL);	

	bool32			IsLockedSplit();



public:						

	static E_WeightType GetWeightType();
	static void			SetWeightType(E_WeightType eWeightType);
	static bool32		ReadWeightDataFromLocalFile(IN CMerch* pMerch, OUT CArray<CWeightData,CWeightData&>& aWeightData, OUT UINT& uiCrc32);

	// ǰ���������Ʒ�л����
	static CMerch		*PeekLastBackMerch(bool32 bRemove=false);	// ��ȡ���һ��������Ʒ
	static void			AddLastCurrentMerch(CMerch *pMerch);		// ������ĵ�ǰ��Ʒ
	static CMerch		*PeekLastPrevMerch(bool32 bRemove=false);	// ��ȡ���һ��ǰ����Ʒ

public:
	static bool32	BePassedCloseTime(CMerch* pMerch);

public:
	bool32			IsActive() { return m_bActive; }
	void			SetActiveFlag(bool32 bActive) { m_bActive = bActive; }
	bool32			IsActiveInFrame();		// �Ƿ���frame��active

	bool32			ParentIsIoView();		// ���״���Ϊioview���ڣ��������������ӦҪ��������

	void			SetHasIgnoreLastViewDataReqFlag(bool32 bIgnored);

	// ���н���ʱ�䶼�ǻ���ʱ��0
	bool32			GetNowServerTime(OUT CGmtTime &TimeNow);
	void			GetTradeTimes(IN const CGmtTime &TimeSpecify, IN const MerchArray &aMerchs, OUT GmtTimeArray &aTradeTimes );	// ָ�����ڻ�ȡ�ϲ���Ľ���ʱ���, ��������Ϣ�յ��޿�����ʱ��
	bool32			IsTimeInTradeTimes(IN const GmtTimeArray &aTradeTimes, IN const CGmtTime &TimeCheck); // ָ��ʱ���Ƿ��ڽ���ʱ�����
	bool32			IsNowInTradeTimes(IN const GmtTimeArray &aTradeTimes);		// ���շ�����ʱ�䣬�����Ƿ��ڽ���ʱ�����
	bool32			IsNowInTradeTimes(IN const MerchArray &aMerchs, bool32 bCheckTradeDay = false); // checkTradeDay�Ƿ�Ӧ�����ָ�������Ƿ�Ϊ������
	bool32			IsNowInTradeTimes(CMerch *pMerch, bool32 bCheckTradeDay = false);		// ���ָ��������Ʒ�Ƿ��ڽ���ʱ����ڣ���Null Merch�Ǵ������
	bool32			IsTimeInTradeDay(IN const MerchArray &aMerchs, const CGmtTime &TimeSpecify); // ָ��ʱ���Ƿ�����Ʒ���ϵĽ�������

	static CMerch  *GetMerchAccordExpMerchStatic(CMerch* pMerch);
	static void		GetSpecialAccordExpMerch(OUT MerchArray &aExpMerchs);	// ��ȡ��Ҫ�ļ���ָ��
protected:
	// !������ͼ���ô˷�������viewdata���ݣ�����ֱ��ʹ��viewdata����
	// bForceReq - ǿ�Ʒ������󣬲��۴����Ƿ�������״̬
	// bLogIfCancel - ������������ڴ������ض����ԣ��Ƿ������һ������δ������־
	// ����ֵ - �������Ƿ�ͨ��viewdata������
	bool32			DoRequestViewData(CMmiCommBase &req, bool32 bForceReq = false, bool32 bLogIfCancel = true);
	bool32			RequestLastIgnoredReqData();		// �������һ�ι��˵������󣬽�����OnIoViewActive�е��ã��滻ÿ��ǿ�Ƶ�forceupdate

	void			RegisterHotKeyTarget(bool32 bReg);	// ע����̾���

public:
	// ���϶���ʱ��, ��ǰ��ͼ���ϳ�ȥ����Ʒ(���Ʊ��۱����ͼҪ�����������)
	virtual			CMerch*	GetDragMerch()	{ return m_pMerchXml; }

	// �ж�����ط��ܲ����ѵ� :)
	virtual			bool32 BeValidDragArea(const CPoint& pt) { return true; }

	// ��ʾԤ������
	void            ShowSetupAlarms();

	// �����رն�ʱ��
	void			SetKillerTimer(bool32 bSet);

public:	
	SmartAttendMerchArray     m_aSmartAttendMerchs;				// ��ע��ҵ���б�

	CArray<T_SysFontSave,T_SysFontSave>		 m_aIoViewFontSave;				// ��Ҫ����XML ��������Ϣ(�û��Լ��޸ĵ�����)
	CArray<T_SysColorSave,T_SysColorSave>	 m_aIoViewColorSave;			// ��Ҫ����XML ����ɫ��Ϣ(�û��Լ��޸ĵ���ɫ)

	CString					m_StrTabShowName;	// ÿ����ͼ��ǩ������
	
	bool32					m_bShowNow;	// ��ǰ��ͼ�Ƿ���ʾ

protected:
	CAbsCenterManager		*m_pAbsCenterManager;
	CIoViewManager	*m_pIoViewManager;
	int32			m_iGuid;
	bool32			m_bDelByBiSplitTrack;
	bool32			m_bLockRedraw;
	static E_WeightType m_eWeightType;			// Ȩ������

	CStringArray	m_aUserBlockNames;			// ������ѡ�ɰ����

	bool32			m_bActive;

	bool32			m_bHasIgnoreLastViewDataReq;

	bool32          m_bFirstRequestData;

private:
	CDelayKeyDown*	m_pDelayPageUpDown;

protected:
	static	CIoViewBase*	m_pIoViewMerchChangeSrc;

	static	bool32			sm_bInBackMerchChanging;		// ���ں����л���Ʒ
	static  MerchArray		sm_aBackMerchs;					// ������Ʒ�б�
	static	MerchArray		sm_aPrevMerchs;					// ǰ����Ʒ�б�

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

#ifndef _IOVIEWPHASESORT_H_
#define _IOVIEWPHASESORT_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "GridCtrlNormal.h"
#include "BlockConfig.h"
#include "PhaseSortCalc.h"
#include "IoViewReport.h"
#include "DlgTimeWait.h"
#include <map>

using std::map;

/////////////////////////////////////////////////////////////////////////////
// CIoViewPhaseSort

enum E_PhaseSortType			// 列表头的类型
{
	EPST_RiseFallPecent = 0,		// 涨跌幅
	EPST_TradeRate,				// 换手率
	EPST_VolChangeRate,			// 量变幅度
	EPST_ShakeRate,				// 震荡幅度
		
	EPST_Count
};

CString GetPhaseSortTypeName(E_PhaseSortType eType);

struct T_PhaseOpenBlockParam
{
	// 打开板块的参数
	int32		m_iBlockId;			// 排行范围
	CGmtTime	m_TimeStart;		// 开始时间
	CGmtTime    m_TimeEnd;			// 结束时间
	bool32      m_bDoPreWeight;		// 是否前复权
	E_PhaseSortType		m_ePhaseSortType;   // 排序字段

	T_PhaseOpenBlockParam();
	bool32		IsCalcParamSame(const T_PhaseOpenBlockParam &pa1) const;
};

bool32 operator==(const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2);
bool32 operator!=(const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2);

// 默认不打开任何板块

class CIoViewPhaseSort : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	typedef CBlockConfig::IdArray	IdArray;

	CIoViewPhaseSort();
	virtual ~CIoViewPhaseSort();

	DECLARE_DYNCREATE(CIoViewPhaseSort)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock 通知
	//virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual DWORD	GetNeedPushDataType() { return 0; } // 不需要任何推送数据
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(CMerch *pMerch);

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);


	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();
public:
	// CBlockConfigListener
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);


public:
	bool32			OpenBlock(const T_PhaseOpenBlockParam &BlockParam, bool32 bReOpen, bool32 bAskUserSetting = false);		// 打开指定板块, 是否打开已经打开了的
	void			ChangeTabToCurrentBlock();
	void			GetPhaseOpenBlockParam(OUT T_PhaseOpenBlockParam &BlockParam)const { BlockParam = m_PhaseOpenBlockParam; }
	
public:
	static	bool	CompareRow(int iRow1, int iRow2);		// 共用了
	static  CIoViewPhaseSort *m_spThis;

	// from CView	
public:
	const T_SimpleTabInfo* GetSimpleTabInfo(int32 iTabIndex);

protected:

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	

	enum E_PhaseHeaderType
	{
		EPHT_No = 0,		// 序号
		EPHT_Code,			// 代码
		EPHT_Name,			// 名称
		EPHT_RiseFall,		// 涨跌幅
		EPHT_TradeRate,		// 换手率
		EPHT_VolChangeRate, // 量变
		EPHT_ShakeRate,		// 震荡
		EPHT_PreClose,		// 昨收
		EPHT_High,			// 高
		EPHT_Low,			// 低
		EPHT_Close,			// 收盘
		EPHT_Vol,			// 量
		EPHT_Amount,		// 金额
		EPHT_MarketRate,	// 市场比

		EPHT_Count,			// 无效值
	};

	enum E_Stage
	{
		ES_WaitPrice = 0,		// 等待行情数据阶段
		ES_WaitCalc,			// 等待数据计算阶段

		ES_Count				// 处理完毕或者是闲着
	};

	typedef map<E_PhaseHeaderType, CString> PhaseHeaderStringMap;
	
	struct T_TitleString
	{
		CString  m_StrTitle;		// 文字
		COLORREF m_clr;				// 颜色
	};
	typedef CArray<T_TitleString, const T_TitleString &> TitleStringArray;

	struct T_ColumnHeader
	{
		CString m_StrHeader;
		E_PhaseHeaderType  m_eHeaderType;			// 标志这个列的东西
		bool32	m_bNeedSort;		// 需要排序，多个排序中仅第一个在第一次时有效

		float	m_fPreferWidthByChar;		// 希望的宽度，几个字符的宽度
		float   m_fFixedRowHeightRatio;		// 固定行的高度系数
		float	m_fNonFixedRowHeightRatio;  // 非固定行的高度与固定行高的比例 默认1.0, 这里的列有的列有两行

		T_ColumnHeader();
		T_ColumnHeader(E_PhaseHeaderType eHeaderType);
		T_ColumnHeader(E_PhaseHeaderType eHeaderType, LPCTSTR pszName, bool32 bSort = false, float fWidthByChar = 0.0f, float fFixedRowHeight = 1.0f, float fNonFixedRowHeight = 1.0f);

		bool32 Initialize(E_PhaseHeaderType eHeaderType);		// 初始化
		bool32 Initialize(const CString &StrHeader);			// 使用名称初始化

		static void InitStatic();
		static CArray<T_ColumnHeader, const T_ColumnHeader &> s_aInitHeaders;
	};
	friend T_ColumnHeader;
	typedef CArray<T_ColumnHeader, const T_ColumnHeader &> ColumnHeaderArray;

	
	
private:
	void			TabChanged();
	void			SetTab(int32 iCurTab);	
	void			SetTabByBlockId(int32 iBlockId);

	void			InitializeTabs();		// 初始化tab相关
	
	CBlockLikeMarket *GetCurrentBlock();					// 获取当前板块

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			InitializeTitleString();							// 初始化当前显示的标题字串
	void			InitializeColumnHeader(E_PhaseSortType eType);	// 初始化当前的排序类型
	void			SetSortColumnBySortType(E_PhaseSortType eType);					// 变更排序类型，不需要重计算

	void			UpdateTableHeader();
	void			UpdateTableAllContent();		// 根据现有数据，刷新显示

	void			RecalcLayout();

	// 固定数据 所有排序交给表格
	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );

	void			DoTrackMenu(CPoint pos);
	
	bool32			CalcPhaseRequestData(INOUT T_PhaseSortCalcRequestData &reqData);
	
	int32			RequestAllPriceData(bool32 bForce = false);					// 请求所有商品的行情数据
	int32			TryRequestMoreNeedPriceMerchs();						// 请求队列中的商品
	void			RequestAttendMerchs(bool32 bForce);					// 请求当前关注的商品的数据
	void			RequestAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch &> &aSmarts, bool32 bForce);
	void			RequestExpKline();
	void			RequestWeightData();

private:	
	int32							m_iSortColumn;

	// 表格相关
	CGridCtrlNormalSys				m_GridCtrl;	
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CRect							m_RectTitle;				// 热门板块分析 -- XXX 今日: 2010-08-02,三
	
	tm								m_tmDisplay;
	// Tab 相关
	CGuiTabWnd						m_GuiTabWnd;
	
	bool32							m_bBlockReportInitialized;	// 是否初始化完毕

	T_PhaseOpenBlockParam			m_PhaseOpenBlockParam;		// 打开该板块的参数
	T_PhaseOpenBlockParam			m_PhaseBlockParamXml;		// 从xml中获取的参数

	int32							m_iPreTab;					// 切换前的一次Tab - 这两个数据仅为保存切换状态用
	int32							m_iCurTab;					// 当前Tab

	T_PhaseSortCalcParam			m_ParamForDisplay;			// 计算完的结果，用于显示
	T_PhaseSortCalcParam			m_ParamForCalc;				// 用于计算的

	TitleStringArray				m_aTitleString;
	ColumnHeaderArray				m_aColumnHeaders;			// 当前列集合

	CDlgTimeWait					m_DlgWait;
	E_Stage							m_eStage;

	CGmtTime						m_TimeLastReqPrice;			// 最后一次请求所有商品行情数据的时间
	MerchArray						m_aMerchsNeedPrice;			// 需要请求行情的商品，貌似一次不能发太多

	typedef map<CMerch *, float>    MerchFloatDataMap;			
	MerchFloatDataMap				m_mapCircAssert;			// 商品的流通股本
	MerchFloatDataMap				m_mapExpAmount;				// 指数的区段总金额
	typedef map<CMerch *, CGmtTime> MerchRequestTimeMap;
	static MerchRequestTimeMap		s_mapMerchReqTime;			// 商品的请求刷新列表

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewPhaseSort)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewPhaseSort)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoBlockReportInitialize();
	afx_msg void OnMsgDoCalc();							// 开始计算
	afx_msg LRESULT OnMsgCalcStart(WPARAM w, LPARAM l);	// 线程通知已经开始计算
	afx_msg LRESULT OnMsgCalcPos(WPARAM w, LPARAM l);	// 线程通知计算的进度
	afx_msg LRESULT OnMsgCalcEnd(WPARAM w, LPARAM l);	// 线程通知计算完毕
	afx_msg LRESULT OnMsgCalcReqData(WPARAM w, LPARAM l); // 线程通知，需要主图的数据
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridCacheHint(NMHDR *pNotifyStruct, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IOVIEWPHASESORT_H_
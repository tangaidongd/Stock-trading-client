#ifndef _IO_VIEW_BLOCK_REPORT_H_
#define _IO_VIEW_BLOCK_REPORT_H_

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
#include "IoViewTimeSaleStatistic.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewBlockReport
class CIoViewBlockReport : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	typedef CBlockConfig::IdArray	IdArray;

	CIoViewBlockReport();
	virtual ~CIoViewBlockReport();

	DECLARE_DYNCREATE(CIoViewBlockReport)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock 通知
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual	CMerch *GetMerchXml();		// 获取选择的领涨股票
	virtual	E_IoViewType GetIoViewType() { return EIVT_MultiBlock; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(int32 iBlockId);

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);

	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsPluginDataView() { return true; }
	virtual	void	OnVDataPluginResp(const CMmiCommBase *pResp);

	void			OnLogicBlockDataUpdate(const IdArray &aUpdatedBlockIds);
	void			OnBlockSortDataUpdate(const CMmiRespBlockSort *pRespBlockSort);
	void			OnBlockDataResp(const CMmiRespLogicBlock *pResp);
public:
	// CBlockConfigListener
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);

	void			SortColumn(int32 iSortCol=-1, int32 iSortType=0);	// iSortType=0随意 >0 Desc < 0 Ascend
	
public:
	static	bool	CompareRow(int iRow1, int iRow2);		// 共用了
	static  CIoViewBlockReport *m_spThis;

	// from CView	
public:
	
protected:

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	

	struct T_BlockReportTab
	{
		enum{
			BCAll  = -2
		};
		int32 m_iBlockCollectionId;	// 对应的板块类别
		int32 m_iStartRow;		// tab开始的行 - 仅在切换时可用
		int32 m_iRowCount;
		int32 m_iColLeftVisible;

		IdArray	m_aSubBlockIds;

		CString m_StrName;

		T_BlockReportTab()
		{
			m_iBlockCollectionId = BCAll;
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iColLeftVisible = 0;
		}
		T_BlockReportTab(const T_BlockReportTab &tab);
		const T_BlockReportTab &operator=(const T_BlockReportTab &tab);

		bool32 IsValid()
		{
			return m_iStartRow >= 0 && m_iRowCount >= 0 && !m_StrName.IsEmpty();
		}
	};
private:									// esc在每个tab之间切换？
	void			TabChanged();
	void			SetTab(int32 iCurTab, int32 iSortCol=-1, int32 iSortType=0);	
	void			SetTabByClassId(int32 iClassId, int32 iSortCol=-1, int32 iSortType=0);

	void			InitializeTabs();		// 初始化tab相关
	
	bool32			TabIsValid(int32 &iTab);					// 获取当前的tab
	CBlockCollection *GetCurrentCollection();					// 获取当前集合
	
	CMerch*			GetBlockMerchXml();							//获取板块信息
		

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableContent( int32 iClassId, int32 iBlockId, bool32 bBlink );
	void			UpdateTableAllContent();		// 根据现有数据，刷新显示

	void			ResetBlockShowData(bool32 bResetTopLeft=true);

	bool32			GetCurrentRowInfo(OUT T_BlockReportTab &tab);		// 有可能出现rowcount比视图太小的情况，需要处理
	void			SetCurrentRowInfo(int32 iRowBegin, int32 iColLeft);
	bool32			GetTabInfoByIndex(int32 iIndex, OUT T_BlockReportTab &tabInfo);
	bool32			UpdateTabInfo(int32 iTabIndex, int32 iRowFirst, int32 iColLeft);

	void			RecalcLayout();

	void			OnMouseWheel(short zDelta);

	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );

	void			DoTrackMenu(CPoint pos);

	void			DoFixedRowClickCB(CCellID& cell, int32 iSortType = 0);	// iSortType=0随意 >0 Desc; < 0 Ascend
	void			DoLocalSort();	// 本地排序处理
	
	void			RequestViewDataCurrentVisibleRow();		// 没有什么数据可以申请的
	void			RequestSortData();

	void			RequestData(CMmiCommBase &req);			// 调用这个申请数据

	void			WatchDayChange(bool32 bDrawIfChange = false);			// 初始化离今天最近的交易日
	bool32			IsPointInGrid();

private:	
	int32							m_iSortColumn;
	bool32							m_bRequestViewSort;

	int32							m_iSortColumnFromXml;	// 从xml中获取的sortcol	
	int32							m_iSortTypeFromXml;		// 从xml中获取的sortType, 第一次调用sortcol会重置0
	bool32                          m_bShowTitle;			// 从xml中获取的是否显示标题
	bool32							m_bHideTab;			    // 从xml中获取的是否隐藏一些板块标签 
	bool32                          m_bUpdateKline;         // 从xml中获取的是否更新K线
	bool32                          m_bUpdateTrend;			// 从xml中获取的是否更新分时
	bool32                          m_bUpdateReport;		// 从xml中获取的是否更新报价表

	CMmiReqBlockSort				m_MmiRequestBlockSort;		// 总会需要一个排序申请的 - 现在没有任何申请

	// 表格相关
	CGridCtrlNormalSys				m_GridCtrl;	
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	bool32							m_bIsShowGridVertScorll;	// 控制是否显示垂直滚动条

	CRect							m_RectTitle;				// 热门板块分析 -- XXX 今日: 2010-08-02,三
	
	// Tab 相关
	CGuiTabWnd						m_GuiTabWnd;
	
	bool32							m_bBlockReportInitialized;	// 是否初始化完毕

	int32							m_iBlockClassId;

	int32							m_iMaxGridVisibleRow;		// 表格最大可见的数据

	int32							m_iPreTab;					// 切换前的一次Tab - 这两个数据仅为保存切换状态用
	int32							m_iCurTab;					// 当前Tab

	tm								m_tmDisplay;

	CMmiReqPushPlugInBlockData		m_mmiReqBlockData;			// 板块报价

	// 自选股的数据获取需要确定
	// 非排序字段 序号, 本地排序字段 名称, 其它字段 服务器排序  -- 自选股其它字段 本地排序
	// 当点击排序时，所有id被会被重置为无效id，等到排序列表回来，则将对应位置的blockid设置为实际id
	IdArray							m_aShowBlockIds;			// 所有可见的block
	
	// 每个页面的所有信息
	CArray<T_BlockReportTab, const T_BlockReportTab&>	m_aTabInfomations;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewBlockReport)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewBlockReport)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// 侦听 - 是不是一定要记录了，貌似不记录也没什么影响
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// 侦听垂直方向滚动，计时发送数据请求
	afx_msg void OnDoBlockReportInitialize();

	afx_msg LRESULT OnScrollPosChange(WPARAM w, LPARAM l);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridCacheHint(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_BLOCK_REPORT_H_

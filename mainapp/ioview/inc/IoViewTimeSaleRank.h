#ifndef _IO_VIEW_TIMESALE_RANK_H_
#define _IO_VIEW_TIMESALE_RANK_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "PlugInStruct.h"
#include "BlockConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSaleRank

// 所有来自IoViewBase的小心要获取IoViewManager的调用

class CIoViewTimeSaleRank : public CIoViewBase
{
// Construction 
public:
	CIoViewTimeSaleRank();
	virtual ~CIoViewTimeSaleRank();

	DECLARE_DYNCREATE(CIoViewTimeSaleRank)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // 父窗口显示 大单排行 - 市场
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// 无xml功能
	virtual CString	ToXml(){return CString("");};
	virtual	CString GetDefaultXML(){return CString("");};
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// 不需要标准右键菜单

	virtual void	LockRedraw(){};				// 不需要lock
	virtual void	UnLockRedraw(){};

	virtual CMerch  *GetMerchXml();
public:	
	void			OnDblClick(CMerch *pMerch);				// 打开某个商品，跟选股操作打开类似

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch){};

	// 以前的数据接口应该不会用到 - 看具体什么数据接口吧 - 不假设任何数据结构
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			// 看到底需要处理不
	virtual void	OnIoViewFontChanged();

	virtual bool32	IsPluginDataView() { return true; };
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);

	void			OnTickExResp(const CMmiRespPeriodTickEx *pResp);		// 分笔统计数据回包
	void			OnMerchSortResp(const CMmiRespPeriodMerchSort *pResp);	// 排行请求回包

public:
	void			OpenBlock(int32 iBlockId);		// 针对板块的, 且只有沪深A股
	void			RefreshView(int32 iPeriod);

public:
	uint8   m_iPeriod;

	// from CView	
public:
	
	CXScrollBar		*SetXSBHorz(CXScrollBar *pHorz = NULL);		// 设置水平滚动条，如果为Null则使用默认滚动条, 返回上次的
	CXScrollBar		*GetXSBHorz() const { return m_pCurrentXSBHorz; };
	void			ResetXSBHorz(bool32 bResetPos=false);
	
	bool32			ShowOrHideXHorzBar();

protected:
	CSize		GetVirtualSize();	// 获取虚拟的大小
	int32		GetVirtualLeftPos();	// 获取虚拟的左边起点
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////
public:
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
	typedef map<CMerch *, T_TickEx>		MerchDataMap;	// 数据缓存
	struct T_Tab : public CGridCtrlCB	// 每个小表格的信息
	{	
		typedef	CArray<E_ReportSortPlugIn, E_ReportSortPlugIn>	SortHeaderArray;
// 		1）超大单换手率10强：超大单换手率排名前10
// 			2）大单换手率10强：大单换手率排名前10
// 			3）超大单买入金额10强：超大单买入额前10名
// 			4）大单买入金额10强：大单买入额前10名
// 			5）成交笔数10强：略 
// 			6）每笔金额10强：略 
// 			7）每笔股数10强： 略
// 			8）买入比例10强：略

		enum E_HeaderType
		{
			HugeSaleChangeRate = 0,
			BigSaleChangeRate,
			HugeBuyAmount,
			BigBuyAmount,
			SaleCount,
			AmountPerSale,
			UnitPerSale,
			BuyRate,

			HT_COUNT
		};

		E_HeaderType		m_eType;
		CGridCtrlNormalSys		m_wndGrid;
		CXScrollBar						m_XSBVert;
		CXScrollBar						m_XSBHorz;

		CRect				m_RectTab;
		CRect				m_RectTitle;
		CIoViewTimeSaleRank	*m_pwndRank;

		MerchArray						m_aMerchs;			// 所有商品数据序列

		CMmiReqPeriodMerchSort				m_MmiRequestSys;	// 排序请求
		
		T_Tab();

		~T_Tab();

		BOOL		Create(CIoViewTimeSaleRank *pParent, int nId);
		void		Draw(CDC &dc);
		void		MoveRect(const CRect &rc, bool32 bDraw=TRUE);	// 分配矩形

		CString     GetTitleString();
		CString     GetNameString();
		void		UpdateTableHeader();
		void		UpdateTableContent(const MerchArray &aMerchs);
		bool32      GetCellValue(OUT GV_ITEM &item);

		E_ReportSortPlugIn	GetSortHeader();

		void		RequestVisibleData(uint8 iPeriod = 1);		// 请求显示数据
		void		RequestSortData(bool32 bForceReq = false, uint8 iPeriod = 1);			// 请求排序数据

		void		OnMerchShowDataUpdate(CMerch *pMerch, bool32 bDrawNow = true);	// 商品显示数据刷新
		void		OnMerchSortUpdate(const MerchArray &aMerchsSort, uint8 iPeriod = 1);			// 商品排行变化 - 需要窗口进行判断到底是哪个的数据
		
		virtual void	OnFixedRowClickCB(CCellID& cell){};		// 这个视图出现这个貌似没有什么意义
		virtual void	OnFixedColumnClickCB(CCellID& cell){};
		virtual void	OnHScrollEnd(){};
		virtual void	OnVScrollEnd(){};
		virtual void	OnCtrlMove( int32 x, int32 y ){};
		virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ){  return false; };

		static	int32		GetNamesSize() { return  m_sStrHeaderTypeNames.GetSize(); }
		static	int32		GetTitlesSize(){ return  m_sStrHeaderTypeTitles.GetSize(); } 
		static CStringArray m_sStrHeaderTypeNames;
		static CStringArray m_sStrHeaderTypeTitles;
		static SortHeaderArray	m_sSortHeaders;
	};
	
	friend struct T_Tab;
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableContent( CMerch *pMerch, bool32 bBlink );
	void			UpdateTableAllContent();

	void			RecalcLayout();

	CBlockLikeMarket	*GetCurrentBlock();
	CBlockLikeMarket	*GetBlock(int32 iBlockId);
	
	void			RequestViewDataCurrentVisibleRow();	
	void			RequestSortData(bool32 bForceReq = false, uint8 iPeriod = 1);

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// 调用这个申请数据

	void			DoTrackMenu(CPoint pos);

	void			CreateBtnList();								// 可选周期按钮

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// 重绘周期按钮
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);
private:	
	
	bool32							m_bInitialized;			// 是否初始化完毕

	int32							m_iBlockId;				// 当前板块id

	// 每个页面的所有信息
	CArray<T_Tab, const T_Tab &>	m_aTabInfos;			// 各个小表格信息

	CGridCtrl						*m_pLastFocusGrid;		// 怎么判断呢？？

	MerchDataMap					m_mapMerchData;			// 所有商品数据缓存

	CXScrollBar						*m_pCurrentXSBHorz;		// 水平滚动条可以交给外部控制


	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// 周期按钮列表	

	Image		*m_pImgBtn;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSaleRank)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSaleRank)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
 
	afx_msg void OnGridGetDispInfo(UINT nId, NMHDR *pNotifyStruct, LRESULT *pResult);
    //linhc 20100911添加选择改变时联动
    afx_msg void OnGridSelRowChanged(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult );
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
		
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TIMESALE_RANK_H_

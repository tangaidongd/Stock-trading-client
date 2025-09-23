#ifndef _IO_VIEW_TIMESALE_STATISTIC_H_
#define _IO_VIEW_TIMESALE_STATISTIC_H_


#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "BlockConfig.h"
#include "PlugInStruct.h"

#include <map>
using std::map;

//////////////////////////////////////////////////////////////////////////
// 表头名称 代号 排序代号
enum E_TimeSaleStatisticHeader
{
	ETSSHCode = 0,	//	代码
	ETSSHName,	//	名称
	ETSSHPriceNew,	//	最新
	ETSSHVolBuySuper,		// 		超大单买入量（万股）
	ETSSHVolSellSuper,		// 		超大单卖出量（万股）
	ETSSHVolNetSuper,		//		超大单净买入量（万股）

	ETSSHVolBuyBig,			// 		大单买入量（万股）
	ETSSHVolSellBig,		// 		大单卖出量（万股）

	ETSSHVolBuyMid,			// 		中单买入量（万股）
	ETSSHVolSellMid,		// 		中单卖出量（万股）

	ETSSHVolBuySmall,		// 		小单买入量（万股）
	ETSSHVolSellSmall,		// 		小单卖出量（万股）

	ETSSHVolBuyMidSmall,	// 		中小单买入量（万股）
	ETSSHVolSellMidSmall,	// 		中小单卖出量（万股）

	ETSSHAmountBuySuper,	// 		超大单买入额（万元）
	ETSSHAmountSellSuper,	// 		超大单卖出额（万元）
	ETSSHNetAmountSuper,	// 		超大单净额

	ETSSHAmountBuyBig,		// 		大单买入额（万元）
	ETSSHAmountSellBig,		// 		大单卖出额（万元）
	ETSSHNetAmountBig,		// 		大单净额

	ETSSHAmountBuyMid,		// 		中单买入额（万元）
	ETSSHAmountSellMid,		// 		中单卖出额（万元）
	ETSSHNetAmountMid,		// 		中单净额

	ETSSHAmountBuySmall,	// 		小单买入额（万元）
	ETSSHAmountSellSmall,	// 		小单卖出额（万元）
	ETSSHNetAmountSmall,	// 		小单净额

	ETSSHAmountTotalBuy,	// 		买入总额（万元）
	ETSSHAmountTotalSell,	// 		卖出总额（万元）
	ETSSHTradeRateSuper,	// 		超大单换手率%
	ETSSHTradeRateBig,		// 		大单换手率%
	ETSSHTradeRateBuyBig,	//		买入大单换手率% (待确认)
	ETSSHTradeCount,		// 		成交笔数
	ETSSHAmountPerTrans,	// 		每笔金额（万元）
	ETSSHStocksPerTrans,		// 		每笔股数（股）
	ETSSHCapitalFlow,		// 		资金流向

	ETSSHCount
};

struct T_TimeSaleStatisticHeader
{
	T_TimeSaleStatisticHeader(const CString &StrName, E_TimeSaleStatisticHeader eHeaderShow, E_ReportSortPlugIn eHeaderSort)
	{
		m_StrName = StrName;
		m_eHeaderShow = eHeaderShow;
		m_eHeaderSort = eHeaderSort;
	}
	T_TimeSaleStatisticHeader()
	{
		m_eHeaderShow = ETSSHCode;
		m_eHeaderSort = ERSPIEnd;
	}
	CString	m_StrName;
	E_TimeSaleStatisticHeader	m_eHeaderShow;
	E_ReportSortPlugIn			m_eHeaderSort;		// 非排序字段使用的是 ERSPIEnd
};

extern	const T_TimeSaleStatisticHeader KTimeSaleStatisticHeaders[];		// 以后如果用的多，在独立
extern	const int32 KTimeSaleStatisticHeadersCount;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSaleStatistic

class CIoViewTimeSaleStatistic : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB
{
// Construction 
public:
	CIoViewTimeSaleStatistic();
	virtual ~CIoViewTimeSaleStatistic();

	DECLARE_DYNCREATE(CIoViewTimeSaleStatistic)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // 父窗口显示 大单统计 - 市场
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// 无xml功能
	virtual CString	ToXml(){return CString("");};
	virtual	CString GetDefaultXML(){return CString("");};
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// 不需要标准右键菜单

	virtual void	LockRedraw();			// 不需要lock - 由于容器有可能为IoViewManager下的ioview，so需要实现这个
	virtual void	UnLockRedraw();

	virtual	CMerch *GetMerchXml();
	
	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);
	virtual void	OnEscBackFrameMerch(CMerch *pMerch);
public:	
	void			OnDblClick(CMerch *pMerch);				// 打开某个商品，跟选股操作打开类似

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);	// 报价表实时最新价
	

	// 以前的数据接口应该不会用到 - 看具体什么数据接口吧 - 不假设任何数据结构
	virtual void	OnVDataForceUpdate();

	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	// 财务数据
	virtual void    OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch *, CMerch *> &aMerchs);

	virtual void	OnIoViewColorChanged();			// 看到底需要处理不
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsPluginDataView() { return true; } 
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);		// 数据包会发送到这里来

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);

	// 需要请求3种数据 - 已经全部在tickex里面了
	void			OnTickExResp(const CMmiRespPeriodTickEx *pResp);		// 分笔统计回包

	void			OnMerchSortResp(const CMmiRespPeriodMerchSort *pResp);	// 排行请求回包

public:

	void		    OpenBlock(int32 iBlockId);		// 假设市场id是唯一的

	CXScrollBar		*SetXSBHorz(CXScrollBar *pHorz = NULL);		// 设置水平滚动条，如果为Null则使用默认滚动条, 返回上次的
	CXScrollBar		*GetXSBHorz() const;

	bool32			ShowOrHideXHorzBar();

	static	bool	CompareRow(int iRow1, int iRow2);		// 共用了
	static  CIoViewTimeSaleStatistic *m_pThis;
	//bool		operator()(int iRow1, int iRow2);  // grid的限制

	// from CView	
public:
	void RefreshView(int32 iPeriod);
	
private:
	int32 m_iPeriod;

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	struct T_TimeSaleStatisticData
	{
		T_TickEx		m_tickEx;
		
		T_TimeSaleStatisticData();
	};
	typedef map<CMerch *, T_TimeSaleStatisticData>	DataMap;

	typedef map<CMerch *, int>				MerchMap;
	
	typedef	CArray<CMerch *, CMerch *>		MerchArray;

	typedef CArray<T_TimeSaleStatisticHeader, const T_TimeSaleStatisticHeader &>	HeaderArray;

	struct T_Tab
	{
		enum E_Sort
		{
			SortNone,
			SortAscend,
			SortDescend,
		};
		int32 m_iBlockId;		// 对应的板块
		int32 m_iStartRow;		// tab开始的行 顶行 - 不含fixed row
		int32 m_iRowCount;		
		int32 m_iScrollXPos;	//	左侧
		int32 m_iSortColumn;

		MerchArray						m_aMerchs;			// 所有商品数据序列
		MerchArray						m_aMerchsVisible;	// 有效可见商品序列 - 大小与aMerchs相同，但是无效数据使用NULL表示，注意！

		E_Sort  m_eSort;

		CString m_StrName;

		T_Tab()
		{
			m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();	// 沪深A
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iScrollXPos = 0;
			m_iSortColumn = -1;
			m_eSort = SortNone;
		}

		bool32 IsValid()
		{
			return /*m_iBlockId >= -1 &&*/ m_iStartRow >= 0 && m_iRowCount >= 0 && !m_StrName.IsEmpty();
		}

		void ResetShowData()
		{
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iScrollXPos = 0;
			m_eSort = SortNone;
			m_iSortColumn = -1;
			m_aMerchsVisible.Copy(m_aMerchs);
		}

		BOOL IsSort()
		{
			ASSERT( (m_eSort == SortNone&&-1==m_iSortColumn) || (m_eSort!=SortNone&&m_iSortColumn != -1) );
			return m_eSort != SortNone;
		}

		void SetSortVisibleMerchs(const MerchArray &aVisibleMerchs)
		{
			// 从可视的第一行开始，所有商品序列被替换，其它位置被清空
			ASSERT( IsSort() );
			ASSERT( m_iStartRow < m_aMerchs.GetSize() && m_aMerchs.GetSize() == m_aMerchsVisible.GetSize() );
			ASSERT( m_iStartRow + aVisibleMerchs.GetSize() <= m_aMerchsVisible.GetSize() );
			ASSERT( m_iStartRow >= 0 );	//
			int i = 0;
			int32 iStartRow = m_iStartRow;	//
			if ( iStartRow < 0 )
			{
				iStartRow = 0;
			}
			for ( i=0; i < iStartRow && i < m_aMerchsVisible.GetSize() ; i++ )
			{
				m_aMerchsVisible[i] = NULL;
			}

			for ( i=0; i < aVisibleMerchs.GetSize() && (iStartRow + i) < m_aMerchsVisible.GetSize() ; i++ )
			{
				ASSERT( aVisibleMerchs[i] != NULL );
				m_aMerchsVisible[ iStartRow + i ] = aVisibleMerchs[i];
			}

			// i指向最后一个没清空的
			for ( i = i+iStartRow; i < m_aMerchsVisible.GetSize() ; i++  )
			{
				m_aMerchsVisible[i] = NULL;
			}
		}

		void ClearSort()
		{
			m_eSort = SortNone;
			m_iSortColumn = -1;

			m_aMerchsVisible.Copy(m_aMerchs);	// 原始所有数据按照原始顺序可见
		}

		BOOL ChangeSortType(int iCol, BOOL *pbColChanged =NULL)	// 返回是否升序
		{
			if ( NULL != pbColChanged )
			{
				*pbColChanged = FALSE;
			}
			if ( iCol != m_iSortColumn )
			{
				m_eSort = SortNone;		// 不同的列重置原先排序状态
				m_iSortColumn = iCol;
				if ( NULL != pbColChanged )
				{
					*pbColChanged = TRUE;
				}
			}
			if ( m_eSort == SortDescend )
			{
				m_eSort = SortAscend;
			}
			else
			{
				m_eSort = SortDescend;
			}
			return m_eSort == SortAscend;
		}
	};

private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableRowContent( CMerch *pMerch, bool32 bBlink );
	void			UpdateTableAllContent();

	void			AppendShowMerchs(int32 iMerchAppCount);

	void			RecalcLayout();

	void			OnMouseWheel(short zDelta);

	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	
	void			GetColumnHeaders(OUT HeaderArray &aHeaders);
	E_ReportSortPlugIn	GetColumnSortHeader(int iCol);
	E_TimeSaleStatisticHeader	GetColumnShowHeader(int iCol);

	CBlockLikeMarket			*GetCurrentBlock();
	CBlockLikeMarket			*GetBlock(int32 iBlockId);

	CMerch			*CheckMerchInBlock(CMerch *pMerch);
	bool32			IsMerchInMerchArray(CMerch *pMerch, const MerchArray &aMerchs);

	void			UpdateVisibleMerchs();		// 更新当前有哪些商品可见信息
	
	void			RequestViewDataCurrentVisibleRow();		// 没有什么数据可以申请的
	void			RequestSortData(bool32 bForce = false);						// 请求排行数据

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// 调用这个申请数据

	void			DoTrackMenu(CPoint pos);

	void			CreateBtnList();								// 可选周期按钮

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// 重绘周期按钮
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:	
	
	//CMmiReqReport			m_MmiRequestSys;		// 总会需要一个排序申请的 - 现在没有任何申请
	CMmiReqPeriodMerchSort	m_MmiRequestSys;
	CMmiReqBlockReport		m_MmiRequestBlockSys;	// 最新价的板块排行报价

	MerchMap				m_mapLastPushMerchs;	// 最后一次推送请求

	// 表格相关
	CGridCtrlNormalSys		m_GridCtrl;	
	CXScrollBar				m_XSBVert;
	CXScrollBar				m_XSBHorz;

	CXScrollBar				*m_pCurrentXSBHorz;		// 水平滚动条可以交给外部控制
	
	// Tab 相关
	//CGuiTabWnd			m_GuiTabWnd;			// 右键菜单选择
	
	bool32					m_bInitialized;			// 是否初始化完毕

	int32					m_iMaxGridVisibleRow;	// 表格最大可见的数据

	DataMap					m_mapMerchData;			// 商品显示数据
	
	T_Tab					m_tabInfo;				// 当前商品显示信息

	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// 周期按钮列表	

	Image		*m_pImgBtn;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSaleStatistic)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSaleStatistic)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnScrollPosChanged(WPARAM w, LPARAM l);
	afx_msg LRESULT OnDoTrackMenu(WPARAM w, LPARAM l);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TIMESALE_STATISTIC_H_

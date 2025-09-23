#ifndef _IO_VIEW_REPORT_RANK_H_
#define _IO_VIEW_REPORT_RANK_H_

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
// CIoViewReportRank
class CIoViewReportRank : public CIoViewBase, public CBlockConfigListener
{
// Construction 
public:
	CIoViewReportRank();
	virtual ~CIoViewReportRank();

	DECLARE_DYNCREATE(CIoViewReportRank)
	

	enum E_RankType{
		ERT_RisePercent = 0,	// 涨幅
		ERT_FallPercent,		// 跌幅
		ERT_Amplitude,			// 振幅
		ERT_5MinRiseSpeed,		// 5分钟涨速
		ERT_5MinFallSpeed,		// 5分钟跌速
		ERT_VolRatio,			// 量比
		ERT_BidRatioFirst,		// 委比前
		ERT_BidRatioLast,		// 委比后
		ERT_Amout,				// 总金额
	};
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // 父窗口显示 市场 - 排行名
	virtual bool32	FromXml(TiXmlElement *pElement);		// xml功能
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// 不需要标准右键菜单

	virtual void	LockRedraw();				
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick();				// 打开某个商品，跟选股操作打开类似

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	
	// 应该是类似这个的接口
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs); // 需要做筛选
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	// 以前的数据接口应该不会用到 - 看具体什么数据接口吧 - 不假设任何数据结构
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType){};
 
	virtual	void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs);
 	
public:
	void		OpenBlock(int32 iBlockId);		// 假设市场id是唯一的
	bool32		ChangeRank(E_RankType eRank);

	void		SetShowBlockNameOnTitle(bool32 bShow = true);		// 名称上是否显示block

	bool32		GetCellCount(OUT int32 &iRowCount, OUT int32 &iColumnCount);
	bool32		GetFocusCell(OUT int32 &iRow, OUT int32 &iColumn);
	bool32		SetFocusCell(int32 iRow, int32 iColumn, bool32 bSetFocus);

private:
	bool IsHaveKlineTrendFrame();			// 当前界面是否有K线或者分时界面

///////////////////////////////////////////////////////////////////////
private:
	struct T_Header		// 没有找到转化成名称的方法，直接写死先，所有排名都是0开始
	{
		E_MerchReportField	m_eField;
		bool32				m_bDescSort;	// 降序？_
		CString				m_StrName;

		T_Header()
		{
			m_eField = EMRFRisePercent;
			m_bDescSort = true;
			m_StrName = _T("今日涨幅排名");
		}
	};
	typedef CArray<T_Header, const T_Header &>	HeaderArray;
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			GetTitleString(OUT CString &StrTitle);

	void			UpdateTableContent( CMerch *pMerch );		// 需不需要更新商品报价呢
	void			UpdateTableAllContent();									// 排行改变

	void			RecalcLayout();

	void			DoTrackMenu(CPoint posScreen);

	void			ChangeRank(const T_Header &headerRank);

	CMerch          *GetSelectMerch();

	CBlockLikeMarket *GetCurrentBlock();
	CBlockLikeMarket *GetBlock(int32 iBlockId);
	
	void			RequestSortViewData(bool32 bForce =false);		// 
	void			RequestSortViewDataAsync();

	void			RequestRealTimePrice();

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// 调用这个申请数据

private:	
	CMmiReqBlockReport				m_MmiRequestSys;		// 总会需要一个排序申请的

	CRect							m_RectTitle;
	// 表格相关
	CGridCtrlNormalSys				m_GridCtrl;	
// 	CXScrollBar						m_XSBVert;			// 不能滚动
// 	CXScrollBar						m_XSBHorz;
	
	bool32							m_bInitialized;		// 是否初始化完毕

	int32							m_iBlockId;			// 当前市场id

	int32							m_iMaxGridVisibleRow;		// 表格最大可见的数据

	MerchArray						m_aMerchs;			// 所有商品数据序列
	
	static HeaderArray				m_saHeaders;
	T_Header						m_headerCur;

	bool32							m_bShowBlockNameOnTitle;

	int32							m_iScreenHeight;	// 屏幕的高度
public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewReportRank)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewReportRank)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult );

	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_REPORT_RANK_H_

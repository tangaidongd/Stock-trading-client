#ifndef _IO_VIEW_MAIN_TIMESALE_H_
#define _IO_VIEW_MAIN_TIMESALE_H_

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

/////////////////////////////////////////////////////////////////////////////
// CIoViewMainTimeSale
class CIoViewMainTimeSale : public CIoViewBase , public CBlockConfigListener
{
// Construction 
public:
	CIoViewMainTimeSale();
	virtual ~CIoViewMainTimeSale();

	DECLARE_DYNCREATE(CIoViewMainTimeSale)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);	// xml功能
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual void	OnIoViewTabShow();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);		// 不需要标准右键菜单 - 默认

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return EIVT_MultiMerch; } 
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(CMerch *pMerch);				// 打开某个商品，跟选股操作打开类似

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);

	// 类似timesale的接口
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch){}
	// 以前的数据接口应该不会用到 - 看具体什么数据接口吧 - 不假设任何数据结构
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			// 看到底需要处理不
	virtual void	OnIoViewFontChanged();

	// block config
	virtual	void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);	// 一般是自选股变更
	virtual	void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);			// 文件更新

	virtual	bool32	IsPluginDataView() { return true; }
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);
	// 主力监控回包
	void			OnMainTimeSaleResp(const CMmiRespMainMonitor *pResp);

public:
	void		OpenTab(int32 iTabPress);		//

	bool32		IsDrawTitle()const  { return m_bDrawTitleString; }
	void		EnableDrawTitle(bool32 bDraw = TRUE) { m_bDrawTitleString = bDraw; }

///////////////////////////////////////////////////////////////////////
private:
	struct T_MainTimeSale
	{
		CMerch *m_pMerch;
		T_RespMainMonitor	m_mainMonitor;
	};
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
	typedef CList<T_MainTimeSale *, T_MainTimeSale *>	MainTimeSaleList;
	typedef CArray<CMmiReqMainMonitor, const CMmiReqMainMonitor &>  ReqInitDataArray;

	enum {
		BtnAll = 0,		// 按钮别名序号
		Btn300,
		BtnUser,
		BtnCur,
		BtnFloat,
	};

	struct T_TabBtn
	{
		CRect	m_RectBtn;
		CString m_StrName;
		int32	m_iBlockId;

		bool32	m_bNeedReqPrivateData;

		T_TabBtn()
		{
			m_RectBtn.SetRectEmpty();
			m_iBlockId = -1;
			m_bNeedReqPrivateData = false;
		}
		T_TabBtn(const T_TabBtn &tab);
		const T_TabBtn &operator=(const T_TabBtn &tab);
	};
	
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableAllContent();

	void			RecalcLayout();

	void			InitializeBtnId();

public:
	int32           GetCurTab() { return m_iCurTab; }

private:
	bool32			GetCurTab(OUT int32 &iCurTab, OUT int32 &iBlockId);
	bool32			IsMerchInCurTab(CMerch *pMerch);
	
	void			RequestPushViewData(bool32 bForce = false);		// 请求推送数据
	void			RequestInitialData();		// 请求最近数据

	void			RequestTabSpecialData();	// 请求tab特殊数据

	void			RequestUserStockData();		// 请求自选股主力监控数据
	void			RequestCurrentStockData();	// 请求当前商品主力监控数据
	
	bool32			GetCurBlockMerchs(OUT MerchArray &aMerchs);
	bool32			GetBlockMerchsByTabIndex(int32 iTab, OUT MerchArray &aMerchs);

	bool32			InsertNewMainTimeSale(CMerch *pMerch, const T_RespMainMonitor &mainTM, bool32 bAddToGrid = false);
	int32			InsertGridRow(int iRow, const T_MainTimeSale &mts, bool32 bDrawRow =false);
	void			UpdateGridRow(int iRow, const T_MainTimeSale &mts);
	void			DeleteRedundantHead(bool32 bDeleteInGrid = false);		// 裁掉多余的头部信息
	T_MainTimeSale  *CheckMTS(T_MainTimeSale *pMTS) const;						// 检查该对象是否存在
	void			UpdateAllGridRow();

	void			RequestData(CMmiCommBase &req);			// 调用这个申请数据

private:	
	CGridCtrlNormalSys				m_GridCtrl;
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;
	
	CRect							m_RectBtn;
	CArray<T_TabBtn, const T_TabBtn &>	m_aTitleBtns;		
	
	bool32							m_bInitialized;		// 是否初始化完毕

	int32							m_iCurTab;			// 当前选中的按钮

	bool32							m_bDrawTitleString;	// 是否绘制 主力监控

	bool32							m_bParentIsDialog;	// 是否为dialog的子

	MainTimeSaleList				m_lstMainTimeSales;	// 所有有序数据都在这里

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewMainTimeSale)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewMainTimeSale)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDoInitialize();
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_MAIN_TIMESALE_H_

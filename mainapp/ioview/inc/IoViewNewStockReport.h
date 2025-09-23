#ifndef _IO_VIEW_NEW_STOCK_REPORT_H_
#define _IO_VIEW_NEW_STOCK_REPORT_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "IoViewNewStock.h"


typedef enum E_NewStockReportHeadType
{
	ENSHT_NONE = 0,			// 其他类in个
	ENSHT_ListedStock,		// 已上市新股
	ENSHT_UnlistedStock,	// 未上市新股

	ENSHT_Count,			//

}E_NewStockReportHeadType;

/////////////////////////////////////////////////////////////////////////////
// CIoViewNewStockReport




class CIoViewNewStockReport : public CIoViewBase, public CNewStockNotify
{
	// Construction 
public:
	CIoViewNewStockReport();
	virtual ~CIoViewNewStockReport();

	DECLARE_DYNCREATE(CIoViewNewStockReport)

	// from CIoViewBase
public:
	
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML();
	virtual void	SetChildFrameTitle();
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch); 		// 通知视图改变关注的商品

	virtual void	OnVDataForceUpdate() {}

	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
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


	// from CNewsStockNotify
public:
	virtual void     OnNewStockResp();
	
	
	// from myself
	void             CreateListedStockTable(CFont *pFont);

	void             RecalcLayout( bool bNeedDraw );

	void             ReloadGridNewStock();

	void			 ResetGridFont();

	void             UpdateOneNewStockRow( int iRow, const T_NewStockInfo &stOut);
	
	virtual	void	UpdatePushMerchs();		// 子类需要添加一些其它的数据进去

	void			 RequestViewDataCurrentVisibleRow();
	
	void             RequestViewDataCurrentVisibleRowAsync();
	

	// 新股表格
	CGridCtrlNormal		m_GridNewStock;
	CXScrollBar			m_XBarNewStockH;
	CXScrollBar			m_XBarNewStockV;
	int					m_iAllNewStockHeight;
	int                 m_iAllColumnWidth;					// 总列宽
	
	CFont				m_Font;		
	E_NewStockReportHeadType		m_eReportHeadType;		// 表头类型
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewNewStockReport)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewNewStockReport)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	//linhc 20100909添加
	afx_msg void OnGridSelRowChanged(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_NEW_STOCK_REPORT_H_

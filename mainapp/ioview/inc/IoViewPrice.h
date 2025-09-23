#ifndef _IO_VIEW_PRICE_H_
#define _IO_VIEW_PRICE_H_

#include "dlgbelongblock.h"
#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "MarkManager.h"

#include <vector>
#include <utility>
#include <map>
using std::pair;
using std::vector;
using std::map;
/////////////////////////////////////////////////////////////////////////////
// CIoViewPrice
#define  SNAPSHOTTIMER				8456325	
#define  IOVIEWPRICEROWHEIGHT		30

enum E_TickProperty
{
	ETP_DuoKai = 0,		// 多开
	ETP_DuoPing,		// 多平
	ETP_DuoHuan,		// 多换
	ETP_KongKai,		// 空开
	ETP_KongPing,		// 空平
	ETP_KongHuan,		// 空换
	ETP_ShuangKai,		// 双开
	ETP_ShuangPing,		// 双平

	ETP_Count
};

CString	TickPropertyToString(E_TickProperty eProperty);

class CIoViewPrice : public CIoViewBase, public CMerchMarkChangeListener, public CGridCtrlCB
{
// Construction
public:
	CIoViewPrice();
	virtual ~CIoViewPrice();

	DECLARE_DYNCREATE(CIoViewPrice)

	// 买卖盘中指数部分新增，放在这里
	enum E_MerchExpType
	{
		EMET_ShangZhengZhiShu,	// 上证指数
		EMET_ShenZhenZhiShu,	// 深证指数
		EMET_Count,				// 无法识别的标志
	};
	
protected:
	virtual void OnFixedRowClickCB(CCellID& cell){}
	virtual void OnFixedColumnClickCB(CCellID& cell){}
    virtual void OnHScrollEnd(){}
    virtual void OnVScrollEnd(){}
	virtual void OnCtrlMove( int32 x, int32 y ){}
	virtual bool32 OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	virtual void OnGridMouseMove(CPoint pt);

	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData();
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	//linhc 20100904添加右击菜单
	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMarketSnapshotUpdate(int32 iMarketId);
	
	//linhc 20100914 修改                  
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);		// 国内期货用到
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);	// 国内期货用到
	virtual void	OnIoViewFontChanged();

	virtual void	OnVDataGeneralNormalUpdate(CMerch* pMerch);

	// 标记
	virtual void	OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

public:
	int32		    CalcRiseFallCounts(bool32 bRise);	
	void			ShowRiseFallCounts(int32 iRiseCounts,int32 iFallCounts);
	
	bool32			BeSpecial();

	// from CView	
protected:
	virtual void	Draw();
	void			SetSize();

	CSize			GetProperSize(OUT float *pfCharWidth=NULL);

private:
	void			SetRowHeightAccordingFont();								// 根据字体大小,设置行高
	bool32		    CreateTable(E_ReportType eMerchKind);		

	// 指数新增字段填充
	typedef vector<CMerch *>    ExpMerchArray;
	typedef	pair<CString,  ExpMerchArray >	ExpMerchPair;
	typedef CArray<ExpMerchPair, const ExpMerchPair & > AppendExpMerchArray;
	E_MerchExpType	GetMerchExpType(CMerch *pMerch);	// 获取指数商品对应的指数类型
	bool32			GetAppendExpMerchArray(E_MerchExpType eMET, OUT AppendExpMerchArray &aExpMerchs);	// 获取指数类型要附加的显示商品列表
	float			GetAppendExpAmout(const ExpMerchPair &expPair);	// 获取对应项的成交量

	// 国内期货
	bool32			IsFutureCn(CMerch *pMerch);
	bool32			GetFutureCnStartEndTime(CMerch *pMerch, OUT CGmtTime &TimeStart, CGmtTime &TimeEnd);
	bool32			GetFutureCnStartEndTime(CMerch *pMerch, OUT CMsTime &TimeStart, CMsTime &TimeEnd);
	bool32			AnalyzeTickData(const CTick &tick, float fPreHold);	// 统计分笔结果
	E_TickProperty	AnalyzeTickProperty(const CTick &tick, float fPreHold); // 分析分笔属性
	void			ResetFutureCnData();	// 重置内容
	void			ShowFutureCnData(bool32 bDraw=true);		// 将数据显示到表格
	void			InitFutureCnTradingDay();
	void			RequestFutureCnTicks();	// 请求分笔数据
	bool32			UpdateFutureCnTick();
	bool32			IsNeedShowFutureCnTick();		// 是否需要分笔统计

private:
	bool32			BeGeneralIndex(OUT bool32& bH);
	bool32			BeShowBuySell(int32 Marketid);
	int32			GetShowBuySellItem(int32 Marketid);	// 获取显示几档
	bool32			SetGridCtrlRow(int32 iItem);		// 创建一个指定大小档数的表格

		
	bool32			BeSpecialBuySell();
	bool32			BeERTMony(); // 是否是做市商

	// 计算内盘字符串
	CString CalculatefVolumnBuy(float fTotalValue, float fBuyNum, float fSellNum,float fRefer,bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false, bool32 bShowNegative=false);
////////////////////////////////////////////////////////////
//
protected:
	BOOL			m_bIsShowBuySell;	// 是否显示买卖5档
	int32			m_iBuySellCnt;		// 显示几档(1-5)
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CGridCtrlSys	m_GridCtrl;
	CImageList		m_ImageList;	
	CStaticSD		m_CtrlTitle;
	CRect			m_rcTitleHead;
	bool32			m_bFromXml;
	CGridCtrlSys	m_GridExpRiseFall;	// 上涨下跌另起表格

	CXTipWnd		m_TipWnd;
	
	int32			m_iRowHeight;

	int32			m_iTitleHeight;
	int32			m_iPerfectHeight;
	
	CRect			m_RectView;
	CRect			m_RectIndexRnG;		// 指数红绿柱的显示区域
	
	E_ReportType     m_eMerchKind;
	CArray<int32,int32>				m_aSeparatorLine;
	CArray<int32,int32>				m_aMarketIdArray;		
		

	AppendExpMerchArray				m_aExpMerchs;

	typedef map<E_ReportType, int32> MerchTypeHeightMap;
	MerchTypeHeightMap				m_mapMerchHeight;	// 各个商品类型的高度

	// 国内期货
	typedef CArray<CTick, CTick>	TickArray;
	CTrendTradingDayInfo			m_TrendTradingDayInfo;	// 
	TickArray						m_aFutureCnTicks;	// 分笔数据统计
	float							m_fVolDuoKai;		// 多开
	float							m_fVolDuoPing;		// 多平
	float							m_fVolKongKai;		// 空开
	float							m_fVolKongPing;		// 空平
	CString							m_StrFutureCnProperty;	// 当前一笔性质
	DWORD							m_dwFutureCnProperty;

	//
	CFont							m_fontBuySell;
	
	// 贵金属买卖价的区域
	CRect							m_RectSpecialBuy;
	CRect							m_RectSpecialSell;


	int32			m_iScreenHeight;	// 屏幕的高度

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewPrice)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewPrice)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	//linhc 20100907添加右击响应菜单
    afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLDBClick(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void OnGridEditBegin(NMHDR *pHdr, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_

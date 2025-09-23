#ifndef _H_IOVIEW_DETAIL_
#define _H_IOVIEW_DETAIL_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "TrendIndex.h"
#include "SaneIndex.h"

extern const int32 KiStatisticaKLineNums;
/////////////////////////////////////////////////////////////////////////////
// CIoViewDetail
 
#define	KLINE_NUMS		(int32)3														// 包含了几条 K 线

typedef struct T_IoViewDetailCell														// 每一条 K 线对应的所有信息
{
	T_IoViewDetailCell()
	{
		m_iTimeUserMultipleDays			= 10;
		m_iTimeUserMultipleMinutes		= 10;

		m_iSaneStaticsNums				= 0;

		m_bTimeToUpdateLatestValues		= false;
		
		m_bNeedCalcSaneLatestValues		= true;

		m_aKLines.RemoveAll();		
		
		m_pIndexMaForSane				= NULL;
		m_aSaneIndexValues.RemoveAll();	
		m_aMerchsHaveCalcLatestValues.RemoveAll();

		m_eNodeInterval					= ENTIMinute;
		m_eHoldState					= EHSNONE;
		m_eActionPrompt					= EAPNONE;
		
		m_TimeToUpdate					= 0;	

		m_iTradeTimes					= 0;
		m_fAccuracyRate					= 0.0;
		m_fProfitability				= 0.0;

		m_RectShow						= CRect(-1, -1, -1, -1);
		m_bCalcing						= false;

	}

	//
	~T_IoViewDetailCell()
	{
		DEL(m_pIndexMaForSane);
	}
	
	//
	int32											m_iTimeUserMultipleMinutes;			// 自定义分钟线
	int32											m_iTimeUserMultipleDays;			// 自定义日线
	
	int32											m_iSaneStaticsNums;					// 稳健统计个数

	bool32											m_bNeedCalcSaneLatestValues;		// 是否要计算最新指标值	

	bool32											m_bTimeToUpdateLatestValues;		// 是否到了更新这根K 线的指标值的时机(除了清仓以外的那些条件.与 10 秒这个条件对应)	
	CGmtTime										m_TimeToUpdate;						// 更新实时数据的时间点	
	CGmtTime										m_TimeToUpdateSane;					
	
	CArray<CKLine, CKLine>							m_aKLines;							// 本周期的K 线序列
	CArray<CMerch*, CMerch*>						m_aMerchsHaveCalcLatestValues;		// 已经算过最新值的商品,防止来回切换商品的时候重复多次计算最新值.
	
	T_IndexOutArray*								m_pIndexMaForSane;					// GGTEMA 指标序列
	CArray<T_SaneIndexNode,T_SaneIndexNode>			m_aSaneIndexValues;					// 稳健指标的值

	E_NodeTimeInterval								m_eNodeInterval;					// K线的周期
	E_HoldState									    m_eHoldState;						// 当前持仓状态
	E_ActionPrompt									m_eActionPrompt;					// 当前操作提示

	int32											m_iTradeTimes;						// 交易次数
	float											m_fAccuracyRate;					// 准确率
	float											m_fProfitability;					// 收益率

	CRect											m_RectShow;							// 前面的复选框
	bool32											m_bCalcing;							// 发现类似多线程的状况?? 函数还没走完,又进来了???

}T_IoViewDetailCell;
 
class CIoViewDetail : public CIoViewBase
{
// Construction
public:
	CIoViewDetail();
	virtual ~CIoViewDetail();

	DECLARE_DYNCREATE(CIoViewDetail)
	
	// from CControlBase
public:
	virtual BOOL		TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void		RequestViewData();	
	virtual void		SetChildFrameTitle();
	virtual bool32		FromXml(TiXmlElement *pElement);
	virtual CString		ToXml();
	virtual	CString		GetDefaultXML();
	
	virtual void		OnIoViewActive();
	virtual void		OnIoViewDeactive();
	virtual bool32		GetStdMenuEnable(MSG* pMsg) { return false; }
	
	virtual void		OnWeightTypeChange();
	virtual void		LockRedraw();
	virtual void		UnLockRedraw();
	// from CIoViewBase
public:
	virtual void		OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void		OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void		OnVDataForceUpdate();
	
	virtual void		OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual	void		OnVDataMerchKLineUpdate(IN CMerch *pMerch);

	virtual void		OnIoViewFontChanged();
	virtual void		OnIoViewColorChanged();

public:
	void				OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue);

private:
	
	void				RequestWeightData();
	bool32				SetTimeInterval( E_NodeTimeInterval eNodeInterval);
	void				SetViewDetailString();
	void				SetViewDetailString(CDC &dc);
	
	bool32				CreateTable();		
	void				DealWithSpecial(const T_SaneIndexNode& Node, const CString& StrTime, const CString& StrPrice);
	void				UpdateOneRow(int32 iUpdateFlag, int32 iRow, int32 iDataIndex);
	void				UpdateTableContent(int32 iUpdateFlag);
	void				SetRowHeightAccordingFont();							

	//
	void				SetUpdateTime(int32 iIndex);									// 设置更新时间点
	void				OnTimerUpdateLatestValuse(UINT uID);							// 更新实时指标值		
	
	//
	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// 判断两端K 线的异同. 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化
////////////////////////////////////////////////////////////
private:
	CGridCtrlSys									m_GridCtrl;
	CXScrollBar										m_XSBVert;
	CXScrollBar										m_XSBHorz;

	int32											m_iRowHeight;						// 行高	
	CRect											m_RectText;							// 文字区域
	CRect											m_RectGrid;							// 表格区域

	int32											m_iCurShow;							// 当前显示的是哪条
	CTime											m_TimePre;							// 表格第一行的节点对应K 线时间
	
	T_IoViewDetailCell								m_aIoViewDetailCells[KLINE_NUMS];	// 指标,K线 等信息数组

	void											OnConditionOpenClose();				// 重算一些值

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewDetail)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewDetail)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMenu(UINT uID);
	afx_msg void OnIndexMenu(UINT uID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.




#endif  //_H_IOVIEW_DETAIL_
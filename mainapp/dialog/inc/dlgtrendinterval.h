#if !defined(_DLGTRENDINTERVAL_H_)
#define _DLGTRENDINTERVAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgtrendinterval.h : header file
//


#include "DialogEx.h"
#include "ViewDataListener.h"
#include "PieChartCtrl.h"
#include <map>

class CMerch;
class CKLine;
class CTick;
class CIoViewTrend;

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendInterval dialog

// 排序用到
typedef struct ListCmpData
{
	int32	m_iFlag;	// 买卖标记
	float	m_fPrice;	// 成交价
	float	m_fVolume;	// 成交量
	long	m_lTime;	// 日期
	
}ListCmpData;

// 第三页的每一行的买卖量
typedef struct T_TrendIntervalBS
{
	T_TrendIntervalBS()
	{
		m_fBuyVolumn = 0.;
		m_fSellVolumn= 0.;
	}

	//
	float		m_fBuyVolumn;
	float		m_fSellVolumn;

}T_TrendIntervalBS;

typedef struct T_TrendInterval
{
public:
	T_TrendInterval()
	{
		Clear();
	}

	void Clear()
	{
		m_fNeipan			= 0.;						
		m_fWaipan			= 0.;						
		
		m_iCountBig			= 0.;					
		m_fVolumnBig		= 0.;					
		
		m_iCountBigBuy      = 0.;					
		m_fVolumnBigBuy     = 0.;				
		
		m_iCountBigSell     = 0.;				
		m_fVolumnBigSell    = 0.;				
		
		m_iCountSmall       = 0.;					
		m_fVolumnSmall      = 0.;					
		
		m_iCountSmallBuy    = 0.;				
		m_fVolumnSmallBuy   = 0.;				
		
		m_iCountSmallSell   = 0.;				
		m_fVolumnSmallSell  = 0.;				
		
		m_iCountUnKnown     = 0.;				
		m_fVolumnUnKnown    = 0.;				
		
		m_fRateBigBuy       = 0.;					
		m_fRateSmallBuy     = 0.;				
		m_fRateBigSell      = 0.;					
		m_fRateSmallSell    = 0.;				
		m_fRateUnKnown      = 0.;					
		
		m_fMinPrice			= 0.;					
		m_fMaxPrice			= 0.;					
		
		m_fMaxBuyVolumn     = 0.;				
		m_fMaxSellVolumn    = 0.;				
		
		m_mapBuySell.clear(); 
	}

	// 基本字段
	float   m_fNeipan;						// 内盘
	float   m_fWaipan;						// 外盘
		
	int32	m_iCountBig;					// 大单数
	float	m_fVolumnBig;					// 大单量

	int32	m_iCountBigBuy;					// 买大单数
	float   m_fVolumnBigBuy;				// 买大单量
	
	int32	m_iCountBigSell;				// 卖大单数
	float   m_fVolumnBigSell;				// 卖大单量

	int32	m_iCountSmall;					// 小单数
	float	m_fVolumnSmall;					// 小单量
	
	int32	m_iCountSmallBuy;				// 买小单数
	float   m_fVolumnSmallBuy;				// 买小单量
	
	int32	m_iCountSmallSell;				// 卖小单数
	float   m_fVolumnSmallSell;				// 卖小单量

	int32	m_iCountUnKnown;				// 未知成交数
	float	m_fVolumnUnKnown;				// 未知成交量

	// 比例 (第一页饼状图)
	float	m_fRateBigBuy;					// 主买大单比例
	float	m_fRateSmallBuy;				// 主买小单比例
	float	m_fRateBigSell;					// 主卖大单比例
	float	m_fRateSmallSell;				// 主卖小单比例
	float	m_fRateUnKnown;					// 未知成交比例

	// 第三页显示的内容
	float   m_fMinPrice;					// 最低价
	float	m_fMaxPrice;					// 最高价

	float	m_fMaxBuyVolumn;				// 最大买量
	float	m_fMaxSellVolumn;				// 最大卖量

	map<float, T_TrendIntervalBS>	m_mapBuySell; // 每一个价格对应的买卖量

}T_TrendInterval;

class CDlgTrendInterval : public CDialogEx , public CViewDataListner
{
	// From CViewDataListner
public: 
	// 分笔
	void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);

public:

	// 设置初始化的参数
	void SetKLineData(CIoViewTrend* pIoViewTrend, CMerch* pMerch, float fPreColse, const CArray<CKLine, CKLine>& aKLineSrc) 
	{
		if ( NULL == pMerch || NULL == pIoViewTrend )
		{
			return;
		}
	
		m_pIoViewTrend = pIoViewTrend;
		m_pMerch = pMerch;
		m_fPricePreClose = fPreColse;
		m_aKLineSrc.Copy(aKLineSrc);
	}

	// 更新显示
	void	UpdateShow(bool32 bForceUpdate = false);
		
	// 第一页
	void	ShowPage1();

 	// 第二页
	void	ShowPage2(bool32 bForceUpdate = false);
	
	// 第三页
	void	ShowPage3();

	// 看数据够不够, 不够就请求
	void	RequestData();

	// 是否是等于开盘时间
	bool32	BeSpecialBegin(IN const CGmtTime& Time, OUT CGmtTime& TimeBegin);

	// 是否等于收盘时间
	bool32	BeSpecialEnd(IN const CGmtTime& Time, OUT CGmtTime& TimeEnd);

	// 取TICK 数据
	void	UpdateLoaclData();

	// 统计数据
	void	CalcStatisticData();

	bool32	IsRectInRgn(const CRgn &rgn, const CRect &rc);

	void	RedrawAsync();

	// 获取大单限值
	void	FromXml();

	// 保存大单限值
	void	ToXml();

	// 
	bool32	CreateDefaultXml();

	// 排序
	void	Sort(int32 iCol, bool32 bAscend);

	// 排序回调函数
	static int	CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);

private:
	
	// Tick 数据是否足够
	bool32		m_bTickEnough;

	// 所属视图
	CIoViewTrend* m_pIoViewTrend;

	// 商品
	CMerch*		m_pMerch;

	// 第一根K 线的昨收价
	float		m_fPricePreClose;

	// 传入的 K 线
	CArray<CKLine, CKLine> m_aKLineSrc;

	// 数据足够的时候, 就保存本地, 以后的计算显示都用本地的, 否则就不停的从商品缓存中取, 看能否取到更新更多的数据, 本地的始终为空
	CArray<CTick, CTick>   m_aTicks;

	// 大单限制
	int32		m_iLimit;

	// 统计信息
	T_TrendInterval m_stResult;

	// 收到的tick数据时间段
	CTick		m_TickStart;
	CTick		m_TickEnd;
	bool32		m_bTryReqTick;	// 尝试请求tick数据的标志

	// 大饼
	CPieChartCtrl m_Pie;

	// 排序相关
	int32		m_iSortColumn;
	bool32		m_bAscend; 

// Construction
public:
	CDlgTrendInterval(CWnd* pParent = NULL);   // standard constructor
	~CDlgTrendInterval();
// Dialog Data
	//{{AFX_DATA(CDlgTrendInterval)
	enum { IDD = IDD_DIALOG_INTERVAL_TREDN };
	CListCtrl	m_ListTick;
	CString	m_StrV1;
	CString	m_StrV2;
	CString	m_StrV3;
	CString	m_StrV4;
	CString	m_StrV5;
	CString	m_StrAvgVolumn;
	CString	m_StrBuy;
	float	m_fClose;
	int		m_iCountMin;
	int		m_iCountTick;
	float	m_fHigh;
	float	m_fLow;
	CString	m_StrNei;
	float	m_fOpen;
	CString	m_StrRiseRate;
	CString	m_StrSell;
	CString m_StrBigAll;
	CString	m_StrShakeRate;
	CString	m_StrTimeBegin;
	CString	m_StrTimeEnd;
	CString	m_StrVolumn;
	CString	m_StrWai;
	int		m_iEditLimit;
	float	m_fAvgPrice;
	int		m_iRadio;
	CImageList m_ImageList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrendInterval)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrendInterval)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnLimitChange();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnColumnclickListTick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(_DLGTRENDINTERVAL_H_)

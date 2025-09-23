#if !defined(_DLGKLINEQUANT_H_)
#define _DLGKLINEQUANT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgklinequant.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgKLineQuant dialog
#include "DialogEx.h"
#include "IoViewShare.h"
#include "afxcmn.h"
#include <vector>


typedef enum E_IndexColorType
{
	EICT_NONE,
	EICT_BLUE,
	EICT_YELLOW
}E_IndexColorType;

typedef struct T_IndexData
{
	E_IndexColorType eClrType;
	uint32			m_iTime;
	float			m_fClose;
	T_IndexData()
	{
		eClrType = EICT_NONE;
		m_iTime  = 0;
		m_fClose = 0.0;
	}
}T_IndexData;

typedef vector<T_IndexData> IndexDataArray;
//////////////////////////////////////////////////////////////////////////
typedef struct T_IndexAnalyseData
{
	uint32			m_iStartTime;
	uint32          m_iEndTime;
	float           m_fRiseFallRate;
	T_IndexAnalyseData()
	{
		m_iStartTime     = 0;
		m_iEndTime      = 0;
		m_fRiseFallRate = 0.0;
	}
}T_IndexAnalyseData;

typedef vector<T_IndexAnalyseData> IndexAnalyseDataArray;
//////////////////////////////////////////////////////////////////////////

typedef struct T_ListItemData
{
	CGmtTime timeStart;
	CGmtTime timeEnd;
	T_ListItemData(CGmtTime &tmStart, CGmtTime &tmEnd)
	{
		timeStart = tmStart;
		timeEnd   = tmEnd;
	}
}T_ListItemData;
typedef vector<T_ListItemData> ListItemDataArray;

//////////////////////////////////////////////////////////////////////////


class CKLine;
class CDlgKLineQuant : public CDialogEx
{
public:
	void		SetKLineData(CIoViewKLine* pKLine, CMerch* pMerch, E_NodeTimeInterval eInterval, int32 iUserDay, int32 iIndexShowPos, int32 iIndexShowCnt, const CArray<CKLine, CKLine>& aKLineSrc);

private:
	
	void		UpdateShow();

	void		ValidTime(bool32 bBeginChange);

	bool32		BeMiniute();
	CString     GetTimeString(const CGmtTime& Time, bool32 bOnlyDay = false);
	CString     GetTimeString(CTime& Time);
	void        UpdateBtnStatus(UINT nID);
	void        ShowListData(UINT nID);

	void        ChangeItemData(const IndexAnalyseDataArray &arrAnalyseData);

	void        InitIndexParam();

	void        AnalyseIndexData();
	
	void        ParseBlueToYellowData();

	void        ParseYellowToBlueData();

private:

	// K 线数据
	CArray<CKLine, CKLine>	m_aKLines;
	
	// 周期
	E_NodeTimeInterval		m_eInterval;


	// 商品
	CMerch*					m_pMerch;

	// 多日线参数
	int32					m_iUserDay;

	// 视图
	CIoViewKLine*			m_pIoViewKLine;


	// 是否隐藏右边List控件
	bool32                  m_bChangeMinStatus;

	CFont			        m_fontStaticText;

	//--- wangyongxue 2016/11/23
	IndexDataArray          m_aIndexData;

	//---保存计算后的结果
	IndexAnalyseDataArray   m_aRisePeriod;		// 上升阶段
	IndexAnalyseDataArray   m_aShakePeriod;     // 震荡阶段
	IndexAnalyseDataArray   m_aFallPeriod;		// 下降阶段

	ListItemDataArray       m_aItemData;		// 保存ItemData数据
	

	// 当前K线选择区域pos及K线数量
	int32                   m_iIndexShowPos;
	int32                   m_iIndexShowCount;
// Construction
public:
	CDlgKLineQuant(CWnd* pParent = NULL);   // standard constructor
	~CDlgKLineQuant();
// Dialog Data
	//{{AFX_DATA(CDlgKLineQuant)
	enum { IDD = IDD_DIALOG_QUANT_KLINE };
	CString	  m_StrTimeBegin;
	CString	  m_StrTimeEnd;
	CString   m_StrNum;
	CTime	  m_TimeBegin;
	CTime	  m_TimeEnd;
	CListCtrl m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgKLineQuant)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgKLineQuant)
	afx_msg void OnButtonIntervalSort();
	afx_msg void OnButtonBlockSort();
	afx_msg void OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnPeriodClick( UINT nId );
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	// 相对收益
	CString m_StrRelativeProfit;
	// 最大收益
	CString m_StrMaxProfit;
	// 出击次数
	int m_iAttackCnt;
	// 累计收益
	CString m_StrRisePeriodTotalProfit;
	// 成功率
	CString m_StrSuccessRate;
	// 平均收益
	CString m_StrRisePeriodAvgProfit;
	// 单次最大规避损失
	CString m_StrMaxSingleLoss;
	// 累计规避损失
	CString m_StrCumulativeLoss;
	// 下降阶段提示次数
	int m_iFallPeriodPromptCnt;
	// 震荡阶段累计收益
	CString m_StrShakePeriodTotalProfit;
	// 震荡阶段平均收益
	CString m_StrShakePeriodAvgProfit;
	// 震荡阶段提示次数
	int m_iShakePeriodPromptCnt;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGKLINEQUANT_H_)

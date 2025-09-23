#if !defined(_DLGKLINEINTEVAL_H_)
#define _DLGKLINEINTEVAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgklineinteval.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgKLineInteval dialog
#include "DialogEx.h"
#include "IoViewShare.h"

class CKLine;

class CDlgKLineInteval : public CDialogEx
{
public:
	void		SetKLineData(CIoViewKLine* pKLine, CMerch* pMerch, E_NodeTimeInterval eInterval, int32 iUserDay, float fCircAsset, float fPreClose, const CArray<CKLine, CKLine>& aKLineSrc);

private:
	
	void		UpdateShow();

	void		ValidTime(bool32 bBeginChange);

	bool32		BeMiniute();

private:

	// K 线数据
	CArray<CKLine, CKLine>	m_aKLines;
	
	// 周期
	E_NodeTimeInterval		m_eInterval;

	// 流通股本
	float					m_fCircAsset;

	// 商品
	CMerch*					m_pMerch;

	// 多日线参数
	int32					m_iUserDay;

	// 视图
	CIoViewKLine*			m_pIoViewKLine;

	// 第一根K 线的昨收盘
	float					m_fPricePreClose;

// Construction
public:
	CDlgKLineInteval(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgKLineInteval)
	enum { IDD = IDD_DIALOG_INTERVAL_KLINE };
	float	m_fClose;
	float	m_fHigh;
	float	m_fLow;
	float	m_fOpen;
	CString	m_StrTimeBegin;
	CString	m_StrTimeEnd;
	float	m_fAvgPrice;
	CString	m_StrNum;
	CString	m_StrVolumn;
	CString	m_StrAmount;
	CString	m_StrRiseRate;
	CString	m_StrShakeRate;
	CString	m_StrChange;
	CTime	m_TimeBegin;
	CTime	m_TimeEnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgKLineInteval)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgKLineInteval)
	afx_msg void OnButtonIntervalSort();
	afx_msg void OnButtonBlockSort();
	afx_msg void OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGKLINEINTEVAL_H_)

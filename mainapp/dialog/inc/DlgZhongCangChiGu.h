#ifndef _DLGZHONGCANGCHIGU_H_
#define _DLGZHONGCANGCHIGU_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgZhongCangChiGu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgZhongCangChiGu dialog
#include "DialogEx.h"

#include "PieChartCtrl.h"

typedef struct _ZhongCangChiGuPseudo
{
	int32	iMarketId;
	CString StrMerchCode;
	CString	StrMerchName;

	float	fBenQiChiGu;				// 本期持股
	float	fShangShiGongSi;			// 占上市公司
	float	fJiJinJingZhi;				// 占基金净值
	float	fShangQiChiGu;				// 上期持股
	float	fPercent;					// 占所有的比例

	_ZhongCangChiGuPseudo()
	{
		iMarketId = -1;
		fBenQiChiGu = fShangQiChiGu = fShangShiGongSi = fJiJinJingZhi = fPercent = 0.0f;
	}
}T_ZhongCangChiGuPseudo;

class CMerch;

class CDlgZhongCangChiGu : public CDialogEx
{
public:
	typedef CArray<T_ZhongCangChiGuPseudo, const T_ZhongCangChiGuPseudo&> ZhongCangChiGuArray;

	void		SetMerch(CMerch *pMerch);

	void		OnDataUpdate(const ZhongCangChiGuArray &aData);		// 数据更新了

private:
	void		RequestViewData();

	void		InitShow();
	void		UpdateStageTip();

	static		int CALLBACK MyCompareRow(LPARAM lp1, LPARAM lp2, LPARAM lpThis);

	void		ClearSortFlag();

	void		FetchDataFromViewData(OUT ZhongCangChiGuArray &aData);	// 从viewdata中取数据
	
	bool32		BeDataAlready();

private:
	ZhongCangChiGuArray		m_aZhongCangChiGu;
	
	int32		m_iStage;		// 0-等待请求数据返回 1-数据已经返回

	int32		m_iSortCol;
	BOOL		m_bSortDesc;

	//
	bool32		m_bDataAlready;

private:
	// 商品
	CMerch*					m_pMerch;
	
	CPieChartCtrl			m_WndPie;
	// Construction
public:
	CDlgZhongCangChiGu(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgZhongCangChiGu)
	enum { IDD = IDD_DIALOG_ZHONGCANGCHIGU };
	CListCtrl	m_WndList;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgZhongCangChiGu)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgZhongCangChiGu)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnColumnClick(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_DLGZHONGCANGCHIGU_H_
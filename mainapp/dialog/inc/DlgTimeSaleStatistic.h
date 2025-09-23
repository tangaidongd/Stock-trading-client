#ifndef _DLG_TM_STATISTIC_H_
#define _DLG_TM_STATISTIC_H_


#include "IoViewTimeSaleStatistic.h"
#include "IoViewTimeSaleRank.h"


//////////////////////////////////////////////////////////////////////////
// CTabWndCtrl
class CTabWndCtrl : public CTabCtrl
{
public:
	DECLARE_DYNAMIC(CTabWndCtrl)

	void	RecalcLayout(CRect rcInflate=CRect(0,0,0,0));
protected:
	afx_msg	BOOL OnSelChange(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
private:
};

class CMerch;
/////////////////////////////////////////////////////////////////////////////
// CDlgTimeSaleStatistic dialog used 

class CDlgTimeSaleStatistic : public CDialog
{
	DECLARE_DYNCREATE(CDlgTimeSaleStatistic)
public:
	~CDlgTimeSaleStatistic();

	static	void	ShowDlgTimeSaleStatistic();
	
private:
	CDlgTimeSaleStatistic(CWnd *pParent = NULL);

	void	RecalcLayout();

	void	ShowTitle();
	
private:
	static	CDlgTimeSaleStatistic		*m_spThis;

	CString				m_StrTitle;

	CIoViewTimeSaleStatistic	m_wndTimeSale;
	CIoViewTimeSaleRank			m_wndRank;
	
	// Dialog Data
	//{{AFX_DATA(CDlgTimeSaleStatistic)
	enum { IDD = IDD_DIALOG_TM_STATISTIC };
	CTabWndCtrl	m_ctrlTab;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTimeSaleStatistic)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual	void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgTimeSaleStatistic)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnSelChange(NMHDR *pNMHeader, LRESULT *pResult);
	afx_msg void		OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif //_DLG_TM_STATISTIC_H_
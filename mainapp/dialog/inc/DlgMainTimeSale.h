#ifndef _DLG_TM_STATISTIC_H_
#define _DLG_TM_STATISTIC_H_


#include "IoViewMainTimeSale.h"


//////////////////////////////////////////////////////////////////////////


class CMerch;
/////////////////////////////////////////////////////////////////////////////
// CDlgMainTimeSale dialog used 

class CDlgMainTimeSale : public CDialog
{
	DECLARE_DYNCREATE(CDlgMainTimeSale)
private:
	CDlgMainTimeSale(CWnd *pParent = NULL);

public:
	static int		ShowPopupMainTimeSale(CIoViewMainTimeSale *pMainTmSrc = NULL);		// 

private:
	void	RecalcLayout();
	
private:
	CIoViewMainTimeSale	m_wndTimeSale;

	CIoViewMainTimeSale	*m_pMainTimeSaleSrc;		// 该成员仅在对话框初始化时有用，因为有可能在随后被关闭

	static	CDlgMainTimeSale *m_spThis;
	
	// Dialog Data
	//{{AFX_DATA(CDlgMainTimeSale)
	enum { IDD = IDD_DIALOG_MAIN_TIMESALE };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMainTimeSale)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgMainTimeSale)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif //_DLG_TM_STATISTIC_H_
#if !defined(_DLGARBITRAGESEL_H_)
#define _DLGARBITRAGESEL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgarbitragesel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrageSel dialog

#include "DialogEx.h"
#include "ArbitrageManage.h"

//
class CDlgArbitrageSel : public CDialogEx
{
public:
	CArbitrage* GetSelArbitrage() { return m_pArbitrageSel; }

private:
	void	InitialCtrls();

private:
	CArbitrage*	m_pArbitrageSel;

// Construction
public:
	CDlgArbitrageSel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgArbitrageSel)
	enum { IDD = IDD_DIALOG_ARBITRAGE_SEL };
	CListCtrl	m_ListArbitrage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgArbitrageSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgArbitrageSel)
	virtual void OnOK();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGARBITRAGESEL_H_)

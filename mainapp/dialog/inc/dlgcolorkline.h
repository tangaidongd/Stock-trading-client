#if !defined(AFX_DLGCOLORKLINE_H_)
#define AFX_DLGCOLORKLINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgcolorkline.h : header file
//

#include "DialogEx.h"
#include "CFormularContent.h"

class CFormulaLib;

/////////////////////////////////////////////////////////////////////////////
// CDlgColorKLine dialog

class CDlgColorKLine : public CDialogEx
{
public:
	void		InitialList();
	CFormularContent*	GetFormular();

private:
	CFormulaLib*	m_pLib;
	CImageList		m_ImageList;
	CFormularContent* m_pFormularSelected;
	
	E_FormularType	m_eType;
// Construction
public:
	CDlgColorKLine(E_FormularType eType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorKLine)
	enum { IDD = IDD_DIALOG_COLOR_KLINE };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorKLine)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorKLine)
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORKLINE_H_)

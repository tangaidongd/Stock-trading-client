#if !defined(AFX_DLGOFFLINEDATATYPECHOOSE_H__)
#define AFX_DLGOFFLINEDATATYPECHOOSE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgofflinedatatypechoose.h : header file
//

#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgOffLineDataTypeChoose dialog

class CDlgOffLineDataTypeChoose : public CDialogEx
{
public:
	int32		GetDownLoadType();
// Construction
public:
	CDlgOffLineDataTypeChoose(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOffLineDataTypeChoose)
	enum { IDD = IDD_DIALOG_OFFLINEDATA };
	int		m_iRatio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOffLineDataTypeChoose)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOffLineDataTypeChoose)
		// NOTE: the ClassWizard will add member functions here	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOFFLINEDATATYPECHOOSE_H__)

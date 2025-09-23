#if !defined(AFX_DLGSYSTEMSETTING_H__DAB14EFC_70BD_418F_BEEA_CE913CA3B51B__INCLUDED_)
#define AFX_DLGSYSTEMSETTING_H__DAB14EFC_70BD_418F_BEEA_CE913CA3B51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsystemsetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemSetting dialog
#include "TabSheet.h"
#include "dlgsystemface.h"
#include "dlgsystemparameter.h"
#include "DialogEx.h"

class CDlgSystemSetting : public CDialogEx
{
// Construction
public:
	CDlgSystemFace m_DlgSystemFace;
	//CDlgSystemParameter m_DlgSystemParameter;	
public:
	CDlgSystemSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSystemSetting)
	enum { IDD = IDD_DIALOG_SYSTEM_SET };
	CTabSheet	m_TabSheet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSystemSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSystemSetting)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYSTEMSETTING_H__DAB14EFC_70BD_418F_BEEA_CE913CA3B51B__INCLUDED_)

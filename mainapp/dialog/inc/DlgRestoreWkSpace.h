#if !defined(AFX_DLGRESTOREWKSPACE_H__44D80062_A8ED_4F7B_BB25_C1491EE0F063__INCLUDED_)
#define AFX_DLGRESTOREWKSPACE_H__44D80062_A8ED_4F7B_BB25_C1491EE0F063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRestoreWkSpace.h : header file
//

#include "DialogEx.h"
#include "WspManager.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgRestoreWkSpace dialog

class CDlgRestoreWkSpace : public CDialogEx
{
// Construction
public:
	CDlgRestoreWkSpace(CWnd* pParent = NULL);   // standard constructor
public:
	T_CfmFileInfo m_CurrentCfmInfo;
public:
// Dialog Data
	//{{AFX_DATA(CDlgRestoreWkSpace)
	enum { IDD = IDD_DIALOG_RESTOREWKSPACE };
	CListCtrl	m_ctrlWkSpaceList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRestoreWkSpace)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRestoreWkSpace)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	typedef CArray<T_CfmFileInfo, const T_CfmFileInfo> CfmArray;
	CfmArray m_aCfms;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESTOREWKSPACE_H__44D80062_A8ED_4F7B_BB25_C1491EE0F063__INCLUDED_)

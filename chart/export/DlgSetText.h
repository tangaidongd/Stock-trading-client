#pragma once
#include "DialogEx.h"
#include "..\res\resource.h"
#include "chartexport.h"
class CHART_EXPORT CDlgSetText : public CDialogEx
{
	// Construction
public:
	CDlgSetText(CWnd* pParent = NULL);   // standard constructor
	// Dialog Data
	//{{AFX_DATA(CDlgSDBar)
	enum { IDD = IDD_DIALOG_SETTEXT };
	CString m_StrText;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetText)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetText)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

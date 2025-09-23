#if !defined(AFX_DLGWORKSPACESET_H__)
#define AFX_DLGWORKSPACESET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgworkspaceset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceSet dialog
#include "GGTongView.h"
#include "listboxex.h"
#include "DialogEx.h"

class CDlgWorkSpaceSet : public CDialogEx
{
public:
	typedef enum E_FileType
	{	
		EFTWorkSpace = 10,
		EFTChildFrame,
		EFTIoViewManger,
		
		EFTCount
	}E_FileType;

// Construction 
public:
	CDlgWorkSpaceSet(CWnd* pParent = NULL);   // standard constructor
	
public:
	void	SetFileInfoArray(const CStringArray & aStrFileInfo, const CStringArray & aStrFileTitle);
	void	OpenFile(CString StrFilePath);
	void	SetNewFileType(CDlgWorkSpaceSet::E_FileType eFileType);
	void	GetCurrentView(CGGTongView * pView);
protected:
	CStringArray	m_aStrFileInfo;
	CStringArray    m_aStrFileTitle;
	E_FileType		m_eFileType;
	CGGTongView *   m_pView;
public:	

// Dialog Data
	//{{AFX_DATA(CDlgWorkSpaceSet)
	enum { IDD = IDD_DIALOG_WORKSPACE };
	CListBoxEx  	m_ListShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWorkSpaceSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWorkSpaceSet)
	afx_msg void OnDblclkListShow();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnButtonDefault();
	afx_msg void OnSelchangeListShow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWORKSPACESET_H__41481F40_02B3_4F50_B6D2_75AD60FDD106__INCLUDED_)

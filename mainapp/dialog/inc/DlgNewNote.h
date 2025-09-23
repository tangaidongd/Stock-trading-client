#if !defined(AFX_DLGNEWNOTE_H_)
#define AFX_DLGNEWNOTE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"

#include "UserNoteInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgNewNote dialog

// 编辑投资日记对话框

class CDlgNewNote : public CDialogEx
{
public:

	static bool32 ModifyNote(INOUT T_UserNoteInfo &note); // 弹出对话框 编辑指定note

private:
	
	// Construction
protected:
	CDlgNewNote(CWnd* pParent = NULL);   // standard constructor

	void		InitNote(const T_UserNoteInfo &note);

	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	CString		GetDlgTitle();

	CFont			m_fontEdit;
	T_UserNoteInfo	m_note;		// 编辑的内容
	
	// Dialog Data
	//{{AFX_DATA(CDlgNewNote)
	enum { IDD = IDD_DIALOG_NEWNOTE };
	CRichEditCtrl m_EditContent;
	CEdit		  m_EditTitle;
	CDateTimeCtrl	m_DateTimePick;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewNote)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgNewNote)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWNOTE_H_)
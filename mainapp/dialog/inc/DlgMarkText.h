#if !defined(AFX_DLGMARKTEXT_H_)
#define AFX_DLGMARKTEXT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"

#include "MarkManager.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkText dialog

// ������ֶԻ���

class CDlgMarkText : public CDialogEx
{
public:
	CDlgMarkText(CWnd* pParent = NULL);   // standard constructor

	void	SetMerch(CMerch *pMerch){ m_pMerch = pMerch; };		// ���������Ʒ
	bool32	IsMarkAdded() const { return m_bMarkAdded; }	// �Ƿ�����˱�ǣ����idok��û�������ض�ɾ��

private:
	
	// Construction
protected:
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);
	
	// Dialog Data
	//{{AFX_DATA(CDlgMarkText)
	enum { IDD = IDD_DIALOG_MARK_TEXT };
	CEdit		  m_Edit;	// ��������
	//}}AFX_DATA

	CMerch		*m_pMerch;
	bool32		m_bMarkAdded;
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarkText)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarkText)
	afx_msg void OnBtnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMARKTEXT_H_)
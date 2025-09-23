#if !defined(_DLG_MENU_USERCYCLESET_H_)
#define _DLG_MENU_USERCYCLESET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgusercycleset.h : header file
//
#include "DialogEx.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet dialog

class CDlgMenuUserCycleSet : public CDialogEx
{
public:
    void SetIoViewParent(CIoViewBase* pIoView)									{ ASSERT(NULL != pIoView); m_pParentIoView = pIoView; }
    void SetInitialParam(E_NodeTimeInterval eNodeInterval, UINT uiUserSet)		{ m_eNodeInterval = eNodeInterval; m_uiUserSet = uiUserSet;}
private:
    CIoViewBase*		m_pParentIoView;
    E_NodeTimeInterval	m_eNodeInterval;
    // Construction
public:
    CDlgMenuUserCycleSet(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    //{{AFX_DATA(CDlgUserCycleSet)
    enum { IDD = IDD_DIALOG_USERCYCLE_MENU };
    CString	m_StrPre;
    CString	m_StrAft;
    UINT	m_uiUserSet;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgUserCycleSet)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDlgUserCycleSet)
    afx_msg void OnClose();
    afx_msg void OnButtonConfirm();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGUSERCYCLESET_H_)

#if !defined(AFX_DLGCONNECTSERVER_H__AB615B89_3220_4710_BFA8_0001BFB9E844__INCLUDED_)
#define AFX_DLGCONNECTSERVER_H__AB615B89_3220_4710_BFA8_0001BFB9E844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgconnectserver.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectServer dialog

class CDlgConnectServer : public CDialog
{
// Construction
public:
	CDlgConnectServer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConnectServer)
	enum { IDD = IDD_DIALOG_CONNECT_SERVER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConnectServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConnectServer)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECTSERVER_H__AB615B89_3220_4710_BFA8_0001BFB9E844__INCLUDED_)

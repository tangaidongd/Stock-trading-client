#if !defined(AFX_DLGSYSUPDATE_H__)
#define AFX_DLGSYSUPDATE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsysupdate.h : header file
//

#include "VersionFun.h"
#include "DialogEx.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSysUpdate dialog


typedef struct T_UpdataInfo
{
	CString	  StrPathServer;
	CString	  StrPathLocalTemp;
	CString   StrPathLocalDes;
	
}T_UpdataInfo;


class CDlgSysUpdate : public CDialogEx, public CBindStatusCB
{
// Construction
public:
	CDlgSysUpdate(CWnd* pParent = NULL);   // standard constructor


public:
	static UINT DownLoadThreadProc( LPVOID pParam );

	void	GetInitialData(CArray<T_UpdataInfo,T_UpdataInfo> & aUpdataInfo);
	void	ShowUpdateLog(CPaintDC & dc);
	void	DownLoadFile();
	bool32	MoveUpdateExe(T_UpdataInfo & UpdateExeInfo);
	virtual bool OnProgress(int iProcess,int iMax);

public:	
	CArray<T_UpdataInfo,T_UpdataInfo>	m_aUpdataInfo;

private:
	bool32			m_bUpdateComplete;
	CWinThread *	m_pDownLoadThread;
	T_UpdataInfo	m_UpdateExeInfo;

public:
// Dialog Data
	//{{AFX_DATA(CDlgSysUpdate)
	enum { IDD = IDD_DIALOG_UPDATE };
	CProgressCtrl	m_progress;
	CListCtrl	m_List;
	CString	m_StrStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSysUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSysUpdate)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg LRESULT OnMsgProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgStatic(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYSUPDATE_H__)
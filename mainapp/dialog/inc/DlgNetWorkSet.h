/////////////////////////////////////////////////////////////////////////////
// CDlgNetWorkSet dialog
#ifndef _DLGNETWORKSET_H_
#define _DLGNETWORKSET_H_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "GridCtrlSys.h"
#include "DialogEx.h"
#include "XScrollBar.h"


class CDlgNetWorkSet : public CDialogEx
{
// Construction
public: 
	CDlgNetWorkSet(CWnd* pParent = NULL);   // standard constructor
	~CDlgNetWorkSet();
	
	
///////////////////////////////////////////////////////////
// 
// protected:
public:
// Dialog Data
	//{{AFX_DATA(CDlgLogIn)
	enum { IDD = IDD_DIALOG_NETWORK_SET };

public:
	CComboBox  m_runMode;
	CTabCtrl   m_tab;
	CGridCtrl       m_Grid;
	BOOL   m_bManual;
protected:
	CXScrollBar  m_XSBVert;
	CXScrollBar  m_XSBHert;
private:
	int        m_rows;
	CArray<T_PlusInfo, T_PlusInfo>  m_aPlusInfo;

	bool m_bUseProxy;
	CString m_StrUserName;
	char   m_SelectedRow;
	CPing  m_ping;

    HANDLE  m_hThread;
    CTime   m_threadStartTime;
    bool  m_isAlreadyFinsh;

public:
	void SetIinitDailog(CString &strName);
	void OnButtonCancel();
	void OnBtnOk();
	void OnButtonNet();
	void OnButtonProxy();
	void AddDataGridTitle();
	void InitDataGrid();
	void SetCellCheckBox();
	void InsertRow();
	//
	int GetNetSelectIndex();
	//

	bool32		    GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo);
protected:
	bool    FromXml();
	bool    ToXml();
	void    OnPing(int nRow);
	virtual void DoDataExchange(CDataExchange* pDX);
	bool	DosPing(LPCSTR szTarget, int &nTime); 
    static UINT WINAPI _ThreadPingProc(LPVOID pParam);
    void  DestroyThread();
	
// Implementation
protected:	
	virtual BOOL OnInitDialog();
	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangleRunMode();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /